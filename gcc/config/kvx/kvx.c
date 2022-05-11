/* Definitions of target machine for GNU compiler KVX cores.
   Copyright (C) 1991-2014 Free Software Foundation, Inc.
   Copyright (C) 2017 Kalray

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

#define IN_TARGET_CODE 1

#include "config.h"
#define INCLUDE_STRING
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "tree.h"
#include "memmodel.h"
#include "gimple.h"
#include "cfghooks.h"
#include "cfgloop.h"
#include "df.h"
#include "tm_p.h"
#include "stringpool.h"
#include "attribs.h"
#include "optabs.h"
#include "regs.h"
#include "emit-rtl.h"
#include "recog.h"
#include "cgraph.h"
#include "diagnostic.h"
#include "insn-attr.h"
#include "alias.h"
#include "fold-const.h"
#include "stor-layout.h"
#include "calls.h"
#include "varasm.h"
#include "output.h"
#include "flags.h"
#include "explow.h"
#include "expr.h"
#include "reload.h"
#include "langhooks.h"
#include "opts.h"
#include "gimplify.h"
#include "dwarf2.h"
#include "dumpfile.h"
#include "builtins.h"
#include "rtl-iter.h"
#include "tm-constrs.h"
#include "sched-int.h"
#include "sel-sched.h"
#include "hw-doloop.h"
#include "cfgrtl.h"
#include "ddg.h"
#include "ifcvt.h"

/* This file should be included last.  */
#include "target-def.h"

#undef TARGET_HAVE_TLS
#define TARGET_HAVE_TLS (true)

static bool kvx_scheduling = false;

rtx kvx_link_reg_rtx;

rtx kvx_divmod_zero;

/* Which arch are we scheduling for */
enum attr_arch kvx_arch_schedule;

/* Information about a function's frame layout.  */
struct GTY (()) kvx_frame_info
{
  /* The total frame size, used for moving $sp in prologue */
  poly_int64 frame_size;

  /* Offsets of save area from frame bottom */
  poly_int64 saved_reg_sp_offset;

  /* Relative offsets within register save area  */
  HOST_WIDE_INT reg_rel_offset[FIRST_PSEUDO_REGISTER];
  /* Register save area size */
  HOST_WIDE_INT saved_regs_size;
  HARD_REG_SET saved_regs;

  /* Offset of virtual frame pointer from new stack pointer/frame bottom */
  poly_int64 virt_frame_pointer_offset;

  /* Offset of hard frame pointer from new stack pointer/frame bottom */
  poly_int64 hard_frame_pointer_offset;

  /* The offset of arg_pointer_rtx from the new stack pointer/frame bottom.  */
  poly_int64 arg_pointer_offset;

  /* Offset to the static chain pointer, if needed */
  poly_int64 static_chain_offset;

  /* Padding size between local area and incoming/varargs */
  HOST_WIDE_INT padding1;

  /* Padding between local area and register save */
  HOST_WIDE_INT padding2;

  /* Padding size between register save and outgoing args */
  HOST_WIDE_INT padding3;

  bool laid_out;
};

struct GTY (()) machine_function
{
  char save_reg[FIRST_PSEUDO_REGISTER];

  kvx_frame_info frame;

  /* If true, the current function has a STATIC_CHAIN slot within its stack
   * frame.  Functions which are nested should have one.  A function which only
   * calls a nested function needs not this slot.  This slot is used to store
   * the value of $r31, which is the registers in which the calling functions
   * store the address to its static chain. */
  int static_chain_needed;

  rtx stack_check_block_label;
  rtx stack_check_block_seq, stack_check_block_last;
};

/*
				~               ~
				|  ..........   |
				|               |    ^
				|               |    |
				| Incomming     |    | Caller frame
				| Args          | <--/ <- incoming $sp [256-bits aligned]
				+---------------+
				| Varargs       |
				|               |
				|               |
				+---------------+
				|               |
				| padding1      |
				|               |
				+---------------+
Argument Pointer / Virt. FP-->  | [Static chain]| [256-bits aligned]
				+---------------+
				| Local         |
				| Variable      |
				|               |
				+---------------+
				|               |
				| padding2      |
				|               |
				+---------------+
				|               |
				| Register      |
				| Save          |
				|               |
				| $ra           | (if frame_pointer_needed)
				| caller FP     | (<- $fp if frame_pointer_needed) [64-bits aligned]
				+---------------+
				|               |
				| padding3      |
				|               |
				+---------------+
				|               |
				| Outgoing      |
				| Args          |
				|               | <- $sp [256-bits aligned]
				+---------------+

*/

enum spill_action
{
  SPILL_COMPUTE_SIZE,
  SPILL_SAVE,
  SPILL_RESTORE
};

static bool should_be_saved_in_prologue (int regno);


static void
kvx_compute_frame_info (void)
{
  struct kvx_frame_info *frame;

  poly_int64 inc_sp_offset = 0;

  if (reload_completed && cfun->machine->frame.laid_out)
    return;

  frame = &cfun->machine->frame;
  memset (frame, 0, sizeof (*frame));
  CLEAR_HARD_REG_SET (frame->saved_regs);

  inc_sp_offset += crtl->args.pretend_args_size;

  /* If any anonymous arg may be in register, push them on the stack */
  if (cfun->stdarg && crtl->args.info.next_arg_reg < KV3_ARG_REG_SLOTS)
    inc_sp_offset
      += UNITS_PER_WORD * (KV3_ARG_REG_SLOTS - crtl->args.info.next_arg_reg);

  if (cfun->machine->static_chain_needed)
    inc_sp_offset += UNITS_PER_WORD;

  HOST_WIDE_INT local_vars_sz = get_frame_size ();
  frame->padding1 = 0;

  if (local_vars_sz > 0)
    {
      frame->padding1 = ROUND_UP (inc_sp_offset, STACK_BOUNDARY / BITS_PER_UNIT)
			- inc_sp_offset;
      inc_sp_offset = ROUND_UP (inc_sp_offset, STACK_BOUNDARY / BITS_PER_UNIT);

      /* Next are automatic variables. */
      inc_sp_offset += local_vars_sz;
    }
#define SLOT_NOT_REQUIRED (-2)
#define SLOT_REQUIRED (-1)

  frame->padding2 = ROUND_UP (inc_sp_offset, UNITS_PER_WORD) - inc_sp_offset;
  inc_sp_offset = ROUND_UP (inc_sp_offset, UNITS_PER_WORD);

  HOST_WIDE_INT reg_offset = 0;

  /* Mark which register should be saved... */
  for (int regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    if (should_be_saved_in_prologue (regno))
      {
	SET_HARD_REG_BIT (frame->saved_regs, regno);
	cfun->machine->frame.reg_rel_offset[regno] = SLOT_REQUIRED;
      }
    else
      cfun->machine->frame.reg_rel_offset[regno] = SLOT_NOT_REQUIRED;

  if (frame_pointer_needed)
    {
      SET_HARD_REG_BIT (frame->saved_regs, HARD_FRAME_POINTER_REGNUM);
      SET_HARD_REG_BIT (frame->saved_regs, KV3_RETURN_POINTER_REGNO);

      cfun->machine->frame.reg_rel_offset[HARD_FRAME_POINTER_REGNUM] = 0;
      cfun->machine->frame.reg_rel_offset[KV3_RETURN_POINTER_REGNO]
	= UNITS_PER_WORD;
      reg_offset = UNITS_PER_WORD * 2;
    }

  /* ... assign stack slots */
  for (int regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    if (cfun->machine->frame.reg_rel_offset[regno] == SLOT_REQUIRED)
      {
	cfun->machine->frame.reg_rel_offset[regno] = reg_offset;
	reg_offset += UNITS_PER_WORD;
      }

  frame->saved_regs_size = reg_offset;

  inc_sp_offset += reg_offset;

  /* At the bottom of the frame are any outgoing stack arguments. */
  inc_sp_offset += crtl->outgoing_args_size;
  frame->padding3
    = ROUND_UP (inc_sp_offset, STACK_BOUNDARY / BITS_PER_UNIT) - inc_sp_offset;

  inc_sp_offset = ROUND_UP (inc_sp_offset, STACK_BOUNDARY / BITS_PER_UNIT);

  frame->hard_frame_pointer_offset = frame->saved_reg_sp_offset
    = crtl->outgoing_args_size + frame->padding3;

  frame->static_chain_offset = frame->virt_frame_pointer_offset
    = frame->saved_reg_sp_offset + frame->saved_regs_size + frame->padding2
      + get_frame_size ();

  frame->arg_pointer_offset
    = frame->virt_frame_pointer_offset + frame->padding1
      + (cfun->machine->static_chain_needed ? UNITS_PER_WORD : 0);

  frame->frame_size = inc_sp_offset;
  frame->laid_out = true;
}

static void
kvx_debug_frame_info (struct kvx_frame_info *fi)
{
  if (!dump_file)
    return;
  fprintf (dump_file, "\nKVX Frame info:\n");

  fprintf (dump_file,
	   " |XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX| %ld/0 (caller frame) <- $sp "
	   "(incoming) %s \n",
	   fi->frame_size.to_constant (),
	   cfun->stdarg && crtl->args.info.next_arg_reg < KV3_ARG_REG_SLOTS
	     ? ""
	     : "and virt frame pointer");

#define DFI_SEP fprintf (dump_file, " +------------------------------+    \n")

#define DFI_FIELD(f, size, bottom, decorate_up, decorate_down)                 \
  fprintf (dump_file,                                                          \
	   " |                              | %ld/%ld %s  \n"                  \
	   " |%30s|                              \n"                           \
	   " |size: %24ld| %ld/%ld  %s\n",                                     \
	   (bottom) + (size) -UNITS_PER_WORD,                                  \
	   (fi->frame_size.to_constant ())                                     \
	     - ((bottom) + (size) -UNITS_PER_WORD),                            \
	   decorate_up, f, (size), (bottom),                                   \
	   (fi->frame_size.to_constant ()) - (bottom), decorate_down)

  DFI_SEP;
  if (cfun->stdarg && crtl->args.info.next_arg_reg < KV3_ARG_REG_SLOTS)
    {
      DFI_FIELD ("varargs", (fi->frame_size - fi->arg_pointer_offset).to_constant(),
		 fi->arg_pointer_offset.to_constant(), "", " <- arg pointer");
      DFI_SEP;
    }

  if (fi->padding1 > 0)
    {
      DFI_FIELD (
	"padding1", fi->padding1,
	fi->virt_frame_pointer_offset.to_constant ()
		   + (cfun->machine->static_chain_needed ? UNITS_PER_WORD : 0),
		 "",
		 cfun->machine->static_chain_needed ? ""
						    : "<- virt frame pointer");
      DFI_SEP;
    }
  if (cfun->machine->static_chain_needed)
    {
      DFI_FIELD ("static chain", (long) UNITS_PER_WORD,
		 fi->virt_frame_pointer_offset.to_constant (), "",
		 "<- virt frame pointer");
      DFI_SEP;
    }

  if (get_frame_size () > 0)
    {
      DFI_FIELD ("locals", get_frame_size ().to_constant (),
		 fi->virt_frame_pointer_offset.to_constant ()
		   - get_frame_size ().to_constant (),
		 "", "");
      DFI_SEP;
    }
  if (fi->padding2 > 0)
    {
      DFI_FIELD ("padding2", fi->padding2,
		 (fi->hard_frame_pointer_offset + fi->saved_regs_size).to_constant(), "", "");
      DFI_SEP;
    }

  if (fi->saved_regs_size > 0)
    {
      unsigned regno;
      hard_reg_set_iterator rsi;
      /* 64 should be already oversized as there are 64 GPRS + possibly $fp and
       * $ra */
      unsigned stacked_regs[64] = {0};

      EXECUTE_IF_SET_IN_HARD_REG_SET (fi->saved_regs, 0, regno, rsi)
      stacked_regs[fi->reg_rel_offset[regno] / UNITS_PER_WORD] = regno;

      for (int i = (fi->saved_regs_size / UNITS_PER_WORD) - 1; i != 0; i--)
	fprintf (dump_file, " |%30s| %ld/-\n", reg_names[stacked_regs[i]],
		 fi->hard_frame_pointer_offset.to_constant ()
		   + fi->reg_rel_offset[stacked_regs[i]]);

      fprintf (dump_file, " |%21s (%ld)%4s| %ld/- %s\n", "saved regs",
	       fi->saved_regs_size, reg_names[stacked_regs[0]],
	       fi->hard_frame_pointer_offset.to_constant (),
	       frame_pointer_needed ? "<- hard frame pointer ($fp)" : "");

      DFI_SEP;
    }
  if (fi->padding3 > 0)
    {
      if (crtl->outgoing_args_size > 0)
	{
	  DFI_FIELD ("padding3", fi->padding3,
		     (crtl->outgoing_args_size + fi->padding3).to_constant(), "", "");
	  DFI_SEP;
	}
      else
	{
	  DFI_FIELD ("padding3", fi->padding3, 0L, "", "<- $sp (callee)");
	  DFI_SEP;
	}
    }
  if (crtl->outgoing_args_size > 0)
    {
      DFI_FIELD ("outgoing", crtl->outgoing_args_size.to_constant (), 0L, "",
		 "<- $sp (callee)");
      DFI_SEP;
    }

  fprintf (dump_file, "Saved regs: ");
  unsigned regno;
  hard_reg_set_iterator rsi;
  EXECUTE_IF_SET_IN_HARD_REG_SET (fi->saved_regs, 0, regno, rsi)
  fprintf (dump_file, " $%s [%ld]", reg_names[regno],
	   fi->reg_rel_offset[regno]);

  fprintf (dump_file, "\n");

  fprintf (dump_file, "KVX Frame info valid :%s\n",
	   fi->laid_out ? "yes" : "no");
}

static HOST_WIDE_INT
kvx_starting_frame_offset (void)
{
  return 0;
}

HOST_WIDE_INT
kvx_first_parm_offset (tree decl ATTRIBUTE_UNUSED)
{
  struct kvx_frame_info *frame;
  kvx_compute_frame_info ();
  frame = &cfun->machine->frame;

  return frame->arg_pointer_offset - frame->virt_frame_pointer_offset;
}

static rtx
kvx_static_chain (const_tree ARG_UNUSED (fndecl), bool incoming_p)
{
  if (incoming_p)
    {
      cfun->machine->static_chain_needed = 1;
      return gen_frame_mem (Pmode, frame_pointer_rtx);
    }
  else
    {
      return gen_rtx_REG (Pmode, 31);
    }
}

/*
 * Takes a location MEM and write a make opcode in the stack which
 * loads the ADDR into the general purpose register REGNO.
 * Returns the number of byte written on the stack.
 */
static int
kvx_emit_make_of_addr (rtx loc, rtx addr, unsigned int regno)
{
  int offset = 0;
  rtx mem;
  unsigned long mask_extend27 = 0xffffffe000000000U,
		mask_upper27 = 0x0000001ffffffc00U,
		mask_lower10 = 0x00000000000003ffU;

  rtx addr_lower10 = copy_to_reg (addr);
  mem = adjust_address (loc, Pmode, offset);
  emit_insn (gen_anddi3 (addr_lower10, addr_lower10, GEN_INT (mask_lower10)));
  emit_insn (gen_ashldi3 (addr_lower10, addr_lower10, GEN_INT (6U)));
  emit_insn (
    gen_add2_insn (addr_lower10, GEN_INT (((0xe0 << 8) + (regno << 2)) << 16)));
  emit_move_insn (mem, addr_lower10);
  offset += 4;

  rtx addr_upper27 = copy_to_reg (addr);
  mem = adjust_address (loc, Pmode, offset);
  emit_insn (gen_anddi3 (addr_upper27, addr_upper27, GEN_INT (mask_upper27)));
  emit_insn (gen_lshrdi3 (addr_upper27, addr_upper27, GEN_INT (10)));
  emit_insn (gen_add2_insn (addr_upper27, GEN_INT (0x8 << 28)));
  emit_move_insn (mem, addr_upper27);
  offset += 4;

  rtx addr_extend27 = copy_to_reg (addr);
  emit_insn (
    gen_anddi3 (addr_extend27, addr_extend27, GEN_INT (mask_extend27)));
  emit_insn (gen_lshrdi3 (addr_extend27, addr_extend27, GEN_INT (37)));
  mem = adjust_address (loc, Pmode, offset);
  emit_move_insn (mem, addr_extend27);
  offset += 4;

  return offset;
}

/*
     This hook is called to initialize a trampoline.  M_TRAMP is an RTX
     for the memory block for the trampoline; FNDECL is the
     'FUNCTION_DECL' for the nested function; STATIC_CHAIN is an RTX for
     the static chain value that should be passed to the function when
     it is called.

     If the target requires any other actions, such as flushing caches
     or enabling stack execution, these actions should be performed
     after initializing the trampoline proper.
*/
static void
kvx_trampoline_init (rtx m_tramp, tree fndecl, rtx chain_value)
{
  rtx mem, fnaddr = XEXP (DECL_RTL (fndecl), 0);
  int offset = 0;

  // make $31 = static_chain_addr
  mem = adjust_address (m_tramp, Pmode, offset);
  offset += kvx_emit_make_of_addr (mem, chain_value, 31);

  // make $30 = fnaddr
  mem = adjust_address (m_tramp, Pmode, offset);
  offset += kvx_emit_make_of_addr (mem, fnaddr, 30);

  // igoto $r30
  mem = adjust_address (m_tramp, SImode, offset);
  emit_move_insn (mem, gen_int_mode (0x0fd8001e, SImode));
  offset += 4;

  emit_insn (gen_kvx_i1invals (chain_value));
  emit_insn (gen_kvx_fence (gen_rtx_CONST_STRING (VOIDmode, "")));

  gcc_assert (offset <= TRAMPOLINE_SIZE);
}

static const char *
kvx_pgr_reg_name (unsigned regno)
{
  static const char *pgr_reg_names[] = {KV3_PGR_REGISTER_NAMES};
  unsigned index = regno - KV3_GPR_FIRST_REGNO;
  gcc_assert (index % 2 == 0);
  return pgr_reg_names[index / 2];
}

static const char *
kvx_qgr_reg_name (unsigned regno)
{
  static const char *qgr_reg_names[] = {KV3_QGR_REGISTER_NAMES};
  unsigned index = regno - KV3_GPR_FIRST_REGNO;
  gcc_assert (index % 4 == 0);
  return qgr_reg_names[index / 4];
}

static const char *
kvx_xvr_reg_name (unsigned regno)
{
  static const char *xvr_reg_names[] = {KV3_XVR_REGISTER_NAMES};
  unsigned index = regno - KV3_XCR_FIRST_REGNO;
  gcc_assert (index % 4 == 0);
  return xvr_reg_names[index / 4];
}

static const char *
kvx_xwr_reg_name (unsigned regno)
{
  static const char *xwr_reg_names[] = {KV3_XWR_REGISTER_NAMES};
  unsigned index = regno - KV3_XCR_FIRST_REGNO;
  gcc_assert (index % 8 == 0);
  return xwr_reg_names[index / 8];
}

static const char *
kvx_xmr_reg_name (unsigned regno)
{
  static const char *xmr_reg_names[] = {KV3_XMR_REGISTER_NAMES};
  unsigned index = regno - KV3_XCR_FIRST_REGNO;
  gcc_assert (index % 16 == 0);
  return xmr_reg_names[index / 16];
}

/* Splits X as a base + offset. Returns true if split successful,
   false if not. BASE_OUT and OFFSET_OUT contain the corresponding
   split. If STRICT is false, base is not always a register.
 */
bool
kvx_split_mem (rtx x, rtx *base_out, rtx *offset_out, bool strict)
{
  if (GET_CODE (x) != PLUS && !REG_P (x))
    return false;

  if (strict
      && !((GET_CODE (x) == PLUS && CONST_INT_P (XEXP (x, 1))
	    && REG_P (XEXP (x, 0)))
	   || REG_P (x)))
    return false;

  if (GET_CODE (x) == PLUS && CONST_INT_P (XEXP (x, 1)))
    {
      *base_out = XEXP (x, 0);
      *offset_out = XEXP (x, 1);
      return true;
    }

  *base_out = x;
  *offset_out = const0_rtx;
  return true;
}

#define KVX_MAX_PACKED_LSU (4)
/* Used during peephole to merge consecutive loads/stores.
   Returns TRUE if the merge was successful, FALSE if not.
   NOPS is the number of load/store to consider in OPERANDS array.
 */

/* OPERANDS contains NOPS (set ...) (2 or 4) that must be all load or
   all store.  The sets are checked for correctness wrt packing.
   On success, the function emits the packed instruction and returns
   TRUE. If the packing could not be done, returns FALSE.
 */
bool
kvx_pack_load_store (rtx operands[], int nops)
{
  rtx set_dests[KVX_MAX_PACKED_LSU];
  rtx set_srcs[KVX_MAX_PACKED_LSU];
  rtx sorted_operands[2 * KVX_MAX_PACKED_LSU];

  /* Only:
     ld + ld => lq
     ld + ld + ld + ld => lo

     sd + sd => sq
     sd + sd + sd + sd => so
  */

  if (nops != 2 && nops != 4)
    return false;

  for (int i = 0; i < nops; i++)
    {
      set_dests[i] = operands[2 * i];
      set_srcs[i] = operands[2 * i + 1];
      sorted_operands[2 * i] = sorted_operands[2 * i + 1] = NULL_RTX;
    }

  /* Only for register size accesses */
  for (int i = 0; i < nops; i++)
    if (GET_MODE (set_dests[i]) != DImode)
      return false;

  bool is_load = false;
  bool is_store = false;
  for (int i = 0; i < nops; i++)
    if (MEM_P (set_srcs[i]) && REG_P (set_dests[i]))
      is_load = true;
    else if (MEM_P (set_dests[i]) && REG_P (set_srcs[i]))
      is_store = true;

  if ((is_store && is_load) || !(is_load || is_store))
    return false;

  /* Used to pick correct operands in both cases (load and store) */
  int op_offset = is_load ? 0 : 1;

  unsigned int min_regno = REGNO (operands[op_offset]);

  /* Find first regno for destination (load)/source (store) */
  for (int i = 1; i < nops; i++)
    if (REGNO (operands[i * 2 + op_offset]) < min_regno)
      min_regno = REGNO (operands[i * 2 + op_offset]);

  /* Sort operands based on regno */
  for (int i = 0; i < nops; i++)
    {
      const int regno = REGNO (operands[i * 2 + op_offset]);
      const int idx = 2 * (regno - min_regno);

      /* Registers are not consecutive */
      if (idx >= (2 * nops))
	return false;

      /* Register used twice in operands */
      if (sorted_operands[idx] != NULL_RTX)
	return false;

      sorted_operands[idx] = operands[2 * i];
      sorted_operands[idx + 1] = operands[2 * i + 1];
    }

  /* Check mem addresses are consecutive */
  rtx base_reg, base_offset;
  if (!kvx_split_mem (XEXP (sorted_operands[1 - op_offset], 0), &base_reg,
		      &base_offset, true))
    return false;

  const unsigned int base_regno = REGNO (base_reg);

  /* Base register is modified by one load */
  if (is_load && base_regno >= REGNO (sorted_operands[op_offset])
      && base_regno <= REGNO (sorted_operands[(nops - 1) * 2 + op_offset]))
    {
      bool mod_before_last = false;
      /* Check the base register is modified in the last load */
      for (int i = 0; i < (nops - 1); i++)
	{
	  if (REGNO (operands[2 * i + op_offset]) == base_regno)
	    {
	      mod_before_last = true;
	      break;
	    }
	}
      if (mod_before_last)
	return false;
    }

  unsigned int next_offset = INTVAL (base_offset) + UNITS_PER_WORD;
  for (int i = 1; i < nops; i++)
    {
      rtx elem = XEXP (sorted_operands[2 * i + 1 - op_offset], 0);

      /* Not addressing next memory word */
      const bool is_plus_bad_offset
	= GET_CODE (elem) == PLUS
	  && (!REG_P (XEXP (elem, 0)) || REGNO (XEXP (elem, 0)) != base_regno
	      || !CONST_INT_P (XEXP (elem, 1))
	      || INTVAL (XEXP (elem, 1)) != next_offset);

      const bool is_reg_bad
	= REG_P (elem) && (REGNO (elem) != base_regno || next_offset != 0);

      if (is_reg_bad || is_plus_bad_offset)
	return false;

      next_offset += UNITS_PER_WORD;
    }

  rtx multi_insn;
  if (is_load)
    multi_insn = gen_load_multiple (sorted_operands[0], sorted_operands[1],
				    GEN_INT (nops));
  else
    multi_insn = gen_store_multiple (sorted_operands[0], sorted_operands[1],
				     GEN_INT (nops));
  if (multi_insn == NULL_RTX)
    return false;

  emit_insn (multi_insn);
  return true;
}

/* Implements TARGET_HARD_REGNO_NREGS. */
static unsigned int
kvx_hard_regno_nregs (unsigned int regno ATTRIBUTE_UNUSED, machine_mode mode)
{
  return (GET_MODE_SIZE (mode) + UNITS_PER_WORD - 1) / UNITS_PER_WORD;
}

static bool
kvx_extension_mode_p (enum machine_mode mode)
{
  switch (mode)
    {
    // 256-bit coprocessor modes
    case E_V1OImode:
    // 512-bit coprocessor modes
    case E_V2OImode:
    // 1024-bit coprocessor modes
    case E_V4OImode:
    // 2048-bit coprocessor modes
    case E_V8OImode:
    // 4096-bit coprocessor modes
    case E_V16OImode:
    // 8192-bit coprocessor modes
    case E_V32OImode:
      return true;
    default:
      break;
    }
  return false;
}

/* Implements TARGET_HARD_REGNO_MODE_OK.  */
static bool
kvx_hard_regno_mode_ok (unsigned regno, enum machine_mode mode)
{
  int nwords = (GET_MODE_SIZE (mode) + UNITS_PER_WORD - 1) / UNITS_PER_WORD;
  nwords = nwords >= 1 ? nwords : 1;
  gcc_assert (__builtin_popcount (nwords) == 1);
  // GPR
  if (IN_RANGE (regno, KV3_GPR_FIRST_REGNO, KV3_GPR_LAST_REGNO))
    {
      return !(regno & (nwords - 1));
    }
  // SFR
  if (IN_RANGE (regno, KV3_SFR_FIRST_REGNO, KV3_SFR_LAST_REGNO))
    {
      return nwords == 1;
    }
  // XCR
  if (IN_RANGE (regno, KV3_XCR_FIRST_REGNO, KV3_XCR_LAST_REGNO))
    {
      return !(regno & (nwords - 1)) && kvx_extension_mode_p (mode);
    }
  return false;
}

/* Implements TARGET_CLASS_MAX_NREGS.  */
static unsigned char
kvx_class_max_nregs (reg_class_t regclass ATTRIBUTE_UNUSED,
		     enum machine_mode mode)
{
  return kvx_hard_regno_nregs (0, mode);
}

static tree kvx_handle_fndecl_attribute (tree *node, tree name,
					 tree args ATTRIBUTE_UNUSED,
					 int flags ATTRIBUTE_UNUSED,
					 bool *no_add_attrs);

static bool function_symbol_referenced_p (rtx x);

static bool symbolic_reference_mentioned_p (rtx op);

static bool kvx_output_addr_const_extra (FILE *, rtx);

static bool kvx_legitimate_address_p (enum machine_mode mode, rtx x,
				      bool strict);

bool kvx_legitimate_pic_symbolic_ref_p (rtx op);

static bool kvx_legitimate_constant_p (enum machine_mode mode ATTRIBUTE_UNUSED,
				       rtx x);

/* Implements TARGET_SECONDARY_RELOAD.  */
static reg_class_t
kvx_secondary_reload (bool in_p ATTRIBUTE_UNUSED, rtx x ATTRIBUTE_UNUSED,
		      reg_class_t reload_class ATTRIBUTE_UNUSED,
		      enum machine_mode reload_mode ATTRIBUTE_UNUSED,
		      secondary_reload_info *sri ATTRIBUTE_UNUSED)
{
  if (flag_pic)
    {
      if (in_p && GET_CODE (x) == SYMBOL_REF && SYMBOL_REF_LOCAL_P (x)
	  && !SYMBOL_REF_EXTERNAL_P (x))
	{
	  if (sri->prev_sri == NULL)
	    {
	      return GPR_REGS;
	    }
	  else
	    {
	      sri->icode = GET_MODE (x) == SImode ? CODE_FOR_reload_in_gotoff_si
						  : CODE_FOR_reload_in_gotoff_di;
	      return NO_REGS;
	    }
	}
      else if (SYMBOLIC_CONST (x) && !kvx_legitimate_constant_p (VOIDmode, x))
	{
	  gcc_unreachable ();
	}
    }

#if 0
  if (reload_class == XCR_REGS && CONSTANT_P(x))
    return GENERAL_REGS;
#endif

  return NO_REGS;
}

/* Table of machine attributes.  */
static const struct attribute_spec kvx_attribute_table[] = {
  /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler,
     affects_type } */
  {"no_save_regs", 0, 0, true, false, false, false, kvx_handle_fndecl_attribute,
   NULL},
  {"farcall", 0, 0, true, false, false, false, kvx_handle_fndecl_attribute, NULL},
  {NULL, 0, 0, false, false, false, false, NULL, NULL}};

/* Returns 0 if there is no TLS ref, != 0 if there is.

  Beware that UNSPEC_TLS are not symbol ref, they are offset within
  TLS.
 */
int
kvx_has_tls_reference (rtx x)
{
  if (!TARGET_HAVE_TLS)
    return false;

  subrtx_iterator::array_type array;
  FOR_EACH_SUBRTX (iter, array, x, ALL)
    {
      const_rtx x = *iter;
      if (GET_CODE (x) == SYMBOL_REF && SYMBOL_REF_TLS_MODEL (x) != 0)
	return true;
      /* Don't recurse into UNSPEC_TLS looking for TLS symbols; these are
	 TLS offsets, not real symbol references.  */
      if (GET_CODE (x) == UNSPEC
	  && (XINT (x, 1) == UNSPEC_TLS_GD || XINT (x, 1) == UNSPEC_TLS_LD
	      || XINT (x, 1) == UNSPEC_TLS_DTPOFF
	      || XINT (x, 1) == UNSPEC_TLS_LE || XINT (x, 1) == UNSPEC_TLS_IE))
	iter.skip_subrtxes ();
    }
  return false;
}

static int
kvx_has_unspec_reference_1 (rtx *x)
{
  return (GET_CODE (*x) == UNSPEC
	  && (XINT (*x, 1) == UNSPEC_GOT || XINT (*x, 1) == UNSPEC_GOTOFF
	      || XINT (*x, 1) == UNSPEC_PCREL || XINT (*x, 1) == UNSPEC_TLS_GD
	      || XINT (*x, 1) == UNSPEC_TLS_LD || XINT (*x, 1) == UNSPEC_TLS_LE
	      || XINT (*x, 1) == UNSPEC_TLS_IE));
}

static int
kvx_has_unspec_reference (rtx x)
{
  subrtx_ptr_iterator::array_type array;
  FOR_EACH_SUBRTX_PTR (iter, array, &x, ALL)
    {
      rtx *x = *iter;
      switch (kvx_has_unspec_reference_1 (x))
	{
	case -1:
	  iter.skip_subrtxes ();
	  break;
	case 0:
	  break;
	default:
	  return 1;
	}
    }
  return 0;
  //   return for_each_rtx (&x, &kvx_has_unspec_reference_1, NULL);
}

static bool
kvx_legitimate_address_register_p (rtx reg, bool strict)
{
  return (REG_P (reg) && IS_GENERAL_REGNO (REGNO (reg), strict)
	  && GET_MODE (reg) == Pmode);
}

static bool
kvx_legitimate_address_offset_register_p (rtx reg, bool strict)
{
  machine_mode mode = GET_MODE (reg);

  if (GET_CODE (reg) == SUBREG)
    reg = SUBREG_REG (reg);

  return (REG_P (reg) && IS_GENERAL_REGNO (REGNO (reg), strict)
	  && mode == Pmode);
}

/**
 * Legitimate address :
 * - (reg)
 * - (plus (reg) (constant))
 * - (plus (reg) (reg))
 * - (plus (mult (reg) (constant)) (reg))
 */
static bool
kvx_legitimate_address_p (machine_mode mode, rtx x, bool strict)
{
  /*
   * ld reg = 0[reg]
   */
  if (kvx_legitimate_address_register_p (x, strict))
    return true;

  /*
   * ld reg = @got[reg]
   * ld reg = @gotoff[reg]
   */
  if (GET_CODE (x) == PLUS
      && kvx_legitimate_address_register_p (XEXP (x, 0), strict)
      && (GET_CODE (XEXP (x, 1)) == UNSPEC
	  && (XINT (XEXP (x, 1), 1) == UNSPEC_GOT
	      || XINT (XEXP (x, 1), 1) == UNSPEC_GOTOFF)))
    return true;

  /*
   * ld reg = const[reg]
   * ld reg = symbol[reg]
   * ld reg = @pcrel(symbol)[reg]
   */
  if (GET_CODE (x) == PLUS
      && kvx_legitimate_address_register_p (XEXP (x, 0), strict)
      && ((CONSTANT_P (XEXP (x, 1))
	   && kvx_legitimate_constant_p (VOIDmode, XEXP (x, 1)))
	  || GET_CODE (XEXP (x, 1)) == CONST_INT)
      && immediate_operand (XEXP (x, 1), DImode)
      && (!current_pass || current_pass->tv_id != TV_CPROP))
    return true;

  /*
   * ld reg = reg[reg]
   */
  if (GET_CODE (x) == PLUS
      && kvx_legitimate_address_register_p (XEXP (x, 0), strict)
      && kvx_legitimate_address_offset_register_p (XEXP (x, 1), strict))
    return true;

  /*
   * ld.xs reg = reg[reg]
   */
  if (GET_CODE (x) == PLUS
      && kvx_legitimate_address_register_p (XEXP (x, 1), strict)
      && GET_CODE (XEXP (x, 0)) == MULT
      && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT
      && INTVAL (XEXP (XEXP (x, 0), 1)) == GET_MODE_SIZE (mode)
      && kvx_legitimate_address_offset_register_p (XEXP (XEXP (x, 0), 0),
						   strict))
    // The .xs addressing mode applies to object sizes 2, 4, 8, 16, 32.
    return GET_MODE_SIZE (mode) > 1 && GET_MODE_SIZE (mode) <= 32;

  return false;
}

static void
kvx_conditional_register_usage (void)
{
  kvx_link_reg_rtx = gen_rtx_REG (Pmode, KV3_RETURN_POINTER_REGNO);
  if (KV3_1)
    {
      // On the kv3-1, only 48 coprocessor registers are available.
      unsigned int regno = KV3_XCR_FIRST_REGNO + (48 * 4);
      for (; regno <= KV3_XCR_LAST_REGNO; regno++)
	fixed_regs[regno] = call_used_regs[regno] = 1;
    }
}

rtx
kvx_return_addr_rtx (int count, rtx frameaddr ATTRIBUTE_UNUSED)
{
  return count == 0 ? get_hard_reg_initial_val (Pmode, KV3_RETURN_POINTER_REGNO)
		    : NULL_RTX;
}

/* Implements INIT_CUMULATIVE_ARGS. */

void
kvx_init_cumulative_args (CUMULATIVE_ARGS *cum,
			  const_tree fntype ATTRIBUTE_UNUSED,
			  rtx libname ATTRIBUTE_UNUSED,
			  tree fndecl ATTRIBUTE_UNUSED,
			  int n_named_args ATTRIBUTE_UNUSED)
{
  cum->next_arg_reg = 0;
  cum->anonymous_arg_offset = 0;
  cum->anonymous_arg_offset_valid = false;
}

/* Information about a single argument.  */
struct kvx_arg_info
{
  /* first register to be used for this arg */
  unsigned int first_reg;

  /* number of registers used */
  int num_regs;

  /* number of words pushed on the stack (in excess of registers) */
  int num_stack;
};

/* Analyzes a single argument and fills INFO struct. Does not modify
   CUM_V. Returns a reg rtx pointing at first argument register to be
   used for given argument or NULL_RTX if argument must be stacked
   because there is no argument slot in registers free/correctly
   aligned. */

static rtx
kvx_get_arg_info (struct kvx_arg_info *info, cumulative_args_t cum_v,
		  machine_mode mode, const_tree type,
		  bool named ATTRIBUTE_UNUSED)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
  HOST_WIDE_INT n_bytes
    = type ? int_size_in_bytes (type) : GET_MODE_SIZE (mode);
  HOST_WIDE_INT n_words = (n_bytes + UNITS_PER_WORD - 1) / UNITS_PER_WORD;

  info->first_reg = cum->next_arg_reg;

  /* If all register argument slots are used, then it must go on the
     stack. */
  if (cum->next_arg_reg >= KV3_ARG_REG_SLOTS)
    {
      info->num_stack = n_words;
      info->num_regs = 0;
      return NULL_RTX;
    }

  info->num_regs = KV3_ARG_REG_SLOTS - info->first_reg;

  if (info->num_regs >= n_words)
    {
      /* All arg fits in remaining registers */
      info->num_regs = n_words;
      info->num_stack = 0;
    }
  else
    {
      /* At least one word on stack */
      info->num_stack = n_words - info->num_regs;
    }

  return gen_rtx_REG (mode, KV3_ARGUMENT_POINTER_REGNO + info->first_reg);
}

