;; Pairing scalars into TI

(define_insn_and_split "kvx_joinp<mode>"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (unspec:TI [(match_operand:FITGPR 1 "register_operand" "r")
                    (match_operand:FITGPR 2 "register_operand" "r")] UNSPEC_JOINP))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (subreg:DI (match_dup 1) 0))
   (set (subreg:DI (match_dup 0) 8)
        (subreg:DI (match_dup 2) 0))]
  ""
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

;; HI


;; SI

(define_insn "addsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (plus:SI (match_operand:SI 1 "register_operand" "r,r,r")
                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "addw %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,4,8")]
)

;; zero-extend version of addsi3
(define_insn "*addsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (plus:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "addw %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,4,8")]
)

(define_insn "ssaddsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (ss_plus:SI (match_operand:SI 1 "register_operand" "r,r")
                    (match_operand:SI 2 "kvx_r_any32_operand" "r,i")))]
  ""
  "addsw %0 = %1, %2"
  [(set_attr "type"   "alu_lite,alu_lite_x")
   (set_attr "length" "4,       8")]
)

;; zero extend version of ssaddsi3
(define_insn "*ssaddsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (ss_plus:SI (match_operand:SI 1 "register_operand" "r,r")
                                    (match_operand:SI 2 "kvx_r_any32_operand" "r,i"))))]
  ""
  "addsw %0 = %1, %2"
  [(set_attr "type"   "alu_lite,alu_lite_x")
   (set_attr "length" "4,       8")]
)

(define_insn "subsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (minus:SI (match_operand:SI 1 "kvx_r_s10_s37_s64_operand" "r,I10,i")
                  (match_operand:SI 2 "register_operand" "r,r,r")))]
  ""
  "sbfw %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,4,8")]
)

;; zero-extend version of subsi3
(define_insn "*subsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (minus:SI (match_operand:SI 1 "kvx_r_s10_s37_s64_operand" "r,I10,i")
                                  (match_operand:SI 2 "register_operand" "r,r,r"))))]
  ""
  "sbfw %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,4,8")]
)

(define_insn "sssubsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (ss_minus:SI (match_operand:SI 1 "kvx_r_any32_operand" "r,i")
                     (match_operand:SI 2 "register_operand" "r,r")))]
  ""
  "sbfsw %0 = %2, %1"
  [(set_attr "type"   "alu_lite,alu_lite_x")
   (set_attr "length" "4,       8")]
)

;; zero extend version of sssubsi3
(define_insn "*sssubsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (ss_minus:SI (match_operand:SI 1 "kvx_r_any32_operand" "r,i")
                                     (match_operand:SI 2 "register_operand" "r,r"))))]
  ""
  "sbfsw %0 = %2, %1"
  [(set_attr "type"   "alu_lite,alu_lite_x")
   (set_attr "length" "4,       8")]
)

(define_insn "mulsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (mult:SI (match_operand:SI 1 "register_operand" "r,r")
                 (match_operand:SI 2 "kvx_r_any32_operand" "r,i")))]
  ""
  "mulw %0 = %1, %2"
  [(set_attr "type" "mau, mau_x")
   (set_attr "length" "4, 8")]
)

;; zero extend version of mulsi3
(define_insn "*mulsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (zero_extend:DI (mult:SI (match_operand:SI 1 "register_operand" "r,r")
                                 (match_operand:SI 2 "kvx_r_any32_operand" "r,i"))))]
  ""
  "mulw %0 = %1, %2"
  [(set_attr "type" "mau, mau_x")
   (set_attr "length" "4, 8")]
)

