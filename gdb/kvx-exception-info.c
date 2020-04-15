#include "config.h"
#include "defs.h"
#include <stdio.h>
#include <inttypes.h>
#include "inferior.h"
#include "kvx-common-tdep.h"
#include "target-descriptions.h"
#include "kvx-target.h"
#include "kvx-exception-info.h"
#include "ptid.h"
#include "gdbcore.h"
#include "gdbthread.h"
#include "common-types.h"
#include "remote.h"

#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
#define MSG_FROM_ARRAY(a, i)                                                   \
  ((i >= ARRAY_SIZE (a) || !a[i]) ? "[unknown]" : a[i])
#define NO_GPRS 64

#define MB (1024ULL * 1024)
#define GB (1024ULL * 1024 * 1024)

struct addr_range_s
{
  const char *name;
  uint64_t size;
} cool_addr_map[] = {
  {"Local SMEM", 4 * MB},	 {"Local Meta", 4 * MB},
  {"Local periph", 8 * MB},       {"Cluster 0 SMEM", 4 * MB},
  {"Cluster 0 Meta", 4 * MB},     {"Cluster 0 periph", 8 * MB},
  {"Cluster 1 SMEM", 4 * MB},     {"Cluster 1 Meta", 4 * MB},
  {"Cluster 1 periph", 8 * MB},   {"Cluster 2 SMEM", 4 * MB},
  {"Cluster 2 Meta", 4 * MB},     {"Cluster 2 periph", 8 * MB},
  {"Cluster 3 SMEM", 4 * MB},     {"Cluster 3 Meta", 4 * MB},
  {"Cluster 3 periph", 8 * MB},   {"Cluster 4 SMEM", 4 * MB},
  {"Cluster 4 Meta", 4 * MB},     {"Cluster 4 periph", 8 * MB},
  {"[reserved]", 32 * MB},	{"AXI config", 256 * MB},
  {"AXI periphs", 256 * MB},      {"PCIE alias", (2 * GB - 640 * MB)},
  {"DDR alias", 2 * GB},	  {"DDR", 64 * GB},
  {"PCIE", (256 * GB - 68 * GB)},
};

enum
{
  EN_REG_ES_EC_DEBUG = 0,
  EN_REG_ES_EC_HWTRAP = 1,
  EN_REG_ES_EC_INT = 2,
  EN_REG_ES_EC_SCALL = 3,
  EN_REG_ES_EC_WDOG = 8,
  EN_REG_ES_EC_DE_HWTRAP = 9,
  EN_REG_ES_EC_DE_INT = 10,
  EN_REG_ES_EC_DE_SCALL = 11,
};
#define EN_REG_ES_EC_FIRST_DE EN_REG_ES_EC_DE_HWTRAP

const char *reg_es_ec_msg[] = {
  "debug interception",				    // 0
  "hardware trap interception",			    // 1
  "interrupt interception",			    // 2
  "syscall interception",			    // 3
  NULL,						    // 4
  NULL,						    // 5
  NULL,						    // 6
  NULL,						    // 7
  "watchdog error",				    // 8
  "double exception interception on hardware trap", // 9
  "double exception interception on interrupt",     // 10
  "double exception interception on scall",	 // 11
};

const char *trap_msg[] = {
  "RESET - reset sequence",				     // 0
  "OPCODE - malformed bundle",				     // 1
  "PRIVILEGE - attempt to execute an privilege instruction", // 2
  "DMISALIGN - effective address for data memory access is not aligned in "
  "areas or "
  "on instructions that do not support misalignment",			// 3
  "PSYSERROR - system error to an external program fetch request",      // 4
  "DSYSERROR - system error to an external data access",		// 5
  "PDECCERROR - double ECC error to an external program fetch request", // 6
  "DDECCERROR - double ECC error to an external data access",		// 7
  "PPARERROR - parity error to an external program fetch request",      // 8
  "DPARERROR - parity error to an external data access",		// 9
  "PSECCERROR - single ECC error to an external program fetch request", // 10
  "DSECCERROR - single ECC error to an external data access",		// 11
  "NOMAPPING - memory translation trap",				// 12
  "PROTECTION - memory translation trap",				// 13
  "WRITETOCLEAN - memory translation trap",				// 14
  "ATOMICTOCLEAN - memory translation trap",				// 15
};

