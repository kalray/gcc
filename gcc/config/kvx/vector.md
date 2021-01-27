;; 64-bit Vector Moves

(define_expand "mov<mode>"
  [(set (match_operand:SIMD64 0 "nonimmediate_operand" "")
        (match_operand:SIMD64 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn "*mov<mode>"
  [(set (match_operand:SIMD64 0 "nonimmediate_operand" "=r, r, r, r, r, r, r,a,b,m,  r,  r,r")
        (match_operand:SIMD64 1 "general_operand"       "r,Ca,Cb,Cm,Za,Zb,Zm,r,r,r,v16,v43,i"))]
  "(!immediate_operand(operands[1], <MODE>mode) || !memory_operand(operands[0], <MODE>mode))"
  {
    switch (which_alternative)
      {
      case 0:
        return "copyd %0 = %1";
      case 1: case 2: case 3: case 4: case 5: case 6:
        return "ld%C1%m1 %0 = %1";
      case 7: case 8: case 9:
        return "sd%m0 %0 = %1";
      case 10: case 11: case 12:
        return "make %0 = %1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "alu_tiny,lsu_auxw_load,lsu_auxw_load_x,lsu_auxw_load_y,lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y,lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,            4,              8,             12,                     4,                       8,                      12,             4,               8,              12,       4,         8,        12")]
)


;; 128-bit Vector Moves

(define_expand "mov<mode>"
  [(set (match_operand:ALL128 0 "nonimmediate_operand" "")
        (match_operand:ALL128 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn_and_split "*mov<mode>_oddreg"
  [(set (match_operand:ALL128 0 "nonimmediate_operand" "=r  , m")
        (match_operand:ALL128 1 "general_operand"      " irm, r"))]
  "(kvx_is_reg_subreg_p (operands[0]) && !kvx_ok_for_paired_reg_p (operands[0]))
    || (kvx_is_reg_subreg_p (operands[1]) && !kvx_ok_for_paired_reg_p (operands[1]))"
  "#"
  "&& reload_completed"
  [(const_int 0)]
  {
    /* This should only happen during function argument preparation */
    kvx_split_128bits_move (operands[0], operands[1], <MODE>mode);
    DONE;
  }
)

(define_insn "*mov<mode>"
  [(set (match_operand:ALL128 0 "kvx_nonimmediate_operand_pair" "=r, r, r, r, r, r, r,a,b,m")
        (match_operand:ALL128 1 "kvx_nonimmediate_operand_pair" " r,Ca,Cb,Cm,Za,Zb,Zm,r,r,r"))]
  "(!immediate_operand(operands[1], <MODE>mode) || !memory_operand(operands[0], <MODE>mode))"
  {
    switch (which_alternative)
      {
      case 0:
        return kvx_asm_pat_copyq (operands[1]);
      case 1: case 2: case 3: case 4: case 5: case 6:
        return "lq%C1%m1 %0 = %1";
      case 7: case 8: case 9:
        return "sq%m0 %0 = %1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "mau,lsu_auxw_load,lsu_auxw_load_x,lsu_auxw_load_y,lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y,lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length" "4,             4,             8,             12,                     4,                       8,                      12,             4,               8,              12")]
)

;; Split what would end-up in a copyq in 2 copyd.
;; copyd uses 1 TINY each instead of the MAU used by copyq
;; at the cost of an extra word on insn
(define_split
  [(set (match_operand:ALL128 0 "register_operand" "")
         (match_operand:ALL128 1 "register_operand" ""))]
  "!optimize_size && reload_completed"
  [(const_int 0)]
  {
    kvx_split_128bits_move (operands[0], operands[1], <MODE>mode);
    DONE;
  }
)

(define_insn_and_split "*mov<mode>_immediate"
    [(set (match_operand:ALL128 0 "register_operand" "=r")
          (match_operand:ALL128 1 "immediate_operand" "i" ))]
  ""
  "#"
  "&& reload_completed"
  [(const_int 0)]
  {
   kvx_split_128bits_move (operands[0], operands[1], <MODE>mode);
   DONE;
  }
)


;; 256-bit Vector Moves

(define_expand "mov<mode>"
  [(set (match_operand:ALL256 0 "nonimmediate_operand" "")
        (match_operand:ALL256 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn_and_split "*mov<mode>_misalign_reg"
  [(set (match_operand:ALL256 0 "nonimmediate_operand" "=r,   m")
        (match_operand:ALL256 1 "general_operand"      " irm, r"))]
  "(kvx_is_reg_subreg_p (operands[0]) && !kvx_ok_for_quad_reg_p (operands[0]))
    || (kvx_is_reg_subreg_p (operands[1]) && !kvx_ok_for_quad_reg_p (operands[1]))"
  "#"
  "&& reload_completed"
  [(const_int 0)]
  {
    /* This should only happen during function argument preparation */
    kvx_split_256bits_move (operands[0], operands[1], <MODE>mode);
    DONE;
  }
)

(define_insn "*mov<mode>"
  [(set (match_operand:ALL256 0 "kvx_nonimmediate_operand_quad" "=r, r, r, r, r, r, r,a,b,m")
        (match_operand:ALL256 1 "kvx_nonimmediate_operand_quad"  "r,Ca,Cb,Cm,Za,Zb,Zm,r,r,r"))]
  "(!immediate_operand(operands[1], <MODE>mode) || !memory_operand(operands[0], <MODE>mode))"
  {
    switch (which_alternative)
      {
      case 0:
        return kvx_asm_pat_copyo ();
      case 1: case 2: case 3: case 4: case 5: case 6:
        return "lo%C1%m1 %0 = %1";
      case 7: case 8: case 9:
        return "so%m0 %0 = %1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "lsu_auxr_auxw,lsu_auxw_load,lsu_auxw_load_x,lsu_auxw_load_y,lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y,lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"           "4,            4,              8,             12,                     4,                       8,                      12,             4,               8,              12")]
)

;; Split what would end-up in a copyo in 4 copyd.
;; copyd uses 1 TINY each instead of the LSU used by copyo
;; at the cost of 3 extra word on insn
(define_split
  [(set (match_operand:ALL256 0 "register_operand" "")
         (match_operand:ALL256 1 "register_operand" ""))]
  "!optimize_size && reload_completed"
  [(const_int 0)]
  {
    kvx_split_256bits_move (operands[0], operands[1], <MODE>mode);
    DONE;
  }
)

(define_insn_and_split "*mov<mode>_immediate"
    [(set (match_operand:ALL256 0 "register_operand" "=r")
          (match_operand:ALL256 1 "immediate_operand" "i" ))]
  ""
  "#"
  "&& reload_completed"
  [(const_int 0)]
  {
   kvx_split_256bits_move (operands[0], operands[1], <MODE>mode);
   DONE;
  }
)

;; Vector Set/Extract/Init/Perm/Shr

(define_expand "vec_set<mode>"
  [(match_operand:SIMDALL 0 "register_operand" "")
   (match_operand:<INNER> 1 "register_operand" "")
   (match_operand 2 "const_int_operand" "")]
  ""
  {
    rtx target = operands[0];
    rtx source = operands[1];
    rtx where = operands[2];
    kvx_expand_vector_insert (target, source, where);
    DONE;
  }
)

(define_expand "vec_extract<mode>"
  [(match_operand:<INNER> 0 "register_operand" "")
   (match_operand:SIMDALL 1 "register_operand" "")
   (match_operand 2 "const_int_operand" "")]
  ""
  {
    rtx target = operands[0];
    rtx source = operands[1];
    rtx where = operands[2];
    kvx_expand_vector_extract (target, source, where);
    DONE;
  }
)

(define_expand "vec_init<mode>"
  [(match_operand:SIMDALL 0 "register_operand" "")
   (match_operand:SIMDALL 1 "" "")]
  ""
  {
    rtx target = operands[0];
    rtx source = operands[1];
    kvx_expand_vector_init (target, source);
    DONE;
  }
)

(define_expand "vec_perm_const<mode>"
  [(match_operand:SIMDALL 0 "register_operand" "")
   (match_operand:SIMDALL 1 "register_operand" "")
   (match_operand:SIMDALL 2 "register_operand" "")
   (match_operand:<MASK> 3 "" "")]
  ""
  {
    rtx target = operands[0];
    rtx source1 = operands[1];
    rtx source2 = operands[2];
    rtx selector = operands[3];
    kvx_expand_vec_perm_const (target, source1, source2, selector);
    DONE;
  }
)

(define_expand "vec_cmp<mode><mask>"
  [(set (match_operand:<MASK> 0 "register_operand")
        (match_operator 1 "comparison_operator"
         [(match_operand:SIMDCMP 2 "register_operand")
          (match_operand:SIMDCMP 3 "reg_or_zero_operand")]))]
  ""
  {
    rtx mask = operands[0];
    rtx cmp = operands[1];
    rtx left = operands[2];
    rtx right = operands[3];
    kvx_lower_comparison (mask, GET_CODE (cmp), left, right);
    DONE;
  }
)

(define_expand "vec_cmpu<mode><mask>"
  [(set (match_operand:<MASK> 0 "register_operand")
        (match_operator 1 "comparison_operator"
         [(match_operand:SIMDCMP 2 "register_operand")
          (match_operand:SIMDCMP 3 "reg_or_zero_operand")]))]
  ""
  {
    rtx mask = operands[0];
    rtx cmp = operands[1];
    rtx left = operands[2];
    rtx right = operands[3];
    kvx_lower_comparison (mask, GET_CODE (cmp), left, right);
    DONE;
  }
)

(define_expand "vcond<SIMDCMP:mode><SIMDCMP2:mode>"
  [(match_operand:SIMDCMP 0 "register_operand")
   (match_operand:SIMDCMP 1 "nonmemory_operand")
   (match_operand:SIMDCMP 2 "nonmemory_operand")
   (match_operator 3 "comparison_operator"
    [(match_operand:SIMDCMP2 4 "register_operand")
     (match_operand:SIMDCMP2 5 "reg_or_zero_operand")])]
  "(GET_MODE_NUNITS (<SIMDCMP:MODE>mode) == GET_MODE_NUNITS (<SIMDCMP2:MODE>mode))"
  {
    rtx target = operands[0];
    rtx select1 = operands[1];
    rtx select2 = operands[2];
    rtx cmp = operands[3];
    rtx left = operands[4];
    rtx right = operands[5];
    kvx_expand_conditional_move (target, select1, select2, cmp, left, right);
    DONE;
  }
)

(define_expand "vcondu<SIMDCMP:mode><SIMDCMP2:mode>"
  [(match_operand:SIMDCMP 0 "register_operand")
   (match_operand:SIMDCMP 1 "nonmemory_operand")
   (match_operand:SIMDCMP 2 "nonmemory_operand")
   (match_operator 3 "comparison_operator"
    [(match_operand:SIMDCMP2 4 "register_operand")
     (match_operand:SIMDCMP2 5 "reg_or_zero_operand")])]
  "(GET_MODE_NUNITS (<SIMDCMP:MODE>mode) == GET_MODE_NUNITS (<SIMDCMP2:MODE>mode))"
  {
    rtx target = operands[0];
    rtx select1 = operands[1];
    rtx select2 = operands[2];
    rtx cmp = operands[3];
    rtx left = operands[4];
    rtx right = operands[5];
    kvx_expand_conditional_move (target, select1, select2, cmp, left, right);
    DONE;
  }
)

(define_expand "vcond_mask_<mode><mask>"
  [(match_operand:SIMDCMP 0 "register_operand")
   (match_operand:SIMDCMP 1 "nonmemory_operand")
   (match_operand:SIMDCMP 2 "nonmemory_operand")
   (match_operand:<MASK> 3 "register_operand")]
  ""
  {
    rtx target = operands[0];
    rtx select1 = operands[1];
    rtx select2 = operands[2];
    rtx mask = operands[3];
    kvx_expand_masked_move (target, select1, select2, mask);
    DONE;
  }
)

(define_expand "vec_shr_<mode>"
  [(match_operand:SIMDALL 0 "register_operand" "")
   (match_operand:SIMDALL 1 "register_operand" "")
   (match_operand:SI 2 "sixbits_unsigned_operand" "")]
  ""
  {
    unsigned bitshift = INTVAL (operands[2]);
    unsigned bitwidth = GET_MODE_BITSIZE (<INNER>mode);
    unsigned eltshift = bitshift / bitwidth;
    gcc_assert (eltshift * bitwidth == bitshift);
    rtx filler = CONST0_RTX (<INNER>mode);
    emit_insn (gen_kvx_shift<lsvs> (operands[0], operands[1], GEN_INT (eltshift), filler));
    DONE;
  }
)


;; Vector Init Helpers

(define_insn "*initx2a"
  [(set (match_operand:SIMD32X2 0 "register_operand" "=r")
        (unspec:SIMD32X2 [(match_operand:<INNER> 1 "register_operand" "r")] UNSPEC_INITX2A))]
  ""
  "insf %0 = %1, 31, 0"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*initx2b"
  [(set (match_operand:SIMD32X2 0 "register_operand" "+r")
        (unspec:SIMD32X2 [(match_operand:<INNER> 1 "register_operand" "r")
                          (match_dup 0)] UNSPEC_INITX2B))]
  ""
  "insf %0 = %1, 63, 32"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*initx4a"
  [(set (match_operand:SIMD16X4 0 "register_operand" "=r")
        (unspec:SIMD16X4 [(match_operand:<INNER> 1 "register_operand" "r")] UNSPEC_INITX4A))]
  ""
  "insf %0 = %1, 15, 0"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*initx4b"
  [(set (match_operand:SIMD16X4 0 "register_operand" "+r")
        (unspec:SIMD16X4 [(match_operand:<INNER> 1 "register_operand" "r")
                          (match_dup 0)] UNSPEC_INITX4B))]
  ""
  "insf %0 = %1, 31, 16"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*initx4c"
  [(set (match_operand:SIMD16X4 0 "register_operand" "+r")
        (unspec:SIMD16X4 [(match_operand:<INNER> 1 "register_operand" "r")
                          (match_dup 0)] UNSPEC_INITX4C))]
  ""
  "insf %0 = %1, 47, 32"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*initx4d"
  [(set (match_operand:SIMD16X4 0 "register_operand" "+r")
        (unspec:SIMD16X4 [(match_operand:<INNER> 1 "register_operand" "r")
                          (match_dup 0)] UNSPEC_INITX4D))]
  ""
  "insf %0 = %1, 63, 48"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*initx8a"
  [(set (match_operand:SIMD8X8 0 "register_operand" "=r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")] UNSPEC_INITX8A))]
  ""
  "insf %0 = %1, 7, 0"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*initx8b"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8B))]
  ""
  "insf %0 = %1, 15, 8"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*initx8c"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8C))]
  ""
  "insf %0 = %1, 23, 16"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*initx8d"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8D))]
  ""
  "insf %0 = %1, 31, 24"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*initx8e"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8E))]
  ""
  "insf %0 = %1, 39, 32"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*initx8f"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8F))]
  ""
  "insf %0 = %1, 47, 40"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*initx8g"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8G))]
  ""
  "insf %0 = %1, 55, 48"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*initx8h"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8H))]
  ""
  "insf %0 = %1, 63, 56"
  [(set_attr "type" "alu_lite")]
)


;; Vector Insert/Extract Helpers

(define_insn "*insf"
  [(set (match_operand:ALL64 0 "register_operand" "+r")
        (unspec:ALL64 [(match_operand:<INNER> 1 "register_operand" "r")
                       (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                       (match_operand:SI 3 "sixbits_unsigned_operand" "i")
                       (match_dup 0)] UNSPEC_INSF))]
  ""
  "insf %0 = %1, %2+%3-1, %3"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*extfz"
  [(set (match_operand:<INNER> 0 "register_operand" "=r")
        (unspec:<INNER> [(match_operand:ALL64 1 "register_operand" "r")
                         (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                         (match_operand:SI 3 "sixbits_unsigned_operand" "i")] UNSPEC_EXTFZ))]
  ""
  "extfz %0 = %1, %2+%3-1, %3"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*slld"
  [(set (match_operand:<INNER> 0 "register_operand" "=r")
        (unspec:<INNER> [(match_operand:ALL64 1 "register_operand" "r")
                         (match_operand:SI 2 "sixbits_unsigned_operand" "i")] UNSPEC_SLLD))]
  ""
  "slld %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*srld"
  [(set (match_operand:<INNER> 0 "register_operand" "=r")
        (unspec:<INNER> [(match_operand:ALL64 1 "register_operand" "r")
                         (match_operand:SI 2 "sixbits_unsigned_operand" "i")] UNSPEC_SRLD))]
  ""
  "srld %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sbmm8"
  [(set (match_operand:ALL64 0 "register_operand" "=r")
        (unspec:ALL64 [(match_operand:<INNER> 1 "register_operand" "r")
                       (match_operand:DI 2 "register_operand" "r")] UNSPEC_SBMM8))]
  ""
  "sbmm8 %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)


;; S64I (V4HI/V2SI)

(define_insn "*compn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S64I 2 "register_operand" "r")
          (match_operand:S64I 3 "register_operand" "r")]))]
  ""
  "compn<suffix>.%1 %0 = %2, %3"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*select<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (if_then_else:S64I (match_operator 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                           (match_operand:S64I 1 "register_operand" "r")
                           (match_operand:S64I 4 "register_operand" "0")))]
  ""
  "cmove<suffix>.%2z %3? %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*select<suffix>_nez"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (if_then_else:S64I (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                               (match_operand:<MASK> 6 "const_zero_operand" ""))
                           (match_operand:S64I 1 "register_operand" "r")
                           (match_operand:S64I 4 "register_operand" "0")))]
  ""
  "cmove<suffix>.%2z %3? %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_select<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "0")
                      (match_operand:<MASK> 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_SELECTWP))]
  ""
  "cmove<suffix>%4 %3? %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "add<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (plus:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "add<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,        8,          8")]
)

(define_insn "ssadd<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (ss_plus:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                      (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "adds<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx2<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (plus:S64I (mult:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                              (const_int 2))
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx2<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx2<suffix>2"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (plus:S64I (ashift:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                                (const_int 1))
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx2<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx4<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (plus:S64I (mult:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                              (const_int 4))
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx4<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx4<suffix>2"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (plus:S64I (ashift:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                                (const_int 2))
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx4<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx8<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (plus:S64I (mult:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                              (const_int 8))
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx8<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx8<suffix>2"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (plus:S64I (ashift:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                                (const_int 3))
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx8<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx16<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (plus:S64I (mult:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                              (const_int 16))
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx16<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx16<suffix>2"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (plus:S64I (ashift:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                                (const_int 4))
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx16<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "sub<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r,v32,vx2")
                    (match_operand:S64I 2 "register_operand" "r,r,r")))]
  ""
  "sbf<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "sssub<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (ss_minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r,v32,vx2")
                       (match_operand:S64I 2 "register_operand" "r,r,r")))]
  ""
  "sbfs<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx2<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:S64I (match_operand:S64I 2 "register_operand" "r,r,r")
                               (const_int 2))))]
  ""
  "sbfx2<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx2<suffix>2"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:S64I (match_operand:S64I 2 "register_operand" "r,r,r")
                                 (const_int 1))))]
  ""
  "sbfx2<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx4<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:S64I (match_operand:S64I 2 "register_operand" "r,r,r")
                               (const_int 4))))]
  ""
  "sbfx4<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx4<suffix>2"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:S64I (match_operand:S64I 2 "register_operand" "r,r,r")
                                 (const_int 2))))]
  ""
  "sbfx4<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx8<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:S64I (match_operand:S64I 2 "register_operand" "r,r,r")
                               (const_int 8))))]
  ""
  "sbfx8<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx8<suffix>2"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:S64I (match_operand:S64I 2 "register_operand" "r,r,r")
                                 (const_int 3))))]
  ""
  "sbfx8<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx16<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:S64I (match_operand:S64I 2 "register_operand" "r,r,r")
                               (const_int 16))))]
  ""
  "sbfx16<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx16<suffix>2"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:S64I (match_operand:S64I 2 "register_operand" "r,r,r")
                                 (const_int 4))))]
  ""
  "sbfx16<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "mul<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (mult:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "mul<suffix> %0 = %1, %2"
  [(set_attr "type" "mau,mau_x,mau_x")
   (set_attr "length" "4,    8,    8")]
)

(define_insn "smin<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (smin:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "min<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "smax<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (smax:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "max<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "umin<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (umin:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "minu<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "umax<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r")
        (umax:S64I (match_operand:S64I 1 "register_operand" "r,r,r")
                   (match_operand:S64I 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "maxu<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "and<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r,r")
        (and:S64I (match_operand:S64I 1 "register_operand" "r,r,r,r")
                  (match_operand:S64I 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "andd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*nand<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r,r")
        (ior:S64I (not:S64I (match_operand:S64I 1 "register_operand" "r,r,r,r"))
                  (not:S64I (match_operand:S64I 2 "nonmemory_operand" "r,v10,v37,i"))))]
  ""
  "nandd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*andn<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r,r")
        (and:S64I (not:S64I (match_operand:S64I 1 "register_operand" "r,r,r,r"))
                  (match_operand:S64I 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "andnd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "ior<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r,r")
        (ior:S64I (match_operand:S64I 1 "register_operand" "r,r,r,r")
                  (match_operand:S64I 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "ord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*nior<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r,r")
        (and:S64I (not:S64I (match_operand:S64I 1 "register_operand" "r,r,r,r"))
                  (not:S64I (match_operand:S64I 2 "nonmemory_operand" "r,v10,v37,i"))))]
  ""
  "nord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*iorn<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r,r")
        (ior:S64I (not:S64I (match_operand:S64I 1 "register_operand" "r,r,r,r"))
                  (match_operand:S64I 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "ornd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "xor<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r,r")
        (xor:S64I (match_operand:S64I 1 "register_operand" "r,r,r,r")
                  (match_operand:S64I 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "xord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*nxor<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r,r")
        (not:S64I (xor:S64I (match_operand:S64I 1 "register_operand" "r,r,r,r")
                            (match_operand:S64I 2 "nonmemory_operand" "r,v10,v37,i"))))]
  ""
  "nxord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "madd<mode><mode>4"
  [(set (match_operand:S64I 0 "register_operand" "=r,r,r,r")
        (plus:S64I (mult:S64I (match_operand:S64I 1 "register_operand" "r,r,r,r")
                              (match_operand:S64I 2 "nonmemory_operand" "r,v10,v37,i"))
                   (match_operand:S64I 3 "register_operand" "0,0,0,0")))]
  ""
  "madd<suffix> %0 = %1, %2"
  [(set_attr "type" "mau_auxr,mau_auxr,mau_auxr_x,mau_auxr_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "msub<mode><mode>4"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (minus:S64I (match_operand:S64I 3 "register_operand" "0")
                    (mult:S64I (match_operand:S64I 1 "register_operand" "r")
                               (match_operand:S64I 2 "register_operand" "r"))))]
  ""
  "msbf<suffix> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "ashl<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r")
        (ashift:S64I (match_operand:S64I 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sll<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "ssashl<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r")
        (ss_ashift:S64I (match_operand:S64I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sls<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "ashr<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r")
        (ashiftrt:S64I (match_operand:S64I 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sra<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "lshr<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r")
        (lshiftrt:S64I (match_operand:S64I 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srl<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "kvx_srs<suffix>s"
  [(set (match_operand:S64I 0 "register_operand" "=r,r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRSWP))]
  ""
  "srs<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "avg<mode>3_floor"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")] UNSPEC_AVGWP))]
  ""
  "avg<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "uavg<mode>3_floor"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")] UNSPEC_AVGUWP))]
  ""
  "avgu<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "avg<mode>3_ceil"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")] UNSPEC_AVGRWP))]
  ""
  "avgr<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "uavg<mode>3_ceil"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")] UNSPEC_AVGRUWP))]
  ""
  "avgru<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "neg<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (neg:S64I (match_operand:S64I 1 "register_operand" "r")))]
  ""
  "neg<suffix> %0 = %1"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length" "8")]
)

(define_insn "abs<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (abs:S64I (match_operand:S64I 1 "register_operand" "r")))]
  ""
  "abs<suffix> %0 = %1"
  [(set_attr "type" "alu_lite_x")
   (set_attr "length" "8")]
)

(define_insn "clrsb<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (clrsb:S64I (match_operand:S64I 1 "register_operand" "r")))]
  ""
  "cls<suffix> %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "clz<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (clz:S64I (match_operand:S64I 1 "register_operand" "r")))]
  ""
  "clz<suffix> %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "ctz<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ctz:S64I (match_operand:S64I 1 "register_operand" "r")))]
  ""
  "ctz<suffix> %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "popcount<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (popcount:S64I (match_operand:S64I 1 "register_operand" "r")))]
  ""
  "cbs<suffix> %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "one_cmpl<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (not:S64I (match_operand:S64I 1 "register_operand" "r")))]
  ""
  "notd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_abd<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (abs:S64I (minus:S64I (match_operand:S64I 2 "nonmemory_operand" "r")
                              (match_operand:S64I 1 "register_operand" "r"))))]
  ""
  "abd<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_expand "kvx_abd<suffix>s"
  [(set (match_operand:S64I 0 "register_operand" "")
        (abs:S64I (minus:S64I (match_operand:S64I 2 "nonmemory_operand" "")
                              (match_operand:S64I 1 "register_operand" ""))))]
  ""
  {
    if (operands[2] == const0_rtx)
      {
        emit_insn (gen_abs<mode>2 (operands[0], operands[1]));
        DONE;
      }
    rtx chunk = gen_reg_rtx (<MODE>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_expand "kvx_adds<suffix>s"
  [(set (match_operand:S64I 0 "register_operand" "")
        (ss_plus:S64I (match_operand:S64I 1 "register_operand" "")
                      (match_operand:S64I 2 "nonmemory_operand" "")))]
  ""
  {
    rtx chunk = gen_reg_rtx (<MODE>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_expand "kvx_sbfs<suffix>s"
  [(set (match_operand:S64I 0 "register_operand" "")
        (ss_minus:S64I (match_operand:S64I 2 "nonmemory_operand" "")
                       (match_operand:S64I 1 "register_operand" "")))]
  ""
  {
    rtx chunk = gen_reg_rtx (<MODE>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_expand "kvx_min<suffix>s"
  [(set (match_operand:S64I 0 "register_operand" "")
        (smin:S64I (match_operand:S64I 1 "register_operand" "")
                   (match_operand:S64I 2 "nonmemory_operand" "")))]
  ""
  {
    rtx chunk = gen_reg_rtx (<MODE>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_expand "kvx_max<suffix>s"
  [(set (match_operand:S64I 0 "register_operand" "")
        (smax:S64I (match_operand:S64I 1 "register_operand" "")
                   (match_operand:S64I 2 "nonmemory_operand" "")))]
  ""
  {
    rtx chunk = gen_reg_rtx (<MODE>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_expand "kvx_minu<suffix>s"
  [(set (match_operand:S64I 0 "register_operand" "")
        (umin:S64I (match_operand:S64I 1 "register_operand" "")
                   (match_operand:S64I 2 "nonmemory_operand" "")))]
  ""
  {
    rtx chunk = gen_reg_rtx (<MODE>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_expand "kvx_maxu<suffix>s"
  [(set (match_operand:S64I 0 "register_operand" "")
        (umax:S64I (match_operand:S64I 1 "register_operand" "")
                   (match_operand:S64I 2 "nonmemory_operand" "")))]
  ""
  {
    rtx chunk = gen_reg_rtx (<MODE>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)


;; S64A

(define_expand "kvx_shift<lsvs>"
  [(match_operand:S64A 0 "register_operand" "")
   (match_operand:S64A 1 "register_operand" "")
   (match_operand:SI 2 "sixbits_unsigned_operand" "")
   (match_operand:<INNER> 3 "nonmemory_operand" "")]
  ""
  {
    int shift = INTVAL (operands[2]) * GET_MODE_BITSIZE (<INNER>mode);
    rtx chunk = NULL_RTX;
    if (operands[3] != CONST0_RTX (<INNER>mode))
      {
        chunk = gen_reg_rtx (<CHUNK>mode);
        kvx_expand_chunk_splat (chunk, operands[3], <INNER>mode);
      }
    kvx_expand_chunk_shift (operands[0], operands[1], chunk, shift);
    DONE;
  }
)


;; V4HI

(define_insn "*mulv4hiv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (mult:V4SI (sign_extend:V4SI (match_operand:V4HI 1 "register_operand" "r"))
                   (sign_extend:V4SI (match_operand:V4HI 2 "register_operand" "r"))))]
  ""
  "mulhwq %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "*umulv4hiv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (mult:V4SI (zero_extend:V4SI (match_operand:V4HI 1 "register_operand" "r"))
                   (zero_extend:V4SI (match_operand:V4HI 2 "register_operand" "r"))))]
  ""
  "muluhwq %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "maddv4hiv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (sign_extend:V4SI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                                                (match_operand:V4HI 2 "nonmemory_operand" "r")))
                   (match_operand:V4SI 3 "register_operand" "0")))]
  ""
  "maddhwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "umaddv4hiv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (zero_extend:V4SI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                                                (match_operand:V4HI 2 "nonmemory_operand" "r")))
                   (match_operand:V4SI 3 "register_operand" "0")))]
  ""
  "madduhwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "msubv4hiv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 3 "register_operand" "0")
                    (sign_extend:V4SI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                                                 (match_operand:V4HI 2 "register_operand" "r")))))]
  ""
  "msbfhwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "umsubv4hiv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 3 "register_operand" "0")
                    (zero_extend:V4SI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                                                 (match_operand:V4HI 2 "register_operand" "r")))))]
  ""
  "msbfuhwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)


;; V2SI

(define_insn "*mulv2siv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (mult:V2DI (sign_extend:V2DI (match_operand:V2SI 1 "register_operand" "r"))
                   (sign_extend:V2DI (match_operand:V2SI 2 "register_operand" "r"))))]
  ""
  "mulwdp %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "*umulv2siv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (mult:V2DI (zero_extend:V2DI (match_operand:V2SI 1 "register_operand" "r"))
                   (zero_extend:V2DI (match_operand:V2SI 2 "register_operand" "r"))))]
  ""
  "muluwdp %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "kvx_conswp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (vec_concat:V2SI (match_operand:SI 1 "register_operand" "0")
                         (match_operand:SI 2 "register_operand" "r")))]
  ""
  "insf %0 = %2, 63, 32"
  [(set_attr "type" "alu_lite")]
)


