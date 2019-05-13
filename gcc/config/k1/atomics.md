;; Machine description for K1C processor synchronization primitives.
;; Copyright (C) 2019 Kalray Inc.
;;
;; This file is part of GCC.
;;
;; GCC is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 3, or (at your option)
;; any later version.
;;
;; GCC is distributed in the hope that it will be useful, but
;; WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;; General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.


;; GCC's builtins as described in gccint:
;;   6.53 Built-in Functions for Memory Model Aware Atomic Operations

;; Atomic compare-and-swap operation with memory model semantics.
(define_expand "atomic_compare_and_swap<mode>"
  [(match_operand:SI 0 "register_operand" "")   ;; bool output
   (match_operand:SIDI 1 "register_operand" "") ;; val output before CAS
   (match_operand:SIDI 2 "memory_operand" "")   ;; memory
   (match_operand:SIDI 3 "register_operand" "") ;; expected
   (match_operand:SIDI 4 "register_operand" "") ;; desired
   (match_operand:SI 5 "const_int_operand")     ;; is_weak
   (match_operand:SI 6 "const_int_operand")     ;; model success
   (match_operand:SI 7 "const_int_operand")]    ;; model failure
  ""
{
  k1_expand_compare_and_swap (operands);
  DONE;
})

;; Atomic load operation with memory model semantics.
(define_expand "atomic_load<mode>"
  [(match_operand:AI 0 "register_operand" "")   ;; val output
   (match_operand:AI 1 "memory_operand" "")     ;; memory
   (match_operand:SI 2 "const_int_operand" "")] ;; model
  ""
{
  k1_emit_pre_barrier(operands[2], true);

  switch (<MODE>mode) {
    case TImode: emit_insn (gen_lqu (operands[0], operands[1]));  break;
    case DImode: emit_insn (gen_ldu (operands[0], operands[1]));  break;
    case SImode: emit_insn (gen_lwzu (operands[0], operands[1])); break;
    case HImode: emit_insn (gen_lhzu (operands[0], operands[1])); break;
    case QImode: emit_insn (gen_lbzu (operands[0], operands[1])); break;
    default: gcc_unreachable ();
    }

  k1_emit_post_barrier(operands[2], true);
  DONE;
})

;; Atomic store operation with memory model semantics.
(define_expand "atomic_store<mode>"
  [(match_operand:AI 0 "memory_operand" "")     ;; memory
   (match_operand:AI 1 "register_operand" "")   ;; val to write
   (match_operand:SI 2 "const_int_operand" "")] ;; model
  ""
{
  k1_emit_pre_barrier(operands[2], true);
  emit_move_insn (operands[0], operands[1]);
  k1_emit_post_barrier(operands[2], true);
  DONE;
})

;; Atomic exchange operation with memory model semantics.
(define_expand "atomic_exchange<mode>"
  [(match_operand:SIDI 0 "register_operand" "") ;; val output (memory content)
   (match_operand:SIDI 1 "memory_operand" "")   ;; memory
   (match_operand:SIDI 2 "register_operand" "") ;; new value
   (match_operand:SI 3 "const_int_operand" "")] ;; model
  ""
{
  k1_expand_atomic_op (SET, operands[0], false, operands[1], operands[2], operands[3]);
  DONE;
})

;; Atomic operation (add, sub, or, and, xor, nand) on memory with memory
;; model semantics.
(define_expand "atomic_<atomic_optab><mode>"
  [(set (match_operand:SIDI 0 "register_operand" "+r")                            ;; op result
    (unspec_volatile:SIDI
      [(atomic_op:SIDI (match_dup 0) (match_operand:SIDI 1 "memory_operand" "r")) ;; op1, op2
       (match_operand:SI 2 "const_int_operand")] UNSPEC_ATOMIC_OP))]              ;; model
  ""
{
  k1_expand_atomic_op (<CODE>, NULL_RTX, false, operands[0], operands[1], operands[2]);
  DONE;
})

;; Atomic operation (add, sub, or, and, xor, nand) on memory with memory
;; model semantics, return the original value.
(define_expand "atomic_fetch_<atomic_optab><mode>"
 [(match_operand:SIDI 0 "register_operand" "")   ;; output (memory content before op)
  (atomic_op:SIDI
   (match_operand:SIDI 1 "memory_operand" "")    ;; op1, op result
   (match_operand:SIDI 2 "register_operand" "")) ;; op2
  (match_operand:SI 3 "const_int_operand")]      ;; model
  ""
{
  k1_expand_atomic_op (<CODE>, operands[0], false, operands[1], operands[2], operands[3]);
  DONE;
})

