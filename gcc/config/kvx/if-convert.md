;; COND_EXEC EXTEND

(define_insn_and_split "*cond_exec_unary_<SHORT:mode>_ext<ANY_EXTEND:lsux>"
  [(cond_exec
     (match_operator 3 "zero_comparison_operator"
      [(match_operand:SIDI 2 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r")
            (ANY_EXTEND:DI (match_operand:SHORT 1 "register_operand" "r")))
       (use (match_operand:DI 4 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 4)
        (ANY_EXTEND:DI (match_dup 1)))
   (cond_exec
     (match_op_dup 3 [(match_dup 2) (const_int 0)])
     (set (match_dup 0) (match_dup 4)))]
)

(define_insn_and_split "*cond_exec_unary_<SHORT:mode>_ext<ANY_EXTEND:lsux>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r")
            (ANY_EXTEND:DI (match_operand:SHORT 1 "register_operand" "r")))
       (use (match_operand:DI 3 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 3)
        (ANY_EXTEND:DI (match_dup 1)))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 2) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 3)))]
)

(define_insn_and_split "*wrapped_unary_<SHORT:mode>_ext<ANY_EXTEND:lsux>"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (ANY_EXTEND:DI (match_operand:SHORT 1 "register_operand" "r")))
   (use (match_operand:DI 2 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (ANY_EXTEND:DI (match_dup 1)))]
)


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

(define_insn_and_split "*cond_exec_unary_<FITGPR:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 3 "register_operand" "r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:FITGPR 0 "register_operand" "=r")
            (match_operator:FITGPR 2 "pred_unary_operator"
             [(match_operand:FITGPR 1 "register_operand" "r")]))
       (use (match_operand:FITGPR 4 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 4)
        (match_op_dup 2 [(match_dup 1)]))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 3) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 4)))]
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


;; COND_EXEC UNARY EXTEND

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

(define_insn_and_split "*cond_exec_unary_<SHORT:mode>_ext<ANY_EXTEND:lsux>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 3 "register_operand" "r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r")
            (match_operator:DI 2 "pred_unary_operator"
             [(ANY_EXTEND:DI (match_operand:SHORT 1 "register_operand" "r"))]))
       (use (match_operand:DI 4 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 4)
        (match_op_dup 2 [(ANY_EXTEND:DI (match_dup 1))]))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 3) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 4)))]
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


;; COND_EXEC ZX UNARY

(define_insn_and_split "*cond_exec_unary_zx<SHORT:mode>di2"
  [(cond_exec
     (match_operator 4 "zero_comparison_operator"
      [(match_operand:SIDI 3 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r")
            (zero_extend:DI (match_operator:SHORT 2 "pred_unary_operator"
                             [(match_operand:SHORT 1 "register_operand" "r")])))
       (use (match_operand:DI 5 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (zero_extend:DI (match_op_dup 2 [(match_dup 1)])))
   (cond_exec
     (match_op_dup 4 [(match_dup 3) (const_int 0)])
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*cond_exec_unary_zx<SHORT:mode>di2.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 3 "register_operand" "r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r")
            (zero_extend:DI (match_operator:SHORT 2 "pred_unary_operator"
                             [(match_operand:SHORT 1 "register_operand" "r")])))
       (use (match_operand:DI 4 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 4)
        (zero_extend:DI (match_op_dup 2 [(match_dup 1)])))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 3) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 4)))]
)

(define_insn_and_split "*wrapped_unary_zx<SHORT:mode>di2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (match_operator:SHORT 2 "pred_unary_operator"
                         [(match_operand:SHORT 1 "register_operand" "r")])))
   (use (match_operand:DI 3 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (zero_extend:DI (match_op_dup 2 [(match_dup 1)])))]
)


;; COND_EXEC CSTORE

