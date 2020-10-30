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
#include "config.h"
#define INCLUDE_STRING
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "rtl-iter.h"
#include "tree.h"
#include "memmodel.h"
#include "gimple.h"
#include "cfghooks.h"
#include "cfgloop.h"
#include "df.h"
#include "tm_p.h"
#include "stringpool.h"
#include "optabs.h"
#include "regs.h"
#include "emit-rtl.h"
#include "recog.h"
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
#include "params.h"
#include "gimplify.h"
#include "dwarf2.h"
#include "gimple-iterator.h"
#include "tree-vectorizer.h"
#include "cfgrtl.h"

#include "dumpfile.h"
#include "builtins.h"
#include "tm-constrs.h"
#include "sched-int.h"
#include "target-globals.h"
#include "common/common-target.h"
#include "selftest.h"
#include "selftest-rtl.h"

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"

#include "calls.h"
#include "function.h"
#include "cfgloop.h"
#include "cppdefault.h"
#include "cpplib.h"
#include "diagnostic.h"
#include "incpath.h"

#include "basic-block.h"
#include "bitmap.h"
#include "hard-reg-set.h"
#include "df.h"
#include "tree.h"
#include "rtl.h"
#include "expr.h"
#include "function.h"
#include "ggc.h"
#include "insn-attr.h"
#include "insn-codes.h"
#include "insn-modes.h"
#include "langhooks.h"
#include "libfuncs.h"
#include "output.h"
#include "hw-doloop.h"
#include "opts.h"
#include "params.h"
#include "emit-rtl.h"
//#include "recog.h"
#include "regs.h"

#include "sched-int.h"
#include "toplev.h"
#include "stor-layout.h"
#include "varasm.h"

#include "tm_p.h"
#include "target.h"
#include "target-def.h"
#include "ira.h"

#include "kvx-protos.h"
#include "kvx-opts.h"

#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

#undef TARGET_HAVE_TLS
#define TARGET_HAVE_TLS (true)

static bool scheduling = false;

rtx kvx_sync_reg_rtx;
rtx kvx_link_reg_rtx;

typedef enum
{
  CLOBBER_INSERT,
  INSN_DELETE,
  REG_COPY_INSERT
} post_packing_action_type;
struct post_packing_action
{
  post_packing_action_type type;
  rtx insn;
  rtx reg, reg2;
  struct post_packing_action *next;
} * post_packing_action;


/* Which arch are we scheduling for */
enum attr_arch kvx_arch_schedule;

/* Information about a function's frame layout.  */
struct GTY (()) kvx_frame_info
{
  /* The total frame size, used for moving $sp in prologue */
  HOST_WIDE_INT frame_size;

  /* Offsets of save area from frame bottom */
  HOST_WIDE_INT saved_reg_sp_offset;

  /* Relative offsets within register save area  */
  HOST_WIDE_INT reg_rel_offset[FIRST_PSEUDO_REGISTER];
  /* Register save area size */
  HOST_WIDE_INT saved_regs_size;
  HARD_REG_SET saved_regs;

  /* Offset of virtual frame pointer from new stack pointer/frame bottom */
  HOST_WIDE_INT virt_frame_pointer_offset;

  /* Offset of hard frame pointer from new stack pointer/frame bottom */
  HOST_WIDE_INT hard_frame_pointer_offset;

  /* The offset of arg_pointer_rtx from the new stack pointer/frame bottom.  */
  HOST_WIDE_INT arg_pointer_offset;

  /* Offset to the static chain pointer, if needed */
  HOST_WIDE_INT static_chain_offset;

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

  /* If true, the current function has a STATIC_CHAIN.  */
  int static_chain_on_stack;

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
 Argument Pointer / Virt. FP--->| [Static chain]| [256-bits aligned]
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

  HOST_WIDE_INT inc_sp_offset = 0;

  frame = &cfun->machine->frame;
  memset (frame, 0, sizeof (*frame));
  CLEAR_HARD_REG_SET (frame->saved_regs);

  inc_sp_offset += crtl->args.pretend_args_size;

  /* If any anonymous arg may be in register, push them on the stack */
  if (cfun->stdarg && crtl->args.info.next_arg_reg < KV3_ARG_REG_SLOTS)
    inc_sp_offset
      += UNITS_PER_WORD * (KV3_ARG_REG_SLOTS - crtl->args.info.next_arg_reg);

  /* FIXME AUTO: trampoline are broken T6775 */
  if (cfun->machine->static_chain_on_stack)
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
      + (cfun->machine->static_chain_on_stack ? UNITS_PER_WORD : 0);

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
	   " |XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX| %d/0 (caller frame) <- $sp "
	   "(incoming) %s \n",
	   fi->frame_size,
	   cfun->stdarg && crtl->args.info.next_arg_reg < KV3_ARG_REG_SLOTS
	     ? ""
	     : "and virt frame pointer");

#define DFI_SEP fprintf (dump_file, " +------------------------------+    \n")

#define DFI_FIELD(f, sz, bottom, decorate_up, decorate_down)                   \
  fprintf (dump_file,                                                          \
	   " |                              | %d/%d %s  \n"                    \
	   " |%30s|                              \n"                           \
	   " |size: %24d| %d/%d  %s\n",                                        \
	   (bottom) + (sz) -UNITS_PER_WORD,                                    \
	   fi->frame_size - ((bottom) + (sz) -UNITS_PER_WORD), decorate_up, f, \
	   (sz), (bottom), fi->frame_size - (bottom), decorate_down)

  DFI_SEP;
  if (cfun->stdarg && crtl->args.info.next_arg_reg < KV3_ARG_REG_SLOTS)
    {
      DFI_FIELD ("varargs", fi->frame_size - fi->arg_pointer_offset,
		 fi->arg_pointer_offset, "", " <- arg pointer");
      DFI_SEP;
    }

  if (fi->padding1 > 0)
    {
      DFI_FIELD (
	"padding1", fi->padding1,
	fi->virt_frame_pointer_offset
	  + (cfun->machine->static_chain_on_stack ? UNITS_PER_WORD : 0),
	"",
	cfun->machine->static_chain_on_stack ? "" : "<- virt frame pointer");
      DFI_SEP;
    }
  if (cfun->machine->static_chain_on_stack)
    {
      DFI_FIELD ("static chain", UNITS_PER_WORD, fi->virt_frame_pointer_offset,
		 "", "<- virt frame pointer");
      DFI_SEP;
    }

  if (get_frame_size () > 0)
    {
      DFI_FIELD ("locals", get_frame_size (),
		 fi->virt_frame_pointer_offset - get_frame_size (), "", "");
      DFI_SEP;
    }
  if (fi->padding2 > 0)
    {
      DFI_FIELD ("padding2", fi->padding2,
		 fi->hard_frame_pointer_offset + fi->saved_regs_size, "", "");
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

      for (unsigned int i = (fi->saved_regs_size / UNITS_PER_WORD) - 1; i != 0;
	   i--)
	fprintf (dump_file, " |%30s| %d/-\n", reg_names[stacked_regs[i]],
		 fi->hard_frame_pointer_offset
		   + fi->reg_rel_offset[stacked_regs[i]]);

      fprintf (dump_file, " |%21s (%d)%4s| %d/- %s\n", "saved regs",
	       fi->saved_regs_size, reg_names[stacked_regs[0]],
	       fi->hard_frame_pointer_offset,
	       frame_pointer_needed ? "<- hard frame pointer ($fp)" : "");

      DFI_SEP;
    }
  if (fi->padding3 > 0)
    {
      if (crtl->outgoing_args_size > 0)
	{
	  DFI_FIELD ("padding3", fi->padding3,
		     crtl->outgoing_args_size + fi->padding3, "", "");
	  DFI_SEP;
	}
      else
	{
	  DFI_FIELD ("padding3", fi->padding3, 0, "", "<- $sp (callee)");
	  DFI_SEP;
	}
    }
  if (crtl->outgoing_args_size > 0)
    {
      DFI_FIELD ("outgoing", crtl->outgoing_args_size, 0, "",
		 "<- $sp (callee)");
      DFI_SEP;
    }

  fprintf (dump_file, "Saved regs: ");
  unsigned regno;
  hard_reg_set_iterator rsi;
  EXECUTE_IF_SET_IN_HARD_REG_SET (fi->saved_regs, 0, regno, rsi)
  fprintf (dump_file, " $%s [%d]", reg_names[regno], fi->reg_rel_offset[regno]);

  fprintf (dump_file, "\n");

  fprintf (dump_file, "KVX Frame info valid :%s\n",
	   fi->laid_out ? "yes" : "no");
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
kvx_static_chain (const_tree fndecl, bool incoming_p ATTRIBUTE_UNUSED)
{
  if (!DECL_STATIC_CHAIN (fndecl))
    return NULL;

  cfun->machine->static_chain_on_stack = 1;

  return gen_frame_mem (Pmode, frame_pointer_rtx);
}

static const char *pgr_reg_names[] = {KV3_PGR_REGISTER_NAMES};
static const char *qgr_reg_names[] = {KV3_QGR_REGISTER_NAMES};

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
kvx_pack_load_store (rtx operands[], unsigned int nops)
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

  for (unsigned i = 0; i < nops; i++)
    {
      set_dests[i] = operands[2 * i];
      set_srcs[i] = operands[2 * i + 1];
      sorted_operands[2 * i] = sorted_operands[2 * i + 1] = NULL_RTX;
    }

  /* Only for register size accesses */
  for (unsigned i = 0; i < nops; i++)
    if (GET_MODE (set_dests[i]) != DImode)
      return false;

  bool is_load = false;
  bool is_store = false;
  for (unsigned i = 0; i < nops; i++)
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
  for (unsigned int i = 1; i < nops; i++)
    if (REGNO (operands[i * 2 + op_offset]) < min_regno)
      min_regno = REGNO (operands[i * 2 + op_offset]);

  /* Sort operands based on regno */
  for (unsigned int i = 0; i < nops; i++)
    {
      const unsigned int regno = REGNO (operands[i * 2 + op_offset]);
      const unsigned int idx = 2 * (regno - min_regno);

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
      for (unsigned int i = 0; i < (nops - 1); i++)
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
  for (unsigned int i = 1; i < nops; i++)
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

/* Implement HARD_REGNO_RENAME_OK.  */
int
kvx_hard_regno_rename_ok (unsigned int from, unsigned int to)
{
  if (IN_RANGE (from, KV3_GPR_FIRST_REGNO, KV3_GPR_LAST_REGNO)
      && IN_RANGE (to, KV3_GPR_FIRST_REGNO, KV3_GPR_LAST_REGNO))
    {
      /* Retain quad alignement */
      if ((from % 4) == 0)
	return ((to % 4) == 0);

      /* Retain pair alignement */
      if ((from % 2) == 0)
	return ((to % 2) == 0);
    }

  return 1;
}

/* Implement HARD_REGNO_MODE_OK.  */
int
kvx_hard_regno_mode_ok (unsigned regno, enum machine_mode mode)
{
  // SI/DI -> KV3_GPR_FIRST_REGNO - KV3_GPR_LAST_REGNO => OK
  // SI/DI -> KV3_SFR_FIRST_REGNO - KV3_SFR_LAST_REGNO => OK
  // TI    -> KV3_GPR_FIRST_REGNO - KV3_GPR_LAST_REGNO && even => OK
  // OI    -> KV3_GPR_FIRST_REGNO - KV3_GPR_LAST_REGNO && 0mod4 => OK
  if (GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
    return 1;
  if (IN_RANGE (regno, KV3_GPR_FIRST_REGNO, KV3_GPR_LAST_REGNO))
    {
      if (GET_MODE_SIZE (mode) == 2 * UNITS_PER_WORD)
	return (regno % 2 == 0);
      if (GET_MODE_SIZE (mode) == 4 * UNITS_PER_WORD)
	return (regno % 4 == 0);
    }
  return 0;
}

static unsigned char
kvx_class_max_nregs (reg_class_t regclass ATTRIBUTE_UNUSED,
		     enum machine_mode mode)
{
  return HARD_REGNO_NREGS (0, mode);
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

/* Table of machine attributes.  */
static const struct attribute_spec kvx_attribute_table[] = {
  /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler,
     affects_type } */
  {"no_save_regs", 0, 0, true, false, false, kvx_handle_fndecl_attribute,
   false},
  {"farcall", 0, 0, true, false, false, kvx_handle_fndecl_attribute, false},
  {NULL, 0, 0, false, false, false, NULL, false}};

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
      && (INTVAL (XEXP (XEXP (x, 0), 1)) == GET_MODE_SIZE (mode))
      && kvx_legitimate_address_offset_register_p (XEXP (XEXP (x, 0), 0),
						   strict))
    // The .xs addressing mode applies to object sizes 2, 4, 8, 16, 32.
    return GET_MODE_SIZE (mode) > 1 && GET_MODE_SIZE (mode) <= 32;

  return false;
}

static void
kvx_conditional_register_usage (void)
{
  kvx_sync_reg_rtx = gen_rtx_REG (SImode, KV3_SYNC_REG_REGNO);
  kvx_link_reg_rtx = gen_rtx_REG (Pmode, KV3_RETURN_POINTER_REGNO);
}

rtx
kvx_return_addr_rtx (int count, rtx frameaddr ATTRIBUTE_UNUSED)
{
  return count == 0 ? get_hard_reg_initial_val (Pmode, KV3_RETURN_POINTER_REGNO)
		    : NULL_RTX;
}

/* Implements the macro INIT_CUMULATIVE_ARGS defined in kvx.h. */

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
		  enum machine_mode mode, const_tree type,
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

/* Implements TARGET_FUNCTION_ARG.
   Returns a reg rtx pointing at first argument register to be
   used for given argument or NULL_RTX if argument must be stacked
   because there is no argument slot in registers free. */

static rtx
kvx_function_arg (cumulative_args_t cum_v, enum machine_mode mode,
		  const_tree type, bool named)
{
  struct kvx_arg_info info;
  return kvx_get_arg_info (&info, cum_v, mode, type, named);
}

/* Implements TARGET_ARG_PARTIAL_BYTES.
   Return the number of bytes, at the beginning of an argument,
   that must be put in registers */

static int
kvx_arg_partial_bytes (cumulative_args_t cum_v, enum machine_mode mode,
		       tree type, bool named ATTRIBUTE_UNUSED)
{
  struct kvx_arg_info info = {0};
  rtx reg = kvx_get_arg_info (&info, cum_v, mode, type, named);
  if (reg != NULL_RTX && info.num_regs > 0 && info.num_stack > 0)
    {
      return info.num_regs * UNITS_PER_WORD;
    }
  return 0;
}

static void
kvx_function_arg_advance (cumulative_args_t cum_v, enum machine_mode mode,
			  const_tree type, bool named)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
  struct kvx_arg_info info = {0};
  kvx_get_arg_info (&info, cum_v, mode, type, named);

  if (info.num_regs > 0)
    {
      cum->next_arg_reg = info.first_reg + info.num_regs;
    }
  else if (named)
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

static rtx
kvx_function_value (const_tree ret_type, const_tree func ATTRIBUTE_UNUSED,
		    bool outgoing ATTRIBUTE_UNUSED)
{
  enum machine_mode mode = TYPE_MODE (ret_type);

  if (mode == BLKmode
      && ((int_size_in_bytes (ret_type) * BITS_PER_UNIT)
	  > (LONG_LONG_TYPE_SIZE)))
    {
      int nexps
	= (int_size_in_bytes (ret_type) + UNITS_PER_WORD - 1) / UNITS_PER_WORD;
      int i;

      rtx ret = gen_rtx_PARALLEL (BLKmode, rtvec_alloc (nexps));
      for (i = 0; i < nexps; i++)
	XVECEXP (ret, 0, i)
	  = gen_rtx_EXPR_LIST (VOIDmode,
			       gen_rtx_REG (DImode,
					    KV3_ARGUMENT_POINTER_REGNO + i),
			       GEN_INT (i * UNITS_PER_WORD));
      return ret;
    }
  else
    return gen_rtx_REG (TYPE_MODE (ret_type), KV3_ARGUMENT_POINTER_REGNO);
}

/* Implements TARGET_RETURN_IN_MSB */
static bool
kvx_return_in_msb (const_tree type ATTRIBUTE_UNUSED)
{
  return false;
}

/* Implements TARGET_RETURN_IN_MEMORY */
static bool
kvx_return_in_memory (const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  HOST_WIDE_INT sz = int_size_in_bytes (type);

  /* Return value can use up to 4 registers (256bits). Larger values
   * or variable sized type must be returned in memory. */
  return (sz > (4 * UNITS_PER_WORD) || sz < 0);
}

/* Implements TARGET_STRUCT_VALUE_RTX */
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
  rtx xops[1];
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
  int regno;
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
  regno = crtl->args.info.next_arg_reg;

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
kvx_decimal_float_supported_p (void)
{
  return true;
}

static bool
kvx_fixed_point_supported_p (void)
{
  return false;
}

