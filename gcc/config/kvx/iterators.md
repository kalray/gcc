;; Mapping between GCC modes and suffixes used by LSU insns
(define_mode_attr lsusize [
  (QI "b")
  (HI "h")
  (HF "h")
  (SI "w")
  (SF "w")
  (DI "d")
  (DF "d")
  (HC "w")
  (SC "d")
  (TI "q")
  (OI "o")
  (V8QI "d")
  (V4HI "d")
  (V4HF "d")
  (V2SI "d")
  (V2SF "d")
  (V16QI "q")
  (V8HI "q")
  (V8HF "q")
  (V4SI "q")
  (V2DI "q")
  (V4SF "q")
  (V2DF "q")
  (V32QI "o")
  (V16HI "o")
  (V16HF "o")
  (V8SI "o")
  (V4DI "o")
  (V8SF "o")
  (V4DF "o")
])

;; Zero extension for LSU when applicable (mode narrower than a register).
(define_mode_attr lsuzx [
  (QI "z")
  (HI "z")
  (HF "z")
  (SI "z")
  (SF "z")
  (HC "z")
  (DI "")
  (DF "")
  (TI "")
  (OI "")
  (V8QI "")
  (V4HI "")
  (V4HF "")
  (V2SI "")
  (V2SF "")
  (V16QI "")
  (V8HI "")
  (V8HF "")
  (V4SI "")
  (V2DI "")
  (V4SF "")
  (V2DF "")
  (V32QI "")
  (V16HI "")
  (V16HF "")
  (V8SI "")
  (V4DI "")
  (V8SF "")
  (V4DF "")
])

;; Code iterator for sign/zero extension
(define_code_iterator ANY_EXTEND [sign_extend zero_extend])

;; Sign-extending or zero-extending sub-word load instructions.
(define_code_attr lsux [
  (sign_extend "s")
  (zero_extend "z")
])

;; Sign-extending or zero-extending in integer instructions.
(define_code_attr unsx [
  (sign_extend "")
  (zero_extend "u")
])

;; Iterator for all integer modes (up to 64-bit)
(define_mode_iterator ALLI [QI HI SI DI])

;; Iterator for SI, QI and HI modes
(define_mode_iterator SHORT [QI HI SI])

;; Iterator for floating-point compare modes (up to 64-bit)
(define_mode_iterator ALLF [SF DF])

;; Attribute for ALLF compares.
(define_mode_attr cfx [
  (SF "w")
  (DF "d")
])

;; Scalar modes used by the mov pattern that fit in a register.
;; TI and OI and to be handled elsewhere.
;; (define_mode_iterator ALLIF [QI HI HF SI SF DI DF HC SC])
(define_mode_iterator ALLIF [QI HI HF SI SF DI DF])

;; Attribute for ALLIF copies (COPYW, COPYD, COPYQ, COPYO).
(define_mode_attr copyx [
  (QI "w")
  (HI "w")
  (HF "w")
  (SI "w")
  (SF "w")
  (DI "d")
  (DF "d")
  (HC "w")
  (SC "d")
  (TI "q")
  (OI "o")
])

(define_mode_iterator ALLP [SI DI])

(define_mode_iterator P [(SI "Pmode == SImode") (DI "Pmode == DImode")])

(define_mode_iterator SIDI [SI DI])

;; insns length for materializing a symbol depending on pointer size,
;; using make insn. Alternatives using these should only be enabled
;; for valid pointer modes: SI or DI. Anything else is an error.
;; Values 999 are used for modes where the alternative must always be disabled.
(define_mode_attr symlen1 [
  (SI "_x") (DI "_y") (QI "") (HI "") (HF "") (SF "") (DF "") (HC "") (SC "")
])
(define_mode_attr symlen2 [
  (SI "8") (DI "12") (QI "999") (HI "999") (HF "999") (SF "999") (DF "999") (HC "999") (SC "999")
])

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

;; Iterator for the modes that fit in a GPR for CMOVED.
(define_mode_iterator FITGPR [
  QI HI HF SI SF DI DF HC SC
  V8QI V4HI V4HF V2SI V2SF
])

;; Iterator for all 64-bit modes.
(define_mode_iterator ALL64 [
  DI DF SC
  V8QI V4HI V4HF V2SI V2SF
])

;; Iterator for the 8-bit x8 vector modes.
(define_mode_iterator SIMD8X8 [
  V8QI
])

