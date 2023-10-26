/*

   Copyright (C) 2009-2014 Kalray SA.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef KVX_PROTOS_H
#define KVX_PROTOS_H

void expand_builtin_trap (void);

void kvx_expand_builtin_maddt (rtx operands[], int add);
void kvx_expand_builtin_fmuldc (rtx operands[], int lanes);

#ifdef HAVE_ATTR_arch
extern enum attr_arch kvx_arch_schedule;
#endif /* HAVE_ATTR_arch */

extern bool kvx_is_farcall_p (rtx op);
extern bool kvx_cannot_change_mode_class (enum machine_mode from,
					  enum machine_mode to,
					  enum reg_class reg_class);

/* Pass management. */

extern rtl_opt_pass *make_pass_prologue_stack_limit (gcc::context *);
extern rtl_opt_pass *make_pass_kvx_shaker (gcc::context *);

/* kvx-shaker */

void kvx_final_prescan_insn (rtx_insn *insn);
const char * kvx_asm_output_opcode (FILE *stream, const char * code);

#ifdef RTX_CODE
#include "tree-pass.h"

extern void kvx_output_function_profiler (FILE *);
extern HOST_WIDE_INT kvx_first_parm_offset (tree decl);

extern void kvx_output_load_multiple (rtx *operands);

extern void kvx_expand_tablejump (rtx op0, rtx op1);
extern void kvx_expand_call (rtx fnaddr, rtx arg, rtx retval, bool sibcall);

extern rtx kvx_return_addr_rtx (int count, rtx frameaddr);

extern bool kvx_have_stack_checking (void);

extern void kvx_expand_prologue (void);

extern void kvx_expand_epilogue (void);

extern void kvx_expand_stack_check (rtx addr);

extern void kvx_expand_helper_pic_call (rtx retval, rtx fnaddr, rtx callarg,
					int sibcall);

extern bool kvx_modifier_enabled_p (const char * mod, rtx x);
extern rtx kvx_modifier_rounding (rtx x);

extern void kvx_ifcvt_machdep_init (struct ce_if_block *, bool after_combine);
extern void kvx_ifcvt_modify_tests (ce_if_block *ce_info,  rtx true_expr, rtx false_expr,
				    rtx_insn *then_start, rtx_insn *then_end,
				    rtx_insn *else_start, rtx_insn *else_end);
extern rtx kvx_ifcvt_modify_insn (struct ce_if_block *, rtx, rtx_insn *);

extern void kvx_print_operand (FILE *file, rtx x, int code);

extern void kvx_print_operand_address (FILE *file, rtx x);

extern bool kvx_print_punct_valid_p (unsigned char code);

extern bool kvx_is_uncached_mem_op_p (rtx op);

extern bool kvx_expand_load_multiple (rtx operands[]);
extern bool kvx_expand_store_multiple (rtx operands[]);

extern bool kvx_load_multiple_operation_p (rtx op, bool is_uncached);
extern bool kvx_expand_unpack (rtx op0, rtx op1, bool signed_p, bool hi_p);

extern bool kvx_store_multiple_operation_p (rtx op);

extern void kvx_init_expanders (void);

extern void kvx_init_cumulative_args (CUMULATIVE_ARGS *cum, const_tree fntype,
				      rtx libname, tree fndecl,
				      int n_named_args);

extern void kvx_emit_immediate_to_register_move (rtx, rtx);

extern void kvx_emit_stack_overflow_block (rtx *seq, rtx *last);

extern void kvx_expand_mov_constant (rtx operands[]);

extern rtx kvx_find_or_create_SC_register (rtx curr_insn, rtx low, rtx high);

extern bool kvx_legitimate_pic_operand_p (rtx x);

extern bool kvx_legitimate_pic_symbolic_ref_p (rtx op);

extern bool kvx_split_mem (rtx x, rtx *base_out, rtx *offset_out, bool strict);

extern bool kvx_pack_load_store (rtx operands[], int nops);

extern bool kvx_is_farcall_p (rtx);

extern bool kvx_expand_memset_mul (rtx *operands, machine_mode mode);

extern void kvx_override_options (void);