/* Implements TARGET_PROMOTE_FUNCTION_MODE.  */
static machine_mode
kvx_promote_function_mode (const_tree type ATTRIBUTE_UNUSED, machine_mode mode,
			   int *punsignedp, const_tree, int for_return)
{
  if (for_return)
    if (GET_MODE_CLASS (mode) == MODE_INT
	&& GET_MODE_SIZE (mode) < UNITS_PER_WORD)
      {
	mode = word_mode;
	*punsignedp = 1;
      }

  return mode;
}

/* Implements TARGET_FUNCTION_ARG.
   Returns a reg rtx pointing at first argument register to be
   used for given argument or NULL_RTX if argument must be stacked
   because there is no argument slot in registers free. */

static rtx
kvx_function_arg (cumulative_args_t cum_v, const function_arg_info &arg)
{
  struct kvx_arg_info info = {0, 0, 0};
  return kvx_get_arg_info (&info, cum_v, arg.mode, arg.type, arg.named);
}

/* Implements TARGET_ARG_PARTIAL_BYTES.
   Return the number of bytes, at the beginning of an argument,
   that must be put in registers */

static int
kvx_arg_partial_bytes (cumulative_args_t cum_v, const function_arg_info &arg)
{
  struct kvx_arg_info info = {0, 0, 0};
  rtx reg = kvx_get_arg_info (&info, cum_v, arg.mode, arg.type, arg.named);
  if (reg != NULL_RTX && info.num_regs > 0 && info.num_stack > 0)
    {
      return info.num_regs * UNITS_PER_WORD;
    }
  return 0;
}

static void
kvx_function_arg_advance (cumulative_args_t cum_v, const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
  struct kvx_arg_info info = {0, 0, 0};
  kvx_get_arg_info (&info, cum_v, arg.mode, arg.type, arg.named);

  if (info.num_regs > 0)
    {
      cum->next_arg_reg = info.first_reg + info.num_regs;
    }
  else if (arg.named)
    {
      cum->anonymous_arg_offset += info.num_stack * UNITS_PER_WORD;
    }
  else if (!cum->anonymous_arg_offset_valid)
    {
      /* First !named arg is in fact the last named arg */
      cum->anonymous_arg_offset += info.num_stack * UNITS_PER_WORD;
      cum->anonymous_arg_offset_valid = true;
    }
}

/* Implements TARGET_FUNCTION_VALUE.  */
static rtx
kvx_function_value (const_tree ret_type, const_tree func,
		    bool outgoing ATTRIBUTE_UNUSED)
{
  int unsignedp = TYPE_UNSIGNED (ret_type);
  enum machine_mode mode = TYPE_MODE (ret_type);
  HOST_WIDE_INT size = int_size_in_bytes (ret_type);

  mode = promote_function_mode (ret_type, mode, &unsignedp, func, 1);

  if (mode == BLKmode && (size * BITS_PER_UNIT > LONG_LONG_TYPE_SIZE))
    {
      int nexps = (size + UNITS_PER_WORD - 1) / UNITS_PER_WORD;
      rtx ret = gen_rtx_PARALLEL (BLKmode, rtvec_alloc (nexps));

      for (int i = 0; i < nexps; i++)
	XVECEXP (ret, 0, i)
	  = gen_rtx_EXPR_LIST (VOIDmode,
			       gen_rtx_REG (DImode,
					    KV3_ARGUMENT_POINTER_REGNO + i),
			       GEN_INT (i * UNITS_PER_WORD));
      return ret;
    }

  return gen_rtx_REG (mode, KV3_ARGUMENT_POINTER_REGNO);
}

/* Implements TARGET_RETURN_IN_MSB.  */
static bool
kvx_return_in_msb (const_tree type ATTRIBUTE_UNUSED)
{
  return false;
}

/* Implements TARGET_RETURN_IN_MEMORY.  */
static bool
kvx_return_in_memory (const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  HOST_WIDE_INT size = int_size_in_bytes (type);

  /* Coprocessor values return in memory. */
  if (kvx_extension_mode_p (TYPE_MODE (type)))
    return true;

  /* Return value can use up to 4 registers (256bits). Larger values
   * or variable sized type must be returned in memory. */
  return (size > (4 * UNITS_PER_WORD) || size < 0);
}

/* Implements TARGET_STRUCT_VALUE_RTX.  */
static rtx
kvx_struct_value_rtx (tree fndecl ATTRIBUTE_UNUSED,
		      int incoming ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (Pmode, KV3_STRUCT_POINTER_REGNO);
}

static void
kvx_asm_output_mi_thunk (FILE *file ATTRIBUTE_UNUSED,
			 tree thunk_fndecl ATTRIBUTE_UNUSED,
			 HOST_WIDE_INT delta ATTRIBUTE_UNUSED,
			 HOST_WIDE_INT vcall_offset ATTRIBUTE_UNUSED,
			 tree function ATTRIBUTE_UNUSED)
{
  const char *fnname = IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (thunk_fndecl));
  rtx xops[1];

  assemble_start_function (thunk_fndecl, fnname);
  if (!TARGET_32)
    {
      if (delta)
	/* FIXME AUTO: this is fixed for build, not checked for correctness ! */
	fprintf (file, "\taddd $r0 = $r0, %i\n", (int) delta);

      if (vcall_offset)
	{
	  /* FIXME AUTO: this is fixed for build, not checked for correctness !
	   */
	  fprintf (file, "\tld $r32 = %i[$r0]\n\t;;\n", (int) delta);
	  fprintf (file, "\tld $r32 = %i[$r32]\n\t;;\n", (int) vcall_offset);
	  fprintf (file, "\taddd $r0 = $r0, $r32\n");
	}
    }
  else
    {
      if (delta)
	fprintf (file, "\taddw $r0 = $r0, %i\n", (int) delta);

      if (vcall_offset)
	{
	  fprintf (file, "\tlwz $r32 = %i[$r0]\n\t;;\n", (int) delta);
	  fprintf (file, "\tlwz $r32 = %i[$r32]\n\t;;\n", (int) vcall_offset);
	  fprintf (file, "\taddw $r0 = $r0, $r32\n");
	}
    }
  xops[0] = XEXP (DECL_RTL (function), 0);
  output_asm_insn ("goto\t%0\n\t;;", xops);

  assemble_end_function (thunk_fndecl, fnname);
}

static bool
kvx_asm_can_output_mi_thunk (const_tree thunk_fndecl ATTRIBUTE_UNUSED,
			     HOST_WIDE_INT delta ATTRIBUTE_UNUSED,
			     HOST_WIDE_INT vcall_offset ATTRIBUTE_UNUSED,
			     const_tree function ATTRIBUTE_UNUSED)
{
  return true;
}

static rtx
kvx_expand_builtin_saveregs (void)
{
  int slot = 0;
  struct kvx_frame_info *frame;

  kvx_compute_frame_info ();
  frame = &cfun->machine->frame;
  HOST_WIDE_INT arg_fp_offset
    = frame->arg_pointer_offset - frame->virt_frame_pointer_offset;
  rtx area = gen_rtx_PLUS (Pmode, frame_pointer_rtx, GEN_INT (arg_fp_offset));

  /* All argument register slots used for named args, nothing to push */
  if (crtl->args.info.next_arg_reg >= KV3_ARG_REG_SLOTS)
    return const0_rtx;

  /* use arg_pointer since saved register slots are not known at that time */
  int regno = crtl->args.info.next_arg_reg;

  if (regno & 1)
    {
      rtx insn = emit_move_insn (gen_rtx_MEM (DImode, area),
				 gen_rtx_REG (DImode, KV3_ARGUMENT_POINTER_REGNO
							+ regno));
      RTX_FRAME_RELATED_P (insn) = 1;
      /* Do not attach a NOTE here as the frame has not been laid out yet.
       Let the kvx_fix_debug_for_bundles function during reorg pass handle these
     */

      regno++;
      slot++;
    }

  for (; regno < KV3_ARG_REG_SLOTS; regno += 2, slot += 2)
    {
      rtx addr
	= gen_rtx_MEM (TImode, gen_rtx_PLUS (Pmode, frame_pointer_rtx,
					     GEN_INT (slot * UNITS_PER_WORD
						      + arg_fp_offset)));
      rtx src = gen_rtx_REG (TImode, KV3_ARGUMENT_POINTER_REGNO + regno);

      rtx insn = emit_move_insn (addr, src);
      RTX_FRAME_RELATED_P (insn) = 1;
      /* Do not attach a NOTE here as the frame has not been laid out yet.
       Let the kvx_fix_debug_for_bundles function during reorg pass handle these
     */
    }

  return area;
}

static void
kvx_expand_va_start (tree valist, rtx nextarg ATTRIBUTE_UNUSED)
{
  rtx va_start_addr = expand_builtin_saveregs ();
  rtx va_r = expand_expr (valist, NULL_RTX, VOIDmode, EXPAND_WRITE);

  struct kvx_frame_info *frame;

  frame = &cfun->machine->frame;
  HOST_WIDE_INT arg_fp_offset
    = frame->arg_pointer_offset - frame->virt_frame_pointer_offset;

  gcc_assert (frame->laid_out);

  /* All arg registers must be used by named parameter, va_start
     must point to caller frame for first anonymous parameter ... */
  if (va_start_addr == const0_rtx && crtl->args.info.anonymous_arg_offset_valid)
    {
      /* ... and there are some more arguments. */
      va_start_addr
	= gen_rtx_PLUS (Pmode, frame_pointer_rtx,
			GEN_INT (crtl->args.info.anonymous_arg_offset
				 + arg_fp_offset));
    }
  else
    {
      /* ... and there are no more argument. */
      va_start_addr
	= gen_rtx_PLUS (Pmode, frame_pointer_rtx, GEN_INT (arg_fp_offset));
    }

  emit_move_insn (va_r, va_start_addr);
}

static bool
kvx_cannot_force_const_mem (enum machine_mode mode ATTRIBUTE_UNUSED,
			    rtx x ATTRIBUTE_UNUSED)
{
  return true;
}

static bool
kvx_fixed_point_supported_p (void)
{
  return false;
}

static bool
kvx_scalar_mode_supported_p (scalar_mode mode)
{
  if (mode == HFmode)
    return true;

  int precision = GET_MODE_PRECISION (mode);

  switch (GET_MODE_CLASS (mode))
    {
    case MODE_PARTIAL_INT:
    case MODE_INT:
      if (precision == SHORT_TYPE_SIZE)
	return true;
      break;

    default:
      break;
    }

  return default_scalar_mode_supported_p (mode);
}

static bool
kvx_libgcc_floating_mode_supported_p (scalar_float_mode mode)
{
  return (mode == HFmode
	  ? true
	  : default_libgcc_floating_mode_supported_p (mode));
}

static const char *
kvx_mangle_type (const_tree type)
{
  /* Half-precision float.  */
  if (TREE_CODE (type) == REAL_TYPE && TYPE_PRECISION (type) == 16)
    return "Dh";

  return NULL;
}

static enum flt_eval_method
kvx_excess_precision (enum excess_precision_type type)
{
  switch (type)
    {
    case EXCESS_PRECISION_TYPE_FAST:
    case EXCESS_PRECISION_TYPE_STANDARD:
    case EXCESS_PRECISION_TYPE_IMPLICIT:
      return FLT_EVAL_METHOD_PROMOTE_TO_FLOAT16;
    default:
      gcc_unreachable ();
    }
  return FLT_EVAL_METHOD_UNPREDICTABLE;
}

static bool
kvx_vector_mode_supported_p (enum machine_mode mode)
{
  switch (mode)
    {
    // 64-bit modes
    case E_V8QImode:
    case E_V4HImode:
    case E_V2SImode:
    case E_V4HFmode:
    case E_V2SFmode:
    case E_V1DImode:
    // 128-bit modes
    case E_V16QImode:
    case E_V8HImode:
    case E_V4SImode:
    case E_V2DImode:
    case E_V8HFmode:
    case E_V4SFmode:
    case E_V2DFmode:
    // 256-bit modes
    case E_V32QImode:
    case E_V16HImode:
    case E_V8SImode:
    case E_V4DImode:
    case E_V16HFmode:
    case E_V8SFmode:
    case E_V4DFmode:
      return true;
    default:
      break;
    }
  return kvx_extension_mode_p (mode);
}

static bool
kvx_support_vector_misalignment (enum machine_mode mode ATTRIBUTE_UNUSED,
				 const_tree type ATTRIBUTE_UNUSED,
				 int misalignment ATTRIBUTE_UNUSED,
				 bool is_packed ATTRIBUTE_UNUSED)
{
  return !TARGET_STRICT_ALIGN;
}

static machine_mode
kvx_vectorize_preferred_simd_mode (scalar_mode mode)
{
  switch (mode)
    {
    case E_HImode:
      return V8HImode;
    case E_SImode:
      return V4SImode;
    case E_DImode:
      return V2DImode;
    case E_HFmode:
      return V8HFmode;
    case E_SFmode:
      return V4SFmode;
    case E_DFmode:
      return V2DFmode;
    default:
      break;
    }
  return word_mode;
}

static bool
kvx_pass_by_reference (cumulative_args_t cum ATTRIBUTE_UNUSED, const function_arg_info &arg)
{
  HOST_WIDE_INT size = GET_MODE_SIZE (arg.mode);

  /* GET_MODE_SIZE (BLKmode) is useless since it is 0.  */
  if (arg.mode == BLKmode && arg.type)
    size = int_size_in_bytes (arg.type);

  /* Aggregates are passed by reference based on their size.  */
  if (arg.type && AGGREGATE_TYPE_P (arg.type))
    size = int_size_in_bytes (arg.type);

  /* Coprocessor arguments are passed by reference.  */
  if (kvx_extension_mode_p (arg.mode))
    return true;

  /* Arguments which are variable sized or larger than 4 registers are
     passed by reference */
  return size < 0 || ((size > (4 * UNITS_PER_WORD)) && arg.mode == BLKmode);
  return (size > (4 * UNITS_PER_WORD) || size < 0);
}

static const char *kvx_unspec_tls_asm_op[]
  = {"@tlsgd", "@tlsld", "@tlsle", "@dtpoff", "@tlsie"};

/* Helper function to produce manifest _Float16 constants, where the float
 * value represented as integer comes from REAL_VALUE_TO_TARGET_SINGLE.
 * Adapted from http://openkb.fr/Half-precision_floating-point_in_Java */
static unsigned
kvx_float_to_half_as_int (unsigned fbits)
{
  unsigned sign = fbits >> 16 & 0x8000;		// sign only
  unsigned val = (fbits & 0x7fffffff) + 0x1000; // rounded value
  if (val >= 0x47800000)			// might be or become NaN/Inf
    {						// avoid Inf due to rounding
      if ((fbits & 0x7fffffff) >= 0x47800000)
	{				     // is or must become NaN/Inf
	  if (val < 0x7f800000)		     // was value but too large
	    return sign | 0x7c00;	     // make it +/-Inf
	  return sign | 0x7c00 |	     // remains +/-Inf or NaN
		 (fbits & 0x007fffff) >> 13; // keep NaN (and Inf) bits
	}
      return sign | 0x7bff; // unrounded not quite Inf
    }
  if (val >= 0x38800000)		     // remains normalized value
    return sign | (val - 0x38000000) >> 13;  // exp - 127 + 15
  if (val < 0x33000000)			     // too small for subnormal
    return sign;			     // becomes +/-0
  val = (fbits & 0x7fffffff) >> 23;	     // tmp exp for subnormal calc
  return sign
	 | ((((fbits & 0x7fffff) | 0x800000) // add subnormal bit
	     + (0x800000 >> (val - 102)))    // round depending on cut off
	    >> (126 - val)); // div by 2^(1-(exp-127+15)) and >> 13 | exp=0
}

static bool kvx_print_offset_zero;

void
kvx_print_operand (FILE *file, rtx x, int code)
{
  rtx operand = x;
  bool force_breg = 0;
  bool force_qreg = 0;
  bool force_preg = 0;
  bool force_treg = 0;
  bool force_zreg = 0;
  bool force_yreg = 0;
  bool force_xreg = 0;
  bool addr_mode = false;
  bool as_address = false;
  bool float_compare = false;
  bool reverse_compare = false;
  bool swap_compare = false;
  int addr_space = 0;

  kvx_print_offset_zero = true;
  switch (code)
    {
    case 0:
      /* No code, print as usual.  */
      break;

    case 'b':
      force_breg = true;
      break;

    case 'o':
      force_qreg = true;
      break;

    case 'q':
      force_preg = true;
      break;

    case 't':
      force_treg = true;
      break;

    case 'z':
      force_zreg = true;
      break;

    case 'y':
      force_yreg = true;
      break;

    case 'x':
      force_xreg = true;
      break;

    case 'A':
      as_address = true;
      break;

    case 'F':
      float_compare = true;
      break;

    case 'R':
      reverse_compare = true;
      break;

    case 'S':
      swap_compare = true;
      break;

    case 'O':
      kvx_print_offset_zero = false;
      break;

    case 'T':
      fprintf (file, "@pcrel(");
      output_addr_const (file, operand);
      fprintf (file, ")");
      return;

    case 'V': /* Print '.u' or '.us' or '.s' variant for memory load. */
      addr_space = MEM_ADDR_SPACE (x);
      if (addr_space == KVX_ADDR_SPACE_BYPASS)
	fprintf (file, ".u");
      else if (addr_space == KVX_ADDR_SPACE_PRELOAD)
	fprintf (file, ".us");
      else if (addr_space == KVX_ADDR_SPACE_SPECULATE)
	fprintf (file, ".s");
      addr_mode = true;
      break;

    case 'X':
      addr_mode = true;
      break;

    default:
      output_operand_lossage ("invalid operand code '%c'", code);
    }

  if ((as_address || addr_mode) && GET_CODE (x) != MEM)
    {
      x = gen_rtx_MEM (Pmode, x);
      operand = x;
    }

  if (COMPARISON_P (x))
    {
      enum rtx_code code = GET_CODE (x);
      if (!float_compare)
	{
	  if (reverse_compare)
	    code = reverse_condition (code);
	  else if (swap_compare)
	    code = swap_condition (code);
	  fprintf (file, "%s", GET_RTX_NAME (code));
	}
      else
	{
	  const char *name = 0;
	  switch (code)
	    {
	    case NE:
	      name = "une";
	      break;
	    case EQ:
	      name = "oeq";
	      break;
	    case GE:
	      name = "oge";
	      break;
	    case LT:
	      name = "olt";
	      break;
	    case UNEQ:
	      name = "ueq";
	      break;
	    case UNGE:
	      name = "uge";
	      break;
	    case UNLT:
	      name = "ult";
	      break;
	    case LTGT:
	      name = "one";
	      break;
	    default:
	      gcc_unreachable ();
	    }
	  fprintf (file, "%s", name);
	}
      return;
    }

  switch (GET_CODE (operand))
    {
    case REG:
      if (REGNO (operand) >= FIRST_PSEUDO_REGISTER)
	error ("incorrect hard register number %d", REGNO (operand));
      else if (extension_register_operand (operand, VOIDmode))
	{
	  if (force_xreg)
	    fprintf (file, "$%s", kvx_xvr_reg_name (REGNO (operand)));
	  else if (force_breg)
	    {
	      int nwords = GET_MODE_SIZE (GET_MODE (x)) / UNITS_PER_WORD;
	      fprintf (file, "$%s..%s", kvx_xvr_reg_name (REGNO (operand)),
		       kvx_xvr_reg_name (REGNO (operand) + nwords - 4));
	    }
	  else if (GET_MODE_SIZE (GET_MODE (x)) == UNITS_PER_WORD * 16)
	    fprintf (file, "$%s", kvx_xmr_reg_name (REGNO (operand)));
	  else if (GET_MODE_SIZE (GET_MODE (x)) == UNITS_PER_WORD * 8)
	    fprintf (file, "$%s", kvx_xwr_reg_name (REGNO (operand)));
	  else
	    fprintf (file, "$%s", kvx_xvr_reg_name (REGNO (operand)));
	}
      else if (force_qreg)
	fprintf (file, "$%s", kvx_qgr_reg_name (REGNO (operand)));
      else if (force_preg)
	fprintf (file, "$%s", kvx_pgr_reg_name (REGNO (operand)));
      else if (force_treg)
	{
	  if (GET_MODE_SIZE (GET_MODE (x)) < UNITS_PER_WORD * 4)
	    error ("using %%t format with operand smaller than 4 registers");
	  fprintf (file, "$%s", reg_names[REGNO (operand) + 3]);
	}
      else if (force_zreg)
	{
	  if (GET_MODE_SIZE (GET_MODE (x)) < UNITS_PER_WORD * 4)
	    error ("using %%z format with operand smaller than 4 registers");
	  fprintf (file, "$%s", reg_names[REGNO (operand) + 2]);
	}
      else if (force_yreg)
	{
	  if (GET_MODE_SIZE (GET_MODE (x)) < UNITS_PER_WORD * 2)
	    error ("using %%y format with operand smaller than 2 registers");
	  fprintf (file, "$%s", reg_names[REGNO (operand) + 1]);
	}
      else if (force_xreg)
	{
	  if (GET_MODE_SIZE (GET_MODE (x)) < UNITS_PER_WORD * 2)
	    error ("using %%x format with operand smaller than 2 registers");
	  fprintf (file, "$%s", reg_names[REGNO (operand)]);
	}
      else if (system_register_operand (operand, VOIDmode))
	fprintf (file, "$%s", reg_names[REGNO (operand)]);
      else if (general_register_operand (operand, VOIDmode))
	{
	  if (GET_MODE_SIZE (GET_MODE (x)) == UNITS_PER_WORD * 4)
	    fprintf (file, "$%s", kvx_qgr_reg_name (REGNO (operand)));
	  else if (GET_MODE_SIZE (GET_MODE (x)) == UNITS_PER_WORD * 2)
	    fprintf (file, "$%s", kvx_pgr_reg_name (REGNO (operand)));
	  else
	    fprintf (file, "$%s", reg_names[REGNO (operand)]);
	}
      else
	gcc_unreachable ();
      return;

    case MEM:
      if (addr_mode)
	{
	  x = XEXP (x, 0);
	  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 0)) == MULT
	      && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT
	      && INTVAL (XEXP (XEXP (x, 0), 1)) > HOST_WIDE_INT_1)
	    fprintf (file, ".xs");
	}
      else
	{
	  x = XEXP (x, 0);
	  if (GET_CODE (x) == UNSPEC)
	    kvx_print_operand (file, x, 0);
	  else
	    output_address (GET_MODE (XEXP (operand, 0)), XEXP (operand, 0));
	}
      return;

    case CONST_DOUBLE: {
	long l[2];
	REAL_VALUE_TYPE r = *CONST_DOUBLE_REAL_VALUE (operand);
	if (GET_MODE (x) == HFmode)
	  {
	    REAL_VALUE_TO_TARGET_SINGLE (r, l[0]);
	    l[0] = kvx_float_to_half_as_int (l[0]);
	    fprintf (file, "0x%04x", (unsigned int) l[0]);
	    return;
	  }
	else if (GET_MODE (x) == SFmode)
	  {
	    REAL_VALUE_TO_TARGET_SINGLE (r, l[0]);
	    fprintf (file, "0x%08x", (unsigned int) l[0]);
	    return;
	  }
	else if (GET_MODE (x) == DFmode)
	  {
	    REAL_VALUE_TO_TARGET_DOUBLE (r, l);
	    fprintf (file, "0x%08x%08x", (unsigned int) l[1],
		     (unsigned int) l[0]);
	    return;
	  }
      }
      gcc_unreachable ();
      return;

    case CONST_INT:
      {
	HOST_WIDE_INT value = INTVAL (x);
	int signed10 = value >= -512 && value < 512;
	const char *format = signed10? HOST_WIDE_INT_PRINT_DEC:
				       "0x" HOST_WIDE_INT_PRINT_PADDED_HEX;
	fprintf (file, format, INTVAL (x));
      }
      return;

      case CONST_VECTOR: {
	int slice = 1 * force_yreg + 2 * force_zreg + 3 * force_treg;
	fprintf (file, "0x" HOST_WIDE_INT_PRINT_PADDED_HEX,
		 kvx_const_vector_value (x, slice));
      }
      return;

    case CONST_STRING:
      /* Case for modifier strings */
      fputs (XSTR (operand, 0), file);
      return;

      default: {
	int is_unspec = 0, unspec;

	if (GET_CODE (operand) == CONST)
	  operand = XEXP (operand, 0);

	if (GET_CODE (operand) == UNSPEC)
	  {
	    is_unspec = 1;
	    unspec = XINT (operand, 1);
	    operand = XVECEXP (operand, 0, 0);
	  }

	if (is_unspec)
	  {
	    switch (unspec)
	      {
	      case UNSPEC_TLS_GD:
	      case UNSPEC_TLS_LD:
	      case UNSPEC_TLS_DTPOFF:
	      case UNSPEC_TLS_LE:
	      case UNSPEC_TLS_IE:
		fputs (kvx_unspec_tls_asm_op[unspec - UNSPEC_TLS_GD], (file));
		fputs ("(", (file));
		break;
	      case UNSPEC_GOT:
		fprintf (file, "@got(");
		break;
	      case UNSPEC_GOTOFF:
		fprintf (file, "@gotoff(");
		break;
	      case UNSPEC_PCREL:
		fprintf (file, "@pcrel(");
		break;
	      default:
		gcc_unreachable ();
	      }
	  }

	/* No need to handle all strange variants, let output_addr_const
	   do it for us.  */
	output_addr_const (file, operand);
	if (is_unspec)
	  fprintf (file, ")");
      }
    }
}

static const char *
kvx_regname (rtx x)
{
  machine_mode mode = GET_MODE (x);

  switch (GET_CODE (x))
    {
    case REG:
      if (GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
	return reg_names[REGNO (x)];
      else if (GET_MODE_SIZE (mode) <= 2 * UNITS_PER_WORD)
	return kvx_pgr_reg_name (REGNO (x));
      else if (GET_MODE_SIZE (mode) <= 4 * UNITS_PER_WORD)
	return kvx_qgr_reg_name (REGNO (x));
      gcc_unreachable ();
    case SUBREG:
      // Addressing mode with register offset
      gcc_assert (TARGET_32);
      gcc_assert (SUBREG_BYTE (x) == 0);
      return kvx_regname (SUBREG_REG (x));
    default:
      gcc_unreachable ();
    }
}

void
kvx_print_operand_address (FILE *file, machine_mode /*mode*/, rtx x)
{
  switch (GET_CODE (x))
    {
    case REG:
    case SUBREG:
      if (kvx_print_offset_zero)
	fprintf (file, "0[$%s]", kvx_regname (x));
      else
	fprintf (file, "[$%s]", kvx_regname (x));
      break;

    case PLUS:
      switch (GET_CODE (XEXP (x, 1)))
	{
	case CONST_INT:
	  fprintf (file, HOST_WIDE_INT_PRINT_DEC, INTVAL (XEXP (x, 1)));
	  break;
	case LABEL_REF:
	case SYMBOL_REF:
	case CONST:
	case UNSPEC:
	  output_addr_const (file, XEXP (x, 1));
	  break;
	case REG:
	case SUBREG:
	  if (GET_CODE (XEXP (x, 0)) == MULT)
	    fprintf (file, "$%s", kvx_regname (XEXP (XEXP (x, 0), 0)));
	  else
	    fprintf (file, "$%s", kvx_regname (XEXP (x, 1)));
	  break;
	default:
	  abort ();
	}
      if (GET_CODE (XEXP (x, 0)) == MULT)
	fprintf (file, "[$%s]", kvx_regname (XEXP (x, 1)));
      else
	fprintf (file, "[$%s]", kvx_regname (XEXP (x, 0)));
      return;

    default:
      output_addr_const (file, x);
      break;
    }
}

bool
kvx_print_punct_valid_p (unsigned char code)
{
  return code == ';';
}

/* Return true for the .xs addressing modes, else false. */
static bool
kvx_mode_dependent_address_p (const_rtx addr,
			      addr_space_t space ATTRIBUTE_UNUSED)
{
  const_rtx x = addr;

  // Same logic as .xs addressing mode in kvx_print_operand
  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 0)) == MULT
      && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT
      && INTVAL (XEXP (XEXP (x, 0), 1)) > HOST_WIDE_INT_1)
    return true;

  return false;
}

