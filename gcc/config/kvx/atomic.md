;; Machine description for MPPA KVX processor synchronization primitives.
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
   (match_operand:SIDI 2 "mematomic_operand" "");; memory
   (match_operand:SIDI 3 "register_operand" "") ;; expected
   (match_operand:SIDI 4 "register_operand" "") ;; desired
   (match_operand:SI 5 "const_int_operand")     ;; is_weak
   (match_operand:SI 6 "const_int_operand")     ;; model success
   (match_operand:SI 7 "const_int_operand")]    ;; model failure
  ""
  {
    kvx_expand_compare_and_swap (operands);
    DONE;
  }
)

;; Atomic load operation with memory model semantics.
(define_expand "atomic_load<mode>"
  [(match_operand:AI 0 "register_operand" "")   ;; val output
   (match_operand:AI 1 "mematomic_operand" "")  ;; memory
   (match_operand:SI 2 "const_int_operand" "")] ;; model
  ""
  {
    kvx_emit_pre_barrier(operands[2], true);
    switch (<MODE>mode) {
      case E_TImode: emit_insn (gen_lqu (operands[0], operands[1]));  break;
      case E_DImode: emit_insn (gen_ldu (operands[0], operands[1]));  break;
      case E_SImode: emit_insn (gen_lwzu (operands[0], operands[1])); break;
      case E_HImode: emit_insn (gen_lhzu (operands[0], operands[1])); break;
      case E_QImode: emit_insn (gen_lbzu (operands[0], operands[1])); break;
      default: gcc_unreachable ();
      }
    kvx_emit_post_barrier(operands[2], true);
    DONE;
  }
)

;; Atomic store operation with memory model semantics.
(define_expand "atomic_store<mode>"
  [(match_operand:AI 0 "mematomic_operand" "")  ;; memory
   (match_operand:AI 1 "register_operand" "")   ;; val to write
   (match_operand:SI 2 "const_int_operand" "")] ;; model
  ""
  {
    kvx_emit_pre_barrier(operands[2], true);
    emit_move_insn (operands[0], operands[1]);
    kvx_emit_post_barrier(operands[2], true);
    DONE;
  }
)

;; Atomic exchange operation with memory model semantics.
(define_expand "atomic_exchange<mode>"
  [(match_operand:SIDI 0 "register_operand" "") ;; val output (memory content)
   (match_operand:SIDI 1 "mematomic_operand" "");; memory
   (match_operand:SIDI 2 "register_operand" "") ;; new value
   (match_operand:SI 3 "const_int_operand" "")] ;; model
  ""
  {
    kvx_expand_atomic_op (SET, operands[0], false, operands[1], operands[2], operands[3]);
    DONE;
  }
)

;; Atomic operation (add, sub, or, and, xor, nand) on memory with memory
;; model semantics.
(define_expand "atomic_<atomic_optab><mode>"
  [(match_operand:SIDI 0 "register_operand" "")    ;; op result
   (atomic_op:SIDI (match_dup 0)
     (match_operand:SIDI 1 "mematomic_operand" ""));; op1, op2
   (match_operand:SI 2 "const_int_operand" "")]    ;; model
  ""
  {
    kvx_expand_atomic_op (<CODE>, NULL_RTX, false, operands[0], operands[1], operands[2]);
    DONE;
  }
)

;; Atomic operation (add, sub, or, and, xor, nand) on memory with memory
;; model semantics, return the original value.
(define_expand "atomic_fetch_<atomic_optab><mode>"
 [(match_operand:SIDI 0 "register_operand" "")    ;; output (memory content before op)
  (atomic_op:SIDI
    (match_operand:SIDI 1 "mematomic_operand" "") ;; op1, op result
    (match_operand:SIDI 2 "register_operand" "")) ;; op2
   (match_operand:SI 3 "const_int_operand")]      ;; model
  ""
  {
    kvx_expand_atomic_op (<CODE>, operands[0], false, operands[1], operands[2], operands[3]);
    DONE;
  }
)

