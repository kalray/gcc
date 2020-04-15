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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "defs.h"
#include "frame-unwind.h"
#include "gdbcore.h"
#include "target-descriptions.h"
#include "user-regs.h"
#include "elf-bfd.h"
#include "gdbthread.h"

#include "elf/kv3.h"
#include "opcode/kv3.h"
#include "kvx-common-tdep.h"

#define __NR_BREAKPOINT_PL0 4050
#define __NR_BREAKPOINT_JTAGISS 4054

struct kvx_frame_cache
{
  CORE_ADDR base; // base address
  CORE_ADDR pc;
  LONGEST framesize;
  CORE_ADDR saved_sp;
};

enum KVX_ARCH kvx_current_arch = KVX_NUM_ARCHES;

struct op_list *branch_insns[KVX_NUM_ARCHES];
struct op_list *pcrel_insn[KVX_NUM_ARCHES];
uint32_t break_op[KVX_NUM_ARCHES];
uint32_t break_jtag_over_iss[KVX_NUM_ARCHES];
uint32_t nop_op[KVX_NUM_ARCHES];

enum KVX_ARCH
kvx_arch (void)
{
  const struct target_desc *desc = target_current_description ();

  if (kvx_current_arch != KVX_NUM_ARCHES)
    return kvx_current_arch;

  if (exec_bfd)
    {
      switch (elf_elfheader (exec_bfd)->e_flags & ELF_KVX_CORE_MASK)
	{
	case ELF_KVX_CORE_KV3_1:
	  kvx_current_arch = KVX_KV3;
	  break;
	default:
	  error (_ ("The KVX binary is compiled for an unknown core."));
	}
    }
  else if (desc && tdesc_architecture (desc))
    {
      const char *name = tdesc_architecture (desc)->printable_name;
      if (!strncmp (name, "kv3", 6))
	kvx_current_arch = KVX_KV3;
      else
	error ("unable to find the current kvx architecture.");
    }

  if (kvx_current_arch == KVX_NUM_ARCHES)
    return KVX_KV3;

  return kvx_current_arch;
}

const char *
kvx_dummy_register_name (struct gdbarch *gdbarch, int regno)
{
  return "";
}

struct type *
kvx_dummy_register_type (struct gdbarch *gdbarch, int regno)
{
  return builtin_type (gdbarch)->builtin_int;
}

CORE_ADDR
kvx_adjust_breakpoint_address (struct gdbarch *gdbarch, CORE_ADDR bpaddr)
{
  gdb_byte syllab_buf[4];
  uint32_t syllab;
  enum bfd_endian order = gdbarch_byte_order_for_code (gdbarch);
  CORE_ADDR adjusted = bpaddr;
  int i = 0;

  adjusted &= ~3;
  if (adjusted == 0 || ptid_equal (inferior_ptid, null_ptid)
      || !is_stopped (inferior_ptid))
    return adjusted;

  /* Look for the end of the bundle preceeding the requested address. */
  do
    {
      if (target_read_memory (adjusted - 4, syllab_buf, 4) != 0)
	return adjusted;

      syllab = extract_unsigned_integer (syllab_buf, 4, order);
    }
  while ((i++ < 7) && (syllab >> 31) && (adjusted -= 4));

  if (i == 8) /* Impossible for correct instructions */
    return bpaddr & ~3;

  return adjusted;
}

/* Return PC of first real instruction.  */
/* This function is nearly completely copied from
 * symtab.c:skip_prologue_using_lineinfo (expect the test in the loop at the
 * end.
 */
