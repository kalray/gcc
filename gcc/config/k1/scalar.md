
;; HI


;; SI


;; DI


;; SF

(define_insn "addsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (plus:SF (match_operand:SF 1 "register_operand" "r")
                 (match_operand:SF 2 "register_operand" "r")))]
  ""
  "faddw %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_faddw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FADDW))]
  ""
  "faddw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "subsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (minus:SF (match_operand:SF 1 "register_operand" "r")
                  (match_operand:SF 2 "register_operand" "r")))]
  ""
  "fsbfw %0 = %2, %1"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_fsbfw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FSBFW))]
  ""
  "fsbfw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "mulsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (mult:SF (match_operand:SF 1 "register_operand" "r")
                 (match_operand:SF 2 "register_operand" "r")))]
  ""
  "fmulw %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_fmulw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FMULW))]
  ""
  "fmulw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "*fmulwd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (mult:DF (float_extend:DF (match_operand:SF 1 "register_operand" "r"))
                 (float_extend:DF (match_operand:SF 2 "register_operand" "r"))))]
  ""
  "fmulwd %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "*fmulwd2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (float_extend:DF (mult:SF (match_operand:SF 1 "register_operand" "r")
                                  (match_operand:SF 2 "register_operand" "r"))))]
  ""
  "fmulwd %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_fmulwd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FMULWD))]
  ""
  "fmulwd%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "fmasf4"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (fma:SF  (match_operand:SF 1 "register_operand" "r")
                 (match_operand:SF 2 "register_operand" "r")
                 (match_operand:SF 3 "register_operand" "0")))]
  ""
  "ffmaw %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "k1_ffmaw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMAW))]
  ""
  "ffmaw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "*ffmawd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (fma:DF  (float_extend:DF (match_operand:SF 1 "register_operand" "r"))
                 (float_extend:DF (match_operand:SF 2 "register_operand" "r"))
                 (match_operand:DF 3 "register_operand" "0")))]
  ""
  "ffmawd %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "k1_ffmawd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand:DF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMAWD))]
  ""
  "ffmawd%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "fnmasf4"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (fma:SF  (neg:SF (match_operand:SF 1 "register_operand" "r"))
                 (match_operand:SF 2 "register_operand" "r")
                 (match_operand:SF 3 "register_operand" "0")))]
  ""
  "ffmsw %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "k1_ffmsw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMSW))]
  ""
  "ffmsw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "*ffmswd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (fma:DF (float_extend:DF (neg:SF (match_operand:SF 1 "register_operand" "r")))
                (float_extend:DF (match_operand:SF 2 "register_operand" "r"))
                (match_operand:DF 3 "register_operand" "0")))]
  ""
  "ffmswd %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "*ffmswd2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (fma:DF (float_extend:DF (match_operand:SF 1 "register_operand" "r"))
                (float_extend:DF (neg:SF (match_operand:SF 2 "register_operand" "r")))
                (match_operand:DF 3 "register_operand" "0")))]
  ""
  "ffmswd %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "k1_ffmswd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand:DF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMSWD))]
  ""
  "ffmswd%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "extendsfdf2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (float_extend:DF (match_operand:SF 1 "register_operand" "r")))]
  ""
  "fwidenlwd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

;; DF

(define_insn "adddf3"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (plus:DF (match_operand:DF 1 "register_operand" "r")
                 (match_operand:DF 2 "register_operand" "r")))]
  ""
  "faddd %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_faddd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:DF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FADDD))]
  ""
  "faddd%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "subdf3"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (minus:DF (match_operand:DF 1 "register_operand" "r")
                  (match_operand:DF 2 "register_operand" "r")))]
  ""
  "fsbfd %0 = %2, %1"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_fsbfd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:DF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FSBFD))]
  ""
  "fsbfd%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "muldf3"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (mult:DF (match_operand:DF 1 "register_operand" "r")
                 (match_operand:DF 2 "register_operand" "r")))]
  ""
  "fmuld %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_fmuld"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:DF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FMULD))]
  ""
  "fmuld%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "fmadf4"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (fma:DF  (match_operand:DF 1 "register_operand" "r")
                 (match_operand:DF 2 "register_operand" "r")
                 (match_operand:DF 3 "register_operand" "0")))]
  ""
  "ffmad %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "k1_ffmad"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:DF 2 "register_operand" "r")
                    (match_operand:DF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMAD))]
  ""
  "ffmad%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "fnmadf4"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (fma:DF  (neg:DF (match_operand:DF 1 "register_operand" "r"))
                 (match_operand:DF 2 "register_operand" "r")
                 (match_operand:DF 3 "register_operand" "0")))]
  ""
  "ffmsd %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "k1_ffmsd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:DF 2 "register_operand" "r")
                    (match_operand:DF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMSD))]
  ""
  "ffmsd%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

;; SC