;; Atomic operation (add, sub, or, and, xor, nand) on memory with memory
;; model semantics, perform the operation then return the result.
(define_expand "atomic_<atomic_optab>_fetch<mode>"
 [(match_operand:SIDI 0 "register_operand" "")    ;; output (op result)
  (atomic_op:SIDI
    (match_operand:SIDI 1 "mematomic_operand" "") ;; op1, op result
    (match_operand:SIDI 2 "register_operand" "")) ;; op2
  (match_operand:SI 3 "const_int_operand" "")]    ;; model
  ""
  {
    kvx_expand_atomic_op (<CODE>, operands[0], true, operands[1], operands[2], operands[3]);
    DONE;
  }
)

;; TO GO FURTHER: atomic_exchange<mode> and
;; atomic_*<atomic_optab>*<mode> patterns above can also be
;; implemented for QI HI and TI modes by using a compare-and-swap loop
;; (with kvx_expand_atomic_op for example).

;; Atomic test-and-set operation on memory byte with memory model
;; semantics.
(define_expand "atomic_test_and_set"
 [(match_operand:QI 0 "register_operand" "")   ;; output (memory content)
  (match_operand:QI 1 "mematomic_operand" "")  ;; memory
  (match_operand:SI 2 "const_int_operand" "")] ;; model
  ""
  {
    kvx_expand_atomic_test_and_set (operands);
    DONE;
  }
)

;; Atomic bitwise operation on memory with memory model semantics,
;; return the original value of the specified bit.
;; - atomic_bit_test_and_set<mode>
;; - atomic_bit_test_and_complement<mode>
;; - atomic_bit_test_and_reset<mode>
;; These builtins are not implemented: atomic_fetch_[or,xor,and]<mode>
;; will be used instead.

;; TO GO FURTHER: atomic_*test_and_* patterns above can be implemented
;; by using a compare-and-swap loop (with kvx_expand_atomic_op for
;; example).

;; Thread fence with memory model semantics.
(define_expand "mem_thread_fence"
  [(match_operand:SI 0 "const_int_operand" "")] ;; model
  ""
  {
    rtx modifier = gen_rtx_CONST_STRING (VOIDmode, "");
    emit_insn (gen_kvx_fence (modifier));
    DONE;
  }
)

;; Signal fence with memory model semantics.
(define_expand "mem_signal_fence"
  [(match_operand:SI 0 "const_int_operand" "")] ;; model
  ""
  {
    /* KVX memory model is strong enough not to require any
       barrier in order to synchronize a thread with itself. */
    DONE;
  }
)


;; KVX's builtins

;; Atomic Load

(define_insn "kvx_al<lsusize>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r,r")
     (unspec_volatile:SIDI [(match_operand:SIDI 1 "mematomic_operand" "c,d,e")
                            (match_operand 2 "" "")] UNSPEC_ALOAD))
   (use (match_dup 1))]
  "KV3_2"
  "al<lsusize>%2%X1 %0 = %O1"
  [(set_attr "type" "lsu_auxw_atomic,lsu_auxw_atomic_x,lsu_auxw_atomic_y")
   (set_attr "length"             "4,                8,               12")]
)

;; Atomic Store

(define_insn "kvx_as<lsusize>"
  [(set (match_operand:SIDI 1 "mematomic_operand"  "=c,d,e")
        (unspec_volatile:SIDI [(match_operand:SIDI 0 "register_operand" "r,r,r")
                               (match_operand 2 "" "")] UNSPEC_ASTORE))
   (clobber (match_dup 1))]
  "KV3_2"
  "as<lsusize>%2%X1 %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)


;; Atomic Load and Clear
(define_expand "kvx_alclr<lsusize>"
  [(set (match_operand:SIDI 0 "register_operand" "")
        (unspec_volatile:SIDI [(match_operand:SIDI 1 "mematomic_operand" "")
                               (match_operand 2 "" "")] UNSPEC_ALCLR))]
  ""
  ""
)

(define_insn "*alclr<lsusize>_1"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r,r")
         (unspec_volatile:SIDI [(match_operand:SIDI 1 "mematomic_operand" "a,b,m")
                                (match_operand 2 "" "")] UNSPEC_ALCLR))]
  "KV3_1"
  "alclr<lsusize>%2%X1 %0 = %1"
  [(set_attr "type" "lsu_auxw_atomic,lsu_auxw_atomic_x,lsu_auxw_atomic_y")
   (set_attr "length"             "4,                8,               12")]
)