;; S128I (V8HI/V4SI)

(define_insn "*compn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S128I 2 "register_operand" "r")
          (match_operand:S128I 3 "register_operand" "r")]))]
  ""
  "compn<halfx>.%1 %x0 = %x2, %x3\n\tcompn<halfx>.%1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*select<suffix>"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (if_then_else:S128I (match_operator 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                            (match_operand:S128I 1 "register_operand" "r")
                            (match_operand:S128I 4 "register_operand" "0")))]
  ""
  "cmove<halfx>.%2z %x3? %x0 = %x1\n\tcmove<halfx>.%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*select<suffix>_nez"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (if_then_else:S128I (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                                (match_operand:<MASK> 6 "const_zero_operand" ""))
                            (match_operand:S128I 1 "register_operand" "r")
                            (match_operand:S128I 4 "register_operand" "0")))]
  ""
  "cmove<halfx>.%2z %x3? %x0 = %x1\n\tcmove<halfx>.%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_select<suffix>"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "0")
                       (match_operand:<MASK> 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECTWQ))]
  ""
  "cmove<halfx>%4 %x3? %x0 = %x1\n\tcmove<halfx>%4 %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "add<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (plus:S128I (match_operand:S128I 1 "register_operand" "r")
                    (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "add<halfx> %x0 = %x1, %x2\n\tadd<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "ssadd<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (ss_plus:S128I (match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "adds<halfx> %x0 = %x1, %x2\n\tadds<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx2<mode>"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (plus:S128I (mult:S128I (match_operand:S128I 1 "register_operand" "r")
                                (const_int 2))
                    (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "addx2<halfx> %x0 = %x1, %x2\n\taddx2<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx2<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (plus:S128I (ashift:S128I (match_operand:S128I 1 "register_operand" "r")
                                  (const_int 1))
                    (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "addx2<halfx> %x0 = %x1, %x2\n\taddx2<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4<mode>"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (plus:S128I (mult:S128I (match_operand:S128I 1 "register_operand" "r")
                                (const_int 4))
                    (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "addx4<halfx> %x0 = %x1, %x2\n\taddx4<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (plus:S128I (ashift:S128I (match_operand:S128I 1 "register_operand" "r")
                                  (const_int 2))
                    (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "addx4<halfx> %x0 = %x1, %x2\n\taddx4<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8<mode>"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (plus:S128I (mult:S128I (match_operand:S128I 1 "register_operand" "r")
                                (const_int 8))
                    (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "addx8<halfx> %x0 = %x1, %x2\n\taddx8<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (plus:S128I (ashift:S128I (match_operand:S128I 1 "register_operand" "r")
                                  (const_int 3))
                    (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "addx8<halfx> %x0 = %x1, %x2\n\taddx8<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16<mode>"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (plus:S128I (mult:S128I (match_operand:S128I 1 "register_operand" "r")
                                (const_int 16))
                    (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "addx16<halfx> %x0 = %x1, %x2\n\taddx16<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (plus:S128I (ashift:S128I (match_operand:S128I 1 "register_operand" "r")
                                  (const_int 4))
                    (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "addx16<halfx> %x0 = %x1, %x2\n\taddx16<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "sub<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (minus:S128I (match_operand:S128I 1 "nonmemory_operand" "r")
                     (match_operand:S128I 2 "register_operand" "r")))]
  ""
  "sbf<halfx> %x0 = %x2, %x1\n\tsbf<halfx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "sssub<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (ss_minus:S128I (match_operand:S128I 1 "nonmemory_operand" "r")
                        (match_operand:S128I 2 "register_operand" "r")))]
  ""
  "sbfs<halfx> %x0 = %x2, %x1\n\tsbfs<halfx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx2<mode>"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (minus:S128I (match_operand:S128I 1 "nonmemory_operand" "r")
                     (mult:S128I (match_operand:S128I 2 "register_operand" "r")
                                 (const_int 2))))]
  ""
  "sbfx2<halfx> %x0 = %x2, %x1\n\tsbfx2<halfx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx2<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (minus:S128I (match_operand:S128I 1 "nonmemory_operand" "r")
                     (ashift:S128I (match_operand:S128I 2 "register_operand" "r")
                                   (const_int 1))))]
  ""
  "sbfx2<halfx> %x0 = %x2, %x1\n\tsbfx2<halfx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4<mode>"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (minus:S128I (match_operand:S128I 1 "nonmemory_operand" "r")
                     (mult:S128I (match_operand:S128I 2 "register_operand" "r")
                                 (const_int 4))))]
  ""
  "sbfx4<halfx> %x0 = %x2, %x1\n\tsbfx4<halfx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (minus:S128I (match_operand:S128I 1 "nonmemory_operand" "r")
                     (ashift:S128I (match_operand:S128I 2 "register_operand" "r")
                                   (const_int 2))))]
  ""
  "sbfx4<halfx> %x0 = %x2, %x1\n\tsbfx4<halfx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8<mode>"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (minus:S128I (match_operand:S128I 1 "nonmemory_operand" "r")
                     (mult:S128I (match_operand:S128I 2 "register_operand" "r")
                                 (const_int 8))))]
  ""
  "sbfx8<halfx> %x0 = %x2, %x1\n\tsbfx8<halfx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (minus:S128I (match_operand:S128I 1 "nonmemory_operand" "r")
                     (ashift:S128I (match_operand:S128I 2 "register_operand" "r")
                                   (const_int 3))))]
  ""
  "sbfx8<halfx> %x0 = %x2, %x1\n\tsbfx8<halfx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16<mode>"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (minus:S128I (match_operand:S128I 1 "nonmemory_operand" "r")
                     (mult:S128I (match_operand:S128I 2 "register_operand" "r")
                                 (const_int 16))))]
  ""
  "sbfx16<halfx> %x0 = %x2, %x1\n\tsbfx16<halfx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (minus:S128I (match_operand:S128I 1 "nonmemory_operand" "r")
                     (ashift:S128I (match_operand:S128I 2 "register_operand" "r")
                                   (const_int 4))))]
  ""
  "sbfx16<halfx> %x0 = %x2, %x1\n\tsbfx16<halfx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "smin<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (smin:S128I (match_operand:S128I 1 "register_operand" "r")
                    (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "min<halfx> %x0 = %x1, %x2\n\tmin<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "smax<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (smax:S128I (match_operand:S128I 1 "register_operand" "r")
                    (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "max<halfx> %x0 = %x1, %x2\n\tmax<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "umin<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (umin:S128I (match_operand:S128I 1 "register_operand" "r")
                    (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "minu<halfx> %x0 = %x1, %x2\n\tminu<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "umax<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (umax:S128I (match_operand:S128I 1 "register_operand" "r")
                    (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "maxu<halfx> %x0 = %x1, %x2\n\tmaxu<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "and<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (and:S128I (match_operand:S128I 1 "register_operand" "r")
                   (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*nand<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (ior:S128I (not:S128I (match_operand:S128I 1 "register_operand" "r"))
                   (not:S128I (match_operand:S128I 2 "nonmemory_operand" "r"))))]
  ""
  "nandd %x0 = %x1, %x2\n\tnandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*andn<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (and:S128I (not:S128I (match_operand:S128I 1 "register_operand" "r"))
                   (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "andnd %x0 = %x1, %x2\n\tandnd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "ior<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (ior:S128I (match_operand:S128I 1 "register_operand" "r")
                   (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*nior<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (and:S128I (not:S128I (match_operand:S128I 1 "register_operand" "r"))
                   (not:S128I (match_operand:S128I 2 "nonmemory_operand" "r"))))]
  ""
  "nord %x0 = %x1, %x2\n\tnord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*iorn<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (ior:S128I (not:S128I (match_operand:S128I 1 "register_operand" "r"))
                   (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "ornd %x0 = %x1, %x2\n\tornd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "xor<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (xor:S128I (match_operand:S128I 1 "register_operand" "r")
                   (match_operand:S128I 2 "nonmemory_operand" "r")))]
  ""
  "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*nxor<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (not:S128I (xor:S128I (match_operand:S128I 1 "register_operand" "r")
                              (match_operand:S128I 2 "nonmemory_operand" "r"))))]
  ""
  "nxord %x0 = %x1, %x2\n\tnxord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "madd<mode><mode>4"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (plus:S128I (mult:S128I (match_operand:S128I 1 "register_operand" "r")
                                (match_operand:S128I 2 "nonmemory_operand" "r"))
                    (match_operand:S128I 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (plus:<HALF> (mult:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                  (subreg:<HALF> (match_dup 2) 0))
                     (subreg:<HALF> (match_dup 3) 0)))
   (set (subreg:<HALF> (match_dup 0) 8)
        (plus:<HALF> (mult:<HALF> (subreg:<HALF> (match_dup 1) 8)
                                  (subreg:<HALF> (match_dup 2) 8))
                     (subreg:<HALF> (match_dup 3) 8)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "msub<mode><mode>4"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (minus:S128I (match_operand:S128I 3 "register_operand" "0")
                    (mult:S128I (match_operand:S128I 1 "register_operand" "r")
                               (match_operand:S128I 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (subreg:<HALF> (match_dup 3) 0)
                      (mult:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                   (subreg:<HALF> (match_dup 2) 0))))
   (set (subreg:<HALF> (match_dup 0) 8)
        (minus:<HALF> (subreg:<HALF> (match_dup 3) 8)
                      (mult:<HALF> (subreg:<HALF> (match_dup 1) 8)
                                   (subreg:<HALF> (match_dup 2) 8))))]
  ""
  [(set_attr "type" "mau_auxr")
   (set_attr "length"      "8")]
)

(define_insn "ashl<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (ashift:S128I (match_operand:S128I 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sll<halfx>s %x0 = %x1, %2\n\tsll<halfx>s %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "ssashl<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (ss_ashift:S128I (match_operand:S128I 1 "register_operand" "r,r")
                         (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sls<halfx>s %x0 = %x1, %2\n\tsls<halfx>s %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "ashr<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (ashiftrt:S128I (match_operand:S128I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sra<halfx>s %x0 = %x1, %2\n\tsra<halfx>s %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "lshr<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (lshiftrt:S128I (match_operand:S128I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srl<halfx>s %x0 = %x1, %2\n\tsrl<halfx>s %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "kvx_srs<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRSWQ))]
  ""
  "srs<halfx>s %x0 = %x1, %2\n\tsrs<halfx>s %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "avg<mode>3_floor"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "r")] UNSPEC_AVGWQ))]
  ""
  "avg<halfx> %x0 = %x1, %x2\n\tavg<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "uavg<mode>3_floor"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "r")] UNSPEC_AVGUWQ))]
  ""
  "avgu<halfx> %x0 = %x1, %x2\n\tavgu<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "avg<mode>3_ceil"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "r")] UNSPEC_AVGRWQ))]
  ""
  "avgr<halfx> %x0 = %x1, %x2\n\tavgr<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "uavg<mode>3_ceil"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "r")] UNSPEC_AVGRUWQ))]
  ""
  "avgru<halfx> %x0 = %x1, %x2\n\tavgru<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "neg<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (neg:S128I (match_operand:S128I 1 "register_operand" "r")))]
  ""
  "neg<halfx> %x0 = %x1\n\tneg<halfx> %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2_x")
   (set_attr "length"        "16")]
)

(define_insn "abs<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (abs:S128I (match_operand:S128I 1 "register_operand" "r")))]
  ""
  "abs<halfx> %x0 = %x1\n\tabs<halfx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2_x")
   (set_attr "length"          "16")]
)

(define_insn "clrsb<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (clrsb:S128I (match_operand:S128I 1 "register_operand" "r")))]
  ""
  "cls<halfx> %x0 = %x1\n\tcls<halfx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "clz<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (clz:S128I (match_operand:S128I 1 "register_operand" "r")))]
  ""
  "clz<halfx> %x0 = %x1\n\tclz<halfx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "ctz<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (ctz:S128I (match_operand:S128I 1 "register_operand" "r")))]
  ""
  "ctz<halfx> %x0 = %x1\n\tctz<halfx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "popcount<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (popcount:S128I (match_operand:S128I 1 "register_operand" "r")))]
  ""
  "cbs<halfx> %x0 = %x1\n\tcbs<halfx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "one_cmpl<mode>2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (not:S128I (match_operand:S128I 1 "register_operand" "r")))]
  ""
  "notd %x0 = %x1\n\tnotd %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_abd<suffix>"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (abs:S128I (minus:S128I (match_operand:S128I 2 "nonmemory_operand" "r")
                                (match_operand:S128I 1 "register_operand" "r"))))]
  ""
  "abd<halfx> %x0 = %x1, %x2\n\tabd<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_abd<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "")
        (abs:S128I (minus:S128I (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V128)
                                (match_operand:S128I 1 "register_operand" ""))))]
  ""
  {
    if (operands[2] == const0_rtx)
      {
        emit_insn (gen_abs<mode>2 (operands[0], operands[1]));
        DONE;
      }
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn "*abd<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (abs:S128I (minus:S128I (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V128)
                                (match_operand:S128I 1 "register_operand" "r"))))]
  ""
  "abd<halfx> %x0 = %x1, %2\n\tabd<halfx> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_adds<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "")
        (ss_plus:S128I (match_operand:S128I 1 "register_operand" "")
                       (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V128)))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn "*adds<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (ss_plus:S128I (match_operand:S128I 1 "register_operand" "r")
                       (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V128)))]
  ""
  "adds<halfx> %x0 = %x1, %2\n\tadds<halfx> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_sbfs<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "")
        (ss_minus:S128I (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V128)
                        (match_operand:S128I 1 "register_operand" "")))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn "*sbfs<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (ss_minus:S128I (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V128)
                        (match_operand:S128I 1 "register_operand" "r")))]
  ""
  "sbfs<halfx> %x0 = %x1, %2\n\tsbfs<halfx> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_min<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "")
        (smin:S128I (match_operand:S128I 1 "register_operand" "")
                    (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V128)))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn "*min<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (smin:S128I (match_operand:S128I 1 "register_operand" "r")
                    (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V128)))]
  ""
  "min<halfx> %x0 = %x1, %2\n\tmin<halfx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_max<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "")
        (smax:S128I (match_operand:S128I 1 "register_operand" "")
                    (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V128)))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn "*max<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (smax:S128I (match_operand:S128I 1 "register_operand" "r")
                    (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V128)))]
  ""
  "max<halfx> %x0 = %x1, %2\n\tmax<halfx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_minu<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "")
        (umin:S128I (match_operand:S128I 1 "register_operand" "")
                    (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V128)))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn "*minu<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (umin:S128I (match_operand:S128I 1 "register_operand" "r")
                    (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V128)))]
  ""
  "minu<halfx> %x0 = %x1, %2\n\tminu<halfx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_maxu<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "")
        (umax:S128I (match_operand:S128I 1 "register_operand" "")
                    (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V128)))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn "*maxu<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (umax:S128I (match_operand:S128I 1 "register_operand" "r")
                    (unspec:S128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V128)))]
  ""
  "maxu<halfx> %x0 = %x1, %2\n\tmaxu<halfx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)


;; S128A

(define_expand "kvx_shift<lsvs>"
  [(match_operand:S128A 0 "register_operand" "")
   (match_operand:S128A 1 "register_operand" "")
   (match_operand:SI 2 "sixbits_unsigned_operand" "")
   (match_operand:<INNER> 3 "nonmemory_operand" "")]
  ""
  {
    int shift = INTVAL (operands[2]) * GET_MODE_BITSIZE (<INNER>mode);
    rtx opnd0_0 = gen_rtx_SUBREG (<CHUNK>mode, operands[0], 0);
    rtx opnd0_1 = gen_rtx_SUBREG (<CHUNK>mode, operands[0], 8);
    rtx opnd1_0 = gen_rtx_SUBREG (<CHUNK>mode, operands[1], 0);
    rtx opnd1_1 = gen_rtx_SUBREG (<CHUNK>mode, operands[1], 8);
    rtx filler = CONST0_RTX (<CHUNK>mode);
    rtx chunk = NULL_RTX;
    if (operands[3] != CONST0_RTX (<INNER>mode))
      {
        chunk = gen_reg_rtx (<CHUNK>mode);
        kvx_expand_chunk_splat (chunk, operands[3], <INNER>mode);
        filler = chunk;
      }
    if (shift < 64)
      {
        kvx_expand_chunk_shift (opnd0_0, opnd1_0, opnd1_1, shift);
        kvx_expand_chunk_shift (opnd0_1, opnd1_1, chunk, shift);
      }
    else if (shift < 128)
      {
        kvx_expand_chunk_shift (opnd0_0, opnd1_1, chunk, shift - 64);
        emit_move_insn (opnd0_1, filler);
      }
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "kvx_cons<lsvs>"
  [(set (match_operand:S128A 0 "register_operand" "=r")
        (vec_concat:S128A (match_operand:<HALF> 1 "register_operand" "0")
                          (match_operand:<HALF> 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 8)
        (match_dup 2))]
  ""
)


;; V8HI

(define_insn_and_split "mulv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4HI (match_dup 0) 0)
        (mult:V4HI (subreg:V4HI (match_dup 1) 0)
                   (subreg:V4HI (match_dup 2) 0)))
   (set (subreg:V4HI (match_dup 0) 8)
        (mult:V4HI (subreg:V4HI (match_dup 1) 8)
                   (subreg:V4HI (match_dup 2) 8)))]
  ""
  [(set_attr "type" "mau_auxr")]
)


;; V4SI

(define_insn "mulv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "mulwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)


;; V2DI

(define_insn_and_split "*compndp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "comparison_operator"
         [(match_operand:V2DI 2 "register_operand" "r")
          (match_operand:V2DI 3 "register_operand" "r")]))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (unspec:V2DI [(match_op_dup 1 [(match_dup 2)
                                       (match_dup 3)])] UNSPEC_COMP128))
   (set (match_dup 0)
        (neg:V2DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_tiny_x2")]
)

(define_insn "*compdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operator:V2DI 1 "comparison_operator"
                       [(match_operand:V2DI 2 "register_operand" "r")
                        (match_operand:V2DI 3 "register_operand" "r")])] UNSPEC_COMP128))]
  ""
  "compd.%1 %x0 = %x2, %x3\n\tcompd.%1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*selectdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (if_then_else:V2DI (match_operator 2 "zero_comparison_operator"
                                             [(match_operand:V2DI 3 "register_operand" "r")
                                              (match_operand:V2DI 5 "const_zero_operand" "")])
                           (match_operand:V2DI 1 "register_operand" "r")
                           (match_operand:V2DI 4 "register_operand" "0")))]
  ""
  "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*selectdp_nez"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (if_then_else:V2DI (ne (match_operator:V2DI 2 "zero_comparison_operator"
                                             [(match_operand:V2DI 3 "register_operand" "r")
                                              (match_operand:V2DI 5 "const_zero_operand" "")])
                               (match_operand:V2DI 6 "const_zero_operand" ""))
                           (match_operand:V2DI 1 "register_operand" "r")
                           (match_operand:V2DI 4 "register_operand" "0")))]
  ""
  "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_selectdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2DI 1 "register_operand" "r")
                      (match_operand:V2DI 2 "register_operand" "0")
                      (match_operand:V2DI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_SELECTDP))]
  ""
  "cmoved%4 %x3? %x0 = %x1\n\tcmoved%4 %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "addv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (plus:V2DI (match_operand:V2DI 1 "register_operand" "r")
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "addd %x0 = %x1, %x2\n\taddd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "ssaddv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (ss_plus:V2DI (match_operand:V2DI 1 "register_operand" "r")
                      (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "addsd %x0 = %x1, %x2\n\taddsd %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx2dp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (plus:V2DI (mult:V2DI (match_operand:V2DI 1 "register_operand" "r")
                              (const_int 2))
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "addx2d %x0 = %x1, %x2\n\taddx2d %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx2dp2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (plus:V2DI (ashift:V2DI (match_operand:V2DI 1 "register_operand" "r")
                                (const_int 1))
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "addx2d %x0 = %x1, %x2\n\taddx2d %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4dp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (plus:V2DI (mult:V2DI (match_operand:V2DI 1 "register_operand" "r")
                              (const_int 4))
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "addx4d %x0 = %x1, %x2\n\taddx4d %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4dp2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (plus:V2DI (ashift:V2DI (match_operand:V2DI 1 "register_operand" "r")
                                (const_int 2))
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "addx4d %x0 = %x1, %x2\n\taddx4d %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8dp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (plus:V2DI (mult:V2DI (match_operand:V2DI 1 "register_operand" "r")
                              (const_int 8))
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "addx8d %x0 = %x1, %x2\n\taddx8d %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8dp2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (plus:V2DI (ashift:V2DI (match_operand:V2DI 1 "register_operand" "r")
                                (const_int 3))
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "addx8d %x0 = %x1, %x2\n\taddx8d %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16dp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (plus:V2DI (mult:V2DI (match_operand:V2DI 1 "register_operand" "r")
                              (const_int 16))
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "addx16d %x0 = %x1, %x2\n\taddx16d %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16dp2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (plus:V2DI (ashift:V2DI (match_operand:V2DI 1 "register_operand" "r")
                                (const_int 4))
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "addx16d %x0 = %x1, %x2\n\taddx16d %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "subv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (minus:V2DI (match_operand:V2DI 1 "nonmemory_operand" "r")
                    (match_operand:V2DI 2 "register_operand" "r")))]
  ""
  "sbfd %x0 = %x2, %x1\n\tsbfd %y0 = %y2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "sssubv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (ss_minus:V2DI (match_operand:V2DI 1 "nonmemory_operand" "r")
                       (match_operand:V2DI 2 "register_operand" "r")))]
  ""
  "sbfsd %x0 = %x2, %x1\n\tsbfsd %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx2dp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (minus:V2DI (match_operand:V2DI 1 "nonmemory_operand" "r")
                    (mult:V2DI (match_operand:V2DI 2 "register_operand" "r")
                               (const_int 2))))]
  ""
  "sbfx2d %x0 = %x2, %x1\n\tsbfx2d %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx2dp2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (minus:V2DI (match_operand:V2DI 1 "nonmemory_operand" "r")
                    (ashift:V2DI (match_operand:V2DI 2 "register_operand" "r")
                                 (const_int 1))))]
  ""
  "sbfx2d %x0 = %x2, %x1\n\tsbfx2d %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4dp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (minus:V2DI (match_operand:V2DI 1 "nonmemory_operand" "r")
                    (mult:V2DI (match_operand:V2DI 2 "register_operand" "r")
                               (const_int 4))))]
  ""
  "sbfx4d %x0 = %x2, %x1\n\tsbfx4d %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4dp2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (minus:V2DI (match_operand:V2DI 1 "nonmemory_operand" "r")
                    (ashift:V2DI (match_operand:V2DI 2 "register_operand" "r")
                                 (const_int 2))))]
  ""
  "sbfx4d %x0 = %x2, %x1\n\tsbfx4d %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8dp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (minus:V2DI (match_operand:V2DI 1 "nonmemory_operand" "r")
                    (mult:V2DI (match_operand:V2DI 2 "register_operand" "r")
                               (const_int 8))))]
  ""
  "sbfx8d %x0 = %x2, %x1\n\tsbfx8d %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8dp2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (minus:V2DI (match_operand:V2DI 1 "nonmemory_operand" "r")
                    (ashift:V2DI (match_operand:V2DI 2 "register_operand" "r")
                                 (const_int 3))))]
  ""
  "sbfx8d %x0 = %x2, %x1\n\tsbfx8d %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16dp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (minus:V2DI (match_operand:V2DI 1 "nonmemory_operand" "r")
                    (mult:V2DI (match_operand:V2DI 2 "register_operand" "r")
                               (const_int 16))))]
  ""
  "sbfx16d %x0 = %x2, %x1\n\tsbfx16d %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16dp2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (minus:V2DI (match_operand:V2DI 1 "nonmemory_operand" "r")
                    (ashift:V2DI (match_operand:V2DI 2 "register_operand" "r")
                                 (const_int 4))))]
  ""
  "sbfx16d %x0 = %x2, %x1\n\tsbfx16d %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "mulv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (mult:V2DI (match_operand:V2DI 1 "register_operand" "r")
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (mult:DI (subreg:DI (match_dup 1) 0)
                 (subreg:DI (match_dup 2) 0)))
   (set (subreg:DI (match_dup 0) 8)
        (mult:DI (subreg:DI (match_dup 1) 8)
                 (subreg:DI (match_dup 2) 8)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn "sminv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (smin:V2DI (match_operand:V2DI 1 "register_operand" "r")
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "mind %x0 = %x1, %x2\n\tmind %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "smaxv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (smax:V2DI (match_operand:V2DI 1 "register_operand" "r")
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "maxd %x0 = %x1, %x2\n\tmaxd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "uminv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (umin:V2DI (match_operand:V2DI 1 "register_operand" "r")
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "minud %x0 = %x1, %x2\n\tminud %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "umaxv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (umax:V2DI (match_operand:V2DI 1 "register_operand" "r")
                   (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "maxud %x0 = %x1, %x2\n\tmaxud %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "andv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (and:V2DI (match_operand:V2DI 1 "register_operand" "r")
                  (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*nandv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (ior:V2DI (not:V2DI (match_operand:V2DI 1 "register_operand" "r"))
                  (not:V2DI (match_operand:V2DI 2 "nonmemory_operand" "r"))))]
  ""
  "nandd %x0 = %x1, %x2\n\tnandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*andnv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (and:V2DI (not:V2DI (match_operand:V2DI 1 "register_operand" "r"))
                  (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "andnd %x0 = %x1, %x2\n\tandnd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "iorv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (ior:V2DI (match_operand:V2DI 1 "register_operand" "r")
                  (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*niorv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (and:V2DI (not:V2DI (match_operand:V2DI 1 "register_operand" "r"))
                  (not:V2DI (match_operand:V2DI 2 "nonmemory_operand" "r"))))]
  ""
  "nord %x0 = %x1, %x2\n\tnord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*iornv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (ior:V2DI (not:V2DI (match_operand:V2DI 1 "register_operand" "r"))
                  (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "ornd %x0 = %x1, %x2\n\tornd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "xorv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (xor:V2DI (match_operand:V2DI 1 "register_operand" "r")
                  (match_operand:V2DI 2 "nonmemory_operand" "r")))]
  ""
  "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*nxorv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (not:V2DI (xor:V2DI (match_operand:V2DI 1 "register_operand" "r")
                            (match_operand:V2DI 2 "nonmemory_operand" "r"))))]
  ""
  "nxord %x0 = %x1, %x2\n\tnxord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "maddv2div2di4"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (plus:V2DI (mult:V2DI (match_operand:V2DI 1 "register_operand" "r")
                              (match_operand:V2DI 2 "nonmemory_operand" "r"))
                   (match_operand:V2DI 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (plus:DI (mult:DI (subreg:DI (match_dup 1) 0)
                          (subreg:DI (match_dup 2) 0))
                 (subreg:DI (match_dup 3) 0)))
   (set (subreg:DI (match_dup 0) 8)
        (plus:DI (mult:DI (subreg:DI (match_dup 1) 8)
                          (subreg:DI (match_dup 2) 8))
                 (subreg:DI (match_dup 3) 8)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "msubv2div2di4"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (minus:V2DI (match_operand:V2DI 3 "register_operand" "0")
                    (mult:V2DI (match_operand:V2DI 1 "register_operand" "r")
                               (match_operand:V2DI 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (minus:DI (subreg:DI (match_dup 3) 0)
                  (mult:DI (subreg:DI (match_dup 1) 0)
                           (subreg:DI (match_dup 2) 0))))
   (set (subreg:DI (match_dup 0) 8)
        (minus:DI (subreg:DI (match_dup 3) 8)
                  (mult:DI (subreg:DI (match_dup 1) 8)
                           (subreg:DI (match_dup 2) 8))))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn "ashlv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r,r")
        (ashift:V2DI (match_operand:V2DI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "slld %x0 = %x1, %2\n\tslld %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2,alu_tiny_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "ssashlv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r,r")
        (ss_ashift:V2DI (match_operand:V2DI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "slsd %x0 = %x1, %2\n\tslsd %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "ashrv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r,r")
        (ashiftrt:V2DI (match_operand:V2DI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srad %x0 = %x1, %2\n\tsrad %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2,alu_tiny_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "lshrv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r,r")
        (lshiftrt:V2DI (match_operand:V2DI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srld %x0 = %x1, %2\n\tsrld %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2,alu_tiny_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "kvx_srsdps"
  [(set (match_operand:V2DI 0 "register_operand" "=r,r")
        (unspec:V2DI [(match_operand:V2DI 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRSDP))]
  ""
  "srsd %x0 = %x1, %2\n\tsrsd %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "negv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (neg:V2DI (match_operand:V2DI 1 "register_operand" "r")))]
  ""
  "negd %x0 = %x1\n\tnegd %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2_x")
   (set_attr "length"        "16")]
)

(define_insn "absv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (abs:V2DI (match_operand:V2DI 1 "register_operand" "r")))]
  ""
  "absd %x0 = %x1\n\tabsd %y0 = %y1"
  [(set_attr "type" "alu_lite_x2_x")
   (set_attr "length"          "16")]
)

(define_insn "clrsbv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (clrsb:V2DI (match_operand:V2DI 1 "register_operand" "r")))]
  ""
  "clsd %x0 = %x1\n\tclsd %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "clzv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (clz:V2DI (match_operand:V2DI 1 "register_operand" "r")))]
  ""
  "clzd %x0 = %x1\n\tclzd %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "ctzv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (ctz:V2DI (match_operand:V2DI 1 "register_operand" "r")))]
  ""
  "ctzd %x0 = %x1\n\tctzd %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "popcountv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (popcount:V2DI (match_operand:V2DI 1 "register_operand" "r")))]
  ""
  "cbsd %x0 = %x1\n\tcbsd %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "one_cmplv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (not:V2DI (match_operand:V2DI 1 "register_operand" "r")))]
  ""
  "notd %x0 = %x1\n\tnotd %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_abddp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (abs:V2DI (minus:V2DI (match_operand:V2DI 2 "nonmemory_operand" "r")
                              (match_operand:V2DI 1 "register_operand" "r"))))]
  ""
  "abdd %x0 = %x1, %x2\n\tabdd %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_abddps"
  [(set (match_operand:V2DI 0 "register_operand" "")
        (abs:V2DI (minus:V2DI (unspec:V2DI [(match_operand:DI 2 "nonmemory_operand" "")] UNSPEC_V128)
                              (match_operand:V2DI 1 "register_operand" ""))))]
  ""
  {
    if (operands[2] == const0_rtx)
      {
        emit_insn (gen_absv2di2 (operands[0], operands[1]));
        DONE;
      }
  }
)