(define_insn_and_split "*cond_exec_cstoresi4"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DISI 0 "register_operand" "=r,r")
            (match_operator:DISI 1 "comparison_operator"
             [(match_operand:SI 2 "register_operand" "r,r")
              (match_operand:SI 3 "kvx_r_any32_operand" "r,i")]))
       (use (match_operand:DISI 6 "register_operand" "r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6)
        (match_op_dup 1 [(match_dup 2) (match_dup 3)]))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*cond_exec_cstoresi4.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:DISI 0 "register_operand" "=r,r")
            (match_operator:DISI 1 "comparison_operator"
             [(match_operand:SI 2 "register_operand" "r,r")
              (match_operand:SI 3 "kvx_r_any32_operand" "r,i")]))
       (use (match_operand:DISI 5 "register_operand" "r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (match_op_dup 1 [(match_dup 2) (match_dup 3)]))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_cstoresi4"
  [(set (match_operand:DISI 0 "register_operand" "=r,r")
        (match_operator:DISI 1 "comparison_operator"
         [(match_operand:SI 2 "register_operand" "r,r")
          (match_operand:SI 3 "kvx_r_any32_operand" "r,i")]))
   (use (match_operand:DISI 4 "register_operand" "r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 1 [(match_dup 2) (match_dup 3)]))]
)


(define_insn_and_split "*cond_exec_cstoredi4"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r,r,r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
            (match_operator:DI 1 "comparison_operator"
             [(match_operand:DI 2 "register_operand" "r,r,r,r")
              (match_operand:DI 3 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")]))
       (use (match_operand:DI 6 "register_operand" "r,r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6)
        (match_op_dup 1 [(match_dup 2) (match_dup 3)]))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*cond_exec_cstoredi4.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r,r,r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
            (match_operator:DI 1 "comparison_operator"
             [(match_operand:DI 2 "register_operand" "r,r,r,r")
              (match_operand:DI 3 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")]))
       (use (match_operand:DI 5 "register_operand" "r,r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (match_op_dup 1 [(match_dup 2) (match_dup 3)]))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_cstoredi4"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (match_operator:DI 1 "comparison_operator"
         [(match_operand:DI 2 "register_operand" "r,r,r,r")
          (match_operand:DI 3 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")]))
   (use (match_operand:DI 4 "register_operand" "r,r,r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 1 [(match_dup 2) (match_dup 3)]))]
)


;; COND_EXEC CSTORE EXTEND

(define_insn_and_split "*cond_exec_extend2_cstoresi4"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r,r")
            (match_operator:DI 1 "comparison_operator"
             [(ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r,r"))
              (match_operand:DI 3 "register_s32_operand" "r,B32")]))
       (use (match_operand:DI 6 "register_operand" "r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6)
        (match_op_dup 1 [(ANY_EXTEND:DI (match_dup 2)) (match_dup 3)]))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*cond_exec_extend2_cstoresi4.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r,r")
            (match_operator:DI 1 "comparison_operator"
             [(ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r,r"))
              (match_operand:DI 3 "register_s32_operand" "r,B32")]))
       (use (match_operand:DI 5 "register_operand" "r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (match_op_dup 1 [(ANY_EXTEND:DI (match_dup 2)) (match_dup 3)]))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_extend2_cstoresi4"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (match_operator:DI 1 "comparison_operator"
         [(ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r,r"))
          (match_operand:DI 3 "register_s32_operand" "r,B32")]))
   (use (match_operand:DI 4 "register_operand" "r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 1 [(ANY_EXTEND:DI (match_dup 2)) (match_dup 3)]))]
)


(define_insn_and_split "*cond_exec_extend3_cstoresi4"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r")
            (match_operator:DI 1 "comparison_operator"
              [(match_operand:DI 2 "register_operand" "r")]))
               (ANY_EXTEND:DI (match_operand:SI 3 "register_operand" "r"))
       (use (match_operand:DI 6 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6)
        (match_op_dup 1 [(match_dup 2) (ANY_EXTEND:DI (match_dup 3))]))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*cond_exec_extend3_cstoresi4.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r")
            (match_operator:DI 1 "comparison_operator"
              [(match_operand:DI 2 "register_operand" "r")]))
               (ANY_EXTEND:DI (match_operand:SI 3 "register_operand" "r"))
       (use (match_operand:DI 5 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (match_op_dup 1 [(match_dup 2) (ANY_EXTEND:DI (match_dup 3))]))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_extend3_cstoresi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (match_operator:DI 1 "comparison_operator"
          [(match_operand:DI 2 "register_operand" "r")]))
           (ANY_EXTEND:DI (match_operand:SI 3 "register_operand" "r"))
   (use (match_operand:DI 4 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 1 [(match_dup 2) (ANY_EXTEND:DI (match_dup 3))]))]
)


;; COND_EXEC WI SHIFT

(define_insn_and_split "*cond_exec_shift_<WI:mode>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:WI 0 "register_operand" "=r")
            (match_operator:WI 3 "pred_shift_operator"
             [(match_operand:WI 1 "register_operand" "r")
              (match_operand:SI 2 "reg_shift_operand" "rU06")]))
       (use (match_operand:WI 6 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*cond_exec_shift_<WI:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:WI 0 "register_operand" "=r")
            (match_operator:WI 3 "pred_shift_operator"
             [(match_operand:WI 1 "register_operand" "r")
              (match_operand:SI 2 "reg_shift_operand" "rU06")]))
       (use (match_operand:WI 5 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_shift_<WI:mode>"
  [(set (match_operand:WI 0 "register_operand" "=r")
        (match_operator:WI 3 "pred_shift_operator"
         [(match_operand:WI 1 "register_operand" "r")
          (match_operand:SI 2 "reg_shift_operand" "rU06")]))
   (use (match_operand:WI 4 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))]
)



;; COND_EXEC ZX SHIFT

(define_insn_and_split "*cond_exec_shift_zx<SHORT:mode>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r")
            (zero_extend:DI (match_operator:SHORT 3 "pred_shift_operator"
                             [(match_operand:SHORT 1 "register_operand" "r")
                              (match_operand:SI 2 "reg_shift_operand" "rU06")])))
       (use (match_operand:DI 6 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6)
        (zero_extend:DI (match_op_dup 3 [(match_dup 1) (match_dup 2)])))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*cond_exec_shift_zx<SHORT:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r")
            (zero_extend:DI (match_operator:SHORT 3 "pred_shift_operator"
                             [(match_operand:SHORT 1 "register_operand" "r")
                              (match_operand:SI 2 "reg_shift_operand" "rU06")])))
       (use (match_operand:DI 5 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (zero_extend:DI (match_op_dup 3 [(match_dup 1) (match_dup 2)])))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_shift_zx<SHORT:mode>"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (match_operator:SHORT 3 "pred_shift_operator"
                         [(match_operand:SHORT 1 "register_operand" "r")
                          (match_operand:SI 2 "reg_shift_operand" "rU06")])))
   (use (match_operand:DI 4 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (zero_extend:DI (match_op_dup 3 [(match_dup 1) (match_dup 2)])))]
)


;; COND_EXEC WI BINARITH

(define_insn_and_split "*cond_exec_binary_<WI:mode>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r,r,r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:WI 0 "register_operand" "=r,r,r,r")
            (match_operator:WI 3 "pred_binarith_operator"
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

(define_insn_and_split "*cond_exec_binary_<WI:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r,r,r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:WI 0 "register_operand" "=r,r,r,r")
            (match_operator:WI 3 "pred_binarith_operator"
             [(match_operand:WI 1 "register_operand" "r,r,r,r")
              (match_operand:WI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")]))
       (use (match_operand:WI 5 "register_operand" "r,r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_binary_<WI:mode>"
  [(set (match_operand:WI 0 "register_operand" "=r,r,r,r")
        (match_operator:WI 3 "pred_binarith_operator"
         [(match_operand:WI 1 "register_operand" "r,r,r,r")
          (match_operand:WI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")]))
   (use (match_operand:WI 4 "register_operand" "r,r,r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))]
)


;; COND_EXEC ZX BINARITH

(define_insn_and_split "*cond_exec_binary_zx<SHORT:mode>di3"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r,r,r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
            (zero_extend:DI (match_operator:SHORT 3 "pred_binarith_operator"
                             [(match_operand:SHORT 1 "register_operand" "r,r,r,r")
                              (match_operand:SHORT 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")])))
       (use (match_operand:DI 6 "register_operand" "r,r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6)
        (zero_extend:DI (match_op_dup 3 [(match_dup 1) (match_dup 2)])))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*cond_exec_binary_zx<SHORT:mode>di3.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r,r,r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
            (zero_extend:DI (match_operator:SHORT 3 "pred_binarith_operator"
                             [(match_operand:SHORT 1 "register_operand" "r,r,r,r")
                              (match_operand:SHORT 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")])))
       (use (match_operand:DI 5 "register_operand" "r,r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (zero_extend:DI (match_op_dup 3 [(match_dup 1) (match_dup 2)])))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_binary_zx<SHORT:mode>di3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (zero_extend:DI (match_operator:SHORT 3 "pred_binarith_operator"
                         [(match_operand:SHORT 1 "register_operand" "r,r,r,r")
                          (match_operand:SHORT 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")])))
   (use (match_operand:DI 4 "register_operand" "r,r,r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (zero_extend:DI (match_op_dup 3 [(match_dup 1) (match_dup 2)])))]
)


;; COND_EXEC *WD BINARITH

(define_insn_and_split "*cond_exec_binary_sidi"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r,r")
            (match_operator:DI 3 "pred_binarith_operator"
             [(ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r,r"))
              (match_operand:DI 2 "register_s32_operand" "r,B32")]))
       (use (match_operand:DI 6 "register_operand" "r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6)
        (match_op_dup 3 [(ANY_EXTEND:DI (match_dup 1)) (match_dup 2)]))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*cond_exec_binary_sidi.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r,r")
            (match_operator:DI 3 "pred_binarith_operator"
             [(ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r,r"))
              (match_operand:DI 2 "register_s32_operand" "r,B32")]))
       (use (match_operand:DI 5 "register_operand" "r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (match_op_dup 3 [(ANY_EXTEND:DI (match_dup 1)) (match_dup 2)]))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_binary_sidi"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (match_operator:DI 3 "pred_binarith_operator"
         [(ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r,r"))
          (match_operand:DI 2 "register_s32_operand" "r,B32")]))
   (use (match_operand:DI 4 "register_operand" "r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 3 [(ANY_EXTEND:DI (match_dup 1)) (match_dup 2)]))]
)


;; COND_EXEC WF BINARITH

(define_insn_and_split "*cond_exec_binary_<WF:mode>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:WF 0 "register_operand" "=r")
            (match_operator:WF 3 "pred_binarith_operator"
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

(define_insn_and_split "*cond_exec_binary_<WF:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:WF 0 "register_operand" "=r")
            (match_operator:WF 3 "pred_binarith_operator"
             [(match_operand:WF 1 "register_operand" "r")
              (match_operand:WF 2 "register_operand" "r")]))
       (use (match_operand:WF 5 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_binary_<WF:mode>"
  [(set (match_operand:WF 0 "register_operand" "=r")
        (match_operator:WF 3 "pred_binarith_operator"
         [(match_operand:WF 1 "register_operand" "r")
          (match_operand:WF 2 "register_operand" "r")]))
   (use (match_operand:WF 4 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))]
)


;; COND_EXEC WI MULTIPLY

(define_insn_and_split "*cond_exec_multiply_<WI:mode>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:WI 0 "register_operand" "=r,r")
            (match_operator:WI 3 "pred_multiply_operator"
             [(match_operand:WI 1 "register_operand" "r,r")
              (match_operand:WI 2 "register_s32_operand" "r,B32")]))
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

(define_insn_and_split "*cond_exec_multiply_<WI:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:WI 0 "register_operand" "=r,r")
            (match_operator:WI 3 "pred_multiply_operator"
             [(match_operand:WI 1 "register_operand" "r,r")
              (match_operand:WI 2 "register_s32_operand" "r,B32")]))
       (use (match_operand:WI 5 "register_operand" "r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_multiply_<WI:mode>"
  [(set (match_operand:WI 0 "register_operand" "=r,r")
        (match_operator:WI 3 "pred_multiply_operator"
         [(match_operand:WI 1 "register_operand" "r,r")
          (match_operand:WI 2 "register_s32_operand" "r,B32")]))
   (use (match_operand:WI 4 "register_operand" "r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))]
)


;; COND_EXEC DISI MADD

(define_insn_and_split "*cond_exec_madd_<DISI:mode>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DISI 0 "register_operand" "=r")
            (plus:DISI (mult:DISI (match_operand:DISI 1 "register_operand" "r")
                                  (match_operand:DISI 2 "register_operand" "r"))
                       (match_operand:DISI 3 "register_operand" "0")))
       (use (match_operand:DISI 6 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6) (match_dup 3))
   (set (match_dup 6)
        (plus:DISI (mult:DISI (match_dup 1) (match_dup 2))
                   (match_dup 6)))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*cond_exec_madd_<DISI:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:DISI 0 "register_operand" "=r")
            (plus:DISI (mult:DISI (match_operand:DISI 1 "register_operand" "r")
                                  (match_operand:DISI 2 "register_operand" "r"))
                       (match_operand:DISI 3 "register_operand" "0")))
       (use (match_operand:DISI 5 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5) (match_dup 3))
   (set (match_dup 5)
        (plus:DISI (mult:DISI (match_dup 1) (match_dup 2))
                   (match_dup 5)))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_madd_<DISI:mode>"
  [(set (match_operand:DISI 0 "register_operand" "=r")
        (plus:DISI (mult:DISI (match_operand:DISI 1 "register_operand" "r")
                              (match_operand:DISI 2 "register_operand" "r"))
                   (match_operand:DISI 3 "register_operand" "0")))
   (use (match_operand:DISI 4 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (plus:DISI (mult:DISI (match_dup 1) (match_dup 2))
                   (match_dup 3)))]
)


;; COND_EXEC DISI MSBF

(define_insn_and_split "*cond_exec_msbf_<DISI:mode>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DISI 0 "register_operand" "=r")
            (minus:DISI (match_operand:DISI 3 "register_operand" "0")
                        (mult:DISI (match_operand:DISI 1 "register_operand" "r")
                                   (match_operand:DISI 2 "register_operand" "r"))))
       (use (match_operand:DISI 6 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6) (match_dup 3))
   (set (match_dup 6)
        (minus:DISI (match_dup 6)
                    (mult:DISI (match_dup 1) (match_dup 2))))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*cond_exec_msbf_<DISI:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:DISI 0 "register_operand" "=r")
            (minus:DISI (match_operand:DISI 3 "register_operand" "0")
                        (mult:DISI (match_operand:DISI 1 "register_operand" "r")
                                   (match_operand:DISI 2 "register_operand" "r"))))
       (use (match_operand:DISI 5 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5) (match_dup 3))
   (set (match_dup 5)
        (minus:DISI (match_dup 5)
                    (mult:DISI (match_dup 1) (match_dup 2))))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_msbf_<DISI:mode>"
  [(set (match_operand:DISI 0 "register_operand" "=r")
        (minus:DISI (match_operand:DISI 3 "register_operand" "0")
                    (mult:DISI (match_operand:DISI 1 "register_operand" "r")
                               (match_operand:DISI 2 "register_operand" "r"))))
   (use (match_operand:DISI 4 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (minus:DISI (match_dup 3)
                    (mult:DISI (match_dup 1) (match_dup 2))))]
)


;; COND_EXEC WF MULTIPLY

(define_insn_and_split "*cond_exec_multiply_<WF:mode>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:WF 0 "register_operand" "=r")
            (match_operator:WF 3 "pred_multiply_operator"
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

(define_insn_and_split "*cond_exec_multiply_<WF:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:WF 0 "register_operand" "=r")
            (match_operator:WF 3 "pred_multiply_operator"
             [(match_operand:WF 1 "register_operand" "r")
              (match_operand:WF 2 "register_operand" "r")]))
       (use (match_operand:WF 5 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_multiply_<WF:mode>"
  [(set (match_operand:WF 0 "register_operand" "=r")
        (match_operator:WF 3 "pred_multiply_operator"
         [(match_operand:WF 1 "register_operand" "r")
          (match_operand:WF 2 "register_operand" "r")]))
   (use (match_operand:WF 4 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (match_op_dup 3 [(match_dup 1) (match_dup 2)]))]
)


;; COND_EXEC WF FMA

(define_insn_and_split "*cond_exec_fma_<WF:mode>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:WF 0 "register_operand" "=r")
            (fma:WF (match_operand:WF 1 "register_operand" "r")
                    (match_operand:WF 2 "register_operand" "r")
                    (match_operand:WF 3 "register_operand" "0")))
       (use (match_operand:WF 6 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6) (match_dup 3))
   (set (match_dup 6)
        (fma:WF (match_dup 1) (match_dup 2) (match_dup 6)))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*cond_exec_fma_<WF:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:WF 0 "register_operand" "=r")
            (fma:WF (match_operand:WF 1 "register_operand" "r")
                    (match_operand:WF 2 "register_operand" "r")
                    (match_operand:WF 3 "register_operand" "0")))
       (use (match_operand:WF 5 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5) (match_dup 3))
   (set (match_dup 5)
        (fma:WF (match_dup 1) (match_dup 2) (match_dup 5)))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_fma_<WF:mode>"
  [(set (match_operand:WF 0 "register_operand" "=r")
        (fma:WF (match_operand:WF 1 "register_operand" "r")
                (match_operand:WF 2 "register_operand" "r")
                (match_operand:WF 3 "register_operand" "0")))
   (use (match_operand:WF 4 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (fma:WF (match_dup 1) (match_dup 2) (match_dup 3)))]
)


;; COND_EXEC WF FNMA

(define_insn_and_split "*cond_exec_fnma_<WF:mode>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:SIDI 4 "register_operand" "r")
       (const_int 0)])
     (parallel
      [(set (match_operand:WF 0 "register_operand" "=r")
            (fma:WF (neg:WF (match_operand:WF 1 "register_operand" "r"))
                    (match_operand:WF 2 "register_operand" "r")
                    (match_operand:WF 3 "register_operand" "0")))
       (use (match_operand:WF 6 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6) (match_dup 3))
   (set (match_dup 6)
        (fma:WF (neg:WF (match_dup 1)) (match_dup 2) (match_dup 6)))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*cond_exec_fnma_<WF:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 4 "register_operand" "r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:WF 0 "register_operand" "=r")
            (fma:WF (neg:WF (match_operand:WF 1 "register_operand" "r"))
                    (match_operand:WF 2 "register_operand" "r")
                    (match_operand:WF 3 "register_operand" "0")))
       (use (match_operand:WF 5 "register_operand" "r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 5) (match_dup 3))
   (set (match_dup 5)
        (fma:WF (neg:WF (match_dup 1)) (match_dup 2) (match_dup 5)))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 4) (const_int 1) (const_int 0)) (const_int 0))
     (set (match_dup 0) (match_dup 5)))]
)

(define_insn_and_split "*wrapped_fnma_<WF:mode>"
  [(set (match_operand:WF 0 "register_operand" "=r")
        (fma:WF (neg:WF (match_operand:WF 1 "register_operand" "r"))
                (match_operand:WF 2 "register_operand" "r")
                (match_operand:WF 3 "register_operand" "0")))
   (use (match_operand:WF 4 "register_operand" "r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (fma:WF (neg:WF (match_dup 1)) (match_dup 2) (match_dup 3)))]
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