(define_insn "*alclr<lsusize>_2"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r,r")
        (unspec_volatile:SIDI [(match_operand:SIDI 1 "mematomic_operand" "c,d,e")
                               (match_operand 2 "" "")] UNSPEC_ALCLR))]
  "KV3_2"
  "alclr<lsusize>%2%X1 %0 = %O1"
  [(set_attr "type" "lsu_auxw_atomic,lsu_auxw_atomic_x,lsu_auxw_atomic_y")
   (set_attr "length"             "4,                8,               12")]
)

;; Atomic Load and Add
(define_expand "kvx_aladd<lsusize>"
  [(parallel
  [(set (match_operand:SIDI 0 "register_operand" "")
        (unspec_volatile:SIDI [(match_operand:SIDI 1 "mematomic_operand" "")
                               (match_operand 3 "" "")] UNSPEC_ALADD))
   (set (match_dup 1)
        (plus:SIDI (match_dup 1)
                   (match_operand:SIDI 2 "nonmemory_operand" "")))]
  )]
  ""
  ""
)

(define_insn "*aladd<lsusize>_1"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r,r")
        (unspec_volatile:SIDI [(match_operand:SIDI 1 "mematomic_operand" "+a,b,m")
                               (match_operand 3 "" "")] UNSPEC_ALADD))
   (set (match_dup 1)
        (plus:SIDI (match_dup 1)
                   (match_operand:SIDI 2 "nonmemory_operand" "0,0,0")))]
  "KV3_1"
  "aladd<lsusize>%3%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_auxw_atomic,lsu_auxr_auxw_atomic_x,lsu_auxr_auxw_atomic_y")
   (set_attr "length"                  "4,                     8,                    12")]
)

(define_insn "*aladd<lsusize>_2"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r,r")
        (unspec_volatile:SIDI [(match_operand:SIDI 1 "mematomic_operand" "+c,d,e")
                               (match_operand 3 "" "")] UNSPEC_ALADD))
   (set (match_dup 1)
        (plus:SIDI (match_dup 1)
                   (match_operand:SIDI 2 "nonmemory_operand" "0,0,0")))]
  "KV3_2"
  "aladd<lsusize>%3%X1 %O1 = %0"
  [(set_attr "type" "lsu_auxr_auxw_atomic,lsu_auxr_auxw_atomic_x,lsu_auxr_auxw_atomic_y")
   (set_attr "length"                  "4,                     8,                    12")]
)

;; Atomic Compare and Swap
(define_expand "kvx_acswap<lsusize>"
  [(parallel
  [(set (match_operand:TI 0 "register_operand" "")
        (unspec_volatile:TI [(match_operand:SIDI 1 "mematomic_operand" "")
                             (match_operand 2 "" "")] UNSPEC_ACSWAP))
   (set (match_dup 1)
        (unspec:SIDI [(match_dup 1) (match_dup 0)] UNSPEC_ACSWAP))]
  )]
  ""
  ""
)

(define_insn "*acswap<lsusize>_1"
  [(set (match_operand:TI 0 "register_operand" "+r,r,r")
        (unspec_volatile:TI [(match_operand:SIDI 1 "mematomic_operand" "+a,b,m")
                             (match_operand 2 "" "")] UNSPEC_ACSWAP))
   (set (match_dup 1)
        (unspec:SIDI [(match_dup 1) (match_dup 0)] UNSPEC_ACSWAP))]
  "KV3_1"
  "acswap<lsusize>%2%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_auxw_atomic,lsu_auxr_auxw_atomic_x,lsu_auxr_auxw_atomic_y")
   (set_attr "length"                  "4,                     8,                    12")]
)

(define_insn "*acswap<lsusize>_2"
  [(set (match_operand:TI 0 "register_operand" "+r,r,r")
        (unspec_volatile:TI [(match_operand:SIDI 1 "mematomic_operand" "+c,d,e")
                             (match_operand 2 "" "")] UNSPEC_ACSWAP))
   (set (match_dup 1)
        (unspec:SIDI [(match_dup 1) (match_dup 0)] UNSPEC_ACSWAP))]
  "KV3_2"
  "acswap<lsusize>%2%X1 %O1 = %0"
  [(set_attr "type" "lsu_auxr_auxw_atomic,lsu_auxr_auxw_atomic_x,lsu_auxr_auxw_atomic_y")
   (set_attr "length"                  "4,                     8,                    12")]
)

