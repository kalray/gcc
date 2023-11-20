;; HI

(define_insn "bswaphi2"
  [(set (match_operand:HI 0 "register_operand" "=r")
        (bswap:HI (match_operand:HI 1 "register_operand" "r")))]
  ""
  "sbmm8 %0 = %1, 0x0102"
  [(set_attr "type" "alu_thin_x")
   (set_attr "length"        "8")]
)
;; zero-extend version of bswapsi2
(define_insn "*bswapsi2_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (bswap:HI (match_operand:HI 1 "register_operand" "r"))))]
  ""
  "sbmm8 %0 = %1, 0x0102"
  [(set_attr "type" "alu_thin_x")
   (set_attr "length"        "8")]
)

;; SIDI (SI/DI)

(define_expand "usadd<mode>3"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
   (match_operand:SIDI 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usadd<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_usadd<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usadd<mode>3_1"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (us_plus:SIDI (match_operand:SIDI 1 "register_operand" "r")
                      (match_operand:SIDI 2 "register_operand" "r")))
   (clobber (match_scratch:SIDI 3 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 0)
        (plus:SIDI (match_dup 1) (match_dup 2)))
   (set (match_dup 3)
        (ltu:SIDI (match_dup 0) (match_dup 1)))
   (set (match_dup 0)
        (if_then_else:SIDI
            (ne (match_dup 3) (const_int 0))
            (const_int -1)
            (match_dup 0)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "usadd<mode>3_2"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (us_plus:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                      (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  "(KV3_2||KV4)"
  "addus<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length"      "4,         8")]
)

(define_expand "ussub<mode>3"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
   (match_operand:SIDI 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_ussub<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_ussub<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "ussub<mode>3_1"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (us_minus:SIDI (match_operand:SIDI 1 "register_operand" "r")
                       (match_operand:SIDI 2 "register_operand" "r")))
   (clobber (match_scratch:SIDI 3 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (umin:SIDI (match_dup 1) (match_dup 2)))
   (set (match_dup 0)
        (minus:SIDI (match_dup 1) (match_dup 3)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "ussub<mode>3_2"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (us_minus:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                       (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  "(KV3_2||KV4)"
  "sbfus<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length"      "4,         8")]
)

(define_insn "ashl<mode>3"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (ashift:SIDI (match_operand:SIDI 1 "register_operand" "r")
                     (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "sll<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "ssashl<mode>3"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (ss_ashift:SIDI (match_operand:SIDI 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "sls<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_expand "usashl<mode>3"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
   (match_operand:SI 2 "reg_shift_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usashl<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_usashl<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usashl<mode>3_1"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (us_ashift:SIDI (match_operand:SIDI 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))
   (clobber (match_scratch:SIDI 3 "=&r"))
   (clobber (match_scratch:SIDI 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 0)
        (ashift:SIDI (match_dup 1) (match_dup 2)))
   (set (match_dup 3)
        (lshiftrt:SIDI (match_dup 0) (match_dup 2)))
   (set (match_dup 4)
        (ne:SIDI (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (if_then_else:SIDI
            (ne (match_dup 4) (const_int 0))
            (const_int -1)
            (match_dup 0)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "usashl<mode>3_2"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (us_ashift:SIDI (match_operand:SIDI 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  "(KV3_2||KV4)"
  "slus<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "ashr<mode>3"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (ashiftrt:SIDI (match_operand:SIDI 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "sra<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "lshr<mode>3"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (lshiftrt:SIDI (match_operand:SIDI 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "srl<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "sshr<mode>3"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (unspec:SIDI [(match_operand:SIDI 1 "register_operand" "r")
                      (match_operand:SI 2 "reg_shift_operand" "rU06")] UNSPEC_SRS))]
  ""
  "srs<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "negdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (neg:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "negd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "negsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (neg:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "negw %0 = %1"
  [(set_attr "type" "alu_tiny_w")]
)

(define_insn "neghi2"
  [(set (match_operand:HI 0 "register_operand" "=r")
        (neg:HI (match_operand:HI 1 "register_operand" "r")))]
  ""
  "neghq %0 = %1"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length" "8")]
)

(define_insn "negqi2"
  [(set (match_operand:QI 0 "register_operand" "=r")
        (neg:QI (match_operand:QI 1 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "negbo %0 = %1"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length" "8")]
)

(define_insn "ssneg<mode>2"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (ss_neg:SIDI (match_operand:SIDI 1 "register_operand" "r")))]
  ""
  "sbfs<suffix> %0 = %1, 0"
  [(set_attr "type" "alu_thin_x")
   (set_attr "length"        "8")]
)

(define_insn "absdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (abs:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "absd %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "abssi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (abs:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "absw %0 = %1"
  [(set_attr "type" "alu_thin_w")]
)

(define_expand "ssabs<mode>2"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_ssabs<mode>2_1 (operands[0], operands[1]));
    else if ((KV3_2||KV4))
      emit_insn (gen_ssabs<mode>2_2 (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "ssabs<mode>2_1"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (ss_abs:SIDI (match_operand:SIDI 1 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 0)
        (ss_neg:SIDI (match_dup 1)))
   (set (match_dup 0)
        (abs:SIDI (match_dup 0)))]
  ""
)

(define_insn "ssabs<mode>2_2"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (ss_abs:SIDI (match_operand:SIDI 1 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "abss<suffix> %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_expand "abd<mode>3"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
   (match_operand:SIDI 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_abd<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_abd<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "abdsi3_1"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (minus:SI (smax:SI (match_operand:SI 1 "register_operand" "r,r,r")
                           (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))
                  (smin:SI (match_dup 1) (match_dup 2))))]
  "KV3_1"
  "abdw %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_w,alu_thin_x")
   (set_attr "length" "4,4,8")]
)

(define_insn "abddi3_1"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (minus:DI (smax:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                           (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i"))
                  (smin:DI (match_dup 1) (match_dup 2))))]
  "KV3_1"
  "abdd %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin,alu_thin_x,alu_thin_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "abd<mode>3_2"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (minus:SIDI (smax:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                               (match_operand:SIDI 2 "register_s32_operand" "r,B32"))
                    (smin:SIDI (match_dup 1) (match_dup 2))))]
  "(KV3_2||KV4)"
  "abd<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length"      "4,         8")]
)
;; zero-extend version of abdsi3
(define_insn "*abdsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (minus:SI (smax:SI (match_operand:SI 1 "register_operand" "r,r")
                                           (match_operand:SI 2 "register_s32_operand" "r,B32"))
                                  (smin:SI (match_dup 1) (match_dup 2)))))]
  "(KV3_2||KV4)"
  "abdw %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length"      "4,         8")]
)

(define_expand "abds<mode>3"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
   (match_operand:SIDI 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_abds<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_abds<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "abds<mode>3_1"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (ss_minus:SIDI (smax:SIDI (match_operand:SIDI 1 "register_operand" "r")
                                  (match_operand:SIDI 2 "register_operand" "r"))
                       (smin:SIDI (match_dup 1) (match_dup 2))))
   (clobber (match_scratch:SIDI 3 "=&r"))
   (clobber (match_scratch:SIDI 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (smax:SIDI (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (smin:SIDI (match_dup 1) (match_dup 2)))
   (set (match_dup 0)
        (ss_minus:SIDI (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "abds<mode>3_2"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (ss_minus:SIDI (smax:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                                  (match_operand:SIDI 2 "register_s32_operand" "r,B32"))
                       (smin:SIDI (match_dup 1) (match_dup 2))))]
  "(KV3_2||KV4)"
  "abds<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length"      "4,         8")]
)
;; zero-extend version of abdssi3
(define_insn "*abdssi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (ss_minus:SI (smax:SI (match_operand:SI 1 "register_operand" "r,r")
                                              (match_operand:SI 2 "register_s32_operand" "r,B32"))
                                     (smin:SI (match_dup 1) (match_dup 2)))))]
  "(KV3_2||KV4)"
  "abdsw %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length"      "4,         8")]
)

(define_expand "abdu<mode>3"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
   (match_operand:SIDI 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_abdu<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_abdu<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "abdu<mode>3_1"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (minus:SIDI (umax:SIDI (match_operand:SIDI 1 "register_operand" "r")
                               (match_operand:SIDI 2 "register_operand" "r"))
                    (umin:SIDI (match_dup 1) (match_dup 2))))
   (clobber (match_scratch:SIDI 3 "=&r"))
   (clobber (match_scratch:SIDI 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (umax:SIDI (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (umin:SIDI (match_dup 1) (match_dup 2)))
   (set (match_dup 0)
        (minus:SIDI (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "abdu<mode>3_2"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (minus:SIDI (umax:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                               (match_operand:SIDI 2 "register_s32_operand" "r,B32"))
                    (umin:SIDI (match_dup 1) (match_dup 2))))]
  "(KV3_2||KV4)"
  "abdu<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length"      "4,         8")]
)
;; zero-extend version of abdusi3
(define_insn "*abdusi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (minus:SI (umax:SI (match_operand:SI 1 "register_operand" "r,r")
                                           (match_operand:SI 2 "register_s32_operand" "r,B32"))
                                  (umin:SI (match_dup 1) (match_dup 2)))))]
  "(KV3_2||KV4)"
  "abduw %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length"      "4,         8")]
)

(define_insn "*addx2<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (ashift:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                                (const_int 1))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx2<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
;; multiplicative version of addx2<suffix>
(define_insn "*addx2<suffix>_m"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (mult:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                              (const_int 2))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx2<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *addx2si
(define_insn "*addx2si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (plus:SI (ashift:SI (match_operand:SI 1 "register_operand" "r,r")
                                            (const_int 1))
                                 (match_operand:SI 2 "register_s32_operand" "r,B32"))))]
  ""
  "addx2w %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*addx4<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (ashift:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                                (const_int 2))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx4<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
;; multiplicative version of addx4<suffix>
(define_insn "*addx4<suffix>_m"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (mult:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                              (const_int 4))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx4<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *addx4si
(define_insn "*addx4si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (plus:SI (ashift:SI (match_operand:SI 1 "register_operand" "r,r")
                                            (const_int 2))
                                 (match_operand:SI 2 "register_s32_operand" "r,B32"))))]
  ""
  "addx4w %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*addx8<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (ashift:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                                (const_int 3))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx8<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
;; multiplicative version of addx8<suffix>
(define_insn "*addx8<suffix>_m"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (mult:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                              (const_int 8))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx8<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *addx8si
(define_insn "*addx8si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (plus:SI (ashift:SI (match_operand:SI 1 "register_operand" "r,r")
                                            (const_int 3))
                                 (match_operand:SI 2 "register_s32_operand" "r,B32"))))]
  ""
  "addx8w %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*addx16<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (ashift:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                                (const_int 4))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx16<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
;; multiplicative version of addx16<suffix>
(define_insn "*addx16<suffix>_m"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (mult:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                              (const_int 16))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx16<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *addx16si
(define_insn "*addx16si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (plus:SI (ashift:SI (match_operand:SI 1 "register_operand" "r,r")
                                            (const_int 4))
                                 (match_operand:SI 2 "register_s32_operand" "r,B32"))))]
  ""
  "addx16w %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*addx32<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (ashift:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                                (const_int 5))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  "(KV3_2||KV4)"
  "addx32<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *addx32si
(define_insn "*addx32si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (plus:SI (ashift:SI (match_operand:SI 1 "register_operand" "r,r")
                                            (const_int 5))
                                 (match_operand:SI 2 "register_s32_operand" "r,B32"))))]
  "(KV3_2||KV4)"
  "addx32w %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*addx64<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (ashift:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                                (const_int 6))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  "(KV3_2||KV4)"
  "addx64<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *addx64si
(define_insn "*addx64si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (plus:SI (ashift:SI (match_operand:SI 1 "register_operand" "r,r")
                                            (const_int 6))
                                 (match_operand:SI 2 "register_s32_operand" "r,B32"))))]
  "(KV3_2||KV4)"
  "addx64w %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*addx32<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (ashift:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                                (const_int 5))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  "(KV3_2||KV4)"
  "addx32<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)
;; multiplicative version of addx32<suffix>
(define_insn_and_split "*addx32<suffix>_m"
  [(set (match_operand:SIDI 0 "register_operand" "=&r,&r")
        (plus:SIDI (mult:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                              (const_int 32))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 0) (ashift:SIDI (match_dup 1) (const_int 5)))
   (set (match_dup 0) (plus:SIDI (match_dup 0) (match_dup 2)))]
)
(define_insn "*addx32<suffix>_m"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (mult:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                              (const_int 32))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  "(KV3_2||KV4)"
  "addx32<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *addx32si
(define_insn "*addx32si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (plus:SI (ashift:SI (match_operand:SI 1 "register_operand" "r,r")
                                            (const_int 5))
                                 (match_operand:SI 2 "register_s32_operand" "r,B32"))))]
  "(KV3_2||KV4)"
  "addx32w %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*addx64<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (ashift:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                                (const_int 6))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  "(KV3_2||KV4)"
  "addx64<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)
;; multiplicative version of addx64<suffix>
(define_insn_and_split "*addx64<suffix>_m"
  [(set (match_operand:SIDI 0 "register_operand" "=&r,&r")
        (plus:SIDI (mult:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                              (const_int 64))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 0) (ashift:SIDI (match_dup 1) (const_int 6)))
   (set (match_dup 0) (plus:SIDI (match_dup 0) (match_dup 2)))]
)
(define_insn "*addx64<suffix>_m"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (plus:SIDI (mult:SIDI (match_operand:SIDI 1 "register_operand" "r,r")
                              (const_int 64))
                   (match_operand:SIDI 2 "register_s32_operand" "r,B32")))]
  "(KV3_2||KV4)"
  "addx64<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *addx64si
(define_insn "*addx64si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (plus:SI (ashift:SI (match_operand:SI 1 "register_operand" "r,r")
                                            (const_int 6))
                                 (match_operand:SI 2 "register_s32_operand" "r,B32"))))]
  "(KV3_2||KV4)"
  "addx64w %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx2<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (minus:SIDI (match_operand:SIDI 1 "register_s32_operand" "r,B32")
                    (ashift:SIDI (match_operand:SIDI 2 "register_operand" "r,r")
                                 (const_int 1))))]
  ""
  "sbfx2<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *sbfx2si
(define_insn "*sbfx2si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (minus:SI (match_operand:SI 1 "register_s32_operand" "r,B32")
                                  (ashift:SI (match_operand:SI 2 "register_operand" "r,r")
                                             (const_int 1)))))]
  ""
  "sbfx2w %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx4<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (minus:SIDI (match_operand:SIDI 1 "register_s32_operand" "r,B32")
                    (ashift:SIDI (match_operand:SIDI 2 "register_operand" "r,r")
                                 (const_int 2))))]
  ""
  "sbfx4<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *sbfx4si
(define_insn "*sbfx4si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (minus:SI (match_operand:SI 1 "register_s32_operand" "r,B32")
                                  (ashift:SI (match_operand:SI 2 "register_operand" "r,r")
                                             (const_int 2)))))]
  ""
  "sbfx4w %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx8<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (minus:SIDI (match_operand:SIDI 1 "register_s32_operand" "r,B32")
                    (ashift:SIDI (match_operand:SIDI 2 "register_operand" "r,r")
                                 (const_int 3))))]
  ""
  "sbfx8<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *sbfx8si
(define_insn "*sbfx8si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (minus:SI (match_operand:SI 1 "register_s32_operand" "r,B32")
                                  (ashift:SI (match_operand:SI 2 "register_operand" "r,r")
                                             (const_int 3)))))]
  ""
  "sbfx8w %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx16<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (minus:SIDI (match_operand:SIDI 1 "register_s32_operand" "r,B32")
                    (ashift:SIDI (match_operand:SIDI 2 "register_operand" "r,r")
                                 (const_int 4))))]
  ""
  "sbfx16<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *sbfx16si
(define_insn "*sbfx16si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (minus:SI (match_operand:SI 1 "register_s32_operand" "r,B32")
                                  (ashift:SI (match_operand:SI 2 "register_operand" "r,r")
                                             (const_int 4)))))]
  ""
  "sbfx16w %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx32<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (minus:SIDI (match_operand:SIDI 1 "register_s32_operand" "r,B32")
                    (ashift:SIDI (match_operand:SIDI 2 "register_operand" "r,r")
                                 (const_int 5))))]
  "(KV3_2||KV4)"
  "sbfx32<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *sbfx32si
(define_insn "*sbfx32si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (minus:SI (match_operand:SI 1 "register_s32_operand" "r,B32")
                                  (ashift:SI (match_operand:SI 2 "register_operand" "r,r")
                                             (const_int 5)))))]
  "(KV3_2||KV4)"
  "sbfx32w %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx64<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (minus:SIDI (match_operand:SIDI 1 "register_s32_operand" "r,B32")
                    (ashift:SIDI (match_operand:SIDI 2 "register_operand" "r,r")
                                 (const_int 6))))]
  "(KV3_2||KV4)"
  "sbfx64<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *sbfx64si
