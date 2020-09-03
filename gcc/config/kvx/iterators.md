;; Mapping between GCC modes and suffixes used by LSU insns
(define_mode_attr lsusize [
  (QI "b")
  (HI "h")
  (SI "w")
  (SF "w")
  (DI "d")
  (DF "d")
  (TI "q")
  (OI "o")
  (V8QI "d")
  (V4HI "d")
  (V2SI "d")
  (V2SF "d")
  (V16QI "q")
  (V8HI "q")
  (V4SI "q")
  (V2DI "q")
  (V4SF "q")
  (V2DF "q")
  (V32QI "o")
  (V16HI "o")
  (V8SI "o")
  (V4DI "o")
  (V8SF "o")
  (V4DF "o")
])

;; Extension for LSU when applicable (mode narrower than a register).
(define_mode_attr lsusext [
  (QI "z")
  (HI "z")
  (SI "z")
  (SF "z")
  (DI "")
  (DF "")
  (TI "")
  (OI "")
  (V8QI "")
  (V4HI "")
  (V2SI "")
  (V2SF "")
  (V16QI "")
  (V8HI "")
  (V4SI "")
  (V2DI "")
  (V4SF "")
  (V2DF "")
  (V32QI "")
  (V16HI"")
  (V8SI "")
  (V4DI "")
  (V8SF "")
  (V4DF "")
])

;; Code iterator for sign/zero extension
(define_code_iterator ANY_EXTEND [sign_extend zero_extend])

;; Sign- or zero-extending data-op
(define_code_attr lsext [
  (sign_extend "s")
  (zero_extend "z")
])

;; Sign- or zero-extending mapping to unsigned mnemonics
(define_code_attr ssfx [
  (sign_extend "")
  (zero_extend "u")
  (smax "")
  (smin "")
  (umax "u")
  (umin "u")
])

;; Iterator for all integer modes (up to 64-bit)
(define_mode_iterator ALLI [QI HI SI DI])

;; Iterator for SI, QI and HI modes
(define_mode_iterator SHORT [QI HI SI])

;; Iterator for all float modes (up to 64-bit)
(define_mode_iterator ALLF [SF DF])

(define_mode_attr sfx [
  (SF "w")
  (DF "d")
  (QI "w")
  (HI "w")
  (SI "w")
  (DI "d")
  (TI "q")
  (OI "o")])

;; Scalar modes used by the mov pattern that fit in a register.
;; TI and OI and to be handled elsewhere.
(define_mode_iterator ALLIF [QI HI SI DI SF DF])

(define_mode_iterator ALLP [SI DI])

(define_mode_iterator P [(SI "Pmode == SImode") (DI "Pmode == DImode")])

(define_code_iterator cb_cond [eq ne gt ge lt le])

(define_mode_iterator SIDI [SI DI])

;; Used for conditional LSU with .odd/.even
(define_code_iterator COND_ODD_EVEN [eq ne])
(define_code_attr lsu_odd_even [(eq "even") (ne "odd")])

(define_mode_attr suffix [(SI "w") (DI "d") (SF "w") (DF "d")])

;; insns length for materializing a symbol depending on pointer size,
;; using make insn. Alternatives using these should only be enabled
;; for valid pointer modes: SI or DI. Anything else is an error.
;; Values 999 are used for modes where the alternative must always be disabled.
(define_mode_attr symlen1 [(SI "_x") (DI "_y") (QI "") (HI "") (SF "") (DF "")])
(define_mode_attr symlen2 [(SI "8") (DI "12") (QI "999") (HI "999") (SF "999") (DF "999")])

(define_attr "disabled" "yes,no" (const_string "no"))

(define_attr "enabled" ""
  (cond [(eq_attr "disabled" "yes") (const_int 0)]
        (const_int 1)))

;; Iterator for Atomic Integer modes
(define_mode_iterator AI [QI HI SI DI TI])

;; Iterator for atomic binary operations
;; (mult op stands for nand)
(define_code_iterator atomic_op [plus ior xor minus and mult])
(define_code_attr atomic_optab [
  (plus "add")
  (ior "or")
  (xor "xor")
  (minus "sub")
  (and "and")
  (mult "nand")])

;; Iterator for the modes tested by cbranch<m>4
(define_mode_iterator CBRANCH [
  SI SF DI DF
])

;; Iterator for the scalar modes that fit in a GPR.
(define_mode_iterator SCALAR [
  QI HI SI SF DI DF
])

;; Iterator for the modes that fit in a GPR.
(define_mode_iterator FITGPR [
  QI HI SI SF DI DF
  V8QI V4HI V2SI V2SF
])

;; Iterator for all 64-bit modes.
(define_mode_iterator ALL64 [
  DI DF
  V8QI V4HI V2SI V2SF
])

