/* Target-dependent code for the Kalray KVX for GDB, the GNU debugger.

   Copyright (C) 2010 Kalray

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include <ctype.h>
#include "arch-utils.h"
#include "cli/cli-decode.h"
#include "event-top.h"
#include "frame-unwind.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "objfiles.h"
#include "observer.h"
#include "reggroups.h"
#include "target-descriptions.h"
#include "remote.h"
#include "event-loop.h"

#include "kvx-target.h"
#include "gdbthread.h"
#include "kvx-common-tdep.h"
#include "rsp-low.h"
#include "solib-kvx-bare.h"

struct kvx_inferior_data
{
  CORE_ADDR step_pad_area;
  CORE_ADDR step_pad_area_lma;
  int has_step_pad_area_p;
  int has_step_pad_area_lma_p;
};

struct displaced_step_closure
{
  /* Take into account that ALUs might have extensions. */
  uint32_t insn_words[8];
  int num_insn_words;

  unsigned branchy : 1;
  unsigned scall_jump : 1;
  unsigned rewrite_RA : 1;
  unsigned rewrite_LE : 1;
  unsigned rewrite_reg : 1;

  int has_pcrel;
  int pcrel_reg;
  int execute_inplace;

  /* The destination address when the branch is taken. */
  uint64_t dest;
  int reg;
};

static const struct kvx_inferior_data *_kvx_inferior_data;
static const struct inferior_data *kvx_inferior_data_token;

static struct kvx_inferior_data *
kvx_inferior_data (struct inferior *inf)
{
  struct kvx_inferior_data *res;

  res = inferior_data (inf, kvx_inferior_data_token);
  if (!res)
    {
      res = xcalloc (1, sizeof (*res));
      set_inferior_data (inf, kvx_inferior_data_token, res);
    }

  return res;
}

static CORE_ADDR
kvx_fetch_tls_load_module_address (struct objfile *objfile)
{
  struct regcache *regs = get_current_regcache ();
  ULONGEST val;

  regcache_raw_read_unsigned (regs,
			      gdbarch_tdep (target_gdbarch ())->local_regnum,
			      &val);
  return val;
}

static int
kvx_is_mmu_enabled (struct gdbarch *gdbarch, struct regcache *regs)
{
  ULONGEST ps;
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  if (regs == NULL)
    regs = get_current_regcache ();

  regcache_raw_read_unsigned (regs, tdep->ps_regnum, &ps);
  return (ps & (1 << PS_MME_BIT)) != 0;
}

void
enable_ps_v64_at_boot (struct regcache *regs)
{
  struct gdbarch_tdep *tdep;
  struct gdbarch *gdbarch;
  ULONGEST ps;

  if (regcache_read_pc (regs) != 0)
    return;

  gdbarch = target_gdbarch ();
  if (!gdbarch)
    return;
  tdep = gdbarch_tdep (gdbarch);
  if (!tdep)
    return;

  regcache_raw_read_unsigned (regs, tdep->ps_regnum, &ps);

  // enable the 64bit mode
  if ((ps & (1 << PS_V64_BIT)) == 0)
    {
      ps |= (1 << PS_V64_BIT);
      regcache_raw_write_unsigned (regs, tdep->ps_regnum, ps);
    }
}

static CORE_ADDR
kvx_displaced_step_location (struct gdbarch *gdbarch)
{
  struct kvx_inferior_data *data = kvx_inferior_data (current_inferior ());

  if (!data->has_step_pad_area_p)
    {
      struct bound_minimal_symbol msym
	= lookup_minimal_symbol ("_debug_start", NULL, NULL);
      if (msym.minsym == NULL)
	error ("Can not locate a suitable step pad area.");
      if (BMSYMBOL_VALUE_ADDRESS (msym) % 4)
	warning ("Step pad area is not 4-byte aligned.");
      data->step_pad_area = (BMSYMBOL_VALUE_ADDRESS (msym) + 3) & ~0x3;
      data->has_step_pad_area_p = 1;

      msym = lookup_minimal_symbol ("_debug_start_lma", NULL, NULL);
      if (msym.minsym != NULL)
	{
	  if (BMSYMBOL_VALUE_ADDRESS (msym) % 4)
	    warning ("Physical step pad area is not 4-byte aligned.");
	  data->step_pad_area_lma = (BMSYMBOL_VALUE_ADDRESS (msym) + 3) & ~0x3;
	  data->has_step_pad_area_lma_p = 1;
	}
    }

  if (data->has_step_pad_area_lma_p)
    {
      if (kvx_is_mmu_enabled (gdbarch, NULL))
	return data->step_pad_area_lma;
    }

  return data->step_pad_area;
}

static uint64_t
extract_mds_bitfield (kv3opc_t *op, uint32_t syllab, int bitfield, int sign)
{
  kvx_bitfield_t *bfield;
  uint64_t res;

  bfield = &op->format[bitfield]->bfield[0];
  res = ((uint64_t) syllab >> bfield->to_offset) & ((1ULL << bfield->size) - 1);

  if (sign && (res & (1ULL << (bfield->size - 1))))
    res |= (0xffffffffffffffffULL << bfield->size);

  return res;
}

