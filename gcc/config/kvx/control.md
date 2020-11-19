;; CSTORE

(define_insn "cstoresi4"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (match_operator:SI   1 "comparison_operator"
         [(match_operand:SI 2 "register_operand" "r,r")
          (match_operand:SI 3 "kvx_r_any32_operand" "r,i")]))]
  ""
  "compw.%1 %0 = %2, %3"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length"      "4,         8")]
)

(define_insn "cstoredi4"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,i")
        (match_operator:DI 1 "comparison_operator"
         [(match_operand:DI 2 "register_operand" "r,r,r,i")
          (match_operand:DI 3 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")]))]
  ""
  "compd.%1 %0 = %2, %3"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length"      "4,       4,         8,        12")]
)

(define_insn "*fcomp<cfx>"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (match_operator:DI 1 "float_comparison_operator"
         [(match_operand:ALLF 2 "register_operand" "r,r")
          (match_operand:ALLF 3 "register_f32_operand" "r,H32")]))]
  ""
  "fcomp<cfx>.%f1 %0 = %2, %3"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length"      "4,         8")]
)

(define_expand "cstore<mode>4" 
  [(set (match_operand:DI 0 "register_operand" "")
        (match_operator:DI 1 "comparison_operator"
         [(match_operand:ALLF 2 "register_operand" "")
          (match_operand:ALLF 3 "nonmemory_operand" "")]))]
  ""
  {
    enum rtx_code cmp_code = GET_CODE (operands[1]);
    kvx_lower_comparison (operands[0], cmp_code, operands[2], operands[3]);
    DONE;
  }
)


;; CBRANCH

(define_expand "cbranch<mode>4"
  [(set (pc)
        (if_then_else (match_operator       0 "comparison_operator"
                       [(match_operand:CBRANCH 1 "register_operand")
                        (match_operand:CBRANCH 2 "nonmemory_operand")])
                      (label_ref (match_operand 3 ""))
                      (pc)))]
  ""
  {
    enum rtx_code cmp_code = GET_CODE (operands[0]);
    rtx pred = kvx_lower_comparison (0, cmp_code, operands[1], operands[2]);
    if (pred)
      {
        PUT_CODE (operands[0], NE);
        operands[1] = pred;
        operands[2] = const0_rtx;
      }
    else if (GET_MODE_CLASS (<MODE>mode) == MODE_FLOAT)
     {
        gcc_unreachable ();
     }
  }
)


;; CB

(define_insn "*cb<mode>"
  [(set (pc)
        (if_then_else (match_operator 0 "zero_comparison_operator"
                                      [(match_operand:SIDI 1 "register_operand" "r")
                                       (const_int 0)])
                      (label_ref (match_operand 2 "" ""))
                      (pc)))]
  ""
  "cb.<SIDI:suffix>%0z %1? %2"
  [(set_attr "type" "bcu")]
)

(define_insn "*cb<mode>.odd"
  [(set (pc)
        (if_then_else (ne (zero_extract:SIDI (match_operand:SIDI 0 "register_operand" "r")
                                             (const_int 1) (const_int 0))
                          (const_int 0))
                      (label_ref (match_operand 1))
                      (pc)))]
  ""
  "cb.odd %0? %1"
  [(set_attr "type" "bcu")]
)

(define_insn "*cb<mode>.even"
  [(set (pc)
        (if_then_else (eq (zero_extract:SIDI (match_operand:SIDI 0 "register_operand" "r")
                                             (const_int 1) (const_int 0))
                          (const_int 0))
                      (label_ref (match_operand 1))
                      (pc)))]
  ""
  "cb.even %0? %1"
  [(set_attr "type" "bcu")]
)


;; CMOVED

(define_insn "*cmov<mode>.<FITGPR:mode>"
  [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
        (if_then_else:FITGPR (match_operator 2 "zero_comparison_operator"
                                               [(match_operand:SIDI 3 "register_operand" "r,r,r,r")
                                                (const_int 0)])
                             (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                             (match_operand:FITGPR 4 "register_operand" "0,0,0,0")))]
  ""
  "cmoved.<SIDI:suffix>%2z %3? %0 = %1"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
   (set_attr "length"      "4,       4,         8,        12")]
)

(define_insn "*cmov<mode>.<FITGPR:mode>.odd"
  [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
        (if_then_else:FITGPR (ne (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r,r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                             (match_operand:FITGPR 3 "register_operand" "0,0,0,0")))]
  ""
  "cmoved.odd %2? %0 = %1"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
   (set_attr "length"      "4,       4,         8,        12")]
)

