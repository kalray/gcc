;; V2SI complex

(define_expand "cmulv2si3"
[(set (match_operand:V2SI 0 "register_operand" "=r")
      (mult:V2SI (match_operand:V2SI 1 "register_operand" "r")
                (match_operand:V2SI 2 "register_operand" "r")))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_cmulv2si3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      {
        rtx temp0 = gen_reg_rtx (V4SImode);
        rtx temp0_lo = gen_lowpart (V2SImode, temp0);
        rtx temp0_hi = gen_highpart (V2SImode, temp0);
        rtx temp0_lo_di = simplify_gen_subreg (DImode, temp0_lo, V2SImode, 0);
        rtx temp0_hi_di = simplify_gen_subreg (DImode, temp0_hi, V2SImode, 0);
        rtx op0_di = simplify_gen_subreg (DImode, operands[0], V2SImode, 0);
        emit_insn (gen_kvx_mm212w (temp0, operands[1], operands[2]));
        emit_insn (gen_kvx_sbmm8d (temp0_hi_di, temp0_hi_di, gen_rtx_CONST_INT (DImode, 0x0804020180402010)));
        emit_insn (gen_addv2si3 (operands[0], temp0_lo, temp0_hi));
        emit_insn (gen_subv2si3 (temp0_lo, temp0_lo, temp0_hi));
        emit_insn (gen_insvdi (op0_di, GEN_INT (32), GEN_INT (0), temp0_lo_di));
      }
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "cmulv2si3_1"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r,r")
        (cmult:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r,r")
                 (match_operand:V2SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  "KV3_1"
  "mulwc %0 = %1, %2"
  [(set_attr "type" "mult_int,mult_int,mult_int_x,mult_int_y")
   (set_attr "length"      "4,       4,         8,        12")]
)

(define_insn "mulconjv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (cmult:V2SI (conj:V2SI (match_operand:V2SI 1 "register_operand" "r"))
                  (match_operand:V2SI 2 "register_operand" "r")))]
  "KV3_1"
  "mulwc.c %0 = %1, %2"
  [(set_attr "type"   "mult_int")]
)

;; V2SF complex

(define_insn "cmulv2sf3"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (cmult:V2SF (match_operand:V2SF 1 "register_operand" "r")
                 (match_operand:V2SF 2 "register_operand" "r")))]
  ""
  "fmulwc %0 = %1, %2"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "cmlav2sf4"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (cfma:V2SF (match_operand:V2SF 1 "register_operand" "r")
                (match_operand:V2SF 2 "register_operand" "r")
                (match_operand:V2SF 3 "register_operand" "0")))]
  "(KV3_2||KV4)"
  "ffmawc %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "cnmlav2sf4"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (cfma:V2SF (neg:V2SF (match_operand:V2SF 1 "register_operand" "r"))
                (match_operand:V2SF 2 "register_operand" "r")
                (match_operand:V2SF 3 "register_operand" "0")))]
  "(KV3_2||KV4)"
  "ffmswc %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "addconjv2sf3"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (plus:V2SF (conj:V2SF (match_operand:V2SF 1 "register_operand" "r"))
                 (match_operand:V2SF 2 "register_operand" "r")))]
  ""
  "faddwc.c %0 = %1, %2"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "subconjv2sf3"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (minus:V2SF (match_operand:V2SF 1 "register_operand" "r")
                  (conj:V2SF (match_operand:V2SF 2 "register_operand" "r"))))]
  ""
  "fsbfwc.c %0 = %2, %1"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "mulconjv2sf3"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (cmult:V2SF (conj:V2SF (match_operand:V2SF 1 "register_operand" "r"))
                 (match_operand:V2SF 2 "register_operand" "r")))]
  ""
  "fmulwc.c %0 = %1, %2"
  [(set_attr "type" "mult_fp4")]
)

;; used by the combine pass
(define_insn "cmla_conjv2sf4"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (cfma:V2SF (conj:V2SF (match_operand:V2SF 1 "register_operand" "r"))
                (match_operand:V2SF 2 "register_operand" "r")
                (match_operand:V2SF 3 "register_operand" "0")))]
  "(KV3_2||KV4)"
  "ffmawc.c %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

;; used by the combine pass
(define_insn "cnmla_conjv2sf4"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (cfma:V2SF (neg:V2SF (conj:V2SF (match_operand:V2SF 1 "register_operand" "r")))
                (match_operand:V2SF 2 "register_operand" "r")
                (match_operand:V2SF 3 "register_operand" "0")))]
  "(KV3_2||KV4)"
  "ffmswc.c %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "conjv2sf2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (conj:V2SF (match_operand:V2SF 1 "register_operand" "r")))]
  ""
  "fnegd %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_expand "crot90v2sf2"
  [(match_operand:V2SF 0 "register_operand" "")
   (match_operand:V2SF 1 "register_operand" "")]
  ""
  {
    emit_insn (gen_conjv2sf2 (operands[0], operands[1]));
    emit_insn (gen_kvx_sbmm8d (simplify_gen_subreg (DImode, operands[0], V2SFmode, 0),
			       simplify_gen_subreg (DImode, operands[0], V2SFmode, 0),
			       gen_rtx_CONST_INT (DImode, 0x0804020180402010)));
    DONE;
  }
)