static void
patch_mds_bitfield (kv3opc_t *op, uint32_t *syllab, int bitfield, int value)
{
  kvx_bitfield_t *bfield;
  uint32_t mask;

  bfield = &op->format[bitfield]->bfield[0];
  mask = ~(((1 << bfield->size) - 1) << bfield->to_offset);
  *syllab &= mask;
  *syllab |= (value << bfield->to_offset) & ~mask;
}

void
send_cluster_break_on_spawn (struct inferior *inf, int v)
{
  char *buf;
  long size = 256;

  buf = (char *) malloc (size);
  sprintf (buf, "kS%dp%x.1", v, inf->pid);
  putpkt (buf);
  getpkt (&buf, &size, 0);
  free (buf);
}

void
send_intercept_trap (struct inferior *inf, unsigned int v)
{
  char *buf;
  long size = 256;

  buf = (char *) malloc (size);
  sprintf (buf, "kT%04xp%x.1", v, inf->pid);
  putpkt (buf);
  getpkt (&buf, &size, 0);
  if (!strcmp (buf, "KO"))
    printf (_ ("Error setting the trap intercepting mask.\n"));

  free (buf);
}

void
send_cluster_stop_all (struct inferior *inf, int v)
{
  char *buf;
  long size = 256;

  buf = (char *) malloc (size);
  sprintf (buf, "kA%dp%x.1", v, inf->pid);
  putpkt (buf);
  getpkt (&buf, &size, 0);
  free (buf);
}

void
send_cluster_debug_ring (struct inferior *inf, int v)
{
  char *buf;
  long size = 256;

  buf = (char *) malloc (size);
  sprintf (buf, "kR%dp%x.1", v, inf->pid);
  putpkt (buf);
  getpkt (&buf, &size, 0);
  free (buf);
}

static void
kvx_enable_hbkp_on_cpu (CORE_ADDR addr, int enable)
{
  char *buf;
  long size = 256;

  buf = (char *) malloc (size);
  sprintf (buf, "kH0x%llx,%d", (unsigned long long) addr, enable);
  putpkt (buf);
  getpkt (&buf, &size, 0);
  free (buf);
}

int
read_memory_no_dcache (uint64_t addr, gdb_byte *user_buf, int len)
{
  char *buf;
  long size;
  int decoded_bytes;

  set_general_thread (inferior_ptid);

  size = 256;
  buf = (char *) malloc (size);
  sprintf (buf, "ku%llx,%d", (unsigned long long) addr, len);
  putpkt (buf);
  getpkt (&buf, &size, 0);

  if (buf[0] == 'E' && isxdigit (buf[1]) && isxdigit (buf[2]) && !buf[3])
    {
      free (buf);
      return 0;
    }

  decoded_bytes = hex2bin (buf, user_buf, len);
  free (buf);

  return decoded_bytes == len;
}

char
get_jtag_over_iss (void)
{
  char ret, *buf = (char *) malloc (256);
  long size = 256;
  strcpy (buf, "kj");
  putpkt (buf);
  getpkt (&buf, &size, 0);
  ret = *buf;
  free (buf);

  return ret;
}

static void
patch_bcu_instruction (struct gdbarch *gdbarch, CORE_ADDR from, CORE_ADDR to,
		       struct regcache *regs,
		       struct displaced_step_closure *dsc)
{
  struct op_list *insn = branch_insns[kvx_arch ()];
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  if (debug_displaced)
    printf_filtered ("displaced: Looking at BCU instruction\n");

  /* In order to limit side effects, we patch every instruction or
     register in order to make branches target the step pad. This
     way we have a simple way to check if branches were taken or
     not. 'scalls' are an exception to this rule, because we don't
     want to change the 'ev' register that might influence other
     things than syscalls.
   */