(define_insn "*abddps"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (abs:V2DI (minus:V2DI (unspec:V2DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V128)
                              (match_operand:V2DI 1 "register_operand" "r"))))]
  ""
  "abdd %x0 = %x1, %2\n\tabdd %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_addsdps"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (ss_plus:V2DI (match_operand:V2DI 1 "register_operand" "r")
                      (unspec:V2DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V128)))]
  ""
  "addsd %x0 = %x1, %2\n\taddsd %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_sbfsdps"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (ss_minus:V2DI (unspec:V2DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V128)
                       (match_operand:V2DI 1 "register_operand" "r")))]
  ""
  "sbfsd %x0 = %x1, %2\n\tsbfsd %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_mindps"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (smin:V2DI (match_operand:V2DI 1 "register_operand" "r")
                   (unspec:V2DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V128)))]
  ""
  "mind %x0 = %x1, %2\n\tmind %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_maxdps"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (smax:V2DI (match_operand:V2DI 1 "register_operand" "r")
                   (unspec:V2DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V128)))]
  ""
  "maxd %x0 = %x1, %2\n\tmaxd %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_minudps"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (umin:V2DI (match_operand:V2DI 1 "register_operand" "r")
                   (unspec:V2DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V128)))]
  ""
  "minud %x0 = %x1, %2\n\tminud %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_maxudps"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (umax:V2DI (match_operand:V2DI 1 "register_operand" "r")
                   (unspec:V2DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V128)))]
  ""
  "maxud %x0 = %x1, %2\n\tmaxud %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)


;; S256I (V16HI/V8SI)

(define_insn_and_split "kvx_select<suffix>"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "0")
                       (match_operand:S256I 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECTWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)
                        (subreg:<HALF> (match_dup 3) 0)
                        (match_dup 4)] UNSPEC_SELECTWQ))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)
                        (subreg:<HALF> (match_dup 3) 16)
                        (match_dup 4)] UNSPEC_SELECTWQ))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "add<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (plus:S256I (match_operand:S256I 1 "register_operand" "r")
                    (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  {
    return "add<quartx> %x0 = %x1, %x2\n\tadd<quartx> %y0 = %y1, %y2\n\t"
           "add<quartx> %z0 = %z1, %z2\n\tadd<quartx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "ssadd<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (ss_plus:S256I (match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_plus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_plus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx2<mode>"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (plus:S256I (mult:S256I (match_operand:S256I 1 "register_operand" "r")
                                (const_int 2))
                    (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (plus:<HALF> (mult:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                  (const_int 2))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (plus:<HALF> (mult:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                  (const_int 2))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx2<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (plus:S256I (ashift:S256I (match_operand:S256I 1 "register_operand" "r")
                                  (const_int 1))
                    (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                    (const_int 1))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                    (const_int 1))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx4<mode>"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (plus:S256I (mult:S256I (match_operand:S256I 1 "register_operand" "r")
                                (const_int 4))
                    (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (plus:<HALF> (mult:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                  (const_int 4))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (plus:<HALF> (mult:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                  (const_int 4))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx4<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (plus:S256I (ashift:S256I (match_operand:S256I 1 "register_operand" "r")
                                  (const_int 2))
                    (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                    (const_int 2))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                    (const_int 2))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx8<mode>"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (plus:S256I (mult:S256I (match_operand:S256I 1 "register_operand" "r")
                                (const_int 8))
                    (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (plus:<HALF> (mult:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                  (const_int 8))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (plus:<HALF> (mult:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                  (const_int 8))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx8<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (plus:S256I (ashift:S256I (match_operand:S256I 1 "register_operand" "r")
                                  (const_int 3))
                    (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                    (const_int 3))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                    (const_int 3))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx16<mode>"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (plus:S256I (mult:S256I (match_operand:S256I 1 "register_operand" "r")
                                (const_int 16))
                    (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (plus:<HALF> (mult:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                  (const_int 16))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (plus:<HALF> (mult:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                  (const_int 16))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx16<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (plus:S256I (ashift:S256I (match_operand:S256I 1 "register_operand" "r")
                                  (const_int 4))
                    (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                    (const_int 4))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                    (const_int 4))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "sub<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (minus:S256I (match_operand:S256I 1 "nonmemory_operand" "r")
                     (match_operand:S256I 2 "register_operand" "r")))]
  ""
  {
    return "sbf<quartx> %x0 = %x2, %x1\n\tsbf<quartx> %y0 = %y2, %y1\n\t"
           "sbf<quartx> %z0 = %z2, %z1\n\tsbf<quartx> %t0 = %t2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "sssub<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (ss_minus:S256I (match_operand:S256I 1 "nonmemory_operand" "r")
                        (match_operand:S256I 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                         (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                         (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx2<mode>"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (minus:S256I (match_operand:S256I 1 "nonmemory_operand" "r")
                     (mult:S256I (match_operand:S256I 2 "register_operand" "r")
                                 (const_int 2))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                      (mult:<HALF> (subreg:<HALF> (match_dup 2) 0)
                                   (const_int 2))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                      (mult:<HALF> (subreg:<HALF> (match_dup 2) 16)
                                   (const_int 2))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx2<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (minus:S256I (match_operand:S256I 1 "nonmemory_operand" "r")
                     (ashift:S256I (match_operand:S256I 2 "register_operand" "r")
                                   (const_int 1))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 0)
                                     (const_int 1))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 16)
                                     (const_int 1))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx4<mode>"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (minus:S256I (match_operand:S256I 1 "nonmemory_operand" "r")
                     (mult:S256I (match_operand:S256I 2 "register_operand" "r")
                                 (const_int 4))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                      (mult:<HALF> (subreg:<HALF> (match_dup 2) 0)
                                   (const_int 4))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                      (mult:<HALF> (subreg:<HALF> (match_dup 2) 16)
                                   (const_int 4))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx4<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (minus:S256I (match_operand:S256I 1 "nonmemory_operand" "r")
                     (ashift:S256I (match_operand:S256I 2 "register_operand" "r")
                                   (const_int 2))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 0)
                                     (const_int 2))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 16)
                                     (const_int 2))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx8<mode>"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (minus:S256I (match_operand:S256I 1 "nonmemory_operand" "r")
                     (mult:S256I (match_operand:S256I 2 "register_operand" "r")
                                 (const_int 8))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                      (mult:<HALF> (subreg:<HALF> (match_dup 2) 0)
                                   (const_int 8))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                      (mult:<HALF> (subreg:<HALF> (match_dup 2) 16)
                                   (const_int 8))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx8<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (minus:S256I (match_operand:S256I 1 "nonmemory_operand" "r")
                     (ashift:S256I (match_operand:S256I 2 "register_operand" "r")
                                   (const_int 3))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 0)
                                     (const_int 3))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 16)
                                     (const_int 3))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx16<mode>"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (minus:S256I (match_operand:S256I 1 "nonmemory_operand" "r")
                     (mult:S256I (match_operand:S256I 2 "register_operand" "r")
                                 (const_int 16))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                      (mult:<HALF> (subreg:<HALF> (match_dup 2) 0)
                                   (const_int 16))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                      (mult:<HALF> (subreg:<HALF> (match_dup 2) 16)
                                   (const_int 16))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx16<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (minus:S256I (match_operand:S256I 1 "nonmemory_operand" "r")
                     (ashift:S256I (match_operand:S256I 2 "register_operand" "r")
                                   (const_int 4))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 0)
                                     (const_int 4))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 16)
                                     (const_int 4))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "smin<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (smin:S256I (match_operand:S256I 1 "register_operand" "r")
                    (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  {
    return "min<quartx> %x0 = %x1, %x2\n\tmin<quartx> %y0 = %y1, %y2\n\t"
           "min<quartx> %z0 = %z1, %z2\n\tmin<quartx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "smax<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (smax:S256I (match_operand:S256I 1 "register_operand" "r")
                    (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  {
    return "max<quartx> %x0 = %x1, %x2\n\tmax<quartx> %y0 = %y1, %y2\n\t"
           "max<quartx> %z0 = %z1, %z2\n\tmax<quartx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "umin<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (umin:S256I (match_operand:S256I 1 "register_operand" "r")
                    (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  {
    return "minu<quartx> %x0 = %x1, %x2\n\tminu<quartx> %y0 = %y1, %y2\n\t"
           "minu<quartx> %z0 = %z1, %z2\n\tminu<quartx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "umax<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (umax:S256I (match_operand:S256I 1 "register_operand" "r")
                    (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  {
    return "maxu<quartx> %x0 = %x1, %x2\n\tmaxu<quartx> %y0 = %y1, %y2\n\t"
           "maxu<quartx> %z0 = %z1, %z2\n\tmaxu<quartx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "and<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (and:S256I (match_operand:S256I 1 "register_operand" "r")
                   (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  {
    return "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2\n\t"
           "andd %z0 = %z1, %z2\n\tandd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*nand<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (ior:S256I (not:S256I (match_operand:S256I 1 "register_operand" "r"))
                   (not:S256I (match_operand:S256I 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nandd %x0 = %x1, %x2\n\tnandd %y0 = %y1, %y2\n\t"
           "nandd %z0 = %z1, %z2\n\tnandd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*andn<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (and:S256I (not:S256I (match_operand:S256I 1 "register_operand" "r"))
                   (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  {
    return "andnd %x0 = %x1, %x2\n\tandnd %y0 = %y1, %y2\n\t"
           "andnd %z0 = %z1, %z2\n\tandnd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "ior<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (ior:S256I (match_operand:S256I 1 "register_operand" "r")
                   (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  {
    return "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2\n\t"
           "ord %z0 = %z1, %z2\n\tord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*nior<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (and:S256I (not:S256I (match_operand:S256I 1 "register_operand" "r"))
                   (not:S256I (match_operand:S256I 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nord %x0 = %x1, %x2\n\tnord %y0 = %y1, %y2\n\t"
           "nord %z0 = %z1, %z2\n\tnord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*iorn<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (ior:S256I (not:S256I (match_operand:S256I 1 "register_operand" "r"))
                   (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  {
    return "ornd %x0 = %x1, %x2\n\tornd %y0 = %y1, %y2\n\t"
           "ornd %z0 = %z1, %z2\n\tornd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "xor<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (xor:S256I (match_operand:S256I 1 "register_operand" "r")
                   (match_operand:S256I 2 "nonmemory_operand" "r")))]
  ""
  {
    return "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2\n\t"
           "xord %z0 = %z1, %z2\n\txord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*nxor<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (not:S256I (xor:S256I (match_operand:S256I 1 "register_operand" "r")
                              (match_operand:S256I 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nxord %x0 = %x1, %x2\n\tnxord %y0 = %y1, %y2\n\t"
           "nxord %z0 = %z1, %z2\n\tnxord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "madd<mode><mode>4"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (plus:S256I (mult:S256I (match_operand:S256I 1 "register_operand" "r")
                                (match_operand:S256I 2 "nonmemory_operand" "r"))
                    (match_operand:S256I 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QUART> (match_dup 0) 0)
        (plus:<QUART> (mult:<QUART> (subreg:<QUART> (match_dup 1) 0)
                                    (subreg:<QUART> (match_dup 2) 0))
                      (subreg:<QUART> (match_dup 3) 0)))
   (set (subreg:<QUART> (match_dup 0) 8)
        (plus:<QUART> (mult:<QUART> (subreg:<QUART> (match_dup 1) 8)
                                    (subreg:<QUART> (match_dup 2) 8))
                      (subreg:<QUART> (match_dup 3) 8)))
   (set (subreg:<QUART> (match_dup 0) 16)
        (plus:<QUART> (mult:<QUART> (subreg:<QUART> (match_dup 1) 16)
                                    (subreg:<QUART> (match_dup 2) 16))
                      (subreg:<QUART> (match_dup 3) 16)))
   (set (subreg:<QUART> (match_dup 0) 24)
        (plus:<QUART> (mult:<QUART> (subreg:<QUART> (match_dup 1) 24)
                                    (subreg:<QUART> (match_dup 2) 24))
                      (subreg:<QUART> (match_dup 3) 24)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "msub<mode><mode>4"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (minus:S256I (match_operand:S256I 3 "register_operand" "0")
                     (mult:S256I (match_operand:S256I 1 "register_operand" "r")
                                 (match_operand:S256I 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QUART> (match_dup 0) 0)
        (minus:<QUART> (subreg:<QUART> (match_dup 3) 0)
                       (mult:<QUART> (subreg:<QUART> (match_dup 1) 0)
                                     (subreg:<QUART> (match_dup 2) 0))))
   (set (subreg:<QUART> (match_dup 0) 8)
        (minus:<QUART> (subreg:<QUART> (match_dup 3) 8)
                       (mult:<QUART> (subreg:<QUART> (match_dup 1) 8)
                                     (subreg:<QUART> (match_dup 2) 8))))
   (set (subreg:<QUART> (match_dup 0) 16)
        (minus:<QUART> (subreg:<QUART> (match_dup 3) 16)
                       (mult:<QUART> (subreg:<QUART> (match_dup 1) 16)
                                     (subreg:<QUART> (match_dup 2) 16))))
   (set (subreg:<QUART> (match_dup 0) 24)
        (minus:<QUART> (subreg:<QUART> (match_dup 3) 24)
                       (mult:<QUART> (subreg:<QUART> (match_dup 1) 24)
                                     (subreg:<QUART> (match_dup 2) 24))))]
  ""
  [(set_attr "type" "mau_auxr")
   (set_attr "length"      "8")]
)

(define_insn_and_split "ashl<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r,r")
        (ashift:S256I (match_operand:S256I 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ashift:<HALF> (subreg:<HALF> (match_dup 1) 0)
                       (match_dup 2)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ashift:<HALF> (subreg:<HALF> (match_dup 1) 16)
                       (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == REG)
      {
        emit_move_insn (operands[3], operands[2]);
        operands[2] = operands[3];
      }
  }
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn_and_split "ssashl<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r,r")
        (ss_ashift:S256I (match_operand:S256I 1 "register_operand" "r,r")
                         (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_ashift:<HALF> (subreg:<HALF> (match_dup 1) 0)
                          (match_dup 2)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_ashift:<HALF> (subreg:<HALF> (match_dup 1) 16)
                          (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == REG)
      {
        emit_move_insn (operands[3], operands[2]);
        operands[2] = operands[3];
      }
  }
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn_and_split "ashr<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r,r")
        (ashiftrt:S256I (match_operand:S256I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ashiftrt:<HALF> (subreg:<HALF> (match_dup 1) 0)
                         (match_dup 2)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ashiftrt:<HALF> (subreg:<HALF> (match_dup 1) 16)
                         (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == REG)
      {
        emit_move_insn (operands[3], operands[2]);
        operands[2] = operands[3];
      }
  }
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn_and_split "lshr<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r,r")
        (lshiftrt:S256I (match_operand:S256I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (lshiftrt:<HALF> (subreg:<HALF> (match_dup 1) 0)
                         (match_dup 2)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (lshiftrt:<HALF> (subreg:<HALF> (match_dup 1) 16)
                         (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == REG)
      {
        emit_move_insn (operands[3], operands[2]);
        operands[2] = operands[3];
      }
  }
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn_and_split "kvx_srs<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "=r,r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRSWO))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (match_dup 2)] UNSPEC_SRSWQ))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (match_dup 2)] UNSPEC_SRSWQ))]
  {
    if (GET_CODE (operands[2]) == REG)
      {
        emit_move_insn (operands[3], operands[2]);
        operands[2] = operands[3];
      }
  }
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn_and_split "avg<mode>3_floor"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "r")] UNSPEC_AVGWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)] UNSPEC_AVGWQ))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)] UNSPEC_AVGWQ))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "uavg<mode>3_floor"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "r")] UNSPEC_AVGUWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)] UNSPEC_AVGUWQ))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)] UNSPEC_AVGUWQ))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "avg<mode>3_ceil"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "r")] UNSPEC_AVGRWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)] UNSPEC_AVGRWQ))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)] UNSPEC_AVGRWQ))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "uavg<mode>3_ceil"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "r")] UNSPEC_AVGRUWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)] UNSPEC_AVGRUWQ))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)] UNSPEC_AVGRUWQ))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "neg<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (neg:S256I (match_operand:S256I 1 "register_operand" "r")))]
  ""
  {
    return "neg<quartx> %x0 = %x1\n\tneg<quartx> %y0 = %y1\n\t"
           "neg<quartx> %z0 = %z1\n\tneg<quartx> %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4_x")
   (set_attr "length"          "32")]
)

(define_insn_and_split "abs<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (abs:S256I (match_operand:S256I 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (abs:<HALF> (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (abs:<HALF> (subreg:<HALF> (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2_x")]
)

