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

#define K1C_SYNC_REG_REGNO (K1C_MDS_REGISTERS + 0)

rtx k1_sync_reg_rtx;
rtx k1_link_reg_rtx;
static rtx k1_data_start_symbol;

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

static int k1_flag_var_tracking;

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
  /* vec<fake_SC_t,va_gc> *fake_SC_registers; */
};

/*

		  +---------------+
		  | Varargs       |
		  |               |
		  |               |
		  +---------------+
      Virt. FP--->| [Static chain]|
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

  /* ABI requires 16-bytes (128bits) alignment. */
#define K1_STACK_ALIGN(LOC) (((LOC) + 0xF) & ~0xF)

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
k1_first_parm_offset (tree decl)
{
  struct k1_frame_info *frame;
  k1_compute_frame_info ();
  frame = &cfun->machine->frame;

  return frame->arg_pointer_fp_offset;
}

static rtx
k1_static_chain (const_tree fndecl, bool incoming_p)
{
  struct k1_frame_info *frame;
  k1_compute_frame_info ();
  frame = &cfun->machine->frame;

  if (!DECL_STATIC_CHAIN (fndecl))
    return NULL;

  cfun->machine->static_chain_on_stack = 1;

  return gen_frame_mem (Pmode, frame_pointer_rtx);
}