  while (insn)
    {
      kv3opc_t *op = insn->op;

      if ((dsc->insn_words[0] & op->codewords[0].mask)
	  != op->codewords[0].opcode)
	{
	  insn = insn->next;
	  continue;
	}

      if (debug_displaced)
	printf_filtered ("displaced: found branchy BCU insn: %s\n", op->as_op);

      dsc->branchy = 1;

      if (strcmp ("call", op->as_op) == 0)
	{
	  dsc->rewrite_RA = 1;
	  dsc->dest
	    = from + extract_mds_bitfield (op, dsc->insn_words[0], 0, 1) * 4;
	  patch_mds_bitfield (op, &dsc->insn_words[0], 0, 0);
	}
      else if (strcmp ("goto", op->as_op) == 0)
	{
	  dsc->dest
	    = from + extract_mds_bitfield (op, dsc->insn_words[0], 0, 1) * 4;
	  patch_mds_bitfield (op, &dsc->insn_words[0], 0, 0);
	}
      else if (strncmp ("cb.", op->as_op, 3) == 0)
	{
	  dsc->dest
	    = from + extract_mds_bitfield (op, dsc->insn_words[0], 1, 1) * 4;
	  patch_mds_bitfield (op, &dsc->insn_words[0], 1, 0);
	}
      else if (strcmp ("icall", op->as_op) == 0)
	{
	  ULONGEST reg_value;

	  dsc->rewrite_RA = 1;
	  dsc->rewrite_reg = 1;
	  dsc->reg = extract_mds_bitfield (op, dsc->insn_words[0], 0, 0)
		     + tdep->r0_regnum;
	  regcache_raw_read_unsigned (regs, dsc->reg, &reg_value);
	  dsc->dest = reg_value;
	  regcache_raw_write_unsigned (regs, dsc->reg, to);
	}
      else if (strcmp ("igoto", op->as_op) == 0)
	{
	  ULONGEST reg_value;

	  dsc->rewrite_reg = 1;
	  dsc->reg = extract_mds_bitfield (op, dsc->insn_words[0], 0, 0)
		     + tdep->r0_regnum;
	  regcache_raw_read_unsigned (regs, dsc->reg, &reg_value);
	  dsc->dest = reg_value;
	  regcache_raw_write_unsigned (regs, dsc->reg, to);
	}
      else if (strcmp ("ret", op->as_op) == 0)
	{
	  ULONGEST reg_value;

	  dsc->rewrite_reg = 1;
	  dsc->reg = tdep->ra_regnum;
	  regcache_raw_read_unsigned (regs, dsc->reg, &reg_value);
	  dsc->dest = reg_value;
	  regcache_raw_write_unsigned (regs, dsc->reg, to);
	}
      else if (strcmp ("rfe", op->as_op) == 0)
	{
	  ULONGEST reg_value;

	  dsc->rewrite_reg = 1;
	  dsc->reg = tdep->spc_regnum;
	  regcache_raw_read_unsigned (regs, dsc->reg, &reg_value);
	  dsc->dest = reg_value;
	  regcache_raw_write_unsigned (regs, dsc->reg, to);
	}
      else if (strcmp ("scall", op->as_op) == 0)
	{
	  ULONGEST syo, ev, ps;
	  int scall_no, scall_slice, target_pl, crt_pl;

	  dsc->scall_jump = 1;
	  scall_no = extract_mds_bitfield (op, dsc->insn_words[0], 0, 0);
	  scall_slice = scall_no / 1024;
	  regcache_raw_read_unsigned (regs, tdep->syo_regnum, &syo);
	  target_pl = (syo >> (scall_slice * 2)) & 3;
	  regcache_raw_read_unsigned (regs, tdep->ps_regnum, &ps);
	  crt_pl = ps & 3;
	  if (crt_pl < target_pl)
	    target_pl = crt_pl;
	  regcache_raw_read_unsigned (regs, tdep->ev_pl0_regnum + target_pl,
				      &ev);
	  dsc->dest = (ev & ~0xFFFULL) + 0x40 * 3;
	}
      else if (strcmp ("loopdo", op->as_op) == 0
	       || strcmp ("loopgtz", op->as_op) == 0
	       || strcmp ("loopnez", op->as_op) == 0)
	{
	  ULONGEST reg_value;

	  dsc->rewrite_LE = 1;
	  dsc->dest
	    = from + extract_mds_bitfield (op, dsc->insn_words[0], 1, 1) * 4;
	  patch_mds_bitfield (op, &dsc->insn_words[0], 1, 0);
	}
      else if (strcmp ("get", op->as_op) == 0 && op->format[1])
	{
	  /* get version with immediate */
	  if (extract_mds_bitfield (op, dsc->insn_words[0], 1, 0)
	      != (gdbarch_pc_regnum (gdbarch) - tdep->srf_offset))
	    break;

	  dsc->branchy = 0;
	  dsc->rewrite_reg = 1;
	  dsc->reg
	    = (extract_mds_bitfield (op, dsc->insn_words[0], 0, 0) & 0x3F)
	      + tdep->r0_regnum;
	  dsc->dest = from;
	}
      else if (strcmp ("iget", op->as_op) == 0)
	{
	  ULONGEST reg, srf_reg;

	  /* indirect get instruction */
	  reg = (extract_mds_bitfield (op, dsc->insn_words[0], 0, 0) & 0x3F)
		+ tdep->r0_regnum;
	  regcache_raw_read_unsigned (regs, reg, &srf_reg);

	  if (srf_reg != (gdbarch_pc_regnum (gdbarch) - tdep->srf_offset))
	    break;

	  dsc->branchy = 0;
	  dsc->rewrite_reg = 1;
	  dsc->reg = reg;
	  dsc->dest = from;
	}
      else
	{
	  internal_error (__FILE__, __LINE__, "Unknwon BCU insn");
	}

      break;
    }
}