(define_insn "mulsidi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (mult:DI (sign_extend:DI (match_operand:SI 1 "register_operand" "r"))
                 (sign_extend:DI (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "mulwd %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "umulsidi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
                 (zero_extend:DI (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "muluwd %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "usmulsidi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
                 (sign_extend:DI (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "mulsuwd %0 = %2, %1"
  [(set_attr "type" "mau")]
)

(define_expand "divsi3"
  [(set (match_operand:SI 0 "register_operand" "")
        (div:SI (match_operand:SI 1 "register_operand" "")
                (match_operand 2 "poweroftwo_6bits_immediate_operand" "")))]
  ""
  {
    HOST_WIDE_INT constant = INTVAL (operands[2]);
    operands[2] = gen_rtx_CONST_INT (VOIDmode, __builtin_ctzll (constant));
    emit_insn (gen_kvx_srsw (operands[0], operands[1], operands[2]));
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
  [(set_attr "type" "mau_auxr")]
)

(define_insn "maddsidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (plus:DI (mult:DI (sign_extend:DI (match_operand:SI 1 "register_operand" "r"))
                          (sign_extend:DI (match_operand:SI 2 "register_operand" "r")))
                 (match_operand:DI 3 "register_operand" "0")))]
  ""
  "maddwd %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "umaddsidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (plus:DI (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
                          (zero_extend:DI (match_operand:SI 2 "register_operand" "r")))
                 (match_operand:DI 3 "register_operand" "0")))]
  ""
  "madduwd %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "usmaddsidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (plus:DI (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
                          (sign_extend:DI (match_operand:SI 2 "register_operand" "r")))
                 (match_operand:DI 3 "register_operand" "0")))]
  ""
  "maddsuwd %0 = %2, %1"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "msubsisi4"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (minus:SI (match_operand:SI 3 "register_operand" "0")
                  (mult:SI (match_operand:SI 1 "register_operand" "r")
                           (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "msbfw %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "msubsidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (minus:DI (match_operand:DI 3 "register_operand" "0")
                  (mult:DI (sign_extend:DI (match_operand:SI 1 "register_operand" "r"))
                           (sign_extend:DI (match_operand:SI 2 "register_operand" "r")))))]
  ""
  "msbfwd %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "umsubsidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (minus:DI (match_operand:DI 3 "register_operand" "0")
                  (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
                           (zero_extend:DI (match_operand:SI 2 "register_operand" "r")))))]
  ""
  "msbfuwd %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "usmsubsidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (minus:DI (match_operand:DI 3 "register_operand" "0")
                  (mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
                           (sign_extend:DI (match_operand:SI 2 "register_operand" "r")))))]
  ""
  "msbfsuwd %0 = %2, %1"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "sminsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (smin:SI (match_operand:SI 1 "register_operand" "r,r,r")
                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "minw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