;; Iterator for the 16-bit x4 vector modes.
(define_mode_iterator SIMD16X4 [
  V4HI V4HF
])

;; Iterator for the 32-bit x2 vector modes.
(define_mode_iterator SIMD32X2 [
  V2SI V2SF
])

;; Iterator for the 64-bit vector modes.
(define_mode_iterator SIMD64 [
  V8QI V4HI V4HF V2SI V2SF
])

;; Iterator for all 128-bit modes.
(define_mode_iterator ALL128 [
  TI
  V16QI V8HI V8HF V4SI V2DI V4SF V2DF
])

;; Iterator for the 128-bit vector modes.
(define_mode_iterator SIMD128 [
  V16QI V8HI V8HF V4SI V2DI V4SF V2DF
])

;; Iterator for all 256-bit modes.
(define_mode_iterator ALL256 [
  OI
  V32QI V16HI V16HF V8SI V4DI V8SF V4DF
])

;; Iterator for the 256-bit vector modes.
(define_mode_iterator SIMD256 [
  V32QI V16HI V16HF V8SI V4DI V8SF V4DF
])

;; Iterator for all SIMD modes that have a compare (no V*QI).
(define_mode_iterator SIMDCMP [
  V4HI V4HF V2SI V2SF
  V8HI V8HF V4SI V2DI V4SF V2DF
  V16HI V16HF V8SI V4DI V8SF V4DF
])

;; Duplicate of SIMDCMP for double iteration in vcond and vcondu SPNs.
(define_mode_iterator SIMDCMP2 [
  V4HI V4HF V2SI V2SF
  V8HI V8HF V4SI V2DI V4SF V2DF
  V16HI V16HF V8SI V4DI V8SF V4DF
])

;; Iterator for all the SIMD modes.
(define_mode_iterator SIMDALL [
  V8QI V4HI V4HF V2SI V2SF
  V16QI V8HI V8HF V4SI V2DI V4SF V2DF
  V32QI V16HI V16HF V8SI V4DI V8SF V4DF
])

;; Iterator for all modes (integer, float, vector) for MOV*CC.
(define_mode_iterator ALLIFV [
  QI HI HF SI SF DI DF TI OI
  V8QI V4HI V4HF V2SI V2SF
  V16QI V8HI V8HF V4SI V2DI V4SF V2DF
  V32QI V16HI V16HF V8SI V4DI V8SF V4DF
])

;; Attribute for arithmetic scalar and vector suffixes.
(define_mode_attr suffix [
  (HI      "h")
  (SI      "w")
  (DI      "d")
  (HF      "h")
  (SF      "w")
  (DF      "d")
  (V8QI    "bo")
  (V4HI    "hq")
  (V4HF    "hq")
  (V2SI    "wp")
  (V2SF    "wp")
  (V16QI   "bx")
  (V8HI    "ho")
  (V8HF    "ho")
  (V4SI    "wq")
  (V4SF    "wq")
  (V2DI    "dp")
  (V2DF    "dp")
  (V32QI   "bv")
  (V16HI   "hx")
  (V16HF   "hx")
  (V8SI    "wo")
  (V8SF    "wo")
  (V4DI    "dq")
  (V4DF    "dq")
])

;; Attribute for LSU and TCA builtin vector suffixes.
(define_mode_attr lsvs [
  (DI      "d")
  (DF      "fd")
  (SC      "fwc")
  (TI      "q")
  (OI      "o")
  (V8QI    "bo")
  (V4HI    "hq")
  (V4HF    "fhq")
  (V2SI    "wp")
  (V2SF    "fwp")
  (V16QI   "bx")
  (V8HI    "ho")
  (V8HF    "fho")
  (V4SI    "wq")
  (V4SF    "fwq")
  (V2DI    "dp")
  (V2DF    "fdp")
  (V32QI   "bv")
  (V16HI   "hx")
  (V16HF   "fhx")
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
  (HF      "HF")
  (SF      "SF")
  (DI      "DI")
  (DF      "DF")
  (SC      "SC")
  (V8QI    "QI")
  (V4HI    "HI")
  (V4HF    "HF")
  (V2SI    "SI")
  (V2SF    "SF")
  (V16QI   "QI")
  (V8HI    "HI")
  (V8HF    "HF")
  (V4SI    "SI")
  (V4SF    "SF")
  (V2DI    "DI")
  (V2DF    "DF")
  (V32QI   "QI")
  (V16HI   "HI")
  (V16HF   "HF")
  (V8SI    "SI")
  (V8SF    "SF")
  (V4DI    "DI")
  (V4DF    "DF")
])