/* Functions to save and restore machine-specific function data.  */
static struct machine_function *
kvx_init_machine_status (void)
{
  struct machine_function *machine;
  machine = ggc_cleared_alloc<machine_function> ();

  return machine;
}

/* Do anything needed before RTL is emitted for each function.  */
void
kvx_init_expanders (void)
{
  /* Arrange to initialize and mark the machine per-function status.  */
  init_machine_status = kvx_init_machine_status;

  if (TARGET_DIVMOD0_TRAP)
    kvx_divmod_zero = const0_rtx;
  else
    kvx_divmod_zero = const1_rtx;
}

/* Handle an attribute requiring a FUNCTION_DECL;
   arguments as in struct attribute_spec.handler.  */
static tree
kvx_handle_fndecl_attribute (tree *node, tree name, tree args ATTRIBUTE_UNUSED,
			     int flags ATTRIBUTE_UNUSED, bool *no_add_attrs)
{
  if (TREE_CODE (*node) != FUNCTION_DECL)
    {
      warning (OPT_Wattributes, "%qE attribute only applies to functions",
	       name);
      *no_add_attrs = true;
    }

  return NULL_TREE;
}

void
kvx_expand_tablejump (rtx op0, rtx op1)
{
  if (flag_pic)
    {
      rtx tmp_reg = gen_reg_rtx (Pmode);
      emit_move_insn (tmp_reg, gen_rtx_LABEL_REF (VOIDmode, op1));

      /* Relative addrs are stored as 32bits value. */
      if (GET_MODE (op0) != Pmode)
	{
	  rtx dreg = gen_reg_rtx (Pmode);
	  emit_insn (gen_extend_insn (dreg, op0, Pmode, GET_MODE (op0), 0));
	  op0 = dreg;
	}
      emit_insn (gen_add2_insn (op0, tmp_reg));
    }

  if (!TARGET_32)
    {
      emit_jump_insn (gen_tablejump_real_di (op0, op1));
    }
  else
    {
      emit_jump_insn (gen_tablejump_real_si (op0, op1));
    }
}

/* Return TRUE if REGNO should be saved in the prologue of current function */
static bool
should_be_saved_in_prologue (int regno)
{
  return (df_regs_ever_live_p (regno)	   // reg is used
	  && !call_used_regs[regno] // reg is callee-saved
	  && REGNO_REG_CLASS (regno) != XCR_REGS
	  && (regno == KV3_RETURN_POINTER_REGNO || !fixed_regs[regno]));
}

static bool
kvx_register_saved_on_entry (int regno)
{
  return cfun->machine->frame.reg_rel_offset[regno] >= 0;
}

/* Returns a REG rtx with the nth hard reg that is safe to use in prologue
   (caller-saved and non fixed reg). Returns NULL_RTX and emits an
   error if no such register can be found. */
static rtx
kvx_get_callersaved_nonfixed_reg (machine_mode ARG_UNUSED (mode), int n)
{
  // start at R16 as as everything before that may be used.
  // We should be able to use the veneer regs if not fixed.
  for (int i = 0, regno = 16; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      bool candidate = call_used_regs[regno] && !fixed_regs[regno]
		       && REGNO_REG_CLASS (regno) != XCR_REGS;
#ifdef GCC_KVX_MPPA_LINUX
      candidate &= !((regno == PROFILE_REGNO) && (crtl->profile));
#endif

      if (!candidate)
	continue;

      if (i == n)
	return gen_rtx_REG (mode, regno);
      else
	i++;
    }

  error ("No scratch register available in function prologue.");
  return NULL_RTX;
}

static void
kvx_emit_single_spill (rtx mem, rtx reg, bool is_load)
{
  rtx set = gen_rtx_SET (is_load ? reg : mem, is_load ? mem : reg);
  rtx insn = emit_insn (set);
  if (!is_load)
    {
      RTX_FRAME_RELATED_P (insn) = 1;
      add_reg_note (insn, REG_CFA_OFFSET, copy_rtx (set));
    }
}

static void
kvx_emit_multiple_spill (rtx mem, rtx reg, int nr, bool is_load)
{
  gcc_assert (nr == 2 || nr == 4);

  rtx operands[4] = {is_load ? reg : mem, is_load ? mem : reg, GEN_INT (nr)};

  if (is_load)
    kvx_expand_load_multiple (operands);
  else
    kvx_expand_store_multiple (operands);
  rtx insn = emit_insn (operands[3]);

  if (!is_load)
    {
      RTX_FRAME_RELATED_P (insn) = 1;
      gcc_assert ((unsigned)XVECLEN (PATTERN (insn), 0) == nr);

      for (int i = 0; i < nr; i++)
	{
	  add_reg_note (insn, REG_CFA_OFFSET,
			copy_rtx (XVECEXP (PATTERN (insn), 0, i)));
	}
    }
}

/* Save/Restore register at offsets previously computed in frame information
 * layout. The routine uses $sp as base register, so it must be set to its
 * initial value in case of dynamic stack allocation.
 */
static void
kvx_save_or_restore_callee_save_registers (bool restore)
{
  struct kvx_frame_info *frame = &cfun->machine->frame;
  rtx insn;
  rtx (*gen_mem_ref) (enum machine_mode, rtx) = gen_rtx_MEM;

  unsigned int regno;

  unsigned int pack_prev_regs[4];
  unsigned int pack_prev_regs_idx = 0;

  enum
  {
    NO_PACK_YET = 0,
    DOUBLE_REG,
    QUAD_REG
  } pack_type
    = NO_PACK_YET;

  hard_reg_set_iterator rsi;

  EXECUTE_IF_SET_IN_HARD_REG_SET (frame->saved_regs, 0, regno, rsi)
  {
    if (kvx_register_saved_on_entry (regno))
      {
	rtx mem
	  = gen_mem_ref (DImode,
			 plus_constant (Pmode, stack_pointer_rtx,
					frame->saved_reg_sp_offset
					  + frame->reg_rel_offset[regno]));

	rtx saved_reg = gen_rtx_REG (DImode, regno);
	rtx orig_save_reg = saved_reg;

	if (regno == KV3_RETURN_POINTER_REGNO)
	  {
	    saved_reg = kvx_get_callersaved_nonfixed_reg (DImode, 0);
	    gcc_assert (saved_reg != NULL_RTX);

	    if (restore == false)
	      {
		rtx src_reg = gen_rtx_REG (DImode, regno);
		insn = emit_move_insn (saved_reg, src_reg);
		RTX_FRAME_RELATED_P (insn) = 1;

		add_reg_note (insn, REG_CFA_REGISTER,
			      gen_rtx_SET (saved_reg, src_reg));
	      }
	  }

	if (regno == KV3_RETURN_POINTER_REGNO)
	  {
	    if (restore)
	      {
		insn = emit_move_insn (saved_reg, mem);
		insn = emit_move_insn (gen_rtx_REG (DImode, regno), saved_reg);
	      }
	    else
	      {
		insn = emit_move_insn (mem, saved_reg);
		RTX_FRAME_RELATED_P (insn) = 1;
		add_reg_note (insn, REG_CFA_OFFSET,
			      gen_rtx_SET (mem, orig_save_reg));
	      }
	  }
	else
	  {
	  process_current_reg:
	    switch (pack_type)
	      {
	      case NO_PACK_YET:
		if (regno & 1)
		  {
		    kvx_emit_single_spill (mem, saved_reg, restore);
		  }
		else
		  {
		    pack_type = (regno % 4 == 0) ? QUAD_REG : DOUBLE_REG;
		    pack_prev_regs[0] = regno;
		    pack_prev_regs_idx = 1;
		  }
		break;

	      case DOUBLE_REG:
		if (pack_prev_regs[0] == (regno - 1)
		    && frame->reg_rel_offset[pack_prev_regs[0]]
			 == (frame->reg_rel_offset[regno] - UNITS_PER_WORD))
		  {
		    kvx_emit_multiple_spill (
		      gen_mem_ref (
			DImode,
			plus_constant (
			  Pmode, stack_pointer_rtx,
			  frame->saved_reg_sp_offset
			    + frame->reg_rel_offset[pack_prev_regs[0]])),
		      gen_rtx_REG (DImode, pack_prev_regs[0]), 2, restore);
		    pack_type = NO_PACK_YET;
		    pack_prev_regs_idx = 0;
		  }
		else
		  {
		    /* Emit previous candidate */
		    kvx_emit_single_spill (
		      gen_mem_ref (
			DImode,
			plus_constant (
			  Pmode, stack_pointer_rtx,
			  frame->saved_reg_sp_offset
			    + frame->reg_rel_offset[pack_prev_regs[0]])),
		      gen_rtx_REG (DImode, pack_prev_regs[0]), restore);

		    pack_type = NO_PACK_YET;
		    goto process_current_reg;
		  }
		break;

	      case QUAD_REG:
		if (pack_prev_regs[pack_prev_regs_idx - 1] == (regno - 1)
		    && frame->reg_rel_offset[pack_prev_regs[pack_prev_regs_idx
							    - 1]]
			 == (frame->reg_rel_offset[regno] - UNITS_PER_WORD))
		  {
		    if (pack_prev_regs_idx == 3)
		      {
			/* Emit a quad register load/store */
			kvx_emit_multiple_spill (
			  gen_mem_ref (
			    DImode,
			    plus_constant (
			      Pmode, stack_pointer_rtx,
			      frame->saved_reg_sp_offset
				+ frame->reg_rel_offset[pack_prev_regs[0]])),
			  gen_rtx_REG (DImode, pack_prev_regs[0]), 4, restore);
			pack_type = NO_PACK_YET;
			pack_prev_regs_idx = 0;
		      }
		    else
		      {
			pack_prev_regs[pack_prev_regs_idx++] = regno;
		      }
		  }
		else if (pack_prev_regs_idx == 3)
		  {
		    /* Emit a double followed by a single register load/store */
		    kvx_emit_multiple_spill (
		      gen_mem_ref (
			DImode,
			plus_constant (
			  Pmode, stack_pointer_rtx,
			  frame->saved_reg_sp_offset
			    + frame->reg_rel_offset[pack_prev_regs[0]])),
		      gen_rtx_REG (DImode, pack_prev_regs[0]), 2, restore);
		    kvx_emit_single_spill (
		      gen_mem_ref (
			DImode,
			plus_constant (
			  Pmode, stack_pointer_rtx,
			  frame->saved_reg_sp_offset
			    + frame->reg_rel_offset[pack_prev_regs[2]])),
		      gen_rtx_REG (DImode, pack_prev_regs[2]), restore);

		    pack_type = NO_PACK_YET;
		    pack_prev_regs_idx = 0;
		    goto process_current_reg;
		  }
		else if (pack_prev_regs_idx == 2)
		  {
		    /* Emit a double register load/store and try to pack the
		       next one */
		    kvx_emit_multiple_spill (
		      gen_mem_ref (
			DImode,
			plus_constant (
			  Pmode, stack_pointer_rtx,
			  frame->saved_reg_sp_offset
			    + frame->reg_rel_offset[pack_prev_regs[0]])),
		      gen_rtx_REG (DImode, pack_prev_regs[0]), 2, restore);

		    pack_type = NO_PACK_YET;
		    pack_prev_regs_idx = 0;
		    goto process_current_reg;
		  }
		else
		  {
		    /* Emit a single single register load/store and try to pack
		       the next one */
		    kvx_emit_single_spill (
		      gen_mem_ref (
			DImode,
			plus_constant (
			  Pmode, stack_pointer_rtx,
			  frame->saved_reg_sp_offset
			    + frame->reg_rel_offset[pack_prev_regs[0]])),
		      gen_rtx_REG (DImode, pack_prev_regs[0]), restore);

		    pack_type = NO_PACK_YET;
		    pack_prev_regs_idx = 0;
		    goto process_current_reg;
		  }
	      }
	  }
      }
  }

  /* Purge remaining register load/store that could not be packed */
  if (pack_type == DOUBLE_REG)
    {
      kvx_emit_single_spill (
	gen_mem_ref (
	  DImode, plus_constant (Pmode, stack_pointer_rtx,
				 frame->saved_reg_sp_offset
				   + frame->reg_rel_offset[pack_prev_regs[0]])),
	gen_rtx_REG (DImode, pack_prev_regs[0]), restore);
    }
  else if (pack_type == QUAD_REG)
    {
      if (pack_prev_regs_idx == 1)
	{
	  kvx_emit_single_spill (
	    gen_mem_ref (
	      DImode,
	      plus_constant (Pmode, stack_pointer_rtx,
			     frame->saved_reg_sp_offset
			       + frame->reg_rel_offset[pack_prev_regs[0]])),
	    gen_rtx_REG (DImode, pack_prev_regs[0]), restore);
	}
      else if (pack_prev_regs_idx == 2)
	{
	  kvx_emit_multiple_spill (
	    gen_mem_ref (
	      DImode,
	      plus_constant (Pmode, stack_pointer_rtx,
			     frame->saved_reg_sp_offset
			       + frame->reg_rel_offset[pack_prev_regs[0]])),
	    gen_rtx_REG (DImode, pack_prev_regs[0]), 2, restore);
	}
      else
	{
	  kvx_emit_multiple_spill (
	    gen_mem_ref (
	      DImode,
	      plus_constant (Pmode, stack_pointer_rtx,
			     frame->saved_reg_sp_offset
			       + frame->reg_rel_offset[pack_prev_regs[0]])),
	    gen_rtx_REG (DImode, pack_prev_regs[0]), 2, restore);

	  kvx_emit_single_spill (
	    gen_mem_ref (
	      DImode,
	      plus_constant (Pmode, stack_pointer_rtx,
			     frame->saved_reg_sp_offset
			       + frame->reg_rel_offset[pack_prev_regs[2]])),
	    gen_rtx_REG (DImode, pack_prev_regs[2]), restore);
	}
    }
}

/* Implements INITIAL_ELIMINATION_OFFSET.  FROM is either the frame pointer
   or argument pointer.  TO is either the stack pointer or frame
   pointer.  */

poly_int64
kvx_initial_elimination_offset (int from, int to)
{
  kvx_compute_frame_info ();
  struct kvx_frame_info *frame = &cfun->machine->frame;

  /* Should never have anything else FRAME_POINTER_REGNUM -> HFP/SP */
  if (from != FRAME_POINTER_REGNUM
      || (to != STACK_POINTER_REGNUM && to != HARD_FRAME_POINTER_REGNUM))
    gcc_unreachable ();

  if (from == FRAME_POINTER_REGNUM && to == STACK_POINTER_REGNUM)
    return frame->virt_frame_pointer_offset;
  else if (from == FRAME_POINTER_REGNUM && to == HARD_FRAME_POINTER_REGNUM)
    return (frame->virt_frame_pointer_offset
	    - frame->hard_frame_pointer_offset);

  gcc_unreachable ();
}

/* Return TRUE if target supports -fstack-limit-register */

bool
kvx_have_stack_checking (void)
{
#ifdef GCC_KVX_MPPA_COS
  return true;
#else
  return false;
#endif
}

void
kvx_expand_prologue (void)
{
  kvx_compute_frame_info ();
  struct kvx_frame_info *frame = &cfun->machine->frame;
  HOST_WIDE_INT size = frame->frame_size.to_constant ();
  rtx insn;

  if (flag_stack_usage_info)
    current_function_static_stack_size = size;

  kvx_debug_frame_info (frame);

  if (size > 0)
    {

      if (crtl->limit_stack)
	{
	  if (kvx_have_stack_checking ())
	    {
	      rtx new_stack_pointer_rtx
		= kvx_get_callersaved_nonfixed_reg (Pmode, 0);
	      rtx stack_limit_reg = kvx_get_callersaved_nonfixed_reg (Pmode, 1);

	      emit_move_insn (stack_limit_reg, stack_limit_rtx);
	      emit_insn (gen_add3_insn (new_stack_pointer_rtx,
					stack_pointer_rtx, GEN_INT (-size)));
	      emit_insn (gen_sub3_insn (new_stack_pointer_rtx, stack_limit_reg,
					new_stack_pointer_rtx));
	      emit_insn (
		gen_ctrapsi4 (gen_rtx_GT (VOIDmode, new_stack_pointer_rtx,
					  const0_rtx),
			      new_stack_pointer_rtx, const0_rtx, GEN_INT (0)));
	    }
	  else
	    {
	      error ("-fstack-limit-* is not supported.");
	    }
	}

      insn = emit_insn (gen_add2_insn (stack_pointer_rtx, GEN_INT (-size)));
      RTX_FRAME_RELATED_P (insn) = 1;

      add_reg_note (insn, REG_CFA_ADJUST_CFA, copy_rtx (PATTERN (insn)));

      /* We need a static chain (ie, we are generating the prologue of
       * a nested function.), we move the value of $r31 into the
       * STATIC_CHAIN stac frame slot (at $sp + virt_frame_ptr_offset) */
      if (cfun->machine->static_chain_needed)
	{
	  insn = emit_move_insn (
	    gen_frame_mem (Pmode,
			   plus_constant (Pmode, stack_pointer_rtx,
					  frame->virt_frame_pointer_offset)),
	    gen_rtx_REG (Pmode, 31));
	}
    }

  /* Save registers */
  kvx_save_or_restore_callee_save_registers (0);

  if (frame_pointer_needed)
    {
      gcc_assert (frame->reg_rel_offset[HARD_FRAME_POINTER_REGNUM] == 0);
      insn = emit_insn (
	gen_add3_insn (hard_frame_pointer_rtx, stack_pointer_rtx,
		       GEN_INT (frame->hard_frame_pointer_offset)));

      RTX_FRAME_RELATED_P (insn) = 1;
      add_reg_note (
	insn, REG_CFA_DEF_CFA,
	gen_rtx_PLUS (
	  Pmode, hard_frame_pointer_rtx,
				  GEN_INT (
				    size - frame->hard_frame_pointer_offset)));
    }
}

void
kvx_expand_epilogue (void)
{
  struct kvx_frame_info *frame = &cfun->machine->frame;
  HOST_WIDE_INT frame_size = frame->frame_size;
  rtx insn;

  if (frame_pointer_needed)
    {
      /* Restore $sp from $fp */
      insn = emit_insn (
	gen_add3_insn (stack_pointer_rtx, hard_frame_pointer_rtx,
		       GEN_INT (-frame->hard_frame_pointer_offset)));

      /* Revert CFA reg to use SP with its initial offset */
      RTX_FRAME_RELATED_P (insn) = 1;
      add_reg_note (insn, REG_CFA_DEF_CFA,
		    gen_rtx_PLUS (DImode, stack_pointer_rtx,
				  GEN_INT (frame->frame_size)));
    }

  /* $sp is now correct and can be used by save_or_restore */
  kvx_save_or_restore_callee_save_registers (1);

  if (frame_size != 0)
    {
      insn = GEN_INT (frame_size);
      insn = emit_insn (gen_add2_insn (stack_pointer_rtx, insn));
      RTX_FRAME_RELATED_P (insn) = 1;
      add_reg_note (insn, REG_CFA_ADJUST_CFA, copy_rtx (PATTERN (insn)));
    }
}

/* Return the TLS model to use for ADDR.  */

static enum tls_model
tls_symbolic_operand_type (rtx addr)
{
  enum tls_model tls_kind = TLS_MODEL_NONE;

  if (GET_CODE (addr) == CONST)
    {
      if (GET_CODE (XEXP (addr, 0)) == PLUS
	  && GET_CODE (XEXP (XEXP (addr, 0), 0)) == SYMBOL_REF)
	tls_kind = SYMBOL_REF_TLS_MODEL (XEXP (XEXP (addr, 0), 0));
    }
  else if (GET_CODE (addr) == SYMBOL_REF)
    tls_kind = SYMBOL_REF_TLS_MODEL (addr);

    /* General TLS model only supported on Linux. ClusterOS and bare
       only supports local exec. */

#ifndef GCC_KVX_MPPA_LINUX
  if (tls_kind != TLS_MODEL_NONE)
    tls_kind = TLS_MODEL_LOCAL_EXEC;
#endif
  return tls_kind;
}

/* Return true if SYMBOL_REF X is thread local */
static bool
kvx_tls_symbol_p (rtx x)
{
  if (!TARGET_HAVE_TLS)
    return false;

  if (GET_CODE (x) != SYMBOL_REF)
    return false;

  return SYMBOL_REF_TLS_MODEL (x) != 0;
}

static rtx
gen_set_gotp_insn (rtx target)
{
  if (TARGET_32)
    return gen_set_gotp_si (target);
  else
    return gen_set_gotp_di (target);
}

static enum kvx_symbol_type
kvx_classify_tls_symbol (rtx x)
{
  enum tls_model tls_kind = tls_symbolic_operand_type (x);
  switch (tls_kind)
    {
    case TLS_MODEL_LOCAL_EXEC:
      return SYMBOL_TLSLE;

      /* General TLS model only supported on Linux. ClusterOS and bare
	 only supports local exec. */
#ifdef GCC_KVX_MPPA_LINUX
    case TLS_MODEL_LOCAL_DYNAMIC:
      return SYMBOL_TLSLD;

    case TLS_MODEL_GLOBAL_DYNAMIC:
      return SYMBOL_TLSGD;

    case TLS_MODEL_INITIAL_EXEC:
      return SYMBOL_TLSIE;
#else
    case TLS_MODEL_LOCAL_DYNAMIC:
    case TLS_MODEL_GLOBAL_DYNAMIC:
    case TLS_MODEL_INITIAL_EXEC:
#endif
    case TLS_MODEL_EMULATED:
    case TLS_MODEL_NONE:
      error ("TLS model not supported.");
      break;

    default:
      gcc_unreachable ();
    }
  return SYMBOL_UNKNOWN;
}

static enum kvx_symbol_type
kvx_classify_symbol (rtx x)
{
  if (kvx_tls_symbol_p (x))
    return kvx_classify_tls_symbol (x);

  /* We keep both way of materializing the absolute address of a label
     because the use of pcrel insn has greater constraints on bundling
     (ALU_FULL) versus a simple make (ALU_TINY) */
  if (GET_CODE (x) == LABEL_REF)
    return flag_pic ? LABEL_PCREL_ABSOLUTE : LABEL_ABSOLUTE;

  if (GET_CODE (x) == SYMBOL_REF)
    {
      if (!flag_pic)
	return SYMBOL_ABSOLUTE;

      if (SYMBOL_REF_LOCAL_P (x) && !SYMBOL_REF_EXTERNAL_P (x))
	return SYMBOL_GOTOFF;
      else
	return SYMBOL_GOT;
    }
  return SYMBOL_ABSOLUTE;
}

static GTY (()) rtx kvx_tls_symbol;

/* Return an instruction sequence that calls __tls_get_addr.  SYM is
   the TLS symbol we are referencing and TYPE is the symbol type to use
   (either global dynamic or local dynamic).  RESULT is an RTX for the
   return value location.  */

static rtx_insn *
kvx_call_tls_get_addr (rtx sym, rtx result, int unspec)
{
  rtx a0 = gen_rtx_REG (Pmode, KV3_ARGUMENT_POINTER_REGNO), func;
  rtx_insn *insn;

  if (!kvx_tls_symbol)
    kvx_tls_symbol = init_one_libfunc ("__tls_get_addr");
  func = gen_rtx_MEM (FUNCTION_MODE, kvx_tls_symbol);

  start_sequence ();

  /* sequence for getting TLS desc address from GOT slot:
   */
  rtx pic_reg = gen_reg_rtx (Pmode);
  emit_insn (gen_set_gotp_insn (pic_reg));

  rtx new_rtx
    = gen_rtx_CONST (Pmode, gen_rtx_UNSPEC (Pmode, gen_rtvec (1, sym), unspec));
  emit_insn (gen_add3_insn (a0, pic_reg, new_rtx));

  insn = emit_call_insn (gen_call_value (result, func, const0_rtx));
  RTL_CONST_CALL_P (insn) = 1;
  use_reg (&CALL_INSN_FUNCTION_USAGE (insn), a0);
  insn = get_insns ();

  end_sequence ();

  return insn;
}

static rtx
kvx_legitimize_tls_reference (rtx x)
{
  rtx addr, pic_reg, eqv;
  rtx dest, tmp;

  if (reload_completed || reload_in_progress)
    return x;

  enum kvx_symbol_type sty = kvx_classify_symbol (x);

  switch (sty)
    {
    case SYMBOL_TLSLE:
      /* address is @tlsle(symbol)[$tp]
       */
      addr
	= gen_rtx_PLUS (Pmode, gen_rtx_REG (Pmode, KV3_LOCAL_POINTER_REGNO),
			gen_rtx_CONST (Pmode,
				       gen_rtx_UNSPEC (Pmode, gen_rtvec (1, x),
						       UNSPEC_TLS_LE)));
      return addr;

    case SYMBOL_TLSGD:
      /* Sequence is:
       * $r0 = @tlsgd(sym) + $got
       * addr = __tls_get_addr()
       */
      tmp = gen_rtx_REG (Pmode, KV3_ARGUMENT_POINTER_REGNO);
      dest = gen_reg_rtx (Pmode);
      emit_libcall_block (kvx_call_tls_get_addr (x, tmp, UNSPEC_TLS_GD),
			  dest /* target */, tmp /* result */, x /* equiv */);
      return dest;

    case SYMBOL_TLSIE:
      pic_reg = gen_reg_rtx (Pmode);
      dest = gen_reg_rtx (Pmode);

      emit_insn (gen_set_gotp_insn (pic_reg));

      addr
	= gen_rtx_PLUS (Pmode, pic_reg,
			gen_rtx_CONST (Pmode,
				       gen_rtx_UNSPEC (Pmode, gen_rtvec (1, x),
						       UNSPEC_TLS_IE)));
      emit_move_insn (dest, gen_rtx_MEM (Pmode, addr));

      return gen_rtx_PLUS (Pmode, gen_rtx_REG (Pmode, KV3_LOCAL_POINTER_REGNO),
			   dest);

    case SYMBOL_TLSLD:
      tmp = gen_rtx_REG (Pmode, KV3_ARGUMENT_POINTER_REGNO);
      dest = gen_reg_rtx (Pmode);

      /* Attach a unique REG_EQUIV, to allow the RTL optimizers to
	 share the LD result with other LD model accesses.  */
      eqv
	= gen_rtx_UNSPEC (Pmode, gen_rtvec (1, const0_rtx), UNSPEC_TLS_LD_CALL);

      emit_libcall_block (kvx_call_tls_get_addr (x, tmp, UNSPEC_TLS_LD),
			  dest /* target */, tmp /* result */, eqv /* equiv */);

      addr
	= gen_rtx_PLUS (Pmode, dest,
			gen_rtx_CONST (Pmode,
				       gen_rtx_UNSPEC (Pmode, gen_rtvec (1, x),
						       UNSPEC_TLS_DTPOFF)));

      return addr;

    case SYMBOL_UNKNOWN:
    default:
      gcc_unreachable ();
    }
}

static bool
function_symbol_referenced_p (rtx x)
{
  const char *format;
  int length;
  int j;

  if (GET_CODE (x) == SYMBOL_REF)
    return SYMBOL_REF_FUNCTION_P (x);

  length = GET_RTX_LENGTH (GET_CODE (x));
  format = GET_RTX_FORMAT (GET_CODE (x));

  for (j = 0; j < length; ++j)
    {
      switch (format[j])
	{
	case 'e':
	  if (function_symbol_referenced_p (XEXP (x, j)))
	    return TRUE;
	  break;

	case 'V':
	case 'E':
	  if (XVEC (x, j) != 0)
	    {
	      int k;
	      for (k = 0; k < XVECLEN (x, j); ++k)
		if (function_symbol_referenced_p (XVECEXP (x, j, k)))
		  return TRUE;
	    }
	  break;

	default:
	  /* Nothing to do.  */
	  break;
	}
    }

  return FALSE;
}

/* Returns TRUE if OP contains a symbol reference or a label reference */
bool
symbolic_reference_mentioned_p (rtx op)
{
  const char *fmt;
  int i;

  if (GET_CODE (op) == SYMBOL_REF || GET_CODE (op) == LABEL_REF)
    return TRUE;

  /* UNSPEC_TLS entries for a symbol include a LABEL_REF for the
     referencing instruction, but they are constant offsets, not
     symbols.  */
  if (GET_CODE (op) == UNSPEC
      && (XINT (op, 1) == UNSPEC_TLS_LE || XINT (op, 1) == UNSPEC_TLS_LD
	  || XINT (op, 1) == UNSPEC_TLS_DTPOFF || XINT (op, 1) == UNSPEC_TLS_GD
	  || XINT (op, 1) == UNSPEC_TLS_IE))
    return false;

  fmt = GET_RTX_FORMAT (GET_CODE (op));
  for (i = GET_RTX_LENGTH (GET_CODE (op)) - 1; i >= 0; i--)
    {
      if (fmt[i] == 'E')
	{
	  int j;

	  for (j = XVECLEN (op, i) - 1; j >= 0; j--)
	    if (symbolic_reference_mentioned_p (XVECEXP (op, i, j)))
	      return TRUE;
	}

      else if (fmt[i] == 'e' && symbolic_reference_mentioned_p (XEXP (op, i)))
	return TRUE;
    }

  return FALSE;
}

/* Returns TRUE if OP is (const (unspec ([] UNSPEC_*))) or
   (unspec ([] UNSPEC_*)) with unspec type compatible with PIC
   code
*/
bool
kvx_legitimate_pic_symbolic_ref_p (rtx op)
{
  /* Unwrap CONST */
  if (GET_CODE (op) == CONST)
    op = XEXP (op, 0);

  /* Unwrap PLUS */
  if (GET_CODE (op) == PLUS && CONST_INT_P (XEXP (op, 1)))
    op = XEXP (op, 0);

  /* Valid ref are wrapped in UNSPEC */
  if (GET_CODE (op) != UNSPEC)
    return false;

  int unspec = XINT ((op), 1);
  return (unspec == UNSPEC_GOT || unspec == UNSPEC_GOTOFF
	  || unspec == UNSPEC_TLS_DTPOFF || unspec == UNSPEC_TLS_GD
	  || unspec == UNSPEC_TLS_LD || unspec == UNSPEC_TLS_LE
	  || unspec == UNSPEC_TLS_IE || unspec == UNSPEC_PCREL);
}

/* Returns TRUE if X can be used as an operand in PIC code.
 * LABELs are rejected as they should be handled by mov expand
 */

bool
kvx_legitimate_pic_operand_p (rtx x)
{
  if (GET_CODE (x) == SYMBOL_REF || GET_CODE (x) == LABEL_REF
      || (GET_CODE (x) == CONST && GET_CODE (XEXP (x, 0)) == PLUS
	  && (GET_CODE (XEXP (XEXP (x, 0), 0)) == SYMBOL_REF
	      || GET_CODE (XEXP (XEXP (x, 0), 0)) == LABEL_REF)))
    return false;

  return true;
}

static rtx
gen_add2_pcrel_insn (rtx dest, rtx pcrel_offset)
{
  if (TARGET_32)
    return gen_add_pcrel_si (dest, pcrel_offset);
  else
    return gen_add_pcrel_di (dest, pcrel_offset);
}