#define ES_RWS_FIRST_DATA 2
const char *es_rwx_msg[] = {
  "not a memory trap",      // 0
  "fetch side memory trap", // 1
  "data side memory trap on a write access (dzerol is considered as a write "
  "access)", // 2 = ES_RWS_FIRST_DATA
  NULL,      // 3
  "data side memory trap on a read access (including DINVALL, DTOUCHL, DINVAL, "
  "WPURGE)",				       // 4
  NULL,					       // 5
  "data side memory trap on an atomic access", // 6
};

#define ES_RWS_STEPI_FIRST_DATA 2
const char *es_rwx_stepi_msg[] = {
  "no data memory access in the stepped bundle (canceled non-trapping loads "
  "and "
  "non executed conditional ld/st are considered as such)", // 0
  NULL,							    // 1
  "data write memory access in the stepped bundle",	 // 2 =
						    // ES_RWS_STEPI_FIRST_DATA
  NULL, // 3
  "data read memory access in the stepped bundle (DINVALL, DTOUCHL, DINVAL, "
  "WPURGE "
  "are considered as read accesses)",		     // 4
  NULL,						     // 5
  "data atomic memory access in the stepped bundle", // 6
};

enum
{
  EN_ES_DC_BREAKPOINT = 0,
  EN_ES_DC_WATCHPOINT = 1,
  EN_ES_DC_STEPI = 2,
  EN_ES_DC_DSU_BREAK = 3,
};

const char *es_dc_msg[] = {
  "hardware breakpoint", // 0
  "hardware watchpoint", // 1
  "stepi",		 // 2
  "dsu break",		 // 3
};

const char *es_nta_msg[] = {
  "trap was caused by a regular data memory access",      // 0
  "trap was caused by a non-trapping data memory access", // 1
};

const char *es_nta_stepi_msg[] = {
  "the stepped bundle contains a regular data memory access",      // 0
  "the stepped bundle contains a non-trapping data memory access", // 1
};

const char *es_as_msg[] = {
  [1] = "byte access",
  [2] = "half-word access",
  [4] = "word access (including atomic word size instructions)",
  [8] = "double-word access (including atomic double-word size instructions)",
  [16] = "quad-word access",
  [17] = "cache maintenance access (DINVALL, DTOUCHL, DINVAL)",
  [31] = "cache line-size access (DZEROL)",
};

const char *es_sfri[] = {
  [0] = "no SFR access instruction",
  [2] = "GET",
  [3] = "IGET",
  [4] = "SET",
  [5] = "WFXL",
  [6] = "WFXM",
  [7] = "RSWAP",
};

union reg_es_s
{
  uint64_t reg;
  union
  {
    struct
    {
      uint64_t ec : 4;    // 0 - 3
      uint64_t oapl : 2;  // 4 - 5
      uint64_t orpl : 2;  // 6 - 7
      uint64_t ptapl : 2; // 8 - 9
      uint64_t ptrpl : 2; // 10 - 11
    } common;
    struct
    {
      uint64_t ec : 4;    // 0 - 3
      uint64_t oapl : 2;  // 4 - 5
      uint64_t orpl : 2;  // 6 - 7
      uint64_t ptapl : 2; // 8 - 9
      uint64_t ptrpl : 2; // 10 - 11

      uint64_t itn : 5;  // 12 - 16
      uint64_t itl : 2;  // 17 - 18
      uint64_t iti : 10; // 19 - 28
    } irq;
    struct
    {
      uint64_t ec : 4;    // 0 - 3
      uint64_t oapl : 2;  // 4 - 5
      uint64_t orpl : 2;  // 6 - 7
      uint64_t ptapl : 2; // 8 - 9
      uint64_t ptrpl : 2; // 10 - 11

      uint64_t sn : 12; // 12 - 23
    } scall;
    struct
    {
      uint64_t ec : 4;    // 0 - 3
      uint64_t oapl : 2;  // 4 - 5
      uint64_t orpl : 2;  // 6 - 7
      uint64_t ptapl : 2; // 8 - 9
      uint64_t ptrpl : 2; // 10 - 11

      uint64_t htc : 5;   // 12 - 16
      uint64_t sfrt : 1;  // 17
      uint64_t sfri : 3;  // 18 - 20
      uint64_t gprp : 6;  // 21 - 26
      uint64_t sfrp : 12; // 27 - 38
      uint64_t rwx : 3;   // 39 - 41
      uint64_t nta : 1;   // 42
      uint64_t uca : 1;   // 43
      uint64_t as : 6;    // 44 - 49
      uint64_t bs : 4;    // 50 - 53
      uint64_t dri : 6;   // 54 - 59
    } trap;
    struct
    {
      uint64_t ec : 4;    // 0 - 3
      uint64_t oapl : 2;  // 4 - 5
      uint64_t orpl : 2;  // 6 - 7
      uint64_t ptapl : 2; // 8 - 9
      uint64_t ptrpl : 2; // 10 - 11

