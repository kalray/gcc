;; Iterator for SI, QI and HI modes
(define_mode_iterator SHORT [QI HI SI])

;; Mapping between GCC modes and suffixes used by LSU insns
(define_mode_attr lsusize [
 (QI "b")
 (HI "h")
 (SI "w")
 (DI "d")
 (TI "q")
 (OI "o")
 (SF "w")
 (DF "d")])

;; Suffix for extension when applicable (for LSU narrower than a
;; register). Leave empty if not applicable
(define_mode_attr lsusext [
  (QI "z")
  (HI "z")
  (SI "z")
  (DI "")
  (TI "")
  (OI "")
  (SF "z")
  (DF "")])

(define_mode_attr hq [(HI "h") (QI "q")])

;; Code iterator for sign/zero extension
(define_code_iterator MAX_UMAX [smax umax])
(define_code_iterator MIN_UMIN [smin umin])

(define_code_attr spfx [(smax "s") (umax "u")
     (smin "s") (umin "u")
])

;; Code iterator for sign/zero extension
(define_code_iterator ANY_EXTEND [sign_extend zero_extend])

;; Sign- or zero-extending data-op
(define_code_attr lsext [(sign_extend "s") (zero_extend "z")])

;; Count lead/trailing zero
(define_code_iterator ANY_ZERO_COUNT [ctz clz])
(define_code_attr c_tl  [(ctz "t") (clz "l")])

;; Sign- or zero-extending mapping to unsigned mnemonics
(define_code_attr ssfx [(sign_extend "") (zero_extend "u")
     (smax "") (smin "")
     (umax "u") (umin "u")
])

;; Iterator for all integer modes (up to 64-bit)
(define_mode_iterator ALLI [QI HI SI DI])

;; Iterator for all integer modes smaller than 64bits
(define_mode_iterator ALL_SMALL_I [QI HI SI])

;; Iterator for all float modes (up to 64-bit)
(define_mode_iterator ALLF [SF DF])

;; Iterator for all float modes (up to 64-bit)
(define_mode_iterator ALLMF [SF DF])

(define_mode_attr sfx [
  (SF "w")
  (DF "d")
  (QI "w")
  (HI "w")
  (SI "w")
  (DI "d")
  (TI "q")
  (OI "o")])

(define_mode_attr fmasfx [(SF "w") (DF "d")] )

;; All modes used by the mov pattern that fit in a register.
;; TI and OI and to be handled elsewhere.
(define_mode_iterator ALLIF [QI HI SI DI SF DF])

(define_mode_iterator ALLP [SI DI])

(define_mode_iterator P [(SI "Pmode == SImode") (DI "Pmode == DImode")])

(define_code_iterator cb_cond [eq ne gt ge lt le])
(define_mode_iterator SIDI [SI DI])

;; Used for conditional LSU with .odd/.even
(define_code_iterator COND_ODD_EVEN [eq ne])
(define_code_attr lsu_odd_even [(eq "even") (ne "odd")])

;; FIXME AUTO: change name of cbvar, used elsewhere.
(define_mode_attr cbvar [(SI "w") (DI "d") (SF "w") (DF "d")])

;; FIXME AUTO: disabling vector support
;;(define_mode_iterator SISIZE [SI SF V2HI])
(define_mode_iterator SISIZE [SI SF])

(define_mode_iterator SISIZESCALAR [SI SF])

;; FIXME AUTO: disabling vector support
;;(define_mode_iterator DISIZE [DI DF V2SI V4HI])
(define_mode_iterator DISIZE [DI DF])

(define_mode_iterator DISIZESCALAR [DI DF])

;; FIXME AUTO: disabling vector support
;;(define_mode_iterator ALLMODES [DI DF V4HI V2SI SI SF V2HI HI QI])

(define_mode_iterator ALLMODES [DI DF SI SF HI QI])

(define_mode_attr lite_prefix [(SI "") (DI "alud_")])

;;(define_mode_attr suffix32b [(SI "w") (DI "")])

(define_mode_attr suffix [(SI "w") (DI "d")])

(define_mode_attr suffix_opx [(SI "") (DI "d_x")])
(define_mode_attr suffix2 [(SI "w") (DI "d")])
(define_mode_attr regclass [(SI "r") (DI "r")])
(define_mode_attr size [(SI "4") (DI "8")])

;; insns length for materializing a symbol depending on pointer size,
;; using make insn. Alternatives using these should only be enabled
;; for valid pointer modes: SI or DI. Anything else is an error.
;; Values 999 are used for modes where the alternative must always be disabled.
(define_mode_attr symlen1 [(SI "_x") (DI "_y") (QI "") (HI "") (SF "") (DF "")])
(define_mode_attr symlen2 [(SI "8") (DI "12") (QI "999") (HI "999") (SF "999") (DF "999")])

;; Used to also select AUX WRITE
(define_mode_attr auxw [(SI "") (DI "") (QI "") (HI "") (TI "_auxw") (OI "_auxw") (SF "") (DF "")])

(define_mode_attr sbfx_resrv [(SI "tiny") (DI "alud_lite")])

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