;; Iterator for the 8-bit x8 vector modes.
(define_mode_iterator SIMD8X8 [
  V8QI
])

;; Iterator for the 16-bit x4 vector modes.
(define_mode_iterator SIMD16X4 [
  V4HI
])

;; Iterator for the 32-bit x2 vector modes.
(define_mode_iterator SIMD32X2 [
  V2SI V2SF
])

;; Iterator for the 64-bit vector modes.
(define_mode_iterator SIMD64 [
  V8QI V4HI V2SI V2SF
])

;; Iterator for all 128-bit modes.
(define_mode_iterator ALL128 [
  TI
  V16QI V8HI V4SI V2DI V4SF V2DF
])

;; Iterator for the 128-bit vector modes.
(define_mode_iterator SIMD128 [
  V16QI V8HI V4SI V2DI V4SF V2DF
])

;; Iterator for all 256-bit modes.
(define_mode_iterator ALL256 [
  OI
  V32QI V16HI V8SI V4DI V8SF V4DF
])

;; Iterator for the 256-bit vector modes.
(define_mode_iterator SIMD256 [
  V32QI V16HI V8SI V4DI V8SF V4DF
])

;; Iterator for all SIMD modes that have a compare (no V*QI).
(define_mode_iterator SIMDCMP [
  V4HI V2SI V2SF
  V8HI V4SI V2DI V4SF V2DF
  V16HI V8SI V4DI V8SF V4DF
])

;; Duplicate of SIMDCMP for double iteration in vcond and vcondu SPNs.
(define_mode_iterator SIMDCMP2 [
  V4HI V2SI V2SF
  V8HI V4SI V2DI V4SF V2DF
  V16HI V8SI V4DI V8SF V4DF
])

;; Iterator for all the SIMD modes.
(define_mode_iterator SIMDALL [
  V8QI V4HI V2SI V2SF
  V16QI V8HI V4SI V2DI V4SF V2DF
  V32QI V16HI V8SI V4DI V8SF V4DF
])

;; Iterator for all modes (integer, float, vector).
(define_mode_iterator ALLIFV [
  QI HI SI SF DI DF TI OI
  V8QI V4HI V2SI V2SF
  V16QI V8HI V4SI V2DI V4SF V2DF
  V32QI V16HI V8SI V4DI V8SF V4DF
])

;; Attribute for LSU and TCA builtin vector suffixes.
(define_mode_attr lsvs [
   (V8QI    "bo")
   (V4HI    "hq")
   (V2SI    "wp")
   (V2SF    "fwp")
   (V16QI   "bx")
   (V8HI    "ho")
   (V4SI    "wq")
   (V4SF    "fwq")
   (V2DI    "dp")
   (V2DF    "fdp")
   (V32QI   "bv")
   (V16HI   "hx")
   (V8SI    "wo")
   (V8SF    "fwo")
   (V4DI    "dq")
   (V4DF    "fdq")
])

;; Attribute to get the inner MODE of a vector mode.
(define_mode_attr INNER [
   (QI      "QI")
   (HI      "HI")
   (SI      "SI")
   (SF      "SF")
   (DI      "DI")
   (DF      "DF")
   (V8QI    "QI")
   (V4HI    "HI")
   (V2SI    "SI")
   (V2SF    "SF")
   (V16QI   "QI")
   (V8HI    "HI")
   (V4SI    "SI")
   (V4SF    "SF")
   (V2DI    "DI")
   (V2DF    "DF")
   (V32QI   "QI")
   (V16HI   "HI")
   (V8SI    "SI")
   (V8SF    "SF")
   (V4DI    "DI")
   (V4DF    "DF")
])

;; Attribute to get the mask MODE of a vector mode.
(define_mode_attr PRED [
   (V8QI    "V8QI")
   (V4HI    "V4HI")
   (V2SI    "V2SI")
   (V2SF    "V2SI")
   (V16QI   "V16QI")
   (V8HI    "V8HI")
   (V4SI    "V4SI")
   (V4SF    "V4SI")
   (V2DI    "V2DI")
   (V2DF    "V2DI")
   (V32QI   "V32QI")
   (V16HI   "V16HI")
   (V8SI    "V8SI")
   (V8SF    "V8SI")
   (V4DI    "V4DI")
   (V4DF    "V4DI")
])

;; Attribute to get the mask mode of a vector mode.
(define_mode_attr pred [
   (V4HI    "v4hi")
   (V2SI    "v2si")
   (V2SF    "v2si")
   (V8HI    "v8hi")
   (V4SI    "v4si")
   (V4SF    "v4si")
   (V2DI    "v2di")
   (V2DF    "v2di")
   (V16HI   "v16hi")
   (V8SI    "v8si")
   (V8SF    "v8si")
   (V4DI    "v4di")
   (V4DF    "v4di")
])

