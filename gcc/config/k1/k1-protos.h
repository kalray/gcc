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

#ifndef _K1_REGS
#define _K1_REGS
#error This is dubious.
//#include "k1b-registers.h"
#endif

#ifdef HAVE_ATTR_arch
extern enum attr_arch k1_arch_schedule;
#endif

extern bool k1_cannot_change_mode_class (enum machine_mode from,
					 enum machine_mode to,
					 enum reg_class reg_class);

#ifdef RTX_CODE
#include "tree-pass.h"

extern int k1_hard_regno_mode_ok (unsigned regno, enum machine_mode mode);
extern void k1_output_load_multiple (rtx *operands);

extern void k1_expand_call (rtx fnaddr, rtx arg, rtx retval, bool sibcall);

extern rtx k1_return_addr_rtx (int count, rtx frameaddr);

extern void k1_expand_prologue (void);

extern void k1_expand_epilogue (void);

extern void k1_expand_stack_check_allocate_stack (rtx target, rtx adjust);

extern void k1_expand_fdpic_icall (rtx retval, rtx fnaddr, rtx callarg,
				   int sibcall);

extern void k1_expand_fdpic_call (rtx retval, rtx fnaddr, rtx callarg,
				  int sibcall);

extern void k1_expand_helper_pic_call (rtx retval, rtx fnaddr, rtx callarg,
				       int sibcall);

extern void k1_target_print_operand (FILE *file, rtx x, int code);

extern void k1_target_print_operand_address (FILE *file, rtx x);

extern bool k1_target_print_punct_valid_p (unsigned char code);

extern void k1_init_expanders (void);

extern void k1_emit_immediate_to_register_move (rtx, rtx);

extern void k1_emit_stack_overflow_block (rtx *seq, rtx *last);

extern void k1_final_prescan_insn (rtx insn, rtx *opvec, int nops);

extern bool k1_expand_mov (rtx operands[]);
extern bool k1_expand_mov_immediate (rtx operands[]);

extern rtx k1_find_or_create_SC_register (rtx curr_insn, rtx low, rtx high);

extern bool k1_legitimate_pic_operand_p (rtx x);

extern bool k1_legitimate_modulo_addressing_p (rtx x, bool strict);

extern bool k1_lowbit_highbit_constant_p (HOST_WIDE_INT val, int *lowbit,
					  int *highbit);

extern void k1_override_options (void);

extern int k1_has_big_immediate (rtx x);

extern void k1_expand_vcondv2si (rtx *);

extern void k1_expand_vcondv4hi (rtx *);

extern void k1_expand_old_sync_instruction (enum rtx_code code, rtx dest,
					    rtx addr, rtx val);

extern void k1_expand_new_sync_instruction (enum rtx_code code, rtx dest,
					    rtx addr, rtx val);

extern void k1_expand_sync_instruction (enum rtx_code code, rtx addr, rtx val);

extern int k1_mau_lsu_double_port_bypass_p (rtx producer, rtx consumer);

extern int k1_has_pic_reference (rtx x);
/* This function is exported for the tirex plugin. Do not make it
   static ! */
extern int k1_has_tls_reference (rtx x);

extern int k1_legitimize_reload_address (rtx *ad, int opnum, int type);

extern bool k1_float_fits_bits (const REAL_VALUE_TYPE *r, unsigned bitsz,
				enum machine_mode mode);

extern HOST_WIDE_INT k1_initial_elimination_offset (int, int);

extern GTY (()) rtx k1_sync_reg_rtx;
extern GTY (()) rtx k1_link_reg_rtx;

#endif
#endif