(define_expand "crot270v2sf2"
  [(match_operand:V2SF 0 "register_operand" "")
   (match_operand:V2SF 1 "register_operand" "")]
  ""
  {
    emit_insn (gen_kvx_sbmm8d (simplify_gen_subreg (DImode, operands[0], V2SFmode, 0),
			       simplify_gen_subreg (DImode, operands[1], V2SFmode, 0),
			       gen_rtx_CONST_INT (DImode, 0x0804020180402010)));
    emit_insn (gen_conjv2sf2 (operands[0], operands[0]));
    DONE;
  }
)

;; Patterns for cadd90sc3 and cadd270sc3 are not implemented because KVX cannot
;; accelerate these operations

;; V2DF

(define_insn "addconjv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (plus:V2DF (conj:V2DF (match_operand:V2DF 1 "register_operand" "r"))
                 (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "fadddc.c %0 = %1, %2"
  [(set (attr "type")
   (if_then_else (match_test "KV3_1")
                 (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_insn "subconjv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (minus:V2DF (match_operand:V2DF 1 "register_operand" "r")
                  (conj:V2DF (match_operand:V2DF 2 "register_operand" "r"))))]
  ""
  "fsbfdc.c %0 = %2, %1"
  [(set (attr "type")
   (if_then_else (match_test "KV3_1")
                 (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_insn_and_split "conjv2df2"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (conj:V2DF (match_operand:V2DF 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0) (subreg:DF (match_dup 1) 0))
   (set (subreg:DF (match_dup 0) 8)
        (neg:DF (subreg:DF (match_dup 1) 8)))]
  ""
)

(define_expand "fast_cmulv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (mult:V2DF (match_operand:V2DF 1 "register_operand" "r")
                 (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  {
    rtx a_r = simplify_gen_subreg (DFmode, operands[1], V2DFmode, 0);
    rtx a_i = simplify_gen_subreg (DFmode, operands[1], V2DFmode, 8);
    rtx b_r = simplify_gen_subreg (DFmode, operands[2], V2DFmode, 0);
    rtx b_i = simplify_gen_subreg (DFmode, operands[2], V2DFmode, 8);
    rtx res_r = simplify_gen_subreg (DFmode, operands[0], V2DFmode, 0);
    rtx res_i = simplify_gen_subreg (DFmode, operands[0], V2DFmode, 8);
    emit_insn (gen_muldf3 (res_r, a_r, b_r));
    emit_insn (gen_muldf3 (res_i, a_r, b_i));
    emit_insn (gen_fnmadf4 (res_r, a_i, b_i, res_r));
    emit_insn (gen_fmadf4 (res_i, a_i, b_r, res_i));
    DONE;
  }
)

;; V2DI complex

(define_insn "conjv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
          (conj:V2DI (match_operand:V2DI 1 "register_operand" "r")))]
  ""
  "copyd %x0 = %x1\n\tnegd %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length" "8")]
)

;; vectors of complex

;; 64-bit complex

;; V8QI complex

(define_expand "conjv8qi2"
  [(set (match_operand:V8QI 0 "register_operand" "=r")
        (conj:V8QI (match_operand:V8QI 1 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    rtx temp = gen_reg_rtx (V8QImode);
    rtx temp_di = simplify_gen_subreg (DImode, temp, V8QImode, 0);
    rtx op0_di = simplify_gen_subreg (DImode, operands[0], V8QImode, 0);
    rtx op1_di = simplify_gen_subreg (DImode, operands[1], V8QImode, 0);
    emit_insn (gen_negv8qi2 (temp, operands[1]));
    emit_insn (gen_kvx_sbmm8d (op0_di, op1_di, gen_rtx_CONST_INT (DImode, 0x0040001000040001)));
    emit_insn (gen_kvx_sbmm8d (temp_di, temp_di, gen_rtx_CONST_INT (DImode, 0x8000200008000200)));
    emit_insn (gen_iordi3 (op0_di, op0_di, temp_di));
    DONE;
  }
)

;; V4HI complex

(define_insn "addconjv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (plus:V4HI (conj:V4HI (match_operand:V4HI 1 "register_operand" "r"))
                    (match_operand:V4HI 2 "register_operand" "r")))]
  "KV3_1"
  "addhcp.c %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "subconjv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (minus:V4HI (match_operand:V4HI 1 "register_operand" "r")
                     (conj:V4HI (match_operand:V4HI 2 "register_operand" "r"))))]
  "KV3_1"
  "sbfhcp.c %0 = %2, %1"
  [(set_attr "type" "alu_tiny")]
)