static const char *prf_reg_names[] = {K1C_PRF_REGISTER_NAMES};
static const char *qrf_reg_names[] = {K1C_QRF_REGISTER_NAMES};

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
bool
k1_pack_load_store (rtx operands[], unsigned int nops)
{
  rtx set_dests[K1_MAX_PACKED_LSU];
  rtx set_srcs[K1_MAX_PACKED_LSU];
  bool is_load = true;
  rtx sorted_operands[nops];

  /* Only ld +ld => lq */
  if (nops != 2)
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

  /* Only loads */
  for (unsigned i = 0; i < nops; i++)
    if (!MEM_P (set_srcs[i]))
      return false;

  /* Sort and check operands */
  if (is_load)
    {
      unsigned int min_regno = REGNO (operands[0]);

      /* Find first regno for destination */
      for (unsigned int i = 1; i < nops; i++)
	if (REGNO (operands[i * 2]) < min_regno)
	  min_regno = REGNO (operands[i * 2]);

      /* Sort operands based on regno */
      for (unsigned int i = 0; i < nops; i++)
	{
	  const unsigned int regno = REGNO (operands[i * 2]);
	  const unsigned int idx = 2 * (regno - min_regno);

	  /* Register are not consecutive */
	  if (idx >= (2 * nops))
	    return false;

	  /* Destination register used twice in operands */
	  if (sorted_operands[idx] != NULL_RTX)
	    return false;

	  sorted_operands[idx] = operands[2 * i];
	  sorted_operands[idx + 1] = operands[2 * i + 1];
	}

      /* Check mem addresses are consecutive */
      rtx base_reg, base_offset;
      if (!k1_split_mem (XEXP (sorted_operands[1], 0), &base_reg, &base_offset,
			 true))
	return false;

      const unsigned int base_regno = REGNO (base_reg);

      /* Base register is modified by one load */
      if (base_regno >= REGNO (sorted_operands[0])
	  && base_regno <= REGNO (sorted_operands[(nops - 1) * 2]))
	{
	  bool mod_before_last = false;
	  /* Check the base register is modified in the last load */
	  for (unsigned int i = 0; i < (nops - 1); i++)
	    {
	      if (REGNO (operands[2 * i]) == base_regno)
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
	  rtx elem = XEXP (sorted_operands[2 * i + 1], 0);

	  /* Not addressing next memory word */
	  if (GET_CODE (elem) != PLUS || !REG_P (XEXP (elem, 0))
	      || REGNO (XEXP (elem, 0)) != base_regno
	      || INTVAL (XEXP (elem, 1)) != next_offset)
	    return false;

	  next_offset += UNITS_PER_WORD;
	}
    }

  rtx lm = gen_load_multiple (sorted_operands[0], sorted_operands[1],
			      GEN_INT (nops));

  if (lm != NULL_RTX)
    emit_insn (lm);
  else
    return false;

  return true;
}

/* Implement HARD_REGNO_RENAME_OK.  */
int
k1_hard_regno_rename_ok (unsigned int from, unsigned int to)
{
  if (from >= K1C_GRF_FIRST_REGNO && from <= K1C_GRF_LAST_REGNO
      && to >= K1C_GRF_FIRST_REGNO && to <= K1C_GRF_LAST_REGNO)
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
  // SI/DI -> K1C_GRF_FIRST_REGNO - K1C_GRF_LAST_REGNO => OK
  // SI/DI -> K1C_SRF_FIRST_REGNO - K1C_SRF_LAST_REGNO => OK
  // TI    -> K1C_GRF_FIRST_REGNO - K1C_GRF_LAST_REGNO && even => OK
  // OI    -> K1C_GRF_FIRST_REGNO - K1C_GRF_LAST_REGNO && 0mod4 => OK
  if (GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
    return 1;
  if (regno >= K1C_GRF_FIRST_REGNO && regno <= K1C_GRF_LAST_REGNO)
    {
      if (mode == TImode)
	return (regno % 2 == 0);
      if (mode == OImode)
	return (regno % 4 == 0);
    }
  return 0;
}

static unsigned char
k1_class_max_nregs (reg_class_t regclass, enum machine_mode mode)
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

static bool
k1_target_legitimate_address_p (enum machine_mode ATTRIBUTE_UNUSED mode, rtx x,
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

enum addressing_mode
{
  ADDR_INVALID,
  ADDR_OFFSET,
  ADDR_MULT,
  ADDR_MOD
};

struct k1_address
{
  enum addressing_mode mode;
  rtx offset;
  rtx base_reg;
  rtx offset_reg;
  int mult;
  int mod;
};

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
      if (GET_CODE (x) == UNSPEC && XINT (x, 1) == UNSPEC_TLS)
	iter.skip_subrtxes ();
    }
  return false;
}

/* Returns 1 if an UNSPEC_TLS has been found (ie. a TLS offset), 0 if
   not */
static int
k1_has_tprel (rtx x)
{
  if (!TARGET_HAVE_TLS)
    return false;
  subrtx_iterator::array_type array;
  FOR_EACH_SUBRTX (iter, array, x, ALL)
    {
      const_rtx x = *iter;
      if (GET_CODE (x) == UNSPEC && XINT (x, 1) == UNSPEC_TLS)
	return true;
    }
  return false;
}

static int
k1_has_unspec_reference_1 (rtx *x, void *data ATTRIBUTE_UNUSED)
{
  return (GET_CODE (*x) == UNSPEC
	  && (XINT (*x, 1) == UNSPEC_GOT || XINT (*x, 1) == UNSPEC_GOTOFF
	      || XINT (*x, 1) == UNSPEC_PCREL || XINT (*x, 1) == UNSPEC_TLS));
}

static int
k1_needs_symbol_reloc_1 (rtx *x, void *gprel)
{
  if (k1_has_unspec_reference_1 (x, gprel))
    return -1;

  /* One could believe that we should filter out functions here,
     but it's not true. Function calls won't use
     legitimate_constant_p, and other uses of function addresses
     need to go through the GOT (think about comparing
     function poitners). */
  return GET_CODE (*x) == SYMBOL_REF;
}

static int
k1_needs_symbol_reloc (rtx x)
{
  subrtx_ptr_iterator::array_type array;
  FOR_EACH_SUBRTX_PTR (iter, array, &x, ALL)
    {
      rtx *x = *iter;
      switch (k1_needs_symbol_reloc_1 (x, NULL))
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
  //    return for_each_rtx (&x, &k1_needs_symbol_reloc_1, NULL);
}

static int
k1_has_unspec_reference (rtx x)
{
  subrtx_ptr_iterator::array_type array;
  FOR_EACH_SUBRTX_PTR (iter, array, &x, ALL)
    {
      rtx *x = *iter;
      switch (k1_has_unspec_reference_1 (x, NULL))
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
  // FIXME AUTO: subreg in MEM
  /* || (GET_CODE (reg) == SUBREG */
  /*     && GET_MODE (reg) == Pmode */
  /*     && REG_P (SUBREG_REG (reg)) */
  /*     && IS_GENERAL_REGNO (REGNO (SUBREG_REG (reg)), strict)); */
}

static bool
k1_legitimate_address_offset_register_p (rtx reg, bool strict)
{
  return (REG_P (reg)
	  && IS_GENERAL_REGNO (REGNO (reg), strict)
	  // AUTO FIXME: was DImode instead of Pmode here. Correct ?
	  && GET_MODE (reg) == Pmode);
  // FIXME AUTO: subreg in MEM
  /* || (GET_CODE (reg) == SUBREG */

  /*     && GET_MODE (reg) == DImode */
  /*     && REG_P (SUBREG_REG (reg)) */
  /*     && IS_GENERAL_REGNO (REGNO (SUBREG_REG (reg)), strict)); */
}

/* static bool */
/* k1_analyze_modulo_address (rtx x, bool strict, struct k1_address *addr) */
/* { */
/*     unsigned HOST_WIDE_INT mul = 0, mod; */

/*     // Modulo addressing not supported on k1b */
/*     if(TARGET_K1BDP || TARGET_K1BIO) { */
/*       return false; */
/*     } */

/*     addr->mode = ADDR_INVALID; */

/*     if (GET_CODE (x) != PLUS */
/*         || !k1_legitimate_address_register_p (XEXP (x, 1), strict)) */
/*         return false; */

/*     x = XEXP (x, 0); */

/*     if (GET_CODE (x) == ZERO_EXTEND) { */
/* 	if (GET_MODE (XEXP (x, 0)) == QImode) */
/* 	    mod = 255; */
/* 	else if (GET_MODE (XEXP (x, 0)) == HImode) */
/* 	    mod = 65535; */
/* 	else */
/* 	    return false; */
/* 	if (GET_CODE (XEXP (x, 0)) == SUBREG) */
/* 	    x = XEXP (x, 0); */
/*     } else if (GET_CODE (x) == AND */
/* 	       && GET_CODE (XEXP (x, 1)) == CONST_INT) { */
/* 	mod = INTVAL (XEXP (x, 1)); */
/*     } else */
/* 	return false; */

/*     x = XEXP (x, 0); */

/*     if (k1_legitimate_address_register_p (x, strict)) { */
/*         mul = 1; */
/*         addr->mult = 1; */
/*         addr->offset_reg = x; */
/*     } else if (GET_CODE (x) == ASHIFT */
/*                && (INTVAL (XEXP (x, 1)) == 0 */
/*                    || INTVAL (XEXP (x, 1)) == 1 */
/*                    || INTVAL (XEXP (x, 1)) == 2 */
/*                    || INTVAL (XEXP (x, 1)) == 3) */
/*                && k1_legitimate_address_register_p (XEXP (x, 0), strict)) {
 */
/*         mul = 1 << INTVAL (XEXP (x, 1)) ; */
/*         addr->mult = mul; */
/*         addr->offset_reg = XEXP (x, 0); */
/*     } else if (GET_CODE (x) == MULT */
/*                && (INTVAL (XEXP (x, 1)) == 1 */
/*                    || INTVAL (XEXP (x, 1)) == 2 */
/*                    || INTVAL (XEXP (x, 1)) == 4 */
/*                    || INTVAL (XEXP (x, 1)) == 8) */
/*                && k1_legitimate_address_register_p (XEXP (x, 0), strict)) {
 */
/*         mul = INTVAL (XEXP (x, 1)); */
/*         addr->mult = mul; */
/*         addr->offset_reg = XEXP (x, 0); */
/*     } else { */
/*         return false; */
/*     } */

/*     if (mul == 0) */
/*         mul = INTVAL (XEXP (x, 1)); */
/*     mod /= mul; */
/*     mod += 1; */
/*     addr->mod = mod; */

/*     if (__builtin_popcount (mod) == 1 && mod <= 65536) { */
/*         addr->mode = ADDR_MOD; */
/*         addr->base_reg = XEXP (x, 1); */
/*         return true; */
/*     } */

/*     return false; */
/* } */

/**
 * Legitimate address :
 * - (plus (reg) (constant)) : {ADDR_OFFSET, offset:constant, base:reg}
 * - (reg) : {ADDR_OFFSET, offset:0, base:reg}
 * - (plus (reg1) (reg2)) : {ADDR_MULT, offset:reg1, base:reg2, mult:1}
 * - (plus (reg1)
 */
static bool
k1_analyze_address (rtx x, bool strict, struct k1_address *addr)
{
  addr->mode = ADDR_INVALID;

  /* A 64bits symbol/label won't fit and symbolic refs should be done
     using @got[off] if not pcrel */
  /* FIXME AUTO: symbols can fit as immediate values on coolidge. This
   * constraint can be relaxed. */
  /* if ((TARGET_64 || flag_pic) && symbolic_reference_mentioned_p(x)){ */
  /*   return false; */
  /* } */

  if (k1_has_tprel (x))
    return false;

  /*
   * Valid :
   * ld reg = @got[reg]
   * ld reg = @gotoff[reg]
   */
  if (GET_CODE (x) == PLUS
      && (GET_CODE (XEXP (x, 1)) == UNSPEC
	  && (XINT (XEXP (x, 1), 1) == UNSPEC_GOT
	      || XINT (XEXP (x, 1), 1) == UNSPEC_GOTOFF))
      && k1_legitimate_address_register_p (XEXP (x, 0), strict))
    {
      addr->mode = ADDR_OFFSET;
      addr->offset = XEXP (x, 1);
      addr->base_reg = XEXP (x, 0);

      return true;
    }
  else if ((!current_pass || current_pass->tv_id != TV_CPROP)
	   && GET_CODE (x) == PLUS
	   && k1_legitimate_address_register_p (XEXP (x, 0), strict)
	   && ((CONSTANT_P (XEXP (x, 1))
		&& k1_legitimate_constant_p (VOIDmode, XEXP (x, 1)))
	       || GET_CODE (XEXP (x, 1)) == CONST_INT)
	   && immediate_operand (XEXP (x, 1), SImode))
    {

      /*
       * Valid:
       * ld reg = const[reg] if const fits in 32bits
       * ld reg = symbol[reg] in 32bits mode only !
       * Invalid:
       * ld reg = symbol[reg] in 64bits
       * ld reg = const[reg] if const does NOT fit in 32bits
       *
       * The constant used by LSU 64 insn cannot hold a full length
       * pointer. Basically, in 64bits, we can't handle a symbol/label
       * in mem ref as we can't guarantee it will fit.
       */
      addr->mode = ADDR_OFFSET;
      addr->offset = XEXP (x, 1);
      addr->base_reg = XEXP (x, 0);

      return true;
    }
  else if (k1_legitimate_address_register_p (x, strict))
    {
      addr->mode = ADDR_OFFSET;
      addr->offset = const0_rtx;
      addr->base_reg = x;
      return true;
    }
  else if (GET_CODE (x) == PLUS
	   && k1_legitimate_address_register_p (XEXP (x, 0), strict)
	   && k1_legitimate_address_offset_register_p (XEXP (x, 1), strict))
    {
      addr->mode = ADDR_MULT;
      addr->mult = 1;
      addr->base_reg = XEXP (x, 0);
      addr->offset_reg = XEXP (x, 1);
      return true;
    }

  /* FIXME AUTO: disable .xs scaling as it is broken in MDS */
  /* else if (GET_CODE (x) == PLUS */
  /*            && k1_legitimate_address_register_p (XEXP (x, 1), strict) */
  /*            && ((GET_CODE (XEXP (x, 0)) == MULT */
  /*                 && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT */
  /*                 && (INTVAL (XEXP (XEXP (x, 0), 1)) == 1 */
  /*                     || INTVAL (XEXP (XEXP (x, 0), 1)) == 2 */
  /*                     || INTVAL (XEXP (XEXP (x, 0), 1)) == 4 */
  /*                     || INTVAL (XEXP (XEXP (x, 0), 1)) == 8)) */
  /*                || (GET_CODE (XEXP (x, 0)) == ASHIFT */
  /*                    && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT */
  /*                    && (INTVAL (XEXP (XEXP (x, 0), 1)) == 0 */
  /*                        || INTVAL (XEXP (XEXP (x, 0), 1)) == 1 */
  /*                        || INTVAL (XEXP (XEXP (x, 0), 1)) == 2 */
  /*                        || INTVAL (XEXP (XEXP (x, 0), 1)) == 3))) */
  /*            && k1_legitimate_address_offset_register_p (XEXP (XEXP (x, 0),
   * 0), strict)) { */
  /*     addr->mode = ADDR_MULT; */
  /*     addr->base_reg = XEXP (x, 1); */
  /*     addr->offset_reg = XEXP (XEXP (x, 0), 0); */
  /*     addr->mult = INTVAL (XEXP (XEXP (x, 0), 1)); */
  /*     if (GET_CODE (XEXP (x, 0)) == ASHIFT) */
  /*         addr->mult = 1 << addr->mult; */
  /* } */

  return false;
}

static void
k1_target_conditional_register_usage (void)
{
  k1_sync_reg_rtx = gen_rtx_REG (SImode, K1C_SYNC_REG_REGNO);
  k1_link_reg_rtx = gen_rtx_REG (Pmode, K1C_RETURN_POINTER_REGNO);
  k1_data_start_symbol
    = gen_rtx_SYMBOL_REF (Pmode,
			  IDENTIFIER_POINTER (get_identifier ("_data_start")));
}

rtx
k1_return_addr_rtx (int count, rtx frameaddr ATTRIBUTE_UNUSED)
{
  return count == 0 ? get_hard_reg_initial_val (Pmode, K1C_RETURN_POINTER_REGNO)
		    : NULL_RTX;
}

/* Implements the macro INIT_CUMULATIVE_ARGS defined in k1.h. */

void
k1_init_cumulative_args (CUMULATIVE_ARGS *cum, const_tree fntype, rtx libname,
			 tree fndecl ATTRIBUTE_UNUSED,
			 int n_named_args ATTRIBUTE_UNUSED)
{
  cum->next_arg_reg = 0;
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
   because there is no argument slot in registers free. */

static rtx
k1_get_arg_info (struct k1_arg_info *info, cumulative_args_t cum_v,
		 enum machine_mode mode, const_tree type,
		 bool named ATTRIBUTE_UNUSED)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
  HOST_WIDE_INT n_bytes
    = type ? int_size_in_bytes (type) : GET_MODE_SIZE (mode);
  HOST_WIDE_INT n_words = (n_bytes + UNITS_PER_WORD - 1) / UNITS_PER_WORD;

  /* Arguments larger than 8 bytes start at the next even boundary.  */
  HOST_WIDE_INT offset = (n_words > 1 && (cum->next_arg_reg & 1)) ? 1 : 0;

  /* If all argument slots are used, then it must go on the stack.  */
  if ((cum->next_arg_reg + offset) >= K1C_ARG_REG_SLOTS)
    {
      info->num_stack = n_words;
      info->num_regs = 0;
      return NULL_RTX;
    }

  info->first_reg = cum->next_arg_reg + offset;
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
		 const_tree type, bool named ATTRIBUTE_UNUSED)
{
  struct k1_arg_info info;
  return k1_get_arg_info (&info, cum_v, mode, type, named);
}

/* Implements TARGET_FUNCTION_ARG_BOUNDARY.
   Align arguments on a 64bits boundary for small arguments, 128 for bigger
   ones. */

static unsigned int
k1_function_arg_boundary (enum machine_mode mode, const_tree type)
{
  HOST_WIDE_INT sz
    = (mode == BLKmode ? int_size_in_bytes (type) : GET_MODE_SIZE (mode));
  return ((sz > UNITS_PER_WORD) ? UNITS_PER_WORD * 2 : UNITS_PER_WORD);
}

/* Implements TARGET_ARG_PARTIAL_BYTES.
   Return the number of bytes, at the beginning of an argument,
   that must be put in registers */

static int
k1_arg_partial_bytes (cumulative_args_t cum_v, enum machine_mode mode,
		      tree type, bool named ATTRIBUTE_UNUSED)
{
  struct k1_arg_info info = {0};
  k1_get_arg_info (&info, cum_v, mode, type, named);
  if (info.num_regs > 0 && info.num_stack > 0)
    {
      return info.num_regs * UNITS_PER_WORD;
    }
  return 0;
}

/* Implements TARGET_FUNCTION_ARG.
   Update CUM to point after this argument. */

static void
k1_function_arg_advance (cumulative_args_t cum_v, enum machine_mode mode,
			 const_tree type, bool named ATTRIBUTE_UNUSED)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
  struct k1_arg_info info = {0};
  k1_get_arg_info (&info, cum_v, mode, type, named);
  if (info.num_regs > 0)
    {
      cum->next_arg_reg = info.first_reg + info.num_regs;
    }
}

static rtx
k1_target_function_value (const_tree ret_type, const_tree func ATTRIBUTE_UNUSED,
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
k1_target_return_in_msb (const_tree type ATTRIBUTE_UNUSED)
{
  return false;
}

/* Implements TARGET_RETURN_IN_MEMORY */
static bool
k1_target_return_in_memory (const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  HOST_WIDE_INT sz = int_size_in_bytes (type);

  return TYPE_MODE (type) == BLKmode
	 && (sz > (K1C_ARG_REG_SLOTS * UNITS_PER_WORD) || sz < 0);
}

/* Implements TARGET_STRUCT_VALUE_RTX */
static rtx
k1_target_struct_value_rtx (tree fndecl ATTRIBUTE_UNUSED,
			    int incoming ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (Pmode, K1C_STRUCT_POINTER_REGNO);
}

static void
k1_target_asm_output_mi_thunk (FILE *file ATTRIBUTE_UNUSED,
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
k1_target_asm_can_output_mi_thunk (const_tree thunk_fndecl ATTRIBUTE_UNUSED,
				   HOST_WIDE_INT delta ATTRIBUTE_UNUSED,
				   HOST_WIDE_INT vcall_offset ATTRIBUTE_UNUSED,
				   const_tree function ATTRIBUTE_UNUSED)
{
  return true;
}

static rtx
k1_target_expand_builtin_saveregs (void)
{
  int regno;
  int slot = 0;
  HOST_WIDE_INT arg_fp_offset;
  struct k1_frame_info *frame;

  k1_compute_frame_info ();
  frame = &cfun->machine->frame;
  rtx area = gen_rtx_PLUS (Pmode, frame_pointer_rtx,
			   GEN_INT (frame->arg_pointer_fp_offset));

  /* All arg register slots used for named args, nothing to push */
  if (crtl->args.info.next_arg_reg >= K1C_ARG_REG_SLOTS)
    return area;

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
k1_target_expand_va_start (tree valist, rtx nextarg ATTRIBUTE_UNUSED)
{
  rtx saveregs_area = expand_builtin_saveregs ();
  rtx va_r = expand_expr (valist, NULL_RTX, VOIDmode, EXPAND_WRITE);
  emit_move_insn (va_r, saveregs_area);
}

static bool
k1_cannot_force_const_mem (enum machine_mode mode ATTRIBUTE_UNUSED,
			   rtx x ATTRIBUTE_UNUSED)
{
  return true;
}

static bool
k1_target_decimal_float_supported_p (void)
{
  return true;
}

static bool
k1_target_fixed_point_supported_p (void)
{
  return false;
}

static bool
k1_target_vector_mode_supported_p (enum machine_mode mode)
{
  return false;
  /* FIXME AUTO: disabling vector support */
  /* return V4HImode == mode || V2SImode == mode || V8SFmode == mode || V4SFmode
   * == mode || V2SFmode == mode || V8SImode == mode; */
}

static bool
k1_target_support_vector_misalignment (enum machine_mode mode ATTRIBUTE_UNUSED,
				       const_tree type ATTRIBUTE_UNUSED,
				       int misalignment ATTRIBUTE_UNUSED,
				       bool is_packed ATTRIBUTE_UNUSED)
{
  return !TARGET_STRICT_ALIGN;
}

static bool
k1_target_promote_prototypes (const_tree fndecl ATTRIBUTE_UNUSED)
{
  return true;
}

static bool
k1_target_pass_by_reference (cumulative_args_t cum ATTRIBUTE_UNUSED,
			     enum machine_mode mode ATTRIBUTE_UNUSED,
			     const_tree type ATTRIBUTE_UNUSED,
			     bool named ATTRIBUTE_UNUSED)
{
  return false;
}

static reg_class_t
k1_target_secondary_reload (bool in_p ATTRIBUTE_UNUSED, rtx x ATTRIBUTE_UNUSED,
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
	  return GRF_REGS;
	}
      else
	{
	  sri->icode = GET_MODE (x) == SImode ? CODE_FOR_reload_in_gotoff_si
					      : CODE_FOR_reload_in_gotoff_di;
	  return NO_REGS;
	}
    }
  else if (SYMBOLIC_CONST (x) && !k1_legitimate_constant_p (VOIDmode, x))
    gcc_unreachable ();

  return NO_REGS;
}

bool
k1_lowbit_highbit_constant_p (HOST_WIDE_INT val_, int *lowbit, int *highbit)
{
  uint32_t val = val_ & 0xFFFFFFFF;
  int popcount = __builtin_popcount (val);
  int ctz = __builtin_ctz (val);
  int clz = __builtin_clz (val);
  int res = 0;
  int tmp;

  if (32 - clz - ctz == popcount)
    {
      res = 1;
      clz = 31 - clz;
      goto end;
    }

  val = ~val;
  popcount = __builtin_popcount (val);
  ctz = __builtin_ctz (val);
  clz = __builtin_clz (val);

  res = 32 - clz - ctz == popcount;
  tmp = 31 - clz;
  clz = ctz - 1;
  ctz = tmp;

end:
  if (lowbit)
    *lowbit = ctz;
  if (highbit)
    *highbit = clz;

  return res;
}

void
k1_target_print_operand (FILE *file, rtx x, int code)
{
  rtx operand = x;
  int quadword_reg = 0;
  int octupleword_reg = 0;
  bool u32 = false;
  bool addressing_mode = false;
  bool as_address = false;
  bool is_float = false;
  /* bool low = false; */
  /* bool high = false; */
  bool lowbit_highbit = false;
  bool signed10 = false;
  /* bool want_plt = false; */
  int lowbit, highbit;

  switch (code)
    {
    case 0:
      /* No code, print as usual.  */
      break;
    case 'a':
      as_address = true;
      break;
    case 'b':
      lowbit_highbit = true;
      break;
    case 'Q': /* Force the use of quadruple even if mode is not compatible */
      quadword_reg++;
      /* fallthrough */
    case 'q':
      quadword_reg++;
      break;
    case 'O': /* Force the use of octuple even if mode is not compatible */
      octupleword_reg++;
      /* fallthrough */
    case 'o':
      octupleword_reg++;
      break;

    case 'f':
      is_float = true;
      break;
    case 'j':
      signed10 = true;
      break;
    case 'S':
      /* high = true; */
      error ("Using %S (high) in asm format");
      break;
    case 'P':
      break;
    case 's':
      /* low = true; */
      error ("Using %s (low) in asm format");
      break;
    case 'u':
      u32 = true;
      break;
    case 'm':
      addressing_mode = true;
      break;
    case 'r':
      gcc_assert (CONST_INT_P (x) && INTVAL (x) < 96);
      if (INTVAL (x) < 16)
	fprintf (file, "$ex" HOST_WIDE_INT_PRINT_DEC,
		 (INTVAL (x) - 0) * 2 + 32);
      else if (INTVAL (x) < 32)
	fprintf (file, "$nh" HOST_WIDE_INT_PRINT_DEC,
		 (INTVAL (x) - 16) * 2 + 32);
      else if (INTVAL (x) < 48)
	fprintf (file, "$nv" HOST_WIDE_INT_PRINT_DEC,
		 (INTVAL (x) - 32) * 2 + 32);
      else
	fprintf (file, "$nd" HOST_WIDE_INT_PRINT_DEC,
		 (INTVAL (x) - 48) * 2 + 32);
      return;
    case 'C': /* Print an additional 'u' in the case of uncached load/store */
      if (k1_is_uncached_mem_op_p (x))
	fprintf (file, ".u");
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
      else if (quadword_reg > 0)
	{
	  if ((quadword_reg == 1) && GET_MODE_SIZE (GET_MODE (x)) <= 16)
	    {
	      warning (0, "using %%q format with non-quad operand");
	    }
	  fprintf (file, "$%s%s", reg_names[REGNO (operand)],
		   reg_names[REGNO (operand) + 1]);
	}
      else if (octupleword_reg > 0)
	{
	  if ((octupleword_reg == 1) && GET_MODE_SIZE (GET_MODE (x)) <= 32)
	    {
	      warning (0, "using %%o format with non-octuple operand");
	    }
	  fprintf (file, "$%s%s%s%s", reg_names[REGNO (operand)],
		   reg_names[REGNO (operand) + 1],
		   reg_names[REGNO (operand) + 2],
		   reg_names[REGNO (operand) + 3]);
	}

      /* else if (low) */
      /*     fprintf (file, "$%s", reg_names[REGNO (operand)]); */
      /* else if (high) */
      /*     fprintf (file, "$%s", reg_names[REGNO (operand)+1]); */

      // FIXME AUTO: coolidge, monoquadruple ?
      else if ((GET_MODE_SIZE (GET_MODE (x)) == 16)
	       && (!system_register_operand (operand, VOIDmode)))
	{
	  gcc_assert (!(REGNO (operand) % 2));
	  fprintf (file, "$r%ir%i", REGNO (operand), REGNO (operand) + 1);
	}
      else
	fprintf (file, "$%s", reg_names[REGNO (operand)]);
      return;

    case MEM:
      if (addressing_mode)
	{
	  x = XEXP (x, 0);
	  if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 0)) == MULT
	      && INTVAL (XEXP (XEXP (x, 0), 1)) > HOST_WIDE_INT_1)
	    {
	      fprintf (file, ".xs");
	    }
	  else if (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 0)) == ASHIFT
		   && INTVAL (XEXP (XEXP (x, 0), 1)) > (HOST_WIDE_INT) 0)
	    {
	      fprintf (file, ".xs");
	    }
	  else if (GET_CODE (x) == PLUS && REG_P (XEXP (x, 1)))
	    {
	      /* fprintf (file, ".x1"); */
	    }
	}
      else
	{
	  x = XEXP (x, 0);
	  if (GET_CODE (x) == UNSPEC)
	    k1_target_print_operand (file, x, 0);
	  else
	    {
	      /* if (want_plt && TARGET_64) */
	      /*   fprintf (file, "@plt64("); */
	      /* else if (want_plt) */
	      /*   fprintf (file, "@plt("); */
	      output_address (GET_MODE (XEXP (operand, 0)), XEXP (operand, 0));
	      /* if (want_plt) */
	      /*   fprintf (file, ")"); */
	    }
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
      output_addr_const (file, operand);
      return;

    case CONST_INT:
      if (lowbit_highbit && (INTVAL (x) < -512 || INTVAL (x) > 511)
	  && k1_lowbit_highbit_constant_p (INTVAL (x), &lowbit, &highbit))
	{
	  fprintf (file, "%i, %i", highbit, lowbit);
	  return;
	}
      /* FIXME, MAKED can take 16 bits immediates */
      /* if (paired_reg) { */
      /*     /\* [JV]: ??? It doesn't work with value 0...: */
      /*        && (INTVAL (x) < -512 || INTVAL (x) > 511)) { */
      /*     *\/ */
      /*     long long val = INTVAL (x); */
      /*     fprintf (file, "0x%x:0x%x", */
      /*              (unsigned int)(val & 0xFFFFFFFF), */
      /*              (unsigned int)(val >> 32)); */
      /*     return; */
      /* } */
      if (u32)
	{
	  /* Unsigned 32 bits value. */
	  fprintf (file, "0x%x", (unsigned int) INTVAL (x));
	}
      else if (signed10)
	{
	  long val = INTVAL (x);
	  if (val < 512)
	    {
	      fprintf (file, HOST_WIDE_INT_PRINT_DEC, INTVAL (x));
	    }
	  else
	    {
	      int ival = (int) val;
	      gcc_assert (__builtin_clz (~ival) >= 23);
	      fprintf (file, "%i", ival);
	    }
	}
      else
	{
	  fprintf (file, HOST_WIDE_INT_PRINT_DEC, INTVAL (x));
	}
      return;

      /* FIXME AUTO: disabling vector support */
      /* case CONST_VECTOR: */
      /*     if (GET_MODE (x) == V2HImode) { */
      /*         gcc_assert (CONST_INT_P (CONST_VECTOR_ELT (x, 0)) */
      /*                     && CONST_INT_P (CONST_VECTOR_ELT (x, 1))); */

      /*         fprintf (file, HOST_WIDE_INT_PRINT_DEC, */
      /*                  (INTVAL (CONST_VECTOR_ELT (x, 0)) & 0xFFFF) */
      /*                  | (INTVAL (CONST_VECTOR_ELT (x, 1)) & 0xFFFF) << 16);
       */
      /*     } else if (GET_MODE (x) == V4HImode) { */
      /*         gcc_assert (CONST_INT_P (CONST_VECTOR_ELT (x, 0)) */
      /*                     && CONST_INT_P (CONST_VECTOR_ELT (x, 1)) */
      /*                     && CONST_INT_P (CONST_VECTOR_ELT (x, 2)) */
      /*                     && CONST_INT_P (CONST_VECTOR_ELT (x, 3))); */

      /*         fprintf (file, "0x%x:0x%x", */
      /*                  (uint32_t)((INTVAL (CONST_VECTOR_ELT (x, 0)) & 0xFFFF)
       */
      /*                             | ((INTVAL (CONST_VECTOR_ELT (x, 1)) &
       * 0xFFFF) << 16)), */
      /*                  (uint32_t)((INTVAL (CONST_VECTOR_ELT (x, 2)) & 0xFFFF)
       */
      /*                             | ((INTVAL (CONST_VECTOR_ELT (x, 3)) &
       * 0xFFFF) << 16))); */
      /*     } else { */
      /*         gcc_assert (GET_MODE (x) == V2SImode || GET_MODE (x) ==
       * V2SFmode); */
      /*         k1_target_print_operand (file, CONST_VECTOR_ELT (x, 0), 'u');
       */
      /*         fprintf (file, ":"); */
      /*         k1_target_print_operand (file, CONST_VECTOR_ELT (x, 1), 'u');
       */
      /*     } */
      /*     return; */

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
	      case UNSPEC_TLS:
		fprintf (file, "@tprel(");
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
  unsigned int regno;

  switch (GET_CODE (x))
    {
    case REG:
      if (GET_MODE (x) == TImode)
	return prf_reg_names[REGNO (x)];
      else if (GET_MODE (x) == OImode)
	return qrf_reg_names[REGNO (x)];
      else
	return reg_names[REGNO (x)];
    case SUBREG:
      gcc_assert (TARGET_32); // FIXME AUTO: don't understand this assert
      gcc_assert (GET_MODE (x) == DImode);
      gcc_assert (GET_MODE (SUBREG_REG (x)) == TImode);
      regno = REGNO (SUBREG_REG (x));
      if (SUBREG_BYTE (x))
	regno++;
      return reg_names[regno];
    default:
      gcc_unreachable ();
    }
}