CORE_ADDR
kvx_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR func_addr)
{
  CORE_ADDR func_start, func_end;
  struct linetable *l;
  struct symtab_and_line sal;
  struct compunit_symtab *cust;
  int ind, i, gcc_compiled = 0;

  sal = find_pc_line (func_addr, 0);
  if (sal.symtab == NULL)
    return func_addr;

  cust = find_pc_compunit_symtab (func_addr);
  if (cust->producer && strncmp (cust->producer, "GNU C", 5) == 0)
    gcc_compiled = 1;

  /* Give up if this symbol has no lineinfo table.  */
  l = SYMTAB_LINETABLE (sal.symtab);
  if (l == NULL)
    return func_addr;

  /* Get the range for the function's PC values, or give up if we
     cannot, for some reason.  */
  if (!find_pc_partial_function (func_addr, NULL, &func_start, &func_end))
    return func_addr;

  /* Linetable entries are ordered by PC values, see the commentary in
     symtab.h where `struct linetable' is defined.  Thus, the first
     entry whose PC is in the range [FUNC_START..FUNC_END[ is the
     address we are looking for.  */
  if (gcc_compiled)
    {
      ind = 0;
      for (i = 0; i < l->nitems; i++)
	{
	  struct linetable_entry *item = &(l->item[i]);

	  if (item->line > 0 && func_start <= item->pc && item->pc < func_end)
	    {
	      if (ind == 0)
		++ind;
	      else
		return item->pc;
	    }
	}
    }
  else
    {
      for (i = 0; i < l->nitems; i++)
	{
	  struct linetable_entry *item = &(l->item[i]);

	  /* Don't use line numbers of zero, they mark special entries in
	    the table.  See the commentary on symtab.h before the
	    definition of struct linetable.  */
	  if (item->line > 0 && func_start < item->pc && item->pc < func_end)
	    return item->pc;
	}
    }

  return func_addr;
}

CORE_ADDR
kvx_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_unwind_register_unsigned (next_frame,
					 gdbarch_pc_regnum (gdbarch));
}

/* Given a GDB frame, determine the address of the calling function's
   frame.  This will be used to create a new GDB frame struct.  */
static void
kvx_frame_this_id (struct frame_info *this_frame, void **this_prologue_cache,
		  struct frame_id *this_id)
{
  if (get_frame_func (this_frame) == get_frame_pc (this_frame))
    *this_id
      = frame_id_build (get_frame_sp (this_frame), get_frame_func (this_frame));
}

/* Get the value of register regnum in the previous stack frame.  */
static struct value *
kvx_frame_prev_register (struct frame_info *this_frame,
			void **this_prologue_cache, int regnum)
{
  if (get_frame_func (this_frame) == get_frame_pc (this_frame))
    {
      if (regnum == gdbarch_pc_regnum (get_frame_arch (this_frame)))
	{
	  return frame_unwind_got_register (
	    this_frame, regnum,
	    user_reg_map_name_to_regnum (get_frame_arch (this_frame), "ra",
					 -1));
	}
      return frame_unwind_got_register (this_frame, regnum, regnum);
    }

  return frame_unwind_got_optimized (this_frame, regnum);
}

const struct frame_unwind kvx_frame_unwind = {NORMAL_FRAME,
					     default_frame_unwind_stop_reason,
					     kvx_frame_this_id,
					     kvx_frame_prev_register,
					     NULL,
					     default_frame_sniffer};

void
kvx_dwarf2_frame_init_reg (struct gdbarch *gdbarch, int regnum,
			  struct dwarf2_frame_state_reg *reg,
			  struct frame_info *this_frame)
{
  if (regnum == gdbarch_pc_regnum (gdbarch))
    reg->how = DWARF2_FRAME_REG_RA;
  else if (regnum == gdbarch_sp_regnum (gdbarch))
    reg->how = DWARF2_FRAME_REG_CFA;
}

int
kvx_print_insn (bfd_vma pc, disassemble_info *di)
{
  int res;
  gdb_byte buf[4];

  res = print_insn_kvx (pc, di);

  target_read_memory (pc + res - 4, buf, 4);
  /* Check if the last syllab has the parallel bit set. If so mark
     the current instruction as having a delay slot.  This forces
     x/i to display the following instructions until the end of the
     bundle.  This is of course a lie, but it manages what ze zant
     to achieve: print the full bundle when invoking 'x/i $pc'.  */
  if (buf[3] & 0x80)
    {
      di->insn_info_valid = 1;
      di->branch_delay_insns = 1;
    }

  return res;
}

static CORE_ADDR
kvx_frame_align (struct gdbarch *gdbarch, CORE_ADDR addr)
{
  return addr & KVX_STACK_ALIGN_MASK;
}

struct frame_id
kvx_dummy_id (struct gdbarch *gdbarch, struct frame_info *this_frame)
{
  CORE_ADDR sp;
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  // force read all registers to avoid writing all registers when returning the
  // dummy call
  get_frame_register_unsigned (this_frame, tdep->r0_regnum + 20);

  sp = get_frame_register_unsigned (this_frame, gdbarch_sp_regnum (gdbarch));
  return frame_id_build (sp, get_frame_pc (this_frame));
}