;; zero-extend version of sminsi3
(define_insn "*sminsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (smin:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "minw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

(define_insn "smaxsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (smax:SI (match_operand:SI 1 "register_operand" "r,r,r")
                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "maxw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

;; zero-extend version of smaxsi3
(define_insn "*smaxsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (smax:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "maxw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

(define_insn "uminsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (umin:SI (match_operand:SI 1 "register_operand" "r,r,r")
                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "minuw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

;; zero-extend version of uminsi3
(define_insn "*uminsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (umin:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "minuw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

(define_insn "umaxsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (umax:SI (match_operand:SI 1 "register_operand" "r,r,r")
                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "maxuw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

;; zero-extend version of umaxsi3
(define_insn "*umaxsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (umax:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "maxuw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

(define_insn "andsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (and:SI (match_operand:SI 1 "register_operand" "r,r,r")
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "andw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

;; zero-extend version of andsi3
(define_insn "*andsi3__zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (and:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "andw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

(define_insn "*nandw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (ior:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                (not:SI (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "nandw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

(define_insn "*andnw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (and:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "andnw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

(define_insn "iorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (ior:SI (match_operand:SI 1 "register_operand" "r,r,r")
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "orw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

;; zero-extend version of iorsi3
(define_insn "*iorsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (ior:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "orw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

(define_insn "*norw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (and:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                (not:SI (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "norw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

(define_insn "*ornw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (ior:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "ornw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

(define_insn "xorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (xor:SI (match_operand:SI 1 "register_operand" "r,r,r")
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "xorw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

;; zero-extend version of xorsi3
(define_insn "*xorsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (xor:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "xorw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

(define_insn "*nxorw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (not:SI (xor:SI (match_operand:SI 1 "register_operand" "r,r,r")
                        (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "nxorw %0 = %1, %2"
  [(set_attr "type"   "alu_tiny,alu_tiny,alu_tiny_x")
   (set_attr "length" "4,       4,       8")]
)

(define_insn "ashlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (ashift:SI (match_operand:SI 1 "register_operand" "r,r")
                   (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sllw %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)

(define_insn "ssashlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (ss_ashift:SI (match_operand:SI 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "slsw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "ashrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (ashiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sraw %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)

(define_insn "lshrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (lshiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srlw %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)

(define_insn "kvx_srsw"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r,r")
                    (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRSW))]
  ""
  "srsw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "rotlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (rotate:SI (match_operand:SI 1 "register_operand" "r,r")
                   (match_operand:SI 2 "rotate_operand" "r,U05")))]
  ""
  "rolw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "rotrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (rotatert:SI (match_operand:SI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "rotate_operand" "r,U05")))]
  ""
  "rorw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "avgsi3_floor"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r,r,r")
                    (match_operand:SI 2 "register_s32_operand" "r,I10,I32")] UNSPEC_AVGW))]
  ""
  "avgw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")]
)

(define_insn "uavgsi3_floor"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r,r,r")
                    (match_operand:SI 2 "register_s32_operand" "r,I10,I32")] UNSPEC_AVGUW))]
  ""
  "avguw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")]
)

(define_insn "avgsi3_ceil"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r,r,r")
                    (match_operand:SI 2 "register_s32_operand" "r,I10,I32")] UNSPEC_AVGRW))]
  ""
  "avgrw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")]
)

(define_insn "uavgsi3_ceil"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r,r,r")
                    (match_operand:SI 2 "register_s32_operand" "r,I10,I32")] UNSPEC_AVGRUW))]
  ""
  "avgruw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")]
)

(define_insn "bswapsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (bswap:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "sbmm8 %0 = %1, 0x01020408"
  [(set_attr "type" "alu_lite_x")
   (set_attr "length"        "8")]
)

(define_insn "negsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (neg:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "negw %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "abssi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (abs:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "absw %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_abdw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (abs:SI (minus:SI (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")
                          (match_operand:SI 1 "register_operand" "r,r,r"))))]
  ""
  "abdw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")]
)

(define_insn "clrsbsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (clrsb:SI (match_operand:SI 1 "register_operand" "r")))]
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

(define_insn "ctzsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (ctz:SI (match_operand:SI 1 "register_operand" "r")))]
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

(define_insn "one_cmplsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (not:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "notw %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_stsuw"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r")
                    (match_operand:SI 2 "register_operand" "r")] UNSPEC_STSUW))]
  ""
  "stsuw %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
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

(define_insn "*addwd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (plus:DI (sign_extend: DI (match_operand:SI 1 "register_operand" "r"))
                 (match_operand:DI 2 "register_operand" "r")))]
  ""
  "addwd %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*adduwd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (plus:DI (zero_extend: DI (match_operand:SI 1 "register_operand" "r"))
                 (match_operand:DI 2 "register_operand" "r")))]
  ""
  "adduwd %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

;; for use by addd with carry
(define_insn "kvx_addd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec_volatile:DI [(match_operand:DI 1 "register_operand" "r")
                             (match_operand:DI 2 "register_operand" "r")
                             (match_operand 3 "" "")] UNSPEC_ADDD))]
  ""
  "addd%3 %0 = %1, %2"
  [(set_attr "type" "alu_full")]
)