      uint64_t dc : 2;    // 12 - 13
      uint64_t bn : 1;    // 14
      uint64_t wn : 1;    // 15
      uint64_t : 2;       // 16 - 17
      uint64_t sfri : 3;  // 18 - 20
      uint64_t gprp : 6;  // 21 - 26
      uint64_t sfrp : 12; // 27 - 38
      uint64_t rwx : 3;   // 39 - 41
      uint64_t nta : 1;   // 42
      uint64_t uca : 1;   // 43
      uint64_t as : 6;    // 44 - 49
      uint64_t bs : 4;    // 50 - 53
      uint64_t dri : 6;   // 54 - 59
    } sys_si;
    struct
    {
      uint64_t ec : 4;    // 0 - 3
      uint64_t oapl : 2;  // 4 - 5
      uint64_t orpl : 2;  // 6 - 7
      uint64_t ptapl : 2; // 8 - 9
      uint64_t ptrpl : 2; // 10 - 11

      uint64_t dc : 2; // 12 - 13
      uint64_t bn : 2; // 14
      uint64_t wn : 2; // 15
    } debug;
  };
};

static void
show_addr_info (uint64_t addr, int mme)
{
  const char *sym_name = NULL;
  struct bound_minimal_symbol min_sym;
  const char *sym_type;

  sym_type = "[unknown symbol type]";
  min_sym = lookup_minimal_symbol_by_pc_section (addr, NULL);
  if (min_sym.minsym)
    {
      int mst = MSYMBOL_TYPE (min_sym.minsym);
      if (mst == mst_text || mst == mst_text_gnu_ifunc
	  || mst == mst_solib_trampoline || mst == mst_file_text)
	sym_type = "function";
      else if (mst == mst_slot_got_plt)
	sym_type = "got_plt section variable";
      else if (mst == mst_data || mst == mst_file_data)
	sym_type = "data section variable";
      else if (mst == mst_bss || mst == mst_file_bss)
	sym_type = "bss section variable";
      else if (mst == mst_abs)
	sym_type = "nonrelocatable variable";
      sym_name = MSYMBOL_PRINT_NAME (min_sym.minsym);
    }

  if (sym_name)
    printf (" in %s %s", sym_type, sym_name);
  else if (!mme)
    {
      int i;
      for (i = 0; i < ARRAY_SIZE (cool_addr_map); i++)
	{
	  if (addr < cool_addr_map[i].size)
	    break;
	  addr -= cool_addr_map[i].size;
	}
      if (i < ARRAY_SIZE (cool_addr_map))
	printf (" in %s memory map range", cool_addr_map[i].name);
    }
}

static void
show_trap_pl_details (union reg_es_s es)
{
  printf ("\torigin absolute PL (es.oapl): %d\n", es.common.oapl);
  printf ("\torigin relative PL (es.orpl): %d\n", es.common.orpl);
  printf ("\tprimary target absolute PL (es.ptapl): %d\n", es.common.ptapl);
  printf ("\tprimary target relative PL (es.ptrpl): %d\n", es.common.ptrpl);
}

static const char *
get_sfr_reg_name (int regnum)
{
  static char name[20];
  const char *ret = NULL;
  struct gdbarch *gdbarch = target_gdbarch ();

  if (gdbarch && regnum + NO_GPRS < gdbarch_num_regs (gdbarch))
    ret = tdesc_register_name (gdbarch, regnum + NO_GPRS);

  if (ret == NULL)
    {
      sprintf (name, "SFR%d", regnum);
      ret = name;
    }

  return ret;
}

static void
show_trap_instruction (union reg_es_s es)
{
  printf ("\ttrap caused by a SFR instruction (es.sfrt): %d\n", es.trap.sfrt);
  printf ("\tsfr instruction implied: %s (es.sfri=%d)",
	  MSG_FROM_ARRAY (es_sfri, es.trap.sfri), es.trap.sfri);
  if (es.trap.sfri)
    {
      printf ("SFR reg: %s (es.sfrp=%d), GRP reg: r%d (es.gprp=%d)",
	      get_sfr_reg_name (es.trap.sfrp), es.trap.sfrp, es.trap.gprp,
	      es.trap.gprp);
    }
  printf ("\n");
}