static struct displaced_step_closure *
kvx_displaced_step_copy_insn (struct gdbarch *gdbarch, CORE_ADDR from,
			     CORE_ADDR to, struct regcache *regs)
{
  struct displaced_step_closure *dsc
    = xzalloc (sizeof (struct displaced_step_closure));
  int i;

  if (debug_displaced)
    printf_filtered ("displaced: copying from %s\n", paddress (gdbarch, from));

  TRY
  {
    do
      {
	read_memory (from + dsc->num_insn_words * 4,
		     (gdb_byte *) (dsc->insn_words + dsc->num_insn_words), 4);
      }
    while (dsc->insn_words[dsc->num_insn_words++] & (1 << 31));
  }
  CATCH (ex, RETURN_MASK_ERROR)
  {
    int mme = kvx_is_mmu_enabled (gdbarch, regs);
    int hbp
      = hardware_breakpoint_inserted_here_p (get_regcache_aspace (regs), from);

    if (!mme || !hbp
	|| (ex.error != MEMORY_ERROR && ex.error != NOT_SUPPORTED_ERROR))
      throw_exception (ex);

    if (debug_displaced)
      printf_filtered ("displaced: stepping inplace a hardware breakpoint "
		       "on an unmapped address\n");

    kvx_enable_hbkp_on_cpu (from, 0);

    dsc->execute_inplace = 1;
    return dsc;
  }
  END_CATCH

  if (debug_displaced)
    {
      int i;
      printf_filtered ("displaced: copied a %i word(s)\n", dsc->num_insn_words);
      for (i = 0; i < dsc->num_insn_words; ++i)
	printf_filtered ("displaced: insn[%i] = %08x\n", i, dsc->insn_words[i]);
    }

  dsc->insn_words[0]
    = extract_unsigned_integer ((const gdb_byte *) dsc->insn_words, 4,
				gdbarch_byte_order (gdbarch));
  if (((dsc->insn_words[0] >> 29) & 0x3) == 0)
    patch_bcu_instruction (gdbarch, from, to, regs, dsc);

  // pcrel instruction - the first syllable after the branch instructions
  for (i = 0; i < dsc->num_insn_words; i++)
    {
      uint32_t crt_word = dsc->insn_words[i];

      if (((crt_word >> 29) & 0x3) != 0)
	{
	  struct op_list *insn = pcrel_insn[kvx_arch ()];

	  if (((crt_word >> 29) & 0x3) != 0x3)
	    break;

	  while (insn)
	    {
	      kv3opc_t *op = insn->op;

	      if ((crt_word & op->codewords[0].mask) != op->codewords[0].opcode)
		{
		  insn = insn->next;
		  continue;
		}

	      dsc->has_pcrel = 1;
	      dsc->pcrel_reg = extract_mds_bitfield (op, crt_word, 0, 0);

	      if (debug_displaced)
		printf_filtered (
		  "displaced: found pcrel insn: destination register r%d\n",
		  dsc->pcrel_reg);
	      break;
	    }

	  break;
	}
    }
  store_unsigned_integer ((gdb_byte *) dsc->insn_words, 4,
			  gdbarch_byte_order (gdbarch), dsc->insn_words[0]);

  write_memory (to, (gdb_byte *) dsc->insn_words, dsc->num_insn_words * 4);

  return dsc;
}

static void
kvx_displaced_step_fixup (struct gdbarch *gdbarch,
			 struct displaced_step_closure *dsc, CORE_ADDR from,
			 CORE_ADDR to, struct regcache *regs)
{
  ULONGEST ps, lc, le, pc, pcrel_reg;
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  int branched = 0;
  int exception = 0;

  if (debug_displaced)
    printf_filtered ("displaced: Fixup\n");

  regcache_raw_read_unsigned (regs, tdep->ps_regnum, &ps);
  pc = regcache_read_pc (regs);
  if (debug_displaced)
    printf_filtered ("displaced: new pc %s\n", paddress (gdbarch, pc));

  if (dsc->execute_inplace)
    {
      kvx_enable_hbkp_on_cpu (from, 1);
      if (debug_displaced)
	printf_filtered ("displaced: inplace executed finished\n");
      return;
    }

  if (dsc->has_pcrel)
    {
      regcache_raw_read_unsigned (regs, dsc->pcrel_reg + tdep->r0_regnum,
				  &pcrel_reg);
      pcrel_reg += from - to;
      if (debug_displaced)
	printf_filtered ("displaced: pcrel patch register r%d to 0x%llx\n",
			 dsc->pcrel_reg, (unsigned long long) pcrel_reg);
      regcache_raw_write_unsigned (regs, dsc->pcrel_reg + tdep->r0_regnum,
				   pcrel_reg);
    }

  if (pc - to == dsc->num_insn_words * 4)
    {
      pc = from + (pc - to);
      if (debug_displaced)
	printf_filtered ("displaced: Didn't branch\n");
    }
  else
    {
      ULONGEST spc;

      /* We branched. */
      branched = 1;
      if (debug_displaced)
	printf_filtered ("displaced: we branched (predicted dest: %llx) \n",
			 (unsigned long long) dsc->dest);
      if (dsc->branchy && (pc == to || (dsc->scall_jump && pc == dsc->dest)))
	{
	  /* The branchy instruction jumped to its destination. */
	  pc = dsc->dest;

	  /* Rewrite RA only if the brach executed correctly. */
	  if (dsc->rewrite_RA)
	    {
	      regcache_raw_write_unsigned (regs, tdep->ra_regnum,
					   from + dsc->num_insn_words * 4);
	      if (debug_displaced)
		printf_filtered ("displaced: rewrite RA\n");
	    }

	  if (dsc->scall_jump)
	    {
	      regcache_raw_write_unsigned (regs, tdep->spc_regnum,
					   from + dsc->num_insn_words * 4);
	      if (debug_displaced)
		printf_filtered ("displaced: rewrite SPC\n");
	    }
	}
      else
	{
	  // Uh oh... seems we've taken some exceptional condition. This means
	  // interrupt or H/W trap
	  regcache_raw_read_unsigned (regs, tdep->spc_regnum, &spc);
	  if (debug_displaced)
	    printf_filtered ("displaced: trapped SPC=%lx\n",
			     (unsigned long) spc);
	  gdb_assert (spc == to);
	  spc = from;
	  regcache_raw_write_unsigned (regs, tdep->spc_regnum, spc);
	  exception = 1;
	}
    }

  /* Rewrite a patched reg unconditionnaly */
  if (dsc->rewrite_reg)
    {
      regcache_raw_write_unsigned (regs, dsc->reg, dsc->dest);
      if (debug_displaced)
	printf_filtered ("displaced: rewrite %i with %llx\n", dsc->reg,
			 (unsigned long long) dsc->dest);
    }

  if (((ps >> 5) & 1) /* HLE */)
    {

      /* The loop setup is done only if H/W loops are actually
	 enabled. */
      if (!exception && dsc->rewrite_LE)
	{
	  regcache_raw_write_unsigned (regs, tdep->le_regnum, dsc->dest);
	  regcache_raw_write_unsigned (regs, tdep->ls_regnum,
				       from + dsc->num_insn_words * 4);
	  if (debug_displaced)
	    printf_filtered ("displaced: rewrite LE\n");
	}

      if (!branched)
	{
	  regcache_raw_read_unsigned (regs, tdep->le_regnum, &le);
	  if (debug_displaced)
	    printf_filtered ("displaced: active loop pc(%llx) le(%llx)\n",
			     (unsigned long long) pc, (unsigned long long) le);

	  if (pc == le)
	    {
	      if (debug_displaced)
		printf_filtered ("displaced: at loop end\n");
	      regcache_raw_read_unsigned (regs, tdep->lc_regnum, &lc);
	      if (lc - 1 == 0)
		regcache_raw_read_unsigned (regs, tdep->le_regnum, &pc);
	      else
		regcache_raw_read_unsigned (regs, tdep->ls_regnum, &pc);
	      if (lc != 0)
		regcache_raw_write_unsigned (regs, tdep->lc_regnum, lc - 1);
	    }
	}
    }

  regcache_write_pc (regs, pc);
  if (debug_displaced)
    printf_filtered ("displaced: writing PC %s\n", paddress (gdbarch, pc));
}