(define_insn "*sbfx64si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (minus:SI (match_operand:SI 1 "register_s32_operand" "r,B32")
                                  (ashift:SI (match_operand:SI 2 "register_operand" "r,r")
                                             (const_int 6)))))]
  "(KV3_2||KV4)"
  "sbfx64w %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx32<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (minus:SIDI (match_operand:SIDI 1 "register_s32_operand" "r,B32")
                    (ashift:SIDI (match_operand:SIDI 2 "register_operand" "r,r")
                                 (const_int 5))))]
  "(KV3_2||KV4)"
  "sbfx32<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *sbfx32si
(define_insn "*sbfx32si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (minus:SI (match_operand:SI 1 "register_s32_operand" "r,B32")
                                  (ashift:SI (match_operand:SI 2 "register_operand" "r,r")
                                             (const_int 5)))))]
  "(KV3_2||KV4)"
  "sbfx32w %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx64<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (minus:SIDI (match_operand:SIDI 1 "register_s32_operand" "r,B32")
                    (ashift:SIDI (match_operand:SIDI 2 "register_operand" "r,r")
                                 (const_int 6))))]
  "(KV3_2||KV4)"
  "sbfx64<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)
;; zero-extend version of *sbfx64si
(define_insn "*sbfx64si_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (minus:SI (match_operand:SI 1 "register_s32_operand" "r,B32")
                                  (ashift:SI (match_operand:SI 2 "register_operand" "r,r")
                                             (const_int 6)))))]
  "(KV3_2||KV4)"
  "sbfx64w %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "kvx_land<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (and:SIDI (ne:SIDI (match_operand:SIDI 1 "register_operand" "%r,r") (const_int 0))
                  (ne:SIDI (match_operand:SIDI 2 "register_s32_operand" "r,B32") (const_int 0))))]
  ""
  "land<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "kvx_lnand<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (ior:SIDI (eq:SIDI (match_operand:SIDI 1 "register_operand" "%r,r") (const_int 0))
                  (eq:SIDI (match_operand:SIDI 2 "register_s32_operand" "r,B32") (const_int 0))))]
  ""
  "lnand<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "kvx_lor<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (ne:SIDI (ior:SIDI (match_operand:SIDI 1 "register_operand" "%r,r")
                           (match_operand:SIDI 2 "register_s32_operand" "r,B32"))
                 (const_int 0)))]
  ""
  "lior<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "kvx_lnor<suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (eq:SIDI (ior:SIDI (match_operand:SIDI 1 "register_operand" "%r,r")
                           (match_operand:SIDI 2 "register_s32_operand" "r,B32"))
                 (const_int 0)))]
  ""
  "lnior<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*zxh_and"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (and:SI (match_operand:SI 1 "register_operand" "r")
                (const_int 65535) ))]
  ""
  "zxhd %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "extv<mode>"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extract:SIDI (match_operand:SIDI 1 "register_operand" "r")
                           (match_operand 2 "sixbits_unsigned_operand" "i")
                           (match_operand 3 "sixbits_unsigned_operand" "i")))]
  ""
  "extfs %0 = %1, %2+%3-1, %3"
  [(set_attr "type" "alu_thin")]
)

(define_insn "extzv<mode>"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extract:SIDI (match_operand:SIDI 1 "register_operand" "r")
                           (match_operand 2 "sixbits_unsigned_operand" "i")
                           (match_operand 3 "sixbits_unsigned_operand" "i")))]
  ""
  "extfz %0 = %1, %2+%3-1, %3"
  [(set_attr "type" "alu_thin")]
)

(define_insn "insv<mode>"
  [(set (zero_extract:SIDI (match_operand:SIDI 0 "register_operand" "+r")
                           (match_operand 1 "sixbits_unsigned_operand" "i")
                           (match_operand 2 "sixbits_unsigned_operand" "i"))
        (match_operand:SIDI 3 "register_operand" "r"))]
  ""
  "insf %0 = %3, %1+%2-1, %2"
  [(set_attr "type" "alu_thin")]
)


;; SI

(define_insn "addsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (plus:SI (match_operand:SI 1 "register_operand" "r,r,r")
                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "addw %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,4,8")]
)
;; zero-extend version of addsi3
(define_insn "*addsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (plus:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "addw %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,4,8")]
)

(define_insn "ssaddsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (ss_plus:SI (match_operand:SI 1 "register_operand" "r,r")
                    (match_operand:SI 2 "kvx_r_any32_operand" "r,i")))]
  ""
  "addsw %0 = %1, %2"
  [(set_attr "type"   "alu_thin,alu_thin_x")
   (set_attr "length" "4,       8")]
)
;; zero extend version of ssaddsi3
(define_insn "*ssaddsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (ss_plus:SI (match_operand:SI 1 "register_operand" "r,r")
                                    (match_operand:SI 2 "kvx_r_any32_operand" "r,i"))))]
  ""
  "addsw %0 = %1, %2"
  [(set_attr "type"   "alu_thin,alu_thin_x")
   (set_attr "length" "4,       8")]
)

;; zero extend version of usaddsi3
(define_insn "*usaddsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (us_plus:SI (match_operand:SI 1 "register_operand" "r,r")
                                    (match_operand:SI 2 "kvx_r_any32_operand" "r,i"))))]
  "(KV3_2||KV4)"
  "addusw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       8")]
)

(define_insn "subsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (minus:SI (match_operand:SI 1 "kvx_r_s10_s37_s64_operand" "r,I10,i")
                  (match_operand:SI 2 "register_operand" "r,r,r")))]
  ""
  "sbfw %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,4,8")]
)
;; zero-extend version of subsi3
(define_insn "*subsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (minus:SI (match_operand:SI 1 "kvx_r_s10_s37_s64_operand" "r,I10,i")
                                  (match_operand:SI 2 "register_operand" "r,r,r"))))]
  ""
  "sbfw %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,4,8")]
)

(define_insn "sssubsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (ss_minus:SI (match_operand:SI 1 "kvx_r_any32_operand" "r,i")
                     (match_operand:SI 2 "register_operand" "r,r")))]
  ""
  "sbfsw %0 = %2, %1"
  [(set_attr "type"   "alu_thin,alu_thin_x")
   (set_attr "length" "4,       8")]
)
;; zero extend version of sssubsi3
(define_insn "*sssubsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (ss_minus:SI (match_operand:SI 1 "kvx_r_any32_operand" "r,i")
                                     (match_operand:SI 2 "register_operand" "r,r"))))]
  ""
  "sbfsw %0 = %2, %1"
  [(set_attr "type"   "alu_thin,alu_thin_x")
   (set_attr "length" "4,       8")]
)

;; zero extend version of ussubsi3
(define_insn "*ussubsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (us_minus:SI (match_operand:SI 1 "kvx_r_any32_operand" "r,i")
                                     (match_operand:SI 2 "register_operand" "r,r"))))]
  "(KV3_2||KV4)"
  "sbfusw %0 = %2, %1"
  [(set_attr "type"   "alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       8")]
)

(define_insn "mulsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (mult:SI (match_operand:SI 1 "register_operand" "r,r")
                 (match_operand:SI 2 "kvx_r_any32_operand" "r,i")))]
  ""
  "mulw %0 = %1, %2"
  [(set_attr "type" "mult_int, mult_int_x")
   (set_attr "length"      "4,          8")]
)
;; zero extend version of mulsi3
(define_insn "*mulsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (mult:SI (match_operand:SI 1 "register_operand" "r,r")
                                 (match_operand:SI 2 "kvx_r_any32_operand" "r,i"))))]
  ""
  "mulw %0 = %1, %2"
  [(set_attr "type" "mult_int, mult_int_x")
   (set_attr "length"      "4,          8")]
)

(define_insn "mulsidi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (mult:DI (sign_extend:DI (match_operand:SI 1 "register_operand" "r"))
                 (sign_extend:DI (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "mulwd %0 = %1, %2"
  [(set_attr "type" "mult_int")]
)

(define_insn "umulsidi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
                 (zero_extend:DI (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "muluwd %0 = %1, %2"
  [(set_attr "type" "mult_int")]
)

(define_insn "usmulsidi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
                 (sign_extend:DI (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "mulsuwd %0 = %2, %1"
  [(set_attr "type" "mult_int")]
)

(define_expand "divsi3"
  [(set (match_operand:SI 0 "register_operand" "")
        (div:SI (match_operand:SI 1 "register_operand" "")
                (match_operand 2 "nonmemory_operand" "")))]
  ""
  {
    if (const_pow2lt64_operand (operands[2], VOIDmode))
      {
        HOST_WIDE_INT constant = INTVAL (operands[2]);
        operands[2] = gen_rtx_CONST_INT (VOIDmode, __builtin_ctzll (constant));
        emit_insn (gen_sshrsi3 (operands[0], operands[1], operands[2]));
      }
    else if (optimize_size)
      {
        rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__divsi3"),
                                            operands[0], LCT_CONST, SImode,
                                            operands[1], SImode, operands[2], SImode);
        if (dest != operands[0])
          emit_move_insn (operands[0], dest);
      }
    else
      {
        operands[2] = force_reg (SImode, operands[2]);
        rtx absa = gen_reg_rtx (SImode);
        emit_insn (gen_abssi2 (absa, operands[1]));
        rtx absb = gen_reg_rtx (SImode);
        emit_insn (gen_abssi2 (absb, operands[2]));
        rtx sign = gen_reg_rtx (SImode);
        emit_insn (gen_xorsi3 (sign, operands[1], operands[2]));
        emit_insn (gen_udivsi3 (operands[0], absa, absb));
        rtx negated = gen_reg_rtx (SImode);
        emit_insn (gen_negsi2 (negated, operands[0]));
        rtx select = gen_rtx_IF_THEN_ELSE (SImode, gen_rtx_LT (VOIDmode, sign, const0_rtx),
                                           negated, operands[0]);
        emit_insn (gen_rtx_SET (operands[0], select));
      }
    DONE;
  }
)

(define_expand "modsi3"
  [(set (match_operand:SI 0 "register_operand" "")
        (mod:SI (match_operand:SI 1 "register_operand" "")
                (match_operand 2 "nonmemory_operand" "")))]
  ""
  {
    if (const_pow2lt64_operand (operands[2], VOIDmode))
      {
        HOST_WIDE_INT constant = INTVAL (operands[2]);
        operands[2] = gen_rtx_CONST_INT (VOIDmode, __builtin_ctzll (constant));
        rtx quo = gen_reg_rtx (SImode);
        emit_insn (gen_sshrsi3 (quo, operands[1], operands[2]));
        rtx temp = gen_reg_rtx (SImode);
        emit_insn (gen_ashlsi3 (temp, quo, operands[2]));
        emit_insn (gen_subsi3 (operands[0], operands[1], temp));
      }
    else if (optimize_size)
      {
        rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__modsi3"),
                                            operands[0], LCT_CONST, SImode,
                                            operands[1], SImode, operands[2], SImode);
        if (dest != operands[0])
          emit_move_insn (operands[0], dest);
      }
    else
      {
        operands[2] = force_reg (SImode, operands[2]);
        rtx absa = gen_reg_rtx (SImode);
        emit_insn (gen_abssi2 (absa, operands[1]));
        rtx absb = gen_reg_rtx (SImode);
        emit_insn (gen_abssi2 (absb, operands[2]));
        emit_insn (gen_umodsi3 (operands[0], absa, absb));
        rtx negated = gen_reg_rtx (SImode);
        emit_insn (gen_negsi2 (negated, operands[0]));
        rtx select = gen_rtx_IF_THEN_ELSE (SImode, gen_rtx_LT (VOIDmode, operands[1], const0_rtx),
                                           negated, operands[0]);
        emit_insn (gen_rtx_SET (operands[0], select));
      }
    DONE;
  }
)

(define_expand "udivsi3"
  [(set (match_operand:SI 0 "register_operand" "")
        (udiv:SI (match_operand:SI 1 "register_operand" "")
                 (match_operand 2 "nonmemory_operand" "")))]
  ""
  {
    if (const_pow2lt64_operand (operands[2], VOIDmode))
      {
        HOST_WIDE_INT constant = INTVAL (operands[2]);
        operands[2] = gen_rtx_CONST_INT (VOIDmode, __builtin_ctzll (constant));
        emit_insn (gen_lshrsi3 (operands[0], operands[1], operands[2]));
      }
    else if (optimize_size)
      {
        rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__udivsi3"),
                                            operands[0], LCT_CONST, SImode,
                                            operands[1], SImode, operands[2], SImode);
        if (dest != operands[0])
          emit_move_insn (operands[0], dest);
      }
    else
      {
        operands[2] = force_reg (SImode, operands[2]);
        rtx s = gen_rtx_CONST_STRING (VOIDmode, KV3 ? ".s" : "");
        rtx rns = gen_rtx_CONST_STRING (VOIDmode, KV3 ? ".rn.s" : ".rn");
    // double double1 = 1.0;
        rtx double1 = copy_to_mode_reg (DFmode, const_double_from_real_value (dconst1, DFmode));
    // float floatb = (float)b;
        rtx floatb = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_floatuw (floatb, operands[2], const0_rtx, rns));
    // float floatrec =  __builtin_kvx_frecw(floatb, ".rn.s");
        rtx floatrec = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_frecw (floatrec, floatb, rns));
    // if ((b & (long)kvx_divmod_zero) == 0) __builtin_trap();
        rtx divnez = gen_label_rtx ();
        rtx pointer = gen_reg_rtx (Pmode);
        rtx notrap = gen_reg_rtx (SImode);
        emit_insn (gen_rtx_SET (pointer, kvx_divmod_zero));
        emit_insn (gen_rtx_SET (notrap, gen_rtx_SUBREG (SImode, pointer, 0)));
        emit_insn (gen_iorsi3 (notrap, operands[2], notrap));
        profile_probability probnez = profile_probability::guessed_always ();
        emit_cmp_and_jump_insns (notrap, const0_rtx, NE, NULL, SImode, 0, divnez, probnez);
        expand_builtin_trap ();
        emit_label (divnez);
    // double doublea = (double)a;
        rtx ulonga = gen_reg_rtx (DImode);
        convert_move (ulonga, operands[1], 1);
        rtx doublea = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_floatud (doublea, ulonga, const0_rtx, rns));
    // double doubleb = (double)b;
        rtx ulongb = gen_reg_rtx (DImode);
        convert_move (ulongb, operands[2], 1);
        rtx doubleb = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_floatud (doubleb, ulongb, const0_rtx, rns));
    // double doublerec = (double)floatrec;
        rtx doublerec = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_fwidenwd (doublerec, floatrec, s));
    // double alpha = __builtin_kvx_ffmsd(doublerec, doubleb, double1, ".rn.s");
        rtx alpha = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_ffmsd (alpha, doublerec, doubleb, double1, rns));
    // double beta = __builtin_kvx_ffmad(alpha, doublerec, doublerec, ".rn.s");
        rtx beta = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_ffmad (beta, alpha, doublerec, doublerec, rns));
    // double gamma = __builtin_kvx_fmuld(doublea, beta, ".rn.s");
        rtx gamma = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_fmuld (gamma, doublea, beta, rns));
    // uint32_t quo = __builtin_kvx_fixedud(gamma, 0, ".rn.s");
        rtx quo = gen_reg_rtx (SImode);
        emit_insn (gen_kvx_fixedud (gen_rtx_SUBREG (DImode, quo, 0), gamma, const0_rtx, rns));
    // int32_t rem = a - quo*b;
        rtx rem = gen_reg_rtx (SImode);
        emit_insn (gen_msubsisi4 (rem, quo, operands[2], operands[1]));
    // uint32_t cond = rem >> 31;
        rtx cond = gen_reg_rtx (SImode);
        emit_insn (gen_ashrsi3 (cond, rem, GEN_INT (31)));
    // uint32_t res = quo + cond;
        emit_insn (gen_addsi3 (operands[0], quo, cond));
    // return res;
      }
    DONE;
  }
)