(define_insn_and_split "clrsb<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (clrsb:S256I (match_operand:S256I 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (clrsb:<HALF> (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (clrsb:<HALF> (subreg:<HALF> (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "clz<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (clz:S256I (match_operand:S256I 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (clz:<HALF> (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (clz:<HALF> (subreg:<HALF> (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "ctz<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (ctz:S256I (match_operand:S256I 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ctz:<HALF> (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ctz:<HALF> (subreg:<HALF> (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "popcount<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (popcount:S256I (match_operand:S256I 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (popcount:<HALF> (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (popcount:<HALF> (subreg:<HALF> (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "one_cmpl<mode>2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (not:S256I (match_operand:S256I 1 "register_operand" "r")))]
  ""
  {
    return "notd %x0 = %x1\n\tnotd %y0 = %y1\n\t"
           "notd %z0 = %z1\n\tnotd %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "kvx_abd<suffix>"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (abs:S256I (minus:S256I (match_operand:S256I 2 "nonmemory_operand" "r")
                                (match_operand:S256I 1 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (abs:<HALF> (minus:<HALF> (subreg:<HALF> (match_dup 2) 0)
                                  (subreg:<HALF> (match_dup 1) 0))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (abs:<HALF> (minus:<HALF> (subreg:<HALF> (match_dup 2) 16)
                                  (subreg:<HALF> (match_dup 1) 16))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_expand "kvx_abd<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "")
        (abs:S256I (minus:S256I (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V256)
                                (match_operand:S256I 1 "register_operand" ""))))]
  ""
  {
    if (operands[2] == const0_rtx)
      {
        emit_insn (gen_abs<mode>2 (operands[0], operands[1]));
        DONE;
      }
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn_and_split "*abd<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (abs:S256I (minus:S256I (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V256)
                                (match_operand:S256I 1 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (abs:<HALF> (minus:<HALF> (unspec:<HALF> [(match_dup 2)] UNSPEC_V128)
                                  (subreg:<HALF> (match_dup 1) 0))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (abs:<HALF> (minus:<HALF> (unspec:<HALF> [(match_dup 2)] UNSPEC_V128)
                                  (subreg:<HALF> (match_dup 1) 16))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_expand "kvx_adds<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "")
        (ss_plus:S256I (match_operand:S256I 1 "register_operand" "")
                       (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V256)))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn_and_split "*adds<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (ss_plus:S256I (match_operand:S256I 1 "register_operand" "r")
                       (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V256)))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_plus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                        (unspec:<HALF> [(match_dup 2)] UNSPEC_V128)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_plus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                        (unspec:<HALF> [(match_dup 2)] UNSPEC_V128)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_expand "kvx_sbfs<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "")
        (ss_minus:S256I (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V256)
                        (match_operand:S256I 1 "register_operand" "")))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn_and_split "*sbfs<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (ss_minus:S256I (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V256)
                        (match_operand:S256I 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_minus:<HALF> (unspec:<HALF> [(match_dup 2)] UNSPEC_V128)
                         (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_minus:<HALF> (unspec:<HALF> [(match_dup 2)] UNSPEC_V128)
                         (subreg:<HALF> (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_expand "kvx_min<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "")
        (smin:S256I (match_operand:S256I 1 "register_operand" "")
                    (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V256)))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn "*min<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (smin:S256I (match_operand:S256I 1 "register_operand" "r")
                    (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V256)))]
  ""
  {
    return "min<quartx> %x0 = %x1, %2\n\tmin<quartx> %y0 = %y1, %2\n\t"
           "min<quartx> %z0 = %z1, %2\n\tmin<quartx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "kvx_max<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "")
        (smax:S256I (match_operand:S256I 1 "register_operand" "")
                    (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V256)))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn "*max<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (smax:S256I (match_operand:S256I 1 "register_operand" "r")
                    (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V256)))]
  ""
  {
    return "max<quartx> %x0 = %x1, %2\n\tmax<quartx> %y0 = %y1, %2\n\t"
           "max<quartx> %z0 = %z1, %2\n\tmax<quartx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "kvx_minu<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "")
        (umin:S256I (match_operand:S256I 1 "register_operand" "")
                    (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V256)))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn "*minu<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (umin:S256I (match_operand:S256I 1 "register_operand" "r")
                    (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V256)))]
  ""
  {
    return "minu<quartx> %x0 = %x1, %2\n\tminu<quartx> %y0 = %y1, %2\n\t"
           "minu<quartx> %z0 = %z1, %2\n\tminu<quartx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "kvx_maxu<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "")
        (umax:S256I (match_operand:S256I 1 "register_operand" "")
                    (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "")] UNSPEC_V256)))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn "*maxu<suffix>s"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (umax:S256I (match_operand:S256I 1 "register_operand" "r")
                    (unspec:S256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_V256)))]
  ""
  {
    return "maxu<quartx> %x0 = %x1, %2\n\tmaxu<quartx> %y0 = %y1, %2\n\t"
           "maxu<quartx> %z0 = %z1, %2\n\tmaxu<quartx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; S256A

(define_expand "kvx_shift<lsvs>"
  [(match_operand:S256A 0 "register_operand" "")
   (match_operand:S256A 1 "register_operand" "")
   (match_operand:SI 2 "register_operand" "")
   (match_operand:<INNER> 3 "nonmemory_operand" "")]
  ""
  {
    int shift = INTVAL (operands[2]) * GET_MODE_BITSIZE (<INNER>mode);
    rtx opnd0_0 = gen_rtx_SUBREG (<CHUNK>mode, operands[0], 0);
    rtx opnd0_1 = gen_rtx_SUBREG (<CHUNK>mode, operands[0], 8);
    rtx opnd0_2 = gen_rtx_SUBREG (<CHUNK>mode, operands[0], 16);
    rtx opnd0_3 = gen_rtx_SUBREG (<CHUNK>mode, operands[0], 24);
    rtx opnd1_0 = gen_rtx_SUBREG (<CHUNK>mode, operands[1], 0);
    rtx opnd1_1 = gen_rtx_SUBREG (<CHUNK>mode, operands[1], 8);
    rtx opnd1_2 = gen_rtx_SUBREG (<CHUNK>mode, operands[1], 16);
    rtx opnd1_3 = gen_rtx_SUBREG (<CHUNK>mode, operands[1], 24);
    rtx filler = CONST0_RTX (<CHUNK>mode);
    rtx chunk = NULL_RTX;
    if (operands[3] != CONST0_RTX (<INNER>mode))
      {
        chunk = gen_reg_rtx (<CHUNK>mode);
        kvx_expand_chunk_splat (chunk, operands[3], <INNER>mode);
        filler = chunk;
      }
    if (shift < 64)
      {
        kvx_expand_chunk_shift (opnd0_0, opnd1_0, opnd1_1, shift);
        kvx_expand_chunk_shift (opnd0_1, opnd1_1, opnd1_2, shift);
        kvx_expand_chunk_shift (opnd0_2, opnd1_2, opnd1_3, shift);
        kvx_expand_chunk_shift (opnd0_3, opnd1_3, chunk, shift);
      }
    else if (shift < 128)
      {
        kvx_expand_chunk_shift (opnd0_0, opnd1_1, opnd1_2, shift - 64);
        kvx_expand_chunk_shift (opnd0_1, opnd1_2, opnd1_3, shift - 64);
        kvx_expand_chunk_shift (opnd0_2, opnd1_3, chunk, shift - 64);
        emit_move_insn (opnd0_3, filler);
      }
    else if (shift < 192)
      {
        kvx_expand_chunk_shift (opnd0_0, opnd1_2, opnd1_3, shift - 128);
        kvx_expand_chunk_shift (opnd0_1, opnd1_3, chunk, shift - 128);
        emit_move_insn (opnd0_2, filler);
        emit_move_insn (opnd0_3, filler);
      }
    else if (shift < 256)
      {
        kvx_expand_chunk_shift (opnd0_0, opnd1_3, chunk, shift - 192);
        emit_move_insn (opnd0_1, filler);
        emit_move_insn (opnd0_2, filler);
        emit_move_insn (opnd0_3, filler);
      }
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "kvx_cons<lsvs>"
  [(set (match_operand:S256A 0 "register_operand" "=r")
        (vec_concat:S256A (match_operand:<HALF> 1 "register_operand" "0")
                          (match_operand:<HALF> 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 16)
        (match_dup 2))]
  ""
)


;; V16HI

(define_insn "*compnhx"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (match_operator:V16HI 1 "comparison_operator"
         [(match_operand:V16HI 2 "register_operand" "r")
          (match_operand:V16HI 3 "register_operand" "r")]))]
  ""
  {
    return "compnhq.%1 %x0 = %x2, %x3\n\tcompnhq.%1 %y0 = %y2, %y3\n\t"
           "compnhq.%1 %z0 = %z2, %z3\n\tcompnhq.%1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*selecthx"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (if_then_else:V16HI (match_operator 2 "zero_comparison_operator"
                                               [(match_operand:V16HI 3 "register_operand" "r")
                                                (match_operand:V16HI 5 "const_zero_operand" "")])
                            (match_operand:V16HI 1 "register_operand" "r")
                            (match_operand:V16HI 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (if_then_else:V8HI (match_op_dup 2 [(subreg:V8HI (match_dup 3) 0)
                                            (const_vector:V8HI [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V8HI (match_dup 1) 0)
                           (subreg:V8HI (match_dup 4) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (if_then_else:V8HI (match_op_dup 2 [(subreg:V8HI (match_dup 3) 16)
                                            (const_vector:V8HI [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V8HI (match_dup 1) 16)
                           (subreg:V8HI (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*selecthx_nez"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (if_then_else:V16HI (ne (match_operator:V16HI 2 "zero_comparison_operator"
                                               [(match_operand:V16HI 3 "register_operand" "r")
                                                (match_operand:V16HI 5 "const_zero_operand" "")])
                                (match_operand:V16HI 6 "const_zero_operand" ""))
                            (match_operand:V16HI 1 "register_operand" "r")
                            (match_operand:V16HI 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (if_then_else:V8HI (match_op_dup 2 [(subreg:V8HI (match_dup 3) 0)
                                            (const_vector:V8HI [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V8HI (match_dup 1) 0)
                           (subreg:V8HI (match_dup 4) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (if_then_else:V8HI (match_op_dup 2 [(subreg:V8HI (match_dup 3) 16)
                                            (const_vector:V8HI [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V8HI (match_dup 1) 16)
                           (subreg:V8HI (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "mulv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (mult:V16HI (match_operand:V16HI 1 "register_operand" "r")
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4HI (match_dup 0) 0)
        (mult:V4HI (subreg:V4HI (match_dup 1) 0)
                   (subreg:V4HI (match_dup 2) 0)))
   (set (subreg:V4HI (match_dup 0) 8)
        (mult:V4HI (subreg:V4HI (match_dup 1) 8)
                   (subreg:V4HI (match_dup 2) 8)))
   (set (subreg:V4HI (match_dup 0) 16)
        (mult:V4HI (subreg:V4HI (match_dup 1) 16)
                   (subreg:V4HI (match_dup 2) 16)))
   (set (subreg:V4HI (match_dup 0) 24)
        (mult:V4HI (subreg:V4HI (match_dup 1) 24)
                   (subreg:V4HI (match_dup 2) 24)))]
  ""
  [(set_attr "type" "mau_auxr")]
)


;; V8SI

(define_insn "*compnwo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (match_operator:V8SI 1 "comparison_operator"
         [(match_operand:V8SI 2 "register_operand" "r")
          (match_operand:V8SI 3 "register_operand" "r")]))]
  ""
  {
      return "compnwp.%1 %x0 = %x2, %x3\n\tcompnwp.%1 %y0 = %y2, %y3\n\t"
             "compnwp.%1 %z0 = %z2, %z3\n\tcompnwp.%1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*selectwo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (if_then_else:V8SI (match_operator 2 "zero_comparison_operator"
                                             [(match_operand:V8SI 3 "register_operand" "r")
                                              (match_operand:V8SI 5 "const_zero_operand" "")])
                           (match_operand:V8SI 1 "register_operand" "r")
                           (match_operand:V8SI 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (if_then_else:V4SI (match_op_dup 2 [(subreg:V4SI (match_dup 3) 0)
                                            (const_vector:V4SI [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V4SI (match_dup 1) 0)
                           (subreg:V4SI (match_dup 4) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (if_then_else:V4SI (match_op_dup 2 [(subreg:V4SI (match_dup 3) 16)
                                            (const_vector:V4SI [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V4SI (match_dup 1) 16)
                           (subreg:V4SI (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)


(define_insn_and_split "*selectwo_nez"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (if_then_else:V8SI (ne (match_operator:V8SI 2 "zero_comparison_operator"
                                             [(match_operand:V8SI 3 "register_operand" "r")
                                              (match_operand:V8SI 5 "const_zero_operand" "")])
                               (match_operand:V8SI 6 "const_zero_operand" ""))
                           (match_operand:V8SI 1 "register_operand" "r")
                           (match_operand:V8SI 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (if_then_else:V4SI (match_op_dup 2 [(subreg:V4SI (match_dup 3) 0)
                                            (const_vector:V4SI [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V4SI (match_dup 1) 0)
                           (subreg:V4SI (match_dup 4) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (if_then_else:V4SI (match_op_dup 2 [(subreg:V4SI (match_dup 3) 16)
                                            (const_vector:V4SI [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V4SI (match_dup 1) 16)
                           (subreg:V4SI (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "mulv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (mult:V8SI (match_operand:V8SI 1 "register_operand" "r")
                    (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (mult:V4SI (subreg:V4SI (match_dup 1) 0)
                   (subreg:V4SI (match_dup 2) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (mult:V4SI (subreg:V4SI (match_dup 1) 16)
                   (subreg:V4SI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "mau_auxr")]
)


;; V4DI

(define_insn_and_split "*compndq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (match_operator:V4DI 1 "comparison_operator"
         [(match_operand:V4DI 2 "register_operand" "r")
          (match_operand:V4DI 3 "register_operand" "r")]))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (unspec:V4DI [(match_op_dup 1 [(match_dup 2)
                                       (match_dup 3)])] UNSPEC_COMP256))
   (set (match_dup 0)
        (neg:V4DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_tiny_x4")]
)

(define_insn "*compdq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operator:V4DI 1 "comparison_operator"
                       [(match_operand:V4DI 2 "register_operand" "r")
                        (match_operand:V4DI 3 "register_operand" "r")])] UNSPEC_COMP256))]
  ""
  {
    return "compd.%1 %x0 = %x2, %x3\n\tcompd.%1 %y0 = %y2, %y3\n\t"
           "compd.%1 %z0 = %z2, %z3\n\tcompd.%1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*selectdq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (if_then_else:V4DI (match_operator 2 "zero_comparison_operator"
                                             [(match_operand:V4DI 3 "register_operand" "r")
                                              (match_operand:V4DI 5 "const_zero_operand" "")])
                           (match_operand:V4DI 1 "register_operand" "r")
                           (match_operand:V4DI 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (if_then_else:V2DI (match_op_dup 2 [(subreg:V2DI (match_dup 3) 0)
                                            (const_vector:V2DI [
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V2DI (match_dup 1) 0)
                           (subreg:V2DI (match_dup 4) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (if_then_else:V2DI (match_op_dup 2 [(subreg:V2DI (match_dup 3) 16)
                                            (const_vector:V2DI [
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V2DI (match_dup 1) 16)
                           (subreg:V2DI (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*selectdq_nez"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (if_then_else:V4DI (ne (match_operator:V4DI 2 "zero_comparison_operator"
                                             [(match_operand:V4DI 3 "register_operand" "r")
                                              (match_operand:V4DI 5 "const_zero_operand" "")])
                               (match_operand:V4DI 6 "const_zero_operand" ""))
                           (match_operand:V4DI 1 "register_operand" "r")
                           (match_operand:V4DI 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (if_then_else:V2DI (match_op_dup 2 [(subreg:V2DI (match_dup 3) 0)
                                            (const_vector:V2DI [
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V2DI (match_dup 1) 0)
                           (subreg:V2DI (match_dup 4) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (if_then_else:V2DI (match_op_dup 2 [(subreg:V2DI (match_dup 3) 16)
                                            (const_vector:V2DI [
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V2DI (match_dup 1) 16)
                           (subreg:V2DI (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "kvx_selectdq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operand:V4DI 1 "register_operand" "r")
                      (match_operand:V4DI 2 "register_operand" "0")
                      (match_operand:V4DI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_SELECTDQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 0)
                      (subreg:V2DI (match_dup 2) 0)
                      (subreg:V2DI (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_SELECTDP))
   (set (subreg:V2DI (match_dup 0) 16)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 16)
                      (subreg:V2DI (match_dup 2) 16)
                      (subreg:V2DI (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_SELECTDP))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "addv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (plus:V4DI (match_operand:V4DI 1 "register_operand" "r")
                    (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "addd %x0 = %x1, %x2\n\taddd %y0 = %y1, %y2\n\t"
           "addd %z0 = %z1, %z2\n\taddd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "ssaddv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (ss_plus:V4DI (match_operand:V4DI 1 "register_operand" "r")
                       (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (ss_plus:V2DI (subreg:V2DI (match_dup 1) 0)
                      (subreg:V2DI (match_dup 2) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (ss_plus:V2DI (subreg:V2DI (match_dup 1) 16)
                      (subreg:V2DI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx2v4di"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (plus:V4DI (mult:V4DI (match_operand:V4DI 1 "register_operand" "r")
                              (const_int 2))
                    (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (plus:V2DI (mult:V2DI (subreg:V2DI (match_dup 1) 0)
                              (const_int 2))
                   (subreg:V2DI (match_dup 2) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (plus:V2DI (mult:V2DI (subreg:V2DI (match_dup 1) 16)
                              (const_int 2))
                   (subreg:V2DI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx2v4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (plus:V4DI (ashift:V4DI (match_operand:V4DI 1 "register_operand" "r")
                                (const_int 1))
                    (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (plus:V2DI (ashift:V2DI (subreg:V2DI (match_dup 1) 0)
                                (const_int 1))
                   (subreg:V2DI (match_dup 2) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (plus:V2DI (ashift:V2DI (subreg:V2DI (match_dup 1) 16)
                                (const_int 1))
                   (subreg:V2DI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx4v4di"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (plus:V4DI (mult:V4DI (match_operand:V4DI 1 "register_operand" "r")
                              (const_int 4))
                    (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (plus:V2DI (mult:V2DI (subreg:V2DI (match_dup 1) 0)
                              (const_int 4))
                   (subreg:V2DI (match_dup 2) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (plus:V2DI (mult:V2DI (subreg:V2DI (match_dup 1) 16)
                              (const_int 4))
                   (subreg:V2DI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx4v4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (plus:V4DI (ashift:V4DI (match_operand:V4DI 1 "register_operand" "r")
                                (const_int 2))
                    (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (plus:V2DI (ashift:V2DI (subreg:V2DI (match_dup 1) 0)
                                (const_int 2))
                   (subreg:V2DI (match_dup 2) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (plus:V2DI (ashift:V2DI (subreg:V2DI (match_dup 1) 16)
                                (const_int 2))
                   (subreg:V2DI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx8v4di"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (plus:V4DI (mult:V4DI (match_operand:V4DI 1 "register_operand" "r")
                              (const_int 8))
                   (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (plus:V2DI (mult:V2DI (subreg:V2DI (match_dup 1) 0)
                              (const_int 8))
                   (subreg:V2DI (match_dup 2) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (plus:V2DI (mult:V2DI (subreg:V2DI (match_dup 1) 16)
                              (const_int 8))
                   (subreg:V2DI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx8v4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (plus:V4DI (ashift:V4DI (match_operand:V4DI 1 "register_operand" "r")
                                (const_int 3))
                    (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (plus:V2DI (ashift:V2DI (subreg:V2DI (match_dup 1) 0)
                                (const_int 3))
                   (subreg:V2DI (match_dup 2) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (plus:V2DI (ashift:V2DI (subreg:V2DI (match_dup 1) 16)
                                (const_int 3))
                   (subreg:V2DI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx16v4di"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (plus:V4DI (mult:V4DI (match_operand:V4DI 1 "register_operand" "r")
                              (const_int 16))
                   (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (plus:V2DI (mult:V2DI (subreg:V2DI (match_dup 1) 0)
                              (const_int 16))
                   (subreg:V2DI (match_dup 2) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (plus:V2DI (mult:V2DI (subreg:V2DI (match_dup 1) 16)
                              (const_int 16))
                   (subreg:V2DI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx16v4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (plus:V4DI (ashift:V4DI (match_operand:V4DI 1 "register_operand" "r")
                                (const_int 4))
                   (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (plus:V2DI (ashift:V2DI (subreg:V2DI (match_dup 1) 0)
                                (const_int 4))
                   (subreg:V2DI (match_dup 2) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (plus:V2DI (ashift:V2DI (subreg:V2DI (match_dup 1) 16)
                                (const_int 4))
                   (subreg:V2DI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "subv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (minus:V4DI (match_operand:V4DI 1 "nonmemory_operand" "r")
                    (match_operand:V4DI 2 "register_operand" "r")))]
  ""
  {
    return "sbfd %x0 = %x2, %x1\n\tsbfd %y0 = %y2, %y1\n\t"
           "sbfd %z0 = %z2, %z1\n\tsbfd %t0 = %t2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "sssubv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (ss_minus:V4DI (match_operand:V4DI 1 "nonmemory_operand" "r")
                       (match_operand:V4DI 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (ss_minus:V2DI (subreg:V2DI (match_dup 1) 0)
                       (subreg:V2DI (match_dup 2) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (ss_minus:V2DI (subreg:V2DI (match_dup 1) 16)
                       (subreg:V2DI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx2v4di"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (minus:V4DI (match_operand:V4DI 1 "nonmemory_operand" "r")
                     (mult:V4DI (match_operand:V4DI 2 "register_operand" "r")
                                (const_int 2))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (minus:V2DI (subreg:V2DI (match_dup 1) 0)
                    (mult:V2DI (subreg:V2DI (match_dup 2) 0)
                               (const_int 2))))
   (set (subreg:V2DI (match_dup 0) 16)
        (minus:V2DI (subreg:V2DI (match_dup 1) 16)
                    (mult:V2DI (subreg:V2DI (match_dup 2) 16)
                               (const_int 2))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx2v4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (minus:V4DI (match_operand:V4DI 1 "nonmemory_operand" "r")
                     (ashift:V4DI (match_operand:V4DI 2 "register_operand" "r")
                                  (const_int 1))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (minus:V2DI (subreg:V2DI (match_dup 1) 0)
                    (ashift:V2DI (subreg:V2DI (match_dup 2) 0)
                                 (const_int 1))))
   (set (subreg:V2DI (match_dup 0) 16)
        (minus:V2DI (subreg:V2DI (match_dup 1) 16)
                    (ashift:V2DI (subreg:V2DI (match_dup 2) 16)
                                 (const_int 1))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx4v4di"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (minus:V4DI (match_operand:V4DI 1 "nonmemory_operand" "r")
                     (mult:V4DI (match_operand:V4DI 2 "register_operand" "r")
                                (const_int 4))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (minus:V2DI (subreg:V2DI (match_dup 1) 0)
                    (mult:V2DI (subreg:V2DI (match_dup 2) 0)
                               (const_int 4))))
   (set (subreg:V2DI (match_dup 0) 16)
        (minus:V2DI (subreg:V2DI (match_dup 1) 16)
                    (mult:V2DI (subreg:V2DI (match_dup 2) 16)
                               (const_int 4))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx4v4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (minus:V4DI (match_operand:V4DI 1 "nonmemory_operand" "r")
                     (ashift:V4DI (match_operand:V4DI 2 "register_operand" "r")
                                  (const_int 2))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (minus:V2DI (subreg:V2DI (match_dup 1) 0)
                    (ashift:V2DI (subreg:V2DI (match_dup 2) 0)
                                 (const_int 2))))
   (set (subreg:V2DI (match_dup 0) 16)
        (minus:V2DI (subreg:V2DI (match_dup 1) 16)
                    (ashift:V2DI (subreg:V2DI (match_dup 2) 16)
                                 (const_int 2))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx8v4di"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (minus:V4DI (match_operand:V4DI 1 "nonmemory_operand" "r")
                    (mult:V4DI (match_operand:V4DI 2 "register_operand" "r")
                               (const_int 8))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (minus:V2DI (subreg:V2DI (match_dup 1) 0)
                    (mult:V2DI (subreg:V2DI (match_dup 2) 0)
                               (const_int 8))))
   (set (subreg:V2DI (match_dup 0) 16)
        (minus:V2DI (subreg:V2DI (match_dup 1) 16)
                    (mult:V2DI (subreg:V2DI (match_dup 2) 16)
                               (const_int 8))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx8v4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (minus:V4DI (match_operand:V4DI 1 "nonmemory_operand" "r")
                    (ashift:V4DI (match_operand:V4DI 2 "register_operand" "r")
                                 (const_int 3))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (minus:V2DI (subreg:V2DI (match_dup 1) 0)
                    (ashift:V2DI (subreg:V2DI (match_dup 2) 0)
                                 (const_int 3))))
   (set (subreg:V2DI (match_dup 0) 16)
        (minus:V2DI (subreg:V2DI (match_dup 1) 16)
                    (ashift:V2DI (subreg:V2DI (match_dup 2) 16)
                                 (const_int 3))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx16v4di"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (minus:V4DI (match_operand:V4DI 1 "nonmemory_operand" "r")
                    (mult:V4DI (match_operand:V4DI 2 "register_operand" "r")
                               (const_int 16))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (minus:V2DI (subreg:V2DI (match_dup 1) 0)
                    (mult:V2DI (subreg:V2DI (match_dup 2) 0)
                               (const_int 16))))
   (set (subreg:V2DI (match_dup 0) 16)
        (minus:V2DI (subreg:V2DI (match_dup 1) 16)
                    (mult:V2DI (subreg:V2DI (match_dup 2) 16)
                               (const_int 16))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx16v4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (minus:V4DI (match_operand:V4DI 1 "nonmemory_operand" "r")
                    (ashift:V4DI (match_operand:V4DI 2 "register_operand" "r")
                                 (const_int 4))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (minus:V2DI (subreg:V2DI (match_dup 1) 0)
                    (ashift:V2DI (subreg:V2DI (match_dup 2) 0)
                                 (const_int 4))))
   (set (subreg:V2DI (match_dup 0) 16)
        (minus:V2DI (subreg:V2DI (match_dup 1) 16)
                    (ashift:V2DI (subreg:V2DI (match_dup 2) 16)
                                 (const_int 4))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "mulv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (mult:V4DI (match_operand:V4DI 1 "register_operand" "r")
                    (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (mult:DI (subreg:DI (match_dup 1) 0)
                 (subreg:DI (match_dup 2) 0)))
   (set (subreg:DI (match_dup 0) 8)
        (mult:DI (subreg:DI (match_dup 1) 8)
                 (subreg:DI (match_dup 2) 8)))
   (set (subreg:DI (match_dup 0) 16)
        (mult:DI (subreg:DI (match_dup 1) 16)
                 (subreg:DI (match_dup 2) 16)))
   (set (subreg:DI (match_dup 0) 24)
        (mult:DI (subreg:DI (match_dup 1) 24)
                 (subreg:DI (match_dup 2) 24)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn "sminv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (smin:V4DI (match_operand:V4DI 1 "register_operand" "r")
                   (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "mind %x0 = %x1, %x2\n\tmind %y0 = %y1, %y2\n\t"
           "mind %z0 = %z1, %z2\n\tmind %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "smaxv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (smax:V4DI (match_operand:V4DI 1 "register_operand" "r")
                   (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "maxd %x0 = %x1, %x2\n\tmaxd %y0 = %y1, %y2\n\t"
           "maxd %z0 = %z1, %z2\n\tmaxd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "uminv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (umin:V4DI (match_operand:V4DI 1 "register_operand" "r")
                   (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "minud %x0 = %x1, %x2\n\tminud %y0 = %y1, %y2\n\t"
           "minud %z0 = %z1, %z2\n\tminud %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "umaxv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (umax:V4DI (match_operand:V4DI 1 "register_operand" "r")
                   (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "maxud %x0 = %x1, %x2\n\tmaxud %y0 = %y1, %y2\n\t"
           "maxud %z0 = %z1, %z2\n\tmaxud %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "andv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (and:V4DI (match_operand:V4DI 1 "register_operand" "r")
                  (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2\n\t"
           "andd %z0 = %z1, %z2\n\tandd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*nandv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (ior:V4DI (not:V4DI (match_operand:V4DI 1 "register_operand" "r"))
                  (not:V4DI (match_operand:V4DI 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nandd %x0 = %x1, %x2\n\tnandd %y0 = %y1, %y2\n\t"
           "nandd %z0 = %z1, %z2\n\tnandd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*andnv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (and:V4DI (not:V4DI (match_operand:V4DI 1 "register_operand" "r"))
                  (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "andnd %x0 = %x1, %x2\n\tandnd %y0 = %y1, %y2\n\t"
           "andnd %z0 = %z1, %z2\n\tandnd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "iorv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (ior:V4DI (match_operand:V4DI 1 "register_operand" "r")
                  (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2\n\t"
           "ord %z0 = %z1, %z2\n\tord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*niorv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (and:V4DI (not:V4DI (match_operand:V4DI 1 "register_operand" "r"))
                  (not:V4DI (match_operand:V4DI 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nord %x0 = %x1, %x2\n\tnord %y0 = %y1, %y2\n\t"
           "nord %z0 = %z1, %z2\n\tnord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*iornv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (ior:V4DI (not:V4DI (match_operand:V4DI 1 "register_operand" "r"))
                  (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "ornd %x0 = %x1, %x2\n\tornd %y0 = %y1, %y2\n\t"
           "ornd %z0 = %z1, %z2\n\tornd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "xorv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (xor:V4DI (match_operand:V4DI 1 "register_operand" "r")
                  (match_operand:V4DI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2\n\t"
           "xord %z0 = %z1, %z2\n\txord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*nxorv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (not:V4DI (xor:V4DI (match_operand:V4DI 1 "register_operand" "r")
                            (match_operand:V4DI 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nxord %x0 = %x1, %x2\n\tnxord %y0 = %y1, %y2\n\t"
           "nxord %z0 = %z1, %z2\n\tnxord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "maddv4div4di4"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (plus:V4DI (mult:V4DI (match_operand:V4DI 1 "register_operand" "r")
                              (match_operand:V4DI 2 "nonmemory_operand" "r"))
                   (match_operand:V4DI 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (plus:DI (mult:DI (subreg:DI (match_dup 1) 0)
                          (subreg:DI (match_dup 2) 0))
                 (subreg:DI (match_dup 3) 0)))
   (set (subreg:DI (match_dup 0) 8)
        (plus:DI (mult:DI (subreg:DI (match_dup 1) 8)
                          (subreg:DI (match_dup 2) 8))
                 (subreg:DI (match_dup 3) 8)))
   (set (subreg:DI (match_dup 0) 16)
        (plus:DI (mult:DI (subreg:DI (match_dup 1) 16)
                          (subreg:DI (match_dup 2) 16))
                 (subreg:DI (match_dup 3) 16)))
   (set (subreg:DI (match_dup 0) 24)
        (plus:DI (mult:DI (subreg:DI (match_dup 1) 24)
                          (subreg:DI (match_dup 2) 24))
                 (subreg:DI (match_dup 3) 24)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "msubv4div4di4"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (minus:V4DI (match_operand:V4DI 3 "register_operand" "0")
                    (mult:V4DI (match_operand:V4DI 1 "register_operand" "r")
                               (match_operand:V4DI 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (minus:DI (subreg:DI (match_dup 3) 0)
                  (mult:DI (subreg:DI (match_dup 1) 0)
                           (subreg:DI (match_dup 2) 0))))
   (set (subreg:DI (match_dup 0) 8)
        (minus:DI (subreg:DI (match_dup 3) 8)
                  (mult:DI (subreg:DI (match_dup 1) 8)
                           (subreg:DI (match_dup 2) 8))))
   (set (subreg:DI (match_dup 0) 16)
        (minus:DI (subreg:DI (match_dup 3) 16)
                  (mult:DI (subreg:DI (match_dup 1) 16)
                           (subreg:DI (match_dup 2) 16))))
   (set (subreg:DI (match_dup 0) 24)
        (minus:DI (subreg:DI (match_dup 3) 24)
                  (mult:DI (subreg:DI (match_dup 1) 24)
                           (subreg:DI (match_dup 2) 24))))]
  ""
  [(set_attr "type" "mau_auxr")
   (set_attr "length"      "8")]
)

(define_insn "ashlv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r,r")
        (ashift:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  {
    return "slld %x0 = %x1, %2\n\tslld %y0 = %y1, %2\n\t"
           "slld %z0 = %z1, %2\n\tslld %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4,alu_tiny_x4")
   (set_attr "length" "       16,         16")]
)

(define_insn_and_split "ssashlv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r,r")
        (ss_ashift:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (ss_ashift:V2DI (subreg:V2DI (match_dup 1) 0)
                        (match_dup 2)))
   (set (subreg:V2DI (match_dup 0) 16)
        (ss_ashift:V2DI (subreg:V2DI (match_dup 1) 16)
                        (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == REG)
      {
        emit_move_insn (operands[3], operands[2]);
        operands[2] = operands[3];
      }
  }
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn "ashrv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r,r")
        (ashiftrt:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  {
    return "srad %x0 = %x1, %2\n\tsrad %y0 = %y1, %2\n\t"
           "srad %z0 = %z1, %2\n\tsrad %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4,alu_tiny_x4")
   (set_attr "length" "       16,         16")]
)

(define_insn "lshrv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r,r")
        (lshiftrt:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  {
    return "srld %x0 = %x1, %2\n\tsrld %y0 = %y1, %2\n\t"
           "srld %z0 = %z1, %2\n\tsrld %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4,alu_tiny_x4")
   (set_attr "length" "       16,         16")]
)

(define_insn_and_split "kvx_srsdqs"
  [(set (match_operand:V4DI 0 "register_operand" "=r,r")
        (unspec:V4DI [(match_operand:V4DI 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRSDQ))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 0)
                      (match_dup 2)] UNSPEC_SRSDP))
   (set (subreg:V2DI (match_dup 0) 16)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 16)
                      (match_dup 2)] UNSPEC_SRSDP))]
  {
    if (GET_CODE (operands[2]) == REG)
      {
        emit_move_insn (operands[3], operands[2]);
        operands[2] = operands[3];
      }
  }
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn "negv4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (neg:V4DI (match_operand:V4DI 1 "register_operand" "r")))]
  ""
  {
    return "negd %x0 = %x1\n\tnegd %y0 = %y1\n\t"
           "negd %z0 = %z1\n\tnegd %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4_x")
   (set_attr "length"          "32")]
)

(define_insn_and_split "absv4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (abs:V4DI (match_operand:V4DI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (abs:V2DI (subreg:V2DI (match_dup 1) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (abs:V2DI (subreg:V2DI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2_x")]
)

(define_insn_and_split "clrsbv4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (clrsb:V4DI (match_operand:V4DI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (clrsb:V2DI (subreg:V2DI (match_dup 1) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (clrsb:V2DI (subreg:V2DI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "clzv4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (clz:V4DI (match_operand:V4DI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (clz:V2DI (subreg:V2DI (match_dup 1) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (clz:V2DI (subreg:V2DI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "ctzv4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (ctz:V4DI (match_operand:V4DI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (ctz:V2DI (subreg:V2DI (match_dup 1) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (ctz:V2DI (subreg:V2DI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "popcountv4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (popcount:V4DI (match_operand:V4DI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (popcount:V2DI (subreg:V2DI (match_dup 1) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (popcount:V2DI (subreg:V2DI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "one_cmplv4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (not:V4DI (match_operand:V4DI 1 "register_operand" "r")))]
  ""
  {
    return "notd %x0 = %x1\n\tnotd %y0 = %y1\n\t"
           "notd %z0 = %z1\n\tnotd %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "kvx_abddq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (abs:V4DI (minus:V4DI (match_operand:V4DI 2 "nonmemory_operand" "r")
                              (match_operand:V4DI 1 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (abs:V2DI (minus:V2DI (subreg:V2DI (match_dup 2) 0)
                              (subreg:V2DI (match_dup 1) 0))))
   (set (subreg:V2DI (match_dup 0) 16)
        (abs:V2DI (minus:V2DI (subreg:V2DI (match_dup 2) 16)
                              (subreg:V2DI (match_dup 1) 16))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_expand "kvx_abddqs"
  [(set (match_operand:V4DI 0 "register_operand" "")
        (abs:V4DI (minus:V4DI (unspec:V4DI [(match_operand:DI 2 "nonmemory_operand" "")] UNSPEC_V256)
                              (match_operand:V4DI 1 "register_operand" ""))))]
  ""
  {
    if (operands[2] == const0_rtx)
      {
        emit_insn (gen_absv4di2 (operands[0], operands[1]));
        DONE;
      }
  }
)

(define_insn_and_split "*abddqs"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (abs:V4DI (minus:V4DI (unspec:V4DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V256)
                              (match_operand:V4DI 1 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (abs:V2DI (minus:V2DI (unspec:V2DI [(match_dup 2)] UNSPEC_V128)
                  (subreg:V2DI (match_dup 1) 0))))
   (set (subreg:V2DI (match_dup 0) 16)
        (abs:V2DI (minus:V2DI (unspec:V2DI [(match_dup 2)] UNSPEC_V128)
                  (subreg:V2DI (match_dup 1) 16))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "kvx_addsdqs"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (ss_plus:V4DI (match_operand:V4DI 1 "register_operand" "r")
                      (unspec:V4DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V256)))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (ss_plus:V2DI (subreg:V2DI (match_dup 1) 0)
                      (unspec:V2DI [(match_dup 2)] UNSPEC_V128)))
   (set (subreg:V2DI (match_dup 0) 16)
        (ss_plus:V2DI (subreg:V2DI (match_dup 1) 16)
                      (unspec:V2DI [(match_dup 2)] UNSPEC_V128)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "kvx_sbfsdqs"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (ss_minus:V4DI (unspec:V4DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V256)
                       (match_operand:V4DI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (ss_minus:V2DI (unspec:V2DI [(match_dup 2)] UNSPEC_V128)
                       (subreg:V2DI (match_dup 1) 0)))
   (set (subreg:V2DI (match_dup 0) 16)
        (ss_minus:V2DI (unspec:V2DI [(match_dup 2)] UNSPEC_V128)
                       (subreg:V2DI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "kvx_mindqs"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (smin:V4DI (match_operand:V4DI 1 "register_operand" "r")
                   (unspec:V4DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V256)))]
  ""
  {
    return "mind %x0 = %x1, %2\n\tmind %y0 = %y1, %2\n\t"
           "mind %z0 = %z1, %2\n\tmind %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "kvx_maxdqs"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (smax:V4DI (match_operand:V4DI 1 "register_operand" "r")
                   (unspec:V4DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V256)))]
  ""
  {
    return "maxd %x0 = %x1, %2\n\tmaxd %y0 = %y1, %2\n\t"
           "maxd %z0 = %z1, %2\n\tmaxd %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "kvx_minudqs"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (umin:V4DI (match_operand:V4DI 1 "register_operand" "r")
                   (unspec:V4DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V256)))]
  ""
  {
    return "minud %x0 = %x1, %2\n\tminud %y0 = %y1, %2\n\t"
           "minud %z0 = %z1, %2\n\tminud %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "kvx_maxudqs"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (umax:V4DI (match_operand:V4DI 1 "register_operand" "r")
                   (unspec:V4DI [(match_operand:DI 2 "nonmemory_operand" "r")] UNSPEC_V256)))]
  ""
  {
    return "maxud %x0 = %x1, %2\n\tmaxud %y0 = %y1, %2\n\t"
           "maxud %z0 = %z1, %2\n\tmaxud %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; S64F (V4HF/V2SF)

(define_insn "*fcompn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(match_operand:S64F 2 "register_operand" "r")
          (match_operand:S64F 3 "register_operand" "r")]))]
  ""
  "fcompn<suffix>.%f1 %0 = %2, %3"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*selectf<suffix>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (if_then_else:S64F (match_operator 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                           (match_operand:S64F 1 "register_operand" "r")
                           (match_operand:S64F 4 "register_operand" "0")))]
  ""
  "cmove<suffix>.%2z %3? %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*selectf<suffix>_nez"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (if_then_else:S64F (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                               (match_operand:<MASK> 6 "const_zero_operand" ""))
                           (match_operand:S64F 1 "register_operand" "r")
                           (match_operand:S64F 4 "register_operand" "0")))]
  ""
  "cmove<suffix>.%2z %3? %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_selectf<suffix>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:S64F 1 "register_operand" "r")
                      (match_operand:S64F 2 "register_operand" "0")
                      (match_operand:<MASK> 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_SELECTFWP))]
  ""
  "cmove<suffix>%4 %3? %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "add<mode>3"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (plus:S64F (match_operand:S64F 1 "register_operand" "r")
                   (match_operand:S64F 2 "register_operand" "r")))]
  ""
  "fadd<suffix> %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fadd<suffix>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:S64F 1 "register_operand" "r")
                      (match_operand:S64F 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDWP))]
  ""
  "fadd<suffix>%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "sub<mode>3"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (minus:S64F (match_operand:S64F 1 "register_operand" "r")
                    (match_operand:S64F 2 "register_operand" "r")))]
  ""
  "fsbf<suffix> %0 = %2, %1"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fsbf<suffix>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:S64F 1 "register_operand" "r")
                      (match_operand:S64F 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFWP))]
  ""
  "fsbf<suffix>%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "mul<mode>3"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (mult:S64F (match_operand:S64F 1 "register_operand" "r")
                   (match_operand:S64F 2 "register_operand" "r")))]
  ""
  "fmul<suffix> %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fmul<suffix>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:S64F 1 "register_operand" "r")
                      (match_operand:S64F 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWP))]
  ""
  "fmul<suffix>%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "fma<mode>4"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (fma:S64F (match_operand:S64F 1 "register_operand" "r")
                  (match_operand:S64F 2 "register_operand" "r")
                  (match_operand:S64F 3 "register_operand" "0")))]
  ""
  "ffma<suffix> %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_ffma<suffix>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:S64F 1 "register_operand" "r")
                      (match_operand:S64F 2 "register_operand" "r")
                      (match_operand:S64F 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMAWP))]
  ""
  "ffma<suffix>%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "fnma<mode>4"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (fma:S64F (neg:S64F (match_operand:S64F 1 "register_operand" "r"))
                  (match_operand:S64F 2 "register_operand" "r")
                  (match_operand:S64F 3 "register_operand" "0")))]
  ""
  "ffms<suffix> %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_ffms<suffix>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:S64F 1 "register_operand" "r")
                      (match_operand:S64F 2 "register_operand" "r")
                      (match_operand:S64F 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSWP))]
  ""
  "ffms<suffix>%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "fmin<mode>3"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (smin:S64F (match_operand:S64F 1 "register_operand" "r")
                   (match_operand:S64F 2 "register_operand" "r")))]
  ""
  "fmin<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "fmax<mode>3"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (smax:S64F (match_operand:S64F 1 "register_operand" "r")
                   (match_operand:S64F 2 "register_operand" "r")))]
  ""
  "fmax<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "neg<mode>2"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (neg:S64F (match_operand:S64F 1 "register_operand" "r")))]
  ""
  "fneg<suffix> %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "abs<mode>2"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (abs:S64F (match_operand:S64F 1 "register_operand" "r")))]
  ""
  "fabs<suffix> %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_expand "copysign<mode>3"
  [(match_operand:S64F 0 "register_operand")
   (match_operand:S64F 1 "register_operand")
   (match_operand:S64F 2 "register_operand")]
  ""
  {
    rtx fabs1 = gen_reg_rtx (<MODE>mode);
    emit_insn (gen_abs<mode>2 (fabs1, operands[1]));
    rtx fneg1 = gen_reg_rtx (<MODE>mode);
    emit_insn (gen_neg<mode>2 (fneg1, fabs1));
    rtx sign2 = gen_reg_rtx (<MASK>mode);
    convert_move (sign2, operands[2], 0);
    rtx ltz = gen_rtx_CONST_STRING (VOIDmode, ".ltz");
    emit_insn (gen_kvx_selectf<suffix> (operands[0], fneg1, fabs1, sign2, ltz));
    DONE;
  }
)

(define_expand "kvx_fmin<suffix>s"
  [(set (match_operand:S64F 0 "register_operand" "")
        (smin:S64F (match_operand:S64F 1 "register_operand" "")
                   (match_operand:S64F 2 "register_operand" "")))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_expand "kvx_fmax<suffix>s"
  [(set (match_operand:S64F 0 "register_operand" "")
        (smax:S64F (match_operand:S64F 1 "register_operand" "")
                   (match_operand:S64F 2 "register_operand" "")))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)


;; V2SF

(define_insn "kvx_fdot2w"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FDOT2W))]
  ""
  "fdot2w%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fdot2wd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FDOT2WD))]
  ""
  "fdot2wd%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fdot2wdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FDOT2WDP))]
  ""
  "fdot2wdp%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_fdot2wzp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FDOT2WZP))]
  ""
  "fdot2wzp%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmaw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand 3 "" "")] UNSPEC_FFDMAW))]
  ""
  {
    emit_insn (gen_kvx_fdot2w (operands[0], operands[1], operands[2], operands[3]));
    DONE;
  }
)

(define_expand "kvx_ffdmsw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand 3 "" "")] UNSPEC_FFDMSW))]
  ""
  {
    rtx fconj = gen_reg_rtx (V2SFmode);
    emit_insn (gen_kvx_fconjwc (fconj, operands[1]));
    emit_insn (gen_kvx_ffdmaw (operands[0], fconj, operands[2], operands[3]));
    DONE;
  }
)

(define_expand "kvx_ffdmdaw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand:SF 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFDMDAW))]
  ""
  {
    rtx ffdma = gen_reg_rtx (SFmode);
    emit_insn (gen_kvx_ffdmaw (ffdma, operands[1], operands[2], operands[4]));
    emit_insn (gen_kvx_faddw (operands[0], ffdma, operands[3], operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffdmsaw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand:SF 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFDMSAW))]
  ""
  {
    rtx ffdmaw = gen_reg_rtx (SFmode);
    rtx fconj = gen_reg_rtx (V2SFmode);
    emit_insn (gen_kvx_fconjwc (fconj, operands[1]));
    emit_insn (gen_kvx_ffdmaw (ffdmaw, fconj, operands[2], operands[4]));
    emit_insn (gen_kvx_fsbfw (operands[0], ffdmaw, operands[3], operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffdmdsw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand:SF 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFDMDSW))]
  ""
  {
    rtx ffdma = gen_reg_rtx (SFmode);
    emit_insn (gen_kvx_ffdmaw (ffdma, operands[1], operands[2], operands[4]));
    emit_insn (gen_kvx_fsbfw (operands[0], ffdma, operands[3], operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffdmasw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand:SF 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFDMASW))]
  ""
  {
    rtx ffdmaw = gen_reg_rtx (SFmode);
    rtx fconj = gen_reg_rtx (V2SFmode);
    emit_insn (gen_kvx_fconjwc (fconj, operands[1]));
    emit_insn (gen_kvx_ffdmaw (ffdmaw, fconj, operands[2], operands[4]));
    emit_insn (gen_kvx_faddw (operands[0], ffdmaw, operands[3], operands[4]));
    DONE;
  }
)

(define_insn "kvx_fmulwc"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWC))]
  ""
  "fmulwc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_ffmawc"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "")
                      (match_operand:V2SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMAWC))]
  ""
  {
    rtx product = gen_reg_rtx (V2SFmode);
    emit_insn (gen_kvx_fmulwc (product, operands[2], operands[1], operands[4]));
    emit_insn (gen_kvx_faddwp (operands[0], product, operands[3], operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffmswc"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "")
                      (match_operand:V2SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMSWC))]
  ""
  {
    rtx product = gen_reg_rtx (V2SFmode);
    emit_insn (gen_kvx_fmulwc (product, operands[2], operands[1], operands[4]));
    emit_insn (gen_kvx_fsbfwp (operands[0], product, operands[3], operands[4]));
    DONE;
  }
)

(define_insn "floatv2siv2sf2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (float:V2SF (match_operand:V2SI 1 "register_operand" "r")))]
  ""
  "floatwp.rn %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_floatwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATWP))]
  ""
  "floatwp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "floatunsv2siv2sf2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unsigned_float:V2SF (match_operand:V2SI 1 "register_operand" "r")))]
  ""
  "floatuwp.rn %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_floatuwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATUWP))]
  ""
  "floatuwp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "fix_truncv2sfv2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (fix:V2SI (match_operand:V2SF 1 "register_operand" "r")))]
  ""
  "fixedwp.rz %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fixedwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDWP))]
  ""
  "fixedwp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "fixuns_truncv2sfv2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unsigned_fix:V2SI (match_operand:V2SF 1 "register_operand" "r")))]
  ""
  "fixeduwp.rz %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fixeduwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDUWP))]
  ""
  "fixeduwp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "extendv2sfv2df2"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (float_extend:V2DF (match_operand:V2SF 1 "register_operand" "r")))]
  ""
  "fwidenlwd %x0 = %1\n\tfwidenmwd %y0 = %1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_frecwp"
  [(match_operand:V2SF 0 "register_operand" "")
   (match_operand:V2SF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    rtx op0x = gen_reg_rtx (SFmode);
    rtx op0y = gen_reg_rtx (SFmode);
    rtx op1y = gen_reg_rtx (SFmode);
    emit_insn (gen_rtx_SET (op1y, gen_rtx_UNSPEC (SFmode, gen_rtvec (2, operands[1], GEN_INT (32)), UNSPEC_SRLD)));
    emit_insn (gen_kvx_frecw (op0x, gen_rtx_SUBREG (SFmode, operands[1], 0), operands[2]));
    emit_insn (gen_kvx_frecw (op0y, op1y, operands[2]));
    emit_insn (gen_kvx_consfwp (operands[0], op0x, op0y));
    DONE;
  }
)

(define_expand "kvx_frsrwp"
  [(match_operand:V2SF 0 "register_operand" "")
   (match_operand:V2SF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    rtx op0x = gen_reg_rtx (SFmode);
    rtx op0y = gen_reg_rtx (SFmode);
    rtx op1y = gen_reg_rtx (SFmode);
    emit_insn (gen_rtx_SET (op1y, gen_rtx_UNSPEC (SFmode, gen_rtvec (2, operands[1], GEN_INT (32)), UNSPEC_SRLD)));
    emit_insn (gen_kvx_frsrw (op0x, gen_rtx_SUBREG (SFmode, operands[1], 0), operands[2]));
    emit_insn (gen_kvx_frsrw (op0y, op1y, operands[2]));
    emit_insn (gen_kvx_consfwp (operands[0], op0x, op0y));
    DONE;
  }
)

(define_expand "kvx_fcdivwp"
  [(match_operand:V2SF 0 "register_operand" "")
   (match_operand:V2SF 1 "register_operand" "")
   (match_operand:V2SF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    rtx regpair = gen_reg_rtx (V4SFmode);
    emit_insn (gen_kvx_consfwq (regpair, operands[1], operands[2]));
    emit_insn (gen_kvx_fcdivwp_insn (operands[0], regpair, operands[3]));
    DONE;
  }
)

(define_insn "kvx_fcdivwp_insn"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand 2 "" "")] UNSPEC_FCDIVWP))]
  ""
  "fcdivwp%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_expand "kvx_fsdivwp"
  [(match_operand:V2SF 0 "register_operand" "")
   (match_operand:V2SF 1 "register_operand" "")
   (match_operand:V2SF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    rtx regpair = gen_reg_rtx (V4SFmode);
    emit_insn (gen_kvx_consfwq (regpair, operands[1], operands[2]));
    emit_insn (gen_kvx_fsdivwp_insn (operands[0], regpair, operands[3]));
    DONE;
  }
)

(define_insn "kvx_fsdivwp_insn"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand 2 "" "")] UNSPEC_FSDIVWP))]
  ""
  "fsdivwp%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fsrecwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand 2 "" "")] UNSPEC_FSRECWP))]
  ""
  "fsrecwp%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fsrsrwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand 2 "" "")] UNSPEC_FSRSRWP))]
  ""
  "fsrsrwp%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fmm212w"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMM212W))]
  ""
  "fmm212w%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fmma212w"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FMMA212W))]
  ""
  "fmma212w%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_fmms212w"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FMMS212W))]
  ""
  "fmms212w%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_fconjwc"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FCONJWC))]
  ""
  "fnegd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_consfwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (vec_concat:V2SF (match_operand:SF 1 "register_operand" "0")
                         (match_operand:SF 2 "register_operand" "r")))]
  ""
  "insf %0 = %2, 63, 32"
  [(set_attr "type" "alu_lite")]
)


;; VXSF

(define_expand "div<mode>3"
  [(set (match_operand:VXSF 0 "register_operand" "")
        (div:VXSF (match_operand:VXSF 1 "reg_or_float1_operand" "")
                  (match_operand:VXSF 2 "register_operand" "")))]
  "flag_reciprocal_math || flag_unsafe_math_optimizations"
  {
    rtx rm = gen_rtx_CONST_STRING (VOIDmode, "");
    rtx rn = gen_rtx_CONST_STRING (VOIDmode, ".rn");
    rtx a = operands[1], b = operands[2];
    if (a == CONST1_RTX (<MODE>mode))
      {
        emit_insn (gen_kvx_frec<suffix> (operands[0], b, rm));
      }
    else if (flag_reciprocal_math)
      {
        rtx t = gen_reg_rtx(<MODE>mode);
        emit_insn (gen_kvx_frec<suffix> (t, b, rm));
        emit_insn (gen_kvx_fmul<suffix> (operands[0], a, t, rm));
      }
    else // (flag_unsafe_math_optimizations)
      {
        rtx re = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_kvx_frec<suffix> (re, b, rn));
        rtx y0 = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_kvx_fmul<suffix> (y0, a, re, rn));
        rtx e0 = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_kvx_ffms<suffix> (e0, b, y0, a, rn));
        rtx y1 = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_kvx_ffma<suffix> (y1, e0, re, y0, rn));
        rtx e1 = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_kvx_ffms<suffix> (e1, b, y1, a, rn));
        rtx y2 = operands[0];
        emit_insn (gen_kvx_ffma<suffix> (y2, e1, re, y1, rm));
      }
    DONE;
  }
)

(define_expand "sqrt<mode>2"
  [(match_operand:VXSF 0 "register_operand" "")
   (match_operand:VXSF 1 "register_operand" "")]
  "flag_reciprocal_math"
  {
    rtx temp = gen_reg_rtx(<MODE>mode);
    rtx rm = gen_rtx_CONST_STRING (VOIDmode, "");
    emit_insn (gen_kvx_frsr<suffix> (temp, operands[1], rm));
    emit_insn (gen_mul<mode>3 (operands[0], operands[1], temp));
    DONE;
  }
)

(define_expand "recip<mode>2"
  [(match_operand:VXSF 0 "register_operand" "")
   (match_operand:VXSF 1 "register_operand" "")]
  ""
  {
    rtx rm = gen_rtx_CONST_STRING (VOIDmode, "");
    emit_insn (gen_kvx_frec<suffix> (operands[0], operands[1], rm));
    DONE;
  }
)

(define_expand "rsqrt<mode>2"
  [(match_operand:VXSF 0 "register_operand" "")
   (match_operand:VXSF 1 "register_operand" "")]
  ""
  {
    rtx rm = gen_rtx_CONST_STRING (VOIDmode, "");
    emit_insn (gen_kvx_frsr<suffix> (operands[0], operands[1], rm));
    DONE;
  }
)


;; S128F (V8HF/V4SF)

(define_insn "*fcompn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(match_operand:S128F 2 "register_operand" "r")
          (match_operand:S128F 3 "register_operand" "r")]))]
  ""
  "fcompn<halfx>.%f1 %x0 = %x2, %x3\n\tfcompn<halfx>.%f1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*selectf<suffix>"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (if_then_else:S128F (match_operator 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                            (match_operand:S128F 1 "register_operand" "r")
                            (match_operand:S128F 4 "register_operand" "0")))]
  ""
  "cmove<halfx>.%2z %x3? %x0 = %x1\n\tcmove<halfx>.%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*selectf<suffix>_nez"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (if_then_else:S128F (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                                (match_operand:<MASK> 6 "const_zero_operand" ""))
                            (match_operand:S128F 1 "register_operand" "r")
                            (match_operand:S128F 4 "register_operand" "0")))]
  ""
  "cmove<halfx>.%2z %x3? %x0 = %x1\n\tcmove<halfx>.%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_selectf<suffix>"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "r")
                       (match_operand:S128F 2 "register_operand" "0")
                       (match_operand:<MASK> 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECTFWQ))]
  ""
  "cmove<halfx>%4 %x3? %x0 = %x1\n\tcmove<halfx>%4 %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "fma<mode>4"
  [(set (match_operand:S128F 0 "register_operand" "")
        (fma:S128F (match_operand:S128F 1 "register_operand" "")
                   (match_operand:S128F 2 "register_operand" "")
                   (match_operand:S128F 3 "register_operand" "")))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_fma<mode>4_1 (operands[0], operands[1], operands[2], operands[3]));
    if (KV3_2)
      emit_insn (gen_fma<mode>4_2 (operands[0], operands[1], operands[2], operands[3]));
    DONE;
  }
)

(define_insn_and_split "fma<mode>4_1"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (fma:S128F (match_operand:S128F 1 "register_operand" "r")
                   (match_operand:S128F 2 "register_operand" "r")
                   (match_operand:S128F 3 "register_operand" "0")))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (fma:<CHUNK> (subreg:<CHUNK> (match_dup 1) 0)
                     (subreg:<CHUNK> (match_dup 2) 0)
                     (subreg:<CHUNK> (match_dup 3) 0)))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (fma:<CHUNK> (subreg:<CHUNK> (match_dup 1) 8)
                     (subreg:<CHUNK> (match_dup 2) 8)
                     (subreg:<CHUNK> (match_dup 3) 8)))]
  ""
)

(define_insn "fma<mode>4_2"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (fma:S128F (match_operand:S128F 1 "register_operand" "r")
                   (match_operand:S128F 2 "register_operand" "r")
                   (match_operand:S128F 3 "register_operand" "0")))]
  "KV3_2"
  "ffma<suffix> %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffma<suffix>"
  [(set (match_operand:S128F 0 "register_operand" "")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "")
                       (match_operand:S128F 2 "register_operand" "")
                       (match_operand:S128F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMAWQ))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_kvx_ffma<suffix>_1 (operands[0], operands[1], operands[2], operands[3], operands[4]));
    if (KV3_2)
      emit_insn (gen_kvx_ffma<suffix>_2 (operands[0], operands[1], operands[2], operands[3], operands[4]));
    DONE;
  }
)

(define_insn_and_split "kvx_ffma<suffix>_1"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "r")
                       (match_operand:S128F 2 "register_operand" "r")
                       (match_operand:S128F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMAWQ))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<CHUNK> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMAWP))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<CHUNK> (match_dup 3) 8)
                         (match_dup 4)] UNSPEC_FFMAWP))]
  ""
)

(define_insn "kvx_ffma<suffix>_2"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "r")
                       (match_operand:S128F 2 "register_operand" "r")
                       (match_operand:S128F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMAWQ))]
  "KV3_2"
  "ffma<suffix>%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "fnma<mode>4"
  [(set (match_operand:S128F 0 "register_operand" "")
        (fma:S128F (neg:S128F (match_operand:S128F 1 "register_operand" ""))
                   (match_operand:S128F 2 "register_operand" "")
                   (match_operand:S128F 3 "register_operand" "")))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_fnma<mode>4_1 (operands[0], operands[1], operands[2], operands[3]));
    if (KV3_2)
      emit_insn (gen_fnma<mode>4_2 (operands[0], operands[1], operands[2], operands[3]));
    DONE;
  }
)

(define_insn_and_split "fnma<mode>4_1"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (fma:S128F (neg:S128F (match_operand:S128F 1 "register_operand" "r"))
                   (match_operand:S128F 2 "register_operand" "r")
                   (match_operand:S128F 3 "register_operand" "0")))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (fma:<CHUNK> (neg:<CHUNK> (subreg:<CHUNK> (match_dup 1) 0))
                     (subreg:<CHUNK> (match_dup 2) 0)
                     (subreg:<CHUNK> (match_dup 3) 0)))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (fma:<CHUNK> (neg:<CHUNK> (subreg:<CHUNK> (match_dup 1) 8))
                     (subreg:<CHUNK> (match_dup 2) 8)
                     (subreg:<CHUNK> (match_dup 3) 8)))]
  ""
)

(define_insn "fnma<mode>4_2"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (fma:S128F (neg:S128F (match_operand:S128F 1 "register_operand" "r"))
                   (match_operand:S128F 2 "register_operand" "r")
                   (match_operand:S128F 3 "register_operand" "0")))]
  "KV3_2"
  "ffms<suffix> %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffms<suffix>"
  [(set (match_operand:S128F 0 "register_operand" "")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "")
                       (match_operand:S128F 2 "register_operand" "")
                       (match_operand:S128F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMSWQ))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_kvx_ffms<suffix>_1 (operands[0], operands[1], operands[2], operands[3], operands[4]));
    if (KV3_2)
      emit_insn (gen_kvx_ffms<suffix>_2 (operands[0], operands[1], operands[2], operands[3], operands[4]));
    DONE;
  }
)

(define_insn_and_split "kvx_ffms<suffix>_1"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "r")
                       (match_operand:S128F 2 "register_operand" "r")
                       (match_operand:S128F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMSWQ))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<CHUNK> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMSWP))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<CHUNK> (match_dup 3) 8)
                         (match_dup 4)] UNSPEC_FFMSWP))]
  ""
)

(define_insn "kvx_ffms<suffix>_2"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "r")
                       (match_operand:S128F 2 "register_operand" "r")
                       (match_operand:S128F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMSWQ))]
  "KV3_2"
  "ffms<suffix>%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "fmin<mode>3"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (smin:S128F (match_operand:S128F 1 "register_operand" "r")
                    (match_operand:S128F 2 "register_operand" "r")))]
  ""
  "fmin<halfx> %x0 = %x1, %x2\n\tfmin<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "fmax<mode>3"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (smax:S128F (match_operand:S128F 1 "register_operand" "r")
                    (match_operand:S128F 2 "register_operand" "r")))]
  ""
  "fmax<halfx> %x0 = %x1, %x2\n\tfmax<halfx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "neg<mode>2"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (neg:S128F (match_operand:S128F 1 "register_operand" "r")))]
  ""
  "fneg<halfx> %x0 = %x1\n\tfneg<halfx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "abs<mode>2"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (abs:S128F (match_operand:S128F 1 "register_operand" "r")))]
  ""
  "fabs<halfx> %x0 = %x1\n\tfabs<halfx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "copysign<mode>3"
  [(match_operand:S128F 0 "register_operand")
   (match_operand:S128F 1 "register_operand")
   (match_operand:S128F 2 "register_operand")]
  ""
  {
    rtx fabs1 = gen_reg_rtx (<MODE>mode);
    emit_insn (gen_abs<mode>2 (fabs1, operands[1]));
    rtx fneg1 = gen_reg_rtx (<MODE>mode);
    emit_insn (gen_neg<mode>2 (fneg1, fabs1));
    rtx sign2 = gen_reg_rtx (<MASK>mode);
    convert_move (sign2, operands[2], 0);
    rtx ltz = gen_rtx_CONST_STRING (VOIDmode, ".ltz");
    emit_insn (gen_kvx_selectf<suffix> (operands[0], fneg1, fabs1, sign2, ltz));
    DONE;
  }
)

(define_expand "kvx_fmin<suffix>s"
  [(set (match_operand:S128F 0 "register_operand" "")
        (smin:S128F (match_operand:S128F 1 "register_operand" "")
                    (match_operand:<CHUNK> 2 "register_operand" "")))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn "*fmin<suffix>s"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (smin:S128F (match_operand:S128F 1 "register_operand" "r")
                    (match_operand:<CHUNK> 2 "register_operand" "r")))]
  ""
  "fmin<halfx> %x0 = %x1, %2\n\tfmin<halfx> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_fmax<suffix>s"
  [(set (match_operand:S128F 0 "register_operand" "")
        (smax:S128F (match_operand:S128F 1 "register_operand" "")
                    (match_operand:<CHUNK> 2 "register_operand" "")))]
  ""
  {
    rtx chunk = gen_reg_rtx (<CHUNK>mode);
    operands[2] = kvx_expand_chunk_splat (chunk, operands[2], <INNER>mode);
  }
)

(define_insn "*fmax<suffix>s"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (smax:S128F (match_operand:S128F 1 "register_operand" "r")
                    (match_operand:<CHUNK> 2 "register_operand" "r")))]
  ""
  "fmax<halfx> %x0 = %x1, %2\n\tfmax<halfx> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)


;; V8HF

(define_expand "addv8hf3"
  [(set (match_operand:V8HF 0 "register_operand" "")
        (plus:V8HF (match_operand:V8HF 1 "register_operand" "")
                   (match_operand:V8HF 2 "register_operand" "")))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_addv8hf3_1 (operands[0], operands[1], operands[2]));
    if (KV3_2)
      emit_insn (gen_addv8hf3_2 (operands[0], operands[1], operands[2]));
    DONE;
  }
)

(define_insn_and_split "addv8hf3_1"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (plus:V8HF (match_operand:V8HF 1 "register_operand" "r")
                   (match_operand:V8HF 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (plus:V4HF (subreg:V4HF (match_dup 1) 0)
                   (subreg:V4HF (match_dup 2) 0)))
   (set (subreg:V4HF (match_dup 0) 8)
        (plus:V4HF (subreg:V4HF (match_dup 1) 8)
                   (subreg:V4HF (match_dup 2) 8)))]
  ""
)

(define_insn "addv8hf3_2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (plus:V8HF (match_operand:V8HF 1 "register_operand" "r")
                   (match_operand:V8HF 2 "register_operand" "r")))]
  "KV3_2"
  "faddho %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_faddho"
  [(set (match_operand:V8HF 0 "register_operand" "")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "")
                      (match_operand:V8HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FADDWQ))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_kvx_faddho_1 (operands[0], operands[1], operands[2], operands[3]));
    if (KV3_2)
      emit_insn (gen_kvx_faddho_2 (operands[0], operands[1], operands[2], operands[3]));
    DONE;
  }
)

(define_insn_and_split "kvx_faddho_1"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDWQ))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADDWP))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FADDWP))]
  ""
)

(define_insn "kvx_faddho_2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDWQ))]
  "KV3_2"
  "faddho%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "subv8hf3"
  [(set (match_operand:V8HF 0 "register_operand" "")
        (minus:V8HF (match_operand:V8HF 1 "register_operand" "")
                    (match_operand:V8HF 2 "register_operand" "")))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_subv8hf3_1 (operands[0], operands[1], operands[2]));
    if (KV3_2)
      emit_insn (gen_subv8hf3_2 (operands[0], operands[1], operands[2]));
    DONE;
  }
)

(define_insn_and_split "subv8hf3_1"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (minus:V8HF (match_operand:V8HF 1 "register_operand" "r")
                    (match_operand:V8HF 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (minus:V4HF (subreg:V4HF (match_dup 1) 0)
                    (subreg:V4HF (match_dup 2) 0)))
   (set (subreg:V4HF (match_dup 0) 8)
        (minus:V4HF (subreg:V4HF (match_dup 1) 8)
                    (subreg:V4HF (match_dup 2) 8)))]
  ""
)

(define_insn "subv8hf3_2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (minus:V8HF (match_operand:V8HF 1 "register_operand" "r")
                    (match_operand:V8HF 2 "register_operand" "r")))]
  "KV3_2"
  "fsbfho %0 = %2, %1"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_fsbfho"
  [(set (match_operand:V8HF 0 "register_operand" "")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "")
                      (match_operand:V8HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FSBFWQ))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_kvx_fsbfho_1 (operands[0], operands[1], operands[2], operands[3]));
    if (KV3_2)
      emit_insn (gen_kvx_fsbfho_2 (operands[0], operands[1], operands[2], operands[3]));
    DONE;
  }
)

(define_insn_and_split "kvx_fsbfho_1"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFWQ))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBFWP))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FSBFWP))]
  ""
)