/* Expands a mov which second operand is a constant. */
void
kvx_expand_mov_constant (rtx operands[])
{
  rtx dest = operands[0];
  rtx src = operands[1];
  rtx new_rtx;

  if (GET_CODE (src) == SYMBOL_REF || GET_CODE (src) == LABEL_REF
      || GET_CODE (src) == CONST)
    {
      rtx base, offset;
      enum kvx_symbol_type sty;
      rtx pic_reg;

      /* If we have (const (plus symbol offset)), separate out the offset
	 before we start classifying the symbol.  */
      split_const (src, &base, &offset);

      sty = kvx_classify_symbol (base);
      switch (sty)
	{
	case SYMBOL_ABSOLUTE:
	case LABEL_ABSOLUTE:
	  /* Emit: dest = sym */
	  emit_insn (gen_rtx_SET (dest, src));
	  break;

	case LABEL_PCREL_ABSOLUTE:
	  /* Emit dest = pc + @pcrel(label) */
	  emit_insn (gen_add2_pcrel_insn (dest, src));
	  break;

	case SYMBOL_GOT:
	  /*
	   * Emit dest = *(@got(sym) + $pic)
	   */
	  pic_reg = gen_reg_rtx (Pmode);
	  emit_insn (gen_set_gotp_insn (pic_reg));

	  new_rtx
	    = gen_rtx_CONST (Pmode, gen_rtx_UNSPEC (Pmode, gen_rtvec (1, base),
						    UNSPEC_GOT));

	  emit_move_insn (dest, pic_reg);

	  emit_move_insn (dest, gen_rtx_MEM (Pmode, gen_rtx_PLUS (Pmode, dest,
								  new_rtx)));
	  if (INTVAL (offset) != 0)
	    emit_insn (gen_add2_insn (dest, offset));

	  crtl->uses_pic_offset_table = true;
	  break;

	case SYMBOL_GOTOFF:
	  /*
	   * Emit dest = @gotoff(sym)[$pic]
	   */
	  pic_reg = gen_reg_rtx (Pmode);
	  emit_insn (gen_set_gotp_insn (pic_reg));

	  new_rtx
	    = gen_rtx_CONST (Pmode, gen_rtx_UNSPEC (Pmode, gen_rtvec (1, base),
						    UNSPEC_GOTOFF));

	  emit_move_insn (dest, pic_reg);

	  emit_move_insn (dest, gen_rtx_PLUS (Pmode, dest, new_rtx));

	  if (INTVAL (offset) != 0)
	    emit_insn (gen_add2_insn (dest, offset));

	  crtl->uses_pic_offset_table = true;
	  break;

	case SYMBOL_TLSIE:
	case SYMBOL_TLSLD:
	case SYMBOL_TLSGD:
	case SYMBOL_TLSLE:
	  operands[1] = kvx_legitimize_tls_reference (base);
	  gcc_assert (operands[1] != src);

	  emit_insn (gen_rtx_SET (dest, operands[1]));

	  if (INTVAL (offset) != 0)
	    emit_insn (gen_add2_insn (dest, offset));

	  break;

	default:
	  gcc_unreachable ();
	}
      return;
    }

  emit_insn (gen_rtx_SET (dest, src));

  /* if (kvx_has_tls_reference (operands[1])) */
  /*   { */
  /*     rtx src = operands[1]; */
  /*     operands[1] = kvx_legitimize_tls_reference (src); */
  /*     gcc_assert (operands[1] != src); */
  /*   } else if (flag_pic) */
  /*   { */
  /*     if (SYMBOLIC_CONST(operands[1])) */
  /* 	operands[1] = kvx_legitimize_pic_address (operands[1],
   * operands[0]); */
  /*   } */
  return;
}

/* Canonicalize a comparison from one we don't have to one we do have.  */
static void
kvx_canonicalize_comparison (int *code, rtx *op0, rtx *op1,
			     bool op0_preserve_value)
{
  machine_mode mode = GET_MODE (*op0);
  bool float_compare_p = (GET_MODE_CLASS (mode) == MODE_FLOAT);
  static struct rtx_def rtx_; PUT_CODE (&rtx_, (enum rtx_code)*code);

  if (!op0_preserve_value && float_compare_p
      && !float_comparison_operator (&rtx_, VOIDmode))
    {
      rtx temp = *op1;
      *op1 = *op0;
      *op0 = temp;
      *code = (int)swap_condition ((enum rtx_code)*code);
    }
}

/* Return the mode of a predicate resulting from comparing MODE. */
static enum machine_mode
kvx_get_predicate_mode (enum machine_mode mode)
{
  switch (mode)
    {
    // 64-bit modes
    case E_V4HFmode:
      return V4HImode;
    case E_V2SFmode:
      return V2SImode;
    // 128-bit modes
    case E_V8HFmode:
      return V8HImode;
    case E_V4SFmode:
      return V4SImode;
    case E_V2DFmode:
      return V2DImode;
    // 256-bit modes
    case E_V16HFmode:
      return V16HImode;
    case E_V8SFmode:
      return V8SImode;
    case E_V4DFmode:
      return V4DImode;
    // Scalar modes
    case E_HFmode:
    case E_SFmode:
    case E_DFmode:
      return DImode;
    // Other modes
    default:
      break;
    }
  return mode;
}

/* Emulate V<n>QI comparisons by expanding them to V4HI comparisons. */
static rtx
kvx_emulate_vxqi_comparison(rtx pred, enum rtx_code cmp_code, rtx left, rtx right)
{
  machine_mode cmp_mode = GET_MODE (left);
  unsigned mode_size = GET_MODE_SIZE (cmp_mode);
  for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
    {
      rtx rightc = simplify_gen_subreg (V4HImode, right, cmp_mode, offset);
      rtx leftc = simplify_gen_subreg (V4HImode, left, cmp_mode, offset);
      rtx predc = simplify_gen_subreg (V4HImode, pred, cmp_mode, offset);
      rtx righto = CONST0_RTX (V4HImode), righte = CONST0_RTX (V4HImode);
      if (!const_zero_operand (right, cmp_mode))
	{
	  righto = gen_reg_rtx (V4HImode), righte = gen_reg_rtx (V4HImode);
	  emit_insn (gen_rtx_SET (righto, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, rightc), UNSPEC_QXOBHQ)));
	  emit_insn (gen_rtx_SET (righte, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, rightc), UNSPEC_QXEBHQ)));
	}
      rtx lefto = gen_reg_rtx (V4HImode), lefte = gen_reg_rtx (V4HImode);
      emit_insn (gen_rtx_SET (lefto, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, leftc), UNSPEC_QXOBHQ)));
      emit_insn (gen_rtx_SET (lefte, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, leftc), UNSPEC_QXEBHQ)));
      rtx predo = gen_reg_rtx (V4HImode), prede = gen_reg_rtx (V4HImode);
      kvx_lower_comparison (predo, cmp_code, lefto, righto);
      kvx_lower_comparison (prede, cmp_code, lefte, righte);
      emit_insn (gen_rtx_SET (predo, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, predo), UNSPEC_QXOBHQ)));
      emit_insn (gen_rtx_SET (prede, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, prede), UNSPEC_ZXOBHQ)));
      emit_insn (gen_rtx_SET (predc, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, predo, prede), UNSPEC_OROEBO)));
    }
  return pred;
}

/* Emit the compare insn and return the predicate register if lowering occurred.
 * Lowering occurs if the comparison is not between scalar integer and zero.
 * In case of floating-point lowering, the left and right operand may be swapped.
 * The predicate register is created if lowering and if NULL_RTX is passed.  */
rtx
kvx_lower_comparison (rtx pred, enum rtx_code cmp_code, rtx left, rtx right)
{
  machine_mode cmp_mode = GET_MODE (left);
  enum mode_class cmp_class = GET_MODE_CLASS (cmp_mode);
  bool float_compare_p
    = (cmp_class == MODE_FLOAT || cmp_class == MODE_VECTOR_FLOAT);

  if (cmp_class == MODE_INT && const_zero_operand (right, cmp_mode))
    return NULL_RTX;

  if (pred == NULL_RTX)
    pred = gen_reg_rtx (kvx_get_predicate_mode (cmp_mode));

  if (VECTOR_MODE_P (cmp_mode) && GET_MODE_INNER (cmp_mode) == QImode)
    return kvx_emulate_vxqi_comparison (pred, cmp_code, left,right);

  machine_mode pred_mode = GET_MODE (pred);
  rtx cmp = gen_rtx_fmt_ee (cmp_code, pred_mode, left, right);

  if (float_compare_p)
    {
      if (!float_comparison_operator (cmp, VOIDmode))
	{
	  enum rtx_code swapped = swap_condition (cmp_code);

	  if (swapped == cmp_code)
	    {
	      rtx tmp = gen_reg_rtx (pred_mode);
	      rtx tmp2 = gen_reg_rtx (pred_mode);
	      rtx cmp2 = copy_rtx (cmp);
	      enum rtx_code cond_code;

	      if (cmp_code == UNORDERED)
		{
		  PUT_CODE (cmp, UNGE);
		  PUT_CODE (cmp2, UNLT);
		  cond_code = AND;
		}
	      else if (cmp_code == ORDERED)
		{
		  PUT_CODE (cmp, GE);
		  PUT_CODE (cmp2, LT);
		  cond_code = IOR;
		}
	      else
		{
		  gcc_unreachable ();
		}

	      emit_insn (gen_rtx_SET (tmp, cmp));
	      emit_insn (gen_rtx_SET (tmp2, cmp2));
	      cmp = gen_rtx_fmt_ee (cond_code, pred_mode, tmp, tmp2);
	    }
	  else
	    {
	      PUT_CODE (cmp, swapped);
	      XEXP (cmp, 0) = right;
	      XEXP (cmp, 1) = left;
	    }
	  gcc_assert (swapped == cmp_code
		      || float_comparison_operator (cmp, VOIDmode));
	}

      XEXP (cmp, 0) = force_reg (cmp_mode, XEXP (cmp, 0));
      XEXP (cmp, 1) = force_reg (cmp_mode, XEXP (cmp, 1));
    }
  else if (VECTOR_MODE_P (cmp_mode))
    {
      XEXP (cmp, 0) = force_reg (cmp_mode, XEXP (cmp, 0));
      XEXP (cmp, 1) = force_reg (cmp_mode, XEXP (cmp, 1));
    }

  emit_insn (gen_rtx_SET (pred, cmp));
  return pred;
}

/* Emulate V<n>QI cond moves by expanding them to V4HI cond moves. */
static void
kvx_emulate_vxqi_simplecond_move(rtx pred, enum rtx_code cmp_code, rtx src, rtx dst)
{
  machine_mode mode = GET_MODE (dst);
  rtx const0_v4hi_rtx = CONST0_RTX (V4HImode);
  unsigned mode_size = GET_MODE_SIZE (mode);
  for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
    {
      rtx srcc = simplify_gen_subreg (V4HImode, src, mode, offset);
      rtx dstc = simplify_gen_subreg (V4HImode, dst, mode, offset);
      rtx predc = simplify_gen_subreg (V4HImode, pred, mode, offset);

      rtx srco = gen_reg_rtx (V4HImode), srce = gen_reg_rtx (V4HImode);
      emit_insn (gen_rtx_SET (srco, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, srcc), UNSPEC_QXOBHQ)));
      emit_insn (gen_rtx_SET (srce, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, srcc), UNSPEC_QXEBHQ)));

      rtx predo = gen_reg_rtx (V4HImode), prede = gen_reg_rtx (V4HImode);
      emit_insn (gen_rtx_SET (predo, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, predc), UNSPEC_QXOBHQ)));
      emit_insn (gen_rtx_SET (prede, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, predc), UNSPEC_QXEBHQ)));

      rtx cmpo = gen_rtx_fmt_ee (cmp_code, VOIDmode, predo, const0_v4hi_rtx);
      rtx cmpe = gen_rtx_fmt_ee (cmp_code, VOIDmode, prede, const0_v4hi_rtx);

      rtx dsto = gen_reg_rtx (V4HImode), dste = gen_reg_rtx (V4HImode);
      emit_insn (gen_rtx_SET (dsto, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, dstc), UNSPEC_QXOBHQ)));
      emit_insn (gen_rtx_SET (dste, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, dstc), UNSPEC_QXEBHQ)));

      emit_insn (gen_rtx_SET (dsto, gen_rtx_IF_THEN_ELSE (V4HImode, cmpo, srco, dsto)));
      emit_insn (gen_rtx_SET (dste, gen_rtx_IF_THEN_ELSE (V4HImode, cmpe, srce, dste)));

      emit_insn (gen_rtx_SET (dsto, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, dsto), UNSPEC_QXOBHQ)));
      emit_insn (gen_rtx_SET (dste, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, dste), UNSPEC_ZXOBHQ)));
      emit_insn (gen_rtx_SET (dstc, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, dsto, dste), UNSPEC_OROEBO)));
    }
}

void
kvx_expand_conditional_move (rtx target, rtx select1, rtx select2,
			     rtx cmp, rtx left, rtx right)
{
  machine_mode mode = GET_MODE (target);
  enum rtx_code cmp_code = GET_CODE (cmp);
  machine_mode cmp_mode = GET_MODE (left);
  rtx src = NULL_RTX, dst = NULL_RTX;
  bool vector_true_false = false;

  if (VECTOR_MODE_P (mode))
    {
      rtx const0_mode_rtx = CONST0_RTX (mode);
      rtx constm1_mode_rtx = CONSTM1_RTX (mode);
      vector_true_false = (select1 == constm1_mode_rtx)
			  && (select2 == const0_mode_rtx);
    }

  if (vector_true_false)
    {
      dst = target;
    }
  else if (!rtx_equal_p (select1, target) && !rtx_equal_p (select2, target))
    {
      if (reg_overlap_mentioned_p (target, cmp))
	dst = gen_reg_rtx (mode);
      else
	dst = target;

      src = force_reg (mode, select1);
      emit_move_insn (dst, select2);
    }
  else if (rtx_equal_p (select1, target))
    {
      src = force_reg (mode, select2);
      dst = target;

      enum mode_class cmp_class = GET_MODE_CLASS (cmp_mode);
      if (cmp_class == MODE_FLOAT || cmp_class == MODE_VECTOR_FLOAT)
	cmp_code = reverse_condition_maybe_unordered (cmp_code);
      else
	cmp_code = reverse_condition (cmp_code);
    }
  else
    {
      src = force_reg (mode, select1);
      dst = target;
    }

  machine_mode pred_mode = kvx_get_predicate_mode (cmp_mode);
  rtx pred = gen_reg_rtx (pred_mode);

  if (kvx_lower_comparison (pred, cmp_code, left, right))
    cmp_code = NE;
  else
    emit_move_insn (pred, left);

  if (vector_true_false)
    {
      emit_insn (gen_rtx_SET (dst, pred));
    }
  else if (VECTOR_MODE_P (cmp_mode) && GET_MODE_INNER (cmp_mode) == QImode)
    {
      kvx_emulate_vxqi_simplecond_move(pred, cmp_code, src, dst);
    }
  else
    {
      rtx const0_pred_mode_rtx = CONST0_RTX (pred_mode);
      rtx cmp0 = gen_rtx_fmt_ee (cmp_code, VOIDmode, pred, const0_pred_mode_rtx);
      emit_insn (gen_rtx_SET (dst, gen_rtx_IF_THEN_ELSE (mode, cmp0, src, dst)));
    }

  if (dst != target)
    emit_move_insn (target, dst);
}

void
kvx_expand_masked_move (rtx target, rtx select1, rtx select2, rtx mask)
{
  machine_mode mode = GET_MODE (target);
  enum rtx_code cmp_code = NE;
  rtx src = NULL_RTX, dst = NULL_RTX;
  bool vector_true_false = false;

  if (VECTOR_MODE_P (mode))
    {
      rtx const0_mode_rtx = CONST0_RTX (mode);
      rtx constm1_mode_rtx = CONSTM1_RTX (mode);
      vector_true_false = (select1 == constm1_mode_rtx)
			  && (select2 == const0_mode_rtx);
    }

  if (vector_true_false)
    {
      dst = target;
    }
  else if (!rtx_equal_p (select1, target) && !rtx_equal_p (select2, target))
    {
      if (reg_overlap_mentioned_p (target, mask))
	dst = gen_reg_rtx (mode);
      else
	dst = target;

      src = force_reg (mode, select1);
      emit_move_insn (dst, select2);
    }
  else if (rtx_equal_p (select1, target))
    {
      src = force_reg (mode, select2);
      dst = target;
      cmp_code = EQ;
    }
  else
    {
      src = force_reg (mode, select1);
      dst = target;
    }

  if (vector_true_false)
    {
      emit_insn (gen_rtx_SET (dst, mask));
    }
  else if (VECTOR_MODE_P (mode) && GET_MODE_INNER (mode) == QImode)
    {
      kvx_emulate_vxqi_simplecond_move(mask, cmp_code, src, dst);
    }
  else
    {
      machine_mode mask_mode = GET_MODE (mask);
      rtx const0_mask_mode_rtx = CONST0_RTX (mask_mode);
      rtx cmp0 = gen_rtx_fmt_ee (cmp_code, VOIDmode, mask, const0_mask_mode_rtx);
      emit_insn (gen_rtx_SET (dst, gen_rtx_IF_THEN_ELSE (mode, cmp0, src, dst)));
    }

  if (dst != target)
    emit_move_insn (target, dst);
}

/* Return the mode of a 64-bit chunk from a vector mode, else the mode. */
static enum machine_mode
kvx_get_chunk_mode (enum machine_mode mode)
{
  switch (mode)
    {
    // 64-bit modes
    case E_V1DImode:
      return DImode;
    // 128-bit modes
    case E_V16QImode:
      return V8QImode;
    case E_V8HImode:
      return V4HImode;
    case E_V4SImode:
      return V2SImode;
    case E_V2DImode:
      return DImode;
    case E_V8HFmode:
      return V4HFmode;
    case E_V4SFmode:
      return V2SFmode;
    case E_V2DFmode:
      return DFmode;
    // 256-bit modes
    case E_V32QImode:
      return V8QImode;
    case E_V16HImode:
      return V4HImode;
    case E_V8SImode:
      return V2SImode;
    case E_V4DImode:
      return DImode;
    case E_V16HFmode:
      return V4HFmode;
    case E_V8SFmode:
      return V2SFmode;
    case E_V4DFmode:
      return DFmode;
    // Other modes
    default:
      break;
    }
  return mode;
}

void
kvx_expand_vector_insert (rtx target, rtx source, rtx where)
{
  machine_mode vector_mode = GET_MODE (target);
  machine_mode inner_mode = GET_MODE_INNER (vector_mode);
  int width = GET_MODE_SIZE (inner_mode);

  if (CONST_INT_P (where))
    {
      int index = INTVAL (where);
      int major = (index*width) / UNITS_PER_WORD;
      int minor = (index*width) % UNITS_PER_WORD;

      if (GET_MODE_SIZE (inner_mode) == UNITS_PER_WORD)
	{
	  rtx op0 = simplify_gen_subreg (inner_mode, target, vector_mode,
					 major * UNITS_PER_WORD);
	  rtx op1 = source;
	  emit_move_insn (op0, op1);
	}
      else
	{
	  rtx op0 = simplify_gen_subreg (DImode, target, vector_mode,
					 major * UNITS_PER_WORD);
	  rtx op1 = gen_lowpart (DImode, source);
	  rtx op2 = GEN_INT (width * 8);
	  rtx op3 = GEN_INT (minor * 8);
	  rtx opi = gen_rtx_ZERO_EXTRACT (DImode, op0, op2, op3);
	  emit_insn (gen_rtx_SET (opi, op1));
	}

      return;
    }

  gcc_unreachable ();
}

void
kvx_expand_vector_extract (rtx target, rtx source, rtx where)
{
  machine_mode vector_mode = GET_MODE (source);
  machine_mode inner_mode = GET_MODE_INNER (vector_mode);
  int width = GET_MODE_SIZE (inner_mode);

  if (CONST_INT_P (where))
    {
      int index = INTVAL (where);
      int major = (index*width) / UNITS_PER_WORD;
      int minor = (index*width) % UNITS_PER_WORD;

      if (GET_MODE_SIZE (inner_mode) == UNITS_PER_WORD)
	{
	  rtx op0 = target;
	  rtx op1 = simplify_gen_subreg (inner_mode, source, vector_mode,
					 major * UNITS_PER_WORD);
	  emit_move_insn (op0, op1);
	}
      else
	{
	  rtx op0 = simplify_gen_subreg (DImode, target, inner_mode, 0);
	  rtx op1 = simplify_gen_subreg (DImode, source, vector_mode,
					 major * UNITS_PER_WORD);
	  rtx op2 = GEN_INT (width * 8);
	  rtx op3 = GEN_INT (minor * 8);
	  rtx extract = gen_rtx_ZERO_EXTRACT (DImode, op1, op2, op3);
	  emit_insn (gen_rtx_SET (op0, extract));
	}

      return;
    }

  gcc_unreachable ();
}

#define KVX_SBMM8_SPLATB0D 0x0101010101010101ULL
#define KVX_SBMM8_SPLATH0D 0x0201020102010201ULL
#define KVX_SBMM8_SPLATW0D 0x0804020108040201ULL
#define KVX_SBMM8_IDENTITY 0x8040201008040201ULL

/* Splat a value of mode smaller than a word into a word size vector chunk.
 * This is used both for initializing a vector from a scalar, and for the
 * vector arithmetic patterns that operate a vector with a scalar.  */
rtx
kvx_expand_chunk_splat (rtx target, rtx source, machine_mode inner_mode)
{
  HOST_WIDE_INT constant = 0;
  machine_mode chunk_mode = GET_MODE (target);
  unsigned inner_size = GET_MODE_SIZE (inner_mode);

  if (inner_size == UNITS_PER_WORD)
    return source;

  if (!REG_P (source) && !SUBREG_P (source))
    source = force_reg (inner_mode, source);

  switch (inner_size)
    {
    case 1:
      constant = KVX_SBMM8_SPLATB0D;
      break;
    case 2:
      constant = KVX_SBMM8_SPLATH0D;
      break;
    case 4:
      constant = KVX_SBMM8_SPLATW0D;
      break;
    default:
      gcc_unreachable ();
    }
  rtx op2 = force_reg (DImode, GEN_INT (constant));
  rtx op1 = gen_lowpart (inner_mode, source);
  rtx sbmm8 = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (2, op1, op2), UNSPEC_SBMM8);
  emit_insn (gen_rtx_SET (target, sbmm8));
  return target;
}

/* Helper function for kvx_expand_vector_init () in case inner mode size < 64 bits.
 * The init source has been partioned into 64-bit chunks, which are inserted into
 * the corresponding 64-bit chunks of the target.
 */
static rtx
kvx_expand_chunk_insert(rtx target, rtx source, int index, machine_mode inner_mode)
{
  machine_mode chunk_mode = GET_MODE (target);
  unsigned inner_size = GET_MODE_SIZE (inner_mode);

  switch (inner_size)
    {
    case 4:
      {
	rtx op1 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	rtx op2 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	emit_insn (gen_rtx_SET (target, gen_rtx_VEC_CONCAT (chunk_mode, op1, op2)));
      }
      break;
    case 2:
      {
	rtx op1 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	rtx op2 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	rtx op3 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	rtx op4 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	rtx initx4a = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (1, op1), UNSPEC_INITX4A);
	rtx initx4b = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (2, op2, target), UNSPEC_INITX4B);
	rtx initx4c = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (2, op3, target), UNSPEC_INITX4C);
	rtx initx4d = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (2, op4, target), UNSPEC_INITX4D);
	emit_insn (gen_rtx_SET (target, initx4a));
	emit_insn (gen_rtx_SET (target, initx4b));
	emit_insn (gen_rtx_SET (target, initx4c));
	emit_insn (gen_rtx_SET (target, initx4d));
      }
      break;
    case 1:
      {
	rtx op1 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	rtx op2 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	rtx op3 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	rtx op4 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	rtx op5 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	rtx op6 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	rtx op7 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	rtx op8 = copy_to_mode_reg (inner_mode, XVECEXP (source, 0, index++));
	rtx initx8a = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (1, op1), UNSPEC_INITX8A);
	rtx initx8b = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (2, op2, target), UNSPEC_INITX8B);
	rtx initx8c = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (2, op3, target), UNSPEC_INITX8C);
	rtx initx8d = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (2, op4, target), UNSPEC_INITX8D);
	rtx initx8e = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (2, op5, target), UNSPEC_INITX8E);
	rtx initx8f = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (2, op6, target), UNSPEC_INITX8F);
	rtx initx8g = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (2, op7, target), UNSPEC_INITX8G);
	rtx initx8h = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (2, op8, target), UNSPEC_INITX8H);
	emit_insn (gen_rtx_SET (target, initx8a));
	emit_insn (gen_rtx_SET (target, initx8b));
	emit_insn (gen_rtx_SET (target, initx8c));
	emit_insn (gen_rtx_SET (target, initx8d));
	emit_insn (gen_rtx_SET (target, initx8e));
	emit_insn (gen_rtx_SET (target, initx8f));
	emit_insn (gen_rtx_SET (target, initx8g));
	emit_insn (gen_rtx_SET (target, initx8h));
      }
      break;
    default:
      gcc_unreachable ();
    }
  return target;
}

/* Called by the vec_duplicate<mode> standard pattern and by
 * kvx_expand_vector_init().  */
void
kvx_expand_vector_duplicate (rtx target, rtx source)
{
  machine_mode vector_mode = GET_MODE (target);
  machine_mode inner_mode = GET_MODE_INNER (vector_mode);
  machine_mode chunk_mode = kvx_get_chunk_mode (vector_mode);

  rtx chunk = gen_reg_rtx (chunk_mode);
  chunk = kvx_expand_chunk_splat (chunk, source, inner_mode);

  unsigned vector_size = GET_MODE_SIZE (vector_mode);
  if (vector_size > UNITS_PER_WORD)
    emit_insn (gen_rtx_SET (target,
			    gen_rtx_VEC_DUPLICATE (vector_mode, chunk)));
  else
    emit_insn (gen_rtx_SET (target, chunk));

  return;
}

/* Called by the vec_init<mode> standard pattern. */
void
kvx_expand_vector_init (rtx target, rtx source)
{
  machine_mode vector_mode = GET_MODE (target);
  unsigned vector_size = GET_MODE_SIZE (vector_mode);
  machine_mode inner_mode = GET_MODE_INNER (vector_mode);
  unsigned inner_size = GET_MODE_SIZE (inner_mode);
  int nunits = GET_MODE_NUNITS (vector_mode);

  /* Special-case for the duplicate of a value. */
  bool duplicate = true;
  rtx x0 = XVECEXP (source, 0, 0);

  for (int i = 1; i < nunits; ++i)
    {
      rtx x = XVECEXP (source, 0, i);
      duplicate &= rtx_equal_p (x, x0);
    }
  if (duplicate)
    {
      kvx_expand_vector_duplicate (target, x0);
      return;
    }

  /* Inner mode size not smaller than word size, delegate to move pattern. */
  if (inner_size >= UNITS_PER_WORD)
    {
      for (int i = 0; i < nunits; i++)
	{
	  rtx op1 = XVECEXP (source, 0, i);
	  rtx op0 = simplify_gen_subreg (inner_mode, target, vector_mode, i*inner_size);
	  emit_move_insn (op0, op1);
	}
      return;
    }

  /* Process vector init chunk by chunk */
  int nwords = vector_size / UNITS_PER_WORD;
  int chunk_nunits = UNITS_PER_WORD / inner_size;
  machine_mode chunk_mode = kvx_get_chunk_mode (vector_mode);

  for (int i = 0; i < nwords; i++)
    {
      int nvars = 0;
      rtx splat = XVECEXP (source, 0, i * chunk_nunits);
      rtx op0 = simplify_gen_subreg (chunk_mode, target, vector_mode, i*UNITS_PER_WORD);

      /* Count the number of variable elements and compute splat. */
      for (int j = 0; j < chunk_nunits; j++)
	{
	  rtx x = XVECEXP (source, 0, i * chunk_nunits + j);
	  nvars += !(CONST_INT_P (x) || CONST_DOUBLE_P (x));
	  splat = rtx_equal_p (splat, x)? splat: 0;
	}

      /* No variable elements, delegate to move chunk const vector. */
      if (nvars == 0)
	{
	  rtvec v = rtvec_alloc (chunk_nunits);
	  for (int j = 0; j < chunk_nunits; j++)
	    RTVEC_ELT (v, j) = XVECEXP (source, 0, i * chunk_nunits + j);
	  rtx constants = gen_rtx_CONST_VECTOR (chunk_mode, v);
	  emit_move_insn (op0, constants);
	  continue;
	}

      /* Splat first element to all words in chunk. */
      if (splat)
	{
	  kvx_expand_chunk_splat (op0, splat, inner_mode);
	  continue;
	}

      /* Insert the variable lanes in chunk. */
      kvx_expand_chunk_insert (op0, source, i * chunk_nunits, inner_mode);
    }
}

/* Collect the SBMM8 immdiate values to implement a swizzle or a shuffle.
   As the largest vector type is 32 bytes and the word is 8 bytes, there
   are at most 4 words to operate in the destination vector. This corresponds
   to the [4] rows in values. A shuffle has up to two vector inputs,
   this corresponds to the [8] columns in values.  */
struct {
  unsigned char from[32];
  union {
    unsigned char bytes[UNITS_PER_WORD];
    unsigned long long dword;
  } values[8][4];
} kvx_expand_vec_perm;

void
kvx_expand_vec_perm_print (FILE *file, machine_mode vector_mode)
{
  int nwords = GET_MODE_SIZE (vector_mode) / UNITS_PER_WORD;
  machine_mode inner_mode = GET_MODE_INNER (vector_mode);
  int nunits = GET_MODE_NUNITS (vector_mode);
  int ibytes = GET_MODE_SIZE (inner_mode);

  for (int i = 0; i < nunits*ibytes; i++)
    fprintf(file, "[%2d]", kvx_expand_vec_perm.from[i]);
  fprintf(file, " from[]\n");

  for (int orig = 0; orig < 2*nwords; orig++) {
    for (int dest = 0; dest < nwords; dest++) {
      for (int lane = 0; lane < UNITS_PER_WORD; lane++)
	fprintf(file, "%03d ", kvx_expand_vec_perm.values[orig][dest].bytes[lane]);
    }
    fprintf(file, "orig[%d]\n", orig);
  }
  for (int dest = 0; dest < nwords; dest++) {
    fprintf(file, "            dest[%d]            |", dest);
  }
  fprintf(file, "\n");
}

/* Special case of kvx_expand_vec_perm_const_emit with a single MOVE. */
rtx
kvx_expand_vec_perm_const_emit_move (rtx target, rtx source1, rtx source2,
				     int dest, int orig)
{
  machine_mode vector_mode = GET_MODE (target);
  machine_mode chunk_mode = kvx_get_chunk_mode (vector_mode);
  int nwords = GET_MODE_SIZE (vector_mode) / UNITS_PER_WORD;
  rtx op0 = simplify_gen_subreg (chunk_mode, target, vector_mode, dest*UNITS_PER_WORD);
  rtx source = orig >= nwords? source2: source1;
  int offset = orig >= nwords? orig - nwords: orig;
  rtx op1 = simplify_gen_subreg (chunk_mode, source, vector_mode, offset*UNITS_PER_WORD);
  emit_move_insn (op0, op1);
  return op0;
}

/* Special case of kvx_expand_vec_perm_const_emit with INSF and MOVE.
   Logic based on the SBMM8 constants that have at most 1 bit set per byte. */
rtx
kvx_expand_vec_perm_const_emit_insf (rtx target, rtx source1, rtx source2,
				     int dest, int orig1, int orig2)
{
  machine_mode vector_mode = GET_MODE (target);
  int nwords = GET_MODE_SIZE (vector_mode) / UNITS_PER_WORD;
  HOST_WIDE_INT constant1 = kvx_expand_vec_perm.values[orig1][dest].dword;
  HOST_WIDE_INT constant2 = kvx_expand_vec_perm.values[orig2][dest].dword;
  HOST_WIDE_INT constanti = 0;
  int origm = -1, origi = -1;

  // The move constant is the one with a subset of bits of KVX_SBMM8_IDENTITY.
  if ((constant1 & KVX_SBMM8_IDENTITY) == constant1)
    {
      origm = orig1, origi = orig2;
      constanti = constant2;
    }
  else if ((constant2 & KVX_SBMM8_IDENTITY) == constant2)
    {
      origm = orig2, origi = orig1;
      constanti = constant1;
    }

  if (!constanti)
    return NULL_RTX;

  // The insert constant must be KVX_SBMM8_IDENTITY shifted left and truncated.
  int shift = __builtin_ctzll (constanti);
  int count = __builtin_popcountll (constanti);
  HOST_WIDE_INT maski
    = (-1ULL >> __builtin_clzll (constanti)) & (KVX_SBMM8_IDENTITY << shift);

  // For speed we prevent the generation of extract as SBMM8 is faster.
  //if (optimize_insn_for_speed_p () && (shift & 7))
    //return NULL_RTX;

  if (maski == constanti && origm >= 0 && origi >= 0)
    {
      machine_mode chunk_mode = kvx_get_chunk_mode (vector_mode);
      rtx targeti = kvx_expand_vec_perm_const_emit_move (target, source1,
							 source2, dest, origm);
      rtx sourcei = origi >= nwords? source2: source1;
      int offseti = origi >= nwords? origi - nwords: origi;
      rtx op0 = simplify_gen_subreg (DImode, targeti, chunk_mode, 0);
      rtx op2 = GEN_INT (count * 8);
      rtx op1 = NULL_RTX;

      // If shift is not a multiple of 8, extract is needed.
      if ((shift & 7))
	{
	  op1 = gen_reg_rtx (DImode);
	  rtx op3 = GEN_INT ((shift & 7) * 8);
	  rtx opi = simplify_gen_subreg (DImode, sourcei, vector_mode,
					 offseti * UNITS_PER_WORD);
	  rtx extract = gen_rtx_ZERO_EXTRACT (DImode, opi, op2, op3);
	  emit_insn (gen_rtx_SET (op1, extract));
	}
      else
	op1 = simplify_gen_subreg (DImode, sourcei, vector_mode,
				   offseti * UNITS_PER_WORD);

      rtx op3 = GEN_INT (shift & -8);
      rtx opi = gen_rtx_ZERO_EXTRACT (DImode, op0, op2, op3);
      emit_insn (gen_rtx_SET (opi, op1));
      return op0;
    }

  return NULL_RTX;
}