void
k1_target_print_operand_address (FILE *file, rtx x)
{
  rtx op, reg;

  op = simplify_rtx (x);

  if (op != NULL && k1_target_legitimate_address_p (GET_MODE (op), op, 0))
    x = op;

  /*
      is_tls = k1_has_tls_reference (x);

      if (is_tls)
	  fprintf (file, "@tprel(");
  */
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
	  if (GET_CODE (XEXP (x, 0)) == AND
	      && (GET_CODE (XEXP (XEXP (x, 0), 0)) == MULT
		  || GET_CODE (XEXP (XEXP (x, 0), 0)) == ASHIFT))
	    fprintf (file, "$%s", k1_regname (XEXP (XEXP (XEXP (x, 0), 0), 0)));
	  else if (GET_CODE (XEXP (x, 0)) == AND
		   || GET_CODE (XEXP (x, 0)) == MULT
		   || GET_CODE (XEXP (x, 0)) == ZERO_EXTEND
		   || GET_CODE (XEXP (x, 0)) == ASHIFT)
	    fprintf (file, "$%s", k1_regname (XEXP (XEXP (x, 0), 0)));
	  else
	    fprintf (file, "$%s", k1_regname (XEXP (x, 1)));
	  break;
	default:
	  abort ();
	}
      /*         if (is_tls)
		   fprintf (file, ")");
      */
      if (GET_CODE (XEXP (x, 0)) == MULT || GET_CODE (XEXP (x, 0)) == AND
	  || GET_CODE (XEXP (x, 0)) == ZERO_EXTEND
	  || GET_CODE (XEXP (x, 0)) == ASHIFT)
	reg = XEXP (x, 1);
      else
	reg = XEXP (x, 0);
      fprintf (file, "[$%s]", k1_regname (reg));
      return; /* RETURN here! not break */

    default:
      output_addr_const (file, x);
      break;
    }

  /*     if (is_tls)
	   fprintf (file, ")");
  */
}

static void
k1_file_start ()
{

  /* Variable tracking should be run after all optimizations which change order
     of insns (like machine reorg).  It also needs a valid CFG.  This can't be
     done in k1_override_options, because flag_var_tracking is finalized after
     that.  */
  k1_flag_var_tracking = flag_var_tracking;
}