static void
kvx_displaced_step_free_closure (struct gdbarch *gdbarch,
				struct displaced_step_closure *closure)
{
  xfree (closure);
}

static int
kvx_register_reggroup_p (struct gdbarch *gdbarch, int regnum,
			struct reggroup *group)
{
  if (gdbarch_register_name (gdbarch, regnum) == NULL
      || *gdbarch_register_name (gdbarch, regnum) == '\0')
    return 0;

  if ((group == save_reggroup || group == restore_reggroup
       || group == all_reggroup)
      && strncmp (gdbarch_register_name (gdbarch, regnum), "oce", 3) == 0)
    return 0;

  return default_register_reggroup_p (gdbarch, regnum, group);
}

static const gdb_byte *
kvx_bare_breakpoint_from_pc (struct gdbarch *gdbarch, CORE_ADDR *pc, int *len)
{
  *len = 4;

  if (cjtag_over_iss == 'o')
    {
      if (!break_jtag_over_iss[kvx_arch ()])
	error (
	  "Cannot find the scall instruction for the current architecture.");
      return (gdb_byte *) &break_jtag_over_iss[kvx_arch ()];
    }

  if (!break_op[kvx_arch ()])
    error ("Cannot find the break instruction for the current architecture.");

  return (gdb_byte *) &break_op[kvx_arch ()];
}

static void
kvx_timeout_os_init_done_cb (void *arg)
{
  *(int *) arg = 1;
}

int
kvx_prepare_os_init_done (void)
{
  static int s_cnt_wait_os_init_done = 0;
  CORE_ADDR gdb_os_init_done_addr;
  uint32_t saved_os_init_done_syl;
  struct bound_minimal_symbol msym;
  const gdb_byte *bp_opcode;
  struct inferior *inf;
  struct inferior_data *data;
  struct regcache *rc;
  int bp_len;

  inf = current_inferior ();
  if (!inf)
    return 0;

  // continue to gdb_os_init_done only for the clusters by spawned by runner
  // and only if the "kalray cont_os_init_done" option is on
  if (after_first_resume || !opt_cont_os_init_done)
    return 0;

  // continue to gdb_os_init_done only after reboot (pc = 0)
  rc = get_thread_regcache (inferior_ptid);
  if (regcache_read_pc (rc) != 0)
    return 0;

  // search the gdb_os_init_done symbol
  msym = lookup_minimal_symbol ("gdb_os_init_done", NULL, NULL);
  if (!msym.minsym)
    return 0;
  gdb_os_init_done_addr = BMSYMBOL_VALUE_ADDRESS (msym);
  if (!gdb_os_init_done_addr)
    return 0;

  // add the breakpoint opcode at the gdb_os_init_done address
  // w/o adding a breakpoint in gdb. This prevents the gdb breakpoints
  // counter increment
  bp_opcode = kvx_bare_breakpoint_from_pc (target_gdbarch (),
					  &gdb_os_init_done_addr, &bp_len);
  gdb_assert (bp_len == 4);
  read_memory (gdb_os_init_done_addr, (gdb_byte *) &saved_os_init_done_syl,
	       bp_len);
  write_memory (gdb_os_init_done_addr, bp_opcode, bp_len);

  // save the info to the inferior data
  wait_os_init_done++;
  data = mppa_inferior_data (inf);
  data->gdb_os_init_done_addr = gdb_os_init_done_addr;
  data->saved_os_init_done_syl = saved_os_init_done_syl;
  data->os_init_done = ++s_cnt_wait_os_init_done;

  return 1;
}

