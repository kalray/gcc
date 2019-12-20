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

#ifndef K1_PROTOS_H
#define K1_PROTOS_H

#ifdef HAVE_ATTR_arch
extern enum attr_arch k1_arch_schedule;
#endif

extern bool k1_cannot_change_mode_class (enum machine_mode from,
					 enum machine_mode to,
					 enum reg_class reg_class);

#ifdef RTX_CODE
#include "tree-pass.h"

extern HOST_WIDE_INT k1_first_parm_offset (tree decl);
extern int k1_starting_frame_offset (void);

extern int k1_hard_regno_mode_ok (unsigned regno, enum machine_mode mode);
extern int k1_hard_regno_rename_ok (unsigned from, unsigned to);
extern void k1_output_load_multiple (rtx *operands);

extern void k1_expand_tablejump (rtx op0, rtx op1);
extern void k1_expand_call (rtx fnaddr, rtx arg, rtx retval, bool sibcall);

extern rtx k1_return_addr_rtx (int count, rtx frameaddr);

extern bool k1_have_stack_checking (void);

extern void k1_expand_prologue (void);

extern void k1_expand_epilogue (void);

extern void k1_expand_stack_check (rtx addr);

extern void k1_expand_helper_pic_call (rtx retval, rtx fnaddr, rtx callarg,
				       int sibcall);

extern void k1_print_operand (FILE *file, rtx x, int code);

extern void k1_print_operand_address (FILE *file, rtx x);

extern bool k1_print_punct_valid_p (unsigned char code);

extern int k1_is_uncached_mem_op_p (rtx op);

extern bool k1_expand_load_multiple (rtx operands[]);

extern bool k1_load_multiple_operation_p (rtx op, bool is_uncached);

extern bool k1_store_multiple_operation_p (rtx op);

extern void k1_init_expanders (void);

extern void k1_init_cumulative_args (CUMULATIVE_ARGS *cum, const_tree fntype,
				     rtx libname, tree fndecl,
				     int n_named_args);

extern void k1_emit_immediate_to_register_move (rtx, rtx);

extern void k1_emit_stack_overflow_block (rtx *seq, rtx *last);

extern void k1_final_prescan_insn (rtx insn, rtx *opvec, int nops);

extern void k1_expand_mov_constant (rtx operands[]);

extern rtx k1_find_or_create_SC_register (rtx curr_insn, rtx low, rtx high);

extern bool k1_legitimate_pic_operand_p (rtx x);

extern bool k1_legitimate_pic_symbolic_ref_p (rtx op);

extern bool k1_legitimate_modulo_addressing_p (rtx x, bool strict);

extern bool k1_split_mem (rtx x, rtx *base_out, rtx *offset_out, bool strict);

extern bool k1_pack_load_store (rtx operands[], unsigned int nops);

extern bool k1_is_farcall_p (rtx);

extern void k1_override_options (void);

const char *k1_asm_pat_copyq (rtx srcreg);
const char *k1_asm_pat_copyo (void);

bool k1_ok_for_paired_reg_p (rtx op);
bool k1_ok_for_quad_reg_p (rtx op);
bool k1_is_reg_subreg_p (rtx op);

void k1_split_128bits_move (rtx dst, rtx src, enum machine_mode mode);
void k1_split_256bits_move (rtx dst, rtx src, enum machine_mode mode);

extern bool k1_has_10bit_imm_or_register_p (rtx x);
extern bool k1_has_10bit_immediate_p (rtx x);
extern bool k1_has_37bit_immediate_p (rtx x);
extern bool k1_has_64bit_immediate_p (rtx x);

extern HOST_WIDE_INT k1_const_vector_value (rtx x, int index);

extern bool k1_has_10bit_vector_const_p (rtx x);

extern bool k1_has_16bit_vector_const_p (rtx x);

extern bool k1_has_32bit_vector_const_p (rtx x);

extern bool k1_has_37bit_vector_const_p (rtx x);

extern bool k1_has_43bit_vector_const_p (rtx x);

extern bool k1_has_32x2bit_vector_const_p (rtx x);

extern bool k1_expand_conditional_move (machine_mode mode, rtx operands[]);

extern void k1_expand_vcondv2si (rtx *);

extern void k1_expand_vcondv4hi (rtx *);

extern void k1_emit_pre_barrier (rtx, bool);

extern void k1_emit_pre_barrier (rtx, bool);
extern void k1_emit_post_barrier (rtx, bool);
extern void k1_expand_compare_and_swap (rtx op[]);
extern void k1_expand_atomic_op (enum rtx_code, rtx, bool, rtx, rtx, rtx);
extern void k1_expand_atomic_test_and_set (rtx op[]);

extern int k1_mau_lsu_double_port_bypass_p (rtx_insn *producer,
					    rtx_insn *consumer);

extern int  k1_has_tls_reference(rtx x);

extern bool k1_float_fits_bits (const REAL_VALUE_TYPE *r, unsigned bitsz,
				enum machine_mode mode);

extern HOST_WIDE_INT k1_initial_elimination_offset (int, int);

char *k1_ctrapsi4 (void);

/*
 */
enum k1_symbol_type
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

extern GTY (()) rtx k1_sync_reg_rtx;
extern GTY (()) rtx k1_link_reg_rtx;

#endif
#endif