(define_expand "umodsi3"
  [(set (match_operand:SI 0 "register_operand" "")
        (umod:SI (match_operand:SI 1 "register_operand" "")
                 (match_operand 2 "nonmemory_operand" "")))]
  ""
  {
    if (const_pow2lt64_operand (operands[2], VOIDmode))
      {
        HOST_WIDE_INT constant = INTVAL (operands[2]);
        operands[2] = gen_rtx_CONST_INT (VOIDmode, constant - 1ULL);
        emit_insn (gen_andsi3 (operands[0], operands[1], operands[2]));
      }
    else if (optimize_size)
      {
        rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__umodsi3"),
                                            operands[0], LCT_CONST, SImode,
                                            operands[1], SImode, operands[2], SImode);
        if (dest != operands[0])
          emit_move_insn (operands[0], dest);
      }
    else
      {
        operands[2] = force_reg (SImode, operands[2]);
        rtx s = gen_rtx_CONST_STRING (VOIDmode, KV3 ? ".s" : "");
        rtx rns = gen_rtx_CONST_STRING (VOIDmode, KV3 ? ".rn.s" : ".rn");
    // double double1 = 1.0;
        rtx double1 = copy_to_mode_reg (DFmode, const_double_from_real_value (dconst1, DFmode));
    // float floatb = (float)b;
        rtx floatb = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_floatuw (floatb, operands[2], const0_rtx, rns));
    // float floatrec =  __builtin_kvx_frecw(floatb, ".rn.s");
        rtx floatrec = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_frecw (floatrec, floatb, rns));
    // if ((b & (long)kvx_divmod_zero) == 0) __builtin_trap();
        rtx divnez = gen_label_rtx ();
        rtx pointer = gen_reg_rtx (Pmode);
        rtx notrap = gen_reg_rtx (SImode);
        emit_insn (gen_rtx_SET (pointer, kvx_divmod_zero));
        emit_insn (gen_rtx_SET (notrap, gen_rtx_SUBREG (SImode, pointer, 0)));
        emit_insn (gen_iorsi3 (notrap, operands[2], notrap));
        profile_probability probnez = profile_probability::guessed_always ();
        emit_cmp_and_jump_insns (notrap, const0_rtx, NE, NULL, SImode, 0, divnez, probnez);
        expand_builtin_trap ();
        emit_label (divnez);
    // double doublea = (double)a;
        rtx ulonga = gen_reg_rtx (DImode);
        convert_move (ulonga, operands[1], 1);
        rtx doublea = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_floatud (doublea, ulonga, const0_rtx, rns));
    // double doubleb = (double)b;
        rtx ulongb = gen_reg_rtx (DImode);
        convert_move (ulongb, operands[2], 1);
        rtx doubleb = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_floatud (doubleb, ulongb, const0_rtx, rns));
    // double doublerec = (double)floatrec;
        rtx doublerec = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_fwidenwd (doublerec, floatrec, s));
    // double alpha = __builtin_kvx_ffmsd(doublerec, doubleb, double1, ".rn.s");
        rtx alpha = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_ffmsd (alpha, doublerec, doubleb, double1, rns));
    // double beta = __builtin_kvx_ffmad(alpha, doublerec, doublerec, ".rn.s");
        rtx beta = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_ffmad (beta, alpha, doublerec, doublerec, rns));
    // double gamma = __builtin_kvx_fmuld(doublea, beta, ".rn.s");
        rtx gamma = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_fmuld (gamma, doublea, beta, rns));
    // uint32_t quo = __builtin_kvx_fixedud(gamma, 0, ".rn.s");
        rtx quo = gen_reg_rtx (SImode);
        emit_insn (gen_kvx_fixedud (gen_rtx_SUBREG (DImode, quo, 0), gamma, const0_rtx, rns));
    // int32_t rem = a - quo*b;
        rtx rem = gen_reg_rtx (SImode);
        emit_insn (gen_msubsisi4 (rem, quo, operands[2], operands[1]));
    // uint32_t cond = rem >> 31;
        rtx cond = gen_reg_rtx (SImode);
        emit_insn (gen_ashrsi3 (cond, rem, GEN_INT (31)));
    // uint32_t res = rem + (b & cond);
        rtx temp = gen_reg_rtx (SImode);
        emit_insn (gen_andsi3 (temp, operands[2], cond));
        emit_insn (gen_addsi3 (operands[0], rem, temp));
    // return res;
      }
    DONE;
  }
)

(define_insn "maddsisi4"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (plus:SI (mult:SI (match_operand:SI 1 "register_operand" "r")
                          (match_operand:SI 2 "register_operand" "r"))
                 (match_operand:SI 3 "register_operand" "0")))]
  ""
  "maddw %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)
;; zero-extend version of maddsisi4
(define_insn "*maddsisi4_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (plus:SI (mult:SI (match_operand:SI 1 "register_operand" "r")
                                          (match_operand:SI 2 "register_operand" "r"))
                                 (match_operand:SI 3 "register_operand" "0"))))]
  ""
  "maddw %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "maddsidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (plus:DI (mult:DI (sign_extend:DI (match_operand:SI 1 "register_operand" "r"))
                          (sign_extend:DI (match_operand:SI 2 "register_operand" "r")))
                 (match_operand:DI 3 "register_operand" "0")))]
  ""
  "maddwd %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "umaddsidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (plus:DI (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
                          (zero_extend:DI (match_operand:SI 2 "register_operand" "r")))
                 (match_operand:DI 3 "register_operand" "0")))]
  ""
  "madduwd %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "*maddsuwd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (plus:DI (mult:DI (sign_extend:DI (match_operand:SI 1 "register_operand" "r"))
                          (zero_extend:DI (match_operand:SI 2 "register_operand" "r")))
                 (match_operand:DI 3 "register_operand" "0")))]
  ""
  "maddsuwd %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "usmaddsidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (plus:DI (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
                          (sign_extend:DI (match_operand:SI 2 "register_operand" "r")))
                 (match_operand:DI 3 "register_operand" "0")))]
  ""
  "maddsuwd %0 = %2, %1"
  [(set_attr "type" "madd_int")]
)

(define_insn "msubsisi4"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (minus:SI (match_operand:SI 3 "register_operand" "0")
                  (mult:SI (match_operand:SI 1 "register_operand" "r")
                           (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "msbfw %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)
;; zero-extend version of msubsisi4
(define_insn "*msubsisi4_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (minus:SI (match_operand:SI 3 "register_operand" "0")
                                  (mult:SI (match_operand:SI 1 "register_operand" "r")
                                           (match_operand:SI 2 "register_operand" "r")))))]
  ""
  "msbfw %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "msubsidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (minus:DI (match_operand:DI 3 "register_operand" "0")
                  (mult:DI (sign_extend:DI (match_operand:SI 1 "register_operand" "r"))
                           (sign_extend:DI (match_operand:SI 2 "register_operand" "r")))))]
  ""
  "msbfwd %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "umsubsidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (minus:DI (match_operand:DI 3 "register_operand" "0")
                  (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
                           (zero_extend:DI (match_operand:SI 2 "register_operand" "r")))))]
  ""
  "msbfuwd %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "*msbfsuwd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (minus:DI (match_operand:DI 3 "register_operand" "0")
                  (mult:DI (sign_extend:DI (match_operand:SI 1 "register_operand" "r"))
                           (zero_extend:DI (match_operand:SI 2 "register_operand" "r")))))]
  ""
  "msbfsuwd %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "usmsubsidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (minus:DI (match_operand:DI 3 "register_operand" "0")
                  (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
                           (sign_extend:DI (match_operand:SI 2 "register_operand" "r")))))]
  ""
  "msbfsuwd %0 = %2, %1"
  [(set_attr "type" "madd_int")]
)

(define_insn "sminsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (smin:SI (match_operand:SI 1 "register_operand" "r,r,r")
                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "minw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)
;; zero-extend version of sminsi3
(define_insn "*sminsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (smin:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "minw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)

(define_insn "smaxsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (smax:SI (match_operand:SI 1 "register_operand" "r,r,r")
                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "maxw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)
;; zero-extend version of smaxsi3
(define_insn "*smaxsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (smax:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "maxw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)

(define_insn "uminsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (umin:SI (match_operand:SI 1 "register_operand" "r,r,r")
                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "minuw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)
;; zero-extend version of uminsi3
(define_insn "*uminsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (umin:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "minuw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)

(define_insn "umaxsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (umax:SI (match_operand:SI 1 "register_operand" "r,r,r")
                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "maxuw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)
;; zero-extend version of umaxsi3
(define_insn "*umaxsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (umax:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "maxuw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)

(define_insn "andsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (and:SI (match_operand:SI 1 "register_operand" "r,r,r")
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "andw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)
;; zero-extend version of andsi3
(define_insn "*andsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (and:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "andw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)

(define_insn "*nandw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (ior:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                (not:SI (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "nandw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)
;; zero-extend version of *nandw
(define_insn "*nandw_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (ior:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                                (not:SI (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))))]
  ""
  "nandw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)

(define_insn "*andnw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (and:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "andnw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)
;; zero-extend version of *andnw
(define_insn "*andnw_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (and:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "andnw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)

(define_insn "iorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (ior:SI (match_operand:SI 1 "register_operand" "r,r,r")
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "iorw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)
;; zero-extend version of iorsi3
(define_insn "*iorsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (ior:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "iorw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)

(define_insn "*norw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (and:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                (not:SI (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "niorw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)
;; zero-extend version of *norw
(define_insn "*norw_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (and:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                                (not:SI (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))))]
  ""
  "niorw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)

(define_insn "*ornw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (ior:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "iornw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)
;; zero-extend version of *ornw
(define_insn "*ornw_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (ior:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "iornw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)

(define_insn "xorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (xor:SI (match_operand:SI 1 "register_operand" "r,r,r")
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "xorw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)
;; zero-extend version of xorsi3
(define_insn "*xorsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (xor:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "xorw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)

(define_insn "*nxorw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (not:SI (xor:SI (match_operand:SI 1 "register_operand" "r,r,r")
                        (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "nxorw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)
;; zero-extend version of *nxorw
(define_insn "*nxorw_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (not:SI (xor:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                        (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))))]
  ""
  "nxorw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_w,alu_tiny_x")
   (set_attr "length" "4,       4,         8")]
)

;; zero-extend version of ashlsi3
(define_insn "*ashlsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (ashift:SI (match_operand:SI 1 "register_operand" "r")
                                   (match_operand:SI 2 "reg_shift_operand" "rU06"))))]
  ""
  "sllw %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

;; zero-extend version of ssashlsi3
(define_insn "*ssashlsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (ss_ashift:SI (match_operand:SI 1 "register_operand" "r")
                                      (match_operand:SI 2 "reg_shift_operand" "rU06"))))]
  ""
  "slsw %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

;; zero-extend version of usashlsi3
(define_insn "*usashlsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (us_ashift:SI (match_operand:SI 1 "register_operand" "r")
                                      (match_operand:SI 2 "reg_shift_operand" "rU06"))))]
  "(KV3_2||KV4)"
  "slusw %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

;; zero-extend version of ashrsi3
(define_insn "*ashrsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (ashiftrt:SI (match_operand:SI 1 "register_operand" "r")
                                     (match_operand:SI 2 "reg_shift_operand" "rU06"))))]
  ""
  "sraw %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

;; zero-extend version of lshrsi3
(define_insn "*lshrsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (lshiftrt:SI (match_operand:SI 1 "register_operand" "r")
                                     (match_operand:SI 2 "reg_shift_operand" "rU06"))))]
  ""
  "srlw %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

;; zero-extend version of sshrsi3
(define_insn "*sshrsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (unspec:SI [(match_operand:SI 1 "register_operand" "r")
                                    (match_operand:SI 2 "reg_shift_operand" "rU06")] UNSPEC_SRS)))]
  ""
  "srsw %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "rotlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (rotate:SI (match_operand:SI 1 "register_operand" "r")
                   (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "rolw %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)
;; zero-extend version of rotlsi3
(define_insn "*rotlsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (rotate:SI (match_operand:SI 1 "register_operand" "r")
                                   (match_operand:SI 2 "reg_shift_operand" "rU06"))))]
  ""
  "rolw %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "rotrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (rotatert:SI (match_operand:SI 1 "register_operand" "r")
                     (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "rorw %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)
;; zero-extend version of rotrsi3
(define_insn "*rotrsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (rotatert:SI (match_operand:SI 1 "register_operand" "r")
                                     (match_operand:SI 2 "reg_shift_operand" "rU06"))))]
  ""
  "rorw %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "avgsi3_floor"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r,r")
                    (match_operand:SI 2 "register_s32_operand" "r,B32")] UNSPEC_AVG))]
  ""
  "avgw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "4,8")]
)
;; zero-extend version of avgsi3_floor
(define_insn "*avgsi3_floor_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (unspec:SI [(match_operand:SI 1 "register_operand" "r,r")
                                    (match_operand:SI 2 "register_s32_operand" "r,B32")] UNSPEC_AVG)))]
  ""
  "avgw %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "4,8")]
)

(define_insn "avgsi3_ceil"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r,r")
                    (match_operand:SI 2 "register_s32_operand" "r,B32")] UNSPEC_AVGR))]
  ""
  "avgrw %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "4,8")]
)
;; zero-extend version of avgsi3_ceil
(define_insn "*avgsi3_ceil_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (unspec:SI [(match_operand:SI 1 "register_operand" "r,r")
                                    (match_operand:SI 2 "register_s32_operand" "r,B32")] UNSPEC_AVGR)))]
  ""
  "avgrw %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "4,8")]
)

(define_insn "uavgsi3_floor"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r,r")
                    (match_operand:SI 2 "register_s32_operand" "r,B32")] UNSPEC_AVGU))]
  ""
  "avguw %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "4,8")]
)
;; zero-extend version of uavgsi3_floor
(define_insn "*uavgsi3_floor_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (unspec:SI [(match_operand:SI 1 "register_operand" "r,r")
                                    (match_operand:SI 2 "register_s32_operand" "r,B32")] UNSPEC_AVGU)))]
  ""
  "avguw %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "4,8")]
)

(define_insn "uavgsi3_ceil"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r,r")
                    (match_operand:SI 2 "register_s32_operand" "r,B32")] UNSPEC_AVGRU))]
  ""
  "avgruw %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "4,8")]
)
;; zero-extend version of uavgsi3_ceil
(define_insn "*uavgsi3_ceil_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (unspec:SI [(match_operand:SI 1 "register_operand" "r,r")
                                    (match_operand:SI 2 "register_s32_operand" "r,B32")] UNSPEC_AVGRU)))]
  ""
  "avgruw %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "4,8")]
)

(define_insn "bswapsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (bswap:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "sbmm8 %0 = %1, 0x01020408"
  [(set_attr "type" "alu_thin_x")
   (set_attr "length"        "8")]
)
;; zero-extend version of bswapsi2
(define_insn "*bswapsi2_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (bswap:SI (match_operand:SI 1 "register_operand" "r"))))]
  ""
  "sbmm8 %0 = %1, 0x01020408"
  [(set_attr "type" "alu_thin_x")
   (set_attr "length"        "8")]
)

;; zero-extend version of negsi2
(define_insn "*negsi2_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (neg:SI (match_operand:SI 1 "register_operand" "r"))))]
  ""
  "negw %0 = %1"
  [(set_attr "type" "alu_tiny_w")]
)

;; zero-extend version of ssnegsi2
(define_insn "*ssnegsi2_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (ss_neg:SI (match_operand:SI 1 "register_operand" "r"))))]
  ""
  "sbfsw %0 = %1, 0"
  [(set_attr "type" "alu_thin_x")
   (set_attr "length"        "8")]
)

;; zero-extend version of abssi2
(define_insn "*abssi2_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (abs:SI (match_operand:SI 1 "register_operand" "r"))))]
  ""
  "absw %0 = %1"
  [(set_attr "type" "alu_thin_w")]
)

;; zero-extend version of ssabssi2
(define_insn "*ssabssi2_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (ss_abs:SI (match_operand:SI 1 "register_operand" "r"))))]
  "(KV3_2||KV4)"
  "abssw %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "clrsbsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (clrsb:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "clsw %0 = %1"
  [(set_attr "type" "alu_lite")]
)
;; zero-extend version of clrsbsi2
(define_insn "*clrsbsi2_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (clrsb:SI (match_operand:SI 1 "register_operand" "r"))))]
  ""
  "clsw %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "clzsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (clz:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "clzw %0 = %1"
  [(set_attr "type" "alu_lite")]
)
;; zero-extend version of clzsi2
(define_insn "*clzsi2_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (clz:SI (match_operand:SI 1 "register_operand" "r"))))]
  ""
  "clzw %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "ctzsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (ctz:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "ctzw %0 = %1"
  [(set_attr "type" "alu_lite")]
)
;; zero-extend version of ctzsi2
(define_insn "*ctzsi2_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (ctz:SI (match_operand:SI 1 "register_operand" "r"))))]
  ""
  "ctzw %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "popcountsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (popcount:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "cbsw %0 = %1"
  [(set_attr "type" "alu_lite")]
)
;; zero-extend version of popcountsi2
(define_insn "*popcountsi2_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (popcount:SI (match_operand:SI 1 "register_operand" "r"))))]
  ""
  "cbsw %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "one_cmplsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (not:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "notw %0 = %1"
  [(set_attr "type" "alu_tiny_w")]
)
;; zero-extend version of one_cmplsi2
(define_insn "*one_cmplsi2_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (not:SI (match_operand:SI 1 "register_operand" "r"))))]
  ""
  "notw %0 = %1"
  [(set_attr "type" "alu_tiny_w")]
)

(define_insn "kvx_stsuw"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r")
                    (match_operand:SI 2 "register_operand" "r")] UNSPEC_STSU))]
  ""
  "stsuw %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)


;; DI

(define_insn "adddi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (plus:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                 (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "addd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)
(define_insn "*add<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (plus:DI (ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r,r"))
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  ""
  "add<unsx>wd %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*addx2<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (plus:DI (ashift:DI (ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r,r"))
                            (const_int 1))
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx2<unsx>wd %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
(define_insn "*addx2uwd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (plus:DI (and:DI (ashift:DI (match_operand:DI 1 "register_operand" "r,r")
                                    (const_int 1))
                         (const_int 8589934590)) ;; 0x1fffffffe
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx2uwd %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*addx4<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (plus:DI (ashift:DI (ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r,r"))
                            (const_int 2))
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx4<unsx>wd %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
(define_insn "*addx4uwd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (plus:DI (and:DI (ashift:DI (match_operand:DI 1 "register_operand" "r,r")
                                    (const_int 2))
                         (const_int 17179869180)) ;; 0x3fffffffc
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx4uwd %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*addx8<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (plus:DI (ashift:DI (ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r,r"))
                            (const_int 3))
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx8<unsx>wd %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
(define_insn "*addx8uwd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (plus:DI (and:DI (ashift:DI (match_operand:DI 1 "register_operand" "r,r")
                                    (const_int 3))
                         (const_int 34359738360)) ;; 0x7fffffff8
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx8uwd %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*addx16<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (plus:DI (ashift:DI (ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r,r"))
                            (const_int 4))
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx16<unsx>wd %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
(define_insn "*addx16uwd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (plus:DI (and:DI (ashift:DI (match_operand:DI 1 "register_operand" "r,r")
                                    (const_int 4))
                         (const_int 68719476720)) ;; 0xffffffff0
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  ""
  "addx16uwd %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*addx32<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (plus:DI (ashift:DI (ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r,r"))
                            (const_int 5))
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  "(KV3_2||KV4)"
  "addx32<unsx>wd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)