static void
kvx_inferior_created (struct target_ops *target, int from_tty)
{
  struct bound_minimal_symbol msym;
  int timedout, timer_id;
  struct inferior *inf;
  struct thread_info *thread;
  struct target_waitstatus ws;
  const int timeout_ms_os_init_done = 1000;

  kvx_current_arch = KVX_NUM_ARCHES;

  if (ptid_get_pid (inferior_ptid) != 1)
    return;

  if (!kvx_prepare_os_init_done ())
    return;

  inf = current_inferior ();
  if (!inf)
    return;

  // save the initial stopped thread waitstatus
  thread = find_thread_ptid (inferior_ptid);
  ws = thread->suspend.waitstatus;

  // continue to gdb_os_init_done
  continue_1 (0);

  // wait to arrive at gdb_os_init_done (or any other stop reason)
  // with any thread of the current inferior
  async_disable_stdin ();
  // avoid printing the stop info, it will be printed later by
  // process_initial_stop_replies
  inf->control.stop_soon = STOP_QUIETLY;
  timedout = 0;
  // if for the execution does not arrive at gdb_os_init_done, avoid
  // gdb stalling by creating a timer
  timer_id = create_timer (timeout_ms_os_init_done, &kvx_timeout_os_init_done_cb,
			   &timedout);
  while (gdb_do_one_event () >= 0)
    {
      // update stopped threads list
      update_thread_list ();
      finish_thread_state (minus_one_ptid);
      // search a stopped thread in the current inferior
      thread = any_live_thread_of_process (inf->pid);
      if (is_stopped (thread->ptid))
	{
	  // set the initial stopped thread waitstatus
	  thread->suspend.waitstatus = ws;
	  break;
	}

      if (timedout)
	break;
    }
  if (!timedout)
    delete_timer (timer_id);
  // restore the gdb state
  inf->control.stop_soon = NO_STOP_QUIETLY;
  async_enable_stdin ();
}

static CORE_ADDR
kvx_push_dummy_code (struct gdbarch *gdbarch, CORE_ADDR sp, CORE_ADDR funcaddr,
		    struct value **args, int nargs, struct type *value_type,
		    CORE_ADDR *real_pc, CORE_ADDR *bp_addr,
		    struct regcache *regcache)
{
  int i, sz;
  uint32_t nop = nop_op[kvx_arch ()];

  // allocate space for a breakpoint and a nop, keep the stack aligned
  sp &= KVX_STACK_ALIGN_MASK;
  sz = (2 * sizeof (nop) + KVX_STACK_ALIGN_BYTES - 1) & KVX_STACK_ALIGN_MASK;
  if (sp < sz)
    {
      error (_ ("Cannot call yet a function from gdb prompt because the stack "
		"pointer is not set yet (sp=0x%llx)"),
	     (unsigned long long) sp);
    }
  sp -= sz;

  // write NOPs on the reserved stack place
  for (i = 0; i < sz / sizeof (nop); i++)
    write_memory (sp + i * sizeof (nop), (gdb_byte *) &nop, sizeof (nop));

  // the breakpoint will be on the second NOP (beginning from the lowest
  // address) when the breakpoint will be inserted, it will search the end of
  // the previous bundle (bit parallel 0) so, it will find our first unparallel
  // NOP
  *bp_addr = sp + sizeof (nop);

  // inferior resumes at the function entry point
  *real_pc = funcaddr;

  return sp;
}

static int
sync_insert_remove_breakpoint (CORE_ADDR addr, int len, uint32_t value)
{
  char *buf;
  long size = 256;

  buf = (char *) malloc (size);
  sprintf (buf, "kB%llx,%d:%llx", (unsigned long long) addr, len,
	   (unsigned long long) value);
  putpkt (buf);
  getpkt (&buf, &size, 0);
  free (buf);

  return 0;
}

static int
kvx_memory_insert_breakpoint (struct gdbarch *gdbarch,
			     struct bp_target_info *bp_tgt)
{
  int ret = default_memory_insert_breakpoint (gdbarch, bp_tgt);

  if (!ret)
    {
      int len = 0;
      CORE_ADDR pc = bp_tgt->placed_address;
      const gdb_byte *bp = kvx_bare_breakpoint_from_pc (gdbarch, &pc, &len);

      sync_insert_remove_breakpoint (bp_tgt->placed_address, len,
					*(uint32_t *) bp);
    }

  return ret;
}

static int
kvx_memory_remove_breakpoint (struct gdbarch *gdbarch,
			     struct bp_target_info *bp_tgt)
{
  int ret = default_memory_remove_breakpoint (gdbarch, bp_tgt);

  if (!ret)
    {
      sync_insert_remove_breakpoint (bp_tgt->placed_address,
					bp_tgt->placed_size,
					*(uint32_t *) bp_tgt->shadow_contents);
    }

  return ret;
}

static void
kvx_write_pc (struct regcache *regcache, CORE_ADDR pc)
{
  struct displaced_step_closure *dsc = get_displaced_step_closure_by_addr (pc);

  if (dsc && dsc->execute_inplace)
    {
      if (debug_displaced)
	printf_filtered ("The PC was not written because the displace "
			 "will be executed inplace\n");

      return;
    }

  regcache_cooked_write_unsigned (
    regcache, gdbarch_pc_regnum (get_regcache_arch (regcache)), pc);
}