(define_insn "kvx_fsbfho_2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFWQ))]
  "KV3_2"
  "fsbfho%3 %0 = %2, %1"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "mulv8hf3"
  [(set (match_operand:V8HF 0 "register_operand" "")
        (mult:V8HF (match_operand:V8HF 1 "register_operand" "")
                   (match_operand:V8HF 2 "register_operand" "")))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_mulv8hf3_1 (operands[0], operands[1], operands[2]));
    if (KV3_2)
      emit_insn (gen_mulv8hf3_2 (operands[0], operands[1], operands[2]));
    DONE;
  }
)

(define_insn_and_split "mulv8hf3_1"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (mult:V8HF (match_operand:V8HF 1 "register_operand" "r")
                   (match_operand:V8HF 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (mult:V4HF (subreg:V4HF (match_dup 1) 0)
                   (subreg:V4HF (match_dup 2) 0)))
   (set (subreg:V4HF (match_dup 0) 8)
        (mult:V4HF (subreg:V4HF (match_dup 1) 8)
                   (subreg:V4HF (match_dup 2) 8)))]
  ""
)

(define_insn "mulv8hf3_2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (mult:V8HF (match_operand:V8HF 1 "register_operand" "r")
                   (match_operand:V8HF 2 "register_operand" "r")))]
  "KV3_2"
  "fmulho %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_fmulho"
  [(set (match_operand:V8HF 0 "register_operand" "")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "")
                      (match_operand:V8HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMULWQ))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_kvx_fmulho_1 (operands[0], operands[1], operands[2], operands[3]));
    if (KV3_2)
      emit_insn (gen_kvx_fmulho_2 (operands[0], operands[1], operands[2], operands[3]));
    DONE;
  }
)

(define_insn_and_split "kvx_fmulho_1"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWQ))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMULWP))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FMULWP))]
  ""
)

(define_insn "kvx_fmulho_2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWQ))]
  "KV3_2"
  "fmulho%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)


;; V4SF

(define_expand  "kvx_fmt22w"
  [(match_operand:V4SF 0 "register_operand" "")
   (match_operand:V4SF 1 "register_operand" "")]
  ""
  {
    int table[4] = {0,2,1,3};
    rtvec values = rtvec_alloc (4);
    for (int i = 0; i < 4; i++)
      RTVEC_ELT (values, i) = GEN_INT (table[i]);
    rtx selector = gen_rtx_CONST_VECTOR (V4SImode, values);
    kvx_expand_vec_perm_const (operands[0], operands[1], operands[1], selector);
    DONE;
  }
)

(define_expand "kvx_fmm222w"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMM222W))]
  ""
  {
    rtx modifiers = operands[3];
    const char *xstr = XSTR (modifiers, 0);
    bool transpose = xstr && xstr[0] == '.' && xstr[1] == 't';
    if (transpose)
      {
        modifiers = gen_rtx_CONST_STRING (VOIDmode, xstr + 2);
        rtx operands1 = gen_reg_rtx (V4SFmode);
        rtx operands2 = gen_reg_rtx (V4SFmode);
        emit_insn (gen_kvx_fmt22w (operands1, operands[1]));
        emit_insn (gen_kvx_fmt22w (operands2, operands[2]));
        operands[1] = operands1;
        operands[2] = operands2;
      }
    rtx accum = gen_reg_rtx (V4SFmode);
    rtx opnd1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx opnd2_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    emit_insn (gen_kvx_fmm212w (accum, opnd1_0, opnd2_0, modifiers));
    rtx opnd1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx opnd2_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    emit_insn (gen_kvx_fmma212w (operands[0], opnd1_1, opnd2_1, accum, modifiers));
    DONE;
  }
)

(define_expand "kvx_fmma222w"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FMMA222W))]
  ""
  {
    rtx modifiers = operands[4];
    const char *xstr = XSTR (modifiers, 0);
    bool transpose = xstr && xstr[0] == '.' && xstr[1] == 't';
    if (transpose)
      {
        modifiers = gen_rtx_CONST_STRING (VOIDmode, xstr + 2);
        rtx operands1 = gen_reg_rtx (V4SFmode);
        rtx operands2 = gen_reg_rtx (V4SFmode);
        emit_insn (gen_kvx_fmt22w (operands1, operands[1]));
        emit_insn (gen_kvx_fmt22w (operands2, operands[2]));
        operands[1] = operands1;
        operands[2] = operands2;
      }
    rtx accum = gen_reg_rtx (V4SFmode);
    rtx opnd1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx opnd2_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    emit_insn (gen_kvx_fmma212w (accum, opnd1_0, opnd2_0, operands[3], modifiers));
    rtx opnd1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx opnd2_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    emit_insn (gen_kvx_fmma212w (operands[0], opnd1_1, opnd2_1, accum, modifiers));
    DONE;
  }
)