(define_insn "ssadddi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (ss_plus:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                    (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "addsd %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
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

(define_insn "*sbfwd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (minus:DI (match_operand:DI 1 "register_operand" "r")
                  (sign_extend:DI (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "sbfwd %0 = %2, %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sbfuwd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (minus:DI (match_operand:DI 1 "register_operand" "r")
                  (zero_extend:DI (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "sbfuwd %0 = %2, %1"
  [(set_attr "type" "alu_lite")]
)

;; for use by sbfd with carry
(define_insn "kvx_sbfd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec_volatile:DI [(match_operand:DI 1 "register_operand" "r")
                             (match_operand:DI 2 "register_operand" "r")
                             (match_operand 3 "" "")] UNSPEC_SBFD))]
  ""
  "sbfd%3 %0 = %1, %2"
  [(set_attr "type" "alu_full")]
)

(define_insn "sssubdi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (ss_minus:DI (match_operand:DI 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                     (match_operand:DI 2 "register_operand" "r,r,r,r")))]
  ""
  "sbfsd %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "muldi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (mult:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                 (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "muld %0 = %1, %2"
  [(set_attr "type" "mau, mau, mau_x, mau_y")
   (set_attr "length" "4, 4, 8, 12")]
)

(define_insn "mulditi3"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (mult:TI (sign_extend:TI (match_operand:DI 1 "register_operand" "r"))
                 (sign_extend:TI (match_operand:DI 2 "register_operand" "r"))))]
  ""
  "muldt %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "umulditi3"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                 (zero_extend:TI (match_operand:DI 2 "register_operand" "r"))))]
  ""
  "muludt %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "usmulditi3"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                 (sign_extend:TI (match_operand:DI 2 "register_operand" "r"))))]
  ""
  "mulsudt %0 = %2, %1"
  [(set_attr "type" "mau")]
)

(define_expand "divdi3"
  [(set (match_operand:DI 0 "register_operand" "")
        (div:DI (match_operand:DI 1 "register_operand" "")
                (match_operand 2 "poweroftwo_6bits_immediate_operand" "")))]
  ""
  {
    HOST_WIDE_INT constant = INTVAL (operands[2]);
    operands[2] = gen_rtx_CONST_INT (VOIDmode, __builtin_ctzll (constant));
    emit_insn (gen_kvx_srsd (operands[0], operands[1], operands[2]));
    DONE;
  }
)

(define_insn "madddidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (plus:DI (mult:DI (match_operand:DI 1 "register_operand" "r")
                          (match_operand:DI 2 "register_operand" "r"))
                 (match_operand:DI 3 "register_operand" "0")))]
  ""
  "maddd %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "maddditi4"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (plus:TI (mult:TI (sign_extend:TI (match_operand:DI 1 "register_operand" "r"))
                          (sign_extend:TI (match_operand:DI 2 "register_operand" "r")))
                 (match_operand:TI 3 "register_operand" "0")))]
  ""
  "madddt %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "umaddditi4"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (plus:TI (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                          (zero_extend:TI (match_operand:DI 2 "register_operand" "r")))
                 (match_operand:TI 3 "register_operand" "0")))]
  ""
  "maddudt %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "usmaddditi4"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (plus:TI (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                          (sign_extend:TI (match_operand:DI 2 "register_operand" "r")))
                 (match_operand:TI 3 "register_operand" "0")))]
  ""
  "maddsudt %0 = %2, %1"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "msubdidi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (minus:DI (match_operand:DI 3 "register_operand" "0")
                  (mult:DI (match_operand:DI 1 "register_operand" "r")
                           (match_operand:DI 2 "register_operand" "r"))))]
  ""
  "msbfd %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "msubditi4"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (minus:TI (match_operand:TI 3 "register_operand" "0")
                  (mult:TI (sign_extend:TI (match_operand:DI 1 "register_operand" "r"))
                           (sign_extend:TI (match_operand:DI 2 "register_operand" "r")))))]
  ""
  "msbfdt %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "umsubditi4"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (minus:TI (match_operand:TI 3 "register_operand" "0")
                  (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                           (zero_extend:TI (match_operand:DI 2 "register_operand" "r")))))]
  ""
  "msbfudt %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "usmsubditi4"
  [(set (match_operand:TI 0 "register_operand" "=r")
        (minus:TI (match_operand:TI 3 "register_operand" "0")
                  (mult:TI (zero_extend:TI (match_operand:DI 1 "register_operand" "r"))
                           (sign_extend:TI (match_operand:DI 2 "register_operand" "r")))))]
  ""
  "msbfsudt %0 = %2, %1"
  [(set_attr "type" "mau_auxr")]
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
  "ord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "*nord"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (and:DI (not:DI (match_operand:DI 1 "register_operand" "r,r,r,r"))
                (not:DI (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i"))))]
  ""
  "nord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "*ornd"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (ior:DI (not:DI (match_operand:DI 1 "register_operand" "r,r,r,r"))
                (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "ornd %0 = %1, %2"
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

(define_insn "ashldi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (ashift:DI (match_operand:DI 1 "register_operand" "r,r")
                   (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "slld %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)

(define_insn "ssashldi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (ss_ashift:DI (match_operand:DI 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "slsd %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "ashrdi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (ashiftrt:DI (match_operand:DI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srad %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)

(define_insn "lshrdi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (lshiftrt:DI (match_operand:DI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srld %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)

(define_insn "kvx_srsd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (unspec:DI [(match_operand:DI 1 "register_operand" "r,r")
                    (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRSD))]
  ""
  "srsd %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "bswapdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (bswap:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "sbmm8 %0 = %1, 0x0102040810204080"
  [(set_attr "type" "alu_lite_y")
   (set_attr "length"       "12")]
)

(define_insn "negdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (neg:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "negd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "absdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (abs:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "absd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_abdd"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (abs:DI (minus:DI (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                          (match_operand:DI 1 "register_operand" "r,r,r,r"))))]
  ""
  "abdd %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
   (set_attr "length" "4,4,8,12")]
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

(define_insn "kvx_sbmm8"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (unspec:DI [(match_operand:DI 1 "register_operand" "r,r,r,r")
                    (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")] UNSPEC_SBMM8))]
  ""
  "sbmm8 %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "kvx_sbmmt8"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (unspec:DI [(match_operand:DI 1 "register_operand" "r,r,r,r")
                    (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")] UNSPEC_SBMMT8))]
  ""
  "sbmmt8 %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "kvx_stsud"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI [(match_operand:DI 1 "register_operand" "r")
                    (match_operand:DI 2 "register_operand" "r")] UNSPEC_STSUD))]
  ""
  "stsud %0 = %1, %2"
  [(set_attr "type" "alu_lite")
   (set_attr "length" "4")]
)


;; TI


;; SF

(define_insn "addsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (plus:SF (match_operand:SF 1 "register_operand" "r")
                 (match_operand:SF 2 "register_operand" "r")))]
  ""
  "faddw %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_faddw"
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

(define_insn "kvx_fsbfw"
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

(define_insn "kvx_fmulw"
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

(define_insn "kvx_fmulwd"
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

(define_insn "kvx_ffmaw"
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

(define_insn "kvx_ffmawd"
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

(define_insn "kvx_ffmsw"
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

(define_insn "kvx_ffmswd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand:DF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMSWD))]
  ""
  "ffmswd%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "fminsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (smin:SF (match_operand:SF 1 "register_operand" "r")
                 (match_operand:SF 2 "register_operand" "r")))]
  ""
  "fminw %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fminw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (smin:SF (match_operand:SF 1 "register_operand" "r")
                 (match_operand:SF 2 "register_operand" "r")))]
  ""
  "fminw %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "fmaxsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (smax:SF (match_operand:SF 1 "register_operand" "r")
                 (match_operand:SF 2 "register_operand" "r")))]
  ""
  "fmaxw %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fmaxw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (smax:SF (match_operand:SF 1 "register_operand" "r")
                 (match_operand:SF 2 "register_operand" "r")))]
  ""
  "fmaxw %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "negsf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (neg:SF (match_operand:SF 1 "register_operand" "r")))]
  ""
  "fnegw %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fnegw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (neg:SF (match_operand:SF 1 "register_operand" "r")))]
  ""
  "fnegw %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "abssf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (abs:SF (match_operand:SF 1 "register_operand" "r")))]
  ""
  "fabsw %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fabsw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (abs:SF (match_operand:SF 1 "register_operand" "r")))]
  ""
  "fabsw %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "floatsisf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (float:SF (match_operand:SI 1 "register_operand" "r")))]
  ""
  "floatw.rn %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_floatw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SI 1 "register_operand" "r")
                    (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FLOATW))]
  ""
  "floatw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "floatunssisf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unsigned_float:SF (match_operand:SI 1 "register_operand" "r")))]
  ""
  "floatuw.rn %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_floatuw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SI 1 "register_operand" "r")
                    (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FLOATUW))]
  ""
  "floatuw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "fix_truncsfsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (fix:SI (match_operand:SF 1 "register_operand" "r")))]
  ""
  "fixedw.rz %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fixedw"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FIXEDW))]
  ""
  "fixedw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "fixuns_truncsfsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unsigned_fix:SI (match_operand:SF 1 "register_operand" "r")))]
  ""
  "fixeduw.rz %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fixeduw"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FIXEDUW))]
  ""
  "fixeduw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "extendsfdf2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (float_extend:DF (match_operand:SF 1 "register_operand" "r")))]
  ""
  "fwidenlwd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_expand "divsf3"
  [(set (match_operand:SF 0 "register_operand" "")
        (div:SF (match_operand:SF 1 "register_operand" "")
                (match_operand:SF 2 "register_operand" "")))]
  ""
  {
    rtx temp = gen_reg_rtx(SFmode);
    emit_insn (gen_recipsf2_insn (temp, CONST1_RTX (SFmode), operands[2]));
    emit_insn (gen_mulsf3 (operands[0], operands[1], temp));
    DONE;
  }
)