;; Attribute to get the mask MODE of a vector mode.
(define_mode_attr MASK [
  (V8QI    "V8QI")
  (V4HI    "V4HI")
  (V4HF    "V4HI")
  (V2SI    "V2SI")
  (V2SF    "V2SI")
  (V16QI   "V16QI")
  (V8HI    "V8HI")
  (V8HF    "V8HI")
  (V4SI    "V4SI")
  (V4SF    "V4SI")
  (V2DI    "V2DI")
  (V2DF    "V2DI")
  (V32QI   "V32QI")
  (V16HI   "V16HI")
  (V16HF   "V16HI")
  (V8SI    "V8SI")
  (V8SF    "V8SI")
  (V4DI    "V4DI")
  (V4DF    "V4DI")
])

;; Attribute to get the mask mode of a vector mode.
(define_mode_attr mask [
  (V8QI    "v8qi")
  (V4HI    "v4hi")
  (V4HF    "v4hi")
  (V2SI    "v2si")
  (V2SF    "v2si")
  (V16QI   "v16qi")
  (V8HI    "v8hi")
  (V8HF    "v8hi")
  (V4SI    "v4si")
  (V4SF    "v4si")
  (V2DI    "v2di")
  (V2DF    "v2di")
  (V32QI   "v32qi")
  (V16HI   "v16hi")
  (V16HF   "v16hi")
  (V8SI    "v8si")
  (V8SF    "v8si")
  (V4DI    "v4di")
  (V4DF    "v4di")
])

;; Attribute to get the half MODE of a vector mode.
(define_mode_attr HALF [
  (V16QI   "V8QI")
  (V8HI    "V4HI")
  (V8HF    "V4HF")
  (V4SI    "V2SI")
  (V4SF    "V2SF")
  (V2DI    "DI")
  (V2DF    "DF")
  (V32QI   "V16QI")
  (V16HI   "V8HI")
  (V16HF   "V8HF")
  (V8SI    "V4SI")
  (V8SF    "V4SF")
  (V4DI    "V2DI")
  (V4DF    "V2DF")
])

;; Attribute to get the half mode of a vector mode.
(define_mode_attr half [
  (V16QI   "v8qi")
  (V8HI    "v4hi")
  (V8HF    "v4hf")
  (V4SI    "v2si")
  (V4SF    "v2sf")
  (V2DI    "di")
  (V2DF    "df")
  (V32QI   "v16qi")
  (V16HI   "v8hi")
  (V16HF   "v8hf")
  (V8SI    "v4si")
  (V8SF    "v4sf")
  (V4DI    "v2di")
  (V4DF    "v2df")
])

;; Attribute to get the half mask MODE of a vector mode.
(define_mode_attr HMASK [
  (V16QI   "V8QI")
  (V8HI    "V4HI")
  (V8HF    "V4HI")
  (V4SI    "V2SI")
  (V4SF    "V2SI")
  (V2DI    "DI")
  (V2DF    "DI")
  (V32QI   "V32QI")
  (V16HI   "V8HI")
  (V16HF   "V8HI")
  (V8SI    "V4SI")
  (V8SF    "V4SI")
  (V4DI    "V2DI")
  (V4DF    "V2DI")
])

;; Attribute to get the wide MODE of a vector mode.
(define_mode_attr WIDE [
  (V8QI    "V8HI")
  (V4HI    "V4SI")
  (V4HF    "V4SF")
  (V2SI    "V2DI")
  (V2SF    "V2DF")
  (V16QI   "V16HI")
  (V8HI    "V8SI")
  (V8HF    "V8SF")
  (V4SI    "V4DI")
  (V4SF    "V4DF")
])

;; Attribute to get the wide mode of a vector mode.
(define_mode_attr wide [
  (V8QI    "v8hi")
  (V4HI    "v4si")
  (V4HF    "v4sf")
  (V2SI    "v2di")
  (V2SF    "v2df")
  (V16QI   "v16hi")
  (V8HI    "v8si")
  (V8HF    "v8sf")
  (V4SI    "v4di")
  (V4SF    "v4df")
])