(define_expand "kvx_fmms222w"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FMMS222W))]
  ""
  {
    rtx modifiers = operands[4];
    const char *xstr = XSTR (modifiers, 0);
    bool transpose = xstr && xstr[0] == '.' && xstr[1] == 't';
    if (transpose)
      {
        modifiers = gen_rtx_CONST_STRING (VOIDmode, xstr + 2);
        rtx operands1 = gen_reg_rtx (V4SFmode);
        rtx operands2 = gen_reg_rtx (V4SFmode);
        emit_insn (gen_kvx_fmt22w (operands1, operands[1]));
        emit_insn (gen_kvx_fmt22w (operands2, operands[2]));
        operands[1] = operands1;
        operands[2] = operands2;
      }
    rtx accum = gen_reg_rtx (V4SFmode);
    rtx opnd1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx opnd2_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    emit_insn (gen_kvx_fmms212w (accum, opnd1_0, opnd2_0, operands[3], modifiers));
    rtx opnd1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx opnd2_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    emit_insn (gen_kvx_fmms212w (operands[0], opnd1_1, opnd2_1, accum, modifiers));
    DONE;
  }
)

(define_expand "kvx_ffdmawp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FFDMAWP))]
  ""
  {
    rtx accum = gen_reg_rtx (V2SFmode);
    rtx opnd1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx opnd2_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    emit_insn (gen_kvx_fmulwp (accum, opnd1_0, opnd2_0, operands[3]));
    rtx opnd1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx opnd2_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    emit_insn (gen_kvx_ffmawp (operands[0], opnd1_1, opnd2_1, accum, operands[3]));
    DONE;
  }
)

(define_expand "kvx_ffdmswp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FFDMSWP))]
  ""
  {
    rtx accum = gen_reg_rtx (V2SFmode);
    rtx opnd1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx opnd2_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    emit_insn (gen_kvx_fmulwp (accum, opnd1_0, opnd2_0, operands[3]));
    rtx opnd1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx opnd2_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    emit_insn (gen_kvx_ffmswp (operands[0], opnd1_1, opnd2_1, accum, operands[3]));
    DONE;
  }
)

(define_expand "kvx_ffdmdawp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMDAWP))]
  ""
  {
    rtx accum = gen_reg_rtx (V2SFmode);
    rtx opnd1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx opnd2_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    emit_insn (gen_kvx_ffmawp (accum, opnd1_0, opnd2_0, operands[3], operands[4]));
    rtx opnd1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx opnd2_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    emit_insn (gen_kvx_ffmawp (operands[0], opnd1_1, opnd2_1, accum, operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffdmsawp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMSAWP))]
  ""
  {
    rtx accum = gen_reg_rtx (V2SFmode);
    rtx opnd1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx opnd2_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    emit_insn (gen_kvx_ffmswp (accum, opnd1_0, opnd2_0, operands[3], operands[4]));
    rtx opnd1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx opnd2_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    emit_insn (gen_kvx_ffmawp (operands[0], opnd1_1, opnd2_1, accum, operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffdmdswp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMDSWP))]
  ""
  {
    rtx accum = gen_reg_rtx (V2SFmode);
    rtx opnd1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx opnd2_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    emit_insn (gen_kvx_ffmswp (accum, opnd1_0, opnd2_0, operands[3], operands[4]));
    rtx opnd1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx opnd2_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    emit_insn (gen_kvx_ffmswp (operands[0], opnd1_1, opnd2_1, accum, operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffdmaswp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMASWP))]
  ""
  {
    rtx accum = gen_reg_rtx (V2SFmode);
    rtx opnd1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx opnd2_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    emit_insn (gen_kvx_ffmawp (accum, opnd1_0, opnd2_0, operands[3], operands[4]));
    rtx opnd1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx opnd2_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    emit_insn (gen_kvx_ffmswp (operands[0], opnd1_1, opnd2_1, accum, operands[4]));
    DONE;
  }
)

(define_insn "kvx_fmulwcp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWCP))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMULWCP))]
  "KV3_1 && reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 0)
                      (subreg:V2SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMULWC))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 8)
                      (subreg:V2SF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FMULWC))]
  ""
)

(define_expand "kvx_ffmawcp"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMAWCP))]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx product = gen_reg_rtx (V2SFmode);
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        rtx opnd2 = gen_rtx_SUBREG (V2SFmode, operands[2], i*8);
        rtx opnd3 = gen_rtx_SUBREG (V2SFmode, operands[3], i*8);
        emit_insn (gen_kvx_fmulwc (product, opnd2, opnd1, operands[4]));
        emit_insn (gen_kvx_faddwp (opnd0, product, opnd3, operands[4]));
      }
    DONE;
  }
)

(define_expand "kvx_ffmswcp"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMSWCP))]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx product = gen_reg_rtx (V2SFmode);
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        rtx opnd2 = gen_rtx_SUBREG (V2SFmode, operands[2], i*8);
        rtx opnd3 = gen_rtx_SUBREG (V2SFmode, operands[3], i*8);
        emit_insn (gen_kvx_fmulwc (product, opnd2, opnd1, operands[4]));
        emit_insn (gen_kvx_fsbfwp (opnd0, product, opnd3, operands[4]));
      }
    DONE;
  }
)

(define_insn "addv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (plus:V4SF (match_operand:V4SF 1 "register_operand" "r")
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "faddwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_faddwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDWQ))]
  ""
  "faddwq%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "subv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (minus:V4SF (match_operand:V4SF 1 "register_operand" "r")
                    (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "fsbfwq %0 = %2, %1"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_fsbfwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFWQ))]
  ""
  "fsbfwq%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "mulv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (mult:V4SF (match_operand:V4SF 1 "register_operand" "r")
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "fmulwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_fmulwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWQ))]
  ""
  "fmulwq%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "floatv4siv4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (float:V4SF (match_operand:V4SI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (float:V2SF (subreg:V2SI (match_dup 1) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (float:V2SF (subreg:V2SI (match_dup 1) 8)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATWQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATWP))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATWP))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "floatunsv4siv4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unsigned_float:V4SF (match_operand:V4SI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unsigned_float:V2SF (subreg:V2SI (match_dup 1) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (unsigned_float:V2SF (subreg:V2SI (match_dup 1) 8)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatuwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATUWQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATUWP))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATUWP))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "fix_truncv4sfv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (fix:V4SI (match_operand:V4SF 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (fix:V2SI (subreg:V2SF (match_dup 1) 0)))
   (set (subreg:V2SI (match_dup 0) 8)
        (fix:V2SI (subreg:V2SF (match_dup 1) 8)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixedwq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unspec:V4SI [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDWQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDWP))
   (set (subreg:V2SI (match_dup 0) 8)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDWP))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "fixuns_truncv4sfv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unsigned_fix:V4SI (match_operand:V4SF 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (unsigned_fix:V2SI (subreg:V2SF (match_dup 1) 0)))
   (set (subreg:V2SI (match_dup 0) 8)
        (unsigned_fix:V2SI (subreg:V2SF (match_dup 1) 8)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixeduwq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unspec:V4SI [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDUWQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDUWP))
   (set (subreg:V2SI (match_dup 0) 8)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDUWP))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_frecwq"
  [(match_operand:V4SF 0 "register_operand" "")
   (match_operand:V4SF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        emit_insn (gen_kvx_frecwp (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)

(define_expand "kvx_frsrwq"
  [(match_operand:V4SF 0 "register_operand" "")
   (match_operand:V4SF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        emit_insn (gen_kvx_frsrwp (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)

(define_expand "kvx_fcdivwq"
  [(match_operand:V4SF 0 "register_operand" "")
   (match_operand:V4SF 1 "register_operand" "")
   (match_operand:V4SF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        rtx opnd2 = gen_rtx_SUBREG (V2SFmode, operands[2], i*8);
        emit_insn (gen_kvx_fcdivwp (opnd0, opnd1, opnd2, operands[3]));
      }
    DONE;
  }
)

(define_expand "kvx_fsdivwq"
  [(match_operand:V4SF 0 "register_operand" "")
   (match_operand:V4SF 1 "register_operand" "")
   (match_operand:V4SF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        rtx opnd2 = gen_rtx_SUBREG (V2SFmode, operands[2], i*8);
        emit_insn (gen_kvx_fsdivwp (opnd0, opnd1, opnd2, operands[3]));
      }
    DONE;
  }
)

(define_expand "kvx_fsrecwq"
  [(match_operand:V4SF 0 "register_operand" "")
   (match_operand:V4SF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], 8*i);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8*i);
        emit_insn (gen_kvx_fsrecwp (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)

(define_expand "kvx_fsrsrwq"
  [(match_operand:V4SF 0 "register_operand" "")
   (match_operand:V4SF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        emit_insn (gen_kvx_fsrsrwp (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)

(define_insn "kvx_fconjwcp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")] UNSPEC_FCONJWCP))]
  ""
  "fnegd %x0 = %x1\n\tfnegd %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)


;; W128A

(define_expand "kvx_shift<lsvs>"
  [(match_operand:W128A 0 "register_operand" "")
   (match_operand:W128A 1 "register_operand" "")
   (match_operand:SI 2 "sixbits_unsigned_operand" "")
   (match_operand:<INNER> 3 "nonmemory_operand" "")]
  ""
  {
    int shift = INTVAL (operands[2]) * GET_MODE_BITSIZE (<INNER>mode);
    rtx opnd0_0 = gen_rtx_SUBREG (<INNER>mode, operands[0], 0);
    rtx opnd0_1 = gen_rtx_SUBREG (<INNER>mode, operands[0], 8);
    rtx opnd1_1 = gen_rtx_SUBREG (<INNER>mode, operands[1], 8);
    rtx filler = operands[3];
    if (shift == 0)
      {
        emit_move_insn (operands[0], operands[1]);
      }
    else if (shift == 64)
      {
        emit_move_insn (opnd0_0, opnd1_1);
        emit_move_insn (opnd0_1, filler);
      }
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "kvx_cons<lsvs>"
  [(set (match_operand:W128A 0 "register_operand" "=r")
        (vec_concat:W128A (match_operand:<HALF> 1 "register_operand" "0")
                          (match_operand:<HALF> 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 8)
        (match_dup 2))]
  ""
)


;; S256F (V16HF/V8SF)

(define_insn_and_split "*fcompn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(match_operand:S256F 2 "register_operand" "r")
          (match_operand:S256F 3 "register_operand" "r")]))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HMASK> (match_dup 0) 0)
        (match_op_dup:<HMASK> 1
         [(subreg:<HALF> (match_dup 2) 0)
          (subreg:<HALF> (match_dup 3) 0)]))
   (set (subreg:<HMASK> (match_dup 0) 16)
        (match_op_dup:<HMASK> 1
         [(subreg:<HALF> (match_dup 2) 16)
          (subreg:<HALF> (match_dup 3) 16)]))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*selectf<suffix>"
  [(set (match_operand:S256F 0 "register_operand" "=r")
        (if_then_else:S256F (match_operator 2 "zero_comparison_operator"
                                             [(match_operand:<MASK> 3 "register_operand" "r")
                                              (match_operand:<MASK> 5 "const_zero_operand" "")])
                           (match_operand:S256F 1 "register_operand" "r")
                           (match_operand:S256F 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 0)
                                            (const_vector:<HMASK> [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:<HALF> (match_dup 1) 0)
                           (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 16)
                                            (const_vector:<HMASK> [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:<HALF> (match_dup 1) 16)
                           (subreg:<HALF> (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*selectf<suffix>_nez"
  [(set (match_operand:S256F 0 "register_operand" "=r")
        (if_then_else:S256F (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                             [(match_operand:<MASK> 3 "register_operand" "r")
                                              (match_operand:<MASK> 5 "const_zero_operand" "")])
                               (match_operand:<MASK> 6 "const_zero_operand" ""))
                           (match_operand:S256F 1 "register_operand" "r")
                           (match_operand:S256F 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 0)
                                            (const_vector:<HMASK> [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:<HALF> (match_dup 1) 0)
                           (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 16)
                                            (const_vector:<HMASK> [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:<HALF> (match_dup 1) 16)
                           (subreg:<HALF> (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "kvx_selectf<suffix>"
  [(set (match_operand:S256F 0 "register_operand" "=r")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "r")
                      (match_operand:S256F 2 "register_operand" "0")
                      (match_operand:<MASK> 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_SELECTFWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                      (subreg:<HALF> (match_dup 2) 0)
                      (subreg:<HMASK> (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_SELECTFWQ))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                      (subreg:<HALF> (match_dup 2) 16)
                      (subreg:<HMASK> (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_SELECTFWQ))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "fma<mode>4"
  [(set (match_operand:S256F 0 "register_operand" "=r")
        (fma:S256F (match_operand:S256F 1 "register_operand" "r")
                   (match_operand:S256F 2 "register_operand" "r")
                   (match_operand:S256F 3 "register_operand" "0")))]
  ""
  "#"
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (fma:S256F (match_operand:S256F 1 "register_operand" "")
                   (match_operand:S256F 2 "register_operand" "")
                   (match_operand:S256F 3 "register_operand" "")))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (fma:<CHUNK> (subreg:<CHUNK> (match_dup 1) 0)
                     (subreg:<CHUNK> (match_dup 2) 0)
                     (subreg:<CHUNK> (match_dup 3) 0)))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (fma:<CHUNK> (subreg:<CHUNK> (match_dup 1) 8)
                     (subreg:<CHUNK> (match_dup 2) 8)
                     (subreg:<CHUNK> (match_dup 3) 8)))
   (set (subreg:<CHUNK> (match_dup 0) 16)
        (fma:<CHUNK> (subreg:<CHUNK> (match_dup 1) 16)
                     (subreg:<CHUNK> (match_dup 2) 16)
                     (subreg:<CHUNK> (match_dup 3) 16)))
   (set (subreg:<CHUNK> (match_dup 0) 24)
        (fma:<CHUNK> (subreg:<CHUNK> (match_dup 1) 24)
                     (subreg:<CHUNK> (match_dup 2) 24)
                     (subreg:<CHUNK> (match_dup 3) 24)))]
  ""
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (fma:S256F (match_operand:S256F 1 "register_operand" "")
                   (match_operand:S256F 2 "register_operand" "")
                   (match_operand:S256F 3 "register_operand" "")))]
  "KV3_2 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (fma:<HALF> (subreg:<HALF> (match_dup 1) 0)
                    (subreg:<HALF> (match_dup 2) 0)
                    (subreg:<HALF> (match_dup 3) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (fma:<HALF> (subreg:<HALF> (match_dup 1) 16)
                    (subreg:<HALF> (match_dup 2) 16)
                    (subreg:<HALF> (match_dup 3) 16)))]
  ""
)

(define_insn "kvx_ffma<suffix>"
  [(set (match_operand:S256F 0 "register_operand" "=r")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "r")
                       (match_operand:S256F 2 "register_operand" "r")
                       (match_operand:S256F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMAWO))]
  ""
  "#"
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "")
                       (match_operand:S256F 2 "register_operand" "")
                       (match_operand:S256F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMAWO))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<CHUNK> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMAWP))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<CHUNK> (match_dup 3) 8)
                         (match_dup 4)] UNSPEC_FFMAWP))
   (set (subreg:<CHUNK> (match_dup 0) 16)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 16)
                         (subreg:<CHUNK> (match_dup 2) 16)
                         (subreg:<CHUNK> (match_dup 3) 16)
                         (match_dup 4)] UNSPEC_FFMAWP))
   (set (subreg:<CHUNK> (match_dup 0) 24)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 24)
                         (subreg:<CHUNK> (match_dup 2) 24)
                         (subreg:<CHUNK> (match_dup 3) 24)
                         (match_dup 4)] UNSPEC_FFMAWP))]
  ""
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "")
                       (match_operand:S256F 2 "register_operand" "")
                       (match_operand:S256F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMAWO))]
  "KV3_2 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)
                        (subreg:<HALF> (match_dup 3) 0)
                        (match_dup 4)] UNSPEC_FFMAWQ))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)
                        (subreg:<HALF> (match_dup 3) 16)
                        (match_dup 4)] UNSPEC_FFMAWQ))]
  ""
)

(define_insn "fnma<mode>4"
  [(set (match_operand:S256F 0 "register_operand" "=r")
        (fma:S256F (neg:S256F (match_operand:S256F 1 "register_operand" "r"))
                   (match_operand:S256F 2 "register_operand" "r")
                   (match_operand:S256F 3 "register_operand" "0")))]
  ""
  "#"
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (fma:S256F (neg:S256F (match_operand:S256F 1 "register_operand" ""))
                   (match_operand:S256F 2 "register_operand" "")
                   (match_operand:S256F 3 "register_operand" "")))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (fma:<CHUNK> (neg:<CHUNK> (subreg:<CHUNK> (match_dup 1) 0))
                     (subreg:<CHUNK> (match_dup 2) 0)
                     (subreg:<CHUNK> (match_dup 3) 0)))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (fma:<CHUNK> (neg:<CHUNK> (subreg:<CHUNK> (match_dup 1) 8))
                     (subreg:<CHUNK> (match_dup 2) 8)
                     (subreg:<CHUNK> (match_dup 3) 8)))
   (set (subreg:<CHUNK> (match_dup 0) 16)
        (fma:<CHUNK> (neg:<CHUNK> (subreg:<CHUNK> (match_dup 1) 16))
                     (subreg:<CHUNK> (match_dup 2) 16)
                     (subreg:<CHUNK> (match_dup 3) 16)))
   (set (subreg:<CHUNK> (match_dup 0) 24)
        (fma:<CHUNK> (neg:<CHUNK> (subreg:<CHUNK> (match_dup 1) 24))
                     (subreg:<CHUNK> (match_dup 2) 24)
                     (subreg:<CHUNK> (match_dup 3) 24)))]
  ""
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (fma:S256F (neg:S256F (match_operand:S256F 1 "register_operand" ""))
                   (match_operand:S256F 2 "register_operand" "")
                   (match_operand:S256F 3 "register_operand" "")))]
  "KV3_2 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (fma:<HALF> (neg:<HALF> (subreg:<HALF> (match_dup 1) 0))
                    (subreg:<HALF> (match_dup 2) 0)
                    (subreg:<HALF> (match_dup 3) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (fma:<HALF> (neg:<HALF> (subreg:<HALF> (match_dup 1) 16))
                    (subreg:<HALF> (match_dup 2) 16)
                    (subreg:<HALF> (match_dup 3) 16)))]
  ""
)

(define_insn "kvx_ffms<suffix>"
  [(set (match_operand:S256F 0 "register_operand" "=r")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "r")
                       (match_operand:S256F 2 "register_operand" "r")
                       (match_operand:S256F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMSWO))]
  ""
  "#"
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "")
                       (match_operand:S256F 2 "register_operand" "")
                       (match_operand:S256F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMSWO))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<CHUNK> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMSWP))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<CHUNK> (match_dup 3) 8)
                         (match_dup 4)] UNSPEC_FFMSWP))
   (set (subreg:<CHUNK> (match_dup 0) 16)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 16)
                         (subreg:<CHUNK> (match_dup 2) 16)
                         (subreg:<CHUNK> (match_dup 3) 16)
                         (match_dup 4)] UNSPEC_FFMSWP))
   (set (subreg:<CHUNK> (match_dup 0) 24)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 24)
                         (subreg:<CHUNK> (match_dup 2) 24)
                         (subreg:<CHUNK> (match_dup 3) 24)
                         (match_dup 4)] UNSPEC_FFMSWP))]
  ""
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "")
                       (match_operand:S256F 2 "register_operand" "")
                       (match_operand:S256F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMSWO))]
  "KV3_2 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)
                        (subreg:<HALF> (match_dup 3) 0)
                        (match_dup 4)] UNSPEC_FFMSWQ))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)
                        (subreg:<HALF> (match_dup 3) 16)
                        (match_dup 4)] UNSPEC_FFMSWQ))]
  ""
)

(define_insn_and_split "fmin<mode>3"
  [(set (match_operand:S256F 0 "register_operand" "=r")
        (smin:S256F (match_operand:S256F 1 "register_operand" "r")
                   (match_operand:S256F 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (smin:<HALF> (subreg:<HALF> (match_dup 1) 0)
                   (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (smin:<HALF> (subreg:<HALF> (match_dup 1) 16)
                   (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "fmax<mode>3"
  [(set (match_operand:S256F 0 "register_operand" "=r")
        (smax:S256F (match_operand:S256F 1 "register_operand" "r")
                   (match_operand:S256F 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (smax:<HALF> (subreg:<HALF> (match_dup 1) 0)
                   (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (smax:<HALF> (subreg:<HALF> (match_dup 1) 16)
                   (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "neg<mode>2"
  [(set (match_operand:S256F 0 "register_operand" "=r")
        (neg:S256F (match_operand:S256F 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (neg:<HALF> (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (neg:<HALF> (subreg:<HALF> (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "abs<mode>2"
  [(set (match_operand:S256F 0 "register_operand" "=r")
        (abs:S256F (match_operand:S256F 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (abs:<HALF> (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (abs:<HALF> (subreg:<HALF> (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_expand "copysign<mode>3"
  [(match_operand:S256F 0 "register_operand")
   (match_operand:S256F 1 "register_operand")
   (match_operand:S256F 2 "register_operand")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (<HALF>mode, operands[0], i*16);
        rtx opnd1 = gen_rtx_SUBREG (<HALF>mode, operands[1], i*16);
        rtx opnd2 = gen_rtx_SUBREG (<HALF>mode, operands[2], i*16);
        emit_insn (gen_copysign<half>3 (opnd0, opnd1, opnd2));
      }
    DONE;
  }
)

(define_expand "kvx_fmin<suffix>s"
  [(match_operand:S256F 0 "register_operand" "")
   (match_operand:S256F 1 "register_operand" "")
   (match_operand:<INNER> 2 "register_operand" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (<CHUNK>mode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (<CHUNK>mode, operands[1], i*8);
        emit_insn (gen_kvx_fmin<chunkx>s (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)

(define_expand "kvx_fmax<suffix>s"
  [(match_operand:S256F 0 "register_operand" "")
   (match_operand:S256F 1 "register_operand" "")
   (match_operand:<INNER> 2 "register_operand" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (<CHUNK>mode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (<CHUNK>mode, operands[1], i*8);
        emit_insn (gen_kvx_fmax<chunkx>s (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)


;; V16HF

(define_insn "addv16hf3"
  [(set (match_operand:V16HF 0 "register_operand" "=r")
        (plus:V16HF (match_operand:V16HF 1 "register_operand" "r")
                   (match_operand:V16HF 2 "register_operand" "r")))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (plus:V16HF (match_operand:V16HF 1 "register_operand" "")
                   (match_operand:V16HF 2 "register_operand" "")))]
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (plus:V4HF (subreg:V4HF (match_dup 1) 0)
                   (subreg:V4HF (match_dup 2) 0)))
   (set (subreg:V4HF (match_dup 0) 8)
        (plus:V4HF (subreg:V4HF (match_dup 1) 8)
                   (subreg:V4HF (match_dup 2) 8)))
   (set (subreg:V4HF (match_dup 0) 16)
        (plus:V4HF (subreg:V4HF (match_dup 1) 16)
                   (subreg:V4HF (match_dup 2) 16)))
   (set (subreg:V4HF (match_dup 0) 24)
        (plus:V4HF (subreg:V4HF (match_dup 1) 24)
                   (subreg:V4HF (match_dup 2) 24)))]
  ""
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (plus:V16HF (match_operand:V16HF 1 "register_operand" "")
                   (match_operand:V16HF 2 "register_operand" "")))]
  "KV3_2 && reload_completed"
  [(set (subreg:V8HF (match_dup 0) 0)
        (plus:V8HF (subreg:V8HF (match_dup 1) 0)
                   (subreg:V8HF (match_dup 2) 0)))
   (set (subreg:V8HF (match_dup 0) 16)
        (plus:V8HF (subreg:V8HF (match_dup 1) 16)
                   (subreg:V8HF (match_dup 2) 16)))]
  ""
)

(define_insn "kvx_faddhx"
  [(set (match_operand:V16HF 0 "register_operand" "=r")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "r")
                      (match_operand:V16HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDWO))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FADDWO))]
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADDWP))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FADDWP))
   (set (subreg:V4HF (match_dup 0) 16)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 16)
                      (subreg:V4HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADDWP))
   (set (subreg:V4HF (match_dup 0) 24)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 24)
                      (subreg:V4HF (match_dup 2) 24)
                      (match_dup 3)] UNSPEC_FADDWP))]
  ""
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FADDWO))]
  "KV3_2 && reload_completed"
  [(set (subreg:V8HF (match_dup 0) 0)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 0)
                      (subreg:V8HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADDWQ))
   (set (subreg:V8HF (match_dup 0) 16)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 16)
                      (subreg:V8HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADDWQ))]
  ""
)

(define_insn "subv16hf3"
  [(set (match_operand:V16HF 0 "register_operand" "=r")
        (minus:V16HF (match_operand:V16HF 1 "register_operand" "r")
                    (match_operand:V16HF 2 "register_operand" "r")))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (minus:V16HF (match_operand:V16HF 1 "register_operand" "")
                    (match_operand:V16HF 2 "register_operand" "")))]
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (minus:V4HF (subreg:V4HF (match_dup 1) 0)
                    (subreg:V4HF (match_dup 2) 0)))
   (set (subreg:V4HF (match_dup 0) 8)
        (minus:V4HF (subreg:V4HF (match_dup 1) 8)
                    (subreg:V4HF (match_dup 2) 8)))
   (set (subreg:V4HF (match_dup 0) 16)
        (minus:V4HF (subreg:V4HF (match_dup 1) 16)
                    (subreg:V4HF (match_dup 2) 16)))
   (set (subreg:V4HF (match_dup 0) 24)
        (minus:V4HF (subreg:V4HF (match_dup 1) 24)
                    (subreg:V4HF (match_dup 2) 24)))]
  ""
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (minus:V16HF (match_operand:V16HF 1 "register_operand" "")
                    (match_operand:V16HF 2 "register_operand" "")))]
  "KV3_2 && reload_completed"
  [(set (subreg:V8HF (match_dup 0) 0)
        (minus:V8HF (subreg:V8HF (match_dup 1) 0)
                    (subreg:V8HF (match_dup 2) 0)))
   (set (subreg:V8HF (match_dup 0) 16)
        (minus:V8HF (subreg:V8HF (match_dup 1) 16)
                    (subreg:V8HF (match_dup 2) 16)))]
  ""
)

(define_insn "kvx_fsbfhx"
  [(set (match_operand:V16HF 0 "register_operand" "=r")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "r")
                      (match_operand:V16HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFWO))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FSBFWO))]
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBFWP))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FSBFWP))
   (set (subreg:V4HF (match_dup 0) 16)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 16)
                      (subreg:V4HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBFWP))
   (set (subreg:V4HF (match_dup 0) 24)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 24)
                      (subreg:V4HF (match_dup 2) 24)
                      (match_dup 3)] UNSPEC_FSBFWP))]
  ""
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FSBFWO))]
  "KV3_2 && reload_completed"
  [(set (subreg:V8HF (match_dup 0) 0)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 0)
                      (subreg:V8HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBFWQ))
   (set (subreg:V8HF (match_dup 0) 16)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 16)
                      (subreg:V8HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBFWQ))]
  ""
)

(define_insn "mulv16hf3"
  [(set (match_operand:V16HF 0 "register_operand" "=r")
        (mult:V16HF (match_operand:V16HF 1 "register_operand" "r")
                   (match_operand:V16HF 2 "register_operand" "r")))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (mult:V16HF (match_operand:V16HF 1 "register_operand" "")
                   (match_operand:V16HF 2 "register_operand" "")))]
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (mult:V4HF (subreg:V4HF (match_dup 1) 0)
                   (subreg:V4HF (match_dup 2) 0)))
   (set (subreg:V4HF (match_dup 0) 8)
        (mult:V4HF (subreg:V4HF (match_dup 1) 8)
                   (subreg:V4HF (match_dup 2) 8)))
   (set (subreg:V4HF (match_dup 0) 16)
        (mult:V4HF (subreg:V4HF (match_dup 1) 16)
                   (subreg:V4HF (match_dup 2) 16)))
   (set (subreg:V4HF (match_dup 0) 24)
        (mult:V4HF (subreg:V4HF (match_dup 1) 24)
                   (subreg:V4HF (match_dup 2) 24)))]
  ""
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (mult:V16HF (match_operand:V16HF 1 "register_operand" "")
                   (match_operand:V16HF 2 "register_operand" "")))]
  "KV3_2 && reload_completed"
  [(set (subreg:V8HF (match_dup 0) 0)
        (mult:V8HF (subreg:V8HF (match_dup 1) 0)
                   (subreg:V8HF (match_dup 2) 0)))
   (set (subreg:V8HF (match_dup 0) 16)
        (mult:V8HF (subreg:V8HF (match_dup 1) 16)
                   (subreg:V8HF (match_dup 2) 16)))]
  ""
)

(define_insn"kvx_fmulhx"
  [(set (match_operand:V16HF 0 "register_operand" "=r")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "r")
                      (match_operand:V16HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWO))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMULWO))]
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMULWP))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FMULWP))
   (set (subreg:V4HF (match_dup 0) 16)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 16)
                      (subreg:V4HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMULWP))
   (set (subreg:V4HF (match_dup 0) 24)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 24)
                      (subreg:V4HF (match_dup 2) 24)
                      (match_dup 3)] UNSPEC_FMULWP))]
  ""
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMULWO))]
  "KV3_2 && reload_completed"
  [(set (subreg:V8HF (match_dup 0) 0)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 0)
                      (subreg:V8HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMULWQ))
   (set (subreg:V8HF (match_dup 0) 16)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 16)
                      (subreg:V8HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMULWQ))]
  ""
)


;; V8SF

(define_expand "kvx_ffdmawq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FFDMAWQ))]
  ""
  {
    rtx accum = gen_reg_rtx (V4SFmode);
    rtx opnd1_0 = gen_rtx_SUBREG (V4SFmode, operands[1], 0);
    rtx opnd2_0 = gen_rtx_SUBREG (V4SFmode, operands[2], 0);
    emit_insn (gen_kvx_fmulwq (accum, opnd1_0, opnd2_0, operands[3]));
    rtx opnd0_0 = gen_rtx_SUBREG (V2SFmode, operands[0], 0);
    rtx opnd0_1 = gen_rtx_SUBREG (V2SFmode, operands[0], 8);
    rtx opnd1_1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 16);
    rtx opnd1_1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 24);
    rtx opnd2_1_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 16);
    rtx opnd2_1_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 24);
    rtx accum_0 = gen_rtx_SUBREG (V2SFmode, accum, 0);
    rtx accum_1 = gen_rtx_SUBREG (V2SFmode, accum, 8);
    emit_insn (gen_kvx_ffmawp (opnd0_0, opnd1_1_0, opnd2_1_0, accum_0, operands[3]));
    emit_insn (gen_kvx_ffmawp (opnd0_1, opnd1_1_1, opnd2_1_1, accum_1, operands[3]));
    DONE;
  }
)