(define_insn "addsc3"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (plus:SC (match_operand:SC 1 "register_operand" "r")
                 (match_operand:SC 2 "register_operand" "r")))]
  ""
  "faddwp %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_faddwc"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (unspec:SC [(match_operand:SC 1 "register_operand" "r")
                    (match_operand:SC 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FADDWC))]
  ""
  "faddwp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_faddcwc"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (unspec:SC [(match_operand:SC 1 "register_operand" "r")
                    (match_operand:SC 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FADDCWC))]
  ""
  "faddcwc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "subsc3"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (minus:SC (match_operand:SC 1 "register_operand" "r")
                  (match_operand:SC 2 "register_operand" "r")))]
  ""
  "fsbfwp %0 = %2, %1"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_fsbfwc"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (unspec:SC [(match_operand:SC 1 "register_operand" "r")
                    (match_operand:SC 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FSBFWC))]
  ""
  "fsbfwp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_fsbfcwc"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (unspec:SC [(match_operand:SC 1 "register_operand" "r")
                    (match_operand:SC 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FSBFCWC))]
  ""
  "fsbfcwc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "mulsc3"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (mult:SC (match_operand:SC 1 "register_operand" "r")
                 (match_operand:SC 2 "register_operand" "r")))]
  ""
  "fmulwc %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_fmulwc"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (unspec:SC [(match_operand:SC 1 "register_operand" "r")
                    (match_operand:SC 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FMULWC))]
  ""
  "fmulwc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_fmulcwc"
  [(set (match_operand:SC 0 "register_operand" "=r")
        (unspec:SC [(match_operand:SC 1 "register_operand" "r")
                    (match_operand:SC 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FMULCWC))]
  ""
  "fmulcwc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "k1_ffmawc"
  [(set (match_operand:SC 0 "register_operand" "")
        (unspec:SC [(match_operand:SC 1 "register_operand" "")
                    (match_operand:SC 2 "register_operand" "")
                    (match_operand:SC 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFMAWC))]
  ""
  {
    rtx prod = gen_reg_rtx (SCmode);
    emit_insn (gen_k1_fmulwc (prod, operands[1], operands[2], operands[4]));
    emit_insn (gen_k1_faddwc (operands[0], prod, operands[3], operands[4]));
    DONE;
  }
)

(define_expand "k1_ffmswc"
  [(set (match_operand:SC 0 "register_operand" "")
        (unspec:SC [(match_operand:SC 1 "register_operand" "")
                    (match_operand:SC 2 "register_operand" "")
                    (match_operand:SC 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFMSWC))]
  ""
  {
    rtx prod = gen_reg_rtx (SCmode);
    emit_insn (gen_k1_fmulwc (prod, operands[1], operands[2], operands[4]));
    emit_insn (gen_k1_fsbfwc (operands[0], prod, operands[3], operands[4]));
    DONE;
  }
)

;; DC

(define_insn "k1_fadddc"
  [(set (match_operand:DC 0 "register_operand" "=r")
        (unspec:DC [(match_operand:DC 1 "register_operand" "r")
                    (match_operand:DC 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FADDDC))]
  ""
  "fadddp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_faddcdc"
  [(set (match_operand:DC 0 "register_operand" "=r")
        (unspec:DC [(match_operand:DC 1 "register_operand" "r")
                    (match_operand:DC 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FADDCWC))]
  ""
  "faddcdc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_fsbfdc"
  [(set (match_operand:DC 0 "register_operand" "=r")
        (unspec:DC [(match_operand:DC 1 "register_operand" "r")
                    (match_operand:DC 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FSBFDC))]
  ""
  "fsbfdp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_fsbfcdc"
  [(set (match_operand:DC 0 "register_operand" "=r")
        (unspec:DC [(match_operand:DC 1 "register_operand" "r")
                    (match_operand:DC 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FSBFCWC))]
  ""
  "fsbfcdc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "k1_fmuldc"
  [(set (match_operand:DC 0 "register_operand" "")
        (unspec:DC [(match_operand:DC 1 "register_operand" "")
                    (match_operand:DC 2 "register_operand" "")
                    (match_operand 3 "" "")] UNSPEC_FMULDC))]
  ""
  {
    rtx real_0 = gen_rtx_SUBREG (DFmode, operands[0], 0);
    rtx imag_0 = gen_rtx_SUBREG (DFmode, operands[0], 8);
    rtx real_1 = gen_rtx_SUBREG (DFmode, operands[1], 0);
    rtx imag_1 = gen_rtx_SUBREG (DFmode, operands[1], 8);
    rtx real_2 = gen_rtx_SUBREG (DFmode, operands[2], 0);
    rtx imag_2 = gen_rtx_SUBREG (DFmode, operands[2], 8);
    rtx real_t = gen_reg_rtx (DFmode), imag_t = gen_reg_rtx (DFmode);
    emit_insn (gen_k1_fmuld (real_t, real_1, real_2, operands[3]));
    emit_insn (gen_k1_ffmsd (real_0, imag_1, imag_2, real_t, operands[3]));
    emit_insn (gen_k1_fmuld (imag_t, real_1, imag_2, operands[3]));
    emit_insn (gen_k1_ffmad (imag_0, real_2, imag_1, imag_t, operands[3]));
    DONE;
  }
)

(define_expand "k1_fmulcdc"
  [(set (match_operand:DC 0 "register_operand" "")
        (unspec:DC [(match_operand:DC 1 "register_operand" "")
                    (match_operand:DC 2 "register_operand" "")
                    (match_operand 3 "" "")] UNSPEC_FMULCDC))]
  ""
  {
    rtx real_0 = gen_rtx_SUBREG (DFmode, operands[0], 0);
    rtx imag_0 = gen_rtx_SUBREG (DFmode, operands[0], 8);
    rtx real_1 = gen_rtx_SUBREG (DFmode, operands[1], 0);
    rtx imag_1 = gen_rtx_SUBREG (DFmode, operands[1], 8);
    rtx real_2 = gen_rtx_SUBREG (DFmode, operands[2], 0);
    rtx imag_2 = gen_rtx_SUBREG (DFmode, operands[2], 8);
    rtx real_t = gen_reg_rtx (DFmode), imag_t = gen_reg_rtx (DFmode);
    emit_insn (gen_k1_fmuld (real_t, real_1, real_2, operands[3]));
    emit_insn (gen_k1_ffmad (real_0, real_t, imag_1, imag_2, operands[3]));
    emit_insn (gen_k1_fmuld (imag_t, real_2, imag_1, operands[3]));
    emit_insn (gen_k1_ffmsd (imag_0, imag_t, real_1, imag_2, operands[3]));
    DONE;
  }
)

(define_expand "k1_ffmadc"
  [(set (match_operand:DC 0 "register_operand" "")
        (unspec:DC [(match_operand:DC 1 "register_operand" "")
                    (match_operand:DC 2 "register_operand" "")
                    (match_operand:DC 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFMADC))]
  ""
  {
    rtx prod = gen_reg_rtx (DCmode);
    rtx real_p = gen_rtx_SUBREG (DFmode, prod, 0);
    rtx imag_p = gen_rtx_SUBREG (DFmode, prod, 8);
    rtx real_1 = gen_rtx_SUBREG (DFmode, operands[1], 0);
    rtx imag_1 = gen_rtx_SUBREG (DFmode, operands[1], 8);
    rtx real_2 = gen_rtx_SUBREG (DFmode, operands[2], 0);
    rtx imag_2 = gen_rtx_SUBREG (DFmode, operands[2], 8);
    rtx real_t = gen_reg_rtx (DFmode), imag_t = gen_reg_rtx (DFmode);
    emit_insn (gen_k1_fmuld (real_t, real_1, real_2, operands[4]));
    emit_insn (gen_k1_ffmsd (real_p, imag_1, imag_2, real_t, operands[4]));
    emit_insn (gen_k1_fmuld (imag_t, real_1, imag_2, operands[4]));
    emit_insn (gen_k1_ffmad (imag_p, imag_1, real_2, imag_t, operands[4]));
    emit_insn (gen_k1_fadddc (operands[0], prod, operands[3], operands[4]));
    DONE;
  }
)

(define_expand "k1_ffmsdc"
  [(set (match_operand:DC 0 "register_operand" "")
        (unspec:DC [(match_operand:DC 1 "register_operand" "")
                    (match_operand:DC 2 "register_operand" "")
                    (match_operand:DC 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFMSDC))]
  ""
  {
    rtx prod = gen_reg_rtx (DCmode);
    rtx real_p = gen_rtx_SUBREG (DFmode, prod, 0);
    rtx imag_p = gen_rtx_SUBREG (DFmode, prod, 8);
    rtx real_1 = gen_rtx_SUBREG (DFmode, operands[1], 0);
    rtx imag_1 = gen_rtx_SUBREG (DFmode, operands[1], 8);
    rtx real_2 = gen_rtx_SUBREG (DFmode, operands[2], 0);
    rtx imag_2 = gen_rtx_SUBREG (DFmode, operands[2], 8);
    rtx real_t = gen_reg_rtx (DFmode), imag_t = gen_reg_rtx (DFmode);
    emit_insn (gen_k1_fmuld (real_t, real_1, real_2, operands[4]));
    emit_insn (gen_k1_ffmsd (real_p, imag_1, imag_2, real_t, operands[4]));
    emit_insn (gen_k1_fmuld (imag_t, real_1, imag_2, operands[4]));
    emit_insn (gen_k1_ffmad (imag_p, imag_1, real_2, imag_t, operands[4]));
    emit_insn (gen_k1_fsbfdc (operands[0], prod, operands[3], operands[4]));
    DONE;
  }
)