(define_insn "*cmov<mode>.<FITGPR:mode>.even"
  [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
        (if_then_else:FITGPR (eq (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r,r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                             (match_operand:FITGPR 3 "register_operand" "0,0,0,0")))]
  ""
  "cmoved.even %2? %0 = %1"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
   (set_attr "length"      "4,       4,         8,        12")]
)

(define_insn "*cmov<mode>.<ALL128:mode>"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (if_then_else:ALL128 (match_operator 2 "zero_comparison_operator"
                                               [(match_operand:SIDI 3 "register_operand" "r")
                                                (const_int 0)])
                             (match_operand:ALL128 1 "register_operand" "r")
                             (match_operand:ALL128 4 "register_operand" "0")))]
  ""
  "cmoved.<SIDI:suffix>%2z %3? %x0 = %x1\n\tcmoved.<SIDI:suffix>%2z %3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*cmov<mode>.<ALL128:mode>.odd"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (if_then_else:ALL128 (ne (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:ALL128 1 "register_operand" "r")
                             (match_operand:ALL128 3 "register_operand" "0")))]
  ""
  "cmoved.odd %2? %x0 = %x1\n\tcmoved.odd %2? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*cmov<mode>.<ALL128:mode>.even"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (if_then_else:ALL128 (eq (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:ALL128 1 "register_operand" "r")
                             (match_operand:ALL128 3 "register_operand" "0")))]
  ""
  "cmoved.even %2? %x0 = %x1\n\tcmoved.even %2? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*cmov<mode>.<ALL256:mode>"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (if_then_else:ALL256 (match_operator 2 "zero_comparison_operator"
                                                [(match_operand:SIDI 3 "register_operand" "r")
                                                 (const_int 0)])
                             (match_operand:ALL256 1 "register_operand" "r")
                             (match_operand:ALL256 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:TI (match_dup 0) 0)
        (if_then_else:TI (match_op_dup 2 [(match_dup 3) (const_int 0)])
                         (subreg:TI (match_dup 1) 0)
                         (subreg:TI (match_dup 4) 0)))
   (set (subreg:TI (match_dup 0) 16)
        (if_then_else:TI (match_op_dup 2 [(match_dup 3) (const_int 0)])
                         (subreg:TI (match_dup 1) 16)
                         (subreg:TI (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*cmov<mode>.<ALL256:mode>.odd"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (if_then_else:ALL256 (ne (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:ALL256 1 "register_operand" "r")
                             (match_operand:ALL256 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:TI (match_dup 0) 0)
        (if_then_else:TI (ne (zero_extract:SIDI (match_dup 2)
                                                (const_int 1) (const_int 0))
                             (const_int 0))
                         (subreg:TI (match_dup 1) 0)
                         (subreg:TI (match_dup 3) 0)))
   (set (subreg:TI (match_dup 0) 16)
        (if_then_else:TI (ne (zero_extract:SIDI (match_dup 2)
                                                (const_int 1) (const_int 0))
                             (const_int 0))
                         (subreg:TI (match_dup 1) 16)
                         (subreg:TI (match_dup 3) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*cmov<mode>.<ALL256:mode>.even"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (if_then_else:ALL256 (eq (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:ALL256 1 "register_operand" "r")
                             (match_operand:ALL256 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:TI (match_dup 0) 0)
        (if_then_else:TI (eq (zero_extract:SIDI (match_dup 2)
                                                (const_int 1) (const_int 0))
                             (const_int 0))
                         (subreg:TI (match_dup 1) 0)
                         (subreg:TI (match_dup 3) 0)))
   (set (subreg:TI (match_dup 0) 16)
        (if_then_else:TI (eq (zero_extract:SIDI (match_dup 2)
                                                (const_int 1) (const_int 0))
                             (const_int 0))
                         (subreg:TI (match_dup 1) 16)
                         (subreg:TI (match_dup 3) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)


;; MOV*CC

(define_expand "mov<mode>cc"
  [(set (match_operand:ALLIFV 0 "register_operand" "")
        (if_then_else:ALLIFV (match_operand 1 "comparison_operator" "")
                             (match_operand:ALLIFV 2 "nonmemory_operand" "")
                             (match_operand:ALLIFV 3 "nonmemory_operand" "")))]
  ""
  {
    rtx target = operands[0];
    rtx select1 = operands[2];
    rtx select2 = operands[3];
    rtx cmp = operands[1];
    rtx left = XEXP (cmp, 0);
    rtx right = XEXP (cmp, 1);
    kvx_expand_conditional_move (target, select1, select2, cmp, left, right);
    DONE;
  }
)