(define_insn "*addx32uwd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (plus:DI (and:DI (ashift:DI (match_operand:DI 1 "register_operand" "r,r")
                                    (const_int 5))
                         (const_int 137438953440)) ;; 0x1fffffffe0
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  "(KV3_2||KV4)"
  "addx32uwd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*addx64<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (plus:DI (ashift:DI (ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r,r"))
                            (const_int 6))
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  "(KV3_2||KV4)"
  "addx64<unsx>wd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)
(define_insn "*addx64uwd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (plus:DI (and:DI (ashift:DI (match_operand:DI 1 "register_operand" "r,r")
                                    (const_int 6))
                         (const_int 274877906880)) ;; 0x3fffffffc0
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  "(KV3_2||KV4)"
  "addx64uwd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "ssadddi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (ss_plus:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                    (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "addsd %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin,alu_thin_x,alu_thin_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "subdi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (minus:DI (match_operand:DI 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                  (match_operand:DI 2 "register_operand" "r,r,r,r")))]
  ""
  "sbfd %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)
(define_insn "*sbf<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (minus:DI (match_operand:DI 1 "register_s32_operand" "r,B32")
                  (ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r,r"))))]
  ""
  "sbf<unsx>wd %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx2<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (minus:DI (match_operand:DI 1 "register_s32_operand" "r,B32")
                  (ashift:DI (ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r,r"))
                             (const_int 1))))]
  ""
  "sbfx2<unsx>wd %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
(define_insn "*sbfx2uwd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (minus:DI (match_operand:DI 1 "register_s32_operand" "r,B32")
                  (and:DI (ashift:DI (match_operand:DI 2 "register_operand" "r,r")
                                     (const_int 1))
                          (const_int 8589934590))))] ;; 0x1fffffffe
  ""
  "sbfx2uwd %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx4<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (minus:DI (match_operand:DI 1 "register_s32_operand" "r,B32")
                  (ashift:DI (ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r,r"))
                             (const_int 2))))]
  ""
  "sbfx4<unsx>wd %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
(define_insn "*sbfx4uwd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (minus:DI (match_operand:DI 1 "register_s32_operand" "r,B32")
                  (and:DI (ashift:DI (match_operand:DI 2 "register_operand" "r,r")
                                     (const_int 2))
                          (const_int 17179869180))))] ;; 0x3fffffffc
  ""
  "sbfx4uwd %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx8<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (minus:DI (match_operand:DI 1 "register_s32_operand" "r,B32")
                  (ashift:DI (ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r,r"))
                             (const_int 3))))]
  ""
  "sbfx8<unsx>wd %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
(define_insn "*sbfx8uwd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (minus:DI (match_operand:DI 1 "register_s32_operand" "r,B32")
                  (and:DI (ashift:DI (match_operand:DI 2 "register_operand" "r,r")
                                     (const_int 3))
                          (const_int 34359738360))))] ;; 0x7fffffff8
  ""
  "sbfx8uwd %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx16<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (minus:DI (match_operand:DI 1 "register_s32_operand" "r,B32")
                  (ashift:DI (ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r,r"))
                             (const_int 4))))]
  ""
  "sbfx16<unsx>wd %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)
(define_insn "*sbfx16uwd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (minus:DI (match_operand:DI 1 "register_s32_operand" "r,B32")
                  (and:DI (ashift:DI (match_operand:DI 2 "register_operand" "r,r")
                                     (const_int 4))
                          (const_int 68719476720))))] ;; 0xffffffff0
  ""
  "sbfx16uwd %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx32<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (minus:DI (match_operand:DI 1 "register_s32_operand" "r,B32")
                  (ashift:DI (ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r,r"))
                             (const_int 5))))]
  "(KV3_2||KV4)"
  "sbfx32<unsx>wd %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)
(define_insn "*sbfx32uwd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (minus:DI (match_operand:DI 1 "register_s32_operand" "r,B32")
                  (and:DI (ashift:DI (match_operand:DI 2 "register_operand" "r,r")
                                     (const_int 5))
                          (const_int 137438953440))))] ;; 0x1fffffffe0
  "(KV3_2||KV4)"
  "sbfx32uwd %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "*sbfx64<unsx>wd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (minus:DI (match_operand:DI 1 "register_s32_operand" "r,B32")
                  (ashift:DI (ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r,r"))
                             (const_int 6))))]
  "(KV3_2||KV4)"
  "sbfx64<unsx>wd %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)
(define_insn "*sbfx64uwd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (minus:DI (match_operand:DI 1 "register_s32_operand" "r,B32")
                  (and:DI (ashift:DI (match_operand:DI 2 "register_operand" "r,r")
                                     (const_int 6))
                          (const_int 274877906880))))] ;; 0x3fffffffc0
  "(KV3_2||KV4)"
  "sbfx64uwd %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "sssubdi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (ss_minus:DI (match_operand:DI 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                     (match_operand:DI 2 "register_operand" "r,r,r,r")))]
  ""
  "sbfsd %0 = %2, %1"
  [(set_attr "type" "alu_thin,alu_thin,alu_thin_x,alu_thin_y")
   (set_attr "length"      "4,       4,         8,        12")]
)

(define_expand "muldi3"
  [(set (match_operand:DI 0 "register_operand" "")
        (mult:DI (match_operand:DI 1 "register_operand" "")
                 (match_operand:DI 2 "register_s32_operand" "")))]
  ""
  ""
)
(define_insn "muldi3_1"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (mult:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                 (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  "KV3_1"
  "muld %0 = %1, %2"
  [(set_attr "type" "mult_int, mult_int, mult_int_x, mult_int_y")
   (set_attr "length"      "4,        4,          8,         12")]
)
(define_insn "muldi3_4"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (mult:DI (match_operand:DI 1 "register_operand" "r,r")
                 (match_operand:DI 2 "register_s32_operand" "r,B32")))]
  "!KV3_1"
  "muld %0 = %1, %2"
  [(set_attr "type" "mult_int, mult_int_x")
   (set_attr "length"      "4,          8")]
)

(define_expand "divdi3"
  [(set (match_operand:DI 0 "register_operand" "")
        (div:DI (match_operand:DI 1 "register_operand" "")
                (match_operand 2 "nonmemory_operand" "")))]
  ""
  {
    if (const_pow2lt64_operand (operands[2], VOIDmode))
      {
        HOST_WIDE_INT constant = INTVAL (operands[2]);
        operands[2] = gen_rtx_CONST_INT (VOIDmode, __builtin_ctzll (constant));
        emit_insn (gen_sshrdi3 (operands[0], operands[1], operands[2]));
      }
    else if (optimize_size)
      {
        rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__divdi3"),
                                            operands[0], LCT_CONST, DImode,
                                            operands[1], DImode, operands[2], DImode);
        if (dest != operands[0])
          emit_move_insn (operands[0], dest);
      }
    else
      {
        operands[2] = force_reg (DImode, operands[2]);
        rtx absa = gen_reg_rtx (DImode);
        emit_insn (gen_absdi2 (absa, operands[1]));
        rtx absb = gen_reg_rtx (DImode);
        emit_insn (gen_absdi2 (absb, operands[2]));
        rtx sign = gen_reg_rtx (DImode);
        emit_insn (gen_xordi3 (sign, operands[1], operands[2]));
        emit_insn (gen_udivdi3 (operands[0], absa, absb));
        rtx negated = gen_reg_rtx (DImode);
        emit_insn (gen_negdi2 (negated, operands[0]));
        rtx select = gen_rtx_IF_THEN_ELSE (DImode, gen_rtx_LT (VOIDmode, sign, const0_rtx),
                                           negated, operands[0]);
        emit_insn (gen_rtx_SET (operands[0], select));
      }
    DONE;
  }
)

(define_expand "moddi3"
  [(set (match_operand:DI 0 "register_operand" "")
        (mod:DI (match_operand:DI 1 "register_operand" "")
                (match_operand 2 "nonmemory_operand" "")))]
  ""
  {
    if (const_pow2lt64_operand (operands[2], VOIDmode))
      {
        HOST_WIDE_INT constant = INTVAL (operands[2]);
        operands[2] = gen_rtx_CONST_INT (VOIDmode, __builtin_ctzll (constant));
        rtx quo = gen_reg_rtx (DImode);
        emit_insn (gen_sshrdi3 (quo, operands[1], operands[2]));
        rtx temp = gen_reg_rtx (DImode);
        emit_insn (gen_ashldi3 (temp, quo, operands[2]));
        emit_insn (gen_subdi3 (operands[0], operands[1], temp));
      }
    else if (optimize_size)
      {
        rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__moddi3"),
                                            operands[0], LCT_CONST, DImode,
                                            operands[1], DImode, operands[2], DImode);
        if (dest != operands[0])
          emit_move_insn (operands[0], dest);
      }
    else
      {
        operands[2] = force_reg (DImode, operands[2]);
        rtx absa = gen_reg_rtx (DImode);
        emit_insn (gen_absdi2 (absa, operands[1]));
        rtx absb = gen_reg_rtx (DImode);
        emit_insn (gen_absdi2 (absb, operands[2]));
        emit_insn (gen_umoddi3 (operands[0], absa, absb));
        rtx negated = gen_reg_rtx (DImode);
        emit_insn (gen_negdi2 (negated, operands[0]));
        rtx select = gen_rtx_IF_THEN_ELSE (DImode, gen_rtx_LT (VOIDmode, operands[1], const0_rtx),
                                           negated, operands[0]);
        emit_insn (gen_rtx_SET (operands[0], select));
      }
    DONE;
  }
)

(define_expand "udivdi3"
  [(set (match_operand:DI 0 "register_operand" "")
        (udiv:DI (match_operand:DI 1 "register_operand" "")
                 (match_operand 2 "nonmemory_operand" "")))]
  ""
  {
    if (const_pow2lt64_operand (operands[2], VOIDmode))
      {
        HOST_WIDE_INT constant = INTVAL (operands[2]);
        operands[2] = gen_rtx_CONST_INT (VOIDmode, __builtin_ctzll (constant));
        emit_insn (gen_lshrdi3 (operands[0], operands[1], operands[2]));
      }
    else if (optimize_size)
      {
        rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__udivdi3"),
                                            operands[0], LCT_CONST, DImode,
                                            operands[1], DImode, operands[2], DImode);
        if (dest != operands[0])
          emit_move_insn (operands[0], dest);
      }
    else
      {
        operands[2] = force_reg (DImode, operands[2]);
        rtx s = gen_rtx_CONST_STRING (VOIDmode, KV3 ? ".s" : "");
        rtx rns = gen_rtx_CONST_STRING (VOIDmode, KV3 ? ".rn.s" : ".rn");
        rtx dnez = gen_rtx_CONST_STRING (VOIDmode, ".dnez");
        rtx deqz = gen_rtx_CONST_STRING (VOIDmode, ".deqz");
    // double double1 = 1.0;
        rtx double1 = copy_to_mode_reg (DFmode, const_double_from_real_value (dconst1, DFmode));
    // bbig = (int64_t) b < 0;
        rtx bbig = gen_reg_rtx (DImode);
        emit_insn (gen_rtx_SET (bbig, gen_rtx_LT (DImode, operands[2], const0_rtx)));
    // int64_t bin01 = b <= 1;
        rtx bin01 = gen_reg_rtx (DImode);
        emit_insn (gen_rtx_SET (bin01, gen_rtx_LEU (DImode, operands[2], const1_rtx)));
    // int64_t special = bbig | bin01;
        rtx special = gen_reg_rtx (DImode);
        emit_insn (gen_iordi3 (special, bbig, bin01));
    // int64_t ageb = a >= b;
        rtx ageb = gen_reg_rtx (DImode);
        emit_insn (gen_rtx_SET (ageb, gen_rtx_GEU (DImode, operands[1], operands[2])));
    // uint64_t q = __builtin_kvx_selectd (a >= b, a, bbig, ".nez");
        rtx q = gen_reg_rtx (DImode);
        emit_insn (gen_kvx_selectd (q, ageb, operands[1], bbig, dnez));
    // uint64_t a_b_q = a - (b & -q);
        rtx a_b_q = gen_reg_rtx (DImode);
        emit_insn (gen_negdi2 (a_b_q, q));
        emit_insn (gen_anddi3 (a_b_q, operands[2], a_b_q));
        emit_insn (gen_subdi3 (a_b_q, operands[1], a_b_q));
    // uint64_t r = __builtin_kvx_selectd (a - (b & -q), 0, bbig, ".nez");
        rtx r = gen_reg_rtx (DImode);
        emit_move_insn (r, const0_rtx);
        emit_insn (gen_kvx_selectd (r, a_b_q, r, bbig, dnez));
    // double doublea = (double)a;
        rtx doublea = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_floatud (doublea, operands[1], const0_rtx, rns));
    // double doubleb = (double)b;
        rtx doubleb = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_floatud (doubleb, operands[2], const0_rtx, rns));
    // float floatb = (float)doubleb;
        rtx floatb = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_fnarrowdw (floatb, doubleb, rns));
    // float floatrec = __builtin_kvx_frecw(floatb, ".rn.s");
        rtx floatrec = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_frecw (floatrec, floatb, rns));
    // if ((b & (long)kvx_divmod_zero) == 0) __builtin_trap();
        rtx divnez = gen_label_rtx ();
        rtx pointer = gen_reg_rtx (Pmode);
        rtx notrap = gen_reg_rtx (DImode);
        emit_insn (gen_rtx_SET (pointer, kvx_divmod_zero));
        emit_insn (gen_rtx_SET (notrap, gen_rtx_SUBREG (DImode, pointer, 0)));
        emit_insn (gen_iordi3 (notrap, operands[2], notrap));
        profile_probability probnez = profile_probability::guessed_always ();
        emit_cmp_and_jump_insns (notrap, const0_rtx, NE, NULL, DImode, 0, divnez, probnez);
        expand_builtin_trap ();
        emit_label (divnez);
    // double doublerec = __builtin_kvx_fwidenwd (floatrec, ".s");
        rtx doublerec = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_fwidenwd (doublerec, floatrec, s));
    // double doubleq0 = __builtin_kvx_fmuld (doublea, doublerec, ".rn.s");
        rtx doubleq0 = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_fmuld (doubleq0, doublea, doublerec, rns));
    // uint64_t q0 = __builtin_kvx_fixedud (doubleq0, 0, ".rn.s");
        rtx q0 = gen_reg_rtx (DImode);
        emit_insn (gen_kvx_fixedud (q0, doubleq0, const0_rtx, rns));
    // int64_t a1 = a - q0 * b;
        rtx a1 = gen_reg_rtx (DImode);
        emit_insn (gen_msubdidi4 (a1, q0, operands[2], operands[1]));
    // double alpha = __builtin_kvx_ffmsd (doubleb, doublerec, double1, ".rn.s");
        rtx alpha = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_ffmsd (alpha, doubleb, doublerec, double1, rns));
    // double beta = __builtin_kvx_ffmad(alpha, doublerec, doublerec, ".rn.s");
        rtx beta = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_ffmad (beta, alpha, doublerec, doublerec, rns));
    // double doublea1 = __builtin_kvx_floatd (a1, 0, ".rn.s");
        rtx doublea1 = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_floatd (doublea1, a1, const0_rtx, rns));
    // double gamma = __builtin_kvx_fmuld(beta, doublea1, ".rn.s");
        rtx gamma = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_fmuld (gamma, beta, doublea1, rns));
    // int64_t q1 = __builtin_kvx_fixedd(gamma, 0, ".rn.s");
        rtx q1 = gen_reg_rtx (DImode);
        emit_insn (gen_kvx_fixedd (q1, gamma, const0_rtx, rns));
    // int64_t rem = a1 - q1 * b;
        rtx rem = gen_reg_rtx (DImode);
        emit_insn (gen_msubdidi4 (rem, q1, operands[2], a1));
    // uint64_t quo = q0 + q1;
        rtx quo = gen_reg_rtx (DImode);
        emit_insn (gen_adddi3 (quo, q0, q1));
    // uint64_t cond = rem >> 63;
        rtx cond = gen_reg_rtx (DImode);
        emit_insn (gen_ashrdi3 (cond, rem, GEN_INT (63)));
    // uint64_t quo_cond = quo + cond;
        rtx quo_cond = gen_reg_rtx (DImode);
        emit_insn (gen_adddi3 (quo_cond, quo, cond));
    // q = __builtin_kvx_selectd (quo + cond, q, special, ".eqz");
        emit_insn (gen_kvx_selectd (q, quo_cond, q, special, deqz));
    // return q;
        emit_move_insn (operands[0], q);
      }
    DONE;
  }
)