;; Atomic operation (add, sub, or, and, xor, nand) on memory with memory
;; model semantics, perform the operation then return the result.
(define_expand "atomic_<atomic_optab>_fetch<mode>"
 [(match_operand:SIDI 0 "register_operand" "")   ;; output (op result)
  (atomic_op:SIDI
   (match_operand:SIDI 1 "memory_operand" "")    ;; op1, op result
   (match_operand:SIDI 2 "register_operand" "")) ;; op2
  (match_operand:SI 3 "const_int_operand")]      ;; model
  ""
{
  k1_expand_atomic_op (<CODE>, operands[0], true, operands[1], operands[2], operands[3]);
  DONE;
})

;; TO GO FURTHER: atomic_exchange<mode> and
;; atomic_*<atomic_optab>*<mode> patterns above can also be
;; implemented for QI HI and TI modes by using a compare-and-swap loop
;; (with k1_expand_atomic_op for example).

;; Atomic test-and-set operation on memory byte with memory model
;; semantics.
;; (define_expand "atomic_test_and_set"
;;  [(match_operand:QI 0 "register_operand" "")   ;; output (memory content)
;;   (match_operand:QI 1 "memory_operand" "")     ;; memory
;;   (match_operand:SI 2 "const_int_operand" "")] ;; model
;;   ""
;; {
;;   FAIL;
;; })
;; This builtin is not implemented: we rely on default implementation
;; provided by gcc (WARNING: this is non-atomic).

;; Atomic bitwise operation on memory with memory model semantics,
;; return the original value of the specified bit.
;; - atomic_bit_test_and_set<mode>
;; - atomic_bit_test_and_complement<mode>
;; - atomic_bit_test_and_reset<mode>
;; These builtins are not implemented: atomic_fetch_[or,xor,and]<mode>
;; will be used instead.

;; TO GO FURTHER: atomic_*test_and_* patterns above can be implemented
;; by using a compare-and-swap loop (with k1_expand_atomic_op for
;; example).

;; Thread fence with memory model semantics.
(define_expand "mem_thread_fence"
  [(match_operand:SI 0 "const_int_operand" "")] ;; model
  ""
{
  emit_insn (gen_fence (k1_sync_reg_rtx));
  DONE;
})

;; Signal fence with memory model semantics.
(define_expand "mem_signal_fence"
  [(match_operand:SI 0 "const_int_operand" "")] ;; model
  ""
{
  /* K1C memory model is strong enough not to require any
     barrier in order to synchronize a thread with itself. */
  DONE;
})


;; K1C's builtins

;; Compare and Swap
(define_insn "acswap<lsusize>"
  [(set (match_operand:TI 0 "register_operand" "+r,r")
    (unspec_volatile:TI [(match_operand:SIDI 1 "memory_operand" "+a,b")] UNSPEC_ACSWAP))
   (set (match_dup 1)
    (unspec:SIDI [(match_dup 1) (match_dup 0)] UNSPEC_ACSWAP))]
   ""
   "acswap<lsusize> %1 = %0"
  [(set_attr "length" "4,8")
   (set_attr "type" "lsu_auxr_auxw_atomic,lsu_auxr_auxw_atomic_x")])

;; TO GO FURTHER: acswap has a variant which directly takes a
;; register_operand as operand 1. It is not implemented yet.

;; Fetch and Add
(define_insn "afadd<lsusize>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r,r")
    (unspec_volatile:SIDI [(match_operand:SIDI 1 "memory_operand" "+a,b,m")] UNSPEC_AFADD))
   (set (match_dup 1)
    (plus:SIDI (match_dup 1)
               (match_operand:SIDI 2 "nonmemory_operand" "0,0,0")))]
   ""
   "afadd<lsusize>%m1 %1 = %0"
  [(set_attr "length" "4,8,12")
   (set_attr "type" "lsu_auxr_auxw_atomic,lsu_auxr_auxw_atomic_x,lsu_auxr_auxw_atomic_y")])

;; Load and Clear
(define_insn "alclr<lsusize>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r,r")
    (unspec_volatile:SIDI [(match_operand:SIDI 1 "memory_operand" "a,b,m")] UNSPEC_ALCLR))]
   ""
   "alclr<lsusize> %0 = %1"
  [(set_attr "length" "4,8,12")
   (set_attr "type" "lsu_auxw_atomic,lsu_auxw_atomic_x,lsu_auxw_atomic_y")])

;; TO GO FURTHER: acswap, afadd, and alclr insns have 'scaling' and
;; 'lsucond' variants which are not supported here yet. Exception for
;; afadd which handles 'scaling' variants (its not working by the way:
;; it was disabled at some point because of some MDS instabilities).