/* Implements swizzle (NULL source2) or shuffle based on kvx_expand_vec_perm.
   Before implementing a generic SBMM8-XORD scheme, we special-case the target
   words that can be computed using a MOVE alone or followed by EXTFZ, INSF. */
void
kvx_expand_vec_perm_const_emit (rtx target, rtx source1, rtx source2)
{
  machine_mode vector_mode = GET_MODE (target);
  int nwords = GET_MODE_SIZE (vector_mode) / UNITS_PER_WORD;
  int range = source2? 2*nwords: nwords;

  for (int dest = 0; dest < nwords; dest++)
    {
      int orig0 = -1, orig1 = -1, orig2 = -1, nconst = 0;

      for (int orig = 0; orig < range; orig++)
	{
	  HOST_WIDE_INT constant = kvx_expand_vec_perm.values[orig][dest].dword;
	  if (constant)
	    {
	      if (constant == KVX_SBMM8_IDENTITY)
		orig0 = orig;
	      else if (orig1 < 0)
		orig1 = orig;
	      else if (orig2 < 0)
		orig2 = orig;
	      nconst++;
	    }
	}

      if (orig0 >= 0
	  && kvx_expand_vec_perm_const_emit_move (target, source1, source2,
						  dest, orig0))
	continue;

      // Force source1 and source2 in registers since we may insert into them.
      if (source1)
	source1 = force_reg (vector_mode, source1);
      if (source2)
	source2 = force_reg (vector_mode, source2);

      if (nconst == 2
	  && kvx_expand_vec_perm_const_emit_insf (target, source1, source2,
						  dest, orig1, orig2))
	continue;

      rtx op0 = simplify_gen_subreg (DImode, target, vector_mode, dest*UNITS_PER_WORD);
      rtx acc = force_reg (DImode, GEN_INT (0));
      for (int orig = 0; orig < range; orig++)
	{
	  HOST_WIDE_INT constant = kvx_expand_vec_perm.values[orig][dest].dword;
	  if (constant)
	    {
	      rtx tmp = gen_reg_rtx (DImode);
	      rtx source = orig >= nwords ? source2 : source1;
	      int offset = orig >= nwords? orig - nwords: orig;
	      rtx op1 = simplify_gen_subreg (DImode, source, vector_mode, offset*UNITS_PER_WORD);
	      rtx op2 = force_reg (DImode, GEN_INT (constant));
	      emit_insn (gen_kvx_sbmm8 (tmp, op1, op2));
	      emit_insn (gen_xordi3 (acc, acc, tmp));
	    }
	}

      emit_move_insn (op0, acc);
    }
}


/* Called by the vec_perm_const<mode> standard pattern.
   First step identifies whether this is a swizzle (one source) or a shuffle.
   Second step fills the kvx_expand_vec_perm structure with SBMM8 immediates.
   Third step emits the permutation with (kvx_expand_vec_perm_const_emit).  */
bool
kvx_expand_vec_perm_const (rtx target, rtx source1, rtx source2, rtx selector)
{
  machine_mode vector_mode = GET_MODE (target);
  machine_mode inner_mode = GET_MODE_INNER (vector_mode);
  int nwords = GET_MODE_SIZE (vector_mode) / UNITS_PER_WORD;
  int nunits = GET_MODE_NUNITS (vector_mode);
  int ibytes = GET_MODE_SIZE (inner_mode);
  unsigned idx_mask = 2*nunits - 1, which = 0;

  gcc_assert (nwords <= 4);
  gcc_assert (nunits*ibytes <= 32);
  memset (&kvx_expand_vec_perm, 0, sizeof (kvx_expand_vec_perm));

  // Fill the kvx_expand_vec_perm.from[] array, where each byte of the
  // destination records the index of the source byte in [source1,source2].
  for (int i = 0; i < nunits; i++)
    {
      rtx elt = XVECEXP (selector, 0, i);
      int index = INTVAL (elt) & idx_mask;
      which |= 1 << (index >= nunits);
      for (int j = 0; j < ibytes; j++)
	kvx_expand_vec_perm.from[i*ibytes + j] = index*ibytes + j;
    }

  // In case the two sources are equal, only index into the first source.
  if (rtx_equal_p (source1, source2) && which == 2)
    {
      for (int i = 0; i < nunits*ibytes; i++)
	kvx_expand_vec_perm.from[i] &= nunits*ibytes - 1;
      which = 1;
    }

  // Use the kvx_expand_vec_perm.from[] array to compute the SBMM8 constants.
  // There is one SBMM8 constant per origin word and per destination word.
  for (int i = 0; i < nwords; i++)
    {
      for (int j = 0; j < nunits*ibytes; j++)
	{
	  int k = kvx_expand_vec_perm.from[j];
	  int dest = j / UNITS_PER_WORD;
	  int lane = j % UNITS_PER_WORD;
	  int orig = k / UNITS_PER_WORD;
	  int shift = k % UNITS_PER_WORD;
	  gcc_assert (orig < 2*nwords && dest < nwords);
	  kvx_expand_vec_perm.values[orig][dest].bytes[lane] = 1 << shift;
	}
    }

#if 0
  kvx_expand_vec_perm_print (stderr, vector_mode);
#endif

  bool overlap = reg_overlap_mentioned_p (target, source1);
  if (which != 1)
    overlap |= reg_overlap_mentioned_p (target, source2);

  rtx temporary = target;
  if (overlap)
    temporary = gen_reg_rtx (vector_mode);

  if (which == 1)
    kvx_expand_vec_perm_const_emit (temporary, source1, NULL_RTX);
  else
    kvx_expand_vec_perm_const_emit (temporary, source1, source2);

  if (overlap)
    emit_move_insn (target, temporary);

  return true;
}

/* Implements TARGET_VECTORIZE_VEC_PERM_CONST.  */
static bool
kvx_vectorize_vec_perm_const (machine_mode vmode, rtx target, rtx op0,
			      rtx op1, const vec_perm_indices &sel)
{
  opt_machine_mode smode = related_int_vector_mode (vmode);
  rtx sel_rtx = vec_perm_indices_to_rtx (smode.else_void (), sel);
  return target ? kvx_expand_vec_perm_const (target, op0, op1, sel_rtx) : true;
}

/* Helper to implement vector cross-element right shift. Two source chunks are
 * provided, with a NULL source2 in case the vacated bits must be cleared. */
rtx
kvx_expand_chunk_shift (rtx target, rtx source1, rtx source2, int shift)
{
  machine_mode mode = GET_MODE (target);
  gcc_assert ((unsigned)shift < 64U);
  if (shift == 0)
    emit_move_insn (target, source1);
  else
    {
      rtx op0 = simplify_gen_subreg (DImode, target, mode, 0);
      rtx op1 = simplify_gen_subreg (DImode, source1, mode, 0);
      if (source2 == NULL_RTX)
	{
	  emit_insn (gen_lshrdi3 (op0, op1, GEN_INT (shift)));
	}
      else
	{
	  rtx temp0 = gen_reg_rtx (DImode);
	  rtx temp1 = gen_reg_rtx (DImode);
	  rtx op2 = simplify_gen_subreg (DImode, source2, mode, 0);
	  emit_insn (gen_ashldi3 (temp1, op2, GEN_INT (64 - shift)));
	  emit_insn (gen_lshrdi3 (temp0, op1, GEN_INT (shift)));
	  emit_insn (gen_iordi3 (op0, temp0, temp1));
	}
    }
  return target;
}

/* Emit a barrier, that is appropriate for memory model MODEL, at the
   start of a sequence implementing an atomic operation.  */
void
kvx_emit_pre_barrier (rtx model)
{
  const enum memmodel mm = memmodel_from_int (INTVAL (model));
  switch (mm & MEMMODEL_BASE_MASK) // treat sync operations as atomic ones
    {
    case MEMMODEL_RELAXED:
    case MEMMODEL_CONSUME:
    case MEMMODEL_ACQUIRE:
      // No pre barrier is required for RELAXED, CONSUME, and ACQUIRE.
      break;
    case MEMMODEL_RELEASE:
    case MEMMODEL_ACQ_REL:
      // FENCE.W.
      {
	rtx modifier = KV3_1 ? gen_rtx_CONST_STRING (VOIDmode, "")
			     : gen_rtx_CONST_STRING (VOIDmode, ".w");
	emit_insn (gen_kvx_fence (modifier));
	break;
      }
    case MEMMODEL_SEQ_CST:
      // FENCE.
      {
	rtx modifier = gen_rtx_CONST_STRING (VOIDmode, "");
	emit_insn (gen_kvx_fence (modifier));
	break;
      }
    default:
      gcc_unreachable ();
    }
}

/* Emit a barrier, that is appropriate for memory model MODEL, at the
   end of a sequence implementing an atomic operation.  */
void
kvx_emit_post_barrier (rtx model)
{
  const enum memmodel mm = memmodel_from_int (INTVAL (model));
  switch (mm & MEMMODEL_BASE_MASK) // treat sync operations as atomic ones
    {
    case MEMMODEL_RELAXED:
    case MEMMODEL_RELEASE:
      // No post barrier is required for RELAXED, and ACQUIRE .
      break;
    case MEMMODEL_ACQUIRE:
    case MEMMODEL_ACQ_REL:
      // Clobber MEM to prevent moving around load and stores.
      {
	emit_insn (gen_kvx_mem_clobber ());
	break;
      }
    case MEMMODEL_SEQ_CST:
      // FENCE.
      {
	rtx modifier = gen_rtx_CONST_STRING (VOIDmode, "");
	emit_insn (gen_kvx_fence (modifier));
	break;
      }
    case MEMMODEL_CONSUME:
      // FENCE.R.
      {
	rtx modifier = KV3_1 ? gen_rtx_CONST_STRING (VOIDmode, "")
			     : gen_rtx_CONST_STRING (VOIDmode, ".r");
	emit_insn (gen_kvx_fence (modifier));
	break;
      }
    default:
      gcc_unreachable ();
    }
}

/* Expand a compare and swap pattern. We do not support weak operation
   (operands[5], operands[6] and operands[7] can be ignored).  */

void
kvx_expand_compare_and_swap (rtx operands[])
{
  rtx mem, oldval, newval, currval;
  rtx cas_retry = gen_label_rtx ();
  rtx cas_return = gen_label_rtx ();
  rtx (*gen) (rtx, rtx, rtx);
  machine_mode mode = GET_MODE (operands[2]);

  gcc_assert ((mode == SImode || mode == DImode));

  mem = operands[2];
  oldval = operands[3];
  newval = operands[4];

  rtx tmp = gen_reg_rtx (TImode);
  rtx low = gen_lowpart (DImode, tmp);
  rtx high = gen_highpart (DImode, tmp);

  // We don't care of operands[6] and operands[7] (memory models to
  // use after the operation). We just need to ensure that memory is
  // consistent before the compare-and-swap.
  emit_insn (gen_mem_thread_fence (GEN_INT (MEMMODEL_SEQ_CST)));

  // Packing data to swap for acswap[wd] insns.
  emit_move_insn (gen_lowpart (mode, high), oldval);
  emit_label (cas_retry);
  emit_move_insn (gen_lowpart (mode, low), newval);

  rtx modifier = gen_rtx_CONST_STRING (VOIDmode, "");
  emit_insn (mode == SImode ? gen_kvx_acswapw (tmp, mem, modifier)
			    : gen_kvx_acswapd (tmp, mem, modifier));

  // If acswap succeeds (LOW is equal to 0x1), then return.
  emit_cmp_and_jump_insns (gen_lowpart (mode, low), const1_rtx, EQ, NULL_RTX,
			   mode, true, cas_return);

  // Else, the acswap has failed, reload MEM (atomically) to ensure
  // that the value wasn't updated to the expected one since.
  currval = gen_reg_rtx (mode);
  gen = mode == SImode ? gen_atomic_loadsi : gen_atomic_loaddi;
  emit_insn (
    gen (gen_lowpart (mode, currval), mem, GEN_INT (MEMMODEL_RELAXED)));

  // If the reloaded MEM is equal to the expected one (HIGH), retry
  // the acswap.
  emit_cmp_and_jump_insns (currval, gen_lowpart (mode, high), EQ, NULL_RTX,
			   mode, true, cas_retry);
  // Else, update HIGH with the current value of MEM, then return.
  emit_move_insn (gen_lowpart (mode, high), currval);

  // LOW contains the boolean to return.
  // HIGH contains the value present in memory before the operation.
  emit_label (cas_return);
  // operands[0] is an output operand which is set to true of false
  // based on whether the operation succeeded.
  emit_move_insn (operands[0], gen_lowpart (SImode, low));
  // operands[1] is an output operand which is set to the contents of
  // the memory before the operation was attempted.
  emit_move_insn (operands[1], gen_lowpart (mode, high));
}

/* Expand an atomic operation pattern (CODE). Only for SImode and
   DImode (CAS loop is too difficult to handle for HI, QI, and TI
   modes). Others modes will be expanded by libatomic if enabled
   anyway. MEM is the memory location where to perform the atomic
   operation with value in VAL. If AFTER is true then store the value
   from MEM into TARGET holds after the operation, if AFTER is false
   then store the value from MEM into TARGET holds before the
   operation. If TARGET is NULL_RTX then discard that value, else
   store the result to TARGET. */

void
kvx_expand_atomic_op (enum rtx_code code, rtx target, bool after, rtx mem,
		      rtx val, rtx model)
{
  machine_mode mode = GET_MODE (mem);
  rtx csloop = gen_label_rtx ();
  rtx tmp = gen_reg_rtx (TImode);
  rtx ret = gen_reg_rtx (mode);
  rtx new_mem_val = gen_lowpart (DImode, tmp);
  rtx curr_mem_val = gen_highpart (DImode, tmp);
  rtx (*gen) (rtx, rtx, rtx);
  rtx op_res, op_res_copy;

  if (target && after)
    op_res_copy = gen_reg_rtx (mode);

  gcc_assert ((mode == SImode || mode == DImode));

  emit_label (csloop); /* cas loop entry point */
  /* copy memory content to perform op on it (atomic uncached load) */
  gen = mode == SImode ? gen_atomic_loadsi : gen_atomic_loaddi;
  emit_insn (
    gen (gen_lowpart (mode, curr_mem_val), mem, GEN_INT (MEMMODEL_RELAXED)));

  /* Perform operation in a cas loop, we do not need to convert
  CURR_MEM_VAL, NEW_MEM_VAL (DImode) and VAL (SImode or
  DImode). expand_simple_binop () is smart enough to select the right
  insn depending on MODE. */
  switch (code)
    {
    case PLUS:
    case IOR:
    case XOR:
    case MINUS:
    case AND:
      op_res = expand_simple_binop (mode, code, curr_mem_val, val, new_mem_val,
				    1, OPTAB_LIB_WIDEN);
      break;
      case MULT: /* NAND */
      {
	rtx x = expand_simple_binop (mode, AND, curr_mem_val, val, NULL_RTX, 1,
				     OPTAB_LIB_WIDEN);
	op_res = expand_simple_unop (mode, NOT, x, new_mem_val, 1);
	break;
      }
    case SET:
      op_res = gen_reg_rtx (mode);
      emit_move_insn (op_res, val);
      break;
    default:
      gcc_unreachable ();
    }

  if (op_res != gen_lowpart (mode, new_mem_val))
    emit_move_insn (gen_lowpart (mode, new_mem_val), op_res);

  /* Save OP_RES when returning it (the result of the operation),
     because ACSWAP will erase it. */
  if (target && after)
    emit_move_insn (op_res_copy, op_res);

  /* Handle pre fence right before acswap. */
  kvx_emit_pre_barrier (model);

  /* Update memory with op result iff memory hasn't been modified since:
     if CURR_MEM_VAL == MEM then update MEM with NEW_MEM_VAL else try again. */
  rtx modifier = gen_rtx_CONST_STRING (VOIDmode, "");
  emit_insn (mode == SImode ? gen_kvx_acswapw (tmp, mem, modifier)
			    : gen_kvx_acswapd (tmp, mem, modifier));

  /* Handle post fence right after acswap. */
  kvx_emit_post_barrier (model);

  /* ACSWAP insn returns 0x0 (fail) or 0x1 (success) in the low part of TMP:
     - if successful: MEM is updated, do not loop
     - if failing: MEM has changed, try again */
  emit_cmp_and_jump_insns (gen_lowpart (mode, new_mem_val), const1_rtx, NE,
			   NULL_RTX, mode, true, csloop);

  /* In addition to modifying MEM, return a value if needed */
  if (target)
    {
      ret = (after
	       /* returning op result */
	       ? op_res_copy
	       /* returning mem content before op */
	       : gen_lowpart (mode, curr_mem_val));
      emit_move_insn (target, ret);
    }
}

/* Expand the atomic test-and-set on byte atomic operation using the
   acswapw insn. */

void
kvx_expand_atomic_test_and_set (rtx operands[])
{
  rtx mem = operands[1];
  rtx model = operands[2];

  rtx tmp = gen_reg_rtx (TImode);
  rtx oldval = gen_lowpart (SImode, (gen_highpart (DImode, tmp)));
  rtx newval = gen_lowpart (SImode, (gen_lowpart (DImode, tmp)));
  rtx (*and3) (rtx, rtx, rtx) = Pmode == SImode ? gen_andsi3 : gen_anddi3;
  rtx (*mul3) (rtx, rtx, rtx) = Pmode == SImode ? gen_mulsi3 : gen_muldi3;

  /* Mask POINTER to get the word ADDRESS := POINTER & -4. */
  rtx pointer = gen_reg_rtx (Pmode);
  emit_move_insn (pointer, XEXP (mem, 0));
  rtx address = gen_reg_rtx (Pmode);
  emit_insn (and3 (address, pointer, GEN_INT (-4)));
  rtx memsi = gen_rtx_MEM (SImode, address);

  /* Boolean CAS loop entry point: load OLDVAL from word ADDRESS. */
  rtx retry = gen_label_rtx ();
  emit_label (retry);
  emit_insn (gen_atomic_loadsi (oldval, memsi, GEN_INT (MEMMODEL_RELAXED)));

  /* Byte SHIFT := ((POINTER & 0x3) * BITS_PER_UNIT). */
  rtx shift = gen_reg_rtx (Pmode);
  emit_insn (and3 (shift, pointer, GEN_INT (0x3)));
  emit_insn (mul3 (shift, shift, GEN_INT (BITS_PER_UNIT)));

  /* Byte to test-and-set is BYTE := OLDVAL >> SHIFT & 0xFF. */
  rtx byte = gen_reg_rtx (SImode);
  emit_insn (gen_lshrsi3 (byte, oldval, gen_lowpart (SImode, shift)));
  emit_insn (gen_andsi3 (byte, byte, GEN_INT (0xFF)));

  /* If BYTE is false, try a compare-and-swap with the byte set to TRUE.
     Else return true (i.e. BYTE) because the lock is already acquired. */
  rtx fini = gen_label_rtx ();
  emit_cmp_and_jump_insns (byte, const0_rtx, NE, NULL_RTX, SImode, true, fini);

  /* Build NEWVAL := OLDVAL | TRUE << SHIFT */
  rtx mask = gen_reg_rtx (SImode);
  emit_move_insn (mask, const1_rtx);
  emit_insn (gen_ashlsi3 (mask, mask, gen_lowpart (SImode, shift)));
  emit_insn (gen_iorsi3 (newval, oldval, mask));

  /* Handle pre fence right before acswap. */
  kvx_emit_pre_barrier (model);

  rtx modifier = gen_rtx_CONST_STRING (VOIDmode, "");
  emit_insn (gen_kvx_acswapw (tmp, memsi, modifier));

  /* Handle post fence right after acswap. */
  kvx_emit_post_barrier (model);

  /* ACSWAP returns 0x0 (fail) or 0x1 (success) in the low part of TMP:
     - if successful: MEM is updated, do not loop,
		      lock is acquired, return false (i.e. BYTE)
     - if failing: MEM has changed, try again */
  emit_cmp_and_jump_insns (newval, const1_rtx, NE, NULL_RTX, SImode, true,
			   retry);

  emit_label (fini);
  emit_move_insn (operands[0], gen_lowpart (QImode, byte));
}

int
kvx_stored_value_bypass_p (rtx_insn *prod_insn, rtx_insn *cons_insn)
{
  rtx prod_set = single_set (prod_insn);
  if (!prod_set)
    return 0;

  rtx cons_set = single_set (cons_insn);
  if (!cons_set)
    return 0;

  rtx produced = SET_DEST (prod_set);
  rtx consumed = SET_SRC (cons_set);

  // Only bypass on the stored value.
  return reg_overlap_mentioned_p (produced, consumed);
}

int
kvx_accumulator_bypass_p (rtx_insn *prod_insn, rtx_insn *cons_insn)
{
  rtx prod_set = single_set (prod_insn);
  if (!prod_set)
    return 0;

  rtx cons_set = single_set (cons_insn);
  if (!cons_set)
    return 0;

  rtx produced = SET_DEST (prod_set);
  rtx consumed = SET_DEST (cons_set);

  // Depends on renaming constraint so only works in SCHED2.
  return reg_overlap_mentioned_p (produced, consumed);
}

static void
kvx_dependencies_fprint (FILE *file, rtx_insn *insn) ATTRIBUTE_UNUSED;

static void
kvx_dependencies_fprint (FILE *file, rtx_insn *insn)
{
  dep_t dep;
  sd_iterator_def sd_it;
  fprintf (file, "forward dependences(# %d)\n", INSN_UID (insn));
  FOR_EACH_DEP (insn, SD_LIST_FORW, sd_it, dep)
    {
      enum reg_note dep_type = DEP_TYPE (dep);
      const char *dtype = "<none>";
      if (dep_type == REG_DEP_TRUE)
	dtype = "true";
      if (dep_type == REG_DEP_ANTI)
	dtype = "anti";
      if (dep_type == REG_DEP_OUTPUT)
	dtype = "output";
      if (dep_type == REG_DEP_CONTROL)
	dtype = "control";
      fprintf (file, "\t%s -> (# %d)\n", dtype, INSN_UID (DEP_CON (dep)));
    }
  fprintf (file, "backward dependences(# %d)\n", INSN_UID (insn));
  FOR_EACH_DEP (insn, SD_LIST_BACK, sd_it, dep)
    {
      enum reg_note dep_type = DEP_TYPE (dep);
      const char *dtype = "<none>";
      if (dep_type == REG_DEP_TRUE)
	dtype = "true";
      if (dep_type == REG_DEP_ANTI)
	dtype = "anti";
      if (dep_type == REG_DEP_OUTPUT)
	dtype = "output";
      if (dep_type == REG_DEP_CONTROL)
	dtype = "control";
      fprintf (file, "\t%s <- (# %d)\n", dtype, INSN_UID (DEP_PRO (dep)));
    }
}

static int
kvx_sched_issue_rate (void)
{
  return 5;
}

static int
kvx_sched_variable_issue (FILE *file ATTRIBUTE_UNUSED,
			  int verbose ATTRIBUTE_UNUSED, rtx_insn *insn,
			  int more)
{
  rtx x = PATTERN (insn);
  if (GET_CODE (x) == CLOBBER || GET_CODE (x) == USE)
    return more;
  // Cannot issue further insns at the same cycle as JUMP or CALL.
  if (JUMP_P (insn) || CALL_P (insn))
    return 0;
  return more - 1;
}

/* Implements TARGET_SCHED_ADJUST_COST.  */
static int
kvx_sched_adjust_cost (rtx_insn *cons_insn, int dep_type, rtx_insn *prod_insn,
		       int cost, unsigned int dw ATTRIBUTE_UNUSED)
{
  if (dep_type == REG_DEP_TRUE)
    {
      // Use (set_of) instead of (reg_overlap_mentioned_p) to catch cases in
      // SCHED2 of producing a register pair and consuming a single register.
      if (JUMP_P (cons_insn))
	// Reduce cost except for the dependence carrying the tested value.
	// Case of carrying is when PROD_INSN modifies a REG used by CONS_INSN.
	{
	  rtx x = PATTERN (cons_insn);
	  if (GET_CODE (x) == PARALLEL)
	    x = XVECEXP (x, 0, 0);
	  if (GET_CODE (x) == SET)
	    {
	      x = SET_SRC (x);
	      if (GET_CODE (x) == IF_THEN_ELSE)
		x = XEXP (XEXP (x, 0), 0);
	      if (GET_CODE (x) == ZERO_EXTRACT)
		x = XEXP (x, 0);
	      if (!REG_P (x) || !set_of (x, prod_insn))
		cost = 0;
	    }
	  else if (ANY_RETURN_P (x))
	    {
	      rtx y = PATTERN (prod_insn);
	      if (GET_CODE (y) == PARALLEL)
		y = XVECEXP (y, 0, 0);
	      if (GET_CODE (y) == SET)
		y = SET_DEST (y);
	      if (!REG_P (y) || REGNO (y) != KV3_RA_REGNO)
		cost = 0;
	    }
	}
      else if (CALL_P (cons_insn))
	// Reduce cost except for the dependence carrying the call target.
	// Case of carrying is when PROD_INSN modifies a REG used by CONS_INSN.
	{
	  rtx x = PATTERN (cons_insn);
	  if (GET_CODE (x) == PARALLEL)
	    x = XVECEXP (x, 0, 0);
	  if (GET_CODE (x) == SET)
	    x = SET_SRC (x);
	  if (GET_CODE (x) == CALL)
	    x = XEXP (XEXP (x, 0), 0);
	  if (!REG_P (x) || !set_of (x, prod_insn))
	    cost = 0;
	}
      else if (recog_memoized (prod_insn) >= 0
	       && recog_memoized (cons_insn) >= 0)
	// Adjust cost between instructions based on their type attribute.
	{
	  rtx prod_set = single_set (prod_insn);
	  rtx cons_set = single_set (cons_insn);
	  enum attr_type prod_type = get_attr_type (prod_insn);
	  // If the producer is a load feeding the target of a conditional
	  // or a scatter load, set cost to 1 instead of the load latency.
	  if (prod_type >= TYPE_LSU_LOAD && prod_type < TYPE_LSU_AUXR_STORE)
	    {
	      rtx x = prod_set ? SET_DEST (prod_set) : 0;
	      rtx op = cons_set ? SET_SRC (cons_set) : 0;
	      if (x && op && GET_CODE (op) == UNSPEC)
		{
		  int unspec = XINT (op, 1);
		  if (SUBREG_P (x))
		    x = SUBREG_REG (x);
		  rtx y = XVECEXP (op, 0, 0);
		  if (SUBREG_P (y))
		    y = SUBREG_REG (y);
		  if (x == y
		      && (unspec == UNSPEC_LOADC || unspec == UNSPEC_XLOADC
			  || unspec == UNSPEC_XLOADS))
		    cost = 1;
		}
	    }
	}
    }
  else if (dep_type == REG_DEP_ANTI)
    {
      cost = 0;
      if (JUMP_P (prod_insn) || CALL_P (prod_insn))
	// Consumer INSN must issue after a JUMP or CALL producer.
	{
	  cost = 1;
	}
      else if (GET_CODE (PATTERN (prod_insn)) == CLOBBER)
	// Delay consumer INSN of CLOBBER for non-zero number of clock cycles.
	// This corrects the rewriting of dependencies by chain_to_prev_insn().
	// Problem appears in cases the CLOBBER of an INSF is located after the
	// producer for the INSF. So we find this producer and apply its cost.
	{
	  cost = 1;
	  dep_t dep;
	  sd_iterator_def sd_it;
	  FOR_EACH_DEP (prod_insn, SD_LIST_BACK, sd_it, dep)
	    {
	      if (DEP_TYPE (dep) == REG_DEP_TRUE)
		{
		  rtx_insn *pro_insn = DEP_PRO (dep);
		  int pro_cost = insn_cost (pro_insn, true);
		  if (cost < pro_cost)
		    cost = pro_cost;
		}
	    }
	}
    }
  else if (dep_type == REG_DEP_OUTPUT)
    {
      cost = 1;
      if (JUMP_P (cons_insn) || CALL_P (cons_insn))
	// Consumer is JUMP or CALL, producer can issue at same clock cycle.
	{
	  cost = 0;
	}
    }

  return cost;
}

static int
kvx_sched_adjust_priority (rtx_insn *insn, int priority)
{
  rtx x = PATTERN (insn);
  // CLOBBER insns better remain first in scheduling group after SCHED1.
  if (GET_CODE (x) == CLOBBER)
    priority += 10;
  return priority;
}

static void
kvx_sched_dependencies_evaluation_hook (rtx_insn *head, rtx_insn *tail)
{
  rtx_insn *next_tail = NEXT_INSN (tail);
  for (rtx_insn *insn = head; insn != next_tail; insn = NEXT_INSN (insn))
    {
      // Place holder, do nothing for now.
    }
}

/* SCHED2 data structure. */
static struct kvx_sched2
{
  int max_uid;
  int prev_uid;
  short *insn_cycle;
  unsigned char *insn_flags;
} kvx_sched2;
#define KVX_SCHED2_INSN_HEAD 1
#define KVX_SCHED2_INSN_START 2
#define KVX_SCHED2_INSN_STOP 4
#define KVX_SCHED2_INSN_TAIL 8
#define KVX_SCHED2_INSN_STALL 16
static void
kvx_sched2_ctor (int max_uid)
{
  kvx_sched2.max_uid = max_uid;
  kvx_sched2.prev_uid = -1;
  kvx_sched2.insn_cycle = XNEWVEC (short, kvx_sched2.max_uid);
  memset (kvx_sched2.insn_cycle, -1, sizeof (short) * kvx_sched2.max_uid);
  kvx_sched2.insn_flags = XCNEWVEC (unsigned char, kvx_sched2.max_uid);
}
static void
kvx_sched2_dtor (void)
{
  kvx_sched2.max_uid = 0;
  kvx_sched2.prev_uid = -1;
  XDELETEVEC (kvx_sched2.insn_cycle);
  kvx_sched2.insn_cycle = 0;
  XDELETEVEC (kvx_sched2.insn_flags);
  kvx_sched2.insn_flags = 0;
}

static void
kvx_sched_init (FILE *file ATTRIBUTE_UNUSED, int verbose ATTRIBUTE_UNUSED,
		int max_ready ATTRIBUTE_UNUSED)
{
  if (reload_completed)
    {
      if ((unsigned) kvx_sched2.prev_uid < (unsigned) kvx_sched2.max_uid)
	{
	  kvx_sched2.insn_flags[kvx_sched2.prev_uid]
	    |= KVX_SCHED2_INSN_STOP | KVX_SCHED2_INSN_TAIL;
	}
      kvx_sched2.prev_uid = -1;
    }
}

static void
kvx_sched_finish (FILE *file ATTRIBUTE_UNUSED, int verbose ATTRIBUTE_UNUSED)
{
  if (reload_completed)
    {
      if ((unsigned) kvx_sched2.prev_uid < (unsigned) kvx_sched2.max_uid)
	{
	  kvx_sched2.insn_flags[kvx_sched2.prev_uid]
	    |= KVX_SCHED2_INSN_STOP | KVX_SCHED2_INSN_TAIL;
	}
    }
}

static void
kvx_sched_init_global (FILE *file ATTRIBUTE_UNUSED,
		       int verbose ATTRIBUTE_UNUSED, int old_max_uid)
{
  kvx_scheduling = true;
  if (reload_completed)
    kvx_sched2_ctor (old_max_uid);
}

static void
kvx_sched_finish_global (FILE *file ATTRIBUTE_UNUSED,
			 int verbose ATTRIBUTE_UNUSED)
{
}