(define_expand "umoddi3"
  [(set (match_operand:DI 0 "register_operand" "")
        (umod:DI (match_operand:DI 1 "register_operand" "")
                 (match_operand 2 "nonmemory_operand" "")))]
  ""
  {
    if (const_pow2lt64_operand (operands[2], VOIDmode))
      {
        HOST_WIDE_INT constant = INTVAL (operands[2]);
        operands[2] = gen_rtx_CONST_INT (VOIDmode, constant - 1ULL);
        emit_insn (gen_anddi3 (operands[0], operands[1], operands[2]));
      }
    else if (optimize_size)
      {
        rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__umoddi3"),
                                            operands[0], LCT_CONST, DImode,
                                            operands[1], DImode, operands[2], DImode);
        if (dest != operands[0])
          emit_move_insn (operands[0], dest);
      }
    else
      {
        operands[2] = force_reg (DImode, operands[2]);
        rtx s = gen_rtx_CONST_STRING (VOIDmode, KV3 ? ".s" : "");
        rtx rns = gen_rtx_CONST_STRING (VOIDmode, KV3 ? ".rn.s" : ".rn");
        rtx dnez = gen_rtx_CONST_STRING (VOIDmode, ".dnez");
        rtx deqz = gen_rtx_CONST_STRING (VOIDmode, ".deqz");
    // double double1 = 1.0;
        rtx double1 = copy_to_mode_reg (DFmode, const_double_from_real_value (dconst1, DFmode));
    // bbig = (int64_t) b < 0;
        rtx bbig = gen_reg_rtx (DImode);
        emit_insn (gen_rtx_SET (bbig, gen_rtx_LT (DImode, operands[2], const0_rtx)));
    // int64_t bin01 = b <= 1;
        rtx bin01 = gen_reg_rtx (DImode);
        emit_insn (gen_rtx_SET (bin01, gen_rtx_LEU (DImode, operands[2], const1_rtx)));
    // int64_t special = bbig | bin01;
        rtx special = gen_reg_rtx (DImode);
        emit_insn (gen_iordi3 (special, bbig, bin01));
    // int64_t ageb = a >= b;
        rtx ageb = gen_reg_rtx (DImode);
        emit_insn (gen_rtx_SET (ageb, gen_rtx_GEU (DImode, operands[1], operands[2])));
    // uint64_t q = __builtin_kvx_selectd (a >= b, a, bbig, ".nez");
        rtx q = gen_reg_rtx (DImode);
        emit_insn (gen_kvx_selectd (q, ageb, operands[1], bbig, dnez));
    // uint64_t a_b_q = a - (b & -q);
        rtx a_b_q = gen_reg_rtx (DImode);
        emit_insn (gen_negdi2 (a_b_q, q));
        emit_insn (gen_anddi3 (a_b_q, operands[2], a_b_q));
        emit_insn (gen_subdi3 (a_b_q, operands[1], a_b_q));
    // uint64_t r = __builtin_kvx_selectd (a - (b & -q), 0, bbig, ".nez");
        rtx r = gen_reg_rtx (DImode);
        emit_move_insn (r, const0_rtx);
        emit_insn (gen_kvx_selectd (r, a_b_q, r, bbig, dnez));
    // double doublea = (double)a;
        rtx doublea = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_floatud (doublea, operands[1], const0_rtx, rns));
    // double doubleb = (double)b;
        rtx doubleb = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_floatud (doubleb, operands[2], const0_rtx, rns));
    // float floatb = (float)doubleb;
        rtx floatb = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_fnarrowdw (floatb, doubleb, rns));
    // float floatrec = __builtin_kvx_frecw(floatb, ".rn.s");
        rtx floatrec = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_frecw (floatrec, floatb, rns));
    // if ((b & (long)kvx_divmod_zero) == 0) __builtin_trap();
        rtx divnez = gen_label_rtx ();
        rtx pointer = gen_reg_rtx (Pmode);
        rtx notrap = gen_reg_rtx (DImode);
        emit_insn (gen_rtx_SET (pointer, kvx_divmod_zero));
        emit_insn (gen_rtx_SET (notrap, gen_rtx_SUBREG (DImode, pointer, 0)));
        emit_insn (gen_iordi3 (notrap, operands[2], notrap));
        profile_probability probnez = profile_probability::guessed_always ();
        emit_cmp_and_jump_insns (notrap, const0_rtx, NE, NULL, DImode, 0, divnez, probnez);
        expand_builtin_trap ();
        emit_label (divnez);
    // double doublerec = __builtin_kvx_fwidenwd (floatrec, ".s");
        rtx doublerec = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_fwidenwd (doublerec, floatrec, s));
    // double doubleq0 = __builtin_kvx_fmuld (doublea, doublerec, ".rn.s");
        rtx doubleq0 = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_fmuld (doubleq0, doublea, doublerec, rns));
    // uint64_t q0 = __builtin_kvx_fixedud (doubleq0, 0, ".rn.s");
        rtx q0 = gen_reg_rtx (DImode);
        emit_insn (gen_kvx_fixedud (q0, doubleq0, const0_rtx, rns));
    // int64_t a1 = a - q0 * b;
        rtx a1 = gen_reg_rtx (DImode);
        emit_insn (gen_msubdidi4 (a1, q0, operands[2], operands[1]));
    // double alpha = __builtin_kvx_ffmsd (doubleb, doublerec, double1, ".rn.s");
        rtx alpha = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_ffmsd (alpha, doubleb, doublerec, double1, rns));
    // double beta = __builtin_kvx_ffmad(alpha, doublerec, doublerec, ".rn.s");
        rtx beta = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_ffmad (beta, alpha, doublerec, doublerec, rns));
    // double doublea1 = __builtin_kvx_floatd (a1, 0, ".rn.s");
        rtx doublea1 = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_floatd (doublea1, a1, const0_rtx, rns));
    // double gamma = __builtin_kvx_fmuld(beta, doublea1, ".rn.s");
        rtx gamma = gen_reg_rtx (DFmode);
        emit_insn (gen_kvx_fmuld (gamma, beta, doublea1, rns));
    // int64_t q1 = __builtin_kvx_fixedd(gamma, 0, ".rn.s");
        rtx q1 = gen_reg_rtx (DImode);
        emit_insn (gen_kvx_fixedd (q1, gamma, const0_rtx, rns));
    // int64_t rem = a1 - q1 * b;
        rtx rem = gen_reg_rtx (DImode);
        emit_insn (gen_msubdidi4 (rem, q1, operands[2], a1));
    // uint64_t quo = q0 + q1;
        rtx quo = gen_reg_rtx (DImode);
        emit_insn (gen_adddi3 (quo, q0, q1));
    // uint64_t cond = rem >> 63;
        rtx cond = gen_reg_rtx (DImode);
        emit_insn (gen_ashrdi3 (cond, rem, GEN_INT (63)));
    // uint64_t rem_b_cond = rem + (b & cond)
        rtx b_cond = gen_reg_rtx (DImode);
        emit_insn (gen_anddi3 (b_cond, operands[2], cond));
        rtx rem_b_cond = gen_reg_rtx (DImode);
        emit_insn (gen_adddi3 (rem_b_cond, rem, b_cond));
    // r = __builtin_kvx_selectd (rem + (b & cond), r, special, ".eqz");
        emit_insn (gen_kvx_selectd (r, rem_b_cond, r, special, deqz));
    // return r;
        emit_move_insn (operands[0], r);
      }
    DONE;
  }
)

(define_insn "mulditi3"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (mult:TI (sign_extend:TI (match_operand:DI 1 "register_operand" "r"))
                 (sign_extend:TI (match_operand:DI 2 "register_operand" "r"))))]
  ""
  "muldt %0 = %1, %2"
  [(set_attr "type" "mult_int")]
)

(define_insn "umulditi3"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                 (zero_extend:TI (match_operand:DI 2 "register_operand" "r"))))]
  ""
  "muludt %0 = %1, %2"
  [(set_attr "type" "mult_int")]
)

(define_insn "usmulditi3"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                 (sign_extend:TI (match_operand:DI 2 "register_operand" "r"))))]
  ""
  "mulsudt %0 = %2, %1"
  [(set_attr "type" "mult_int")]
)

(define_insn_and_split "smuldi3_highpart"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (truncate:DI
          (lshiftrt:TI
            (mult:TI
              (sign_extend:TI (match_operand:DI 1 "register_operand" "r"))
              (sign_extend:TI (match_operand:DI 2 "register_operand" "r")))
            (const_int 64))))
   (clobber (match_scratch:TI 3 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3)
        (mult:TI
          (sign_extend:TI (match_dup 1))
          (sign_extend:TI (match_dup 2))))
   (set (match_dup 0)
        (subreg:DI (match_dup 3) 8))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (TImode);
  }
)

(define_insn_and_split "umuldi3_highpart"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (truncate:DI
          (lshiftrt:TI
            (mult:TI
              (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
              (zero_extend:TI (match_operand:DI 2 "register_operand" "r")))
            (const_int 64))))
   (clobber (match_scratch:TI 3 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3)
        (mult:TI
          (zero_extend:TI (match_dup 1))
          (zero_extend:TI (match_dup 2))))
   (set (match_dup 0)
        (subreg:DI (match_dup 3) 8))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (TImode);
  }
)

(define_insn "madddidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (plus:DI (mult:DI (match_operand:DI 1 "register_operand" "r")
                          (match_operand:DI 2 "register_operand" "r"))
                 (match_operand:DI 3 "register_operand" "0")))]
  ""
  "maddd %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "maddditi4"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (plus:TI (mult:TI (sign_extend:TI (match_operand:DI 1 "register_operand" "r"))
                          (sign_extend:TI (match_operand:DI 2 "register_operand" "r")))
                 (match_operand:TI 3 "register_operand" "0")))]
  ""
  "madddt %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "umaddditi4"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (plus:TI (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                          (zero_extend:TI (match_operand:DI 2 "register_operand" "r")))
                 (match_operand:TI 3 "register_operand" "0")))]
  ""
  "maddudt %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "*madduzdt"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (plus:TI (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                          (zero_extend:TI (match_operand:DI 2 "register_operand" "r")))
                 (lshiftrt:TI (match_operand:TI 3 "register_operand" "0") (const_int 64))))]
  ""
  "madduzdt %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "*maddsudt"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (plus:TI (mult:TI (sign_extend:TI (match_operand:DI 1 "register_operand" "r"))
                          (zero_extend:TI (match_operand:DI 2 "register_operand" "r")))
                 (match_operand:TI 3 "register_operand" "0")))]
  ""
  "maddsudt %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "usmaddditi4"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (plus:TI (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                          (sign_extend:TI (match_operand:DI 2 "register_operand" "r")))
                 (match_operand:TI 3 "register_operand" "0")))]
  ""
  "maddsudt %0 = %2, %1"
  [(set_attr "type" "madd_int")]
)

(define_insn "msubdidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (minus:DI (match_operand:DI 3 "register_operand" "0")
                  (mult:DI (match_operand:DI 1 "register_operand" "r")
                           (match_operand:DI 2 "register_operand" "r"))))]
  ""
  "msbfd %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "msubditi4"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (minus:TI (match_operand:TI 3 "register_operand" "0")
                  (mult:TI (sign_extend:TI (match_operand:DI 1 "register_operand" "r"))
                           (sign_extend:TI (match_operand:DI 2 "register_operand" "r")))))]
  ""
  "msbfdt %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "umsubditi4"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (minus:TI (match_operand:TI 3 "register_operand" "0")
                  (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                           (zero_extend:TI (match_operand:DI 2 "register_operand" "r")))))]
  ""
  "msbfudt %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "*msbfuzdt"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (minus:TI (lshiftrt:TI (match_operand:TI 3 "register_operand" "0") (const_int 64))
                  (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                           (zero_extend:TI (match_operand:DI 2 "register_operand" "r")))))]
  ""
  "msbfuzdt %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "*msbfsudt"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (minus:TI (match_operand:TI 3 "register_operand" "0")
                  (mult:TI (sign_extend:TI (match_operand:DI 1 "register_operand" "r"))
                           (zero_extend:TI (match_operand:DI 2 "register_operand" "r")))))]
  ""
  "msbfsudt %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "usmsubditi4"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (minus:TI (match_operand:TI 3 "register_operand" "0")
                  (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                           (sign_extend:TI (match_operand:DI 2 "register_operand" "r")))))]
  ""
  "msbfsudt %0 = %2, %1"
  [(set_attr "type" "madd_int")]
)

(define_insn "smindi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (smin:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                 (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "mind %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "smaxdi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (smax:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                 (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "maxd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "umindi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (umin:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                 (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "minud %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "umaxdi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (umax:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                 (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "maxud %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "anddi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (and:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "andd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "*nandd"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (ior:DI (not:DI (match_operand:DI 1 "register_operand" "r,r,r,r"))
                (not:DI (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i"))))]
  ""
  "nandd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "*andnd"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (and:DI (not:DI (match_operand:DI 1 "register_operand" "r,r,r,r"))
                (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "andnd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "iordi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (ior:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "iord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "*nord"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (and:DI (not:DI (match_operand:DI 1 "register_operand" "r,r,r,r"))
                (not:DI (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i"))))]
  ""
  "niord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "*ornd"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (ior:DI (not:DI (match_operand:DI 1 "register_operand" "r,r,r,r"))
                (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "iornd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "xordi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (xor:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "xord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "*nxord"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (not:DI (xor:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                        (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i"))))]
  ""
  "nxord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn_and_split "rotldi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (rotate:DI (match_operand:DI 1 "register_operand" "r")
                   (match_operand:SI 2 "register_operand" "r")))
   (clobber (match_scratch:SI 3 "=&r"))
   (clobber (match_scratch:DI 4 "=&r"))
   (clobber (match_scratch:DI 5 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3) (neg:SI (match_dup 2)))
   (set (match_dup 4) (ashift:DI (match_dup 1) (match_dup 2)))
   (set (match_dup 5) (lshiftrt:DI (match_dup 1) (match_dup 3)))
   (set (match_dup 0) (ior:DI (match_dup 4) (match_dup 5)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (SImode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (DImode);
    if (GET_CODE (operands[5]) == SCRATCH)
      operands[5] = gen_reg_rtx (DImode);
  }
)

(define_insn_and_split "rotrdi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (rotatert:DI (match_operand:DI 1 "register_operand" "r")
                     (match_operand:SI 2 "register_operand" "r")))
   (clobber (match_scratch:SI 3 "=&r"))
   (clobber (match_scratch:DI 4 "=&r"))
   (clobber (match_scratch:DI 5 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3) (neg:SI (match_dup 2)))
   (set (match_dup 4) (lshiftrt:DI (match_dup 1) (match_dup 2)))
   (set (match_dup 5) (ashift:DI (match_dup 1) (match_dup 3)))
   (set (match_dup 0) (ior:DI (match_dup 4) (match_dup 5)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (SImode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (DImode);
    if (GET_CODE (operands[5]) == SCRATCH)
      operands[5] = gen_reg_rtx (DImode);
  }
)

(define_insn "bswapdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (bswap:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "sbmm8 %0 = %1, 0x0102040810204080"
  [(set_attr "type" "alu_thin_y")
   (set_attr "length"       "12")]
)

(define_insn "clrsbdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (clrsb:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "clsd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "clzdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (clz:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "clzd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "ctzdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (ctz:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "ctzd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "popcountdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (popcount:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "cbsd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "one_cmpldi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (not:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "notd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)


;; TI

(define_insn "kvx_tilow64"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (subreg:DI (match_operand:TI 1 "nonimmediate_operand" "r") 0))]
  ""
  "copyd %0 = %x1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_tihigh64"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (subreg:DI (match_operand:TI 1 "nonimmediate_operand" "r") 8))]
  ""
  "copyd %0 = %y1"
  [(set_attr "type" "alu_tiny")]
)

(define_expand "addti3"
  [(set (match_operand:TI 0 "register_operand" "")
        (plus:TI (match_operand:TI 1 "register_operand" "")
                 (match_operand:TI 2 "register_s32_operand" "")))]
  ""
)

(define_insn_and_split "*addti3_reg"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (plus:TI (match_operand:TI 1 "register_operand" "r")
                 (match_operand:TI 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(parallel
    [(set (subreg:DI (match_dup 0) 0)
          (unspec:DI [(subreg:DI (match_dup 1) 0)
                      (subreg:DI (match_dup 2) 0)
                      (const_string ".i")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_ADDCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 1) 0)
                      (subreg:DI (match_dup 2) 0)] UNSPEC_CARRY))])
   (parallel
    [(set (subreg:DI (match_dup 0) 8)
          (unspec:DI [(subreg:DI (match_dup 1) 8)
                      (subreg:DI (match_dup 2) 8)
                      (const_string "")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_ADDCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 1) 8)
                      (subreg:DI (match_dup 2) 8)] UNSPEC_CARRY))])]
)