(define_expand "conjv4hi2"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (conj:V4HI (match_operand:V4HI 1 "register_operand" "r")))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_conjv4hi2_1 (operands[0], operands[1]));
    else if ((KV3_2||KV4))
      emit_insn (gen_conjv4hi2_2 (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length" "8")]
)

(define_insn "conjv4hi2_1"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (conj:V4HI (match_operand:V4HI 1 "register_operand" "r")))]
  "KV3_1"
  "addhcp.c %0 = %1, 0"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length" "8")]
)

(define_insn_and_split "conjv4hi2_2"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (conj:V4HI (match_operand:V4HI 1 "register_operand" "r")))
   (clobber (match_scratch:V4HI 2 "=&r"))]
  "(KV3_2||KV4)"
  "#"
  "(KV3_2||KV4) && reload_completed"
  [(set (match_dup 2) (neg:V4HI (match_dup 1)))
   (set (subreg:DI (match_dup 0) 0) (unspec:DI [(subreg:DI (match_dup 1) 0)
                                                (const_int 35253091566081)] UNSPEC_SBMM8D))
   (set (subreg:DI (match_dup 2) 0) (unspec:DI [(subreg:DI (match_dup 2) 0)
                                                (const_int -9205357638210813952)] UNSPEC_SBMM8D))
   (set (subreg:DI (match_dup 0) 0) (ior:DI (subreg:DI (match_dup 0) 0)
                                            (subreg:DI (match_dup 2) 0)))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V4HImode);
  }
)

;; 128-bit complex

;; V16QI complex

(define_expand "conjv16qi2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (conj:V16QI (match_operand:V16QI 1 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    emit_insn (gen_conjv8qi2 (gen_lowpart (V8QImode, operands[0]),
                              gen_lowpart (V8QImode, operands[1])));
    emit_insn (gen_conjv8qi2 (gen_highpart (V8QImode, operands[0]),
                              gen_highpart (V8QImode, operands[1])));
    DONE;
  }
)

;; V128CC

(define_insn "addconj<mode>3"
  [(set (match_operand:V128CC 0 "register_operand" "=r")
        (plus:V128CC (conj:V128CC (match_operand:V128CC 1 "register_operand" "r"))
                     (match_operand:V128CC 2 "register_operand" "r")))]
  ""
  "add<chunkxc> %x0 = %x1, %x2\n\tadd<chunkxc> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length" "8")]
)

(define_insn "subconj<mode>3"
  [(set (match_operand:V128CC 0 "register_operand" "=r")
        (minus:V128CC (match_operand:V128CC 1 "register_operand" "r")
                      (conj:V128CC (match_operand:V128CC 2 "register_operand" "r"))))]
  ""
  "sbf<chunkxc> %x0 = %x2, %x1\n\tsbf<chunkxc> %y0 = %y2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length" "8")]
)

(define_insn "conj<mode>2_1"
  [(set (match_operand:V128CC 0 "register_operand" "=r")
        (conj:V128CC (match_operand:V128CC 1 "register_operand" "r")))]
  ""
  "add<chunkxc> %x0 = %x1, 0\n\tadd<chunkxc> %y0 = %y1, 0"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length" "8")]
)

(define_expand "conj<mode>2"
  [(set (match_operand:V128CB 0 "register_operand" "=r")
        (conj:V128CB (match_operand:V128CB 1 "register_operand" "r")))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_conj<mode>2_1 (operands[0], operands[1]));
    else if ((KV3_2||KV4))
      {
         emit_insn (gen_conj<half>2 (gen_lowpart (<HALF>mode, operands[0]),
                                     gen_lowpart (<HALF>mode, operands[1])));
         emit_insn (gen_conj<half>2 (gen_highpart (<HALF>mode, operands[0]),
                                     gen_highpart (<HALF>mode, operands[1])));
      }
    else
      gcc_unreachable ();
    DONE;
  }
)

;; V4SI complex

(define_expand "cmulv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (cmult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                   (match_operand:V4SI 2 "register_operand" "r")))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_cmulv4si3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      {
        emit_insn (gen_cmulv2si3 (gen_lowpart (V2SImode, operands[0]),
                                gen_lowpart (V2SImode, operands[1]),
                                gen_lowpart (V2SImode, operands[2])));
        emit_insn (gen_cmulv2si3 (gen_highpart (V2SImode, operands[0]),
                                gen_highpart (V2SImode, operands[1]),
                                gen_highpart (V2SImode, operands[2])));
      }
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "cmulv4si3_1"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (cmult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                    (match_operand:V4SI 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (cmult:V2SI (subreg:V2SI (match_dup 1) 0)
                  (subreg:V2SI (match_dup 2) 0)))
   (set (subreg:V2SI (match_dup 0) 8)
        (cmult:V2SI (subreg:V2SI (match_dup 1) 8)
                  (subreg:V2SI (match_dup 2) 8)))]
  ""
)

