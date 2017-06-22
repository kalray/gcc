;; Iterator for SI, QI and HI modes
(define_mode_iterator SHORT [QI HI SI])

(define_mode_attr lsusize [(QI "b") (HI "h") (SI "w") (DI "d") (SF "w") (DF "d")])

(define_mode_attr lsusext [(QI "z") (HI "z") (SI "z") (DI "") (SF "z") (DF "")])
(define_mode_attr hq [(HI "h") (QI "q")])

;; Iterator for all integer modes (up to 64-bit)
(define_mode_iterator ALLI [QI HI SI DI])

;; Iterator for all float modes (up to 64-bit)
(define_mode_iterator ALLF [SF DF])

(define_mode_attr sfx [(SF "w") (DF "d") (SI "w") (DI "d")] )
(define_mode_attr fmasfx [(SF "") (DF "d")] )

;; All modes that fit in 1 register. Used for LSU.
(define_mode_iterator ALLIF [QI HI SI DI SF DF])

(define_mode_iterator ALLP [SI DI])

(define_mode_iterator P [(SI "Pmode == SImode") (DI "Pmode == DImode")])

(define_mode_attr SRFSIZE [(SI "R32") (DI "R64")])
(define_mode_attr SRFSIZEp [(SI "32") (DI "64")])

(define_code_iterator cb_cond [eq ne gt ge lt le])
(define_mode_iterator SIDI [SI DI])

(define_mode_attr cbvar [(SI "w") (DI "d") (SF "w") (DF "d")])

;; FIXME AUTO: disabling vector support
;;(define_mode_iterator SISIZE [SI SF V2HI])
(define_mode_iterator SISIZE [SI SF ])

(define_mode_iterator SISIZESCALAR [SI SF])

;; FIXME AUTO: disabling vector support
;;(define_mode_iterator DISIZE [DI DF V2SI V4HI])
(define_mode_iterator DISIZE [DI DF])

(define_mode_iterator DISIZESCALAR [DI DF])

;; FIXME AUTO: disabling vector support
;;(define_mode_iterator ALLMODES [DI DF V4HI V2SI SI SF V2HI HI QI])

(define_mode_iterator ALLMODES [DI DF SI SF HI QI])

(define_mode_iterator I [(SI "") (DI "")])
(define_mode_attr lite_prefix [(SI "") (DI "alud_")])

;;(define_mode_attr suffix32b [(SI "w") (DI "")])

(define_mode_attr suffix [(SI "w") (DI "d")])

(define_mode_attr suffix_opx [(SI "") (DI "d_x")])
(define_mode_attr suffix2 [(SI "w") (DI "d")])
(define_mode_attr regclass [(SI "r") (DI "r")])
(define_mode_attr size [(SI "4") (DI "8")])

(define_mode_attr sbfx_resrv [(SI "tiny") (DI "alud_lite")])