static void
k1_set_current_function (tree decl ATTRIBUTE_UNUSED)
{

  /* flag_var_tracking might be reset by cl_optimization_restore in
     invoke_set_current_function_hook(). This hook is called right
     after that, reset flag_var_tracking to 0. Reminder: The
     var_tracking pass is run explicitely in our machine_reorg. */

  flag_var_tracking = 0;
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
k1_target_print_punct_valid_p (unsigned char code)
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
      rtx (*gen_addpcrel) (rtx target, rtx label)
	= !TARGET_32 ? gen_add_pcrel_di : gen_add_pcrel_si;
      rtx tmp_reg = gen_reg_rtx (Pmode);
      emit_insn (gen_addpcrel (tmp_reg, op1));

      /* Relative addrs are stored as 32bits value. */
      if (GET_MODE (op0) != Pmode)
	{
	  rtx dreg = gen_reg_rtx (Pmode);
	  emit_insn (gen_extend_insn (dreg, op0, Pmode, GET_MODE (op0), 1));
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

void
k1_emit_stack_overflow_block (rtx *seq, rtx *last)
{
  tree handler_name;
  rtx handler_name_rtx;

  if (cfun->machine->stack_check_block_seq != NULL_RTX)
    {
      *seq = cfun->machine->stack_check_block_seq;
      *last = cfun->machine->stack_check_block_last;
      return;
    }

  handler_name = get_identifier ("__stack_overflow_detected");
  handler_name_rtx
    = gen_rtx_SYMBOL_REF (Pmode, IDENTIFIER_POINTER (handler_name));

  *seq = emit_label (cfun->machine->stack_check_block_label);
  LABEL_NUSES (cfun->machine->stack_check_block_label) = 1;
  *last = emit_call_insn (
    gen_call (gen_rtx_MEM (Pmode, handler_name_rtx), const0_rtx));
  emit_barrier_after (*last);

  cfun->machine->stack_check_block_seq = *seq;
  cfun->machine->stack_check_block_last = *last;
}

rtx
k1_get_stack_check_block (void)
{
  rtx seq, last;

  if (cfun->machine->stack_check_block_seq)
    {
      if (BLOCK_FOR_INSN (cfun->machine->stack_check_block_seq))
	return block_label (
	  BLOCK_FOR_INSN (cfun->machine->stack_check_block_seq));
      else
	return cfun->machine->stack_check_block_label;
    }

  if (cfun->machine->stack_check_block_label == NULL_RTX)
    cfun->machine->stack_check_block_label = gen_label_rtx ();

  if (currently_expanding_to_rtl)
    return cfun->machine->stack_check_block_label;

  push_topmost_sequence ();
  k1_emit_stack_overflow_block (&seq, &last);
  pop_topmost_sequence ();
  create_basic_block (seq, last, EXIT_BLOCK_PTR_FOR_FN (cfun)->prev_bb);

  return cfun->machine->stack_check_block_label;
}

void
k1_expand_stack_check_allocate_stack (rtx target, rtx adjust)
{
  int saved_stack_pointer_delta;
  tree stack_end = get_identifier ("__stack_end");
  rtx stack_end_sym
    = gen_rtx_SYMBOL_REF (Pmode, IDENTIFIER_POINTER (stack_end));
  rtx stack_end_val = gen_reg_rtx (Pmode), tmp = gen_reg_rtx (Pmode);
  rtx label, seq, last, jump_over;

  saved_stack_pointer_delta = stack_pointer_delta;

  anti_adjust_stack (adjust);

  /* Even if size is constant, don't modify stack_pointer_delta.
     The constant size alloca should preserve
     crtl->preferred_stack_boundary alignment.  */
  stack_pointer_delta = saved_stack_pointer_delta;

  emit_move_insn (target, virtual_stack_dynamic_rtx);

  label = k1_get_stack_check_block ();

  if (TARGET_STACK_CHECK_USE_TLS)
    {
      stack_end_sym
	= gen_rtx_UNSPEC (Pmode, gen_rtvec (1, stack_end_sym), UNSPEC_TLS);
      emit_move_insn (
	stack_end_val,
	gen_rtx_MEM (Pmode,
		     gen_rtx_PLUS (Pmode,
				   gen_rtx_REG (Pmode, K1C_LOCAL_POINTER_REGNO),
				   gen_rtx_CONST (Pmode, stack_end_sym))));
    }
  else
    {
      /* [JV] Multiring: Waiting for allocated VSFR0 and maturity. */
      /*BD3 emit_move_insn (stack_end_val,
		      gen_rtx_REG (Pmode, K1C_VSFR0_REGNO));*/
      gcc_unreachable ();
    }
  emit_insn (
    gen_rtx_SET (tmp, gen_rtx_MINUS (Pmode, stack_pointer_rtx, stack_end_val)));
  emit_cmp_and_jump_insns (tmp, const0_rtx, LT, NULL_RTX, Pmode, 0, label);
  JUMP_LABEL (get_last_insn ()) = label;

  jump_over = gen_label_rtx ();
  emit_jump_insn (gen_jump (jump_over));
  JUMP_LABEL (get_last_insn ()) = jump_over;
  k1_emit_stack_overflow_block (&seq, &last);
  emit_label (jump_over);
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

/* Returns a REG rtx with a hard reg that is safe to use in prologue
   (caller-saved and non fixed reg). Returns NULL_RTX and emits an
   error if no such register can be found. */
static rtx
k1_get_callersaved_nonfixed_reg (machine_mode mode)
{
  int regno;
  // start at R16 as as everything before that may be used.
  // We should be able to use the veneer regs if not fixed.
  for (regno = 16; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      if (call_really_used_regs[regno] && !fixed_regs[regno])
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
  unsigned regno2;

  for (regno = 0; regno < limit; regno++)
    {
      if (k1_register_saved_on_entry (regno))
	{
	  rtx mem;

	  mem = gen_mem_ref (DImode, plus_constant (Pmode, base_rtx,
						    frame->reg_offset[regno]));

	  regno2 = k1_register_saved_on_entry (regno + 1) ? regno + 1 : 0;

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
		saved_reg = k1_get_callersaved_nonfixed_reg (spill_mode);
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
  HOST_WIDE_INT src, dest;
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

void
k1_expand_prologue (void)
{
  k1_compute_frame_info ();
  struct k1_frame_info *frame = &cfun->machine->frame;
  HOST_WIDE_INT size = frame->initial_sp_offset;
  rtx insn;

  if (size > 0)
    {
      insn = emit_insn (gen_add2_insn (stack_pointer_rtx, GEN_INT (-size)));
      RTX_FRAME_RELATED_P (insn) = 1;

      add_reg_note (insn, REG_CFA_ADJUST_CFA, PATTERN (insn));
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

      rtx insn
	= emit_move_insn (gen_rtx_REG (DImode, HARD_FRAME_POINTER_REGNUM),
			  fp_mem);
    }

  k1_save_or_restore_callee_save_registers (1);

  if (frame_size != 0)
    {
      insn = GEN_INT (frame_size);
      insn = emit_insn (gen_add2_insn (stack_pointer_rtx, insn));
      RTX_FRAME_RELATED_P (insn) = 1;
      add_reg_note (insn, REG_CFA_ADJUST_CFA, PATTERN (insn));
    }
}

static rtx
k1_legitimize_tls_reference (rtx x)
{
  rtx reg;
  rtx (*gen_add) (rtx target, rtx op1, rtx op2)
    = !TARGET_32 ? gen_adddi3 : gen_addsi3;

  if (reload_completed || reload_in_progress)
    return x;

  reg = gen_reg_rtx (Pmode);
  emit_move_insn (reg,
		  gen_rtx_CONST (Pmode, gen_rtx_UNSPEC (Pmode, gen_rtvec (1, x),
							UNSPEC_TLS)));

  emit_insn (gen_add (reg, gen_rtx_REG (Pmode, K1C_LOCAL_POINTER_REGNO), reg));
  return reg;
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

/* Returns TRUE if OP contains a symbol or label reference */
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
  if (GET_CODE (op) == UNSPEC && XINT (op, 1) == UNSPEC_TLS)
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
	  || unspec == UNSPEC_TLS || unspec == UNSPEC_PCREL);
}

/* Returns TRUE if X can be used as an operand in PIC code.
   Mainly */
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

enum k1_symbol_type
k1_classify_symbol (rtx x)
{
  if (k1_tls_symbol_p (x))
    return SYMBOL_TPREL;

  if (GET_CODE (x) == LABEL_REF)
    return flag_pic ? LABEL_PCREL_ABSOLUTE : SYMBOL_ABSOLUTE;

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
      rtx mem, base, offset;
      enum k1_symbol_type sty;
      rtx pic_reg;
      rtx (*gen_set_gotp) (rtx target)
	= !TARGET_32 ? gen_set_gotp_di : gen_set_gotp_si;
      rtx (*gen_addpcrel) (rtx target, rtx label)
	= !TARGET_32 ? gen_add_pcrel_di : gen_add_pcrel_si;

      /* If we have (const (plus symbol offset)), separate out the offset
	 before we start classifying the symbol.  */
      split_const (src, &base, &offset);

      sty = k1_classify_symbol (base);
      switch (sty)
	{
	case SYMBOL_ABSOLUTE:
	  /* Emit: dest = sym */
	  emit_insn (gen_rtx_SET (dest, src));
	  break;

	case LABEL_PCREL_ABSOLUTE:
	  /* Emit: dest = @pcrel(sym) */
	  emit_insn (gen_addpcrel (dest, XEXP (base, 0)));

	  if (INTVAL (offset) != 0)
	    emit_insn (gen_add2_insn (dest, offset));

	  break;

	case SYMBOL_GOT:
	  /*
	   * Emit dest = *(@got(sym) + $pic)
	   */
	  pic_reg = gen_reg_rtx (Pmode);
	  emit_insn (gen_set_gotp (pic_reg));

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
	  emit_insn (gen_set_gotp (pic_reg));

	  new_rtx
	    = gen_rtx_CONST (Pmode, gen_rtx_UNSPEC (Pmode, gen_rtvec (1, base),
						    UNSPEC_GOTOFF));

	  emit_move_insn (dest, pic_reg);

	  emit_move_insn (dest, gen_rtx_PLUS (Pmode, dest, new_rtx));

	  if (INTVAL (offset) != 0)
	    emit_insn (gen_add2_insn (dest, offset));

	  crtl->uses_pic_offset_table = true;
	  break;

	case SYMBOL_TPREL:
	  operands[1] = k1_legitimize_tls_reference (src);
	  emit_insn (gen_rtx_SET (dest, operands[1]));
	  gcc_assert (operands[1] != src);
	  break;
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
  /* 	operands[1] = k1_target_legitimize_pic_address (operands[1],
   * operands[0]); */
  /*   } */
  return;
}

/* FIXME AUTO: fix cost function for coolidge */
/* See T7748 */
static int
k1_target_register_move_cost (enum machine_mode mode,
			      reg_class_t from ATTRIBUTE_UNUSED,
			      reg_class_t to ATTRIBUTE_UNUSED)
{
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
  if (GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
    return 5;
  else if (GET_MODE_SIZE (mode) == 2 * UNITS_PER_WORD)
    return 10;
  else
    return INT_MAX;
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

// FIXME AUTO: disabling legacy code for atomic
/* enum sync_insn_type { SYNC_OLD, SYNC_NEW, SYNC }; */

/* static void */
/* k1_generic_expand_sync_instruction (enum rtx_code code, rtx dest, rtx addr,
 * rtx val, */
/*                                     enum sync_insn_type type) */
/* { */
/*     rtx reg = gen_reg_rtx (DImode); */
/*     rtx res = gen_reg_rtx (SImode); */
/*     rtx label = gen_label_rtx (); */
/*     rtx lowpart = gen_lowpart (SImode, reg); */
/*     rtx highpart = gen_highpart (SImode, reg); */

/*     /\* Force a register operand. If that's not necessary, a following */
/*        pass will simplify it. *\/ */
/*     if (!REG_P (val)) */
/*         val = force_reg (SImode, val); */

/*     emit_insn (gen_rtx_CLOBBER (DImode, reg)); */
/*     emit_insn (gen_memory_barrier ()); */
/*     emit_insn (gen_lwzu (lowpart, addr)); */
/*     emit_label (label); */
/*     emit_move_insn (highpart, lowpart); */

/*     switch (code) { */
/*     case PLUS: */
/*         emit_insn (gen_addsi3 (lowpart, highpart, val)); break; */
/*     case MINUS: */
/*         emit_insn (gen_sub3_insn (lowpart, highpart, val)); break; */
/*     case IOR: */
/*         emit_insn (gen_iorsi3 (lowpart, highpart, val)); break; */
/*     case AND: */
/*         emit_insn (gen_andsi3 (lowpart, highpart, val)); break; */
/*     case XOR: */
/*         emit_insn (gen_xorsi3 (lowpart, highpart, val)); break; */
/*     case NOT: */
/*         emit_insn (gen_nand (lowpart, highpart, val)); break; */
/*     case SET: */
/*         emit_move_insn (lowpart, val); break; */
/*     default: */
/*         gcc_unreachable (); */
/*     }; */

/*     if (type == SYNC_NEW) */
/*         emit_move_insn (res, lowpart); */

/*     emit_insn (gen_cws (reg, addr, reg)); */
/*     emit_cmp_and_jump_insns (lowpart, highpart, NE, NULL_RTX, SImode, 0,
 * label); */

/*     if (dest) { */
/*         if (type == SYNC_NEW) */
/*             emit_move_insn (dest, res); */
/*         else if (type == SYNC_OLD) */
/*             emit_move_insn (dest, lowpart); */
/*     } */
/* } */

/* void */
/* k1_expand_old_sync_instruction (enum rtx_code code, rtx dest, rtx addr, rtx
 * val) */
/* { */
/*     k1_generic_expand_sync_instruction (code, dest, addr, val, SYNC_OLD); */
/* } */

/* void */
/* k1_expand_new_sync_instruction (enum rtx_code code, rtx dest, rtx addr, rtx
 * val) */
/* { */
/*     k1_generic_expand_sync_instruction (code, dest, addr, val, SYNC_NEW); */
/* } */

/* void */
/* k1_expand_sync_instruction (enum rtx_code code, rtx addr, rtx val) */
/* { */
/*     k1_generic_expand_sync_instruction (code, NULL, addr, val, SYNC); */
/* } */

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
  K1_BUILTIN_FABSD,
  K1_BUILTIN_FNEGW,
  K1_BUILTIN_FNEGD,
  K1_BUILTIN_FMAXW,
  K1_BUILTIN_FMAXD,
  K1_BUILTIN_FMINW,
  K1_BUILTIN_FMIND,
  K1_BUILTIN_FINVW,
  K1_BUILTIN_FISRW,
  K1_BUILTIN_FADDW,
  K1_BUILTIN_FADDD,
  K1_BUILTIN_FSBFW,
  K1_BUILTIN_FSBFD,
  K1_BUILTIN_FMULW,
  K1_BUILTIN_FMULD,
  K1_BUILTIN_FMULWD,
  K1_BUILTIN_FFMAW,
  K1_BUILTIN_FFMAD,
  K1_BUILTIN_FFMAWD,
  K1_BUILTIN_FFMSW,
  K1_BUILTIN_FFMSD,
  K1_BUILTIN_FFMSWD,

  K1_BUILTIN_FCDIVW,
  K1_BUILTIN_FCDIVD,
  K1_BUILTIN_FENCE,
  K1_BUILTIN_FFMSWP,
  K1_BUILTIN_FLOAT,
  K1_BUILTIN_FLOATD,
  K1_BUILTIN_FLOATU,
  K1_BUILTIN_FLOATUD,
  K1_BUILTIN_FLOATWP,
  K1_BUILTIN_FLOATUWP,
  K1_BUILTIN_FIXED,
  K1_BUILTIN_FIXEDD,
  K1_BUILTIN_FIXEDU,
  K1_BUILTIN_FIXEDUD,
  K1_BUILTIN_FIXEDWP,
  K1_BUILTIN_FIXEDUWP,
  K1_BUILTIN_FMULWP,
  K1_BUILTIN_FMULRNWP,
  K1_BUILTIN_FSBFWP,
  K1_BUILTIN_FSBFWWP,
  K1_BUILTIN_FSDIVW,
  K1_BUILTIN_FSDIVD,
  K1_BUILTIN_FSINVW,
  K1_BUILTIN_FSINVD,
  K1_BUILTIN_FSISRW,
  K1_BUILTIN_FSISRD,
  K1_BUILTIN_GET,
  /* K1_BUILTIN_GET_R, */
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
  K1_BUILTIN_SET_PS,
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
k1_target_init_builtins (void)
{
  tree vect2SF = build_vector_type (float_type_node, 2);
  tree vect2SI = build_vector_type (intSI_type_node, 2);
  tree vect2HI = build_vector_type (intHI_type_node, 2);
  tree unsigned_vect2HI = build_vector_type (unsigned_intHI_type_node, 2);

  tree intQI_pointer_node;
  tree intHI_pointer_node;
  tree intSI_pointer_node;
  tree intDI_pointer_node;
  tree float_pointer_node;

  tree const_intQI_node;
  tree const_intHI_node;
  tree const_intSI_node;
  tree const_intDI_node;
  tree const_float_node;

  tree const_intQI_pointer_node;
  tree const_intHI_pointer_node;
  tree const_intSI_pointer_node;
  tree const_intDI_pointer_node;
  tree const_float_pointer_node;

  tree unsigned_vect2HI_pointer_node;

  intQI_pointer_node = build_pointer_type (intQI_type_node);
  intHI_pointer_node = build_pointer_type (intHI_type_node);
  intSI_pointer_node = build_pointer_type (intSI_type_node);
  intDI_pointer_node = build_pointer_type (intDI_type_node);
  float_pointer_node = build_pointer_type (float_type_node);

  const_intQI_node = build_qualified_type (intQI_type_node, TYPE_QUAL_CONST);
  const_intHI_node = build_qualified_type (intHI_type_node, TYPE_QUAL_CONST);
  const_intSI_node = build_qualified_type (intSI_type_node, TYPE_QUAL_CONST);
  const_intDI_node = build_qualified_type (intDI_type_node, TYPE_QUAL_CONST);
  const_float_node = build_qualified_type (float_type_node, TYPE_QUAL_CONST);

  const_intQI_pointer_node = build_pointer_type (const_intQI_node);
  const_intHI_pointer_node = build_pointer_type (const_intHI_node);
  const_intSI_pointer_node = build_pointer_type (const_intSI_node);
  const_intDI_pointer_node = build_pointer_type (const_intDI_node);
  const_float_pointer_node = build_pointer_type (const_float_node);

  unsigned_vect2HI_pointer_node = build_pointer_type (unsigned_vect2HI);

#define uintQI unsigned_intQI_type_node
#define uintHI unsigned_intHI_type_node
#define uintSI unsigned_intSI_type_node
#define uintDI unsigned_intDI_type_node
#define uintTI unsigned_intTI_type_node

#define intQI intQI_type_node
#define intHI intHI_type_node
#define intSI intSI_type_node
#define intDI intDI_type_node
#define intTI intTI_type_node

#define floatSF float_type_node
#define floatDF double_type_node
#define VOID void_type_node

#define voidPTR ptr_type_node
#define constVoidPTR const_ptr_type_node

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

  /* FIXME AUTO: disabling vector support */
  /* ADD_K1_BUILTIN (ABDHP,   "abdhp",       intSI,  intSI, intSI); */
  /* ADD_K1_BUILTIN (ADDHP,   "addhp",       intSI,  intSI, intSI); */

  ADD_K1_BUILTIN (ABDW, "abdw", intSI, intSI, intSI);
  ADD_K1_BUILTIN (ABDD, "abdd", intDI, intDI, intDI);
  ADD_K1_BUILTIN (ADDSW, "addsw", intSI, intSI, intSI);
  ADD_K1_BUILTIN (ADDSD, "addsd", intDI, intDI, intDI);
  ADD_K1_BUILTIN (SBFSW, "sbfsw", intSI, intSI, intSI);
  ADD_K1_BUILTIN (SBFSD, "sbfsd", intDI, intDI, intDI);
  ADD_K1_BUILTIN (AVGW, "avgw", intSI, intSI, intSI);
  ADD_K1_BUILTIN (AVGUW, "avguw", uintSI, uintSI, uintSI);
  ADD_K1_BUILTIN (AVGRW, "avgrw", intSI, intSI, intSI);
  ADD_K1_BUILTIN (AVGRUW, "avgruw", uintSI, uintSI, uintSI);
  ADD_K1_BUILTIN (AWAIT, "await", VOID);
  ADD_K1_BUILTIN (BARRIER, "barrier", VOID);
  ADD_K1_BUILTIN (CBSW, "cbsw", intSI, uintSI);
  ADD_K1_BUILTIN (CBSD, "cbsd", intDI, uintDI);
  ADD_K1_BUILTIN (CLZW, "clzw", intSI, uintSI);
  ADD_K1_BUILTIN (CLZD, "clzd", intDI, uintDI);
  ADD_K1_BUILTIN (CTZW, "ctzw", intSI, uintSI);
  ADD_K1_BUILTIN (CTZD, "ctzd", intDI, uintDI);
  ADD_K1_BUILTIN (ACSWAPW, "acswapw", uintTI, voidPTR, uintDI, uintDI);
  ADD_K1_BUILTIN (ACSWAPD, "acswapd", uintTI, voidPTR, uintDI, uintDI);
  ADD_K1_BUILTIN (AFADDD, "afaddd", uintDI, voidPTR, intDI);
  ADD_K1_BUILTIN (AFADDW, "afaddw", uintSI, voidPTR, intSI);
  ADD_K1_BUILTIN (ALCLRD, "alclrd", uintDI, voidPTR);
  ADD_K1_BUILTIN (ALCLRW, "alclrw", uintSI, voidPTR);
  ADD_K1_BUILTIN (DINVAL, "dinval", VOID);
  ADD_K1_BUILTIN (DINVALL, "dinvall", VOID, constVoidPTR);
  ADD_K1_BUILTIN (DOZE, "doze", VOID);
  ADD_K1_BUILTIN (DTOUCHL, "dtouchl", VOID, constVoidPTR);
  ADD_K1_BUILTIN (DZEROL, "dzerol", VOID, voidPTR);
  ADD_K1_BUILTIN (EXTFZ, "extfz", uintSI, uintSI, uintSI, uintSI);

  ADD_K1_BUILTIN (FABSW, "fabsw", floatSF, floatSF);
  ADD_K1_BUILTIN (FABSD, "fabsd", floatDF, floatDF);
  ADD_K1_BUILTIN (FNEGW, "fnegw", floatSF, floatSF);
  ADD_K1_BUILTIN (FNEGD, "fnegd", floatDF, floatDF);
  ADD_K1_BUILTIN (FMAXW, "fmaxw", floatSF, floatSF, floatSF);
  ADD_K1_BUILTIN (FMAXD, "fmaxd", floatDF, floatDF, floatDF);
  ADD_K1_BUILTIN (FMINW, "fminw", floatSF, floatSF, floatSF);
  ADD_K1_BUILTIN (FMIND, "fmind", floatDF, floatDF, floatDF);
  ADD_K1_BUILTIN (FINVW, "finvw", floatSF, floatSF, uintSI, uintSI);
  ADD_K1_BUILTIN (FISRW, "fisrw", floatSF, floatSF, uintSI, uintSI);
  ADD_K1_BUILTIN (FADDW, "faddw", floatSF, floatSF, floatSF, uintSI, uintSI);
  ADD_K1_BUILTIN (FADDD, "faddd", floatDF, floatDF, floatDF, uintSI, uintSI);
  ADD_K1_BUILTIN (FSBFW, "fsbfw", floatSF, floatSF, floatSF, uintSI, uintSI);
  ADD_K1_BUILTIN (FSBFD, "fsbfd", floatDF, floatDF, floatDF, uintSI, uintSI);
  ADD_K1_BUILTIN (FMULW, "fmulw", floatSF, floatSF, floatSF, uintSI, uintSI);
  ADD_K1_BUILTIN (FMULD, "fmuld", floatDF, floatDF, floatDF, uintSI, uintSI);
  ADD_K1_BUILTIN (FMULWD, "fmulwd", floatDF, floatSF, floatSF, uintSI, uintSI);
  ADD_K1_BUILTIN (FFMAW, "ffmaw", floatSF, floatSF, floatSF, floatSF, uintSI,
		  uintSI);
  ADD_K1_BUILTIN (FFMAD, "ffmad", floatDF, floatDF, floatDF, floatDF, uintSI,
		  uintSI);
  ADD_K1_BUILTIN (FFMAWD, "ffmawd", floatDF, floatDF, floatSF, floatSF, uintSI,
		  uintSI);
  ADD_K1_BUILTIN (FFMSW, "ffmsw", floatSF, floatSF, floatSF, floatSF, uintSI,
		  uintSI);
  ADD_K1_BUILTIN (FFMSD, "ffmsd", floatDF, floatDF, floatDF, floatDF, uintSI,
		  uintSI);
  ADD_K1_BUILTIN (FFMSWD, "ffmswd", floatDF, floatDF, floatSF, floatSF, uintSI,
		  uintSI);

  ADD_K1_BUILTIN (FCDIVW, "fcdivw", floatSF, floatSF, floatSF);
  ADD_K1_BUILTIN (FCDIVD, "fcdivd", floatDF, floatDF, floatDF);
  ADD_K1_BUILTIN (FENCE, "fence", VOID);

  ADD_K1_BUILTIN (FLOAT, "float", floatSF, uintQI, intSI, uintQI);
  ADD_K1_BUILTIN (FLOATD, "floatd", floatDF, uintQI, intDI, uintQI);
  ADD_K1_BUILTIN (FLOATU, "floatu", floatSF, uintQI, uintSI, uintQI);
  ADD_K1_BUILTIN (FLOATUD, "floatud", floatDF, uintQI, uintDI, uintQI);
  ADD_K1_BUILTIN (FIXED, "fixed", intSI, uintQI, floatSF, uintQI);
  ADD_K1_BUILTIN (FIXEDD, "fixedd", intDI, uintQI, floatDF, uintQI);
  ADD_K1_BUILTIN (FIXEDU, "fixedu", uintSI, uintQI, floatSF, uintQI);
  ADD_K1_BUILTIN (FIXEDUD, "fixedud", uintDI, uintQI, floatDF, uintQI);

  ADD_K1_BUILTIN (FSDIVW, "fsdivw", floatSF, floatSF, floatSF);
  ADD_K1_BUILTIN (FSDIVD, "fsdivd", floatDF, floatDF, floatDF);
  ADD_K1_BUILTIN (FSINVW, "fsinvw", floatSF, floatSF);
  ADD_K1_BUILTIN (FSINVD, "fsinvd", floatDF, floatDF);
  ADD_K1_BUILTIN (FSISRW, "fsisrw", floatSF, floatSF);
  ADD_K1_BUILTIN (FSISRD, "fsisrd", floatDF, floatDF);
  ADD_K1_BUILTIN (GET, "get", uintDI, intSI);
  /* FIXME AUTO: Disabling get builtin. Ref T7705 */
  /* ADD_K1_BUILTIN (GET_R,   "get_r",       uintDI,  intSI); */
  ADD_K1_BUILTIN (WFXL, "wfxl", VOID, uintQI, uintDI);
  ADD_K1_BUILTIN (WFXM, "wfxm", VOID, uintQI, uintDI);
  ADD_K1_BUILTIN (IINVAL, "iinval", VOID);
  ADD_K1_BUILTIN (IINVALS, "iinvals", VOID, constVoidPTR);
  ADD_K1_BUILTIN (LBSU, "lbsu", intQI, constVoidPTR);
  ADD_K1_BUILTIN (LBZU, "lbzu", uintQI, constVoidPTR);
  ADD_K1_BUILTIN (LHSU, "lhsu", intHI, constVoidPTR);
  ADD_K1_BUILTIN (LHZU, "lhzu", uintHI, constVoidPTR);
  ADD_K1_BUILTIN (LDU, "ldu", uintDI, constVoidPTR);
  ADD_K1_BUILTIN (LWZU, "lwzu", uintSI, constVoidPTR);
  ADD_K1_BUILTIN (SBMM8, "sbmm8", uintDI, uintDI, uintDI);
  ADD_K1_BUILTIN (SBMMT8, "sbmmt8", uintDI, uintDI, uintDI);
  ADD_K1_BUILTIN (SATD, "satd", intDI, intDI, uintQI);
  ADD_K1_BUILTIN (SATUD, "satud", uintDI, intDI, uintQI);
  ADD_K1_BUILTIN (SET, "set", VOID, intSI, uintDI);
  ADD_K1_BUILTIN (SET_PS, "set_ps", VOID, intSI, uintDI);

  ADD_K1_BUILTIN (SLEEP, "sleep", VOID);
  ADD_K1_BUILTIN (STOP, "stop", VOID);
  ADD_K1_BUILTIN (STSUW, "stsuw", uintSI, uintSI, uintSI);
  ADD_K1_BUILTIN (STSUD, "stsud", uintDI, uintDI, uintDI);
  ADD_K1_BUILTIN (SYNCGROUP, "syncgroup", VOID, uintDI);
  ADD_K1_BUILTIN (TLBDINVAL, "tlbdinval", VOID);
  ADD_K1_BUILTIN (TLBIINVAL, "tlbiinval", VOID);
  ADD_K1_BUILTIN (TLBPROBE, "tlbprobe", VOID);
  ADD_K1_BUILTIN (TLBREAD, "tlbread", VOID);
  ADD_K1_BUILTIN (TLBWRITE, "tlbwrite", VOID);

  ADD_K1_BUILTIN (FWIDENLHW, "fwidenlhw", floatSF, uintSI);
  ADD_K1_BUILTIN (FWIDENMHW, "fwidenmhw", floatSF, uintSI);

  ADD_K1_BUILTIN (FNARROWWH, "fnarrowwh", uintHI, floatSF);
  ADD_K1_BUILTIN (WAITIT, "waitit", uintSI, uintSI);
}

static tree
k1_target_builtin_decl (unsigned code, bool initialize_p ATTRIBUTE_UNUSED)
{
  if (code >= ARRAY_SIZE (builtin_fndecls))
    return error_mark_node;
  return builtin_fndecls[code];
}

char k1_modifiers[64];

static void
k1_verify_modifier_rounding (rtx arg, const char *mnemonic,
			     const char *position)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned bits = 3;
      unsigned shift = 64 - bits;
      unsigned mask = (1 << bits) - 1;
      long long tmp = INTVAL (arg);
      signed long long stmp = tmp;
      unsigned long long utmp = tmp;
      if (tmp == (stmp << shift) >> shift || tmp == (utmp << shift) >> shift)
	{
	  switch ((unsigned) tmp & mask)
	    {
	    case 0:
	      strcat (k1_modifiers, ".rn");
	      break;
	    case 1:
	      strcat (k1_modifiers, ".ru");
	      break;
	    case 2:
	      strcat (k1_modifiers, ".rd");
	      break;
	    case 3:
	      strcat (k1_modifiers, ".rz");
	      break;
	    case 4:
	      strcat (k1_modifiers, ".rna");
	      break;
	    case 5:
	      strcat (k1_modifiers, ".rnz");
	      break;
	    case 6:
	      strcat (k1_modifiers, ".ro");
	      break;
	    default:
	      break;
	    }
	  return;
	}
    }
  error ("__builtin_k1_%s expects a 3-bit field immediate %s argument.",
	 mnemonic, position);
}

static void
k1_verify_modifier_silent (rtx arg, const char *mnemonic, const char *position)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned long long tmp = INTVAL (arg);
      if (tmp == 0LL || tmp == 1LL)
	{
	  if (tmp == 1LL)
	    strcat (k1_modifiers, ".s");
	  return;
	}
    }
  error ("__builtin_k1_%s expects a boolean immediate %s argument.", mnemonic,
	 position);
}

static const char *
k1_modifiers_lookup (void)
{
  static const char *table[] = {
    ".rn",  ".rn.s",  ".ru",  ".ru.s",	".rd", ".rd.s", ".rz", ".rz.s",
    ".rna", ".rna.s", ".rnz", ".rnz.s", ".ro", ".ro.s", "",    ".s",
  };
  int i;
  for (i = 0; i < sizeof (table) / sizeof (*table); i++)
    {
      if (!strcmp (k1_modifiers, table[i]))
	return table[i];
    }
  error ("__builtin_k1 modifiers %s not found.", k1_modifiers);
  return 0;
}

static bool
verify_const_bool_arg (rtx arg)
{
  if (GET_CODE (arg) != CONST_INT || GET_MODE (arg) != VOIDmode)
    return false;
  unsigned long long tmp = INTVAL (arg);
  return tmp == 0LL || tmp == 1LL;
}

static bool
verify_const_field_arg (rtx arg, int bits)
{
  if (GET_CODE (arg) != CONST_INT || GET_MODE (arg) != VOIDmode)
    return false;
  unsigned shift = 64 - bits;
  long long tmp = INTVAL (arg);
  signed long long stmp = tmp;
  unsigned long long utmp = tmp;
  if (tmp == (stmp << shift) >> shift)
    return true;
  if (tmp == (utmp << shift) >> shift)
    return true;
  return false;
}

static bool
verify_const_int_arg (rtx arg, int bits, bool signed_p)
{
  if (GET_CODE (arg) != CONST_INT
      && (GET_CODE (arg) != CONST_DOUBLE || GET_MODE (arg) != VOIDmode))
    return false;

  if (signed_p)
    {
      signed long long tmp
	= CONST_INT_P (arg) ? INTVAL (arg) : CONST_DOUBLE_LOW (arg);
      return tmp >= ~((1LL << (bits - 1)) - 1)
	     && tmp <= ((1LL << (bits - 1)) - 1);
    }
  else
    {
      unsigned long long tmp
	= (unsigned int) (CONST_INT_P (arg) ? INTVAL (arg)
					    : CONST_DOUBLE_LOW (arg));
      return tmp <= (1ULL << bits) - 1;
    }
}

static rtx
k1_expand_builtin_get (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx reg;

  if (!verify_const_int_arg (arg1, 9, false))
    {
      error ("__builtin_k1_get expects a 9-bit unsigned immediate argument.");
      return NULL_RTX;
    }
  const int regno = INTVAL (arg1) + K1C_SRF_FIRST_REGNO;

  if (regno > K1C_SRF_LAST_REGNO)
    {
      error ("__builtin_k1_get called with illegal SRF register index : %d",
	     INTVAL (arg1));
    }

  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);

  reg = gen_rtx_REG (DImode, regno);

  if (INTVAL (arg1) == K1C_PCR_REGNO - K1C_SRF_FIRST_REGNO)
    emit_move_insn (target, reg);
  else
    emit_insn (gen_get_volatile (target, reg, k1_sync_reg_rtx));

  return target;
}

/* FIXME AUTO: Disabling get builtin. Ref T7705 */
/* static rtx */
/* k1_expand_builtin_get_r (rtx target, tree args) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */

/*     if (!target) */
/*         target = gen_reg_rtx (DImode); */

/*     target = force_reg (DImode, target); */
/*     arg1 = force_reg (DImode, arg1); */
/*     emit_insn (gen_getdi_r (target, arg1, k1_sync_reg_rtx)); */

/*     return target; */
/* } */

static rtx
k1_expand_builtin_set (rtx target ATTRIBUTE_UNUSED, tree args, bool ps)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!verify_const_int_arg (arg1, 9, false))
    {
      error (
	"__builtin_k1_set expects a 9-bit unsigned immediate first argument.");
      return NULL_RTX;
    }

  if (ps && INTVAL (arg1) != K1C_PS_REGNO - K1C_SRF_FIRST_REGNO)
    {
      error ("__builtin_k1_set_ps must be called on the $ps register.");
    }
  int regno = INTVAL (arg1) + K1C_SRF_FIRST_REGNO;

  if (regno > K1C_SRF_LAST_REGNO)
    {
      error ("__builtin_k1_set called with illegal SRF register index : %d",
	     INTVAL (arg1));
    }

  if (!ps && INTVAL (arg1) == K1C_PS_REGNO - K1C_SRF_FIRST_REGNO)
    {
      ps = true;
    }

  arg2 = force_reg (DImode, arg2);
  rtx sys_reg = gen_rtx_REG (DImode, regno);

  if (ps)
    emit_insn (gen_set_ps_volatile (sys_reg, arg2, k1_sync_reg_rtx));
  else
    emit_insn (gen_set_volatile (sys_reg, arg2, k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_waitit (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);
  emit_insn (gen_waitit (target, arg1, k1_sync_reg_rtx));

  return target;
}

static rtx
k1_expand_builtin_wfxl (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  if (!verify_const_int_arg (arg1, 9, false))
    {
      error (
	"__builtin_k1_wfxl expects a 9-bit unsigned immediate first argument.");
      return NULL_RTX;
    }
  int regno = INTVAL (arg1) + K1C_SRF_FIRST_REGNO;
  rtx arg = gen_rtx_REG (DImode, regno);
  arg2 = force_reg (DImode, arg2);
  if (INTVAL (arg1) == K1C_PS_REGNO - K1C_SRF_FIRST_REGNO)
    {
      emit_insn (gen_wfxl_ps (arg, arg2, k1_sync_reg_rtx));
    }
  else
    {
      emit_insn (gen_wfxl (arg, arg2, k1_sync_reg_rtx));
    }
  return NULL_RTX;
}

static rtx
k1_expand_builtin_wfxm (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  if (!verify_const_int_arg (arg1, 9, false))
    {
      error (
	"__builtin_k1_wfxm expects a 9-bit unsigned immediate first argument.");
      return NULL_RTX;
    }
  int regno = INTVAL (arg1) + K1C_SRF_FIRST_REGNO;
  rtx arg = gen_rtx_REG (DImode, regno);
  arg2 = force_reg (DImode, arg2);
  if (INTVAL (arg1) == K1C_PS_REGNO - K1C_SRF_FIRST_REGNO)
    {
      emit_insn (gen_wfxm_ps (arg, arg2, k1_sync_reg_rtx));
    }
  else
    {
      emit_insn (gen_wfxm (arg, arg2, k1_sync_reg_rtx));
    }
  return NULL_RTX;
}

static rtx
k1_expand_builtin_sbmm8 (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (DImode);
  target = force_reg (DImode, target);
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);
  emit_insn (gen_sbmm8 (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_sbmmt8 (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (DImode);
  target = force_reg (DImode, target);
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);
  emit_insn (gen_sbmmt8 (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_await (rtx target ATTRIBUTE_UNUSED, tree args)
{
  emit_insn (gen_await (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_doze (rtx target ATTRIBUTE_UNUSED, tree args)
{
  emit_insn (gen_doze (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_sleep (rtx target ATTRIBUTE_UNUSED, tree args)
{
  emit_insn (gen_sleep (k1_sync_reg_rtx));

  return NULL_RTX;
}

static rtx
k1_expand_builtin_stop (rtx target ATTRIBUTE_UNUSED, tree args)
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

  if (!target)
    target = gen_reg_rtx (DImode);
  target = force_reg (DImode, target);
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (SImode, arg2);
  emit_insn (gen_satd (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_satud (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (DImode);
  target = force_reg (DImode, target);
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (SImode, arg2);
  emit_insn (gen_satud (target, arg1, arg2));
  return target;
}

/*
  static rtx
  k1_expand_builtin_scall (rtx target, tree args)
  {
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg;
  int i;

  if (!verify_const_int_arg (arg1, 12, false)) {
  error ("__builtin_k1_scall expects a 12 bits immediate argument.");
  return NULL_RTX;
  }

  for (i = 0; i < call_expr_nargs(args) - 1; ++i) {
  arg = expand_normal (CALL_EXPR_ARG (args, i+1));
  emit_move_insn (gen_rtx_REG (SImode, i), arg);
  }

  emit_call_insn (gen_scall (arg1));
  if (target != NULL)
  emit_move_insn (target, gen_rtx_REG (SImode, FIRST_ARG_REGNO));

  return target;
  } */

/* FIXME AUTO: disabling vector support */
/* static rtx */
/* k1_expand_builtin_abdhp (rtx target, tree args) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */

/*     if (!target) */
/*         target = gen_reg_rtx (SImode); */
/*     target = force_reg (SImode, target); */
/*     arg1 = force_reg (SImode, arg1); */
/*     arg2 = force_reg (SImode, arg2); */
/*     emit_insn (gen_abdhp (target, arg1, arg2)); */
/*     return target; */
/* } */

/* static rtx */
/* k1_expand_builtin_addhp (rtx target, tree args) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */

/*     if (!target) */
/*         target = gen_reg_rtx (SImode); */
/*     target = force_reg (SImode, target); */
/*     arg1 = force_reg (SImode, arg1); */
/*     arg2 = force_reg (SImode, arg2); */
/*     emit_insn (gen_addhp (target, arg1, arg2)); */
/*     return target; */
/* } */

/* static rtx */
/* k1_expand_builtin_sbfhp (rtx target, tree args) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */

/*     if (!target) */
/*         target = gen_reg_rtx (SImode); */
/*     target = force_reg (SImode, target); */
/*     arg1 = force_reg (SImode, arg1); */
/*     arg2 = force_reg (SImode, arg2); */
/*     emit_insn (gen_sbfhp (target, arg1, arg2)); */
/*     return target; */
/* } */

/* FIXME AUTO: disabling vector support */
/* static rtx */
/* k1_expand_builtin_landhp (rtx target, tree args) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */

/*     if (!target) */
/*         target = gen_reg_rtx (SImode); */
/*     target = force_reg (SImode, target); */
/*     arg1 = force_reg (SImode, arg1); */
/*     arg2 = force_reg (SImode, arg2); */
/*     emit_insn (gen_landhp (target, arg1, arg2)); */
/*     return target; */
/* } */

/* FIXME AUTO: disabling vector support */
/* static rtx */
/* k1_expand_builtin_sllhps (rtx target, tree args) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */

/*     if (!target) */
/*         target = gen_reg_rtx (SImode); */
/*     target = force_reg (SImode, target); */
/*     arg1 = force_reg (SImode, arg1); */
/*     arg2 = force_reg (SImode, arg2); */
/*     emit_insn (gen_sllhps (target, arg1, arg2)); */
/*     return target; */
/* } */

/* FIXME AUTO: disabling vector support */
/* static rtx */
/* k1_expand_builtin_srahps (rtx target, tree args) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */

/*     if (!target) */
/*         target = gen_reg_rtx (SImode); */
/*     target = force_reg (SImode, target); */
/*     arg1 = force_reg (SImode, arg1); */
/*     arg2 = force_reg (SImode, arg2); */
/*     emit_insn (gen_srahps (target, arg1, arg2)); */
/*     return target; */
/* } */

static rtx
k1_expand_builtin_abdw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);
  arg2 = force_reg (SImode, arg2);
  emit_insn (gen_abdw (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_abdd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (DImode);
  target = force_reg (DImode, target);
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);
  emit_insn (gen_abdd (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_addsw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);
  arg2 = force_reg (SImode, arg2);
  emit_insn (gen_ssaddsi3 (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_addsd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (DImode);
  target = force_reg (DImode, target);
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);
  emit_insn (gen_ssadddi3 (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_sbfsw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);
  arg2 = force_reg (SImode, arg2);
  emit_insn (gen_sssubsi3 (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_sbfsd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (DImode);
  target = force_reg (DImode, target);
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);
  emit_insn (gen_sssubdi3 (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_avgw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);
  arg2 = force_reg (SImode, arg2);
  emit_insn (gen_avgsi3_floor (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_avguw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);
  arg2 = force_reg (SImode, arg2);
  emit_insn (gen_uavgsi3_floor (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_avgrw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);
  arg2 = force_reg (SImode, arg2);
  emit_insn (gen_avgsi3_ceil (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_avgruw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);
  arg2 = force_reg (SImode, arg2);
  emit_insn (gen_uavgsi3_ceil (target, arg1, arg2));
  return target;
}

static rtx
k1_expand_builtin_cbsw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);
  emit_insn (gen_popcountsi2 (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_cbsd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (DImode);

  target = force_reg (DImode, target);
  arg1 = force_reg (DImode, arg1);
  emit_insn (gen_popcountdi2 (target, arg1));
  return target;
}

/*
 * Checks the that the TARGET rtx is valid:
 * - is non null
 * - is a register
 * - has the correct mode MODE
 */
static rtx
k1_builtin_helper_check_reg_target (rtx target, enum machine_mode mode)
{
  if (!target)
    target = gen_reg_rtx (mode);

  target = force_reg (DImode, target);

  gcc_assert (GET_MODE (target) == mode);

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
k1_expand_builtin_afaddd (rtx target, tree args)
{
  MEMREF (0, DImode, mem_target);
  GETREG (1, DImode, addend_and_return);

  target = k1_builtin_helper_check_reg_target (target, DImode);

  emit_insn (gen_afaddd (target, mem_target, addend_and_return,
			 gen_rtx_CONST_INT (SImode, 0) /* unused mem model */));
  return target;
}

static rtx
k1_expand_builtin_afaddw (rtx target, tree args)
{
  MEMREF (0, DImode, mem_target);
  GETREG (1, SImode, addend_and_return);

  target = k1_builtin_helper_check_reg_target (target, SImode);

  emit_insn (gen_afaddw (target, mem_target, addend_and_return,
			 gen_rtx_CONST_INT (SImode, 0) /* unused mem model */));
  return target;
}

static rtx
k1_expand_builtin_acswap (rtx target, tree args, bool double_p)
{
  rtx ptr = expand_normal (CALL_EXPR_ARG (args, 0));

  rtx mem_ref;

  if (!REG_P (ptr))
    ptr = force_reg (Pmode, ptr);

  mem_ref = gen_rtx_MEM (double_p ? DImode : SImode, ptr);

  rtx new_val = expand_normal (CALL_EXPR_ARG (args, 1));
  rtx expect_val = expand_normal (CALL_EXPR_ARG (args, 2));

  rtx tmp = gen_reg_rtx (TImode);

  if (!target)
    target = gen_reg_rtx (TImode);
  else
    target = force_reg (TImode, target);

  emit_move_insn (gen_lowpart (DImode, tmp), new_val);
  emit_move_insn (gen_highpart (DImode, tmp), expect_val);

  if (double_p)
    emit_insn (gen_acswapd (tmp, mem_ref));
  else
    emit_insn (gen_acswapw (tmp, mem_ref));

  emit_move_insn (target, tmp);

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

#if 0
static rtx
k1_expand_builtin_cwmoved (rtx target, tree args)
{
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));
    rtx cond;

    if (!target)
        target = gen_reg_rtx (SImode);
    target = force_reg (SImode, target);
    arg1 = force_reg (DImode, arg1);
    arg2 = force_reg (DImode, arg2);
    arg3 = force_reg (DImode, arg3);

    cond = gen_rtx_NE (VOIDmode, arg1, GEN_INT (0));

    emit_move_insn (target, arg3);
    emit_insn (gen_cmovesi (target, cond, arg1, arg2, target));

    return target;
}
#endif

// FIXME AUTO: cmovef is not a K1 insn, not a builtin
/* static rtx */
/* k1_expand_builtin_cmovef (rtx target, tree args) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */
/*     rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2)); */
/*     rtx cond; */

/*     if (!target) */
/*         target = gen_reg_rtx (SFmode); */
/*     target = force_reg (SFmode, target); */
/*     arg1 = force_reg (SImode, arg1); */
/*     arg2 = force_reg (SFmode, arg2); */
/*     arg3 = force_reg (SFmode, arg3); */

/*     cond = gen_rtx_NE (VOIDmode, arg1, GEN_INT (0)); */

/*     emit_move_insn (target, arg3); */
/*     emit_insn (gen_cmovesf (target, cond, arg1, arg2, target)); */

/*     return target; */
/* } */

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
  rtx mem_ref;

  if (!REG_P (ptr))
    ptr = force_reg (Pmode, ptr);

  mem_ref = gen_rtx_MEM (mode, ptr);

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

#define K1_EXPAND_BUILTIN_2_(name, tmode, smode)                               \
  static rtx k1_expand_builtin_##name (rtx target, tree args)                  \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    arg1 = force_reg (smode, arg1);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    if (!REG_P (target) || GET_MODE (target) != tmode)                         \
      {                                                                        \
	target = force_reg (tmode, target);                                    \
      }                                                                        \
    emit_insn (gen_k1_##name (target, arg1));                                  \
    return target;                                                             \
  }

K1_EXPAND_BUILTIN_2_ (fabsw, SFmode, SFmode)
K1_EXPAND_BUILTIN_2_ (fabsd, DFmode, DFmode)
K1_EXPAND_BUILTIN_2_ (fnegw, SFmode, SFmode)
K1_EXPAND_BUILTIN_2_ (fnegd, DFmode, DFmode)

#define K1_EXPAND_BUILTIN_3_(name, tmode, smode)                               \
  static rtx k1_expand_builtin_##name (rtx target, tree args)                  \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    arg1 = force_reg (smode, arg1);                                            \
    arg2 = force_reg (smode, arg2);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    if (!REG_P (target) || GET_MODE (target) != tmode)                         \
      {                                                                        \
	target = force_reg (tmode, target);                                    \
      }                                                                        \
    emit_insn (gen_k1_##name (target, arg1, arg2));                            \
    return target;                                                             \
  }

K1_EXPAND_BUILTIN_3_ (fmaxw, SFmode, SFmode)
K1_EXPAND_BUILTIN_3_ (fmaxd, DFmode, DFmode)
K1_EXPAND_BUILTIN_3_ (fminw, SFmode, SFmode)
K1_EXPAND_BUILTIN_3_ (fmind, DFmode, DFmode)

#define K1_EXPAND_BUILTIN_2_MODIFIERS(name, tmode, smode)                      \
  static rtx k1_expand_builtin_##name (rtx target, tree args)                  \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    arg1 = force_reg (smode, arg1);                                            \
    k1_modifiers[0] = '\0';                                                    \
    k1_verify_modifier_rounding (arg2, #name, "second");                       \
    k1_verify_modifier_silent (arg3, #name, "third");                          \
    rtx modifiers = gen_rtx_CONST_STRING (VOIDmode, k1_modifiers_lookup ());   \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    if (!REG_P (target) || GET_MODE (target) != tmode)                         \
      {                                                                        \
	target = force_reg (tmode, target);                                    \
      }                                                                        \
    emit_insn (gen_k1_##name (target, arg1, modifiers));                       \
    return target;                                                             \
  }

K1_EXPAND_BUILTIN_2_MODIFIERS (finvw, SFmode, SFmode)
K1_EXPAND_BUILTIN_2_MODIFIERS (fisrw, SFmode, SFmode)

#define K1_EXPAND_BUILTIN_3_MODIFIERS(name, tmode, smode)                      \
  static rtx k1_expand_builtin_##name (rtx target, tree args)                  \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    rtx arg4 = expand_normal (CALL_EXPR_ARG (args, 3));                        \
    arg1 = force_reg (smode, arg1);                                            \
    arg2 = force_reg (smode, arg2);                                            \
    k1_modifiers[0] = '\0';                                                    \
    k1_verify_modifier_rounding (arg3, #name, "third");                        \
    k1_verify_modifier_silent (arg4, #name, "fourth");                         \
    rtx modifiers = gen_rtx_CONST_STRING (VOIDmode, k1_modifiers_lookup ());   \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    if (!REG_P (target) || GET_MODE (target) != tmode)                         \
      {                                                                        \
	target = force_reg (tmode, target);                                    \
      }                                                                        \
    emit_insn (gen_k1_##name (target, arg1, arg2, modifiers));                 \
    return target;                                                             \
  }

K1_EXPAND_BUILTIN_3_MODIFIERS (faddw, SFmode, SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fsbfw, SFmode, SFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmulw, SFmode, SFmode)

K1_EXPAND_BUILTIN_3_MODIFIERS (faddd, DFmode, DFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fsbfd, DFmode, DFmode)
K1_EXPAND_BUILTIN_3_MODIFIERS (fmuld, DFmode, DFmode)

K1_EXPAND_BUILTIN_3_MODIFIERS (fmulwd, DFmode, SFmode)

#define K1_EXPAND_BUILTIN_4_MODIFIERS(name, tmode, smode)                      \
  static rtx k1_expand_builtin_##name (rtx target, tree args)                  \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    rtx arg4 = expand_normal (CALL_EXPR_ARG (args, 3));                        \
    rtx arg5 = expand_normal (CALL_EXPR_ARG (args, 4));                        \
    arg1 = force_reg (tmode, arg1);                                            \
    arg2 = force_reg (smode, arg2);                                            \
    arg3 = force_reg (smode, arg3);                                            \
    k1_modifiers[0] = '\0';                                                    \
    k1_verify_modifier_rounding (arg4, #name, "fourth");                       \
    k1_verify_modifier_silent (arg5, #name, "fifth");                          \
    rtx modifiers = gen_rtx_CONST_STRING (VOIDmode, k1_modifiers_lookup ());   \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    if (!REG_P (target) || GET_MODE (target) != tmode)                         \
      {                                                                        \
	target = force_reg (tmode, target);                                    \
      }                                                                        \
    emit_insn (gen_k1_##name (target, arg1, arg2, arg3, modifiers));           \
    return target;                                                             \
  }

K1_EXPAND_BUILTIN_4_MODIFIERS (ffmaw, SFmode, SFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (ffmsw, SFmode, SFmode)

K1_EXPAND_BUILTIN_4_MODIFIERS (ffmad, DFmode, DFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (ffmsd, DFmode, DFmode)

K1_EXPAND_BUILTIN_4_MODIFIERS (ffmawd, DFmode, SFmode)
K1_EXPAND_BUILTIN_4_MODIFIERS (ffmswd, DFmode, SFmode)

static rtx
k1_expand_builtin_float (rtx target, tree args, bool usigned)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));

  if (!verify_const_int_arg (arg1, 2, false))
    {
      error ("__builtin_k1_float%s expects a 2-bit unsigned immediate first "
	     "argument.",
	     usigned ? "u" : "");
      return NULL_RTX;
    }

  if (!verify_const_int_arg (arg3, 6, false))
    {
      error ("__builtin_k1_float%s expects a 6-bit unsigned immediate third "
	     "argument.",
	     usigned ? "u" : "");
      return NULL_RTX;
    }

  if (!target)
    target = gen_reg_rtx (SFmode);
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (SFmode, target);
    }

  arg2 = force_reg (SImode, arg2);

  if (usigned)
    emit_insn (gen_floatuw (target, arg1, arg2, arg3));
  else
    emit_insn (gen_floatw (target, arg1, arg2, arg3));

  return target;
}

static rtx
k1_expand_builtin_floatd (rtx target, tree args, bool usigned)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));

  if (!verify_const_int_arg (arg1, 2, false))
    {
      error ("__builtin_k1_float%sd expects a 2-bit unsigned immediate first "
	     "argument.",
	     usigned ? "u" : "");
      return NULL_RTX;
    }

  if (!verify_const_int_arg (arg3, 6, false))
    {
      error ("__builtin_k1_float%sd expects a 6-bit unsigned immediate third "
	     "argument.",
	     usigned ? "u" : "");
      return NULL_RTX;
    }

  if (!target)
    target = gen_reg_rtx (DFmode);
  if (!REG_P (target) || GET_MODE (target) != DFmode)
    {
      target = force_reg (DFmode, target);
    }

  arg2 = force_reg (DImode, arg2);

  if (usigned)
    emit_insn (gen_floatud (target, arg1, arg2, arg3));
  else
    emit_insn (gen_floatd (target, arg1, arg2, arg3));

  return target;
}

static rtx
k1_expand_builtin_fixed (rtx target, tree args, bool usigned)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));

  if (!verify_const_int_arg (arg1, 2, false))
    {
      error ("__builtin_k1_fixed%s expects a 2-bit unsigned immediate first "
	     "argument.",
	     usigned ? "u" : "");
      return NULL_RTX;
    }

  if (!verify_const_int_arg (arg3, 6, false))
    {
      error ("__builtin_k1_fixed%s expects a 6-bit unsigned immediate third "
	     "argument.",
	     usigned ? "u" : "");
      return NULL_RTX;
    }

  if (!target)
    target = gen_reg_rtx (SImode);
  if (!REG_P (target) || GET_MODE (target) != SImode)
    {
      target = force_reg (SImode, target);
    }

  arg2 = force_reg (SFmode, arg2);

  if (usigned)
    emit_insn (gen_fixeduw (target, arg1, arg2, arg3));
  else
    emit_insn (gen_fixedw (target, arg1, arg2, arg3));

  return target;
}

static rtx
k1_expand_builtin_fixedd (rtx target, tree args, bool usigned)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));

  if (!verify_const_int_arg (arg1, 2, false))
    {
      error ("__builtin_k1_fixed%sd expects a 2-bit unsigned immediate first "
	     "argument.",
	     usigned ? "u" : "");
      return NULL_RTX;
    }

  if (!verify_const_int_arg (arg3, 6, false))
    {
      error ("__builtin_k1_fixed%sd expects a 6-bit unsigned immediate third "
	     "argument.",
	     usigned ? "u" : "");
      return NULL_RTX;
    }

  if (!target)
    target = gen_reg_rtx (DImode);
  if (!REG_P (target) || GET_MODE (target) != DImode)
    {
      target = force_reg (DImode, target);
    }

  arg2 = force_reg (DFmode, arg2);

  if (usigned)
    emit_insn (gen_fixedud (target, arg1, arg2, arg3));
  else
    emit_insn (gen_fixedd (target, arg1, arg2, arg3));

  return target;
}

static rtx
k1_expand_builtin_fsdivw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  arg1 = force_reg (SFmode, arg1);
  arg2 = force_reg (SFmode, arg2);

  if (!target)
    target = gen_reg_rtx (SFmode);
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (SFmode, target);
    }
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
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (SFmode, target);
    }
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
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (SFmode, target);
    }
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
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (SFmode, target);
    }
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
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (DFmode, target);
    }
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
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (DFmode, target);
    }
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
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (DFmode, target);
    }
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
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (DFmode, target);
    }
  emit_insn (gen_fsisrd (target, arg1));

  return target;
}

#if 0
static rtx
k1_expand_builtin_ffmaw (rtx target, tree args, int rn)
{
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));

    arg1 = force_reg (SFmode, arg1);
    arg2 = force_reg (SFmode, arg2);
    arg3 = force_reg (SFmode, arg3);

    if (!target)
        target = gen_reg_rtx (SFmode);
    if (!REG_P(target) || GET_MODE (target) != SFmode) {
        target = force_reg (SFmode, target);
    }
    if (rn)
      emit_insn (gen_ffmawrn (target, arg2, arg3, arg1));
    else
      emit_insn (gen_fmasf4 (target, arg2, arg3, arg1));

    return target;
}

static rtx
k1_expand_builtin_ffmad (rtx target, tree args, int rn)
{
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));

    arg1 = force_reg (DFmode, arg1);
    arg2 = force_reg (DFmode, arg2);
    arg3 = force_reg (DFmode, arg3);

    if (!target)
        target = gen_reg_rtx (DFmode);
    if (!REG_P(target) || GET_MODE (target) != DFmode) {
        target = force_reg (DFmode, target);
    }
    if (rn)
      emit_insn (gen_ffmadrn (target, arg2, arg3, arg1));
    else
      emit_insn (gen_fmadf4 (target, arg2, arg3, arg1));

    return target;
}

static rtx
k1_expand_builtin_ffmawd (rtx target, tree args, int rn)
{
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));

    arg1 = force_reg (DFmode, arg1);
    arg2 = force_reg (SFmode, arg2);
    arg3 = force_reg (SFmode, arg3);

    if (!target)
        target = gen_reg_rtx (DFmode);
    if (!REG_P(target) || GET_MODE (target) != DFmode) {
        target = force_reg (DFmode, target);
    }
    if (rn)
      emit_insn (gen_ffmawdrn (target, arg1, arg2, arg3));
    else
      emit_insn (gen_ffmawd (target, arg1, arg2, arg3));

    return target;
}

static rtx
k1_expand_builtin_ffmsw (rtx target, tree args, int rn)
{
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));

    arg1 = force_reg (SFmode, arg1);
    arg2 = force_reg (SFmode, arg2);
    arg3 = force_reg (SFmode, arg3);

    if (!target)
        target = gen_reg_rtx (SFmode);
    if (!REG_P(target) || GET_MODE (target) != SFmode) {
        target = force_reg (SFmode, target);
    }

    if (rn)
      emit_insn (gen_ffmswrn (target, arg2, arg3, arg1));
    else
      emit_insn (gen_fnmasf4 (target, arg2, arg3, arg1));

    return target;
}

static rtx
k1_expand_builtin_ffmsd (rtx target, tree args, int rn)
{
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));

    arg1 = force_reg (DFmode, arg1);
    arg2 = force_reg (DFmode, arg2);
    arg3 = force_reg (DFmode, arg3);

    if (!target)
        target = gen_reg_rtx (DFmode);
    if (!REG_P(target) || GET_MODE (target) != DFmode) {
        target = force_reg (DFmode, target);
    }

    if (rn)
      emit_insn (gen_ffmsdrn (target, arg2, arg3, arg1));
    else
      emit_insn (gen_fnmadf4 (target, arg2, arg3, arg1));

    return target;
}