(define_insn_and_split "cmulconjv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (cmult:V4SI (conj:V4SI (match_operand:V4SI 1 "register_operand" "r"))
                    (match_operand:V4SI 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (cmult:V2SI (conj:V2SI (subreg:V2SI (match_dup 1) 0))
                  (subreg:V2SI (match_dup 2) 0)))
   (set (subreg:V2SI (match_dup 0) 8)
        (cmult:V2SI (conj:V2SI (subreg:V2SI (match_dup 1) 8))
                  (subreg:V2SI (match_dup 2) 8)))]
  ""
)

;; V4SF complex

(define_expand "cmulv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (cmult:V4SF (match_operand:V4SF 1 "register_operand" "r")
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_cmulv4sf3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_cmulv4sf3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "cmulv4sf3_1"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (cmult:V4SF (match_operand:V4SF 1 "register_operand" "r")
                   (match_operand:V4SF 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (cmult:V2SF (subreg:V2SF (match_dup 1) 0)
                 (subreg:V2SF (match_dup 2) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (cmult:V2SF (subreg:V2SF (match_dup 1) 8)
                 (subreg:V2SF (match_dup 2) 8)))]
  ""
)

(define_insn "cmulv4sf3_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (cmult:V4SF (match_operand:V4SF 1 "register_operand" "r")
                 (match_operand:V4SF 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "fmulwcp %0 = %1, %2"
  [(set_attr "type" "mult_int")]
)

(define_insn "cmlav4sf4"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (cfma:V4SF (match_operand:V4SF 1 "register_operand" "r")
                  (match_operand:V4SF 2 "register_operand" "r")
                  (match_operand:V4SF 3 "register_operand" "0")))]
  "(KV3_2||KV4)"
  "ffmawcp %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "cnmlav4sf4"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (cfma:V4SF (neg:V4SF (match_operand:V4SF 1 "register_operand" "r"))
                  (match_operand:V4SF 2 "register_operand" "r")
                  (match_operand:V4SF 3 "register_operand" "0")))]
  "(KV3_2||KV4)"
  "ffmswcp %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "addconjv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (plus:V4SF (conj:V4SF (match_operand:V4SF 1 "register_operand" "r"))
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "faddwcp.c %0 = %1, %2"
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_insn "subconjv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (minus:V4SF (match_operand:V4SF 1 "register_operand" "r")
                    (conj:V4SF (match_operand:V4SF 2 "register_operand" "r"))))]
  ""
  "fsbfwcp.c %0 = %2, %1"
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_expand "mulconjv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (cmult:V4SF (conj:V4SF (match_operand:V4SF 1 "register_operand" "r"))
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_mulconjv4sf3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_mulconjv4sf3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "mulconjv4sf3_1"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (cmult:V4SF (conj:V4SF (match_operand:V4SF 1 "register_operand" "r"))
                   (match_operand:V4SF 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (cmult:V2SF (conj:V2SF (subreg:V2SF (match_dup 1) 0))
                 (subreg:V2SF (match_dup 2) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (cmult:V2SF (conj:V2SF (subreg:V2SF (match_dup 1) 8))
                 (subreg:V2SF (match_dup 2) 8)))]
  ""
)

(define_insn "mulconjv4sf3_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (cmult:V4SF (conj:V4SF (match_operand:V4SF 1 "register_operand" "r"))
                   (match_operand:V4SF 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "fmulwcp.c %0 = %1, %2"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "fmaconjv4sf4"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (cfma:V4SF (conj:V4SF (match_operand:V4SF 1 "register_operand" "r"))
                  (match_operand:V4SF 2 "register_operand" "r")
                  (match_operand:V4SF 3 "register_operand" "0")))]
  "(KV3_2||KV4)"
  "ffmawcp.c %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "fnmaconjv4sf4"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (cfma:V4SF (neg:V4SF (conj:V4SF (match_operand:V4SF 1 "register_operand" "r")))
                  (match_operand:V4SF 2 "register_operand" "r")
                  (match_operand:V4SF 3 "register_operand" "0")))]
  "(KV3_2||KV4)"
  "ffmswcp.c %0 = %1, %2"
  [(set_attr "type" "madd_fp4")]
)

(define_insn "conjv4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (conj:V4SF (match_operand:V4SF 1 "register_operand" "r")))]
  ""
  "fnegd %x0 = %x1\n\tfnegd %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length" "8")]
)