static int
kvx_sched_dfa_new_cycle (FILE *dump ATTRIBUTE_UNUSED,
			 int verbose ATTRIBUTE_UNUSED, rtx_insn *insn,
			 int last_clock ATTRIBUTE_UNUSED, int clock,
			 int *sort_p ATTRIBUTE_UNUSED)
{
  // Use this hook to record the cycle and flags of INSN in SCHED2.
  int uid = INSN_UID (insn);
  if ((unsigned) uid < (unsigned) kvx_sched2.max_uid
      && GET_CODE (PATTERN (insn)) != USE
      && GET_CODE (PATTERN (insn)) != CLOBBER)
    {
      int prev_uid = kvx_sched2.prev_uid;
      if (prev_uid < 0)
	{
	  // Head of the scheduling region, start a new bundle.
	  kvx_sched2.insn_flags[uid]
	    = KVX_SCHED2_INSN_HEAD | KVX_SCHED2_INSN_START;
	}
      else if (clock > kvx_sched2.insn_cycle[prev_uid])
	{
	  // Advanced clock, stop previous bundle and start a new one.
	  kvx_sched2.insn_flags[prev_uid] |= KVX_SCHED2_INSN_STOP;
	  kvx_sched2.insn_flags[uid] = KVX_SCHED2_INSN_START;
	}
      else if (kvx_sched2.insn_flags[prev_uid] & KVX_SCHED2_INSN_STOP)
	{
	  // Previous bundle was stopped for some reason, start a new one.
	  kvx_sched2.insn_flags[uid] |= KVX_SCHED2_INSN_START;
	}

      if (JUMP_P (insn) || CALL_P (insn))
	{
	  // JUMP or CALL, stop the current bundle regardless of clock.
	  kvx_sched2.insn_flags[uid] |= KVX_SCHED2_INSN_STOP;
	}

      kvx_sched2.insn_cycle[uid] = clock;
      kvx_sched2.prev_uid = uid;
    }
  return 0;
}

static void
kvx_sched_set_sched_flags (struct spec_info_def *spec_info)
{
  // Speculative scheduling is enabled by non-zero spec_info->mask.
  spec_info->mask = 0;
}

// Always return true, as long-running instructions are fully pipelined.
static bool
kvx_sched_can_speculate_insn (rtx_insn *insn ATTRIBUTE_UNUSED)
{
  return true;
}

static int
kvx_sched_sms_res_mii (struct ddg *g)
{
  int insn_count = 0;
  int tiny_count = 0;
  int thin_count = 0;
  int lite_count = 0;
  int full_count = 0;
  int auxr_count = 0;
  int lsu_count = 0;
  int mau_count = 0;
  int bcu_count = 0;
  int tca_count = 0;
  int issue_rate = kvx_sched_issue_rate ();

  for (int i = 0; i < g->num_nodes; i++)
    {
      rtx_insn *insn = g->nodes[i].insn;
      if (NONDEBUG_INSN_P (insn))
	{
	  insn_count++;
	  // Keep the TYPE tests in sync with the order of the types.md file.
	  enum attr_type type = get_attr_type (insn);
	  if (type == TYPE_ALL)
	    {
	      insn_count += issue_rate - 1;
	      lsu_count++, mau_count++;
	      bcu_count++;
	    }
	  else if (type == TYPE_ALU_NOP)
	    ;
	  else if (type >= TYPE_ALU_TINY && type < TYPE_LSU)
	    {
	      if (type >= TYPE_ALU_TINY && type < TYPE_ALU_TINY_X2)
		tiny_count++;
	      else if (type >= TYPE_ALU_TINY_X2 && type < TYPE_ALU_TINY_X4)
		tiny_count += 2;
	      else if (type >= TYPE_ALU_TINY_X4 && type < TYPE_ALU_THIN)
		tiny_count += 4;
	      else if (type >= TYPE_ALU_THIN && type < TYPE_ALU_THIN_X2)
		thin_count++;
	      else if (type >= TYPE_ALU_THIN_X2 && type < TYPE_ALU_LITE)
		thin_count += 2;
	      else if (type >= TYPE_ALU_LITE && type < TYPE_ALU_LITE_X2)
		lite_count++;
	      else if (type >= TYPE_ALU_LITE_X2 && type < TYPE_ALU_FULL)
		lite_count += 2;
	      else if (type >= TYPE_ALU_FULL && type < TYPE_LSU)
		full_count++;
	      else
		gcc_unreachable ();
	    }
	  else if (type >= TYPE_LSU && type < TYPE_MAU)
	    {
	      lsu_count++;
	      if (type >= TYPE_LSU_AUXR_STORE && type < TYPE_LSU_CRRP_STORE)
		auxr_count++;
	    }
	  else if (type >= TYPE_MAU && type < TYPE_BCU)
	    {
	      mau_count++;
	      if (type >= TYPE_MAU_AUXR)
		auxr_count++;
	    }
	  else if (type >= TYPE_BCU && type < TYPE_TCA)
	    bcu_count++;
	  else if (type >= TYPE_TCA && type <= TYPE_TCA_FLOAT)
	    tca_count++;
	  else
	    gcc_unreachable ();
	}
    }

  if (KV3_1)
    lite_count += thin_count;
  if (KV3_2)
    tiny_count += thin_count;

  int res_mii = (insn_count + issue_rate - 1) / issue_rate;
  if (res_mii < (tiny_count + 3)/4)
    res_mii = (tiny_count + 3)/4;
  if (res_mii < (lite_count + 1)/2)
    res_mii = (lite_count + 1)/2;
  if (res_mii < full_count)
    res_mii = full_count;
  if (res_mii < auxr_count)
    res_mii = auxr_count;
  if (res_mii < lsu_count)
    res_mii = lsu_count;
  if (res_mii < mau_count)
    res_mii = mau_count;
  if (res_mii < bcu_count)
    res_mii = bcu_count;
  if (res_mii < tca_count)
    res_mii = tca_count;

  return res_mii;
}

/* FIXME AUTO: This must be fixed for coolidge */
/* See T7749 */
static int
kvx_sched_reassociation_width (unsigned int opc, enum machine_mode mode)
{
  int res = 1;

  /* see tree.c:associative_tree_code () for possible values of opc. */

  switch (opc)
    {
    case BIT_IOR_EXPR:
    case BIT_AND_EXPR:
    case BIT_XOR_EXPR:
      if (mode == SImode || mode == HImode || mode == QImode || mode == DImode)
	res = 4;
      else if (mode == TImode)
	res = 2;
      break;
    case PLUS_EXPR:
    case MIN_EXPR:
    case MAX_EXPR:
      if (mode == SImode || mode == HImode || mode == QImode || mode == DImode)
	res = 4;
      break;
    case MULT_EXPR:
      break;
    }

  return res;
}

/* Test if X is of the form reg[reg] or .xs reg = reg[reg] or signed10bits[reg]
 */
bool
kvx_has_10bit_imm_or_reg_p (rtx x)
{
  if (MEM_P (x))
    x = XEXP (x, 0);

  if (REG_P (x))
    return true;

  /*
   * ld reg = reg[reg]
   */
  if (GET_CODE (x) == PLUS && REG_P (XEXP (x, 0)) && REG_P (XEXP (x, 1)))
    return true;

  /*
   * ld.xs reg = reg[reg]
   */
  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 0)) == MULT
      && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT
      && INTVAL (XEXP (XEXP (x, 0), 1)) > HOST_WIDE_INT_1)
    return true;

  return kvx_has_10bit_immediate_p (x);
}

bool
kvx_has_10bit_immediate_p (rtx x)
{
  if (MEM_P (x))
    x = XEXP (x, 0);

  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 1)) == CONST_INT
      && REG_P (XEXP (x, 0)))
    return IN_RANGE (INTVAL (XEXP (x, 1)), -512, 511);

  return false;
}

bool
kvx_has_37bit_immediate_p (rtx x)
{
  if (MEM_P (x))
    x = XEXP (x, 0);

  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 1)) == CONST_INT
      && REG_P (XEXP (x, 0)))
    return !IN_RANGE (INTVAL (XEXP (x, 1)), -512, 511)
	   && IN_RANGE (INTVAL (XEXP (x, 1)), -(1LL << 36), (1LL << 36) - 1);

  return false;
}

bool
kvx_has_64bit_immediate_p (rtx x)
{
  if (MEM_P (x))
    x = XEXP (x, 0);

  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 1)) == CONST_INT
      && REG_P (XEXP (x, 0)))
    return !IN_RANGE (INTVAL (XEXP (x, 1)), -(1LL << 36), (1LL << 36) - 1);

  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 1)) != CONST_INT
      && REG_P (XEXP (x, 0)))
    return true;

  return false;
}

bool
kvx_has_27bit_immediate_p (rtx x)
{
  if (MEM_P (x))
    x = XEXP (x, 0);

  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 1)) == CONST_INT
      && REG_P (XEXP (x, 0)))
    return IN_RANGE (INTVAL (XEXP (x, 1)), -(1LL << 26), (1LL << 26) - 1);

  return false;
}

bool
kvx_has_54bit_immediate_p (rtx x)
{
  if (MEM_P (x))
    x = XEXP (x, 0);

  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 1)) == CONST_INT
      && REG_P (XEXP (x, 0)))
    return !IN_RANGE (INTVAL (XEXP (x, 1)), -(1LL << 26), (1LL << 26) - 1)
	   && IN_RANGE (INTVAL (XEXP (x, 1)), -(1LL << 53), (1LL << 53) - 1);

  return false;
}

/* Test whether the memory operand X should be accessed cached or
   uncached (bypass or preload) based on its memory address space.  */
bool
kvx_is_uncached_mem_op_p (rtx x)
{
  gcc_assert (MEM_P (x));
  if (!MEM_P (x))
    return false;

  /* __convert[_no_sync] addr space should not come here. */
  gcc_assert (MEM_ADDR_SPACE (x) < KVX_ADDR_SPACE_CONVERT);

#if 1
  int addr_space = MEM_ADDR_SPACE (x);
  return addr_space == KVX_ADDR_SPACE_BYPASS
	 || addr_space == KVX_ADDR_SPACE_PRELOAD;
#else
  return MEM_ADDR_SPACE (x);
#endif
}

HOST_WIDE_INT
kvx_const_vector_value (rtx x, int slice)
{
  HOST_WIDE_INT value = 0;
  if (GET_CODE (x) == CONST_VECTOR)
    {
      machine_mode mode = GET_MODE (x);
      machine_mode inner_mode = GET_MODE_INNER (mode);
      int index = slice * (8 / GET_MODE_SIZE (inner_mode));
      if (inner_mode == QImode)
	{
	  HOST_WIDE_INT val_0 = INTVAL (CONST_VECTOR_ELT (x, index + 0));
	  HOST_WIDE_INT val_1 = INTVAL (CONST_VECTOR_ELT (x, index + 1));
	  HOST_WIDE_INT val_2 = INTVAL (CONST_VECTOR_ELT (x, index + 2));
	  HOST_WIDE_INT val_3 = INTVAL (CONST_VECTOR_ELT (x, index + 3));
	  HOST_WIDE_INT val_4 = INTVAL (CONST_VECTOR_ELT (x, index + 4));
	  HOST_WIDE_INT val_5 = INTVAL (CONST_VECTOR_ELT (x, index + 5));
	  HOST_WIDE_INT val_6 = INTVAL (CONST_VECTOR_ELT (x, index + 6));
	  HOST_WIDE_INT val_7 = INTVAL (CONST_VECTOR_ELT (x, index + 7));
	  value = (val_0 & 0xFF) | (val_1 & 0xFF) << 8 | (val_2 & 0xFF) << 16
		  | (val_3 & 0xFF) << 24 | (val_4 & 0xFF) << 32
		  | (val_5 & 0xFF) << 40 | (val_6 & 0xFF) << 48
		  | (val_7 & 0xFF) << 56;
	}
      else if (inner_mode == HImode)
	{
	  HOST_WIDE_INT val_0 = INTVAL (CONST_VECTOR_ELT (x, index + 0));
	  HOST_WIDE_INT val_1 = INTVAL (CONST_VECTOR_ELT (x, index + 1));
	  HOST_WIDE_INT val_2 = INTVAL (CONST_VECTOR_ELT (x, index + 2));
	  HOST_WIDE_INT val_3 = INTVAL (CONST_VECTOR_ELT (x, index + 3));
	  value = (val_0 & 0xFFFF) | (val_1 & 0xFFFF) << 16
		  | (val_2 & 0xFFFF) << 32 | (val_3 & 0xFFFF) << 48;
	}
      else if (inner_mode == SImode)
	{
	  HOST_WIDE_INT val_0 = INTVAL (CONST_VECTOR_ELT (x, index + 0));
	  HOST_WIDE_INT val_1 = INTVAL (CONST_VECTOR_ELT (x, index + 1));
	  value = (val_0 & 0xFFFFFFFF) | (val_1 & 0xFFFFFFFF) << 32;
	}
      else if (inner_mode == DImode)
	{
	  value = INTVAL (CONST_VECTOR_ELT (x, index + 0));
	}
      else if (inner_mode == HFmode)
	{
	  long val_0 = 0, val_1 = 0, val_2 = 0, val_3 = 0;
	  rtx elt_0 = CONST_VECTOR_ELT (x, index + 0);
	  rtx elt_1 = CONST_VECTOR_ELT (x, index + 1);
	  rtx elt_2 = CONST_VECTOR_ELT (x, index + 2);
	  rtx elt_3 = CONST_VECTOR_ELT (x, index + 3);
	  REAL_VALUE_TO_TARGET_SINGLE (*CONST_DOUBLE_REAL_VALUE (elt_0), val_0);
	  REAL_VALUE_TO_TARGET_SINGLE (*CONST_DOUBLE_REAL_VALUE (elt_1), val_1);
	  REAL_VALUE_TO_TARGET_SINGLE (*CONST_DOUBLE_REAL_VALUE (elt_2), val_2);
	  REAL_VALUE_TO_TARGET_SINGLE (*CONST_DOUBLE_REAL_VALUE (elt_3), val_3);
	  val_0 = kvx_float_to_half_as_int (val_0);
	  val_1 = kvx_float_to_half_as_int (val_1);
	  val_2 = kvx_float_to_half_as_int (val_2);
	  val_3 = kvx_float_to_half_as_int (val_3);
	  value = (val_0 & 0xFFFF) | (val_1 & 0xFFFF) << 16
		  | (val_2 & 0xFFFF) << 32 | (val_3 & 0xFFFF) << 48;
	}
      else if (inner_mode == SFmode)
	{
	  long val_0 = 0, val_1 = 0;
	  rtx elt_0 = CONST_VECTOR_ELT (x, index + 0);
	  rtx elt_1 = CONST_VECTOR_ELT (x, index + 1);
	  REAL_VALUE_TO_TARGET_SINGLE (*CONST_DOUBLE_REAL_VALUE (elt_0), val_0);
	  REAL_VALUE_TO_TARGET_SINGLE (*CONST_DOUBLE_REAL_VALUE (elt_1), val_1);
	  value = ((HOST_WIDE_INT) val_0 & 0xFFFFFFFF)
		  | ((HOST_WIDE_INT) val_1 & 0xFFFFFFFF) << 32;
	}
      else if (inner_mode == DFmode)
	{
	  long val[2] = {0, 0};
	  rtx elt_0 = CONST_VECTOR_ELT (x, index + 0);
	  REAL_VALUE_TO_TARGET_DOUBLE (*CONST_DOUBLE_REAL_VALUE (elt_0), val);
	  value = ((HOST_WIDE_INT) val[0] & 0xFFFFFFFF)
		  | ((HOST_WIDE_INT) val[1] & 0xFFFFFFFF) << 32;
	}
      else
	gcc_unreachable ();
      return value;
    }
  else
    gcc_unreachable ();
  return value;
}

bool
kvx_has_10bit_vector_const_p (rtx x)
{
  HOST_WIDE_INT value = kvx_const_vector_value (x, 0);
  return SIGNED_INT_FITS_N_BITS (value, 10);
}

bool
kvx_has_16bit_vector_const_p (rtx x)
{
  HOST_WIDE_INT value = kvx_const_vector_value (x, 0);
  return SIGNED_INT_FITS_N_BITS (value, 16);
}

bool
kvx_has_32bit_vector_const_p (rtx x)
{
  HOST_WIDE_INT value = kvx_const_vector_value (x, 0);
  return SIGNED_INT_FITS_N_BITS (value, 32);
}

bool
kvx_has_37bit_vector_const_p (rtx x)
{
  HOST_WIDE_INT value = kvx_const_vector_value (x, 0);
  return SIGNED_INT_FITS_N_BITS (value, 37);
}

bool
kvx_has_43bit_vector_const_p (rtx x)
{
  HOST_WIDE_INT value = kvx_const_vector_value (x, 0);
  return SIGNED_INT_FITS_N_BITS (value, 43);
}

bool
kvx_has_32x2bit_vector_const_p (rtx ARG_UNUSED (x))
{
  //HOST_WIDE_INT value = kvx_const_vector_value (x, 0);
  // Need the dual immediate syntax to be fixed in assembler.
  // return (value&0xFFFFFFFF) == ((value>>32)&0xFFFFFFFF);
  return false;
}

/* Helper function for kvx_expand_load_multiple and
   kvx_expand_store_multiple */
static bool
kvx_expand_load_store_multiple (rtx operands[], bool is_load)
{
  const int reg_op_idx = is_load ? 0 : 1;
  const int mem_op_idx = is_load ? 1 : 0;

  int count = INTVAL (operands[2]);
  int regno = REGNO (operands[reg_op_idx]);

  if (GET_CODE (operands[2]) != CONST_INT
      || GET_MODE (operands[reg_op_idx]) != DImode || (count != 2 && count != 4)
      || ((count == 2) && (regno & 1)) || ((count == 4) && (regno & 3))
      || !MEM_P (operands[mem_op_idx]) || !REG_P (operands[reg_op_idx])
      || (TARGET_STRICT_ALIGN
	  && ((int) MEM_ALIGN (operands[mem_op_idx]))
	       < (count * UNITS_PER_WORD))
      || MEM_VOLATILE_P (operands[mem_op_idx])
      || REGNO (operands[reg_op_idx]) > KV3_GPR_LAST_REGNO)
    return false;

  operands[3] = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc (count));

  rtx base, offset;
  if (!kvx_split_mem (XEXP (operands[mem_op_idx], 0), &base, &offset, false))
    return false;

  if (!REG_P (base))
    base = force_reg (Pmode, base);

  /* Add a PLUS so that we have a simpler match in load multiple patterns */
  XEXP (operands[mem_op_idx], 0) = gen_rtx_PLUS (Pmode, base, offset);

  for (int i = 0; i < count; i++)
    {
      rtx addr
	= adjust_address_nv (operands[mem_op_idx], DImode, i * UNITS_PER_WORD);

      /* Force a PLUS even for offset 0 so that we have a simpler
	 match in load multiple patterns */
      if (REG_P (XEXP (addr, 0)))
	XEXP (addr, 0) = gen_rtx_PLUS (Pmode, XEXP (addr, 0), const0_rtx);

      if (is_load)
	XVECEXP (operands[3], 0, i)
	  = gen_rtx_SET (gen_rtx_REG (DImode, regno + i), addr);
      else
	XVECEXP (operands[3], 0, i)
	  = gen_rtx_SET (addr, gen_rtx_REG (DImode, regno + i));
    }

  return true;
}

/* Expands a store multiple with operand 0 being the first destination
   address, operand 1 the first source register and operand 2 the
   number of consecutive stores to pack. */
bool
kvx_expand_store_multiple (rtx operands[])
{
  return kvx_expand_load_store_multiple (operands, false);
}

/* Expands a load multiple with operand 0 being the first destination
   register, operand 1 the first source address and operand 2 the
   number of consecutive loads to pack. */
bool
kvx_expand_load_multiple (rtx operands[])
{
  return kvx_expand_load_store_multiple (operands, true);
}

/*
 * When IS_LOAD is TRUE, returns TRUE if OP is a load multiple
 * operation and all mems have same address space ADDR_SPACE.
 * When IS_LOAD is FALSE, returns TRUE if OP is a store multiple
 * operation.
 * Returns FALSE otherwise.
 */
static bool
kvx_load_store_multiple_operation_p (rtx op, bool is_uncached, bool is_load)
{
  int count = XVECLEN (op, 0);

  /* Perform a quick check so we don't blow up below.  */
  if (count != 2 && count != 4)
    return 0;

  for (int i = 0; i < count; i++)
    {
      rtx set = XVECEXP (op, 0, i);
      if (GET_CODE (set) != SET)
	return false;

      rtx reg_part = is_load ? SET_DEST (set) : SET_SRC (set);
      rtx mem_part = is_load ? SET_SRC (set) : SET_DEST (set);

      if (!REG_P (reg_part) || !MEM_P (mem_part) || MEM_VOLATILE_P (mem_part))
	return false;

#if 1
      if (is_load && is_uncached != !!kvx_is_uncached_mem_op_p (mem_part))
#else
      if (addr_space < 0)
	addr_space = MEM_ADDR_SPACE (mem_part);

      if (is_load && addr_space != MEM_ADDR_SPACE (mem_part))
#endif
	return false;
    }

#if 0
  if (is_uncached && addr_space != KVX_ADDR_SPACE_BYPASS
      && addr_space != KVX_ADDR_SPACE_PRELOAD)
    return false;
  if (!is_uncached && addr_space != ADDR_SPACE_GENERIC
      && addr_space != KVX_ADDR_SPACE_SPECULATE)
    return false;

#endif
  rtx first_mem
    = is_load ? SET_SRC (XVECEXP (op, 0, 0)) : SET_DEST (XVECEXP (op, 0, 0));
  rtx first_reg
    = is_load ? SET_DEST (XVECEXP (op, 0, 0)) : SET_SRC (XVECEXP (op, 0, 0));

  if (TARGET_STRICT_ALIGN
      && ((int) MEM_ALIGN (first_mem)) < (count * UNITS_PER_WORD))
    return false;

  unsigned int dest_regno = REGNO (first_reg);

  /* register number must be correctly aligned */
  if (dest_regno < FIRST_PSEUDO_REGISTER && (dest_regno % count != 0))
    return false;

  HOST_WIDE_INT expected_offset = 0;
  rtx base;

  for (int i = 0; i < count; i++)
    {
      rtx elt = XVECEXP (op, 0, i);
      rtx base_cur, offset_cur;
      rtx mem_elt = is_load ? SET_SRC (elt) : SET_DEST (elt);
      rtx reg_elt = is_load ? SET_DEST (elt) : SET_SRC (elt);

      if (!kvx_split_mem (XEXP (mem_elt, 0), &base_cur, &offset_cur, false))
	return false;

      if (i == 0)
	{
	  expected_offset = INTVAL (offset_cur);
	  base = base_cur;
	}
      else
	{
	  expected_offset += UNITS_PER_WORD;
	}

      if (GET_MODE (reg_elt) != DImode || REGNO (reg_elt) != dest_regno + i
	  || GET_MODE (mem_elt) != DImode || !rtx_equal_p (base_cur, base)
	  || expected_offset != INTVAL (offset_cur))

	return false;
    }

  return true;
}

/*
 * Returns TRUE if OP is a load multiple operation and all its mems
 * address spaces are KVX_ADDR_SPACE_BYPASS if IS_UNCACHED is true.
 */
bool
kvx_load_multiple_operation_p (rtx op, bool is_uncached)
{
  return kvx_load_store_multiple_operation_p (op, is_uncached, true);
}

/*
 * Returns TRUE if OP is a store multiple operation.
 */
bool
kvx_store_multiple_operation_p (rtx op)
{
  return kvx_load_store_multiple_operation_p (op, false, false);
}

/* Used during CFA note fixups.  When a FRAME_RELATED_P insn is being
   moved around a CFA-defining insn, its CFA NOTE must be changed
   accordingly to use correct register instead of OLD_BASE.
*/
static void
kvx_swap_fp_sp_in_note (rtx note, rtx old_base)
{
  XEXP (note, 0) = copy_rtx (XEXP (note, 0));
  rtx note_pat = XEXP (note, 0);

  rtx new_base_reg = (REGNO (old_base) == REGNO (hard_frame_pointer_rtx))
		       ? stack_pointer_rtx
		       : hard_frame_pointer_rtx;
  rtx mem_dest = SET_DEST (note_pat);
  struct kvx_frame_info *frame = &cfun->machine->frame;

  if (frame->hard_frame_pointer_offset == 0)
    {
      if (GET_CODE (XEXP (mem_dest, 0)) == PLUS)
	XEXP (XEXP (mem_dest, 0), 0) = new_base_reg;
      else
	XEXP (mem_dest, 0) = new_base_reg;
    }
  else
    {
      HOST_WIDE_INT new_offset
	= (new_base_reg == hard_frame_pointer_rtx)
				   ? -frame->hard_frame_pointer_offset
				   : frame->hard_frame_pointer_offset;
      if (GET_CODE (XEXP (mem_dest, 0)) == PLUS)
	{
	  rtx plus = XEXP (mem_dest, 0);
	  HOST_WIDE_INT old_offset = INTVAL (XEXP (plus, 1));
	  new_offset += old_offset;
	}
      XEXP (mem_dest, 0) = gen_rtx_PLUS (Pmode, new_base_reg,
					 gen_rtx_CONST_INT (Pmode, new_offset));
    }
}

static void
kvx_fix_debug_for_bundle_1 (rtx_insn *start_insn, rtx_insn *stop_insn)
{
  /* Start from the end so that NOTEs will be added in the correct order. */
  rtx_insn *binsn = stop_insn;
  bool last_do = false;
  do
    {
      last_do = (binsn == start_insn);
      if (INSN_P (binsn) && RTX_FRAME_RELATED_P (binsn))
	{
	  bool handled = false;
	  for (rtx note = REG_NOTES (binsn); note; note = XEXP (note, 1))
	    {
	      switch (REG_NOTE_KIND (note))
		{
		case REG_CFA_DEF_CFA:
		case REG_CFA_ADJUST_CFA:
		case REG_CFA_REGISTER:
		case REG_CFA_RESTORE:
		case REG_CFA_OFFSET:
		  handled = true;
		  if (binsn != stop_insn)
		    {
		      /* Move note to last insn in bundle */
		      add_shallow_copy_of_reg_note (stop_insn, note);
		      remove_note (binsn, note);
		    }
		  break;

		case REG_CFA_EXPRESSION:
		case REG_CFA_SET_VDRAP:
		case REG_CFA_WINDOW_SAVE:
		case REG_CFA_FLUSH_QUEUE:
		  error ("Unexpected CFA notes found.");
		  break;

		default:
		  break;
		}
	    }

	  if (!handled)
	    {
	      /* This *must* be some mem write emitted by builtin_save_regs,
	       * or a bug */
	      add_reg_note (stop_insn, REG_CFA_OFFSET,
			    copy_rtx (PATTERN (binsn)));
	    }

	  RTX_FRAME_RELATED_P (binsn) = 0;
	  RTX_FRAME_RELATED_P (stop_insn) = 1;
	}

      binsn = PREV_INSN (binsn);
    }
  while (!last_do);
}

static unsigned
kvx_fix_debug_for_bundle_2 (rtx_insn *start_insn, rtx_insn *stop_insn,
			    unsigned cur_cfa_reg)
{
  rtx_insn *binsn = start_insn;
  bool last_do = false;
  do
    {
      last_do = (binsn == stop_insn);
      if (INSN_P (binsn) && RTX_FRAME_RELATED_P (binsn))
	{
	  for (rtx note = REG_NOTES (binsn); note; note = XEXP (note, 1))
	    {

	      switch (REG_NOTE_KIND (note))
		{
		  case REG_CFA_DEF_CFA: {
		    rtx pat = XEXP (note, 0);

		    /* (PLUS ( CFA_REG, OFFSET)) */
		    gcc_assert (GET_CODE (pat) == PLUS);
		    cur_cfa_reg = REGNO (XEXP (pat, 0));
		    break;
		  }
		  /* We only need to fixup register spill */
		  case REG_CFA_OFFSET: {
		    rtx pat = XEXP (note, 0);

		    gcc_assert (GET_CODE (pat) == SET);

		    rtx mem_dest = SET_DEST (pat);
		    rtx base_reg;
		    if (GET_CODE (XEXP (mem_dest, 0)) == PLUS)
		      {
			base_reg = XEXP (XEXP (mem_dest, 0), 0);
		      }
		    else
		      {
			base_reg = XEXP (mem_dest, 0);
		      }
		    gcc_assert (REG_P (base_reg));

		    if (REGNO (base_reg) != cur_cfa_reg)
		      {
			/* Most likely an insn was moved around and
			   its note has not been modified accordingly.
			   We need to rebuild an offset based on
			   current CFA.
			*/
			kvx_swap_fp_sp_in_note (note, base_reg);
		      }
		  }
		  break;

		default:
		  break;
		}
	    }
	}
      binsn = NEXT_INSN (binsn);
    }
  while (!last_do);
  return cur_cfa_reg;
}

/* Visit all bundles and force all debug insns after the last insn in
   the bundle. */
static void
kvx_fix_debug_for_bundles (void)
{
  unsigned cur_cfa_reg = REGNO (stack_pointer_rtx);
  rtx_insn *start_insn = 0, *stop_insn = 0;
  basic_block bb;
  FOR_EACH_BB_FN (bb, cfun)
    {
      rtx_insn *insn;
      FOR_BB_INSNS (bb, insn)
	{
	  if (NONDEBUG_INSN_P (insn) && GET_CODE (PATTERN (insn)) != USE
	      && GET_CODE (PATTERN (insn)) != CLOBBER)
	    {
	      int uid = INSN_UID (insn);
	      if ((unsigned) uid >= (unsigned) kvx_sched2.max_uid
		  || kvx_sched2.insn_cycle[uid] < 0)
		{
		  if (!start_insn)
		    start_insn = stop_insn = insn;
		}
	      else
		{
		  unsigned flags = kvx_sched2.insn_flags[uid];
		  if (flags & KVX_SCHED2_INSN_HEAD)
		    cur_cfa_reg = REGNO (stack_pointer_rtx);
		  if (flags & KVX_SCHED2_INSN_START)
		    start_insn = insn;
		  if (flags & KVX_SCHED2_INSN_STOP)
		    stop_insn = insn;
		}
	      if (start_insn && stop_insn)
		{
		  kvx_fix_debug_for_bundle_1 (start_insn, stop_insn);
		  cur_cfa_reg
		    = kvx_fix_debug_for_bundle_2 (start_insn, stop_insn,
						  cur_cfa_reg);
		  start_insn = stop_insn = 0;
		}
	    }
	}
    }
  if (start_insn || stop_insn)
    gcc_assert (!start_insn && !stop_insn);
}

/* Fix the INSN issue cycle accounting for the KV3_1 stall effects of AUXR RAW.
   We maintain a scoreboard to compute the cumulative delay implied by stalls
   based on an array of last write dates.  */
static void
kvx_sched2_fix_insn_issue (rtx_insn *insn, rtx *opvec, int noperands)
{
  int uid = INSN_UID (insn);
  gcc_assert ((unsigned) uid < (unsigned) kvx_sched2.max_uid);

  // Scoreboard structure, initialized at each SCHE2 region.
  static struct
  {
    int delay;
    short write[KV3_GPR_LAST_REGNO + 1];
  } scoreboard;
  if (kvx_sched2.insn_flags[uid] & KVX_SCHED2_INSN_HEAD)
    {
      scoreboard.delay = 0;
      memset (scoreboard.write, -1, sizeof (scoreboard.write));
      gcc_assert (KV3_GPR_FIRST_REGNO == 0);
    }

  if (NONDEBUG_INSN_P (insn))
    {
      int stall = 0;
      int cycle = kvx_sched2.insn_cycle[uid] + scoreboard.delay;
      enum attr_type type = get_attr_type (insn);
      // Keep TYPE tests in sync with the order of the types.md file.
      if (type >= TYPE_MAU_AUXR && type <= TYPE_MAU_AUXR_FP16
	  && noperands > 3 && REG_P (opvec[3]))
	{
	  int regno = REGNO (opvec[3]);
	  int regno_quad = (regno & -4);
	  machine_mode mode = GET_MODE (opvec[3]);
	  unsigned mode_size = GET_MODE_SIZE (mode);
	  gcc_assert (regno <= KV3_GPR_LAST_REGNO);
	  if (mode_size <= UNITS_PER_WORD)
	    {
	      for (int i = 0; i < 4; i += 2)
		{
		  int j = (regno + i) & 3;
		  int write = scoreboard.write[regno_quad + j];
		  int delta = write - cycle;
		  if (stall < delta)
		    stall = delta;
		}
	    }
	  else if (mode_size >= 2 * UNITS_PER_WORD)
	    {
	      for (int i = 0; i < 4; i++)
		{
		  int write = scoreboard.write[regno_quad + i];
		  int delta = write - cycle;
		  if (stall < delta)
		    stall = delta;
		}
	    }
	}
      if (type >= TYPE_MAU && type < TYPE_BCU && noperands > 0
	  && REG_P (opvec[0]))
	{
	  int regno = REGNO (opvec[0]);
	  machine_mode mode = GET_MODE (opvec[0]);
	  int cost = FLOAT_MODE_P (mode) ? 4 : (INTEGRAL_MODE_P (mode) ? 3 : 1);
	  int i = hard_regno_nregs (regno, mode);
	  while (--i >= 0)
	    {
	      scoreboard.write[regno + i] = cycle + cost + stall;
	    }
	}

      if (stall)
	kvx_sched2.insn_flags[uid] |= KVX_SCHED2_INSN_STALL;
      kvx_sched2.insn_cycle[uid] = cycle + stall;
      scoreboard.delay += stall;
    }
}