CORE_ADDR
kvx_push_dummy_call (struct gdbarch *gdbarch, struct value *function,
		    struct regcache *regcache, CORE_ADDR bp_addr, int nargs,
		    struct value **args, CORE_ADDR sp, int struct_return,
		    CORE_ADDR struct_addr)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  gdb_byte *argslotsbuf = NULL;
  unsigned int argslotsnb = 0;
  int i, n, r0_regnum, gpr_reg_sz, by_ref;
  const int params_regs = 12;
  const int by_ref_arg_min_nb_regs = 5;

  r0_regnum = user_reg_map_name_to_regnum (gdbarch, "r0", -1);
  gpr_reg_sz = register_size (gdbarch, r0_regnum);

  sp = kvx_frame_align (gdbarch, sp);

  /* Allocate arguments to the virtual argument slots  */
  for (i = 0; i < nargs; i++)
    {
      int newslots, typelen = TYPE_LENGTH (value_enclosing_type (args[i]));

      newslots = (typelen + gpr_reg_sz - 1) / gpr_reg_sz;
      by_ref = 0;
      if (newslots >= by_ref_arg_min_nb_regs)
	{
	  n = (newslots * gpr_reg_sz + KVX_STACK_ALIGN_BYTES - 1)
	      & KVX_STACK_ALIGN_MASK;
	  if (sp < n)
	    error (
	      _ ("Not enough place in stack to store argument %d (size %d)"), i,
	      typelen);
	  sp -= n;
	  write_memory (sp, value_contents (args[i]), typelen);
	  newslots = 1;
	  by_ref = 1;
	}

      argslotsbuf
	= xrealloc (argslotsbuf, (argslotsnb + newslots) * gpr_reg_sz);
      memset (&argslotsbuf[argslotsnb * gpr_reg_sz], 0, newslots * gpr_reg_sz);
      if (by_ref)
	memcpy (&argslotsbuf[argslotsnb * gpr_reg_sz], &sp, sizeof (sp));
      else
	memcpy (&argslotsbuf[argslotsnb * gpr_reg_sz], value_contents (args[i]),
		typelen);

      argslotsnb += newslots;
    }

  n = argslotsnb - params_regs;
  if (n > 0)
    {
      if (sp < n * gpr_reg_sz)
	error (_ ("Not enough place in stack to store arguments not passed "
		  "in registers"));

      sp -= n * gpr_reg_sz;
      write_memory (sp, argslotsbuf + params_regs * gpr_reg_sz, n * gpr_reg_sz);
    }

  n = argslotsnb > params_regs ? params_regs : argslotsnb;
  for (i = 0; i < n; i++)
    regcache_cooked_write (regcache, i + r0_regnum,
			   &argslotsbuf[i * gpr_reg_sz]);

  if (struct_return)
    regcache_cooked_write_unsigned (regcache, r0_regnum + 15, struct_addr);

  regcache_cooked_write_unsigned (regcache, gdbarch_sp_regnum (gdbarch), sp);
  regcache_cooked_write_unsigned (regcache, tdep->ra_regnum, bp_addr);

  return sp;
}

static void
kvx_store_return_value (struct gdbarch *gdbarch, struct type *type,
		       struct regcache *regcache, const gdb_byte *buf)
{
  int i, len = TYPE_LENGTH (type);
  int r0_regnum = user_reg_map_name_to_regnum (gdbarch, "r0", -1);
  int sz = register_size (gdbarch, r0_regnum);

  for (i = 0; len >= sz; i++, len -= sz)
    regcache_raw_write (regcache, i + r0_regnum, buf + i * sz);

  if (len > 0)
    {
      gdb_byte tmp[sz];

      memset (tmp, 0, sz);
      memcpy (tmp, buf + i * sz, len);
      regcache_raw_write (regcache, i + r0_regnum, tmp);
    }
}

static void
kvx_extract_return_value (struct gdbarch *gdbarch, struct type *type,
			 struct regcache *regcache, gdb_byte *buf)
{
  int i, len = TYPE_LENGTH (type);
  int r0_regnum = user_reg_map_name_to_regnum (gdbarch, "r0", -1);
  int sz = register_size (gdbarch, r0_regnum);

  for (i = 0; len >= sz; i++, len -= sz)
    regcache_raw_read (regcache, i + r0_regnum, buf + i * sz);

  if (len > 0)
    {
      gdb_byte tmp[sz];

      regcache_raw_read (regcache, i + r0_regnum, tmp);
      memcpy (buf + i * sz, tmp, len);
    }
}