(define_expand "sqrtsf2"
  [(set (match_operand:SF 0 "register_operand" "")
        (sqrt:SF (match_operand:SF 1 "register_operand" "")))]
  ""
  {
    rtx temp = gen_reg_rtx(SFmode);
    emit_insn (gen_rsqrtsf2_insn (temp, CONST1_RTX (SFmode), operands[1]));
    emit_insn (gen_mulsf3 (operands[0], operands[1], temp));
    DONE;
  }
)

;; Jeff Law - [committed] Fix v850e3v5 recipf and rsqrt issues
;; Generic code demands that the recip and rsqrt named patterns
;; have precisely one operand.  So that what is exposed in the
;; expander via the strange UNSPEC.  However, those expanders
;; generate normal looking recip and rsqrt patterns.

(define_expand "recipsf2"
  [(set (match_operand:SF 0 "register_operand" "")
   (unspec:SF [(match_operand:SF 1 "register_operand" "")] UNSPEC_FRECW_))]
  ""
  {
    emit_insn (gen_recipsf2_insn (operands[0], CONST1_RTX (SFmode), operands[1]));
    DONE;
  }
)

(define_insn "recipsf2_insn"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (div:SF (match_operand:SF 1 "const_float_1_operand" "")
                (match_operand:SF 2 "register_operand" "r")))]
  ""
  "frecw %0 = %2"
  [(set_attr "type" "alu_full_copro")]
)

