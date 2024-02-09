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
#include "system.h"
#include "coretypes.h"
#include "tm.h"

#include "calls.h"
#include "cfgloop.h"
#include "cppdefault.h"
#include "cpplib.h"
#include "diagnostic.h"
#include "incpath.h"
#include "df.h"
#include "expr.h"
#include "function.h"
#include "ggc.h"
#include "hard-reg-set.h"
#include "insn-attr.h"
#include "insn-codes.h"
#include "insn-modes.h"
#include "langhooks.h"
#include "libfuncs.h"
#include "output.h"
#include "hw-doloop.h"
#include "opts.h"
#include "params.h"
#include "recog.h"
#include "regs.h"
#include "rtl.h"
#include "sched-int.h"
#include "toplev.h"
#include "stor-layout.h"
#include "varasm.h"
#include "tree.h"
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
static int emit_colon;

/* interface with MDS */
enum k1c_abi k1c_cur_abi;

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

  /* The offset of arg_pointer_rtx from the frame pointer.  */
  HOST_WIDE_INT arg_pointer_fp_offset;

  /* Offset from the frame pointer to the first local variable slot */
  HOST_WIDE_INT fp_local_offset;

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

  rtx pic_reg;

  rtx stack_check_block_label;
  rtx stack_check_block_seq, stack_check_block_last;
  /* vec<fake_SC_t,va_gc> *fake_SC_registers; */
};

/* K1 frame info

     +-------------------------------+
     |                               |
     |  incoming stack arguments     |
     +-------------------------------+
     |                               | <----CALLER FRAME
     | scratch area                  |
     |                       +-------------------------------+
 /-->+-----------------------|                               |
 |                           |  callee-allocated save area   |
 |                           |  for register varargs         |
initial stack pointer        |                               |
			     +-------------------------------+ <---- frame
pointer / arg pointer |            padding            |
			     +-------------------------------+
			     |                               |
			     | local variable                |
			     |                               |
			     +-------------------------------+
			     |                               |
			     | callee-saved register         |
			     |                               |
			     +-------------------------------+
			     | RA                            |
			     +-------------------------------+
			     | dynamic allocation            |
			     +-------------------------------+
			     |                               |
			     | outgoing stack arguments      |
			     |                               |
			     +-------------------------------+
			     |                               | ^
			     | scratch area                  | |
STACK_POINTER_OFFSET (K1C_SCRATCH_AREA_SIZE) |                               | v
			     +-------------------------------+ <---- stack
pointer

 */

/*

		  +---------------+
		  | Varargs       |
		  |               |
		  |               |
		  +---------------+
	    FP--->| [Static chain]|
		  +---------------+
		  | Local         |
		  | Variable      |
		  |               |
		  +---------------+
		  |               |
		  | Register      |
		  | Save          |
		  |               |
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
      cfun->machine->frame.reg_offset[FRAME_POINTER_REGNUM] = sp_offset;
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

  /* If either of FP or automatic var is moved, beware of this
     value */
  /* frame->fp_local_offset = UNITS_PER_WORD; */

  /* Next are automatic variables. */
  sp_offset += get_frame_size ();

  /* ABI requires 16-bytes (128bits) alignment. */
#define K1_STACK_ALIGN(LOC) (((LOC) + 0b1111) & ~0b1111)

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

