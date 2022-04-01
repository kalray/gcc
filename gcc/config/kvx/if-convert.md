;; COND_EXEC UNARY

(define_insn_and_split "*cond_exec_unary_<FITGPR:mode>"
  [(cond_exec
     (match_operator 4 "zero_comparison_operator"
      [(match_operand:SIDI 3 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:FITGPR 0 "register_operand" "=r")
            (match_operator:FITGPR 2 "pred_unary_operator"
             [(match_operand:FITGPR 1 "register_operand" "r")]))
       (use (match_operand:FITGPR 5 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (match_op_dup 2 [(match_dup 1)]))
   (cond_exec
     (match_op_dup 4 [(match_dup 3) (const_int 0)])
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_unary_<FITGPR:mode>"
  [(set (match_operand:FITGPR 0 "register_operand" "=r")
        (match_operator:FITGPR 2 "pred_unary_operator"
         [(match_operand:FITGPR 1 "register_operand" "r")]))
   (use (match_operand:FITGPR 3 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 2 [(match_dup 1)]))]
)


;; COND_EXEC EXTEND UNARY

(define_insn_and_split "*cond_exec_unary_<SHORT:mode>_ext<ANY_EXTEND:lsux>"
  [(cond_exec
     (match_operator 4 "zero_comparison_operator"
      [(match_operand:SIDI 3 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r")
            (match_operator:DI 2 "pred_unary_operator"
             [(ANY_EXTEND:DI (match_operand:SHORT 1 "register_operand" "r"))]))
       (use (match_operand:DI 5 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (match_op_dup 2 [(ANY_EXTEND:DI (match_dup 1))]))
   (cond_exec
     (match_op_dup 4 [(match_dup 3) (const_int 0)])
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_unary_<SHORT:mode>_ext<ANY_EXTEND:lsux>"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (match_operator:DI 2 "pred_unary_operator"
         [(ANY_EXTEND:DI (match_operand:SHORT 1 "register_operand" "r"))]))
   (use (match_operand:DI 3 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 2 [(ANY_EXTEND:DI (match_dup 1))]))]
)


;; COND_EXEC WI SHIFT

(define_insn_and_split "*cond_exec_shift_<WI:mode>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:WI 0 "register_operand" "=r,r")
            (match_operator:WI 3 "pred_shift_operator"
             [(match_operand:WI 1 "register_operand" "r,r")
              (match_operand:SI 2 "sat_shift_operand" "r,U06")]))
       (use (match_operand:WI 6 "register_operand" "r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*wrapped_shift_<WI:mode>"
  [(set (match_operand:WI 0 "register_operand" "=r,r")
        (match_operator:WI 3 "pred_shift_operator"
         [(match_operand:WI 1 "register_operand" "r,r")
          (match_operand:SI 2 "sat_shift_operand" "r,U06")]))
   (use (match_operand:WI 4 "register_operand" "r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))]
)


;; COND_EXEC WI BINARY

(define_insn_and_split "*cond_exec_binary_<WI:mode>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r,r,r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:WI 0 "register_operand" "=r,r,r,r")
            (match_operator:WI 3 "pred_binary_operator"
             [(match_operand:WI 1 "register_operand" "r,r,r,r")
              (match_operand:WI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")]))
       (use (match_operand:WI 6 "register_operand" "r,r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*wrapped_binary_<WI:mode>"
  [(set (match_operand:WI 0 "register_operand" "=r,r,r,r")
        (match_operator:WI 3 "pred_binary_operator"
         [(match_operand:WI 1 "register_operand" "r,r,r,r")
          (match_operand:WI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")]))
   (use (match_operand:WI 4 "register_operand" "r,r,r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))]
)


;; COND_EXEC *WD BINARY

(define_insn_and_split "*cond_exec_binary_sidi"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r,r,r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
            (match_operator:DI 3 "pred_binary_operator"
             [(ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r,r,r,r"))
              (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")]))
       (use (match_operand:DI 6 "register_operand" "r,r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6)
        (match_op_dup 3 [(ANY_EXTEND:DI (match_dup 1)) (match_dup 2)]))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*wrapped_binary_sidi"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (match_operator:DI 3 "pred_binary_operator"
         [(ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r,r,r,r"))
          (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")]))
   (use (match_operand:DI 4 "register_operand" "r,r,r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 3 [(ANY_EXTEND:DI (match_dup 1)) (match_dup 2)]))]
)


;; COND_EXEC WF BINARY

(define_insn_and_split "*cond_exec_binary_<WF:mode>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:WF 0 "register_operand" "=r")
            (match_operator:WF 3 "pred_binary_operator"
             [(match_operand:WF 1 "register_operand" "r")
              (match_operand:WF 2 "register_operand" "r")]))
       (use (match_operand:WF 6 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*wrapped_binary_<WF:mode>"
  [(set (match_operand:WF 0 "register_operand" "=r")
        (match_operator:WF 3 "pred_binary_operator"
         [(match_operand:WF 1 "register_operand" "r")
          (match_operand:WF 2 "register_operand" "r")]))
   (use (match_operand:WF 4 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))]
)


;; Automated COND_EXEC

;; (define_attr "predicable" "no,yes"
;;   (const_string "no"))

;; (define_cond_exec
;;   [(match_operator 0 "zero_comparison_operator"
;;     [(match_operand:SIDI 1 "register_operand" "r")
;;      (const_int 0)])]
;;   ""
;;   "<suffix>%0z %1?"
;; )

;; ;; sample move template for conditional execution
;; (define_insn "*move<mode>"
;;   [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
;;         (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i"))]
;;   ""
;;   "MOVE %0 = %1"
;;   [(set_attr "predicable" "yes")]
;; )