(define_insn_and_split "*addti3_pos32"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (plus:TI (match_operand:TI 1 "register_operand" "r")
                 (match_operand:TI 2 "const_pos32_operand" "i")))]
  ""
  "#"
  "reload_completed"
  [(parallel
    [(set (subreg:DI (match_dup 0) 0)
          (unspec:DI [(subreg:DI (match_dup 1) 0)
                      (match_dup 2)
                      (const_string ".i")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_ADDCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 1) 0)
                      (match_dup 2)] UNSPEC_CARRY))])
   (parallel
    [(set (subreg:DI (match_dup 0) 8)
          (unspec:DI [(subreg:DI (match_dup 1) 8)
                      (const_int 0)
                      (const_string "")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_ADDCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 1) 8)
                      (const_int 0)] UNSPEC_CARRY))])]
)

(define_insn_and_split "*addti3_neg32"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (plus:TI (match_operand:TI 1 "register_operand" "r")
                 (match_operand:TI 2 "const_neg32_operand" "i")))]
  ""
  "#"
  "reload_completed"
  [(parallel
    [(set (subreg:DI (match_dup 0) 0)
          (unspec:DI [(subreg:DI (match_dup 1) 0)
                      (match_dup 2)
                      (const_string ".i")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_ADDCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 1) 0)
                      (match_dup 2)] UNSPEC_CARRY))])
   (parallel
    [(set (subreg:DI (match_dup 0) 8)
          (unspec:DI [(subreg:DI (match_dup 1) 8)
                      (const_int -1)
                      (const_string "")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_ADDCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 1) 8)
                      (const_int -1)] UNSPEC_CARRY))])]
)

(define_expand "subti3"
  [(set (match_operand:TI 0 "register_operand" "")
        (minus:TI (match_operand:TI 1 "register_operand" "")
                  (match_operand:TI 2 "register_operand" "")))]
  ""
)

(define_insn_and_split "*subti3_reg"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (minus:TI (match_operand:TI 1 "register_operand" "r")
                  (match_operand:TI 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(parallel
    [(set (subreg:DI (match_dup 0) 0)
          (unspec:DI [(subreg:DI (match_dup 2) 0)
                      (subreg:DI (match_dup 1) 0)
                      (const_string ".i")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_SBFCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 2) 0)
                      (subreg:DI (match_dup 1) 0)] UNSPEC_BORROW))])
   (parallel
    [(set (subreg:DI (match_dup 0) 8)
          (unspec:DI [(subreg:DI (match_dup 2) 8)
                      (subreg:DI (match_dup 1) 8)
                      (const_string "")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_SBFCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 2) 8)
                      (subreg:DI (match_dup 1) 8)] UNSPEC_BORROW))])]
)

(define_insn_and_split "*subti3_pos32"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (minus:TI (match_operand:TI 1 "const_pos32_operand" "i")
                  (match_operand:TI 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(parallel
    [(set (subreg:DI (match_dup 0) 0)
          (unspec:DI [(subreg:DI (match_dup 2) 0)
                      (match_dup 1)
                      (const_string ".i")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_SBFCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 2) 0)
                      (match_dup 1)] UNSPEC_BORROW))])
   (parallel
    [(set (subreg:DI (match_dup 0) 8)
          (unspec:DI [(subreg:DI (match_dup 2) 8)
                      (const_int 0)
                      (const_string "")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_SBFCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 2) 8)
                      (const_int 0)] UNSPEC_BORROW))])]
)

(define_insn_and_split "*subti3_neg32"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (minus:TI (match_operand:TI 1 "const_neg32_operand" "i")
                  (match_operand:TI 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(parallel
    [(set (subreg:DI (match_dup 0) 0)
          (unspec:DI [(subreg:DI (match_dup 2) 0)
                      (match_dup 1)
                      (const_string ".i")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_SBFCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 2) 0)
                      (match_dup 1)] UNSPEC_BORROW))])
   (parallel
    [(set (subreg:DI (match_dup 0) 8)
          (unspec:DI [(subreg:DI (match_dup 2) 8)
                      (const_int -1)
                      (const_string "")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_SBFCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 2) 8)
                      (const_int -1)] UNSPEC_BORROW))])]
)

(define_insn_and_split "negti2"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (neg:TI (match_operand:TI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(parallel
    [(set (subreg:DI (match_dup 0) 0)
          (unspec:DI [(subreg:DI (match_dup 1) 0)
                      (const_int 0)
                      (const_string ".i")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_SBFCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 1) 0)
                      (const_int 0)] UNSPEC_BORROW))])
   (parallel
    [(set (subreg:DI (match_dup 0) 8)
          (unspec:DI [(subreg:DI (match_dup 1) 8)
                      (const_int 0)
                      (const_string "")
                      (reg:DI KVX_CS_REGNO)] UNSPEC_SBFCD))
     (set (reg:DI KVX_CS_REGNO)
          (unspec:DI [(reg:DI KVX_CS_REGNO)
                      (subreg:DI (match_dup 1) 8)
                      (const_int 0)] UNSPEC_BORROW))])]
)

(define_expand "multi3"
  [(set (match_operand:TI 0 "register_operand")
        (mult:TI (match_operand:TI 1 "register_operand")
                 (match_operand:TI 2 "nonmemory_operand")))]
  ""
  {
    if (!kvx_expand_memset_mul (operands, TImode))
      {
        operands[2] = force_reg (TImode, operands[2]);
        rtx hi_0 = simplify_gen_subreg (DImode, operands[0], TImode, 8);
        rtx lo_1 = simplify_gen_subreg (DImode, operands[1], TImode, 0);
        rtx hi_1 = simplify_gen_subreg (DImode, operands[1], TImode, 8);
        rtx lo_2 = simplify_gen_subreg (DImode, operands[2], TImode, 0);
        rtx hi_2 = simplify_gen_subreg (DImode, operands[2], TImode, 8);
        emit_insn (gen_umulditi3 (operands[0], lo_1, lo_2));
        emit_insn (gen_madddidi4 (hi_0, lo_1, hi_2, hi_0));
        emit_insn (gen_madddidi4 (hi_0, hi_1, lo_2, hi_0));
      }
    DONE;
  }
)

(define_insn_and_split "ashlti3"
  [(set (match_operand:TI 0 "register_operand" "=&r")
        (ashift:TI (match_operand:TI 1 "register_operand" "r")
                   (match_operand:SI 2 "const_ge64_operand" "i")))]
  ""
  "#"
  "reload_completed"
  ;; "make %x0 = 0\n\tslld %y0 = %x1, (%2 & 63)"
  [(set (subreg:DI (match_dup 0) 0)
        (const_int 0))
   (set (subreg:DI (match_dup 0) 8)
        (ashift:DI (subreg:DI (match_dup 1) 0) (match_dup 2)))]
  {
    gcc_checking_assert (CONST_INT_P (operands[2]));
    operands[2] = GEN_INT (INTVAL (operands[2]) & 63);
  }
  [(set_attr "type" "alu_tiny_x2")]
)

(define_insn_and_split "ashrti3"
  [(set (match_operand:TI 0 "register_operand" "=&r")
        (ashiftrt:TI (match_operand:TI 1 "register_operand" "r")
                     (match_operand:SI 2 "const_ge64_operand" "i")))]
  ""
  "#"
  "reload_completed"
  ;; "srad %x0 = %y1, (%2 & 63)\n\tsrad %y0 = %y1, 63"
  [(set (subreg:DI (match_dup 0) 0)
        (ashiftrt:DI (subreg:DI (match_dup 1) 8) (match_dup 2)))
   (set (subreg:DI (match_dup 0) 8)
        (ashiftrt:DI (subreg:DI (match_dup 1) 8) (const_int 63)))]
  {
    gcc_checking_assert (CONST_INT_P (operands[2]));
    operands[2] = GEN_INT (INTVAL (operands[2]) & 63);
  }
  [(set_attr "type" "alu_tiny_x2")]
)

(define_insn_and_split "lshrti3"
  [(set (match_operand:TI 0 "register_operand" "=&r")
        (lshiftrt:TI (match_operand:TI 1 "register_operand" "r")
                     (match_operand:SI 2 "const_ge64_operand" "i")))]
  ""
  "#"
  "reload_completed"
  ;; "srld %x0 = %y1, (%2 & 63)\n\tmake %y0 = 0"
  [(set (subreg:DI (match_dup 0) 0)
        (lshiftrt:DI (subreg:DI (match_dup 1) 8) (match_dup 2)))
   (set (subreg:DI (match_dup 0) 8) (const_int 0))]
  {
    gcc_checking_assert (CONST_INT_P (operands[2]));
    operands[2] = GEN_INT (INTVAL (operands[2]) & 63);
  }
  [(set_attr "type" "alu_tiny_x2")]
)


;; OI

;; Pattern for the expansion of the stored constant in memset.
(define_expand "muloi3"
  [(set (match_operand:OI 0 "register_operand")
        (mult:OI (match_operand:OI 1 "register_operand")
                 (match_operand:OI 2 "nonmemory_operand")))]
  ""
  {
    if (kvx_expand_memset_mul (operands, OImode))
      DONE;
    FAIL;
  }
)

;; CPLX_I

(define_insn "add<mode>3"
  [(set (match_operand:CPLX_I 0 "register_operand" "=r,r")
          (plus:CPLX_I
           (match_operand:CPLX_I 1 "register_operand" "r,r")
           (match_operand:CPLX_I 2 "register_s32_operand" "r,i")))]
  ""
  "add<suffix> %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny_x")
   (set_attr "length" "4,8")]
)

(define_insn "sub<mode>3"
  [(set (match_operand:CPLX_I 0 "register_operand" "=r,r")
          (minus:CPLX_I
           (match_operand:CPLX_I 1 "register_s32_operand" "r,i")
           (match_operand:CPLX_I 2 "register_operand" "r,r")))]
  ""
  "sbf<suffix> %0 = %2, %1"
  [(set_attr "type"   "alu_tiny,alu_tiny_x")
   (set_attr "length" "4,8")]
)

(define_insn "neg<mode>2"
  [(set (match_operand:CPLX_I 0 "register_operand" "=r")
          (neg:CPLX_I (match_operand:CPLX_I 1 "register_operand" "r")))]
  ""
  "neg<suffix> %0 = %1"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length" "8")]
)

(define_insn "addconj<mode>3"
  [(set (match_operand:CPLX_C 0 "register_operand" "=r,r")
          (plus:CPLX_C
           (conj:CPLX_C (match_operand:CPLX_C 1 "register_operand" "r,r"))
           (match_operand:CPLX_C 2 "register_s32_operand" "r,i")))]
  "KV3_1"
  "add<suffixc> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "4,8")]
)

(define_insn "subconj<mode>3"
  [(set (match_operand:CPLX_C 0 "register_operand" "=r,r")
          (minus:CPLX_C
           (match_operand:CPLX_C 1 "register_s32_operand" "r,i")
           (conj:CPLX_C (match_operand:CPLX_C 2 "register_operand" "r,r"))))]
  "KV3_1"
  "sbf<suffixc> %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x")
   (set_attr "length" "4,8")]
)

(define_expand "conj<mode>2"
  [(set (match_operand:CPLX_C 0 "register_operand" "=r")
        (conj:CPLX_C (match_operand:CPLX_C 1 "register_operand" "r")))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_addconj<mode>3 (operands[0], operands[1], gen_rtx_CONST_INT (<MODE>mode, 0)));
    else if ((KV3_2||KV4))
      emit_insn (gen_conj<mode>2_2 (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "conj<mode>2_2"
  [(set (match_operand:CPLX_C 0 "register_operand" "=r")
        (conj:CPLX_C (match_operand:CPLX_C 1 "register_operand" "r")))
   (clobber (match_scratch:CPLX_C 2 "=&r"))]
  "(KV3_2||KV4)"
  "#"
  "(KV3_2||KV4) && reload_completed"
  [(set (match_dup 2) (match_dup 1))
   (set (match_dup 0)
        (neg:<MODE> (match_dup 1)))
   (set (zero_extract:<HWIDE> (subreg:<HWIDE> (match_dup 0) 0)
                        (const_int <innersize>)
                        (const_int 0))
        (subreg:<HWIDE> (match_dup 2) 0))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (<MODE>mode);
  }
)

;; CQI

(define_insn_and_split "conjcqi2"
  [(set (match_operand:CQI 0 "register_operand" "=r")
        (conj:CQI (match_operand:CQI 1 "register_operand" "r")))
   (clobber (match_scratch:CQI 2 "=&r"))]
  "(KV3_2||KV4)"
  "#"
  "(KV3_2||KV4)"
  [(set (match_dup 2) (match_dup 1))
   (set (match_dup 0)
        (neg:CQI (match_dup 1)))
   (set (zero_extract:SI (subreg:SI (match_dup 0) 0)
                        (const_int 8)
                        (const_int 0))
        (subreg:SI (match_dup 2) 0))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (CQImode);
  }
)

;; CSI

(define_expand "mulcsi3"
[(set (match_operand:CSI 0 "register_operand" "=r")
      (mult:CSI (match_operand:CSI 1 "register_operand" "r")
                (match_operand:CSI 2 "register_operand" "r")))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_mulcsi3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      {
        rtx temp0 = gen_reg_rtx (V2CSImode);
        rtx temp0_lo = gen_lowpart (CSImode, temp0);
        rtx temp0_hi = gen_highpart (CSImode, temp0);
        rtx temp0_lo_di = simplify_gen_subreg (DImode, temp0_lo, CSImode, 0);
        rtx temp0_hi_di = simplify_gen_subreg (DImode, temp0_hi, CSImode, 0);
        rtx op0_di = simplify_gen_subreg (DImode, operands[0], CSImode, 0);
        emit_insn (gen_kvx_mm212w (simplify_gen_subreg (V4SImode, temp0, V2CSImode, 0),
                                   simplify_gen_subreg (V2SImode, operands[1], CSImode, 0),
                                   simplify_gen_subreg (V2SImode, operands[2], CSImode, 0)));
        emit_insn (gen_kvx_sbmm8d (temp0_hi_di, temp0_hi_di, gen_rtx_CONST_INT (DImode, 0x0804020180402010)));
        emit_insn (gen_addcsi3 (operands[0], temp0_lo, temp0_hi));
        emit_insn (gen_subcsi3 (temp0_lo, temp0_lo, temp0_hi));
        emit_insn (gen_insvdi (op0_di, GEN_INT (32), GEN_INT (0), temp0_lo_di));
      }
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "mulcsi3_1"
  [(set (match_operand:CSI 0 "register_operand" "=r,r,r,r")
        (mult:CSI (match_operand:CSI 1 "register_operand" "r,r,r,r")
                 (match_operand:CSI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  "KV3_1"
  "mulwc %0 = %1, %2"
  [(set_attr "type" "mult_int,mult_int,mult_int_x,mult_int_y")
   (set_attr "length"      "4,       4,         8,        12")]
)

(define_insn "mulconjcsi3"
  [(set (match_operand:CSI 0 "register_operand" "=r")
        (mult:CSI (conj:CSI (match_operand:CSI 1 "register_operand" "r"))
                  (match_operand:CSI 2 "register_operand" "r")))]
  "KV3_1"
  "mulwc.c %0 = %1, %2"
  [(set_attr "type"   "mult_int")]
)

;; CDI

(define_insn "addcdi3"
  [(set (match_operand:CDI 0 "register_operand" "=r")
          (plus:CDI
           (match_operand:CDI 1 "register_operand" "r")
           (match_operand:CDI 2 "register_operand" "r")))]
  ""
  "addd %x0 = %x1, %x2\n\taddd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length" "8")]
)

(define_insn "subcdi3"
  [(set (match_operand:CDI 0 "register_operand" "=r")
        (minus:CDI (match_operand:CDI 1 "register_operand" "r")
                  (match_operand:CDI 2 "register_operand" "r")))]
  ""
  "sbfd %x0 = %x2, %x1\n\tsbfd %y0 = %y2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length" "8")]
)

(define_insn "negcdi2"
  [(set (match_operand:CDI 0 "register_operand" "=r")
          (neg:CDI (match_operand:CDI 1 "register_operand" "r")))]
  ""
  "negd %x0 = %x1\n\tnegd %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length" "8")]
)

(define_insn "conjcdi2"
  [(set (match_operand:CDI 0 "register_operand" "=r")
          (conj:CDI (match_operand:CDI 1 "register_operand" "r")))]
  ""
  "copyd %x0 = %x1\n\tnegd %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length" "8")]
)

;; HF

(define_insn "addhf3"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (plus:HF (match_operand:HF 1 "register_operand" "r")
                 (match_operand:HF 2 "register_operand" "r")))]
  ""
  "faddhq %0 = %1, %2"
  [(set_attr "type" "mult_fp3")]
)

(define_insn "subhf3"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (minus:HF (match_operand:HF 1 "register_operand" "r")
                  (match_operand:HF 2 "register_operand" "r")))]
  ""
  "fsbfhq %0 = %2, %1"
  [(set_attr "type" "mult_fp3")]
)

(define_insn "mulhf3"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (mult:HF (match_operand:HF 1 "register_operand" "r")
                 (match_operand:HF 2 "register_operand" "r")))]
  ""
  "fmulhq %0 = %1, %2"
  [(set_attr "type" "mult_fp3")]
)