int
k1_starting_frame_offset (void)
{
  struct k1_frame_info *frame;
  k1_compute_frame_info ();
  frame = &cfun->machine->frame;

  return frame->fp_local_offset;
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

static rtx
k1_pic_register (void)
{
  if (cfun->machine->pic_reg == NULL_RTX && can_create_pseudo_p ())
    {
      cfun->machine->pic_reg = gen_reg_rtx (Pmode);
      /* We need this use for reload to handle the reg before we emit
	 insn using it in the expand_prologue */
      emit_use (cfun->machine->pic_reg);
    }

  if (cfun->machine->pic_reg != NULL_RTX)
    return cfun->machine->pic_reg;

  gcc_unreachable ();
}

static const char *prf_reg_names[] = {K1C_K1C_PRF_REGISTER_NAMES};
/* Implement HARD_REGNO_MODE_OK.  */

int
k1_hard_regno_mode_ok (unsigned regno, enum machine_mode mode)
{
  // SI/DI -> K1C_GRF_FIRST_REGNO - K1C_GRF_LAST_REGNO => OK
  // SI/DI -> K1C_SRF_FIRST_REGNO - K1C_SRF_LAST_REGNO => OK
  // TI    -> K1C_GRF_FIRST_REGNO - K1C_GRF_LAST_REGNO && even => OK

  return (GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
	 || (mode == TImode && (regno >= K1C_GRF_FIRST_REGNO)
	     && (regno <= K1C_GRF_LAST_REGNO) && (regno % 2 == 0));
}

static unsigned char
k1_class_max_nregs (reg_class_t regclass, enum machine_mode mode)
{
  switch (regclass)
    {
    case GRF_REGS:
    case PRF_REGS:
    case ALL_REGS:
      return HARD_REGNO_NREGS (0, mode);

    case SRF_REGS:
    case SRF32_REGS:
    case SRF64_REGS:
      return 1;

    case NO_REGS:
      return 0;

    default:
      break;
    }
  gcc_unreachable ();
}

bool
k1_cannot_change_mode_class (enum machine_mode from, enum machine_mode to,
			     enum reg_class reg_class)
{
  // FIXME FOR COOLIDGE
  if (TARGET_32 && (reg_class == SRF_REGS && (from == SImode && to == SImode)))
    return true; // SRF in 32bits mode can't be anything else than SI

  if (!TARGET_32)
    {
      if ((reg_class == SRF32_REGS) && (from == SImode || to == SImode))
	return true;

      if ((reg_class == SRF64_REGS) && (from == DImode || to == DImode))
	return true;
    }

  return false;
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

static rtx k1_target_legitimize_pic_address (rtx orig, rtx reg);

static bool k1_has_gprel (rtx x);

static rtx k1_legitimize_gp_address (rtx x, rtx reg);

static bool k1_rtx_constant_in_small_data_p (enum machine_mode mode);

static bool k1_legitimate_constant_p (enum machine_mode mode ATTRIBUTE_UNUSED,
				      rtx x);

/* Table of machine attributes.  */
static const struct attribute_spec k1_attribute_table[] = {
  /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler,
     affects_type } */
  {"no_save_regs", 0, 0, true, false, false, k1_handle_fndecl_attribute, false},
  {"farcall", 0, 0, true, false, false, k1_handle_fndecl_attribute, false},
  {NULL, 0, 0, false, false, false, NULL, false}};

/* The -G setting, or the configuration's default small-data limit if
   no -G option is given.  */
static unsigned int k1_small_data_threshold;

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

/* A processor implementing K1.  */
// FIXME FOR COOLIDGE
struct processor
{
  const char *const name;
  //  enum aarch64_processor core;
  const char *arch;
  const unsigned long flags;
  /* const struct tune_params *const tune; */
};

/* Target specification.  These are populated as commandline arguments
   are processed, or NULL if not specified.  */
static const struct processor *selected_arch;
static const struct processor *selected_cpu;
// static const struct processor *selected_tune;

static int
k1_tls_symbol_ref_1 (rtx *x, void *data ATTRIBUTE_UNUSED)
{
  if (GET_CODE (*x) == SYMBOL_REF)
    return SYMBOL_REF_TLS_MODEL (*x) != 0;

  /* Don't recurse into UNSPEC_TLS looking for TLS symbols; these are
     TLS offsets, not real symbol references.  */
  if (GET_CODE (*x) == UNSPEC && XINT (*x, 1) == UNSPEC_TLS)
    return -1;

  return 0;
}

/* Returns 0 if there is no TLS ref, != 0 if there is.

  Beware that UNSPEC_TLS are not symbol ref, they are offset within
  TLS.
 */
int
k1_has_tls_reference (rtx x)
{
  return for_each_rtx (&x, &k1_tls_symbol_ref_1, NULL);
}

static int
k1_has_tprel_1 (rtx *x, void *data ATTRIBUTE_UNUSED)
{
  if (GET_CODE (*x) == UNSPEC && XINT (*x, 1) == UNSPEC_TLS)
    return 1;

  return 0;
}

/* Returns 1 if an UNSPEC_TLS has been found (ie. a TLS offset), 0 if
   not */
static int
k1_has_tprel (rtx x)
{
  return for_each_rtx (&x, &k1_has_tprel_1, NULL);
}

static int
k1_has_unspec_reference_1 (rtx *x, void *data ATTRIBUTE_UNUSED)
{
  return (GET_CODE (*x) == UNSPEC
	  && (XINT (*x, 1) == UNSPEC_GOT || XINT (*x, 1) == UNSPEC_GOTOFF
	      || XINT (*x, 1) == UNSPEC_GPREL || XINT (*x, 1) == UNSPEC_GPREL10
	      || XINT (*x, 1) == UNSPEC_TLS
	      /* || XINT (*x, 1) == UNSPEC_FUNCDESC_GOT */
	      /* || XINT (*x, 1) == UNSPEC_FUNCDESC_GOTOFF) */
	      ));
}

static int
k1_needs_symbol_reloc_1 (rtx *x, void *gprel)
{
  rtx **sym = (rtx **) gprel;

  if (k1_has_unspec_reference_1 (x, gprel))
    return -1;

  if (!gprel)
    {
      /* One could believe that we should filter out functions here,
	 but it's not true. Function calls won't use
	 legitimate_constant_p, and other uses of function addresses
	 need to go through the GOT (think about comparing
	 function poitners). */
      return GET_CODE (*x) == SYMBOL_REF;
    }
  else
    {
      int res = GET_CODE (*x) == SYMBOL_REF && !SYMBOL_REF_FUNCTION_P (*x)
		&& !(SYMBOL_REF_DECL (*x)
		     && (decl_readonly_section (SYMBOL_REF_DECL (*x), 0)
			 || TREE_READONLY (SYMBOL_REF_DECL (*x))));
      if (res)
	*sym = x;
      return res;
    }
}

static int
k1_needs_symbol_reloc (rtx x)
{
  return for_each_rtx (&x, &k1_needs_symbol_reloc_1, NULL);
}

static int
k1_needs_gp_symbol_reloc (rtx x, rtx **sym)
{
  rtx *s;
  int res;

  res = for_each_rtx (&x, &k1_needs_symbol_reloc_1, &s);
  if (res && sym)
    *sym = s;

  return res;
}

/* static int */
/* k1_has_symbol_or_label (rtx *x, void *dummy) */
/* { */
/*   return GET_CODE (*x) == SYMBOL_REF || GET_CODE (*x) == LABEL_REF; */
/* } */

static int
k1_has_unspec_reference (rtx x)
{
  return for_each_rtx (&x, &k1_has_unspec_reference_1, NULL);
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

#define SET_ABI_PARAMS(ARCH, CORE, CONV)                                       \
  do                                                                           \
    {                                                                          \
      char tmp_fixed_regs[] = {                                                \
	ARCH##_ABI_##CORE##_##CONV##_FIXED_REGISTERS 1,                        \
      };                                                                       \
      gcc_assert (sizeof (tmp_fixed_regs) == sizeof (fixed_regs));             \
      size_t _idx_tmp;                                                         \
      for (_idx_tmp = 0; _idx_tmp < sizeof (fixed_regs); _idx_tmp++)           \
	{                                                                      \
	  if (fixed_regs[_idx_tmp] == 2)                                       \
	    fixed_regs[_idx_tmp] = tmp_fixed_regs[_idx_tmp];                   \
	}                                                                      \
                                                                               \
      char tmp_call_used_regs[] = {                                            \
	ARCH##_ABI_##CORE##_##CONV##_CALL_USED_REGISTERS 1,                    \
      };                                                                       \
      gcc_assert (sizeof (tmp_call_used_regs) == sizeof (call_used_regs));     \
      for (_idx_tmp = 0; _idx_tmp < sizeof (call_used_regs); _idx_tmp++)       \
	{                                                                      \
	  if (call_used_regs[_idx_tmp] == 2)                                   \
	    call_used_regs[_idx_tmp] = tmp_call_used_regs[_idx_tmp];           \
	}                                                                      \
                                                                               \
      char tmp_call_really_used_regs[] = {                                     \
	ARCH##_ABI_##CORE##_##CONV##_CALL_REALLY_USED_REGISTERS 1,             \
      };                                                                       \
      gcc_assert (sizeof (tmp_call_really_used_regs)                           \
		  == sizeof (call_really_used_regs));                          \
      for (_idx_tmp = 0; _idx_tmp < sizeof (fixed_regs); _idx_tmp++)           \
	{                                                                      \
	  if (call_really_used_regs[_idx_tmp] == 2)                            \
	    call_really_used_regs[_idx_tmp]                                    \
	      = tmp_call_really_used_regs[_idx_tmp];                           \
	}                                                                      \
    }                                                                          \
  while (0)

static void
k1_target_conditional_register_usage (void)
{
  const int is_k1c = true;

  k1c_cur_abi = K1C_ABI_K1C_REGULAR;
  SET_ABI_PARAMS (K1C, K1C, REGULAR);

  // FIXME FOR COOLIDGE
  // FIXME AUTO: rework ABI selection
  /* if (flag_pic && TARGET_32){ */
  /*   k1c_cur_abi = K1C_ABI_K1C_PIC; */
  /*   SET_ABI_PARAMS(K1C,K1C,PIC); */
  /* } /\* else if (TARGET_FDPIC && !TARGET_64){ *\/ */
  /*   /\* k1b_cur_abi = K1B_ABI_K1BDP_FDPIC; *\/ */
  /*   /\* /\\* if we're in FDPIC, fix the r9 register *\\/ *\/ */
  /*   /\* fix_register ("r9", 1, 1); *\/ */
  /* }  */
  /* else if (flag_pic && !TARGET_32) { */
  /*   k1c_cur_abi = K1C_ABI_K1C_PIC64; */
  /*   SET_ABI_PARAMS(K1C,K1C,PIC64); */
  /* } /\* else if (TARGET_FDPIC && TARGET_64) { *\/ */
  /*     error ("64bits does not support FDPIC"); */
  /* }  */
  /* else if (!TARGET_32) { */
  /*   k1c_cur_abi = K1C_ABI_K1C_EMBEDDED64; */
  /*   SET_ABI_PARAMS(K1C,K1C,EMBEDDED64); */
  /* } else { */
  /*   k1c_cur_abi = K1C_ABI_K1C_EMBEDDED; */
  /*   SET_ABI_PARAMS(K1C,K1C,EMBEDDED); */
  /* } */

  /* the following exists because there is no FDPIC ABI, simply patch the
   * default one */
  /* if (TARGET_FDPIC){ */
  /*   /\* if we're in FDPIC, fix the r9 register *\/ */
  /*   fix_register ("r9", 1, 1); */
  /* } */

  k1_sync_reg_rtx = gen_rtx_REG (SImode, K1C_SYNC_REG_REGNO);

  k1_link_reg_rtx = gen_rtx_REG (Pmode, K1C_RETURN_POINTER_REGNO);
  k1_data_start_symbol
    = gen_rtx_SYMBOL_REF (Pmode,
			  IDENTIFIER_POINTER (get_identifier ("_data_start")));
  K1C_ADJUST_REGISTER_NAMES;

  const char *prf_names[] = {K1C_K1C_PRF_REGISTER_NAMES};
  memcpy (prf_reg_names, prf_names, sizeof (prf_reg_names));
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

  /* Arguments larger than 4 bytes start at the next even boundary.  */
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
  struct k1_arg_info info;
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
      regno++;
      slot++;
    }

  /* Until load_multiple is fixed, these will be split in several ld */
  for (; regno < K1C_ARG_REG_SLOTS; regno += 2, slot += 2)
    {
      rtx insn = emit_move_insn (
	gen_rtx_MEM (TImode,
		     gen_rtx_PLUS (Pmode, frame_pointer_rtx,
				   GEN_INT (slot * UNITS_PER_WORD
					    + frame->arg_pointer_fp_offset))),
	gen_rtx_REG (TImode, K1C_ARGUMENT_POINTER_REGNO + regno));
      RTX_FRAME_RELATED_P (insn) = 1;
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

static int
k1_legitimize_reload_packed_address (rtx *x, void *data ATTRIBUTE_UNUSED)
{
  if (REG_P (*x) && REGNO (*x) >= FIRST_PSEUDO_REGISTER)
    *x = gen_rtx_REG (GET_MODE (*x), 62);

  return 0;
}

int
k1_legitimize_reload_address (rtx *ad, int opnum, int type ATTRIBUTE_UNUSED)
{
  if (recog_data.constraints[opnum][0] == 'Q'
      || (recog_data.constraints[opnum][0] == '='
	  && recog_data.constraints[opnum][1] == 'Q'))
    {
      *ad = copy_rtx (*ad);
      gcc_assert (GET_CODE (*ad) == PLUS);
      for_each_rtx (ad, k1_legitimize_reload_packed_address, NULL);

      return 1;
    }

  return 0;
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

/* void */
/* k1_output_load_multiple (rtx *operands) */
/* { */
/*   char pattern[100]; */
/*   unsigned int regno_dest = REGNO(operands[2]); */
/*   sprintf(pattern, k1_is_uncached_mem_op (operands [1])  */
/* 	  ? "lq.u $r%dr%d = 0[%%1]" : "lq $r%dr%d = 0[%%1]",  */
/* 	  regno_dest, regno_dest + 1); */

/*   gcc_assert(!(regno_dest & 1)); */

/*   output_asm_insn(pattern, operands); */
/* } */

/* void */
/* k1_output_store_multiple (rtx *operands) */
/* { */
/*   char pattern[100]; */
/*   unsigned int regno_dest = REGNO(operands[2]); */
/*   sprintf(pattern, k1_is_uncached_mem_op (operands [1])  */
/* 	  ? "sq.u 0[%%1] = $r%dr%d" : "sq.u $r%dr%d = 0[%%1]",  */
/* 	  regno_dest, regno_dest + 1); */

/*   gcc_assert(!(regno_dest & 1)); */

/*   output_asm_insn(pattern, operands); */
/* } */

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
    case 'Q':
      quadword_reg++;
      /* fallthrough */
    case 'q':
      quadword_reg++;
      break;

    case 'O':
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
      if (k1_is_uncached_mem_op (x))
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
		   reg_names[REGNO (operand)] + 2,
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
	      output_address (XEXP (operand, 0));
	      /* if (want_plt) */
	      /*   fprintf (file, ")"); */
	    }
	}
      return;

    case CONST_DOUBLE:
      /* if (paired_reg) { */
      /*     if (GET_MODE (operand) == VOIDmode) { */
      /*         fprintf (file, "0x%x:0x%x", */
      /*                  (unsigned int)CONST_DOUBLE_LOW (x), */
      /*                  (unsigned int)CONST_DOUBLE_HIGH (x)); */
      /*     } else if (GET_MODE (operand) == DFmode) { */
      /*         REAL_VALUE_TYPE r; */
      /*         long l[2]; */
      /*         REAL_VALUE_FROM_CONST_DOUBLE (r, operand); */
      /*         REAL_VALUE_TO_TARGET_DOUBLE (r, l); */
      /*         fprintf (file, "0x%x:0x%x", */
      /*                  (unsigned int)l[0], (unsigned int)l[1]); */

      /*     } else { */
      /*         gcc_unreachable (); */
      /*     } */
      /*     return; */
      /* } else */ if (GET_MODE (x) == SFmode)
	{
	  REAL_VALUE_TYPE r;
	  long l;
	  REAL_VALUE_FROM_CONST_DOUBLE (r, operand);
	  REAL_VALUE_TO_TARGET_SINGLE (r, l);
	  fprintf (file, "0x%x", (unsigned int) l);
	  return;
	}
      else if (GET_MODE (x) == DFmode)
	{
	  /* this is a double that should fit on less than 64bits */
	  REAL_VALUE_TYPE r;
	  long l[2];
	  REAL_VALUE_FROM_CONST_DOUBLE (r, operand);
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
		fprintf (file, "@tprel");
		if (!TARGET_32)
		  fprintf (file, "64");
		fprintf (file, "(");
		break;
	      case UNSPEC_GOT:
		fprintf (file, "@got");
		if (!TARGET_32)
		  fprintf (file, "64");
		fprintf (file, "(");
		break;
	      case UNSPEC_GOTOFF:
		fprintf (file, "@gotoff");
		if (!TARGET_32)
		  fprintf (file, "64");
		fprintf (file, "(");
		break;
	      /* case UNSPEC_FUNCDESC_GOTOFF: */
	      /*   fprintf (file, "@gotoff_funcdesc("); */
	      /*   break; */
	      /* case UNSPEC_FUNCDESC_GOT: */
	      /*   fprintf (file, "@got_funcdesc("); */
	      /*   break; */
	      case UNSPEC_GPREL:
		fprintf (file, "@gprel(");
		break;
	      case UNSPEC_GPREL10:
		fprintf (file, "@gprel10(");
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

#if 0
/* AP: seems it's not used anymore */
static rtx
un_unpsec(rtx x) {
    if (GET_CODE (x) == UNSPEC) {
        switch(XINT (x, 1)) {
          case UNSPEC_TLS:
            x = XVECEXP (x, 0, 0);
            break;
/* leave those -- handled by OUTPUT_ADDR_CONST_EXTRA macro */
          case UNSPEC_PIC:
          case UNSPEC_GOT:
          case UNSPEC_GOTOFF:
            break;
          default:
            gcc_unreachable();
            break;
        }
    } else if (GET_CODE (x) == PLUS) {
        XEXP (x, 0) = un_unpsec (XEXP (x, 0));
        XEXP (x, 1) = un_unpsec (XEXP (x, 1));
    } else if (GET_CODE (x) == CONST) {
        XEXP (x, 0) = un_unpsec (XEXP (x, 0));
    }

    return x;
}

/*FBT: un_unspec IS important */
static rtx
un_unspec(rtx x) {
    if (GET_CODE (x) == UNSPEC) {
        gcc_assert (XINT (x, 1) == UNSPEC_TLS);
        x = XVECEXP (x, 0, 0);
    } else if (GET_CODE (x) == PLUS) {
        XEXP (x, 0) = un_unspec (XEXP (x, 0));
        XEXP (x, 1) = un_unspec (XEXP (x, 1));
    } else if (GET_CODE (x) == CONST) {
        XEXP (x, 0) = un_unspec (XEXP (x, 0));
    }

    return x;
}
#endif

static const char *
k1_regname (rtx x)
{
  unsigned int regno;

  switch (GET_CODE (x))
    {
    case REG:
      if (GET_MODE (x) == TImode)
	return prf_reg_names[REGNO (x)];
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

/* Return true if rtx constants of mode MODE should be put into a small
   data section.  */

static bool
k1_rtx_constant_in_small_data_p (enum machine_mode mode)
{
  /* FIXME: For now for all GPREL, but we need a switch for that */
  return (TARGET_LOCAL_SDATA
	  && GET_MODE_SIZE (mode) <= k1_small_data_threshold);
}

/* Implement TARGET_ASM_SELECT_RTX_SECTION.  */

static section *
k1_select_rtx_section (enum machine_mode mode, rtx x,
		       unsigned HOST_WIDE_INT align)
{
  if (k1_rtx_constant_in_small_data_p (mode))
    {
      return get_named_section (NULL, ".sdata", 0);
    }

  return default_elf_select_rtx_section (mode, x, align);
}

/* Implement TARGET_IN_SMALL_DATA_P.  */

static bool
k1_in_small_data_p (const_tree decl)
{
  unsigned HOST_WIDE_INT size;

  if (!TARGET_LOCAL_SDATA || TREE_CODE (decl) == STRING_CST
      || TREE_CODE (decl) == FUNCTION_DECL)
    return false;

  if (TREE_CODE (decl) == VAR_DECL && DECL_SECTION_NAME (decl) != 0)
    {
      const char *name;

      /* Reject anything that isn't in a known small-data section.  */
      name = TREE_STRING_POINTER (DECL_SECTION_NAME (decl));
      if (strcmp (name, ".sdata") != 0 && strcmp (name, ".sbss") != 0)
	return false;

      /* If a symbol is defined externally, the assembler will use the
	 usual -G rules when deciding how to implement macros.  */
      /*      if (mips_lo_relocs[SYMBOL_GP_RELATIVE] || !DECL_EXTERNAL (decl))
	      return true; */
    }
#if 0
  /* Enforce -mlocal-sdata.  */
  if (!TARGET_LOCAL_SDATA && !TREE_PUBLIC (decl))
    return false;

  /* Enforce -mextern-sdata.  */
  if (!TARGET_EXTERN_SDATA && DECL_P (decl))
    {
      if (DECL_EXTERNAL (decl))
	return false;
      if (DECL_COMMON (decl) && DECL_INITIAL (decl) == NULL)
	return false;
    }
#endif

  /* We have traditionally not treated zero-sized objects as small data,
     so this is now effectively part of the ABI.  */
  size = int_size_in_bytes (TREE_TYPE (decl));

  return size > 0 && size <= k1_small_data_threshold;
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
  machine = (struct machine_function *) ggc_alloc_cleared_atomic (
    sizeof (struct machine_function));

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
      emit_move_insn (stack_end_val, gen_rtx_REG (Pmode, K1C_SR2_REGNO));
    }
  emit_insn (
    gen_rtx_SET (Pmode, tmp,
		 gen_rtx_MINUS (Pmode, stack_pointer_rtx, stack_end_val)));
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

/* Save/Restore register at offsets previously computed in frame information
 * layout.
 */
static void
k1_save_or_restore_callee_save_registers (bool restore)
{
  struct k1_frame_info *frame = &cfun->machine->frame;
  rtx insn;
  rtx base_rtx = stack_pointer_rtx;
  rtx (*gen_mem_ref) (enum machine_mode, rtx)
    = (frame_pointer_needed) ? gen_frame_mem : gen_rtx_MEM;
  unsigned limit = FIRST_PSEUDO_REGISTER;
  unsigned regno;
  unsigned regno2;

  for (regno = 0; regno < limit; regno++)
    {
      /* Already taken care in prologue/epilogue */
      if (regno == FRAME_POINTER_REGNUM && frame_pointer_needed)
	continue;

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
	    int saved_regno = regno;

	    if (regno == K1C_RETURN_POINTER_REGNO)
	      {
		/* spill_mode = Pmode; */
		rtx reg2 = gen_rtx_REG (spill_mode, 32);
		if (restore == false)
		  {
		    insn
		      = emit_move_insn (reg2, gen_rtx_REG (spill_mode, regno));
		    RTX_FRAME_RELATED_P (insn) = 1;
		  }
		saved_regno = 32;
	      }

	    if (restore)
	      {
		insn
		  = emit_move_insn (gen_rtx_REG (spill_mode, saved_regno), mem);

		// add_reg_note (insn, REG_CFA_RESTORE, gen_rtx_REG (spill_mode,
		// saved_regno));

		if (regno == K1C_RETURN_POINTER_REGNO)
		  {
		    insn
		      = emit_move_insn (gen_rtx_REG (spill_mode, regno),
					gen_rtx_REG (spill_mode, saved_regno));
		  }
	      }
	    else
	      {
		insn
		  = emit_move_insn (mem, gen_rtx_REG (spill_mode, saved_regno));
		RTX_FRAME_RELATED_P (insn) = 1;
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

  /* Should never have anything else FRAME_POINTER_REGNUM */
  if (from != FRAME_POINTER_REGNUM || to != STACK_POINTER_REGNUM)
    gcc_unreachable ();

  if (from == FRAME_POINTER_REGNUM && to == STACK_POINTER_REGNUM)
    return frame->frame_pointer_offset;

  gcc_unreachable ();
}

void
k1_expand_prologue (void)
{
  k1_compute_frame_info ();
  struct k1_frame_info *frame = &cfun->machine->frame;
  HOST_WIDE_INT size = frame->initial_sp_offset;
  rtx insn;
  rtx (*gen_set_gotp) (rtx target)
    = !TARGET_32 ? gen_set_gotp_di : gen_set_gotp_si;

  if (flag_pic && crtl->uses_pic_offset_table)
    {
      insn = emit_insn (gen_set_gotp (k1_pic_register ()));
      /* df_set_regs_ever_live(K1C_GLOBAL_POINTER_REGNO, true); */
    }

  if (size > 0)
    {
      insn
	= gen_add3_insn (stack_pointer_rtx, stack_pointer_rtx, GEN_INT (-size));
      RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
    }

  if (frame_pointer_needed)
    {
      /* Save previous FP */
      rtx fp_mem = gen_rtx_MEM (
	DImode,
	plus_constant (Pmode, stack_pointer_rtx,
		       cfun->machine->frame.reg_offset[FRAME_POINTER_REGNUM]));

      rtx insn
	= emit_move_insn (fp_mem, gen_rtx_REG (DImode, FRAME_POINTER_REGNUM));
      RTX_FRAME_RELATED_P (insn) = 1;

      /* FIXME AUTO: Add FP/RA store at frame bottom here */
      insn = gen_add3_insn (frame_pointer_rtx, stack_pointer_rtx,
			    GEN_INT (frame->frame_pointer_offset));
      RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
    }

  /* Save registers */
  k1_save_or_restore_callee_save_registers (0);
}

void
k1_expand_epilogue (void)
{
  struct k1_frame_info *frame = &cfun->machine->frame;
  HOST_WIDE_INT frame_size = frame->initial_sp_offset;
  rtx insn;

  k1_save_or_restore_callee_save_registers (1);

  if (frame_pointer_needed)
    {
      insn = emit_insn (gen_add3_insn (stack_pointer_rtx, frame_pointer_rtx,
				       GEN_INT (-frame->frame_pointer_offset)));
      RTX_FRAME_RELATED_P (insn) = 1;

      /* Restore previous FP */
      rtx fp_mem = gen_rtx_MEM (
	DImode,
	plus_constant (Pmode, stack_pointer_rtx,
		       cfun->machine->frame.reg_offset[FRAME_POINTER_REGNUM]));

      rtx insn
	= emit_move_insn (gen_rtx_REG (DImode, FRAME_POINTER_REGNUM), fp_mem);
    }

  if (frame_size != 0)
    {
      insn = GEN_INT (frame_size);
      insn = emit_insn (
	gen_add3_insn (stack_pointer_rtx, stack_pointer_rtx, insn));
      RTX_FRAME_RELATED_P (insn) = 1;
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

/* handle a symbol for PIC */
static rtx
k1_handle_symbol (rtx addr, rtx reg)
{
  int unspec = UNSPEC_GOT;
  rtx new_rtx = addr;

  if (reg == 0)
    {
      reg = gen_reg_rtx (Pmode);
      gcc_assert (can_create_pseudo_p ());
    }

  /* simply emit the sequence
   * lw $ry = @got(foo)[$r14]
   *
   * or
   * lw $ry = @got_funcdesc(bar)[$r14]
   */
  new_rtx = gen_rtx_UNSPEC (Pmode, gen_rtvec (1, addr), unspec);
  new_rtx = gen_rtx_CONST (Pmode, new_rtx);
  new_rtx = gen_rtx_PLUS (Pmode, k1_pic_register (), new_rtx);

  new_rtx = gen_const_mem (Pmode, new_rtx);
  emit_move_insn (reg, new_rtx);
  crtl->uses_pic_offset_table = TRUE;
  return reg;
}

bool
k1_legitimate_pic_operand_p (rtx x)
{
  if (GET_CODE (x) == SYMBOL_REF
      || (GET_CODE (x) == CONST && GET_CODE (XEXP (x, 0)) == PLUS
	  && GET_CODE (XEXP (XEXP (x, 0), 0)) == SYMBOL_REF))
    return false;

  return true;
}

static rtx
k1_target_legitimize_pic_address (rtx orig, rtx reg)
{
  rtx addr = orig;
  rtx new_rtx = orig;

  /* Local symbol references in (FD)PIC/PIE mode are relative to global
   * offset table, but do not require GOT entry.
   * to obtain address we generate code similar to:
   *
   * add $r0 = $r14, @gotoff(symbol);;
   *
   * with the optimizations turned off we can end up with two instructions
   *
   * make $r0 = @gotoff(symbol);;
   * add $r0 = $r14, $r0;;
   *
   * of course for functions in FDPIC we will get @gotoff_funcdesc
   */
  if (GET_CODE (addr) == SYMBOL_REF && SYMBOL_REF_LOCAL_P (addr)
      && !SYMBOL_REF_EXTERNAL_P (addr))
    {
      /* make it @gotoff by default,
       * i.e., not read-only local data,
       * and read-only local data in PIC */
      int unspec = UNSPEC_GOTOFF;

      /* @gotoff_funcdesc for function in FDPIC */
      /* if (TARGET_FDPIC && SYMBOL_REF_FUNCTION_P (addr)) */
      /*     unspec = UNSPEC_FUNCDESC_GOTOFF; */
      /* try to decide what case we really have here */
      /* else */
      if (SYMBOL_REF_DECL (addr)
	  && (!DECL_P (SYMBOL_REF_DECL (addr))
	      || !DECL_COMMON (SYMBOL_REF_DECL (addr))))
	{
	  tree decl = SYMBOL_REF_DECL (addr);
	  tree init = TREE_CODE (decl) == VAR_DECL
			? DECL_INITIAL (decl)
			: TREE_CODE (decl) == CONSTRUCTOR ? decl : 0;
	  int reloc = 0;
	  bool named_section, readonly;

	  if (init && init != error_mark_node)
	    reloc = compute_reloc_for_constant (init);

	  named_section
	    = TREE_CODE (decl) == VAR_DECL
	      && lookup_attribute ("section", DECL_ATTRIBUTES (decl));
	  readonly = decl_readonly_section (decl, reloc);
	  /* this captures also the readonly data with initializers
	   * which have to be relocated during runtime, i.e., data.rel.ro
	   * we don't want that */
	  /* || TREE_READONLY(decl); */

	  /* Taken from frv, seems to work */
	  if (named_section)
	    return k1_handle_symbol (addr, reg);
	  /* read-only in FDPIC -- we need to find our .rodata section
	   * so go through the _gp symbol
	   */
	  /* else if (readonly && TARGET_FDPIC) */
	  /*   return k1_handle_label_or_readonly(addr, reg); */
	}

      if (reg == NULL_RTX)
	{
	  gcc_assert (can_create_pseudo_p ());
	  reg = gen_reg_rtx (Pmode);
	}

      new_rtx = gen_rtx_UNSPEC (Pmode, gen_rtvec (1, addr), unspec);
      new_rtx = gen_rtx_CONST (Pmode, new_rtx);

      emit_move_insn (reg, k1_pic_register ());
      emit_move_insn (reg, gen_rtx_PLUS (Pmode, reg, new_rtx));
      crtl->uses_pic_offset_table = TRUE;

      return reg;
    }
  /* Non-local symbols require GOT entry to obtain the pointer. We generate:
   * lw $r0 = @got(symbol)[$r14]
   */
  else if (GET_CODE (addr) == SYMBOL_REF)
    return k1_handle_symbol (addr, reg);

  /* Checkout if we don't have to add UNSPEC in const and plus */
  else if (GET_CODE (addr) == CONST || GET_CODE (addr) == PLUS)
    {
      rtx base;

      if (GET_CODE (addr) == CONST)
	{
	  addr = XEXP (addr, 0);
	  if (GET_CODE (addr) == UNSPEC)
	    return orig;
	  gcc_assert (GET_CODE (addr) == PLUS);
	}

      if (XEXP (addr, 0)
	  == has_hard_reg_initial_val (Pmode, PIC_OFFSET_TABLE_REGNUM))
	return orig;

      if (reg == 0)
	{
	  gcc_assert (can_create_pseudo_p ());
	  reg = gen_reg_rtx (Pmode);
	}

      base = k1_target_legitimize_pic_address (XEXP (addr, 0), reg);
      addr = k1_target_legitimize_pic_address (XEXP (addr, 1),
					       base == reg ? NULL_RTX : reg);

      if (GET_CODE (base) == UNSPEC
	  && (GET_CODE (XVECEXP (base, 0, 0)) == SYMBOL_REF
	      || GET_CODE (XVECEXP (base, 0, 0)) == LABEL_REF)
	  && GET_CODE (addr) == CONST_INT)
	{
	  XVECEXP (base, 0, 0)
	    = gen_rtx_PLUS (Pmode, XVECEXP (base, 0, 0), addr);
	  return base;
	}

      return gen_rtx_PLUS (Pmode, base, addr);
    }

  return orig;
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
    return SYMBOL_ABSOLUTE;

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

/* Expands a mov which second operand is a constant. Returns TRUE
   if caller pattern should not expand anymore (ie. call DONE macro).
*/
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

      /* If we have (const (plus symbol offset)), separate out the offset
	 before we start classifying the symbol.  */
      split_const (src, &base, &offset);

      sty = k1_classify_symbol (base);
      switch (sty)
	{
	case SYMBOL_ABSOLUTE:
	  /* Emit: dest = sym */
	  emit_insn (gen_rtx_SET (Pmode, dest, src));
	  break;

	case SYMBOL_GOT:
	  gcc_assert (INTVAL (offset) == 0);
	  /*
	   * Emit dest = *(@got(sym) + $pic)
	   */
	  new_rtx = gen_rtx_UNSPEC (Pmode, gen_rtvec (1, base), UNSPEC_GOT);

	  emit_move_insn (dest, k1_pic_register ());

	  emit_move_insn (dest, gen_rtx_MEM (Pmode, gen_rtx_PLUS (Pmode, dest,
								  new_rtx)));

	  crtl->uses_pic_offset_table = true;
	  break;

	case SYMBOL_GOTOFF:
	  gcc_assert (INTVAL (offset) == 0);
	  /*
	   * Emit dest = @gotoff(sym)[$pic]
	   */
	  new_rtx = gen_rtx_UNSPEC (Pmode, gen_rtvec (1, base), UNSPEC_GOTOFF);

	  emit_move_insn (dest, k1_pic_register ());

	  emit_move_insn (dest, gen_rtx_PLUS (Pmode, dest, new_rtx));

	  crtl->uses_pic_offset_table = true;
	  break;

	  /* int unspec = UNSPEC_GOTOFF; */
	  /* if (dest == NULL_RTX) */
	  /*   { */
	  /*     gcc_assert (can_create_pseudo_p ()); */
	  /*     dest = gen_reg_rtx (Pmode); */
	  /*   } */

	  /* new_rtx = gen_rtx_UNSPEC (Pmode, gen_rtvec (1, src), unspec); */
	  /* new_rtx = gen_rtx_CONST (Pmode, new_rtx); */

	  /* emit_move_insn(dest, k1_pic_register_initial_val ()); */
	  /* emit_move_insn(dest, gen_rtx_PLUS (Pmode, dest, new_rtx)); */
	  /* crtl->uses_pic_offset_table = TRUE; */
	  break;

	case SYMBOL_TPREL:
	  operands[1] = k1_legitimize_tls_reference (src);
	  emit_insn (gen_rtx_SET (VOIDmode, dest, operands[1]));
	  gcc_assert (operands[1] != src);
	  break;
	}
      return;
    }

  emit_insn (gen_rtx_SET (VOIDmode, dest, src));

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

/* This expander may be needed later. */
/* bool */
/* k1_expand_mov (rtx operands[]) */
/* { */
/*     if (k1_has_tls_reference (operands[1])) */
/*       { */
/*         rtx src = operands[1]; */
/*         operands[1] = k1_legitimize_tls_reference (src); */
/*         gcc_assert (operands[1] != src); */
/*       } */
/*     else if (!register_operand (operands[0], GET_MODE(operands[0]))) */
/*       operands[1] = force_reg (GET_MODE(operands[0]), operands[1]); */
/*     else if (flag_pic /\* || TARGET_FDPIC *\/) */
/*       { */
/*         if (SYMBOLIC_CONST(operands[1])) */
/*           operands[1] = k1_target_legitimize_pic_address (operands[1],
 * operands[0]); */
/*       } */
/*     else if (TARGET_GPREL) { */
/*       operands[1] = k1_legitimize_gp_address(operands[1], operands[0]); */
/*     } */

/*     return false; */
/* } */

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
  if (GET_MODE_SIZE (mode) <= 4)
    return 5;
  else if (GET_MODE_SIZE (mode) == 8)
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
  /* FIXME AUTO: disabling vector support */
  /* K1_BUILTIN_ABDHP, */
  /* K1_BUILTIN_ADDHP, */
  K1_BUILTIN_ADDS,
  K1_BUILTIN_AWAIT,
  K1_BUILTIN_BARRIER,
  /* FIXME AUTO: no more bwlu */
  /* K1_BUILTIN_BWLU, */
  /* FIXME AUTO: disabling vector support */
  /* K1_BUILTIN_BWLUHP, */
  /* K1_BUILTIN_BWLUWP, */
  K1_BUILTIN_CBS,
  K1_BUILTIN_CBSW,
  K1_BUILTIN_CBSD,
  K1_BUILTIN_CBSDL,
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
  K1_BUILTIN_FADDRN,
  K1_BUILTIN_FADDRND,
  /* FIXME AUTO: disabling vector support */
  /* K1_BUILTIN_FADDWP, */
  /* K1_BUILTIN_FADDRNWP, */
  K1_BUILTIN_FCDIV,
  K1_BUILTIN_FCDIVD,
  K1_BUILTIN_FCMA,
  K1_BUILTIN_FCMAWD,
  K1_BUILTIN_FCMS,
  K1_BUILTIN_FCMSWD,
  K1_BUILTIN_FDMA,
  K1_BUILTIN_FDMAWD,
  K1_BUILTIN_FDMS,
  K1_BUILTIN_FDMSWD,
  K1_BUILTIN_FENCE,
  K1_BUILTIN_FFMAW,
  K1_BUILTIN_FFMAWRN,
  K1_BUILTIN_FFMAD,
  K1_BUILTIN_FFMADRN,
  /* FIXME AUTO: disabling vector support */
  /* K1_BUILTIN_FFMAWP, */
  /* K1_BUILTIN_FFMARNWP, */
  K1_BUILTIN_FFMAWD,
  K1_BUILTIN_FFMAWDRN,
  K1_BUILTIN_FFMSW,
  K1_BUILTIN_FFMSWRN,
  K1_BUILTIN_FFMSWP,
  K1_BUILTIN_FFMSWPRN,
  K1_BUILTIN_FFMSD,
  K1_BUILTIN_FFMSDRN,
  K1_BUILTIN_FFMSWD,
  K1_BUILTIN_FFMSWDRN,

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
  K1_BUILTIN_FMULRN,
  K1_BUILTIN_FMULRND,
  K1_BUILTIN_FMULWP,
  K1_BUILTIN_FMULRNWP,
  K1_BUILTIN_FMULWD,
  K1_BUILTIN_FMULRNWD,
  K1_BUILTIN_FSBFRN,
  K1_BUILTIN_FSBFRND,
  K1_BUILTIN_FSBFWP,
  K1_BUILTIN_FSBFRNWP,
  K1_BUILTIN_FSDIV,
  K1_BUILTIN_FSINV,
  K1_BUILTIN_FSINVN,
  K1_BUILTIN_FSISR,
  K1_BUILTIN_FSDIVD,
  K1_BUILTIN_FSINVD,
  K1_BUILTIN_FSINVND,
  K1_BUILTIN_FSISRD,
  K1_BUILTIN_GET,
  K1_BUILTIN_GET_R,
  K1_BUILTIN_WFXL,
  K1_BUILTIN_WFXM,
  K1_BUILTIN_IINVAL,
  K1_BUILTIN_IINVALS,
  K1_BUILTIN_ITOUCHL,
  K1_BUILTIN_LANDHP,
  K1_BUILTIN_LBSU,
  K1_BUILTIN_LBZU,
  K1_BUILTIN_LHSU,
  K1_BUILTIN_LHZU,
  K1_BUILTIN_LDU,
  K1_BUILTIN_LWZU,
  K1_BUILTIN_MADUUCIWD,
  /* K1_BUILTIN_RXOR, */

  /* FIXME AUTO: disabling vector support */
  /* K1_BUILTIN_SBFHP, */
  K1_BUILTIN_SBMM8,
  K1_BUILTIN_SBMMT8,
  K1_BUILTIN_SCALL,
  K1_BUILTIN_SET,
  K1_BUILTIN_SET_PS,
  /* FIXME AUTO: disabling vector support */
  /* K1_BUILTIN_SLLHPS, */
  /* K1_BUILTIN_SLLHPS_R, */
  /* K1_BUILTIN_SRAHPS, */
  /* K1_BUILTIN_SRAHPS_R, */
  K1_BUILTIN_SLEEP,
  K1_BUILTIN_STOP,
  K1_BUILTIN_STSU,
  K1_BUILTIN_STSUD,
  K1_BUILTIN_SYNCGROUP,
  K1_BUILTIN_TOUCHL,
  K1_BUILTIN_TLBDINVAL,
  K1_BUILTIN_TLBIINVAL,
  K1_BUILTIN_TLBPROBE,
  K1_BUILTIN_TLBREAD,
  K1_BUILTIN_TLBWRITE,
  K1_BUILTIN_SAT,
  K1_BUILTIN_SATD,
  K1_BUILTIN_FWIDENLHW,
  K1_BUILTIN_FWIDENLHWP,
  K1_BUILTIN_FWIDENMHW,
  K1_BUILTIN_FWIDENMHWP,
  K1_BUILTIN_FNARROWWH,
  K1_BUILTIN_FNARROWDWP,
  K1_BUILTIN_WAITIT,

  K1_BUILTIN_SRFSIZE,

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

  ADD_K1_BUILTIN (ADDS, "adds", intSI, intSI, intSI);
  ADD_K1_BUILTIN (AWAIT, "await", VOID);
  ADD_K1_BUILTIN (BARRIER, "barrier", VOID);
  /* FIXME AUTO: no more bwlu */
  /* ADD_K1_BUILTIN (BWLU,    "bwlu",        uintSI, uintSI, uintSI, uintSI,
   * uintSI, uintHI); */

  /* FIXME AUTO: disabling vector support */
  /* ADD_K1_BUILTIN (BWLUHP,  "bwluhp",      uintSI,  uintSI,  uintSI, uintSI);
   */
  /* ADD_K1_BUILTIN (BWLUWP,  "bwluwp",      uintDI,  uintDI,  uintDI, uintSI);
   */
  ADD_K1_BUILTIN (CBS, "cbs", intSI, uintSI);
  ADD_K1_BUILTIN (CBSDL, "cbsdl", intDI, uintDI);
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
  ADD_K1_BUILTIN (FADDRN, "faddrn", floatSF, floatSF, floatSF);
  ADD_K1_BUILTIN (FADDRND, "faddrnd", floatDF, floatDF, floatDF);

  /* FIXME AUTO: disabling vector support */
  /* ADD_K1_BUILTIN (FADDWP,  "faddwp",      vect2SF,vect2SF,vect2SF); */
  /* ADD_K1_BUILTIN (FADDRNWP,"faddrnwp",    vect2SF,vect2SF,vect2SF); */

  ADD_K1_BUILTIN (FCDIV, "fcdiv", floatSF, floatSF, floatSF);
  ADD_K1_BUILTIN (FCDIVD, "fcdivd", floatDF, floatDF, floatDF);
  /* FIXME AUTO : Disable use of create_SC_* */
  /* ADD_K1_BUILTIN (FCMA,    "fcma",
   * floatSF,floatSF,floatSF,floatSF,floatSF); */
  /* ADD_K1_BUILTIN (FCMAWD,  "fcmawd",
   * floatDF,floatSF,floatSF,floatSF,floatSF); */
  /* ADD_K1_BUILTIN (FCMS,    "fcms",
   * floatSF,floatSF,floatSF,floatSF,floatSF); */
  /* ADD_K1_BUILTIN (FCMSWD,  "fcmswd",
   * floatDF,floatSF,floatSF,floatSF,floatSF); */
  /* ADD_K1_BUILTIN (FDMA,    "fdma",
   * floatSF,floatSF,floatSF,floatSF,floatSF); */
  /* ADD_K1_BUILTIN (FDMAWD,  "fdmawd",
   * floatDF,floatSF,floatSF,floatSF,floatSF); */
  /* ADD_K1_BUILTIN (FDMS,    "fdms",
   * floatSF,floatSF,floatSF,floatSF,floatSF); */
  /* ADD_K1_BUILTIN (FDMSWD,  "fdmswd",
   * floatDF,floatSF,floatSF,floatSF,floatSF); */
  ADD_K1_BUILTIN (FENCE, "fence", VOID);

  ADD_K1_BUILTIN (FFMAD, "ffmad", floatDF, floatDF, floatDF, floatDF);
  ADD_K1_BUILTIN (FFMADRN, "ffmadrn", floatDF, floatDF, floatDF, floatDF);

  /* FIXME AUTO: disabling vector support */
  /* ADD_K1_BUILTIN (FFMAWP,  "ffmawp",      vect2SF,vect2SF,vect2SF,vect2SF);
   */
  /* ADD_K1_BUILTIN (FFMARNWP,"ffmarnwp",    vect2SF,vect2SF,vect2SF,vect2SF);
   */

  ADD_K1_BUILTIN (FFMAW, "ffmaw", floatSF, floatSF, floatSF, floatSF);
  ADD_K1_BUILTIN (FFMAWRN, "ffmawrn", floatSF, floatSF, floatSF, floatSF);

  ADD_K1_BUILTIN (FFMAWD, "ffmawd", floatDF, floatDF, floatSF, floatSF);
  ADD_K1_BUILTIN (FFMAWDRN, "ffmawdrn", floatDF, floatDF, floatSF, floatSF);

  /* FIXME AUTO: disabling vector support */

  ADD_K1_BUILTIN (FFMSW, "ffmsw", floatSF, floatSF, floatSF, floatSF);
  ADD_K1_BUILTIN (FFMSWRN, "ffmswrn", floatSF, floatSF, floatSF, floatSF);

  /* FIXME AUTO: disabling vector support */
  /* ADD_K1_BUILTIN (FFMSWP,  "ffmswp",      vect2SF,vect2SF,vect2SF,vect2SF);
   */
  /* ADD_K1_BUILTIN (FFMSRNWP,"ffmsrnwp",    vect2SF,vect2SF,vect2SF,vect2SF);
   */

  ADD_K1_BUILTIN (FFMSD, "ffmsd", floatDF, floatDF, floatDF, floatDF);
  ADD_K1_BUILTIN (FFMSDRN, "ffmsdrn", floatDF, floatDF, floatDF, floatDF);

  ADD_K1_BUILTIN (FFMSWD, "ffmswd", floatDF, floatDF, floatSF, floatSF);
  ADD_K1_BUILTIN (FFMSWDRN, "ffmswdrn", floatDF, floatDF, floatSF, floatSF);

  ADD_K1_BUILTIN (FLOAT, "float", floatSF, uintQI, intSI, uintQI);
  ADD_K1_BUILTIN (FLOATD, "floatd", floatDF, uintQI, intDI, uintQI);
  ADD_K1_BUILTIN (FLOATU, "floatu", floatSF, uintQI, uintSI, uintQI);
  ADD_K1_BUILTIN (FLOATUD, "floatud", floatDF, uintQI, uintDI, uintQI);
  ADD_K1_BUILTIN (FIXED, "fixed", intSI, uintQI, floatSF, uintQI);
  ADD_K1_BUILTIN (FIXEDD, "fixedd", intDI, uintQI, floatDF, uintQI);
  ADD_K1_BUILTIN (FIXEDU, "fixedu", uintSI, uintQI, floatSF, uintQI);
  ADD_K1_BUILTIN (FIXEDUD, "fixedud", uintDI, uintQI, floatDF, uintQI);
  ADD_K1_BUILTIN (FMULRN, "fmulrn", floatSF, floatSF, floatSF);
  ADD_K1_BUILTIN (FMULRND, "fmulrnd", floatDF, floatDF, floatDF);

  /* FIXME AUTO: disabling vector support */
  /* ADD_K1_BUILTIN (FMULWP,  "fmulwp",      vect2SF,vect2SF,vect2SF); */
  /* ADD_K1_BUILTIN (FMULRNWP,"fmulrnwp",    vect2SF,vect2SF,vect2SF); */

  ADD_K1_BUILTIN (FMULWD, "fmulwd", floatDF, floatSF, floatSF);
  ADD_K1_BUILTIN (FMULRNWD, "fmulrnwd", floatDF, floatSF, floatSF);
  ADD_K1_BUILTIN (FSBFRN, "fsbfrn", floatSF, floatSF, floatSF);
  ADD_K1_BUILTIN (FSBFRND, "fsbfrnd", floatDF, floatDF, floatDF);

  /* FIXME AUTO: disabling vector support */
  /* ADD_K1_BUILTIN (FSBFWP,  "fsbfwp",      vect2SF,vect2SF,vect2SF); */
  /* ADD_K1_BUILTIN (FSBFRNWP,"fsbfrnwp",    vect2SF,vect2SF,vect2SF); */

  ADD_K1_BUILTIN (FSDIV, "fsdiv", floatSF, floatSF, floatSF);
  ADD_K1_BUILTIN (FSINV, "fsinv", floatSF, floatSF);
  ADD_K1_BUILTIN (FSINVN, "fsinvn", floatSF, floatSF);
  ADD_K1_BUILTIN (FSISR, "fsisr", floatSF, floatSF);
  ADD_K1_BUILTIN (FSDIVD, "fsdivd", floatDF, floatDF, floatDF);
  ADD_K1_BUILTIN (FSINVD, "fsinvd", floatDF, floatDF);
  ADD_K1_BUILTIN (FSINVND, "fsinvnd", floatDF, floatDF);
  ADD_K1_BUILTIN (FSISRD, "fsisrd", floatDF, floatDF);
  ADD_K1_BUILTIN (GET, "get", uintDI, intSI);
  ADD_K1_BUILTIN (GET_R, "get_r", uintDI, intSI);
  ADD_K1_BUILTIN (WFXL, "wfxl", VOID, uintQI, uintDI);
  ADD_K1_BUILTIN (WFXM, "wfxm", VOID, uintQI, uintDI);
  ADD_K1_BUILTIN (IINVAL, "iinval", VOID);
  ADD_K1_BUILTIN (IINVALS, "iinvals", VOID, constVoidPTR);
  ADD_K1_BUILTIN (ITOUCHL, "itouchl", VOID, voidPTR);
  /* FIXME AUTO: disabling vector support */
  /* ADD_K1_BUILTIN (LANDHP,  "landhp",      intSI,  intSI,  intSI); */
  ADD_K1_BUILTIN (LBSU, "lbsu", intQI, constVoidPTR);
  ADD_K1_BUILTIN (LBZU, "lbzu", uintQI, constVoidPTR);
  ADD_K1_BUILTIN (LHSU, "lhsu", intHI, constVoidPTR);
  ADD_K1_BUILTIN (LHZU, "lhzu", uintHI, constVoidPTR);
  ADD_K1_BUILTIN (LDU, "ldu", uintDI, constVoidPTR);
  ADD_K1_BUILTIN (LWZU, "lwzu", uintSI, constVoidPTR);
  /* ADD_K1_BUILTIN (RXOR,    "r_xord",      VOID,   uintQI, uintDI, uintHI); */

  /* FIXME AUTO: disabling vector support */
  /* ADD_K1_BUILTIN (SBFHP,   "sbfhp",       intSI,  intSI,  intSI); */
  ADD_K1_BUILTIN (SBMM8, "sbmm8", uintDI, uintDI, uintDI);
  ADD_K1_BUILTIN (SBMMT8, "sbmmt8", uintDI, uintDI, uintDI);
  /* FIXME AUTO: sat does not exist in k1c yet  */
  /* ADD_K1_BUILTIN (SAT,     "sat",         intSI,  intSI,  uintQI); */
  ADD_K1_BUILTIN (SATD, "satd", intDI, intDI, uintQI);
  ADD_K1_BUILTIN (SET, "set", VOID, intSI, uintDI);
  ADD_K1_BUILTIN (SET_PS, "set_ps", VOID, intSI, uintDI);

  /* FIXME AUTO: disabling vector support */
  /* ADD_K1_BUILTIN (SLLHPS,  "sllhps",      uintSI,  uintSI,  uintSI); */
  /* ADD_K1_BUILTIN (SLLHPS_R,"sllhps_r",    uintSI,  uintSI,  uintSI); */
  /* ADD_K1_BUILTIN (SRAHPS,  "srahps",      uintSI,  uintSI,  uintSI); */
  /* ADD_K1_BUILTIN (SRAHPS_R,"srahps_r",    uintSI,  uintSI,  uintSI); */
  ADD_K1_BUILTIN (SLEEP, "sleep", VOID);
  ADD_K1_BUILTIN (STOP, "stop", VOID);
  ADD_K1_BUILTIN (STSU, "stsu", uintSI, uintSI, uintSI);
  ADD_K1_BUILTIN (STSUD, "stsud", uintDI, uintDI, uintDI);
  ADD_K1_BUILTIN (SYNCGROUP, "syncgroup", VOID, uintSI);
  ADD_K1_BUILTIN (TLBDINVAL, "tlbdinval", VOID);
  ADD_K1_BUILTIN (TLBIINVAL, "tlbiinval", VOID);
  ADD_K1_BUILTIN (TLBPROBE, "tlbprobe", VOID);
  ADD_K1_BUILTIN (TLBREAD, "tlbread", VOID);
  ADD_K1_BUILTIN (TLBWRITE, "tlbwrite", VOID);

  ADD_K1_BUILTIN (FWIDENLHW, "fwidenlhw", floatSF, uintSI);
  ADD_K1_BUILTIN (FWIDENMHW, "fwidenmhw", floatSF, uintSI);

  /* FIXME AUTO: disabling vector support */
  /* ADD_K1_BUILTIN (FWIDENLHWP,  "fwidenlhwp", vect2SF , uintDI); */
  /* ADD_K1_BUILTIN (FWIDENMHWP,  "fwidenmhwp", vect2SF , uintDI); */
  /* ADD_K1_BUILTIN (FNARROWDWP,  "fnarrowdwp", vect2SI , vect2SF); */

  ADD_K1_BUILTIN (FNARROWWH, "fnarrowwh", uintHI, floatSF);
  ADD_K1_BUILTIN (WAITIT, "waitit", uintSI, uintSI);

  ADD_K1_BUILTIN (SRFSIZE, "srfsize", intSI, intSI);
}

static tree
k1_target_builtin_decl (unsigned code, bool initialize_p ATTRIBUTE_UNUSED)
{
  if (code >= ARRAY_SIZE (builtin_fndecls))
    return error_mark_node;

  return builtin_fndecls[code];
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

  if (!verify_const_int_arg (arg1, 6, false))
    {
      error ("__builtin_k1_get expects a 6 bits immediate argument.");
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

static rtx
k1_expand_builtin_get_r (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  if (!target)
    target = gen_reg_rtx (DImode);

  target = force_reg (DImode, target);
  arg1 = force_reg (DImode, arg1);
  emit_insn (gen_getdi_r (target, arg1, k1_sync_reg_rtx));

  return target;
}

static rtx
k1_expand_builtin_set (rtx target ATTRIBUTE_UNUSED, tree args, bool ps)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!verify_const_int_arg (arg1, 6, false))
    {
      error ("__builtin_k1_set expects a 6 bits immediate first argument.");
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

/*
 * MODE selects get/getd assembly insn
 */
static rtx
k1_expand_builtin_srfsize (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx reg_size;
  int reg_size_val;

  if (!verify_const_int_arg (arg1, 6, false))
    {
      error ("__builtin_k1_srfsize expects a 6 bits immediate argument.");
      return NULL_RTX;
    }
  const int regno = INTVAL (arg1) + K1C_SRF_FIRST_REGNO;

  if (!target)
    target = gen_reg_rtx (SImode);

  target = force_reg (SImode, target);
  reg_size_val
    = (!TARGET_32 && ((REGNO_REG_CLASS (regno) == SRF64_REGS))) ? 8 : 4;

  reg_size = gen_rtx_CONST_INT (VOIDmode, reg_size_val);
  emit_move_insn (target, reg_size);

  return target;
}

static rtx
k1_expand_builtin_wfxl (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  if (!verify_const_int_arg (arg1, 6, false))
    {
      error ("__builtin_k1_wfxl expects a 6 bits immediate first argument.");
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
  if (!verify_const_int_arg (arg1, 6, false))
    {
      error ("__builtin_k1_wfxm expects a 6 bits immediate first argument.");
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

/* FIXME AUTO: no more bwlu */
/* static rtx */
/* k1_expand_builtin_bwlu (rtx target, tree args) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */
/*     rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2)); */
/*     rtx arg4 = expand_normal (CALL_EXPR_ARG (args, 3)); */
/*     rtx arg5 = expand_normal (CALL_EXPR_ARG (args, 4)); */

/*     if (!target) */
/*         target = gen_reg_rtx (SImode); */
/*     target = force_reg (SImode, target); */
/*     arg1 = force_reg (SImode, arg1); */
/*     arg2 = force_reg (SImode, arg2); */
/*     arg3 = force_reg (SImode, arg3); */
/*     arg4 = force_reg (SImode, arg4); */

/*     if (!verify_const_int_arg (arg5, 16, false)) { */
/*         error ("__builtin_k1_bwlu expects a 16 bits immediate third
 * argument."); */
/*         return NULL_RTX; */
/*     } */

/*     emit_insn (gen_bwlu (target, arg1, arg2, arg3, arg4, arg5)); */

/*     return target; */
/* } */

/* FIXME AUTO: disabling vector support */
/* static rtx */
/* k1_expand_builtin_bwluhp (rtx target, tree args) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */
/*     rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2)); */

/*     if (!target) */
/*         target = gen_reg_rtx (SImode); */
/*     target = force_reg (SImode, target); */
/*     arg1 = force_reg (SImode, arg1); */
/*     arg2 = force_reg (SImode, arg2); */

/*     if (!verify_const_int_arg (arg3, 32, false)) { */
/*         error ("__builtin_k1_bwluhp expects a 32 bits immediate third
 * argument."); */
/*         return NULL_RTX; */
/*     } */

/*     emit_insn (gen_bwluhp (target, arg1, arg2, arg3)); */

/*     return target; */
/* } */

/* static rtx */
/* k1_expand_builtin_bwluwp (rtx target, tree args) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */
/*     rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2)); */

/*     if (!target) */
/*         target = gen_reg_rtx (DImode); */
/*     target = force_reg (DImode, target); */
/*     arg1 = force_reg (DImode, arg1); */
/*     arg2 = force_reg (DImode, arg2); */

/*     if (!verify_const_int_arg (arg3, 32, false)) { */
/*         error ("__builtin_k1_bwluwp expects a 32 bits immediate third
 * argument."); */
/*         return NULL_RTX; */
/*     } */

/*     emit_insn (gen_bwluwp (target, arg1, arg2, arg3)); */

/*     return target; */
/* } */

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

  arg1 = force_reg (SImode, arg1);
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

/* FIXME AUTO: sat does not exist in k1c yet */
/* static rtx */
/* k1_expand_builtin_sat (rtx target, tree args) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */

/*     if (!target) */
/*         target = gen_reg_rtx (SImode); */
/*     target = force_reg (SImode, target); */
/*     arg1 = force_reg (SImode, arg1); */
/*     arg2 = force_reg (SImode, arg2); */
/*     emit_insn (gen_sat (target, arg1, arg2)); */
/*     return target; */
/* } */

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

/* static rtx */
/* k1_expand_builtin_rxor (rtx target, tree args) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */
/*     rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2)); */

/*     if (!verify_const_int_arg (arg1, 6, false)) { */
/*         error ("__builtin_k1_r_xor expects a 6 bits immediate first
 * argument."); */
/*         return NULL_RTX; */
/*     } */

/*     if (!verify_const_int_arg (arg3, 10, false)) { */
/*         error ("__builtin_k1_r_xor expects a 10 bits immediate third
 * argument."); */
/*         return NULL_RTX; */
/*     } */

/*     arg2 = force_reg (DImode, arg2); */
/*     emit_insn (gen_remote_xord (arg1, arg2, arg3, k1_sync_reg_rtx)); */
/*     return target; */
/* } */

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
k1_expand_builtin_adds (rtx target, tree args)
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
k1_expand_builtin_itouchl (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  arg1 = gen_rtx_MEM (SImode, force_reg (Pmode, arg1));
  emit_insn (gen_itouchl (arg1, k1_sync_reg_rtx));

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

static rtx
k1_expand_builtin_faddrn (rtx target, tree args)
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
  emit_insn (gen_faddw_rn_s (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_faddrnd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  arg1 = force_reg (DFmode, arg1);
  arg2 = force_reg (DFmode, arg2);

  if (!target)
    target = gen_reg_rtx (DFmode);
  if (!REG_P (target) || GET_MODE (target) != DFmode)
    {
      target = force_reg (DFmode, target);
    }
  emit_insn (gen_faddd_rn_s (target, arg1, arg2));

  return target;
}

/* FIXME AUTO: disabling vector support */
/* static rtx */
/* k1_expand_builtin_faddwp (rtx target, tree args, bool rn) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */

/*     arg1 = force_reg (V2SFmode, arg1); */
/*     arg2 = force_reg (V2SFmode, arg2); */

/*     if (!target) */
/*         target = gen_reg_rtx (V2SFmode); */
/*     if (!REG_P(target) || GET_MODE (target) != V2SFmode) { */
/*         target = force_reg (V2SFmode, target); */
/*     } */
/*     if (rn) */
/*         emit_insn (gen_faddrnwp (target, arg1, arg2)); */
/*     else */
/*         emit_insn (gen_addv2sf3 (target, arg1, arg2)); */

/*     return target; */
/* } */

static rtx
k1_expand_builtin_fsbfrn (rtx target, tree args)
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
  emit_insn (gen_fsbfw_rn_s (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_fsbfrnd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  arg1 = force_reg (DFmode, arg1);
  arg2 = force_reg (DFmode, arg2);

  if (!target)
    target = gen_reg_rtx (DFmode);
  if (!REG_P (target) || GET_MODE (target) != DFmode)
    {
      target = force_reg (DFmode, target);
    }
  emit_insn (gen_fsbfd_rn_s (target, arg1, arg2));

  return target;
}

/* FIXME AUTO: disabling vector support */
/* static rtx */
/* k1_expand_builtin_fsbfwp (rtx target, tree args, bool rn) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */

/*     arg1 = force_reg (V2SFmode, arg1); */
/*     arg2 = force_reg (V2SFmode, arg2); */

/*     if (!target) */
/*         target = gen_reg_rtx (V2SFmode); */
/*     if (!REG_P(target) || GET_MODE (target) != V2SFmode) { */
/*         target = force_reg (V2SFmode, target); */
/*     } */
/*     if (rn) */
/*         emit_insn (gen_fsbfrnwp (target, arg1, arg2)); */
/*     else */
/*         emit_insn (gen_fsbfwp (target, arg1, arg2)); */

/*     return target; */
/* } */

static rtx
k1_expand_builtin_fmulrn (rtx target, tree args)
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
  emit_insn (gen_fmulrn (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_fmulrnd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  arg1 = force_reg (DFmode, arg1);
  arg2 = force_reg (DFmode, arg2);

  if (!target)
    target = gen_reg_rtx (DFmode);
  if (!REG_P (target) || GET_MODE (target) != DFmode)
    {
      target = force_reg (DFmode, target);
    }
  emit_insn (gen_fmulrnd (target, arg1, arg2));

  return target;
}

/* FIXME AUTO: disabling vector support */
/* static rtx */
/* k1_expand_builtin_fmulwp (rtx target, tree args, bool rn) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */

/*     arg1 = force_reg (V2SFmode, arg1); */
/*     arg2 = force_reg (V2SFmode, arg2); */

/*     if (!target) */
/*         target = gen_reg_rtx (V2SFmode); */
/*     if (!REG_P(target) || GET_MODE (target) != V2SFmode) { */
/*         target = force_reg (V2SFmode, target); */
/*     } */
/*     if (rn) */
/*         emit_insn (gen_fmulrnwp (target, arg1, arg2)); */
/*     else */
/*         emit_insn (gen_mulv2sf3 (target, arg1, arg2)); */

/*     return target; */
/* } */

static rtx
k1_expand_builtin_fmulwd (rtx target, tree args, bool rn)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  arg1 = force_reg (SFmode, arg1);
  arg2 = force_reg (SFmode, arg2);

  if (!target)
    target = gen_reg_rtx (DFmode);
  if (!REG_P (target) || GET_MODE (target) != DFmode)
    {
      target = force_reg (DFmode, target);
    }
  if (rn)
    emit_insn (gen_fmulrnwd (target, arg1, arg2));
  else
    emit_insn (gen_fmulwd (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_float (rtx target, tree args, bool usigned)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));

  if (!verify_const_int_arg (arg1, 2, false))
    {
      error ("__builtin_k1_float%s expects an unsigned 2bits immediate first "
	     "argument.",
	     usigned ? "u" : "");
      return NULL_RTX;
    }

  if (!verify_const_int_arg (arg3, 6, false))
    {
      error ("__builtin_k1_float%s expects an unsigned 6 bits immediate third "
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
      error ("__builtin_k1_float%sd expects an unsigned 2bits immediate first "
	     "argument.",
	     usigned ? "u" : "");
      return NULL_RTX;
    }

  if (!verify_const_int_arg (arg3, 6, false))
    {
      error ("__builtin_k1_float%sd expects an unsigned 6 bits immediate third "
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
      error ("__builtin_k1_fixed%s expects an unsigned 2bits immediate first "
	     "argument.",
	     usigned ? "u" : "");
      return NULL_RTX;
    }

  if (!verify_const_int_arg (arg3, 6, false))
    {
      error ("__builtin_k1_fixed%s expects an unsigned 6 bits immediate third "
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
      error ("__builtin_k1_fixed%sd expects an unsigned 2bits immediate first "
	     "argument.",
	     usigned ? "u" : "");
      return NULL_RTX;
    }

  if (!verify_const_int_arg (arg3, 6, false))
    {
      error ("__builtin_k1_fixed%sd expects an unsigned 6 bits immediate third "
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
k1_expand_builtin_fsdiv (rtx target, tree args)
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
  emit_insn (gen_fsdiv (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_fsinv (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  arg1 = force_reg (SFmode, arg1);

  if (!target)
    target = gen_reg_rtx (SFmode);
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (SFmode, target);
    }
  emit_insn (gen_fsinv (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_fsinvn (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  arg1 = force_reg (SFmode, arg1);

  if (!target)
    target = gen_reg_rtx (SFmode);
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (SFmode, target);
    }
  emit_insn (gen_fsinvn (target, arg1));

  return target;
}

static rtx
k1_expand_builtin_fcdiv (rtx target, tree args)
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
  emit_insn (gen_fcdiv (target, arg1, arg2));

  return target;
}

static rtx
k1_expand_builtin_fsisr (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  arg1 = force_reg (SFmode, arg1);

  if (!target)
    target = gen_reg_rtx (SFmode);
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (SFmode, target);
    }
  emit_insn (gen_fsisr (target, arg1));

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
k1_expand_builtin_fsinvnd (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));

  arg1 = force_reg (DFmode, arg1);

  if (!target)
    target = gen_reg_rtx (DFmode);
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (DFmode, target);
    }
  emit_insn (gen_fsinvnd (target, arg1));

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

/* FIXME AUTO : Disable use of create_SC_* */
/* static rtx */
/* k1_expand_builtin_complex_product (rtx target, tree args, int builtin) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */
/*     rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2)); */
/*     rtx arg4 = expand_normal (CALL_EXPR_ARG (args, 3)); */

/*     rtx sc1, sc2; */
/*     rtx last = get_last_insn_anywhere (); */

/*     arg1 = force_reg (SFmode, arg1); */
/*     arg2 = force_reg (SFmode, arg2); */
/*     arg3 = force_reg (SFmode, arg3); */
/*     arg4 = force_reg (SFmode, arg4); */

/*     sc1 = force_reg (SCmode, k1_find_or_create_SC_register (last, arg1,
 * arg2)); */
/*     sc2 = force_reg (SCmode, k1_find_or_create_SC_register (last, arg3,
 * arg4)); */

/*     if (!target) */
/*         target = gen_reg_rtx (SFmode); */
/*     if (!REG_P(target) || GET_MODE (target) != SFmode) { */
/*         target = force_reg (SFmode, target); */
/*     } */

/*     switch (builtin) { */
/*     case K1_BUILTIN_FDMA: */
/*         emit_insn (gen_fdma (target, sc1, sc2)); */
/*         break; */
/*     case K1_BUILTIN_FDMS: */
/*         emit_insn (gen_fdms (target, sc1, sc2)); */
/*         break; */
/*     case K1_BUILTIN_FCMA: */
/*         emit_insn (gen_fcma (target, sc1, sc2)); */
/*         break; */
/*     case K1_BUILTIN_FCMS: */
/*         emit_insn (gen_fcms (target, sc1, sc2)); */
/*         break; */
/*     default: */
/*         gcc_unreachable (); */
/*     } */

/*     return target; */
/* } */

/* FIXME AUTO : Disable use of create_SC_* */
/* static rtx */
/* k1_expand_builtin_double_complex_product (rtx target, tree args, int builtin)
 */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */
/*     rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2)); */
/*     rtx arg4 = expand_normal (CALL_EXPR_ARG (args, 3)); */

/*     rtx sc1, sc2; */
/*     rtx last = get_last_insn_anywhere (); */

/*     arg1 = force_reg (SFmode, arg1); */
/*     arg2 = force_reg (SFmode, arg2); */
/*     arg3 = force_reg (SFmode, arg3); */
/*     arg4 = force_reg (SFmode, arg4); */

/*     sc1 = force_reg (SCmode, k1_find_or_create_SC_register (last, arg1,
 * arg2)); */
/*     sc2 = force_reg (SCmode, k1_find_or_create_SC_register (last, arg3,
 * arg4)); */

/*     if (!target) */
/*         target = gen_reg_rtx (DFmode); */
/*     if (!REG_P(target) || GET_MODE (target) != DFmode) { */
/*         target = force_reg (DFmode, target); */
/*     } */

/*     switch (builtin) { */
/*     case K1_BUILTIN_FDMAWD: */
/*         emit_insn (gen_fdmawd (target, sc1, sc2)); */
/*         break; */
/*     case K1_BUILTIN_FDMSWD: */
/*         emit_insn (gen_fdmswd (target, sc1, sc2)); */
/*         break; */
/*     case K1_BUILTIN_FCMAWD: */
/*         emit_insn (gen_fcmawd (target, sc1, sc2)); */
/*         break; */
/*     case K1_BUILTIN_FCMSWD: */
/*         emit_insn (gen_fcmswd (target, sc1, sc2)); */
/*         break; */
/*     default: */
/*         gcc_unreachable (); */
/*     } */

/*     return target; */
/* } */

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
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
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
  if (!REG_P (target) || GET_MODE (target) != DFmode)
    {
      target = force_reg (DFmode, target);
    }
  if (rn)
    emit_insn (gen_ffmadrn (target, arg2, arg3, arg1));
  else
    emit_insn (gen_fmadf4 (target, arg2, arg3, arg1));

  return target;
}

/* FIXME AUTO: disabling vector support */
/* static rtx */
/* k1_expand_builtin_ffmawp (rtx target, tree args, int rn) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */
/*     rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2)); */

/*     arg1 = force_reg (V2SFmode, arg1); */
/*     arg2 = force_reg (V2SFmode, arg2); */
/*     arg3 = force_reg (V2SFmode, arg3); */

/*     if (!target) */
/*         target = gen_reg_rtx (V2SFmode); */
/*     if (!REG_P(target) || GET_MODE (target) != V2SFmode) { */
/*         target = force_reg (V2SFmode, target); */
/*     } */
/*     if (rn) */
/*         emit_insn (gen_ffmarnwp (target, arg2, arg3, arg1)); */
/*     else */
/*         emit_insn (gen_fmav2sf4 (target, arg2, arg3, arg1)); */

/*     return target; */
/* } */

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
  if (!REG_P (target) || GET_MODE (target) != DFmode)
    {
      target = force_reg (DFmode, target);
    }
  if (rn)
    emit_insn (gen_ffmswdrn (target, arg1, arg2, arg3));
  else
    emit_insn (gen_ffmswd (target, arg1, arg2, arg3));

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
  if (!REG_P (target) || GET_MODE (target) != DFmode)
    {
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
  if (!REG_P (target) || GET_MODE (target) != SFmode)
    {
      target = force_reg (SFmode, target);
    }

  if (rn)
    emit_insn (gen_ffmswrn (target, arg2, arg3, arg1));
  else
    emit_insn (gen_fnmasf4 (target, arg2, arg3, arg1));

  return target;
}

/* FIXME AUTO: disabling vector support */
/* static rtx */
/* k1_expand_builtin_ffmswp (rtx target, tree args, int rn) */
/* { */
/*     rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0)); */
/*     rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1)); */
/*     rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2)); */

/*     arg1 = force_reg (V2SFmode, arg1); */
/*     arg2 = force_reg (V2SFmode, arg2); */
/*     arg3 = force_reg (V2SFmode, arg3); */

/*     if (!target) */
/*         target = gen_reg_rtx (V2SFmode); */
/*     if (!REG_P(target) || GET_MODE (target) != V2SFmode) { */
/*         target = force_reg (V2SFmode, target); */
/*     } */

/*     if (rn) */
/*         emit_insn (gen_ffmsrnwp (target, arg2, arg3, arg1)); */
/*     else */
/*         emit_insn (gen_fnmav2sf4 (target, arg2, arg3, arg1)); */

/*     return target; */
/* } */

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
  if (!REG_P (target) || GET_MODE (target) != DFmode)
    {
      target = force_reg (DFmode, target);
    }

  if (rn)
    emit_insn (gen_ffmsdrn (target, arg2, arg3, arg1));
  else
    emit_insn (gen_fnmadf4 (target, arg2, arg3, arg1));

  return target;
}

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

  if (!verify_const_int_arg (arg2, 5, false))
    {
      error ("__builtin_k1_extfz expects a 5 bits immediate second argument.");
      return NULL_RTX;
    }

  if (!verify_const_int_arg (arg3, 5, false))
    {
      error ("__builtin_k1_extfz expects a 5 bits immediate third argument.");
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
k1_expand_builtin_stsu (rtx target, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));

  if (!target)
    target = gen_reg_rtx (SImode);
  target = force_reg (SImode, target);
  arg1 = force_reg (SImode, arg1);
  arg2 = force_reg (SImode, arg2);
  emit_insn (gen_stsu (target, arg1, arg2));

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
      /* FIXME AUTO: disabling vector support */
    /* case K1_BUILTIN_ABDHP: */
    /*     return k1_expand_builtin_abdhp (target, exp); */
    /* case K1_BUILTIN_ADDHP: */
    /*     return k1_expand_builtin_addhp (target, exp); */
    case K1_BUILTIN_ADDS:
      return k1_expand_builtin_adds (target, exp);
    case K1_BUILTIN_AWAIT:
      return k1_expand_builtin_await (target, exp);
    case K1_BUILTIN_BARRIER:
      return k1_expand_builtin_barrier ();

      /* FIXME AUTO: no more bwlu */
      /* case K1_BUILTIN_BWLU: */
      /*     return k1_expand_builtin_bwlu (target, exp); */

      /* FIXME AUTO: disabling vector support */
      /* case K1_BUILTIN_BWLUHP: */
      /*     return k1_expand_builtin_bwluhp (target, exp); */

      /* FIXME AUTO: disabling vector support */
    /* case K1_BUILTIN_BWLUWP: */
    /*     return k1_expand_builtin_bwluwp (target, exp); */
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
    case K1_BUILTIN_CBS:
    case K1_BUILTIN_CBSW:
      return k1_expand_builtin_cbsw (target, exp);
    case K1_BUILTIN_CBSD:
    case K1_BUILTIN_CBSDL:
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
    case K1_BUILTIN_FADDRN:
      return k1_expand_builtin_faddrn (target, exp);
    case K1_BUILTIN_FADDRND:
      return k1_expand_builtin_faddrnd (target, exp);

      /* FIXME AUTO: disabling vector support */
      /* case K1_BUILTIN_FADDWP: */
      /* case K1_BUILTIN_FADDRNWP: */
      /*     return k1_expand_builtin_faddwp (target, exp, fcode ==
       * K1_BUILTIN_FADDRNWP); */

    case K1_BUILTIN_FSBFRN:
      return k1_expand_builtin_fsbfrn (target, exp);
    case K1_BUILTIN_FSBFRND:
      return k1_expand_builtin_fsbfrnd (target, exp);

      /* FIXME AUTO: disabling vector support */
    /* case K1_BUILTIN_FSBFWP: */
    /* case K1_BUILTIN_FSBFRNWP: */
    /*     return k1_expand_builtin_fsbfwp (target, exp, fcode ==
     * K1_BUILTIN_FSBFRNWP); */

    /* FIXME AUTO : Disable use of create_SC_* */
    /* case K1_BUILTIN_FDMA: */
    /* case K1_BUILTIN_FDMS: */
    /* case K1_BUILTIN_FCMA: */
    /* case K1_BUILTIN_FCMS: */
    /*     return k1_expand_builtin_complex_product (target, exp, fcode); */
    /* case K1_BUILTIN_FDMAWD: */
    /* case K1_BUILTIN_FDMSWD: */
    /* case K1_BUILTIN_FCMAWD: */
    /* case K1_BUILTIN_FCMSWD: */
    /*     return k1_expand_builtin_double_complex_product (target, exp, fcode);
     */
    case K1_BUILTIN_FENCE:
      return k1_expand_builtin_fence ();
    case K1_BUILTIN_FFMAW:
    case K1_BUILTIN_FFMAWRN:
      return k1_expand_builtin_ffmaw (target, exp, fcode == K1_BUILTIN_FFMAWRN);
    case K1_BUILTIN_FFMAD:
    case K1_BUILTIN_FFMADRN:
      return k1_expand_builtin_ffmad (target, exp, fcode == K1_BUILTIN_FFMADRN);

      /* FIXME AUTO: disabling vector support */
      /* case K1_BUILTIN_FFMAWP: */
      /* case K1_BUILTIN_FFMARNWP: */
      /*     return k1_expand_builtin_ffmawp (target, exp, fcode ==
       * K1_BUILTIN_FFMARNWP); */

    case K1_BUILTIN_FFMAWD:
    case K1_BUILTIN_FFMAWDRN:
      return k1_expand_builtin_ffmawd (target, exp,
				       fcode == K1_BUILTIN_FFMAWDRN);
    case K1_BUILTIN_FFMSW:
    case K1_BUILTIN_FFMSWRN:
      return k1_expand_builtin_ffmsw (target, exp, fcode == K1_BUILTIN_FFMSWRN);
      /* FIXME AUTO: disabling vector support */
    /* case K1_BUILTIN_FFMSWP: */
    /* case K1_BUILTIN_FFMSRNWP: */
    /*     return k1_expand_builtin_ffmswp (target, exp, fcode ==
     * K1_BUILTIN_FFMSRNWP); */
    case K1_BUILTIN_FFMSD:
    case K1_BUILTIN_FFMSDRN:
      return k1_expand_builtin_ffmsd (target, exp, fcode == K1_BUILTIN_FFMSDRN);

    case K1_BUILTIN_FFMSWD:
    case K1_BUILTIN_FFMSWDRN:
      return k1_expand_builtin_ffmswd (target, exp,
				       fcode == K1_BUILTIN_FFMSWDRN);

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
    case K1_BUILTIN_FMULRN:
      return k1_expand_builtin_fmulrn (target, exp);
    case K1_BUILTIN_FMULRND:
      return k1_expand_builtin_fmulrnd (target, exp);

      /* FIXME AUTO: disabling vector support */
      /* case K1_BUILTIN_FMULWP: */
      /* case K1_BUILTIN_FMULRNWP: */
      /*     return k1_expand_builtin_fmulwp (target, exp, fcode ==
       * K1_BUILTIN_FMULRNWP); */

    case K1_BUILTIN_FMULWD:
    case K1_BUILTIN_FMULRNWD:
      return k1_expand_builtin_fmulwd (target, exp,
				       fcode == K1_BUILTIN_FMULRNWD);

    case K1_BUILTIN_FCDIV:
      return k1_expand_builtin_fcdiv (target, exp);
    case K1_BUILTIN_FSDIV:
      return k1_expand_builtin_fsdiv (target, exp);
    case K1_BUILTIN_FSINV:
      return k1_expand_builtin_fsinv (target, exp);
    case K1_BUILTIN_FSINVN:
      return k1_expand_builtin_fsinvn (target, exp);
    case K1_BUILTIN_FSISR:
      return k1_expand_builtin_fsisr (target, exp);
    case K1_BUILTIN_FCDIVD:
      return k1_expand_builtin_fcdivd (target, exp);
    case K1_BUILTIN_FSDIVD:
      return k1_expand_builtin_fsdivd (target, exp);
    case K1_BUILTIN_FSINVD:
      return k1_expand_builtin_fsinvd (target, exp);
    case K1_BUILTIN_FSINVND:
      return k1_expand_builtin_fsinvnd (target, exp);
    case K1_BUILTIN_FSISRD:
      return k1_expand_builtin_fsisrd (target, exp);
    case K1_BUILTIN_GET:
      return k1_expand_builtin_get (target, exp);
    case K1_BUILTIN_GET_R:
      return k1_expand_builtin_get_r (target, exp);
    case K1_BUILTIN_WFXL:
      return k1_expand_builtin_wfxl (target, exp);
    case K1_BUILTIN_WFXM:
      return k1_expand_builtin_wfxm (target, exp);
    case K1_BUILTIN_IINVAL:
      return k1_expand_builtin_iinval ();
    case K1_BUILTIN_IINVALS:
      return k1_expand_builtin_iinvals (target, exp);
    case K1_BUILTIN_ITOUCHL:
      return k1_expand_builtin_itouchl (target, exp);
      /* FIXME AUTO: disabling vector support */
    /* case K1_BUILTIN_LANDHP: */
    /*     return k1_expand_builtin_landhp (target, exp); */
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
      /* case K1_BUILTIN_RXOR: */
      /*     return k1_expand_builtin_rxor (target, exp); */

      /* FIXME AUTO: sat does not exist in k1c yet  */
    /* case K1_BUILTIN_SAT: */
    /*     return k1_expand_builtin_sat (target, exp); */
    case K1_BUILTIN_SATD:
      return k1_expand_builtin_satd (target, exp);
      /* FIXME AUTO: disabling vector support */
    /* case K1_BUILTIN_SBFHP: */
    /*     return k1_expand_builtin_sbfhp (target, exp); */
    case K1_BUILTIN_SBMM8:
      return k1_expand_builtin_sbmm8 (target, exp);
    case K1_BUILTIN_SBMMT8:
      return k1_expand_builtin_sbmmt8 (target, exp);
    case K1_BUILTIN_SET:
    case K1_BUILTIN_SET_PS:
      return k1_expand_builtin_set (target, exp, fcode == K1_BUILTIN_SET_PS);
      /* FIXME AUTO: disabling vector support */
    /* case K1_BUILTIN_SLLHPS: */
    /* case K1_BUILTIN_SLLHPS_R: */
    /*     return k1_expand_builtin_sllhps (target, exp); */
    /* case K1_BUILTIN_SRAHPS: */
    /* case K1_BUILTIN_SRAHPS_R: */
    /*     return k1_expand_builtin_srahps (target, exp); */
    case K1_BUILTIN_SLEEP:
      return k1_expand_builtin_sleep (target, exp);
    case K1_BUILTIN_STOP:
      return k1_expand_builtin_stop (target, exp);
    case K1_BUILTIN_STSU:
      return k1_expand_builtin_stsu (target, exp);
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

      /* FIXME AUTO: disabling vector support */
      /* case K1_BUILTIN_FWIDENLHWP: */
      /*   return k1_expand_builtin_fwiden_wp (target, exp, 1); */

    case K1_BUILTIN_FWIDENMHW:
      return k1_expand_builtin_fwiden (target, exp, 0);

      /* FIXME AUTO: disabling vector support */
      /* case K1_BUILTIN_FWIDENMHWP: */
      /*   return k1_expand_builtin_fwiden_wp (target, exp, 0); */

    case K1_BUILTIN_FNARROWWH:
      return k1_expand_builtin_fnarrowwh (target, exp);

      /* FIXME AUTO: disabling vector support */
      /* case K1_BUILTIN_FNARROWDWP: */
      /*   return k1_expand_builtin_fnarrowdwp (target, exp); */

    case K1_BUILTIN_WAITIT:
      return k1_expand_builtin_waitit (target, exp);

    case K1_BUILTIN_SRFSIZE:
      return k1_expand_builtin_srfsize (target, exp);

    default:
      internal_error ("bad builtin code");
      break;
    }
}

int
k1_mau_lsu_double_port_bypass_p (rtx producer, rtx consumer)
{
  rtx produced = SET_DEST (single_set (producer));
  rtx consumed = PATTERN (consumer);

  if (GET_CODE (consumed) == PARALLEL)
    consumed = XVECEXP (consumed, 0, 0);
  consumed = SET_DEST (consumed);

  return reg_overlap_mentioned_p (produced, consumed);
}

static int
k1_target_sched_adjust_cost (rtx insn, rtx link, rtx dep_insn, int cost)
{
  enum reg_note kind = REG_NOTE_KIND (link);
  enum attr_class insn_class = get_attr_class (insn);
  /* On the k1, it is possible to read then write the same register in a bundle
   * so we set the WAR cost to 0 unless insn is a control-flow consuming reg
   * then it is 1 */
  if (kind == REG_DEP_ANTI)
    {
      cost = (insn_class == CLASS_BRANCH) | (insn_class == CLASS_JUMP)
	     | (insn_class == CLASS_LINK);
    }
  else
    /* Just to be sure, force the WAW cost to 1 */
    if (kind == REG_DEP_OUTPUT)
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
			       int verbose ATTRIBUTE_UNUSED, rtx insn,
			       int last_clock, int clock,
			       int *sort_p ATTRIBUTE_UNUSED)
{
  if (clock != last_clock)
    return 0;

  if (reg_mentioned_p (k1_sync_reg_rtx, insn))
    return 1;

  return 0;
}

struct replace_reg_data
{
  rtx old;
  rtx replacement;
  rtx asm_op; /* See k1_replace_reg_rtx comment */
};

static int
replace_reg (rtx *x, void *data)
{
  struct replace_reg_data *replace_data = (struct replace_reg_data *) data;
  rtx old = replace_data->old;
  rtx replace = replace_data->replacement;

  if (!*x)
    return -1;

  if (GET_CODE (*x) == SUBREG && rtx_equal_p (SUBREG_REG (*x), old))
    {
      rtx subreg = simplify_subreg (GET_MODE (*x), copy_rtx (replace),
				    GET_MODE (replace), SUBREG_BYTE (*x));
      if (subreg == NULL_RTX
	  && GET_MODE_SIZE (GET_MODE (*x)) > GET_MODE_SIZE (GET_MODE (replace)))
	{
	  /* Paradoxical subreg that couldn't be simplified. As the
	     upper bits are undefined, we can as well mark them as
	     0. */
	  subreg = gen_rtx_ZERO_EXTEND (DImode, copy_rtx (replace));
	}
      *x = subreg;
      return -1;
    }
  else if (rtx_equal_p (*x, old))
    {
      *x = copy_rtx (replace);
      return -1;
    }
  else if (GET_CODE (*x) == ASM_OPERANDS && replace_data->asm_op)
    {
      *x = replace_data->asm_op;
      return -1;
    }

  return 0;
}

/* Replace all occurences of pseudo with regno REGNO and rtx OLD
   by rtx REPLACE. Don't do the replacement in insns IGNORE1 and
   IGNORE2.

   Take special care of ASM_OPERANDS. Asm statements can be grouped
   together in PARALLEL rtxs when their asm_operands are the same. The
   asm_operands, need to be shared between all theit occurences in the
   parallel, otherwise check_asm_operands () fails. Thus we need to
   take specific measures to generate a conformant RTL statement after
   copy_rtx'ing the original stmt. */
static bool
k1_replace_reg_rtx (int regno, rtx old, rtx replace, rtx ignore1, rtx ignore2,
		    int remove_notes, int force_success)
{
  df_ref adef, next = NULL;
  rtx insn, tmp_insn;
  struct replace_reg_data data;
  bool replaced_everywhere = true;

  data.old = old;
  data.replacement = replace;

  for (adef = DF_REG_DEF_CHAIN (regno); adef; adef = next)
    {

      gcc_assert (DF_REF_CLASS (adef) != DF_REF_ARTIFICIAL);
      insn = DF_REF_INSN (adef);
      next = DF_REF_NEXT_REG (adef);

      if (insn == ignore1 || insn == ignore2)
	continue;

      tmp_insn = copy_rtx (insn);
      data.asm_op = NULL_RTX;
      if (asm_noperands (PATTERN (insn)) > 0)
	{
	  rtx asm_op = extract_asm_operands (PATTERN (tmp_insn));
	  for_each_rtx (&asm_op, replace_reg, &data);
	  data.asm_op = asm_op;
	}

      for_each_rtx (&PATTERN (tmp_insn), replace_reg, &data);
      if (!INSN_P (tmp_insn) || DEBUG_INSN_P (tmp_insn)
	  || GET_CODE (PATTERN (tmp_insn)) == USE
	  || GET_CODE (PATTERN (tmp_insn)) == CLOBBER
	  || recog (PATTERN (tmp_insn), tmp_insn, 0) >= 0
	  || check_asm_operands (PATTERN (tmp_insn)))
	{
	  data.asm_op = NULL_RTX;
	  for_each_rtx (&PATTERN (insn), replace_reg, &data);

	  if (REG_NOTES (insn))
	    {
	      if (remove_notes)
		{
		  rtx note = find_regno_note (insn, REG_DEAD, regno);
		  remove_note (insn, note);
		}
	      else
		{
		  for_each_rtx (&REG_NOTES (insn), replace_reg, &data);
		}
	    }
	  df_ref_remove (adef);
	}
      else
	{
	  if (force_success)
	    print_rtl_single (stdout, tmp_insn);
	  replaced_everywhere = false;
	}
    }
  for (adef = DF_REG_USE_CHAIN (regno); adef; adef = next)
    {

      gcc_assert (DF_REF_CLASS (adef) != DF_REF_ARTIFICIAL);
      insn = DF_REF_INSN (adef);
      next = DF_REF_NEXT_REG (adef);

      if (insn == ignore1 || insn == ignore2)
	continue;

      tmp_insn = copy_rtx (insn);
      data.asm_op = NULL_RTX;
      if (asm_noperands (PATTERN (insn)) > 0)
	{
	  rtx asm_op = extract_asm_operands (PATTERN (tmp_insn));
	  for_each_rtx (&asm_op, replace_reg, &data);
	  data.asm_op = asm_op;
	}

      for_each_rtx (&PATTERN (tmp_insn), replace_reg, &data);

      if (!INSN_P (tmp_insn) || DEBUG_INSN_P (tmp_insn)
	  || GET_CODE (PATTERN (tmp_insn)) == USE
	  || GET_CODE (PATTERN (tmp_insn)) == CLOBBER
	  || recog (PATTERN (tmp_insn), tmp_insn, 0) >= 0
	  || check_asm_operands (PATTERN (tmp_insn)))
	{
	  data.asm_op = NULL_RTX;
	  for_each_rtx (&PATTERN (insn), replace_reg, &data);

	  if (REG_NOTES (insn))
	    {
	      if (remove_notes)
		{
		  rtx note = find_regno_note (insn, REG_DEAD, regno);
		  remove_note (insn, note);
		}
	      else
		{
		  for_each_rtx (&REG_NOTES (insn), replace_reg, &data);
		}
	    }
	  df_ref_remove (adef);
	}
      else
	{
	  if (force_success)
	    print_rtl_single (stdout, tmp_insn);
	  replaced_everywhere = false;
	}
    }

  gcc_assert (replaced_everywhere || !force_success);
  return replaced_everywhere;
}

/* Remove INSN from all dependency lists, resolve its forward
   dependencies (and maybe mark its dependencies ready) and finally
   remove it from the insn flow.  */
static void
k1_sched_remove_insn (rtx insn)
{
  sd_iterator_def sd_it;
  dep_t dep;

  /* The skeleton of this loop is copied from
     haifa-sched.c:schedule_insn ().  */
  for (sd_it = sd_iterator_start (insn, SD_LIST_FORW);
       sd_iterator_cond (&sd_it, &dep);)
    {
      rtx next = DEP_CON (dep);
      bool cancelled = (DEP_STATUS (dep) & DEP_CANCELLED) != 0;

      /* Resolve the dependence between INSN and NEXT.
	 sd_resolve_dep () moves current dep to another list thus
	 advancing the iterator.  */
      sd_resolve_dep (sd_it);

      /* Don't bother trying to mark next as ready if
	 insn is a debug insn.  If insn is the last hard
	 dependency, it will have already been discounted.  */
      if (DEBUG_INSN_P (insn) && !DEBUG_INSN_P (next))
	continue;

      if (cancelled)
	continue;

      if (!IS_SPECULATION_BRANCHY_CHECK_P (insn))
	{
	  /* With EBB scheduling, you can get to dependencies which
	     can't be scheduled because in another BB. */
	  if (INSN_BB (next) == INSN_BB (insn))
	    {
	      if (HID (next)->todo_spec == 0)
		HID (next)->todo_spec = HARD_DEP;
	      try_ready (next);
	    }
	}
      else
	gcc_unreachable ();
    }

  sd_it = sd_iterator_start (insn, (SD_LIST_FORW | SD_LIST_RES_FORW
				    | SD_LIST_BACK | SD_LIST_RES_BACK));

  while (sd_iterator_cond (&sd_it, &dep))
    sd_delete_dep (sd_it);

  sd_finish_insn (insn);
  current_sched_info->add_remove_insn (insn, 1);
  remove_insn (insn);
  set_insn_deleted (insn);
}

/* Extend the reg_info_p array when creating a new pseudo with regno
   REGNO.  The new register is meant to replace OLD_REGNO1 and
   OLD_REGNO2, thus use their properties to feed the new reg_info
   entry.  */
static void
k1_add_reginfo (int regno, int old_regno1, int old_regno2)
{
  int old_max_regno = max_regno;
  int i;
  max_regno = max_reg_num ();

  /* Extend REG_INFO_P, if needed.  */
  if ((unsigned int) max_regno - 1 >= reg_info_p_size)
    {
      size_t new_reg_info_p_size = max_regno + 128;
      reg_info_p
	= (struct reg_info_t *) xrecalloc (reg_info_p, new_reg_info_p_size,
					   reg_info_p_size,
					   sizeof (*reg_info_p));
      reg_info_p_size = new_reg_info_p_size;
    }
  /* Extend sched_regno_pressure_class (was init in haifa-sched.c, sched_init())
   */
  sched_regno_pressure_class
    = (enum reg_class *) xrealloc (sched_regno_pressure_class,
				   max_regno * sizeof (enum reg_class));
  resize_reg_info ();

  for (i = old_max_regno; i < max_regno; i++)
    {
      setup_reg_classes (i, NO_REGS, NO_REGS, NO_REGS);
      /* will be set to actual values in a later pass, for now, we need to
       * replace all the "-1" in the reg_pref[regno] */

      sched_regno_pressure_class[i]
	= ira_pressure_class_translate[reg_allocno_class (i)];
    }

  REG_FREQ (regno) = MAX (REG_FREQ (old_regno1), REG_FREQ (old_regno2));
  REG_N_DEATHS (regno)
    = MAX (REG_N_DEATHS (old_regno1), REG_N_DEATHS (old_regno2));
  REG_N_DEATHS (regno)
    = MAX (REG_N_DEATHS (old_regno1), REG_N_DEATHS (old_regno2));
  REG_LIVE_LENGTH (regno)
    = MAX (REG_LIVE_LENGTH (old_regno1), REG_LIVE_LENGTH (old_regno2));
  REG_N_CALLS_CROSSED (regno)
    = MAX (REG_N_CALLS_CROSSED (old_regno1), REG_N_CALLS_CROSSED (old_regno2));
  REG_FREQ_CALLS_CROSSED (regno) = MAX (REG_FREQ_CALLS_CROSSED (old_regno1),
					REG_FREQ_CALLS_CROSSED (old_regno2));
  REG_N_THROWING_CALLS_CROSSED (regno)
    = MAX (REG_N_THROWING_CALLS_CROSSED (old_regno1),
	   REG_N_THROWING_CALLS_CROSSED (old_regno2));
  REG_BASIC_BLOCK (regno)
    = MAX (REG_BASIC_BLOCK (old_regno1), REG_BASIC_BLOCK (old_regno2));

  df_grow_reg_info ();
}

static void
add_post_packing_clobber_insertion (rtx reg, rtx insn)
{
  struct post_packing_action *action;

  action = (struct post_packing_action *) xmalloc (sizeof (*action));
  action->type = CLOBBER_INSERT;
  action->next = post_packing_action;
  action->reg = reg;
  action->insn = insn;
  post_packing_action = action;
}

static void
add_post_packing_insn_deletion (rtx insn, rtx x)
{
  struct post_packing_action *action;

  action = (struct post_packing_action *) xmalloc (sizeof (*action));
  action->type = INSN_DELETE;
  action->next = post_packing_action;
  action->insn = insn;
  action->reg = x;
  post_packing_action = action;
}

static void
add_post_packing_reg_copy_insertion (rtx reg, rtx reg_orig, rtx insn)
{
  struct post_packing_action *action;

  action = (struct post_packing_action *) xmalloc (sizeof (*action));
  action->type = REG_COPY_INSERT;
  action->next = post_packing_action;
  action->reg = reg;
  action->reg2 = reg_orig;
  action->insn = insn;
  post_packing_action = action;
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

static rtx
k1_patch_mem_for_double_access (rtx x)
{
  rtx y = shallow_copy_rtx (x);
  for_each_rtx (&y, k1_decr_reg_use_count, NULL);
  for_each_rtx (&y, &k1_patch_mem_for_double_access_1, NULL);
  for_each_rtx (&y, k1_incr_reg_use_count, NULL);
  return y;
}

/* Number of mem access packing that occured for the current
   function.  */
static int packed_mems;

/* Pack the passed memory accesses:
   - *_X parameters describe the lowpart of the mem reference.
   - *_Y parameters describe the highpart of the mem reference.

   Emit the packed instruction in CYCLE scheduling cycle. */
/* FIXME AUTO: Disable memory packing */
#if 0
static int
k1_pack_mems (enum machine_mode mode,
              enum rtx_code extension,
              rtx insn_x, rtx mem_x, int regno_x,
              rtx insn_y, rtx mem_y, int regno_y,
              int cycle)
{
    rtx load, insn, load_insn;
    int is_store = MEM_P (SET_DEST (single_set (insn_x)));
    int i;
    sd_iterator_def sd_it;
    dep_t dep;
    df_ref adef;

    mem_x = shallow_copy_rtx (mem_x);

    /* If we have hard regs, we already checked that they are in the
       right order.  Just use the DImode register with the lowest
       offset.  */
    if (regno_x < FIRST_PSEUDO_REGISTER) {
        load = gen_rtx_REG (DImode, regno_x);
    } else {
        load = gen_reg_rtx (DImode);
        k1_add_reginfo (REGNO (load), regno_x, regno_y);
    }

    /* We'll access the register only by subparts. Emit a
       clobber of the whole register so that the liveness
       analysis can deduce that it's live only from this point
       on. */
    if (is_store) {
        /* For a store we need to emit the clobber before every def of
           the subregs. */
        bitmap def_bbs = BITMAP_ALLOC (NULL);
        int min_tick = INT_MAX;
        rtx min_tick_insn = NULL_RTX;
        int tick;

        for (adef = DF_REG_DEF_CHAIN (regno_x);
             adef;
             adef = DF_REF_NEXT_REG (adef)) {

            bitmap_set_bit (def_bbs, DF_REF_BB (adef)->index);
            if (DF_REF_CLASS (adef) != DF_REF_ARTIFICIAL) {
                tick = HID (DF_REF_INSN (adef))->tick;
                if (tick >= 0 && tick < min_tick ) {
                    min_tick = HID (DF_REF_INSN (adef))->tick;
                    min_tick_insn = DF_REF_INSN (adef);
                }
            } else {
                return 0;
            }
        }

        for (adef = DF_REG_DEF_CHAIN (regno_y);
             adef;
             adef = DF_REF_NEXT_REG (adef)) {

            bitmap_set_bit (def_bbs, DF_REF_BB (adef)->index);
            if (DF_REF_CLASS (adef) != DF_REF_ARTIFICIAL) {
                tick = HID (DF_REF_INSN (adef))->tick;
                if (tick >= 0 && tick < min_tick) {
                    min_tick = HID (DF_REF_INSN (adef))->tick;
                    min_tick_insn = DF_REF_INSN (adef);
                }
            } else {
                return 0;
            }
        }

        if (bitmap_single_bit_set_p (def_bbs)) {
            if (min_tick_insn) {
                add_post_packing_clobber_insertion (load, min_tick_insn);
            } else {
                return 0;
            }
        } else {
            basic_block bb;
            calculate_dominance_info (CDI_DOMINATORS);
            bb = nearest_common_dominator_for_set (CDI_DOMINATORS,
                                                   def_bbs);

            if (bb && BB_END (bb)
                && HID(BB_END (bb))->tick >= 0) {
                if (bitmap_bit_p (def_bbs, bb->index)) {
                    add_post_packing_clobber_insertion (load, BB_HEAD (bb));
                } else {
                    add_post_packing_clobber_insertion (load, BB_END (bb));
                }
            } else {
                return 0;
            }
        }

    } else {
        /* For a load, we can emit the clobber right before the load. */
        rtx clobber = gen_rtx_CLOBBER (DImode, load);
        insn = sched_emit_insn (clobber);
        HID(insn)->tick = cycle;
        /* printf ("INSERTED3: %i(%i)%i\n", INSN_UID(PREV_INSN(insn)), INSN_UID(insn), INSN_UID(NEXT_INSN(insn))); */
    }

    /* If we're replacing pseudo register references, then replace all
       refs with the equivalent subreg that we generated.  */
    if (regno_x >= FIRST_PSEUDO_REGISTER) {
        rtx lowpart_rtx, highpart_rtx;
        rtx lowpart_replacement, highpart_replacement;

        if (REG_P (SET_DEST(single_set(insn_x)))) {
            lowpart_rtx = SET_DEST(single_set(insn_x));
            highpart_rtx = SET_DEST(single_set(insn_y));
        } else {
            lowpart_rtx = SET_SRC(single_set(insn_x));
            highpart_rtx = SET_SRC(single_set(insn_y));
        }

        if (GET_MODE_SIZE (GET_MODE (lowpart_rtx)) < UNITS_PER_WORD) {
            lowpart_replacement = gen_lowpart (SImode, load);
            PUT_MODE (lowpart_replacement, mode);
        } else {
            lowpart_replacement = gen_lowpart (GET_MODE (lowpart_rtx), load);
        }

        if (GET_MODE_SIZE (GET_MODE (highpart_rtx)) < UNITS_PER_WORD) {
            highpart_replacement = gen_highpart (SImode, load);
            PUT_MODE (highpart_replacement, mode);
        } else {
            highpart_replacement = gen_highpart (GET_MODE (highpart_rtx), load);
        }

        /* printf ("============ REPLACING ==================\n"); */
        /* print_rtl_single (stdout, insn_x); */
        /* print_rtl_single (stdout, mem_x); */
        /* print_rtl_single (stdout, lowpart_rtx); */
        /* print_rtl_single (stdout, lowpart_replacement); */
        /* printf ("------------------------------------------\n"); */
        /* print_rtl_single (stdout, insn_y); */
        /* print_rtl_single (stdout, mem_y); */
        /* print_rtl_single (stdout, highpart_rtx); */
        /* print_rtl_single (stdout, highpart_replacement); */
        /* printf ("==========================================\n"); */

        /* The memory access might contain references to the changed
           pseudos.  */
        replace_rtx (mem_x, lowpart_rtx,
                     copy_rtx (lowpart_replacement));
        replace_rtx (mem_x, highpart_rtx,
                     copy_rtx (highpart_replacement));

        /* Only update the second part if the pseudo to be stored
           is different. Otherwise, emit a copy to generate a correct
           double register for the access.
           Do this before the global replacement, as we need the
           DEF information that is destroyed by k1_replace_reg_rtx
        */
        if (regno_y != regno_x) {
            k1_replace_reg_rtx (regno_y, highpart_rtx,
                                copy_rtx (highpart_replacement),
                                insn_x, insn_y, false, true);
            replace_rtx (mem_y, lowpart_rtx,
                         copy_rtx (lowpart_replacement));
            replace_rtx (mem_y, highpart_rtx,
                         copy_rtx (highpart_replacement));
        } else {
            gcc_assert (is_store);

            /* Emit the definition as far up as we can (typically,
               this prevents us from rematerializing half of a constant
               right in the middle of a loop.

               For now only do this if there's only one def. Multiple
               defs include tricky things like infered zeros from loop
               counters and this won't work very well. */
            adef = DF_REG_DEF_CHAIN (regno_x);
            if (adef && !DF_REF_NEXT_REG (adef)) {
                add_post_packing_reg_copy_insertion (copy_rtx (highpart_replacement),
                                                     copy_rtx (lowpart_replacement),
                                                     DF_REF_INSN(adef));
            } else {
                insn = sched_emit_insn (k1_gen_reg_copy (copy_rtx (highpart_replacement),
                                                         copy_rtx (lowpart_replacement)));
                HID(insn)->tick = cycle;
                recog_memoized (insn);
            }
        }

        /* Replace all references to the lowpart.  */
        k1_replace_reg_rtx (regno_x, lowpart_rtx,
                            copy_rtx (lowpart_replacement),
                            insn_x, insn_y, false, true);
    }

    mem_y = k1_patch_mem_for_double_access (mem_x);

    {
      /* FIXME AUTO: disabling vector support */
	/* enum machine_mode memmode = mode == HImode ? V2HImode : DImode; */
	enum machine_mode memmode = DImode;
	int subreg_offset = GET_MODE_SIZE (memmode) / 2;
	enum machine_mode x_mode = GET_MODE (mem_x);
	enum machine_mode y_mode = GET_MODE (mem_y);

	PUT_MODE (mem_x, memmode);
	if (memmode == DImode) {
	    mem_y = gen_rtx_SUBREG (y_mode, mem_x, subreg_offset);
	    mem_x = gen_rtx_SUBREG (x_mode, mem_x, 0);
	} else {
	    static rtx elem0 = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc (1));
	    static rtx elem1 = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc (1));
	    XVECEXP (elem0, 0, 0) = const0_rtx;
	    XVECEXP (elem1, 0, 0) = const1_rtx;

	    mem_y = gen_rtx_VEC_SELECT (y_mode, mem_x, elem1);
	    mem_x = gen_rtx_VEC_SELECT (x_mode, mem_x, elem0);
	}
	if (extension == SIGN_EXTEND) {
	    mem_x = gen_rtx_SIGN_EXTEND (SImode, mem_x);
	    mem_y = gen_rtx_SIGN_EXTEND (SImode, mem_y);
	    x_mode = y_mode = SImode;
	} else if (extension == ZERO_EXTEND) {
	    mem_x = gen_rtx_ZERO_EXTEND (SImode, mem_x);
	    mem_y = gen_rtx_ZERO_EXTEND (SImode, mem_y);
	    x_mode = y_mode = SImode;
	}
    /* Use gen_rtx_SUBREG because our double_load instruction
       requires a SUBREG. gen_{low,high}part will give us a
       simple REG when applied on a hard register. */
    if (is_store)
        load_insn = gen_double_store (mem_x,
                                      gen_rtx_SUBREG (x_mode, load, 0),
                                      mem_y,
                                      gen_rtx_SUBREG (y_mode, load, 4));
    else
        load_insn = gen_double_load (gen_rtx_SUBREG (x_mode, load, 0),
                                     mem_x,
                                     gen_rtx_SUBREG (y_mode, load, 4),
                                     mem_y);
    }
    insn = sched_emit_insn (load_insn);
    INSN_LOCATION (insn) = INSN_LOCATION (insn_x);
    HID(insn)->tick = cycle;
    recog_memoized (insn);
    /* printf ("INSERTED: %i(%i)%i\n", INSN_UID(PREV_INSN(load_insn)), INSN_UID(load_insn), INSN_UID(NEXT_INSN(load_insn))); */
    /* Report the back dependencies of the replaced instructions to
       our new instruction.  */
    for (i = 0; i < 2; ++i) {
        rtx old = i == 0 ? insn_x : insn_y;
        for (sd_it = sd_iterator_start (old, SD_LIST_BACK);
             sd_iterator_cond (&sd_it, &dep);
             sd_iterator_next (&sd_it)) {
            if (DEP_PRO (dep) != old) {
                /* This might happen with some debug_insns... Don't know
                   why though.  */
                continue;
            } else {
                DEP_PRO (dep) = insn;
            }

            sd_add_dep (dep, false);
        }
    }

    packed_mems++;
    return 1;
}
#endif /* DISABLE MEMORY PACKING */

static rtx
k1_diff_addr_offset_addr_offset (enum machine_mode mode,
				 struct k1_address *addr1,
				 struct k1_address *addr2, rtx *insn_to_remove)
{
  /* Look for such cases:

     add $r1 = $r3, 4
     add $r2 = $r3, 8
     lw.x4 $r6 = symbol[$r1]
     lw.x4 $r7 = symbol[$r2]

     which can be replaced by:

     add $r1 = $r3, 4
     ld $p6 = symbol[$r1] */
  rtx insn1 = NULL_RTX, insn2 = NULL_RTX, x, def_rtx, diff;
  df_ref ref;

  if (addr1->offset != addr2->offset)
    return NULL_RTX;

  if (!REG_P (addr1->base_reg) || !REG_P (addr2->base_reg))
    return NULL_RTX;

  diff = gen_rtx_MINUS (Pmode, addr2->base_reg, addr1->base_reg);

  for (x = addr1->base_reg; x != NULL;
       x = (x == addr1->base_reg) ? addr2->base_reg : NULL)
    {
      if (DF_REG_DEF_COUNT (REGNO (x)) != 1)
	// Previously, this was not considered an error.
	// Simple case where r1 is defined outside a loop and r3/r2 is redefined
	// inside a loop: Mem access must not be packed.
	//
	// This failure should be made less restrictive as it
	// currently prevents most mem packing inside a loop
	return NULL_RTX;

      ref = DF_REG_DEF_CHAIN (REGNO (x));
      if (DF_REF_CLASS (ref) == DF_REF_ARTIFICIAL)
	continue;
      def_rtx = DF_REF_INSN (ref);

      if (DF_REG_USE_COUNT (REGNO (x)) == 1 && insn_to_remove)
	{
	  if (x == addr1->base_reg)
	    insn1 = def_rtx;
	  else
	    insn2 = def_rtx;
	}
      def_rtx = single_set (def_rtx);
      if (!def_rtx)
	return NULL_RTX;

      def_rtx = SET_SRC (def_rtx);
      diff = simplify_replace_rtx (diff, x, def_rtx);
    }

  if (diff && CONST_INT_P (diff) && abs (INTVAL (diff)) == GET_MODE_SIZE (mode))
    {
      if (INTVAL (diff) < 0)
	{
	  if (insn1)
	    *insn_to_remove = insn1;
	}
      else
	{
	  if (insn2)
	    *insn_to_remove = insn2;
	}
    }

  return diff;
}

static rtx
k1_diff_addr_mult_addr_mult (enum machine_mode mode ATTRIBUTE_UNUSED,
			     struct k1_address *addr1, struct k1_address *addr2,
			     rtx *insn_to_remove, rtx *insn_to_remove2)
{
  /* Look for such cases:

     add $r1 = $r3, 1
     lw.x4 $r6 = $r2[$r3]
     lw.x4 $r7 = $r2[$r1]

     which can be replaced by:

     ld $p6 = $r2[$r3] */
  rtx offset1, offset2, x, def_rtx, diff;
  df_ref ref;

  if (addr1->mult != addr2->mult)
    return NULL_RTX;

  if (addr1->mult != 1)
    {
      if (addr1->base_reg != addr2->base_reg)
	return NULL_RTX;
      offset1 = addr1->offset_reg;
      offset2 = addr2->offset_reg;
    }
  else
    {
      /* When the multiplier is one, the notion of base/offset
	 registers isn't so clear. If we find a common register,
	 then it's the base. */
      if (addr1->base_reg == addr2->base_reg)
	{
	  offset1 = addr1->offset_reg;
	  offset2 = addr2->offset_reg;
	}
      else if (addr1->base_reg == addr2->offset_reg)
	{
	  offset1 = addr1->offset_reg;
	  offset2 = addr2->base_reg;
	}
      else if (addr1->offset_reg == addr2->base_reg)
	{
	  offset1 = addr1->base_reg;
	  offset2 = addr2->offset_reg;
	}
      else if (addr1->offset_reg == addr2->offset_reg)
	{
	  offset1 = addr1->base_reg;
	  offset2 = addr2->base_reg;
	}
      else
	{
	  return NULL_RTX;
	}
    }

  if (!REG_P (offset1) || !REG_P (offset2))
    return NULL_RTX;

  diff = gen_rtx_MINUS (Pmode, offset2, offset1);

  for (x = offset1; x != NULL; x = (x == offset1) ? offset2 : NULL)
    {
      if (DF_REG_DEF_COUNT (REGNO (x)) != 1)
	// Previously, this was not considered an error.
	// Simple case where r1 is defined outside a loop and r3 is redefined
	// inside a loop: Mem access must not be packed.
	//
	// This failure should be made less restrictive as it
	// currently prevents most mem packing inside a loop
	return NULL_RTX;

      ref = DF_REG_DEF_CHAIN (REGNO (x));
      if (DF_REF_CLASS (ref) == DF_REF_ARTIFICIAL)
	continue;
      def_rtx = DF_REF_INSN (ref);
      if (DF_REG_USE_COUNT (REGNO (x)) == 1)
	{
	  if (insn_to_remove && *insn_to_remove == NULL_RTX)
	    *insn_to_remove = def_rtx;
	  else if (insn_to_remove2 && *insn_to_remove2 == NULL_RTX)
	    *insn_to_remove2 = def_rtx;
	}
      def_rtx = single_set (def_rtx);
      if (!def_rtx)
	return NULL_RTX;

      def_rtx = SET_SRC (def_rtx);
      diff = simplify_replace_rtx (diff, x, def_rtx);
    }

  if (diff && CONST_INT_P (diff))
    diff = simplify_rtx (gen_rtx_MULT (SImode, diff, GEN_INT (addr1->mult)));
  return diff;
}

static rtx
k1_diff_addr_offset_addr_mult (enum machine_mode mode ATTRIBUTE_UNUSED,
			       rtx mem1, rtx mem2, rtx mem_to_modify,
			       rtx reg_to_replace, rtx *insn_to_remove)
{
  /* Look for such cases:

     add $r1 = $r2, $r3
     lw.x1 $r4 = $r2[$r3]
     lw $r5 = 4[$r1]

     which can be replaced by:

     ld $p4 = $r2[$r3] */
  df_ref ref;
  rtx def_rtx;

  if (!REG_P (reg_to_replace))
    return NULL_RTX;

  if (DF_REG_DEF_COUNT (REGNO (reg_to_replace)) != 1)
    return NULL_RTX;

  ref = DF_REG_DEF_CHAIN (REGNO (reg_to_replace));
  if (DF_REF_CLASS (ref) == DF_REF_ARTIFICIAL)
    return NULL_RTX;
  def_rtx = DF_REF_INSN (ref);
  if (DF_REG_USE_COUNT (REGNO (reg_to_replace)) == 1 && insn_to_remove)
    *insn_to_remove = def_rtx;
  def_rtx = single_set (def_rtx);
  if (!def_rtx)
    return NULL_RTX;

  def_rtx = SET_SRC (def_rtx);
  replace_rtx (mem_to_modify, reg_to_replace, def_rtx);
  return simplify_rtx (gen_rtx_MINUS (Pmode, XEXP (mem2, 0), XEXP (mem1, 0)));
}

/* Tests if mem X loaded/stored to regno REGNO_X and mem Y
   loaded/stored to regno REGNO_Y are contiguous memory accesses.
   If packing these mem accesses allows to remove an intermediate
   instruction, store it to INSN_TO_REMOVE.
   Return an rtx containing the simplified difference between the
   addresses.  */
static rtx
k1_contiguous_mem_access_p (enum machine_mode mode ATTRIBUTE_UNUSED, rtx x,
			    int regno_x, rtx y, int regno_y,
			    rtx *insn_to_remove, rtx *insn_to_remove2)
{
  rtx diff;

  if (insn_to_remove)
    *insn_to_remove = NULL_RTX;
  if (insn_to_remove2)
    *insn_to_remove2 = NULL_RTX;

  if ((regno_x < FIRST_PSEUDO_REGISTER && regno_y >= FIRST_PSEUDO_REGISTER)
      || (regno_x >= FIRST_PSEUDO_REGISTER && regno_y < FIRST_PSEUDO_REGISTER))
    return NULL_RTX;

  if (regno_y < FIRST_PSEUDO_REGISTER && abs (regno_y - regno_x) != 1)
    return NULL_RTX;

  if (GET_MODE_SIZE (GET_MODE (y)) != GET_MODE_SIZE (mode))
    return NULL_RTX;

  if (GET_MODE_SIZE (GET_MODE (x)) != GET_MODE_SIZE (mode))
    return NULL_RTX;

  diff = simplify_rtx (gen_rtx_MINUS (Pmode, XEXP (y, 0), XEXP (x, 0)));

  if (diff == NULL_RTX || !CONST_INT_P (diff))
    {
      struct k1_address addr1, addr2;

      k1_analyze_address (XEXP (x, 0), false, &addr1);
      k1_analyze_address (XEXP (y, 0), false, &addr2);

      if (addr1.mode == ADDR_MULT && addr2.mode == ADDR_OFFSET)
	{
	  y = copy_rtx (y);
	  return k1_diff_addr_offset_addr_mult (mode, x, y, y, addr2.base_reg,
						insn_to_remove);
	}
      else if (addr1.mode == ADDR_OFFSET && addr2.mode == ADDR_MULT)
	{
	  x = copy_rtx (x);
	  return k1_diff_addr_offset_addr_mult (mode, x, y, x, addr2.base_reg,
						insn_to_remove);
	}
      else if (addr1.mode == ADDR_MULT && addr2.mode == ADDR_MULT)
	{
	  return k1_diff_addr_mult_addr_mult (mode, &addr1, &addr2,
					      insn_to_remove, insn_to_remove2);
	}
      else if (addr1.mode == ADDR_OFFSET && addr2.mode == ADDR_OFFSET)
	{
	  return k1_diff_addr_offset_addr_offset (mode, &addr1, &addr2,
						  insn_to_remove);
	}
      else
	{
	  return NULL_RTX;
	}
    }

  return diff;
}

enum pass
{
  PACK_LOADS,
  PACK_STORES
};

static int
k1_is_mem (rtx x, rtx *mem, enum rtx_code *extension)
{
  if (MEM_P (x))
    {
      *mem = x;
      *extension = UNKNOWN;
      return 1;
    }
  else if (GET_CODE (x) == SIGN_EXTEND && MEM_P (XEXP (x, 0)))
    {
      *mem = XEXP (x, 0);
      *extension = SIGN_EXTEND;
      return 1;
    }
  else if (GET_CODE (x) == ZERO_EXTEND && MEM_P (XEXP (x, 0)))
    {
      *mem = XEXP (x, 0);
      *extension = ZERO_EXTEND;
      return 1;
    }

  return 0;
}

/*
 * Tests wether the MEM[] contained in X is suitable for packing with
 * another mem access using MODE. MEM is set to the MEM[] rtl subexpr,
 * extension is set with the insn rtx code of X and REGNO is set with
 * the register being read/writen respectively by the load/store insn.
 * returns 1 if the X is a candidate for packing, 0 if not.
 */
static int
k1_interesting_mem_access (int pass, enum machine_mode mode, rtx x, rtx *mem,
			   enum rtx_code *extension, int *regno)
{
  if (pass == PACK_LOADS)
    {
      if (!x || !k1_is_mem (SET_SRC (x), mem, extension)
	  || MEM_VOLATILE_P (*mem) || !REG_P (SET_DEST (x)))
	return 0;
      *regno = REGNO (SET_DEST (x));
    }
  else
    {
      if (!x || !k1_is_mem (SET_DEST (x), mem, extension)
	  || MEM_VOLATILE_P (*mem) || !REG_P (SET_SRC (x)))
	return 0;
      *regno = REGNO (SET_SRC (x));
    }

  if (GET_MODE_SIZE (GET_MODE (*mem)) != GET_MODE_SIZE (mode))
    return 0;

  return 1;
}

static int
k1_lsu_load_p (rtx insn)
{
  switch (get_attr_type (insn))
    {
    case TYPE_LSU_ATOMIC:
    case TYPE_LSU_ATOMIC_X:
    case TYPE_LSU_LOAD:
    case TYPE_LSU_LOAD_X:
      return 1;
    default:
      return 0;
    }
}

static int
k1_lsu_store_p (rtx insn)
{
  switch (get_attr_type (insn))
    {
    case TYPE_LSU_STORE:
    case TYPE_LSU_STORE_X:
    case TYPE_LSU:
    case TYPE_LSU_X:
      return 1;
    default:
      return 0;
    }
}

static int
k1_lsu_p (rtx insn)
{
  switch (get_attr_type (insn))
    {
    case TYPE_LSU_ATOMIC:
    case TYPE_LSU_ATOMIC_X:
    case TYPE_LSU_LOAD:
    case TYPE_LSU_LOAD_X:
    case TYPE_LSU_STORE:
    case TYPE_LSU_STORE_X:
    case TYPE_LSU:
    case TYPE_LSU_X:
      return 1;
    default:
      return 0;
    }
}

/* Hijack the scheduler ready list reordering to pack memory access
   together.  */
static int
k1_target_sched_reorder (FILE *file ATTRIBUTE_UNUSED,
			 int verbose ATTRIBUTE_UNUSED, rtx *ready, int *nreadyp,
			 int cycle)
{
  int nready = *nreadyp;
  int i, j, n, offset, regno_x, regno_y;
  rtx x, y, diff, insn_to_remove, insn_to_remove2;
  rtx *copy, *it = ready + 2;
  int pass;
  sd_iterator_def sd_it;
  dep_t dep;
  enum machine_mode modes[] = {HImode, SImode};
  enum machine_mode mode;
  enum rtx_code extension, extension2;
  unsigned int mode_idx;

  /* FIXME AUTO: Disable memory packing */
  return 1;
#if 0
    /* We only want to do that in the pre-ira scheduling pass.  */
    if (TARGET_STRICT_ALIGN || reload_completed || reload_in_progress) {
	int all_deps_ready_insn = -1;

        /* Look for LSU operation that block instruction that are only
	   dependant on ready instructions. Make those prioritary. */
        for (i = nready-1; i >= 0; --i) {
            enum rtx_code pat_code;

            pat_code = GET_CODE (PATTERN (ready[i]));
            if (pat_code == USE || pat_code == CLOBBER || pat_code == ASM_INPUT
                || pat_code == ADDR_VEC || pat_code == ADDR_DIFF_VEC)
                continue;

            if (k1_lsu_load_p (ready[i])) {
		for (sd_it = sd_iterator_start (ready[i], SD_LIST_FORW);
		     sd_iterator_cond (&sd_it, &dep);
		     sd_iterator_next (&sd_it)) {
		    rtx next = DEP_CON (dep);
		    sd_iterator_def sd_it2;
		    dep_t dep2;
		    bool all_deps_ready = true;
		    bool all_other_deps_emitted = true;

		    if (DEBUG_INSN_P (next))
			continue;

		    for (sd_it2 = sd_iterator_start (next, SD_LIST_BACK);
			 sd_iterator_cond (&sd_it2, &dep2);
			 sd_iterator_next (&sd_it2)) {
			rtx pro = DEP_PRO (dep2);

			if (DEBUG_INSN_P (pro))
			    continue;

			pat_code = GET_CODE (PATTERN (pro));
			if (pat_code == USE || pat_code == CLOBBER
			    || pat_code == ADDR_VEC || pat_code == ADDR_DIFF_VEC)
			    continue;

			/* -1 is QUEUE_READY. See haifa-sched.c */
			if ((HID (pro)->queue_index) != -1) {
			    all_deps_ready = false;
			    break;
			}

			if (pro != ready[i]) {
			    all_other_deps_emitted = false;
			    if (all_deps_ready_insn >= 0)
				break;
			}
		    }

		    if (!all_deps_ready)
			continue;

		    if (!all_other_deps_emitted) {
			if (all_deps_ready_insn < 0)
			    all_deps_ready_insn = i;
		    } else {
			rtx tmp = ready[i];
			/* All other deps have been emitted! Just put
			   this one out! */
			ready[i] = ready[nready-1];
			ready[nready-1] = tmp;

			return k1_target_sched_issue_rate ();
		    }
		}
            }
        }

	if (all_deps_ready_insn >= 0) {
	    /* No LSU was the last dependency of another instruction,
	       but the instruction stored at index
	       'all_deps_ready_insn' blocks an instruction that only
	       depends on other ready instructions. */
	    rtx tmp = ready[all_deps_ready_insn];
	    ready[all_deps_ready_insn] = ready[nready-1];
	    ready[nready-1] = tmp;

	    return k1_target_sched_issue_rate ();
	}

        /* Make LSU load operations prioritary */
        for (i = nready-1; i >= 0; --i) {
            enum rtx_code pat_code;

            pat_code = GET_CODE (PATTERN (ready[i]));
            if (pat_code == USE || pat_code == CLOBBER || pat_code == ASM_INPUT
                || pat_code == ADDR_VEC || pat_code == ADDR_DIFF_VEC)
                continue;

            if (k1_lsu_load_p (ready[i])) {
                rtx tmp = ready[i];
                ready[i] = ready[nready-1];
                ready[nready-1] = tmp;

                return k1_target_sched_issue_rate ();
            }
        }
        /* and then the stores */
        for (i = nready-1; i >= 0; --i) {
            enum rtx_code pat_code;

            pat_code = GET_CODE (PATTERN (ready[i]));
            if (pat_code == USE || pat_code == CLOBBER || pat_code == ASM_INPUT
                || pat_code == ADDR_VEC || pat_code == ADDR_DIFF_VEC)
                continue;

            if (k1_lsu_store_p (ready[i])) {
                rtx tmp = ready[i];
                ready[i] = ready[nready-1];
                ready[nready-1] = tmp;

                return k1_target_sched_issue_rate ();
            }
        }

        return k1_target_sched_issue_rate ();
    }

    for (mode_idx = 0; mode_idx < ARRAY_SIZE(modes); ++mode_idx) {
        mode = modes[mode_idx];

        for (pass = PACK_LOADS; pass <= PACK_STORES; ++pass) {

            if (mode != SImode && pass != PACK_LOADS)
                continue;

            for (i = nready-1; i >= 0; --i) {
                x = single_set (ready[i]);

                if (!k1_interesting_mem_access (pass, mode, x, &x, &extension, &regno_x))
                    continue;

                for (j = i-1; j >= 0; --j) {
                    insn_to_remove = NULL_RTX;
                    insn_to_remove2 = NULL_RTX;
                    y = single_set (ready[j]);

                    if (!k1_interesting_mem_access (pass, mode, y, &y, &extension2, &regno_y))
                        continue;

                    if (extension2 != extension)
                        continue;

                    diff = k1_contiguous_mem_access_p (mode,
                                                       x, regno_x,
                                                       y, regno_y,
                                                       &insn_to_remove,
                                                       &insn_to_remove2);
                    offset = (diff && CONST_INT_P (diff)) ? INTVAL (diff) : 0;
                    if (abs (offset) != GET_MODE_SIZE(mode))
                        continue;

                    /* Make x the low part of the memory reference. */
                    if (offset < 0) {
                        int tmp;
                        rtx z;
                        z = x; x = y; y = z;
                        tmp = regno_x; regno_x = regno_y; regno_y = tmp;
                        tmp = i; i = j; j = tmp;
                    }

                    if (TARGET_STRICT_ALIGN
                        && MEM_ALIGN (x) < 2*GET_MODE_SIZE (mode)*BITS_PER_UNIT)
                        continue;

                    /* If we have 2 hard registers, make sure they are in the
                       right order. */
                    if (regno_x < FIRST_PSEUDO_REGISTER
                        && (regno_x % 2
                            || regno_x > regno_y)) {
                        if (offset < 0) {
                            int tmp;
                            rtx z;
                            z = x; x = y; y = z;
                            tmp = regno_x; regno_x = regno_y; regno_y = tmp;
                            tmp = i; i = j; j = tmp;
                        }

                        continue;
                    }

                    /* Create the new instruction and make the necessary
                       adjustement to the dependent instructions.  */
                    if (!k1_pack_mems (mode, extension,
                                       ready[i], x, regno_x,
                                       ready[j], y, regno_y,
                                       cycle)) {
                        if (offset < 0) {
                            int tmp;
                            rtx z;
                            z = x; x = y; y = z;
                            tmp = regno_x; regno_x = regno_y; regno_y = tmp;
                            tmp = i; i = j; j = tmp;
                        }
                        continue;
                    }

                    /* Remove the now useless instructions from the ready
                       list.  */
                    copy = (rtx*)alloca (nready * sizeof(*copy));
                    memcpy (copy, ready,  nready * sizeof(*copy));

                    n = 0;
                    ready[0] = copy[j];
                    ready[1] = copy[i];
                    while (n != nready) {
                        if (n == j || n == i) { n++; continue; }
                        *it = copy[n];
                        ++it; ++n;
                    }

                    *nreadyp -= 2;

                    /* The things we do in k1_sched_remove_insn can modify the
                       ready list, thus we store the 2 insns we want to remove
                       apart.  */
                    copy[0] = ready[0];
                    copy[1] = ready[1];

                    for (i = 0; i <= 1; ++i) {
                        /* Remove the insn and all dependencies.  */
                        k1_sched_remove_insn (copy[i]);
                    }

                    /* If the packing allows to remove an additional
                       instruction, do that here.  */

                    if (insn_to_remove) {
                        sd_iterator_def sd_it;
                        dep_t dep;
                        struct k1_address addr;

                    remove_again:
                        k1_analyze_address (XEXP (x, 0), false, &addr);
                        if (addr.mode == ADDR_INVALID)
                            return 0;

                        /* The instruction might have been scheduled
                           earlier and already removed from the dep
                           structures. */
                        if (INSN_FORW_DEPS (insn_to_remove)) {
                            /* The instruction should be selected for removal
                               only if it has no other dependencies that one of
                               the 2 mem accesses.  Simply remove all its
                               deps.  */
                            sd_it = sd_iterator_start (insn_to_remove,
                                                       (SD_LIST_FORW
                                                        | SD_LIST_RES_FORW
                                                        | SD_LIST_BACK
                                                        | SD_LIST_RES_BACK));

                            while (sd_iterator_cond (&sd_it, &dep))
                                sd_delete_dep (sd_it);
                        }

                        add_post_packing_insn_deletion (insn_to_remove, x);

                        if (insn_to_remove2) {
                            insn_to_remove = insn_to_remove2;
                            insn_to_remove2 = NULL_RTX;
                            goto remove_again;
                        }
                    }

                    /* As we changed the ready list, don't do more than one
                       packing per iteration.  Returning 0 here means the
                       cycle is finished: we'll get called again with an
                       updated ready list.  */
                    return 0;
                }

                if (reg_mentioned_p (SET_DEST(single_set(ready[i])),
                                     SET_SRC(single_set(ready[i]))))
                    continue;

                /* Two contiguous loads/stores can't possibly be data
                   dependent, however, such dependencies are introduced when
                   the subreg pass splits a double register an keeps its
                   points-to information untouched.
                   Look for such cases: if the current mem access blocks a
                   contiguous access, just remove that dependency.  */
                for (sd_it = sd_iterator_start (ready[i], SD_LIST_FORW);
                     sd_iterator_cond (&sd_it, &dep);
                     sd_iterator_next (&sd_it)) {
		    bool cancelled = (DEP_STATUS (dep) & DEP_CANCELLED) != 0;
		    rtx con_insn = DEP_CON (dep);
                    y = single_set (con_insn);

		    if (cancelled)
			continue;

                    if (!k1_interesting_mem_access (pass, mode, y, &y, &extension, &regno_y))
                        continue;

                    if (reg_mentioned_p (SET_DEST(single_set(con_insn)),
                                         SET_SRC(single_set(con_insn))))
                        continue;

                    diff = k1_contiguous_mem_access_p (mode,
                                                       x, regno_x,
                                                       y, regno_y,
                                                       NULL, NULL);
                    offset = (diff && CONST_INT_P (diff)) ? INTVAL (diff) : 0;
                    if (abs (offset) != GET_MODE_SIZE(mode))
                        continue;

                    /* Resolve the dependence between INSN and NEXT.
                       sd_resolve_dep () moves current dep to another list thus
                       advancing the iterator.  */
                    sd_resolve_dep (sd_it);

		    /* And don't break transitive dependencies. All
		       the forward dependencies of the now released
		       instruction become our forward dependencies as
		       well. */
		    for (sd_it = sd_iterator_start (con_insn, SD_LIST_FORW);
			 sd_iterator_cond (&sd_it, &dep);
			 sd_iterator_next (&sd_it)) {
			dep_def _new_dep, *new_dep = &_new_dep;
			if ((DEP_STATUS (dep) & DEP_CANCELLED) != 0)
			    continue;
			new_dep = (dep_t)memcpy (new_dep, dep, sizeof(_new_dep));
			DEP_PRO (new_dep) = ready[i];
			sd_add_dep (new_dep, false);
		    }

                    /* Don't bother trying to mark next as ready if
                       insn is a debug insn.  If insn is the last hard
                       dependency, it will have already been discounted.  */
                    if (DEBUG_INSN_P (ready[i])
                        && !DEBUG_INSN_P (con_insn))
                        break;

                    if (!IS_SPECULATION_BRANCHY_CHECK_P (ready[i])) {
                        if (HID(con_insn)->todo_spec == 0)
                            HID(con_insn)->todo_spec = HARD_DEP;
                        try_ready (con_insn);
                    } else
                        gcc_unreachable ();

                    /* We called try_ready that might change the ready
                       list pointers. Do not iterate more, or we risk
                       a corruption on upcoming list accesses. */
                    return 0;
                }
            }
        }
    }

    /* Look if there's no instruction in the ready list that blocks a
       memory access.  If that's the case, make it first so that it'll
       get scheduled and allow us to pack that access with others in
       the next call to this function.  */
    {
        sd_iterator_def sd_it;
        dep_t dep;
        for (i = nready-1; i >= 0; --i) {
            rtx set;

            if ((set = single_set (ready[i]))) {
                if (k1_lsu_p (ready[i])) {
                    /* We don't pack DImode or QImode LSUs. Emit them
                       right away */
                    if (GET_MODE (SET_DEST (set)) == DImode
                        || GET_MODE (SET_DEST (set)) == QImode) {
                        rtx tmp = ready[i];
                        ready[i] = ready[nready-1];
                        ready[nready-1] = tmp;
                        return 1;
                    }

                    /* It's a LSU, do not bother with the following
                       loop. */
                    continue;
                } else if (GET_CODE (SET_DEST (set)) == SUBREG) {
                    for (sd_it = sd_iterator_start (ready[i], SD_LIST_RES_BACK);
                         sd_iterator_cond (&sd_it, &dep);
                         sd_iterator_next (&sd_it)) {
                        rtx prev = DEP_PRO (dep);

                        /* if we find the various subreg sets that
                           follow a clobber, try to emit these right
                           after the clobber so that live ranges
                           (that start at the clobber) aren't
                           artificially interferring with
                           instructions that might be inserted inbetween.  */
                        if ((GET_CODE (PATTERN (prev)) == CLOBBER
                             && SUBREG_REG (SET_DEST (set)) == XEXP (SET_DEST (set), 0))
                            || (GET_CODE (PATTERN (prev))
                                   && GET_CODE (SET_DEST (PATTERN (prev))) == SUBREG
                                   && SUBREG_REG (SET_DEST (PATTERN (prev))) == SUBREG_REG (SET_DEST (set)))
                            ) {
                            rtx tmp = ready[i];
                            ready[i] = ready[nready-1];
                            ready[nready-1] = tmp;
                            return 1;
                        }
                    }
                }
            }

            for (sd_it = sd_iterator_start (ready[i], SD_LIST_FORW);
                 sd_iterator_cond (&sd_it, &dep);
                 sd_iterator_next (&sd_it)) {
                rtx next = DEP_CON (dep);

                if (single_set (next)
                    && k1_lsu_p (next)) {
                    rtx tmp = ready[i];
                    ready[i] = ready[nready-1];
                    ready[nready-1] = tmp;

                    return 1;
                }
            }
        }
    }

    return 1;
#endif /* DISABLE MEMORY PACKING */
}

static int
k1_has_big_immediate_1 (rtx *x, void *data ATTRIBUTE_UNUSED)
{
  return GET_CODE (*x) == SYMBOL_REF || GET_CODE (*x) == LABEL_REF
	 || GET_CODE (*x) == CONST_DOUBLE
	 || (GET_CODE (*x) == CONST_INT
	     && (INTVAL (*x) < -512 || INTVAL (*x) > 511));
}

int
k1_has_big_immediate (rtx x)
{
  return for_each_rtx (&x, k1_has_big_immediate_1, NULL);
}

int
k1_is_uncached_mem_op (rtx op)
{
  /* __convert[_no_sync] addr space should not come here. */
  gcc_assert (!MEM_P (op)
	      || (MEM_ADDR_SPACE (op) == ADDR_SPACE_GENERIC
		  || MEM_ADDR_SPACE (op) == K1_ADDR_SPACE_UNCACHED));

  return MEM_P (op) && MEM_ADDR_SPACE (op) == K1_ADDR_SPACE_UNCACHED;
}

int
k1_adjust_insn_length (rtx insn, int length)
{
  rtx mem = 0;

  insn = PATTERN (insn);

  if (GET_CODE (insn) != SET)
    return length;

  if (GET_CODE (SET_SRC (insn)) == MEM)
    mem = SET_SRC (insn);
  else if (GET_CODE (SET_DEST (insn)) == MEM)
    mem = SET_DEST (insn);

  if (!mem)
    return length;

  return k1_has_big_immediate (mem) ? 8 : 4;
}

static int
k1_uses_register_1 (rtx *x, void *data ATTRIBUTE_UNUSED)
{
  rtx *set_dst = (rtx *) data;

  if (!*x)
    return 0;
  if (GET_CODE (*x) == CLOBBER || GET_CODE (*x) == USE
      || GET_CODE (*x) == EXPR_LIST)
    return -1;

  if (GET_CODE (*x) == SET)
    {
      *set_dst = SET_DEST (*x);
    }

  return GET_CODE (*x) == REG && *x != *set_dst;
}

static int
k1_uses_register (rtx x)
{
  rtx set_dst = NULL;

  return for_each_rtx (&x, k1_uses_register_1, &set_dst);
}

static state_t state;
static bool emited_colon;

/* Unused variables... */
/* static int errata_store_load_load;  */
/* static int had_load;*/
/* static int had_store;*/

struct bundle_regs
{
  int set_dest;
  rtx scanned_insn;
  HARD_REG_SET uses;
  HARD_REG_SET defs;
};

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
      for_each_rtx (&SET_DEST (*x), k1_scan_insn_registers_1, regs);
      regs->set_dest = 0;
      for_each_rtx (&SET_SRC (*x), k1_scan_insn_registers_1, regs);
      return -1;
    }

  if (MEM_P (*x))
    {
      regs->set_dest = 0;
      for_each_rtx (&XEXP (*x, 0), k1_scan_insn_registers_1, regs);
      return -1;
    }

  if (GET_CODE (*x) == CLOBBER)
    {
      if (REG_P (XEXP (*x, 0)))
	SET_HARD_REG_BIT (regs->defs, REGNO (XEXP (*x, 0)));
      if (GET_MODE_SIZE (GET_MODE (*x)) > 4)
	SET_HARD_REG_BIT (regs->defs, REGNO (XEXP (*x, 0)) + 1);
      return -1;
    }

  if (GET_CODE (*x) == EXPR_LIST
      && (enum reg_note) GET_MODE (*x) != REG_DEP_TRUE)
    {
      return -1;
    }

  if (REG_P (*x))
    {
      if (REGNO (*x) > FIRST_PSEUDO_REGISTER)
	{
	  warning (
	    0,
	    "Pseudo register found while scanning final asm. Please report.");
	}

      if (regs->set_dest)
	SET_HARD_REG_BIT (regs->defs, REGNO (*x));
      else
	SET_HARD_REG_BIT (regs->uses, REGNO (*x));

      if (GET_MODE_SIZE (GET_MODE (*x)) > 4)
	{
	  if (regs->set_dest)
	    SET_HARD_REG_BIT (regs->defs, REGNO (*x) + 1);
	  else
	    SET_HARD_REG_BIT (regs->uses, REGNO (*x) + 1);
	}

      if (REGNO (*x) > 128)
	print_rtl_single (stdout, regs->scanned_insn);
    }

  return 0;
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
  for_each_rtx (&insn, k1_scan_insn_registers_1, regs);
}

static HARD_REG_SET current_bundle_reg_defs;
static int bundle_length;

static void
k1_clear_insn_registers (void)
{
  CLEAR_HARD_REG_SET (current_bundle_reg_defs);
}

void
k1_final_prescan_insn (rtx insn, rtx *opvec ATTRIBUTE_UNUSED,
		       int nops ATTRIBUTE_UNUSED)
{
  int can_issue;
  struct bundle_regs regs;

  if (!INSN_P (insn) || DEBUG_INSN_P (insn) || GET_CODE (PATTERN (insn)) == USE
      || GET_CODE (PATTERN (insn)) == CLOBBER || LABEL_P (insn))
    return;

  /* fprintf (asm_out_file, "/\* %d *\/", INSN_UID (insn)); */
  if (!scheduling || !TARGET_BUNDLING)
    return;

  if (!state)
    state = xcalloc (1, state_size ());
  can_issue = state_transition (state, insn) < 0;
  k1_scan_insn_registers (insn, &regs);
  bundle_length += get_attr_length (insn);

#if 0
    if (can_issue)
        fprintf(asm_out_file, "# state: %i\n", *(unsigned char*)state);
#endif

  if (!can_issue
      || hard_reg_set_intersect_p (regs.uses, current_bundle_reg_defs)
      || hard_reg_set_intersect_p (regs.defs, current_bundle_reg_defs)
      || bundle_length > 8 * 4)
    {
      if (!emited_colon)
	fprintf (asm_out_file,
		 "\t;; /* Can't issue next in the same bundle */\n");
      state_reset (state);
      k1_clear_insn_registers ();
      IOR_HARD_REG_SET (current_bundle_reg_defs, regs.defs);
      state_transition (state, insn);
      bundle_length = get_attr_length (insn);
#if 0
        fprintf(asm_out_file, "# state: %i\n", *(unsigned char*)state);
#endif
      return;
    }

  IOR_HARD_REG_SET (current_bundle_reg_defs, regs.defs);
}

static void
k1_target_asm_final_postscan_insn (FILE *file, rtx insn,
				   rtx *opvec ATTRIBUTE_UNUSED,
				   int noperands ATTRIBUTE_UNUSED)
{
  rtx next_real_insn = RTX_NEXT (insn);

  emited_colon = false;

  if (!scheduling || !TARGET_BUNDLING)
    {
      fprintf (file, "\t;;\n");
      return;
    }

  if (!INSN_P (insn) || DEBUG_INSN_P (insn) || GET_CODE (PATTERN (insn)) == USE
      || GET_CODE (PATTERN (insn)) == CLOBBER || LABEL_P (insn))
    return;

  if (!state)
    state = xcalloc (1, state_size ());

  while (next_real_insn
	 && (!INSN_P (next_real_insn) || DEBUG_INSN_P (next_real_insn)
	     || GET_CODE (PATTERN (next_real_insn)) == USE
	     || GET_CODE (PATTERN (next_real_insn)) == CLOBBER))
    {
      if (next_real_insn
	  && (LABEL_P (next_real_insn)
	      || NOTE_INSN_BASIC_BLOCK_P (next_real_insn)))
	break;
      next_real_insn = RTX_NEXT (next_real_insn);
    }

  emit_colon = !next_real_insn || LABEL_P (next_real_insn)
	       || NOTE_INSN_BASIC_BLOCK_P (next_real_insn)
	       || GET_MODE (next_real_insn) == TImode || JUMP_P (insn)
	       || CALL_P (insn);

  if (emit_colon && !JUMP_P (insn) && !CALL_P (insn) && next_real_insn
      && (CALL_P (next_real_insn) || JUMP_P (next_real_insn))
      && !k1_uses_register (PATTERN (next_real_insn)))
    emit_colon = false;

  if (emit_colon)
    {
      emited_colon = true;
      fprintf (file, "\t;;\n");
      bundle_length = 0;
      state_reset (state);
      k1_clear_insn_registers ();

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
	  /* Penalty for double-ALU size and lack of
	     schedulatbilty.  */
	  if (GET_MODE (*x) == DImode)
	    cost->total += COSTS_N_INSNS (1) + 1;
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

static bool
k1_target_rtx_costs (rtx x, int code ATTRIBUTE_UNUSED,
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

  for_each_rtx (cost.toplevel, k1_rtx_operand_cost, &cost);
  *total = cost.total;
  return true;
}

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
  packed_mems = 0;
}

static void
k1_target_sched_finish_global (FILE *file ATTRIBUTE_UNUSED,
			       int verbose ATTRIBUTE_UNUSED)
{
  if (packed_mems)
    {
      while (post_packing_action)
	{
	  struct post_packing_action *action = post_packing_action;
	  post_packing_action = action->next;
	  if (action->type == CLOBBER_INSERT)
	    {
	      rtx head = BB_HEAD (BLOCK_FOR_INSN (action->insn));
	      rtx insn = action->insn;

	      if (insn == head)
		{
		  /* Do not insert the CLOBBER before the basic-block
		     start note. */
		  while (GET_CODE (action->insn) == NOTE
			 || GET_CODE (action->insn) == CODE_LABEL)
		    {
		      action->insn = NEXT_INSN (action->insn);
		    }
		}
	      else
		{
		  rtx set;
		  /* Both parts of the double reg might have been
		     set in the same tick, thus we might arbitrarily
		     insert the clobber inbetween the subreg
		     sets... check that it's not the case.  */
		  /* we know that  (insn != head) here. */
		  do
		    {
		      insn = PREV_INSN (insn);
		      set = single_set (insn);
		      if (set && GET_CODE (SET_DEST (set)) == SUBREG
			  && SUBREG_REG (SET_DEST (set)) == action->reg)
			{
			  action->insn = insn;
			  break;
			}
		    }
		  while (insn != head);
		}

	      emit_insn_before (gen_rtx_CLOBBER (DImode, action->reg),
				action->insn);
	    }
	  else if (action->type == INSN_DELETE)
	    {
	      rtx insn_to_remove = action->insn;
	      rtx set;
	      /* rtx x; */
	      /* x = copy_rtx (x); */
	      /* replace_rtx (x, SET_DEST (single_set (insn_to_remove)), */
	      /*              SET_SRC (single_set (insn_to_remove))); */

	      /* Replace the register it defines everywhere
		 (should be only in the MEM of one of the mem
		 acesses.)  */
	      if ((set = single_set (insn_to_remove)) && REG_P (SET_DEST (set))
		  && k1_replace_reg_rtx (REGNO (SET_DEST (set)), SET_DEST (set),
					 SET_SRC (set), insn_to_remove, NULL,
					 true, false))
		{
		  remove_insn (insn_to_remove);
		  set_insn_deleted (insn_to_remove);
		}
	    }
	  else if (action->type == REG_COPY_INSERT)
	    {
	      emit_insn_after (k1_gen_reg_copy (action->reg, action->reg2),
			       action->insn);
	    }
	  else
	    {
	      gcc_unreachable ();
	    }
	  free (action);
	}

      df_insn_rescan_all ();
      free_dominance_info (CDI_DOMINATORS);
    }
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

static void
clobber_reg (rtx *call_fusage, rtx reg)
{
  gcc_assert (REG_P (reg) && REGNO (reg) < FIRST_PSEUDO_REGISTER);

  *call_fusage = gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER (VOIDmode, reg),
				    *call_fusage);
}

void
k1_expand_call (rtx fnaddr, rtx arg, rtx retval, bool sibcall)
{
  rtx call;
  rtx use = NULL_RTX;

  /* if (TARGET_FDPIC) { */
  /*     crtl->uses_pic_offset_table = TRUE; */
  /*     use_reg (&use, pic_offset_table_rtx); */

  /*     if (!jump_operand (fnaddr, Pmode)) { */
  /*         rtx funcdesc_addr = XEXP (fnaddr, 0); */
  /*         /\* Load the real function pointer *\/ */
  /*         fnaddr = gen_rtx_MEM (Pmode, force_reg(Pmode, fnaddr)); */
  /*         /\* Load the fdpic register for the destination *\/ */
  /*         emit_move_insn (pic_offset_table_rtx, gen_rtx_MEM (SImode,
   * plus_constant (Pmode, funcdesc_addr, 4))); */
  /*     } else { */
  /*         /\* Make sure the fdpic register is set to our current GOT *\/ */
  /*         emit_move_insn (pic_offset_table_rtx, k1_pic_register_initial_val
   * ()); */
  /*     } */
  /* } else */
  if (TARGET_GPREL)
    {
      gcc_unreachable ();
      /* rtx callee = XEXP (fnaddr, 0); */

      /* /\* Functions in GPREL expect to have a correct r14 on */
      /*    entry. Put a restoration before every call. *\/ */
      /* /\* If we are after register allocation, we must be emitting */
      /*    the stack overflow check block. Let's say that this */
      /*    function can't use global data... *\/ */
      /* if (can_create_pseudo_p ()) */
      /*     emit_move_insn (pic_offset_table_rtx, k1_pic_register_initial_val
       * ()); */

      /* use_reg (&use, pic_offset_table_rtx); */

      /* /\* A function that isn't local might be PIC, and as such */
      /*    clobber the r14 value. *\/ */
      /* if (GET_CODE (callee) == SYMBOL_REF && !SYMBOL_REF_LOCAL_P (callee)) */
      /*     clobber_reg (&use, pic_offset_table_rtx); */
    }

  if (!jump_operand (fnaddr, Pmode) || K1_FARCALL)
    {
      rtx callee = copy_addr_to_reg (XEXP (fnaddr, 0));
      fnaddr = gen_rtx_MEM (Pmode, callee);
    }

  if (!sibcall)
    {
      clobber_reg (&use, k1_link_reg_rtx);

      if (retval)
	call = emit_call_insn (gen_real_call_value (retval, fnaddr, arg));
      else
	call = emit_call_insn (gen_real_call (fnaddr, arg));
    }
  else
    {
      if (retval)
	call = emit_call_insn (gen_sibcall_value_real (retval, fnaddr, arg));
      else
	call = emit_call_insn (gen_sibcall_real (fnaddr, arg));
    }

  CALL_INSN_FUNCTION_USAGE (call) = use;
}

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

  if (TARGET_GPREL && k1_needs_gp_symbol_reloc (x, NULL))
    {
      return false;
    }
  else if (flag_pic && k1_needs_symbol_reloc (x))
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
  /* else if (k1_has_gprel(x)) { */
  /*   return k1_legitimize_gp_address (x, NULL); */
  /* }  */
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
  if (as != K1_ADDR_SPACE_UNCACHED && as != ADDR_SPACE_GENERIC)
    return false;

  return k1_target_legitimate_address_p (mode, exp, strict);
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
  return true; // Address spaces (GENERIC or __UNCACHED) refer to the same space
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
k1_dependencies_evaluation_hook (rtx head, rtx tail)
{
  rtx insn, insn2, next_tail, last_sync = head;

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
k1_invalid_within_doloop (const_rtx insn)
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
  rtx insn = loop->loop_end;

  emit_insn_before (gen_addsi3 (loop->iter_reg, loop->iter_reg, constm1_rtx),
		    loop->loop_end);

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
hwloop_pattern_reg (rtx insn)
{
  rtx reg;

  if (!JUMP_P (insn) || recog_memoized (insn) != CODE_FOR_loop_end)
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
  rtx insn, seq, entry_after;

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

  df_analyze ();

  /* Doloop optimization. */
  k1_reorg_loops ();

  /* This is needed. Else final pass will crash on debug_insn-s */
  if (k1_flag_var_tracking)
    {
      compute_bb_for_insn ();
      timevar_push (TV_VAR_TRACKING);
      variable_tracking_main ();
      timevar_pop (TV_VAR_TRACKING);
      free_bb_for_insn ();
    }
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

  /* Set the small data limit.  */
  k1_small_data_threshold
    = (global_options_set.x_g_switch_value ? g_switch_value
					   : K1C_DEFAULT_GVALUE);
  g_switch_value = k1_small_data_threshold;
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
	/* case UNSPEC_FUNCDESC_GOT: */
	/*   fputs ("@got_funcdesc(", (fp)); */
	/*   output_addr_const ((fp), XVECEXP ((x), 0, 0)); */
	/*   fputs (")", (fp)); */
	/*   return true; */
	/* case UNSPEC_FUNCDESC_GOTOFF: */
	/*   fputs ("@gotoff_funcdesc(", (fp)); */
	/*   output_addr_const ((fp), XVECEXP ((x), 0, 0)); */
	/*   fputs (")", (fp)); */
	/*   return true; */
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
	case UNSPEC_GPREL:
	  fputs ("@gprel(", (fp));
	  output_addr_const ((fp), XVECEXP ((x), 0, 0));
	  fputs (")", (fp));
	  return true;
	case UNSPEC_GPREL10:
	  fputs ("@gprel10(", (fp));
	  output_addr_const ((fp), XVECEXP ((x), 0, 0));
	  fputs (")", (fp));
	  return true;
	default:
	  return false;
	}
    }
  return false;
}

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
      if (mode == SImode || mode == HImode || mode == QImode)
	res = 4;
      else if (mode == DImode)
	res = 2;
      break;
    case PLUS_EXPR:
    case MIN_EXPR:
    case MAX_EXPR:
      if (mode == SImode || mode == HImode || mode == QImode)
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

#undef TARGET_ASM_SELECT_RTX_SECTION
#define TARGET_ASM_SELECT_RTX_SECTION k1_select_rtx_section

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

#undef TARGET_SCHED_FINISH_GLOBAL
#define TARGET_SCHED_FINISH_GLOBAL k1_target_sched_finish_global

#undef TARGET_SCHED_DFA_NEW_CYCLE
#define TARGET_SCHED_DFA_NEW_CYCLE k1_target_sched_dfa_new_cycle

/* FIXME AUTO: do not pack mem on coolidge yet */
/* #undef TARGET_SCHED_REORDER */
/* #define TARGET_SCHED_REORDER k1_target_sched_reorder */

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

#undef TARGET_IN_SMALL_DATA_P
#define TARGET_IN_SMALL_DATA_P k1_in_small_data_p

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
/* #undef TARGET_STATIC_CHAIN */
/* #define TARGET_STATIC_CHAIN k1_static_chain */

struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-k1.h"