;; Attribute to get the wide suffix of a vector mode.
(define_mode_attr widex [
  (V8QI    "ho")
  (V4HI    "wq")
  (V4HF    "wq")
  (V2SI    "dp")
  (V2SF    "dp")
  (V16QI   "hx")
  (V8HI    "wo")
  (V8HF    "wo")
  (V4SI    "dq")
  (V4SF    "dq")
])

;; Attribute to get the widening suffix of a vector mode.
(define_mode_attr widenx [
  (V8QI    "bho")
  (V4HI    "hwq")
  (V4HF    "hwq")
  (V2SI    "wdp")
  (V2SF    "wdp")
  (V16QI   "bhx")
  (V8HI    "hwo")
  (V8HF    "hwo")
  (V4SI    "wdq")
  (V4SF    "wdq")
])

;; Attribute to get the half wide MODE of a vector mode.
(define_mode_attr HWIDE [
  (V8QI    "V4HI")
  (V4HI    "V2SI")
  (V4HF    "V2SF")
  (V2SI    "DI")
  (V2SF    "DF")
  (V16QI   "V8HI")
  (V8HI    "V4SI")
  (V8HF    "V4SF")
  (V4SI    "V2DI")
  (V4SF    "V2DF")
])

;; Attribute to get the half wide mode of a vector mode.
(define_mode_attr hwide [
  (V8QI    "v4hi")
  (V4HI    "v2si")
  (V4HF    "v2sf")
  (V2SI    "di")
  (V2SF    "df")
  (V16QI   "v8hi")
  (V8HI    "v4si")
  (V8HF    "v4sf")
  (V4SI    "v2di")
  (V4SF    "v2df")
])

;; Attribute to get the half widening suffix of a vector mode.
(define_mode_attr hwidenx [
  (V8QI    "bhq")
  (V4HI    "hwp")
  (V4HF    "hwp")
  (V2SI    "wd")
  (V2SF    "wd")
  (V16QI   "bho")
  (V8HI    "hwq")
  (V8HF    "hwq")
  (V4SI    "wdp")
  (V4SF    "wdp")
])

;; Attribute to get the trunc(ate) suffix of a vector mode.
(define_mode_attr truncx [
  (V8QI    "hbo")
  (V4HI    "whq")
  (V4HF    "whq")
  (V2SI    "dwp")
  (V2SF    "dwp")
  (V16QI   "hbx")
  (V8HI    "who")
  (V8HF    "who")
  (V4SI    "dwq")
  (V4SF    "dwq")
])

;; Attribute to get the trunc(ate) attribute type of fnarrow<truncx>
(define_mode_attr fnarrowt [
  (V4HF    "alu_lite")
  (V2SF    "alu_full")
])

;; Attribute to get the half trunc(ate) suffix of a vector mode.
(define_mode_attr htruncx [
  (V8QI    "hbq")
  (V4HI    "whp")
  (V4HF    "whp")
  (V2SI    "dw")
  (V2SF    "dw")
  (V16QI   "hbo")
  (V8HI    "whq")
  (V8HF    "whq")
  (V4SI    "dwp")
  (V4SF    "dwp")
])

;; Attribute to get the chunk MODE of a vector mode.
(define_mode_attr CHUNK [
  (V8QI    "V8QI")
  (V4HI    "V4HI")
  (V4HF    "V4HF")
  (V2SI    "V2SI")
  (V2SF    "V2SF")
  (DI      "DI")
  (DF      "DF")
  (V16QI   "V8QI")
  (V8HI    "V4HI")
  (V8HF    "V4HF")
  (V4SI    "V2SI")
  (V4SF    "V2SF")
  (V2DI    "DI")
  (V2DF    "DF")
  (V32QI   "V8QI")
  (V16HI   "V4HI")
  (V16HF   "V4HF")
  (V8SI    "V2SI")
  (V8SF    "V2SF")
  (V4DI    "DI")
  (V4DF    "DF")
])

;; Attribute to get the chunk mode of a vector mode.
(define_mode_attr chunk [
  (V8QI    "v8qi")
  (V4HI    "v4hi")
  (V4HF    "v4hf")
  (V2SI    "v2si")
  (V2SF    "v2sf")
  (DI      "di")
  (DF      "df")
  (V16QI   "v8qi")
  (V8HI    "v4hi")
  (V8HF    "v4hf")
  (V4SI    "v2si")
  (V4SF    "v2sf")
  (V2DI    "di")
  (V2DF    "df")
  (V32QI   "v8qi")
  (V16HI   "v4hi")
  (V16HF   "v4hf")
  (V8SI    "v2si")
  (V8SF    "v2sf")
  (V4DI    "di")
  (V4DF    "df")
])

