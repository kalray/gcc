
(define_register_constraint "RXX" "(SRF_REGS)"
     "Any system register (32/64bits) constraint.")

(define_register_constraint "R32" "(TARGET_64 ? SRF32_REGS : SRF_REGS)"
     "32bits System register constraint.")

(define_register_constraint "R64" "(TARGET_64 ? SRF64_REGS : NO_REGS)"
     "64bits System register constraint.")

(define_constraint "S"
  "A symbol operand"
  (match_test "k1_symbol_operand(op,mode)"))

(define_constraint "U02"
  "An unsigned 2-bit constant."
  (and (match_code "const_int")
       (match_test "UNSIGNED_INT_FITS_N_BITS (ival, 2)")))

(define_constraint "U05"
  "An unsigned 5-bit constant."
  (and (match_code "const_int")
       (match_test "UNSIGNED_INT_FITS_N_BITS (ival, 5)")))

(define_constraint "U06"
  "An unsigned 6-bit constant."
  (and (match_code "const_int")
       (match_test "UNSIGNED_INT_FITS_N_BITS (ival, 6)")))

(define_constraint "U10"
  "An unsigned 10-bit constant."
  (and (match_code "const_int")
       (match_test "UNSIGNED_INT_FITS_N_BITS (ival, 10)")))

(define_constraint "U32"
  "An unsigned 32-bit constant."
  (and (match_code "const_int")
       (match_test "UNSIGNED_INT_FITS_N_BITS (ival, 32)")))

(define_constraint "U37"
  "An unsigned 37-bit constant."
  (and (match_code "const_int")
       (match_test "UNSIGNED_INT_FITS_N_BITS (ival, 37)")))

;; Not used yet, and does not work as-is. Macro expands
;; to shift with shift amount > size of internal repr.
;; Leaving this comment to avoid doing in the same error.
;; (define_constraint "U64"
;;   "An unsigned 64-bit constant."
;;   (and (match_code "const_int")
;;        (match_test "UNSIGNED_INT_FITS_N_BITS (ival, 64)")))

(define_constraint "I10"
  "A signed 10-bit constant."
  (and (match_code "const_int")
       (match_test "ival >= -512 && ival <= 511")))

(define_constraint "J10"
  "An unsigned 32-bit constant expressible as a signed 10."
  (and (match_code "const_int")
       (match_test "(ival >= 0 && ival <= 511) || (__builtin_clz (~ival) >= 23)")))

(define_constraint "G37"
  "An unsigned constant expressible as a signed 37."
  (and (match_code "const_double")
       (match_test "(hval >= 0 && hval <= 7) || (__builtin_clz (~hval) >= 29)")))


(define_constraint "H16"
  "A real value that fits on 16bits"
  (and (match_code "const_double")
       (match_test "k1_float_fits_bits(rval,16,mode)")))

(define_constraint "H43"
  "A real value that fits on 43bits"
  (and (match_code "const_double")
       (match_test "k1_float_fits_bits(rval,43,mode)")))

(define_constraint "I08"
  "A signed 8-bit non symbolic constant."
  (and (match_code "const_int")
       (match_test "SIGNED_INT_FITS_N_BITS (ival, 8)")))

(define_constraint "I16"  
  "A signed 16-bit non symbolic constant."
  (and (match_code "const_int")
       (match_test "SIGNED_INT_FITS_N_BITS (ival, 16)")))

(define_constraint "I32"  
  "A signed 32-bit non symbolic constant."
  (and (match_code "const_int")
       (match_test "SIGNED_INT_FITS_N_BITS (ival, 32)")))

(define_constraint "I37"
  "A signed 37-bit non symbolic constant."
   (and (match_code "const_int")
       (match_test "SIGNED_INT_FITS_N_BITS (ival, 37)")))

(define_constraint "B32"
  "A signed 32-bit constant (possibly symbolic if targeting 32bits addressing))."
   (ior (and (match_test "k1_symbol_operand(op,mode)")
             (match_test "!TARGET_64"))
        (match_test "satisfies_constraint_I32(op)")))

(define_constraint "B37"
  "A signed 37-bit constant (possibly symbolic if targeting 32bits addressing))."
   (ior (and (match_test "k1_symbol_operand(op,mode)")
             (match_test "!TARGET_64"))
        (match_test "satisfies_constraint_I37(op)")))

(define_constraint "I43"
  "A signed 43-bit non symbolic constant."
  (and (match_code "const_int")
       (match_test "SIGNED_INT_FITS_N_BITS (ival, 43)")))

;; Not used yet, and does not work as-is.
;; (define_constraint "I64"
;;   "A signed 64-bit constant."
;;   (and (match_code "const_int")
;;        (match_test "SIGNED_INT_FITS_N_BITS (ival, 64)")))

(define_constraint "Ilh"  
  "Constants that can be expressed as lowbit/highbit pair"
  (and (match_code "const_int")
       (match_test "k1_lowbit_highbit_constant_p (ival, NULL, NULL)")))

(define_memory_constraint "a"
  "Memory operands with small immediates"
  (and (match_code "mem")
       (match_test "!k1_has_big_immediate (op)")))

;; (define_memory_constraint "Q"
;;   "Packed memory operand, need not be correct."
;;   (match_code "mem"))

(define_memory_constraint "Cm"
  "Cached memory operands"
  (and (match_code "mem")
       (match_test "!K1_FORCE_UNCACHED_LSU")))

(define_memory_constraint "Ca"
  "Cached memory operands"
  (and (match_code "mem")
       (match_test "!k1_has_big_immediate (op)")
       (match_test "!K1_FORCE_UNCACHED_LSU")))

(define_memory_constraint "Zm"
  "Uncached memory operands"
  (and (match_code "mem")
       (match_test "K1_FORCE_UNCACHED_LSU")))

(define_memory_constraint "Za"
  "Uncached memory operands"
  (and (match_code "mem")
       (match_test "!k1_has_big_immediate (op)")
       (match_test "K1_FORCE_UNCACHED_LSU")))


(define_address_constraint "A"  
  "Memory address with small immediates"
  (and (match_test "address_operand (op, mode)")
       (match_test "!k1_has_big_immediate (op)")))