(define_insn "*fmulhw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (mult:SF (float_extend:SF (match_operand:HF 1 "register_operand" "r"))
                 (float_extend:SF (match_operand:HF 2 "register_operand" "r"))))]
  "KV3"
  "fmulhw %0 = %1, %2"
  [(set_attr "type" "mult_fp3")]
)

(define_expand "divhf3"
  [(set (match_operand:HF 0 "register_operand" "")
        (div:HF (match_operand:HF 1 "register_float1_operand" "")
                (match_operand:HF 2 "register_operand" "")))]
  ""
  {
    rtx a = gen_reg_rtx(SFmode);
    rtx b = gen_reg_rtx(SFmode);
    rtx r = gen_reg_rtx(SFmode);
    rtx d = gen_reg_rtx(SFmode);
    emit_insn (gen_extendhfsf2 (a, operands[1]));
    emit_insn (gen_extendhfsf2 (b, operands[2]));
    rtx rm = gen_rtx_CONST_STRING (VOIDmode, "");
    emit_insn (gen_kvx_frecw (r, b, rm));
    emit_insn (gen_mulsf3 (d, a, r));
    emit_insn (gen_truncsfhf2 (operands[0], d));
    DONE;
  }
)

(define_insn "fmahf4"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (fma:HF  (match_operand:HF 1 "register_operand" "r")
                 (match_operand:HF 2 "register_operand" "r")
                 (match_operand:HF 3 "register_operand" "0")))]
  ""
  "ffmahq %0 = %1, %2"
  [(set_attr "type" "madd_fp3")]
)

(define_insn "*ffmahw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (fma:SF  (float_extend:SF (match_operand:HF 1 "register_operand" "r"))
                 (float_extend:SF (match_operand:HF 2 "register_operand" "r"))
                 (match_operand:SF 3 "register_operand" "0")))]
  "KV3"
  "ffmahw %0 = %1, %2"
  [(set_attr "type" "madd_fp3")]
)

(define_insn "fnmahf4"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (fma:HF  (neg:HF (match_operand:HF 1 "register_operand" "r"))
                 (match_operand:HF 2 "register_operand" "r")
                 (match_operand:HF 3 "register_operand" "0")))]
  ""
  "ffmshq %0 = %1, %2"
  [(set_attr "type" "madd_fp3")]
)

(define_insn "*ffmshw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (fma:SF (float_extend:SF (neg:HF (match_operand:HF 1 "register_operand" "r")))
                (float_extend:SF (match_operand:HF 2 "register_operand" "r"))
                (match_operand:SF 3 "register_operand" "0")))]
  "KV3"
  "ffmshw %0 = %1, %2"
  [(set_attr "type" "madd_fp3")]
)

(define_insn "*ffmshw2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (fma:SF (float_extend:SF (match_operand:HF 1 "register_operand" "r"))
                (float_extend:SF (neg:HF (match_operand:HF 2 "register_operand" "r")))
                (match_operand:SF 3 "register_operand" "0")))]
  "KV3"
  "ffmshw %0 = %1, %2"
  [(set_attr "type" "madd_fp3")]
)

(define_insn "fminhf3"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (smin:HF (match_operand:HF 1 "register_operand" "r")
                 (match_operand:HF 2 "register_operand" "r")))]
  ""
  "fminhq %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "fmaxhf3"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (smax:HF (match_operand:HF 1 "register_operand" "r")
                 (match_operand:HF 2 "register_operand" "r")))]
  ""
  "fmaxhq %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "neghf2"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (neg:HF (match_operand:HF 1 "register_operand" "r")))]
  ""
  "fneghq %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "abshf2"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (abs:HF (match_operand:HF 1 "register_operand" "r")))]
  ""
  "fabshq %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_getsignh"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI  [(match_operand:HF 1 "register_operand" "r")] UNSPEC_GETSIGN))]
  ""
  "extfs %0 = %1, 15, 15"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_setsignh"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (unspec:HF [(match_operand:HF 1 "register_operand" "0")
                    (match_operand:DI 2 "register_operand" "r")] UNSPEC_SETSIGN))]
  ""
  "insf %0 = %2, 15, 15"
  [(set_attr "type" "alu_thin")]
)

(define_expand "copysignhf3"
  [(match_operand:HF 0 "register_operand")
   (match_operand:HF 1 "register_operand")
   (match_operand:HF 2 "register_operand")]
  ""
  {
    rtx sign2 = gen_reg_rtx (DImode);
    emit_insn (gen_kvx_getsignh (sign2, operands[2]));
    emit_insn (gen_kvx_setsignh (operands[0], operands[1], sign2));
    DONE;
  }
)

(define_expand "xorsignhf3"
  [(match_operand:HF 0 "register_operand")
   (match_operand:HF 1 "register_operand")
   (match_operand:HF 2 "register_operand")]
  ""
  {
    rtx mask = GEN_INT (0x8000);
    rtx sign2 = gen_reg_rtx (HFmode);
    emit_insn (gen_rtx_SET (sign2, gen_rtx_UNSPEC (HFmode, gen_rtvec (2, operands[2], mask), UNSPEC_ANDD)));
    emit_insn (gen_rtx_SET (operands[0], gen_rtx_UNSPEC (HFmode, gen_rtvec (2, operands[1], sign2), UNSPEC_XORD)));
    DONE;
  }
)

(define_expand "floatsihf2"
  [(set (match_operand:HF 0 "register_operand" "")
        (float:HF (match_operand:SI 1 "register_operand" "")))]
  ""
  {
    rtx tempsf = gen_reg_rtx (SFmode);
    emit_insn (gen_floatsisf2 (tempsf, operands[1]));
    emit_insn (gen_truncsfhf2 (operands[0], tempsf));
    DONE;
  }
)

(define_expand "floatdihf2"
  [(set (match_operand:HF 0 "register_operand" "")
        (float:HF (match_operand:DI 1 "register_operand" "")))]
  ""
  {
    rtx tempdf = gen_reg_rtx (DFmode);
    emit_insn (gen_floatdidf2 (tempdf, operands[1]));
    rtx tempsf = gen_reg_rtx (SFmode);
    emit_insn (gen_truncdfsf2 (tempsf, tempdf));
    emit_insn (gen_truncsfhf2 (operands[0], tempsf));
    DONE;
  }
)

(define_expand "floatunssihf2"
  [(set (match_operand:HF 0 "register_operand" "")
        (unsigned_float:HF (match_operand:SI 1 "register_operand" "")))]
  ""
  {
    rtx tempsf = gen_reg_rtx (SFmode);
    emit_insn (gen_floatunssisf2 (tempsf, operands[1]));
    emit_insn (gen_truncsfhf2 (operands[0], tempsf));
    DONE;
  }
)

(define_expand "floatunsdihf2"
  [(set (match_operand:HF 0 "register_operand" "")
        (unsigned_float:HF (match_operand:DI 1 "register_operand" "")))]
  ""
  {
    rtx tempdf = gen_reg_rtx (DFmode);
    emit_insn (gen_floatunsdidf2 (tempdf, operands[1]));
    rtx tempsf = gen_reg_rtx (SFmode);
    emit_insn (gen_truncdfsf2 (tempsf, tempdf));
    emit_insn (gen_truncsfhf2 (operands[0], tempsf));
    DONE;
  }
)

(define_insn "extendhfsf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (float_extend:SF (match_operand:HF 1 "register_operand" "r")))]
  ""
  "fwidenlhw %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_expand "extendhfdf2"
  [(set (match_operand:DF 0 "register_operand" "")
        (float_extend:DF (match_operand:HF 1 "register_operand" "")))]
  ""
  {
    rtx tempsf = gen_reg_rtx (SFmode);
    emit_insn (gen_extendhfsf2 (tempsf, operands[1]));
    emit_insn (gen_extendsfdf2 (operands[0], tempsf));
    DONE;
  }
)


;; SF

(define_insn "addsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (plus:SF (match_operand:SF 1 "register_operand" "r")
                 (match_operand:SF 2 "register_operand" "r")))]
  ""
  "faddw %0 = %1, %2"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "subsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (minus:SF (match_operand:SF 1 "register_operand" "r")
                  (match_operand:SF 2 "register_operand" "r")))]
  ""
  "fsbfw %0 = %2, %1"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "mulsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (mult:SF (match_operand:SF 1 "register_operand" "r")
                 (match_operand:SF 2 "register_operand" "r")))]
  ""
  "fmulw %0 = %1, %2"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "*fmulwd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (mult:DF (float_extend:DF (match_operand:SF 1 "register_operand" "r"))
                 (float_extend:DF (match_operand:SF 2 "register_operand" "r"))))]
  "KV3"
  "fmulwd %0 = %1, %2"
  [(set_attr "type" "mult_fp4")]
)

(define_expand "divsf3"
  [(set (match_operand:SF 0 "register_operand" "")
        (div:SF (match_operand:SF 1 "register_float1_operand" "")
                (match_operand:SF 2 "register_operand" "")))]
  ""
  {
    rtx rm = gen_rtx_CONST_STRING (VOIDmode, "");
    rtx rn = gen_rtx_CONST_STRING (VOIDmode, ".rn");
    rtx a = operands[1], b = operands[2];
    if (a == CONST1_RTX (SFmode))
      {
        emit_insn (gen_kvx_frecw (operands[0], b, rm));
      }
    else if (flag_reciprocal_math)
      {
        rtx t = gen_reg_rtx(SFmode);
        emit_insn (gen_kvx_frecw (t, b, rm));
        emit_insn (gen_kvx_fmulw (operands[0], a, t, rm));
      }
    else if (flag_unsafe_math_optimizations)
      {
        rtx re = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_frecw (re, b, rn));
        rtx y0 = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_fmulw (y0, a, re, rn));
        rtx e0 = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_ffmsw (e0, b, y0, a, rn));
        rtx y1 = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_ffmaw (y1, e0, re, y0, rn));
        rtx e1 = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_ffmsw (e1, b, y1, a, rn));
        rtx y2 = operands[0];
        emit_insn (gen_kvx_ffmaw (y2, e1, re, y1, rm));
      }
    else
      {
        emit_library_call_value
          (gen_rtx_SYMBOL_REF (Pmode, "__divsf3"),
          operands[0], LCT_CONST, SFmode,
          operands[1], SFmode, operands[2], SFmode);
      }
    DONE;
  }
)

(define_insn "fmasf4"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (fma:SF  (match_operand:SF 1 "register_operand" "r")
                 (match_operand:SF 2 "register_operand" "r")
                 (match_operand:SF 3 "register_operand" "0")))]
  ""
  "ffmaw %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "*ffmawd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (fma:DF  (float_extend:DF (match_operand:SF 1 "register_operand" "r"))
                 (float_extend:DF (match_operand:SF 2 "register_operand" "r"))
                 (match_operand:DF 3 "register_operand" "0")))]
  "KV3"
  "ffmawd %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "fnmasf4"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (fma:SF  (neg:SF (match_operand:SF 1 "register_operand" "r"))
                 (match_operand:SF 2 "register_operand" "r")
                 (match_operand:SF 3 "register_operand" "0")))]
  ""
  "ffmsw %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "*ffmswd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (fma:DF (float_extend:DF (neg:SF (match_operand:SF 1 "register_operand" "r")))
                (float_extend:DF (match_operand:SF 2 "register_operand" "r"))
                (match_operand:DF 3 "register_operand" "0")))]
  "KV3"
  "ffmswd %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "*ffmswd2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (fma:DF (float_extend:DF (match_operand:SF 1 "register_operand" "r"))
                (float_extend:DF (neg:SF (match_operand:SF 2 "register_operand" "r")))
                (match_operand:DF 3 "register_operand" "0")))]
  "KV3"
  "ffmswd %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "fminsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (smin:SF (match_operand:SF 1 "register_operand" "r")
                 (match_operand:SF 2 "register_operand" "r")))]
  ""
  "fminw %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "fmaxsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (smax:SF (match_operand:SF 1 "register_operand" "r")
                 (match_operand:SF 2 "register_operand" "r")))]
  ""
  "fmaxw %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "negsf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (neg:SF (match_operand:SF 1 "register_operand" "r")))]
  ""
  "fnegw %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "abssf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (abs:SF (match_operand:SF 1 "register_operand" "r")))]
  ""
  "fabsw %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_getsignw"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI  [(match_operand:SF 1 "register_operand" "r")] UNSPEC_GETSIGN))]
  ""
  "extfs %0 = %1, 31, 31"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_setsignw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "0")
                    (match_operand:DI 2 "register_operand" "r")] UNSPEC_SETSIGN))]
  ""
  "insf %0 = %2, 31, 31"
  [(set_attr "type" "alu_thin")]
)

(define_expand "copysignsf3"
  [(match_operand:SF 0 "register_operand")
   (match_operand:SF 1 "register_operand")
   (match_operand:SF 2 "register_operand")]
  ""
  {
    rtx sign2 = gen_reg_rtx (DImode);
    emit_insn (gen_kvx_getsignw (sign2, operands[2]));
    emit_insn (gen_kvx_setsignw (operands[0], operands[1], sign2));
    DONE;
  }
)

(define_expand "xorsignsf3"
  [(match_operand:SF 0 "register_operand")
   (match_operand:SF 1 "register_operand")
   (match_operand:SF 2 "register_operand")]
  ""
  {
    rtx mask = GEN_INT (0x80000000);
    rtx sign2 = gen_reg_rtx (SFmode);
    emit_insn (gen_rtx_SET (sign2, gen_rtx_UNSPEC (SFmode, gen_rtvec (2, operands[2], mask), UNSPEC_ANDD)));
    emit_insn (gen_rtx_SET (operands[0], gen_rtx_UNSPEC (SFmode, gen_rtvec (2, operands[1], sign2), UNSPEC_XORD)));
    DONE;
  }
)

(define_insn "floatsisf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (float:SF (match_operand:SI 1 "register_operand" "r")))]
  ""
  {
    if (KV3)
      return "floatw.rn %0 = %1, 0";
    return "floatw.rn %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)

(define_insn "floatunssisf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unsigned_float:SF (match_operand:SI 1 "register_operand" "r")))]
  ""
  {
    if (KV3)
      return "floatuw.rn %0 = %1, 0";
    return "floatuw.rn %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)

(define_expand "floatdisf2"
  [(set (match_operand:SF 0 "register_operand" "")
        (float:SF (match_operand:DI 1 "register_operand" "")))]
  ""
  {
    if (!flag_unsafe_math_optimizations)
      {
        rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__floatdisf"),
                                            operands[0], LCT_CONST, SFmode,
                                            operands[1], DImode);
        if (dest != operands[0])
          emit_move_insn (operands[0], dest);
      }
    else
      {
        rtx temp = gen_reg_rtx (DFmode);
        emit_insn (gen_rtx_SET (temp, gen_rtx_FLOAT (DFmode, operands[1])));
        emit_insn (gen_rtx_SET (operands[0], gen_rtx_FLOAT_TRUNCATE (SFmode, temp)));
      }
    DONE;
  }
)

(define_expand "floatunsdisf2"
  [(set (match_operand:SF 0 "register_operand" "")
        (unsigned_float:SF (match_operand:DI 1 "register_operand" "")))]
  ""
  {
    if (!flag_unsafe_math_optimizations)
      {
        rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__floatundisf"),
                                            operands[0], LCT_CONST, SFmode,
                                            operands[1], DImode);
        if (dest != operands[0])
          emit_move_insn (operands[0], dest);
      }
    else
      {
        rtx temp = gen_reg_rtx (DFmode);
        emit_insn (gen_rtx_SET (temp, gen_rtx_UNSIGNED_FLOAT (DFmode, operands[1])));
        emit_insn (gen_rtx_SET (operands[0], gen_rtx_FLOAT_TRUNCATE (SFmode, temp)));
      }
    DONE;
  }
)

(define_insn "fix_truncsfsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (fix:SI (match_operand:SF 1 "register_operand" "r")))]
  ""
  {
    if (KV3)
      return "fixedw.rz %0 = %1, 0";
    return "fixedw.rz %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)
;; zero-extend version of fix_truncsfsi2
(define_insn "*fix_truncsfsi2_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (fix:SI (match_operand:SF 1 "register_operand" "r"))))]
  ""
  {
    if (KV3)
      return "fixedw.rz %0 = %1, 0";
    return "fixedw.rz %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)

(define_insn "fixuns_truncsfsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unsigned_fix:SI (match_operand:SF 1 "register_operand" "r")))]
  ""
  {
    if (KV3)
      return "fixeduw.rz %0 = %1, 0";
    return "fixeduw.rz %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)
;; zero-extend version of fixuns_truncsfsi2
(define_insn "*fixuns_truncsfsi2_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (unsigned_fix:SI (match_operand:SF 1 "register_operand" "r"))))]
  ""
  {
    if (KV3)
      return "fixeduw.rz %0 = %1, 0";
    return "fixeduw.rz %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)

(define_insn "truncsfhf2"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (float_truncate:HF (match_operand:SF 1 "register_operand" "r")))]
  ""
  "fnarrowwh %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "extendsfdf2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (float_extend:DF (match_operand:SF 1 "register_operand" "r")))]
  ""
  "fwidenlwd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_expand "sqrtsf2"
  [(match_operand:SF 0 "register_operand" "")
   (match_operand:SF 1 "register_operand" "")]
  "flag_reciprocal_math"
  {
    rtx tempsf = gen_reg_rtx(SFmode);
    rtx rm = gen_rtx_CONST_STRING (VOIDmode, "");
    emit_insn (gen_kvx_frsrw (tempsf, operands[1], rm));
    emit_insn (gen_mulsf3 (operands[0], operands[1], tempsf));
    DONE;
  }
)

(define_expand "rsqrtsf2"
  [(match_operand:SF 0 "register_operand" "")
   (match_operand:SF 1 "register_operand" "")]
  ""
  {
    rtx rm = gen_rtx_CONST_STRING (VOIDmode, "");
    emit_insn (gen_kvx_frsrw (operands[0], operands[1], rm));
    DONE;
  }
)

(define_expand "roundevensf2"
  [(set (match_operand:SF 0 "register_operand" "")
        (match_operand:SF 1 "register_operand" ""))]
  ""
  {
    rtx cs_val = gen_reg_rtx (DImode);
    rtx tempsi = gen_reg_rtx (SImode);
    rtx tempsf = gen_reg_rtx (SFmode);
    rtx cs = gen_rtx_REG (DImode, 68);
    rtx rn = gen_rtx_CONST_STRING (VOIDmode, ".rn");
    rtx deqz = gen_rtx_CONST_STRING (VOIDmode, ".deqz");
    /* 1. Reset the error bits (IO and XIO) we check in CS */
    emit_insn (gen_kvx_get (cs_val, cs));
    emit_insn (gen_anddi3 (cs_val, cs_val, GEN_INT (0xfffffffffffffdfd)));
    emit_insn (gen_kvx_set (cs, cs_val));
    /* 2. Convert floating point to fixed point */
    emit_insn (gen_kvx_fixedw (tempsi, operands[1], const0_rtx, rn));
    emit_insn (gen_kvx_get (cs_val, cs));
    emit_insn (gen_anddi3 (cs_val, cs_val, GEN_INT (0x202)));
    /* 3. Convert back to floating point, and take into account corner cases. */
    emit_insn (gen_kvx_floatw (tempsf, tempsi, const0_rtx, rn));
    emit_insn (gen_kvx_selectfw (operands[0], tempsf, operands[1], cs_val, deqz));
    DONE;
  }
)


;; DF

(define_insn "adddf3"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (plus:DF (match_operand:DF 1 "register_operand" "r")
                 (match_operand:DF 2 "register_operand" "r")))]
  ""
  "faddd %0 = %1, %2"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "subdf3"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (minus:DF (match_operand:DF 1 "register_operand" "r")
                  (match_operand:DF 2 "register_operand" "r")))]
  ""
  "fsbfd %0 = %2, %1"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "muldf3"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (mult:DF (match_operand:DF 1 "register_operand" "r")
                 (match_operand:DF 2 "register_operand" "r")))]
  ""
  "fmuld %0 = %1, %2"
  [(set_attr "type" "mult_fp4")]
)