static void
show_trap_info (union reg_es_s es)
{
  show_trap_instruction (es);

  printf ("\ttrap cause: %s (es.htc=0x%x)\n",
	  es.trap.htc >= ARRAY_SIZE (trap_msg) ? "[unknown]"
					       : trap_msg[es.trap.htc],
	  es.trap.htc);
  printf ("\tread write execute: %s (es.rwx=0x%x)\n",
	  MSG_FROM_ARRAY (es_rwx_msg, es.trap.rwx), es.trap.rwx);
  if (es.trap.rwx >= ES_RWS_FIRST_DATA)
    {
      printf ("\tnon trapping access: %s (es.nta=0x%x)\n",
	      MSG_FROM_ARRAY (es_nta_msg, es.trap.nta), es.trap.nta);
      printf ("\ttrap caused by an %scached memory access instruction "
	      "(es.uca=0x%x)\n",
	      es.trap.uca ? "un" : "", es.trap.uca);
      printf ("\taccess size: %s (es.as=0x%x)\n",
	      MSG_FROM_ARRAY (es_as_msg, es.trap.as), es.trap.as);
      printf (
	"\tsource or destination operand register "
	"of the data memory access that caused the trap: r%d (es.dri=%u)\n",
	es.trap.dri, es.trap.dri);
    }
  printf ("\tbundle size: %d bits (%d syllable%s) (es.bs=%d)\n",
	  es.trap.bs * 32, es.trap.bs, es.trap.bs > 1 ? "s" : "", es.trap.bs);
}

static void
show_debug_trap_info (union reg_es_s es)
{
  printf ("Debug cause: %s (es.dc=%d)\n",
	  MSG_FROM_ARRAY (es_dc_msg, es.debug.dc), es.debug.dc);
  switch (es.debug.dc)
    {
    case EN_ES_DC_BREAKPOINT:
      printf ("\tbreakpoint number: %d (es.bn=%d)\n", es.debug.bn, es.debug.bn);
      break;
    case EN_ES_DC_WATCHPOINT:
      printf ("\twatchpoint number: %d (es.wn=%d)\n", es.debug.wn, es.debug.wn);
      break;
    default:
      break;
    }
}

static void
stopped_cpu_status (void)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (target_gdbarch ());
  struct regcache *reg_cache = get_current_regcache ();
  union reg_es_s es;
  uint64_t pc, spc, ea, ps;
  int mme, ps_et;

  // read required registers
  regcache_raw_read_unsigned (reg_cache, tdep->es_pl0_regnum, &es.reg);
  regcache_raw_read_unsigned (reg_cache, tdep->ea_pl0_regnum, &ea);
  regcache_raw_read_unsigned (reg_cache, tdep->ps_regnum, &ps);

  mme = (ps & (1 << PS_MME_BIT)) != 0;
  ps_et = (ps & (1 << PS_ET_BIT)) != 0;

  printf ("The processor entered debug mode because of a %s (es.ec=0x%x).\n",
	  MSG_FROM_ARRAY (reg_es_ec_msg, es.common.ec), es.common.ec);

  show_trap_pl_details (es);

  if (es.common.ec == EN_REG_ES_EC_HWTRAP
      || es.common.ec == EN_REG_ES_EC_DE_HWTRAP)
    {
      if (ps_et)
	{
	  printf ("Trap information (es=0x%" PRIx64 "):\n", es.reg);
	  printf ("\texception address (ea): 0x%" PRIx64, ea);
	  show_addr_info (ea, mme);
	  printf ("\n");
	  show_trap_info (es);
	}
      else
	{
	  printf ("Not in a trap (ps.et=%d)\n", ps_et);
	}
    }
  else if (es.common.ec == EN_REG_ES_EC_SCALL
	   || es.common.ec == EN_REG_ES_EC_DE_SCALL)
    {
      printf ("Scall number: %d (es.sn=%d)\n", es.scall.sn, es.scall.sn);
    }
  else if (es.common.ec == EN_REG_ES_EC_INT
	   || es.common.ec == EN_REG_ES_EC_DE_INT)
    {
      printf ("\tinterrupt number %d: (es.itn=%d)\n", es.irq.itn, es.irq.itn);
      printf ("\tinterrupt level: %d (es.itl=%d)\n", es.irq.itl, es.irq.itl);
      printf ("\tinterrupt info (copy of mes for memory related interrupts: 12 "
	      "for SECCs, "
	      "16 for Data asynchronous memory errors, 17 for I/D line "
	      "invalidation): %x (es.iti=%d)\n",
	      es.irq.iti, es.irq.iti);
    }
  else if (es.common.ec == EN_REG_ES_EC_DEBUG)
    {
      printf ("\texception address (ea): 0x%" PRIx64, ea);
      show_addr_info (ea, mme);
      printf ("\n");
      show_debug_trap_info (es);
    }

  // PC information
  pc = regcache_read_pc (reg_cache);
  printf ("pc: 0x%" PRIx64, pc);
  show_addr_info (pc, mme);
  printf ("\n");

  // SPC info
  regcache_raw_read_unsigned (reg_cache, tdep->spc_regnum, &spc);
  printf ("spc: 0x%" PRIx64, spc);
  show_addr_info (spc, mme);
  printf ("\n");
}