(define_insn "kvx_frecw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FRECW))]
  ""
  "frecw%2 %0 = %1"
  [(set_attr "type" "alu_full_copro")]
)

(define_expand "rsqrtsf2"
  [(set (match_operand:SF 0 "register_operand" "=")
   (unspec:SF [(match_operand:SF 1 "register_operand" "")]
                UNSPEC_FRSRW_))]
  ""
  {
    emit_insn (gen_rsqrtsf2_insn (operands[0], CONST1_RTX (SFmode), operands[1]));
    DONE;
  }
)

(define_insn "rsqrtsf2_insn"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (div:SF (match_operand:SF 1 "const_float_1_operand" "")
                (sqrt:SF (match_operand:SF 2 "register_operand" "r"))))]
  ""
  "frsrw %0 = %2"
  [(set_attr "type" "alu_full_copro")]
)

(define_insn "kvx_frsrw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FRSRW))]
  ""
  "frsrw%2 %0 = %1"
  [(set_attr "type" "alu_full_copro")]
)

(define_expand "kvx_fcdivw"
  [(match_operand:SF 0 "register_operand" "")
   (match_operand:SF 1 "register_operand" "")
   (match_operand:SF 2 "register_operand" "")
   (match_operand 3 "" "")]
 ""
 {
    rtx regpair = gen_reg_rtx (TImode);
    emit_insn (gen_kvx_joinpsf (regpair, operands[1], operands[2]));
    emit_insn (gen_kvx_fcdivw_insn (operands[0], regpair, operands[3]));
    DONE;
 }
)