static bool
kvx_vector_mode_supported_p (enum machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (mode)
    {
    case V8QImode:
    case V16QImode:
    case V32QImode:
    case V4HImode:
    case V8HImode:
    case V16HImode:
    case V2SImode:
    case V4SImode:
    case V8SImode:
    case V2DImode:
    case V4DImode:
    case V2SFmode:
    case V4SFmode:
    case V8SFmode:
    case V2DFmode:
    case V4DFmode:
      return true;
    default:
      break;
    }
  return false;
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
kvx_vectorize_preferred_simd_mode (machine_mode mode)
{
  switch (mode)
    {
    case HImode:
      return V8HImode;
    case SImode:
      return V4SImode;
    case DImode:
      return V2DImode;
    case SFmode:
      return V4SFmode;
    case DFmode:
      return V2DFmode;
    default:
      break;
    }
  return word_mode;
}

static bool
kvx_pass_by_reference (cumulative_args_t cum ATTRIBUTE_UNUSED,
		       enum machine_mode mode, const_tree type,
		       bool named ATTRIBUTE_UNUSED)
{
  HOST_WIDE_INT size;

  /* GET_MODE_SIZE (BLKmode) is useless since it is 0.  */
  size = (mode == BLKmode && type) ? int_size_in_bytes (type)
				   : (int) GET_MODE_SIZE (mode);

  /* Aggregates are passed by reference based on their size.  */
  if (type && AGGREGATE_TYPE_P (type))
    size = int_size_in_bytes (type);

  /* Variable sized arguments are always returned by reference.  */
  if (size < 0)
    return true;

  /* Arguments which are variable sized or larger than 4 registers are
     passed by reference */
  return (size > (4 * UNITS_PER_WORD)) && mode == BLKmode;
}

static reg_class_t
kvx_secondary_reload (bool in_p ATTRIBUTE_UNUSED, rtx x ATTRIBUTE_UNUSED,
		      reg_class_t reload_class ATTRIBUTE_UNUSED,
		      enum machine_mode reload_mode ATTRIBUTE_UNUSED,
		      secondary_reload_info *sri ATTRIBUTE_UNUSED)
{
  if (!flag_pic)
    return NO_REGS;

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
  return NO_REGS;
}

static const char *kvx_unspec_tls_asm_op[]
  = {"@tlsgd", "@tlsld", "@tlsle", "@dtpoff", "@tlsie"};

void
kvx_print_operand (FILE *file, rtx x, int code)
{
  rtx operand = x;
  bool select_qreg = 0;
  bool select_preg = 0;
  bool select_treg = 0;
  bool select_zreg = 0;
  bool select_yreg = 0;
  bool select_xreg = 0;
  bool addressing_mode = false;
  bool as_address = false;
  bool is_float = false;
  bool must_be_reversed = false;
  int addr_space = 0;

  switch (code)
    {
    case 0:
      /* No code, print as usual.  */
      break;

    case 'a':
      as_address = true;
      break;

    case 'o':
      select_qreg = true;
      break;

    case 'q':
      select_preg = true;
      break;

    case 't':
      select_treg = true;
      break;

    case 'z':
      select_zreg = true;
      break;

    case 'y':
      select_yreg = true;
      break;

    case 'x':
      select_xreg = true;
      break;

    case 'f':
      is_float = true;
      break;

    case 'R':
      must_be_reversed = true;
      break;

    case 'T':
      fprintf (file, "@pcrel(");
      output_addr_const (file, operand);
      fprintf (file, ")");
      return;

    case 'm':
      addressing_mode = true;
      break;

    case 'C': /* Print an additional '.u' or '.us' in the case of uncached load
	       */
      addr_space = kvx_is_uncached_mem_op_p (x);
      if (addr_space == KVX_ADDR_SPACE_BYPASS)
	fprintf (file, ".u");
      if (addr_space == KVX_ADDR_SPACE_PRELOAD)
	fprintf (file, ".us");
      return;

    default:
      output_operand_lossage ("invalid operand code '%c'", code);
    }

  if ((as_address || addressing_mode) && GET_CODE (x) != MEM)
    {
      x = gen_rtx_MEM (Pmode, x);
      operand = x;
    }

  if (COMPARISON_P (x))
    {
      if (!is_float)
	{
	  if (must_be_reversed)
	    fprintf (file, "%s",
		     GET_RTX_NAME (reverse_condition (GET_CODE (x))));
	  else
	    fprintf (file, "%s", GET_RTX_NAME (GET_CODE (x)));
	}
      else
	{
	  const char *name;
	  switch (GET_CODE (x))
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
	error ("internal error: bad register: %d", REGNO (operand));
      if (system_register_operand (operand, VOIDmode))
	gcc_assert (GET_MODE_SIZE (GET_MODE (x)) <= UNITS_PER_WORD);
      if (select_qreg)
	{
	  fprintf (file, "$%s", qgr_reg_names[REGNO (operand)]);
	}
      else if (select_preg)
	{
	  fprintf (file, "$%s", pgr_reg_names[REGNO (operand)]);
	}
      else if (select_treg)
	{
	  if (GET_MODE_SIZE (GET_MODE (x)) < UNITS_PER_WORD * 4)
	    error ("using %%t format with operand smaller than 4 registers");
	  fprintf (file, "$%s", reg_names[REGNO (operand) + 3]);
	}
      else if (select_zreg)
	{
	  if (GET_MODE_SIZE (GET_MODE (x)) < UNITS_PER_WORD * 4)
	    error ("using %%z format with operand smaller than 4 registers");
	  fprintf (file, "$%s", reg_names[REGNO (operand) + 2]);
	}
      else if (select_yreg)
	{
	  if (GET_MODE_SIZE (GET_MODE (x)) < UNITS_PER_WORD * 2)
	    error ("using %%y format with operand smaller than 2 registers");
	  fprintf (file, "$%s", reg_names[REGNO (operand) + 1]);
	}
      else if (select_xreg)
	{
	  if (GET_MODE_SIZE (GET_MODE (x)) < UNITS_PER_WORD * 2)
	    error ("using %%x format with operand smaller than 2 registers");
	  fprintf (file, "$%s", reg_names[REGNO (operand)]);
	}
      else if (GET_MODE_SIZE (GET_MODE (x)) == UNITS_PER_WORD * 4)
	{
	  fprintf (file, "$%s", qgr_reg_names[REGNO (operand)]);
	}
      else if (GET_MODE_SIZE (GET_MODE (x)) == UNITS_PER_WORD * 2)
	{
	  fprintf (file, "$%s", pgr_reg_names[REGNO (operand)]);
	}
      else
	fprintf (file, "$%s", reg_names[REGNO (operand)]);
      return;

    case MEM:
      if (addressing_mode)
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

    case CONST_DOUBLE:
      if (GET_MODE (x) == SFmode)
	{
	  REAL_VALUE_TYPE r;
	  long l;
	  r = *CONST_DOUBLE_REAL_VALUE (operand);
	  REAL_VALUE_TO_TARGET_SINGLE (r, l);
	  fprintf (file, "0x%08x", (unsigned int) l);
	  return;
	}
      else if (GET_MODE (x) == DFmode)
	{
	  /* this is a double that should fit on less than 64bits */
	  REAL_VALUE_TYPE r;
	  long l[2];
	  r = *CONST_DOUBLE_REAL_VALUE (operand);
	  REAL_VALUE_TO_TARGET_DOUBLE (r, l);
	  fprintf (file, "0x%08x%08x", (unsigned int) l[1], (unsigned int) l[0]);
	  return;
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
	int slice = 1 * select_yreg + 2 * select_zreg + 3 * select_treg;
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
	return pgr_reg_names[REGNO (x)];
      else if (GET_MODE_SIZE (mode) <= 4 * UNITS_PER_WORD)
	return qgr_reg_names[REGNO (x)];
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
kvx_print_operand_address (FILE *file, rtx x)
{
  switch (GET_CODE (x))
    {
    case REG:
    case SUBREG:
      fprintf (file, "0[$%s]", kvx_regname (x));
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
}

bool
kvx_print_punct_valid_p (unsigned char code)
{
  return code == ';';
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
	  && !call_really_used_regs[regno] // reg is callee-saved
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
kvx_get_callersaved_nonfixed_reg (machine_mode mode, unsigned int n)
{
  int regno;
  unsigned int i;
  // start at R16 as as everything before that may be used.
  // We should be able to use the veneer regs if not fixed.
  for (i = 0, regno = 16; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      bool candidate = call_really_used_regs[regno] && !fixed_regs[regno];
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
kvx_emit_multiple_spill (rtx mem, rtx reg, unsigned nr, bool is_load)
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
      gcc_assert (XVECLEN (PATTERN (insn), 0) == nr);

      for (unsigned int i = 0; i < nr; i++)
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

  unsigned regno;

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

/* Implement INITIAL_ELIMINATION_OFFSET.  FROM is either the frame pointer
   or argument pointer.  TO is either the stack pointer or frame
   pointer.  */

HOST_WIDE_INT
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
#if defined(GCC_KVX_MPPA_COS)
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
  HOST_WIDE_INT size = frame->frame_size;
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
      add_reg_note (insn, REG_CFA_DEF_CFA,
		    gen_rtx_PLUS (Pmode, hard_frame_pointer_rtx,
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

/* FIXME AUTO: fix cost function for coolidge */
/* See T7748 */
static int
kvx_register_move_cost (machine_mode mode, reg_class_t from ATTRIBUTE_UNUSED,
			reg_class_t to ATTRIBUTE_UNUSED)
{
  int n_copyd = (GET_MODE_SIZE (mode) + UNITS_PER_WORD - 1) / UNITS_PER_WORD;
  /* Provide a cost slightly above one of a simple instruction. This prevents
       postreload from transforming:
	  make $r2 = 0
	  make $r3 = 0
	  ;;
       into
	  make $r2 = 0
	  ;;
	  make $r3 = $r2
	  ;;
    */
  return (COSTS_N_INSNS (1) + 1) * n_copyd;
}


/* Canonicalize a comparison from one we don't have to one we do have.  */
static void
kvx_canonicalize_comparison (int *code, rtx *op0, rtx *op1,
			     bool op0_preserve_value)
{
  machine_mode mode = GET_MODE (*op0);
  bool float_compare_p = (GET_MODE_CLASS (mode) == MODE_FLOAT);
  static struct rtx_def rtx_; PUT_CODE (&rtx_, (enum rtx_code)*code);

  if (! op0_preserve_value && float_compare_p
      && ! float_comparison_operator (&rtx_, VOIDmode))
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
    case V2SFmode:
      return V2SImode;
    case V4SFmode:
      return V4SImode;
    case V8SFmode:
      return V8SImode;
    case V2DFmode:
      return V2DImode;
    case V4DFmode:
      return V4DImode;
    case SFmode:
    case DFmode:
      return DImode;
    default:
      break;
    }
  return mode;
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

  machine_mode pred_mode = GET_MODE (pred);
  rtx cmp = gen_rtx_fmt_ee (cmp_code, pred_mode, left, right);

  if (float_compare_p)
    {
      if (! float_comparison_operator (cmp, VOIDmode))
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
      rtx const0_rtx_mode = CONST0_RTX (mode);
      rtx constm1_rtx_mode = CONSTM1_RTX (mode);
      vector_true_false = (select1 == constm1_rtx_mode)
	                  && (select2 == const0_rtx_mode);
    }

  if (vector_true_false)
    {
      dst = target;
    }
  else if (! rtx_equal_p (select1, target)
	   && ! rtx_equal_p (select2, target))
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
  else
    {
      rtx const0_rtx_pred_mode = CONST0_RTX (pred_mode);
      rtx cmp0 = gen_rtx_fmt_ee (cmp_code, VOIDmode, pred, const0_rtx_pred_mode);
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
      rtx const0_rtx_mode = CONST0_RTX (mode);
      rtx constm1_rtx_mode = CONSTM1_RTX (mode);
      vector_true_false = (select1 == constm1_rtx_mode)
	                  && (select2 == const0_rtx_mode);
    }

  if (vector_true_false)
    {
      dst = target;
    }
  else if (! rtx_equal_p (select1, target)
	   && ! rtx_equal_p (select2, target))
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
  else
    {
      machine_mode mask_mode = GET_MODE (mask);
      rtx const0_rtx_mask_mode = CONST0_RTX (mask_mode);
      rtx cmp0 = gen_rtx_fmt_ee (cmp_code, VOIDmode, mask, const0_rtx_mask_mode);
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
    case V16QImode:
      return V8QImode;
    case V8HImode:
      return V4HImode;
    case V4SImode:
      return V2SImode;
    case V4SFmode:
      return V2SFmode;
    case V2DImode:
      return DImode;
    case V2DFmode:
      return DFmode;
    case V32QImode:
      return V8QImode;
    case V16HImode:
      return V4HImode;
    case V8SImode:
      return V2SImode;
    case V8SFmode:
      return V2SFmode;
    case V4DImode:
      return DImode;
    case V4DFmode:
      return DFmode;
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
	  rtx op0 = simplify_gen_subreg (inner_mode, target, vector_mode, major*UNITS_PER_WORD);
	  rtx op1 = source;
	  emit_move_insn (op0, op1);
	}
      else
	{
	  machine_mode chunk_mode = kvx_get_chunk_mode (vector_mode);
	  rtx op0 = simplify_gen_subreg (chunk_mode, target, vector_mode, major*UNITS_PER_WORD);
	  rtx op1 = source;
	  rtx op2 = GEN_INT (width*8);
	  rtx op3 = GEN_INT (minor*8);
	  rtx insert = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (4, op1, op2, op3, op0), UNSPEC_INSF);
	  emit_insn (gen_rtx_SET (op0, insert));
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
	  rtx op1 = simplify_gen_subreg(inner_mode, source, vector_mode, major*UNITS_PER_WORD);
	  emit_move_insn (op0, op1);
	}
      else
	{
	  machine_mode chunk_mode = kvx_get_chunk_mode (vector_mode);
	  rtx op0 = target;
	  rtx op1 = simplify_gen_subreg(chunk_mode, source, vector_mode, major*UNITS_PER_WORD);
	  rtx op2 = GEN_INT (width*8);
	  rtx op3 = GEN_INT (minor*8);
	  rtx extract = gen_rtx_UNSPEC (inner_mode, gen_rtvec (3, op1, op2, op3), UNSPEC_EXTFZ);
	  emit_insn (gen_rtx_SET (op0, extract));
	}

      return;
    }

  gcc_unreachable ();
}

/* Splat a value of mode smaller than 8 bytes into a 64-bit vector chunk.
 * This is used both for initializing a vector from a scalar, and for the
 * vector arithmetic patterns that operate a vector with a scalar.  */
rtx
kvx_expand_chunk_splat (rtx target, rtx source, machine_mode inner_mode)
{
  HOST_WIDE_INT constant = 0;
  machine_mode chunk_mode = GET_MODE (target);
  unsigned inner_size = GET_MODE_SIZE (inner_mode);

  if (!REG_P (source) && !SUBREG_P (source))
    source = force_reg (inner_mode, source);

  switch (inner_size)
    {
    case 1:
      constant = 0x0101010101010101LL;
      break;
    case 2:
      constant = 0x0201020102010201LL;
      break;
    case 4:
      constant = 0x0804020108040201LL;
      break;
    default:
      gcc_unreachable ();
    }
  rtx op2 = force_reg (DImode, GEN_INT (constant));
  rtx op1 = simplify_gen_subreg (inner_mode, source, GET_MODE (source), 0);
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
	rtx initx2a = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (1, op1), UNSPEC_INITX2A);
	rtx initx2b = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (2, op2, target), UNSPEC_INITX2B);
	emit_insn (gen_rtx_SET (target, initx2a));
	emit_insn (gen_rtx_SET (target, initx2b));
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

/* Called by the vec_init<mode> standard pattern. */
void
kvx_expand_vector_init (rtx target, rtx source)
{
  machine_mode vector_mode = GET_MODE (target);
  unsigned vector_size = GET_MODE_SIZE (vector_mode);
  machine_mode inner_mode = GET_MODE_INNER (vector_mode);
  unsigned inner_size = GET_MODE_SIZE (inner_mode);
  int nunits = GET_MODE_NUNITS (vector_mode);

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

bool
kvx_vectorize_vec_perm_const_ok (enum machine_mode vector_mode,
				 const unsigned char *sel)
{
  return true;
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
  HOST_WIDE_INT constant0 = 0x8040201008040201ULL, constanti = 0;
  int origm = -1, origi = -1;

  // The move constant is the one with a subset of bits of constant0.
  if ((constant1 & constant0) == constant1)
    {
      origm = orig1, origi = orig2;
      constanti = constant2;
    }
  else if ((constant2 & constant0) == constant2)
    {
      origm = orig2, origi = orig1;
      constanti = constant1;
    }

  if (!constanti)
    return NULL_RTX;

  // The insert constant must be constant0 shifted left and truncated.
  int shift = __builtin_ctzll (constanti);
  int count = __builtin_popcountll (constanti);
  HOST_WIDE_INT maski = (-1ULL >> __builtin_clzll (constanti)) & (constant0 << shift);
 
  // For speed we prevent the generation of extract as SBMM8 is faster.
  //if (optimize_insn_for_speed_p () && (shift & 7))
    //return NULL_RTX;

  if (maski == constanti && origm >= 0 && origi >= 0)
    {
      machine_mode inner_mode = GET_MODE_INNER (vector_mode);
      machine_mode chunk_mode = kvx_get_chunk_mode (vector_mode);
      rtx op0 = kvx_expand_vec_perm_const_emit_move (target, source1, source2,
						     dest, origm);
      rtx sourcei = origi >= nwords? source2: source1;
      int offseti = origi >= nwords? origi - nwords: origi;
      rtx op2 = GEN_INT (count*8);
      rtx op1 = NULL_RTX;

      // If shift is not a multiple of 8, extract is needed.
      if ((shift & 7))
	{
	  op1 = gen_reg_rtx (inner_mode);
	  rtx op3 = GEN_INT ((shift & 7)*8);
	  rtx opi = simplify_gen_subreg (chunk_mode, sourcei, vector_mode, offseti*UNITS_PER_WORD);
	  rtx extract = gen_rtx_UNSPEC (inner_mode, gen_rtvec (3, opi, op2, op3), UNSPEC_EXTFZ);
	  emit_insn (gen_rtx_SET (op1, extract));
	}
      else
	{
	  op1 = simplify_gen_subreg (inner_mode, sourcei, vector_mode, offseti*UNITS_PER_WORD);
	}

      rtx op3 = GEN_INT (shift & -8);
      rtx insert = gen_rtx_UNSPEC (chunk_mode, gen_rtvec (4, op1, op2, op3, op0), UNSPEC_INSF);
      emit_insn (gen_rtx_SET (op0, insert));
      return op0;
    }

  return NULL_RTX;
}

/* Implement swizzle (NULL source2) or shuffle based on kvx_expand_vec_perm.
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
      HOST_WIDE_INT constant0 = 0x8040201008040201ULL;
      int orig0 = -1, orig1 = -1, orig2 = -1, nconst = 0;

      for (int orig = 0; orig < range; orig++)
	{
	  HOST_WIDE_INT constant = kvx_expand_vec_perm.values[orig][dest].dword;
	  if (constant)
	    {
	      if (constant == constant0)
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
	      rtx source = orig >= nwords? source2: source1;
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
void
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

  if (which == 1)
    kvx_expand_vec_perm_const_emit (target, source1, NULL_RTX);
  else
    kvx_expand_vec_perm_const_emit (target, source1, source2);
}

/* Emit a barrier, that is appropriate for memory model MODEL, at the
   start of a sequence implementing an atomic operation. We always use
   a very conservative memory model since the KV3 has a relaxed memory
   consistency model, meaning that all loads and stores are scheduled
   out-of-order at different memory addresses. Only simple load/store
   operations are performed with more usual memory constraints (if
   MOVE is true). */

void
kvx_emit_pre_barrier (rtx model, bool move)
{
  const enum memmodel mm = memmodel_from_int (INTVAL (model));
  switch (mm & MEMMODEL_BASE_MASK) // treat sync operations as atomic ones
    {
    case MEMMODEL_RELAXED:
    case MEMMODEL_CONSUME:
    case MEMMODEL_ACQUIRE:
    case MEMMODEL_ACQ_REL:
      // no barrier is required for RELAXED, CONSUME, ACQUIRE, and
      // ACQ_REL memory models with MOVE operations (loads/stores). Be
      // conservative for any other cases, emit a fence.
      if (move)
	break;
    case MEMMODEL_RELEASE:
    case MEMMODEL_SEQ_CST:
      emit_insn (gen_mem_thread_fence (model));
      break;
    default:
      gcc_unreachable ();
    }
}

/* Emit a barrier, that is appropriate for memory model MODEL, at the
   end of a sequence implementing an atomic operation. See
   kvx_emit_pre_barrier () for MOVE. */

void
kvx_emit_post_barrier (rtx model, bool move)
{
  const enum memmodel mm = memmodel_from_int (INTVAL (model));
  switch (mm & MEMMODEL_BASE_MASK) // treat sync operations as atomic ones
    {
    case MEMMODEL_RELAXED:
    case MEMMODEL_RELEASE:
    case MEMMODEL_ACQ_REL:
      // no barrier is required for RELAXED, RELEASE, and ACQ_REL
      // memory models with MOVE operations (loads/stores). Be
      // conservative for any other cases, emit a fence.
      if (move)
	break;
    case MEMMODEL_ACQUIRE:
    case MEMMODEL_CONSUME:
    case MEMMODEL_SEQ_CST:
      emit_insn (gen_mem_thread_fence (model));
      break;
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

  emit_insn (mode == SImode ? gen_acswapw (tmp, mem) : gen_acswapd (tmp, mem));

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

  kvx_emit_pre_barrier (model, false);

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

  /* Update memory with op result iff memory hasn't been modifyed
  since, i.e: if CURR_MEM_VAL == MEM; then update MEM with
  NEW_MEM_VAL; else try again */
  emit_insn (mode == SImode ? gen_acswapw (tmp, mem) : gen_acswapd (tmp, mem));
  /* ACSWAP insn returns 0x0 (fail) or 0x1 (success) in the low part
     of TMP:
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

  kvx_emit_post_barrier (model, false);
}

/* Expand the atomic test-and-set on byte atomic operation using the
   acswapw insn. */

void
kvx_expand_atomic_test_and_set (rtx operands[])
{
  rtx mem = operands[1];   /* memory to be modified */
  rtx model = operands[2]; /* memory model */

  rtx retry = gen_label_rtx ();
  rtx fini = gen_label_rtx ();
  rtx pos = gen_reg_rtx (Pmode);
  rtx offset = gen_reg_rtx (Pmode);
  rtx memsi;
  rtx tmp = gen_reg_rtx (TImode);
  rtx val = gen_lowpart (SImode, (gen_highpart (DImode, tmp)));
  rtx newval = gen_lowpart (SImode, (gen_lowpart (DImode, tmp)));
  rtx byte = gen_reg_rtx (SImode);
  rtx mask = gen_reg_rtx (SImode);
  rtx (*gen3) (rtx, rtx, rtx);
  rtx (*gen2) (rtx, rtx);

  kvx_emit_pre_barrier (model, false);

  emit_move_insn (pos, XEXP (mem, 0)); /* copy MEM pointer */
  /* find the place of the byte to test-and-set within the memory
     word-aligned it belongs to (POS is 0, 1, 2, or 3) */
  gen3 = Pmode == SImode ? gen_andsi3 : gen_anddi3;
  emit_insn (gen3 (pos, pos, GEN_INT (0x3)));
  /* the address of word containing the byte is MEM+OFFSET, with
     OFFSET <- -POS */
  gen2 = Pmode == SImode ? gen_negsi2 : gen_negdi2;
  emit_insn (gen2 (offset, pos));
  /* if MEM already has an offset, update OFFSET */
  gen3 = Pmode == SImode ? gen_addsi3 : gen_adddi3;
  if (GET_CODE (XEXP (mem, 0)) == PLUS)
    emit_insn (gen3 (offset, offset, XEXP (XEXP (mem, 0), 1)));

  /* load the word containing the byte to test-and-set
    - if MEM is already of the form offset[addr]: load OFFSET[addr]
    - else: load OFFSET[MEM] */
  if (GET_CODE (XEXP (mem, 0)) == PLUS)
    memsi = gen_rtx_MEM (SImode,
			 gen_rtx_PLUS (Pmode, XEXP (XEXP (mem, 0), 0), offset));
  else
    memsi = gen_rtx_MEM (SImode, gen_rtx_PLUS (Pmode, XEXP (mem, 0), offset));

  /* POS = (POS*BITS_PER_UNIT) */
  gen3 = Pmode == SImode ? gen_mulsi3 : gen_muldi3;
  emit_insn (gen3 (pos, pos, GEN_INT (BITS_PER_UNIT)));

  emit_label (retry); /* cas loop entry point */
  emit_insn (gen_atomic_loadsi (val, memsi, GEN_INT (MEMMODEL_RELAXED)));

  /* keep only the byte to test-and-set: BYTE <- VAL >> POS & 0xFF */
  emit_insn (gen_lshrsi3 (byte, val, gen_lowpart (SImode, pos)));
  emit_insn (gen_andsi3 (byte, byte, GEN_INT (0xFF)));

  /* if BYTE is false, try a compare-and-swap with the byte set to
     TRUE, else return true (i.e. BYTE) because the lock is already
     acquired */
  emit_cmp_and_jump_insns (byte, const0_rtx, NE, NULL_RTX, SImode, true, fini);
  /* NEWVAL <- VAL | TRUE << (POS*BITS_PER_UNIT) */
  emit_move_insn (mask, const1_rtx);
  emit_insn (gen_ashlsi3 (mask, mask, gen_lowpart (SImode, pos)));
  emit_insn (gen_iorsi3 (newval, val, mask));
  emit_insn (gen_acswapw (tmp, memsi));

  /* ACSWAP insn returns 0x0 (fail) or 0x1 (success) in the low part of TMP:
     - if successful: MEM is updated, do not loop,
		      lock is acquired, return false (i.e. BYTE)
     - if failing: MEM has changed, try again */
  emit_cmp_and_jump_insns (newval, const1_rtx, NE, NULL_RTX, SImode, true,
			   retry);

  emit_label (fini);
  emit_move_insn (operands[0], gen_lowpart (QImode, byte));

  kvx_emit_post_barrier (model, false);
}

enum kvx_builtin
{
  KVX_BUILTIN_ADDD,
  KVX_BUILTIN_SBFD,

  KVX_BUILTIN_ABDHQ,
  KVX_BUILTIN_ABDHO,
  KVX_BUILTIN_ABDHX,
  KVX_BUILTIN_ABDW,
  KVX_BUILTIN_ABDWP,
  KVX_BUILTIN_ABDWQ,
  KVX_BUILTIN_ABDWO,
  KVX_BUILTIN_ABDD,
  KVX_BUILTIN_ABDDP,
  KVX_BUILTIN_ABDDQ,

  KVX_BUILTIN_ABDHQS,
  KVX_BUILTIN_ABDHOS,
  KVX_BUILTIN_ABDHXS,
  KVX_BUILTIN_ABDWPS,
  KVX_BUILTIN_ABDWQS,
  KVX_BUILTIN_ABDWOS,
  KVX_BUILTIN_ABDDPS,
  KVX_BUILTIN_ABDDQS,

  KVX_BUILTIN_AVGHQ,
  KVX_BUILTIN_AVGHO,
  KVX_BUILTIN_AVGHX,
  KVX_BUILTIN_AVGW,
  KVX_BUILTIN_AVGWP,
  KVX_BUILTIN_AVGWQ,
  KVX_BUILTIN_AVGWO,

  KVX_BUILTIN_AVGUHQ,
  KVX_BUILTIN_AVGUHO,
  KVX_BUILTIN_AVGUHX,
  KVX_BUILTIN_AVGUW,
  KVX_BUILTIN_AVGUWP,
  KVX_BUILTIN_AVGUWQ,
  KVX_BUILTIN_AVGUWO,

  KVX_BUILTIN_AVGRHQ,
  KVX_BUILTIN_AVGRHO,
  KVX_BUILTIN_AVGRHX,
  KVX_BUILTIN_AVGRW,
  KVX_BUILTIN_AVGRWP,
  KVX_BUILTIN_AVGRWQ,
  KVX_BUILTIN_AVGRWO,

  KVX_BUILTIN_AVGRUHQ,
  KVX_BUILTIN_AVGRUHO,
  KVX_BUILTIN_AVGRUHX,
  KVX_BUILTIN_AVGRUW,
  KVX_BUILTIN_AVGRUWP,
  KVX_BUILTIN_AVGRUWQ,
  KVX_BUILTIN_AVGRUWO,

  KVX_BUILTIN_ADDSHQ,
  KVX_BUILTIN_ADDSHO,
  KVX_BUILTIN_ADDSHX,
  KVX_BUILTIN_ADDSW,
  KVX_BUILTIN_ADDSWP,
  KVX_BUILTIN_ADDSWQ,
  KVX_BUILTIN_ADDSWO,
  KVX_BUILTIN_ADDSD,
  KVX_BUILTIN_ADDSDP,
  KVX_BUILTIN_ADDSDQ,

  KVX_BUILTIN_ADDSHQS,
  KVX_BUILTIN_ADDSHOS,
  KVX_BUILTIN_ADDSHXS,
  KVX_BUILTIN_ADDSWPS,
  KVX_BUILTIN_ADDSWQS,
  KVX_BUILTIN_ADDSWOS,
  KVX_BUILTIN_ADDSDPS,
  KVX_BUILTIN_ADDSDQS,

  KVX_BUILTIN_SBFSHQ,
  KVX_BUILTIN_SBFSHO,
  KVX_BUILTIN_SBFSHX,
  KVX_BUILTIN_SBFSW,
  KVX_BUILTIN_SBFSWP,
  KVX_BUILTIN_SBFSWQ,
  KVX_BUILTIN_SBFSWO,
  KVX_BUILTIN_SBFSD,
  KVX_BUILTIN_SBFSDP,
  KVX_BUILTIN_SBFSDQ,

  KVX_BUILTIN_SBFSHQS,
  KVX_BUILTIN_SBFSHOS,
  KVX_BUILTIN_SBFSHXS,
  KVX_BUILTIN_SBFSWPS,
  KVX_BUILTIN_SBFSWQS,
  KVX_BUILTIN_SBFSWOS,
  KVX_BUILTIN_SBFSDPS,
  KVX_BUILTIN_SBFSDQS,

  KVX_BUILTIN_MINHQ,
  KVX_BUILTIN_MINHO,
  KVX_BUILTIN_MINHX,
  KVX_BUILTIN_MINW,
  KVX_BUILTIN_MINWP,
  KVX_BUILTIN_MINWQ,
  KVX_BUILTIN_MINWO,
  KVX_BUILTIN_MIND,
  KVX_BUILTIN_MINDP,
  KVX_BUILTIN_MINDQ,

  KVX_BUILTIN_MINHQS,
  KVX_BUILTIN_MINHOS,
  KVX_BUILTIN_MINHXS,
  KVX_BUILTIN_MINWPS,
  KVX_BUILTIN_MINWQS,
  KVX_BUILTIN_MINWOS,
  KVX_BUILTIN_MINDPS,
  KVX_BUILTIN_MINDQS,

  KVX_BUILTIN_MAXHQ,
  KVX_BUILTIN_MAXHO,
  KVX_BUILTIN_MAXHX,
  KVX_BUILTIN_MAXW,
  KVX_BUILTIN_MAXWP,
  KVX_BUILTIN_MAXWQ,
  KVX_BUILTIN_MAXWO,
  KVX_BUILTIN_MAXD,
  KVX_BUILTIN_MAXDP,
  KVX_BUILTIN_MAXDQ,

  KVX_BUILTIN_MAXHQS,
  KVX_BUILTIN_MAXHOS,
  KVX_BUILTIN_MAXHXS,
  KVX_BUILTIN_MAXWPS,
  KVX_BUILTIN_MAXWQS,
  KVX_BUILTIN_MAXWOS,
  KVX_BUILTIN_MAXDPS,
  KVX_BUILTIN_MAXDQS,

  KVX_BUILTIN_MINUHQ,
  KVX_BUILTIN_MINUHO,
  KVX_BUILTIN_MINUHX,
  KVX_BUILTIN_MINUW,
  KVX_BUILTIN_MINUWP,
  KVX_BUILTIN_MINUWQ,
  KVX_BUILTIN_MINUWO,
  KVX_BUILTIN_MINUD,
  KVX_BUILTIN_MINUDP,
  KVX_BUILTIN_MINUDQ,

  KVX_BUILTIN_MINUHQS,
  KVX_BUILTIN_MINUHOS,
  KVX_BUILTIN_MINUHXS,
  KVX_BUILTIN_MINUWPS,
  KVX_BUILTIN_MINUWQS,
  KVX_BUILTIN_MINUWOS,
  KVX_BUILTIN_MINUDPS,
  KVX_BUILTIN_MINUDQS,

  KVX_BUILTIN_MAXUHQ,
  KVX_BUILTIN_MAXUHO,
  KVX_BUILTIN_MAXUHX,
  KVX_BUILTIN_MAXUW,
  KVX_BUILTIN_MAXUWP,
  KVX_BUILTIN_MAXUWQ,
  KVX_BUILTIN_MAXUWO,
  KVX_BUILTIN_MAXUD,
  KVX_BUILTIN_MAXUDP,
  KVX_BUILTIN_MAXUDQ,

  KVX_BUILTIN_MAXUHQS,
  KVX_BUILTIN_MAXUHOS,
  KVX_BUILTIN_MAXUHXS,
  KVX_BUILTIN_MAXUWPS,
  KVX_BUILTIN_MAXUWQS,
  KVX_BUILTIN_MAXUWOS,
  KVX_BUILTIN_MAXUDPS,
  KVX_BUILTIN_MAXUDQS,

  KVX_BUILTIN_SLLHQS,
  KVX_BUILTIN_SLLHOS,
  KVX_BUILTIN_SLLHXS,
  KVX_BUILTIN_SLLWPS,
  KVX_BUILTIN_SLLWQS,
  KVX_BUILTIN_SLLWOS,
  KVX_BUILTIN_SLLDPS,
  KVX_BUILTIN_SLLDQS,

  KVX_BUILTIN_SLSHQS,
  KVX_BUILTIN_SLSHOS,
  KVX_BUILTIN_SLSHXS,
  KVX_BUILTIN_SLSWPS,
  KVX_BUILTIN_SLSWQS,
  KVX_BUILTIN_SLSWOS,
  KVX_BUILTIN_SLSDPS,
  KVX_BUILTIN_SLSDQS,

  KVX_BUILTIN_SRAHQS,
  KVX_BUILTIN_SRAHOS,
  KVX_BUILTIN_SRAHXS,
  KVX_BUILTIN_SRAWPS,
  KVX_BUILTIN_SRAWQS,
  KVX_BUILTIN_SRAWOS,
  KVX_BUILTIN_SRADPS,
  KVX_BUILTIN_SRADQS,

  KVX_BUILTIN_SRLHQS,
  KVX_BUILTIN_SRLHOS,
  KVX_BUILTIN_SRLHXS,
  KVX_BUILTIN_SRLWPS,
  KVX_BUILTIN_SRLWQS,
  KVX_BUILTIN_SRLWOS,
  KVX_BUILTIN_SRLDPS,
  KVX_BUILTIN_SRLDQS,

  KVX_BUILTIN_SRSHQS,
  KVX_BUILTIN_SRSHOS,
  KVX_BUILTIN_SRSHXS,
  KVX_BUILTIN_SRSWPS,
  KVX_BUILTIN_SRSWQS,
  KVX_BUILTIN_SRSWOS,
  KVX_BUILTIN_SRSDPS,
  KVX_BUILTIN_SRSDQS,

  KVX_BUILTIN_CLSW,
  KVX_BUILTIN_CLSWP,
  KVX_BUILTIN_CLSWQ,
  KVX_BUILTIN_CLSWO,
  KVX_BUILTIN_CLSD,
  KVX_BUILTIN_CLSDP,
  KVX_BUILTIN_CLSDQ,

  KVX_BUILTIN_CLZW,
  KVX_BUILTIN_CLZWP,
  KVX_BUILTIN_CLZWQ,
  KVX_BUILTIN_CLZWO,
  KVX_BUILTIN_CLZD,
  KVX_BUILTIN_CLZDP,
  KVX_BUILTIN_CLZDQ,

  KVX_BUILTIN_CTZW,
  KVX_BUILTIN_CTZWP,
  KVX_BUILTIN_CTZWQ,
  KVX_BUILTIN_CTZWO,
  KVX_BUILTIN_CTZD,
  KVX_BUILTIN_CTZDP,
  KVX_BUILTIN_CTZDQ,

  KVX_BUILTIN_CBSW,
  KVX_BUILTIN_CBSWP,
  KVX_BUILTIN_CBSWQ,
  KVX_BUILTIN_CBSWO,
  KVX_BUILTIN_CBSD,
  KVX_BUILTIN_CBSDP,
  KVX_BUILTIN_CBSDQ,

  KVX_BUILTIN_AWAIT,
  KVX_BUILTIN_BARRIER,
  KVX_BUILTIN_ACSWAPW,
  KVX_BUILTIN_ACSWAPD,
  KVX_BUILTIN_ALADDD,
  KVX_BUILTIN_ALADDW,
  KVX_BUILTIN_ALCLRD,
  KVX_BUILTIN_ALCLRW,
  KVX_BUILTIN_DINVAL,
  KVX_BUILTIN_DINVALL,
  KVX_BUILTIN_DTOUCHL,
  KVX_BUILTIN_DZEROL,
  KVX_BUILTIN_FENCE,

  KVX_BUILTIN_SELECTHQ,
  KVX_BUILTIN_SELECTHO,
  KVX_BUILTIN_SELECTHX,
  KVX_BUILTIN_SELECTWP,
  KVX_BUILTIN_SELECTWQ,
  KVX_BUILTIN_SELECTWO,
  KVX_BUILTIN_SELECTDP,
  KVX_BUILTIN_SELECTDQ,
  KVX_BUILTIN_SELECTFWP,
  KVX_BUILTIN_SELECTFWQ,
  KVX_BUILTIN_SELECTFWO,
  KVX_BUILTIN_SELECTFDP,
  KVX_BUILTIN_SELECTFDQ,

  KVX_BUILTIN_COPYSIGNW,
  KVX_BUILTIN_COPYSIGNWP,
  KVX_BUILTIN_COPYSIGNWQ,
  KVX_BUILTIN_COPYSIGNWO,
  KVX_BUILTIN_COPYSIGND,
  KVX_BUILTIN_COPYSIGNDP,
  KVX_BUILTIN_COPYSIGNDQ,

  KVX_BUILTIN_FMINW,
  KVX_BUILTIN_FMINWP,
  KVX_BUILTIN_FMINWQ,
  KVX_BUILTIN_FMINWO,
  KVX_BUILTIN_FMIND,
  KVX_BUILTIN_FMINDP,
  KVX_BUILTIN_FMINDQ,

  KVX_BUILTIN_FMINWPS,
  KVX_BUILTIN_FMINWQS,
  KVX_BUILTIN_FMINWOS,
  KVX_BUILTIN_FMINDPS,
  KVX_BUILTIN_FMINDQS,

  KVX_BUILTIN_FMAXW,
  KVX_BUILTIN_FMAXWP,
  KVX_BUILTIN_FMAXWQ,
  KVX_BUILTIN_FMAXWO,
  KVX_BUILTIN_FMAXD,
  KVX_BUILTIN_FMAXDP,
  KVX_BUILTIN_FMAXDQ,

  KVX_BUILTIN_FMAXWPS,
  KVX_BUILTIN_FMAXWQS,
  KVX_BUILTIN_FMAXWOS,
  KVX_BUILTIN_FMAXDPS,
  KVX_BUILTIN_FMAXDQS,

  KVX_BUILTIN_FNEGW,
  KVX_BUILTIN_FNEGWP,
  KVX_BUILTIN_FNEGWQ,
  KVX_BUILTIN_FNEGWO,
  KVX_BUILTIN_FNEGD,
  KVX_BUILTIN_FNEGDP,
  KVX_BUILTIN_FNEGDQ,

  KVX_BUILTIN_FABSW,
  KVX_BUILTIN_FABSWP,
  KVX_BUILTIN_FABSWQ,
  KVX_BUILTIN_FABSWO,
  KVX_BUILTIN_FABSD,
  KVX_BUILTIN_FABSDP,
  KVX_BUILTIN_FABSDQ,

  KVX_BUILTIN_FRECW,
  KVX_BUILTIN_FRECWP,
  KVX_BUILTIN_FRECWQ,
  KVX_BUILTIN_FRECWO,

  KVX_BUILTIN_FRSRW,
  KVX_BUILTIN_FRSRWP,
  KVX_BUILTIN_FRSRWQ,
  KVX_BUILTIN_FRSRWO,

  KVX_BUILTIN_FADDW,
  KVX_BUILTIN_FADDWP,
  KVX_BUILTIN_FADDWQ,
  KVX_BUILTIN_FADDWO,
  KVX_BUILTIN_FADDD,
  KVX_BUILTIN_FADDDP,
  KVX_BUILTIN_FADDDQ,

  KVX_BUILTIN_FADDCWC,
  KVX_BUILTIN_FADDCWCP,
  KVX_BUILTIN_FADDCWCQ,
  KVX_BUILTIN_FADDCDC,
  KVX_BUILTIN_FADDCDCP,

  KVX_BUILTIN_FSBFW,
  KVX_BUILTIN_FSBFWP,
  KVX_BUILTIN_FSBFWQ,
  KVX_BUILTIN_FSBFWO,
  KVX_BUILTIN_FSBFD,
  KVX_BUILTIN_FSBFDP,
  KVX_BUILTIN_FSBFDQ,

  KVX_BUILTIN_FSBFCWC,
  KVX_BUILTIN_FSBFCWCP,
  KVX_BUILTIN_FSBFCWCQ,
  KVX_BUILTIN_FSBFCDC,
  KVX_BUILTIN_FSBFCDCP,

  KVX_BUILTIN_FMULW,
  KVX_BUILTIN_FMULWP,
  KVX_BUILTIN_FMULWQ,
  KVX_BUILTIN_FMULWO,
  KVX_BUILTIN_FMULD,
  KVX_BUILTIN_FMULDP,
  KVX_BUILTIN_FMULDQ,

  KVX_BUILTIN_FMULWD,
  KVX_BUILTIN_FMULWC,
  KVX_BUILTIN_FMULWCP,
  KVX_BUILTIN_FMULWCQ,
  KVX_BUILTIN_FMULCWC,
  KVX_BUILTIN_FMULCWCP,
  KVX_BUILTIN_FMULCWCQ,
  KVX_BUILTIN_FMULDC,
  KVX_BUILTIN_FMULDCP,
  KVX_BUILTIN_FMULCDC,
  KVX_BUILTIN_FMULCDCP,
  KVX_BUILTIN_FMM212W,

  KVX_BUILTIN_FFMAW,
  KVX_BUILTIN_FFMAWP,
  KVX_BUILTIN_FFMAWQ,
  KVX_BUILTIN_FFMAWO,
  KVX_BUILTIN_FFMAD,
  KVX_BUILTIN_FFMADP,
  KVX_BUILTIN_FFMADQ,
  KVX_BUILTIN_FFMAWD,
  KVX_BUILTIN_FMMA212W,

  KVX_BUILTIN_FFMSW,
  KVX_BUILTIN_FFMSWP,
  KVX_BUILTIN_FFMSWQ,
  KVX_BUILTIN_FFMSWO,
  KVX_BUILTIN_FFMSD,
  KVX_BUILTIN_FFMSDP,
  KVX_BUILTIN_FFMSDQ,
  KVX_BUILTIN_FFMSWD,
  KVX_BUILTIN_FMMS212W,

  KVX_BUILTIN_FLOATW,
  KVX_BUILTIN_FLOATWP,
  KVX_BUILTIN_FLOATWQ,
  KVX_BUILTIN_FLOATWO,
  KVX_BUILTIN_FLOATD,
  KVX_BUILTIN_FLOATDP,
  KVX_BUILTIN_FLOATDQ,

  KVX_BUILTIN_FLOATUW,
  KVX_BUILTIN_FLOATUWP,
  KVX_BUILTIN_FLOATUWQ,
  KVX_BUILTIN_FLOATUWO,
  KVX_BUILTIN_FLOATUD,
  KVX_BUILTIN_FLOATUDP,
  KVX_BUILTIN_FLOATUDQ,

  KVX_BUILTIN_FIXEDW,
  KVX_BUILTIN_FIXEDWP,
  KVX_BUILTIN_FIXEDWQ,
  KVX_BUILTIN_FIXEDWO,
  KVX_BUILTIN_FIXEDD,
  KVX_BUILTIN_FIXEDDP,
  KVX_BUILTIN_FIXEDDQ,

  KVX_BUILTIN_FIXEDUW,
  KVX_BUILTIN_FIXEDUWP,
  KVX_BUILTIN_FIXEDUWQ,
  KVX_BUILTIN_FIXEDUWO,
  KVX_BUILTIN_FIXEDUD,
  KVX_BUILTIN_FIXEDUDP,
  KVX_BUILTIN_FIXEDUDQ,

  KVX_BUILTIN_FCONJWC,
  KVX_BUILTIN_FCONJWCP,
  KVX_BUILTIN_FCONJWCQ,
  KVX_BUILTIN_FCONJDC,
  KVX_BUILTIN_FCONJDCP,

  KVX_BUILTIN_FCDIVW,
  KVX_BUILTIN_FCDIVWP,
  KVX_BUILTIN_FCDIVWQ,
  KVX_BUILTIN_FCDIVWO,
  KVX_BUILTIN_FCDIVD,
  KVX_BUILTIN_FCDIVDP,
  KVX_BUILTIN_FCDIVDQ,

  KVX_BUILTIN_FSDIVW,
  KVX_BUILTIN_FSDIVWP,
  KVX_BUILTIN_FSDIVWQ,
  KVX_BUILTIN_FSDIVWO,
  KVX_BUILTIN_FSDIVD,
  KVX_BUILTIN_FSDIVDP,
  KVX_BUILTIN_FSDIVDQ,

  KVX_BUILTIN_FSRECW,
  KVX_BUILTIN_FSRECWP,
  KVX_BUILTIN_FSRECWQ,
  KVX_BUILTIN_FSRECWO,
  KVX_BUILTIN_FSRECD,
  KVX_BUILTIN_FSRECDP,
  KVX_BUILTIN_FSRECDQ,

  KVX_BUILTIN_FSRSRW,
  KVX_BUILTIN_FSRSRWP,
  KVX_BUILTIN_FSRSRWQ,
  KVX_BUILTIN_FSRSRWO,
  KVX_BUILTIN_FSRSRD,
  KVX_BUILTIN_FSRSRDP,
  KVX_BUILTIN_FSRSRDQ,

  KVX_BUILTIN_GET,
  KVX_BUILTIN_WFXL,
  KVX_BUILTIN_WFXM,
  KVX_BUILTIN_IINVAL,
  KVX_BUILTIN_IINVALS,
  KVX_BUILTIN_LBSU,
  KVX_BUILTIN_LBZU,
  KVX_BUILTIN_LHSU,
  KVX_BUILTIN_LHZU,
  KVX_BUILTIN_LDU,
  KVX_BUILTIN_LWZU,
  KVX_BUILTIN_SET,
  KVX_BUILTIN_SLEEP,
  KVX_BUILTIN_STOP,
  KVX_BUILTIN_SYNCGROUP,
  KVX_BUILTIN_TLBDINVAL,
  KVX_BUILTIN_TLBIINVAL,
  KVX_BUILTIN_TLBPROBE,
  KVX_BUILTIN_TLBREAD,
  KVX_BUILTIN_TLBWRITE,
  KVX_BUILTIN_WAITIT,

  KVX_BUILTIN_SATD,
  KVX_BUILTIN_SATUD,
  KVX_BUILTIN_STSUW,
  KVX_BUILTIN_STSUD,
  KVX_BUILTIN_SBMM8,
  KVX_BUILTIN_SBMMT8,
  KVX_BUILTIN_FWIDENLHW,
  KVX_BUILTIN_FWIDENMHW,
  KVX_BUILTIN_FNARROWWH,

  KVX_BUILTIN_LBZ,
  KVX_BUILTIN_LBS,
  KVX_BUILTIN_LHZ,
  KVX_BUILTIN_LHS,
  KVX_BUILTIN_LWZ,
  KVX_BUILTIN_LWS,
  KVX_BUILTIN_LD,
  KVX_BUILTIN_LQ,
  KVX_BUILTIN_LWF,
  KVX_BUILTIN_LDF,

  KVX_BUILTIN_LDBO,
  KVX_BUILTIN_LDHQ,
  KVX_BUILTIN_LDWP,
  KVX_BUILTIN_LDFWP,

  KVX_BUILTIN_SDBO,
  KVX_BUILTIN_SDHQ,
  KVX_BUILTIN_SDWP,
  KVX_BUILTIN_SDFWP,

  KVX_BUILTIN_LQBX,
  KVX_BUILTIN_LQHO,
  KVX_BUILTIN_LQWQ,
  KVX_BUILTIN_LQDP,
  KVX_BUILTIN_LQFWQ,
  KVX_BUILTIN_LQFDP,

  KVX_BUILTIN_SQBX,
  KVX_BUILTIN_SQHO,
  KVX_BUILTIN_SQWQ,
  KVX_BUILTIN_SQDP,
  KVX_BUILTIN_SQFWQ,
  KVX_BUILTIN_SQFDP,

  KVX_BUILTIN_LOBV,
  KVX_BUILTIN_LOHX,
  KVX_BUILTIN_LOWO,
  KVX_BUILTIN_LODQ,
  KVX_BUILTIN_LOFWO,
  KVX_BUILTIN_LOFDQ,

  KVX_BUILTIN_SOBV,
  KVX_BUILTIN_SOHX,
  KVX_BUILTIN_SOWO,
  KVX_BUILTIN_SODQ,
  KVX_BUILTIN_SOFWO,
  KVX_BUILTIN_SOFDQ,

  KVX_BUILTIN_LVBV,
  KVX_BUILTIN_LVHX,
  KVX_BUILTIN_LVWO,
  KVX_BUILTIN_LVDQ,
  KVX_BUILTIN_LVFWO,
  KVX_BUILTIN_LVFDQ,

  KVX_BUILTIN_SVBV,
  KVX_BUILTIN_SVHX,
  KVX_BUILTIN_SVWO,
  KVX_BUILTIN_SVDQ,
  KVX_BUILTIN_SVFWO,
  KVX_BUILTIN_SVFDQ,

  KVX_BUILTIN_MOVEFOBV,
  KVX_BUILTIN_MOVEFOHX,
  KVX_BUILTIN_MOVEFOWO,
  KVX_BUILTIN_MOVEFODQ,
  KVX_BUILTIN_MOVEFOFWO,
  KVX_BUILTIN_MOVEFOFDQ,

  KVX_BUILTIN_MOVETOBV,
  KVX_BUILTIN_MOVETOHX,
  KVX_BUILTIN_MOVETOWO,
  KVX_BUILTIN_MOVETODQ,
  KVX_BUILTIN_MOVETOFWO,
  KVX_BUILTIN_MOVETOFDQ,

  KVX_BUILTIN_SWAPVOBV,
  KVX_BUILTIN_SWAPVOHX,
  KVX_BUILTIN_SWAPVOWO,
  KVX_BUILTIN_SWAPVODQ,
  KVX_BUILTIN_SWAPVOFWO,
  KVX_BUILTIN_SWAPVOFDQ,

  KVX_BUILTIN_ALIGNOBV,
  KVX_BUILTIN_ALIGNOHX,
  KVX_BUILTIN_ALIGNOWO,
  KVX_BUILTIN_ALIGNODQ,
  KVX_BUILTIN_ALIGNOFWO,
  KVX_BUILTIN_ALIGNOFDQ,

  KVX_BUILTIN_ALIGNV,
  KVX_BUILTIN_COPYV,
  KVX_BUILTIN_MT4X4D,
  KVX_BUILTIN_MM4ABW,
  KVX_BUILTIN_FMM4AHW0,
  KVX_BUILTIN_FMM4AHW1,
  KVX_BUILTIN_FMM4AHW2,
  KVX_BUILTIN_FMM4AHW3,
  KVX_BUILTIN__LAST_
};

static tree builtin_fndecls[KVX_BUILTIN__LAST_];

static void
kvx_init_builtins (void)
{

#define VOID void_type_node
#define VPTR ptr_type_node
#define CVPTR const_ptr_type_node
#define BOOL boolean_type_node

#define INT8 intQI_type_node
#define INT16 intHI_type_node
#define INT32 intSI_type_node
#define INT64 intDI_type_node
#define INT128 intTI_type_node

#define UINT8 unsigned_intQI_type_node
#define UINT16 unsigned_intHI_type_node
#define UINT32 unsigned_intSI_type_node
#define UINT64 unsigned_intDI_type_node
#define UINT128 unsigned_intTI_type_node

#define FLOAT32 float_type_node
#define FLOAT64 double_type_node
#define COMPLEX64 complex_float_type_node
#define COMPLEX128 complex_double_type_node

  tree STRING = build_pointer_type (
    build_qualified_type (char_type_node, TYPE_QUAL_CONST));

  tree INT8X8 = build_vector_type (INT8, 8);
  tree INT8X16 = build_vector_type (INT8, 16);
  tree INT8X32 = build_vector_type (INT8, 32);
  tree INT16X4 = build_vector_type (INT16, 4);
  tree INT16X8 = build_vector_type (INT16, 8);
  tree INT16X16 = build_vector_type (INT16, 16);
  tree INT32X2 = build_vector_type (INT32, 2);
  tree INT32X4 = build_vector_type (INT32, 4);
  tree INT32X8 = build_vector_type (INT32, 8);
  tree INT64X2 = build_vector_type (INT64, 2);
  tree INT64X4 = build_vector_type (INT64, 4);
  tree UINT16X4 = build_vector_type (UINT16, 4);
  tree UINT16X8 = build_vector_type (UINT16, 8);
  tree UINT16X16 = build_vector_type (UINT16, 16);
  tree UINT32X2 = build_vector_type (UINT32, 2);
  tree UINT32X4 = build_vector_type (UINT32, 4);
  tree UINT32X8 = build_vector_type (UINT32, 8);
  tree UINT64X2 = build_vector_type (UINT64, 2);
  tree UINT64X4 = build_vector_type (UINT64, 4);
  tree FLOAT32X2 = build_vector_type (FLOAT32, 2);
  tree FLOAT32X4 = build_vector_type (FLOAT32, 4);
  tree FLOAT32X8 = build_vector_type (FLOAT32, 8);
  tree FLOAT64X2 = build_vector_type (FLOAT64, 2);
  tree FLOAT64X4 = build_vector_type (FLOAT64, 4);

#define ADD_KVX_BUILTIN_VARAGS(UC_NAME, LC_NAME, ...)                          \
  builtin_fndecls[KVX_BUILTIN_##UC_NAME]                                       \
    = add_builtin_function ("__builtin_kvx_" LC_NAME,                          \
			    build_varargs_function_type_list (__VA_ARGS__,     \
							      NULL_TREE),      \
			    KVX_BUILTIN_##UC_NAME, BUILT_IN_MD, NULL,          \
			    NULL_TREE)

#define ADD_KVX_BUILTIN(UC_NAME, LC_NAME, ...)                                 \
  builtin_fndecls[KVX_BUILTIN_##UC_NAME]                                       \
    = add_builtin_function ("__builtin_kvx_" LC_NAME,                          \
			    build_function_type_list (__VA_ARGS__, NULL_TREE), \
			    KVX_BUILTIN_##UC_NAME, BUILT_IN_MD, NULL,          \
			    NULL_TREE)

#define CARRY STRING
#define SIMDCOND STRING
#define ROUNDING STRING
#define SILENT STRING
#define BYPASS STRING
#define SCATTER STRING

  ADD_KVX_BUILTIN (ADDD, "addd", UINT64, UINT64, UINT64, CARRY); // Scalar
  ADD_KVX_BUILTIN (SBFD, "sbfd", UINT64, UINT64, UINT64, CARRY); // Scalar

  ADD_KVX_BUILTIN (ABDHQ, "abdhq", INT16X4, INT16X4, INT16X4); // Vector
  ADD_KVX_BUILTIN (ABDHO, "abdho", INT16X8, INT16X8, INT16X8); // Vector
  ADD_KVX_BUILTIN (ABDHX, "abdhx", INT16X16, INT16X16, INT16X16); // Vector
  ADD_KVX_BUILTIN (ABDW, "abdw", INT32, INT32, INT32); // Scalar
  ADD_KVX_BUILTIN (ABDWP, "abdwp", INT32X2, INT32X2, INT32X2); // Vector
  ADD_KVX_BUILTIN (ABDWQ, "abdwq", INT32X4, INT32X4, INT32X4); // Vector
  ADD_KVX_BUILTIN (ABDWO, "abdwo", INT32X8, INT32X8, INT32X8); // Vector
  ADD_KVX_BUILTIN (ABDD, "abdd", INT64, INT64, INT64); // Scalar
  ADD_KVX_BUILTIN (ABDDP, "abddp", INT64X2, INT64X2, INT64X2); // Vector
  ADD_KVX_BUILTIN (ABDDQ, "abddq", INT64X4, INT64X4, INT64X4); // Vector

  ADD_KVX_BUILTIN (ABDHQS, "abdhqs", INT16X4, INT16X4, INT16); // Vector
  ADD_KVX_BUILTIN (ABDHOS, "abdhos", INT16X8, INT16X8, INT16); // Vector
  ADD_KVX_BUILTIN (ABDHXS, "abdhxs", INT16X16, INT16X16, INT16); // Vector
  ADD_KVX_BUILTIN (ABDWPS, "abdwps", INT32X2, INT32X2, INT32); // Vector
  ADD_KVX_BUILTIN (ABDWQS, "abdwqs", INT32X4, INT32X4, INT32); // Vector
  ADD_KVX_BUILTIN (ABDWOS, "abdwos", INT32X8, INT32X8, INT32); // Vector
  ADD_KVX_BUILTIN (ABDDPS, "abddps", INT64X2, INT64X2, INT64); // Vector
  ADD_KVX_BUILTIN (ABDDQS, "abddqs", INT64X4, INT64X4, INT64); // Vector

  ADD_KVX_BUILTIN (AVGHQ, "avghq", INT16X4, INT16X4, INT16X4); // Vector
  ADD_KVX_BUILTIN (AVGHO, "avgho", INT16X8, INT16X8, INT16X8); // Vector
  ADD_KVX_BUILTIN (AVGHX, "avghx", INT16X16, INT16X16, INT16X16); // Vector
  ADD_KVX_BUILTIN (AVGW, "avgw", INT32, INT32, INT32); // Scalar
  ADD_KVX_BUILTIN (AVGWP, "avgwp", INT32X2, INT32X2, INT32X2); // Vector
  ADD_KVX_BUILTIN (AVGWQ, "avgwq", INT32X4, INT32X4, INT32X4); // Vector
  ADD_KVX_BUILTIN (AVGWO, "avgwo", INT32X8, INT32X8, INT32X8); // Vector

  ADD_KVX_BUILTIN (AVGUHQ, "avguhq", UINT16X4, UINT16X4, UINT16X4); // Vector
  ADD_KVX_BUILTIN (AVGUHO, "avguho", UINT16X8, UINT16X8, UINT16X8); // Vector
  ADD_KVX_BUILTIN (AVGUHX, "avguhx", UINT16X16, UINT16X16, UINT16X16); // Vector
  ADD_KVX_BUILTIN (AVGUW, "avguw", UINT32, UINT32, UINT32); // Scalar
  ADD_KVX_BUILTIN (AVGUWP, "avguwp", UINT32X2, UINT32X2, UINT32X2); // Vector
  ADD_KVX_BUILTIN (AVGUWQ, "avguwq", UINT32X4, UINT32X4, UINT32X4); // Vector
  ADD_KVX_BUILTIN (AVGUWO, "avguwo", UINT32X8, UINT32X8, UINT32X8); // Vector

  ADD_KVX_BUILTIN (AVGRHQ, "avgrhq", INT16X4, INT16X4, INT16X4); // Vector
  ADD_KVX_BUILTIN (AVGRHO, "avgrho", INT16X8, INT16X8, INT16X8); // Vector
  ADD_KVX_BUILTIN (AVGRHX, "avgrhx", INT16X16, INT16X16, INT16X16); // Vector
  ADD_KVX_BUILTIN (AVGRW, "avgrw", INT32, INT32, INT32); // Scalar
  ADD_KVX_BUILTIN (AVGRWP, "avgrwp", INT32X2, INT32X2, INT32X2); // Vector
  ADD_KVX_BUILTIN (AVGRWQ, "avgrwq", INT32X4, INT32X4, INT32X4); // Vector
  ADD_KVX_BUILTIN (AVGRWO, "avgrwo", INT32X8, INT32X8, INT32X8); // Vector

  ADD_KVX_BUILTIN (AVGRUHQ, "avgruhq", UINT16X4, UINT16X4, UINT16X4); // Vector
  ADD_KVX_BUILTIN (AVGRUHO, "avgruho", UINT16X8, UINT16X8, UINT16X8); // Vector
  ADD_KVX_BUILTIN (AVGRUHX, "avgruhx", UINT16X16, UINT16X16, UINT16X16); // Vector
  ADD_KVX_BUILTIN (AVGRUW, "avgruw", UINT32, UINT32, UINT32); // Scalar
  ADD_KVX_BUILTIN (AVGRUWP, "avgruwp", UINT32X2, UINT32X2, UINT32X2); // Vector
  ADD_KVX_BUILTIN (AVGRUWQ, "avgruwq", UINT32X4, UINT32X4, UINT32X4); // Vector
  ADD_KVX_BUILTIN (AVGRUWO, "avgruwo", UINT32X8, UINT32X8, UINT32X8); // Vector

  ADD_KVX_BUILTIN (ADDSHQ, "addshq", INT16X4, INT16X4, INT16X4); // Vector
  ADD_KVX_BUILTIN (ADDSHO, "addsho", INT16X8, INT16X8, INT16X8); // Vector
  ADD_KVX_BUILTIN (ADDSHX, "addshx", INT16X16, INT16X16, INT16X16); // Vector
  ADD_KVX_BUILTIN (ADDSW, "addsw", INT32, INT32, INT32); // Scalar
  ADD_KVX_BUILTIN (ADDSWP, "addswp", INT32X2, INT32X2, INT32X2); // Vector
  ADD_KVX_BUILTIN (ADDSWQ, "addswq", INT32X4, INT32X4, INT32X4); // Vector
  ADD_KVX_BUILTIN (ADDSWO, "addswo", INT32X8, INT32X8, INT32X8); // Vector
  ADD_KVX_BUILTIN (ADDSD, "addsd", INT64, INT64, INT64); // Scalar
  ADD_KVX_BUILTIN (ADDSDP, "addsdp", INT64X2, INT64X2, INT64X2); // Vector
  ADD_KVX_BUILTIN (ADDSDQ, "addsdq", INT64X4, INT64X4, INT64X4); // Vector

  ADD_KVX_BUILTIN (ADDSHQS, "addshqs", INT16X4, INT16X4, INT16); // Vector
  ADD_KVX_BUILTIN (ADDSHOS, "addshos", INT16X8, INT16X8, INT16); // Vector
  ADD_KVX_BUILTIN (ADDSHXS, "addshxs", INT16X16, INT16X16, INT16); // Vector
  ADD_KVX_BUILTIN (ADDSWPS, "addswps", INT32X2, INT32X2, INT32); // Vector
  ADD_KVX_BUILTIN (ADDSWQS, "addswqs", INT32X4, INT32X4, INT32); // Vector
  ADD_KVX_BUILTIN (ADDSWOS, "addswos", INT32X8, INT32X8, INT32); // Vector
  ADD_KVX_BUILTIN (ADDSDPS, "addsdps", INT64X2, INT64X2, INT64); // Vector
  ADD_KVX_BUILTIN (ADDSDQS, "addsdqs", INT64X4, INT64X4, INT64); // Vector

  ADD_KVX_BUILTIN (SBFSHQ, "sbfshq", INT16X4, INT16X4, INT16X4); // Vector
  ADD_KVX_BUILTIN (SBFSHO, "sbfsho", INT16X8, INT16X8, INT16X8); // Vector
  ADD_KVX_BUILTIN (SBFSHX, "sbfshx", INT16X16, INT16X16, INT16X16); // Vector
  ADD_KVX_BUILTIN (SBFSW, "sbfsw", INT32, INT32, INT32); // Scalar
  ADD_KVX_BUILTIN (SBFSWP, "sbfswp", INT32X2, INT32X2, INT32X2); // Vector
  ADD_KVX_BUILTIN (SBFSWQ, "sbfswq", INT32X4, INT32X4, INT32X4); // Vector
  ADD_KVX_BUILTIN (SBFSWO, "sbfswo", INT32X8, INT32X8, INT32X8); // Vector
  ADD_KVX_BUILTIN (SBFSD, "sbfsd", INT64, INT64, INT64); // Scalar
  ADD_KVX_BUILTIN (SBFSDP, "sbfsdp", INT64X2, INT64X2, INT64X2); // Vector
  ADD_KVX_BUILTIN (SBFSDQ, "sbfsdq", INT64X4, INT64X4, INT64X4); // Vector

  ADD_KVX_BUILTIN (SBFSHQS, "sbfshqs", INT16X4, INT16X4, INT16); // Vector
  ADD_KVX_BUILTIN (SBFSHOS, "sbfshos", INT16X8, INT16X8, INT16); // Vector
  ADD_KVX_BUILTIN (SBFSHXS, "sbfshxs", INT16X16, INT16X16, INT16); // Vector
  ADD_KVX_BUILTIN (SBFSWPS, "sbfswps", INT32X2, INT32X2, INT32); // Vector
  ADD_KVX_BUILTIN (SBFSWQS, "sbfswqs", INT32X4, INT32X4, INT32); // Vector
  ADD_KVX_BUILTIN (SBFSWOS, "sbfswos", INT32X8, INT32X8, INT32); // Vector
  ADD_KVX_BUILTIN (SBFSDPS, "sbfsdps", INT64X2, INT64X2, INT64); // Vector
  ADD_KVX_BUILTIN (SBFSDQS, "sbfsdqs", INT64X4, INT64X4, INT64); // Vector

  ADD_KVX_BUILTIN (MINHQ, "minhq", INT16X4, INT16X4, INT16X4); // Vector
  ADD_KVX_BUILTIN (MINHO, "minho", INT16X8, INT16X8, INT16X8); // Vector
  ADD_KVX_BUILTIN (MINHX, "minhx", INT16X16, INT16X16, INT16X16); // Vector
  ADD_KVX_BUILTIN (MINW, "minw", INT32, INT32, INT32); // Scalar
  ADD_KVX_BUILTIN (MINWP, "minwp", INT32X2, INT32X2, INT32X2); // Vector
  ADD_KVX_BUILTIN (MINWQ, "minwq", INT32X4, INT32X4, INT32X4); // Vector
  ADD_KVX_BUILTIN (MINWO, "minwo", INT32X8, INT32X8, INT32X8); // Vector
  ADD_KVX_BUILTIN (MIND, "mind", INT64, INT64, INT64); // Scalar
  ADD_KVX_BUILTIN (MINDP, "mindp", INT64X2, INT64X2, INT64X2); // Vector
  ADD_KVX_BUILTIN (MINDQ, "mindq", INT64X4, INT64X4, INT64X4); // Vector

  ADD_KVX_BUILTIN (MINHQS, "minhqs", INT16X4, INT16X4, INT16); // Vector
  ADD_KVX_BUILTIN (MINHOS, "minhos", INT16X8, INT16X8, INT16); // Vector
  ADD_KVX_BUILTIN (MINHXS, "minhxs", INT16X16, INT16X16, INT16); // Vector
  ADD_KVX_BUILTIN (MINWPS, "minwps", INT32X2, INT32X2, INT32); // Vector
  ADD_KVX_BUILTIN (MINWQS, "minwqs", INT32X4, INT32X4, INT32); // Vector
  ADD_KVX_BUILTIN (MINWOS, "minwos", INT32X8, INT32X8, INT32); // Vector
  ADD_KVX_BUILTIN (MINDPS, "mindps", INT64X2, INT64X2, INT64); // Vector
  ADD_KVX_BUILTIN (MINDQS, "mindqs", INT64X4, INT64X4, INT64); // Vector

  ADD_KVX_BUILTIN (MAXHQ, "maxhq", INT16X4, INT16X4, INT16X4); // Vector
  ADD_KVX_BUILTIN (MAXHO, "maxho", INT16X8, INT16X8, INT16X8); // Vector
  ADD_KVX_BUILTIN (MAXHX, "maxhx", INT16X16, INT16X16, INT16X16); // Vector
  ADD_KVX_BUILTIN (MAXW, "maxw", INT32, INT32, INT32); // Scalar
  ADD_KVX_BUILTIN (MAXWP, "maxwp", INT32X2, INT32X2, INT32X2); // Vector
  ADD_KVX_BUILTIN (MAXWQ, "maxwq", INT32X4, INT32X4, INT32X4); // Vector
  ADD_KVX_BUILTIN (MAXWO, "maxwo", INT32X8, INT32X8, INT32X8); // Vector
  ADD_KVX_BUILTIN (MAXD, "maxd", INT64, INT64, INT64); // Scalar
  ADD_KVX_BUILTIN (MAXDP, "maxdp", INT64X2, INT64X2, INT64X2); // Vector
  ADD_KVX_BUILTIN (MAXDQ, "maxdq", INT64X4, INT64X4, INT64X4); // Vector

  ADD_KVX_BUILTIN (MAXHQS, "maxhqs", INT16X4, INT16X4, INT16); // Vector
  ADD_KVX_BUILTIN (MAXHOS, "maxhos", INT16X8, INT16X8, INT16); // Vector
  ADD_KVX_BUILTIN (MAXHXS, "maxhxs", INT16X16, INT16X16, INT16); // Vector
  ADD_KVX_BUILTIN (MAXWPS, "maxwps", INT32X2, INT32X2, INT32); // Vector
  ADD_KVX_BUILTIN (MAXWQS, "maxwqs", INT32X4, INT32X4, INT32); // Vector
  ADD_KVX_BUILTIN (MAXWOS, "maxwos", INT32X8, INT32X8, INT32); // Vector
  ADD_KVX_BUILTIN (MAXDPS, "maxdps", INT64X2, INT64X2, INT64); // Vector
  ADD_KVX_BUILTIN (MAXDQS, "maxdqs", INT64X4, INT64X4, INT64); // Vector

  ADD_KVX_BUILTIN (MINUHQ, "minuhq", UINT16X4, UINT16X4, UINT16X4); // Vector
  ADD_KVX_BUILTIN (MINUHO, "minuho", UINT16X8, UINT16X8, UINT16X8); // Vector
  ADD_KVX_BUILTIN (MINUHX, "minuhx", UINT16X16, UINT16X16, UINT16X16); // Vector
  ADD_KVX_BUILTIN (MINUW, "minuw", UINT32, UINT32, UINT32); // Scalar
  ADD_KVX_BUILTIN (MINUWP, "minuwp", UINT32X2, UINT32X2, UINT32X2); // Vector
  ADD_KVX_BUILTIN (MINUWQ, "minuwq", UINT32X4, UINT32X4, UINT32X4); // Vector
  ADD_KVX_BUILTIN (MINUWO, "minuwo", UINT32X8, UINT32X8, UINT32X8); // Vector
  ADD_KVX_BUILTIN (MINUD, "minud", UINT64, UINT64, UINT64); // Scalar
  ADD_KVX_BUILTIN (MINUDP, "minudp", UINT64X2, UINT64X2, UINT64X2); // Vector
  ADD_KVX_BUILTIN (MINUDQ, "minudq", UINT64X4, UINT64X4, UINT64X4); // Vector

  ADD_KVX_BUILTIN (MINUHQS, "minuhqs", UINT16X4, UINT16X4, UINT16); // Vector
  ADD_KVX_BUILTIN (MINUHOS, "minuhos", UINT16X8, UINT16X8, UINT16); // Vector
  ADD_KVX_BUILTIN (MINUHXS, "minuhxs", UINT16X16, UINT16X16, UINT16); // Vector
  ADD_KVX_BUILTIN (MINUWPS, "minuwps", UINT32X2, UINT32X2, UINT32); // Vector
  ADD_KVX_BUILTIN (MINUWQS, "minuwqs", UINT32X4, UINT32X4, UINT32); // Vector
  ADD_KVX_BUILTIN (MINUWOS, "minuwos", UINT32X8, UINT32X8, UINT32); // Vector
  ADD_KVX_BUILTIN (MINUDPS, "minudps", UINT64X2, UINT64X2, UINT64); // Vector
  ADD_KVX_BUILTIN (MINUDQS, "minudqs", UINT64X4, UINT64X4, UINT64); // Vector

  ADD_KVX_BUILTIN (MAXUHQ, "maxuhq", UINT16X4, UINT16X4, UINT16X4); // Vector
  ADD_KVX_BUILTIN (MAXUHO, "maxuho", UINT16X8, UINT16X8, UINT16X8); // Vector
  ADD_KVX_BUILTIN (MAXUHX, "maxuhx", UINT16X16, UINT16X16, UINT16X16); // Vector
  ADD_KVX_BUILTIN (MAXUW, "maxuw", UINT32, UINT32, UINT32); // Scalar
  ADD_KVX_BUILTIN (MAXUWP, "maxuwp", UINT32X2, UINT32X2, UINT32X2); // Vector
  ADD_KVX_BUILTIN (MAXUWQ, "maxuwq", UINT32X4, UINT32X4, UINT32X4); // Vector
  ADD_KVX_BUILTIN (MAXUWO, "maxuwo", UINT32X8, UINT32X8, UINT32X8); // Vector
  ADD_KVX_BUILTIN (MAXUD, "maxud", UINT64, UINT64, UINT64); // Scalar
  ADD_KVX_BUILTIN (MAXUDP, "maxudp", UINT64X2, UINT64X2, UINT64X2); // Vector
  ADD_KVX_BUILTIN (MAXUDQ, "maxudq", UINT64X4, UINT64X4, UINT64X4); // Vector

  ADD_KVX_BUILTIN (MAXUHQS, "maxuhqs", UINT16X4, UINT16X4, UINT16); // Vector
  ADD_KVX_BUILTIN (MAXUHOS, "maxuhos", UINT16X8, UINT16X8, UINT16); // Vector
  ADD_KVX_BUILTIN (MAXUHXS, "maxuhxs", UINT16X16, UINT16X16, UINT16); // Vector
  ADD_KVX_BUILTIN (MAXUWPS, "maxuwps", UINT32X2, UINT32X2, UINT32); // Vector
  ADD_KVX_BUILTIN (MAXUWQS, "maxuwqs", UINT32X4, UINT32X4, UINT32); // Vector
  ADD_KVX_BUILTIN (MAXUWOS, "maxuwos", UINT32X8, UINT32X8, UINT32); // Vector
  ADD_KVX_BUILTIN (MAXUDPS, "maxudps", UINT64X2, UINT64X2, UINT64); // Vector
  ADD_KVX_BUILTIN (MAXUDQS, "maxudqs", UINT64X4, UINT64X4, UINT64); // Vector

  ADD_KVX_BUILTIN (SLLHQS, "sllhqs", UINT16X4, UINT16X4, UINT32); // Vector
  ADD_KVX_BUILTIN (SLLHOS, "sllhos", UINT16X8, UINT16X8, UINT32); // Vector
  ADD_KVX_BUILTIN (SLLHXS, "sllhxs", UINT16X16, UINT16X16, UINT32); // Vector
  ADD_KVX_BUILTIN (SLLWPS, "sllwps", UINT32X2, UINT32X2, UINT32); // Vector
  ADD_KVX_BUILTIN (SLLWQS, "sllwqs", UINT32X4, UINT32X4, UINT32); // Vector
  ADD_KVX_BUILTIN (SLLWOS, "sllwos", UINT32X8, UINT32X8, UINT32); // Vector
  ADD_KVX_BUILTIN (SLLDPS, "slldps", UINT64X2, UINT64X2, UINT32); // Vector
  ADD_KVX_BUILTIN (SLLDQS, "slldqs", UINT64X4, UINT64X4, UINT32); // Vector

  ADD_KVX_BUILTIN (SLSHQS, "slshqs", INT16X4, INT16X4, UINT32); // Vector
  ADD_KVX_BUILTIN (SLSHOS, "slshos", INT16X8, INT16X8, UINT32); // Vector
  ADD_KVX_BUILTIN (SLSHXS, "slshxs", INT16X16, INT16X16, UINT32); // Vector
  ADD_KVX_BUILTIN (SLSWPS, "slswps", INT32X2, INT32X2, UINT32); // Vector
  ADD_KVX_BUILTIN (SLSWQS, "slswqs", INT32X4, INT32X4, UINT32); // Vector
  ADD_KVX_BUILTIN (SLSWOS, "slswos", INT32X8, INT32X8, UINT32); // Vector
  ADD_KVX_BUILTIN (SLSDPS, "slsdps", INT64X2, INT64X2, UINT32); // Vector
  ADD_KVX_BUILTIN (SLSDQS, "slsdqs", INT64X4, INT64X4, UINT32); // Vector

  ADD_KVX_BUILTIN (SRAHQS, "srahqs", INT16X4, INT16X4, UINT32); // Vector
  ADD_KVX_BUILTIN (SRAHOS, "srahos", INT16X8, INT16X8, UINT32); // Vector
  ADD_KVX_BUILTIN (SRAHXS, "srahxs", INT16X16, INT16X16, UINT32); // Vector
  ADD_KVX_BUILTIN (SRAWPS, "srawps", INT32X2, INT32X2, UINT32); // Vector
  ADD_KVX_BUILTIN (SRAWQS, "srawqs", INT32X4, INT32X4, UINT32); // Vector
  ADD_KVX_BUILTIN (SRAWOS, "srawos", INT32X8, INT32X8, UINT32); // Vector
  ADD_KVX_BUILTIN (SRADPS, "sradps", INT64X2, INT64X2, UINT32); // Vector
  ADD_KVX_BUILTIN (SRADQS, "sradqs", INT64X4, INT64X4, UINT32); // Vector

  ADD_KVX_BUILTIN (SRLHQS, "srlhqs", UINT16X4, UINT16X4, UINT32); // Vector
  ADD_KVX_BUILTIN (SRLHOS, "srlhos", UINT16X8, UINT16X8, UINT32); // Vector
  ADD_KVX_BUILTIN (SRLHXS, "srlhxs", UINT16X16, UINT16X16, UINT32); // Vector
  ADD_KVX_BUILTIN (SRLWPS, "srlwps", UINT32X2, UINT32X2, UINT32); // Vector
  ADD_KVX_BUILTIN (SRLWQS, "srlwqs", UINT32X4, UINT32X4, UINT32); // Vector
  ADD_KVX_BUILTIN (SRLWOS, "srlwos", UINT32X8, UINT32X8, UINT32); // Vector
  ADD_KVX_BUILTIN (SRLDPS, "srldps", UINT64X2, UINT64X2, UINT32); // Vector
  ADD_KVX_BUILTIN (SRLDQS, "srldqs", UINT64X4, UINT64X4, UINT32); // Vector

  ADD_KVX_BUILTIN (SRSHQS, "srshqs", INT16X4, INT16X4, UINT32); // Vector
  ADD_KVX_BUILTIN (SRSHOS, "srshos", INT16X8, INT16X8, UINT32); // Vector
  ADD_KVX_BUILTIN (SRSHXS, "srshxs", INT16X16, INT16X16, UINT32); // Vector
  ADD_KVX_BUILTIN (SRSWPS, "srswps", INT32X2, INT32X2, UINT32); // Vector
  ADD_KVX_BUILTIN (SRSWQS, "srswqs", INT32X4, INT32X4, UINT32); // Vector
  ADD_KVX_BUILTIN (SRSWOS, "srswos", INT32X8, INT32X8, UINT32); // Vector
  ADD_KVX_BUILTIN (SRSDPS, "srsdps", INT64X2, INT64X2, UINT32); // Vector
  ADD_KVX_BUILTIN (SRSDQS, "srsdqs", INT64X4, INT64X4, UINT32); // Vector

  ADD_KVX_BUILTIN (CLSW, "clsw", UINT32, UINT32); // Scalar
  ADD_KVX_BUILTIN (CLSWP, "clswp", UINT32X2, UINT32X2); // Vector
  ADD_KVX_BUILTIN (CLSWQ, "clswq", UINT32X4, UINT32X4); // Vector
  ADD_KVX_BUILTIN (CLSWO, "clswo", UINT32X8, UINT32X8); // Vector
  ADD_KVX_BUILTIN (CLSD, "clsd", UINT64, UINT64); // Scalar
  ADD_KVX_BUILTIN (CLSDP, "clsdp", UINT64X2, UINT64X2); // Vector
  ADD_KVX_BUILTIN (CLSDQ, "clsdq", UINT64X4, UINT64X4); // Vector

  ADD_KVX_BUILTIN (CLZW, "clzw", UINT32, UINT32); // Scalar
  ADD_KVX_BUILTIN (CLZWP, "clzwp", UINT32X2, UINT32X2); // Vector
  ADD_KVX_BUILTIN (CLZWQ, "clzwq", UINT32X4, UINT32X4); // Vector
  ADD_KVX_BUILTIN (CLZWO, "clzwo", UINT32X8, UINT32X8); // Vector
  ADD_KVX_BUILTIN (CLZD, "clzd", UINT64, UINT64); // Scalar
  ADD_KVX_BUILTIN (CLZDP, "clzdp", UINT64X2, UINT64X2); // Vector
  ADD_KVX_BUILTIN (CLZDQ, "clzdq", UINT64X4, UINT64X4); // Vector

  ADD_KVX_BUILTIN (CTZW, "ctzw", UINT32, UINT32); // Scalar
  ADD_KVX_BUILTIN (CTZWP, "ctzwp", UINT32X2, UINT32X2); // Vector
  ADD_KVX_BUILTIN (CTZWQ, "ctzwq", UINT32X4, UINT32X4); // Vector
  ADD_KVX_BUILTIN (CTZWO, "ctzwo", UINT32X8, UINT32X8); // Vector
  ADD_KVX_BUILTIN (CTZD, "ctzd", UINT64, UINT64); // Scalar
  ADD_KVX_BUILTIN (CTZDP, "ctzdp", UINT64X2, UINT64X2); // Vector
  ADD_KVX_BUILTIN (CTZDQ, "ctzdq", UINT64X4, UINT64X4); // Vector

  ADD_KVX_BUILTIN (CBSW, "cbsw", UINT32, UINT32); // Scalar
  ADD_KVX_BUILTIN (CBSWP, "cbswp", UINT32X2, UINT32X2); // Vector
  ADD_KVX_BUILTIN (CBSWQ, "cbswq", UINT32X4, UINT32X4); // Vector
  ADD_KVX_BUILTIN (CBSWO, "cbswo", UINT32X8, UINT32X8); // Vector
  ADD_KVX_BUILTIN (CBSD, "cbsd", UINT64, UINT64); // Scalar
  ADD_KVX_BUILTIN (CBSDP, "cbsdp", UINT64X2, UINT64X2); // Vector
  ADD_KVX_BUILTIN (CBSDQ, "cbsdq", UINT64X4, UINT64X4); // Vector

  ADD_KVX_BUILTIN (AWAIT, "await", VOID); // Control
  ADD_KVX_BUILTIN (BARRIER, "barrier", VOID); // Control
  ADD_KVX_BUILTIN (ACSWAPW, "acswapw", UINT32, VPTR, UINT32, UINT32); // Atomic
  ADD_KVX_BUILTIN (ACSWAPD, "acswapd", UINT64, VPTR, UINT64, UINT64); // Atomic
  ADD_KVX_BUILTIN (ALADDD, "aladdd", UINT64, VPTR, UINT64); // Atomic
  ADD_KVX_BUILTIN (ALADDW, "aladdw", UINT32, VPTR, UINT32); // Atomic
  ADD_KVX_BUILTIN (ALADDD, "afaddd", UINT64, VPTR, UINT64); // Deprecated
  ADD_KVX_BUILTIN (ALADDW, "afaddw", UINT32, VPTR, UINT32); // Deprecated
  ADD_KVX_BUILTIN (ALCLRD, "alclrd", UINT64, VPTR); // Atomic
  ADD_KVX_BUILTIN (ALCLRW, "alclrw", UINT32, VPTR); // Atomic
  ADD_KVX_BUILTIN (DINVAL, "dinval", VOID); // Memory
  ADD_KVX_BUILTIN (DINVALL, "dinvall", VOID, CVPTR); // Memory
  ADD_KVX_BUILTIN (DTOUCHL, "dtouchl", VOID, CVPTR); // Memory
  ADD_KVX_BUILTIN (DZEROL, "dzerol", VOID, VPTR); // Deprecated
  ADD_KVX_BUILTIN (FENCE, "fence", VOID); // Memory

  ADD_KVX_BUILTIN (SELECTHQ, "selecthq", INT16X4, INT16X4, INT16X4, INT16X4, SIMDCOND); // Vector
  ADD_KVX_BUILTIN (SELECTHO, "selectho", INT16X8, INT16X8, INT16X8, INT16X8, SIMDCOND); // Vector
  ADD_KVX_BUILTIN (SELECTHX, "selecthx", INT16X16, INT16X16, INT16X16, INT16X16, SIMDCOND); // Vector
  ADD_KVX_BUILTIN (SELECTWP, "selectwp", INT32X2, INT32X2, INT32X2, INT32X2, SIMDCOND); // Vector
  ADD_KVX_BUILTIN (SELECTWQ, "selectwq", INT32X4, INT32X4, INT32X4, INT32X4, SIMDCOND); // Vector
  ADD_KVX_BUILTIN (SELECTWO, "selectwo", INT32X8, INT32X8, INT32X8, INT32X8, SIMDCOND); // Vector
  ADD_KVX_BUILTIN (SELECTDP, "selectdp", INT64X2, INT64X2, INT64X2, INT64X2, SIMDCOND); // Vector
  ADD_KVX_BUILTIN (SELECTDQ, "selectdq", INT64X4, INT64X4, INT64X4, INT64X4, SIMDCOND); // Vector
  ADD_KVX_BUILTIN (SELECTFWP, "selectfwp", FLOAT32X2, FLOAT32X2, FLOAT32X2, INT32X2, SIMDCOND); // Vector
  ADD_KVX_BUILTIN (SELECTFWQ, "selectfwq", FLOAT32X4, FLOAT32X4, FLOAT32X4, INT32X4, SIMDCOND); // Vector
  ADD_KVX_BUILTIN (SELECTFWO, "selectfwo", FLOAT32X8, FLOAT32X8, FLOAT32X8, INT32X8, SIMDCOND); // Vector
  ADD_KVX_BUILTIN (SELECTFDP, "selectfdp", FLOAT64X2, FLOAT64X2, FLOAT64X2, INT64X2, SIMDCOND); // Vector
  ADD_KVX_BUILTIN (SELECTFDQ, "selectfdq", FLOAT64X4, FLOAT64X4, FLOAT64X4, INT64X4, SIMDCOND); // Vector

  ADD_KVX_BUILTIN (COPYSIGNW, "copysignw", FLOAT32, FLOAT32, FLOAT32); // Scalar
  ADD_KVX_BUILTIN (COPYSIGNWP, "copysignwp", FLOAT32X2, FLOAT32X2, FLOAT32X2); // Vector
  ADD_KVX_BUILTIN (COPYSIGNWQ, "copysignwq", FLOAT32X4, FLOAT32X4, FLOAT32X4); // Vector
  ADD_KVX_BUILTIN (COPYSIGNWO, "copysignwo", FLOAT32X8, FLOAT32X8, FLOAT32X8); // Vector
  ADD_KVX_BUILTIN (COPYSIGND, "copysignd", FLOAT64, FLOAT64, FLOAT64); // Scalar
  ADD_KVX_BUILTIN (COPYSIGNDP, "copysigndp", FLOAT64X2, FLOAT64X2, FLOAT64X2); // Vector
  ADD_KVX_BUILTIN (COPYSIGNDQ, "copysigndq", FLOAT64X4, FLOAT64X4, FLOAT64X4); // Vector

  ADD_KVX_BUILTIN (FMINW, "fminw", FLOAT32, FLOAT32, FLOAT32); // Scalar
  ADD_KVX_BUILTIN (FMINWP, "fminwp", FLOAT32X2, FLOAT32X2, FLOAT32X2); // Vector
  ADD_KVX_BUILTIN (FMINWQ, "fminwq", FLOAT32X4, FLOAT32X4, FLOAT32X4); // Vector
  ADD_KVX_BUILTIN (FMINWO, "fminwo", FLOAT32X8, FLOAT32X8, FLOAT32X8); // Vector
  ADD_KVX_BUILTIN (FMIND, "fmind", FLOAT64, FLOAT64, FLOAT64); // Scalar
  ADD_KVX_BUILTIN (FMINDP, "fmindp", FLOAT64X2, FLOAT64X2, FLOAT64X2); // Vector
  ADD_KVX_BUILTIN (FMINDQ, "fmindq", FLOAT64X4, FLOAT64X4, FLOAT64X4); // Vector

  ADD_KVX_BUILTIN (FMINWPS, "fminwps", FLOAT32X2, FLOAT32X2, FLOAT32); // Vector
  ADD_KVX_BUILTIN (FMINWQS, "fminwqs", FLOAT32X4, FLOAT32X4, FLOAT32); // Vector
  ADD_KVX_BUILTIN (FMINWOS, "fminwos", FLOAT32X8, FLOAT32X8, FLOAT32); // Vector
  ADD_KVX_BUILTIN (FMINDPS, "fmindps", FLOAT64X2, FLOAT64X2, FLOAT64); // Vector
  ADD_KVX_BUILTIN (FMINDQS, "fmindqs", FLOAT64X4, FLOAT64X4, FLOAT64); // Vector

  ADD_KVX_BUILTIN (FMAXW, "fmaxw", FLOAT32, FLOAT32, FLOAT32); // Scalar
  ADD_KVX_BUILTIN (FMAXWP, "fmaxwp", FLOAT32X2, FLOAT32X2, FLOAT32X2); // Vector
  ADD_KVX_BUILTIN (FMAXWQ, "fmaxwq", FLOAT32X4, FLOAT32X4, FLOAT32X4); // Vector
  ADD_KVX_BUILTIN (FMAXWO, "fmaxwo", FLOAT32X8, FLOAT32X8, FLOAT32X8); // Vector
  ADD_KVX_BUILTIN (FMAXD, "fmaxd", FLOAT64, FLOAT64, FLOAT64); // Scalar
  ADD_KVX_BUILTIN (FMAXDP, "fmaxdp", FLOAT64X2, FLOAT64X2, FLOAT64X2); // Vector
  ADD_KVX_BUILTIN (FMAXDQ, "fmaxdq", FLOAT64X4, FLOAT64X4, FLOAT64X4); // Vector

  ADD_KVX_BUILTIN (FMAXWPS, "fmaxwps", FLOAT32X2, FLOAT32X2, FLOAT32); // Vector
  ADD_KVX_BUILTIN (FMAXWQS, "fmaxwqs", FLOAT32X4, FLOAT32X4, FLOAT32); // Vector
  ADD_KVX_BUILTIN (FMAXWOS, "fmaxwos", FLOAT32X8, FLOAT32X8, FLOAT32); // Vector
  ADD_KVX_BUILTIN (FMAXDPS, "fmaxdps", FLOAT64X2, FLOAT64X2, FLOAT64); // Vector
  ADD_KVX_BUILTIN (FMAXDQS, "fmaxdqs", FLOAT64X4, FLOAT64X4, FLOAT64); // Vector

  ADD_KVX_BUILTIN (FNEGW, "fnegw", FLOAT32, FLOAT32); // Scalar
  ADD_KVX_BUILTIN (FNEGWP, "fnegwp", FLOAT32X2, FLOAT32X2); // Vector
  ADD_KVX_BUILTIN (FNEGWQ, "fnegwq", FLOAT32X4, FLOAT32X4); // Vector
  ADD_KVX_BUILTIN (FNEGWO, "fnegwo", FLOAT32X8, FLOAT32X8); // Vector
  ADD_KVX_BUILTIN (FNEGD, "fnegd", FLOAT64, FLOAT64); // Scalar
  ADD_KVX_BUILTIN (FNEGDP, "fnegdp", FLOAT64X2, FLOAT64X2); // Vector
  ADD_KVX_BUILTIN (FNEGDQ, "fnegdq", FLOAT64X4, FLOAT64X4); // Vector

  ADD_KVX_BUILTIN (FABSW, "fabsw", FLOAT32, FLOAT32); // Scalar
  ADD_KVX_BUILTIN (FABSWP, "fabswp", FLOAT32X2, FLOAT32X2); // Vector
  ADD_KVX_BUILTIN (FABSWQ, "fabswq", FLOAT32X4, FLOAT32X4); // Vector
  ADD_KVX_BUILTIN (FABSWO, "fabswo", FLOAT32X8, FLOAT32X8); // Vector
  ADD_KVX_BUILTIN (FABSD, "fabsd", FLOAT64, FLOAT64); // Scalar
  ADD_KVX_BUILTIN (FABSDP, "fabsdp", FLOAT64X2, FLOAT64X2); // Vector
  ADD_KVX_BUILTIN (FABSDQ, "fabsdq", FLOAT64X4, FLOAT64X4); // Vector

  ADD_KVX_BUILTIN (FRECW, "finvw", FLOAT32, FLOAT32, ROUNDING); // Deprecated
  ADD_KVX_BUILTIN (FRECW, "frecw", FLOAT32, FLOAT32, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FRECWP, "frecwp", FLOAT32X2, FLOAT32X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FRECWQ, "frecwq", FLOAT32X4, FLOAT32X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FRECWO, "frecwo", FLOAT32X8, FLOAT32X8, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FRSRW, "fisrw", FLOAT32, FLOAT32, ROUNDING); // Deprecated
  ADD_KVX_BUILTIN (FRSRW, "frsrw", FLOAT32, FLOAT32, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FRSRWP, "frsrwp", FLOAT32X2, FLOAT32X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FRSRWQ, "frsrwq", FLOAT32X4, FLOAT32X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FRSRWO, "frsrwo", FLOAT32X8, FLOAT32X8, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FADDW, "faddw", FLOAT32, FLOAT32, FLOAT32, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FADDWP, "faddwp", FLOAT32X2, FLOAT32X2, FLOAT32X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FADDWQ, "faddwq", FLOAT32X4, FLOAT32X4, FLOAT32X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FADDWO, "faddwo", FLOAT32X8, FLOAT32X8, FLOAT32X8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FADDD, "faddd", FLOAT64, FLOAT64, FLOAT64, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FADDDP, "fadddp", FLOAT64X2, FLOAT64X2, FLOAT64X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FADDDQ, "fadddq", FLOAT64X4, FLOAT64X4, FLOAT64X4, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FADDCWC, "faddcwc", FLOAT32X2, FLOAT32X2, FLOAT32X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FADDCWCP, "faddcwcp", FLOAT32X4, FLOAT32X4, FLOAT32X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FADDCWCQ, "faddcwcq", FLOAT32X8, FLOAT32X8, FLOAT32X8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FADDCDC, "faddcdc", FLOAT64X2, FLOAT64X2, FLOAT64X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FADDCDCP, "faddcdcp", FLOAT64X4, FLOAT64X4, FLOAT64X4, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FSBFW, "fsbfw", FLOAT32, FLOAT32, FLOAT32, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FSBFWP, "fsbfwp", FLOAT32X2, FLOAT32X2, FLOAT32X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FSBFWQ, "fsbfwq", FLOAT32X4, FLOAT32X4, FLOAT32X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FSBFWO, "fsbfwo", FLOAT32X8, FLOAT32X8, FLOAT32X8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FSBFD, "fsbfd", FLOAT64, FLOAT64, FLOAT64, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FSBFDP, "fsbfdp", FLOAT64X2, FLOAT64X2, FLOAT64X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FSBFDQ, "fsbfdq", FLOAT64X4, FLOAT64X4, FLOAT64X4, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FSBFCWC, "fsbfcwc", FLOAT32X2, FLOAT32X2, FLOAT32X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FSBFCWCP, "fsbfcwcp", FLOAT32X4, FLOAT32X4, FLOAT32X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FSBFCWCQ, "fsbfcwcq", FLOAT32X8, FLOAT32X8, FLOAT32X8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FSBFCDC, "fsbfcdc", FLOAT64X2, FLOAT64X2, FLOAT64X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FSBFCDCP, "fsbfcdcp", FLOAT64X4, FLOAT64X4, FLOAT64X4, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FMULW, "fmulw", FLOAT32, FLOAT32, FLOAT32, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FMULWP, "fmulwp", FLOAT32X2, FLOAT32X2, FLOAT32X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMULWQ, "fmulwq", FLOAT32X4, FLOAT32X4, FLOAT32X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMULWO, "fmulwo", FLOAT32X8, FLOAT32X8, FLOAT32X8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMULD, "fmuld", FLOAT64, FLOAT64, FLOAT64, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FMULDP, "fmuldp", FLOAT64X2, FLOAT64X2, FLOAT64X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMULDQ, "fmuldq", FLOAT64X4, FLOAT64X4, FLOAT64X4, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FMULWD, "fmulwd", FLOAT64, FLOAT32, FLOAT32, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FMULWC, "fmulwc", FLOAT32X2, FLOAT32X2, FLOAT32X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMULWCP, "fmulwcp", FLOAT32X4, FLOAT32X4, FLOAT32X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMULWCQ, "fmulwcq", FLOAT32X8, FLOAT32X8, FLOAT32X8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMULCWC, "fmulcwc", FLOAT32X2, FLOAT32X2, FLOAT32X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMULCWCP, "fmulcwcp", FLOAT32X4, FLOAT32X4, FLOAT32X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMULCWCQ, "fmulcwcq", FLOAT32X8, FLOAT32X8, FLOAT32X8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMULDC, "fmuldc", FLOAT64X2, FLOAT64X2, FLOAT64X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMULDCP, "fmuldcp", FLOAT64X4, FLOAT64X4, FLOAT64X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMULCDC, "fmulcdc", FLOAT64X2, FLOAT64X2, FLOAT64X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMULCDCP, "fmulcdcp", FLOAT64X4, FLOAT64X4, FLOAT64X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FMM212W, "fmm212w", FLOAT32X4, FLOAT32X2, FLOAT32X2, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FFMAW, "ffmaw", FLOAT32, FLOAT32, FLOAT32, FLOAT32, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FFMAWP, "ffmawp", FLOAT32X2, FLOAT32X2, FLOAT32X2, FLOAT32X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FFMAWQ, "ffmawq", FLOAT32X4, FLOAT32X4, FLOAT32X4, FLOAT32X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FFMAWO, "ffmawo", FLOAT32X8, FLOAT32X8, FLOAT32X8, FLOAT32X8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FFMAD, "ffmad", FLOAT64, FLOAT64, FLOAT64, FLOAT64, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FFMADP, "ffmadp", FLOAT64X2, FLOAT64X2, FLOAT64X2, FLOAT64X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FFMADQ, "ffmadq", FLOAT64X4, FLOAT64X4, FLOAT64X4, FLOAT64X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FFMAWD, "ffmawd", FLOAT64, FLOAT32, FLOAT32, FLOAT64, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FMMA212W, "fmma212w", FLOAT32X4, FLOAT32X2, FLOAT32X2, FLOAT32X4, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FFMSW, "ffmsw", FLOAT32, FLOAT32, FLOAT32, FLOAT32, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FFMSWP, "ffmswp", FLOAT32X2, FLOAT32X2, FLOAT32X2, FLOAT32X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FFMSWQ, "ffmswq", FLOAT32X4, FLOAT32X4, FLOAT32X4, FLOAT32X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FFMSWO, "ffmswo", FLOAT32X8, FLOAT32X8, FLOAT32X8, FLOAT32X8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FFMSD, "ffmsd", FLOAT64, FLOAT64, FLOAT64, FLOAT64, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FFMSDP, "ffmsdp", FLOAT64X2, FLOAT64X2, FLOAT64X2, FLOAT64X2, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FFMSDQ, "ffmsdq", FLOAT64X4, FLOAT64X4, FLOAT64X4, FLOAT64X4, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FFMSWD, "ffmswd", FLOAT64, FLOAT32, FLOAT32, FLOAT64, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FMMS212W, "fmms212w", FLOAT32X4, FLOAT32X2, FLOAT32X2, FLOAT32X4, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FLOATW, "floatw", FLOAT32, INT32, UINT8, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FLOATWP, "floatwp", FLOAT32X2, INT32X2, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FLOATWQ, "floatwq", FLOAT32X4, INT32X4, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FLOATWO, "floatwo", FLOAT32X8, INT32X8, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FLOATD, "floatd", FLOAT64, INT64, UINT8, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FLOATDP, "floatdp", FLOAT64X2, INT64X2, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FLOATDQ, "floatdq", FLOAT64X4, INT64X4, UINT8, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FLOATUW, "floatuw", FLOAT32, UINT32, UINT8, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FLOATUWP, "floatuwp", FLOAT32X2, UINT32X2, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FLOATUWQ, "floatuwq", FLOAT32X4, UINT32X4, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FLOATUWO, "floatuwo", FLOAT32X8, UINT32X8, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FLOATUD, "floatud", FLOAT64, UINT64, UINT8, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FLOATUDP, "floatudp", FLOAT64X2, UINT64X2, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FLOATUDQ, "floatudq", FLOAT64X4, UINT64X4, UINT8, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FIXEDW, "fixedw", INT32, FLOAT32, UINT8, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FIXEDWP, "fixedwp", INT32X2, FLOAT32X2, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FIXEDWQ, "fixedwq", INT32X4, FLOAT32X4, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FIXEDWO, "fixedwo", INT32X8, FLOAT32X8, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FIXEDD, "fixedd", INT64, FLOAT64, UINT8, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FIXEDDP, "fixeddp", INT64X2, FLOAT64X2, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FIXEDDQ, "fixeddq", INT64X4, FLOAT64X4, UINT8, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FIXEDUW, "fixeduw", UINT32, FLOAT32, UINT8, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FIXEDUWP, "fixeduwp", UINT32X2, FLOAT32X2, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FIXEDUWQ, "fixeduwq", UINT32X4, FLOAT32X4, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FIXEDUWO, "fixeduwo", UINT32X8, FLOAT32X8, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FIXEDUD, "fixedud", UINT64, FLOAT64, UINT8, ROUNDING); // Scalar
  ADD_KVX_BUILTIN (FIXEDUDP, "fixedudp", UINT64X2, FLOAT64X2, UINT8, ROUNDING); // Vector
  ADD_KVX_BUILTIN (FIXEDUDQ, "fixedudq", UINT64X4, FLOAT64X4, UINT8, ROUNDING); // Vector

  ADD_KVX_BUILTIN (FCONJWC, "fconjwc", FLOAT32X2, FLOAT32X2); // Vector
  ADD_KVX_BUILTIN (FCONJWCP, "fconjwcp", FLOAT32X4, FLOAT32X4); // Vector
  ADD_KVX_BUILTIN (FCONJWCQ, "fconjwcq", FLOAT32X8, FLOAT32X8); // Vector
  ADD_KVX_BUILTIN (FCONJDC, "fconjdc", FLOAT64X2, FLOAT64X2); // Vector
  ADD_KVX_BUILTIN (FCONJDCP, "fconjdcp", FLOAT64X4, FLOAT64X4); // Vector

  ADD_KVX_BUILTIN (FCDIVW, "fcdivw", FLOAT32, FLOAT32, FLOAT32, SILENT); // Scalar
  ADD_KVX_BUILTIN (FCDIVWP, "fcdivwp", FLOAT32X2, FLOAT32X2, FLOAT32X2, SILENT); // Vector
  ADD_KVX_BUILTIN (FCDIVWQ, "fcdivwq", FLOAT32X4, FLOAT32X4, FLOAT32X4, SILENT); // Vector
  ADD_KVX_BUILTIN (FCDIVWO, "fcdivwo", FLOAT32X8, FLOAT32X8, FLOAT32X8, SILENT); // Vector
  ADD_KVX_BUILTIN (FCDIVD, "fcdivd", FLOAT64, FLOAT64, FLOAT64, SILENT); // Scalar
  ADD_KVX_BUILTIN (FCDIVDP, "fcdivdp", FLOAT64X2, FLOAT64X2, FLOAT64X2, SILENT); // Vector
  ADD_KVX_BUILTIN (FCDIVDQ, "fcdivdq", FLOAT64X4, FLOAT64X4, FLOAT64X4, SILENT); // Vector

  ADD_KVX_BUILTIN (FSDIVW, "fsdivw", FLOAT32, FLOAT32, FLOAT32, SILENT); // Scalar
  ADD_KVX_BUILTIN (FSDIVWP, "fsdivwp", FLOAT32X2, FLOAT32X2, FLOAT32X2, SILENT); // Vector
  ADD_KVX_BUILTIN (FSDIVWQ, "fsdivwq", FLOAT32X4, FLOAT32X4, FLOAT32X4, SILENT); // Vector
  ADD_KVX_BUILTIN (FSDIVWO, "fsdivwo", FLOAT32X8, FLOAT32X8, FLOAT32X8, SILENT); // Vector
  ADD_KVX_BUILTIN (FSDIVD, "fsdivd", FLOAT64, FLOAT64, FLOAT64, SILENT); // Scalar
  ADD_KVX_BUILTIN (FSDIVDP, "fsdivdp", FLOAT64X2, FLOAT64X2, FLOAT64X2, SILENT); // Vector
  ADD_KVX_BUILTIN (FSDIVDQ, "fsdivdq", FLOAT64X4, FLOAT64X4, FLOAT64X4, SILENT); // Vector

  ADD_KVX_BUILTIN (FSRECW, "fsrecw", FLOAT32, FLOAT32, SILENT); // Scalar
  ADD_KVX_BUILTIN (FSRECWP, "fsrecwp", FLOAT32X2, FLOAT32X2, SILENT); // Vector
  ADD_KVX_BUILTIN (FSRECWQ, "fsrecwq", FLOAT32X4, FLOAT32X4, SILENT); // Vector
  ADD_KVX_BUILTIN (FSRECWO, "fsrecwo", FLOAT32X8, FLOAT32X8, SILENT); // Vector
  ADD_KVX_BUILTIN (FSRECD, "fsrecd", FLOAT64, FLOAT64, SILENT); // Scalar
  ADD_KVX_BUILTIN (FSRECDP, "fsrecdp", FLOAT64X2, FLOAT64X2, SILENT); // Vector
  ADD_KVX_BUILTIN (FSRECDQ, "fsrecdq", FLOAT64X4, FLOAT64X4, SILENT); // Vector

  ADD_KVX_BUILTIN (FSRSRW, "fsrsrw", FLOAT32, FLOAT32, SILENT); // Scalar
  ADD_KVX_BUILTIN (FSRSRWP, "fsrsrwp", FLOAT32X2, FLOAT32X2, SILENT); // Vector
  ADD_KVX_BUILTIN (FSRSRWQ, "fsrsrwq", FLOAT32X4, FLOAT32X4, SILENT); // Vector
  ADD_KVX_BUILTIN (FSRSRWO, "fsrsrwo", FLOAT32X8, FLOAT32X8, SILENT); // Vector
  ADD_KVX_BUILTIN (FSRSRD, "fsrsrd", FLOAT64, FLOAT64, SILENT); // Scalar
  ADD_KVX_BUILTIN (FSRSRDP, "fsrsrdp", FLOAT64X2, FLOAT64X2, SILENT); // Vector
  ADD_KVX_BUILTIN (FSRSRDQ, "fsrsrdq", FLOAT64X4, FLOAT64X4, SILENT); // Vector

  ADD_KVX_BUILTIN (GET, "get", UINT64, INT32); // Control
  ADD_KVX_BUILTIN (WFXL, "wfxl", VOID, UINT8, UINT64); // Control
  ADD_KVX_BUILTIN (WFXM, "wfxm", VOID, UINT8, UINT64); // Control
  ADD_KVX_BUILTIN (IINVAL, "iinval", VOID); // Memory
  ADD_KVX_BUILTIN (IINVALS, "iinvals", VOID, CVPTR); // Memory
  ADD_KVX_BUILTIN (LBSU, "lbsu", INT8, CVPTR); // Deprecated
  ADD_KVX_BUILTIN (LBZU, "lbzu", UINT8, CVPTR); // Deprecated
  ADD_KVX_BUILTIN (LHSU, "lhsu", INT16, CVPTR); // Deprecated
  ADD_KVX_BUILTIN (LHZU, "lhzu", UINT16, CVPTR); // Deprecated
  ADD_KVX_BUILTIN (LDU, "ldu", UINT64, CVPTR); // Deprecated
  ADD_KVX_BUILTIN (LWZU, "lwzu", UINT32, CVPTR); // Deprecated
  ADD_KVX_BUILTIN (SET, "set", VOID, INT32, UINT64); // Control
  ADD_KVX_BUILTIN (SLEEP, "sleep", VOID); // Control
  ADD_KVX_BUILTIN (STOP, "stop", VOID); // Control
  ADD_KVX_BUILTIN (SYNCGROUP, "syncgroup", VOID, UINT64); // Control
  ADD_KVX_BUILTIN (TLBDINVAL, "tlbdinval", VOID); // Memory
  ADD_KVX_BUILTIN (TLBIINVAL, "tlbiinval", VOID); // Memory
  ADD_KVX_BUILTIN (TLBPROBE, "tlbprobe", VOID); // Memory
  ADD_KVX_BUILTIN (TLBREAD, "tlbread", VOID); // Memory
  ADD_KVX_BUILTIN (TLBWRITE, "tlbwrite", VOID); // Memory
  ADD_KVX_BUILTIN (WAITIT, "waitit", UINT32, UINT32); // Control

  ADD_KVX_BUILTIN (SATD, "satd", INT64, INT64, UINT8); // Scalar
  ADD_KVX_BUILTIN (SATUD, "satud", UINT64, INT64, UINT8); // Scalar
  ADD_KVX_BUILTIN (STSUW, "stsuw", UINT32, UINT32, UINT32); // Scalar
  ADD_KVX_BUILTIN (STSUD, "stsud", UINT64, UINT64, UINT64); // Scalar
  ADD_KVX_BUILTIN (SBMM8, "sbmm8", UINT64, UINT64, UINT64); // Scalar
  ADD_KVX_BUILTIN (SBMMT8, "sbmmt8", UINT64, UINT64, UINT64); // Scalar
  ADD_KVX_BUILTIN (FWIDENLHW, "fwidenlhw", FLOAT32, UINT32); // Scalar
  ADD_KVX_BUILTIN (FWIDENMHW, "fwidenmhw", FLOAT32, UINT32); // Scalar
  ADD_KVX_BUILTIN (FNARROWWH, "fnarrowwh", UINT16, FLOAT32); // Scalar

  ADD_KVX_BUILTIN (LBZ, "lbz", INT64, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LBS, "lbs", INT64, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LHZ, "lhz", INT64, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LHS, "lhs", INT64, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LWZ, "lwz", INT64, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LWS, "lws", INT64, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LD, "ld", INT64, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LQ, "lq", INT128, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LWF, "lwf", FLOAT32, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LDF, "ldf", FLOAT64, CVPTR, BYPASS, BOOL); // Memory

  ADD_KVX_BUILTIN (LDBO, "ldbo", INT8X8, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LDHQ, "ldhq", INT16X4, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LDWP, "ldwp", INT32X2, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LDFWP, "ldfwp", FLOAT32X2, CVPTR, BYPASS, BOOL); // Memory

  ADD_KVX_BUILTIN (SDBO, "sdbo", VOID, VPTR, INT8X8, BOOL); // Memory
  ADD_KVX_BUILTIN (SDHQ, "sdhq", VOID, VPTR, INT16X4, BOOL); // Memory
  ADD_KVX_BUILTIN (SDWP, "sdwp", VOID, VPTR, INT32X2, BOOL); // Memory
  ADD_KVX_BUILTIN (SDFWP, "sdfwp", VOID, VPTR, FLOAT32X2, BOOL); // Memory

  ADD_KVX_BUILTIN (LQBX, "lqbx", INT8X16, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LQHO, "lqho", INT16X8, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LQWQ, "lqwq", INT32X4, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LQDP, "lqdp", INT64X2, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LQFWQ, "lqfwq", FLOAT32X4, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LQFDP, "lqfdp", FLOAT64X2, CVPTR, BYPASS, BOOL); // Memory

  ADD_KVX_BUILTIN (SQBX, "sqbx", VOID, VPTR, INT8X16, BOOL); // Memory
  ADD_KVX_BUILTIN (SQHO, "sqho", VOID, VPTR, INT16X8, BOOL); // Memory
  ADD_KVX_BUILTIN (SQWQ, "sqwq", VOID, VPTR, INT32X4, BOOL); // Memory
  ADD_KVX_BUILTIN (SQDP, "sqdp", VOID, VPTR, INT64X2, BOOL); // Memory
  ADD_KVX_BUILTIN (SQFWQ, "sqfwq", VOID, VPTR, FLOAT32X4, BOOL); // Memory
  ADD_KVX_BUILTIN (SQFDP, "sqfdp", VOID, VPTR, FLOAT64X2, BOOL); // Memory

  ADD_KVX_BUILTIN (LOBV, "lobv", INT8X32, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LOHX, "lohx", INT16X16, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LOWO, "lowo", INT32X8, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LODQ, "lodq", INT64X4, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LOFWO, "lofwo", FLOAT32X8, CVPTR, BYPASS, BOOL); // Memory
  ADD_KVX_BUILTIN (LOFDQ, "lofdq", FLOAT64X4, CVPTR, BYPASS, BOOL); // Memory

  ADD_KVX_BUILTIN (SOBV, "sobv", VOID, VPTR, INT8X32, BOOL); // Memory
  ADD_KVX_BUILTIN (SOHX, "sohx", VOID, VPTR, INT16X16, BOOL); // Memory
  ADD_KVX_BUILTIN (SOWO, "sowo", VOID, VPTR, INT32X8, BOOL); // Memory
  ADD_KVX_BUILTIN (SODQ, "sodq", VOID, VPTR, INT64X4, BOOL); // Memory
  ADD_KVX_BUILTIN (SOFWO, "sofwo", VOID, VPTR, FLOAT32X8, BOOL); // Memory
  ADD_KVX_BUILTIN (SOFDQ, "sofdq", VOID, VPTR, FLOAT64X4, BOOL); // Memory

  ADD_KVX_BUILTIN (LVBV, "lvbv", VOID, UINT8, CVPTR, SCATTER); // Coprocessor
  ADD_KVX_BUILTIN (LVHX, "lvhx", VOID, UINT8, CVPTR, SCATTER); // Coprocessor
  ADD_KVX_BUILTIN (LVWO, "lvwo", VOID, UINT8, CVPTR, SCATTER); // Coprocessor
  ADD_KVX_BUILTIN (LVDQ, "lvdq", VOID, UINT8, CVPTR, SCATTER); // Coprocessor
  ADD_KVX_BUILTIN (LVFWO, "lvfwo", VOID, UINT8, CVPTR, SCATTER); // Coprocessor
  ADD_KVX_BUILTIN (LVFDQ, "lvfdq", VOID, UINT8, CVPTR, SCATTER); // Coprocessor

  ADD_KVX_BUILTIN (SVBV, "svbv", VOID, VPTR, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (SVHX, "svhx", VOID, VPTR, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (SVWO, "svwo", VOID, VPTR, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (SVDQ, "svdq", VOID, VPTR, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (SVFWO, "svfwo", VOID, VPTR, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (SVFDQ, "svfdq", VOID, VPTR, UINT8); // Coprocessor

  ADD_KVX_BUILTIN (MOVEFOBV, "movefobv", INT8X32, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (MOVEFOHX, "movefohx", INT16X16, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (MOVEFOWO, "movefowo", INT32X8, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (MOVEFODQ, "movefodq", INT64X4, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (MOVEFOFWO, "movefofwo", FLOAT32X8, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (MOVEFOFDQ, "movefofdq", FLOAT64X4, UINT8); // Coprocessor

  ADD_KVX_BUILTIN (MOVETOBV, "movetobv", VOID, UINT8, INT8X32); // Coprocessor
  ADD_KVX_BUILTIN (MOVETOHX, "movetohx", VOID, UINT8, INT16X16); // Coprocessor
  ADD_KVX_BUILTIN (MOVETOWO, "movetowo", VOID, UINT8, INT32X8); // Coprocessor
  ADD_KVX_BUILTIN (MOVETODQ, "movetodq", VOID, UINT8, INT64X4); // Coprocessor
  ADD_KVX_BUILTIN (MOVETOFWO, "movetofwo", VOID, UINT8, FLOAT32X8); // Coprocessor
  ADD_KVX_BUILTIN (MOVETOFDQ, "movetofdq", VOID, UINT8, FLOAT64X4); // Coprocessor

  ADD_KVX_BUILTIN (SWAPVOBV, "swapvobv", INT8X32, UINT8, INT8X32); // Coprocessor
  ADD_KVX_BUILTIN (SWAPVOHX, "swapvohx", INT16X16, UINT8, INT16X16); // Coprocessor
  ADD_KVX_BUILTIN (SWAPVOWO, "swapvowo", INT32X8, UINT8, INT32X8); // Coprocessor
  ADD_KVX_BUILTIN (SWAPVODQ, "swapvodq", INT64X4, UINT8, INT64X4); // Coprocessor
  ADD_KVX_BUILTIN (SWAPVOFWO, "swapvofwo", FLOAT32X8, UINT8, FLOAT32X8); // Coprocessor
  ADD_KVX_BUILTIN (SWAPVOFDQ, "swapvofdq", FLOAT64X4, UINT8, FLOAT64X4); // Coprocessor

  ADD_KVX_BUILTIN (ALIGNOBV, "alignobv", INT8X32, UINT8, UINT8, UINT32); // Coprocessor
  ADD_KVX_BUILTIN (ALIGNOHX, "alignohx", INT16X16, UINT8, UINT8, UINT32); // Coprocessor
  ADD_KVX_BUILTIN (ALIGNOWO, "alignowo", INT32X8, UINT8, UINT8, UINT32); // Coprocessor
  ADD_KVX_BUILTIN (ALIGNODQ, "alignodq", INT64X4, UINT8, UINT8, UINT32); // Coprocessor
  ADD_KVX_BUILTIN (ALIGNOFWO, "alignofwo", FLOAT32X8, UINT8, UINT8, UINT32); // Coprocessor
  ADD_KVX_BUILTIN (ALIGNOFDQ, "alignofdq", FLOAT64X4, UINT8, UINT8, UINT32); // Coprocessor

  ADD_KVX_BUILTIN (ALIGNV, "alignv", VOID, UINT8, UINT8, UINT8, UINT32); // Coprocessor
  ADD_KVX_BUILTIN (COPYV, "copyv", VOID, UINT8, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (MT4X4D, "mt4x4d", VOID, UINT8, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (MM4ABW, "mm4abw", VOID, UINT8, UINT8, UINT8, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (FMM4AHW0, "fmm4ahw0", VOID, UINT8, UINT8, UINT8, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (FMM4AHW1, "fmm4ahw1", VOID, UINT8, UINT8, UINT8, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (FMM4AHW2, "fmm4ahw2", VOID, UINT8, UINT8, UINT8, UINT8); // Coprocessor
  ADD_KVX_BUILTIN (FMM4AHW3, "fmm4ahw3", VOID, UINT8, UINT8, UINT8, UINT8); // Coprocessor
}

static tree
kvx_builtin_decl (unsigned code, bool initialize_p ATTRIBUTE_UNUSED)
{
  if (code >= ARRAY_SIZE (builtin_fndecls))
    return error_mark_node;
  return builtin_fndecls[code];
}

static inline const char *
tree_string_constant (tree arg)
{
  tree offset_tree = 0;
  arg = string_constant (arg, &offset_tree);
  return arg ? TREE_STRING_POINTER (arg) : "";
}

static rtx
build_carry_arg (tree arg, const char *name)
{
  const char *modifier = tree_string_constant (arg);
  static const char *table[] = {
    ".c",  ".ci",
  };
  for (int i = 0; i < (int) (sizeof (table) / sizeof (*table)); i++)
    {
      if (!strcmp (modifier, table[i]))
	return gen_rtx_CONST_STRING (VOIDmode, table[i]);
    }
  error ("__builtin_kvx_%s modifier %s not recognized.", name, modifier);
  return 0;
}

static rtx
build_silent_arg (tree arg, const char *name)
{
  const char *modifier = tree_string_constant (arg);
  static const char *table[] = {
    "",
    ".s",
  };
  for (int i = 0; i < (int) (sizeof (table) / sizeof (*table)); i++)
    {
      if (!strcmp (modifier, table[i]))
	return gen_rtx_CONST_STRING (VOIDmode, table[i]);
    }
  error ("__builtin_kvx_%s modifier %s not recognized.", name, modifier);
  return 0;
}

static rtx
build_rounding_silent_arg (tree arg, const char *name)
{
  const char *modifier = tree_string_constant (arg);
  static const char *table[] = {
    ".rn",  ".rn.s",  ".ru",  ".ru.s",	".rd", ".rd.s", ".rz", ".rz.s",
    ".rna", ".rna.s", ".rnz", ".rnz.s", ".ro", ".ro.s", "",    ".s",
  };
  for (int i = 0; i < (int) (sizeof (table) / sizeof (*table)); i++)
    {
      if (!strcmp (modifier, table[i]))
	return gen_rtx_CONST_STRING (VOIDmode, table[i]);
    }
  error ("__builtin_kvx_%s modifier %s not recognized.", name, modifier);
  return 0;
}

static rtx
build_simdcond_arg (tree arg, const char *name, machine_mode imode)
{
  const char *modifier = tree_string_constant (arg);
  static const char *table[] = {
    ".nez", ".eqz", ".ltz", ".gez", ".lez", ".gtz", ".odd", ".even",
  };
  static const char *table2[] = {
    ".dnez", ".deqz", ".dltz", ".dgez", ".dlez", ".dgtz", ".odd", ".even",
  };
  // In case of empty string, assume .nez
  if (!modifier[0])
    modifier = ".nez";
  for (int i = 0; i < (int)(sizeof(table)/sizeof(*table)); i++)
    {
      if (!strcmp (modifier, table[i]))
	return imode == DImode?
	  gen_rtx_CONST_STRING (VOIDmode, table2[i]):
	  gen_rtx_CONST_STRING (VOIDmode, table[i]);
    }
  error ("__builtin_kvx_%s modifier %s not recognized.", name, modifier);
  return 0;
}

static rtx
build_bypass_speculate_arg (tree arg, const char *name)
{
  const char *modifier = tree_string_constant (arg);
  static const char *table[] = {
    ".u",
    ".us",
    "",
    ".s",
  };
  for (int i = 0; i < (int) (sizeof (table) / sizeof (*table)); i++)
    {
      if (!strcmp (modifier, table[i]))
	return gen_rtx_CONST_STRING (VOIDmode, table[i]);
    }
  error ("__builtin_kvx_%s modifier %s not recognized.", name, modifier);
  return 0;
}

static rtx
build_scatter_speculate_arg (tree arg, const char *name)
{
  const char *modifier = tree_string_constant (arg);
  static const char *table[] = {
    ".c0", ".c0.s", ".c1", ".c1.s", ".c2", ".c2.s", ".c3", ".c3.s", "", ".s",
  };
  for (int i = 0; i < (int) (sizeof (table) / sizeof (*table)); i++)
    {
      if (!strcmp (modifier, table[i]))
	return gen_rtx_CONST_STRING (VOIDmode, table[i]);
    }
  error ("__builtin_kvx_%s modifier %s not recognized.", name, modifier);
  return 0;
}

static rtx
build_xvr_reg_name_arg (rtx arg)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned regno = INTVAL (arg);
      if (regno < 64)
	{
	  static const char *xvr_reg_names[] = {
	    "a0",  "a1",  "a2",	 "a3",	"a4",  "a5",  "a6",  "a7",
	    "a8",  "a9",  "a10", "a11", "a12", "a13", "a14", "a15",
	    "a16", "a17", "a18", "a19", "a20", "a21", "a22", "a23",
	    "a24", "a25", "a26", "a27", "a28", "a29", "a30", "a31",
	    "a32", "a33", "a34", "a35", "a36", "a37", "a38", "a39",
	    "a40", "a41", "a42", "a43", "a44", "a45", "a46", "a47",
	    "a48", "a49", "a50", "a51", "a52", "a53", "a54", "a55",
	    "a56", "a57", "a58", "a59", "a60", "a61", "a62", "a63",
	  };
	  return gen_rtx_CONST_STRING (VOIDmode, xvr_reg_names[regno]);
	}
    }
  return 0;
}

static rtx
build_xwr_reg_name_arg (rtx arg)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned regno = INTVAL (arg);
      if (regno < 64)
	{
	  static const char *xwr_reg_names[] = {
	    "a0a1",   "ERROR", "a2a3",   "ERROR",
	    "a4a5",   "ERROR", "a6a7",   "ERROR",
	    "a8a9",   "ERROR", "a10a11", "ERROR",
	    "a12a13", "ERROR", "a14a15", "ERROR",
	    "a16a17", "ERROR", "a18a19", "ERROR",
	    "a20a21", "ERROR", "a22a23", "ERROR",
	    "a24a25", "ERROR", "a26a27", "ERROR",
	    "a28a29", "ERROR", "a30a31", "ERROR",
	    "a32a33", "ERROR", "a34a35", "ERROR",
	    "a36a37", "ERROR", "a38a39", "ERROR",
	    "a40a41", "ERROR", "a42a43", "ERROR",
	    "a44a45", "ERROR", "a46a47", "ERROR",
	    "a48a49", "ERROR", "a50a51", "ERROR",
	    "a52a53", "ERROR", "a54a55", "ERROR",
	    "a56a57", "ERROR", "a58a59", "ERROR",
	    "a60a61", "ERROR", "a62a63", "ERROR",
	  };
	  return gen_rtx_CONST_STRING (VOIDmode, xwr_reg_names[regno]);
	}
    }
  return 0;
}

static rtx
build_xmr_reg_name_arg (rtx arg)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned regno = INTVAL (arg);
      if (regno < 64)
	{
	  static const char *xmr_reg_names[] = {
	    "a0a1a2a3",     "ERROR", "ERROR", "ERROR",
	    "a4a5a6a7",     "ERROR", "ERROR", "ERROR",
	    "a8a9a10a11",   "ERROR", "ERROR", "ERROR",
	    "a12a13a14a15", "ERROR", "ERROR", "ERROR",
	    "a16a17a18a19", "ERROR", "ERROR", "ERROR",
	    "a20a21a22a23", "ERROR", "ERROR", "ERROR",
	    "a24a25a26a27", "ERROR", "ERROR", "ERROR",
	    "a28a29a30a31", "ERROR", "ERROR", "ERROR",
	    "a32a33a34a35", "ERROR", "ERROR", "ERROR",
	    "a36a37a38a39", "ERROR", "ERROR", "ERROR",
	    "a40a41a42a43", "ERROR", "ERROR", "ERROR",
	    "a44a45a46a47", "ERROR", "ERROR", "ERROR",
	    "a48a49a50a51", "ERROR", "ERROR", "ERROR",
	    "a52a53a54a55", "ERROR", "ERROR", "ERROR",
	    "a56a57a58a59", "ERROR", "ERROR", "ERROR",
	    "a60a61a62a63", "ERROR", "ERROR", "ERROR",
	  };
	  return gen_rtx_CONST_STRING (VOIDmode, xmr_reg_names[regno]);
	}
    }
  return 0;
}

static rtx
verify_const_bool_arg (rtx arg, const char *name, const char *where)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned long long tmp = INTVAL (arg);
      if (tmp == 0LL || tmp == 1LL)
	return arg;
    }
  error ("__builtin_kvx_%s expects a boolean immediate in %s argument.", name,
	 where);
  return 0;
}

static rtx
verify_const_int_arg (rtx arg, int bits, const char *name, const char *where)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned shift = 64 - bits;
      long long tmp = INTVAL (arg);
      signed long long stmp = tmp;
      if (tmp == (stmp << shift) >> shift)
	return arg;
    }
  error ("__builtin_kvx_%s expects a %d-bit signed immediate in %s argument.",
	 name, bits, where);
  return 0;
}

static rtx
verify_const_uint_arg (rtx arg, int bits, const char *name, const char *where)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned shift = 64 - bits;
      long long tmp = INTVAL (arg);
      unsigned long long utmp = tmp;
      if (tmp == (utmp << shift) >> shift)
	return arg;
    }
  error ("__builtin_kvx_%s expects a %d-bit unsigned immediate in %s argument.",
	 name, bits, where);
  return 0;
}

static rtx
verify_const_field_arg (rtx arg, int bits, const char *name, const char *where)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned shift = 64 - bits;
      long long tmp = INTVAL (arg);
      signed long long stmp = tmp;
      unsigned long long utmp = tmp;
      if (tmp == (stmp << shift) >> shift)
	return arg;
      if (tmp == (utmp << shift) >> shift)
	return arg;
    }
  error ("__builtin_kvx_%s expects a %d-bit signed or unsigned immediate in %s "
	 "argument.",
	 name, bits, where);
  return 0;
}

static rtx
kvx_expand_builtin_get (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = verify_const_uint_arg (arg1, 9, "get", "first");
  int regno = INTVAL (arg1) + KV3_SFR_FIRST_REGNO;
  rtx sys_reg = gen_rtx_REG (DImode, regno);

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  if (regno == KV3_PCR_REGNO)
    emit_move_insn (target, sys_reg);
  else
    emit_insn (gen_kvx_get (target, sys_reg, kvx_sync_reg_rtx));

  return target;
}

static rtx
kvx_expand_builtin_set (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = verify_const_uint_arg (arg1, 9, "set", "first");
  arg2 = force_reg (DImode, arg2);
  int regno = INTVAL (arg1) + KV3_SFR_FIRST_REGNO;
  rtx sys_reg = gen_rtx_REG (DImode, regno);

  emit_insn (gen_kvx_set (sys_reg, arg2, kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_wfxl (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = verify_const_uint_arg (arg1, 9, "wfxl", "first");
  arg2 = force_reg (DImode, arg2);
  int regno = INTVAL (arg1) + KV3_SFR_FIRST_REGNO;
  rtx sys_reg = gen_rtx_REG (DImode, regno);

  emit_insn (gen_kvx_wfxl (sys_reg, arg2, kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_wfxm (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = verify_const_uint_arg (arg1, 9, "wfxm", "first");
  arg2 = force_reg (DImode, arg2);
  int regno = INTVAL (arg1) + KV3_SFR_FIRST_REGNO;
  rtx sys_reg = gen_rtx_REG (DImode, regno);

  emit_insn (gen_kvx_wfxm (sys_reg, arg2, kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_waitit (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (SImode, arg1);

  if (!target)
    target = gen_reg_rtx (SImode);
  else
    target = force_reg (SImode, target);

  emit_insn (gen_waitit (target, arg1, kvx_sync_reg_rtx));

  return target;
}

static rtx
kvx_expand_builtin_sbmm8 (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  emit_insn (gen_kvx_sbmm8 (target, arg1, arg2));

  return target;
}

static rtx
kvx_expand_builtin_sbmmt8 (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  emit_insn (gen_kvx_sbmmt8 (target, arg1, arg2));

  return target;
}

static rtx
kvx_expand_builtin_await (rtx target ATTRIBUTE_UNUSED,
			  tree args ATTRIBUTE_UNUSED)
{
  emit_insn (gen_await (kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_sleep (rtx target ATTRIBUTE_UNUSED,
			  tree args ATTRIBUTE_UNUSED)
{
  emit_insn (gen_sleep (kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_stop (rtx target ATTRIBUTE_UNUSED,
			 tree args ATTRIBUTE_UNUSED)
{
  emit_insn (gen_stop (kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_syncgroup (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (DImode, arg1);

  emit_insn (gen_syncgroup (arg1, kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_barrier (void)
{
  emit_insn (gen_barrier (kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_dinval (void)
{
  emit_insn (gen_dinval (kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_iinval (void)
{
  emit_insn (gen_iinval (kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_tlbdinval (void)
{
  emit_insn (gen_tlbdinval (kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_tlbiinval (void)
{
  emit_insn (gen_tlbiinval (kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_tlbprobe (void)
{
  emit_insn (gen_tlbprobe (kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_tlbread (void)
{
  emit_insn (gen_tlbread (kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_tlbwrite (void)
{
  emit_insn (gen_tlbwrite (kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_satd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (SImode, arg2);

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  emit_insn (gen_satd (target, arg1, arg2));
  return target;
}

static rtx
kvx_expand_builtin_satud (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (SImode, arg2);

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  emit_insn (gen_satud (target, arg1, arg2));
  return target;
}

static rtx
kvx_builtin_helper_memref_ptr (rtx ptr, enum machine_mode mode)
{
  ptr = force_reg (Pmode, ptr);
  rtx mem_target = gen_rtx_MEM (mode, ptr);

  return mem_target;
}

/*
 * unpack the rtx arg ARGNUM and create a (mem:MODE ) using
 * the unpacked rtx as base address
 */
#define MEMREF(argnum, mode, varname)                                          \
  rtx varname;                                                                 \
  {                                                                            \
    rtx _tmp = expand_normal (CALL_EXPR_ARG (args, argnum));                   \
    rtx _mem_target = kvx_builtin_helper_memref_ptr (_tmp, mode);              \
    varname = _mem_target;                                                     \
  }

#define GETREG(argnum, mode, varname)                                          \
  rtx varname = expand_normal (CALL_EXPR_ARG (args, argnum));                  \
  varname = force_reg (mode, varname);

static rtx
kvx_expand_builtin_aladd (rtx target, tree args, enum machine_mode mode)
{
  MEMREF (0, mode, mem_target);
  GETREG (1, mode, addend_and_return);

  if (!target)
    target = gen_reg_rtx (mode);
  else
    target = force_reg (mode, target);

  switch (mode)
    {
    case DImode:
      emit_insn (gen_aladdd (target, mem_target, addend_and_return));
      break;
    case SImode:
      emit_insn (gen_aladdw (target, mem_target, addend_and_return));
      break;
    default:
      gcc_unreachable ();
    }

  return target;
}

static rtx
kvx_expand_builtin_acswap (rtx target, tree args, enum machine_mode mode)
{
  rtx ptr = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!REG_P (ptr))
    ptr = force_reg (Pmode, ptr);

  rtx mem_ref = gen_rtx_MEM (mode, ptr);

  rtx new_val = expand_normal (CALL_EXPR_ARG (args, 1));
  rtx expect_val = expand_normal (CALL_EXPR_ARG (args, 2));

  rtx tmp = gen_reg_rtx (TImode);

  if (!target)
    target = gen_reg_rtx (mode);
  else
    target = force_reg (mode, target);

  emit_move_insn (gen_rtx_SUBREG (mode, tmp, 0), new_val);
  emit_move_insn (gen_rtx_SUBREG (mode, tmp, 8), expect_val);

  switch (mode)
    {
    case DImode:
      emit_insn (gen_acswapd (tmp, mem_ref));
      break;
    case SImode:
      emit_insn (gen_acswapw (tmp, mem_ref));
      break;
    default:
      gcc_unreachable ();
    }

  rtx result = gen_lowpart_SUBREG (mode, tmp);
  emit_move_insn (target, result);

  return target;
}

static rtx
kvx_expand_builtin_fence (void)
{
  emit_insn (gen_fence (kvx_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
kvx_expand_builtin_dinvall (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = gen_rtx_MEM (SImode, force_reg (Pmode, arg1));

  emit_insn (gen_dinvall (arg1, kvx_sync_reg_rtx));

  return target;
}

static rtx
kvx_expand_builtin_dtouchl (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (Pmode, arg1);

  emit_insn (gen_prefetch (arg1, const0_rtx, const0_rtx));

  return target;
}

static rtx
kvx_expand_builtin_dzerol (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = gen_rtx_MEM (SImode, force_reg (Pmode, arg1));

  emit_insn (gen_dzerol (arg1, kvx_sync_reg_rtx));

  return target;
}

static rtx
kvx_expand_builtin_iinvals (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = gen_rtx_MEM (SImode, force_reg (Pmode, arg1));

  emit_insn (gen_iinvals (arg1, kvx_sync_reg_rtx));

  return target;
}

static rtx
kvx_expand_builtin_alclr (rtx target, tree args, enum machine_mode mode)
{
  rtx ptr = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!REG_P (ptr))
    ptr = force_reg (Pmode, ptr);

  rtx mem_ref = gen_rtx_MEM (mode, ptr);

  if (!target)
    target = gen_reg_rtx (mode);
  if (!REG_P (target) || GET_MODE (target) != mode)
    {
      target = force_reg (mode, target);
    }

  switch (mode)
    {
    case DImode:
      emit_insn (gen_alclrd (target, mem_ref));
      break;
    case SImode:
      emit_insn (gen_alclrw (target, mem_ref));
      break;
    default:
      gcc_unreachable ();
    }

  return target;
}

#define KVX_EXPAND_BUILTIN_2_STANDARD(name, name2, tmode, smode)               \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    arg1 = force_reg (smode, arg1);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_##name2 (target, arg1));                                    \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_2_MODIFIERS(name, tmode, smode)                     \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = build_rounding_silent_arg (CALL_EXPR_ARG (args, 1), #name);     \
    arg1 = force_reg (smode, arg1);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_kvx_##name (target, arg1, arg2));                           \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_2_SILENT(name, tmode, smode)                        \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = build_silent_arg (CALL_EXPR_ARG (args, 1), #name);              \
    arg1 = force_reg (smode, arg1);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_kvx_##name (target, arg1, arg2));                           \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_3_STANDARD(name, name2, tmode, smode)               \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    arg1 = force_reg (smode, arg1);                                            \
    arg2 = force_reg (smode, arg2);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_##name2 (target, arg1, arg2));                              \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR(name, name2, vmode, smode)          \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    arg1 = force_reg (vmode, arg1);                                            \
    arg2 = force_not_mem (arg2);                                               \
    if (!target)                                                               \
      target = gen_reg_rtx (vmode);                                            \
    else                                                                       \
      target = force_reg (vmode, target);                                      \
    emit_insn (gen_##name2 (target, arg1, arg2));                              \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_3_MODIFIERS(name, tmode, smode)                     \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = build_rounding_silent_arg (CALL_EXPR_ARG (args, 2), #name);     \
    arg1 = force_reg (smode, arg1);                                            \
    arg2 = force_reg (smode, arg2);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_kvx_##name (target, arg1, arg2, arg3));                     \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_3_SILENT(name, mode, tmode, smode)                  \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = build_silent_arg (CALL_EXPR_ARG (args, 2), #name);              \
    arg1 = force_reg (smode, arg1);                                            \
    arg2 = force_reg (smode, arg2);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_kvx_##name (target, arg1, arg2, arg3));                     \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_3_CARRY(name, name2, tmode, smode)                  \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = build_carry_arg (CALL_EXPR_ARG (args, 2), #name);               \
    arg1 = force_reg (smode, arg1);                                            \
    arg2 = force_reg (smode, arg2);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_##name2 (target, arg1, arg2, arg3));                        \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_4_MODIFIERS(name, tmode, smode)                     \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    rtx arg4 = build_rounding_silent_arg (CALL_EXPR_ARG (args, 3), #name);     \
    arg1 = force_reg (smode, arg1);                                            \
    arg2 = force_reg (smode, arg2);                                            \
    arg3 = force_reg (tmode, arg3);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_kvx_##name (target, arg1, arg2, arg3, arg4));               \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_FCONVERT(name, tmode, smode)                        \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = build_rounding_silent_arg (CALL_EXPR_ARG (args, 2), #name);     \
    arg2 = verify_const_uint_arg (arg2, 6, #name, "second");                   \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    arg1 = force_reg (smode, arg1);                                            \
    emit_insn (gen_kvx_##name (target, arg1, arg2, arg3));                     \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_SELECT(name, tmode, cmode)                          \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    machine_mode imode = GET_MODE_INNER (cmode);                               \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    rtx arg4 = build_simdcond_arg (CALL_EXPR_ARG (args, 3), #name, imode);     \
    arg1 = force_reg (tmode, arg1);                                            \
    arg2 = force_reg (tmode, arg2);                                            \
    arg3 = force_reg (cmode, arg3);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_kvx_##name (target, arg1, arg2, arg3, arg4));               \
    return target;                                                             \
  }

KVX_EXPAND_BUILTIN_3_CARRY(addd, kvx_addd, DImode, DImode)
KVX_EXPAND_BUILTIN_3_CARRY(sbfd, kvx_sbfd, DImode, DImode)

KVX_EXPAND_BUILTIN_3_STANDARD (abdhq, kvx_abdhq, V4HImode, V4HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (abdho, kvx_abdho, V8HImode, V8HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (abdhx, kvx_abdhx, V16HImode, V16HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (abdw, kvx_abdw, SImode, SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (abdwp, kvx_abdwp, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (abdwq, kvx_abdwq, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (abdwo, kvx_abdwo, V8SImode, V8SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (abdd, kvx_abdd, DImode, DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (abddp, kvx_abddp, V2DImode, V2DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (abddq, kvx_abddq, V4DImode, V4DImode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (abdhqs, kvx_abdhqs, V4HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (abdhos, kvx_abdhos, V8HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (abdhxs, kvx_abdhxs, V16HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (abdwps, kvx_abdwps, V2SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (abdwqs, kvx_abdwqs, V4SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (abdwos, kvx_abdwos, V8SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (abddps, kvx_abddps, V2DImode, DImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (abddqs, kvx_abddqs, V4DImode, DImode)

KVX_EXPAND_BUILTIN_3_STANDARD (avghq, avgv4hi3_floor, V4HImode, V4HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgho, avgv8hi3_floor, V8HImode, V8HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avghx, avgv16hi3_floor, V16HImode, V16HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgw, avgsi3_floor, SImode, SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgwp, avgv2si3_floor, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgwq, avgv4si3_floor, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgwo, avgv8si3_floor, V8SImode, V8SImode)

KVX_EXPAND_BUILTIN_3_STANDARD (avguhq, uavgv4hi3_floor, V4HImode, V4HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avguho, uavgv8hi3_floor, V8HImode, V8HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avguhx, uavgv16hi3_floor, V16HImode, V16HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avguw, uavgsi3_floor, SImode, SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avguwp, uavgv2si3_floor, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avguwq, uavgv4si3_floor, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avguwo, uavgv8si3_floor, V8SImode, V8SImode)

KVX_EXPAND_BUILTIN_3_STANDARD (avgrhq, avgv4hi3_ceil, V4HImode, V4HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgrho, avgv8hi3_ceil, V8HImode, V8HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgrhx, avgv16hi3_ceil, V16HImode, V16HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgrw, avgsi3_ceil, SImode, SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgrwp, avgv2si3_ceil, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgrwq, avgv4si3_ceil, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgrwo, avgv8si3_ceil, V8SImode, V8SImode)

KVX_EXPAND_BUILTIN_3_STANDARD (avgruhq, uavgv4hi3_ceil, V4HImode, V4HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgruho, uavgv8hi3_ceil, V8HImode, V8HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgruhx, uavgv16hi3_ceil, V16HImode, V16HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgruw, uavgsi3_ceil, SImode, SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgruwp, uavgv2si3_ceil, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgruwq, uavgv4si3_ceil, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (avgruwo, uavgv8si3_ceil, V8SImode, V8SImode)

KVX_EXPAND_BUILTIN_3_STANDARD (addshq, ssaddv4hi3, V4HImode, V4HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (addsho, ssaddv8hi3, V8HImode, V8HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (addshx, ssaddv16hi3, V16HImode, V16HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (addsw, ssaddsi3, SImode, SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (addswp, ssaddv2si3, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (addswq, ssaddv4si3, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (addswo, ssaddv8si3, V8SImode, V8SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (addsd, ssadddi3, DImode, DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (addsdp, ssaddv2di3, V2DImode, V2DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (addsdq, ssaddv4di3, V4DImode, V4DImode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (addshqs, kvx_addshqs, V4HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (addshos, kvx_addshos, V8HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (addshxs, kvx_addshxs, V16HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (addswps, kvx_addswps, V2SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (addswqs, kvx_addswqs, V4SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (addswos, kvx_addswos, V8SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (addsdps, kvx_addsdps, V2DImode, DImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (addsdqs, kvx_addsdqs, V4DImode, DImode)

KVX_EXPAND_BUILTIN_3_STANDARD (sbfshq, sssubv4hi3, V4HImode, V4HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (sbfsho, sssubv8hi3, V8HImode, V8HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (sbfshx, sssubv16hi3, V16HImode, V16HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (sbfsw, sssubsi3, SImode, SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (sbfswp, sssubv2si3, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (sbfswq, sssubv4si3, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (sbfswo, sssubv8si3, V8SImode, V8SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (sbfsd, sssubdi3, DImode, DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (sbfsdp, sssubv2di3, V2DImode, V2DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (sbfsdq, sssubv4di3, V4DImode, V4DImode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sbfshqs, kvx_sbfshqs, V4HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sbfshos, kvx_sbfshos, V8HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sbfshxs, kvx_sbfshxs, V16HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sbfswps, kvx_sbfswps, V2SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sbfswqs, kvx_sbfswqs, V4SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sbfswos, kvx_sbfswos, V8SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sbfsdps, kvx_sbfsdps, V2DImode, DImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sbfsdqs, kvx_sbfsdqs, V4DImode, DImode)

KVX_EXPAND_BUILTIN_3_STANDARD (minhq, sminv4hi3, V4HImode, V4HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minho, sminv8hi3, V8HImode, V8HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minhx, sminv16hi3, V16HImode, V16HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minw, sminsi3, SImode, SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minwp, sminv2si3, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minwq, sminv4si3, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minwo, sminv8si3, V8SImode, V8SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (mind, smindi3, DImode, DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (mindp, sminv2di3, V2DImode, V2DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (mindq, sminv4di3, V4DImode, V4DImode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minhqs, kvx_minhqs, V4HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minhos, kvx_minhos, V8HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minhxs, kvx_minhxs, V16HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minwps, kvx_minwps, V2SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minwqs, kvx_minwqs, V4SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minwos, kvx_minwos, V8SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (mindps, kvx_mindps, V2DImode, DImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (mindqs, kvx_mindqs, V4DImode, DImode)

KVX_EXPAND_BUILTIN_3_STANDARD (maxhq, smaxv4hi3, V4HImode, V4HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxho, smaxv8hi3, V8HImode, V8HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxhx, smaxv16hi3, V16HImode, V16HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxw, smaxsi3, SImode, SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxwp, smaxv2si3, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxwq, smaxv4si3, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxwo, smaxv8si3, V8SImode, V8SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxd, smaxdi3, DImode, DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxdp, smaxv2di3, V2DImode, V2DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxdq, smaxv4di3, V4DImode, V4DImode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxhqs, kvx_maxhqs, V4HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxhos, kvx_maxhos, V8HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxhxs, kvx_maxhxs, V16HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxwps, kvx_maxwps, V2SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxwqs, kvx_maxwqs, V4SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxwos, kvx_maxwos, V8SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxdps, kvx_maxdps, V2DImode, DImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxdqs, kvx_maxdqs, V4DImode, DImode)

KVX_EXPAND_BUILTIN_3_STANDARD (minuhq, uminv4hi3, V4HImode, V4HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minuho, uminv8hi3, V8HImode, V8HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minuhx, uminv16hi3, V16HImode, V16HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minuw, uminsi3, SImode, SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minuwp, uminv2si3, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minuwq, uminv4si3, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minuwo, uminv8si3, V8SImode, V8SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minud, umindi3, DImode, DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minudp, uminv2di3, V2DImode, V2DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (minudq, uminv4di3, V4DImode, V4DImode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minuhqs, kvx_minuhqs, V4HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minuhos, kvx_minuhos, V8HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minuhxs, kvx_minuhxs, V16HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minuwps, kvx_minuwps, V2SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minuwqs, kvx_minuwqs, V4SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minuwos, kvx_minuwos, V8SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minudps, kvx_minudps, V2DImode, DImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (minudqs, kvx_minudqs, V4DImode, DImode)

KVX_EXPAND_BUILTIN_3_STANDARD (maxuhq, umaxv4hi3, V4HImode, V4HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxuho, umaxv8hi3, V8HImode, V8HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxuhx, umaxv16hi3, V16HImode, V16HImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxuw, umaxsi3, SImode, SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxuwp, umaxv2si3, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxuwq, umaxv4si3, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxuwo, umaxv8si3, V8SImode, V8SImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxud, umaxdi3, DImode, DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxudp, umaxv2di3, V2DImode, V2DImode)
KVX_EXPAND_BUILTIN_3_STANDARD (maxudq, umaxv4di3, V4DImode, V4DImode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxuhqs, kvx_maxuhqs, V4HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxuhos, kvx_maxuhos, V8HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxuhxs, kvx_maxuhxs, V16HImode, HImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxuwps, kvx_maxuwps, V2SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxuwqs, kvx_maxuwqs, V4SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxuwos, kvx_maxuwos, V8SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxudps, kvx_maxudps, V2DImode, DImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (maxudqs, kvx_maxudqs, V4DImode, DImode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sllhqs, ashlv4hi3, V4HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sllhos, ashlv8hi3, V8HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sllhxs, ashlv16hi3, V16HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sllwps, ashlv2si3, V2SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sllwqs, ashlv4si3, V4SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sllwos, ashlv8si3, V8SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (slldps, ashlv2di3, V2DImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (slldqs, ashlv4di3, V4DImode, SImode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (slshqs, ssashlv4hi3, V4HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (slshos, ssashlv8hi3, V8HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (slshxs, ssashlv16hi3, V16HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (slswps, ssashlv2si3, V2SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (slswqs, ssashlv4si3, V4SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (slswos, ssashlv8si3, V8SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (slsdps, ssashlv2di3, V2DImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (slsdqs, ssashlv4di3, V4DImode, SImode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srahqs, ashrv4hi3, V4HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srahos, ashrv8hi3, V8HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srahxs, ashrv16hi3, V16HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srawps, ashrv2si3, V2SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srawqs, ashrv4si3, V4SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srawos, ashrv8si3, V8SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sradps, ashrv2di3, V2DImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (sradqs, ashrv4di3, V4DImode, SImode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srlhqs, lshrv4hi3, V4HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srlhos, lshrv8hi3, V8HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srlhxs, lshrv16hi3, V16HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srlwps, lshrv2si3, V2SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srlwqs, lshrv4si3, V4SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srlwos, lshrv8si3, V8SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srldps, lshrv2di3, V2DImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srldqs, lshrv4di3, V4DImode, SImode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srshqs, kvx_srshqs, V4HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srshos, kvx_srshos, V8HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srshxs, kvx_srshxs, V16HImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srswps, kvx_srswps, V2SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srswqs, kvx_srswqs, V4SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srswos, kvx_srswos, V8SImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srsdps, kvx_srsdps, V2DImode, SImode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (srsdqs, kvx_srsdqs, V4DImode, SImode)

KVX_EXPAND_BUILTIN_2_STANDARD (clsw, clrsbsi2, SImode, SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (clswp, clrsbv2si2, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (clswq, clrsbv4si2, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (clswo, clrsbv8si2, V8SImode, V8SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (clsd, clrsbdi2, DImode, DImode)
KVX_EXPAND_BUILTIN_2_STANDARD (clsdp, clrsbv2di2, V2DImode, V2DImode)
KVX_EXPAND_BUILTIN_2_STANDARD (clsdq, clrsbv4di2, V4DImode, V4DImode)

KVX_EXPAND_BUILTIN_2_STANDARD (clzw, clzsi2, SImode, SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (clzwp, clzv2si2, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (clzwq, clzv4si2, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (clzwo, clzv8si2, V8SImode, V8SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (clzd, clzdi2, DImode, DImode)
KVX_EXPAND_BUILTIN_2_STANDARD (clzdp, clzv2di2, V2DImode, V2DImode)
KVX_EXPAND_BUILTIN_2_STANDARD (clzdq, clzv4di2, V4DImode, V4DImode)

KVX_EXPAND_BUILTIN_2_STANDARD (ctzw, ctzsi2, SImode, SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (ctzwp, ctzv2si2, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (ctzwq, ctzv4si2, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (ctzwo, ctzv8si2, V8SImode, V8SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (ctzd, ctzdi2, DImode, DImode)
KVX_EXPAND_BUILTIN_2_STANDARD (ctzdp, ctzv2di2, V2DImode, V2DImode)
KVX_EXPAND_BUILTIN_2_STANDARD (ctzdq, ctzv4di2, V4DImode, V4DImode)

KVX_EXPAND_BUILTIN_2_STANDARD (cbsw, popcountsi2, SImode, SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (cbswp, popcountv2si2, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (cbswq, popcountv4si2, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (cbswo, popcountv8si2, V8SImode, V8SImode)
KVX_EXPAND_BUILTIN_2_STANDARD (cbsd, popcountdi2, DImode, DImode)
KVX_EXPAND_BUILTIN_2_STANDARD (cbsdp, popcountv2di2, V2DImode, V2DImode)
KVX_EXPAND_BUILTIN_2_STANDARD (cbsdq, popcountv4di2, V4DImode, V4DImode)

KVX_EXPAND_BUILTIN_SELECT (selecthq, V4HImode, V4HImode)
KVX_EXPAND_BUILTIN_SELECT (selectho, V8HImode, V8HImode)
KVX_EXPAND_BUILTIN_SELECT (selecthx, V16HImode, V16HImode)
KVX_EXPAND_BUILTIN_SELECT (selectwp, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_SELECT (selectwq, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_SELECT (selectwo, V8SImode, V8SImode)
KVX_EXPAND_BUILTIN_SELECT (selectdp, V2DImode, V2DImode)
KVX_EXPAND_BUILTIN_SELECT (selectdq, V4DImode, V4DImode)
KVX_EXPAND_BUILTIN_SELECT (selectfwp, V2SFmode, V2SImode)
KVX_EXPAND_BUILTIN_SELECT (selectfwq, V4SFmode, V4SImode)
KVX_EXPAND_BUILTIN_SELECT (selectfwo, V8SFmode, V8SImode)
KVX_EXPAND_BUILTIN_SELECT (selectfdp, V2DFmode, V2DImode)
KVX_EXPAND_BUILTIN_SELECT (selectfdq, V4DFmode, V4DImode)

KVX_EXPAND_BUILTIN_3_STANDARD (copysignw, copysignsf3, SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (copysignwp, copysignv2sf3, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (copysignwq, copysignv4sf3, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (copysignwo, copysignv8sf3, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (copysignd, copysigndf3, DFmode, DFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (copysigndp, copysignv2df3, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (copysigndq, copysignv4df3, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_3_STANDARD (fminw, fminsf3, SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (fminwp, fminv2sf3, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (fminwq, fminv4sf3, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (fminwo, fminv8sf3, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (fmind, fmindf3, DFmode, DFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (fmindp, fminv2df3, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (fmindq, fminv4df3, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (fminwps, kvx_fminwps, V2SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (fminwqs, kvx_fminwqs, V4SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (fminwos, kvx_fminwos, V8SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (fmindps, kvx_fmindps, V2DFmode, DFmode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (fmindqs, kvx_fmindqs, V4DFmode, DFmode)

KVX_EXPAND_BUILTIN_3_STANDARD (fmaxw, fmaxsf3, SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (fmaxwp, fmaxv2sf3, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (fmaxwq, fmaxv4sf3, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (fmaxwo, fmaxv8sf3, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (fmaxd, fmaxdf3, DFmode, DFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (fmaxdp, fmaxv2df3, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_3_STANDARD (fmaxdq, fmaxv4df3, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (fmaxwps, kvx_fmaxwps, V2SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (fmaxwqs, kvx_fmaxwqs, V4SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (fmaxwos, kvx_fmaxwos, V8SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (fmaxdps, kvx_fmaxdps, V2DFmode, DFmode)
KVX_EXPAND_BUILTIN_3_VECTOR_SCALAR (fmaxdqs, kvx_fmaxdqs, V4DFmode, DFmode)

KVX_EXPAND_BUILTIN_2_STANDARD (fnegw, negsf2, SFmode, SFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fnegwp, negv2sf2, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fnegwq, negv4sf2, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fnegwo, negv8sf2, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fnegd, negdf2, DFmode, DFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fnegdp, negv2df2, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fnegdq, negv4df2, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_2_STANDARD (fabsw, abssf2, SFmode, SFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fabswp, absv2sf2, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fabswq, absv4sf2, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fabswo, absv8sf2, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fabsd, absdf2, DFmode, DFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fabsdp, absv2df2, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fabsdq, absv4df2, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_2_MODIFIERS (frecw, SFmode, SFmode)
KVX_EXPAND_BUILTIN_2_MODIFIERS (frecwp, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_2_MODIFIERS (frecwq, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_2_MODIFIERS (frecwo, V8SFmode, V8SFmode)

KVX_EXPAND_BUILTIN_2_MODIFIERS (frsrw, SFmode, SFmode)
KVX_EXPAND_BUILTIN_2_MODIFIERS (frsrwp, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_2_MODIFIERS (frsrwq, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_2_MODIFIERS (frsrwo, V8SFmode, V8SFmode)

KVX_EXPAND_BUILTIN_3_MODIFIERS (faddw, SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (faddwp, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (faddwq, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (faddwo, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (faddd, DFmode, DFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fadddp, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fadddq, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_3_MODIFIERS (faddcwc, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (faddcwcp, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (faddcwcq, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (faddcdc, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (faddcdcp, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_3_MODIFIERS (fsbfw, SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fsbfwp, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fsbfwq, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fsbfwo, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fsbfd, DFmode, DFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fsbfdp, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fsbfdq, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_3_MODIFIERS (fsbfcwc, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fsbfcwcp, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fsbfcwcq, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fsbfcdc, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fsbfcdcp, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_3_MODIFIERS (fmulw, SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmulwp, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmulwq, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmulwo, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmuld, DFmode, DFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmuldp, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmuldq, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_3_MODIFIERS (fmulwd, DFmode, SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmulwc, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmulwcp, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmulwcq, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmulcwc, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmulcwcp, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmulcwcq, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmuldc, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmuldcp, V4DFmode, V4DFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmulcdc, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmulcdcp, V4DFmode, V4DFmode)
KVX_EXPAND_BUILTIN_3_MODIFIERS (fmm212w, V4SFmode, V2SFmode)

KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmaw, SFmode, SFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmawp, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmawq, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmawo, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmad, DFmode, DFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmadp, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmadq, V4DFmode, V4DFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmawd, DFmode, SFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (fmma212w, V4SFmode, V2SFmode)

KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmsw, SFmode, SFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmswp, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmswq, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmswo, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmsd, DFmode, DFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmsdp, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmsdq, V4DFmode, V4DFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (ffmswd, DFmode, SFmode)
KVX_EXPAND_BUILTIN_4_MODIFIERS (fmms212w, V4SFmode, V2SFmode)

KVX_EXPAND_BUILTIN_FCONVERT (floatw, SFmode, SImode)
KVX_EXPAND_BUILTIN_FCONVERT (floatwp, V2SFmode, V2SImode)
KVX_EXPAND_BUILTIN_FCONVERT (floatwq, V4SFmode, V4SImode)
KVX_EXPAND_BUILTIN_FCONVERT (floatwo, V8SFmode, V8SImode)
KVX_EXPAND_BUILTIN_FCONVERT (floatd, DFmode, DImode)
KVX_EXPAND_BUILTIN_FCONVERT (floatdp, V2DFmode, V2DImode)
KVX_EXPAND_BUILTIN_FCONVERT (floatdq, V4DFmode, V4DImode)

KVX_EXPAND_BUILTIN_FCONVERT (floatuw, SFmode, SImode)
KVX_EXPAND_BUILTIN_FCONVERT (floatuwp, V2SFmode, V2SImode)
KVX_EXPAND_BUILTIN_FCONVERT (floatuwq, V4SFmode, V4SImode)
KVX_EXPAND_BUILTIN_FCONVERT (floatuwo, V8SFmode, V8SImode)
KVX_EXPAND_BUILTIN_FCONVERT (floatud, DFmode, DImode)
KVX_EXPAND_BUILTIN_FCONVERT (floatudp, V2DFmode, V2DImode)
KVX_EXPAND_BUILTIN_FCONVERT (floatudq, V4DFmode, V4DImode)

KVX_EXPAND_BUILTIN_FCONVERT (fixedw, SImode, SFmode)
KVX_EXPAND_BUILTIN_FCONVERT (fixedwp, V2SImode, V2SFmode)
KVX_EXPAND_BUILTIN_FCONVERT (fixedwq, V4SImode, V4SFmode)
KVX_EXPAND_BUILTIN_FCONVERT (fixedwo, V8SImode, V8SFmode)
KVX_EXPAND_BUILTIN_FCONVERT (fixedd, DImode, DFmode)
KVX_EXPAND_BUILTIN_FCONVERT (fixeddp, V2DImode, V2DFmode)
KVX_EXPAND_BUILTIN_FCONVERT (fixeddq, V4DImode, V4DFmode)

KVX_EXPAND_BUILTIN_FCONVERT (fixeduw, SImode, SFmode)
KVX_EXPAND_BUILTIN_FCONVERT (fixeduwp, V2SImode, V2SFmode)
KVX_EXPAND_BUILTIN_FCONVERT (fixeduwq, V4SImode, V4SFmode)
KVX_EXPAND_BUILTIN_FCONVERT (fixeduwo, V8SImode, V8SFmode)
KVX_EXPAND_BUILTIN_FCONVERT (fixedud, DImode, DFmode)
KVX_EXPAND_BUILTIN_FCONVERT (fixedudp, V2DImode, V2DFmode)
KVX_EXPAND_BUILTIN_FCONVERT (fixedudq, V4DImode, V4DFmode)

KVX_EXPAND_BUILTIN_2_STANDARD (fconjwc, kvx_fconjwc, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fconjwcp, kvx_fconjwcp, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fconjwcq, kvx_fconjwcq, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fconjdc, kvx_fconjdc, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_2_STANDARD (fconjdcp, kvx_fconjdcp, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_3_SILENT (fcdivw, sf, SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_SILENT (fcdivwp, v2sf, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_3_SILENT (fcdivwq, v4sf, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_3_SILENT (fcdivwo, v8sf, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_3_SILENT (fcdivd, df, DFmode, DFmode)
KVX_EXPAND_BUILTIN_3_SILENT (fcdivdp, v2df, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_3_SILENT (fcdivdq, v4df, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_3_SILENT (fsdivw, sf, SFmode, SFmode)
KVX_EXPAND_BUILTIN_3_SILENT (fsdivwp, v2sf, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_3_SILENT (fsdivwq, v4sf, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_3_SILENT (fsdivwo, v8sf, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_3_SILENT (fsdivd, df, DFmode, DFmode)
KVX_EXPAND_BUILTIN_3_SILENT (fsdivdp, v2df, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_3_SILENT (fsdivdq, v4df, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_2_SILENT (fsrecw, SFmode, SFmode)
KVX_EXPAND_BUILTIN_2_SILENT (fsrecwp, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_2_SILENT (fsrecwq, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_2_SILENT (fsrecwo, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_2_SILENT (fsrecd, DFmode, DFmode)
KVX_EXPAND_BUILTIN_2_SILENT (fsrecdp, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_2_SILENT (fsrecdq, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_2_SILENT (fsrsrw, SFmode, SFmode)
KVX_EXPAND_BUILTIN_2_SILENT (fsrsrwp, V2SFmode, V2SFmode)
KVX_EXPAND_BUILTIN_2_SILENT (fsrsrwq, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_2_SILENT (fsrsrwo, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_2_SILENT (fsrsrd, DFmode, DFmode)
KVX_EXPAND_BUILTIN_2_SILENT (fsrsrdp, V2DFmode, V2DFmode)
KVX_EXPAND_BUILTIN_2_SILENT (fsrsrdq, V4DFmode, V4DFmode)

static rtx
kvx_expand_builtin_lbsu (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = gen_rtx_MEM (QImode, force_reg (Pmode, arg1));

  if (!target)
    target = gen_reg_rtx (QImode);
  else
    target = force_reg (QImode, target);

  emit_insn (gen_lbsu (target, arg1));

  return target;
}

static rtx
kvx_expand_builtin_lbzu (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = gen_rtx_MEM (QImode, force_reg (Pmode, arg1));

  if (!target)
    target = gen_reg_rtx (QImode);
  else
    target = force_reg (QImode, target);

  emit_insn (gen_lbzu (target, arg1));

  return target;
}

static rtx
kvx_expand_builtin_ldu (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = gen_rtx_MEM (DImode, force_reg (Pmode, arg1));

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  emit_insn (gen_ldu (target, arg1));

  return target;
}

static rtx
kvx_expand_builtin_lhsu (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = gen_rtx_MEM (HImode, force_reg (Pmode, arg1));

  if (!target)
    target = gen_reg_rtx (HImode);
  else
    target = force_reg (HImode, target);

  emit_insn (gen_lhsu (target, arg1));

  return target;
}

static rtx
kvx_expand_builtin_lhzu (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = gen_rtx_MEM (HImode, force_reg (Pmode, arg1));

  if (!target)
    target = gen_reg_rtx (HImode);
  else
    target = force_reg (HImode, target);

  emit_insn (gen_lhzu (target, arg1));

  return target;
}

static rtx
kvx_expand_builtin_lwzu (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = gen_rtx_MEM (SImode, force_reg (Pmode, arg1));

  if (!target)
    target = gen_reg_rtx (SImode);
  else
    target = force_reg (SImode, target);

  emit_insn (gen_lwzu (target, arg1));

  return target;
}

static rtx
kvx_expand_builtin_stsuw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (SImode, arg1);
  arg2 = force_reg (SImode, arg2);

  if (!target)
    target = gen_reg_rtx (SImode);
  else
    target = force_reg (SImode, target);

  emit_insn (gen_kvx_stsuw (target, arg1, arg2));

  return target;
}

static rtx
kvx_expand_builtin_stsud (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  emit_insn (gen_kvx_stsud (target, arg1, arg2));

  return target;
}

static rtx
kvx_expand_builtin_fwiden (rtx target, tree args, int low_bits)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (SImode, arg1);

  if (!target)
    target = gen_reg_rtx (SFmode);
  else
    target = force_reg (SFmode, target);

  if (low_bits)
    emit_insn (gen_builtin_extendhfsf2 (target, arg1));
  else
    emit_insn (gen_builtin_extendhfsf2_tophalf (target, arg1));

  return target;
}

static rtx
kvx_expand_builtin_fnarrowwh (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (SFmode, arg1);

  if (!target)
    target = gen_reg_rtx (HImode);
  else
    target = force_reg (HImode, target);

  emit_insn (gen_builtin_truncsfhf2 (target, arg1));

  return target;
}

#define KVX_EXPAND_BUILTIN_LX(name, tmode, smode)                              \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = build_bypass_speculate_arg (CALL_EXPR_ARG (args, 1), #name);    \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    arg1 = gen_rtx_MEM (smode, force_reg (Pmode, arg1));                       \
    arg3 = verify_const_bool_arg (arg3, #name, "third");                       \
    MEM_VOLATILE_P (arg1) = (INTVAL (arg3) != 0);                              \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_kvx_##name (target, arg1, arg2));                           \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_SX(name, mode)                                      \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    machine_mode tmode = mode;                                                 \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    arg1 = gen_rtx_MEM (tmode, force_reg (Pmode, arg1));                       \
    arg2 = force_reg (tmode, arg2);                                            \
    arg3 = verify_const_bool_arg (arg3, #name, "third");                       \
    MEM_VOLATILE_P (arg1) = (INTVAL (arg3) != 0);                              \
    emit_insn (gen_kvx_##name (arg1, arg2));                                   \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_LV(name, mode)                                      \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    machine_mode smode = mode;                                                 \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = build_scatter_speculate_arg (CALL_EXPR_ARG (args, 2), #name);   \
    arg1 = verify_const_uint_arg (arg1, 6, #name, "first");                    \
    arg2 = gen_rtx_MEM (smode, force_reg (Pmode, arg2));                       \
    const char *string = XSTR (arg3, 0);                                       \
    if (*string && string[1] == 'c') {                                         \
      arg1 = build_xmr_reg_name_arg (arg1);                                    \
      emit_insn (gen_kvx_##name (arg1, arg2, arg3));                           \
    } else {                                                                   \
      arg1 = build_xvr_reg_name_arg (arg1);                                    \
      emit_insn (gen_kvx_##name (arg1, arg2, arg3));                           \
    }                                                                          \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_SV(name, mode)                                      \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    machine_mode smode = mode;                                                 \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    arg1 = gen_rtx_MEM (smode, force_reg (Pmode, arg1));                       \
    arg2 = verify_const_uint_arg (arg2, 6, #name, "second");                   \
    arg2 = build_xvr_reg_name_arg (arg2);                                      \
    emit_insn (gen_kvx_##name (arg1, arg2));                                   \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_MOVEFO(name, mode)                                  \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    machine_mode tmode = mode;                                                 \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    arg1 = verify_const_uint_arg (arg1, 6, #name, "first");                    \
    arg1 = build_xvr_reg_name_arg (arg1);                                      \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_kvx_##name (target, arg1));                                 \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_MOVETO(name, mode)                                  \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    machine_mode smode = mode;                                                 \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    arg1 = verify_const_uint_arg (arg1, 6, #name, "first");                    \
    arg1 = build_xvr_reg_name_arg (arg1);                                      \
    arg2 = force_reg (smode, arg2);                                            \
    emit_insn (gen_kvx_##name (arg1, arg2));                                   \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_SWAPVO(name, mode)                                   \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    machine_mode tmode = mode;                                                 \
    machine_mode smode = mode;                                                 \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    arg1 = verify_const_uint_arg (arg1, 6, #name, "first");                    \
    arg1 = build_xvr_reg_name_arg (arg1);                                      \
    arg2 = force_reg (smode, arg2);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_kvx_##name (target, arg1, arg2));                           \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_ALIGNO(name, mode)                                  \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    machine_mode tmode = mode;                                                 \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    arg1 = verify_const_uint_arg (arg1, 6, #name, "first");                    \
    arg1 = build_xvr_reg_name_arg (arg1);                                      \
    arg2 = verify_const_uint_arg (arg2, 6, #name, "second");                   \
    arg2 = build_xvr_reg_name_arg (arg2);                                      \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_kvx_##name (target, arg1, arg2, arg3));                     \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_MM4AX(name)                                         \
  static rtx                                                                   \
  kvx_expand_builtin_##name (rtx target, tree args)                            \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    rtx arg4 = expand_normal (CALL_EXPR_ARG (args, 3));                        \
    arg1 = verify_const_uint_arg (arg1, 6, #name, "first");                    \
    arg1 = build_xwr_reg_name_arg (arg1);                                      \
    arg2 = verify_const_uint_arg (arg2, 6, #name, "second");                   \
    arg2 = build_xwr_reg_name_arg (arg2);                                      \
    arg3 = verify_const_uint_arg (arg3, 6, #name, "third");                    \
    arg3 = build_xvr_reg_name_arg (arg3);                                      \
    arg4 = verify_const_uint_arg (arg4, 6, #name, "fourth");                   \
    arg4 = build_xvr_reg_name_arg (arg4);                                      \
    emit_insn (gen_kvx_##name (arg1, arg2, arg3, arg4));                       \
    return target;                                                             \
  }

KVX_EXPAND_BUILTIN_LX (lbz, DImode, QImode)
KVX_EXPAND_BUILTIN_LX (lbs, DImode, QImode)
KVX_EXPAND_BUILTIN_LX (lhz, DImode, HImode)
KVX_EXPAND_BUILTIN_LX (lhs, DImode, HImode)
KVX_EXPAND_BUILTIN_LX (lwz, DImode, SImode)
KVX_EXPAND_BUILTIN_LX (lws, DImode, SImode)
KVX_EXPAND_BUILTIN_LX (ld, DImode, DImode)
KVX_EXPAND_BUILTIN_LX (lq, TImode, TImode)
KVX_EXPAND_BUILTIN_LX (lwf, SFmode, SFmode)
KVX_EXPAND_BUILTIN_LX (ldf, DFmode, DFmode)

KVX_EXPAND_BUILTIN_LX (ldbo, V8QImode, V8QImode)
KVX_EXPAND_BUILTIN_LX (ldhq, V4HImode, V4HImode)
KVX_EXPAND_BUILTIN_LX (ldwp, V2SImode, V2SImode)
KVX_EXPAND_BUILTIN_LX (ldfwp, V2SFmode, V2SFmode)

KVX_EXPAND_BUILTIN_LX (lqbx, V16QImode, V16QImode)
KVX_EXPAND_BUILTIN_LX (lqho, V8HImode, V8HImode)
KVX_EXPAND_BUILTIN_LX (lqwq, V4SImode, V4SImode)
KVX_EXPAND_BUILTIN_LX (lqdp, V2DImode, V2DImode)
KVX_EXPAND_BUILTIN_LX (lqfwq, V4SFmode, V4SFmode)
KVX_EXPAND_BUILTIN_LX (lqfdp, V2DFmode, V2DFmode)

KVX_EXPAND_BUILTIN_LX (lobv, V32QImode, V32QImode)
KVX_EXPAND_BUILTIN_LX (lohx, V16HImode, V16HImode)
KVX_EXPAND_BUILTIN_LX (lowo, V8SImode, V8SImode)
KVX_EXPAND_BUILTIN_LX (lodq, V4DImode, V4DImode)
KVX_EXPAND_BUILTIN_LX (lofwo, V8SFmode, V8SFmode)
KVX_EXPAND_BUILTIN_LX (lofdq, V4DFmode, V4DFmode)

KVX_EXPAND_BUILTIN_SX (sdbo, V8QImode)
KVX_EXPAND_BUILTIN_SX (sdhq, V4HImode)
KVX_EXPAND_BUILTIN_SX (sdwp, V2SImode)
KVX_EXPAND_BUILTIN_SX (sdfwp, V2SFmode)

KVX_EXPAND_BUILTIN_SX (sqbx, V16QImode)
KVX_EXPAND_BUILTIN_SX (sqho, V8HImode)
KVX_EXPAND_BUILTIN_SX (sqwq, V4SImode)
KVX_EXPAND_BUILTIN_SX (sqdp, V2DImode)
KVX_EXPAND_BUILTIN_SX (sqfwq, V4SFmode)
KVX_EXPAND_BUILTIN_SX (sqfdp, V2DFmode)

KVX_EXPAND_BUILTIN_SX (sobv, V32QImode)
KVX_EXPAND_BUILTIN_SX (sohx, V16HImode)
KVX_EXPAND_BUILTIN_SX (sowo, V8SImode)
KVX_EXPAND_BUILTIN_SX (sodq, V4DImode)
KVX_EXPAND_BUILTIN_SX (sofwo, V8SFmode)
KVX_EXPAND_BUILTIN_SX (sofdq, V4DFmode)

KVX_EXPAND_BUILTIN_LV (lvbv, V32QImode)
KVX_EXPAND_BUILTIN_LV (lvhx, V16HImode)
KVX_EXPAND_BUILTIN_LV (lvwo, V8SImode)
KVX_EXPAND_BUILTIN_LV (lvdq, V4DImode)
KVX_EXPAND_BUILTIN_LV (lvfwo, V8SFmode)
KVX_EXPAND_BUILTIN_LV (lvfdq, V4DFmode)

KVX_EXPAND_BUILTIN_SV (svbv, V32QImode)
KVX_EXPAND_BUILTIN_SV (svhx, V16HImode)
KVX_EXPAND_BUILTIN_SV (svwo, V8SImode)
KVX_EXPAND_BUILTIN_SV (svdq, V4DImode)
KVX_EXPAND_BUILTIN_SV (svfwo, V8SFmode)
KVX_EXPAND_BUILTIN_SV (svfdq, V4DFmode)

KVX_EXPAND_BUILTIN_MOVEFO (movefobv, V32QImode)
KVX_EXPAND_BUILTIN_MOVEFO (movefohx, V16HImode)
KVX_EXPAND_BUILTIN_MOVEFO (movefowo, V8SImode)
KVX_EXPAND_BUILTIN_MOVEFO (movefodq, V4DImode)
KVX_EXPAND_BUILTIN_MOVEFO (movefofwo, V8SFmode)
KVX_EXPAND_BUILTIN_MOVEFO (movefofdq, V4DFmode)

KVX_EXPAND_BUILTIN_MOVETO (movetobv, V32QImode)
KVX_EXPAND_BUILTIN_MOVETO (movetohx, V16HImode)
KVX_EXPAND_BUILTIN_MOVETO (movetowo, V8SImode)
KVX_EXPAND_BUILTIN_MOVETO (movetodq, V4DImode)
KVX_EXPAND_BUILTIN_MOVETO (movetofwo, V8SFmode)
KVX_EXPAND_BUILTIN_MOVETO (movetofdq, V4DFmode)

KVX_EXPAND_BUILTIN_SWAPVO (swapvobv, V32QImode)
KVX_EXPAND_BUILTIN_SWAPVO (swapvohx, V16HImode)
KVX_EXPAND_BUILTIN_SWAPVO (swapvowo, V8SImode)
KVX_EXPAND_BUILTIN_SWAPVO (swapvodq, V4DImode)
KVX_EXPAND_BUILTIN_SWAPVO (swapvofwo, V8SFmode)
KVX_EXPAND_BUILTIN_SWAPVO (swapvofdq, V4DFmode)

KVX_EXPAND_BUILTIN_ALIGNO (alignobv, V32QImode)
KVX_EXPAND_BUILTIN_ALIGNO (alignohx, V16HImode)
KVX_EXPAND_BUILTIN_ALIGNO (alignowo, V8SImode)
KVX_EXPAND_BUILTIN_ALIGNO (alignodq, V4DImode)
KVX_EXPAND_BUILTIN_ALIGNO (alignofwo, V8SFmode)
KVX_EXPAND_BUILTIN_ALIGNO (alignofdq, V4DFmode)

static rtx
kvx_expand_builtin_alignv (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));
  rtx arg4 = expand_normal (CALL_EXPR_ARG (args, 3));
  arg1 = verify_const_uint_arg (arg1, 6, "alignv", "first");
  arg1 = build_xvr_reg_name_arg (arg1);
  arg2 = verify_const_uint_arg (arg2, 6, "alignv", "second");
  arg2 = build_xvr_reg_name_arg (arg2);
  arg3 = verify_const_uint_arg (arg3, 6, "alignv", "third");
  arg3 = build_xvr_reg_name_arg (arg3);
  emit_insn (gen_kvx_alignv (arg1, arg2, arg3, arg4));
  return target;
}

static rtx
kvx_expand_builtin_copyv (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = verify_const_uint_arg (arg1, 6, "copyv", "first");
  arg1 = build_xvr_reg_name_arg (arg1);
  arg2 = verify_const_uint_arg (arg2, 6, "copyv", "second");
  arg2 = build_xvr_reg_name_arg (arg2);
  emit_insn (gen_kvx_copyv (arg1, arg2));
  return target;
}

static rtx
kvx_expand_builtin_mt4x4d (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = verify_const_uint_arg (arg1, 6, "mt4x4d", "first");
  arg1 = build_xmr_reg_name_arg (arg1);
  arg2 = verify_const_uint_arg (arg2, 6, "mt4x4d", "second");
  arg2 = build_xmr_reg_name_arg (arg2);
  emit_insn (gen_kvx_mt4x4d (arg1, arg2));
  return target;
}

KVX_EXPAND_BUILTIN_MM4AX (mm4abw)
KVX_EXPAND_BUILTIN_MM4AX (fmm4ahw0)
KVX_EXPAND_BUILTIN_MM4AX (fmm4ahw1)
KVX_EXPAND_BUILTIN_MM4AX (fmm4ahw2)
KVX_EXPAND_BUILTIN_MM4AX (fmm4ahw3)

static rtx
kvx_expand_builtin (tree exp, rtx target, rtx subtarget ATTRIBUTE_UNUSED,
		    enum machine_mode mode ATTRIBUTE_UNUSED,
		    int ignore ATTRIBUTE_UNUSED)
{
  tree fndecl = TREE_OPERAND (CALL_EXPR_FN (exp), 0);
  unsigned int fcode = DECL_FUNCTION_CODE (fndecl);

  switch (fcode)
    {
    case KVX_BUILTIN_ADDD: return kvx_expand_builtin_addd (target, exp);
    case KVX_BUILTIN_SBFD: return kvx_expand_builtin_sbfd (target, exp);

    case KVX_BUILTIN_ABDHQ: return kvx_expand_builtin_abdhq (target, exp);
    case KVX_BUILTIN_ABDHO: return kvx_expand_builtin_abdho (target, exp);
    case KVX_BUILTIN_ABDHX: return kvx_expand_builtin_abdhx (target, exp);
    case KVX_BUILTIN_ABDW: return kvx_expand_builtin_abdw (target, exp);
    case KVX_BUILTIN_ABDWP: return kvx_expand_builtin_abdwp (target, exp);
    case KVX_BUILTIN_ABDWQ: return kvx_expand_builtin_abdwq (target, exp);
    case KVX_BUILTIN_ABDWO: return kvx_expand_builtin_abdwo (target, exp);
    case KVX_BUILTIN_ABDD: return kvx_expand_builtin_abdd (target, exp);
    case KVX_BUILTIN_ABDDP: return kvx_expand_builtin_abddp (target, exp);
    case KVX_BUILTIN_ABDDQ: return kvx_expand_builtin_abddq (target, exp);

    case KVX_BUILTIN_ABDHQS: return kvx_expand_builtin_abdhqs (target, exp);
    case KVX_BUILTIN_ABDHOS: return kvx_expand_builtin_abdhos (target, exp);
    case KVX_BUILTIN_ABDHXS: return kvx_expand_builtin_abdhxs (target, exp);
    case KVX_BUILTIN_ABDWPS: return kvx_expand_builtin_abdwps (target, exp);
    case KVX_BUILTIN_ABDWQS: return kvx_expand_builtin_abdwqs (target, exp);
    case KVX_BUILTIN_ABDWOS: return kvx_expand_builtin_abdwos (target, exp);
    case KVX_BUILTIN_ABDDPS: return kvx_expand_builtin_abddps (target, exp);
    case KVX_BUILTIN_ABDDQS: return kvx_expand_builtin_abddqs (target, exp);

    case KVX_BUILTIN_AVGHQ: return kvx_expand_builtin_avghq (target, exp);
    case KVX_BUILTIN_AVGHO: return kvx_expand_builtin_avgho (target, exp);
    case KVX_BUILTIN_AVGHX: return kvx_expand_builtin_avghx (target, exp);
    case KVX_BUILTIN_AVGW: return kvx_expand_builtin_avgw (target, exp);
    case KVX_BUILTIN_AVGWP: return kvx_expand_builtin_avgwp (target, exp);
    case KVX_BUILTIN_AVGWQ: return kvx_expand_builtin_avgwq (target, exp);
    case KVX_BUILTIN_AVGWO: return kvx_expand_builtin_avgwo (target, exp);

    case KVX_BUILTIN_AVGUHQ: return kvx_expand_builtin_avguhq (target, exp);
    case KVX_BUILTIN_AVGUHO: return kvx_expand_builtin_avguho (target, exp);
    case KVX_BUILTIN_AVGUHX: return kvx_expand_builtin_avguhx (target, exp);
    case KVX_BUILTIN_AVGUW: return kvx_expand_builtin_avguw (target, exp);
    case KVX_BUILTIN_AVGUWP: return kvx_expand_builtin_avguwp (target, exp);
    case KVX_BUILTIN_AVGUWQ: return kvx_expand_builtin_avguwq (target, exp);
    case KVX_BUILTIN_AVGUWO: return kvx_expand_builtin_avguwo (target, exp);

    case KVX_BUILTIN_AVGRHQ: return kvx_expand_builtin_avgrhq (target, exp);
    case KVX_BUILTIN_AVGRHO: return kvx_expand_builtin_avgrho (target, exp);
    case KVX_BUILTIN_AVGRHX: return kvx_expand_builtin_avgrhx (target, exp);
    case KVX_BUILTIN_AVGRW: return kvx_expand_builtin_avgrw (target, exp);
    case KVX_BUILTIN_AVGRWP: return kvx_expand_builtin_avgrwp (target, exp);
    case KVX_BUILTIN_AVGRWQ: return kvx_expand_builtin_avgrwq (target, exp);
    case KVX_BUILTIN_AVGRWO: return kvx_expand_builtin_avgrwo (target, exp);

    case KVX_BUILTIN_AVGRUHQ: return kvx_expand_builtin_avgruhq (target, exp);
    case KVX_BUILTIN_AVGRUHO: return kvx_expand_builtin_avgruho (target, exp);
    case KVX_BUILTIN_AVGRUHX: return kvx_expand_builtin_avgruhx (target, exp);
    case KVX_BUILTIN_AVGRUW: return kvx_expand_builtin_avgruw (target, exp);
    case KVX_BUILTIN_AVGRUWP: return kvx_expand_builtin_avgruwp (target, exp);
    case KVX_BUILTIN_AVGRUWQ: return kvx_expand_builtin_avgruwq (target, exp);
    case KVX_BUILTIN_AVGRUWO: return kvx_expand_builtin_avgruwo (target, exp);

    case KVX_BUILTIN_ADDSHQ: return kvx_expand_builtin_addshq (target, exp);
    case KVX_BUILTIN_ADDSHO: return kvx_expand_builtin_addsho (target, exp);
    case KVX_BUILTIN_ADDSHX: return kvx_expand_builtin_addshx (target, exp);
    case KVX_BUILTIN_ADDSW: return kvx_expand_builtin_addsw (target, exp);
    case KVX_BUILTIN_ADDSWP: return kvx_expand_builtin_addswp (target, exp);
    case KVX_BUILTIN_ADDSWQ: return kvx_expand_builtin_addswq (target, exp);
    case KVX_BUILTIN_ADDSWO: return kvx_expand_builtin_addswo (target, exp);
    case KVX_BUILTIN_ADDSD: return kvx_expand_builtin_addsd (target, exp);
    case KVX_BUILTIN_ADDSDP: return kvx_expand_builtin_addsdp (target, exp);
    case KVX_BUILTIN_ADDSDQ: return kvx_expand_builtin_addsdq (target, exp);

    case KVX_BUILTIN_ADDSHQS: return kvx_expand_builtin_addshqs (target, exp);
    case KVX_BUILTIN_ADDSHOS: return kvx_expand_builtin_addshos (target, exp);
    case KVX_BUILTIN_ADDSHXS: return kvx_expand_builtin_addshxs (target, exp);
    case KVX_BUILTIN_ADDSWPS: return kvx_expand_builtin_addswps (target, exp);
    case KVX_BUILTIN_ADDSWQS: return kvx_expand_builtin_addswqs (target, exp);
    case KVX_BUILTIN_ADDSWOS: return kvx_expand_builtin_addswos (target, exp);
    case KVX_BUILTIN_ADDSDPS: return kvx_expand_builtin_addsdps (target, exp);
    case KVX_BUILTIN_ADDSDQS: return kvx_expand_builtin_addsdqs (target, exp);

    case KVX_BUILTIN_SBFSHQ: return kvx_expand_builtin_sbfshq (target, exp);
    case KVX_BUILTIN_SBFSHO: return kvx_expand_builtin_sbfsho (target, exp);
    case KVX_BUILTIN_SBFSHX: return kvx_expand_builtin_sbfshx (target, exp);
    case KVX_BUILTIN_SBFSW: return kvx_expand_builtin_sbfsw (target, exp);
    case KVX_BUILTIN_SBFSWP: return kvx_expand_builtin_sbfswp (target, exp);
    case KVX_BUILTIN_SBFSWQ: return kvx_expand_builtin_sbfswq (target, exp);
    case KVX_BUILTIN_SBFSWO: return kvx_expand_builtin_sbfswo (target, exp);
    case KVX_BUILTIN_SBFSD: return kvx_expand_builtin_sbfsd (target, exp);
    case KVX_BUILTIN_SBFSDP: return kvx_expand_builtin_sbfsdp (target, exp);
    case KVX_BUILTIN_SBFSDQ: return kvx_expand_builtin_sbfsdq (target, exp);

    case KVX_BUILTIN_SBFSHQS: return kvx_expand_builtin_sbfshqs (target, exp);
    case KVX_BUILTIN_SBFSHOS: return kvx_expand_builtin_sbfshos (target, exp);
    case KVX_BUILTIN_SBFSHXS: return kvx_expand_builtin_sbfshxs (target, exp);
    case KVX_BUILTIN_SBFSWPS: return kvx_expand_builtin_sbfswps (target, exp);
    case KVX_BUILTIN_SBFSWQS: return kvx_expand_builtin_sbfswqs (target, exp);
    case KVX_BUILTIN_SBFSWOS: return kvx_expand_builtin_sbfswos (target, exp);
    case KVX_BUILTIN_SBFSDPS: return kvx_expand_builtin_sbfsdps (target, exp);
    case KVX_BUILTIN_SBFSDQS: return kvx_expand_builtin_sbfsdqs (target, exp);

    case KVX_BUILTIN_MINHQ: return kvx_expand_builtin_minhq (target, exp);
    case KVX_BUILTIN_MINHO: return kvx_expand_builtin_minho (target, exp);
    case KVX_BUILTIN_MINHX: return kvx_expand_builtin_minhx (target, exp);
    case KVX_BUILTIN_MINW: return kvx_expand_builtin_minw (target, exp);
    case KVX_BUILTIN_MINWP: return kvx_expand_builtin_minwp (target, exp);
    case KVX_BUILTIN_MINWQ: return kvx_expand_builtin_minwq (target, exp);
    case KVX_BUILTIN_MINWO: return kvx_expand_builtin_minwo (target, exp);
    case KVX_BUILTIN_MIND: return kvx_expand_builtin_mind (target, exp);
    case KVX_BUILTIN_MINDP: return kvx_expand_builtin_mindp (target, exp);
    case KVX_BUILTIN_MINDQ: return kvx_expand_builtin_mindq (target, exp);

    case KVX_BUILTIN_MINHQS: return kvx_expand_builtin_minhqs (target, exp);
    case KVX_BUILTIN_MINHOS: return kvx_expand_builtin_minhos (target, exp);
    case KVX_BUILTIN_MINHXS: return kvx_expand_builtin_minhxs (target, exp);
    case KVX_BUILTIN_MINWPS: return kvx_expand_builtin_minwps (target, exp);
    case KVX_BUILTIN_MINWQS: return kvx_expand_builtin_minwqs (target, exp);
    case KVX_BUILTIN_MINWOS: return kvx_expand_builtin_minwos (target, exp);
    case KVX_BUILTIN_MINDPS: return kvx_expand_builtin_mindps (target, exp);
    case KVX_BUILTIN_MINDQS: return kvx_expand_builtin_mindqs (target, exp);

    case KVX_BUILTIN_MAXHQ: return kvx_expand_builtin_maxhq (target, exp);
    case KVX_BUILTIN_MAXHO: return kvx_expand_builtin_maxho (target, exp);
    case KVX_BUILTIN_MAXHX: return kvx_expand_builtin_maxhx (target, exp);
    case KVX_BUILTIN_MAXW: return kvx_expand_builtin_maxw (target, exp);
    case KVX_BUILTIN_MAXWP: return kvx_expand_builtin_maxwp (target, exp);
    case KVX_BUILTIN_MAXWQ: return kvx_expand_builtin_maxwq (target, exp);
    case KVX_BUILTIN_MAXWO: return kvx_expand_builtin_maxwo (target, exp);
    case KVX_BUILTIN_MAXD: return kvx_expand_builtin_maxd (target, exp);
    case KVX_BUILTIN_MAXDP: return kvx_expand_builtin_maxdp (target, exp);
    case KVX_BUILTIN_MAXDQ: return kvx_expand_builtin_maxdq (target, exp);

    case KVX_BUILTIN_MAXHQS: return kvx_expand_builtin_maxhqs (target, exp);
    case KVX_BUILTIN_MAXHOS: return kvx_expand_builtin_maxhos (target, exp);
    case KVX_BUILTIN_MAXHXS: return kvx_expand_builtin_maxhxs (target, exp);
    case KVX_BUILTIN_MAXWPS: return kvx_expand_builtin_maxwps (target, exp);
    case KVX_BUILTIN_MAXWQS: return kvx_expand_builtin_maxwqs (target, exp);
    case KVX_BUILTIN_MAXWOS: return kvx_expand_builtin_maxwos (target, exp);
    case KVX_BUILTIN_MAXDPS: return kvx_expand_builtin_maxdps (target, exp);
    case KVX_BUILTIN_MAXDQS: return kvx_expand_builtin_maxdqs (target, exp);

    case KVX_BUILTIN_MINUHQ: return kvx_expand_builtin_minuhq (target, exp);
    case KVX_BUILTIN_MINUHO: return kvx_expand_builtin_minuho (target, exp);
    case KVX_BUILTIN_MINUHX: return kvx_expand_builtin_minuhx (target, exp);
    case KVX_BUILTIN_MINUW: return kvx_expand_builtin_minuw (target, exp);
    case KVX_BUILTIN_MINUWP: return kvx_expand_builtin_minuwp (target, exp);
    case KVX_BUILTIN_MINUWQ: return kvx_expand_builtin_minuwq (target, exp);
    case KVX_BUILTIN_MINUWO: return kvx_expand_builtin_minuwo (target, exp);
    case KVX_BUILTIN_MINUD: return kvx_expand_builtin_minud (target, exp);
    case KVX_BUILTIN_MINUDP: return kvx_expand_builtin_minudp (target, exp);
    case KVX_BUILTIN_MINUDQ: return kvx_expand_builtin_minudq (target, exp);

    case KVX_BUILTIN_MINUHQS: return kvx_expand_builtin_minuhqs (target, exp);
    case KVX_BUILTIN_MINUHOS: return kvx_expand_builtin_minuhos (target, exp);
    case KVX_BUILTIN_MINUHXS: return kvx_expand_builtin_minuhxs (target, exp);
    case KVX_BUILTIN_MINUWPS: return kvx_expand_builtin_minuwps (target, exp);
    case KVX_BUILTIN_MINUWQS: return kvx_expand_builtin_minuwqs (target, exp);
    case KVX_BUILTIN_MINUWOS: return kvx_expand_builtin_minuwos (target, exp);
    case KVX_BUILTIN_MINUDPS: return kvx_expand_builtin_minudps (target, exp);
    case KVX_BUILTIN_MINUDQS: return kvx_expand_builtin_minudqs (target, exp);

    case KVX_BUILTIN_MAXUHQ: return kvx_expand_builtin_maxuhq (target, exp);
    case KVX_BUILTIN_MAXUHO: return kvx_expand_builtin_maxuho (target, exp);
    case KVX_BUILTIN_MAXUHX: return kvx_expand_builtin_maxuhx (target, exp);
    case KVX_BUILTIN_MAXUW: return kvx_expand_builtin_maxuw (target, exp);
    case KVX_BUILTIN_MAXUWP: return kvx_expand_builtin_maxuwp (target, exp);
    case KVX_BUILTIN_MAXUWQ: return kvx_expand_builtin_maxuwq (target, exp);
    case KVX_BUILTIN_MAXUWO: return kvx_expand_builtin_maxuwo (target, exp);
    case KVX_BUILTIN_MAXUD: return kvx_expand_builtin_maxud (target, exp);
    case KVX_BUILTIN_MAXUDP: return kvx_expand_builtin_maxudp (target, exp);
    case KVX_BUILTIN_MAXUDQ: return kvx_expand_builtin_maxudq (target, exp);

    case KVX_BUILTIN_MAXUHQS: return kvx_expand_builtin_maxuhqs (target, exp);
    case KVX_BUILTIN_MAXUHOS: return kvx_expand_builtin_maxuhos (target, exp);
    case KVX_BUILTIN_MAXUHXS: return kvx_expand_builtin_maxuhxs (target, exp);
    case KVX_BUILTIN_MAXUWPS: return kvx_expand_builtin_maxuwps (target, exp);
    case KVX_BUILTIN_MAXUWQS: return kvx_expand_builtin_maxuwqs (target, exp);
    case KVX_BUILTIN_MAXUWOS: return kvx_expand_builtin_maxuwos (target, exp);
    case KVX_BUILTIN_MAXUDPS: return kvx_expand_builtin_maxudps (target, exp);
    case KVX_BUILTIN_MAXUDQS: return kvx_expand_builtin_maxudqs (target, exp);

    case KVX_BUILTIN_SLLHQS: return kvx_expand_builtin_sllhqs (target, exp);
    case KVX_BUILTIN_SLLHOS: return kvx_expand_builtin_sllhos (target, exp);
    case KVX_BUILTIN_SLLHXS: return kvx_expand_builtin_sllhxs (target, exp);
    case KVX_BUILTIN_SLLWPS: return kvx_expand_builtin_sllwps (target, exp);
    case KVX_BUILTIN_SLLWQS: return kvx_expand_builtin_sllwqs (target, exp);
    case KVX_BUILTIN_SLLWOS: return kvx_expand_builtin_sllwos (target, exp);
    case KVX_BUILTIN_SLLDPS: return kvx_expand_builtin_slldps (target, exp);
    case KVX_BUILTIN_SLLDQS: return kvx_expand_builtin_slldqs (target, exp);

    case KVX_BUILTIN_SLSHQS: return kvx_expand_builtin_slshqs (target, exp);
    case KVX_BUILTIN_SLSHOS: return kvx_expand_builtin_slshos (target, exp);
    case KVX_BUILTIN_SLSHXS: return kvx_expand_builtin_slshxs (target, exp);
    case KVX_BUILTIN_SLSWPS: return kvx_expand_builtin_slswps (target, exp);
    case KVX_BUILTIN_SLSWQS: return kvx_expand_builtin_slswqs (target, exp);
    case KVX_BUILTIN_SLSWOS: return kvx_expand_builtin_slswos (target, exp);
    case KVX_BUILTIN_SLSDPS: return kvx_expand_builtin_slsdps (target, exp);
    case KVX_BUILTIN_SLSDQS: return kvx_expand_builtin_slsdqs (target, exp);

    case KVX_BUILTIN_SRAHQS: return kvx_expand_builtin_srahqs (target, exp);
    case KVX_BUILTIN_SRAHOS: return kvx_expand_builtin_srahos (target, exp);
    case KVX_BUILTIN_SRAHXS: return kvx_expand_builtin_srahxs (target, exp);
    case KVX_BUILTIN_SRAWPS: return kvx_expand_builtin_srawps (target, exp);
    case KVX_BUILTIN_SRAWQS: return kvx_expand_builtin_srawqs (target, exp);
    case KVX_BUILTIN_SRAWOS: return kvx_expand_builtin_srawos (target, exp);
    case KVX_BUILTIN_SRADPS: return kvx_expand_builtin_sradps (target, exp);
    case KVX_BUILTIN_SRADQS: return kvx_expand_builtin_sradqs (target, exp);

    case KVX_BUILTIN_SRLHQS: return kvx_expand_builtin_srlhqs (target, exp);
    case KVX_BUILTIN_SRLHOS: return kvx_expand_builtin_srlhos (target, exp);
    case KVX_BUILTIN_SRLHXS: return kvx_expand_builtin_srlhxs (target, exp);
    case KVX_BUILTIN_SRLWPS: return kvx_expand_builtin_srlwps (target, exp);
    case KVX_BUILTIN_SRLWQS: return kvx_expand_builtin_srlwqs (target, exp);
    case KVX_BUILTIN_SRLWOS: return kvx_expand_builtin_srlwos (target, exp);
    case KVX_BUILTIN_SRLDPS: return kvx_expand_builtin_srldps (target, exp);
    case KVX_BUILTIN_SRLDQS: return kvx_expand_builtin_srldqs (target, exp);

    case KVX_BUILTIN_SRSHQS: return kvx_expand_builtin_srshqs (target, exp);
    case KVX_BUILTIN_SRSHOS: return kvx_expand_builtin_srshos (target, exp);
    case KVX_BUILTIN_SRSHXS: return kvx_expand_builtin_srshxs (target, exp);
    case KVX_BUILTIN_SRSWPS: return kvx_expand_builtin_srswps (target, exp);
    case KVX_BUILTIN_SRSWQS: return kvx_expand_builtin_srswqs (target, exp);
    case KVX_BUILTIN_SRSWOS: return kvx_expand_builtin_srswos (target, exp);
    case KVX_BUILTIN_SRSDPS: return kvx_expand_builtin_srsdps (target, exp);
    case KVX_BUILTIN_SRSDQS: return kvx_expand_builtin_srsdqs (target, exp);

    case KVX_BUILTIN_CLSW: return kvx_expand_builtin_clsw (target, exp);
    case KVX_BUILTIN_CLSWP: return kvx_expand_builtin_clswp (target, exp);
    case KVX_BUILTIN_CLSWQ: return kvx_expand_builtin_clswq (target, exp);
    case KVX_BUILTIN_CLSWO: return kvx_expand_builtin_clswo (target, exp);
    case KVX_BUILTIN_CLSD: return kvx_expand_builtin_clsd (target, exp);
    case KVX_BUILTIN_CLSDP: return kvx_expand_builtin_clsdp (target, exp);
    case KVX_BUILTIN_CLSDQ: return kvx_expand_builtin_clsdq (target, exp);

    case KVX_BUILTIN_CLZW: return kvx_expand_builtin_clzw (target, exp);
    case KVX_BUILTIN_CLZWP: return kvx_expand_builtin_clzwp (target, exp);
    case KVX_BUILTIN_CLZWQ: return kvx_expand_builtin_clzwq (target, exp);
    case KVX_BUILTIN_CLZWO: return kvx_expand_builtin_clzwo (target, exp);
    case KVX_BUILTIN_CLZD: return kvx_expand_builtin_clzd (target, exp);
    case KVX_BUILTIN_CLZDP: return kvx_expand_builtin_clzdp (target, exp);
    case KVX_BUILTIN_CLZDQ: return kvx_expand_builtin_clzdq (target, exp);

    case KVX_BUILTIN_CTZW: return kvx_expand_builtin_ctzw (target, exp);
    case KVX_BUILTIN_CTZWP: return kvx_expand_builtin_ctzwp (target, exp);
    case KVX_BUILTIN_CTZWQ: return kvx_expand_builtin_ctzwq (target, exp);
    case KVX_BUILTIN_CTZWO: return kvx_expand_builtin_ctzwo (target, exp);
    case KVX_BUILTIN_CTZD: return kvx_expand_builtin_ctzd (target, exp);
    case KVX_BUILTIN_CTZDP: return kvx_expand_builtin_ctzdp (target, exp);
    case KVX_BUILTIN_CTZDQ: return kvx_expand_builtin_ctzdq (target, exp);

    case KVX_BUILTIN_CBSW: return kvx_expand_builtin_cbsw (target, exp);
    case KVX_BUILTIN_CBSWP: return kvx_expand_builtin_cbswp (target, exp);
    case KVX_BUILTIN_CBSWQ: return kvx_expand_builtin_cbswq (target, exp);
    case KVX_BUILTIN_CBSWO: return kvx_expand_builtin_cbswo (target, exp);
    case KVX_BUILTIN_CBSD: return kvx_expand_builtin_cbsd (target, exp);
    case KVX_BUILTIN_CBSDP: return kvx_expand_builtin_cbsdp (target, exp);
    case KVX_BUILTIN_CBSDQ: return kvx_expand_builtin_cbsdq (target, exp);

    case KVX_BUILTIN_AWAIT: return kvx_expand_builtin_await (target, exp);
    case KVX_BUILTIN_BARRIER: return kvx_expand_builtin_barrier ();
    case KVX_BUILTIN_ACSWAPW: return kvx_expand_builtin_acswap (target, exp, SImode);
    case KVX_BUILTIN_ACSWAPD: return kvx_expand_builtin_acswap (target, exp, DImode);
    case KVX_BUILTIN_ALADDD: return kvx_expand_builtin_aladd (target, exp, DImode);
    case KVX_BUILTIN_ALADDW: return kvx_expand_builtin_aladd (target, exp, SImode);
    case KVX_BUILTIN_ALCLRD: return kvx_expand_builtin_alclr (target, exp, DImode);
    case KVX_BUILTIN_ALCLRW: return kvx_expand_builtin_alclr (target, exp, SImode);
    case KVX_BUILTIN_DINVAL: return kvx_expand_builtin_dinval ();
    case KVX_BUILTIN_DINVALL: return kvx_expand_builtin_dinvall (target, exp);
    case KVX_BUILTIN_DTOUCHL: return kvx_expand_builtin_dtouchl (target, exp);
    case KVX_BUILTIN_DZEROL: return kvx_expand_builtin_dzerol (target, exp);
    case KVX_BUILTIN_FENCE: return kvx_expand_builtin_fence ();

    case KVX_BUILTIN_SELECTHQ: return kvx_expand_builtin_selecthq (target, exp);
    case KVX_BUILTIN_SELECTHO: return kvx_expand_builtin_selectho (target, exp);
    case KVX_BUILTIN_SELECTHX: return kvx_expand_builtin_selecthx (target, exp);
    case KVX_BUILTIN_SELECTWP: return kvx_expand_builtin_selectwp (target, exp);
    case KVX_BUILTIN_SELECTWQ: return kvx_expand_builtin_selectwq (target, exp);
    case KVX_BUILTIN_SELECTWO: return kvx_expand_builtin_selectwo (target, exp);
    case KVX_BUILTIN_SELECTDP: return kvx_expand_builtin_selectdp (target, exp);
    case KVX_BUILTIN_SELECTDQ: return kvx_expand_builtin_selectdq (target, exp);
    case KVX_BUILTIN_SELECTFWP: return kvx_expand_builtin_selectfwp (target, exp);
    case KVX_BUILTIN_SELECTFWQ: return kvx_expand_builtin_selectfwq (target, exp);
    case KVX_BUILTIN_SELECTFWO: return kvx_expand_builtin_selectfwo (target, exp);
    case KVX_BUILTIN_SELECTFDP: return kvx_expand_builtin_selectfdp (target, exp);
    case KVX_BUILTIN_SELECTFDQ: return kvx_expand_builtin_selectfdq (target, exp);

    case KVX_BUILTIN_COPYSIGNW: return kvx_expand_builtin_copysignw (target, exp);
    case KVX_BUILTIN_COPYSIGNWP: return kvx_expand_builtin_copysignwp (target, exp);
    case KVX_BUILTIN_COPYSIGNWQ: return kvx_expand_builtin_copysignwq (target, exp);
    case KVX_BUILTIN_COPYSIGNWO: return kvx_expand_builtin_copysignwo (target, exp);
    case KVX_BUILTIN_COPYSIGND: return kvx_expand_builtin_copysignd (target, exp);
    case KVX_BUILTIN_COPYSIGNDP: return kvx_expand_builtin_copysigndp (target, exp);
    case KVX_BUILTIN_COPYSIGNDQ: return kvx_expand_builtin_copysigndq (target, exp);

    case KVX_BUILTIN_FMINW: return kvx_expand_builtin_fminw (target, exp);
    case KVX_BUILTIN_FMINWP: return kvx_expand_builtin_fminwp (target, exp);
    case KVX_BUILTIN_FMINWQ: return kvx_expand_builtin_fminwq (target, exp);
    case KVX_BUILTIN_FMINWO: return kvx_expand_builtin_fminwo (target, exp);
    case KVX_BUILTIN_FMIND: return kvx_expand_builtin_fmind (target, exp);
    case KVX_BUILTIN_FMINDP: return kvx_expand_builtin_fmindp (target, exp);
    case KVX_BUILTIN_FMINDQ: return kvx_expand_builtin_fmindq (target, exp);

    case KVX_BUILTIN_FMINWPS: return kvx_expand_builtin_fminwps (target, exp);
    case KVX_BUILTIN_FMINWQS: return kvx_expand_builtin_fminwqs (target, exp);
    case KVX_BUILTIN_FMINWOS: return kvx_expand_builtin_fminwos (target, exp);
    case KVX_BUILTIN_FMINDPS: return kvx_expand_builtin_fmindps (target, exp);
    case KVX_BUILTIN_FMINDQS: return kvx_expand_builtin_fmindqs (target, exp);

    case KVX_BUILTIN_FMAXW: return kvx_expand_builtin_fmaxw (target, exp);
    case KVX_BUILTIN_FMAXWP: return kvx_expand_builtin_fmaxwp (target, exp);
    case KVX_BUILTIN_FMAXWQ: return kvx_expand_builtin_fmaxwq (target, exp);
    case KVX_BUILTIN_FMAXWO: return kvx_expand_builtin_fmaxwo (target, exp);
    case KVX_BUILTIN_FMAXD: return kvx_expand_builtin_fmaxd (target, exp);
    case KVX_BUILTIN_FMAXDP: return kvx_expand_builtin_fmaxdp (target, exp);
    case KVX_BUILTIN_FMAXDQ: return kvx_expand_builtin_fmaxdq (target, exp);

    case KVX_BUILTIN_FMAXWPS: return kvx_expand_builtin_fmaxwps (target, exp);
    case KVX_BUILTIN_FMAXWQS: return kvx_expand_builtin_fmaxwqs (target, exp);
    case KVX_BUILTIN_FMAXWOS: return kvx_expand_builtin_fmaxwos (target, exp);
    case KVX_BUILTIN_FMAXDPS: return kvx_expand_builtin_fmaxdps (target, exp);
    case KVX_BUILTIN_FMAXDQS: return kvx_expand_builtin_fmaxdqs (target, exp);

    case KVX_BUILTIN_FNEGW: return kvx_expand_builtin_fnegw (target, exp);
    case KVX_BUILTIN_FNEGWP: return kvx_expand_builtin_fnegwp (target, exp);
    case KVX_BUILTIN_FNEGWQ: return kvx_expand_builtin_fnegwq (target, exp);
    case KVX_BUILTIN_FNEGWO: return kvx_expand_builtin_fnegwo (target, exp);
    case KVX_BUILTIN_FNEGD: return kvx_expand_builtin_fnegd (target, exp);
    case KVX_BUILTIN_FNEGDP: return kvx_expand_builtin_fnegdp (target, exp);
    case KVX_BUILTIN_FNEGDQ: return kvx_expand_builtin_fnegdq (target, exp);

    case KVX_BUILTIN_FABSW: return kvx_expand_builtin_fabsw (target, exp);
    case KVX_BUILTIN_FABSWP: return kvx_expand_builtin_fabswp (target, exp);
    case KVX_BUILTIN_FABSWQ: return kvx_expand_builtin_fabswq (target, exp);
    case KVX_BUILTIN_FABSWO: return kvx_expand_builtin_fabswo (target, exp);
    case KVX_BUILTIN_FABSD: return kvx_expand_builtin_fabsd (target, exp);
    case KVX_BUILTIN_FABSDP: return kvx_expand_builtin_fabsdp (target, exp);
    case KVX_BUILTIN_FABSDQ: return kvx_expand_builtin_fabsdq (target, exp);

    case KVX_BUILTIN_FRECW: return kvx_expand_builtin_frecw (target, exp);
    case KVX_BUILTIN_FRECWP: return kvx_expand_builtin_frecwp (target, exp);
    case KVX_BUILTIN_FRECWQ: return kvx_expand_builtin_frecwq (target, exp);
    case KVX_BUILTIN_FRECWO: return kvx_expand_builtin_frecwo (target, exp);

    case KVX_BUILTIN_FRSRW: return kvx_expand_builtin_frsrw (target, exp);
    case KVX_BUILTIN_FRSRWP: return kvx_expand_builtin_frsrwp (target, exp);
    case KVX_BUILTIN_FRSRWQ: return kvx_expand_builtin_frsrwq (target, exp);
    case KVX_BUILTIN_FRSRWO: return kvx_expand_builtin_frsrwo (target, exp);

    case KVX_BUILTIN_FADDW: return kvx_expand_builtin_faddw (target, exp);
    case KVX_BUILTIN_FADDWP: return kvx_expand_builtin_faddwp (target, exp);
    case KVX_BUILTIN_FADDWQ: return kvx_expand_builtin_faddwq (target, exp);
    case KVX_BUILTIN_FADDWO: return kvx_expand_builtin_faddwo (target, exp);
    case KVX_BUILTIN_FADDD: return kvx_expand_builtin_faddd (target, exp);
    case KVX_BUILTIN_FADDDP: return kvx_expand_builtin_fadddp (target, exp);
    case KVX_BUILTIN_FADDDQ: return kvx_expand_builtin_fadddq (target, exp);

    case KVX_BUILTIN_FADDCWC: return kvx_expand_builtin_faddcwc (target, exp);
    case KVX_BUILTIN_FADDCWCP: return kvx_expand_builtin_faddcwcp (target, exp);
    case KVX_BUILTIN_FADDCWCQ: return kvx_expand_builtin_faddcwcq (target, exp);
    case KVX_BUILTIN_FADDCDC: return kvx_expand_builtin_faddcdc (target, exp);
    case KVX_BUILTIN_FADDCDCP: return kvx_expand_builtin_faddcdcp (target, exp);

    case KVX_BUILTIN_FSBFW: return kvx_expand_builtin_fsbfw (target, exp);
    case KVX_BUILTIN_FSBFWP: return kvx_expand_builtin_fsbfwp (target, exp);
    case KVX_BUILTIN_FSBFWQ: return kvx_expand_builtin_fsbfwq (target, exp);
    case KVX_BUILTIN_FSBFWO: return kvx_expand_builtin_fsbfwo (target, exp);
    case KVX_BUILTIN_FSBFD: return kvx_expand_builtin_fsbfd (target, exp);
    case KVX_BUILTIN_FSBFDP: return kvx_expand_builtin_fsbfdp (target, exp);
    case KVX_BUILTIN_FSBFDQ: return kvx_expand_builtin_fsbfdq (target, exp);

    case KVX_BUILTIN_FSBFCWC: return kvx_expand_builtin_fsbfcwc (target, exp);
    case KVX_BUILTIN_FSBFCWCP: return kvx_expand_builtin_fsbfcwcp (target, exp);
    case KVX_BUILTIN_FSBFCWCQ: return kvx_expand_builtin_fsbfcwcq (target, exp);
    case KVX_BUILTIN_FSBFCDC: return kvx_expand_builtin_fsbfcdc (target, exp);
    case KVX_BUILTIN_FSBFCDCP: return kvx_expand_builtin_fsbfcdcp (target, exp);

    case KVX_BUILTIN_FMULW: return kvx_expand_builtin_fmulw (target, exp);
    case KVX_BUILTIN_FMULWP: return kvx_expand_builtin_fmulwp (target, exp);
    case KVX_BUILTIN_FMULWQ: return kvx_expand_builtin_fmulwq (target, exp);
    case KVX_BUILTIN_FMULWO: return kvx_expand_builtin_fmulwo (target, exp);
    case KVX_BUILTIN_FMULD: return kvx_expand_builtin_fmuld (target, exp);
    case KVX_BUILTIN_FMULDP: return kvx_expand_builtin_fmuldp (target, exp);
    case KVX_BUILTIN_FMULDQ: return kvx_expand_builtin_fmuldq (target, exp);

    case KVX_BUILTIN_FMULWD: return kvx_expand_builtin_fmulwd (target, exp);
    case KVX_BUILTIN_FMULWC: return kvx_expand_builtin_fmulwc (target, exp);
    case KVX_BUILTIN_FMULWCP: return kvx_expand_builtin_fmulwcp (target, exp);
    case KVX_BUILTIN_FMULWCQ: return kvx_expand_builtin_fmulwcq (target, exp);
    case KVX_BUILTIN_FMULCWC: return kvx_expand_builtin_fmulcwc (target, exp);
    case KVX_BUILTIN_FMULCWCP: return kvx_expand_builtin_fmulcwcp (target, exp);
    case KVX_BUILTIN_FMULCWCQ: return kvx_expand_builtin_fmulcwcq (target, exp);
    case KVX_BUILTIN_FMULDC: return kvx_expand_builtin_fmuldc (target, exp);
    case KVX_BUILTIN_FMULDCP: return kvx_expand_builtin_fmuldcp (target, exp);
    case KVX_BUILTIN_FMULCDC: return kvx_expand_builtin_fmulcdc (target, exp);
    case KVX_BUILTIN_FMULCDCP: return kvx_expand_builtin_fmulcdcp (target, exp);
    case KVX_BUILTIN_FMM212W: return kvx_expand_builtin_fmm212w (target, exp);

    case KVX_BUILTIN_FFMAW: return kvx_expand_builtin_ffmaw (target, exp);
    case KVX_BUILTIN_FFMAWP: return kvx_expand_builtin_ffmawp (target, exp);
    case KVX_BUILTIN_FFMAWQ: return kvx_expand_builtin_ffmawq (target, exp);
    case KVX_BUILTIN_FFMAWO: return kvx_expand_builtin_ffmawo (target, exp);
    case KVX_BUILTIN_FFMAD: return kvx_expand_builtin_ffmad (target, exp);
    case KVX_BUILTIN_FFMADP: return kvx_expand_builtin_ffmadp (target, exp);
    case KVX_BUILTIN_FFMADQ: return kvx_expand_builtin_ffmadq (target, exp);
    case KVX_BUILTIN_FFMAWD: return kvx_expand_builtin_ffmawd (target, exp);
    case KVX_BUILTIN_FMMA212W: return kvx_expand_builtin_fmma212w (target, exp);

    case KVX_BUILTIN_FFMSW: return kvx_expand_builtin_ffmsw (target, exp);
    case KVX_BUILTIN_FFMSWP: return kvx_expand_builtin_ffmswp (target, exp);
    case KVX_BUILTIN_FFMSWQ: return kvx_expand_builtin_ffmswq (target, exp);
    case KVX_BUILTIN_FFMSWO: return kvx_expand_builtin_ffmswo (target, exp);
    case KVX_BUILTIN_FFMSD: return kvx_expand_builtin_ffmsd (target, exp);
    case KVX_BUILTIN_FFMSDP: return kvx_expand_builtin_ffmsdp (target, exp);
    case KVX_BUILTIN_FFMSDQ: return kvx_expand_builtin_ffmsdq (target, exp);
    case KVX_BUILTIN_FFMSWD: return kvx_expand_builtin_ffmswd (target, exp);
    case KVX_BUILTIN_FMMS212W: return kvx_expand_builtin_fmms212w (target, exp);

    case KVX_BUILTIN_FLOATW: return kvx_expand_builtin_floatw (target, exp);
    case KVX_BUILTIN_FLOATWP: return kvx_expand_builtin_floatwp (target, exp);
    case KVX_BUILTIN_FLOATWQ: return kvx_expand_builtin_floatwq (target, exp);
    case KVX_BUILTIN_FLOATWO: return kvx_expand_builtin_floatwo (target, exp);
    case KVX_BUILTIN_FLOATD: return kvx_expand_builtin_floatd (target, exp);
    case KVX_BUILTIN_FLOATDP: return kvx_expand_builtin_floatdp (target, exp);
    case KVX_BUILTIN_FLOATDQ: return kvx_expand_builtin_floatdq (target, exp);

    case KVX_BUILTIN_FLOATUW: return kvx_expand_builtin_floatuw (target, exp);
    case KVX_BUILTIN_FLOATUWP: return kvx_expand_builtin_floatuwp (target, exp);
    case KVX_BUILTIN_FLOATUWQ: return kvx_expand_builtin_floatuwq (target, exp);
    case KVX_BUILTIN_FLOATUWO: return kvx_expand_builtin_floatuwo (target, exp);
    case KVX_BUILTIN_FLOATUD: return kvx_expand_builtin_floatud (target, exp);
    case KVX_BUILTIN_FLOATUDP: return kvx_expand_builtin_floatudp (target, exp);
    case KVX_BUILTIN_FLOATUDQ: return kvx_expand_builtin_floatudq (target, exp);

    case KVX_BUILTIN_FIXEDW: return kvx_expand_builtin_fixedw (target, exp);
    case KVX_BUILTIN_FIXEDWP: return kvx_expand_builtin_fixedwp (target, exp);
    case KVX_BUILTIN_FIXEDWQ: return kvx_expand_builtin_fixedwq (target, exp);
    case KVX_BUILTIN_FIXEDWO: return kvx_expand_builtin_fixedwo (target, exp);
    case KVX_BUILTIN_FIXEDD: return kvx_expand_builtin_fixedd (target, exp);
    case KVX_BUILTIN_FIXEDDP: return kvx_expand_builtin_fixeddp (target, exp);
    case KVX_BUILTIN_FIXEDDQ: return kvx_expand_builtin_fixeddq (target, exp);

    case KVX_BUILTIN_FIXEDUW: return kvx_expand_builtin_fixeduw (target, exp);
    case KVX_BUILTIN_FIXEDUWP: return kvx_expand_builtin_fixeduwp (target, exp);
    case KVX_BUILTIN_FIXEDUWQ: return kvx_expand_builtin_fixeduwq (target, exp);
    case KVX_BUILTIN_FIXEDUWO: return kvx_expand_builtin_fixeduwo (target, exp);
    case KVX_BUILTIN_FIXEDUD: return kvx_expand_builtin_fixedud (target, exp);
    case KVX_BUILTIN_FIXEDUDP: return kvx_expand_builtin_fixedudp (target, exp);
    case KVX_BUILTIN_FIXEDUDQ: return kvx_expand_builtin_fixedudq (target, exp);

    case KVX_BUILTIN_FCONJWC: return kvx_expand_builtin_fconjwc (target, exp);
    case KVX_BUILTIN_FCONJWCP: return kvx_expand_builtin_fconjwcp (target, exp);
    case KVX_BUILTIN_FCONJWCQ: return kvx_expand_builtin_fconjwcq (target, exp);
    case KVX_BUILTIN_FCONJDC: return kvx_expand_builtin_fconjdc (target, exp);
    case KVX_BUILTIN_FCONJDCP: return kvx_expand_builtin_fconjdcp (target, exp);

    case KVX_BUILTIN_FCDIVW: return kvx_expand_builtin_fcdivw (target, exp);
    case KVX_BUILTIN_FCDIVWP: return kvx_expand_builtin_fcdivwp (target, exp);
    case KVX_BUILTIN_FCDIVWQ: return kvx_expand_builtin_fcdivwq (target, exp);
    case KVX_BUILTIN_FCDIVWO: return kvx_expand_builtin_fcdivwo (target, exp);
    case KVX_BUILTIN_FCDIVD: return kvx_expand_builtin_fcdivd (target, exp);
    case KVX_BUILTIN_FCDIVDP: return kvx_expand_builtin_fcdivdp (target, exp);
    case KVX_BUILTIN_FCDIVDQ: return kvx_expand_builtin_fcdivdq (target, exp);

    case KVX_BUILTIN_FSDIVW: return kvx_expand_builtin_fsdivw (target, exp);
    case KVX_BUILTIN_FSDIVWP: return kvx_expand_builtin_fsdivwp (target, exp);
    case KVX_BUILTIN_FSDIVWQ: return kvx_expand_builtin_fsdivwq (target, exp);
    case KVX_BUILTIN_FSDIVWO: return kvx_expand_builtin_fsdivwo (target, exp);
    case KVX_BUILTIN_FSDIVD: return kvx_expand_builtin_fsdivd (target, exp);
    case KVX_BUILTIN_FSDIVDP: return kvx_expand_builtin_fsdivdp (target, exp);
    case KVX_BUILTIN_FSDIVDQ: return kvx_expand_builtin_fsdivdq (target, exp);

    case KVX_BUILTIN_FSRECW: return kvx_expand_builtin_fsrecw (target, exp);
    case KVX_BUILTIN_FSRECWP: return kvx_expand_builtin_fsrecwp (target, exp);
    case KVX_BUILTIN_FSRECWQ: return kvx_expand_builtin_fsrecwq (target, exp);
    case KVX_BUILTIN_FSRECWO: return kvx_expand_builtin_fsrecwo (target, exp);
    case KVX_BUILTIN_FSRECD: return kvx_expand_builtin_fsrecd (target, exp);
    case KVX_BUILTIN_FSRECDP: return kvx_expand_builtin_fsrecdp (target, exp);
    case KVX_BUILTIN_FSRECDQ: return kvx_expand_builtin_fsrecdq (target, exp);

    case KVX_BUILTIN_FSRSRW: return kvx_expand_builtin_fsrsrw (target, exp);
    case KVX_BUILTIN_FSRSRWP: return kvx_expand_builtin_fsrsrwp (target, exp);
    case KVX_BUILTIN_FSRSRWQ: return kvx_expand_builtin_fsrsrwq (target, exp);
    case KVX_BUILTIN_FSRSRWO: return kvx_expand_builtin_fsrsrwo (target, exp);
    case KVX_BUILTIN_FSRSRD: return kvx_expand_builtin_fsrsrd (target, exp);
    case KVX_BUILTIN_FSRSRDP: return kvx_expand_builtin_fsrsrdp (target, exp);
    case KVX_BUILTIN_FSRSRDQ: return kvx_expand_builtin_fsrsrdq (target, exp);

    case KVX_BUILTIN_GET: return kvx_expand_builtin_get (target, exp);
    case KVX_BUILTIN_WFXL: return kvx_expand_builtin_wfxl (target, exp);
    case KVX_BUILTIN_WFXM: return kvx_expand_builtin_wfxm (target, exp);
    case KVX_BUILTIN_IINVAL: return kvx_expand_builtin_iinval ();
    case KVX_BUILTIN_IINVALS: return kvx_expand_builtin_iinvals (target, exp);
    case KVX_BUILTIN_LBSU: return kvx_expand_builtin_lbsu (target, exp);
    case KVX_BUILTIN_LBZU: return kvx_expand_builtin_lbzu (target, exp);
    case KVX_BUILTIN_LDU: return kvx_expand_builtin_ldu (target, exp);
    case KVX_BUILTIN_LHSU: return kvx_expand_builtin_lhsu (target, exp);
    case KVX_BUILTIN_LHZU: return kvx_expand_builtin_lhzu (target, exp);
    case KVX_BUILTIN_LWZU: return kvx_expand_builtin_lwzu (target, exp);
    case KVX_BUILTIN_SET: return kvx_expand_builtin_set (target, exp);
    case KVX_BUILTIN_SLEEP: return kvx_expand_builtin_sleep (target, exp);
    case KVX_BUILTIN_STOP: return kvx_expand_builtin_stop (target, exp);
    case KVX_BUILTIN_SYNCGROUP: return kvx_expand_builtin_syncgroup (target, exp);
    case KVX_BUILTIN_TLBDINVAL: return kvx_expand_builtin_tlbdinval ();
    case KVX_BUILTIN_TLBIINVAL: return kvx_expand_builtin_tlbiinval ();
    case KVX_BUILTIN_TLBPROBE: return kvx_expand_builtin_tlbprobe ();
    case KVX_BUILTIN_TLBREAD: return kvx_expand_builtin_tlbread ();
    case KVX_BUILTIN_TLBWRITE: return kvx_expand_builtin_tlbwrite ();
    case KVX_BUILTIN_WAITIT: return kvx_expand_builtin_waitit (target, exp);

    case KVX_BUILTIN_SATD: return kvx_expand_builtin_satd (target, exp);
    case KVX_BUILTIN_SATUD: return kvx_expand_builtin_satud (target, exp);
    case KVX_BUILTIN_STSUW: return kvx_expand_builtin_stsuw (target, exp);
    case KVX_BUILTIN_STSUD: return kvx_expand_builtin_stsud (target, exp);
    case KVX_BUILTIN_SBMM8: return kvx_expand_builtin_sbmm8 (target, exp);
    case KVX_BUILTIN_SBMMT8: return kvx_expand_builtin_sbmmt8 (target, exp);
    case KVX_BUILTIN_FWIDENLHW: return kvx_expand_builtin_fwiden (target, exp, 1);
    case KVX_BUILTIN_FWIDENMHW: return kvx_expand_builtin_fwiden (target, exp, 0);
    case KVX_BUILTIN_FNARROWWH: return kvx_expand_builtin_fnarrowwh (target, exp);

    case KVX_BUILTIN_LBZ: return kvx_expand_builtin_lbz (target, exp);
    case KVX_BUILTIN_LBS: return kvx_expand_builtin_lbs (target, exp);
    case KVX_BUILTIN_LHZ: return kvx_expand_builtin_lhz (target, exp);
    case KVX_BUILTIN_LHS: return kvx_expand_builtin_lhs (target, exp);
    case KVX_BUILTIN_LWZ: return kvx_expand_builtin_lwz (target, exp);
    case KVX_BUILTIN_LWS: return kvx_expand_builtin_lws (target, exp);
    case KVX_BUILTIN_LD: return kvx_expand_builtin_ld (target, exp);
    case KVX_BUILTIN_LQ: return kvx_expand_builtin_lq (target, exp);
    case KVX_BUILTIN_LWF: return kvx_expand_builtin_lwf (target, exp);
    case KVX_BUILTIN_LDF: return kvx_expand_builtin_ldf (target, exp);

    case KVX_BUILTIN_LDBO: return kvx_expand_builtin_ldbo (target, exp);
    case KVX_BUILTIN_LDHQ: return kvx_expand_builtin_ldhq (target, exp);
    case KVX_BUILTIN_LDWP: return kvx_expand_builtin_ldwp (target, exp);
    case KVX_BUILTIN_LDFWP: return kvx_expand_builtin_ldfwp (target, exp);

    case KVX_BUILTIN_SDBO: return kvx_expand_builtin_sdbo (target, exp);
    case KVX_BUILTIN_SDHQ: return kvx_expand_builtin_sdhq (target, exp);
    case KVX_BUILTIN_SDWP: return kvx_expand_builtin_sdwp (target, exp);
    case KVX_BUILTIN_SDFWP: return kvx_expand_builtin_sdfwp (target, exp);

    case KVX_BUILTIN_LQBX: return kvx_expand_builtin_lqbx (target, exp);
    case KVX_BUILTIN_LQHO: return kvx_expand_builtin_lqho (target, exp);
    case KVX_BUILTIN_LQWQ: return kvx_expand_builtin_lqwq (target, exp);
    case KVX_BUILTIN_LQDP: return kvx_expand_builtin_lqdp (target, exp);
    case KVX_BUILTIN_LQFWQ: return kvx_expand_builtin_lqfwq (target, exp);
    case KVX_BUILTIN_LQFDP: return kvx_expand_builtin_lqfdp (target, exp);

    case KVX_BUILTIN_SQBX: return kvx_expand_builtin_sqbx (target, exp);
    case KVX_BUILTIN_SQHO: return kvx_expand_builtin_sqho (target, exp);
    case KVX_BUILTIN_SQWQ: return kvx_expand_builtin_sqwq (target, exp);
    case KVX_BUILTIN_SQDP: return kvx_expand_builtin_sqdp (target, exp);
    case KVX_BUILTIN_SQFWQ: return kvx_expand_builtin_sqfwq (target, exp);
    case KVX_BUILTIN_SQFDP: return kvx_expand_builtin_sqfdp (target, exp);

    case KVX_BUILTIN_LOBV: return kvx_expand_builtin_lobv (target, exp);
    case KVX_BUILTIN_LOHX: return kvx_expand_builtin_lohx (target, exp);
    case KVX_BUILTIN_LOWO: return kvx_expand_builtin_lowo (target, exp);
    case KVX_BUILTIN_LODQ: return kvx_expand_builtin_lodq (target, exp);
    case KVX_BUILTIN_LOFWO: return kvx_expand_builtin_lofwo (target, exp);
    case KVX_BUILTIN_LOFDQ: return kvx_expand_builtin_lofdq (target, exp);

    case KVX_BUILTIN_SOBV: return kvx_expand_builtin_sobv (target, exp);
    case KVX_BUILTIN_SOHX: return kvx_expand_builtin_sohx (target, exp);
    case KVX_BUILTIN_SOWO: return kvx_expand_builtin_sowo (target, exp);
    case KVX_BUILTIN_SODQ: return kvx_expand_builtin_sodq (target, exp);
    case KVX_BUILTIN_SOFWO: return kvx_expand_builtin_sofwo (target, exp);
    case KVX_BUILTIN_SOFDQ: return kvx_expand_builtin_sofdq (target, exp);

    case KVX_BUILTIN_LVBV: return kvx_expand_builtin_lvbv (target, exp);
    case KVX_BUILTIN_LVHX: return kvx_expand_builtin_lvhx (target, exp);
    case KVX_BUILTIN_LVWO: return kvx_expand_builtin_lvwo (target, exp);
    case KVX_BUILTIN_LVDQ: return kvx_expand_builtin_lvdq (target, exp);
    case KVX_BUILTIN_LVFWO: return kvx_expand_builtin_lvfwo (target, exp);
    case KVX_BUILTIN_LVFDQ: return kvx_expand_builtin_lvfdq (target, exp);

    case KVX_BUILTIN_SVBV: return kvx_expand_builtin_svbv (target, exp);
    case KVX_BUILTIN_SVHX: return kvx_expand_builtin_svhx (target, exp);
    case KVX_BUILTIN_SVWO: return kvx_expand_builtin_svwo (target, exp);
    case KVX_BUILTIN_SVDQ: return kvx_expand_builtin_svdq (target, exp);
    case KVX_BUILTIN_SVFWO: return kvx_expand_builtin_svfwo (target, exp);
    case KVX_BUILTIN_SVFDQ: return kvx_expand_builtin_svfdq (target, exp);

    case KVX_BUILTIN_MOVEFOBV: return kvx_expand_builtin_movefobv (target, exp);
    case KVX_BUILTIN_MOVEFOHX: return kvx_expand_builtin_movefohx (target, exp);
    case KVX_BUILTIN_MOVEFOWO: return kvx_expand_builtin_movefowo (target, exp);
    case KVX_BUILTIN_MOVEFODQ: return kvx_expand_builtin_movefodq (target, exp);
    case KVX_BUILTIN_MOVEFOFWO: return kvx_expand_builtin_movefofwo (target, exp);
    case KVX_BUILTIN_MOVEFOFDQ: return kvx_expand_builtin_movefofdq (target, exp);

    case KVX_BUILTIN_MOVETOBV: return kvx_expand_builtin_movetobv (target, exp);
    case KVX_BUILTIN_MOVETOHX: return kvx_expand_builtin_movetohx (target, exp);
    case KVX_BUILTIN_MOVETOWO: return kvx_expand_builtin_movetowo (target, exp);
    case KVX_BUILTIN_MOVETODQ: return kvx_expand_builtin_movetodq (target, exp);
    case KVX_BUILTIN_MOVETOFWO: return kvx_expand_builtin_movetofwo (target, exp);
    case KVX_BUILTIN_MOVETOFDQ: return kvx_expand_builtin_movetofdq (target, exp);

    case KVX_BUILTIN_SWAPVOBV: return kvx_expand_builtin_swapvobv (target, exp);
    case KVX_BUILTIN_SWAPVOHX: return kvx_expand_builtin_swapvohx (target, exp);
    case KVX_BUILTIN_SWAPVOWO: return kvx_expand_builtin_swapvowo (target, exp);
    case KVX_BUILTIN_SWAPVODQ: return kvx_expand_builtin_swapvodq (target, exp);
    case KVX_BUILTIN_SWAPVOFWO: return kvx_expand_builtin_swapvofwo (target, exp);
    case KVX_BUILTIN_SWAPVOFDQ: return kvx_expand_builtin_swapvofdq (target, exp);

    case KVX_BUILTIN_ALIGNOBV: return kvx_expand_builtin_alignobv (target, exp);
    case KVX_BUILTIN_ALIGNOHX: return kvx_expand_builtin_alignohx (target, exp);
    case KVX_BUILTIN_ALIGNOWO: return kvx_expand_builtin_alignowo (target, exp);
    case KVX_BUILTIN_ALIGNODQ: return kvx_expand_builtin_alignodq (target, exp);
    case KVX_BUILTIN_ALIGNOFWO: return kvx_expand_builtin_alignofwo (target, exp);
    case KVX_BUILTIN_ALIGNOFDQ: return kvx_expand_builtin_alignofdq (target, exp);

    case KVX_BUILTIN_ALIGNV: return kvx_expand_builtin_alignv (target, exp);
    case KVX_BUILTIN_COPYV: return kvx_expand_builtin_copyv (target, exp);
    case KVX_BUILTIN_MT4X4D: return kvx_expand_builtin_mt4x4d (target, exp);
    case KVX_BUILTIN_MM4ABW: return kvx_expand_builtin_mm4abw (target, exp);
    case KVX_BUILTIN_FMM4AHW0: return kvx_expand_builtin_fmm4ahw0 (target, exp);
    case KVX_BUILTIN_FMM4AHW1: return kvx_expand_builtin_fmm4ahw1 (target, exp);
    case KVX_BUILTIN_FMM4AHW2: return kvx_expand_builtin_fmm4ahw2 (target, exp);
    case KVX_BUILTIN_FMM4AHW3: return kvx_expand_builtin_fmm4ahw3 (target, exp);

    default:
      break;
    }
  internal_error ("bad builtin code");
  return NULL_RTX;
}

int
kv3_mau_lsu_double_port_bypass_p (rtx_insn *producer, rtx_insn *consumer)
{
  rtx produced = SET_DEST (single_set (producer));
  rtx consumed = PATTERN (consumer);

  if (GET_CODE (consumed) == PARALLEL)
    consumed = XVECEXP (consumed, 0, 0);
  consumed = SET_DEST (consumed);

  return reg_overlap_mentioned_p (produced, consumed);
}

static int
kvx_sched_adjust_cost (rtx_insn *insn, int dep_type,
		       rtx_insn *dep_insn ATTRIBUTE_UNUSED, int cost,
		       unsigned int)
{
  enum attr_class insn_class = get_attr_class (insn);
  /* On the kvx, it is possible to read then write the same register in a
   * bundle so we set the WAR cost to 0 unless insn is a control-flow consuming
   * reg then it is 1 */
  if (dep_type == REG_DEP_ANTI)
    {
      cost = (insn_class == CLASS_BRANCH) | (insn_class == CLASS_JUMP)
	     | (insn_class == CLASS_LINK);
    }
  else
    /* Just to be sure, force the WAW cost to 1 */
    if (dep_type == REG_DEP_OUTPUT)
    {
      cost = 1;
    }

  return cost;
}

static int
kvx_sched_issue_rate (void)
{
  return 5;
}

static int
kvx_sched_dfa_new_cycle (FILE *dump ATTRIBUTE_UNUSED,
			 int verbose ATTRIBUTE_UNUSED, rtx_insn *insn,
			 int last_clock, int clock,
			 int *sort_p ATTRIBUTE_UNUSED)
{
  if (clock != last_clock)
    return 0;

  if (reg_mentioned_p (kvx_sync_reg_rtx, insn))
    return 1;

  return 0;
}

/* Test if X is of the form reg[reg] or .xs reg = reg[reg] or signed10bits[reg]
 */
bool
kvx_has_10bit_imm_or_register_p (rtx x)
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

  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 1)) == CONST_INT)
    return IN_RANGE (INTVAL (XEXP (x, 1)), -512, 511);

  return false;
}

bool
kvx_has_37bit_immediate_p (rtx x)
{
  if (MEM_P (x))
    x = XEXP (x, 0);

  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 1)) == CONST_INT)
    return !IN_RANGE (INTVAL (XEXP (x, 1)), -512, 511)
	   && IN_RANGE (INTVAL (XEXP (x, 1)), -(1LL << 36), (1LL << 36) - 1);

  return false;
}

bool
kvx_has_64bit_immediate_p (rtx x)
{
  if (MEM_P (x))
    x = XEXP (x, 0);

  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 1)) == CONST_INT)
    return !IN_RANGE (INTVAL (XEXP (x, 1)), -(1LL << 36), (1LL << 36) - 1);

  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 1)) != CONST_INT)
    return true;

  return false;
}

/* Test whether the memory operand X should be accessed cached or
   uncached (bypass or preload) regarding it's name address space.
   If non-zero, the return value is the MEM_ADDR_SPACE. */
int
kvx_is_uncached_mem_op_p (rtx x)
{
  gcc_assert (MEM_P (x));
  if (!MEM_P (x))
    return false;

  /* __convert[_no_sync] addr space should not come here. */
  gcc_assert (MEM_ADDR_SPACE (x) < KVX_ADDR_SPACE_CONVERT);

  return MEM_ADDR_SPACE (x);
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
      else if (inner_mode == SFmode)
	{
	  long val[2] = {0, 0};
	  rtx elt_0 = CONST_VECTOR_ELT (x, index + 0);
	  rtx elt_1 = CONST_VECTOR_ELT (x, index + 1);
	  REAL_VALUE_TO_TARGET_SINGLE (*CONST_DOUBLE_REAL_VALUE (elt_0),
				       val[0]);
	  REAL_VALUE_TO_TARGET_SINGLE (*CONST_DOUBLE_REAL_VALUE (elt_1),
				       val[1]);
	  value = ((HOST_WIDE_INT) val[0] & 0xFFFFFFFF)
		  | ((HOST_WIDE_INT) val[1] & 0xFFFFFFFF) << 32;
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
kvx_has_32x2bit_vector_const_p (rtx x)
{
  HOST_WIDE_INT value = kvx_const_vector_value (x, 0);
  // Need the dual immediate syntax to be fixed in assembler.
  // return (value&0xFFFFFFFF) == ((value>>32)&0xFFFFFFFF);
  return false;
}

/* Helper function for kvx_expand_load_multiple and
   kvx_expand_store_multiple */
static bool
kvx_expand_load_store_multiple (rtx operands[], bool is_load)
{
  int regno;
  int count;
  int i;

  const int reg_op_idx = is_load ? 0 : 1;
  const int mem_op_idx = is_load ? 1 : 0;

  count = INTVAL (operands[2]);
  regno = REGNO (operands[reg_op_idx]);

  if (GET_CODE (operands[2]) != CONST_INT
      || GET_MODE (operands[reg_op_idx]) != DImode || (count != 2 && count != 4)
      || ((count == 2) && (regno & 1)) || ((count == 4) && (regno & 3))
      || !MEM_P (operands[mem_op_idx]) || !REG_P (operands[reg_op_idx])
      || (TARGET_STRICT_ALIGN
	  && MEM_ALIGN (operands[mem_op_idx]) < (count * UNITS_PER_WORD))
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

  for (i = 0; i < count; i++)
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
 * operation and all mems are cached/uncached depending on
 * IS_UNCACHED.
 * When IS_LOAD is FALSE, returns TRUE if OP is a store multiple
 * operation.
 * Returns FALSE otherwise.
 */
static bool
kvx_load_store_multiple_operation_p (rtx op, bool is_uncached, bool is_load)
{
  int count = XVECLEN (op, 0);
  unsigned int dest_regno;
  int i;

  /* Perform a quick check so we don't blow up below.  */
  if (count != 2 && count != 4)
    return 0;

  for (i = 0; i < count; i++)
    {
      rtx set = XVECEXP (op, 0, i);
      if (GET_CODE (set) != SET)
	return false;

      rtx reg_part = is_load ? SET_DEST (set) : SET_SRC (set);
      rtx mem_part = is_load ? SET_SRC (set) : SET_DEST (set);

      if (!REG_P (reg_part) || !MEM_P (mem_part) || MEM_VOLATILE_P (mem_part))
	return false;

      if (is_load && is_uncached != !!kvx_is_uncached_mem_op_p (mem_part))
	return false;
    }

  rtx first_mem
    = is_load ? SET_SRC (XVECEXP (op, 0, 0)) : SET_DEST (XVECEXP (op, 0, 0));
  rtx first_reg
    = is_load ? SET_DEST (XVECEXP (op, 0, 0)) : SET_SRC (XVECEXP (op, 0, 0));

  if (TARGET_STRICT_ALIGN && MEM_ALIGN (first_mem) < (count * UNITS_PER_WORD))
    return false;

  dest_regno = REGNO (first_reg);

  /* register number must be correctly aligned */
  if (dest_regno < FIRST_PSEUDO_REGISTER && (dest_regno % count != 0))
    return false;

  HOST_WIDE_INT expected_offset = 0;
  rtx base;

  for (i = 0; i < count; i++)
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
 * Returns TRUE if OP is a load multiple operation and all mems are
 * cached/uncached depending on IS_UNCACHED.
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

/* Following funtions are used for bundling insn before ASM emission */

struct bundle_state
{
  /* Unique bundle state number to identify them in the debugging
     output */
  int unique_num;

  /* First insn in the bundle */
  rtx_insn *insn;

  /* Last insn in the bundle */
  rtx_insn *last_insn;

  /* Number of insn in the bundle */
  int insns_num;

  /* Registers being defined within the bundle */
  HARD_REG_SET reg_defs;

  /* All bundle states are in the following chain.  */
  struct bundle_state *allocated_states_chain;

  struct bundle_state *next;
  state_t dfa_state;
};

/* Bundles list for current function */
static struct bundle_state *cur_bundle_list;

/* The unique number of next bundle state.  */
static int bundle_states_num;

/* All allocated bundle states are in the following chain.  */
static struct bundle_state *allocated_bundle_states_chain;

/* All allocated but not used bundle states are in the following
   chain.  */
static struct bundle_state *free_bundle_state_chain;

struct bundle_regs
{
  int set_dest;
  rtx scanned_insn;
  HARD_REG_SET uses;
  HARD_REG_SET defs;
};

static void kvx_scan_insn_registers_wrap (rtx *x, void *data);

static int
kvx_scan_insn_registers_1 (rtx *x, void *data)
{
  struct bundle_regs *regs = (struct bundle_regs *) data;

  if (!*x)
    return 0;

  if (GET_CODE (*x) == USE && CALL_P (regs->scanned_insn))
    return -1;

  if (GET_CODE (*x) == SET)
    {
      regs->set_dest = 1;
      /* for_each_rtx (&SET_DEST (*x), kvx_scan_insn_registers_1, regs); */
      kvx_scan_insn_registers_wrap (&SET_DEST (*x), regs);
      regs->set_dest = 0;
      /* for_each_rtx (&SET_SRC (*x), kvx_scan_insn_registers_1, regs); */
      kvx_scan_insn_registers_wrap (&SET_SRC (*x), regs);
      return -1;
    }

  if (MEM_P (*x))
    {
      regs->set_dest = 0;
      /* for_each_rtx (&XEXP (*x, 0), kvx_scan_insn_registers_1, regs); */
      kvx_scan_insn_registers_wrap (&XEXP (*x, 0), regs);
      return -1;
    }

  if (GET_CODE (*x) == CLOBBER)
    {
      if (REG_P (XEXP (*x, 0)))
	SET_HARD_REG_BIT (regs->defs, REGNO (XEXP (*x, 0)));

      /* double/quadruple/octuple register */
      /* Also mark the implicitely defined registers */
      if (GET_MODE_SIZE (GET_MODE (*x)) > UNITS_PER_WORD)
	{
	  unsigned word;
	  for (word = 1; word < GET_MODE_SIZE (GET_MODE (*x)) / UNITS_PER_WORD;
	       word++)
	    {
	      SET_HARD_REG_BIT (regs->defs, REGNO (XEXP (*x, 0)) + word);
	    }
	}
      return -1;
    }

  if (GET_CODE (*x) == EXPR_LIST
      && (enum reg_note) GET_MODE (*x) != REG_DEP_TRUE)
    {
      return -1;
    }

  if (REG_P (*x))
    {
      /* Must be a real hard registers */
      gcc_assert (REGNO (*x) <= KV3_MDS_REGISTERS);

      if (regs->set_dest)
	SET_HARD_REG_BIT (regs->defs, REGNO (*x));
      else
	SET_HARD_REG_BIT (regs->uses, REGNO (*x));

      if (GET_MODE_SIZE (GET_MODE (*x)) > UNITS_PER_WORD)
	{
	  unsigned word;
	  for (word = 1; word < GET_MODE_SIZE (GET_MODE (*x)) / UNITS_PER_WORD;
	       word++)
	    {
	      SET_HARD_REG_BIT (regs->set_dest ? regs->defs : regs->uses,
				REGNO (*x) + word);
	    }
	}
    }

  return 0;
}

static void
kvx_scan_insn_registers_wrap (rtx *x, void *data)
{
  subrtx_ptr_iterator::array_type array;
  FOR_EACH_SUBRTX_PTR (iter, array, x, ALL)
    {
      rtx *x = *iter;
      if (kvx_scan_insn_registers_1 (x, data) == -1)
	{
	  iter.skip_subrtxes ();
	}
    }
}

static void
kvx_scan_insn_registers (rtx insn, struct bundle_regs *regs)
{
  if (GET_CODE (insn) == CLOBBER || GET_CODE (insn) == USE || !INSN_P (insn))
    return;

  regs->set_dest = 0;
  regs->scanned_insn = insn;
  CLEAR_HARD_REG_SET (regs->uses);
  CLEAR_HARD_REG_SET (regs->defs);
  /* for_each_rtx (&insn, kvx_scan_insn_registers_1, regs); */
  kvx_scan_insn_registers_wrap (&insn, regs);
}

/* Skip over irrelevant NOTEs and such and look for the next insn we
   would consider bundling.  */
static rtx_insn *
next_insn_to_bundle (rtx_insn *r, rtx_insn *end)
{
  for (; r != end; r = NEXT_INSN (r))
    {
      if (NONDEBUG_INSN_P (r) && GET_CODE (PATTERN (r)) != USE
	  && GET_CODE (PATTERN (r)) != CLOBBER)
	return r;
    }

  return NULL;
}
/* Finish work with abstract data `bundle states'.  */

static void
finish_bundle_states (void)
{
  struct bundle_state *curr_state, *next_state;

  for (curr_state = allocated_bundle_states_chain; curr_state != NULL;
       curr_state = next_state)
    {
      next_state = curr_state->allocated_states_chain;
      free (curr_state->dfa_state);
      free (curr_state);
    }
}

/* Start work with abstract data `bundle states'.  */

static void
initiate_bundle_states (void)
{
  if (cur_bundle_list != NULL)
    finish_bundle_states ();

  bundle_states_num = 0;
  cur_bundle_list = NULL;
  free_bundle_state_chain = NULL;
  allocated_bundle_states_chain = NULL;
}

static struct bundle_state *
get_free_bundle_state (void)
{
  struct bundle_state *result;

  if (free_bundle_state_chain != NULL)
    {
      result = free_bundle_state_chain;
      free_bundle_state_chain = result->next;
    }
  else
    {
      result = XNEW (struct bundle_state);
      result->allocated_states_chain = allocated_bundle_states_chain;
      allocated_bundle_states_chain = result;
      result->dfa_state = xmalloc (dfa_state_size);
    }

  result->next = NULL;
  result->insn = NULL;
  result->last_insn = NULL;
  result->insns_num = 0;

  CLEAR_HARD_REG_SET (result->reg_defs);
  state_reset (result->dfa_state);

  result->unique_num = bundle_states_num;
  bundle_states_num++;

  return result;
}

/* The following function frees given bundle state.  */
static void free_bundle_state (struct bundle_state *state) ATTRIBUTE_UNUSED;

static void
free_bundle_state (struct bundle_state *state)
{
  state->next = free_bundle_state_chain;
  free_bundle_state_chain = state;
}

static int
kvx_insn_is_bundle_end_p (rtx insn)
{
  bundle_state *i;
  for (i = cur_bundle_list; i; i = i->next)
    {
      gcc_assert (i->insn != NULL_RTX);
      gcc_assert (i->last_insn != NULL_RTX);
      if (i->last_insn == insn)
	return 1;
    }
  return 0;
}

static void kvx_dump_bundles (void);

static void
kvx_gen_bundles (void)
{
  bundle_state *cur_bstate = NULL;

  dfa_start ();

  basic_block bb;
  FOR_EACH_BB_FN (bb, cfun)
    {
      rtx_insn *next, *prev;
      rtx_insn *end = NEXT_INSN (BB_END (bb));

      if (next_insn_to_bundle (BB_HEAD (bb), end) == NULL)
	continue;

      /* BB has no insn to bundle */
      if (next_insn_to_bundle (BB_HEAD (bb), end) == NULL_RTX)
	continue;

      if (cur_bstate == NULL)
	{
	  /* First bundle for function */
	  cur_bstate = get_free_bundle_state ();
	  cur_bundle_list = cur_bstate;
	}
      else if (cur_bstate->insns_num != 0) /* can reuse preallocated
					      bundle if previous BB
					      ended-up empty of
					      insns */
	{
	  cur_bstate->next = get_free_bundle_state ();
	  cur_bstate = cur_bstate->next;
	}

      prev = NULL;
      for (rtx_insn *insn = next_insn_to_bundle (BB_HEAD (bb), end); insn;
	   prev = insn, insn = next)
	{
	  next = next_insn_to_bundle (NEXT_INSN (insn), end);

	  struct bundle_regs cur_insn_regs = {0};

	  /* First, check if the insn fits in the bundle:
	   * - bundle size
	   * - exu resources
	   */
	  int can_issue = state_transition (cur_bstate->dfa_state, insn) < 0;

	  /* Scan insn for registers definitions and usage. */
	  kvx_scan_insn_registers (insn, &cur_insn_regs);

	  const int insn_raw = hard_reg_set_intersect_p (cur_insn_regs.uses,
							 cur_bstate->reg_defs);
	  const int insn_waw = hard_reg_set_intersect_p (cur_insn_regs.defs,
							 cur_bstate->reg_defs);
	  const int insn_jump = JUMP_P (insn) || CALL_P (insn);
	  const int next_is_label = (next != NULL) && LABEL_P (next);

	  /* Current insn can't be bundled with other insn, create a new one. */
	  if (!can_issue || insn_raw || insn_waw)
	    {
	      gcc_assert (cur_bstate->insn != NULL);

	      gcc_assert (prev != NULL);

	      cur_bstate->next = get_free_bundle_state ();
	      cur_bstate = cur_bstate->next;

	      state_transition (cur_bstate->dfa_state, insn);
	    }

	  if (cur_bstate->insn == NULL)
	    {
	      /* First insn in bundle */
	      cur_bstate->insn = insn;
	      cur_bstate->insns_num = 1;
	    }
	  else
	    {
	      cur_bstate->insns_num++;
	    }
	  cur_bstate->last_insn = insn;

	  IOR_HARD_REG_SET (cur_bstate->reg_defs, cur_insn_regs.defs);

	  /* Current insn is a jump, don't bundle following insns. */
	  /* If there is a label in the middle of a possible bundle,
	     split it */
	  if ((insn_jump && next != NULL) || next_is_label)
	    {
	      gcc_assert (cur_bstate->insn != NULL);
	      cur_bstate->next = get_free_bundle_state ();
	      cur_bstate = cur_bstate->next;
	    }
	}
    }

  dfa_finish ();
}

static void kvx_dump_bundles (void) ATTRIBUTE_UNUSED;

static void
kvx_dump_bundles (void)
{
  bundle_state *i;
  for (i = cur_bundle_list; i; i = i->next)
    {

      rtx_insn *binsn = i->insn;
      fprintf (stderr, "BUNDLE START %d\n", i->unique_num);

      if (i->insn == NULL_RTX)
	fprintf (stderr, " invalid bundle %d: insn is NULL\n", i->unique_num);

      if (i->last_insn == NULL_RTX)
	fprintf (stderr, " invalid bundle %d: last_insn is NULL\n",
		 i->unique_num);

      int bundle_stop = 0;
      do
	{
	  bundle_stop = (binsn == i->last_insn);
	  debug (binsn);
	  binsn = NEXT_INSN (binsn);
	}
      while (!bundle_stop);
      fprintf (stderr, "BUNDLE STOP %d\n", i->unique_num);
    }
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
      HOST_WIDE_INT new_offset = (new_base_reg == hard_frame_pointer_rtx)
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

/* Visit all bundles and force all debug insns after the last insn in
   the bundle. */
static void
kvx_fix_debug_for_bundles (void)
{
  bundle_state *i;
  unsigned int cur_cfa_reg = REGNO (stack_pointer_rtx);

  for (i = cur_bundle_list; i; i = i->next)
    {
      rtx_insn *binsn = i->last_insn;

      gcc_assert (i->insn != NULL);
      gcc_assert (i->last_insn != NULL);

      bool bundle_start;
      bool bundle_end;
      /* Start from the end so that NOTEs will be added in the correct order. */
      do
	{
	  bundle_start = (binsn == i->insn);
	  bundle_end = (binsn == i->last_insn);

	  if (INSN_P (binsn) && RTX_FRAME_RELATED_P (binsn))
	    {
	      rtx note;
	      bool handled = false;
	      for (note = REG_NOTES (binsn); note; note = XEXP (note, 1))
		{
		  switch (REG_NOTE_KIND (note))
		    {
		    case REG_CFA_DEF_CFA:
		    case REG_CFA_ADJUST_CFA:
		    case REG_CFA_REGISTER:
		    case REG_CFA_RESTORE:
		    case REG_CFA_OFFSET:
		      handled = true;
		      if (!bundle_end)
			{
			  /* Move note to last insn in bundle */
			  add_shallow_copy_of_reg_note (i->last_insn, note);
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
		  add_reg_note (i->last_insn, REG_CFA_OFFSET,
				copy_rtx (PATTERN (binsn)));
		}

	      RTX_FRAME_RELATED_P (binsn) = 0;
	      RTX_FRAME_RELATED_P (i->last_insn) = 1;
	    }

	  binsn = PREV_INSN (binsn);
	}
      while (!bundle_start);

      /* Iterate from start and fix possible MEM using the incorrect base
       * register */
      /* This only works if FRAME_RELATED_P insns are in sequence. If
	 this is not the case, relying on NEXT_INSN is *incorrect* */
      binsn = i->insn;
      do
	{
	  bundle_start = (binsn == i->insn);
	  bundle_end = (binsn == i->last_insn);

	  if (INSN_P (binsn) && RTX_FRAME_RELATED_P (binsn))
	    {
	      rtx note;

	      for (note = REG_NOTES (binsn); note; note = XEXP (note, 1))
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
      while (!bundle_end);
    }
}

static void
kvx_asm_final_postscan_insn (FILE *file, rtx_insn *insn,
			     rtx *opvec ATTRIBUTE_UNUSED,
			     int noperands ATTRIBUTE_UNUSED)
{
  if (!scheduling || !TARGET_BUNDLING || kvx_insn_is_bundle_end_p (insn))
    {
      fprintf (file, "\t;;\n");
      return;
    }
}

static int
kvx_sched_first_cycle_multipass_dfa_lookahead (void)
{
  return 5;
}

struct cost_walker
{
  rtx *toplevel;
  int total;
};

/* FIXME AUTO: fix cost function for coolidge */
/* See T7748 */
static int
kvx_rtx_operand_cost (rtx *x, void *arg)
{
  struct cost_walker *cost = (struct cost_walker *) arg;
  enum rtx_code code;

  /* print_rtl_single(stdout, *x); */

  code = GET_CODE (*x);

  if (x == cost->toplevel)
    {
      switch (code)
	{
	case MULT:
	  cost->total += COSTS_N_INSNS (2);
	  break;
	case DIV:
	case MOD:
	  if (CONST_INT_P (XEXP (*x, 1))
	      && __builtin_popcount (INTVAL (XEXP (*x, 1))) == 1
	      && INTVAL (XEXP (*x, 1)) > 0)
	    {
	      cost->total += COSTS_N_INSNS (1);
	      break;
	    }
	  /* Fall through */
	case UDIV:
	case UMOD:
	  cost->total += COSTS_N_INSNS (15);
	  break;
	case USE:
	  /* Used in combine.c as a marker.  */
	  break;
	case CONST_INT:
	  if (INTVAL (*x) >= -32768 && INTVAL (*x) <= 32767)
	    {
	      cost->total += COSTS_N_INSNS (1);
	      return 0;
	    }
	default:
	  cost->total += COSTS_N_INSNS (1);
	}
    }

  switch (code)
    {
    case CONST_INT:
      if (INTVAL (*x) >= -512 && INTVAL (*x) <= 511)
	break;
      /* Fall through */
    case CONST:
    case LABEL_REF:
    case SYMBOL_REF:
      cost->total += COSTS_N_INSNS (1);
      break;
    case CONST_DOUBLE:
      cost->total += COSTS_N_INSNS (2);
      break;
    default:
      break;
    }

  return 0;
}

/* FIXME AUTO: fix cost function for coolidge */
/* See T7748 */
static bool
kvx_rtx_costs (rtx x, machine_mode mode ATTRIBUTE_UNUSED,
	       int outer_code ATTRIBUTE_UNUSED, int opno ATTRIBUTE_UNUSED,
	       int *total, bool speed ATTRIBUTE_UNUSED)
{
  struct cost_walker cost = {0, 0};

  if (GET_CODE (x) == INSN)
    x = PATTERN (x);

  if (GET_CODE (x) == SET)
    {
      if (MEM_P (SET_SRC (x)))
	{
	  *total = 14; /* Memory load: let's count a bit more than 3
			  simple INSNs. */
	  return true;
	}
      else if (MEM_P (SET_DEST (x)))
	{
	  *total = 6; /* Memory store */
	  return true;
	}
      else if (REG_P (SET_SRC (x)))
	{
	  *total = kvx_register_move_cost (GET_MODE (SET_DEST (x)), NO_REGS,
					   NO_REGS);
	  return true;
	}
      x = SET_SRC (x);
    }

  cost.toplevel = &x;
  /* We don't select instruction depending on 'execution cost' as
     nearly everything has the same costs. Instead try to use a cost
     function that reflects the size of the generated
     instructions. That should help for performance also as the code
     footprint gets lower. */

  subrtx_ptr_iterator::array_type array;
  FOR_EACH_SUBRTX_PTR (iter, array, cost.toplevel, ALL)
    {
      rtx *x = *iter;
      kvx_rtx_operand_cost (x, &cost);
    }

  *total = cost.total;
  return true;
}

static int
kvx_address_cost (rtx x, machine_mode mode ATTRIBUTE_UNUSED,
		  addr_space_t space ATTRIBUTE_UNUSED,
		  bool speed ATTRIBUTE_UNUSED)
{
  int cost = COSTS_N_INSNS (1);
  gcc_assert (cost > 1);

  // Lower cost in case of .xs addressing mode
  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 0)) == MULT
      && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT
      && INTVAL (XEXP (XEXP (x, 0), 1)) > HOST_WIDE_INT_1)
    return cost - 1;

  return cost;
}

static void
kvx_sched_set_sched_flags (struct spec_info_def *spec ATTRIBUTE_UNUSED)
{
  /* TARGET_SCHED_SET_SCHED_FLAGS is called right before the
     scheduler calls df_analyze (). Use this hook to add the
     problems we're interested in for the mem access packing.  */
  if (!reload_completed && common_sched_info->sched_pass_id != SCHED_SMS_PASS)
    {
      df_chain_add_problem (DF_DU_CHAIN | DF_UD_CHAIN);
    }
}

static void
kvx_sched_init_global (FILE *file ATTRIBUTE_UNUSED,
		       int verbose ATTRIBUTE_UNUSED,
		       int max_ready ATTRIBUTE_UNUSED)
{
  scheduling = true;
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

/* Implements TARGET_ADDR_SPACE_POINTER_MODE */
static machine_mode
kvx_addr_space_pointer_mode (addr_space_t address_space ATTRIBUTE_UNUSED)
{
  return ptr_mode;
}

/* Implements TARGET_ADDR_SPACE_ADDRESS_MODE */
static machine_mode
kvx_addr_space_address_mode (addr_space_t address_space ATTRIBUTE_UNUSED)
{
  return Pmode;
}

/* Implements TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P */
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
      return kvx_legitimate_address_p (mode, exp, strict);

    case KVX_ADDR_SPACE_CONVERT:
      return false;
    }
}

/* Implements TARGET_ADDR_SPACE_LEGITIMIZE_ADDRESS */
static rtx
kvx_addr_space_legitimize_address (rtx x, rtx oldx, machine_mode mode,
				   addr_space_t as)
{
  if (as == KVX_ADDR_SPACE_CONVERT)
    error ("__convert should be used only in explicit pointer casting");

  return kvx_legitimize_address (x, oldx, mode);
}

/* Implements TARGET_ADDR_SPACE_SUBSET_P */
static bool
kvx_addr_space_subset_p (addr_space_t subset ATTRIBUTE_UNUSED,
			 addr_space_t superset ATTRIBUTE_UNUSED)
{
  // Address spaces (GENERIC or __UNCACHED) refer to the same space
  return true;
}

/* Implements TARGET_ADDR_SPACE_CONVERT */
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
kvx_function_prologue (FILE *file ATTRIBUTE_UNUSED,
		       HOST_WIDE_INT size ATTRIBUTE_UNUSED)
{
  dfa_start ();
}

static void
kvx_function_epilogue (FILE *file ATTRIBUTE_UNUSED,
		       HOST_WIDE_INT size ATTRIBUTE_UNUSED)
{
  dfa_finish ();
}

static void
kvx_dependencies_evaluation_hook (rtx_insn *head, rtx_insn *tail)
{
  rtx_insn *insn, *insn2, *next_tail, *last_sync = head;

  next_tail = NEXT_INSN (tail);

  for (insn = head; insn != next_tail; insn = NEXT_INSN (insn))
    if (INSN_P (insn))
      {
	int sync = reg_mentioned_p (kvx_sync_reg_rtx, insn);

	if (sync)
	  {
	    for (insn2 = last_sync; insn2 != insn; insn2 = NEXT_INSN (insn2))
	      if (INSN_P (insn2))
		{
		  add_dependence (insn, insn2, REG_DEP_TRUE);
		}
	    last_sync = insn;
	  }
      }

  if (last_sync != head)
    for (insn2 = last_sync; insn2 != next_tail; insn2 = NEXT_INSN (insn2))
      if (INSN_P (insn2))
	add_dependence (insn2, last_sync, REG_DEP_TRUE);
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

/* A callback for the hw-doloop pass.  Called when a loop we have discovered
   turns out not to be optimizable; we have to split the loop_end pattern into
   a subtract and a test.  */

static void
hwloop_fail (hwloop_info loop)
{
  rtx test;
  rtx_insn *insn = loop->loop_end;
  int has_reload = (recog_memoized (insn) == CODE_FOR_loop_end_withreload);

  emit_insn_before (gen_addsi3 (loop->iter_reg, loop->iter_reg, constm1_rtx),
		    loop->loop_end);

  /* Copy back the counter in the memory as reload has to be handled
     by ourself */
  if (has_reload)
    {
      rtx mem_dest = SET_DEST (XVECEXP (PATTERN (loop->loop_end), 0, 2));
      emit_insn_before (gen_movsi (mem_dest, loop->iter_reg), loop->loop_end);
    }

  test = gen_rtx_NE (VOIDmode, loop->iter_reg, const0_rtx);
  insn = emit_jump_insn_before (gen_cbranchsi4 (test, loop->iter_reg,
						const0_rtx, loop->start_label),
				loop->loop_end);

  JUMP_LABEL (insn) = loop->start_label;
  LABEL_NUSES (loop->start_label)++;


  delete_insn (loop->loop_end);
}

/* A callback for the hw-doloop pass.  This function examines INSN; if
   it is a doloop_end pattern we recognize, return the reg rtx for the
   loop counter.  Otherwise, return NULL_RTX.  */

static rtx
hwloop_pattern_reg (rtx_insn *insn)
{
  rtx reg;

  if (!JUMP_P (insn)
      || (recog_memoized (insn) != CODE_FOR_loop_end
	  && recog_memoized (insn) != CODE_FOR_loop_end_withreload))
    return NULL_RTX;

  reg = SET_DEST (XVECEXP (PATTERN (insn), 0, 1));
  if (!REG_P (reg))
    return NULL_RTX;

  return reg;
}

static bool
hwloop_optimize (hwloop_info loop)
{
  int i;
  edge entry_edge;
  basic_block entry_bb;
  rtx iter_reg;
  rtx_insn *insn;
  rtx_insn *seq, *entry_after;

  if (!TARGET_HWLOOP)
    {
      if (dump_file)
	fprintf (dump_file, ";; loop %d not hw optimised as opt disabled\n",
		 loop->loop_no);
      return false;
    }

  if (loop->depth > 1)
    {
      if (dump_file)
	fprintf (dump_file, ";; loop %d is not innermost\n", loop->loop_no);
      return false;
    }

  if (loop->jumps_within)
    {
      if (dump_file)
	fprintf (dump_file, ";; loop %d jumps within\n", loop->loop_no);
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

  if (loop->has_call)
    {
      if (dump_file)
	fprintf (dump_file, ";; loop %d has calls\n", loop->loop_no);
      return false;
    }

  if (loop->blocks.length () > 1)
    {
      if (dump_file)
	fprintf (dump_file, ";; loop %d has more than one basic block\n",
		 loop->loop_no);
      return false;
    }

  // FIXME AUTO: do we let asm in the loop ?

  /* Scan all the blocks to make sure they don't use iter_reg.  */
  /* FIXME AUTO: hwloop can still be used for branching. */
  if (loop->iter_reg_used || loop->iter_reg_used_outside)
    {
      if (dump_file)
	fprintf (dump_file, ";; loop %d uses iterator\n", loop->loop_no);
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

  /* Place the zero_cost_loop_start instruction before the loop.  */
  entry_bb = entry_edge->src;

  start_sequence ();
  loop->end_label = gen_label_rtx ();

  emit_label_after (loop->end_label, loop->loop_end);

  insn = emit_insn (
    gen_loop_start (loop->iter_reg, loop->iter_reg, loop->end_label));

  seq = get_insns ();

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
      while (DEBUG_INSN_P (entry_after)
	     || (NOTE_P (entry_after)
		 && NOTE_KIND (entry_after) != NOTE_INSN_BASIC_BLOCK
		 /* Make sure we don't split a call and its corresponding
		    CALL_ARG_LOCATION note.  */
		 && NOTE_KIND (entry_after) != NOTE_INSN_CALL_ARG_LOCATION))
	entry_after = PREV_INSN (entry_after);

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
  = {hwloop_pattern_reg, hwloop_optimize, hwloop_fail};

static void
kvx_reorg_loops (void)
{
  if (optimize)
    reorg_loops (false, &kvx_doloop_hooks);
}

/* Implement the TARGET_MACHINE_DEPENDENT_REORG pass.  */

static void
kvx_reorg (void)
{
  compute_bb_for_insn ();

  /* If optimizing, we'll have split before scheduling.  */
  if (optimize == 0)
    split_all_insns ();

  df_analyze ();

  /* Doloop optimization. */
  kvx_reorg_loops ();

  /* Do it even if ! TARGET_BUNDLING because it also takes care of
   cleaning previous data */
  initiate_bundle_states ();

  if (scheduling && TARGET_BUNDLING)
    {
      kvx_gen_bundles ();
      kvx_fix_debug_for_bundles ();
    }

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

/* Implement TARGET_OPTION_OVERRIDE.  */

static void
kvx_option_override (void)
{
  unsigned int i;
  cl_deferred_option *opt;
  vec<cl_deferred_option> *v = (vec<cl_deferred_option> *) kvx_deferred_options;

  if (v)
    FOR_EACH_VEC_ELT (*v, i, opt)
      {
	switch (opt->opt_index)
	  {
	  case OPT_ffixed_reg:
	    kvx_handle_fixed_reg_option (opt->arg);
	    break;
	  default:
	    gcc_unreachable ();
	  }
      }

  kvx_arch_schedule = ARCH_COOLIDGE;
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
kvx_reassociation_width (unsigned int opc, enum machine_mode mode)
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

/* Implement TARGET_SCHED_CAN_SPECULATE_INSN.  Return true if INSN can be
   scheduled for speculative execution.  Reject the long-running division
   and square-root instructions. (Like in aarch64.c) */

static bool
kvx_sched_can_speculate_insn (rtx_insn *insn)
{
  switch (get_attr_type (insn))
    {
    case TYPE_ALU_FULL_COPRO:
      return false;
    default:
      return true;
    }
}

bool
kvx_float_fits_bits (const REAL_VALUE_TYPE *r, unsigned bitsz,
		     enum machine_mode mode)
{
  long l[2];
  gcc_assert (mode == DFmode || mode == SFmode);

  if (mode == SFmode)
    {
      REAL_VALUE_TO_TARGET_SINGLE (*r, l[0]);
      return (l[0] == 0)
	     || (8 * sizeof (l[0]) - __builtin_clzl (l[0])) <= bitsz;
    }
  else
    { /*  (mode == DFmode) */
      REAL_VALUE_TO_TARGET_DOUBLE (*r, l);
      if (bitsz > 32)
	{
	  if (l[1] == 0)
	    {
	      return true;
	    }
	  bitsz -= 32;
	  return (8 * sizeof (l[1]) - __builtin_clzl (l[1])) <= bitsz;
	}
      else
	{
	  if (l[1] != 0)
	    {
	      return false;
	    }
	  return ((8 * sizeof (l[0]) - __builtin_clzl (l[0])) <= bitsz);
	}
    }
}

/* Returns a pattern suitable for copyq asm insn with the paired
   register SRCREG correctly split in 2 separate register reference:
   $r0r1 => "copyq %0 = $r0, $r1" if optimising for size.  Returns "#"
   if it is not the case to force the insn to be split in 2 copyd
   insns at the cost of an extra instruction.
 */
const char *
kvx_asm_pat_copyq (rtx srcreg)
{
  static char templ[128];

  /* The single-word copyq will be split in 2 copyd */
  if (!optimize_size)
    return "#";

  snprintf (templ, sizeof (templ), "copyq %%0 = $r%d, $r%d", REGNO (srcreg),
	    REGNO (srcreg) + 1);
  return templ;
}

/* Returns the pattern for copyo when optimizing for code size or
   forces a split by returning #
 */
const char *
kvx_asm_pat_copyo (void)
{
  if (!optimize_size)
    return "#";
  return "copyo %0 = %1";
}

/* Returns TRUE if OP is a REG (directly or through a SUBREG) */
bool
kvx_is_reg_subreg_p (rtx op)
{
  return REG_P (op) || (SUBREG_P (op) && REG_P (SUBREG_REG (op)));
}

/* Returns the regno associated with the REG or SUBREG in OP */
unsigned int
kvx_regno_subregno (rtx op)
{
  if (REG_P (op))
    return REGNO (op);
  else
    return REGNO (SUBREG_REG (op));
}

/* Returns TRUE if OP is a pseudo REG (directly or through a SUBREG)
 */
static bool
kvx_is_pseudo_reg_subreg_p (rtx op)
{
  return ((REG_P (op) && !HARD_REGISTER_P (op))
	  || (SUBREG_P (op) && REG_P (SUBREG_REG (op))
	      && !HARD_REGISTER_P (SUBREG_REG (op))));
}

/* Returns TRUE if OP is a hard (sub)register aligned on ALIGN or a
 * pseudo (sub)register, FALSE for all other cases. */
static bool
kvx_check_align_reg (rtx op, int align)
{
  if (!kvx_is_reg_subreg_p (op))
    return false;
  if (kvx_is_pseudo_reg_subreg_p (op))
    return true;

  const bool aligned_reg = REG_P (op) && REGNO (op) % align == 0;

  const bool aligned_subreg
    = SUBREG_P (op) && REG_P (SUBREG_REG (op))
      && (REGNO (SUBREG_REG (op)) + SUBREG_BYTE (op) / UNITS_PER_WORD) % align
	   == 0;

  return aligned_reg || aligned_subreg;
}

/* Returns TRUE if OP is an even hard (sub)register or a pseudo
 * (sub)register, FALSE for all other cases. It is used to check
 * correct alignement for some SIMD insn or 128bits load/store */
bool
kvx_ok_for_paired_reg_p (rtx op)
{
  return kvx_check_align_reg (op, 2);
}

/* Returns TRUE if OP is a hard (sub)register quad aligned or a pseudo
 * (sub)register, FALSE for all other cases. It is used to check
 * correct alignement for some SIMD insn or 256bits load/store */
bool
kvx_ok_for_quad_reg_p (rtx op)
{
  return kvx_check_align_reg (op, 4);
}

/* Split a 128bit move op in mode MODE from SRC to DST in 2 smaller
   64bit moves */
void
kvx_split_128bits_move (rtx dst, rtx src, enum machine_mode mode)
{
  rtx lo_src = gen_lowpart (word_mode, src);
  rtx hi_src = gen_highpart_mode (word_mode, mode, src);

  /* Avoid overwriting source by dest in case register ranges are
     overlapping */
  bool low_first = true;

  if (kvx_is_reg_subreg_p (dst) && kvx_is_reg_subreg_p (src))
    low_first = kvx_regno_subregno (dst) < kvx_regno_subregno (src);

  if (low_first)
    {
      emit_insn (gen_movdi (gen_lowpart (word_mode, dst), lo_src));
      emit_insn (gen_movdi (gen_highpart (word_mode, dst), hi_src));
    }
  else
    {
      emit_insn (gen_movdi (gen_highpart (word_mode, dst), hi_src));
      emit_insn (gen_movdi (gen_lowpart (word_mode, dst), lo_src));
    }
}

/* Split a 256bit move op in mode MODE from SRC to DST in 2 smaller
   128bit moves */
void
kvx_split_256bits_move (rtx dst, rtx src, enum machine_mode mode)
{
  /* Avoid overwriting source by dest in case register ranges are
     overlapping */
  bool low_first = true;

  if (kvx_is_reg_subreg_p (dst) && kvx_is_reg_subreg_p (src))
    low_first = kvx_regno_subregno (dst) < kvx_regno_subregno (src);

  emit_insn (
    gen_movti (simplify_gen_subreg (TImode, dst, mode, low_first ? 0 : 16),
	       simplify_gen_subreg (TImode, src, mode, low_first ? 0 : 16)));

  emit_insn (
    gen_movti (simplify_gen_subreg (TImode, dst, mode, low_first ? 16 : 0),
	       simplify_gen_subreg (TImode, src, mode, low_first ? 16 : 0)));
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
  emit_library_call (fun, LCT_NORMAL, VOIDmode, 1, ra_arg, Pmode);
}
#endif

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

#undef TARGET_CLASS_MAX_NREGS
#define TARGET_CLASS_MAX_NREGS kvx_class_max_nregs

#undef TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE kvx_option_override

#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE kvx_function_value

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

#undef TARGET_DECIMAL_FLOAT_SUPPORTED_P
#define TARGET_DECIMAL_FLOAT_SUPPORTED_P kvx_decimal_float_supported_p

#undef TARGET_FIXED_POINT_SUPPORTED_P
#define TARGET_FIXED_POINT_SUPPORTED_P kvx_fixed_point_supported_p

#undef TARGET_VECTOR_MODE_SUPPORTED_P
#define TARGET_VECTOR_MODE_SUPPORTED_P kvx_vector_mode_supported_p

#undef TARGET_VECTORIZE_SUPPORT_VECTOR_MISALIGNMENT
#define TARGET_VECTORIZE_SUPPORT_VECTOR_MISALIGNMENT                           \
  kvx_support_vector_misalignment

#undef TARGET_VECTORIZE_PREFERRED_SIMD_MODE
#define TARGET_VECTORIZE_PREFERRED_SIMD_MODE kvx_vectorize_preferred_simd_mode

#undef TARGET_VECTORIZE_VEC_PERM_CONST_OK
#define TARGET_VECTORIZE_VEC_PERM_CONST_OK kvx_vectorize_vec_perm_const_ok

#undef TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES hook_bool_const_tree_true

#undef TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES kvx_arg_partial_bytes

#undef TARGET_MUST_PASS_IN_STACK
#define TARGET_MUST_PASS_IN_STACK must_pass_in_stack_var_size

#undef TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE kvx_pass_by_reference

#undef TARGET_SECONDARY_RELOAD
#define TARGET_SECONDARY_RELOAD kvx_secondary_reload

#undef TARGET_INIT_BUILTINS
#define TARGET_INIT_BUILTINS kvx_init_builtins

#undef TARGET_EXPAND_BUILTIN
#define TARGET_EXPAND_BUILTIN kvx_expand_builtin

#undef TARGET_CANNOT_FORCE_CONST_MEM
#define TARGET_CANNOT_FORCE_CONST_MEM kvx_cannot_force_const_mem

#undef TARGET_SCHED_ADJUST_COST
#define TARGET_SCHED_ADJUST_COST kvx_sched_adjust_cost

#undef TARGET_SCHED_ISSUE_RATE
#define TARGET_SCHED_ISSUE_RATE kvx_sched_issue_rate

#undef TARGET_SCHED_FIRST_CYCLE_MULTIPASS_DFA_LOOKAHEAD
#define TARGET_SCHED_FIRST_CYCLE_MULTIPASS_DFA_LOOKAHEAD                       \
  kvx_sched_first_cycle_multipass_dfa_lookahead

#undef TARGET_SCHED_SET_SCHED_FLAGS
#define TARGET_SCHED_SET_SCHED_FLAGS kvx_sched_set_sched_flags

#undef TARGET_SCHED_INIT_GLOBAL
#define TARGET_SCHED_INIT_GLOBAL kvx_sched_init_global

#undef TARGET_SCHED_DFA_NEW_CYCLE
#define TARGET_SCHED_DFA_NEW_CYCLE kvx_sched_dfa_new_cycle

#undef TARGET_SCHED_REASSOCIATION_WIDTH
#define TARGET_SCHED_REASSOCIATION_WIDTH kvx_reassociation_width

#undef TARGET_RTX_COSTS
#define TARGET_RTX_COSTS kvx_rtx_costs

#undef TARGET_ADDRESS_COST
#define TARGET_ADDRESS_COST kvx_address_cost

#undef TARGET_REGISTER_MOVE_COST
#define TARGET_REGISTER_MOVE_COST kvx_register_move_cost

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

#undef TARGET_SCHED_DEPENDENCIES_EVALUATION_HOOK
#define TARGET_SCHED_DEPENDENCIES_EVALUATION_HOOK                              \
  kvx_dependencies_evaluation_hook

#undef TARGET_MACHINE_DEPENDENT_REORG
#define TARGET_MACHINE_DEPENDENT_REORG kvx_reorg

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

#undef TARGET_MODE_DEPENDENT_ADDRESS_P
#define TARGET_MODE_DEPENDENT_ADDRESS_P kvx_mode_dependent_address_p

#undef TARGET_SCHED_CAN_SPECULATE_INSN
#define TARGET_SCHED_CAN_SPECULATE_INSN kvx_sched_can_speculate_insn

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

/* FIXME AUTO: trampoline are broken T6775 */
#undef TARGET_STATIC_CHAIN
#define TARGET_STATIC_CHAIN kvx_static_chain

struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-kvx.h"