enum return_value_convention
kvx_return_value (struct gdbarch *gdbarch, struct value *func_type,
		 struct type *type, struct regcache *regcache,
		 gdb_byte *readbuf, const gdb_byte *writebuf)
{
  if (TYPE_LENGTH (type) > 32)
    return RETURN_VALUE_STRUCT_CONVENTION;

  if (writebuf)
    kvx_store_return_value (gdbarch, type, regcache, writebuf);
  else if (readbuf)
    kvx_extract_return_value (gdbarch, type, regcache, readbuf);
  return RETURN_VALUE_REGISTER_CONVENTION;
}

int
kvx_get_longjmp_target (struct frame_info *frame, CORE_ADDR *pc)
{
  /* R0 point to the jmpbuf, and RA is at offset 0xA0 in the buf */
  gdb_byte buf[sizeof (uint64_t)];
  CORE_ADDR r0;
  struct gdbarch *gdbarch = get_frame_arch (frame);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);

  get_frame_register (
    frame, user_reg_map_name_to_regnum (get_frame_arch (frame), "r0", -1), buf);
  r0 = extract_unsigned_integer (buf, sizeof (buf), byte_order);
  // 0xA0 = offset of RA in the jmp_buf struct
  if (target_read_memory (r0 + 0xA0, buf,sizeof (buf)))
    return 0;

  *pc = extract_unsigned_integer (buf, sizeof (buf), byte_order);
  return 1;
}

static void
add_op (struct op_list **list, kv3opc_t *op)
{
  struct op_list *op_l = malloc (sizeof (struct op_list));

  op_l->op = op;
  op_l->next = *list;

  *list = op_l;
}

void
kvx_look_for_insns (void)
{
  int i;

  for (i = 0; i < KVX_NUM_ARCHES; ++i)
    {
      kv3opc_t *op;

      switch (i)
	{
	case KVX_KV3:
	  op = kv3_v1_optab;
	  break;
	default:
	  internal_error (__FILE__, __LINE__, "Unknown arch id.");
	}

      while (op->as_op[0])
	{
	  if (strcmp ("call", op->as_op) == 0)
	    add_op (&branch_insns[i], op);
	  else if (strcmp ("icall", op->as_op) == 0)
	    add_op (&branch_insns[i], op);
	  else if (strcmp ("goto", op->as_op) == 0)
	    add_op (&branch_insns[i], op);
	  else if (strcmp ("igoto", op->as_op) == 0)
	    add_op (&branch_insns[i], op);
	  else if (strcmp ("ret", op->as_op) == 0)
	    add_op (&branch_insns[i], op);
	  else if (strcmp ("rfe", op->as_op) == 0)
	    add_op (&branch_insns[i], op);
	  else if (strcmp ("pcrel", op->as_op) == 0)
	    add_op (&pcrel_insn[i], op);
	  else if (strcmp ("scall", op->as_op) == 0)
	    {
	      if (op->format && op->format[0] && op->format[0]->regs == NULL)
		{
		  break_jtag_over_iss[i]
		    = (op->codewords[0].opcode & op->codewords[0].mask)
		      | ((__NR_BREAKPOINT_JTAGISS
			  & ((1 << op->format[0]->bfield->size) - 1))
			 << op->format[0]->bfield->to_offset);
		  break_op[i]
		    = (op->codewords[0].opcode & op->codewords[0].mask)
		      | ((__NR_BREAKPOINT_PL0
			  & ((1 << op->format[0]->bfield->size) - 1))
			 << op->format[0]->bfield->to_offset);
		}
	      add_op (&branch_insns[i], op);
	    }
	  else if (strncmp ("cb.", op->as_op, 3) == 0)
	    add_op (&branch_insns[i], op);
	  else if (strcmp ("loopdo", op->as_op) == 0)
	    add_op (&branch_insns[i], op);
	  else if (strcmp ("loopgtz", op->as_op) == 0)
	    add_op (&branch_insns[i], op);
	  else if (strcmp ("loopnez", op->as_op) == 0)
	    add_op (&branch_insns[i], op);
	  else if (strcmp ("get", op->as_op) == 0)
	    add_op (&branch_insns[i], op);
	  else if (strcmp ("iget", op->as_op) == 0)
	    add_op (&branch_insns[i], op);
	  else if (strcmp ("nop", op->as_op) == 0)
	    nop_op[i] = op->codewords[0].opcode;

	  ++op;
	}
    }
}