static struct gdbarch *
kvx_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;
  const struct target_desc *tdesc;
  struct tdesc_arch_data *tdesc_data;
  int i;
  unsigned long mach;
  int has_pc = -1, has_sp = -1, has_le = -1, has_ls = -1, has_ps = -1;
  int has_ev = -1, has_lc = -1, has_local = -1, has_ra = -1, has_spc = -1;
  int has_ea_pl0 = -1, has_es_pl0 = -1, has_syo = -1, has_ev_pl0 = -1;
  int has_r0 = -1;

  static const char kvx_ev_name[] = "ev";
  static const char kvx_lc_name[] = "lc";
  static const char kvx_ls_name[] = "ls";
  static const char kvx_le_name[] = "le";
  static const char kvx_ps_name[] = "ps";
  static const char kvx_ra_name[] = "ra";
  static const char kvx_spc_name[] = "spc";
  static const char kvx_local_name[] = "r13";
  static const char kvx_ea_pl0_name[] = "ea_pl0";
  static const char kvx_es_pl0_name[] = "es_pl0";
  static const char kvx_syo_name[] = "syo";
  static const char kvx_ev_pl0_name[] = "ev_pl0";
  static const char kvx_r0_name[] = "r0";

  const char *pc_name;
  const char *sp_name;

  if (ptid_equal (inferior_ptid, null_ptid))
    {
      static int non_stop_set = 0;
      char set_non_stop_cmd[] = "set non-stop";
      if (!non_stop_set)
	{
	  non_stop_set = 1;
	  execute_command (set_non_stop_cmd, 0);
	}
    }

  /* If there is already a candidate, use it.  */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return arches->gdbarch;

  tdep = xzalloc (sizeof (struct gdbarch_tdep));
  gdbarch = gdbarch_alloc (&info, tdep);

  pc_name = kv3_pc_name (gdbarch);
  sp_name = kv3_sp_name (gdbarch);
  mach = gdbarch_bfd_arch_info (gdbarch)->mach;

  /* This could (should?) be extracted from MDS */
  set_gdbarch_short_bit (gdbarch, 16);
  set_gdbarch_int_bit (gdbarch, 32);
  set_gdbarch_long_bit (gdbarch,
			gdbarch_bfd_arch_info (gdbarch)->bits_per_address);
  set_gdbarch_long_long_bit (gdbarch, 64);
  set_gdbarch_float_bit (gdbarch, 32);
  set_gdbarch_double_bit (gdbarch, 64);
  set_gdbarch_long_double_bit (gdbarch, 64);
  set_gdbarch_ptr_bit (gdbarch,
		       gdbarch_bfd_arch_info (gdbarch)->bits_per_address);
  set_gdbarch_memory_insert_breakpoint (gdbarch, kvx_memory_insert_breakpoint);
  set_gdbarch_memory_remove_breakpoint (gdbarch, kvx_memory_remove_breakpoint);
  set_gdbarch_write_pc (gdbarch, kvx_write_pc);

  /* Get the kvx target description from INFO.  */
  tdesc = info.target_desc;
  if (tdesc_has_registers (tdesc))
    {
      set_gdbarch_num_regs (gdbarch, 0);
      tdesc_data = tdesc_data_alloc ();
      tdesc_use_registers (gdbarch, tdesc, tdesc_data);

      for (i = 0; i < gdbarch_num_regs (gdbarch); ++i)
	{
	  if (strcmp (tdesc_register_name (gdbarch, i), kvx_r0_name) == 0)
	    has_r0 = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), pc_name) == 0)
	    has_pc = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), sp_name) == 0)
	    has_sp = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_le_name) == 0)
	    has_le = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_ls_name) == 0)
	    has_ls = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_ps_name) == 0)
	    has_ps = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_lc_name) == 0)
	    has_lc = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_local_name)
		   == 0)
	    has_local = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_ra_name) == 0)
	    has_ra = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_spc_name) == 0)
	    has_spc = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_ev_name) == 0)
	    has_ev = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_ea_pl0_name)
		   == 0)
	    has_ea_pl0 = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_es_pl0_name)
		   == 0)
	    has_es_pl0 = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_syo_name) == 0)
	    has_syo = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_ev_pl0_name)
		   == 0)
	    has_ev_pl0 = i;
	}

      if (has_r0 < 0)
	error ("There's no '%s' register!", kvx_r0_name);
      if (has_pc < 0)
	error ("There's no '%s' register!", pc_name);
      if (has_sp < 0)
	error ("There's no '%s' register!", sp_name);
      if (has_le < 0)
	error ("There's no '%s' register!", kvx_le_name);
      if (has_ls < 0)
	error ("There's no '%s' register!", kvx_ls_name);
      if (has_lc < 0)
	error ("There's no '%s' register!", kvx_lc_name);
      if (has_ps < 0)
	error ("There's no '%s' register!", kvx_ps_name);
      if (has_local < 0)
	error ("There's no '%s' register!", kvx_local_name);
      if (has_ra < 0)
	error ("There's no '%s' register!", kvx_ra_name);
      if (has_spc < 0)
	error ("There's no '%s' register!", kvx_spc_name);
      if (has_ev < 0)
	error ("There's no '%s' register!", kvx_ev_name);
      if (has_ea_pl0 < 0)
	error ("There's no '%s' register!", kvx_ea_pl0_name);
      if (has_es_pl0 < 0)
	error ("There's no '%s' register!", kvx_es_pl0_name);
      if (has_syo < 0)
	error ("There's no '%s' register!", kvx_syo_name);
      if (has_ev_pl0 < 0)
	error ("There's no '%s' register!", kvx_ev_pl0_name);

      tdep->r0_regnum = has_r0;
      tdep->ev_regnum = has_ev;
      tdep->le_regnum = has_le;
      tdep->ls_regnum = has_ls;
      tdep->lc_regnum = has_lc;
      tdep->ps_regnum = has_ps;
      tdep->ra_regnum = has_ra;
      tdep->spc_regnum = has_spc;
      tdep->local_regnum = has_local;
      tdep->ea_pl0_regnum = has_ea_pl0;
      tdep->es_pl0_regnum = has_es_pl0;
      tdep->ev_pl0_regnum = has_ev_pl0;
      tdep->syo_regnum = has_syo;
      tdep->uint256 = arch_integer_type (gdbarch, 256, 0, "uint256_t");
      tdep->uint512 = arch_integer_type (gdbarch, 512, 0, "uint512_t");
      tdep->uint1024 = arch_integer_type (gdbarch, 1024, 0, "uint1024_t");

      if (has_r0 == 0)
	tdep->srf_offset = has_pc;
      else
	tdep->srf_offset = has_pc;

      set_gdbarch_pc_regnum (gdbarch, has_pc);
      set_gdbarch_sp_regnum (gdbarch, has_sp);
    }
  else
    {
      set_gdbarch_num_regs (gdbarch, 1);
      set_gdbarch_register_name (gdbarch, kvx_dummy_register_name);
      set_gdbarch_register_type (gdbarch, kvx_dummy_register_type);
    }

  set_gdbarch_register_reggroup_p (gdbarch, kvx_register_reggroup_p);

  set_gdbarch_num_pseudo_regs (gdbarch, kv3_num_pseudos (gdbarch));
  set_tdesc_pseudo_register_name (gdbarch, kv3_pseudo_register_name);
  set_tdesc_pseudo_register_type (gdbarch, kv3_pseudo_register_type);
  set_tdesc_pseudo_register_reggroup_p (gdbarch,
					kv3_pseudo_register_reggroup_p);

  set_gdbarch_pseudo_register_read (gdbarch, kv3_pseudo_register_read);
  set_gdbarch_pseudo_register_write (gdbarch, kv3_pseudo_register_write);
  set_gdbarch_dwarf2_reg_to_regnum (gdbarch, kv3_dwarf2_reg_to_regnum);
  dwarf2_frame_set_init_reg (gdbarch, kvx_dwarf2_frame_init_reg);

  set_gdbarch_return_value (gdbarch, kvx_return_value);
  set_gdbarch_push_dummy_call (gdbarch, kvx_push_dummy_call);
  set_gdbarch_dummy_id (gdbarch, kvx_dummy_id);

  set_gdbarch_call_dummy_location (gdbarch, ON_STACK);
  set_gdbarch_push_dummy_code (gdbarch, kvx_push_dummy_code);

  set_gdbarch_skip_prologue (gdbarch, kvx_skip_prologue);
  set_gdbarch_unwind_pc (gdbarch, kvx_unwind_pc);
  dwarf2_append_unwinders (gdbarch);
  frame_unwind_append_unwinder (gdbarch, &kvx_frame_unwind);

  set_gdbarch_fetch_tls_load_module_address (gdbarch,
					     kvx_fetch_tls_load_module_address);

  set_gdbarch_breakpoint_from_pc (gdbarch, kvx_bare_breakpoint_from_pc);
  set_gdbarch_adjust_breakpoint_address (gdbarch, kvx_adjust_breakpoint_address);
  /* Settings that should be unnecessary.  */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  set_gdbarch_print_insn (gdbarch, kvx_print_insn);

  /* Displaced stepping */
  set_gdbarch_displaced_step_copy_insn (gdbarch, kvx_displaced_step_copy_insn);
  set_gdbarch_displaced_step_fixup (gdbarch, kvx_displaced_step_fixup);
  set_gdbarch_displaced_step_free_closure (gdbarch,
					   kvx_displaced_step_free_closure);
  set_gdbarch_displaced_step_location (gdbarch, kvx_displaced_step_location);
  set_gdbarch_max_insn_length (gdbarch, 8 * 4);

  set_gdbarch_get_longjmp_target (gdbarch, kvx_get_longjmp_target);

  if (tdesc_has_registers (tdesc))
    {
      set_solib_ops (gdbarch, &kvx_bare_solib_ops);
    }

  return gdbarch;
}

static void
kvx_cleanup_inferior_data (struct inferior *inf, void *data)
{
  xfree (data);
}

extern initialize_file_ftype _initialize_kvx_tdep;

void
_initialize_kvx_tdep (void)
{
  kvx_look_for_insns ();
  gdbarch_register (bfd_arch_kvx, kvx_gdbarch_init, NULL);

  observer_attach_inferior_created (kvx_inferior_created);

  kvx_inferior_data_token
    = register_inferior_data_with_cleanup (NULL, kvx_cleanup_inferior_data);
}
