
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
  "A signed 2-bits unsigned constant."
  (and (match_code "const_int") 
       (match_test "ival >= 0 && ival <= 3")))

(define_constraint "U05"
  "A signed 5-bits unsigned constant."
  (and (match_code "const_int") 
       (match_test "ival >= 0 && ival <= 31")))

(define_constraint "U06"
  "A signed 6-bits unsigned constant."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 63")))

(define_constraint "U10"
  "A signed 6-bits unsigned constant."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 1023")))

(define_constraint "U32"
  "A 32-bits unsigned constant."
  (ior (and (match_code "const_int")
            (match_test "ival >= 0 && ival <= 4294967295u"))
       (and (match_code "const_double")
            (match_test "hval == 0"))))

(define_constraint "U37"
  "A 37-bits unsigned constant."
  (ior (and (match_code "const_int")
            (match_test "ival >= 0 && ival <= 4294967295u"))
       (and (match_code "const_double")
            (match_test "hval >=0 && hval <= 4294967295u"))))

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
  "A signed 8-bit constant."
  (and (match_code "const_int")
       (match_test "ival >= -128 && ival <= 127")))

(define_constraint "I16"  
  "A signed 16-bit constant."
  (and (match_code "const_int")
       (match_test "ival >= -32768 && ival <= 32767")))

(define_constraint "I32"  
  "A signed 32-bit constant."
  (and (match_code "const_int")
       (match_test "ival >= -2147483648LL && ival <= 2147483647LL")))

(define_constraint "D37"  
  "A signed 37-bit constant."
  (ior (and (match_code "const_double")
            (match_test "hval >= -16 && hval <= 15"))
       (and (match_code "const_int")
            (match_test "ival > -68719476736LL && ival <= 68719476735LL"))))

(define_constraint "I43"
  "A signed 43-bit constant."
  (and (match_code "const_int")
       (match_test "ival >= -4398046511104LL && ival <= 4398046511103LL")))

;; (define_constraint "I64"  
;;   "A signed 64-bit constant."
;;   (ior (and (match_code "const_double")
;;             (match_test "hval >= -68719476736 && hval <= 68719476735"))
;;        (and (match_code "const_int")
;;             (match_test "ival > -68719476736 && ival <= 68719476735"))))

(define_constraint "Ilh"  
  "Constants that can be expressed as lowbit/highbit pair"
  (and (match_code "const_int")
       (match_test "k1_lowbit_highbit_constant_p (ival, NULL, NULL)")))

(define_memory_constraint "a"  
  "Memory operands with small immediates"
  (and (match_code "mem")
       (match_test "!k1_has_big_immediate (op)")))

(define_memory_constraint "Q"  
  "Packed memory operand, need not be correct."
  (match_code "mem"))

(define_address_constraint "A"  
  "Memory address with small immediates"
  (and (match_test "address_operand (op, mode)")
       (match_test "!k1_has_big_immediate (op)")))