(define_expand "divdf3"
  [(set (match_operand:DF 0 "register_operand" "")
        (div:DF (match_operand:DF 1 "register_operand" "")
                (match_operand:DF 2 "register_operand" "")))]
  ""
  {
    emit_library_call_value
      (gen_rtx_SYMBOL_REF (Pmode, "__divdf3"),
      operands[0], LCT_CONST, DFmode,
      operands[1], DFmode, operands[2], DFmode);
    DONE;
  }
)

(define_insn "fmadf4"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (fma:DF  (match_operand:DF 1 "register_operand" "r")
                 (match_operand:DF 2 "register_operand" "r")
                 (match_operand:DF 3 "register_operand" "0")))]
  ""
  "ffmad %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "fnmadf4"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (fma:DF  (neg:DF (match_operand:DF 1 "register_operand" "r"))
                 (match_operand:DF 2 "register_operand" "r")
                 (match_operand:DF 3 "register_operand" "0")))]
  ""
  "ffmsd %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "fmindf3"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (smin:DF (match_operand:DF 1 "register_operand" "r")
                 (match_operand:DF 2 "register_operand" "r")))]
  ""
  "fmind %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "fmaxdf3"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (smax:DF (match_operand:DF 1 "register_operand" "r")
                 (match_operand:DF 2 "register_operand" "r")))]
  ""
  "fmaxd %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "negdf2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (neg:DF (match_operand:DF 1 "register_operand" "r")))]
  ""
  "fnegd %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "absdf2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (abs:DF (match_operand:DF 1 "register_operand" "r")))]
  ""
  "fabsd %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_getsignd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI  [(match_operand:DF 1 "register_operand" "r")] UNSPEC_GETSIGN))]
  ""
  "extfs %0 = %1, 63, 63"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_setsignd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "0")
                    (match_operand:DI 2 "register_operand" "r")] UNSPEC_SETSIGN))]
  ""
  "insf %0 = %2, 63, 63"
  [(set_attr "type" "alu_thin")]
)

(define_expand "copysigndf3"
  [(match_operand:DF 0 "register_operand")
   (match_operand:DF 1 "register_operand")
   (match_operand:DF 2 "register_operand")]
  ""
  {
    rtx sign2 = gen_reg_rtx (DImode);
    emit_insn (gen_kvx_getsignd (sign2, operands[2]));
    emit_insn (gen_kvx_setsignd (operands[0], operands[1], sign2));
    DONE;
  }
)

(define_expand "xorsigndf3"
  [(match_operand:DF 0 "register_operand")
   (match_operand:DF 1 "register_operand")
   (match_operand:DF 2 "register_operand")]
  ""
  {
    rtx mask = GEN_INT (0x8000000000000000);
    rtx sign2 = gen_reg_rtx (DFmode);
    emit_insn (gen_rtx_SET (sign2, gen_rtx_UNSPEC (DFmode, gen_rtvec (2, operands[2], mask), UNSPEC_ANDD)));
    emit_insn (gen_rtx_SET (operands[0], gen_rtx_UNSPEC (DFmode, gen_rtvec (2, operands[1], sign2), UNSPEC_XORD)));
    DONE;
  }
)

(define_insn "floatdidf2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (float:DF (match_operand:DI 1 "register_operand" "r")))]
  ""
  {
    if (KV3)
      return "floatd.rn %0 = %1, 0";
    return "floatd.rn %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)

(define_insn "floatunsdidf2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unsigned_float:DF (match_operand:DI 1 "register_operand" "r")))]
  ""
  {
    if (KV3)
      return "floatud.rn %0 = %1, 0";
    return "floatud.rn %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)

(define_insn "fix_truncdfdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (fix:DI (match_operand:DF 1 "register_operand" "r")))]
  ""
  {
    if (KV3)
      return "fixedd.rz %0 = %1, 0";
    return "fixedd.rz %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)

(define_insn "fixuns_truncdfdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unsigned_fix:DI (match_operand:DF 1 "register_operand" "r")))]
  ""
  {
    if (KV3)
      return "fixedud.rz %0 = %1, 0";
    return "fixedud.rz %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)

(define_expand "truncdfhf2"
  [(set (match_operand:HF 0 "register_operand" "")
        (float_truncate:HF (match_operand:DF 1 "register_operand" "")))]
  ""
  {
    rtx tempsf = gen_reg_rtx (SFmode);
    emit_insn (gen_truncdfsf2 (tempsf, operands[1]));
    emit_insn (gen_truncsfhf2 (operands[0], tempsf));
    DONE;
  }
)

(define_insn "truncdfsf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (float_truncate:SF (match_operand:DF 1 "register_operand" "r")))]
  ""
  "fnarrowdw %0 = %1"
  [(set_attr "type" "alu_full")]
)

(define_expand "roundevendf2"
  [(set (match_operand:DF 0 "register_operand" "")
        (match_operand:DF 1 "register_operand" ""))]
  ""
  {
    rtx cs_val = gen_reg_rtx (DImode);
    rtx tempdi = gen_reg_rtx (DImode);
    rtx tempdf = gen_reg_rtx (DFmode);
    rtx cs = gen_rtx_REG (DImode, 68);
    rtx rn = gen_rtx_CONST_STRING (VOIDmode, ".rn");
    rtx deqz = gen_rtx_CONST_STRING (VOIDmode, ".deqz");
    /* 1. Reset the error bits (IO and XIO) we check in CS */
    emit_insn (gen_kvx_get (cs_val, cs));
    emit_insn (gen_anddi3 (cs_val, cs_val, GEN_INT (0xfffffffffffffdfd)));
    emit_insn (gen_kvx_set (cs, cs_val));
    /* 2. Convert floating point to fixed point */
    emit_insn (gen_kvx_fixedd (tempdi, operands[1], const0_rtx, rn));
    emit_insn (gen_kvx_get (cs_val, cs));
    emit_insn (gen_anddi3 (cs_val, cs_val, GEN_INT (2)));
    /* 3. Convert back to floating point, and take into account corner cases. */
    emit_insn (gen_kvx_floatd (tempdf, tempdi, const0_rtx, rn));
    emit_insn (gen_kvx_selectfd (operands[0], tempdf, operands[1], cs_val, deqz));
    DONE;
  }
)

;; SC

(define_insn "addsc3"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (plus:SC (match_operand:SC 1 "register_operand" "r")
                 (match_operand:SC 2 "register_operand" "r")))]
  ""
  "faddwp %0 = %1, %2"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "subsc3"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (minus:SC (match_operand:SC 1 "register_operand" "r")
                  (match_operand:SC 2 "register_operand" "r")))]
  ""
  "fsbfwp %0 = %2, %1"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "negsc2"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (neg:SC (match_operand:SC 1 "register_operand" "r")))]
  ""
  "fnegwp %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "mulsc3"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (mult:SC (match_operand:SC 1 "register_operand" "r")
                 (match_operand:SC 2 "register_operand" "r")))]
  ""
  "fmulwc %0 = %1, %2"
  [(set_attr "type" "dotp_fp4")]
)

(define_insn "fmasc4"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (fma:SC (match_operand:SC 1 "register_operand" "r")
                (match_operand:SC 2 "register_operand" "r")
                (match_operand:SC 3 "register_operand" "0")))]
  "(KV3_2||KV4)"
  "ffmawc %0 = %1, %2"
  [(set_attr "type" "dmda_fp4")]
)

(define_insn "fnmasc4"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (fma:SC (neg:SC (match_operand:SC 1 "register_operand" "r"))
                (match_operand:SC 2 "register_operand" "r")
                (match_operand:SC 3 "register_operand" "0")))]
  "(KV3_2||KV4)"
  "ffmswc %0 = %1, %2"
  [(set_attr "type" "dmda_fp4")]
)

(define_insn "addconjsc3"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (plus:SC (conj:SC (match_operand:SC 1 "register_operand" "r"))
                 (match_operand:SC 2 "register_operand" "r")))]
  "KV3"
  "faddwc.c %0 = %1, %2"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "subconjsc3"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (minus:SC (match_operand:SC 1 "register_operand" "r")
                  (conj:SC (match_operand:SC 2 "register_operand" "r"))))]
  "KV3"
  "fsbfwc.c %0 = %2, %1"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "mulconjsc3"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (mult:SC (conj:SC (match_operand:SC 1 "register_operand" "r"))
                 (match_operand:SC 2 "register_operand" "r")))]
  "KV3"
  "fmulwc.c %0 = %1, %2"
  [(set_attr "type" "dotp_fp4")]
)

(define_insn "fmaconjsc4"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (fma:SC (conj:SC (match_operand:SC 1 "register_operand" "r"))
                (match_operand:SC 2 "register_operand" "r")
                (match_operand:SC 3 "register_operand" "0")))]
  "KV3_2"
  "ffmawc.c %0 = %1, %2"
  [(set_attr "type" "dmda_fp4")]
)

(define_insn "fnmaconjsc4"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (fma:SC (neg:SC (match_operand:SC 1 "register_operand" "r"))
                (conj:SC (match_operand:SC 2 "register_operand" "r"))
                (match_operand:SC 3 "register_operand" "0")))]
  "KV3_2"
  "ffmswc.c %0 = %2, %1"
  [(set_attr "type" "dmda_fp4")]
)

(define_insn "conjsc2"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (conj:SC (match_operand:SC 1 "register_operand" "r")))]
  ""
  "fnegd %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_expand "crot90sc2"
  [(match_operand:SC 0 "register_operand" "")
   (match_operand:SC 1 "register_operand" "")]
  ""
  {
    emit_insn (gen_conjsc2 (operands[0], operands[1]));
    emit_insn (gen_kvx_sbmm8d (simplify_gen_subreg (DImode, operands[0], SCmode, 0),
			       simplify_gen_subreg (DImode, operands[0], SCmode, 0),
			       gen_rtx_CONST_INT (DImode, 0x0804020180402010)));
    DONE;
  }
)

(define_expand "crot270sc2"
  [(match_operand:SC 0 "register_operand" "")
   (match_operand:SC 1 "register_operand" "")]
  ""
  {
    emit_insn (gen_kvx_sbmm8d (simplify_gen_subreg (DImode, operands[0], SCmode, 0),
			       simplify_gen_subreg (DImode, operands[1], SCmode, 0),
			       gen_rtx_CONST_INT (DImode, 0x0804020180402010)));
    emit_insn (gen_conjsc2 (operands[0], operands[0]));
    DONE;
  }
)


;; Patterns for cadd90sc3 and cadd270sc3 are not implemented because KVX cannot
;; accelerate these operations

;; DC

(define_insn_and_split "adddc3"
  [(set (match_operand:DC 0 "register_operand" "=r")
        (plus:DC (match_operand:DC 1 "register_operand" "r")
                 (match_operand:DC 2 "register_operand" "r")))]
  ""
  "fadddc %0 = %1, %2"
  "KV4 && reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (plus:DF (subreg:DF (match_dup 1) 0)
                 (subreg:DF (match_dup 2) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (plus:DF (subreg:DF (match_dup 1) 8)
                 (subreg:DF (match_dup 2) 8)))]
  ""
  [(set (attr "type")
   (if_then_else (match_test "KV3_1")
                 (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_insn_and_split "subdc3"
  [(set (match_operand:DC 0 "register_operand" "=r")
        (minus:DC (match_operand:DC 1 "register_operand" "r")
                  (match_operand:DC 2 "register_operand" "r")))]
  ""
  "fsbfdc %0 = %2, %1"
  "KV4 && reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (minus:DF (subreg:DF (match_dup 1) 0)
                  (subreg:DF (match_dup 2) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (minus:DF (subreg:DF (match_dup 1) 8)
                  (subreg:DF (match_dup 2) 8)))]
  ""
  [(set (attr "type")
   (if_then_else (match_test "KV3_1")
                 (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_insn_and_split "negdc2"
  [(set (match_operand:DC 0 "register_operand" "=r")
        (neg:DC (match_operand:DC 1 "register_operand" "r")))]
  ""
  "fnegd %x0 = %x1\n\tfnegd %y0 = %y1"
  "KV4 && reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (neg:DF (subreg:DF (match_dup 1) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (neg:DF (subreg:DF (match_dup 1) 8)))]
  ""
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length" "8")]
)

(define_insn "addconjdc3"
  [(set (match_operand:DC 0 "register_operand" "=r")
        (plus:DC (conj:DC (match_operand:DC 1 "register_operand" "r"))
                 (match_operand:DC 2 "register_operand" "r")))]
  "KV3"
  "fadddc.c %0 = %1, %2"
  [(set (attr "type")
   (if_then_else (match_test "KV3_1")
                 (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_insn "subconjdc3"
  [(set (match_operand:DC 0 "register_operand" "=r")
        (minus:DC (match_operand:DC 1 "register_operand" "r")
                  (conj:DC (match_operand:DC 2 "register_operand" "r"))))]
  "KV3"
  "fsbfdc.c %0 = %2, %1"
  [(set (attr "type")
   (if_then_else (match_test "KV3_1")
                 (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_insn_and_split "conjdc2"
  [(set (match_operand:DC 0 "register_operand" "=r")
        (conj:DC (match_operand:DC 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0) (subreg:DF (match_dup 1) 0))
   (set (subreg:DF (match_dup 0) 8)
        (neg:DF (subreg:DF (match_dup 1) 8)))]
  ""
)

(define_expand "fast_muldc3"
  [(set (match_operand:DC 0 "register_operand" "=r")
        (mult:DC (match_operand:DC 1 "register_operand" "r")
                 (match_operand:DC 2 "register_operand" "r")))]
  ""
  {
    rtx a_r = simplify_gen_subreg (DFmode, operands[1], DCmode, 0);
    rtx a_i = simplify_gen_subreg (DFmode, operands[1], DCmode, 8);
    rtx b_r = simplify_gen_subreg (DFmode, operands[2], DCmode, 0);
    rtx b_i = simplify_gen_subreg (DFmode, operands[2], DCmode, 8);
    rtx res_r = simplify_gen_subreg (DFmode, operands[0], DCmode, 0);
    rtx res_i = simplify_gen_subreg (DFmode, operands[0], DCmode, 8);
    emit_insn (gen_muldf3 (res_r, a_r, b_r));
    emit_insn (gen_muldf3 (res_i, a_r, b_i));
    emit_insn (gen_fnmadf4 (res_r, a_i, b_i, res_r));
    emit_insn (gen_fmadf4 (res_i, a_i, b_r, res_i));
    DONE;
  }
)