;; Attribute to get the chunk suffix of a vector mode.
(define_mode_attr chunkx [
  (V8QI    "bo")
  (V4HI    "hq")
  (V4HF    "hq")
  (V2SI    "wp")
  (V2SF    "wp")
  (DI      "d")
  (DF      "d")
  (V16QI   "bo")
  (V8HI    "hq")
  (V8HF    "hq")
  (V4SI    "wp")
  (V4SF    "wp")
  (V2DI    "d")
  (V2DF    "d")
  (V32QI   "bo")
  (V16HI   "hq")
  (V16HF   "hq")
  (V8SI    "wp")
  (V8SF    "wp")
  (V4DI    "d")
  (V4DF    "d")
])

;; Attribute to get the suffix of a vector by scalar instruction.
(define_mode_attr chunkxs [
  (V8QI    "bos")
  (V4HI    "hqs")
  (V4HF    "hqs")
  (V2SI    "wps")
  (V2SF    "wps")
  (DI      "d")
  (DF      "d")
  (V16QI   "bos")
  (V8HI    "hqs")
  (V8HF    "hqs")
  (V4SI    "wps")
  (V4SF    "wps")
  (V2DI    "d")
  (V2DF    "d")
  (V32QI   "bos")
  (V16HI   "hqs")
  (V16HF   "hqs")
  (V8SI    "wps")
  (V8SF    "wps")
  (V4DI    "d")
  (V4DF    "d")
])

;; Iterator for the small elements 64-bit vector integer modes.
(define_mode_iterator S64I [
  V4HI V2SI
])

;; Iterator for V8QI S64I
(define_mode_iterator S64K [
  V8QI V4HI V2SI
])

;; Iterator for S64K minus V2SI
(define_mode_iterator S64L [
  V8QI V4HI
])

;; Iterator for the small elements 128-bit vector integer modes.
(define_mode_iterator S128I [
  V8HI V4SI
])

;; Iterator for V8HI V2DI
(define_mode_iterator V128I [
  V8HI V2DI
])

;; Iterator for the non-byte 128-bit vector integer modes.
(define_mode_iterator V128J [
  V8HI V4SI V2DI
])

;; Iterator for V16QI S128I
(define_mode_iterator S128K [
  V16QI V8HI V4SI
])

;; Iterator for the small elements 256-bit vector integer modes.
(define_mode_iterator S256I [
  V16HI V8SI
])

;; Iterator for V16HI V4DI
(define_mode_iterator V256I [
  V16HI V4DI
])

;; Iterator for the non-byte 256-bit vector integer modes.
(define_mode_iterator V256J [
  V16HI V8SI V4DI
])

;; Iterator for the small elements 64-bit vector FP modes.
(define_mode_iterator S64F [
  V4HF V2SF
])

;; Iterator for the small elements 128-bit vector FP modes.
(define_mode_iterator S128F [
  V8HF V4SF
])

;; Iterator for all the 128-bit vector FP modes.
(define_mode_iterator V128F [
  V8HF V4SF V2DF
])

;; Iterator for the small elements 256-bit vector FP modes.
(define_mode_iterator S256F [
  V16HF V8SF
])

;; Iterator for all the 256-bit vector FP modes.
(define_mode_iterator V256F [
  V16HF V8SF V4DF
])

;; Iterator for V8QI S64I S64F
(define_mode_iterator S64A [
  V8QI V4HI V2SI
  V4HF V2SF
])

;; Iterator for V16QI S128I S128F
(define_mode_iterator S128A [
  V16QI V8HI V4SI
  V8HF V4SF
])

;; Iterator for V32QI S256I S256F
(define_mode_iterator S256A [
  V32QI V16HI V8SI
  V16HF V8SF
])

;; Iterator for the 64-bit elements 128-bit vector modes.
(define_mode_iterator W128A [
  V2DI V2DF
])

;; Iterator for the 64-bit elements 256-bit vector modes.
(define_mode_iterator W256A [
  V4DI V4DF
])

;; Iterator for the vector SF modes.
(define_mode_iterator VXSF [
  V2SF
  V4SF
  V8SF
])