(define_expand "kvx_ffdmswq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FFDMSWQ))]
  ""
  {
    rtx accum = gen_reg_rtx (V4SFmode);
    rtx opnd1_0 = gen_rtx_SUBREG (V4SFmode, operands[1], 0);
    rtx opnd2_0 = gen_rtx_SUBREG (V4SFmode, operands[2], 0);
    emit_insn (gen_kvx_fmulwq (accum, opnd1_0, opnd2_0, operands[3]));
    rtx opnd0_0 = gen_rtx_SUBREG (V2SFmode, operands[0], 0);
    rtx opnd0_1 = gen_rtx_SUBREG (V2SFmode, operands[0], 8);
    rtx opnd1_1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 16);
    rtx opnd1_1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 24);
    rtx opnd2_1_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 16);
    rtx opnd2_1_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 24);
    rtx accum_0 = gen_rtx_SUBREG (V2SFmode, accum, 0);
    rtx accum_1 = gen_rtx_SUBREG (V2SFmode, accum, 8);
    emit_insn (gen_kvx_ffmswp (opnd0_0, opnd1_1_0, opnd2_1_0, accum_0, operands[3]));
    emit_insn (gen_kvx_ffmswp (opnd0_1, opnd1_1_1, opnd2_1_1, accum_1, operands[3]));
    DONE;
  }
)

(define_expand "kvx_ffdmdawq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMDAWQ))]
  ""
  {
    rtx accum_0 = gen_reg_rtx (V2SFmode);
    rtx accum_1 = gen_reg_rtx (V2SFmode);
    rtx opnd1_0_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx opnd1_0_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx opnd2_0_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    rtx opnd2_0_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    rtx opnd3_0 = gen_rtx_SUBREG (V2SFmode, operands[3], 0);
    rtx opnd3_1 = gen_rtx_SUBREG (V2SFmode, operands[3], 8);
    emit_insn (gen_kvx_ffmawp (accum_0, opnd1_0_0, opnd2_0_0, opnd3_0, operands[4]));
    emit_insn (gen_kvx_ffmawp (accum_1, opnd1_0_1, opnd2_0_1, opnd3_1, operands[4]));
    rtx opnd0_0 = gen_rtx_SUBREG (V2SFmode, operands[0], 0);
    rtx opnd0_1 = gen_rtx_SUBREG (V2SFmode, operands[0], 8);
    rtx opnd1_1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 16);
    rtx opnd1_1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 24);
    rtx opnd2_1_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 16);
    rtx opnd2_1_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 24);
    emit_insn (gen_kvx_ffmawp (opnd0_0, opnd1_1_0, opnd2_1_0, accum_0, operands[4]));
    emit_insn (gen_kvx_ffmawp (opnd0_1, opnd1_1_1, opnd2_1_1, accum_1, operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffdmsawq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMSAWQ))]
  ""
  {
    rtx accum_0 = gen_reg_rtx (V2SFmode);
    rtx accum_1 = gen_reg_rtx (V2SFmode);
    rtx opnd1_0_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx opnd1_0_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx opnd2_0_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    rtx opnd2_0_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    rtx opnd3_0 = gen_rtx_SUBREG (V2SFmode, operands[3], 0);
    rtx opnd3_1 = gen_rtx_SUBREG (V2SFmode, operands[3], 8);
    emit_insn (gen_kvx_ffmswp (accum_0, opnd1_0_0, opnd2_0_0, opnd3_0, operands[4]));
    emit_insn (gen_kvx_ffmswp (accum_1, opnd1_0_1, opnd2_0_1, opnd3_1, operands[4]));
    rtx opnd0_0 = gen_rtx_SUBREG (V2SFmode, operands[0], 0);
    rtx opnd0_1 = gen_rtx_SUBREG (V2SFmode, operands[0], 8);
    rtx opnd1_1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 16);
    rtx opnd1_1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 24);
    rtx opnd2_1_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 16);
    rtx opnd2_1_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 24);
    emit_insn (gen_kvx_ffmawp (opnd0_0, opnd1_1_0, opnd2_1_0, accum_0, operands[4]));
    emit_insn (gen_kvx_ffmawp (opnd0_1, opnd1_1_1, opnd2_1_1, accum_1, operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffdmdswq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMDSWQ))]
  ""
  {
    rtx accum_0 = gen_reg_rtx (V2SFmode);
    rtx accum_1 = gen_reg_rtx (V2SFmode);
    rtx opnd1_0_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx opnd1_0_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx opnd2_0_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    rtx opnd2_0_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    rtx opnd3_0 = gen_rtx_SUBREG (V2SFmode, operands[3], 0);
    rtx opnd3_1 = gen_rtx_SUBREG (V2SFmode, operands[3], 8);
    emit_insn (gen_kvx_ffmswp (accum_0, opnd1_0_0, opnd2_0_0, opnd3_0, operands[4]));
    emit_insn (gen_kvx_ffmswp (accum_1, opnd1_0_1, opnd2_0_1, opnd3_1, operands[4]));
    rtx opnd0_0 = gen_rtx_SUBREG (V2SFmode, operands[0], 0);
    rtx opnd0_1 = gen_rtx_SUBREG (V2SFmode, operands[0], 8);
    rtx opnd1_1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 16);
    rtx opnd1_1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 24);
    rtx opnd2_1_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 16);
    rtx opnd2_1_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 24);
    emit_insn (gen_kvx_ffmswp (opnd0_0, opnd1_1_0, opnd2_1_0, accum_0, operands[4]));
    emit_insn (gen_kvx_ffmswp (opnd0_1, opnd1_1_1, opnd2_1_1, accum_1, operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffdmaswq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMASWQ))]
  ""
  {
    rtx accum_0 = gen_reg_rtx (V2SFmode);
    rtx accum_1 = gen_reg_rtx (V2SFmode);
    rtx opnd1_0_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx opnd1_0_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx opnd2_0_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    rtx opnd2_0_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    rtx opnd3_0 = gen_rtx_SUBREG (V2SFmode, operands[3], 0);
    rtx opnd3_1 = gen_rtx_SUBREG (V2SFmode, operands[3], 8);
    emit_insn (gen_kvx_ffmawp (accum_0, opnd1_0_0, opnd2_0_0, opnd3_0, operands[4]));
    emit_insn (gen_kvx_ffmawp (accum_1, opnd1_0_1, opnd2_0_1, opnd3_1, operands[4]));
    rtx opnd0_0 = gen_rtx_SUBREG (V2SFmode, operands[0], 0);
    rtx opnd0_1 = gen_rtx_SUBREG (V2SFmode, operands[0], 8);
    rtx opnd1_1_0 = gen_rtx_SUBREG (V2SFmode, operands[1], 16);
    rtx opnd1_1_1 = gen_rtx_SUBREG (V2SFmode, operands[1], 24);
    rtx opnd2_1_0 = gen_rtx_SUBREG (V2SFmode, operands[2], 16);
    rtx opnd2_1_1 = gen_rtx_SUBREG (V2SFmode, operands[2], 24);
    emit_insn (gen_kvx_ffmswp (opnd0_0, opnd1_1_0, opnd2_1_0, accum_0, operands[4]));
    emit_insn (gen_kvx_ffmswp (opnd0_1, opnd1_1_1, opnd2_1_1, accum_1, operands[4]));
    DONE;
  }
)

(define_insn "kvx_fmulwcq"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWCQ))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V8SF 0 "register_operand" "")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMULWCQ))]
  "KV3_1 && reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 0)
                      (subreg:V2SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMULWC))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 8)
                      (subreg:V2SF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FMULWC))
   (set (subreg:V2SF (match_dup 0) 16)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 16)
                      (subreg:V2SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMULWC))
   (set (subreg:V2SF (match_dup 0) 24)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 24)
                      (subreg:V2SF (match_dup 2) 24)
                      (match_dup 3)] UNSPEC_FMULWC))]
  ""
)

(define_expand "kvx_ffmawcq"
  [(set (match_operand:V8SF 0 "register_operand" "")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V8SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMAWCQ))]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx product = gen_reg_rtx (V2SFmode);
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        rtx opnd2 = gen_rtx_SUBREG (V2SFmode, operands[2], i*8);
        rtx opnd3 = gen_rtx_SUBREG (V2SFmode, operands[3], i*8);
        emit_insn (gen_kvx_fmulwc (product, opnd2, opnd1, operands[4]));
        emit_insn (gen_kvx_faddwp (opnd0, product, opnd3, operands[4]));
      }
    DONE;
  }
)

(define_expand "kvx_ffmswcq"
  [(set (match_operand:V8SF 0 "register_operand" "")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V8SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMSWCQ))]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx product = gen_reg_rtx (V2SFmode);
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        rtx opnd2 = gen_rtx_SUBREG (V2SFmode, operands[2], i*8);
        rtx opnd3 = gen_rtx_SUBREG (V2SFmode, operands[3], i*8);
        emit_insn (gen_kvx_fmulwc (product, opnd2, opnd1, operands[4]));
        emit_insn (gen_kvx_fsbfwp (opnd0, product, opnd3, operands[4]));
      }
    DONE;
  }
)

