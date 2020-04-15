#ifndef _KVX_COMMON_TDEP_
#define _KVX_COMMON_TDEP_

#include "dwarf2-frame.h"
#include "dis-asm.h"
#include "opcode/kv3.h"
#include "gdbtypes.h"

#define PS_ET_BIT 2
#define PS_MME_BIT 11
#define PS_V64_BIT 16
#define KVX_STACK_ALIGN_BYTES 8ULL
#define KVX_STACK_ALIGN_MASK ~(KVX_STACK_ALIGN_BYTES - 1)

enum KVX_ARCH
{
  KVX_KV3,
  KVX_NUM_ARCHES
};

struct gdbarch_tdep
{
  int ev_regnum;
  int ls_regnum;
  int le_regnum;
  int lc_regnum;
  int ps_regnum;
  int ra_regnum;
  int spc_regnum;
  int syo_regnum;
  int ea_pl0_regnum;
  int es_pl0_regnum;
  int ev_pl0_regnum;
  int r0_regnum;
  int srf_offset;

  int local_regnum;
  struct type *uint256;
  struct type *uint512;
  struct type *uint1024;
};

struct op_list
{
  kv3opc_t *op;
  struct op_list *next;
};

extern enum KVX_ARCH kvx_current_arch;
extern const struct frame_unwind kvx_frame_unwind;
extern struct op_list *branch_insns[KVX_NUM_ARCHES];
extern struct op_list *pcrel_insn[KVX_NUM_ARCHES];
extern uint32_t break_op[KVX_NUM_ARCHES];
extern uint32_t break_jtag_over_iss[KVX_NUM_ARCHES];
extern uint32_t nop_op[KVX_NUM_ARCHES];

enum KVX_ARCH kvx_arch (void);
void kvx_look_for_insns (void);
const char *kv3_pseudo_register_name (struct gdbarch *gdbarch, int regnr);
struct type *kv3_pseudo_register_type (struct gdbarch *gdbarch, int reg_nr);
int kv3_pseudo_register_reggroup_p (struct gdbarch *gdbarch, int regnum,
				    struct reggroup *reggroup);
enum register_status kv3_pseudo_register_read (struct gdbarch *gdbarch,
					       struct regcache *regcache,
					       int regnum, gdb_byte *buf);
void kv3_pseudo_register_write (struct gdbarch *gdbarch,
				struct regcache *regcache, int regnum,
				const gdb_byte *buf);
int kv3_dwarf2_reg_to_regnum (struct gdbarch *gdbarch, int reg);
int kv3_num_pseudos (struct gdbarch *);
const char *kv3_pc_name (struct gdbarch *);
const char *kv3_sp_name (struct gdbarch *);
const char *kvx_dummy_register_name (struct gdbarch *gdbarch, int regno);
struct type *kvx_dummy_register_type (struct gdbarch *gdbarch, int regno);
void kvx_dwarf2_frame_init_reg (struct gdbarch *gdbarch, int regnum,
			       struct dwarf2_frame_state_reg *reg,
			       struct frame_info *this_frame);
enum return_value_convention
kvx_return_value (struct gdbarch *gdbarch, struct value *func_type,
		 struct type *type, struct regcache *regcache,
		 gdb_byte *readbuf, const gdb_byte *writebuf);
struct frame_id kvx_dummy_id (struct gdbarch *gdbarch,
			     struct frame_info *this_frame);
CORE_ADDR kvx_push_dummy_call (struct gdbarch *gdbarch, struct value *function,
			      struct regcache *regcache, CORE_ADDR bp_addr,
			      int nargs, struct value **args, CORE_ADDR sp,
			      int struct_return, CORE_ADDR struct_addr);
CORE_ADDR kvx_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR func_addr);
CORE_ADDR kvx_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame);
CORE_ADDR kvx_adjust_breakpoint_address (struct gdbarch *gdbarch,
					CORE_ADDR bpaddr);
int kvx_print_insn (bfd_vma pc, disassemble_info *di);
int kvx_get_longjmp_target (struct frame_info *frame, CORE_ADDR *pc);

#endif
