/* Definitions of target machine for GNU compiler, for K1 MPPA.
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

#include "k1-protos.h"
#include "k1-opts.h"

#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

#undef TARGET_HAVE_TLS
#define TARGET_HAVE_TLS (true)

static bool scheduling = false;

rtx k1_sync_reg_rtx;
rtx k1_link_reg_rtx;

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
enum attr_arch k1_arch_schedule;

/* Information about a function's frame layout.  */
struct GTY (()) k1_frame_info
{
  /* The size of the frame in bytes.  */
  HOST_WIDE_INT total_size;

  /* The offset from the initial SP value to its new value */
  HOST_WIDE_INT initial_sp_offset;

  /* Offsets of save area from frame bottom */
  HOST_WIDE_INT saved_reg_sp_offset;

  /* Offset of virtual frame pointer from stack pointer/frame bottom */
  HOST_WIDE_INT frame_pointer_offset;

  /* Offset of hard frame pointer from stack pointer/frame bottom */
  HOST_WIDE_INT hard_frame_pointer_offset;

  /* The offset of arg_pointer_rtx from the frame pointer.  */
  HOST_WIDE_INT arg_pointer_fp_offset;

  /* Offset to the static chain pointer, if needed */
  HOST_WIDE_INT static_chain_fp_offset;

  HOST_WIDE_INT reg_offset[FIRST_PSEUDO_REGISTER];
  HOST_WIDE_INT saved_regs_size;

  bool laid_out;
};

struct GTY (()) machine_function
{
  char save_reg[FIRST_PSEUDO_REGISTER];