/* Implements TARGET_ASM_FINAL_POSTSCAN_INSN.  */
static void
kvx_asm_final_postscan_insn (FILE *file, rtx_insn *insn,
			     rtx *opvec ATTRIBUTE_UNUSED,
			     int noperands ATTRIBUTE_UNUSED)
{
  if (kvx_sched2.insn_cycle)
    {
      int uid = INSN_UID (insn);
      if ((unsigned) uid >= (unsigned) kvx_sched2.max_uid
	  || kvx_sched2.insn_cycle[uid] < 0)
	{
	  if (TARGET_SCHED2_DATES)
	    fprintf (file, "\t;;\t# (unscheduled)\n");
	  else
	    fprintf (file, "\t;;\n");
	  return;
	}
      if (KV3_1)
	kvx_sched2_fix_insn_issue (insn, opvec, noperands);
      if (kvx_sched2.insn_flags[uid] & KVX_SCHED2_INSN_STOP)
	{
	  if (TARGET_SCHED2_DATES)
	    {
	      if (recog_memoized (insn) != CODE_FOR_doloop_end_si
		  && recog_memoized (insn) != CODE_FOR_doloop_end_di)
		{
		  const char *stalled = "";
		  if (kvx_sched2.insn_flags[uid] & KVX_SCHED2_INSN_STALL)
		    stalled = "(stalled)";
		  int cycle = kvx_sched2.insn_cycle[uid];
		  fprintf (file, "\t;;\t# (end cycle %d)%s\n", cycle, stalled);
		}
	    }
	  else
	    fprintf (file, "\t;;\n");
	  return;
	}
    }
  if (!kvx_sched2.insn_cycle || !kvx_scheduling)
    {
      fprintf (file, "\t;;\n");
      return;
    }
}

/* Compute the cost of an addressing mode that contains X.  */
static int
kvx_address_cost (rtx x, machine_mode mode ATTRIBUTE_UNUSED,
		  addr_space_t space ATTRIBUTE_UNUSED,
		  bool speed ATTRIBUTE_UNUSED)
{
  // RISC-style offset[reg] addressing mode, default cost is zero.
  int cost = 0;

  // Case of .xs reg[reg] addressing mode.
  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 0)) == MULT
      && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT)
    cost = 1;

  // Case of reg[reg] addressing mode.
  else if (GET_CODE (x) == PLUS && REG_P (XEXP (x, 0)) && REG_P (XEXP (x, 1)))
    cost = 2;

  // Case of 0[reg] addressing mode, prefer reg[reg] if possible.
  else if (REG_P (x) || SUBREG_P (x))
    cost = 3;

  if (DUMP_COSTS)
    {
      fprintf (stderr,
	       "kvx_address_cost(mode=%s, space=%d, speed=%d, init=%d)\n",
	       GET_MODE_NAME (mode), space, speed, !cfun->decl);
      debug_rtx (x);
      fprintf (stderr, "\tcost=%d\n\n", cost);
    }

  return cost;
}

static int kvx_cost_factor = 0;
//#define COST_FACTOR(n) (kvx_cost_factor ? (n + kvx_cost_factor - 1)/kvx_cost_factor : n)
#define COST_FACTOR(n) (((n) + 1) >> 1)

static int
kvx_type_all_cost (int nunits, int penalty, bool speed)
{
  int factor = 4 * nunits;
  return speed ? COSTS_N_INSNS (factor) + penalty : COSTS_N_INSNS (4);
}

static int
kvx_type_tiny_cost (int nunits, int penalty, bool speed)
{
  int factor = COST_FACTOR(1) * nunits;
  return speed ? COSTS_N_INSNS (factor) + penalty : COSTS_N_INSNS (1);
}

static int
kvx_type_thin_cost (int nunits, int penalty, bool speed)
{
  int factor = COST_FACTOR(2) * nunits;
  return speed ? COSTS_N_INSNS (factor) + penalty : COSTS_N_INSNS (1);
}

static int
kvx_type_lite_cost (int nunits, int penalty, bool speed)
{
  int factor = COST_FACTOR(2) * nunits;
  return speed ? COSTS_N_INSNS (factor) + penalty : COSTS_N_INSNS (1);
}

static int
kvx_type_full_cost (int nunits, int penalty, bool speed)
{
  int factor = COST_FACTOR(4) * nunits;
  return speed ? COSTS_N_INSNS (factor) + penalty : COSTS_N_INSNS (1);
}

static int
kvx_type_lsu_cost (int nunits, int penalty, bool speed)
{
  int factor = COST_FACTOR(4) * nunits;
  return speed ? COSTS_N_INSNS (factor) + penalty : COSTS_N_INSNS (1);
}

static int
kvx_type_mau_cost (int nunits, int penalty, bool speed)
{
  int factor = COST_FACTOR(4) * nunits;
  return speed ? COSTS_N_INSNS (factor) + penalty : COSTS_N_INSNS (1);
}

static int
kvx_type_bcu_cost (int nunits, int penalty, bool speed)
{
  int factor = COST_FACTOR(4) * nunits;
  return speed ? COSTS_N_INSNS (factor) + penalty : COSTS_N_INSNS (1);
}

static int
kvx_type_tca_cost (int nunits, int penalty, bool speed)
{
  int factor = COST_FACTOR(4) * nunits;
  return speed ? COSTS_N_INSNS (factor) + penalty : COSTS_N_INSNS (1);
}

/* Compute a (partial) cost for rtx X.  Return true if the complete
   cost has been computed, and false if subexpressions should be
   scanned.  In either case, *TOTAL contains the cost result.
   The initial value of *TOTAL is the default value computed by
   (rtx_cost).  It may be left unmodified.  OUTER_CODE contains the
   code of the superexpression of x.  */
static bool
kvx_rtx_costs (rtx x, machine_mode mode, int outer_code,
	       int opno , int *total, bool speed)
{
  int latency = 1;
  bool float_mode_p = FLOAT_MODE_P (mode);
  unsigned mode_size = GET_MODE_SIZE (mode);
  int lsucount = mode_size ? (mode_size + 31) / 32 : 1;
  int nwords = (mode_size + UNITS_PER_WORD - 1) / UNITS_PER_WORD;
  nwords = nwords >= 1 ? nwords : 1;

  if (DUMP_COSTS)
    {
      fprintf (stderr, "kvx_rtx_costs(mode=%s, code=%s, opno=%d speed=%d, init=%d)\n",
	       GET_MODE_NAME (mode), GET_RTX_NAME (outer_code), opno, speed,
	       !cfun->decl);
      debug_rtx (x);
    }

  // By default there is no added cost.
  *total = 0;

  switch (GET_CODE (x))
    {
    // RTX_CONST_OBJ:
    case CONST_INT:
      {
	HOST_WIDE_INT intval = INTVAL (x);
	if (SIGNED_INT_FITS_N_BITS (intval, 10))
	  // Assume 10-bit immediates are available at no cost.
	  *total = 0;
	else if (SIGNED_INT_FITS_N_BITS (intval, 32))
	  *total = speed ? 1 : 4;
	else
	  *total = speed ? 2 : 8;
      }
      if (outer_code == SET)
	{
	  *total += kvx_type_tiny_cost (nwords, 0, speed);
	}
      goto end_recurse;

    case CONST_WIDE_INT:
    case CONST_DOUBLE:
    case CONST_VECTOR:
    case CONST:
    case LABEL_REF:
    case SYMBOL_REF:
      *total = speed ? 2 * nwords : 8 * nwords;
      if (outer_code == SET)
	{
	  *total += *total = kvx_type_tiny_cost (nwords, 0, speed);
	}
      goto end_recurse;

    // RTX_OBJ:
    case REG:
    case SCRATCH:
      goto end_recurse;

    case MEM:
      latency = opno ? 3 : 1;
      *total = kvx_type_lsu_cost (lsucount, (latency - 1), speed);
      goto end_recurse;

    // RTX_COMPARE:
    // RTX_COMM_COMPARE:
    case GE:
    case GT:
    case LE:
    case LT:
    case GEU:
    case GTU:
    case LEU:
    case LTU:
    case UNGE:
    case UNGT:
    case UNLE:
    case UNLT:
    case NE:
    case EQ:
    case UNORDERED:
    case ORDERED:
    case UNEQ:
    case LTGT:
      if (outer_code != IF_THEN_ELSE)
	{
	  // COMP* and FCOMP* instructions.
	  latency = 2;
	  *total = float_mode_p
		     ? kvx_type_lite_cost (nwords, (latency - 1), speed)
		     : kvx_type_tiny_cost (nwords, (latency - 1), speed);
	}
      // Recurse for immediates.
      break;

    // RTX_UNARY:
    case NEG:
    case NOT:
    case SIGN_EXTEND:
    case ZERO_EXTEND:
      if (outer_code == SET)
	{
	  enum rtx_code inner_code = GET_CODE (XEXP (x, 0));
	  enum rtx_class inner_class = GET_RTX_CLASS (inner_code);
	  if (inner_code ==  SUBREG || inner_class == RTX_OBJ
	      || (!float_mode_p && inner_class == RTX_UNARY))
	    // Stand-alone NEG / NOT / SIGN_EXTEND / ZERO_EXTEND.
	    {
	      *total = kvx_type_tiny_cost (nwords, 0, speed);
	      goto end_recurse;
	    }
	}
    case TRUNCATE:
      // Recurse for inner arithmetic.
      break;

    case FLOAT_EXTEND:
    case FLOAT_TRUNCATE:
    case FIX:
    case FLOAT:
    case UNSIGNED_FIX:
    case UNSIGNED_FLOAT:
    case ABS:
    case SQRT:
    case BSWAP:
    case FFS:
    case CLRSB:
    case CLZ:
    case CTZ:
    case POPCOUNT:
    case PARITY:
    case VEC_DUPLICATE:
    case SS_NEG:
    case US_NEG:
    case SS_ABS:
    case SS_TRUNCATE:
    case US_TRUNCATE:
      *total = float_mode_p ? kvx_type_lite_cost (nwords, 0, speed)
			    : kvx_type_tiny_cost (nwords, 0, speed);
      goto end_recurse;

    // RTX_COMM_ARITH:
    // RTX_BIN_ARITH:
    case PLUS:
    case MINUS:
      latency = 2 + float_mode_p * 2;
      *total = float_mode_p ? kvx_type_mau_cost (nwords, (latency - 1), speed)
			    : kvx_type_tiny_cost (nwords, 0, speed);
      // Recurse for immediates or for inner MULT or SHIFT.
      break;

    case MULT:
    case SS_MULT:
    case US_MULT:
      latency = 2 + float_mode_p * 2;
      *total = kvx_type_mau_cost (nwords, (latency - 1), speed);
      if (!float_mode_p && (outer_code == PLUS || outer_code == MINUS))
	{
	  // MADD*, MSBF* instructions, subtract cost of PLUS / MINUS.
	  *total -= kvx_type_tiny_cost (nwords, 0, speed);
	  gcc_checking_assert (*total > 0);
	}
      goto end_recurse;

    case AND:
    case IOR:
    case XOR:
    case SMIN:
    case SMAX:
    case UMIN:
    case UMAX:
    case SS_PLUS:
    case US_PLUS:
      *total = float_mode_p ? kvx_type_lite_cost (nwords, 0, speed)
			    : kvx_type_tiny_cost (nwords, 0, speed);
      // Recurse for immediates.
      break;

    case COMPARE:
      // Unused.
      break;

    case DIV:
    case SS_DIV:
    case US_DIV:
    case MOD:
    case UDIV:
    case UMOD:
      if (CONST_INT_P (XEXP (x, 1))
	  && __builtin_popcount (INTVAL (XEXP (x, 1))) == 1)
	{
	  // Integer divide by a power of 2.
	  *total = kvx_type_tiny_cost (nwords, 0, speed);
	  goto end_recurse;
	}
      *total = kvx_type_all_cost (8, 0, speed);
      goto end_recurse;

    case ASHIFT:
    case ROTATE:
    case ASHIFTRT:
    case LSHIFTRT:
    case ROTATERT:
      if (outer_code != PLUS && outer_code != MINUS)
	{
	  // ADDX* and SBFX* instructions.
	  *total = kvx_type_tiny_cost (nwords, 0, speed);
	}
      break;

    case VEC_SELECT:
    case VEC_CONCAT:
    case VEC_SERIES:
    case SS_MINUS:
    case SS_ASHIFT:
    case US_ASHIFT:
    case US_MINUS:
      *total = kvx_type_lite_cost (nwords, 0, speed);
      break;

    // RTX_TERNARY:
    case IF_THEN_ELSE:
      if (mode == VOIDmode)
	{
	  *total = kvx_type_bcu_cost (1, 0, speed);
	}
      else
	{
	  *total = kvx_type_lite_cost (nwords, 0, speed);
	}
      goto end_recurse;

    case VEC_MERGE:
      break;

    case FMA:
      latency = 2 + float_mode_p * 2;
      *total = kvx_type_mau_cost (nwords, (latency - 1), speed);
      goto end_recurse;

    // RTX_BITFIELD_OPS:
    case SIGN_EXTRACT:
    case ZERO_EXTRACT:
      if (outer_code == SET)
	*total = kvx_type_lite_cost (nwords, 0, speed);
      goto end_recurse;

    // RTX_EXTRA:
    case SET:
      if (REG_SUBREG_P (SET_SRC (x)) && REG_SUBREG_P (SET_DEST (x)))
	{
	  // Set word MOVE cost to 2, add 1 per extra word.
	  *total = 1 + nwords;
	  goto end_recurse;
	}
      break;

    case UNSPEC:
      switch (XINT (x, 1))
	{
	case UNSPEC_SBMM8:
	case UNSPEC_SBMMT8:
	case UNSPEC_SBMM8S:
	case UNSPEC_SBMM8XY:
	case UNSPEC_SRS:
	  *total = kvx_type_tiny_cost (nwords, 0, speed);
	  goto end_recurse;
	default:
	  *total = kvx_type_lite_cost (nwords, 0, speed);
	  break;
	}
      break;

    case UNSPEC_VOLATILE:
      *total = kvx_type_all_cost (nwords, 0, speed);
      goto end_recurse;

    case PREFETCH:
      *total = kvx_type_lsu_cost (1, 0, speed);
      goto end_recurse;

    case CALL:
    case RETURN:
      *total = kvx_type_bcu_cost (1, 0, speed);
      goto end_recurse;

    default:
      break;
    }

  if (DUMP_COSTS)
    fprintf (stderr, "\tcost=%d (inner)\n\n", *total);
  return false;

end_recurse:
  if (DUMP_COSTS)
    fprintf (stderr, "\tcost=%d (leaf)\n\n", *total);
  return true;
}

/* Compute a full rtx cost using the same logic as the generic (rtx_cost).
   Used to observe the full effects of recursively calling (kvx_rtx_costs).  */
static int
kvx_full_rtx_cost (rtx x, machine_mode mode, enum rtx_code outer_code,
		   int opno, bool speed)
{
  int total = 0;
  if (x == 0)
    return 0;

  if (GET_MODE (x) != VOIDmode)
    mode = GET_MODE (x);

  enum rtx_code code = GET_CODE (x);
  if (code == REG)
    return 0;

  // No TARGETS_MODES_TIEABLE_P.
  if (code != SUBREG && code != TRUNCATE)
    if (kvx_rtx_costs (x, mode, outer_code, opno, &total, speed))
      return total;

  const char *fmt = GET_RTX_FORMAT (code);
  for (int i = GET_RTX_LENGTH (code) - 1; i >= 0; i--)
    if (fmt[i] == 'e')
      total += kvx_full_rtx_cost (XEXP (x, i), mode, code, i, speed);
    else if (fmt[i] == 'E')
      for (int j = 0; j < XVECLEN (x, i); j++)
	total += kvx_full_rtx_cost (XVECEXP (x, i, j), mode, code, i, speed);

  return total;
}

/* Describes the relative cost of RTL instructions.
   /!\ Synchronize with the types.md type and latencies.  */
static int
kvx_insn_cost (rtx_insn *insn, bool speed)
{
  if (recog_memoized (insn) < 0)
    {
      if (DUMP_COSTS)
	{
	  fprintf (stderr, "kvx_insn_cost(uid=%d, speed=%d, init=%d)\n",
		   INSN_UID (insn), speed, !cfun->decl);
	  debug_rtx (PATTERN (insn));
	  fprintf (stderr, "\tcost=0 (not memoized)\n\n");
	}
      return 0;
    }

  enum attr_type type = get_attr_type (insn);
  if (type == TYPE_ALU_NOP)
    return 0;

  if (!speed)
    return get_attr_length (insn);

  int cost = 0;
  if (type == TYPE_ALL)
    {
      cost = kvx_type_all_cost (1, 0, speed);
    }
  else if (type >= TYPE_ALU_TINY && type < TYPE_ALU_THIN)
    {
      int nunits = 1;
      if (type >= TYPE_ALU_TINY_X2 && type <= TYPE_ALU_TINY_X2_Y)
	nunits = 2;
      if (type >= TYPE_ALU_TINY_X4 && type <= TYPE_ALU_TINY_X4_X)
	nunits = 4;
      cost += kvx_type_tiny_cost (nunits, 0, speed);
    }
  else if (type >= TYPE_ALU_THIN && type < TYPE_ALU_LITE)
    {
      int nunits = 1;
      if (type >= TYPE_ALU_THIN_X2 && type < TYPE_ALU_LITE)
	nunits = 2;
      cost += kvx_type_thin_cost (nunits, 0, speed);
    }
  else if (type >= TYPE_ALU_LITE && type < TYPE_ALU_FULL)
    {
      int nunits = 1;
      if (type >= TYPE_ALU_LITE_X2 && type < TYPE_ALU_FULL)
	nunits = 2;
      cost += kvx_type_lite_cost (nunits, 0, speed);
    }
  else if (type >= TYPE_ALU_FULL && type < TYPE_LSU)
    {
      int penalty = (type == TYPE_ALU_FULL_COPRO) * (15 - 1);
      cost += kvx_type_full_cost (1, penalty, speed);
    }
  else if (type >= TYPE_LSU && type < TYPE_MAU)
    {
      int penalty = 0;
      if (type >= TYPE_LSU_LOAD && type <= TYPE_LSU_AUXW_LOAD_Y)
	penalty = (3 - 1);
      if ((type >= TYPE_LSU_AUXW_LOAD_UNCACHED
	   && type <= TYPE_LSU_LOAD_UNCACHED_Y)
	  || (type >= TYPE_LSU_AUXW_ATOMIC
	      && type <= TYPE_LSU_AUXR_AUXW_ATOMIC_Y))
	penalty = (24 - 1);
      cost += kvx_type_lsu_cost (1, penalty, speed);
    }
  else if (type >= TYPE_MAU && type < TYPE_BCU)
    {
      int penalty = (2 - 1);
      if (type == TYPE_MAU_FPU || type == TYPE_MAU_AUXR_FPU)
	penalty = (4 - 1);
      else if (type == TYPE_MAU_FP16 || type == TYPE_MAU_AUXR_FP16)
	penalty = (3 - 1);
      cost += kvx_type_mau_cost (1, penalty, speed);
    }
  else if (type >= TYPE_BCU && type < TYPE_TCA)
    {
      cost += kvx_type_bcu_cost (1, 0, speed);
    }
  else if (type >= TYPE_TCA && type <= TYPE_TCA_FLOAT)
    {
      cost += kvx_type_tca_cost (1, 3, speed);
    }
  else
    gcc_unreachable ();

  if (DUMP_COSTS)
    {
      fprintf (stderr, "kvx_insn_cost(uid=%d, speed=%d, init=%d)\n",
	       INSN_UID (insn), speed, !cfun->decl);
      debug_rtx (PATTERN (insn));
      int rtx_cost = kvx_full_rtx_cost (PATTERN (insn), VOIDmode, INSN, 3, speed);
      fprintf (stderr, "\tcost=%d\trtx_cost=%d%s\n\n", cost, rtx_cost,
	       (rtx_cost > cost || rtx_cost == 0) ? "\tFIXME!" : "");
    }

  return cost;
}

/* Implements TARGET_REGISTER_MOVE_COST.  (Used by register allocation.)  */
static int
kvx_register_move_cost (machine_mode mode, reg_class_t from ATTRIBUTE_UNUSED,
			reg_class_t to ATTRIBUTE_UNUSED)
{
  int nwords = (GET_MODE_SIZE (mode) + UNITS_PER_WORD - 1) / UNITS_PER_WORD;
  if (kvx_extension_mode_p (mode))
    nwords = GET_MODE_NUNITS (mode);
  nwords = nwords >= 1 ? nwords : 1;

  // Set word MOVE cost to 2, add 1 per extra word.
  int cost = 1 + nwords;

  if (DUMP_COSTS)
    {
      fprintf (stderr,
	       "kvx_register_move_cost(mode=%s, from=%s, to=%s, init=%d)\n",
	       GET_MODE_NAME (mode), reg_class_names[from], reg_class_names[to],
	       !cfun->decl);
      fprintf (stderr, "\tcost=%d\n\n", cost);
    }

  return cost;
}

/* Implements TARGET_MEMORY_MOVE_COST. (Used by register allocation.)  */
static int
kvx_memory_move_cost (machine_mode mode, reg_class_t rclass ATTRIBUTE_UNUSED,
		      bool in)
{
  // Assume in-cache load latency is 3 cycles.
  int penalty = in ? (3 - 1) : 0;
  unsigned mode_size = GET_MODE_SIZE (mode);
  int lsucount = mode_size ? (mode_size + 31) / 32 : 1;
  int cost = kvx_type_lsu_cost (lsucount, penalty, true);

  if (DUMP_COSTS)
    {
      fprintf (stderr,
	       "kvx_memory_move_cost(mode=%s, rclass=%s, in=%d, init=%d)\n",
	       GET_MODE_NAME (mode), reg_class_names[rclass], in, !cfun->decl);
      fprintf (stderr, "\tcost=%d\n\n", cost);
    }

  gcc_assert (cost > 0);
  return cost;
}

/* Implements TARGET_COMPUTE_PRESSURE_CLASSES.  */
static int
kvx_compute_pressure_classes (reg_class *classes)
{
  // Register pressure matters in GPR_REGS and XCR_REGS.
  int n = 0;
  classes[n++] = GPR_REGS;
  classes[n++] = XCR_REGS;
  return n;
}

static bool
kvx_function_ok_for_sibcall (tree decl, tree exp ATTRIBUTE_UNUSED)
{
  /* Sometimes GCC allocates a call_used_register to hold the sibcall
     destination... This of course  won't work because we can't restore that
     register to the right value and do the call (In fact we could by bundling
     the igoto and the restore together).
     FIXME: I think we can avoid this by defining sibcall and
     sibcall_value expanders that will use scratch registers
     instead of any GPR. */

  if (!decl)
    return false;

  /* Do not tail-call to farcall, there are cases where our prologue
   * overwrite the target register
   */
  tree attrs = TYPE_ATTRIBUTES (TREE_TYPE (decl));
  if (KVX_FARCALL || lookup_attribute ("farcall", attrs))
    return false;

  /* Do not tail-call calls to weak symbol
   * Same reason as for farcall
   */
  if (DECL_WEAK (decl))
    {
      return false;
    }

  return true;
}

static bool
kvx_legitimate_constant_p (enum machine_mode mode ATTRIBUTE_UNUSED, rtx x)
{
  if (kvx_has_tls_reference (x))
    return false;

  if (kvx_has_unspec_reference (x))
    {
      if (GET_CODE (x) == CONST)
	x = XEXP (x, 0);

      if (GET_CODE (x) == UNSPEC)
	return true;

      if (GET_CODE (x) == PLUS || GET_CODE (x) == MINUS)
	return GET_CODE (XEXP (x, 0)) == UNSPEC && CONST_INT_P (XEXP (x, 1));

      return false;
    }

  return true;
}

static rtx
kvx_legitimize_address (rtx x, rtx oldx ATTRIBUTE_UNUSED,
			enum machine_mode mode)
{
  if (kvx_has_tls_reference (x))
    return kvx_legitimize_tls_reference (x);
  else if (GET_CODE (x) == PLUS
	   && (GET_CODE (XEXP (x, 0)) == MULT
	       || GET_CODE (XEXP (x, 0)) == ZERO_EXTEND))
    {
      rtx reg = gen_reg_rtx (Pmode);
      rtx cst = XEXP (x, 1);

      XEXP (x, 1) = reg;
      if (kvx_legitimate_address_p (mode, x, false))
	{
	  emit_move_insn (reg, cst);
	  return copy_rtx (x);
	}
      else
	{
	  XEXP (x, 1) = cst;
	  return x;
	}
    }
  return x;
}

/* Implements TARGET_ADDR_SPACE_POINTER_MODE.  */
static scalar_int_mode
kvx_addr_space_pointer_mode (addr_space_t address_space ATTRIBUTE_UNUSED)
{
  return ptr_mode;
}

/* Implements TARGET_ADDR_SPACE_ADDRESS_MODE.  */
static scalar_int_mode
kvx_addr_space_address_mode (addr_space_t address_space ATTRIBUTE_UNUSED)
{
  return Pmode;
}

/* Implements TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P.  */
static bool
kvx_addr_space_legitimate_address_p (machine_mode mode, rtx exp, bool strict,
				     addr_space_t as ATTRIBUTE_UNUSED)
{
  switch (as)
    {
    default:
      gcc_unreachable ();

    case ADDR_SPACE_GENERIC:
    case KVX_ADDR_SPACE_BYPASS:
    case KVX_ADDR_SPACE_PRELOAD:
    case KVX_ADDR_SPACE_SPECULATE:
      return kvx_legitimate_address_p (mode, exp, strict);

    case KVX_ADDR_SPACE_CONVERT:
      return false;
    }
}

/* Implements TARGET_ADDR_SPACE_LEGITIMIZE_ADDRESS.  */
static rtx
kvx_addr_space_legitimize_address (rtx x, rtx oldx, machine_mode mode,
				   addr_space_t as)
{
  if (as == KVX_ADDR_SPACE_CONVERT)
    error ("__convert should be used only in explicit pointer casting");

  return kvx_legitimize_address (x, oldx, mode);
}

/* Implements TARGET_ADDR_SPACE_SUBSET_P.  */
static bool
kvx_addr_space_subset_p (addr_space_t subset ATTRIBUTE_UNUSED,
			 addr_space_t superset ATTRIBUTE_UNUSED)
{
  // Address spaces (GENERIC or __UNCACHED) refer to the same space
  return true;
}

/* Implements TARGET_ADDR_SPACE_CONVERT.  */
static rtx
kvx_addr_space_convert (rtx op, tree from_type, tree to_type ATTRIBUTE_UNUSED)
{
  if (KVX_WARN_ADDRESS_SPACE_CONVERSION
      && TYPE_ADDR_SPACE (TREE_TYPE (from_type)) != KVX_ADDR_SPACE_CONVERT
      && TYPE_ADDR_SPACE (TREE_TYPE (to_type)) != KVX_ADDR_SPACE_CONVERT)
    {

      warning (0,
	       TYPE_ADDR_SPACE (TREE_TYPE (from_type)) > ADDR_SPACE_GENERIC
		 ? "Implicit conversion from uncached pointer to cached one"
		 : "Implicit conversion from cached pointer to uncached one");
      inform (input_location,
	      "Use (__convert <type> *) to acknowledge this conversion");
    }
  return op;
}

static void
kvx_function_prologue (FILE *file ATTRIBUTE_UNUSED)
{
}

/* Implements TARGET_ASM_FUNCTION_EPILOGUE.  */
static void
kvx_function_epilogue (FILE *file ATTRIBUTE_UNUSED)
{
  kvx_sched2_dtor ();
}

/* NULL if INSN insn is valid within a low-overhead loop.
   Otherwise return why doloop cannot be applied.  */

static const char *
kvx_invalid_within_doloop (const rtx_insn *insn)
{
  if (CALL_P (insn))
    return "Function call in the loop.";

  if (JUMP_TABLE_DATA_P (insn))
    return "Computed branch in the loop.";

  if (!INSN_P (insn))
    return NULL;

  rtx body = PATTERN (insn);
  rtx asm_ops = extract_asm_operands (body);

  if (asm_ops && GET_CODE (body) == PARALLEL)
    {
      int i, regno, nparallel = XVECLEN (body, 0);

      for (i = 0; i < nparallel; i++)
	{
	  rtx clobber = XVECEXP (body, 0, i);
	  if (GET_CODE (clobber) == CLOBBER && REG_P (XEXP (clobber, 0))
	      && (regno = REGNO (XEXP (clobber, 0)))
	      && (regno == KV3_LC_REGNO || regno == KV3_LS_REGNO
		  || regno == KV3_LE_REGNO))
	    return "HW Loop register clobbered by asm.";
	}
    }

  return NULL;
}

/* A callback for the hw-doloop pass.  Called when a candidate doloop turns out
   not to be optimizable. The doloop_end pattern must be split into a decrement
   of the loop counter and looping branch if not zero, assuming without reload.
   In case of loop counter reload the doloop_end pattern was already split.  */

static void
kvx_hwloop_fail (hwloop_info loop)
{
  if (recog_memoized (loop->loop_end) != CODE_FOR_doloop_end_si
      && recog_memoized (loop->loop_end) != CODE_FOR_doloop_end_di)
    return;

  rtx (*gen_add) (rtx, rtx, rtx);
  rtx (*gen_cbranch) (rtx, rtx, rtx, rtx);
  machine_mode mode = GET_MODE (loop->iter_reg);
  gen_add = (mode == SImode) ? gen_addsi3 : gen_adddi3;
  gen_cbranch = (mode == SImode) ? gen_cbranchsi4 : gen_cbranchdi4;

  rtx decr = gen_add (loop->iter_reg, loop->iter_reg, constm1_rtx);
  emit_insn_before (decr, loop->loop_end);

  rtx test = gen_rtx_NE (VOIDmode, NULL_RTX, NULL_RTX);
  rtx cbranch
    = gen_cbranch (test, loop->iter_reg, const0_rtx, loop->start_label);
  rtx_insn *jump_insn = emit_jump_insn_before (cbranch, loop->loop_end);

  JUMP_LABEL (jump_insn) = loop->start_label;
  LABEL_NUSES (loop->start_label)++;

  delete_insn (loop->loop_end);
}

/* A callback for the hw-doloop pass.  This function examines INSN; if
   it is a doloop_end pattern we recognize, return the reg rtx for the
   loop counter.  Otherwise, return NULL_RTX.  */

static rtx
kvx_hwloop_pattern_reg (rtx_insn *insn)
{
  if (!JUMP_P (insn)
      || (recog_memoized (insn) != CODE_FOR_doloop_end_si
	  && recog_memoized (insn) != CODE_FOR_doloop_end_di))
    return NULL_RTX;

  rtx reg = SET_DEST (XVECEXP (PATTERN (insn), 0, 1));
  if (!REG_P (reg))
    return NULL_RTX;

  return reg;
}