static const char *cpu_status[] = {
  "sleeping", // 0
  "reseting", // 1
  "running",  // 2
  "idle",     // 3
};
#define CPU_FSM_IDLE 3

enum
{
  DBG_SFR_FPC_IDX = 0,
  DBG_SFR_STAPC_IDX,
  DBG_SFR_PS_IDX,
  DBG_SFR_SPC_IDX,
  DBG_SFR_SPS_IDX,
  DBG_SFR_EA_IDX,
  DBG_SFR_ES_IDX,
  DBG_SFR_SR_IDX,
  DBG_SFR_RA_IDX,
  DBG_SFR_NO_REGS
};
static const char *dbg_sfr_name[]
  = {"fetch pc", "stall pc", "ps", "spc", "sps", "ea", "es", "sr", "ra", ""};

union fpc_extra_u
{
  struct
  {
    uint16_t f_instr_valid : 1;     // bit 0
    uint16_t f_stall : 1;	   // bit 1
    uint16_t rr_instr_valid : 1;    // bit 2
    uint16_t rr_stall : 1;	  // bit 3
    uint16_t e1_instr_valid : 1;    // bit 4
    uint16_t e1_stall : 1;	  // bit 5
    uint16_t e2_instr_valid : 1;    // bit 6
    uint16_t e2_stall : 1;	  // bit 7
    uint16_t e3_instr_valid : 1;    // bit 8
    uint16_t e3_stall : 1;	  // bit 9
    uint16_t icache_on_going_i : 1; // bit 10
    uint16_t dcache_on_going_i : 1; // bit 11
    uint16_t de_stop : 1;	   // bit 12
    uint16_t wdog_err : 1;	  // bit 13
    uint16_t reserved_1 : 2;	// bit 14 - 15
  };
  uint16_t uint16_t;
};

union stall_pc_extra_u
{
  struct
  {
    uint16_t reset : 1;		  // bit 0 (stall_pc bit 48)
    uint16_t clk_enable : 1;      // bit 1
    uint16_t clk_idle_enable : 1; // bit 2
    uint16_t pwc_idle : 2;	// bit 3 - 4
    uint16_t pwc_fsm : 2;	 // bit 5 - 6
    uint16_t pwc_wd : 1;	  // bit 7
    uint16_t reserved_1 : 8;      // bit 8 - 15
  };
  uint16_t uint16_t;
};