(define_insn_and_split "addv8sf3"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (plus:V8SF (match_operand:V8SF 1 "register_operand" "r")
                   (match_operand:V8SF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (plus:V4SF (subreg:V4SF (match_dup 1) 0)
                   (subreg:V4SF (match_dup 2) 0)))
   (set (subreg:V4SF (match_dup 0) 16)
        (plus:V4SF (subreg:V4SF (match_dup 1) 16)
                   (subreg:V4SF (match_dup 2) 16)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_faddwo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADDWQ))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADDWQ))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "subv8sf3"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (minus:V8SF (match_operand:V8SF 1 "register_operand" "r")
                    (match_operand:V8SF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (minus:V4SF (subreg:V4SF (match_dup 1) 0)
                    (subreg:V4SF (match_dup 2) 0)))
   (set (subreg:V4SF (match_dup 0) 16)
        (minus:V4SF (subreg:V4SF (match_dup 1) 16)
                    (subreg:V4SF (match_dup 2) 16)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fsbfwo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBFWQ))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBFWQ))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "mulv8sf3"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (mult:V8SF (match_operand:V8SF 1 "register_operand" "r")
                   (match_operand:V8SF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (mult:V4SF (subreg:V4SF (match_dup 1) 0)
                   (subreg:V4SF (match_dup 2) 0)))
   (set (subreg:V4SF (match_dup 0) 16)
        (mult:V4SF (subreg:V4SF (match_dup 1) 16)
                   (subreg:V4SF (match_dup 2) 16)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fmulwo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMULWQ))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMULWQ))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "floatv8siv8sf2"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (float:V8SF (match_operand:V8SI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (float:V2SF (subreg:V2SI (match_dup 1) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (float:V2SF (subreg:V2SI (match_dup 1) 8)))
   (set (subreg:V2SF (match_dup 0) 16)
        (float:V2SF (subreg:V2SI (match_dup 1) 16)))
   (set (subreg:V2SF (match_dup 0) 24)
        (float:V2SF (subreg:V2SI (match_dup 1) 24)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatwo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATWP))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATWP))
   (set (subreg:V2SF (match_dup 0) 16)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 16)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATWP))
   (set (subreg:V2SF (match_dup 0) 24)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 24)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATWP))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "floatunsv8siv8sf2"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unsigned_float:V8SF (match_operand:V8SI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unsigned_float:V2SF (subreg:V2SI (match_dup 1) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (unsigned_float:V2SF (subreg:V2SI (match_dup 1) 8)))
   (set (subreg:V2SF (match_dup 0) 16)
        (unsigned_float:V2SF (subreg:V2SI (match_dup 1) 16)))
   (set (subreg:V2SF (match_dup 0) 24)
        (unsigned_float:V2SF (subreg:V2SI (match_dup 1) 24)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatuwo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATUWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATUWP))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATUWP))
   (set (subreg:V2SF (match_dup 0) 16)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 16)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATUWP))
   (set (subreg:V2SF (match_dup 0) 24)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 24)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATUWP))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "fix_truncv8sfv8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (fix:V8SI (match_operand:V8SF 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (fix:V2SI (subreg:V2SF (match_dup 1) 0)))
   (set (subreg:V2SI (match_dup 0) 8)
        (fix:V2SI (subreg:V2SF (match_dup 1) 8)))
   (set (subreg:V2SI (match_dup 0) 16)
        (fix:V2SI (subreg:V2SF (match_dup 1) 16)))
   (set (subreg:V2SI (match_dup 0) 24)
        (fix:V2SI (subreg:V2SF (match_dup 1) 24)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixedwo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (unspec:V8SI [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDWP))
   (set (subreg:V2SI (match_dup 0) 8)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDWP))
   (set (subreg:V2SI (match_dup 0) 16)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 16)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDWP))
   (set (subreg:V2SI (match_dup 0) 24)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 24)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDWP))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "fixuns_truncv8sfv8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (unsigned_fix:V8SI (match_operand:V8SF 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (unsigned_fix:V2SI (subreg:V2SF (match_dup 1) 0)))
   (set (subreg:V2SI (match_dup 0) 8)
        (unsigned_fix:V2SI (subreg:V2SF (match_dup 1) 8)))
   (set (subreg:V2SI (match_dup 0) 16)
        (unsigned_fix:V2SI (subreg:V2SF (match_dup 1) 16)))
   (set (subreg:V2SI (match_dup 0) 24)
        (unsigned_fix:V2SI (subreg:V2SF (match_dup 1) 24)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixeduwo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (unspec:V8SI [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDUWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDUWP))
   (set (subreg:V2SI (match_dup 0) 8)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDUWP))
   (set (subreg:V2SI (match_dup 0) 16)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 16)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDUWP))
   (set (subreg:V2SI (match_dup 0) 24)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 24)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDUWP))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_frecwo"
  [(match_operand:V8SF 0 "register_operand" "")
   (match_operand:V8SF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        emit_insn (gen_kvx_frecwp (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)

(define_expand "kvx_frsrwo"
  [(match_operand:V8SF 0 "register_operand" "")
   (match_operand:V8SF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        emit_insn (gen_kvx_frsrwp (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)

(define_expand "kvx_fcdivwo"
  [(match_operand:V8SF 0 "register_operand" "")
   (match_operand:V8SF 1 "register_operand" "")
   (match_operand:V8SF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        rtx opnd2 = gen_rtx_SUBREG (V2SFmode, operands[2], i*8);
        emit_insn (gen_kvx_fcdivwp (opnd0, opnd1, opnd2, operands[3]));
      }
    DONE;
  }
)

(define_expand "kvx_fsdivwo"
  [(match_operand:V8SF 0 "register_operand" "")
   (match_operand:V8SF 1 "register_operand" "")
   (match_operand:V8SF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        rtx opnd2 = gen_rtx_SUBREG (V2SFmode, operands[2], i*8);
        emit_insn (gen_kvx_fsdivwp (opnd0, opnd1, opnd2, operands[3]));
      }
    DONE;
  }
)

(define_expand "kvx_fsrecwo"
  [(match_operand:V8SF 0 "register_operand" "")
   (match_operand:V8SF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], 8*i);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], 8*i);
        emit_insn (gen_kvx_fsrecwp (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)

(define_expand "kvx_fsrsrwo"
  [(match_operand:V8SF 0 "register_operand" "")
   (match_operand:V8SF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        emit_insn (gen_kvx_fsrsrwp (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)

(define_insn_and_split "kvx_fconjwcq"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")] UNSPEC_FCONJWCQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)] UNSPEC_FCONJWCP))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)] UNSPEC_FCONJWCP))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)


;; V4DF

(define_insn_and_split "*fcompdq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (match_operator:V4DI 1 "float_comparison_operator"
         [(match_operand:V4DF 2 "register_operand" "r")
          (match_operand:V4DF 3 "register_operand" "r")]))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (match_op_dup:V2DI 1
         [(subreg:V2DF (match_dup 2) 0)
          (subreg:V2DF (match_dup 3) 0)]))
   (set (subreg:V2DI (match_dup 0) 16)
        (match_op_dup:V2DI 1
         [(subreg:V2DF (match_dup 2) 16)
          (subreg:V2DF (match_dup 3) 16)]))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*selectfdq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (if_then_else:V4DF (match_operator 2 "zero_comparison_operator"
                                             [(match_operand:V4DI 3 "register_operand" "r")
                                              (match_operand:V4DI 5 "const_zero_operand" "")])
                           (match_operand:V4DF 1 "register_operand" "r")
                           (match_operand:V4DF 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (if_then_else:V2DF (match_op_dup 2 [(subreg:V2DI (match_dup 3) 0)
                                            (const_vector:V2DI [
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V2DF (match_dup 1) 0)
                           (subreg:V2DF (match_dup 4) 0)))
   (set (subreg:V2DF (match_dup 0) 16)
        (if_then_else:V2DF (match_op_dup 2 [(subreg:V2DI (match_dup 3) 16)
                                            (const_vector:V2DI [
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V2DF (match_dup 1) 16)
                           (subreg:V2DF (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*selectfdq_nez"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (if_then_else:V4DF (ne (match_operator:V4DI 2 "zero_comparison_operator"
                                             [(match_operand:V4DI 3 "register_operand" "r")
                                              (match_operand:V4DI 5 "const_zero_operand" "")])
                               (match_operand:V4DI 6 "const_zero_operand" ""))
                           (match_operand:V4DF 1 "register_operand" "r")
                           (match_operand:V4DF 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (if_then_else:V2DF (match_op_dup 2 [(subreg:V2DI (match_dup 3) 0)
                                            (const_vector:V2DI [
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V2DF (match_dup 1) 0)
                           (subreg:V2DF (match_dup 4) 0)))
   (set (subreg:V2DF (match_dup 0) 16)
        (if_then_else:V2DF (match_op_dup 2 [(subreg:V2DI (match_dup 3) 16)
                                            (const_vector:V2DI [
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V2DF (match_dup 1) 16)
                           (subreg:V2DF (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "kvx_selectfdq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "0")
                      (match_operand:V4DI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_SELECTFDQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 0)
                      (subreg:V2DF (match_dup 2) 0)
                      (subreg:V2DI (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_SELECTFDP))
   (set (subreg:V2DF (match_dup 0) 16)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 16)
                      (subreg:V2DF (match_dup 2) 16)
                      (subreg:V2DI (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_SELECTFDP))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "addv4df3"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (plus:V4DF (match_operand:V4DF 1 "register_operand" "r")
                   (match_operand:V4DF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (plus:V2DF (subreg:V2DF (match_dup 1) 0)
                   (subreg:V2DF (match_dup 2) 0)))
   (set (subreg:V2DF (match_dup 0) 16)
        (plus:V2DF (subreg:V2DF (match_dup 1) 16)
                   (subreg:V2DF (match_dup 2) 16)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fadddq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDDQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 0)
                      (subreg:V2DF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADDDP))
   (set (subreg:V2DF (match_dup 0) 16)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 16)
                      (subreg:V2DF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADDDP))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "subv4df3"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (minus:V4DF (match_operand:V4DF 1 "register_operand" "r")
                    (match_operand:V4DF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (minus:V2DF (subreg:V2DF (match_dup 1) 0)
                    (subreg:V2DF (match_dup 2) 0)))
   (set (subreg:V2DF (match_dup 0) 16)
        (minus:V2DF (subreg:V2DF (match_dup 1) 16)
                    (subreg:V2DF (match_dup 2) 16)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fsbfdq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFDQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 0)
                      (subreg:V2DF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBFDP))
   (set (subreg:V2DF (match_dup 0) 16)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 16)
                      (subreg:V2DF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBFDP))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "mulv4df3"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (mult:V4DF (match_operand:V4DF 1 "register_operand" "r")
                   (match_operand:V4DF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (mult:DF (subreg:DF (match_dup 1) 0)
                 (subreg:DF (match_dup 2) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (mult:DF (subreg:DF (match_dup 1) 8)
                 (subreg:DF (match_dup 2) 8)))
   (set (subreg:DF (match_dup 0) 16)
        (mult:DF (subreg:DF (match_dup 1) 16)
                 (subreg:DF (match_dup 2) 16)))
   (set (subreg:DF (match_dup 0) 24)
        (mult:DF (subreg:DF (match_dup 1) 24)
                 (subreg:DF (match_dup 2) 24)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fmuldq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULDQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DF (match_dup 1) 0)
                    (subreg:DF (match_dup 2) 0)
                    (match_dup 3)] UNSPEC_FMULD))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DF (match_dup 1) 8)
                    (subreg:DF (match_dup 2) 8)
                    (match_dup 3)] UNSPEC_FMULD))
   (set (subreg:DF (match_dup 0) 16)
        (unspec:DF [(subreg:DF (match_dup 1) 16)
                    (subreg:DF (match_dup 2) 16)
                    (match_dup 3)] UNSPEC_FMULD))
   (set (subreg:DF (match_dup 0) 24)
        (unspec:DF [(subreg:DF (match_dup 1) 24)
                    (subreg:DF (match_dup 2) 24)
                    (match_dup 3)] UNSPEC_FMULD))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_fmuldcp"
  [(set (match_operand:V4DF 0 "register_operand" "")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "")
                      (match_operand:V4DF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMULDCP))]
  ""
  {
    rtx modifiers = operands[3];
    const char *xstr = XSTR (modifiers, 0);
    bool conjugate = xstr && xstr[0] == '.' && xstr[1] == 'c';
    if (conjugate)
      modifiers = gen_rtx_CONST_STRING (VOIDmode, xstr + 2);
    for (int i = 0; i < 2; i++)
      {
        rtx real_0 = gen_rtx_SUBREG (DFmode, operands[0], i*16+0);
        rtx imag_0 = gen_rtx_SUBREG (DFmode, operands[0], i*16+8);
        rtx real_1 = gen_rtx_SUBREG (DFmode, operands[1], i*16+0);
        rtx imag_1 = gen_rtx_SUBREG (DFmode, operands[1], i*16+8);
        rtx real_2 = gen_rtx_SUBREG (DFmode, operands[2], i*16+0);
        rtx imag_2 = gen_rtx_SUBREG (DFmode, operands[2], i*16+8);
        rtx real_t = gen_reg_rtx (DFmode), imag_t = gen_reg_rtx (DFmode);
        if (conjugate)
          {
            emit_insn (gen_kvx_fmuld (real_t, real_1, real_2, modifiers));
            emit_insn (gen_kvx_ffmad (real_0, imag_1, imag_2, real_t, modifiers));
            emit_insn (gen_kvx_fmuld (imag_t, real_1, imag_2, modifiers));
            emit_insn (gen_kvx_ffmsd (imag_0, real_2, imag_1, imag_t, modifiers));
          }
        else
          {
            emit_insn (gen_kvx_fmuld (real_t, real_1, real_2, modifiers));
            emit_insn (gen_kvx_ffmsd (real_0, imag_1, imag_2, real_t, modifiers));
            emit_insn (gen_kvx_fmuld (imag_t, real_1, imag_2, modifiers));
            emit_insn (gen_kvx_ffmad (imag_0, real_2, imag_1, imag_t, modifiers));
          }
      }
    DONE;
  }
)

(define_expand "kvx_ffmadcp"
  [(set (match_operand:V4DF 0 "register_operand")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand")
                      (match_operand:V4DF 2 "register_operand")
                      (match_operand:V4DF 3 "register_operand")
                      (match_operand 4 "" "")] UNSPEC_FFMADCP))]
  ""
  {
    rtx product = gen_reg_rtx (V4DFmode);
    emit_insn (gen_kvx_fmuldcp (product, operands[2], operands[1], operands[4]));
    emit_insn (gen_kvx_fadddq (operands[0], product, operands[3], operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffmsdcp"
  [(set (match_operand:V4DF 0 "register_operand")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand")
                      (match_operand:V4DF 2 "register_operand")
                      (match_operand:V4DF 3 "register_operand")
                      (match_operand 4 "" "")] UNSPEC_FFMSDCP))]
  ""
  {
    rtx product = gen_reg_rtx (V4DFmode);
    emit_insn (gen_kvx_fmuldcp (product, operands[2], operands[1], operands[4]));
    emit_insn (gen_kvx_fsbfdq (operands[0], product, operands[3], operands[4]));
    DONE;
  }
)

(define_insn_and_split "fmav4df4"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (fma:V4DF (match_operand:V4DF 1 "register_operand" "r")
                  (match_operand:V4DF 2 "register_operand" "r")
                  (match_operand:V4DF 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (fma:DF  (subreg:DF (match_dup 1) 0)
                 (subreg:DF (match_dup 2) 0)
                 (subreg:DF (match_dup 3) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (fma:DF  (subreg:DF (match_dup 1) 8)
                 (subreg:DF (match_dup 2) 8)
                 (subreg:DF (match_dup 3) 8)))
   (set (subreg:DF (match_dup 0) 16)
        (fma:DF  (subreg:DF (match_dup 1) 16)
                 (subreg:DF (match_dup 2) 16)
                 (subreg:DF (match_dup 3) 16)))
   (set (subreg:DF (match_dup 0) 24)
        (fma:DF  (subreg:DF (match_dup 1) 24)
                 (subreg:DF (match_dup 2) 24)
                 (subreg:DF (match_dup 3) 24)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_ffmadq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand:V4DF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMADQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DF (match_dup 1) 0)
                    (subreg:DF (match_dup 2) 0)
                    (subreg:DF (match_dup 3) 0)
                    (match_dup 4)] UNSPEC_FFMAD))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DF (match_dup 1) 8)
                    (subreg:DF (match_dup 2) 8)
                    (subreg:DF (match_dup 3) 8)
                    (match_dup 4)] UNSPEC_FFMAD))
   (set (subreg:DF (match_dup 0) 16)
        (unspec:DF [(subreg:DF (match_dup 1) 16)
                    (subreg:DF (match_dup 2) 16)
                    (subreg:DF (match_dup 3) 16)
                    (match_dup 4)] UNSPEC_FFMAD))
   (set (subreg:DF (match_dup 0) 24)
        (unspec:DF [(subreg:DF (match_dup 1) 24)
                    (subreg:DF (match_dup 2) 24)
                    (subreg:DF (match_dup 3) 24)
                    (match_dup 4)] UNSPEC_FFMAD))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "fnmav4df4"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (fma:V4DF (neg:V4DF (match_operand:V4DF 1 "register_operand" "r"))
                  (match_operand:V4DF 2 "register_operand" "r")
                  (match_operand:V4DF 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (fma:DF  (neg:DF (subreg:DF (match_dup 1) 0))
                 (subreg:DF (match_dup 2) 0)
                 (subreg:DF (match_dup 3) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (fma:DF  (neg:DF (subreg:DF (match_dup 1) 8))
                 (subreg:DF (match_dup 2) 8)
                 (subreg:DF (match_dup 3) 8)))
   (set (subreg:DF (match_dup 0) 16)
        (fma:DF  (neg:DF (subreg:DF (match_dup 1) 16))
                 (subreg:DF (match_dup 2) 16)
                 (subreg:DF (match_dup 3) 16)))
   (set (subreg:DF (match_dup 0) 24)
        (fma:DF  (neg:DF (subreg:DF (match_dup 1) 24))
                 (subreg:DF (match_dup 2) 24)
                 (subreg:DF (match_dup 3) 24)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_ffmsdq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand:V4DF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSDQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DF (match_dup 1) 0)
                    (subreg:DF (match_dup 2) 0)
                    (subreg:DF (match_dup 3) 0)
                    (match_dup 4)] UNSPEC_FFMSD))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DF (match_dup 1) 8)
                    (subreg:DF (match_dup 2) 8)
                    (subreg:DF (match_dup 3) 8)
                    (match_dup 4)] UNSPEC_FFMSD))
   (set (subreg:DF (match_dup 0) 16)
        (unspec:DF [(subreg:DF (match_dup 1) 16)
                    (subreg:DF (match_dup 2) 16)
                    (subreg:DF (match_dup 3) 16)
                    (match_dup 4)] UNSPEC_FFMSD))
   (set (subreg:DF (match_dup 0) 24)
        (unspec:DF [(subreg:DF (match_dup 1) 24)
                    (subreg:DF (match_dup 2) 24)
                    (subreg:DF (match_dup 3) 24)
                    (match_dup 4)] UNSPEC_FFMSD))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "fminv4df3"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (smin:V4DF (match_operand:V4DF 1 "register_operand" "r")
                   (match_operand:V4DF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (smin:V2DF (subreg:V2DF (match_dup 1) 0)
                   (subreg:V2DF (match_dup 2) 0)))
   (set (subreg:V2DF (match_dup 0) 16)
        (smin:V2DF (subreg:V2DF (match_dup 1) 16)
                   (subreg:V2DF (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "fmaxv4df3"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (smax:V4DF (match_operand:V4DF 1 "register_operand" "r")
                   (match_operand:V4DF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (smax:V2DF (subreg:V2DF (match_dup 1) 0)
                   (subreg:V2DF (match_dup 2) 0)))
   (set (subreg:V2DF (match_dup 0) 16)
        (smax:V2DF (subreg:V2DF (match_dup 1) 16)
                   (subreg:V2DF (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "negv4df2"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (neg:V4DF (match_operand:V4DF 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (neg:V2DF (subreg:V2DF (match_dup 1) 0)))
   (set (subreg:V2DF (match_dup 0) 16)
        (neg:V2DF (subreg:V2DF (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "absv4df2"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (abs:V4DF (match_operand:V4DF 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (abs:V2DF (subreg:V2DF (match_dup 1) 0)))
   (set (subreg:V2DF (match_dup 0) 16)
        (abs:V2DF (subreg:V2DF (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_expand "copysignv4df3"
  [(match_operand:V4DF 0 "register_operand")
   (match_operand:V4DF 1 "register_operand")
   (match_operand:V4DF 2 "register_operand")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2DFmode, operands[0], i*16);
        rtx opnd1 = gen_rtx_SUBREG (V2DFmode, operands[1], i*16);
        rtx opnd2 = gen_rtx_SUBREG (V2DFmode, operands[2], i*16);
        emit_insn (gen_copysignv2df3 (opnd0, opnd1, opnd2));
      }
    DONE;
  }
)

(define_insn_and_split "floatv4div4df2"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (float:V4DF (match_operand:V4DI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (float:DF (subreg:DI (match_dup 1) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (float:DF (subreg:DI (match_dup 1) 8)))
   (set (subreg:DF (match_dup 0) 16)
        (float:DF (subreg:DI (match_dup 1) 16)))
   (set (subreg:DF (match_dup 0) 24)
        (float:DF (subreg:DI (match_dup 1) 24)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatdq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATDQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DI (match_dup 1) 0)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FLOATD))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DI (match_dup 1) 8)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FLOATD))
   (set (subreg:DF (match_dup 0) 16)
        (unspec:DF [(subreg:DI (match_dup 1) 16)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FLOATD))
   (set (subreg:DF (match_dup 0) 24)
        (unspec:DF [(subreg:DI (match_dup 1) 24)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FLOATD))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "floatunsv4div4df2"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unsigned_float:V4DF (match_operand:V4DI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unsigned_float:DF (subreg:DI (match_dup 1) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (unsigned_float:DF (subreg:DI (match_dup 1) 8)))
   (set (subreg:DF (match_dup 0) 16)
        (unsigned_float:DF (subreg:DI (match_dup 1) 16)))
   (set (subreg:DF (match_dup 0) 24)
        (unsigned_float:DF (subreg:DI (match_dup 1) 24)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatudq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATUDQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DI (match_dup 1) 0)
                    (match_dup 2)
                    (match_operand 3 "" "")] UNSPEC_FLOATUD))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DI (match_dup 1) 8)
                    (match_dup 2)
                    (match_operand 3 "" "")] UNSPEC_FLOATUD))
   (set (subreg:DF (match_dup 0) 16)
        (unspec:DF [(subreg:DI (match_dup 1) 16)
                    (match_dup 2)
                    (match_operand 3 "" "")] UNSPEC_FLOATUD))
   (set (subreg:DF (match_dup 0) 24)
        (unspec:DF [(subreg:DI (match_dup 1) 24)
                    (match_dup 2)
                    (match_operand 3 "" "")] UNSPEC_FLOATUD))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "fix_truncv4dfv4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (fix:V4DI (match_operand:V4DF 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (fix:DI (subreg:DF (match_dup 1) 0)))
   (set (subreg:DI (match_dup 0) 8)
        (fix:DI (subreg:DF (match_dup 1) 8)))
   (set (subreg:DI (match_dup 0) 16)
        (fix:DI (subreg:DF (match_dup 1) 16)))
   (set (subreg:DI (match_dup 0) 24)
        (fix:DI (subreg:DF (match_dup 1) 24)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixeddq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDDQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (unspec:DI [(subreg:DF (match_dup 1) 0)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDD))
   (set (subreg:DI (match_dup 0) 8)
        (unspec:DI [(subreg:DF (match_dup 1) 8)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDD))
   (set (subreg:DI (match_dup 0) 16)
        (unspec:DI [(subreg:DF (match_dup 1) 16)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDD))
   (set (subreg:DI (match_dup 0) 24)
        (unspec:DI [(subreg:DF (match_dup 1) 24)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDD))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "fixuns_truncv4dfv4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unsigned_fix:V4DI (match_operand:V4DF 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (unsigned_fix:DI (subreg:DF (match_dup 1) 0)))
   (set (subreg:DI (match_dup 0) 8)
        (unsigned_fix:DI (subreg:DF (match_dup 1) 8)))
   (set (subreg:DI (match_dup 0) 16)
        (unsigned_fix:DI (subreg:DF (match_dup 1) 16)))
   (set (subreg:DI (match_dup 0) 24)
        (unsigned_fix:DI (subreg:DF (match_dup 1) 24)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixedudq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDUDQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (unspec:DI [(subreg:DF (match_dup 1) 0)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDUD))
   (set (subreg:DI (match_dup 0) 8)
        (unspec:DI [(subreg:DF (match_dup 1) 8)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDUD))
   (set (subreg:DI (match_dup 0) 16)
        (unspec:DI [(subreg:DF (match_dup 1) 16)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDUD))
   (set (subreg:DI (match_dup 0) 24)
        (unspec:DI [(subreg:DF (match_dup 1) 24)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDUD))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_fcdivdq"
  [(match_operand:V4DF 0 "register_operand" "")
   (match_operand:V4DF 1 "register_operand" "")
   (match_operand:V4DF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (DFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (DFmode, operands[1], i*8);
        rtx opnd2 = gen_rtx_SUBREG (DFmode, operands[2], i*8);
        emit_insn (gen_kvx_fcdivd (opnd0, opnd1, opnd2, operands[3]));
      }
    DONE;
  }
)

(define_expand "kvx_fsdivdq"
  [(match_operand:V4DF 0 "register_operand" "")
   (match_operand:V4DF 1 "register_operand" "")
   (match_operand:V4DF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (DFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (DFmode, operands[1], i*8);
        rtx opnd2 = gen_rtx_SUBREG (DFmode, operands[2], i*8);
        emit_insn (gen_kvx_fsdivd (opnd0, opnd1, opnd2, operands[3]));
      }
    DONE;
  }
)

(define_expand "kvx_fsrecdq"
  [(match_operand:V4DF 0 "register_operand" "")
   (match_operand:V4DF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (DFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (DFmode, operands[1], i*8);
        emit_insn (gen_kvx_fsrecd (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)

(define_expand "kvx_fsrsrdq"
  [(match_operand:V4DF 0 "register_operand" "")
   (match_operand:V4DF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (DFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (DFmode, operands[1], i*8);
        emit_insn (gen_kvx_fsrsrd (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)

(define_insn_and_split "kvx_fmindqs"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (smin:V4DF (match_operand:V4DF 1 "register_operand" "r")
                   (match_operand:DF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (smin:V2DF (subreg:V2DF (match_dup 1) 0)
                   (match_dup 2)))
   (set (subreg:V2DF (match_dup 0) 16)
        (smin:V2DF (subreg:V2DF (match_dup 1) 16)
                   (match_dup 2)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "kvx_fmaxdqs"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (smax:V4DF (match_operand:V4DF 1 "register_operand" "r")
                   (match_operand:DF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (smax:V2DF (subreg:V2DF (match_dup 1) 0)
                   (match_dup 2)))
   (set (subreg:V2DF (match_dup 0) 16)
        (smax:V2DF (subreg:V2DF (match_dup 1) 16)
                   (match_dup 2)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "kvx_fconjdcp"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")] UNSPEC_FCONJDC))]
  ""
  "copyd %x0 = %x1\n\tfnegd %y0 = %y1\n\tcopyd %z0 = %z1\n\tfnegd %t0 = %t1"
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; W256A

(define_expand "kvx_shift<lsvs>"
  [(match_operand:W256A 0 "register_operand" "")
   (match_operand:W256A 1 "register_operand" "")
   (match_operand:SI 2 "register_operand" "")
   (match_operand:<INNER> 3 "nonmemory_operand" "")]
  ""
  {
    int shift = INTVAL (operands[2]) * GET_MODE_BITSIZE (<INNER>mode);
    rtx opnd0_0 = gen_rtx_SUBREG (<INNER>mode, operands[0], 0);
    rtx opnd0_1 = gen_rtx_SUBREG (<INNER>mode, operands[0], 8);
    rtx opnd0_2 = gen_rtx_SUBREG (<INNER>mode, operands[0], 16);
    rtx opnd0_3 = gen_rtx_SUBREG (<INNER>mode, operands[0], 24);
    rtx opnd1_1 = gen_rtx_SUBREG (<INNER>mode, operands[1], 8);
    rtx opnd1_2 = gen_rtx_SUBREG (<INNER>mode, operands[1], 16);
    rtx opnd1_3 = gen_rtx_SUBREG (<INNER>mode, operands[1], 24);
    rtx filler = operands[3];
    if (shift == 0)
      {
        emit_move_insn (operands[0], operands[1]);
      }
    else if (shift  == 64)
      {
        emit_move_insn (opnd0_0, opnd1_1);
        emit_move_insn (opnd0_1, opnd1_2);
        emit_move_insn (opnd0_2, opnd1_3);
        emit_move_insn (opnd0_3, filler);
      }
    else if (shift == 128)
      {
        emit_move_insn (opnd0_0, opnd1_2);
        emit_move_insn (opnd0_1, opnd1_3);
        emit_move_insn (opnd0_2, filler);
        emit_move_insn (opnd0_3, filler);
      }
    else if (shift == 192)
      {
        emit_move_insn (opnd0_0, opnd1_3);
        emit_move_insn (opnd0_1, filler);
        emit_move_insn (opnd0_2, filler);
        emit_move_insn (opnd0_3, filler);
      }
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "kvx_cons<lsvs>"
  [(set (match_operand:W256A 0 "register_operand" "=r")
        (vec_concat:W256A (match_operand:<HALF> 1 "register_operand" "0")
                          (match_operand:<HALF> 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 16)
        (match_dup 2))]
  ""
)


;; V2DF

(define_insn_and_split "*fcompndp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "float_comparison_operator"
         [(match_operand:V2DF 2 "register_operand" "r")
          (match_operand:V2DF 3 "register_operand" "r")]))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (unspec:V2DI [(match_op_dup 1 [(match_dup 2)
                                       (match_dup 3)])] UNSPEC_COMP128))
   (set (match_dup 0)
        (neg:V2DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*fcompdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operator:V2DI 1 "float_comparison_operator"
                       [(match_operand:V2DF 2 "register_operand" "r")
                        (match_operand:V2DF 3 "register_operand" "r")])] UNSPEC_COMP128))]
  ""
  "fcompd.%f1 %x0 = %x2, %x3\n\tfcompd.%f1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*selectfdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (if_then_else:V2DF (match_operator 2 "zero_comparison_operator"
                                             [(match_operand:V2DI 3 "register_operand" "r")
                                              (match_operand:V2DI 5 "const_zero_operand" "")])
                           (match_operand:V2DF 1 "register_operand" "r")
                           (match_operand:V2DF 4 "register_operand" "0")))]
  ""
  "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*selectfdp_nez"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (if_then_else:V2DF (ne (match_operator:V2DI 2 "zero_comparison_operator"
                                             [(match_operand:V2DI 3 "register_operand" "r")
                                              (match_operand:V2DI 5 "const_zero_operand" "")])
                               (match_operand:V2DI 6 "const_zero_operand" ""))
                           (match_operand:V2DF 1 "register_operand" "r")
                           (match_operand:V2DF 4 "register_operand" "0")))]
  ""
  "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_selectfdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "0")
                      (match_operand:V2DI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_SELECTFDP))]
  ""
  "cmoved%4 %x3? %x0 = %x1\n\tcmoved%4 %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "addv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (plus:V2DF (match_operand:V2DF 1 "register_operand" "r")
                   (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "fadddp %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_fadddp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDDP))]
  ""
  "fadddp%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "subv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (minus:V2DF (match_operand:V2DF 1 "register_operand" "r")
                    (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "fsbfdp %0 = %2, %1"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_fsbfdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFDP))]
  ""
  "fsbfdp%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "mulv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (mult:V2DF (match_operand:V2DF 1 "register_operand" "r")
                   (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (mult:DF (subreg:DF (match_dup 1) 0)
                 (subreg:DF (match_dup 2) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (mult:DF (subreg:DF (match_dup 1) 8)
                 (subreg:DF (match_dup 2) 8)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fmuldp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULDP))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DF (match_dup 1) 0)
                    (subreg:DF (match_dup 2) 0)
                    (match_dup 3)] UNSPEC_FMULD))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DF (match_dup 1) 8)
                    (subreg:DF (match_dup 2) 8)
                    (match_dup 3)] UNSPEC_FMULD))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_fmuldc"
  [(set (match_operand:V2DF 0 "register_operand")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand")
                      (match_operand:V2DF 2 "register_operand")
                      (match_operand 3 "" "")] UNSPEC_FMULDC))]
  ""
  {
    rtx modifiers = operands[3];
    const char *xstr = XSTR (modifiers, 0);
    bool conjugate = xstr && xstr[0] == '.' && xstr[1] == 'c';
    if (conjugate)
      modifiers = gen_rtx_CONST_STRING (VOIDmode, xstr + 2);
    rtx real_0 = gen_rtx_SUBREG (DFmode, operands[0], 0);
    rtx imag_0 = gen_rtx_SUBREG (DFmode, operands[0], 8);
    rtx real_1 = gen_rtx_SUBREG (DFmode, operands[1], 0);
    rtx imag_1 = gen_rtx_SUBREG (DFmode, operands[1], 8);
    rtx real_2 = gen_rtx_SUBREG (DFmode, operands[2], 0);
    rtx imag_2 = gen_rtx_SUBREG (DFmode, operands[2], 8);
    rtx real_t = gen_reg_rtx (DFmode), imag_t = gen_reg_rtx (DFmode);
    if (conjugate)
      {
        emit_insn (gen_kvx_fmuld (real_t, real_1, real_2, modifiers));
        emit_insn (gen_kvx_ffmad (real_0, imag_1, imag_2, real_t, modifiers));
        emit_insn (gen_kvx_fmuld (imag_t, real_1, imag_2, modifiers));
        emit_insn (gen_kvx_ffmsd (imag_0, real_2, imag_1, imag_t, modifiers));
      }
    else
      {
        emit_insn (gen_kvx_fmuld (real_t, real_1, real_2, modifiers));
        emit_insn (gen_kvx_ffmsd (real_0, imag_1, imag_2, real_t, modifiers));
        emit_insn (gen_kvx_fmuld (imag_t, real_1, imag_2, modifiers));
        emit_insn (gen_kvx_ffmad (imag_0, real_2, imag_1, imag_t, modifiers));
      }
    DONE;
  }
)

(define_expand "kvx_ffmadc"
  [(set (match_operand:V2DF 0 "register_operand")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand")
                      (match_operand:V2DF 2 "register_operand")
                      (match_operand:V2DF 3 "register_operand")
                      (match_operand 4 "" "")] UNSPEC_FFMADC))]
  ""
  {
    rtx product = gen_reg_rtx (V2DFmode);
    emit_insn (gen_kvx_fmuldc (product, operands[2], operands[1], operands[4]));
    emit_insn (gen_kvx_fadddp (operands[0], product, operands[3], operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffmsdc"
  [(set (match_operand:V2DF 0 "register_operand")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand")
                      (match_operand:V2DF 2 "register_operand")
                      (match_operand:V2DF 3 "register_operand")
                      (match_operand 4 "" "")] UNSPEC_FFMSDC))]
  ""
  {
    rtx product = gen_reg_rtx (V2DFmode);
    emit_insn (gen_kvx_fmuldc (product, operands[2], operands[1], operands[4]));
    emit_insn (gen_kvx_fsbfdp (operands[0], product, operands[3], operands[4]));
    DONE;
  }
)

(define_insn_and_split "fmav2df4"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (fma:V2DF (match_operand:V2DF 1 "register_operand" "r")
                  (match_operand:V2DF 2 "register_operand" "r")
                  (match_operand:V2DF 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (fma:DF  (subreg:DF (match_dup 1) 0)
                 (subreg:DF (match_dup 2) 0)
                 (subreg:DF (match_dup 3) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (fma:DF  (subreg:DF (match_dup 1) 8)
                 (subreg:DF (match_dup 2) 8)
                 (subreg:DF (match_dup 3) 8)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_ffmadp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand:V2DF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMADP))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DF (match_dup 1) 0)
                    (subreg:DF (match_dup 2) 0)
                    (subreg:DF (match_dup 3) 0)
                    (match_dup 4)] UNSPEC_FFMAD))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DF (match_dup 1) 8)
                    (subreg:DF (match_dup 2) 8)
                    (subreg:DF (match_dup 3) 8)
                    (match_dup 4)] UNSPEC_FFMAD))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "fnmav2df4"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (fma:V2DF (neg:V2DF (match_operand:V2DF 1 "register_operand" "r"))
                  (match_operand:V2DF 2 "register_operand" "r")
                  (match_operand:V2DF 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (fma:DF  (neg:DF (subreg:DF (match_dup 1) 0))
                 (subreg:DF (match_dup 2) 0)
                 (subreg:DF (match_dup 3) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (fma:DF  (neg:DF (subreg:DF (match_dup 1) 8))
                 (subreg:DF (match_dup 2) 8)
                 (subreg:DF (match_dup 3) 8)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_ffmsdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand:V2DF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSDP))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DF (match_dup 1) 0)
                    (subreg:DF (match_dup 2) 0)
                    (subreg:DF (match_dup 3) 0)
                    (match_dup 4)] UNSPEC_FFMSD))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DF (match_dup 1) 8)
                    (subreg:DF (match_dup 2) 8)
                    (subreg:DF (match_dup 3) 8)
                    (match_dup 4)] UNSPEC_FFMSD))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "fminv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (smin:V2DF (match_operand:V2DF 1 "register_operand" "r")
                   (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "fmind %x0 = %x1, %x2\n\tfmind %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "fmaxv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (smax:V2DF (match_operand:V2DF 1 "register_operand" "r")
                   (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "fmaxd %x0 = %x1, %x2\n\tfmaxd %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "negv2df2"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (neg:V2DF (match_operand:V2DF 1 "register_operand" "r")))]
  ""
  "fnegd %x0 = %x1\n\tfnegd %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "absv2df2"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (abs:V2DF (match_operand:V2DF 1 "register_operand" "r")))]
  ""
  "fabsd %x0 = %x1\n\tfabsd %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "copysignv2df3"
  [(match_operand:V2DF 0 "register_operand")
   (match_operand:V2DF 1 "register_operand")
   (match_operand:V2DF 2 "register_operand")]
  ""
  {
    rtx fabs1 = gen_reg_rtx (V2DFmode);
    emit_insn (gen_absv2df2 (fabs1, operands[1]));
    rtx fneg1 = gen_reg_rtx (V2DFmode);
    emit_insn (gen_negv2df2 (fneg1, fabs1));
    rtx sign2 = gen_reg_rtx (V2DImode);
    convert_move (sign2, operands[2], 0);
    rtx ltz = gen_rtx_CONST_STRING (VOIDmode, ".dltz");
    emit_insn (gen_kvx_selectfdp (operands[0], fneg1, fabs1, sign2, ltz));
    DONE;
  }
)

(define_insn_and_split "floatv2div2df2"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (float:V2DF (match_operand:V2DI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (float:DF (subreg:DI (match_dup 1) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (float:DF (subreg:DI (match_dup 1) 8)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATDP))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DI (match_dup 1) 0)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FLOATD))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DI (match_dup 1) 8)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FLOATD))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "floatunsv2div2df2"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unsigned_float:V2DF (match_operand:V2DI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unsigned_float:DF (subreg:DI (match_dup 1) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (unsigned_float:DF (subreg:DI (match_dup 1) 8)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatudp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATUDP))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DI (match_dup 1) 0)
                    (match_dup 2)
                    (match_operand 3 "" "")] UNSPEC_FLOATUD))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DI (match_dup 1) 8)
                    (match_dup 2)
                    (match_operand 3 "" "")] UNSPEC_FLOATUD))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "fix_truncv2dfv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (fix:V2DI (match_operand:V2DF 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (fix:DI (subreg:DF (match_dup 1) 0)))
   (set (subreg:DI (match_dup 0) 8)
        (fix:DI (subreg:DF (match_dup 1) 8)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixeddp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDDP))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (unspec:DI [(subreg:DF (match_dup 1) 0)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDD))
   (set (subreg:DI (match_dup 0) 8)
        (unspec:DI [(subreg:DF (match_dup 1) 8)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDD))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "fixuns_truncv2dfv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unsigned_fix:V2DI (match_operand:V2DF 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (unsigned_fix:DI (subreg:DF (match_dup 1) 0)))
   (set (subreg:DI (match_dup 0) 8)
        (unsigned_fix:DI (subreg:DF (match_dup 1) 8)))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixedudp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDUDP))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (unspec:DI [(subreg:DF (match_dup 1) 0)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDUD))
   (set (subreg:DI (match_dup 0) 8)
        (unspec:DI [(subreg:DF (match_dup 1) 8)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDUD))]
  ""
  [(set_attr "type" "mau_fpu")]
)


(define_expand "kvx_fcdivdp"
  [(match_operand:V2DF 0 "register_operand" "")
   (match_operand:V2DF 1 "register_operand" "")
   (match_operand:V2DF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (DFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (DFmode, operands[1], i*8);
        rtx opnd2 = gen_rtx_SUBREG (DFmode, operands[2], i*8);
        emit_insn (gen_kvx_fcdivd (opnd0, opnd1, opnd2, operands[3]));
      }
    DONE;
  }
)

(define_expand "kvx_fsdivdp"
  [(match_operand:V2DF 0 "register_operand" "")
   (match_operand:V2DF 1 "register_operand" "")
   (match_operand:V2DF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (DFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (DFmode, operands[1], i*8);
        rtx opnd2 = gen_rtx_SUBREG (DFmode, operands[2], i*8);
        emit_insn (gen_kvx_fsdivd (opnd0, opnd1, opnd2, operands[3]));
      }
    DONE;
  }
)

(define_expand "kvx_fsrecdp"
  [(match_operand:V2DF 0 "register_operand" "")
   (match_operand:V2DF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (DFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (DFmode, operands[1], i*8);
        emit_insn (gen_kvx_fsrecd (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)

(define_expand "kvx_fsrsrdp"
  [(match_operand:V2DF 0 "register_operand" "")
   (match_operand:V2DF 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (DFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (DFmode, operands[1], i*8);
        emit_insn (gen_kvx_fsrsrd (opnd0, opnd1, operands[2]));
      }
    DONE;
  }
)

(define_insn "kvx_fmindps"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (smin:V2DF (match_operand:V2DF 1 "register_operand" "r")
                   (match_operand:DF 2 "register_operand" "r")))]
  ""
  "fmind %x0 = %x1, %2\n\tfmind %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_fmaxdps"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (smax:V2DF (match_operand:V2DF 1 "register_operand" "r")
                   (match_operand:DF 2 "register_operand" "r")))]
  ""
  "fmaxd %x0 = %x1, %2\n\tfmaxd %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_fconjdc"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")] UNSPEC_FCONJDC))]
  ""
  "copyd %x0 = %x1\n\tfnegd %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)


;; KVX_LD, KVX_SD

(define_insn "kvx_ld<SIMD64:lsvs>"
  [(set (match_operand:SIMD64 0 "register_operand" "=r,r,r")
        (unspec:SIMD64 [(match_operand:SIMD64 1 "memory_operand" "a,b,m")
                        (match_operand 2 "" "")] UNSPEC_LD))
   (use (match_dup 1))]
  ""
  "ld%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_sd<SIMD64:lsvs>"
  [(unspec_volatile:SIMD64 [(match_operand:SIMD64 0 "memory_operand" "a,b,m")
                            (match_operand:SIMD64 1 "register_operand" "r,r,r")] UNSPEC_SD)
   (clobber (match_dup 0))]
  ""
  "sd%m0 %0 = %1"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

;; KVX_LQ, KVX_SQ

(define_insn "kvx_lq<SIMD128:lsvs>"
  [(set (match_operand:SIMD128 0 "register_operand" "=r,r,r")
        (unspec:SIMD128 [(match_operand:SIMD128 1 "memory_operand" "a,b,m")
                         (match_operand 2 "" "")] UNSPEC_LQ))
   (use (match_dup 1))]
  ""
  "lq%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_sq<SIMD128:lsvs>"
  [(unspec_volatile:SIMD128 [(match_operand:SIMD128 0 "memory_operand" "a,b,m")
                             (match_operand:SIMD128 1 "register_operand" "r,r,r")] UNSPEC_SQ)
   (clobber (match_dup 0))]
  ""
  "sq%m0 %0 = %1"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

;; KVX_LO, KVX_SO

(define_insn "kvx_lo<SIMD256:lsvs>"
  [(set (match_operand:SIMD256 0 "register_operand" "=r,r,r")
        (unspec:SIMD256 [(match_operand:SIMD256 1 "memory_operand" "a,b,m")
                         (match_operand 2 "" "")] UNSPEC_LO))
   (use (match_dup 1))]
  ""
  "lo%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_so<SIMD256:lsvs>"
  [(unspec_volatile:SIMD256 [(match_operand:SIMD256 0 "memory_operand" "a,b,m")
                             (match_operand:SIMD256 1 "register_operand" "r,r,r")] UNSPEC_SO)
   (clobber (match_dup 0))]
  ""
  "so%m0 %0 = %1"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

;; KVX_LV, KVX_SV

(define_insn "kvx_lv<SIMD256:lsvs>"
  [(unspec_volatile:SIMD256 [(match_operand 0 "" "")
                             (match_operand:SIMD256 1 "memory_operand" "a,b,m")
                             (match_operand 2 "" "")] UNSPEC_LV)
   (use (match_dup 1))]
  ""
  "lv%2%m1 $%0 = %1"
  [(set_attr "type" "lsu_load_uncached,lsu_load_uncached_x,lsu_load_uncached_y")
   (set_attr "length"               "4,                  8,                 12")]
)

(define_insn "kvx_sv<SIMD256:lsvs>"
  [(unspec_volatile:SIMD256 [(match_operand:SIMD256 0 "memory_operand" "a,b,m")
                             (match_operand 1 "" "")] UNSPEC_SV)
   (clobber (match_dup 0))]
  ""
  "sv%m0 %0 = $%1"
  [(set_attr "type" "lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y")
   (set_attr "length"            "4,               8,              12")]
)


;; KVX_MOVETO, KVX_MOVEFO, KVX_SWAPVO

(define_insn "kvx_moveto<SIMD256:lsvs>"
  [(unspec_volatile:SIMD256 [(match_operand 0 "" "")
                             (match_operand:SIMD256 1 "register_operand" "r")] UNSPEC_MOVETO)]
  ""
  "movetq $%0.lo = %x1, %y1\n\tmovetq $%0.hi = %z1, %t1"
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "kvx_movefo<SIMD256:lsvs>"
  [(set (match_operand:SIMD256 0 "register_operand" "=r")
        (unspec_volatile:SIMD256 [(match_operand 1 "" "")] UNSPEC_MOVEFO))]
  ""
  "movefo %0 = $%1"
  [(set_attr "type" "bcu_tiny_auxw_crrp")]
)

(define_insn "kvx_swapvo<SIMD256:lsvs>"
  [(set (match_operand:SIMD256 0 "register_operand" "=r")
        (unspec_volatile:SIMD256 [(match_operand 1 "" "")
                                  (match_operand:SIMD256 2 "register_operand" "0")] UNSPEC_SWAPVO))]
  ""
  "movetq $%1.lo = %x0, %y0\n\tmovetq $%1.hi = %z0, %t0\n\tmovefo %0 = $%1"
  [(set_attr "type" "all")
   (set_attr "length" "12")]
)

;; KVX_ALIGNO, KVX_ALIGNV

(define_insn "kvx_aligno<SIMD256:lsvs>"
  [(set (match_operand:SIMD256 0 "register_operand" "=r,r")
        (unspec_volatile:SIMD256 [(match_operand 1 "" "")
                                  (match_operand 2 "" "")
                                  (match_operand:SI 3 "sat_shift_operand" "r,U06")] UNSPEC_ALIGNO))]
  ""
  "aligno %0 = $%1, $%2, %3"
  [(set_attr "type" "bcu_tiny_auxw_crrp,bcu_tiny_auxw_crrp")]
)

(define_insn "kvx_alignv"
  [(unspec_volatile [(match_operand 0 "" "")
                     (match_operand 1 "" "")
                     (match_operand 2 "" "")
                     (match_operand:SI 3 "sat_shift_operand" "r,U06")] UNSPEC_ALIGNV)]
  ""
  "alignv $%0 = $%1, $%2, %3"
  [(set_attr "type" "bcu_crrp_crwl_crwh,bcu_crrp_crwl_crwh")]
)


;; KVX_COPYV, KVX_MT4X4D

(define_insn "kvx_copyv"
  [(unspec_volatile [(match_operand 0 "" "")
                     (match_operand 1 "" "")] UNSPEC_COPYV)]
  ""
  "copyv $%0 = $%1"
  [(set_attr "type" "bcu_crrp_crwl_crwh")]
)

(define_insn "kvx_mt4x4d"
  [(unspec_volatile [(match_operand 0 "" "")
                     (match_operand 1 "" "")] UNSPEC_MT4X4D)]
  ""
  "mt4x4d $%0 = $%1"
  [(set_attr "type" "tca")]
)


;; KVX_MM4ABW, KVX_FMM4AHW*

(define_insn "kvx_mm4abw"
  [(unspec_volatile [(match_operand 0 "" "")
                     (match_operand 1 "" "")
                     (match_operand 2 "" "")
                     (match_operand 3 "" "")] UNSPEC_MM4ABW)]
  ""
  "mm4abw $%0 = $%1, $%2, $%3"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_fmm4ahw0"
  [(unspec_volatile [(match_operand 0 "" "")
                     (match_operand 1 "" "")
                     (match_operand 2 "" "")
                     (match_operand 3 "" "")] UNSPEC_FMM4AHW0)]
  ""
  "fmm4ahw0 $%0 = $%1, $%2, $%3"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_fmm4ahw1"
  [(unspec_volatile [(match_operand 0 "" "")
                     (match_operand 1 "" "")
                     (match_operand 2 "" "")
                     (match_operand 3 "" "")] UNSPEC_FMM4AHW1)]
  ""
  "fmm4ahw1 $%0 = $%1, $%2, $%3"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_fmm4ahw2"
  [(unspec_volatile [(match_operand 0 "" "")
                     (match_operand 1 "" "")
                     (match_operand 2 "" "")
                     (match_operand 3 "" "")] UNSPEC_FMM4AHW2)]
  ""
  "fmm4ahw2 $%0 = $%1, $%2, $%3"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_fmm4ahw3"
  [(unspec_volatile [(match_operand 0 "" "")
                     (match_operand 1 "" "")
                     (match_operand 2 "" "")
                     (match_operand 3 "" "")] UNSPEC_FMM4AHW3)]
  ""
  "fmm4ahw3 $%0 = $%1, $%2, $%3"
  [(set_attr "type" "tca")]
)


