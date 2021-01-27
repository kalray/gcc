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

#ifdef HAVE_ATTR_arch
extern enum attr_arch kvx_arch_schedule;
#endif

extern bool kvx_cannot_change_mode_class (enum machine_mode from,
					  enum machine_mode to,
					  enum reg_class reg_class);

#ifdef RTX_CODE
#include "tree-pass.h"

extern void kvx_output_function_profiler (FILE *);
extern HOST_WIDE_INT kvx_first_parm_offset (tree decl);
extern int kvx_starting_frame_offset (void);

extern int kvx_hard_regno_mode_ok (unsigned regno, enum machine_mode mode);
extern int kvx_hard_regno_rename_ok (unsigned from, unsigned to);
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

extern void kvx_print_operand (FILE *file, rtx x, int code);

extern void kvx_print_operand_address (FILE *file, rtx x);

extern bool kvx_print_punct_valid_p (unsigned char code);

extern int kvx_is_uncached_mem_op_p (rtx op);

extern bool kvx_expand_load_multiple (rtx operands[]);
extern bool kvx_expand_store_multiple (rtx operands[]);

extern bool kvx_load_multiple_operation_p (rtx op, bool is_uncached);

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

extern bool kvx_pack_load_store (rtx operands[], unsigned int nops);

extern bool kvx_is_farcall_p (rtx);

extern void kvx_override_options (void);

const char *kvx_asm_pat_copyq (rtx srcreg);
const char *kvx_asm_pat_copyo (void);

bool kvx_ok_for_paired_reg_p (rtx op);
bool kvx_ok_for_quad_reg_p (rtx op);
bool kvx_is_reg_subreg_p (rtx op);

void kvx_split_128bits_move (rtx dst, rtx src, enum machine_mode mode);
void kvx_split_256bits_move (rtx dst, rtx src, enum machine_mode mode);

extern bool kvx_has_10bit_imm_or_register_p (rtx x);
extern bool kvx_has_10bit_immediate_p (rtx x);
extern bool kvx_has_37bit_immediate_p (rtx x);
extern bool kvx_has_64bit_immediate_p (rtx x);

extern HOST_WIDE_INT kvx_const_vector_value (rtx x, int index);

extern bool kvx_has_10bit_vector_const_p (rtx x);

extern bool kvx_has_16bit_vector_const_p (rtx x);

extern bool kvx_has_32bit_vector_const_p (rtx x);

extern bool kvx_has_37bit_vector_const_p (rtx x);

extern bool kvx_has_43bit_vector_const_p (rtx x);

extern bool kvx_has_32x2bit_vector_const_p (rtx x);

extern rtx kvx_lower_comparison (rtx pred, enum rtx_code cmp_code, rtx left,
				 rtx right);

extern void kvx_expand_conditional_move (rtx target, rtx select1, rtx select2,
					 rtx cmp, rtx left, rtx right);

extern void kvx_expand_masked_move (rtx target, rtx select1, rtx select2,
				    rtx mask);

extern void kvx_expand_vector_insert (rtx target, rtx source, rtx where);

extern void kvx_expand_vector_extract (rtx target, rtx source, rtx where);

extern rtx kvx_expand_chunk_splat (rtx target, rtx source,
				   enum machine_mode inner_mode);

extern void kvx_expand_vector_init (rtx target, rtx source);

extern void kvx_expand_vec_perm_const (rtx target, rtx source1, rtx source2, rtx selector);

extern rtx kvx_expand_chunk_shift (rtx target, rtx source1, rtx source2, int shift);

extern void kvx_emit_pre_barrier (rtx, bool);
extern void kvx_emit_post_barrier (rtx, bool);
extern void kvx_expand_compare_and_swap (rtx op[]);
extern void kvx_expand_atomic_op (enum rtx_code, rtx, bool, rtx, rtx, rtx);
extern void kvx_expand_atomic_test_and_set (rtx op[]);

extern int kv3_mau_lsu_double_port_bypass_p (rtx_insn *producer,
					     rtx_insn *consumer);

extern int kvx_has_tls_reference (rtx x);

extern bool kvx_float_fits_bits (const REAL_VALUE_TYPE *r, unsigned bitsz,
				 enum machine_mode mode);

extern HOST_WIDE_INT kvx_initial_elimination_offset (int, int);

char *kvx_ctrapsi4 (void);

/*
 */
enum kvx_symbol_type
{
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

extern GTY (()) rtx kvx_sync_reg_rtx;
extern GTY (()) rtx kvx_link_reg_rtx;

#endif
#endif