static rtx
k1_expand_builtin_ffmswd (rtx target, tree args, int rn)
{
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));

    arg1 = force_reg (DFmode, arg1);
    arg2 = force_reg (SFmode, arg2);
    arg3 = force_reg (SFmode, arg3);

    if (!target)
        target = gen_reg_rtx (DFmode);
    if (!REG_P(target) || GET_MODE (target) != DFmode) {
        target = force_reg (DFmode, target);
    }
    if (rn)
      emit_insn (gen_ffmswdrn (target, arg1, arg2, arg3));
    else
      emit_insn (gen_ffmswd (target, arg1, arg2, arg3));

    return target;
}
#endif

static rtx
k1_expand_builtin_lbsu (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (QImode);
  if (!REG_P (target) || GET_MODE (target) != QImode)
    {
      target = force_reg (QImode, target);
    }

  arg1 = gen_rtx_MEM (QImode, force_reg (Pmode, arg1));
  emit_insn (gen_lbsu (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_lbzu (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (QImode);
  if (!REG_P (target) || GET_MODE (target) != QImode)
    {
      target = force_reg (QImode, target);
    }

  arg1 = gen_rtx_MEM (QImode, force_reg (Pmode, arg1));
  emit_insn (gen_lbzu (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_ldu (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (DImode);
  if (!REG_P (target) || GET_MODE (target) != DImode)
    {
      target = force_reg (DImode, target);
    }

  arg1 = gen_rtx_MEM (DImode, force_reg (Pmode, arg1));
  emit_insn (gen_ldu (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_lhsu (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (HImode);
  if (!REG_P (target) || GET_MODE (target) != HImode)
    {
      target = force_reg (HImode, target);
    }

  arg1 = gen_rtx_MEM (HImode, force_reg (Pmode, arg1));
  emit_insn (gen_lhsu (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_lhzu (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (HImode);
  if (!REG_P (target) || GET_MODE (target) != HImode)
    {
      target = force_reg (HImode, target);
    }

  arg1 = gen_rtx_MEM (HImode, force_reg (Pmode, arg1));
  emit_insn (gen_lhzu (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_lwzu (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (SImode);
  if (!REG_P (target) || GET_MODE (target) != SImode)
    {
      target = force_reg (SImode, target);
    }

  arg1 = gen_rtx_MEM (SImode, force_reg (Pmode, arg1));

  emit_insn (gen_lwzu (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_extfz (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));

  if (!verify_const_int_arg (arg2, 6, false))
    {
      error ("__builtin_k1_extfz expects a 6-bit unsigned immediate second "
	     "argument.");
      return NULL_RTX;
    }

  if (!verify_const_int_arg (arg3, 6, false))
    {
      error ("__builtin_k1_extfz expects a 6-bit unsigned immediate third "
	     "argument.");
      return NULL_RTX;
    }

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);

  arg2 = gen_rtx_CONST_INT (SImode, INTVAL (arg2) - INTVAL (arg3) + 1);

  emit_insn (gen_extzv (target, arg1, arg2, arg3));

  return target;
}

static rtx
k1_expand_builtin_stsuw (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);
  arg2 = force_reg (SImode, arg2);
  emit_insn (gen_stsuw (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_stsud (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (DImode);
  target = force_reg (DImode, target);
  arg1 = force_reg (DImode, arg1);
  arg2 = force_reg (DImode, arg2);
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
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (SFmode, target);
    }
  if (low_bits)
    emit_insn (gen_builtin_extendhfsf2 (target, arg1));
  else
    emit_insn (gen_builtin_extendhfsf2_tophalf (target, arg1));

  return target;
}

/* FIXME AUTO: disabling vector support */
/* static rtx */
/* k1_expand_builtin_fwiden_wp (rtx target, tree args, int low_bits) */
/* { */
/*   rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */

/*   arg1 = force_reg (DImode, arg1); */

/*   if (!target) */
/*     target = gen_reg_rtx (V2SFmode); */
/*   if (!REG_P(target) || GET_MODE (target) != V2SFmode) { */
/*     target = force_reg (V2SFmode, target); */
/*   } */
/*   if(low_bits) */
/*     emit_insn (gen_builtin_extendv2hfv2sf2 (target, arg1)); */
/*   else */
/*     emit_insn (gen_builtin_extendv2hfv2sf2_tophalf (target, arg1)); */

/*   return target; */
/* } */

static rtx
k1_expand_builtin_fnarrowwh (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  arg1 = force_reg (SFmode, arg1);

  if (!target)
    target = gen_reg_rtx (HImode);
  if (!REG_P (target) || GET_MODE (target) != HImode)
    {
      target = force_reg (HImode, target);
    }
  emit_insn (gen_builtin_truncsfhf2 (target, arg1));

  return target;
}

/* FIXME AUTO: disabling vector support */
/* static rtx */
/* k1_expand_builtin_fnarrowdwp (rtx target, tree args) */
/* { */
/*   rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */

/*   arg1 = force_reg (V2SFmode, arg1); */

/*   if (!target) */
/*     target = gen_reg_rtx (V2SImode); */
/*   if (!REG_P(target) || GET_MODE (target) != V2SImode) { */
/*     target = force_reg (HImode, target); */
/*   } */
/*   emit_insn (gen_builtin_truncv2sf_2hf2(target, arg1)); */

/*   return target; */
/* } */

static rtx
k1_target_expand_builtin (tree exp, rtx target, rtx subtarget ATTRIBUTE_UNUSED,
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
      return k1_expand_builtin_acswap (target, exp, 0);
    case K1_BUILTIN_ACSWAPD:
      return k1_expand_builtin_acswap (target, exp, 1);
    case K1_BUILTIN_AFADDD:
      return k1_expand_builtin_afaddd (target, exp);
    case K1_BUILTIN_AFADDW:
      return k1_expand_builtin_afaddw (target, exp);
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
    case K1_BUILTIN_FABSD:
      return k1_expand_builtin_fabsd (target, exp);
    case K1_BUILTIN_FNEGW:
      return k1_expand_builtin_fnegw (target, exp);
    case K1_BUILTIN_FNEGD:
      return k1_expand_builtin_fnegd (target, exp);
    case K1_BUILTIN_FMAXW:
      return k1_expand_builtin_fmaxw (target, exp);
    case K1_BUILTIN_FMAXD:
      return k1_expand_builtin_fmaxd (target, exp);
    case K1_BUILTIN_FMINW:
      return k1_expand_builtin_fminw (target, exp);
    case K1_BUILTIN_FMIND:
      return k1_expand_builtin_fmind (target, exp);
    case K1_BUILTIN_FINVW:
      return k1_expand_builtin_finvw (target, exp);
    case K1_BUILTIN_FISRW:
      return k1_expand_builtin_fisrw (target, exp);
    case K1_BUILTIN_FADDW:
      return k1_expand_builtin_faddw (target, exp);
    case K1_BUILTIN_FADDD:
      return k1_expand_builtin_faddd (target, exp);
    case K1_BUILTIN_FSBFW:
      return k1_expand_builtin_fsbfw (target, exp);
    case K1_BUILTIN_FSBFD:
      return k1_expand_builtin_fsbfd (target, exp);
    case K1_BUILTIN_FMULW:
      return k1_expand_builtin_fmulw (target, exp);
    case K1_BUILTIN_FMULD:
      return k1_expand_builtin_fmuld (target, exp);
    case K1_BUILTIN_FMULWD:
      return k1_expand_builtin_fmulwd (target, exp);
    case K1_BUILTIN_FFMAW:
      return k1_expand_builtin_ffmaw (target, exp);
    case K1_BUILTIN_FFMAD:
      return k1_expand_builtin_ffmad (target, exp);
    case K1_BUILTIN_FFMAWD:
      return k1_expand_builtin_ffmawd (target, exp);
    case K1_BUILTIN_FFMSW:
      return k1_expand_builtin_ffmsw (target, exp);
    case K1_BUILTIN_FFMSD:
      return k1_expand_builtin_ffmsd (target, exp);
    case K1_BUILTIN_FFMSWD:
      return k1_expand_builtin_ffmswd (target, exp);

    case K1_BUILTIN_FENCE:
      return k1_expand_builtin_fence ();
    case K1_BUILTIN_FLOAT:
    case K1_BUILTIN_FLOATU:
      return k1_expand_builtin_float (target, exp, fcode == K1_BUILTIN_FLOATU);
    case K1_BUILTIN_FLOATD:
    case K1_BUILTIN_FLOATUD:
      return k1_expand_builtin_floatd (target, exp,
				       fcode == K1_BUILTIN_FLOATUD);
    case K1_BUILTIN_FIXED:
    case K1_BUILTIN_FIXEDU:
      return k1_expand_builtin_fixed (target, exp, fcode == K1_BUILTIN_FIXEDU);
    case K1_BUILTIN_FIXEDD:
    case K1_BUILTIN_FIXEDUD:
      return k1_expand_builtin_fixedd (target, exp,
				       fcode == K1_BUILTIN_FIXEDUD);
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
    /* FIXME AUTO: Disabling get builtin. Ref T7705 */
    /* case K1_BUILTIN_GET_R: */
    /*   return k1_expand_builtin_get_r (target, exp); */
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
    case K1_BUILTIN_SET_PS:
      return k1_expand_builtin_set (target, exp, fcode == K1_BUILTIN_SET_PS);
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
      internal_error ("bad builtin code");
      break;
    }
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
k1_target_sched_adjust_cost (rtx_insn *insn, int dep_type, rtx_insn *dep_insn,
			     int cost, unsigned int)
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
k1_target_sched_issue_rate (void)
{
  return 5;
}

static int
k1_target_sched_dfa_new_cycle (FILE *dump ATTRIBUTE_UNUSED,
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

static rtx
k1_gen_reg_copy (rtx reg, rtx reg2)
{
  rtx insn;

  if (GET_MODE (reg) == SImode)
    {
      insn = gen_movsi (reg, reg2);
    }
  else if (GET_MODE (reg) == SFmode)
    {
      insn = gen_movsf (reg, reg2);
    }
  /* FIXME AUTO: disabling vector support */
  /* else if (GET_MODE (reg) == V2HImode){ */
  /*     insn = gen_movv2hi (reg, reg2); */
  /* }  */
  else
    {
      gcc_unreachable ();
    }

  return insn;
}

static int
k1_patch_mem_for_double_access_1 (rtx *x, void *data ATTRIBUTE_UNUSED)
{
  if (MEM_P (*x))
    {
      *x = shallow_copy_rtx (*x);
      XEXP (*x, 0)
	= plus_constant (Pmode, XEXP (*x, 0), GET_MODE_SIZE (GET_MODE (*x)));
      return 1;
    }

  return 0;
}

static int
k1_decr_reg_use_count (rtx *x, void *data ATTRIBUTE_UNUSED)
{
  if (REG_P (*x) && REGNO (*x) >= FIRST_PSEUDO_REGISTER
      && DF_REG_USE_GET (REGNO (*x)))
    {
      if (DF_REG_USE_COUNT (REGNO (*x)) >= 1)
	DF_REG_USE_COUNT (REGNO (*x))--;
    }

  return 0;
}

static int
k1_incr_reg_use_count (rtx *x, void *data ATTRIBUTE_UNUSED)
{
  if (REG_P (*x) && REGNO (*x) >= FIRST_PSEUDO_REGISTER
      && DF_REG_USE_GET (REGNO (*x)))
    {
      DF_REG_USE_COUNT (REGNO (*x))++;
    }

  return 0;
}

int
k1_has_big_immediate (rtx x)
{
  /* return for_each_rtx (&x, k1_has_big_immediate_1, NULL); */
  subrtx_ptr_iterator::array_type array;
  FOR_EACH_SUBRTX_PTR (iter, array, &x, ALL)
    {
      rtx *x = *iter;
      if (GET_CODE (*x) == SYMBOL_REF || GET_CODE (*x) == LABEL_REF
	  || GET_CODE (*x) == CONST_DOUBLE
	  || (GET_CODE (*x) == CONST_INT
	      && (INTVAL (*x) < -512 || INTVAL (*x) > 511)))
	{
	  return 1;
	}
    }
  return 0;
}

/* Test whether the memory operand OP should be accessed cached or
   uncached regarding it's name address space and the value of the
   flag K1_FORCE_UNCACHED_LSU. */
bool
k1_is_uncached_mem_op_p (rtx op)
{
  /* __convert[_no_sync] addr space should not come here. */
  gcc_assert (!MEM_P (op)
	      || (MEM_ADDR_SPACE (op) == ADDR_SPACE_GENERIC
		  || MEM_ADDR_SPACE (op) == K1_ADDR_SPACE_UNCACHED));

  return MEM_P (op) && MEM_ADDR_SPACE (op) == K1_ADDR_SPACE_UNCACHED;
}

/*
 * Returns TRUE if OP is a load multiple operation and all mems are
 * cached/uncached depending on IS_UNCACHED.
 */
bool
k1_load_multiple_operation_p (rtx op, bool is_uncached)
{
  int count = XVECLEN (op, 0);
  unsigned int dest_regno;
  rtx src_addr;
  int i;

  /* Perform a quick check so we don't blow up below.  */
  if (count != 2 && count != 4)
    return 0;

  for (i = 0; i < count; i++)
    {
      rtx set = XVECEXP (op, 0, i);

      if (GET_CODE (set) != SET || !REG_P (SET_DEST (set))
	  || !MEM_P (SET_SRC (set)) || MEM_VOLATILE_P (SET_SRC (set)))
	return 0;

      if (is_uncached != k1_is_uncached_mem_op_p (SET_SRC (set)))
	return 0;
    }

  if (MEM_ALIGN (SET_SRC (XVECEXP (op, 0, 0))) < (count * UNITS_PER_WORD))
    return 0;

  dest_regno = REGNO (SET_DEST (XVECEXP (op, 0, 0)));

  /* register number must be correctly aligned */
  if (dest_regno < FIRST_PSEUDO_REGISTER && (dest_regno % count != 0))
    return 0;

  src_addr = XEXP (SET_SRC (XVECEXP (op, 0, 0)), 0);
  HOST_WIDE_INT expected_offset = 0;
  int base_regno = -1;

  for (i = 0; i < count; i++)
    {
      rtx elt = XVECEXP (op, 0, i);
      rtx base, offset;

      if (!k1_split_mem (XEXP (SET_SRC (elt), 0), &base, &offset, true))
	return 0;

      if (i == 0)
	{
	  expected_offset = INTVAL (offset);
	  base_regno = REGNO (base);
	}
      else
	{
	  expected_offset += UNITS_PER_WORD;
	}

      if (!REG_P (SET_DEST (elt)) || GET_MODE (SET_DEST (elt)) != DImode
	  || REGNO (SET_DEST (elt)) != dest_regno + i
	  || GET_MODE (SET_SRC (elt)) != DImode || base_regno != REGNO (base)
	  || expected_offset != INTVAL (offset))

	return 0;
    }

  return 1;
}

/*
 * Returns TRUE if OP is a store multiple operation.
 */
bool
k1_store_multiple_operation_p (rtx op)
{
  int count = XVECLEN (op, 0);
  unsigned int src_regno;
  rtx dest_addr;
  int i;

  /* Perform a quick check so we don't blow up below.  */
  if ((count != 2 && count != 4) || GET_CODE (XVECEXP (op, 0, 0)) != SET
      || GET_CODE (SET_DEST (XVECEXP (op, 0, 0))) != MEM
      || GET_CODE (SET_SRC (XVECEXP (op, 0, 0))) != REG)
    return 0;

  if (MEM_ALIGN (SET_DEST (XVECEXP (op, 0, 0))) < (count * UNITS_PER_WORD))
    return 0;

  src_regno = REGNO (SET_SRC (XVECEXP (op, 0, 0)));
  dest_addr = XEXP (SET_DEST (XVECEXP (op, 0, 0)), 0);

  for (i = 1; i < count; i++)
    {
      rtx elt = XVECEXP (op, 0, i);

      if (GET_CODE (elt) != SET || !REG_P (SET_SRC (elt))
	  || GET_MODE (SET_SRC (elt)) != DImode
	  || REGNO (SET_SRC (elt)) != src_regno + i || !MEM_P (SET_DEST (elt))
	  || MEM_VOLATILE_P (SET_DEST (elt))
	  || GET_MODE (SET_DEST (elt)) != DImode
	  || GET_CODE (XEXP (SET_DEST (elt), 0)) != PLUS
	  || !rtx_equal_p (XEXP (XEXP (SET_DEST (elt), 0), 0), dest_addr)
	  || GET_CODE (XEXP (XEXP (SET_DEST (elt), 0), 1)) != CONST_INT
	  || INTVAL (XEXP (XEXP (SET_DEST (elt), 0), 1)) != i * 8)
	return 0;
    }

  return 1;
}

/* Following funtions are used for bundling insn before ASM emission */

/* Contains the state corresponding to the bundle under construction */
static state_t state;
/* Track registers being defined within the bundle */
static HARD_REG_SET current_bundle_reg_defs;

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

static int
k1_uses_register (rtx x)
{
  rtx set_dst = NULL;

  subrtx_ptr_iterator::array_type array;
  FOR_EACH_SUBRTX_PTR (iter, array, &x, ALL)
    {
      rtx *x = *iter;

      if (!*x)
	{
	  continue;
	}

      if (GET_CODE (*x) == CLOBBER || GET_CODE (*x) == USE
	  || GET_CODE (*x) == EXPR_LIST)
	{
	  iter.skip_subrtxes ();
	}
      else if (GET_CODE (*x) == SET)
	{
	  set_dst = SET_DEST (*x);
	}

      if (GET_CODE (*x) == REG && *x != set_dst)
	{
	  return 1;
	}
    }
  /* return for_each_rtx (&x, k1_uses_register_1, &set_dst); */
}

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
  /* for_each_rtx (&SET_DEST (*x), k1_scan_insn_registers_1, regs); */
  struct bundle_regs *regs = (struct bundle_regs *) data;

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

static void
k1_clear_insn_registers (void)
{
  CLEAR_HARD_REG_SET (current_bundle_reg_defs);
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
      result->next = NULL;

      result->insn = NULL;
      result->last_insn = NULL;
      result->insns_num = 0;

      CLEAR_HARD_REG_SET (result->reg_defs);

      result->dfa_state = xmalloc (dfa_state_size);
      state_reset (result->dfa_state);

      result->allocated_states_chain = allocated_bundle_states_chain;
      allocated_bundle_states_chain = result;
    }
  result->unique_num = bundle_states_num;
  bundle_states_num++;
  return result;
}

/* The following function frees given bundle state.  */

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
  int cur_cfa_reg = REGNO (stack_pointer_rtx);

  for (i = cur_bundle_list; i; i = i->next)
    {
      rtx_insn *binsn = i->last_insn;

      gcc_assert (i->insn != NULL);
      gcc_assert (i->last_insn != NULL);

      int bundle_start;
      int bundle_end;
      /* Start from the end so that NOTEs will be added in the correct order. */
      do
	{
	  bundle_start = (binsn == i->insn);
	  bundle_end = (binsn == i->last_insn);

	  if (RTX_FRAME_RELATED_P (binsn))
	    {
	      rtx note;
	      bool handled = false;
	      for (note = REG_NOTES (binsn); note; note = XEXP (note, 1))
		{
		  rtx pat = XEXP (note, 0);
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

	  if (RTX_FRAME_RELATED_P (binsn))
	    {
	      rtx note;

	      for (note = REG_NOTES (binsn); note; note = XEXP (note, 1))
		{
		  rtx pat = XEXP (note, 0);
		  switch (REG_NOTE_KIND (note))
		    {
		    case REG_CFA_DEF_CFA:
		      /* (PLUS ( CFA_REG, OFFSET)) */
		      gcc_assert (GET_CODE (pat) == PLUS);
		      cur_cfa_reg = REGNO (XEXP (pat, 0));
		      break;

		      /* We only need to fixup register spill */
		    case REG_CFA_OFFSET:
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
k1_target_asm_final_postscan_insn (FILE *file, rtx_insn *insn,
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
k1_target_sched_first_cycle_multipass_dfa_lookahead (void)
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
	  else if (GET_CODE (XEXP (*x, 1)) == CONST_DOUBLE
		   && GET_MODE (XEXP (*x, 1)) == VOIDmode)
	    {
	      rtx op = XEXP (*x, 1);
	      long long div = CONST_INT_P (op)
				? INTVAL (op)
				: ((long long) CONST_DOUBLE_HIGH (op) << 32
				   | CONST_DOUBLE_LOW (op));
	      if (div > 0 && __builtin_popcount (div) == 1)
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
k1_target_rtx_costs (rtx x, machine_mode mode ATTRIBUTE_UNUSED,
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
	  *total = k1_target_register_move_cost (GET_MODE (SET_DEST (x)),
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

/* FIXME AUTO: fix cost function for coolidge */
/* See T7748 */
static int
k1_target_address_cost (rtx x, machine_mode mode ATTRIBUTE_UNUSED,
			addr_space_t space ATTRIBUTE_UNUSED,
			bool speed ATTRIBUTE_UNUSED)
{
  struct k1_address addr;
  int cost;

  /* Just compare the size of the operands */
  if (!k1_analyze_address (x, false, &addr))
    gcc_unreachable ();

  if (addr.mode == ADDR_OFFSET
      && (!CONST_INT_P (addr.offset) || INTVAL (addr.offset) > 511
	  || INTVAL (addr.offset) < -512))
    cost = COSTS_N_INSNS (2);
  else
    cost = COSTS_N_INSNS (1);

  /* Give a slight advantage to more complicated addressing
     modes. This allows fwprop to create more complicated addressing
     modes and thus possibly to remove preliminary computations
     that are used more than once. (combine handles the case where
     it's used only once.) */
  if (current_pass->tv_id == TV_FWPROP
      && (addr.mode == ADDR_MOD || (addr.mode == ADDR_MULT && addr.mult != 1)))
    cost--;

  return cost;
}

static void
k1_target_sched_set_sched_flags (struct spec_info_def *spec ATTRIBUTE_UNUSED)
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
k1_target_sched_init_global (FILE *file ATTRIBUTE_UNUSED,
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

static void
k1_target_trampoline_init (rtx m_tramp, tree fndecl, rtx static_chain)
{
  /* FIXME AUTO: trampoline are broken T6775 */

  /* rtx fun_addr = copy_to_reg (XEXP (DECL_RTL (fndecl), 0)); */
  /* rtx chain = copy_to_reg (static_chain); */
  /* rtx mem = adjust_address (m_tramp, Pmode, 0); */

  /* rtx scratch = gen_reg_rtx (Pmode); */
  /* rtx scratch2 = gen_reg_rtx (Pmode); */

  /* /\* make using 2 words: lower10/upper22 format *\/ */

  /* /\* first word with lower10 + make opcode *\/ */
  /* emit_move_insn (scratch, chain); */
  /* emit_insn (gen_extzv (scratch, scratch, GEN_INT (10), GEN_INT (0))); */
  /* emit_insn (gen_ashlsi3 (scratch, scratch, GEN_INT (6))); */
  /* emit_insn (gen_iorsi3 (scratch, scratch, GEN_INT ((int)0xe02c0000))); */

  /* /\* second word immx with upper22 *\/ */
  /* emit_insn (gen_lshrsi3 (chain, chain, GEN_INT (10))); */

  /* /\* goto with addr from m_tramp *\/ */
  /* emit_move_insn (scratch2, XEXP(m_tramp, 0)); */
  /* emit_insn (gen_subsi3 (scratch2, fun_addr, scratch2)); */
  /* emit_insn (gen_extzv (scratch2, scratch2, GEN_INT (27), GEN_INT(2))); */
  /* emit_insn (gen_iorsi3 (scratch2, scratch2, GEN_INT ((int)0x90000000))); */

  /* emit_move_insn (mem, scratch2); */
  /* mem = adjust_address (m_tramp, Pmode, 4); */
  /* emit_move_insn (mem, scratch); */
  /* mem = adjust_address (m_tramp, Pmode, 8); */
  /* emit_move_insn (mem, chain); */

  /* emit_insn (gen_iinvals (adjust_address (m_tramp, Pmode, 0),
   * k1_sync_reg_rtx)); */
  /* emit_insn (gen_iinvals (mem, k1_sync_reg_rtx)); */
}

/* We recognize patterns that aren't canonical addresses, because we
   might generate those with our use of simplify_replace_rtx() in our
   mem access packing. */
/* bool */
/* k1_legitimate_modulo_addressing_p (rtx x, bool strict) */
/* { */
/*     struct k1_address addr; */
/*     return k1_analyze_modulo_address (x, strict, &addr); */
/* } */

/* We recognize patterns that aren't canonical addresses, because we
   might generate those with our use of simplify_replace_rtx() in our
   mem access packing. */
static bool
k1_target_legitimate_address_p (enum machine_mode ATTRIBUTE_UNUSED mode, rtx x,
				bool strict)
{
  struct k1_address addr;
  bool ret = k1_analyze_address (x, strict, &addr);
  return ret;
}

static bool
k1_legitimate_constant_p (enum machine_mode mode ATTRIBUTE_UNUSED, rtx x)
{
  if (x == k1_data_start_symbol)
    return true;

  if (k1_has_tls_reference (x))
    return false;

  if (flag_pic && k1_needs_symbol_reloc (x))
    {
      return false;
    }

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
k1_target_legitimize_address (rtx x, rtx oldx ATTRIBUTE_UNUSED,
			      enum machine_mode mode ATTRIBUTE_UNUSED)
{
  if (k1_has_tls_reference (x))
    return k1_legitimize_tls_reference (x);
  else if (GET_CODE (x) == PLUS
	   && (GET_CODE (XEXP (x, 0)) == MULT
	       || GET_CODE (XEXP (x, 0)) == ZERO_EXTEND))
    {
      rtx reg = gen_reg_rtx (Pmode);
      rtx cst = XEXP (x, 1);
      struct k1_address addr;

      XEXP (x, 1) = reg;
      if (k1_analyze_address (x, false, &addr))
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
    case K1_ADDR_SPACE_UNCACHED:
      return k1_target_legitimate_address_p (mode, exp, strict);

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

  return k1_target_legitimize_address (x, oldx, mode);
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
	       TYPE_ADDR_SPACE (TREE_TYPE (from_type)) == K1_ADDR_SPACE_UNCACHED
		 ? "Implicit conversion of uncached pointer to cached one"
		 : "Implicit conversion of cached pointer to uncached one");
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

/* FIXME AUTO : Disable use of create_SC_* */
/* rtx */
/* k1_find_or_create_SC_register (rtx curr_insn, rtx low, rtx high) */
/* { */
/*     int i, generating_concat = generating_concat_p; */
/*     struct fake_SC sc; */
/*     struct fake_SC *elt; */

/*     if (cfun->machine->fake_SC_registers) */
/*     FOR_EACH_VEC_ELT (*cfun->machine->fake_SC_registers, i, elt) { */
/*        if (rtx_equal_p (low, elt->low) */
/*             && rtx_equal_p (high, elt->high)) { */
/*             rtx l = SET_DEST (PATTERN (elt->low_insn)); */
/*             rtx h = SET_DEST (PATTERN (elt->high_insn)); */
/*             rtx p; */
/*             int parts = 0; */

/*             for (p = prev_nonnote_insn_bb (curr_insn); p != NULL_RTX ; */
/*                  p = prev_nonnote_insn_bb (p)) */
/*                 if (INSN_P (p)) { */
/*                     rtx set = single_set (p); */

/* 		    if (reg_set_p (low, p) */
/* 			|| reg_set_p (high, p)) */
/* 		      break; */

/*                     if (set */
/*                         && elt->low_insn == p */
/*                         && rtx_equal_p (l, SET_DEST (set))) { */
/*                         parts |= 1; */
/*                     } else if (set */
/*                                && elt->high_insn == p */
/*                                && rtx_equal_p (h, SET_DEST (set))) { */
/*                         parts |= 2; */
/*                     } */

/*                     if (parts == 3) break; */
/*                 } */

/*             if (parts == 3) return elt->sc; */
/*         } */
/*     } */

/*     /\* If we want the RTL patterns to match, don't generate a SC register */
/*        containing a concat. *\/ */
/*     generating_concat_p = 0; */
/*     sc.sc = gen_reg_rtx (SCmode); */
/*     /\* We'll access the register only by subparts. Emit a */
/*        clobber of the whole register so that the liveness */
/*        analysis can deduce that it's live only from this point */
/*        on. *\/ */
/*     emit_insn (gen_rtx_CLOBBER (SCmode, sc.sc)); */
/*     generating_concat_p = generating_concat; */
/*     sc.low = low; */
/*     sc.high = high; */

/*     sc.low_insn = emit_move_insn (gen_lowpart(SFmode, sc.sc), low); */
/*     sc.high_insn = emit_move_insn (gen_highpart(SFmode, sc.sc), high); */

/*     vec_safe_push (cfun->machine->fake_SC_registers, sc); */

/*     return sc.sc; */
/* } */

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

  if (scheduling && TARGET_BUNDLING)
    {
      k1_gen_bundles ();
      k1_fix_debug_for_bundles ();
    }

  /* This is needed. Else final pass will crash on debug_insn-s */
  if (k1_flag_var_tracking)
    {
      compute_bb_for_insn ();
      timevar_push (TV_VAR_TRACKING);
      variable_tracking_main ();
      timevar_pop (TV_VAR_TRACKING);
      free_bb_for_insn ();
    }

  df_finish_pass (false);
}

/* FIXME AUTO: disabling vector support */
/* static enum machine_mode */
/* k1_units_preffered_simd_mode (enum machine_mode mode) */
/* { */
/*     switch (mode) { */
/*     case SImode: */
/*         return V2SImode; */
/*     case HImode: */
/*         return V4HImode; */
/*     case SFmode: */
/*         return V8SFmode; */
/*     default:; */
/*     } */

/*     return SImode; */
/* } */

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

  // FIXME FOR COOLIDGE

  /* if (k1_architecture() >= K1B) */
  /*   k1_arch_schedule = ARCH_BOSTAN; */
  /* else */
  /*   k1_arch_schedule = ARCH_ANDEY; */

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
      switch (XINT ((x), 1))
	{
	case UNSPEC_PIC:
	  /* GLOBAL_OFFSET_TABLE or local symbols, no suffix.  */
	  output_addr_const ((fp), XVECEXP ((x), 0, 0));
	  return true;
	case UNSPEC_GOT:
	  fputs ("@got", (fp));
	  /* if (TARGET_64) fputs ("64", (fp)); */
	  fputs ("(", (fp));

	  output_addr_const ((fp), XVECEXP ((x), 0, 0));
	  fputs (")", (fp));
	  return true;
	case UNSPEC_GOTOFF:
	  fputs ("@gotoff", (fp));
	  /* if (TARGET_64) fputs ("64", (fp)); */
	  fputs ("(", (fp));

	  output_addr_const ((fp), XVECEXP ((x), 0, 0));
	  fputs (")", (fp));
	  return true;
	case UNSPEC_TLS:
	  fputs ("@tprel", (fp));
	  if (!TARGET_32)
	    fputs ("64", (fp));
	  fputs ("(", (fp));

	  output_addr_const ((fp), XVECEXP ((x), 0, 0));
	  fputs (")", (fp));
	  return true;
	case UNSPEC_PCREL:
	  fputs ("@pcrel(", (fp));
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

static bool
k1_mode_dependent_address_p (const_rtx addr ATTRIBUTE_UNUSED,
			     addr_space_t space ATTRIBUTE_UNUSED)
{
  return current_pass->tv_id == TV_LOWER_SUBREG;
}

static bool
k1_lra_p (void)
{
  return !TARGET_RELOAD;
}

static int
k1_target_register_priority (int regno)
{
  if (regno == 10 || regno == 15)
    return 0;

  return 1;
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
  rtx mem_p_saved_ra = gen_rtx_REG (SImode, 38);
  rtx fp = memory_address (Pmode, frame_pointer_rtx);
  rtx fun = gen_rtx_SYMBOL_REF (Pmode, "__mcount");
  emit_library_call (fun, LCT_NORMAL, VOIDmode, 2, mem_p_saved_ra, Pmode, fp,
		     Pmode);
}

/* Initialize the GCC target structure.  */

#undef TARGET_CLASS_MAX_NREGS
#define TARGET_CLASS_MAX_NREGS k1_class_max_nregs

#undef TARGET_LRA_P
#define TARGET_LRA_P k1_lra_p

#undef TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE k1_option_override

#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE k1_target_function_value

#undef TARGET_RETURN_IN_MSB
#define TARGET_RETURN_IN_MSB k1_target_return_in_msb

#undef TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY k1_target_return_in_memory

#undef TARGET_STRUCT_VALUE_RTX
#define TARGET_STRUCT_VALUE_RTX k1_target_struct_value_rtx

#undef TARGET_ASM_OUTPUT_MI_THUNK
#define TARGET_ASM_OUTPUT_MI_THUNK k1_target_asm_output_mi_thunk

#undef TARGET_ASM_CAN_OUTPUT_MI_THUNK
#define TARGET_ASM_CAN_OUTPUT_MI_THUNK k1_target_asm_can_output_mi_thunk

#undef TARGET_ASM_ALIGNED_DI_OP
#define TARGET_ASM_ALIGNED_DI_OP "\t.8byte\t"

#undef TARGET_EXPAND_BUILTIN_SAVEREGS
#define TARGET_EXPAND_BUILTIN_SAVEREGS k1_target_expand_builtin_saveregs

#undef TARGET_EXPAND_BUILTIN_VA_START
#define TARGET_EXPAND_BUILTIN_VA_START k1_target_expand_va_start

#undef TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P k1_target_legitimate_address_p

#undef TARGET_DECIMAL_FLOAT_SUPPORTED_P
#define TARGET_DECIMAL_FLOAT_SUPPORTED_P k1_target_decimal_float_supported_p

#undef TARGET_FIXED_POINT_SUPPORTED_P
#define TARGET_FIXED_POINT_SUPPORTED_P k1_target_fixed_point_supported_p

#undef TARGET_VECTOR_MODE_SUPPORTED_P
#define TARGET_VECTOR_MODE_SUPPORTED_P k1_target_vector_mode_supported_p

#undef TARGET_VECTORIZE_SUPPORT_VECTOR_MISALIGNMENT
#define TARGET_VECTORIZE_SUPPORT_VECTOR_MISALIGNMENT                           \
  k1_target_support_vector_misalignment

#undef TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES k1_target_promote_prototypes

#undef TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES k1_arg_partial_bytes

#undef TARGET_MUST_PASS_IN_STACK
#define TARGET_MUST_PASS_IN_STACK must_pass_in_stack_var_size

#undef TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE k1_target_pass_by_reference

#undef TARGET_SECONDARY_RELOAD
#define TARGET_SECONDARY_RELOAD k1_target_secondary_reload

#undef TARGET_INIT_BUILTINS
#define TARGET_INIT_BUILTINS k1_target_init_builtins

#undef TARGET_EXPAND_BUILTIN
#define TARGET_EXPAND_BUILTIN k1_target_expand_builtin

#undef TARGET_CANNOT_FORCE_CONST_MEM
#define TARGET_CANNOT_FORCE_CONST_MEM k1_cannot_force_const_mem

#undef TARGET_SCHED_ADJUST_COST
#define TARGET_SCHED_ADJUST_COST k1_target_sched_adjust_cost

#undef TARGET_SCHED_ISSUE_RATE
#define TARGET_SCHED_ISSUE_RATE k1_target_sched_issue_rate

#undef TARGET_SCHED_FIRST_CYCLE_MULTIPASS_DFA_LOOKAHEAD
#define TARGET_SCHED_FIRST_CYCLE_MULTIPASS_DFA_LOOKAHEAD                       \
  k1_target_sched_first_cycle_multipass_dfa_lookahead

#undef TARGET_SCHED_SET_SCHED_FLAGS
#define TARGET_SCHED_SET_SCHED_FLAGS k1_target_sched_set_sched_flags

#undef TARGET_SCHED_INIT_GLOBAL
#define TARGET_SCHED_INIT_GLOBAL k1_target_sched_init_global

#undef TARGET_SCHED_DFA_NEW_CYCLE
#define TARGET_SCHED_DFA_NEW_CYCLE k1_target_sched_dfa_new_cycle

#undef TARGET_SCHED_REASSOCIATION_WIDTH
#define TARGET_SCHED_REASSOCIATION_WIDTH k1_reassociation_width

#undef TARGET_RTX_COSTS
#define TARGET_RTX_COSTS k1_target_rtx_costs

#undef TARGET_ADDRESS_COST
#define TARGET_ADDRESS_COST k1_target_address_cost

#undef TARGET_REGISTER_MOVE_COST
#define TARGET_REGISTER_MOVE_COST k1_target_register_move_cost

#undef TARGET_REGISTER_PRIORITY
#define TARGET_REGISTER_PRIORITY k1_target_register_priority

#undef TARGET_FUNCTION_OK_FOR_SIBCALL
#define TARGET_FUNCTION_OK_FOR_SIBCALL k1_function_ok_for_sibcall

#undef TARGET_TRAMPOLINE_INIT
#define TARGET_TRAMPOLINE_INIT k1_target_trampoline_init

#undef TARGET_BUILTIN_DECL
#define TARGET_BUILTIN_DECL k1_target_builtin_decl

#undef TARGET_LEGITIMIZE_ADDRESS
#define TARGET_LEGITIMIZE_ADDRESS k1_target_legitimize_address

#undef TARGET_CONST_ANCHOR
#define TARGET_CONST_ANCHOR 0x200

#undef TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE k1_function_prologue

#undef TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE k1_function_epilogue

#undef TARGET_ASM_FINAL_POSTSCAN_INSN
#define TARGET_ASM_FINAL_POSTSCAN_INSN k1_target_asm_final_postscan_insn

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

#undef TARGET_FUNCTION_ARG_BOUNDARY
#define TARGET_FUNCTION_ARG_BOUNDARY k1_function_arg_boundary

#undef TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE k1_function_arg_advance

#undef TARGET_LEGITIMATE_CONSTANT_P
#define TARGET_LEGITIMATE_CONSTANT_P k1_legitimate_constant_p

#undef TARGET_CONDITIONAL_REGISTER_USAGE
#define TARGET_CONDITIONAL_REGISTER_USAGE k1_target_conditional_register_usage

#undef TARGET_ASM_OUTPUT_ADDR_CONST_EXTRA
#define TARGET_ASM_OUTPUT_ADDR_CONST_EXTRA k1_output_addr_const_extra

#undef TARGET_INVALID_WITHIN_DOLOOP
#define TARGET_INVALID_WITHIN_DOLOOP k1_invalid_within_doloop

#undef TARGET_MODE_DEPENDENT_ADDRESS_P
#define TARGET_MODE_DEPENDENT_ADDRESS_P k1_mode_dependent_address_p

#undef TARGET_CAN_USE_DOLOOP_P
#define TARGET_CAN_USE_DOLOOP_P can_use_doloop_if_innermost

#undef TARGET_SET_CURRENT_FUNCTION
#define TARGET_SET_CURRENT_FUNCTION k1_set_current_function

#undef TARGET_ASM_FILE_START
#define TARGET_ASM_FILE_START k1_file_start

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
