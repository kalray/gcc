;; CB

(define_insn "*cb<mode>"
  [(set (pc)
        (if_then_else (match_operator 0 "signed_comparison_operator"
                                      [(match_operand:SIDI 1 "register_operand" "r")
                                       (const_int 0)])
                      (label_ref (match_operand 2 "" ""))
                      (pc)))]
  ""
  "cb.<SIDI:cbvar>%0z %1? %2"
  [(set_attr "type" "bcu")
   (set_attr "class" "branch")]
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
  [(set_attr "type" "bcu")
   (set_attr "class" "branch")]
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
  [(set_attr "type" "bcu")
   (set_attr "class" "branch")]
)


;; CMOVED

(define_insn "*cmov<mode>.<FITGPR:mode>"
  [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
        (if_then_else:FITGPR (match_operator 2 "signed_comparison_operator"
                                               [(match_operand:SIDI 3 "register_operand" "r,r,r,r")
                                                (const_int 0)])
                             (match_operand:FITGPR 1 "k1_r_s10_s37_s64_operand" "r,I10,B37,i")
                             (match_operand:FITGPR 4 "register_operand" "0,0,0,0")))]
  ""
  "cmoved.<SIDI:cbvar>%2z %3? %0 = %1"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
   (set_attr "length"      "4,       4,         8,        12")]
)

(define_insn "*cmov<mode>.<FITGPR:mode>.odd"
  [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
        (if_then_else:FITGPR (ne (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r,r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:FITGPR 1 "k1_r_s10_s37_s64_operand" "r,I10,B37,i")
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
                             (match_operand:FITGPR 1 "k1_r_s10_s37_s64_operand" "r,I10,B37,i")
                             (match_operand:FITGPR 3 "register_operand" "0,0,0,0")))]
  ""
  "cmoved.even %2? %0 = %1"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
   (set_attr "length"      "4,       4,         8,        12")]
)

(define_insn "*cmov<mode>.<ALL128:mode>"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (if_then_else:ALL128 (match_operator 2 "signed_comparison_operator"
                                               [(match_operand:SIDI 3 "register_operand" "r")
                                                (const_int 0)])
                             (match_operand:ALL128 1 "register_operand" "r")
                             (match_operand:ALL128 4 "register_operand" "0")))]
  ""
  "cmoved.<SIDI:cbvar>%2z %3? %x0 = %x1\n\tcmoved.<SIDI:cbvar>%2z %3? %y0 = %y1"
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
        (if_then_else:ALL256 (match_operator 2 "signed_comparison_operator"
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
    if (!k1_expand_conditional_move (<MODE>mode, operands))
      FAIL;
    DONE;
  }
)