extern bool kvx_hardreg_misaligned_p (rtx op, int align);
extern void kvx_split_128bits_move (rtx dst, rtx src);
extern void kvx_split_256bits_move (rtx dst, rtx src);
extern void kvx_make_128bit_const (rtx dst, rtx src);
extern void kvx_make_256bit_const (rtx dst, rtx src);
extern void kvx_make_512bit_const (rtx dst, rtx src);

extern bool kvx_has_10bit_imm_or_reg_p (rtx x);
extern bool kvx_has_10bit_immediate_p (rtx x);
extern bool kvx_has_37bit_immediate_p (rtx x);
extern bool kvx_has_64bit_immediate_p (rtx x);
extern bool kvx_has_27bit_immediate_p (rtx x);
extern bool kvx_has_54bit_immediate_p (rtx x);

extern HOST_WIDE_INT kvx_const_vector_value (rtx x, int index);

extern bool kvx_has_10bit_vector_const_p (rtx x);

extern bool kvx_has_16bit_vector_const_p (rtx x);

extern bool kvx_has_32bit_vector_const_p (rtx x);

extern bool kvx_has_37bit_vector_const_p (rtx x);

extern bool kvx_has_43bit_vector_const_p (rtx x);

extern bool kvx_has_32x2bit_vector_const_p (rtx x);

extern enum machine_mode kvx_get_predicate_mode (enum machine_mode mode);

extern void kvx_lower_comparison (rtx pred, rtx comp, enum machine_mode mode);

extern void kvx_expand_conditional_move (rtx target, rtx select1, rtx select2, rtx cmp);

extern void kvx_expand_masked_move (rtx target, rtx select1, rtx select2,
				    rtx mask);

extern void kvx_expand_vector_insert (rtx target, rtx source, rtx where);

extern void kvx_expand_vector_extract (rtx target, rtx source, rtx where);

extern void kvx_expand_chunk_splat (rtx target, rtx source,
				    enum machine_mode inner_mode);

extern void kvx_expand_vector_init (rtx target, rtx source);

extern void kvx_expand_vector_duplicate (rtx target, rtx source);

extern bool kvx_expand_vec_perm_const (rtx target, rtx source1, rtx source2, rtx selector);

extern void kvx_expand_vector_shift (rtx target, rtx source, rtx chunk,
				     unsigned bits, int left);

extern void kvx_expand_any64_eqz (rtx target, rtx source, enum machine_mode mode);

extern void kvx_emit_pre_barrier (rtx);
extern void kvx_emit_post_barrier (rtx);
extern void kvx_expand_compare_and_swap (rtx, rtx, rtx, rtx, rtx, rtx, rtx, rtx);
extern void kvx_expand_atomic_op (enum rtx_code, rtx, bool, rtx, rtx, rtx);
extern void kvx_expand_atomic_test_and_set (rtx op[]);

extern int kvx_branch_tested_bypass_p (rtx_insn *prod_insn, rtx_insn *cons_insn);
extern int kvx_stored_value_bypass_p (rtx_insn *prod_insn, rtx_insn *cons_insn);
extern int kvx_accumulator_bypass_p (rtx_insn *prod_insn, rtx_insn *cons_insn);

extern int kvx_has_tls_reference (rtx x);

extern bool kvx_float_fits_bits (const REAL_VALUE_TYPE *r, unsigned bitsz,
				 enum machine_mode mode);

extern poly_int64 kvx_initial_elimination_offset (int, int);

extern int kvx_get_real_frame_size (function *);

/*
 */
enum kvx_symbol_type
{
  SYMBOL_UNKNOWN,
  LABEL_PCREL_ABSOLUTE,
  LABEL_ABSOLUTE,
  SYMBOL_ABSOLUTE,
  SYMBOL_GOT,
  SYMBOL_GOTOFF,

  SYMBOL_TLSGD,
  SYMBOL_TLSLD,
  SYMBOL_TLSIE,
  SYMBOL_TLSLE
};

extern GTY (()) rtx kvx_link_reg_rtx;

extern GTY (()) rtx kvx_divmod_zero;

extern GTY (()) unsigned long kvx_xundef_counter;

#endif /* RTX_CODE */
#endif /* kvx-protos.h */