(define_insn "kvx_fcdivw_insn"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:TI 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FCDIVW))]
  ""
  "fcdivw %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_expand "kvx_fsdivw"
  [(match_operand:SF 0 "register_operand" "")
   (match_operand:SF 1 "register_operand" "")
   (match_operand:SF 2 "register_operand" "")
   (match_operand 3 "" "")]
 ""
 {
    rtx regpair = gen_reg_rtx (TImode);
    emit_insn (gen_kvx_joinpsf (regpair, operands[1], operands[2]));
    emit_insn (gen_kvx_fsdivw_insn (operands[0], regpair, operands[3]));
    DONE;
 }
)

(define_insn "kvx_fsdivw_insn"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:TI 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FSDIVW))]
  ""
  "fsdivw%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fsrecw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FSRECW))]
  ""
  "fsrecw%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fsrsrw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FSRSRW))]
  ""
  "fsrsrw%2 %0 = %1"
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

(define_insn "kvx_faddd"
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

(define_insn "kvx_fsbfd"
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

(define_insn "kvx_fmuld"
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

(define_insn "kvx_ffmad"
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

(define_insn "kvx_ffmsd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:DF 2 "register_operand" "r")
                    (match_operand:DF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMSD))]
  ""
  "ffmsd%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "fmindf3"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (smin:DF (match_operand:DF 1 "register_operand" "r")
                 (match_operand:DF 2 "register_operand" "r")))]
  ""
  "fmind %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fmind"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (smin:DF (match_operand:DF 1 "register_operand" "r")
                 (match_operand:DF 2 "register_operand" "r")))]
  ""
  "fmind %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "fmaxdf3"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (smax:DF (match_operand:DF 1 "register_operand" "r")
                 (match_operand:DF 2 "register_operand" "r")))]
  ""
  "fmaxd %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fmaxd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (smax:DF (match_operand:DF 1 "register_operand" "r")
                 (match_operand:DF 2 "register_operand" "r")))]
  ""
  "fmaxd %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "negdf2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (neg:DF (match_operand:DF 1 "register_operand" "r")))]
  ""
  "fnegd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fnegd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (neg:DF (match_operand:DF 1 "register_operand" "r")))]
  ""
  "fnegd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "absdf2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (abs:DF (match_operand:DF 1 "register_operand" "r")))]
  ""
  "fabsd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fabsd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (abs:DF (match_operand:DF 1 "register_operand" "r")))]
  ""
  "fabsd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "floatdidf2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (float:DF (match_operand:DI 1 "register_operand" "r")))]
  ""
  "floatd.rn %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_floatd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DI 1 "register_operand" "r")
                    (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FLOATD))]
  ""
  "floatd%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "floatunsdidf2"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unsigned_float:DF (match_operand:DI 1 "register_operand" "r")))]
  ""
  "floatud.rn %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_floatud"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DI 1 "register_operand" "r")
                    (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FLOATUD))]
  ""
  "floatud%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "fix_truncdfdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (fix:DI (match_operand:DF 1 "register_operand" "r")))]
  ""
  "fixedd.rz %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fixedd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FIXEDD))]
  ""
  "fixedd%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "fixuns_truncdfdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unsigned_fix:DI (match_operand:DF 1 "register_operand" "r")))]
  ""
  "fixedud.rz %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fixedud"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FIXEDUD))]
  ""
  "fixedud%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_fcdivd"
  [(match_operand:DF 0 "register_operand" "")
   (match_operand:DF 1 "register_operand" "")
   (match_operand:DF 2 "register_operand" "")
   (match_operand 3 "" "")]
 ""
 {
    rtx regpair = gen_reg_rtx (TImode);
    emit_insn (gen_kvx_joinpdf (regpair, operands[1], operands[2]));
    emit_insn (gen_kvx_fcdivd_insn (operands[0], regpair, operands[3]));
    DONE;
 }
)