static bool
kvx_hwloop_optimize (hwloop_info loop)
{
  int i;
  edge entry_edge;
  rtx iter_reg;
  rtx_insn *insn;
  rtx_insn *seq, *entry_after;

  if (loop->jumps_within)
    {
      if (dump_file)
	fprintf (dump_file, ";; loop %d jumps within\n", loop->loop_no);
      return false;
    }

  if (loop->jumps_outof)
    {
      if (dump_file)
	fprintf (dump_file, ";; loop %d has early exit\n", loop->loop_no);
      return false;
    }

  if (!loop->incoming_dest)
    {
      if (dump_file)
	fprintf (dump_file, ";; loop %d has more than one entry\n",
		 loop->loop_no);
      return false;
    }

  if (loop->incoming_dest != loop->head)
    {
      if (dump_file)
	fprintf (dump_file, ";; loop %d is not entered from head\n",
		 loop->loop_no);
      return false;
    }

  if (loop->blocks.length () > 1)
    {
      if (dump_file)
	fprintf (dump_file, ";; loop %d has more than one basic block\n",
		 loop->loop_no);
      return false;
    }

  if (loop->iter_reg_used || loop->iter_reg_used_outside)
    {
      if (dump_file)
	fprintf (dump_file, ";; loop %d uses iterator register\n",
		 loop->loop_no);
      return false;
    }

  /* Check if start_label appears before doloop_end.  */
  insn = loop->start_label;
  while (insn && insn != loop->loop_end)
    insn = NEXT_INSN (insn);

  if (!insn)
    {
      if (dump_file)
	fprintf (dump_file, ";; loop %d start_label not before loop_end\n",
		 loop->loop_no);
      return false;
    }

  /* Get the loop iteration register.  */
  iter_reg = loop->iter_reg;

  gcc_assert (REG_P (iter_reg));

  entry_edge = NULL;

  FOR_EACH_VEC_SAFE_ELT (loop->incoming, i, entry_edge)
    if (entry_edge->flags & EDGE_FALLTHRU)
      break;

  if (entry_edge == NULL)
    return false;

  loop->end_label = block_label (loop->successor);

  start_sequence ();
  insn = emit_insn (gen_kvx_loopdo (loop->iter_reg, loop->end_label));
  seq = get_insns ();

  /* Place the zero_cost_loop_start instruction before the loop.  */
  basic_block entry_bb = entry_edge->src;
  if (!single_succ_p (entry_bb) || vec_safe_length (loop->incoming) > 1)
    {
      basic_block new_bb;
      edge e;
      edge_iterator ei;

      emit_insn_before (seq, BB_HEAD (loop->head));
      seq = emit_label_before (gen_label_rtx (), seq);
      new_bb = create_basic_block (seq, insn, entry_bb);
      FOR_EACH_EDGE (e, ei, loop->incoming)
	{
	  if (!(e->flags & EDGE_FALLTHRU))
	    redirect_edge_and_branch_force (e, new_bb);
	  else
	    redirect_edge_succ (e, new_bb);
	}

      make_edge (new_bb, loop->head, 0);
    }
  else if (entry_bb != ENTRY_BLOCK_PTR_FOR_FN (cfun))
    {
      entry_after = BB_END (entry_bb);
      emit_insn_after (seq, entry_after);
    }
  else
    {
      // Loop is at the very beginning of function
      insert_insn_on_edge (seq,
			   single_succ_edge (ENTRY_BLOCK_PTR_FOR_FN (cfun)));
      commit_one_edge_insertion (
	single_succ_edge (ENTRY_BLOCK_PTR_FOR_FN (cfun)));
    }

  end_sequence ();

  return true;
}

static struct hw_doloop_hooks kvx_doloop_hooks
  = { kvx_hwloop_pattern_reg, kvx_hwloop_optimize, kvx_hwloop_fail };

/* Implements TARGET_MACHINE_DEPENDENT_REORG.  */
static void
kvx_machine_dependent_reorg (void)
{
  compute_bb_for_insn ();

  /* If optimizing, we'll have split before scheduling.  */
  if (optimize == 0)
    split_all_insns ();

  /* Doloop optimization. */
  if (optimize)
    reorg_loops (true, &kvx_doloop_hooks);

  df_analyze ();

  if (optimize && flag_schedule_insns_after_reload)
    {
      timevar_push (TV_SCHED2);

      if (flag_selective_scheduling2 && !maybe_skip_selective_scheduling ())
	run_selective_scheduling ();
      else
	schedule_ebbs ();

      timevar_pop (TV_SCHED2);
    }

  if (kvx_scheduling)
    kvx_fix_debug_for_bundles ();

  /* This is needed. Else final pass will crash on debug_insn-s */
  if (flag_var_tracking)
    {
      compute_bb_for_insn ();
      timevar_push (TV_VAR_TRACKING);
      variable_tracking_main ();
      timevar_pop (TV_VAR_TRACKING);
      free_bb_for_insn ();
    }

  df_finish_pass (false);
}

static bool
kvx_handle_fixed_reg_option (const char *arg)
{
  const char *dash;
  int first_reg = -1, last_reg = -1, i;

  if (*arg++ != '=')
    return false;

  dash = strchr (arg, '-');

  for (i = 0; i < FIRST_PSEUDO_REGISTER; ++i)
    if (strncmp (reg_names[i], arg, dash - arg) == 0)
      {
	first_reg = i;
	break;
      }

  if (first_reg < 0)
    {
      error ("Unknown register %s passed to -ffixed-reg.", arg);
      return false;
    }

  if (dash)
    {
      for (i = 0; i < FIRST_PSEUDO_REGISTER; ++i)
	if (strcmp (reg_names[i], dash + 1) == 0)
	  {
	    last_reg = i;
	    break;
	  }
      if (last_reg < 0)
	{
	  error ("Unknown register %s passed to -ffixed-reg.", dash + 1);
	  return false;
	}
    }
  else
    {
      last_reg = first_reg;
    }

  if (first_reg > last_reg)
    {
      error ("Invalid range passed to -ffixed-reg.");
      return false;
    }

  for (i = first_reg; i <= last_reg; ++i)
    {
      fix_register (reg_names[i], 1, 1);
    }

  return true;
}

static bool
kvx_handle_stack_limit_register_option (const char *arg)
{
  if (kvx_have_stack_checking ())
    {
      int reg = decode_reg_name (arg);
      if (reg < 0)
        error ("unrecognized register name %qs", arg);
      /* Only allow $sr as stack-limit register */
      else if (strncmp (arg, "sr", 2))
        {
          error ("only $sr can be used as stack-limit register");
        }
      else
        {
          /* Deactivate previous OPT_fstack_limit_symbol_ options.  */
          opt_fstack_limit_symbol_arg = NULL;
          opt_fstack_limit_register_no = reg;
        }

      return true;
    }
  else
    {
      error ("-fstack-limit-* is not supported.");
    }

  return false;
}

/* Implements TARGET_OPTION_OVERRIDE.  */
static void
kvx_option_override (void)
{
  int i;
  cl_deferred_option *opt;
  vec < cl_deferred_option > *v =
    (vec < cl_deferred_option > *)kvx_deferred_options;

  if (v)
    FOR_EACH_VEC_ELT (*v, i, opt)
    {
      switch (opt->opt_index)
        {
        case OPT_ffixed_reg:
          kvx_handle_fixed_reg_option (opt->arg);
          break;
        case OPT_fstack_limit_register_:
          kvx_handle_stack_limit_register_option (opt->arg);
          break;
        default:
          gcc_unreachable ();
        }
    }

  kvx_arch_schedule = ARCH_KV3_1;
  if (KV3_2)
    kvx_arch_schedule = ARCH_KV3_2;

  const char *KVX_COST_FACTOR = getenv ("KVX_COST_FACTOR");
  if (KVX_COST_FACTOR)
    kvx_cost_factor = atoi (KVX_COST_FACTOR);
}

/* Recognize machine-specific patterns that may appear within
   constants.  Used for PIC-specific UNSPECs.
   Previously a macro. */
static bool
kvx_output_addr_const_extra (FILE *fp, rtx x)
{
  if (GET_CODE (x) == UNSPEC)
    {
      int unspec_code = XINT ((x), 1);
      switch (unspec_code)
	{
	case UNSPEC_PIC:
	  /* GLOBAL_OFFSET_TABLE or local symbols, no suffix.  */
	  output_addr_const ((fp), XVECEXP ((x), 0, 0));
	  return true;

	case UNSPEC_GOT:
	  fputs ("@got", (fp));
	  fputs ("(", (fp));
	  output_addr_const ((fp), XVECEXP ((x), 0, 0));
	  fputs (")", (fp));
	  return true;

	case UNSPEC_PCREL:
	  fputs ("@pcrel(", (fp));
	  output_addr_const ((fp), XVECEXP ((x), 0, 0));
	  fputs (")", (fp));
	  return true;

	case UNSPEC_GOTOFF:
	  fputs ("@gotoff", (fp));
	  fputs ("(", (fp));
	  output_addr_const ((fp), XVECEXP ((x), 0, 0));
	  fputs (")", (fp));
	  return true;

	case UNSPEC_TLS_GD:
	case UNSPEC_TLS_LD:
	case UNSPEC_TLS_DTPOFF:
	case UNSPEC_TLS_LE:
	case UNSPEC_TLS_IE:
	  fputs (kvx_unspec_tls_asm_op[unspec_code - UNSPEC_TLS_GD], (fp));
	  fputs ("(", (fp));
	  output_addr_const ((fp), XVECEXP ((x), 0, 0));
	  fputs (")", (fp));
	  return true;

	default:
	  return false;
	}
    }
  return false;
}

/* FIXME AUTO: This must be fixed for coolidge */
/* See T7749 */
static int
kvx_reassociation_width (unsigned int opc, machine_mode mode) ATTRIBUTE_UNUSED;

static int
kvx_reassociation_width (unsigned int opc, machine_mode mode)
{
  int res = 1;

  /* see tree.c:associative_tree_code () for possible values of opc. */

  switch (opc)
    {
    case BIT_IOR_EXPR:
    case BIT_AND_EXPR:
    case BIT_XOR_EXPR:
      if (mode == SImode || mode == HImode || mode == QImode || mode == DImode)
	res = 4;
      else if (mode == TImode)
	res = 2;
      break;
    case PLUS_EXPR:
    case MIN_EXPR:
    case MAX_EXPR:
      if (mode == SImode || mode == HImode || mode == QImode || mode == DImode)
	res = 4;
      break;
    case MULT_EXPR:
      break;
    }

  return res;
}

bool
kvx_float_fits_bits (const REAL_VALUE_TYPE *r, unsigned bitsz,
		     enum machine_mode mode)
{
  if (bitsz >= 64)
    return true;

  long l[2];
  unsigned long long value = 0;
  if (mode == HFmode)
    {
      REAL_VALUE_TO_TARGET_SINGLE (*r, l[0]);
      value = kvx_float_to_half_as_int (l[0]);
    }
  else if (mode == SFmode)
    {
      REAL_VALUE_TO_TARGET_SINGLE (*r, l[0]);
      value = l[0] & 0xFFFFFFFFULL;
    }
  else if (mode == DFmode)
    {
      REAL_VALUE_TO_TARGET_DOUBLE (*r, l);
      value = (l[0] & 0xFFFFFFFFULL) | ((unsigned long long) l[1] << 32);
    }
  else
    gcc_unreachable ();

  return SIGNED_INT_FITS_N_BITS (value, bitsz);
}

/* Returns TRUE if OP is a hard (sub)register misaligned on ALIGN.  */
bool
kvx_hardreg_misaligned_p (rtx op, int align)
{
  if (REG_P (op))
    {
      if (!HARD_REGISTER_P (op))
	return false;

      if (REGNO (op) % align == 0)
	return false;

      return true;
    }
  else if (SUBREG_P (op) && REG_P (SUBREG_REG (op)))
    {
      if (!HARD_REGISTER_P (SUBREG_REG (op)))
	return false;

      unsigned wordno = SUBREG_BYTE (op) / UNITS_PER_WORD;
      if ((REGNO (SUBREG_REG (op)) + wordno) % align == 0)
	return false;

      return true;
    }

  return false;
}

/* Split a 128-bit register move into 64-bit moves.  */
void
kvx_split_128bits_move (rtx dst, rtx src)
{
  enum machine_mode mode = GET_MODE (dst);
  rtx dst_0 = simplify_gen_subreg (DImode, dst, mode, 0);
  rtx dst_1 = simplify_gen_subreg (DImode, dst, mode, 8);
  rtx src_0 = simplify_gen_subreg (DImode, src, mode, 0);
  rtx src_1 = simplify_gen_subreg (DImode, src, mode, 8);
  if (!reg_overlap_mentioned_p (dst, src)
      || reg_or_subregno (dst) < reg_or_subregno (src))
    {
      emit_insn (gen_movdi (dst_0, src_0));
      emit_insn (gen_movdi (dst_1, src_1));
    }
  else
    {
      emit_insn (gen_movdi (dst_1, src_1));
      emit_insn (gen_movdi (dst_0, src_0));
    }
}

/* Split a 256-bit register move into 64-bit moves.  */
void
kvx_split_256bits_move (rtx dst, rtx src)
{
  enum machine_mode mode = GET_MODE (dst);
  rtx dst_0 = simplify_gen_subreg (DImode, dst, mode, 0);
  rtx dst_1 = simplify_gen_subreg (DImode, dst, mode, 8);
  rtx dst_2 = simplify_gen_subreg (DImode, dst, mode, 16);
  rtx dst_3 = simplify_gen_subreg (DImode, dst, mode, 24);
  rtx src_0 = simplify_gen_subreg (DImode, src, mode, 0);
  rtx src_1 = simplify_gen_subreg (DImode, src, mode, 8);
  rtx src_2 = simplify_gen_subreg (DImode, src, mode, 16);
  rtx src_3 = simplify_gen_subreg (DImode, src, mode, 24);
  if (!reg_overlap_mentioned_p (dst, src)
      || reg_or_subregno (dst) < reg_or_subregno (src))
    {
      emit_insn (gen_movdi (dst_0, src_0));
      emit_insn (gen_movdi (dst_1, src_1));
      emit_insn (gen_movdi (dst_2, src_2));
      emit_insn (gen_movdi (dst_3, src_3));
    }
  else
    {
      emit_insn (gen_movdi (dst_3, src_3));
      emit_insn (gen_movdi (dst_2, src_2));
      emit_insn (gen_movdi (dst_1, src_1));
      emit_insn (gen_movdi (dst_0, src_0));
    }
}

/* Make a 128bit constant from SRC into DST in DI chunks.  */
void
kvx_make_128bit_const (rtx dst, rtx src)
{
  int nunits = 0;
  HOST_WIDE_INT value_0 = 0, value_1 = 0;

  switch (GET_CODE (src))
    {
    case CONST_INT:
      value_0 = INTVAL (src);
      value_1 = value_0 < 0 ? HOST_WIDE_INT_M1 : HOST_WIDE_INT_0;
      break;
    case CONST_WIDE_INT:
      nunits = CONST_WIDE_INT_NUNITS (src);
      if (nunits > 0)
	value_0 = CONST_WIDE_INT_ELT (src, 0);
      if (nunits > 1)
	value_1 = CONST_WIDE_INT_ELT (src, 1);
      else
	value_1 = value_0 < 0 ? HOST_WIDE_INT_M1 : HOST_WIDE_INT_0;
      break;
    case CONST_VECTOR:
      value_0 = kvx_const_vector_value (src, 0);
      value_1 = kvx_const_vector_value (src, 1);
      break;
    default:
      gcc_unreachable ();
    }

  enum machine_mode mode = GET_MODE (dst);
  rtx dst_0 = simplify_gen_subreg (DImode, dst, mode, 0);
  rtx dst_1 = simplify_gen_subreg (DImode, dst, mode, 8);

  emit_insn (gen_movdi (dst_0, GEN_INT (value_0)));
  emit_insn (gen_movdi (dst_1, GEN_INT (value_1)));
}

/* Make a 256bit constant from SRC into DST in DI chunks.  */
void
kvx_make_256bit_const (rtx dst, rtx src)
{
  int nunits = 0;
  HOST_WIDE_INT value_0 = 0, value_1 = 0, value_2 = 0, value_3 = 0;

  switch (GET_CODE (src))
    {
    case CONST_INT:
      value_0 = INTVAL (src);
      value_1 = value_0 < 0 ? HOST_WIDE_INT_M1 : HOST_WIDE_INT_0;
      value_2 = value_1 < 0 ? HOST_WIDE_INT_M1 : HOST_WIDE_INT_0;
      value_3 = value_2 < 0 ? HOST_WIDE_INT_M1 : HOST_WIDE_INT_0;
      break;
    case CONST_WIDE_INT:
      nunits = CONST_WIDE_INT_NUNITS (src);
      if (nunits > 0)
	value_0 = CONST_WIDE_INT_ELT (src, 0);
      if (nunits > 1)
	value_1 = CONST_WIDE_INT_ELT (src, 1);
      else
	value_1 = value_0 < 0 ? HOST_WIDE_INT_M1 : HOST_WIDE_INT_0;
      if (nunits > 2)
	value_2 = CONST_WIDE_INT_ELT (src, 2);
      else
	value_2 = value_1 < 0 ? HOST_WIDE_INT_M1 : HOST_WIDE_INT_0;
      if (nunits > 3)
	value_3 = CONST_WIDE_INT_ELT (src, 3);
      else
	value_3 = value_2 < 0 ? HOST_WIDE_INT_M1 : HOST_WIDE_INT_0;
      break;
    case CONST_VECTOR:
      value_0 = kvx_const_vector_value (src, 0);
      value_1 = kvx_const_vector_value (src, 1);
      value_2 = kvx_const_vector_value (src, 2);
      value_3 = kvx_const_vector_value (src, 3);
      break;
    default:
      gcc_unreachable ();
    }

  enum machine_mode mode = GET_MODE (dst);
  rtx dst_0 = simplify_gen_subreg (DImode, dst, mode, 0);
  rtx dst_1 = simplify_gen_subreg (DImode, dst, mode, 8);
  rtx dst_2 = simplify_gen_subreg (DImode, dst, mode, 16);
  rtx dst_3 = simplify_gen_subreg (DImode, dst, mode, 24);

  emit_insn (gen_movdi (dst_0, GEN_INT (value_0)));
  emit_insn (gen_movdi (dst_1, GEN_INT (value_1)));
  emit_insn (gen_movdi (dst_2, GEN_INT (value_2)));
  emit_insn (gen_movdi (dst_3, GEN_INT (value_3)));
}

/* Returns TRUE if OP is a symbol and has the farcall attribute or if
   -mfarcall is in use. */
bool
kvx_is_farcall_p (rtx op)
{
  bool farcall = KVX_FARCALL;
  if (!farcall
      && (GET_CODE (XEXP (op, 0)) == SYMBOL_REF
	  && SYMBOL_REF_FUNCTION_P (XEXP (op, 0))
	  && SYMBOL_REF_DECL (XEXP (op, 0)) != NULL_TREE))
    farcall
      = lookup_attribute ("farcall",
			  DECL_ATTRIBUTES (SYMBOL_REF_DECL (XEXP (op, 0))))
	!= NULL;
  return farcall;
}

#ifdef GCC_KVX_MPPA_LINUX
void
kvx_output_function_profiler (FILE *file)
{
  int temp_reg = REGNO (kvx_get_callersaved_nonfixed_reg (Pmode, 2));
  fprintf (file, "\n\tget $r%d = $ra", PROFILE_REGNO);
  if (KVX_FARCALL)
    {
      fprintf (file, "\n\tmake $r%d = __mcount\n\t;;\n\t", temp_reg);
      fprintf (file, "\n\ticall $r%d\n\t;;\n\t", temp_reg);
    }
  else
    {
      fputs ("\n\t;;\n\tcall __mcount\n\t;;\n\t", file);
    }
}
#else
void
kvx_profile_hook (void)
{
  rtx ra_arg = get_hard_reg_initial_val (Pmode, KV3_RETURN_POINTER_REGNO);
  rtx fun = gen_rtx_SYMBOL_REF (Pmode, "__mcount");
  emit_library_call (fun, LCT_NORMAL, VOIDmode, ra_arg, Pmode);
}
#endif

static HOST_WIDE_INT
kvx_constant_alignment (const_tree exp, HOST_WIDE_INT align)
{
  return ((TREE_CODE (exp) == STRING_CST
	   && !optimize_size
	   && (align) < BITS_PER_WORD )
	  ? BITS_PER_WORD : (align));
}

/* Returns asm template for ctrapsi4 */
char *
kvx_ctrapsi4 (void)
{
  static char asm_template[] = "cb.@%R0z %1? 1f\n\t"
			       ";;\n\t"
			       "get $r0 = $pc\n\t"
			       "copyd $r1 = $r12\n\t"
			       ";;\n\t"
			       "call __stack_overflow_detected\n\t"
			       ";;\n\t"
			       "1:\n\t";
  char *width = strchr (asm_template, '@');

  if (width)
    *width = TARGET_32 ? 'w' : 'd';
  return asm_template;
}


/* Initialize the GCC target structure.  */

#undef TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE kvx_option_override

#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE kvx_function_value

#undef TARGET_OMIT_STRUCT_RETURN_REG
#define TARGET_OMIT_STRUCT_RETURN_REG true

#undef TARGET_RETURN_IN_MSB
#define TARGET_RETURN_IN_MSB kvx_return_in_msb

#undef TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY kvx_return_in_memory

#undef TARGET_STRUCT_VALUE_RTX
#define TARGET_STRUCT_VALUE_RTX kvx_struct_value_rtx

#undef TARGET_ASM_OUTPUT_MI_THUNK
#define TARGET_ASM_OUTPUT_MI_THUNK kvx_asm_output_mi_thunk

#undef TARGET_ASM_CAN_OUTPUT_MI_THUNK
#define TARGET_ASM_CAN_OUTPUT_MI_THUNK kvx_asm_can_output_mi_thunk

#undef TARGET_ASM_ALIGNED_DI_OP
#define TARGET_ASM_ALIGNED_DI_OP "\t.8byte\t"

#undef TARGET_EXPAND_BUILTIN_SAVEREGS
#define TARGET_EXPAND_BUILTIN_SAVEREGS kvx_expand_builtin_saveregs

#undef TARGET_EXPAND_BUILTIN_VA_START
#define TARGET_EXPAND_BUILTIN_VA_START kvx_expand_va_start

#undef TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P kvx_legitimate_address_p

#undef TARGET_FIXED_POINT_SUPPORTED_P
#define TARGET_FIXED_POINT_SUPPORTED_P kvx_fixed_point_supported_p

#undef TARGET_SCALAR_MODE_SUPPORTED_P
#define TARGET_SCALAR_MODE_SUPPORTED_P kvx_scalar_mode_supported_p

#undef TARGET_VECTOR_MODE_SUPPORTED_P
#define TARGET_VECTOR_MODE_SUPPORTED_P kvx_vector_mode_supported_p

#undef TARGET_LIBGCC_FLOATING_MODE_SUPPORTED_P
#define TARGET_LIBGCC_FLOATING_MODE_SUPPORTED_P                                \
  kvx_libgcc_floating_mode_supported_p

#undef TARGET_MANGLE_TYPE
#define TARGET_MANGLE_TYPE kvx_mangle_type

#undef TARGET_C_EXCESS_PRECISION
#define TARGET_C_EXCESS_PRECISION kvx_excess_precision

#undef TARGET_VECTORIZE_SUPPORT_VECTOR_MISALIGNMENT
#define TARGET_VECTORIZE_SUPPORT_VECTOR_MISALIGNMENT                           \
  kvx_support_vector_misalignment

#undef TARGET_VECTORIZE_PREFERRED_SIMD_MODE
#define TARGET_VECTORIZE_PREFERRED_SIMD_MODE kvx_vectorize_preferred_simd_mode

#undef TARGET_PROMOTE_FUNCTION_MODE
#define TARGET_PROMOTE_FUNCTION_MODE kvx_promote_function_mode

#undef TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES hook_bool_const_tree_true

#undef TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES kvx_arg_partial_bytes

#undef TARGET_MUST_PASS_IN_STACK
#define TARGET_MUST_PASS_IN_STACK must_pass_in_stack_var_size

#undef TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE kvx_pass_by_reference

#undef TARGET_INIT_BUILTINS
#define TARGET_INIT_BUILTINS kvx_init_builtins

#undef TARGET_EXPAND_BUILTIN
#define TARGET_EXPAND_BUILTIN kvx_expand_builtin

#undef TARGET_RESOLVE_OVERLOADED_BUILTIN
#define TARGET_RESOLVE_OVERLOADED_BUILTIN kvx_resolve_overloaded_builtin

#undef TARGET_CANNOT_FORCE_CONST_MEM
#define TARGET_CANNOT_FORCE_CONST_MEM kvx_cannot_force_const_mem

#undef TARGET_ADDRESS_COST
#define TARGET_ADDRESS_COST kvx_address_cost

#undef TARGET_RTX_COSTS
#define TARGET_RTX_COSTS kvx_rtx_costs

#undef TARGET_INSN_COST
#define TARGET_INSN_COST kvx_insn_cost

#undef TARGET_REGISTER_MOVE_COST
#define TARGET_REGISTER_MOVE_COST kvx_register_move_cost

#undef TARGET_MEMORY_MOVE_COST
#define TARGET_MEMORY_MOVE_COST kvx_memory_move_cost

#undef TARGET_COMPUTE_PRESSURE_CLASSES
#define TARGET_COMPUTE_PRESSURE_CLASSES kvx_compute_pressure_classes

#undef TARGET_SCHED_ISSUE_RATE
#define TARGET_SCHED_ISSUE_RATE kvx_sched_issue_rate

#undef TARGET_SCHED_VARIABLE_ISSUE
#define TARGET_SCHED_VARIABLE_ISSUE kvx_sched_variable_issue

#undef TARGET_SCHED_ADJUST_COST
#define TARGET_SCHED_ADJUST_COST kvx_sched_adjust_cost

#undef TARGET_SCHED_ADJUST_PRIORITY
#define TARGET_SCHED_ADJUST_PRIORITY kvx_sched_adjust_priority

#undef TARGET_SCHED_DEPENDENCIES_EVALUATION_HOOK
#define TARGET_SCHED_DEPENDENCIES_EVALUATION_HOOK                              \
  kvx_sched_dependencies_evaluation_hook

#undef TARGET_SCHED_INIT
#define TARGET_SCHED_INIT kvx_sched_init

#undef TARGET_SCHED_FINISH
#define TARGET_SCHED_FINISH kvx_sched_finish

#undef TARGET_SCHED_INIT_GLOBAL
#define TARGET_SCHED_INIT_GLOBAL kvx_sched_init_global

#undef TARGET_SCHED_FINISH_GLOBAL
#define TARGET_SCHED_FINISH_GLOBAL kvx_sched_finish_global

#undef TARGET_SCHED_DFA_NEW_CYCLE
#define TARGET_SCHED_DFA_NEW_CYCLE kvx_sched_dfa_new_cycle

#undef TARGET_SCHED_SET_SCHED_FLAGS
#define TARGET_SCHED_SET_SCHED_FLAGS kvx_sched_set_sched_flags

#undef TARGET_SCHED_CAN_SPECULATE_INSN
#define TARGET_SCHED_CAN_SPECULATE_INSN kvx_sched_can_speculate_insn

#undef TARGET_SCHED_SMS_RES_MII
#define TARGET_SCHED_SMS_RES_MII kvx_sched_sms_res_mii

#undef TARGET_SCHED_EXPOSED_PIPELINE
#define TARGET_SCHED_EXPOSED_PIPELINE true

#undef TARGET_SCHED_REASSOCIATION_WIDTH
#define TARGET_SCHED_REASSOCIATION_WIDTH kvx_sched_reassociation_width

#undef TARGET_FUNCTION_OK_FOR_SIBCALL
#define TARGET_FUNCTION_OK_FOR_SIBCALL kvx_function_ok_for_sibcall

#undef TARGET_BUILTIN_DECL
#define TARGET_BUILTIN_DECL kvx_builtin_decl

#undef TARGET_LEGITIMIZE_ADDRESS
#define TARGET_LEGITIMIZE_ADDRESS kvx_legitimize_address

#undef TARGET_CONST_ANCHOR
#define TARGET_CONST_ANCHOR 0x200

#undef TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE kvx_function_prologue

#undef TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE kvx_function_epilogue

#undef TARGET_ASM_FINAL_POSTSCAN_INSN
#define TARGET_ASM_FINAL_POSTSCAN_INSN kvx_asm_final_postscan_insn

#undef TARGET_MACHINE_DEPENDENT_REORG
#define TARGET_MACHINE_DEPENDENT_REORG kvx_machine_dependent_reorg

#undef TARGET_ATTRIBUTE_TABLE
#define TARGET_ATTRIBUTE_TABLE kvx_attribute_table

#undef TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG kvx_function_arg

#undef TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE kvx_function_arg_advance

#undef TARGET_LEGITIMATE_CONSTANT_P
#define TARGET_LEGITIMATE_CONSTANT_P kvx_legitimate_constant_p

#undef TARGET_CONDITIONAL_REGISTER_USAGE
#define TARGET_CONDITIONAL_REGISTER_USAGE kvx_conditional_register_usage

#undef TARGET_CANONICALIZE_COMPARISON
#define TARGET_CANONICALIZE_COMPARISON kvx_canonicalize_comparison

#undef TARGET_ASM_OUTPUT_ADDR_CONST_EXTRA
#define TARGET_ASM_OUTPUT_ADDR_CONST_EXTRA kvx_output_addr_const_extra

#undef TARGET_INVALID_WITHIN_DOLOOP
#define TARGET_INVALID_WITHIN_DOLOOP kvx_invalid_within_doloop

#undef TARGET_PRINT_OPERAND
#define TARGET_PRINT_OPERAND kvx_print_operand

#undef TARGET_PRINT_OPERAND_ADDRESS
#define TARGET_PRINT_OPERAND_ADDRESS kvx_print_operand_address

#undef TARGET_PRINT_OPERAND_PUNCT_VALID_P
#define TARGET_PRINT_OPERAND_PUNCT_VALID_P kvx_print_punct_valid_p

#undef TARGET_MODE_DEPENDENT_ADDRESS_P
#define TARGET_MODE_DEPENDENT_ADDRESS_P kvx_mode_dependent_address_p

#undef TARGET_CAN_USE_DOLOOP_P
#define TARGET_CAN_USE_DOLOOP_P can_use_doloop_if_innermost

#undef TARGET_ADDR_SPACE_POINTER_MODE
#define TARGET_ADDR_SPACE_POINTER_MODE kvx_addr_space_pointer_mode

#undef TARGET_ADDR_SPACE_ADDRESS_MODE
#define TARGET_ADDR_SPACE_ADDRESS_MODE kvx_addr_space_address_mode
#undef TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P
#define TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P                                 \
  kvx_addr_space_legitimate_address_p

#undef TARGET_ADDR_SPACE_LEGITIMIZE_ADDRESS
#define TARGET_ADDR_SPACE_LEGITIMIZE_ADDRESS kvx_addr_space_legitimize_address

#undef TARGET_ADDR_SPACE_SUBSET_P
#define TARGET_ADDR_SPACE_SUBSET_P kvx_addr_space_subset_p

#undef TARGET_ADDR_SPACE_CONVERT
#define TARGET_ADDR_SPACE_CONVERT kvx_addr_space_convert

#undef TARGET_CONSTANT_ALIGNMENT
#define TARGET_CONSTANT_ALIGNMENT kvx_constant_alignment

#undef TARGET_HARD_REGNO_NREGS
#define TARGET_HARD_REGNO_NREGS kvx_hard_regno_nregs

#undef TARGET_HARD_REGNO_MODE_OK
#define TARGET_HARD_REGNO_MODE_OK kvx_hard_regno_mode_ok

#undef TARGET_CLASS_MAX_NREGS
#define TARGET_CLASS_MAX_NREGS kvx_class_max_nregs

#undef TARGET_SECONDARY_RELOAD
#define TARGET_SECONDARY_RELOAD kvx_secondary_reload

#undef TARGET_STARTING_FRAME_OFFSET
#define TARGET_STARTING_FRAME_OFFSET kvx_starting_frame_offset

#undef TARGET_STATIC_CHAIN
#define TARGET_STATIC_CHAIN kvx_static_chain

#undef TARGET_TRAMPOLINE_INIT
#define TARGET_TRAMPOLINE_INIT kvx_trampoline_init

#undef TARGET_DELAY_SCHED2
#define TARGET_DELAY_SCHED2 (true)

#undef TARGET_DELAY_VARTRACK
#define TARGET_DELAY_VARTRACK (true)

#undef TARGET_VECTORIZE_VEC_PERM_CONST
#define TARGET_VECTORIZE_VEC_PERM_CONST kvx_vectorize_vec_perm_const

void kvx_init_builtins (void);
tree kvx_builtin_decl (unsigned code, bool initialize_p);
rtx kvx_expand_builtin (tree exp, rtx target, rtx subtarget,
			enum machine_mode mode, int ignore);
tree kvx_resolve_overloaded_builtin (location_t loc, tree ob_fndecl,
				     void *passed_arglist);

struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-kvx.h"