  k1_frame_info frame;

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
				| Args          |    |
				+---------------+ <--/
				+---------------+
				| Varargs       |
				|               |
				|               |
				+---------------+
 Argument Pointer / Virt. FP--->| [Static chain]|
				+---------------+
				| Local         |
				| Variable      |
				|               |
				+---------------+
				|               |
				| Register      |
				| Save          |
				|               |
				| $ra           | (if frame_pointer_needed)
		     Hard FP--->| caller FP     | (if frame_pointer_needed)
				+---------------+
				|               |
				| Outgoing      |
				| Args          |
			  SP--->|               |
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
k1_compute_frame_info (void)
{
  struct k1_frame_info *frame;

  /* Offset of new SP wrt incoming SP */
  /* Used for creating pointers from new SP values. */
  HOST_WIDE_INT sp_offset = 0;

  /* FP offset wrt new SP */
  HOST_WIDE_INT fp_offset = 0;

  frame = &cfun->machine->frame;
  memset (frame, 0, sizeof (*frame));

  /* At the bottom of the frame are any outgoing stack arguments. */
  sp_offset += crtl->outgoing_args_size;

  /* Saved registers area */
  frame->saved_reg_sp_offset = sp_offset;

  frame->hard_frame_pointer_offset = sp_offset;

#define SLOT_NOT_REQUIRED (-2)
#define SLOT_REQUIRED (-1)

  /* Mark which register should be saved... */
  for (int regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    if (should_be_saved_in_prologue (regno))
      cfun->machine->frame.reg_offset[regno] = SLOT_REQUIRED;
    else
      cfun->machine->frame.reg_offset[regno] = SLOT_NOT_REQUIRED;

  if (frame_pointer_needed)
    {
      /* Enforce ABI that requires the FP to point to previous FP value and $ra
       */
      cfun->machine->frame.reg_offset[HARD_FRAME_POINTER_REGNUM] = sp_offset;
      cfun->machine->frame.reg_offset[K1C_RETURN_POINTER_REGNO]
	= sp_offset + UNITS_PER_WORD;
      sp_offset += 2 * UNITS_PER_WORD;
    }

  /* ... assign stack slots */
  for (int regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    if (cfun->machine->frame.reg_offset[regno] == SLOT_REQUIRED)
      {
	cfun->machine->frame.reg_offset[regno] = sp_offset;
	sp_offset += UNITS_PER_WORD;
      }

  frame->saved_regs_size = sp_offset - frame->saved_reg_sp_offset;

  /* Next are automatic variables. */
  sp_offset += get_frame_size ();

  /* ABI requires 8-bytes (64-bits) alignment. */
#define K1_STACK_ALIGN(LOC) (((LOC) + 0x7) & ~0x7)

  /* Vararg area must be correctly aligned, else var args may not be correctly
   * pushed */
  sp_offset = K1_STACK_ALIGN (sp_offset);

  /* Frame pointer points between automatic var & varargs */
  fp_offset = sp_offset;

  /* FIXME AUTO: trampoline are broken T6775 */
  if (cfun->machine->static_chain_on_stack)
    {
      frame->static_chain_fp_offset = sp_offset - fp_offset;
      sp_offset += UNITS_PER_WORD;
    }

  frame->arg_pointer_fp_offset = sp_offset - fp_offset;

  /* If any anonymous arg may be in register, push them on the stack */
  /* This can't break alignment */
  if (cfun->stdarg && crtl->args.info.next_arg_reg < K1C_ARG_REG_SLOTS)
    sp_offset
      += UNITS_PER_WORD * (K1C_ARG_REG_SLOTS - crtl->args.info.next_arg_reg);

  /* Next is the callee-allocated area for pretend stack arguments.  */
  sp_offset += crtl->args.pretend_args_size;

  frame->initial_sp_offset = sp_offset;

  frame->frame_pointer_offset = fp_offset;
  frame->total_size = sp_offset;

  frame->laid_out = true;
}

HOST_WIDE_INT
k1_first_parm_offset (tree decl ATTRIBUTE_UNUSED)
{
  struct k1_frame_info *frame;
  k1_compute_frame_info ();
  frame = &cfun->machine->frame;

  return frame->arg_pointer_fp_offset;
}

static rtx
k1_static_chain (const_tree fndecl, bool incoming_p ATTRIBUTE_UNUSED)
{
  if (!DECL_STATIC_CHAIN (fndecl))
    return NULL;

  cfun->machine->static_chain_on_stack = 1;

  return gen_frame_mem (Pmode, frame_pointer_rtx);
}

static const char *prf_reg_names[] = {K1C_PGR_REGISTER_NAMES};
static const char *qrf_reg_names[] = {K1C_QGR_REGISTER_NAMES};

/* Splits X as a base + offset. Returns true if split successful,
   false if not. BASE_OUT and OFFSET_OUT contain the corresponding
   split. If STRICT is false, base is not always a register.
 */
bool
k1_split_mem (rtx x, rtx *base_out, rtx *offset_out, bool strict)
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

#define K1_MAX_PACKED_LSU (4)
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
k1_pack_load_store (rtx operands[], unsigned int nops)
{
  rtx set_dests[K1_MAX_PACKED_LSU];
  rtx set_srcs[K1_MAX_PACKED_LSU];
  rtx sorted_operands[2 * K1_MAX_PACKED_LSU];

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
  if (!k1_split_mem (XEXP (sorted_operands[1 - op_offset], 0), &base_reg,
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
k1_hard_regno_rename_ok (unsigned int from, unsigned int to)
{
  if (IN_RANGE (from, K1C_GPR_FIRST_REGNO, K1C_GPR_LAST_REGNO)
      && IN_RANGE (to, K1C_GPR_FIRST_REGNO, K1C_GPR_LAST_REGNO))
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
k1_hard_regno_mode_ok (unsigned regno, enum machine_mode mode)
{
  // SI/DI -> K1C_GPR_FIRST_REGNO - K1C_GPR_LAST_REGNO => OK
  // SI/DI -> K1C_SFR_FIRST_REGNO - K1C_SFR_LAST_REGNO => OK
  // TI    -> K1C_GPR_FIRST_REGNO - K1C_GPR_LAST_REGNO && even => OK
  // OI    -> K1C_GPR_FIRST_REGNO - K1C_GPR_LAST_REGNO && 0mod4 => OK
  if (GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
    return 1;
  if (IN_RANGE (regno, K1C_GPR_FIRST_REGNO, K1C_GPR_LAST_REGNO))
    {
      if (GET_MODE_SIZE (mode) == 2 * UNITS_PER_WORD)
	return (regno % 2 == 0);
      if (GET_MODE_SIZE (mode) == 4 * UNITS_PER_WORD)
	return (regno % 4 == 0);
    }
  return 0;
}

static unsigned char
k1_class_max_nregs (reg_class_t regclass ATTRIBUTE_UNUSED,
		    enum machine_mode mode)
{
  return HARD_REGNO_NREGS (0, mode);
}

static tree k1_handle_fndecl_attribute (tree *node, tree name,
					tree args ATTRIBUTE_UNUSED,
					int flags ATTRIBUTE_UNUSED,
					bool *no_add_attrs);

static bool function_symbol_referenced_p (rtx x);

static bool symbolic_reference_mentioned_p (rtx op);

static bool k1_output_addr_const_extra (FILE *, rtx);

static bool k1_legitimate_address_p (enum machine_mode mode, rtx x,
					    bool strict);

bool k1_legitimate_pic_symbolic_ref_p (rtx op);

static bool k1_legitimate_constant_p (enum machine_mode mode ATTRIBUTE_UNUSED,
				      rtx x);

/* Table of machine attributes.  */
static const struct attribute_spec k1_attribute_table[] = {
  /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler,
     affects_type } */
  {"no_save_regs", 0, 0, true, false, false, k1_handle_fndecl_attribute, false},
  {"farcall", 0, 0, true, false, false, k1_handle_fndecl_attribute, false},
  {NULL, 0, 0, false, false, false, NULL, false}};

/* Returns 0 if there is no TLS ref, != 0 if there is.

  Beware that UNSPEC_TLS are not symbol ref, they are offset within
  TLS.
 */
int
k1_has_tls_reference (rtx x)
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
k1_has_unspec_reference_1 (rtx *x)
{
  return (GET_CODE (*x) == UNSPEC
	  && (XINT (*x, 1) == UNSPEC_GOT || XINT (*x, 1) == UNSPEC_GOTOFF
	      || XINT (*x, 1) == UNSPEC_PCREL || XINT (*x, 1) == UNSPEC_TLS_GD
	      || XINT (*x, 1) == UNSPEC_TLS_LD || XINT (*x, 1) == UNSPEC_TLS_LE
	      || XINT (*x, 1) == UNSPEC_TLS_IE));
}

static int
k1_has_unspec_reference (rtx x)
{
  subrtx_ptr_iterator::array_type array;
  FOR_EACH_SUBRTX_PTR (iter, array, &x, ALL)
    {
      rtx *x = *iter;
      switch (k1_has_unspec_reference_1 (x))
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
  //   return for_each_rtx (&x, &k1_has_unspec_reference_1, NULL);
}

static bool
k1_legitimate_address_register_p (rtx reg, bool strict)
{
  return (REG_P (reg) && IS_GENERAL_REGNO (REGNO (reg), strict)
	  && GET_MODE (reg) == Pmode);
}

static bool
k1_legitimate_address_offset_register_p (rtx reg, bool strict)
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
k1_legitimate_address_p (machine_mode mode, rtx x, bool strict)
{

  /*
   * ld reg = 0[reg]
   */
  if (k1_legitimate_address_register_p (x, strict))
    return true;

  /*
   * ld reg = @got[reg]
   * ld reg = @gotoff[reg]
   */
  if (GET_CODE (x) == PLUS
      && k1_legitimate_address_register_p (XEXP (x, 0), strict)
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
      && k1_legitimate_address_register_p (XEXP (x, 0), strict)
      && ((CONSTANT_P (XEXP (x, 1))
	   && k1_legitimate_constant_p (VOIDmode, XEXP (x, 1)))
	  || GET_CODE (XEXP (x, 1)) == CONST_INT)
      && immediate_operand (XEXP (x, 1), DImode)
      && (!current_pass || current_pass->tv_id != TV_CPROP))
    return true;

  /*
   * ld reg = reg[reg]
   */
  if (GET_CODE (x) == PLUS
      && k1_legitimate_address_register_p (XEXP (x, 0), strict)
      && k1_legitimate_address_offset_register_p (XEXP (x, 1), strict))
    return true;

  /*
   * ld.xs reg = reg[reg]
   */
  if (GET_CODE (x) == PLUS
      && k1_legitimate_address_register_p (XEXP (x, 1), strict)
      && GET_CODE (XEXP (x, 0)) == MULT
      && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT
      && (INTVAL (XEXP (XEXP (x, 0), 1)) == GET_MODE_SIZE (mode))
      && k1_legitimate_address_offset_register_p (XEXP (XEXP (x, 0), 0),
						  strict))
    // The .xs addressing mode applies to object sizes 2, 4, 8, 16, 32.
    return GET_MODE_SIZE (mode) > 1 && GET_MODE_SIZE (mode) <= 32;

  return false;
}

static void
k1_conditional_register_usage (void)
{
  k1_sync_reg_rtx = gen_rtx_REG (SImode, K1C_SYNC_REG_REGNO);
  k1_link_reg_rtx = gen_rtx_REG (Pmode, K1C_RETURN_POINTER_REGNO);
}

rtx
k1_return_addr_rtx (int count, rtx frameaddr ATTRIBUTE_UNUSED)
{
  return count == 0 ? get_hard_reg_initial_val (Pmode, K1C_RETURN_POINTER_REGNO)
		    : NULL_RTX;
}

/* Implements the macro INIT_CUMULATIVE_ARGS defined in k1.h. */

void
k1_init_cumulative_args (CUMULATIVE_ARGS *cum,
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
struct k1_arg_info
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
k1_get_arg_info (struct k1_arg_info *info, cumulative_args_t cum_v,
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
  if (cum->next_arg_reg >= K1C_ARG_REG_SLOTS)
    {
      info->num_stack = n_words;
      info->num_regs = 0;
      return NULL_RTX;
    }

  info->num_regs = K1C_ARG_REG_SLOTS - info->first_reg;

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

  return gen_rtx_REG (mode, K1C_ARGUMENT_POINTER_REGNO + info->first_reg);
}

/* Implements TARGET_FUNCTION_ARG.
   Returns a reg rtx pointing at first argument register to be
   used for given argument or NULL_RTX if argument must be stacked
   because there is no argument slot in registers free. */

static rtx
k1_function_arg (cumulative_args_t cum_v, enum machine_mode mode,
		 const_tree type, bool named)
{
  struct k1_arg_info info;
  return k1_get_arg_info (&info, cum_v, mode, type, named);
}

/* Implements TARGET_ARG_PARTIAL_BYTES.
   Return the number of bytes, at the beginning of an argument,
   that must be put in registers */

static int
k1_arg_partial_bytes (cumulative_args_t cum_v, enum machine_mode mode,
		      tree type, bool named ATTRIBUTE_UNUSED)
{
  struct k1_arg_info info = {0};
  rtx reg = k1_get_arg_info (&info, cum_v, mode, type, named);
  if (reg != NULL_RTX && info.num_regs > 0 && info.num_stack > 0)
    {
      return info.num_regs * UNITS_PER_WORD;
    }
  return 0;
}

static void
k1_function_arg_advance (cumulative_args_t cum_v, enum machine_mode mode,
			 const_tree type, bool named)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
  struct k1_arg_info info = {0};
  k1_get_arg_info (&info, cum_v, mode, type, named);

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
k1_function_value (const_tree ret_type, const_tree func ATTRIBUTE_UNUSED,
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
					    K1C_ARGUMENT_POINTER_REGNO + i),
			       GEN_INT (i * UNITS_PER_WORD));
      return ret;
    }
  else
    return gen_rtx_REG (TYPE_MODE (ret_type), K1C_ARGUMENT_POINTER_REGNO);
}

/* Implements TARGET_RETURN_IN_MSB */
static bool
k1_return_in_msb (const_tree type ATTRIBUTE_UNUSED)
{
  return false;
}

/* Implements TARGET_RETURN_IN_MEMORY */
static bool
k1_return_in_memory (const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  HOST_WIDE_INT sz = int_size_in_bytes (type);

  /* Return value can use up to 4 registers (256bits). Larger values
   * or variable sized type must be returned in memory. */
  return (sz > (4 * UNITS_PER_WORD) || sz < 0);
}

/* Implements TARGET_STRUCT_VALUE_RTX */
static rtx
k1_struct_value_rtx (tree fndecl ATTRIBUTE_UNUSED,
		     int incoming ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (Pmode, K1C_STRUCT_POINTER_REGNO);
}

static void
k1_asm_output_mi_thunk (FILE *file ATTRIBUTE_UNUSED,
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
k1_asm_can_output_mi_thunk (const_tree thunk_fndecl ATTRIBUTE_UNUSED,
			    HOST_WIDE_INT delta ATTRIBUTE_UNUSED,
			    HOST_WIDE_INT vcall_offset ATTRIBUTE_UNUSED,
			    const_tree function ATTRIBUTE_UNUSED)
{
  return true;
}

static rtx
k1_expand_builtin_saveregs (void)
{
  int regno;
  int slot = 0;
  struct k1_frame_info *frame;

  k1_compute_frame_info ();
  frame = &cfun->machine->frame;
  rtx area = gen_rtx_PLUS (Pmode, frame_pointer_rtx,
			   GEN_INT (frame->arg_pointer_fp_offset));

  /* All argument register slots used for named args, nothing to push */
  if (crtl->args.info.next_arg_reg >= K1C_ARG_REG_SLOTS)
    return const0_rtx;

  /* use arg_pointer since saved register slots are not known at that time */
  regno = crtl->args.info.next_arg_reg;

  if (regno & 1)
    {
      rtx insn = emit_move_insn (gen_rtx_MEM (DImode, area),
				 gen_rtx_REG (DImode, K1C_ARGUMENT_POINTER_REGNO
							+ regno));
      RTX_FRAME_RELATED_P (insn) = 1;
      /* Do not attach a NOTE here as the frame has not been laid out yet.
       Let the k1_fix_debug_for_bundles function during reorg pass handle these
     */

      regno++;
      slot++;
    }

  for (; regno < K1C_ARG_REG_SLOTS; regno += 2, slot += 2)
    {
      rtx addr
	= gen_rtx_MEM (TImode,
		       gen_rtx_PLUS (Pmode, frame_pointer_rtx,
				     GEN_INT (slot * UNITS_PER_WORD
					      + frame->arg_pointer_fp_offset)));
      rtx src = gen_rtx_REG (TImode, K1C_ARGUMENT_POINTER_REGNO + regno);

      rtx insn = emit_move_insn (addr, src);
      RTX_FRAME_RELATED_P (insn) = 1;
      /* Do not attach a NOTE here as the frame has not been laid out yet.
       Let the k1_fix_debug_for_bundles function during reorg pass handle these
     */
    }

  return area;
}

static void
k1_expand_va_start (tree valist, rtx nextarg ATTRIBUTE_UNUSED)
{
  rtx va_start_addr = expand_builtin_saveregs ();
  rtx va_r = expand_expr (valist, NULL_RTX, VOIDmode, EXPAND_WRITE);

  struct k1_frame_info *frame;

  frame = &cfun->machine->frame;
  gcc_assert (frame->laid_out);

  /* All arg registers must be used by named parameter, va_start
     must point to caller frame for first anonymous parameter ... */
  if (va_start_addr == const0_rtx && crtl->args.info.anonymous_arg_offset_valid)
    {
      /* ... and there are some more arguments. */
      va_start_addr
	= gen_rtx_PLUS (Pmode, frame_pointer_rtx,
			GEN_INT (crtl->args.info.anonymous_arg_offset
				 + frame->arg_pointer_fp_offset));
    }
  else
    {
      /* ... and there are no more argument. */
      va_start_addr = gen_rtx_PLUS (Pmode, frame_pointer_rtx,
				    GEN_INT (frame->arg_pointer_fp_offset));
    }

  emit_move_insn (va_r, va_start_addr);
}

static bool
k1_cannot_force_const_mem (enum machine_mode mode ATTRIBUTE_UNUSED,
			   rtx x ATTRIBUTE_UNUSED)
{
  return true;
}

static bool
k1_decimal_float_supported_p (void)
{
  return true;
}

static bool
k1_fixed_point_supported_p (void)
{
  return false;
}

static bool
k1_vector_mode_supported_p (enum machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (mode)
    {
    case V4HImode:
    case V8HImode:
    case V2SImode:
    case V4SImode:
    case V2DImode:
    case V4DImode:
    case V2SFmode:
    case V4SFmode:
    case V2DFmode:
      return true;
    default:
      break;
    }
  return false;
}

static bool
k1_support_vector_misalignment (enum machine_mode mode ATTRIBUTE_UNUSED,
				const_tree type ATTRIBUTE_UNUSED,
				int misalignment ATTRIBUTE_UNUSED,
				bool is_packed ATTRIBUTE_UNUSED)
{
  return !TARGET_STRICT_ALIGN;
}

static bool
k1_promote_prototypes (const_tree fndecl ATTRIBUTE_UNUSED)
{
  return true;
}

static bool
k1_pass_by_reference (cumulative_args_t cum ATTRIBUTE_UNUSED,
		      enum machine_mode mode, const_tree type,
		      bool named ATTRIBUTE_UNUSED)
{
  HOST_WIDE_INT size;
  machine_mode dummymode;
  int nregs;

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
k1_secondary_reload (bool in_p ATTRIBUTE_UNUSED, rtx x ATTRIBUTE_UNUSED,
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
  else if (SYMBOLIC_CONST (x) && !k1_legitimate_constant_p (VOIDmode, x))
    {
      gcc_unreachable ();
    }
  return NO_REGS;
}

static const char *k1_unspec_tls_asm_op[]
  = {"@tlsgd", "@tlsld", "@tlsle", "@dtpoff", "@tlsie"};

void
k1_print_operand (FILE *file, rtx x, int code)
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
      addr_space = k1_is_uncached_mem_op_p (x);
      if (addr_space == K1_ADDR_SPACE_BYPASS)
	fprintf (file, ".u");
      if (addr_space == K1_ADDR_SPACE_PRELOAD)
	fprintf (file, ".us");
      return;

    default:
      gcc_unreachable ();
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
	  fprintf (file, "$%s", qrf_reg_names[REGNO (operand)]);
	}
      else if (select_preg)
	{
	  fprintf (file, "$%s", prf_reg_names[REGNO (operand)]);
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
	  fprintf (file, "$%s", qrf_reg_names[REGNO (operand)]);
	}
      else if (GET_MODE_SIZE (GET_MODE (x)) == UNITS_PER_WORD * 2)
	{
	  fprintf (file, "$%s", prf_reg_names[REGNO (operand)]);
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
	    k1_print_operand (file, x, 0);
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
	  fprintf (file, "0x%x", (unsigned int) l);
	  return;
	}
      else if (GET_MODE (x) == DFmode)
	{
	  /* this is a double that should fit on less than 64bits */
	  REAL_VALUE_TYPE r;
	  long l[2];
	  r = *CONST_DOUBLE_REAL_VALUE (operand);
	  REAL_VALUE_TO_TARGET_DOUBLE (r, l);
	  fprintf (file, "0x%x%08x", (unsigned int) l[1], (unsigned int) l[0]);
	  return;
	}
      gcc_unreachable ();
      return;

    case CONST_INT:
      fprintf (file, HOST_WIDE_INT_PRINT_DEC, INTVAL (x));
      return;

      case CONST_VECTOR: {
	int slice = 1 * select_yreg + 2 * select_zreg + 3 * select_treg;
	fprintf (file, HOST_WIDE_INT_PRINT_HEX,
		 k1_const_vector_value (x, slice));
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
		fputs (k1_unspec_tls_asm_op[unspec - UNSPEC_TLS_GD], (file));
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
k1_regname (rtx x)
{
  machine_mode mode = GET_MODE (x);

  switch (GET_CODE (x))
    {
    case REG:
      if (GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
	return reg_names[REGNO (x)];
      else if (GET_MODE_SIZE (mode) <= 2 * UNITS_PER_WORD)
	return prf_reg_names[REGNO (x)];
      else if (GET_MODE_SIZE (mode) <= 4 * UNITS_PER_WORD)
	return qrf_reg_names[REGNO (x)];
      gcc_unreachable ();
    case SUBREG:
      // Addressing mode with register offset
      gcc_assert (TARGET_32);
      gcc_assert (SUBREG_BYTE (x) == 0);
      return k1_regname (SUBREG_REG (x));
    default:
      gcc_unreachable ();
    }
}

void
k1_print_operand_address (FILE *file, rtx x)
{
  switch (GET_CODE (x))
    {
    case REG:
    case SUBREG:
      fprintf (file, "0[$%s]", k1_regname (x));
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
	    fprintf (file, "$%s", k1_regname (XEXP (XEXP (x, 0), 0)));
	  else
	    fprintf (file, "$%s", k1_regname (XEXP (x, 1)));
	  break;
	default:
	  abort ();
	}
      if (GET_CODE (XEXP (x, 0)) == MULT)
	fprintf (file, "[$%s]", k1_regname (XEXP (x, 1)));
      else
	fprintf (file, "[$%s]", k1_regname (XEXP (x, 0)));
      return;

    default:
      output_addr_const (file, x);
      break;
    }
}

/* Functions to save and restore machine-specific function data.  */
static struct machine_function *
k1_init_machine_status (void)
{
  struct machine_function *machine;
  machine = ggc_cleared_alloc<machine_function> ();

  return machine;
}

/* Do anything needed before RTL is emitted for each function.  */
void
k1_init_expanders (void)
{
  /* Arrange to initialize and mark the machine per-function status.  */
  init_machine_status = k1_init_machine_status;
}

bool
k1_print_punct_valid_p (unsigned char code)
{
  return code == ';';
}

/* Handle an attribute requiring a FUNCTION_DECL;
   arguments as in struct attribute_spec.handler.  */
static tree
k1_handle_fndecl_attribute (tree *node, tree name, tree args ATTRIBUTE_UNUSED,
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
k1_expand_tablejump (rtx op0, rtx op1)
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

/* Return TRUE if REGNO should be saves in the prologue of current function */
static bool
should_be_saved_in_prologue (int regno)
{
  return (df_regs_ever_live_p (regno)	   // reg is used
	  && !call_really_used_regs[regno] // reg is callee-saved
	  && (regno == K1C_RETURN_POINTER_REGNO || !fixed_regs[regno]));
}

static bool
k1_register_saved_on_entry (int regno)
{
  return cfun->machine->frame.reg_offset[regno] >= 0;
}

/* Returns a REG rtx with the nth hard reg that is safe to use in prologue
   (caller-saved and non fixed reg). Returns NULL_RTX and emits an
   error if no such register can be found. */
static rtx
k1_get_callersaved_nonfixed_reg (machine_mode mode, unsigned int n)
{
  int regno;
  unsigned int i;
  // start at R16 as as everything before that may be used.
  // We should be able to use the veneer regs if not fixed.
  for (i = 0, regno = 16; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      if (call_really_used_regs[regno] && !fixed_regs[regno] && (i++ == n))
	return gen_rtx_REG (mode, regno);
    }

  error ("No scratch register available in function prologue.");
  return NULL_RTX;
}

/* Save/Restore register at offsets previously computed in frame information
 * layout.
 */
static void
k1_save_or_restore_callee_save_registers (bool restore)
{
  struct k1_frame_info *frame = &cfun->machine->frame;
  rtx insn;
  rtx base_rtx = stack_pointer_rtx;
  rtx (*gen_mem_ref) (enum machine_mode, rtx) = gen_rtx_MEM;

  unsigned limit = FIRST_PSEUDO_REGISTER;
  unsigned regno;

  for (regno = 0; regno < limit; regno++)
    {
      if (k1_register_saved_on_entry (regno))
	{
	  rtx mem;

	  mem = gen_mem_ref (DImode, plus_constant (Pmode, base_rtx,
						    frame->reg_offset[regno]));

	  /* regno2 = k1_register_saved_on_entry(regno+1)? regno + 1 : 0; */
	  /* if (0 && regno2) */
	  /*   { */
	  /*     rtx mem2; */
	  /*     /\* Next highest register to be saved.  *\/ */
	  /*     mem2 = gen_mem_ref (Pmode, */
	  /* 			  plus_constant */
	  /* 			  (Pmode, */
	  /* 			   base_rtx, */
	  /* 			   start_offset + increment)); */
	  /*     if (restore) */
	  /* 	{ */
	  /* 	  insn = emit_insn */
	  /* 	    ( gen_load_multiple(mem, gen_rtx_REG(DImode, regno), */
	  /* 				GEN_INT(2))); */

	  /* 	  /\* insn = emit_insn *\/ */
	  /* 	  /\*   ( gen_load_pairdi (gen_rtx_REG (DImode, regno), mem,
	   * *\/ */
	  /* 	  /\* 		     gen_rtx_REG (DImode, regno2), mem2)); *\/
	   */

	  /* 	  /\* add_reg_note (insn, REG_CFA_RESTORE, gen_rtx_REG (DImode,
	   * regno)); *\/ */
	  /* 	  /\* add_reg_note (insn, REG_CFA_RESTORE, gen_rtx_REG (DImode,
	   * regno2)); *\/ */
	  /* 	} */
	  /*     else */
	  /* 	{ */
	  /* 	  /\* insn = emit_insn *\/ */
	  /* 	  /\*   ( gen_store_pairdi (mem, gen_rtx_REG (DImode, regno),
	   * *\/ */
	  /* 	  /\* 			mem2, gen_rtx_REG (DImode, regno2))); *\/
	   */
	  /* 	  insn = emit_insn */
	  /* 	    ( gen_store_multiple(gen_rtx_REG(DImode, regno), */
	  /* 				 mem, GEN_INT(2))); */
	  /* 	} */

	  /* 	  /\* The first part of a frame-related parallel insn */
	  /* 	     is always assumed to be relevant to the frame */
	  /* 	     calculations; subsequent parts, are only */
	  /* 	     frame-related if explicitly marked.  *\/ */
	  /*     RTX_FRAME_RELATED_P (XVECEXP (PATTERN (insn), 0, */
	  /* 				    1)) = 1; */
	  /*     regno = regno2; */
	  /*     start_offset += increment * 2; */
	  /*   } */
	  /* else */
	  {
	    machine_mode spill_mode = DImode;
	    rtx saved_reg = gen_rtx_REG (spill_mode, regno);
	    rtx orig_save_reg = saved_reg;

	    if (regno == K1C_RETURN_POINTER_REGNO)
	      {
		/* spill_mode = Pmode; */
		saved_reg = k1_get_callersaved_nonfixed_reg (spill_mode, 0);
		gcc_assert (saved_reg != NULL_RTX);

		if (restore == false)
		  {
		    rtx src_reg = gen_rtx_REG (spill_mode, regno);
		    insn = emit_move_insn (saved_reg, src_reg);
		    RTX_FRAME_RELATED_P (insn) = 1;

		    add_reg_note (insn, REG_CFA_REGISTER,
				  gen_rtx_SET (saved_reg, src_reg));
		  }
	      }

	    if (restore)
	      {
		insn = emit_move_insn (saved_reg, mem);
		if (regno == K1C_RETURN_POINTER_REGNO)
		  {
		    insn = emit_move_insn (gen_rtx_REG (spill_mode, regno),
					   saved_reg);
		  }
	      }
	    else
	      {
		insn = emit_move_insn (mem, saved_reg);
		RTX_FRAME_RELATED_P (insn) = 1;
		add_reg_note (insn, REG_CFA_OFFSET,
			      gen_rtx_SET (mem,
					   (regno == K1C_RETURN_POINTER_REGNO)
					     ? orig_save_reg
					     : saved_reg));
	      }
	  }
	}
    }
}

/* Implement INITIAL_ELIMINATION_OFFSET.  FROM is either the frame pointer
   or argument pointer.  TO is either the stack pointer or frame
   pointer.  */

HOST_WIDE_INT
k1_initial_elimination_offset (int from, int to)
{
  k1_compute_frame_info ();
  struct k1_frame_info *frame = &cfun->machine->frame;

  /* Should never have anything else FRAME_POINTER_REGNUM -> HFP/SP */
  if (from != FRAME_POINTER_REGNUM
      || (to != STACK_POINTER_REGNUM && to != HARD_FRAME_POINTER_REGNUM))
    gcc_unreachable ();

  if (from == FRAME_POINTER_REGNUM && to == STACK_POINTER_REGNUM)
    return frame->frame_pointer_offset;
  else if (from == FRAME_POINTER_REGNUM && to == HARD_FRAME_POINTER_REGNUM)
    return (frame->frame_pointer_offset - frame->hard_frame_pointer_offset);

  gcc_unreachable ();
}

/* Return TRUE if target supports -fstack-limit-register */

bool
k1_have_stack_checking (void)
{
#if defined(GCC_K1_MPPA_COS)
  return true;
#else
  return false;
#endif
}

void
k1_expand_prologue (void)
{
  k1_compute_frame_info ();
  struct k1_frame_info *frame = &cfun->machine->frame;
  HOST_WIDE_INT size = frame->initial_sp_offset;
  rtx insn;

  if (size > 0)
    {

      if (crtl->limit_stack)
	{
	  if (k1_have_stack_checking ())
	    {
	      rtx new_stack_pointer_rtx
		= k1_get_callersaved_nonfixed_reg (Pmode, 0);
	      rtx stack_limit_reg = k1_get_callersaved_nonfixed_reg (Pmode, 1);

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
  k1_save_or_restore_callee_save_registers (0);

  if (frame_pointer_needed)
    {
      gcc_assert (frame->reg_offset[HARD_FRAME_POINTER_REGNUM] >= 0);
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
k1_expand_epilogue (void)
{
  struct k1_frame_info *frame = &cfun->machine->frame;
  HOST_WIDE_INT frame_size = frame->initial_sp_offset;
  rtx insn;

  if (frame_pointer_needed)
    {
      insn = emit_insn (
	gen_add3_insn (stack_pointer_rtx, hard_frame_pointer_rtx,
		       GEN_INT (-frame->hard_frame_pointer_offset)));

      /* Revert CFA reg to use SP with its initial offset */
      RTX_FRAME_RELATED_P (insn) = 1;
      add_reg_note (insn, REG_CFA_DEF_CFA,
		    gen_rtx_PLUS (DImode, stack_pointer_rtx,
				  GEN_INT (frame->initial_sp_offset)));

      /* Restore previous FP */
      rtx fp_mem = gen_rtx_MEM (
	DImode, plus_constant (
		  Pmode, stack_pointer_rtx,
		  cfun->machine->frame.reg_offset[HARD_FRAME_POINTER_REGNUM]));

      emit_move_insn (gen_rtx_REG (DImode, HARD_FRAME_POINTER_REGNUM), fp_mem);
    }

  k1_save_or_restore_callee_save_registers (1);

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

#ifndef GCC_K1_MPPA_LINUX
  if (tls_kind != TLS_MODEL_NONE)
    tls_kind = TLS_MODEL_LOCAL_EXEC;
#endif
  return tls_kind;
}

/* Return true if SYMBOL_REF X is thread local */
static bool
k1_tls_symbol_p (rtx x)
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

static enum k1_symbol_type
k1_classify_tls_symbol (rtx x)
{
  enum tls_model tls_kind = tls_symbolic_operand_type (x);
  switch (tls_kind)
    {
    case TLS_MODEL_LOCAL_EXEC:
      return SYMBOL_TLSLE;

      /* General TLS model only supported on Linux. ClusterOS and bare
	 only supports local exec. */
#ifdef GCC_K1_MPPA_LINUX
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

static enum k1_symbol_type
k1_classify_symbol (rtx x)
{
  if (k1_tls_symbol_p (x))
    return k1_classify_tls_symbol (x);

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

static GTY (()) rtx k1_tls_symbol;

/* Return an instruction sequence that calls __tls_get_addr.  SYM is
   the TLS symbol we are referencing and TYPE is the symbol type to use
   (either global dynamic or local dynamic).  RESULT is an RTX for the
   return value location.  */

static rtx_insn *
k1_call_tls_get_addr (rtx sym, rtx result, int unspec)
{
  rtx a0 = gen_rtx_REG (Pmode, K1C_ARGUMENT_POINTER_REGNO), func;
  rtx_insn *insn;

  if (!k1_tls_symbol)
    k1_tls_symbol = init_one_libfunc ("__tls_get_addr");
  func = gen_rtx_MEM (FUNCTION_MODE, k1_tls_symbol);

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
k1_legitimize_tls_reference (rtx x)
{
  rtx addr, pic_reg, eqv;
  rtx dest, tmp;

  if (reload_completed || reload_in_progress)
    return x;

  enum k1_symbol_type sty = k1_classify_symbol (x);

  switch (sty)
    {
    case SYMBOL_TLSLE:
      /* address is @tlsle(symbol)[$tp]
       */
      addr
	= gen_rtx_PLUS (Pmode, gen_rtx_REG (Pmode, K1C_LOCAL_POINTER_REGNO),
			gen_rtx_CONST (Pmode,
				       gen_rtx_UNSPEC (Pmode, gen_rtvec (1, x),
						       UNSPEC_TLS_LE)));
      return addr;

    case SYMBOL_TLSGD:
      /* Sequence is:
       * $r0 = @tlsgd(sym) + $got
       * addr = __tls_get_addr()
       */
      tmp = gen_rtx_REG (Pmode, K1C_ARGUMENT_POINTER_REGNO);
      dest = gen_reg_rtx (Pmode);
      emit_libcall_block (k1_call_tls_get_addr (x, tmp, UNSPEC_TLS_GD),
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

      return gen_rtx_PLUS (Pmode, gen_rtx_REG (Pmode, K1C_LOCAL_POINTER_REGNO),
			   dest);

    case SYMBOL_TLSLD:
      tmp = gen_rtx_REG (Pmode, K1C_ARGUMENT_POINTER_REGNO);
      dest = gen_reg_rtx (Pmode);

      /* Attach a unique REG_EQUIV, to allow the RTL optimizers to
	 share the LD result with other LD model accesses.  */
      eqv
	= gen_rtx_UNSPEC (Pmode, gen_rtvec (1, const0_rtx), UNSPEC_TLS_LD_CALL);

      emit_libcall_block (k1_call_tls_get_addr (x, tmp, UNSPEC_TLS_LD),
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
k1_legitimate_pic_symbolic_ref_p (rtx op)
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
k1_legitimate_pic_operand_p (rtx x)
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
k1_expand_mov_constant (rtx operands[])
{
  rtx dest = operands[0];
  rtx src = operands[1];
  rtx new_rtx;

  if (GET_CODE (src) == SYMBOL_REF || GET_CODE (src) == LABEL_REF
      || GET_CODE (src) == CONST)
    {
      rtx base, offset;
      enum k1_symbol_type sty;
      rtx pic_reg;

      /* If we have (const (plus symbol offset)), separate out the offset
	 before we start classifying the symbol.  */
      split_const (src, &base, &offset);

      sty = k1_classify_symbol (base);
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
	  operands[1] = k1_legitimize_tls_reference (base);
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

  /* if (k1_has_tls_reference (operands[1])) */
  /*   { */
  /*     rtx src = operands[1]; */
  /*     operands[1] = k1_legitimize_tls_reference (src); */
  /*     gcc_assert (operands[1] != src); */
  /*   } else if (flag_pic) */
  /*   { */
  /*     if (SYMBOLIC_CONST(operands[1])) */
  /* 	operands[1] = k1_legitimize_pic_address (operands[1],
   * operands[0]); */
  /*   } */
  return;
}

/* FIXME AUTO: fix cost function for coolidge */
/* See T7748 */
static int
k1_register_move_cost (machine_mode mode,
		      reg_class_t from ATTRIBUTE_UNUSED,
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
k1_canonicalize_comparison (int *code, rtx *op0, rtx *op1,
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

/* Emit the compare insn and return the register rtx. */
static rtx
k1_gen_compare_reg (RTX_CODE code, rtx cmp)
{
  rtx x = XEXP (cmp, 0);
  rtx y = XEXP (cmp, 1);
  machine_mode mode = GET_MODE (x);
  bool float_compare_p = (GET_MODE_CLASS (mode) == MODE_FLOAT);

  if (float_compare_p)
    {
      if (! float_comparison_operator (cmp, VOIDmode))
	{
	  RTX_CODE swapped = swap_condition (code);

	  if (swapped == code)
	      /* CODE must be ORDERED or UNORDERED, bail out. */
	      return NULL_RTX;

	  code = swapped;
	  rtx t = force_reg (mode, y);
	  y = x, x = t;
	}

      mode = DImode;
    }

  if (CONSTANT_P (y))
    y = force_reg (GET_MODE (x), y);

  rtx cc_cmp = gen_rtx_fmt_ee (code, mode, x, y);
  gcc_assert (! float_compare_p ||
	      float_comparison_operator (cc_cmp, VOIDmode));

  rtx cc_reg = gen_reg_rtx (mode);
  emit_insn (gen_rtx_SET (cc_reg, cc_cmp));
  return cc_reg;
}

/* Emit code to conditionally move either OPERANDS[2] or OPERANDS[3] into
   OPERANDS[0] in MODE.  OPERANDS[1] is the operator of the condition. */
bool
k1_expand_conditional_move (machine_mode mode, rtx operands[])
{
  rtx cmp = operands[1];
  enum rtx_code cmp_code = GET_CODE (cmp);
  machine_mode cmp_mode = GET_MODE (XEXP (cmp, 0));
  rtx cc_reg = NULL_RTX, src = NULL_RTX, dst = NULL_RTX;

  if (! rtx_equal_p (operands[2], operands[0])
      && ! rtx_equal_p (operands[3], operands[0]))
    {
      if (reg_overlap_mentioned_p (operands[0], cmp))
	dst = gen_reg_rtx (mode);
      else
	dst = operands[0];

      src = operands[2];
      emit_move_insn (dst, operands[3]);
    }
  else if (rtx_equal_p (operands[2], operands[0]))
    {
      src = operands[3];
      dst = operands[0];

      if (GET_MODE_CLASS (cmp_mode) == MODE_FLOAT)
	cmp_code = reverse_condition_maybe_unordered (cmp_code);
      else
	cmp_code = reverse_condition (cmp_code);
    }
  else
    {
      src = operands[2];
      dst = operands[0];
    }

  if (XEXP (cmp, 1) == const0_rtx && (cmp_mode == DImode || cmp_mode == SImode))
    cc_reg = force_reg (cmp_mode, XEXP (cmp, 0));
  else
    {
      cc_reg = k1_gen_compare_reg (cmp_code, cmp);

      if (! cc_reg)
	return false;

      cmp_code = NE;
    }

  rtx cmp0 = gen_rtx_fmt_ee (cmp_code, VOIDmode, cc_reg, const0_rtx);

  emit_insn (gen_rtx_SET (dst, gen_rtx_IF_THEN_ELSE (mode, cmp0, src, dst)));

  if (dst != operands[0])
    emit_move_insn (operands[0], dst);

  return true;
}

/* Expander for vector cmove with the pattern:

   [(set (match_operand:V2SI 0 "register_operand" "")
   (if_then_else:V2SI
   (match_operator 3 ""
   [(match_operand:V2SI 4 "nonimmediate_operand" "")
   (match_operand:V2SI 5 "nonimmediate_operand" "")])
   (match_operand:V2SI 1 "general_operand" "")
   (match_operand:V2SI 2 "general_operand" "")))] */

/* FIXME AUTO: disabling vector support */
/* void */
/* k1_expand_vcondv2si (rtx *operands) */
/* { */
/*     bool invert = false; */

/*     if (operands[2] != operands[0]) { */
/*         if (operands[1] != operands[0]) { */
/*             emit_move_insn (operands[0], operands[2]); */
/*             operands[2] = operands[0]; */
/*         } else { */
/*             invert = true; */
/*         } */
/*     } */

/*     if (operands[5] != CONST0_RTX (V2SImode)) { */
/*         rtx reg = gen_reg_rtx (V2SImode); */
/*         emit_insn (gen_cstorev2si4 (reg, operands[3], operands[4],
 * operands[5])); */

/*         PUT_CODE (operands[3], NE); */
/*         operands[4] = reg; */
/*         operands[5] = CONST0_RTX (V2SImode); */
/*     } */

/*     if (invert) { */
/*         rtx tmp; */

/*         PUT_CODE (operands[3], reverse_condition (GET_CODE (operands[3])));
 */
/*         tmp = operands[1]; */
/*         operands[1] = operands[2]; */
/*         operands[2] = tmp; */
/*     } */

/*     PUT_MODE (operands[3], SImode); */
/*     emit_insn (gen_cmovesi (gen_lowpart (SImode, operands[0]), */
/*                             operands[3], */
/*                             gen_lowpart (SImode, operands[4]), */
/*                             gen_lowpart (SImode, operands[1]), */
/*                             gen_lowpart (SImode, operands[2]))); */
/*     emit_insn (gen_cmovesi (gen_highpart (SImode, operands[0]), */
/*                             operands[3], */
/*                             gen_highpart (SImode, operands[4]), */
/*                             gen_highpart (SImode, operands[1]), */
/*                             gen_highpart (SImode, operands[2]))); */
/* } */

/* Expander for vector cmove with the pattern:

   [(set (match_operand:V4HI 0 "register_operand" "")
   (if_then_else:V4HI
   (match_operator 3 ""
   [(match_operand:V4HI 4 "nonimmediate_operand" "")
   (match_operand:V4HI 5 "nonimmediate_operand" "")])
   (match_operand:V4HI 1 "general_operand" "")
   (match_operand:V4HI 2 "general_operand" "")))] */

/* FIXME AUTO: disabling vector support */
/* void */
/* k1_expand_vcondv4hi (rtx *operands) */
/* { */
/*     bool invert = false; */

/*     if (operands[2] != operands[0]) { */
/*         if (operands[1] != operands[0]) { */
/*             emit_move_insn (operands[0], operands[2]); */
/*             operands[2] = operands[0]; */
/*         } else { */
/*             invert = true; */
/*         } */
/*     } */

/*     if (operands[5] != CONST0_RTX (V4HImode)) { */
/*         rtx reg = gen_reg_rtx (V4HImode); */
/*         emit_insn (gen_cstorev4hi4 (reg, operands[3], operands[4],
 * operands[5])); */

/*         PUT_CODE (operands[3], NE); */
/*         operands[4] = reg; */
/*         operands[5] = CONST0_RTX (V4HImode); */
/*     } */

/*     if (invert) { */
/*         rtx tmp; */

/*         PUT_CODE (operands[3], reverse_condition (GET_CODE (operands[3])));
 */
/*         tmp = operands[1]; */
/*         operands[1] = operands[2]; */
/*         operands[2] = tmp; */
/*     } */

/*     PUT_MODE (operands[3], V2HImode); */
/*     emit_insn (gen_cmovev2hi (gen_lowpart (V2HImode, operands[0]), */
/*                               operands[3], */
/*                               gen_lowpart (V2HImode, operands[4]), */
/*                               gen_lowpart (V2HImode, operands[1]), */
/*                               gen_lowpart (V2HImode, operands[2]))); */
/*     emit_insn (gen_cmovev2hi (gen_highpart (V2HImode, operands[0]), */
/*                               operands[3], */
/*                               gen_highpart (V2HImode, operands[4]), */
/*                               gen_highpart (V2HImode, operands[1]), */
/*                               gen_highpart (V2HImode, operands[2]))); */
/* } */

/* Emit a barrier, that is appropriate for memory model MODEL, at the
   start of a sequence implementing an atomic operation. We always use
   a very conservative memory model since K1C has a relaxed memory
   consistency model meaning that all loads and stores are scheduled
   out-of-order at different memory addresses. Only simple load/store
   operations are performed with more usual memory constraints (if
   MOVE is true). */

void
k1_emit_pre_barrier (rtx model, bool move)
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
   k1_emit_pre_barrier () for MOVE. */

void
k1_emit_post_barrier (rtx model, bool move)
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
k1_expand_compare_and_swap (rtx operands[])
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
   DImode (cas loop is too difficult to handle for HI, QI, and TI
   modes). Others modes will be expanded by libatomic if enabled
   anyway. MEM is the memory location where to perform the atomic
   operation with value in VAL. If AFTER is true then store the value
   from MEM into TARGET holds after the operation, if AFTER is false
   then store the value from MEM into TARGET holds before the
   operation. If TARGET is NULL_RTX then discard that value, else
   store the result to TARGET. */

void
k1_expand_atomic_op (enum rtx_code code, rtx target, bool after, rtx mem,
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

  k1_emit_pre_barrier (model, false);

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

  k1_emit_post_barrier (model, false);
}

/* Expand the atomic test-and-set on byte atomic operation using the
   acswapw insn. */

void
k1_expand_atomic_test_and_set (rtx operands[])
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

  k1_emit_pre_barrier (model, false);

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

  k1_emit_post_barrier (model, false);
}

enum k1_builtin
{
  K1_BUILTIN_ABDW,
  K1_BUILTIN_ABDD,
  K1_BUILTIN_ADDSW,
  K1_BUILTIN_ADDSD,
  K1_BUILTIN_SBFSW,
  K1_BUILTIN_SBFSD,
  K1_BUILTIN_AVGW,
  K1_BUILTIN_AVGUW,
  K1_BUILTIN_AVGRW,
  K1_BUILTIN_AVGRUW,
  K1_BUILTIN_AWAIT,
  K1_BUILTIN_BARRIER,
  K1_BUILTIN_CBSW,
  K1_BUILTIN_CBSD,
  K1_BUILTIN_CLZW,
  K1_BUILTIN_CLZD,
  K1_BUILTIN_CTZW,
  K1_BUILTIN_CTZD,
  K1_BUILTIN_ACSWAPW,
  K1_BUILTIN_ACSWAPD,
  K1_BUILTIN_AFADDD,
  K1_BUILTIN_AFADDW,
  K1_BUILTIN_ALCLRD,
  K1_BUILTIN_ALCLRW,
  K1_BUILTIN_DFLUSH,
  K1_BUILTIN_DFLUSHL,
  K1_BUILTIN_DINVAL,
  K1_BUILTIN_DINVALL,
  K1_BUILTIN_DOZE,
  K1_BUILTIN_DPURGE,
  K1_BUILTIN_DPURGEL,
  K1_BUILTIN_DTOUCHL,
  K1_BUILTIN_DZEROL,
  K1_BUILTIN_EXTFZ,

  K1_BUILTIN_FABSW,
  K1_BUILTIN_FABSWP,
  K1_BUILTIN_FABSWQ,
  K1_BUILTIN_FABSD,
  K1_BUILTIN_FABSDP,
  K1_BUILTIN_FNEGW,
  K1_BUILTIN_FNEGWP,
  K1_BUILTIN_FNEGWQ,
  K1_BUILTIN_FNEGD,
  K1_BUILTIN_FNEGDP,
  K1_BUILTIN_FMAXW,
  K1_BUILTIN_FMAXWP,
  K1_BUILTIN_FMAXWQ,
  K1_BUILTIN_FMAXD,
  K1_BUILTIN_FMAXDP,
  K1_BUILTIN_FMINW,
  K1_BUILTIN_FMINWP,
  K1_BUILTIN_FMINWQ,
  K1_BUILTIN_FMIND,
  K1_BUILTIN_FMINDP,
  K1_BUILTIN_FINVW,
  K1_BUILTIN_FISRW,
  K1_BUILTIN_FADDW,
  K1_BUILTIN_FADDWP,
  K1_BUILTIN_FADDWQ,
  K1_BUILTIN_FADDD,
  K1_BUILTIN_FADDDP,
  K1_BUILTIN_FADDCWC,
  K1_BUILTIN_FADDCWCP,
  K1_BUILTIN_FADDCDC,
  K1_BUILTIN_FSBFW,
  K1_BUILTIN_FSBFWP,
  K1_BUILTIN_FSBFWQ,
  K1_BUILTIN_FSBFD,
  K1_BUILTIN_FSBFDP,
  K1_BUILTIN_FSBFCWC,
  K1_BUILTIN_FSBFCWCP,
  K1_BUILTIN_FSBFCDC,
  K1_BUILTIN_FMULW,
  K1_BUILTIN_FMULWP,
  K1_BUILTIN_FMULWQ,
  K1_BUILTIN_FMULD,
  K1_BUILTIN_FMULDP,
  K1_BUILTIN_FMULWD,
  K1_BUILTIN_FMULWC,
  K1_BUILTIN_FMULWCP,
  K1_BUILTIN_FMULCWC,
  K1_BUILTIN_FMULCWCP,
  K1_BUILTIN_FMULDC,
  K1_BUILTIN_FMULCDC,
  K1_BUILTIN_FMM2WQ,
  K1_BUILTIN_FFMAW,
  K1_BUILTIN_FFMAWP,
  K1_BUILTIN_FFMAWQ,
  K1_BUILTIN_FFMAD,
  K1_BUILTIN_FFMADP,
  K1_BUILTIN_FFMAWD,
  K1_BUILTIN_FMM2AWQ,
  K1_BUILTIN_FFMSW,
  K1_BUILTIN_FFMSWP,
  K1_BUILTIN_FFMSWQ,
  K1_BUILTIN_FFMSD,
  K1_BUILTIN_FFMSDP,
  K1_BUILTIN_FFMSWD,
  K1_BUILTIN_FMM2SWQ,
  K1_BUILTIN_FLOATW,
  K1_BUILTIN_FLOATWP,
  K1_BUILTIN_FLOATWQ,
  K1_BUILTIN_FLOATD,
  K1_BUILTIN_FLOATDP,
  K1_BUILTIN_FLOATUW,
  K1_BUILTIN_FLOATUWP,
  K1_BUILTIN_FLOATUWQ,
  K1_BUILTIN_FLOATUD,
  K1_BUILTIN_FLOATUDP,
  K1_BUILTIN_FIXEDW,
  K1_BUILTIN_FIXEDWP,
  K1_BUILTIN_FIXEDWQ,
  K1_BUILTIN_FIXEDD,
  K1_BUILTIN_FIXEDDP,
  K1_BUILTIN_FIXEDUW,
  K1_BUILTIN_FIXEDUWP,
  K1_BUILTIN_FIXEDUWQ,
  K1_BUILTIN_FIXEDUD,
  K1_BUILTIN_FIXEDUDP,

  K1_BUILTIN_FCDIVW,
  K1_BUILTIN_FCDIVD,
  K1_BUILTIN_FENCE,
  K1_BUILTIN_FMULRNWP,
  K1_BUILTIN_FSBFWWP,
  K1_BUILTIN_FSDIVW,
  K1_BUILTIN_FSDIVD,
  K1_BUILTIN_FSINVW,
  K1_BUILTIN_FSINVD,
  K1_BUILTIN_FSISRW,
  K1_BUILTIN_FSISRD,
  K1_BUILTIN_GET,
  K1_BUILTIN_WFXL,
  K1_BUILTIN_WFXM,
  K1_BUILTIN_IINVAL,
  K1_BUILTIN_IINVALS,
  K1_BUILTIN_LANDHP,
  K1_BUILTIN_LBSU,
  K1_BUILTIN_LBZU,
  K1_BUILTIN_LHSU,
  K1_BUILTIN_LHZU,
  K1_BUILTIN_LDU,
  K1_BUILTIN_LWZU,
  K1_BUILTIN_MADUUCIWD,
  K1_BUILTIN_SBMM8,
  K1_BUILTIN_SBMMT8,
  K1_BUILTIN_SCALL,
  K1_BUILTIN_SET,
  K1_BUILTIN_SLEEP,
  K1_BUILTIN_STOP,
  K1_BUILTIN_STSUW,
  K1_BUILTIN_STSUD,
  K1_BUILTIN_SYNCGROUP,
  K1_BUILTIN_TOUCHL,
  K1_BUILTIN_TLBDINVAL,
  K1_BUILTIN_TLBIINVAL,
  K1_BUILTIN_TLBPROBE,
  K1_BUILTIN_TLBREAD,
  K1_BUILTIN_TLBWRITE,
  K1_BUILTIN_SATD,
  K1_BUILTIN_SATUD,
  K1_BUILTIN_FWIDENLHW,
  K1_BUILTIN_FWIDENLHWP,
  K1_BUILTIN_FWIDENMHW,
  K1_BUILTIN_FWIDENMHWP,
  K1_BUILTIN_FNARROWWH,
  K1_BUILTIN_FNARROWDWP,
  K1_BUILTIN_WAITIT,
  K1_LAST_BUILTIN
};

static tree builtin_fndecls[K1_LAST_BUILTIN];

static void
k1_init_builtins (void)
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

#define ADD_K1_BUILTIN_VARAGS(UC_NAME, LC_NAME, ...)                           \
  builtin_fndecls[K1_BUILTIN_##UC_NAME]                                        \
    = add_builtin_function ("__builtin_k1_" LC_NAME,                           \
			    build_varargs_function_type_list (__VA_ARGS__,     \
							      NULL_TREE),      \
			    K1_BUILTIN_##UC_NAME, BUILT_IN_MD, NULL,           \
			    NULL_TREE)

#define ADD_K1_BUILTIN(UC_NAME, LC_NAME, ...)                                  \
  builtin_fndecls[K1_BUILTIN_##UC_NAME]                                        \
    = add_builtin_function ("__builtin_k1_" LC_NAME,                           \
			    build_function_type_list (__VA_ARGS__, NULL_TREE), \
			    K1_BUILTIN_##UC_NAME, BUILT_IN_MD, NULL,           \
			    NULL_TREE)

  ADD_K1_BUILTIN (ABDW, "abdw", INT32, INT32, INT32); // Scalar
  ADD_K1_BUILTIN (ABDD, "abdd", INT64, INT64, INT64); // Scalar
  ADD_K1_BUILTIN (ADDSW, "addsw", INT32, INT32, INT32); // Scalar
  ADD_K1_BUILTIN (ADDSD, "addsd", INT64, INT64, INT64); // Scalar
  ADD_K1_BUILTIN (SBFSW, "sbfsw", INT32, INT32, INT32); // Scalar
  ADD_K1_BUILTIN (SBFSD, "sbfsd", INT64, INT64, INT64); // Scalar
  ADD_K1_BUILTIN (AVGW, "avgw", INT32, INT32, INT32); // Scalar
  ADD_K1_BUILTIN (AVGUW, "avguw", UINT32, UINT32, UINT32); // Scalar
  ADD_K1_BUILTIN (AVGRW, "avgrw", INT32, INT32, INT32); // Scalar
  ADD_K1_BUILTIN (AVGRUW, "avgruw", UINT32, UINT32, UINT32); // Scalar
  ADD_K1_BUILTIN (AWAIT, "await", VOID); // Control
  ADD_K1_BUILTIN (BARRIER, "barrier", VOID); // Control
  ADD_K1_BUILTIN (CBSW, "cbsw", INT32, UINT32); // Scalar
  ADD_K1_BUILTIN (CBSD, "cbsd", INT64, UINT64); // Scalar
  ADD_K1_BUILTIN (CLZW, "clzw", INT32, UINT32); // Scalar
  ADD_K1_BUILTIN (CLZD, "clzd", INT64, UINT64); // Scalar
  ADD_K1_BUILTIN (CTZW, "ctzw", INT32, UINT32); // Scalar
  ADD_K1_BUILTIN (CTZD, "ctzd", INT64, UINT64); // Scalar
  ADD_K1_BUILTIN (ACSWAPW, "acswapw", UINT32, VPTR, UINT32, UINT32); // Atomic
  ADD_K1_BUILTIN (ACSWAPD, "acswapd", UINT64, VPTR, UINT64, UINT64); // Atomic
  ADD_K1_BUILTIN (AFADDD, "afaddd", UINT64, VPTR, INT64); // Atomic
  ADD_K1_BUILTIN (AFADDW, "afaddw", UINT32, VPTR, INT32); // Atomic
  ADD_K1_BUILTIN (ALCLRD, "alclrd", UINT64, VPTR); // Atomic
  ADD_K1_BUILTIN (ALCLRW, "alclrw", UINT32, VPTR); // Atomic
  ADD_K1_BUILTIN (DINVAL, "dinval", VOID); // Memory
  ADD_K1_BUILTIN (DINVALL, "dinvall", VOID, CVPTR); // Memory
  ADD_K1_BUILTIN (DOZE, "doze", VOID); // Control
  ADD_K1_BUILTIN (DTOUCHL, "dtouchl", VOID, CVPTR); // Memory
  ADD_K1_BUILTIN (DZEROL, "dzerol", VOID, VPTR); // Memory
  ADD_K1_BUILTIN (EXTFZ, "extfz", UINT32, UINT32, UINT32, UINT32); // Scalar

  ADD_K1_BUILTIN (FABSW, "fabsw", FLOAT32, FLOAT32); // Scalar
  ADD_K1_BUILTIN (FABSWP, "fabswp", FLOAT32X2, FLOAT32X2); // Vector
  ADD_K1_BUILTIN (FABSWQ, "fabswq", FLOAT32X4, FLOAT32X4); // Vector
  ADD_K1_BUILTIN (FABSD, "fabsd", FLOAT64, FLOAT64); // Scalar
  ADD_K1_BUILTIN (FABSDP, "fabsdp", FLOAT64X2, FLOAT64X2); // Vector
  ADD_K1_BUILTIN (FNEGW, "fnegw", FLOAT32, FLOAT32); // Scalar
  ADD_K1_BUILTIN (FNEGWP, "fnegwp", FLOAT32X2, FLOAT32X2); // Vector
  ADD_K1_BUILTIN (FNEGWQ, "fnegwq", FLOAT32X4, FLOAT32X4); // Vector
  ADD_K1_BUILTIN (FNEGD, "fnegd", FLOAT64, FLOAT64); // Scalar
  ADD_K1_BUILTIN (FNEGDP, "fnegdp", FLOAT64X2, FLOAT64X2); // Vector
  ADD_K1_BUILTIN (FMAXW, "fmaxw", FLOAT32, FLOAT32, FLOAT32); // Scalar
  ADD_K1_BUILTIN (FMAXWP, "fmaxwp", FLOAT32X2, FLOAT32X2, FLOAT32X2); // Vector
  ADD_K1_BUILTIN (FMAXWQ, "fmaxwq", FLOAT32X4, FLOAT32X4, FLOAT32X4); // Vector
  ADD_K1_BUILTIN (FMAXD, "fmaxd", FLOAT64, FLOAT64, FLOAT64); // Scalar
  ADD_K1_BUILTIN (FMAXDP, "fmaxdp", FLOAT64X2, FLOAT64X2, FLOAT64X2); // Vector
  ADD_K1_BUILTIN (FMINW, "fminw", FLOAT32, FLOAT32, FLOAT32); // Scalar
  ADD_K1_BUILTIN (FMINWP, "fminwp", FLOAT32X2, FLOAT32X2, FLOAT32X2); // Vector
  ADD_K1_BUILTIN (FMINWQ, "fminwq", FLOAT32X4, FLOAT32X4, FLOAT32X4); // Vector
  ADD_K1_BUILTIN (FMIND, "fmind", FLOAT64, FLOAT64, FLOAT64); // Scalar
  ADD_K1_BUILTIN (FMINDP, "fmindp", FLOAT64X2, FLOAT64X2, FLOAT64X2); // Vector
  ADD_K1_BUILTIN (FINVW, "finvw", FLOAT32, FLOAT32, STRING); // Scalar
  ADD_K1_BUILTIN (FISRW, "fisrw", FLOAT32, FLOAT32, STRING); // Scalar
  ADD_K1_BUILTIN (FADDW, "faddw", FLOAT32, FLOAT32, FLOAT32, STRING); // Scalar
  ADD_K1_BUILTIN (FADDWP, "faddwp", FLOAT32X2, FLOAT32X2, FLOAT32X2, STRING); // Vector
  ADD_K1_BUILTIN (FADDWQ, "faddwq", FLOAT32X4, FLOAT32X4, FLOAT32X4, STRING); // Vector
  ADD_K1_BUILTIN (FADDD, "faddd", FLOAT64, FLOAT64, FLOAT64, STRING); // Scalar
  ADD_K1_BUILTIN (FADDDP, "fadddp", FLOAT64X2, FLOAT64X2, FLOAT64X2, STRING); // Vector
  ADD_K1_BUILTIN (FADDCWC, "faddcwc", FLOAT32X2, FLOAT32X2, FLOAT32X2, STRING); // Vector
  ADD_K1_BUILTIN (FADDCWCP, "faddcwcp", FLOAT32X4, FLOAT32X4, FLOAT32X4, STRING); // Vector
  ADD_K1_BUILTIN (FADDCDC, "faddcdc", FLOAT64X2, FLOAT64X2, FLOAT64X2, STRING); // Vector
  ADD_K1_BUILTIN (FSBFW, "fsbfw", FLOAT32, FLOAT32, FLOAT32, STRING); // Scalar
  ADD_K1_BUILTIN (FSBFWP, "fsbfwp", FLOAT32X2, FLOAT32X2, FLOAT32X2, STRING); // Vector
  ADD_K1_BUILTIN (FSBFWQ, "fsbfwq", FLOAT32X4, FLOAT32X4, FLOAT32X4, STRING); // Vector
  ADD_K1_BUILTIN (FSBFD, "fsbfd", FLOAT64, FLOAT64, FLOAT64, STRING); // Scalar
  ADD_K1_BUILTIN (FSBFDP, "fsbfdp", FLOAT64X2, FLOAT64X2, FLOAT64X2, STRING); // Vector
  ADD_K1_BUILTIN (FSBFCWC, "fsbfcwc", FLOAT32X2, FLOAT32X2, FLOAT32X2, STRING); // Vector
  ADD_K1_BUILTIN (FSBFCWCP, "fsbfcwcp", FLOAT32X4, FLOAT32X4, FLOAT32X4, STRING); // Vector
  ADD_K1_BUILTIN (FSBFCDC, "fsbfcdc", FLOAT64X2, FLOAT64X2, FLOAT64X2, STRING); // Vector
  ADD_K1_BUILTIN (FMULW, "fmulw", FLOAT32, FLOAT32, FLOAT32, STRING); // Scalar
  ADD_K1_BUILTIN (FMULWP, "fmulwp", FLOAT32X2, FLOAT32X2, FLOAT32X2, STRING); // Vector
  ADD_K1_BUILTIN (FMULWQ, "fmulwq", FLOAT32X4, FLOAT32X4, FLOAT32X4, STRING); // Vector
  ADD_K1_BUILTIN (FMULD, "fmuld", FLOAT64, FLOAT64, FLOAT64, STRING); // Scalar
  ADD_K1_BUILTIN (FMULDP, "fmuldp", FLOAT64X2, FLOAT64X2, FLOAT64X2, STRING); // Vector
  ADD_K1_BUILTIN (FMULWD, "fmulwd", FLOAT64, FLOAT32, FLOAT32, STRING); // Scalar
  ADD_K1_BUILTIN (FMULWC, "fmulwc", FLOAT32X2, FLOAT32X2, FLOAT32X2, STRING); // Vector
  ADD_K1_BUILTIN (FMULWCP, "fmulwcp", FLOAT32X4, FLOAT32X4, FLOAT32X4, STRING); // Vector
  ADD_K1_BUILTIN (FMULCWC, "fmulcwc", FLOAT32X2, FLOAT32X2, FLOAT32X2, STRING); // Vector
  ADD_K1_BUILTIN (FMULCWCP, "fmulcwcp", FLOAT32X4, FLOAT32X4, FLOAT32X4, STRING); // Vector
  ADD_K1_BUILTIN (FMULDC, "fmuldc", FLOAT64X2, FLOAT64X2, FLOAT64X2, STRING); // Vector
  ADD_K1_BUILTIN (FMULCDC, "fmulcdc", FLOAT64X2, FLOAT64X2, FLOAT64X2, STRING); // Vector
  ADD_K1_BUILTIN (FMM2WQ, "fmm2wq", FLOAT32X4, FLOAT32X2, FLOAT32X2, STRING); // Vector
  ADD_K1_BUILTIN (FFMAW, "ffmaw", FLOAT32, FLOAT32, FLOAT32, FLOAT32, STRING); // Scalar
  ADD_K1_BUILTIN (FFMAWP, "ffmawp", FLOAT32X2, FLOAT32X2, FLOAT32X2, FLOAT32X2, STRING); // Vector
  ADD_K1_BUILTIN (FFMAWQ, "ffmawq", FLOAT32X4, FLOAT32X4, FLOAT32X4, FLOAT32X4, STRING); // Vector
  ADD_K1_BUILTIN (FFMAD, "ffmad", FLOAT64, FLOAT64, FLOAT64, FLOAT64, STRING); // Scalar
  ADD_K1_BUILTIN (FFMADP, "ffmadp", FLOAT64X2, FLOAT64X2, FLOAT64X2, FLOAT64X2, STRING); // Vector
  ADD_K1_BUILTIN (FFMAWD, "ffmawd", FLOAT64, FLOAT32, FLOAT32, FLOAT64, STRING); // Scalar
  ADD_K1_BUILTIN (FMM2AWQ, "fmm2awq", FLOAT32X4, FLOAT32X2, FLOAT32X2, FLOAT32X4, STRING); // Vector
  ADD_K1_BUILTIN (FFMSW, "ffmsw", FLOAT32, FLOAT32, FLOAT32, FLOAT32, STRING); // Scalar
  ADD_K1_BUILTIN (FFMSWP, "ffmswp", FLOAT32X2, FLOAT32X2, FLOAT32X2, FLOAT32X2, STRING); // Vector
  ADD_K1_BUILTIN (FFMSWQ, "ffmswq", FLOAT32X4, FLOAT32X4, FLOAT32X4, FLOAT32X4, STRING); // Vector
  ADD_K1_BUILTIN (FFMSD, "ffmsd", FLOAT64, FLOAT64, FLOAT64, FLOAT64, STRING); // Scalar
  ADD_K1_BUILTIN (FFMSDP, "ffmsdp", FLOAT64X2, FLOAT64X2, FLOAT64X2, FLOAT64X2, STRING); // Vector
  ADD_K1_BUILTIN (FFMSWD, "ffmswd", FLOAT64, FLOAT32, FLOAT32, FLOAT64, STRING); // Scalar
  ADD_K1_BUILTIN (FMM2SWQ, "fmm2swq", FLOAT32X4, FLOAT32X2, FLOAT32X2, FLOAT32X4, STRING); // Vector
  ADD_K1_BUILTIN (FLOATW, "floatw", FLOAT32, INT32, UINT8, STRING); // Scalar
  ADD_K1_BUILTIN (FLOATWP, "floatwp", FLOAT32X2, INT32X2, UINT8, STRING); // Vector
  ADD_K1_BUILTIN (FLOATWQ, "floatwq", FLOAT32X4, INT32X4, UINT8, STRING); // Vector
  ADD_K1_BUILTIN (FLOATD, "floatd", FLOAT64, INT64, UINT8, STRING); // Scalar
  ADD_K1_BUILTIN (FLOATDP, "floatdp", FLOAT64X2, INT64X2, UINT8, STRING); // Vector
  ADD_K1_BUILTIN (FLOATUW, "floatuw", FLOAT32, UINT32, UINT8, STRING); // Scalar
  ADD_K1_BUILTIN (FLOATUWP, "floatuwp", FLOAT32X2, UINT32X2, UINT8, STRING); // Vector
  ADD_K1_BUILTIN (FLOATUWQ, "floatuwq", FLOAT32X4, UINT32X4, UINT8, STRING); // Vector
  ADD_K1_BUILTIN (FLOATUD, "floatud", FLOAT64, UINT64, UINT8, STRING); // Scalar
  ADD_K1_BUILTIN (FLOATUDP, "floatudp", FLOAT64X2, UINT64X2, UINT8, STRING); // Vector
  ADD_K1_BUILTIN (FIXEDW, "fixedw", INT32, FLOAT32, UINT8, STRING); // Scalar
  ADD_K1_BUILTIN (FIXEDWP, "fixedwp", INT32X2, FLOAT32X2, UINT8, STRING); // Vector
  ADD_K1_BUILTIN (FIXEDWQ, "fixedwq", INT32X4, FLOAT32X4, UINT8, STRING); // Vector
  ADD_K1_BUILTIN (FIXEDD, "fixedd", INT64, FLOAT64, UINT8, STRING); // Scalar
  ADD_K1_BUILTIN (FIXEDDP, "fixeddp", INT64X2, FLOAT64X2, UINT8, STRING); // Vector
  ADD_K1_BUILTIN (FIXEDUW, "fixeduw", UINT32, FLOAT32, UINT8, STRING); // Scalar
  ADD_K1_BUILTIN (FIXEDUWP, "fixeduwp", UINT32X2, FLOAT32X2, UINT8, STRING); // Vector
  ADD_K1_BUILTIN (FIXEDUWQ, "fixeduwq", UINT32X4, FLOAT32X4, UINT8, STRING); // Vector
  ADD_K1_BUILTIN (FIXEDUD, "fixedud", UINT64, FLOAT64, UINT8, STRING); // Scalar
  ADD_K1_BUILTIN (FIXEDUDP, "fixedudp", UINT64X2, FLOAT64X2, UINT8, STRING); // Vector

  ADD_K1_BUILTIN (FCDIVW, "fcdivw", FLOAT32, FLOAT32, FLOAT32); // Scalar
  ADD_K1_BUILTIN (FCDIVD, "fcdivd", FLOAT64, FLOAT64, FLOAT64); // Scalar
  ADD_K1_BUILTIN (FENCE, "fence", VOID); // Control

  ADD_K1_BUILTIN (FSDIVW, "fsdivw", FLOAT32, FLOAT32, FLOAT32); // Scalar
  ADD_K1_BUILTIN (FSDIVD, "fsdivd", FLOAT64, FLOAT64, FLOAT64); // Scalar
  ADD_K1_BUILTIN (FSINVW, "fsinvw", FLOAT32, FLOAT32); // Scalar
  ADD_K1_BUILTIN (FSINVD, "fsinvd", FLOAT64, FLOAT64); // Scalar
  ADD_K1_BUILTIN (FSISRW, "fsisrw", FLOAT32, FLOAT32); // Scalar
  ADD_K1_BUILTIN (FSISRD, "fsisrd", FLOAT64, FLOAT64); // Scalar
  ADD_K1_BUILTIN (GET, "get", UINT64, INT32); // Control
  ADD_K1_BUILTIN (WFXL, "wfxl", VOID, UINT8, UINT64); // Control
  ADD_K1_BUILTIN (WFXM, "wfxm", VOID, UINT8, UINT64); // Control
  ADD_K1_BUILTIN (IINVAL, "iinval", VOID); // Memory
  ADD_K1_BUILTIN (IINVALS, "iinvals", VOID, CVPTR); // Memory
  ADD_K1_BUILTIN (LBSU, "lbsu", INT8, CVPTR); // Memory
  ADD_K1_BUILTIN (LBZU, "lbzu", UINT8, CVPTR); // Memory
  ADD_K1_BUILTIN (LHSU, "lhsu", INT16, CVPTR); // Memory
  ADD_K1_BUILTIN (LHZU, "lhzu", UINT16, CVPTR); // Memory
  ADD_K1_BUILTIN (LDU, "ldu", UINT64, CVPTR); // Memory
  ADD_K1_BUILTIN (LWZU, "lwzu", UINT32, CVPTR); // Memory
  ADD_K1_BUILTIN (SBMM8, "sbmm8", UINT64, UINT64, UINT64); // Scalar
  ADD_K1_BUILTIN (SBMMT8, "sbmmt8", UINT64, UINT64, UINT64); // Scalar
  ADD_K1_BUILTIN (SATD, "satd", INT64, INT64, UINT8); // Scalar
  ADD_K1_BUILTIN (SATUD, "satud", UINT64, INT64, UINT8); // Scalar
  ADD_K1_BUILTIN (SET, "set", VOID, INT32, UINT64); // Control

  ADD_K1_BUILTIN (SLEEP, "sleep", VOID); // Control
  ADD_K1_BUILTIN (STOP, "stop", VOID); // Control
  ADD_K1_BUILTIN (STSUW, "stsuw", UINT32, UINT32, UINT32); // Scalar
  ADD_K1_BUILTIN (STSUD, "stsud", UINT64, UINT64, UINT64); // Scalar
  ADD_K1_BUILTIN (SYNCGROUP, "syncgroup", VOID, UINT64); // Control
  ADD_K1_BUILTIN (TLBDINVAL, "tlbdinval", VOID); // Memory
  ADD_K1_BUILTIN (TLBIINVAL, "tlbiinval", VOID); // Memory
  ADD_K1_BUILTIN (TLBPROBE, "tlbprobe", VOID); // Memory
  ADD_K1_BUILTIN (TLBREAD, "tlbread", VOID); // Memory
  ADD_K1_BUILTIN (TLBWRITE, "tlbwrite", VOID); // Memory

  ADD_K1_BUILTIN (FWIDENLHW, "fwidenlhw", FLOAT32, UINT32); // Scalar
  ADD_K1_BUILTIN (FWIDENMHW, "fwidenmhw", FLOAT32, UINT32); // Scalar
  ADD_K1_BUILTIN (FNARROWWH, "fnarrowwh", UINT16, FLOAT32); // Scalar
  ADD_K1_BUILTIN (WAITIT, "waitit", UINT32, UINT32); // Control
}

static tree
k1_builtin_decl (unsigned code, bool initialize_p ATTRIBUTE_UNUSED)
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
  error ("__builtin_k1_%s modifier %s not recognized.", name, modifier);
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
  error ("__builtin_k1_%s modifier %s not recognized.", name, modifier);
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
  error ("__builtin_k1_%s modifier %s not recognized.", name, modifier);
  return 0;
}

static rtx
build_arf_reg_name_arg (rtx arg)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned regno = INTVAL (arg);
      if (regno < 64)
	{
	  static const char *arf_reg_names[] = {
	    "a0",  "a1",  "a2",	 "a3",	"a4",  "a5",  "a6",  "a7",
	    "a8",  "a9",  "a10", "a11", "a12", "a13", "a14", "a15",
	    "a16", "a17", "a18", "a19", "a20", "a21", "a22", "a23",
	    "a24", "a25", "a26", "a27", "a28", "a29", "a30", "a31",
	    "a32", "a33", "a34", "a35", "a36", "a37", "a38", "a39",
	    "a40", "a41", "a42", "a43", "a44", "a45", "a46", "a47",
	    "a48", "a49", "a50", "a51", "a52", "a53", "a54", "a55",
	    "a56", "a57", "a58", "a59", "a60", "a61", "a62", "a63",
	  };
	  return gen_rtx_CONST_STRING (VOIDmode, arf_reg_names[regno]);
	}
    }
  return 0;
}

static rtx
build_xrf_reg_name_arg (rtx arg)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned regno = INTVAL (arg);
      if (regno < 64)
	{
	  static const char *xrf_reg_names[] = {
	    "a0a1a2a3",	    "ERROR", "ERROR", "ERROR",
	    "a4a5a6a7",	    "ERROR", "ERROR", "ERROR",
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
	  return gen_rtx_CONST_STRING (VOIDmode, xrf_reg_names[regno]);
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
  error ("__builtin_k1_%s expects a boolean immediate in %s argument.", name,
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
  error ("__builtin_k1_%s expects a %d-bit signed immediate in %s argument.",
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
  error ("__builtin_k1_%s expects a %d-bit unsigned immediate in %s argument.",
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
  error ("__builtin_k1_%s expects a %d-bit signed or unsigned immediate in %s "
	 "argument.",
	 name, bits, where);
  return 0;
}

static rtx
k1_expand_builtin_get (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = verify_const_uint_arg (arg1, 9, "get", "first");
  int regno = INTVAL (arg1) + K1C_SFR_FIRST_REGNO;
  rtx sys_reg = gen_rtx_REG (DImode, regno);

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  if (regno == K1C_PCR_REGNO)
    emit_move_insn (target, sys_reg);
  else
    emit_insn (gen_k1_get (target, sys_reg, k1_sync_reg_rtx));

  return target;
}

static rtx
k1_expand_builtin_set (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = verify_const_uint_arg (arg1, 9, "set", "first");
  arg2 = force_reg (DImode, arg2);
  int regno = INTVAL (arg1) + K1C_SFR_FIRST_REGNO;
  rtx sys_reg = gen_rtx_REG (DImode, regno);

  emit_insn (gen_k1_set (sys_reg, arg2, k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_wfxl (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = verify_const_uint_arg (arg1, 9, "wfxl", "first");
  arg2 = force_reg (DImode, arg2);
  int regno = INTVAL (arg1) + K1C_SFR_FIRST_REGNO;
  rtx sys_reg = gen_rtx_REG (DImode, regno);

  emit_insn (gen_k1_wfxl (sys_reg, arg2, k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_wfxm (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = verify_const_uint_arg (arg1, 9, "wfxm", "first");
  arg2 = force_reg (DImode, arg2);
  int regno = INTVAL (arg1) + K1C_SFR_FIRST_REGNO;
  rtx sys_reg = gen_rtx_REG (DImode, regno);

  emit_insn (gen_k1_wfxm (sys_reg, arg2, k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_waitit (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (SImode, arg1);

  if (!target)
    target = gen_reg_rtx (SImode);
  else
    target = force_reg (SImode, target);

  emit_insn (gen_waitit (target, arg1, k1_sync_reg_rtx));

  return target;
}

static rtx
k1_expand_builtin_sbmm8 (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  emit_insn (gen_sbmm8 (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_sbmmt8 (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  emit_insn (gen_sbmmt8 (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_await (rtx target ATTRIBUTE_UNUSED,
			 tree args ATTRIBUTE_UNUSED)
{
  emit_insn (gen_await (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_doze (rtx target ATTRIBUTE_UNUSED, tree args ATTRIBUTE_UNUSED)
{
  emit_insn (gen_doze (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_sleep (rtx target ATTRIBUTE_UNUSED,
			 tree args ATTRIBUTE_UNUSED)
{
  emit_insn (gen_sleep (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_stop (rtx target ATTRIBUTE_UNUSED, tree args ATTRIBUTE_UNUSED)
{
  emit_insn (gen_stop (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_syncgroup (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (DImode, arg1);

  emit_insn (gen_syncgroup (arg1, k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_barrier (void)
{
  emit_insn (gen_barrier (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_dinval (void)
{
  emit_insn (gen_dinval (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_iinval (void)
{
  emit_insn (gen_iinval (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_tlbdinval (void)
{
  emit_insn (gen_tlbdinval (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_tlbiinval (void)
{
  emit_insn (gen_tlbiinval (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_tlbprobe (void)
{
  emit_insn (gen_tlbprobe (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_tlbread (void)
{
  emit_insn (gen_tlbread (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_tlbwrite (void)
{
  emit_insn (gen_tlbwrite (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_satd (rtx target, tree args)
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
k1_expand_builtin_satud (rtx target, tree args)
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
k1_expand_builtin_addsw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (SImode, arg1);
  arg2 = force_reg (SImode, arg2);

  if (!target)
    target = gen_reg_rtx (SImode);
  else
    target = force_reg (SImode, target);

  emit_insn (gen_ssaddsi3 (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_addsd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  emit_insn (gen_ssadddi3 (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_sbfsw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (SImode, arg1);
  arg2 = force_reg (SImode, arg2);

  if (!target)
    target = gen_reg_rtx (SImode);
  else
    target = force_reg (SImode, target);

  emit_insn (gen_sssubsi3 (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_sbfsd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  emit_insn (gen_sssubdi3 (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_cbsw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (SImode, arg1);

  if (!target)
    target = gen_reg_rtx (SImode);
  else
    target = force_reg (SImode, target);

  emit_insn (gen_popcountsi2 (target, arg1));
  return target;
}

static rtx
k1_expand_builtin_cbsd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (DImode, arg1);

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  emit_insn (gen_popcountdi2 (target, arg1));
  return target;
}

static rtx
k1_builtin_helper_memref_ptr (rtx ptr, enum machine_mode mode)
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
    rtx _mem_target = k1_builtin_helper_memref_ptr (_tmp, mode);               \
    varname = _mem_target;                                                     \
  }

#define GETREG(argnum, mode, varname)                                          \
  rtx varname = expand_normal (CALL_EXPR_ARG (args, argnum));                  \
  varname = force_reg (mode, varname);

static rtx
k1_expand_builtin_afadd (rtx target, tree args, enum machine_mode mode)
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
      emit_insn (gen_afaddd (target, mem_target, addend_and_return));
      break;
    case SImode:
      emit_insn (gen_afaddw (target, mem_target, addend_and_return));
      break;
    default:
      gcc_unreachable ();
    }

  return target;
}

static rtx
k1_expand_builtin_acswap (rtx target, tree args, enum machine_mode mode)
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
k1_expand_builtin_ctzw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);
  emit_insn (gen_ctzsi2 (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_clzw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);
  emit_insn (gen_clzsi2 (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_ctzd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (DImode);
  target = force_reg (DImode, target);
  arg1 = force_reg (DImode, arg1);
  emit_insn (gen_ctzdi2 (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_clzd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (DImode);
  target = force_reg (DImode, target);
  arg1 = force_reg (DImode, arg1);
  emit_insn (gen_clzdi2 (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_fence (void)
{
  emit_insn (gen_fence (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_dinvall (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = gen_rtx_MEM (SImode, force_reg (Pmode, arg1));

  emit_insn (gen_dinvall (arg1, k1_sync_reg_rtx));

  return target;
}

static rtx
k1_expand_builtin_dtouchl (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (Pmode, arg1);

  emit_insn (gen_prefetch (arg1, const0_rtx, const0_rtx));

  return target;
}

static rtx
k1_expand_builtin_dzerol (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = gen_rtx_MEM (SImode, force_reg (Pmode, arg1));

  emit_insn (gen_dzerol (arg1, k1_sync_reg_rtx));

  return target;
}

static rtx
k1_expand_builtin_iinvals (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = gen_rtx_MEM (SImode, force_reg (Pmode, arg1));

  emit_insn (gen_iinvals (arg1, k1_sync_reg_rtx));

  return target;
}

static rtx
k1_expand_builtin_alclr (rtx target, tree args, enum machine_mode mode)
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

#define K1_EXPAND_BUILTIN_2_STANDARD(name, name2, tmode, smode)                \
  static rtx k1_expand_builtin_##name (rtx target, tree args)                  \
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

K1_EXPAND_BUILTIN_2_STANDARD (fabsw, abssf2, SFmode, SFmode)
K1_EXPAND_BUILTIN_2_STANDARD (fabswp, absv2sf2, V2SFmode, V2SFmode)
K1_EXPAND_BUILTIN_2_STANDARD (fabswq, absv4sf2, V4SFmode, V4SFmode)
K1_EXPAND_BUILTIN_2_STANDARD (fabsd, absdf2, DFmode, DFmode)
K1_EXPAND_BUILTIN_2_STANDARD (fabsdp, absv2df2, V2DFmode, V2DFmode)

K1_EXPAND_BUILTIN_2_STANDARD (fnegw, negsf2, SFmode, SFmode)
K1_EXPAND_BUILTIN_2_STANDARD (fnegwp, negv2sf2, V2SFmode, V2SFmode)
K1_EXPAND_BUILTIN_2_STANDARD (fnegwq, negv4sf2, V4SFmode, V4SFmode)
K1_EXPAND_BUILTIN_2_STANDARD (fnegd, negdf2, DFmode, DFmode)
K1_EXPAND_BUILTIN_2_STANDARD (fnegdp, negv2df2, V2DFmode, V2DFmode)

#define K1_EXPAND_BUILTIN_3_STANDARD(name, name2, tmode, smode)                \
  static rtx k1_expand_builtin_##name (rtx target, tree args)                  \
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

K1_EXPAND_BUILTIN_3_STANDARD (abdw, k1_abdw, SImode, SImode)
K1_EXPAND_BUILTIN_3_STANDARD (abdd, k1_abdd, DImode, DImode)
K1_EXPAND_BUILTIN_3_STANDARD (avgw, k1_avgw, SImode, SImode)
K1_EXPAND_BUILTIN_3_STANDARD (avguw, k1_avguw, SImode, SImode)
K1_EXPAND_BUILTIN_3_STANDARD (avgrw, k1_avgrw, SImode, SImode)
K1_EXPAND_BUILTIN_3_STANDARD (avgruw, k1_avgruw, SImode, SImode)

K1_EXPAND_BUILTIN_3_STANDARD (fmaxw, fmaxsf3, SFmode, SFmode)
K1_EXPAND_BUILTIN_3_STANDARD (fmaxwp, fmaxv2sf3, V2SFmode, V2SFmode)
K1_EXPAND_BUILTIN_3_STANDARD (fmaxwq, fmaxv4sf3, V4SFmode, V4SFmode)
K1_EXPAND_BUILTIN_3_STANDARD (fmaxd, fmaxdf3, DFmode, DFmode)
K1_EXPAND_BUILTIN_3_STANDARD (fmaxdp, fmaxv2df3, V2DFmode, V2DFmode)

K1_EXPAND_BUILTIN_3_STANDARD (fminw, fminsf3, SFmode, SFmode)
K1_EXPAND_BUILTIN_3_STANDARD (fminwp, fminv2sf3, V2SFmode, V2SFmode)
K1_EXPAND_BUILTIN_3_STANDARD (fminwq, fminv4sf3, V4SFmode, V4SFmode)
K1_EXPAND_BUILTIN_3_STANDARD (fmind, fmindf3, DFmode, DFmode)
K1_EXPAND_BUILTIN_3_STANDARD (fmindp, fminv2df3, V2DFmode, V2DFmode)

#define K1_EXPAND_BUILTIN_2_MODIFIERS(name, tmode, smode)                      \
  static rtx k1_expand_builtin_##name (rtx target, tree args)                  \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = build_rounding_silent_arg (CALL_EXPR_ARG (args, 1), #name);     \
    arg1 = force_reg (smode, arg1);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_k1_##name (target, arg1, arg2));                            \
    return target;                                                             \
  }

K1_EXPAND_BUILTIN_2_MODIFIERS (finvw, SFmode, SFmode)
K1_EXPAND_BUILTIN_2_MODIFIERS (fisrw, SFmode, SFmode)

#define K1_EXPAND_BUILTIN_3_MODIFIERS(name, tmode, smode)                      \
  static rtx k1_expand_builtin_##name (rtx target, tree args)                  \
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
    emit_insn (gen_k1_##name (target, arg1, arg2, arg3));                      \
    return target;                                                             \
  }

K1_EXPAND_BUILTIN_3_MODIFIERS (faddw, SFmode, SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (faddwp, V2SFmode, V2SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (faddwq, V4SFmode, V4SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (faddd, DFmode, DFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fadddp, V2DFmode, V2DFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (faddcwc, V2SFmode, V2SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (faddcwcp, V4SFmode, V4SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (faddcdc, V2DFmode, V2DFmode)

K1_EXPAND_BUILTIN_3_MODIFIERS (fsbfw, SFmode, SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fsbfwp, V2SFmode, V2SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fsbfwq, V4SFmode, V4SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fsbfd, DFmode, DFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fsbfdp, V2DFmode, V2DFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fsbfcwc, V2SFmode, V2SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fsbfcwcp, V4SFmode, V4SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fsbfcdc, V2DFmode, V2DFmode)

K1_EXPAND_BUILTIN_3_MODIFIERS (fmulw, SFmode, SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmulwp, V2SFmode, V2SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmulwq, V4SFmode, V4SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmuld, DFmode, DFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmuldp, V2DFmode, V2DFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmulwd, DFmode, SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmulwc, V2SFmode, V2SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmulwcp, V4SFmode, V4SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmulcwc, V2SFmode, V2SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmulcwcp, V4SFmode, V4SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmuldc, V2DFmode, V2DFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmulcdc, V2DFmode, V2DFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmm2wq, V4SFmode, V2SFmode)

#define K1_EXPAND_BUILTIN_4_MODIFIERS(name, tmode, smode)                      \
  static rtx k1_expand_builtin_##name (rtx target, tree args)                  \
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
    emit_insn (gen_k1_##name (target, arg1, arg2, arg3, arg4));                \
    return target;                                                             \
  }

K1_EXPAND_BUILTIN_4_MODIFIERS (ffmaw, SFmode, SFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (ffmawp, V2SFmode, V2SFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (ffmawq, V4SFmode, V4SFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (ffmad, DFmode, DFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (ffmadp, V2DFmode, V2DFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (ffmawd, DFmode, SFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (fmm2awq, V4SFmode, V2SFmode)

K1_EXPAND_BUILTIN_4_MODIFIERS (ffmsw, SFmode, SFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (ffmswp, V2SFmode, V2SFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (ffmswq, V4SFmode, V4SFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (ffmsd, DFmode, DFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (ffmsdp, V2DFmode, V2DFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (ffmswd, DFmode, SFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (fmm2swq, V4SFmode, V2SFmode)

#define K1_EXPAND_BUILTIN_FCONVERT(name, tmode, smode)                         \
  static rtx k1_expand_builtin_##name (rtx target, tree args)                  \
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
    emit_insn (gen_k1_##name (target, arg1, arg2, arg3));                      \
    return target;                                                             \
  }

K1_EXPAND_BUILTIN_FCONVERT (floatw, SFmode, SImode)
K1_EXPAND_BUILTIN_FCONVERT (floatwp, V2SFmode, V2SImode)
K1_EXPAND_BUILTIN_FCONVERT (floatwq, V4SFmode, V4SImode)
K1_EXPAND_BUILTIN_FCONVERT (floatd, DFmode, DImode)
K1_EXPAND_BUILTIN_FCONVERT (floatdp, V2DFmode, V2DImode)
K1_EXPAND_BUILTIN_FCONVERT (floatuw, SFmode, SImode)
K1_EXPAND_BUILTIN_FCONVERT (floatuwp, V2SFmode, V2SImode)
K1_EXPAND_BUILTIN_FCONVERT (floatuwq, V4SFmode, V4SImode)
K1_EXPAND_BUILTIN_FCONVERT (floatud, DFmode, DImode)
K1_EXPAND_BUILTIN_FCONVERT (floatudp, V2DFmode, V2DImode)
K1_EXPAND_BUILTIN_FCONVERT (fixedw, SImode, SFmode)
K1_EXPAND_BUILTIN_FCONVERT (fixedwp, V2SImode, V2SFmode)
K1_EXPAND_BUILTIN_FCONVERT (fixedwq, V4SImode, V4SFmode)
K1_EXPAND_BUILTIN_FCONVERT (fixedd, DImode, DFmode)
K1_EXPAND_BUILTIN_FCONVERT (fixeddp, V2DImode, V2DFmode)
K1_EXPAND_BUILTIN_FCONVERT (fixeduw, SImode, SFmode)
K1_EXPAND_BUILTIN_FCONVERT (fixeduwp, V2SImode, V2SFmode)
K1_EXPAND_BUILTIN_FCONVERT (fixeduwq, V4SImode, V4SFmode)
K1_EXPAND_BUILTIN_FCONVERT (fixedud, DImode, DFmode)
K1_EXPAND_BUILTIN_FCONVERT (fixedudp, V2DImode, V2DFmode)

static rtx
k1_expand_builtin_fsdivw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (SFmode, arg1);
  arg2 = force_reg (SFmode, arg2);

  if (!target)
    target = gen_reg_rtx (SFmode);
  else
    target = force_reg (SFmode, target);

  emit_insn (gen_fsdivw (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_fsinvw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (SFmode, arg1);

  if (!target)
    target = gen_reg_rtx (SFmode);
  else
    target = force_reg (SFmode, target);

  emit_insn (gen_fsinvw (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_fcdivw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (SFmode, arg1);
  arg2 = force_reg (SFmode, arg2);

  if (!target)
    target = gen_reg_rtx (SFmode);
  else
    target = force_reg (SFmode, target);

  emit_insn (gen_fcdivw (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_fsisrw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (SFmode, arg1);

  if (!target)
    target = gen_reg_rtx (SFmode);
  else
    target = force_reg (SFmode, target);

  emit_insn (gen_fsisrw (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_fsdivd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (DFmode, arg1);
  arg2 = force_reg (DFmode, arg2);

  if (!target)
    target = gen_reg_rtx (DFmode);
  else
    target = force_reg (DFmode, target);

  emit_insn (gen_fsdivd (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_fsinvd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (DFmode, arg1);

  if (!target)
    target = gen_reg_rtx (DFmode);
  else
    target = force_reg (DFmode, target);

  emit_insn (gen_fsinvd (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_fcdivd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (DFmode, arg1);
  arg2 = force_reg (DFmode, arg2);

  if (!target)
    target = gen_reg_rtx (DFmode);
  else
    target = force_reg (DFmode, target);

  emit_insn (gen_fcdivd (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_fsisrd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  arg1 = force_reg (DFmode, arg1);

  if (!target)
    target = gen_reg_rtx (DFmode);
  else
    target = force_reg (DFmode, target);

  emit_insn (gen_fsisrd (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_lbsu (rtx target, tree args)
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
k1_expand_builtin_lbzu (rtx target, tree args)
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
k1_expand_builtin_ldu (rtx target, tree args)
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
k1_expand_builtin_lhsu (rtx target, tree args)
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
k1_expand_builtin_lhzu (rtx target, tree args)
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
k1_expand_builtin_lwzu (rtx target, tree args)
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
k1_expand_builtin_extfz (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));
  arg1 = force_reg (SImode, arg1);
  arg2 = verify_const_uint_arg (arg2, 6, "extfz", "second");
  arg3 = verify_const_uint_arg (arg3, 6, "extfz", "third");
  arg2 = gen_rtx_CONST_INT (SImode, INTVAL (arg2) - INTVAL (arg3) + 1);

  if (!target)
    target = gen_reg_rtx (SImode);
  else
    target = force_reg (SImode, target);

  emit_insn (gen_extzv (target, arg1, arg2, arg3));

  return target;
}

static rtx
k1_expand_builtin_stsuw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (SImode, arg1);
  arg2 = force_reg (SImode, arg2);

  if (!target)
    target = gen_reg_rtx (SImode);
  else
    target = force_reg (SImode, target);

  emit_insn (gen_stsuw (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_stsud (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  emit_insn (gen_stsud (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_fwiden (rtx target, tree args, int low_bits)
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
k1_expand_builtin_fnarrowwh (rtx target, tree args)
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

static rtx
k1_expand_builtin (tree exp, rtx target, rtx subtarget ATTRIBUTE_UNUSED,
		   enum machine_mode mode ATTRIBUTE_UNUSED,
		   int ignore ATTRIBUTE_UNUSED)
{
  tree fndecl = TREE_OPERAND (CALL_EXPR_FN (exp), 0);
  unsigned int fcode = DECL_FUNCTION_CODE (fndecl);

  switch (fcode)
    {
    case K1_BUILTIN_ABDW:
      return k1_expand_builtin_abdw (target, exp);
    case K1_BUILTIN_ABDD:
      return k1_expand_builtin_abdd (target, exp);
    case K1_BUILTIN_ADDSW:
      return k1_expand_builtin_addsw (target, exp);
    case K1_BUILTIN_ADDSD:
      return k1_expand_builtin_addsd (target, exp);
    case K1_BUILTIN_SBFSW:
      return k1_expand_builtin_sbfsw (target, exp);
    case K1_BUILTIN_SBFSD:
      return k1_expand_builtin_sbfsd (target, exp);
    case K1_BUILTIN_AVGW:
      return k1_expand_builtin_avgw (target, exp);
    case K1_BUILTIN_AVGUW:
      return k1_expand_builtin_avguw (target, exp);
    case K1_BUILTIN_AVGRW:
      return k1_expand_builtin_avgrw (target, exp);
    case K1_BUILTIN_AVGRUW:
      return k1_expand_builtin_avgruw (target, exp);
    case K1_BUILTIN_AWAIT:
      return k1_expand_builtin_await (target, exp);
    case K1_BUILTIN_BARRIER:
      return k1_expand_builtin_barrier ();
    case K1_BUILTIN_ACSWAPW:
      return k1_expand_builtin_acswap (target, exp, SImode);
    case K1_BUILTIN_ACSWAPD:
      return k1_expand_builtin_acswap (target, exp, DImode);
    case K1_BUILTIN_AFADDD:
      return k1_expand_builtin_afadd (target, exp, DImode);
    case K1_BUILTIN_AFADDW:
      return k1_expand_builtin_afadd (target, exp, SImode);
    case K1_BUILTIN_ALCLRD:
      return k1_expand_builtin_alclr (target, exp, DImode);
    case K1_BUILTIN_ALCLRW:
      return k1_expand_builtin_alclr (target, exp, SImode);
    case K1_BUILTIN_CBSW:
      return k1_expand_builtin_cbsw (target, exp);
    case K1_BUILTIN_CBSD:
      return k1_expand_builtin_cbsd (target, exp);
    case K1_BUILTIN_CLZW:
      return k1_expand_builtin_clzw (target, exp);
    case K1_BUILTIN_CLZD:
      return k1_expand_builtin_clzd (target, exp);
    case K1_BUILTIN_CTZW:
      return k1_expand_builtin_ctzw (target, exp);
    case K1_BUILTIN_CTZD:
      return k1_expand_builtin_ctzd (target, exp);
    case K1_BUILTIN_DINVAL:
      return k1_expand_builtin_dinval ();
    case K1_BUILTIN_DINVALL:
      return k1_expand_builtin_dinvall (target, exp);
    case K1_BUILTIN_DOZE:
      return k1_expand_builtin_doze (target, exp);
    case K1_BUILTIN_DTOUCHL:
      return k1_expand_builtin_dtouchl (target, exp);
    case K1_BUILTIN_DZEROL:
      return k1_expand_builtin_dzerol (target, exp);
    case K1_BUILTIN_EXTFZ:
      return k1_expand_builtin_extfz (target, exp);

    case K1_BUILTIN_FABSW:
      return k1_expand_builtin_fabsw (target, exp);
    case K1_BUILTIN_FABSWP:
      return k1_expand_builtin_fabswp (target, exp);
    case K1_BUILTIN_FABSWQ:
      return k1_expand_builtin_fabswq (target, exp);
    case K1_BUILTIN_FABSD:
      return k1_expand_builtin_fabsd (target, exp);
    case K1_BUILTIN_FABSDP:
      return k1_expand_builtin_fabsdp (target, exp);
    case K1_BUILTIN_FNEGW:
      return k1_expand_builtin_fnegw (target, exp);
    case K1_BUILTIN_FNEGWP:
      return k1_expand_builtin_fnegwp (target, exp);
    case K1_BUILTIN_FNEGWQ:
      return k1_expand_builtin_fnegwq (target, exp);
    case K1_BUILTIN_FNEGD:
      return k1_expand_builtin_fnegd (target, exp);
    case K1_BUILTIN_FNEGDP:
      return k1_expand_builtin_fnegdp (target, exp);
    case K1_BUILTIN_FMAXW:
      return k1_expand_builtin_fmaxw (target, exp);
    case K1_BUILTIN_FMAXWP:
      return k1_expand_builtin_fmaxwp (target, exp);
    case K1_BUILTIN_FMAXWQ:
      return k1_expand_builtin_fmaxwq (target, exp);
    case K1_BUILTIN_FMAXD:
      return k1_expand_builtin_fmaxd (target, exp);
    case K1_BUILTIN_FMAXDP:
      return k1_expand_builtin_fmaxdp (target, exp);
    case K1_BUILTIN_FMINW:
      return k1_expand_builtin_fminw (target, exp);
    case K1_BUILTIN_FMINWP:
      return k1_expand_builtin_fminwp (target, exp);
    case K1_BUILTIN_FMINWQ:
      return k1_expand_builtin_fminwq (target, exp);
    case K1_BUILTIN_FMIND:
      return k1_expand_builtin_fmind (target, exp);
    case K1_BUILTIN_FMINDP:
      return k1_expand_builtin_fmindp (target, exp);
    case K1_BUILTIN_FINVW:
      return k1_expand_builtin_finvw (target, exp);
    case K1_BUILTIN_FISRW:
      return k1_expand_builtin_fisrw (target, exp);
    case K1_BUILTIN_FADDW:
      return k1_expand_builtin_faddw (target, exp);
    case K1_BUILTIN_FADDWP:
      return k1_expand_builtin_faddwp (target, exp);
    case K1_BUILTIN_FADDWQ:
      return k1_expand_builtin_faddwq (target, exp);
    case K1_BUILTIN_FADDD:
      return k1_expand_builtin_faddd (target, exp);
    case K1_BUILTIN_FADDDP:
      return k1_expand_builtin_fadddp (target, exp);
    case K1_BUILTIN_FADDCWC:
      return k1_expand_builtin_faddcwc (target, exp);
    case K1_BUILTIN_FADDCWCP:
      return k1_expand_builtin_faddcwcp (target, exp);
    case K1_BUILTIN_FADDCDC:
      return k1_expand_builtin_faddcdc (target, exp);
    case K1_BUILTIN_FSBFW:
      return k1_expand_builtin_fsbfw (target, exp);
    case K1_BUILTIN_FSBFWP:
      return k1_expand_builtin_fsbfwp (target, exp);
    case K1_BUILTIN_FSBFWQ:
      return k1_expand_builtin_fsbfwq (target, exp);
    case K1_BUILTIN_FSBFD:
      return k1_expand_builtin_fsbfd (target, exp);
    case K1_BUILTIN_FSBFDP:
      return k1_expand_builtin_fsbfdp (target, exp);
    case K1_BUILTIN_FSBFCWC:
      return k1_expand_builtin_fsbfcwc (target, exp);
    case K1_BUILTIN_FSBFCWCP:
      return k1_expand_builtin_fsbfcwcp (target, exp);
    case K1_BUILTIN_FSBFCDC:
      return k1_expand_builtin_fsbfcdc (target, exp);
    case K1_BUILTIN_FMULW:
      return k1_expand_builtin_fmulw (target, exp);
    case K1_BUILTIN_FMULWP:
      return k1_expand_builtin_fmulwp (target, exp);
    case K1_BUILTIN_FMULWQ:
      return k1_expand_builtin_fmulwq (target, exp);
    case K1_BUILTIN_FMULD:
      return k1_expand_builtin_fmuld (target, exp);
    case K1_BUILTIN_FMULDP:
      return k1_expand_builtin_fmuldp (target, exp);
    case K1_BUILTIN_FMULWD:
      return k1_expand_builtin_fmulwd (target, exp);
    case K1_BUILTIN_FMULWC:
      return k1_expand_builtin_fmulwc (target, exp);
    case K1_BUILTIN_FMULWCP:
      return k1_expand_builtin_fmulwcp (target, exp);
    case K1_BUILTIN_FMULCWC:
      return k1_expand_builtin_fmulcwc (target, exp);
    case K1_BUILTIN_FMULCWCP:
      return k1_expand_builtin_fmulcwcp (target, exp);
    case K1_BUILTIN_FMULDC:
      return k1_expand_builtin_fmuldc (target, exp);
    case K1_BUILTIN_FMULCDC:
      return k1_expand_builtin_fmulcdc (target, exp);
    case K1_BUILTIN_FMM2WQ:
      return k1_expand_builtin_fmm2wq (target, exp);
    case K1_BUILTIN_FFMAW:
      return k1_expand_builtin_ffmaw (target, exp);
    case K1_BUILTIN_FFMAWP:
      return k1_expand_builtin_ffmawp (target, exp);
    case K1_BUILTIN_FFMAWQ:
      return k1_expand_builtin_ffmawq (target, exp);
    case K1_BUILTIN_FFMAD:
      return k1_expand_builtin_ffmad (target, exp);
    case K1_BUILTIN_FFMADP:
      return k1_expand_builtin_ffmadp (target, exp);
    case K1_BUILTIN_FFMAWD:
      return k1_expand_builtin_ffmawd (target, exp);
    case K1_BUILTIN_FMM2AWQ:
      return k1_expand_builtin_fmm2awq (target, exp);
    case K1_BUILTIN_FFMSW:
      return k1_expand_builtin_ffmsw (target, exp);
    case K1_BUILTIN_FFMSWP:
      return k1_expand_builtin_ffmswp (target, exp);
    case K1_BUILTIN_FFMSWQ:
      return k1_expand_builtin_ffmswq (target, exp);
    case K1_BUILTIN_FFMSD:
      return k1_expand_builtin_ffmsd (target, exp);
    case K1_BUILTIN_FFMSDP:
      return k1_expand_builtin_ffmsdp (target, exp);
    case K1_BUILTIN_FFMSWD:
      return k1_expand_builtin_ffmswd (target, exp);
    case K1_BUILTIN_FMM2SWQ:
      return k1_expand_builtin_fmm2swq (target, exp);
    case K1_BUILTIN_FLOATW:
      return k1_expand_builtin_floatw (target, exp);
    case K1_BUILTIN_FLOATWP:
      return k1_expand_builtin_floatwp (target, exp);
    case K1_BUILTIN_FLOATWQ:
      return k1_expand_builtin_floatwq (target, exp);
    case K1_BUILTIN_FLOATD:
      return k1_expand_builtin_floatd (target, exp);
    case K1_BUILTIN_FLOATDP:
      return k1_expand_builtin_floatdp (target, exp);
    case K1_BUILTIN_FLOATUW:
      return k1_expand_builtin_floatuw (target, exp);
    case K1_BUILTIN_FLOATUWP:
      return k1_expand_builtin_floatuwp (target, exp);
    case K1_BUILTIN_FLOATUWQ:
      return k1_expand_builtin_floatuwq (target, exp);
    case K1_BUILTIN_FLOATUD:
      return k1_expand_builtin_floatud (target, exp);
    case K1_BUILTIN_FLOATUDP:
      return k1_expand_builtin_floatudp (target, exp);
    case K1_BUILTIN_FIXEDW:
      return k1_expand_builtin_fixedw (target, exp);
    case K1_BUILTIN_FIXEDWP:
      return k1_expand_builtin_fixedwp (target, exp);
    case K1_BUILTIN_FIXEDWQ:
      return k1_expand_builtin_fixedwq (target, exp);
    case K1_BUILTIN_FIXEDD:
      return k1_expand_builtin_fixedd (target, exp);
    case K1_BUILTIN_FIXEDDP:
      return k1_expand_builtin_fixeddp (target, exp);
    case K1_BUILTIN_FIXEDUW:
      return k1_expand_builtin_fixeduw (target, exp);
    case K1_BUILTIN_FIXEDUWP:
      return k1_expand_builtin_fixeduwp (target, exp);
    case K1_BUILTIN_FIXEDUWQ:
      return k1_expand_builtin_fixeduwq (target, exp);
    case K1_BUILTIN_FIXEDUD:
      return k1_expand_builtin_fixedud (target, exp);
    case K1_BUILTIN_FIXEDUDP:
      return k1_expand_builtin_fixedudp (target, exp);

    case K1_BUILTIN_FENCE:
      return k1_expand_builtin_fence ();

    case K1_BUILTIN_FSDIVW:
      return k1_expand_builtin_fsdivw (target, exp);
    case K1_BUILTIN_FSDIVD:
      return k1_expand_builtin_fsdivd (target, exp);
    case K1_BUILTIN_FSINVW:
      return k1_expand_builtin_fsinvw (target, exp);
    case K1_BUILTIN_FSINVD:
      return k1_expand_builtin_fsinvd (target, exp);
    case K1_BUILTIN_FSISRW:
      return k1_expand_builtin_fsisrw (target, exp);
    case K1_BUILTIN_FSISRD:
      return k1_expand_builtin_fsisrd (target, exp);
    case K1_BUILTIN_FCDIVW:
      return k1_expand_builtin_fcdivw (target, exp);
    case K1_BUILTIN_FCDIVD:
      return k1_expand_builtin_fcdivd (target, exp);
    case K1_BUILTIN_GET:
      return k1_expand_builtin_get (target, exp);
    case K1_BUILTIN_WFXL:
      return k1_expand_builtin_wfxl (target, exp);
    case K1_BUILTIN_WFXM:
      return k1_expand_builtin_wfxm (target, exp);
    case K1_BUILTIN_IINVAL:
      return k1_expand_builtin_iinval ();
    case K1_BUILTIN_IINVALS:
      return k1_expand_builtin_iinvals (target, exp);
    case K1_BUILTIN_LBSU:
      return k1_expand_builtin_lbsu (target, exp);
    case K1_BUILTIN_LBZU:
      return k1_expand_builtin_lbzu (target, exp);
    case K1_BUILTIN_LDU:
      return k1_expand_builtin_ldu (target, exp);
    case K1_BUILTIN_LHSU:
      return k1_expand_builtin_lhsu (target, exp);
    case K1_BUILTIN_LHZU:
      return k1_expand_builtin_lhzu (target, exp);
    case K1_BUILTIN_LWZU:
      return k1_expand_builtin_lwzu (target, exp);
    case K1_BUILTIN_SATD:
      return k1_expand_builtin_satd (target, exp);
    case K1_BUILTIN_SATUD:
      return k1_expand_builtin_satud (target, exp);
    case K1_BUILTIN_SBMM8:
      return k1_expand_builtin_sbmm8 (target, exp);
    case K1_BUILTIN_SBMMT8:
      return k1_expand_builtin_sbmmt8 (target, exp);
    case K1_BUILTIN_SET:
      return k1_expand_builtin_set (target, exp);
    case K1_BUILTIN_SLEEP:
      return k1_expand_builtin_sleep (target, exp);
    case K1_BUILTIN_STOP:
      return k1_expand_builtin_stop (target, exp);
    case K1_BUILTIN_STSUW:
      return k1_expand_builtin_stsuw (target, exp);
    case K1_BUILTIN_STSUD:
      return k1_expand_builtin_stsud (target, exp);
    case K1_BUILTIN_SYNCGROUP:
      return k1_expand_builtin_syncgroup (target, exp);
    case K1_BUILTIN_TLBDINVAL:
      return k1_expand_builtin_tlbdinval ();
    case K1_BUILTIN_TLBIINVAL:
      return k1_expand_builtin_tlbiinval ();
    case K1_BUILTIN_TLBPROBE:
      return k1_expand_builtin_tlbprobe ();
    case K1_BUILTIN_TLBREAD:
      return k1_expand_builtin_tlbread ();
    case K1_BUILTIN_TLBWRITE:
      return k1_expand_builtin_tlbwrite ();
    case K1_BUILTIN_FWIDENLHW:
      return k1_expand_builtin_fwiden (target, exp, 1);
    case K1_BUILTIN_FWIDENMHW:
      return k1_expand_builtin_fwiden (target, exp, 0);
    case K1_BUILTIN_FNARROWWH:
      return k1_expand_builtin_fnarrowwh (target, exp);
    case K1_BUILTIN_WAITIT:
      return k1_expand_builtin_waitit (target, exp);
    default:
      break;
    }
  internal_error ("bad builtin code");
  return NULL_RTX;
}

int
k1_mau_lsu_double_port_bypass_p (rtx_insn *producer, rtx_insn *consumer)
{
  rtx produced = SET_DEST (single_set (producer));
  rtx consumed = PATTERN (consumer);

  if (GET_CODE (consumed) == PARALLEL)
    consumed = XVECEXP (consumed, 0, 0);
  consumed = SET_DEST (consumed);

  return reg_overlap_mentioned_p (produced, consumed);
}

static int
k1_sched_adjust_cost (rtx_insn *insn, int dep_type,
		      rtx_insn *dep_insn ATTRIBUTE_UNUSED, int cost,
		      unsigned int)
{
  enum attr_class insn_class = get_attr_class (insn);
  /* On the k1, it is possible to read then write the same register in a bundle
   * so we set the WAR cost to 0 unless insn is a control-flow consuming reg
   * then it is 1 */
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
k1_sched_issue_rate (void)
{
  return 4;
}

static int
k1_sched_dfa_new_cycle (FILE *dump ATTRIBUTE_UNUSED,
		        int verbose ATTRIBUTE_UNUSED, rtx_insn *insn,
		        int last_clock, int clock,
		        int *sort_p ATTRIBUTE_UNUSED)
{
  if (clock != last_clock)
    return 0;

  if (reg_mentioned_p (k1_sync_reg_rtx, insn))
    return 1;

  return 0;
}

/* Test if X is of the form reg[reg] or .xs reg = reg[reg] or signed10bits[reg]
 */
bool
k1_has_10bit_imm_or_register_p (rtx x)
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

  return k1_has_10bit_immediate_p (x);
}

bool
k1_has_10bit_immediate_p (rtx x)
{
  if (MEM_P (x))
    x = XEXP (x, 0);

  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 1)) == CONST_INT)
    return IN_RANGE (INTVAL (XEXP (x, 1)), -512, 511);

  return false;
}

bool
k1_has_37bit_immediate_p (rtx x)
{
  if (MEM_P (x))
    x = XEXP (x, 0);

  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 1)) == CONST_INT)
    return !IN_RANGE (INTVAL (XEXP (x, 1)), -512, 511)
	   && IN_RANGE (INTVAL (XEXP (x, 1)), -(1LL << 36), (1LL << 36) - 1);

  return false;
}

bool
k1_has_64bit_immediate_p (rtx x)
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
k1_is_uncached_mem_op_p (rtx x)
{
  gcc_assert (MEM_P (x));
  if (!MEM_P (x))
    return false;

  /* __convert[_no_sync] addr space should not come here. */
  gcc_assert (MEM_ADDR_SPACE (x) < K1_ADDR_SPACE_CONVERT);

  return MEM_ADDR_SPACE (x);
}

HOST_WIDE_INT
k1_const_vector_value (rtx x, int slice)
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
k1_has_10bit_vector_const_p (rtx x)
{
  HOST_WIDE_INT value = k1_const_vector_value (x, 0);
  return SIGNED_INT_FITS_N_BITS (value, 10);
}

bool
k1_has_16bit_vector_const_p (rtx x)
{
  HOST_WIDE_INT value = k1_const_vector_value (x, 0);
  return SIGNED_INT_FITS_N_BITS (value, 16);
}

bool
k1_has_32bit_vector_const_p (rtx x)
{
  HOST_WIDE_INT value = k1_const_vector_value (x, 0);
  return SIGNED_INT_FITS_N_BITS (value, 32);
}

bool
k1_has_37bit_vector_const_p (rtx x)
{
  HOST_WIDE_INT value = k1_const_vector_value (x, 0);
  return SIGNED_INT_FITS_N_BITS (value, 37);
}

bool
k1_has_43bit_vector_const_p (rtx x)
{
  HOST_WIDE_INT value = k1_const_vector_value (x, 0);
  return SIGNED_INT_FITS_N_BITS (value, 43);
}

bool
k1_has_32x2bit_vector_const_p (rtx x)
{
  HOST_WIDE_INT value = k1_const_vector_value (x, 0);
  // Need the dual immediate syntax to be fixed in assembler.
  // return (value&0xFFFFFFFF) == ((value>>32)&0xFFFFFFFF);
  return false;
}

/* Helper function for k1_expand_load_multiple and
   k1_expand_store_multiple */
static bool
k1_expand_load_store_multiple (rtx operands[], bool is_load)
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
      || REGNO (operands[reg_op_idx]) > K1C_GPR_LAST_REGNO)
    return false;

  operands[3] = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc (count));

  rtx base, offset;
  if (!k1_split_mem (XEXP (operands[mem_op_idx], 0), &base, &offset, false))
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
k1_expand_store_multiple (rtx operands[])
{
  return k1_expand_load_store_multiple (operands, false);
}

/* Expands a load multiple with operand 0 being the first destination
   register, operand 1 the first source address and operand 2 the
   number of consecutive loads to pack. */
bool
k1_expand_load_multiple (rtx operands[])
{
  return k1_expand_load_store_multiple (operands, true);
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
k1_load_store_multiple_operation_p (rtx op, bool is_uncached, bool is_load)
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

      if (is_load && is_uncached != !!k1_is_uncached_mem_op_p (mem_part))
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

      if (!k1_split_mem (XEXP (mem_elt, 0), &base_cur, &offset_cur, false))
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
k1_load_multiple_operation_p (rtx op, bool is_uncached)
{
  return k1_load_store_multiple_operation_p (op, is_uncached, true);
}

/*
 * Returns TRUE if OP is a store multiple operation.
 */
bool
k1_store_multiple_operation_p (rtx op)
{
  return k1_load_store_multiple_operation_p (op, false, false);
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

static void k1_scan_insn_registers_wrap (rtx *x, void *data);

static int
k1_scan_insn_registers_1 (rtx *x, void *data)
{
  struct bundle_regs *regs = (struct bundle_regs *) data;

  if (!*x)
    return 0;

  if (GET_CODE (*x) == USE && CALL_P (regs->scanned_insn))
    return -1;

  if (GET_CODE (*x) == SET)
    {
      regs->set_dest = 1;
      /* for_each_rtx (&SET_DEST (*x), k1_scan_insn_registers_1, regs); */
      k1_scan_insn_registers_wrap (&SET_DEST (*x), regs);
      regs->set_dest = 0;
      /* for_each_rtx (&SET_SRC (*x), k1_scan_insn_registers_1, regs); */
      k1_scan_insn_registers_wrap (&SET_SRC (*x), regs);
      return -1;
    }

  if (MEM_P (*x))
    {
      regs->set_dest = 0;
      /* for_each_rtx (&XEXP (*x, 0), k1_scan_insn_registers_1, regs); */
      k1_scan_insn_registers_wrap (&XEXP (*x, 0), regs);
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
      gcc_assert (REGNO (*x) <= K1C_MDS_REGISTERS);

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
k1_scan_insn_registers_wrap (rtx *x, void *data)
{
  subrtx_ptr_iterator::array_type array;
  FOR_EACH_SUBRTX_PTR (iter, array, x, ALL)
    {
      rtx *x = *iter;
      if (k1_scan_insn_registers_1 (x, data) == -1)
	{
	  iter.skip_subrtxes ();
	}
    }
}

static void
k1_scan_insn_registers (rtx insn, struct bundle_regs *regs)
{
  if (GET_CODE (insn) == CLOBBER || GET_CODE (insn) == USE || !INSN_P (insn))
    return;

  regs->set_dest = 0;
  regs->scanned_insn = insn;
  CLEAR_HARD_REG_SET (regs->uses);
  CLEAR_HARD_REG_SET (regs->defs);
  /* for_each_rtx (&insn, k1_scan_insn_registers_1, regs); */
  k1_scan_insn_registers_wrap (&insn, regs);
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
k1_insn_is_bundle_end_p (rtx insn)
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

static void k1_dump_bundles (void);

static void
k1_gen_bundles (void)
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
	  k1_scan_insn_registers (insn, &cur_insn_regs);

	  const int insn_raw = hard_reg_set_intersect_p (cur_insn_regs.uses,
							 cur_bstate->reg_defs);
	  const int insn_waw = hard_reg_set_intersect_p (cur_insn_regs.defs,
							 cur_bstate->reg_defs);
	  const int insn_jump = JUMP_P (insn) || CALL_P (insn);
	  const int next_is_label = (next != NULL) && LABEL_P (next);

	  /* Current insn can be bundled with other insn, create a new one. */
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

static void k1_dump_bundles (void) ATTRIBUTE_UNUSED;

static void
k1_dump_bundles (void)
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
k1_swap_fp_sp_in_note (rtx note, rtx old_base)
{
  XEXP (note, 0) = copy_rtx (XEXP (note, 0));
  rtx note_pat = XEXP (note, 0);

  rtx new_base_reg = (REGNO (old_base) == REGNO (hard_frame_pointer_rtx))
		       ? stack_pointer_rtx
		       : hard_frame_pointer_rtx;
  rtx mem_dest = SET_DEST (note_pat);
  struct k1_frame_info *frame = &cfun->machine->frame;

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
k1_fix_debug_for_bundles (void)
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
			    k1_swap_fp_sp_in_note (note, base_reg);
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
k1_asm_final_postscan_insn (FILE *file, rtx_insn *insn,
			    rtx *opvec ATTRIBUTE_UNUSED,
			    int noperands ATTRIBUTE_UNUSED)
{
  if (!scheduling || !TARGET_BUNDLING || k1_insn_is_bundle_end_p (insn))
    {
      fprintf (file, "\t;;\n");
      return;
    }
}

static int
k1_sched_first_cycle_multipass_dfa_lookahead (void)
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
k1_rtx_operand_cost (rtx *x, void *arg)
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
k1_rtx_costs (rtx x, machine_mode mode ATTRIBUTE_UNUSED,
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
	  *total = k1_register_move_cost (GET_MODE (SET_DEST (x)),
						 NO_REGS, NO_REGS);
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
      k1_rtx_operand_cost (x, &cost);
    }

  *total = cost.total;
  return true;
}

static int
k1_address_cost (rtx x, machine_mode mode ATTRIBUTE_UNUSED,
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
k1_sched_set_sched_flags (struct spec_info_def *spec ATTRIBUTE_UNUSED)
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
k1_sched_init_global (FILE *file ATTRIBUTE_UNUSED,
		      int verbose ATTRIBUTE_UNUSED,
		      int max_ready ATTRIBUTE_UNUSED)
{
  scheduling = true;
}

static bool
k1_function_ok_for_sibcall (tree decl, tree exp ATTRIBUTE_UNUSED)
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
  if (K1_FARCALL || lookup_attribute ("farcall", attrs))
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
k1_legitimate_constant_p (enum machine_mode mode ATTRIBUTE_UNUSED, rtx x)
{
  if (k1_has_tls_reference (x))
    return false;

  if (k1_has_unspec_reference (x))
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
k1_legitimize_address (rtx x, rtx oldx ATTRIBUTE_UNUSED,
		       enum machine_mode mode)
{
  if (k1_has_tls_reference (x))
    return k1_legitimize_tls_reference (x);
  else if (GET_CODE (x) == PLUS
	   && (GET_CODE (XEXP (x, 0)) == MULT
	       || GET_CODE (XEXP (x, 0)) == ZERO_EXTEND))
    {
      rtx reg = gen_reg_rtx (Pmode);
      rtx cst = XEXP (x, 1);

      XEXP (x, 1) = reg;
      if (k1_legitimate_address_p (mode, x, false))
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
k1_addr_space_pointer_mode (addr_space_t address_space ATTRIBUTE_UNUSED)
{
  return ptr_mode;
}

/* Implements TARGET_ADDR_SPACE_ADDRESS_MODE */
static machine_mode
k1_addr_space_address_mode (addr_space_t address_space ATTRIBUTE_UNUSED)
{
  return Pmode;
}

/* Implements TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P */
static bool
k1_addr_space_legitimate_address_p (machine_mode mode, rtx exp, bool strict,
				    addr_space_t as ATTRIBUTE_UNUSED)
{
  switch (as)
    {
    default:
      gcc_unreachable ();

    case ADDR_SPACE_GENERIC:
    case K1_ADDR_SPACE_BYPASS:
    case K1_ADDR_SPACE_PRELOAD:
      return k1_legitimate_address_p (mode, exp, strict);

    case K1_ADDR_SPACE_CONVERT:
      return false;
    }
}

/* Implements TARGET_ADDR_SPACE_LEGITIMIZE_ADDRESS */
static rtx
k1_addr_space_legitimize_address (rtx x, rtx oldx, machine_mode mode,
				  addr_space_t as)
{
  if (as == K1_ADDR_SPACE_CONVERT)
    error ("__convert should be used only in explicit pointer casting");

  return k1_legitimize_address (x, oldx, mode);
}

/* Implements TARGET_ADDR_SPACE_SUBSET_P */
static bool
k1_addr_space_subset_p (addr_space_t subset ATTRIBUTE_UNUSED,
			addr_space_t superset ATTRIBUTE_UNUSED)
{
  // Address spaces (GENERIC or __UNCACHED) refer to the same space
  return true;
}

/* Implements TARGET_ADDR_SPACE_CONVERT */
static rtx
k1_addr_space_convert (rtx op, tree from_type, tree to_type ATTRIBUTE_UNUSED)
{
  if (K1_WARN_ADDRESS_SPACE_CONVERSION
      && TYPE_ADDR_SPACE (TREE_TYPE (from_type)) != K1_ADDR_SPACE_CONVERT
      && TYPE_ADDR_SPACE (TREE_TYPE (to_type)) != K1_ADDR_SPACE_CONVERT)
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
k1_function_prologue (FILE *file ATTRIBUTE_UNUSED,
		      HOST_WIDE_INT size ATTRIBUTE_UNUSED)
{
  dfa_start ();
}

static void
k1_function_epilogue (FILE *file ATTRIBUTE_UNUSED,
		      HOST_WIDE_INT size ATTRIBUTE_UNUSED)
{
  dfa_finish ();
}

static void
k1_dependencies_evaluation_hook (rtx_insn *head, rtx_insn *tail)
{
  rtx_insn *insn, *insn2, *next_tail, *last_sync = head;

  next_tail = NEXT_INSN (tail);

  for (insn = head; insn != next_tail; insn = NEXT_INSN (insn))
    if (INSN_P (insn))
      {
	int sync = reg_mentioned_p (k1_sync_reg_rtx, insn);

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

static const char *
k1_invalid_within_doloop (const rtx_insn *insn)
{
  rtx asm_ops, body;

  if (CALL_P (insn))
    return "Function call in loop.";

  if (JUMP_TABLE_DATA_P (insn))
    return "Computed branch in the loop.";

  if (!INSN_P (insn))
    return NULL;

  body = PATTERN (insn);
  if (volatile_insn_p (body))
    return "unspec or asm volatile in the loop.";

  asm_ops = extract_asm_operands (body);
  if (asm_ops && GET_CODE (body) == PARALLEL)
    {
      int i, regno, nparallel = XVECLEN (body, 0); /* Includes CLOBBERs.  */

      for (i = 0; i < nparallel; i++)
	{
	  rtx clobber = XVECEXP (body, 0, i);

	  if (GET_CODE (clobber) == CLOBBER && REG_P (XEXP (clobber, 0))
	      && (regno = REGNO (XEXP (clobber, 0)))
	      && (regno == K1C_LC_REGNO || regno == K1C_LS_REGNO
		  || regno == K1C_LE_REGNO))
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

  test = gen_rtx_NE (VOIDmode, loop->iter_reg, const0_rtx);
  insn = emit_jump_insn_before (gen_cbranchsi4 (test, loop->iter_reg,
						const0_rtx, loop->start_label),
				loop->loop_end);

  JUMP_LABEL (insn) = loop->start_label;
  LABEL_NUSES (loop->start_label)++;

  /* Copy back the counter in the memory as reload has to be handled
     by ourself */
  if (has_reload)
    {
      rtx mem_dest = SET_DEST (XVECEXP (PATTERN (loop->loop_end), 0, 2));
      emit_insn (gen_movsi (mem_dest, loop->iter_reg));
    }

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

static struct hw_doloop_hooks k1_doloop_hooks
  = {hwloop_pattern_reg, hwloop_optimize, hwloop_fail};

static void
k1_reorg_loops (void)
{
  if (optimize)
    reorg_loops (false, &k1_doloop_hooks);
}

/* Implement the TARGET_MACHINE_DEPENDENT_REORG pass.  */

static void
k1_reorg (void)
{
  compute_bb_for_insn ();

  /* If optimizing, we'll have split before scheduling.  */
  if (optimize == 0)
    split_all_insns ();

  df_analyze ();

  /* Doloop optimization. */
  k1_reorg_loops ();

  /* Do it even if ! TARGET_BUNDLING because it also takes care of
   cleaning previous data */
  initiate_bundle_states ();

  if (scheduling && TARGET_BUNDLING)
    {
      k1_gen_bundles ();
      k1_fix_debug_for_bundles ();
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
k1_handle_fixed_reg_option (const char *arg)
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
k1_option_override (void)
{
  unsigned int i;
  cl_deferred_option *opt;
  vec<cl_deferred_option> *v = (vec<cl_deferred_option> *) k1_deferred_options;

  if (v)
    FOR_EACH_VEC_ELT (*v, i, opt)
      {
	switch (opt->opt_index)
	  {
	  case OPT_ffixed_reg:
	    k1_handle_fixed_reg_option (opt->arg);
	    break;
	  default:
	    gcc_unreachable ();
	  }
      }

  k1_arch_schedule = ARCH_COOLIDGE;
}

/* Recognize machine-specific patterns that may appear within
   constants.  Used for PIC-specific UNSPECs.
   Previously a macro. */
static bool
k1_output_addr_const_extra (FILE *fp, rtx x)
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
	  fputs (k1_unspec_tls_asm_op[unspec_code - UNSPEC_TLS_GD], (fp));
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
k1_reassociation_width (unsigned int opc, enum machine_mode mode)
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
k1_mode_dependent_address_p (const_rtx addr,
			     addr_space_t space ATTRIBUTE_UNUSED)
{
  const_rtx x = addr;

  // Same logic as .xs addressing mode in k1_print_operand
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
k1_sched_can_speculate_insn (rtx_insn *insn)
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
k1_float_fits_bits (const REAL_VALUE_TYPE *r, unsigned bitsz,
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
k1_asm_pat_copyq (rtx srcreg)
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
k1_asm_pat_copyo (void)
{
  if (!optimize_size)
    return "#";
  return "copyo %0 = %1";
}

/* Returns TRUE if OP is a REG (directly or through a SUBREG) */
bool
k1_is_reg_subreg_p (rtx op)
{
  return REG_P (op) || (SUBREG_P (op) && REG_P (SUBREG_REG (op)));
}

/* Returns TRUE if OP is a pseudo REG (directly or through a SUBREG)
 */
static bool
k1_is_pseudo_reg_subreg_p (rtx op)
{
  return ((REG_P (op) && !HARD_REGISTER_P (op))
	  || (SUBREG_P (op) && REG_P (SUBREG_REG (op))
	      && !HARD_REGISTER_P (SUBREG_REG (op))));
}

/* Returns TRUE if OP is a hard (sub)register aligned on ALIGN or a
 * pseudo (sub)register, FALSE for all other cases. */
static bool
k1_check_align_reg (rtx op, int align)
{
  if (!k1_is_reg_subreg_p (op))
    return false;
  if (k1_is_pseudo_reg_subreg_p (op))
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
k1_ok_for_paired_reg_p (rtx op)
{
  return k1_check_align_reg (op, 2);
}

/* Returns TRUE if OP is a hard (sub)register quad aligned or a pseudo
 * (sub)register, FALSE for all other cases. It is used to check
 * correct alignement for some SIMD insn or 256bits load/store */
bool
k1_ok_for_quad_reg_p (rtx op)
{
  return k1_check_align_reg (op, 4);
}

/* Split a 128bit move op in mode MODE from SRC to DST in 2 smaller
   64bit moves */
void
k1_split_128bits_move (rtx dst, rtx src, enum machine_mode mode)
{
  rtx lo_src = gen_lowpart (word_mode, src);
  rtx hi_src = gen_highpart_mode (word_mode, mode, src);

  emit_insn (gen_movdi (gen_lowpart (word_mode, dst), lo_src));
  emit_insn (gen_movdi (gen_highpart (word_mode, dst), hi_src));
}

/* Split a 256bit move op in mode MODE from SRC to DST in 2 smaller
   128bit moves */
void
k1_split_256bits_move (rtx dst, rtx src, enum machine_mode mode)
{
  emit_insn (gen_movti (simplify_gen_subreg (TImode, dst, mode, 0),
			simplify_gen_subreg (TImode, src, mode, 0)));

  emit_insn (gen_movti (simplify_gen_subreg (TImode, dst, mode, 16),
			simplify_gen_subreg (TImode, src, mode, 16)));
}

/* Returns TRUE if OP is a symbol and has the farcall attribute or if
   -mfarcall is in use. */
bool
k1_is_farcall_p (rtx op)
{
  bool farcall = K1_FARCALL;
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

void
k1_profile_hook (void)
{
  rtx ra_arg = get_hard_reg_initial_val (Pmode, K1C_RETURN_POINTER_REGNO);
  rtx fun = gen_rtx_SYMBOL_REF (Pmode, "__mcount");
  emit_library_call (fun, LCT_NORMAL, VOIDmode, 1, ra_arg, Pmode);
}

/* Returns asm template for ctrapsi4 */
char *
k1_ctrapsi4 (void)
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
#define TARGET_CLASS_MAX_NREGS k1_class_max_nregs

#undef TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE k1_option_override

#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE k1_function_value

#undef TARGET_RETURN_IN_MSB
#define TARGET_RETURN_IN_MSB k1_return_in_msb

#undef TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY k1_return_in_memory

#undef TARGET_STRUCT_VALUE_RTX
#define TARGET_STRUCT_VALUE_RTX k1_struct_value_rtx

#undef TARGET_ASM_OUTPUT_MI_THUNK
#define TARGET_ASM_OUTPUT_MI_THUNK k1_asm_output_mi_thunk

#undef TARGET_ASM_CAN_OUTPUT_MI_THUNK
#define TARGET_ASM_CAN_OUTPUT_MI_THUNK k1_asm_can_output_mi_thunk

#undef TARGET_ASM_ALIGNED_DI_OP
#define TARGET_ASM_ALIGNED_DI_OP "\t.8byte\t"

#undef TARGET_EXPAND_BUILTIN_SAVEREGS
#define TARGET_EXPAND_BUILTIN_SAVEREGS k1_expand_builtin_saveregs

#undef TARGET_EXPAND_BUILTIN_VA_START
#define TARGET_EXPAND_BUILTIN_VA_START k1_expand_va_start

#undef TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P k1_legitimate_address_p

#undef TARGET_DECIMAL_FLOAT_SUPPORTED_P
#define TARGET_DECIMAL_FLOAT_SUPPORTED_P k1_decimal_float_supported_p

#undef TARGET_FIXED_POINT_SUPPORTED_P
#define TARGET_FIXED_POINT_SUPPORTED_P k1_fixed_point_supported_p

#undef TARGET_VECTOR_MODE_SUPPORTED_P
#define TARGET_VECTOR_MODE_SUPPORTED_P k1_vector_mode_supported_p

#undef TARGET_VECTORIZE_SUPPORT_VECTOR_MISALIGNMENT
#define TARGET_VECTORIZE_SUPPORT_VECTOR_MISALIGNMENT                           \
  k1_support_vector_misalignment

#undef TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES k1_promote_prototypes

#undef TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES k1_arg_partial_bytes

#undef TARGET_MUST_PASS_IN_STACK
#define TARGET_MUST_PASS_IN_STACK must_pass_in_stack_var_size

#undef TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE k1_pass_by_reference

#undef TARGET_SECONDARY_RELOAD
#define TARGET_SECONDARY_RELOAD k1_secondary_reload

#undef TARGET_INIT_BUILTINS
#define TARGET_INIT_BUILTINS k1_init_builtins

#undef TARGET_EXPAND_BUILTIN
#define TARGET_EXPAND_BUILTIN k1_expand_builtin

#undef TARGET_CANNOT_FORCE_CONST_MEM
#define TARGET_CANNOT_FORCE_CONST_MEM k1_cannot_force_const_mem

#undef TARGET_SCHED_ADJUST_COST
#define TARGET_SCHED_ADJUST_COST k1_sched_adjust_cost

#undef TARGET_SCHED_ISSUE_RATE
#define TARGET_SCHED_ISSUE_RATE k1_sched_issue_rate

#undef TARGET_SCHED_FIRST_CYCLE_MULTIPASS_DFA_LOOKAHEAD
#define TARGET_SCHED_FIRST_CYCLE_MULTIPASS_DFA_LOOKAHEAD                       \
  k1_sched_first_cycle_multipass_dfa_lookahead

#undef TARGET_SCHED_SET_SCHED_FLAGS
#define TARGET_SCHED_SET_SCHED_FLAGS k1_sched_set_sched_flags

#undef TARGET_SCHED_INIT_GLOBAL
#define TARGET_SCHED_INIT_GLOBAL k1_sched_init_global

#undef TARGET_SCHED_DFA_NEW_CYCLE
#define TARGET_SCHED_DFA_NEW_CYCLE k1_sched_dfa_new_cycle

#undef TARGET_SCHED_REASSOCIATION_WIDTH
#define TARGET_SCHED_REASSOCIATION_WIDTH k1_reassociation_width

#undef TARGET_RTX_COSTS
#define TARGET_RTX_COSTS k1_rtx_costs

#undef TARGET_ADDRESS_COST
#define TARGET_ADDRESS_COST k1_address_cost

#undef TARGET_REGISTER_MOVE_COST
#define TARGET_REGISTER_MOVE_COST k1_register_move_cost

#undef TARGET_FUNCTION_OK_FOR_SIBCALL
#define TARGET_FUNCTION_OK_FOR_SIBCALL k1_function_ok_for_sibcall

#undef TARGET_BUILTIN_DECL
#define TARGET_BUILTIN_DECL k1_builtin_decl

#undef TARGET_LEGITIMIZE_ADDRESS
#define TARGET_LEGITIMIZE_ADDRESS k1_legitimize_address

#undef TARGET_CONST_ANCHOR
#define TARGET_CONST_ANCHOR 0x200

#undef TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE k1_function_prologue

#undef TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE k1_function_epilogue

#undef TARGET_ASM_FINAL_POSTSCAN_INSN
#define TARGET_ASM_FINAL_POSTSCAN_INSN k1_asm_final_postscan_insn

#undef TARGET_SCHED_DEPENDENCIES_EVALUATION_HOOK
#define TARGET_SCHED_DEPENDENCIES_EVALUATION_HOOK                              \
  k1_dependencies_evaluation_hook

#undef TARGET_MACHINE_DEPENDENT_REORG
#define TARGET_MACHINE_DEPENDENT_REORG k1_reorg

#undef TARGET_ATTRIBUTE_TABLE
#define TARGET_ATTRIBUTE_TABLE k1_attribute_table

/* FIXME AUTO: disabling vector support */
/* #undef TARGET_VECTORIZE_PREFERRED_SIMD_MODE */
/* #define TARGET_VECTORIZE_PREFERRED_SIMD_MODE k1_units_preffered_simd_mode */

#undef TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG k1_function_arg

#undef TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE k1_function_arg_advance

#undef TARGET_LEGITIMATE_CONSTANT_P
#define TARGET_LEGITIMATE_CONSTANT_P k1_legitimate_constant_p

#undef TARGET_CONDITIONAL_REGISTER_USAGE
#define TARGET_CONDITIONAL_REGISTER_USAGE k1_conditional_register_usage

#undef TARGET_CANONICALIZE_COMPARISON
#define TARGET_CANONICALIZE_COMPARISON k1_canonicalize_comparison

#undef TARGET_ASM_OUTPUT_ADDR_CONST_EXTRA
#define TARGET_ASM_OUTPUT_ADDR_CONST_EXTRA k1_output_addr_const_extra

#undef TARGET_INVALID_WITHIN_DOLOOP
#define TARGET_INVALID_WITHIN_DOLOOP k1_invalid_within_doloop

#undef TARGET_MODE_DEPENDENT_ADDRESS_P
#define TARGET_MODE_DEPENDENT_ADDRESS_P k1_mode_dependent_address_p

#undef TARGET_SCHED_CAN_SPECULATE_INSN
#define TARGET_SCHED_CAN_SPECULATE_INSN k1_sched_can_speculate_insn

#undef TARGET_CAN_USE_DOLOOP_P
#define TARGET_CAN_USE_DOLOOP_P can_use_doloop_if_innermost

#undef TARGET_ADDR_SPACE_POINTER_MODE
#define TARGET_ADDR_SPACE_POINTER_MODE k1_addr_space_pointer_mode

#undef TARGET_ADDR_SPACE_ADDRESS_MODE
#define TARGET_ADDR_SPACE_ADDRESS_MODE k1_addr_space_address_mode
#undef TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P
#define TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P                                 \
  k1_addr_space_legitimate_address_p

#undef TARGET_ADDR_SPACE_LEGITIMIZE_ADDRESS
#define TARGET_ADDR_SPACE_LEGITIMIZE_ADDRESS k1_addr_space_legitimize_address

#undef TARGET_ADDR_SPACE_SUBSET_P
#define TARGET_ADDR_SPACE_SUBSET_P k1_addr_space_subset_p

#undef TARGET_ADDR_SPACE_CONVERT
#define TARGET_ADDR_SPACE_CONVERT k1_addr_space_convert

/* FIXME AUTO: trampoline are broken T6775 */
#undef TARGET_STATIC_CHAIN
#define TARGET_STATIC_CHAIN k1_static_chain

struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-k1.h"