(define_insn "kvx_fcdivd_insn"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:TI 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FCDIVD))]
  ""
  "fcdivd%2 %0 = %1"
  [(set_attr "type" "alu_lite")
])

(define_expand "kvx_fsdivd"
  [(match_operand:DF 0 "register_operand" "")
   (match_operand:DF 1 "register_operand" "")
   (match_operand:DF 2 "register_operand" "")
   (match_operand 3 "" "")]
 ""
 {
    rtx regpair = gen_reg_rtx (TImode);
    emit_insn (gen_kvx_joinpdf (regpair, operands[1], operands[2]));
    emit_insn (gen_kvx_fsdivd_insn (operands[0], regpair, operands[3]));
    DONE;
 }
)

(define_insn "kvx_fsdivd_insn"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:TI 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FSDIVD))]
  ""
  "fsdivd%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fsrecd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FSRECD))]
  ""
  "fsrecd%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fsrsrd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FSRSRD))]
  ""
  "fsrsrd%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)


;; KVX_LBX, KVX_LHX, KVX_LWX

(define_insn "kvx_lbz"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:QI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LBZ))
   (use (match_dup 1))]
  ""
  "lbz%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lbs"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:QI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LBS))
   (use (match_dup 1))]
  ""
  "lbs%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lhz"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:HI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LHZ))
   (use (match_dup 1))]
  ""
  "lhz%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lhs"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:HI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LHS))
   (use (match_dup 1))]
  ""
  "lhs%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lwz"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:SI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LWZ))
   (use (match_dup 1))]
  ""
  "lwz%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lws"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:SI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LWS))
   (use (match_dup 1))]
  ""
  "lws%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lwf"
 [(set (match_operand:SF 0 "register_operand" "=r,r,r")
       (unspec:SF [(match_operand:SF 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LWF))
   (use (match_dup 1))]
  ""
  "lwz%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)


;; KVX_LD, KVX_LO, KVX_LDF

(define_insn "kvx_ld"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:DI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LD))
   (use (match_dup 1))]
  ""
  "ld%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lq"
 [(set (match_operand:TI 0 "register_operand" "=r,r,r")
       (unspec:TI [(match_operand:TI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LQ))
   (use (match_dup 1))]
  ""
  "lq%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_ldf"
 [(set (match_operand:DF 0 "register_operand" "=r,r,r")
       (unspec:DF [(match_operand:DF 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LDF))
   (use (match_dup 1))]
  ""
  "ld%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