static void
show_debug_sfr_regs (void)
{
  char *buf = (char *) malloc (512), *pbuf;
  long size = 512;
  unsigned long long dbg_sfrs[DBG_SFR_NO_REGS + 1];
  union fpc_extra_u fpc_extra;
  union stall_pc_extra_u stall_pc_extra;
  int i, n, offset;

  sprintf (buf, "kfp%x.%lx", inferior_ptid.pid, inferior_ptid.lwp);
  putpkt (buf);
  getpkt (&buf, &size, 0);

  if (*buf == 'E')
    {
      free (buf);
      printf ("Info: cannot get the debug SFR registers (not implemented yet "
	      "by ISS)\n");
      return;
    }

  for (n = 0, pbuf = buf;
       n <= DBG_SFR_NO_REGS
       && sscanf (pbuf, "%llx %n", &dbg_sfrs[n], &offset) == 1;
       n++, pbuf += offset)
    ;

  printf ("SFR debug registers:\n");
  if (n != DBG_SFR_NO_REGS)
    printf ("warning: read %d registers instead of %d\n", n, DBG_SFR_NO_REGS);

  fpc_extra.uint16_t = (uint16_t) (dbg_sfrs[DBG_SFR_FPC_IDX] >> 48);
  dbg_sfrs[DBG_SFR_FPC_IDX] &= 0xFFFFFFFFFFFFULL;
  stall_pc_extra.uint16_t = (uint16_t) (dbg_sfrs[DBG_SFR_STAPC_IDX] >> 48);
  dbg_sfrs[DBG_SFR_STAPC_IDX] &= 0xFFFFFFFFFFFFULL;
  for (i = 0; i < n; i++)
    printf ("  %s: 0x%llx\n", dbg_sfr_name[i], dbg_sfrs[i]);

  printf ("  extra info:\n");
  printf ("    f instruction: valid: %d, stall: %d\n", fpc_extra.f_instr_valid,
	  fpc_extra.f_stall);
  printf ("    rr instruction: valid: %d, stall: %d\n",
	  fpc_extra.rr_instr_valid, fpc_extra.rr_stall);
  printf ("    e1 instruction: valid: %d, stall: %d\n",
	  fpc_extra.e1_instr_valid, fpc_extra.e1_stall);
  printf ("    e2 instruction: valid: %d, stall: %d\n",
	  fpc_extra.e2_instr_valid, fpc_extra.e2_stall);
  printf ("    e3 instruction: valid: %d, stall: %d\n",
	  fpc_extra.e3_instr_valid, fpc_extra.e3_stall);
  printf ("    icache on going: %d, dcache on going: %d\n",
	  fpc_extra.icache_on_going_i, fpc_extra.dcache_on_going_i);
  printf ("    double exception stop: %d\n", fpc_extra.de_stop);
  printf ("    watchdog error: %d\n", fpc_extra.wdog_err);

  printf ("    reset: %d\n", stall_pc_extra.reset);
  printf ("    clk_enable: %d\n", stall_pc_extra.clk_enable);
  printf ("    clk_idle_enable: %d\n", stall_pc_extra.clk_idle_enable);
  printf ("    pwc_crfr_wdf/idle: %d\n", stall_pc_extra.pwc_idle);
  printf ("    pwc_fsm: %d\n", stall_pc_extra.pwc_fsm);
  printf ("    pwc_wdf: %d\n", stall_pc_extra.pwc_wd);

  free (buf);
}

static void
show_pwr_cpu_status (void)
{
  uint64_t power_control_status_addr, status = 0;
  uint64_t rst,
    power_control_rst_addr
    = 0xA41040; // &mppa_pwr_ctrl[0]->vector_proc_control.reset_on_wakeup.write
  int idle, fsm, wd;
  int vehicle = inferior_ptid.lwp - 1;

  // 0xA40400 = &mppa_pwr_ctrl[0]->pe_status[vehicle].proc_sts
  // 0xA44100 = &mppa_pwr_ctrl[0]->rm_status.proc_sts
  if (vehicle < 16) // pe
    power_control_status_addr = 0xA40400 + vehicle * 16;
  else
    power_control_status_addr = 0xA44100;

  if (!read_memory_no_dcache (power_control_status_addr, (gdb_byte *) &status,
			      8))
    error (_ ("[cannot read power controller status]\n"));
  if (!read_memory_no_dcache (power_control_rst_addr, (gdb_byte *) &rst, 8))
    error (_ ("[cannot read power controller reset]\n"));

  wd = status & 1;
  idle = (status >> 1) & 3;
  fsm = (status >> 3) & 3;

  printf ("The processor state: %s", cpu_status[fsm]);
  if (fsm == CPU_FSM_IDLE)
    printf ("%d", idle);
  else
    printf (", last idle: %d", idle);
  printf (", watchdog %s", wd ? "set" : "not set");
  printf (", reset on wakeup %s\n", ((1 << vehicle) & rst) ? "set" : "not set");
}

void
mppa_cpu_status_command (char *args, int from_tty)
{
  if (ptid_equal (inferior_ptid, null_ptid))
    {
      printf (_ ("No thread selected.\n"));
      return;
    }

  if (is_stopped (inferior_ptid))
    stopped_cpu_status ();
  else
    {
      printf ("The processor is not in debug mode.\n");
      show_debug_sfr_regs ();
    }

  show_pwr_cpu_status ();
}

void
mppa_cpu_debug_sfr_regs_command (char *args, int from_tty)
{
  if (ptid_equal (inferior_ptid, null_ptid))
    {
      printf (_ ("No thread selected.\n"));
      return;
    }

  show_debug_sfr_regs ();
}
