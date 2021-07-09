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
    // late splitting of mem will not be possible in the general case
    if ((MEM_P (operands[0]) && !kvx_ok_for_paired_reg_p (operands[1]))
        ||(MEM_P (operands[1]) && !kvx_ok_for_paired_reg_p (operands[0])))
    {
       gcc_assert (! reload_completed && !reload_in_progress);
       rtx tmp = gen_reg_rtx(<MODE>mode);
       emit_move_insn (tmp, operands[1]);
       emit_move_insn (operands[0], tmp);
       DONE;
    }

    if (MEM_P (operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn_and_split "*mov<mode>_oddreg"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (match_operand:ALL128 1 "nonmemory_operand" " ir"))]
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
          (match_operand:ALL128 1 "vec_or_scalar_immediate_operand" "i" ))]
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
    // late splitting of mem will not be possible in the general case
    if ((MEM_P (operands[0]) && !kvx_ok_for_quad_reg_p (operands[1]))
        || MEM_P (operands[1]) && !kvx_ok_for_quad_reg_p (operands[0]))
    {
      gcc_assert (! reload_completed && !reload_in_progress);
      rtx tmp = gen_reg_rtx(<MODE>mode);
      emit_move_insn (tmp, operands[1]);
      emit_move_insn (operands[0], tmp);
      DONE;
    }

    if (MEM_P (operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn_and_split "*mov<mode>_misalign_reg"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (match_operand:ALL256 1 "nonmemory_operand" "ir"))]
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
          (match_operand:ALL256 1 "vec_or_scalar_immediate_operand" "i" ))]
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

(define_insn "*sbmm8"
  [(set (match_operand:ALL64 0 "register_operand" "=r")
        (unspec:ALL64 [(match_operand:FITGPR 1 "register_operand" "r")
                       (match_operand:DI 2 "register_operand" "r")] UNSPEC_SBMM8))]
  ""
  "sbmm8 %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sbmm8xy"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (unspec:ALL128 [(match_operand:SIMD128 1 "register_operand" "r")
                        (match_operand:DI 2 "register_operand" "r")
                        (match_operand:DI 3 "register_operand" "r")] UNSPEC_SBMM8XY))]
  ""
  "sbmm8 %x0 = %x1, %2\n\tsbmm8 %y0 = %y1, %3"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length" "8")]
)


;; Vector Insert/Extract Helpers

(define_insn "*andd"
  [(set (match_operand:ALL64 0 "register_operand" "=r")
        (unspec:ALL64 [(match_operand:ALL64 1 "register_operand" "r")
                       (match_operand:FITGPR 2 "register_operand" "r")] UNSPEC_ANDD))]
  ""
  "andd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*xord"
  [(set (match_operand:ALL64 0 "register_operand" "=r")
        (unspec:ALL64 [(match_operand:ALL64 1 "register_operand" "r")
                       (match_operand:FITGPR 2 "register_operand" "r")] UNSPEC_XORD))]
  ""
  "xord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*slld"
  [(set (match_operand:FITGPR 0 "register_operand" "=r")
        (unspec:FITGPR [(match_operand:SIMD64 1 "register_operand" "r")
                        (match_operand:SI 2 "sixbits_unsigned_operand" "i")] UNSPEC_SLLD))]
  ""
  "slld %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*srld"
  [(set (match_operand:FITGPR 0 "register_operand" "=r")
        (unspec:FITGPR [(match_operand:SIMD64 1 "register_operand" "r")
                        (match_operand:SI 2 "sixbits_unsigned_operand" "i")] UNSPEC_SRLD))]
  ""
  "srld %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*insf"
  [(set (match_operand:ALL64 0 "register_operand" "+r")
        (unspec:ALL64 [(match_operand:FITGPR 1 "register_operand" "r")
                       (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                       (match_operand:SI 3 "sixbits_unsigned_operand" "i")
                       (match_dup 0)] UNSPEC_INSF))]
  ""
  "insf %0 = %1, %2+%3-1, %3"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*extfz"
  [(set (match_operand:FITGPR 0 "register_operand" "=r")
        (unspec:FITGPR [(match_operand:ALL64 1 "register_operand" "r")
                        (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                        (match_operand:SI 3 "sixbits_unsigned_operand" "i")] UNSPEC_EXTFZ))]
  ""
  "extfz %0 = %1, %2+%3-1, %3"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*zxextfz_8_0"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 0)] UNSPEC_EXTFZ)))]
  ""
  "zxbd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*zxextfz_8_8"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 8)] UNSPEC_EXTFZ)))]
  ""
  "extfz %0 = %1, 15, 8"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*zxextfz_8_16"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 16)] UNSPEC_EXTFZ)))]
  ""
  "extfz %0 = %1, 23, 16"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*zxextfz_8_24"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 24)] UNSPEC_EXTFZ)))]
  ""
  "extfz %0 = %1, 31, 24"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*zxextfz_8_32"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 32)] UNSPEC_EXTFZ)))]
  ""
  "extfz %0 = %1, 39, 32"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*zxextfz_8_40"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 40)] UNSPEC_EXTFZ)))]
  ""
  "extfz %0 = %1, 47, 40"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*zxextfz_8_48"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 48)] UNSPEC_EXTFZ)))]
  ""
  "extfz %0 = %1, 55, 48"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*zxextfz_8_56"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 56)] UNSPEC_EXTFZ)))]
  ""
  "srld %0 = %1, 56"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*sxextfz_8_0"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 0)] UNSPEC_EXTFZ)))]
  ""
  "sxbd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sxextfz_8_8"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 8)] UNSPEC_EXTFZ)))]
  ""
  "extfs %0 = %1, 15, 8"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sxextfz_8_16"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 16)] UNSPEC_EXTFZ)))]
  ""
  "extfs %0 = %1, 23, 16"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sxextfz_8_24"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 24)] UNSPEC_EXTFZ)))]
  ""
  "extfs %0 = %1, 31, 24"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sxextfz_8_32"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 32)] UNSPEC_EXTFZ)))]
  ""
  "extfs %0 = %1, 39, 32"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sxextfz_8_40"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 40)] UNSPEC_EXTFZ)))]
  ""
  "extfs %0 = %1, 47, 40"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sxextfz_8_48"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 48)] UNSPEC_EXTFZ)))]
  ""
  "extfs %0 = %1, 55, 48"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sxextfz_8_56"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 8) (const_int 56)] UNSPEC_EXTFZ)))]
  ""
  "srad %0 = %1, 56"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*zxextfz_16_0"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 16) (const_int 0)] UNSPEC_EXTFZ)))]
  ""
  "zxhd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*zxextfz_16_16"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 16) (const_int 16)] UNSPEC_EXTFZ)))]
  ""
  "srlw %0 = %1, 16"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*zxextfz_16_32"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 16) (const_int 32)] UNSPEC_EXTFZ)))]
  ""
  "extfz %0 = %1, 47, 32"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*zxextfz_16_48"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 16) (const_int 48)] UNSPEC_EXTFZ)))]
  ""
  "srld %0 = %1, 48"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*sxextfz_16_0"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 16) (const_int 0)] UNSPEC_EXTFZ)))]
  ""
  "sxhd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sxextfz_16_16"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 16) (const_int 16)] UNSPEC_EXTFZ)))]
  ""
  "extfs %0 = %1, 31, 16"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sxextfz_16_32"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 16) (const_int 32)] UNSPEC_EXTFZ)))]
  ""
  "extfs %0 = %1, 47, 32"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sxextfz_16_48"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 16) (const_int 48)] UNSPEC_EXTFZ)))]
  ""
  "srad %0 = %1, 48"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*zxextfz_32_0"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 32) (const_int 0)] UNSPEC_EXTFZ)))]
  ""
  "zxwd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*zxextfz_32_32"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (zero_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 33) (const_int 32)] UNSPEC_EXTFZ)))]
  ""
  "srld %0 = %1, 32"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*sxextfz_32_0"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 32) (const_int 0)] UNSPEC_EXTFZ)))]
  ""
  "sxwd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sxextfz_32_32"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (sign_extend:SIDI (unspec:<ALL64:INNER> [(match_operand:ALL64 1 "register_operand" "r")
                                                 (const_int 32) (const_int 32)] UNSPEC_EXTFZ)))]
  ""
  "srad %0 = %1, 32"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*fextfz_16_0"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (float_extend:SF (unspec:HF [(match_operand:V4HF 1 "register_operand" "r")
                                     (const_int 16) (const_int 0)] UNSPEC_EXTFZ)))]
  ""
  "fwidenlhw %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*fextfz_16_16"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (float_extend:SF (unspec:HF [(match_operand:V4HF 1 "register_operand" "r")
                                     (const_int 16) (const_int 16)] UNSPEC_EXTFZ)))]
  ""
  "fwidenmhw %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn_and_split "*fextfz_32_0"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r") (const_int 32) (const_int 0)] UNSPEC_EXTFZ))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0) (subreg:SF (match_dup 1) 0))]
  ""
)

(define_insn_and_split "*v128"
  [(set (match_operand:SIMD128 0 "register_operand" "=r")
        (unspec:SIMD128 [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128))]
  ""
  "copyd %x0 = %1\n\tcopyd %y0 = %1"
  ""
  [(set (subreg:<CHUNK> (match_dup 0) 0) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 8) (match_dup 1))]
  ""
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*v256"
  [(set (match_operand:SIMD256 0 "register_operand" "=r")
        (unspec:SIMD256 [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256))]
  ""
  "copyd %x0 = %1\n\tcopyd %y0 = %1\n\tcopyd %z0 = %1\n\tcopyd %t0 = %1"
  ""
  [(set (subreg:<CHUNK> (match_dup 0) 0) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 8) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 16) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 24) (match_dup 1))]
  ""
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; S64I (V4HI V2SI)

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
                      (match_operand 4 "" "")] UNSPEC_SELECT64))]
  ""
  "cmove<suffix>%4 %3? %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "add<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (plus:S64I (match_operand:S64I 1 "register_operand" "r")
                   (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "add<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "ssadd<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ss_plus:S64I (match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "adds<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn_and_split "usadd<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (us_plus:S64I (match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")))
   (clobber (match_scratch:S64I 3 "=&r"))
   (clobber (match_scratch:S64I 4 "=&r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 3)
        (plus:S64I (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (ltu:S64I (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (ior:S64I (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite")]
)

(define_insn "*addx2<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (plus:S64I (ashift:S64I (match_operand:S64I 1 "register_operand" "r")
                                (const_int 1))
                   (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "addx2<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*addx4<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (plus:S64I (ashift:S64I (match_operand:S64I 1 "register_operand" "r")
                                (const_int 2))
                   (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "addx4<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*addx8<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (plus:S64I (ashift:S64I (match_operand:S64I 1 "register_operand" "r")
                                (const_int 3))
                   (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "addx8<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*addx16<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (plus:S64I (ashift:S64I (match_operand:S64I 1 "register_operand" "r")
                                (const_int 4))
                   (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "addx16<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_expand "kvx_add<suffix>"
  [(match_operand:S64I 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand:S64I 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_add<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssadd<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen_usadd<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "sub<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r")
                    (match_operand:S64I 2 "register_operand" "r")))]
  ""
  "sbf<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "sssub<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ss_minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r")
                       (match_operand:S64I 2 "register_operand" "r")))]
  ""
  "sbfs<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn_and_split "ussub<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (us_minus:S64I (match_operand:S64I 1 "register_operand" "r")
                       (match_operand:S64I 2 "register_operand" "r")))
   (clobber (match_scratch:S64I 3 "=&r"))
   (clobber (match_scratch:S64I 4 "=&r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 3)
        (minus:S64I (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (leu:S64I (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (and:S64I (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sbfx2<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r")
                    (ashift:S64I (match_operand:S64I 2 "register_operand" "r")
                                 (const_int 1))))]
  ""
  "sbfx2<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sbfx4<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r")
                    (ashift:S64I (match_operand:S64I 2 "register_operand" "r")
                                 (const_int 2))))]
  ""
  "sbfx4<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sbfx8<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r")
                    (ashift:S64I (match_operand:S64I 2 "register_operand" "r")
                                 (const_int 3))))]
  ""
  "sbfx8<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*sbfx16<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (minus:S64I (match_operand:S64I 1 "nonmemory_operand" "r")
                    (ashift:S64I (match_operand:S64I 2 "register_operand" "r")
                                 (const_int 4))))]
  ""
  "sbfx16<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_lite")]
)

(define_expand "kvx_sbf<suffix>"
  [(match_operand:S64I 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand:S64I 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_sub<mode>3 (operands[0], operands[2], operands[1]));
    else if (xstr[1] == 's')
      emit_insn (gen_sssub<mode>3 (operands[0], operands[2], operands[1]));
    else if (xstr[1] == 'u')
      emit_insn (gen_ussub<mode>3 (operands[0], operands[2], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "mul<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (mult:S64I (match_operand:S64I 1 "register_operand" "r")
                   (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "mul<suffix> %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "smin<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (smin:S64I (match_operand:S64I 1 "register_operand" "r")
                   (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "min<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "smax<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (smax:S64I (match_operand:S64I 1 "register_operand" "r")
                   (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "max<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "umin<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (umin:S64I (match_operand:S64I 1 "register_operand" "r")
                   (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "minu<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "umax<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (umax:S64I (match_operand:S64I 1 "register_operand" "r")
                   (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "maxu<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "and<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (and:S64I (match_operand:S64I 1 "register_operand" "r")
                  (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "andd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*nand<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ior:S64I (not:S64I (match_operand:S64I 1 "register_operand" "r"))
                  (not:S64I (match_operand:S64I 2 "nonmemory_operand" "r"))))]
  ""
  "nandd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*andn<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (and:S64I (not:S64I (match_operand:S64I 1 "register_operand" "r"))
                  (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "andnd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "ior<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ior:S64I (match_operand:S64I 1 "register_operand" "r")
                  (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "ord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*nior<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (and:S64I (not:S64I (match_operand:S64I 1 "register_operand" "r"))
                  (not:S64I (match_operand:S64I 2 "nonmemory_operand" "r"))))]
  ""
  "nord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*iorn<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ior:S64I (not:S64I (match_operand:S64I 1 "register_operand" "r"))
                  (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "ornd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "xor<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (xor:S64I (match_operand:S64I 1 "register_operand" "r")
                  (match_operand:S64I 2 "nonmemory_operand" "r")))]
  ""
  "xord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*nxor<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (not:S64I (xor:S64I (match_operand:S64I 1 "register_operand" "r")
                            (match_operand:S64I 2 "nonmemory_operand" "r"))))]
  ""
  "nxord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "madd<mode><mode>4"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (plus:S64I (mult:S64I (match_operand:S64I 1 "register_operand" "r")
                              (match_operand:S64I 2 "nonmemory_operand" "r"))
                   (match_operand:S64I 3 "register_operand" "0")))]
  ""
  "madd<suffix> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
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

(define_insn_and_split "usashl<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r")
        (us_ashift:S64I (match_operand:S64I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:S64I 3 "=&r,&r"))
   (clobber (match_scratch:S64I 4 "=&r,&r"))
   (clobber (match_scratch:S64I 5 "=&r,&r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 3)
        (ashift:S64I (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (lshiftrt:S64I (match_dup 3) (match_dup 2)))
   (set (match_dup 5)
        (ne:S64I (match_dup 4) (match_dup 1)))
   (set (match_dup 0)
        (ior:S64I (match_dup 3) (match_dup 5)))]
  ""
  [(set_attr "type" "alu_lite,alu_lite")]
)

(define_expand "kvx_shl<suffix>s"
  [(match_operand:S64I 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand:SI 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    operands[2] = force_reg (SImode, operands[2]);
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_ashl<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssashl<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen_usashl<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r')
      emit_insn (gen_rotl<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
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
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRS64))]
  ""
  "srs<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_expand "kvx_shr<suffix>s"
  [(match_operand:S64I 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand:SI 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    operands[2] = force_reg (SImode, operands[2]);
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_lshr<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'a' && !xstr[2])
      emit_insn (gen_ashr<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'a' && xstr[2] == 'r')
      emit_insn (gen_kvx_srs<suffix>s (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r')
      emit_insn (gen_rotr<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "avg<mode>3_floor"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")] UNSPEC_AVGWP))]
  ""
  "avg<suffix> %0 = %1, %2"
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

(define_insn "uavg<mode>3_floor"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")] UNSPEC_AVGUWP))]
  ""
  "avgu<suffix> %0 = %1, %2"
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

(define_expand "kvx_avg<suffix>"
  [(match_operand:S64I 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand:S64I 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_avg<mode>3_floor (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r' && !xstr[2])
      emit_insn (gen_avg<mode>3_ceil (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u' && !xstr[2])
      emit_insn (gen_uavg<mode>3_floor (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r' && xstr[2] == 'u')
      emit_insn (gen_uavg<mode>3_ceil (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "neg<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (neg:S64I (match_operand:S64I 1 "register_operand" "r")))]
  ""
  "neg<suffix> %0 = %1"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length"        "8")]
)

(define_insn "ssneg<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ss_neg:S64I (match_operand:S64I 1 "register_operand" "r")))]
  ""
  "sbfs<suffix> %0 = %1, 0"
  [(set_attr "type" "alu_lite_x")
   (set_attr "length"        "8")]
)

(define_expand "kvx_neg<suffix>"
  [(match_operand:S64I 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_neg<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssneg<mode>2 (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "abs<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (abs:S64I (match_operand:S64I 1 "register_operand" "r")))]
  ""
  "abs<suffix> %0 = %1"
  [(set_attr "type" "alu_lite_x")
   (set_attr "length"        "8")]
)

(define_insn_and_split "ssabs<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ss_abs:S64I (match_operand:S64I 1 "register_operand" "r")))]
  ""
  "#"
  ""
  [(set (match_dup 0)
        (ss_neg:S64I (match_dup 1)))
   (set (match_dup 0)
        (abs:S64I (match_dup 0)))]
  ""
)

(define_expand "kvx_abs<suffix>"
  [(match_operand:S64I 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_abs<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssabs<mode>2 (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
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

(define_expand "kvx_bitcnt<suffix>"
  [(match_operand:S64I 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_popcount<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 'l' && xstr[2] == 'z')
      emit_insn (gen_clz<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 'l' && xstr[2] == 's')
      emit_insn (gen_clrsb<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 't')
      emit_insn (gen_ctz<mode>2 (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "one_cmpl<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (not:S64I (match_operand:S64I 1 "register_operand" "r")))]
  ""
  "notd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "abd<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (abs:S64I (minus:S64I (match_operand:S64I 1 "register_operand" "r")
                              (match_operand:S64I 2 "register_operand" "r"))))]
  ""
  "abd<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn_and_split "ssabd<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ss_abs:S64I (minus:S64I (match_operand:S64I 1 "register_operand" "r")
                                 (match_operand:S64I 2 "register_operand" "r"))))
   (clobber (match_scratch:S64I 3 "=&r"))
   (clobber (match_scratch:S64I 4 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3)
        (ss_minus:S64I (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (ss_minus:S64I (match_dup 2) (match_dup 1)))
   (set (match_dup 0)
        (smax:S64I (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
)

(define_expand "kvx_abd<suffix>"
  [(match_operand:S64I 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand:S64I 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_abd<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssabd<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "_mul<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                     (sign_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r"))))]
  ""
  "mul<widenx> %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "_mulu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (mult:<WIDE> (zero_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                     (zero_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r"))))]
  ""
  "mulu<widenx> %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "_mulsu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                     (zero_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r"))))]
  ""
  "mulsu<widenx> %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_expand "kvx_mul<widenx>"
  [(match_operand:<WIDE> 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand:S64I 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen__mul<widenx> (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen__mulu<widenx> (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen__mulsu<widenx> (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "_madd<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (plus:<WIDE> (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                                  (sign_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r")))
                     (match_operand:<WIDE> 3 "register_operand" "0")))]
  ""
  "madd<widenx> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "_maddu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (plus:<WIDE> (mult:<WIDE> (zero_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                                  (zero_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r")))
                     (match_operand:<WIDE> 3 "register_operand" "0")))]
  ""
  "maddu<widenx> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "_maddsu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (plus:<WIDE> (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                                  (zero_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r")))
                     (match_operand:<WIDE> 3 "register_operand" "0")))]
  ""
  "maddsu<widenx> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_expand "kvx_madd<widenx>"
  [(match_operand:<WIDE> 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand:S64I 2 "register_operand" "")
   (match_operand:<WIDE> 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[4], 0);
    if (!*xstr)
      emit_insn (gen__madd<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 'u')
      emit_insn (gen__maddu<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 's')
      emit_insn (gen__maddsu<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "_msbf<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (minus:<WIDE> (match_operand:<WIDE> 3 "register_operand" "0")
                      (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                                   (sign_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r")))))]
  ""
  "msbf<widenx> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "_msbfu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (minus:<WIDE> (match_operand:<WIDE> 3 "register_operand" "0")
                      (mult:<WIDE> (zero_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                                   (zero_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r")))))]
  ""
  "msbfu<widenx> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "_msbfsu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (minus:<WIDE> (match_operand:<WIDE> 3 "register_operand" "0")
                      (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                                   (zero_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r")))))]
  ""
  "msbfsu<widenx> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_expand "kvx_msbf<widenx>"
  [(match_operand:<WIDE> 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand:S64I 2 "register_operand" "")
   (match_operand:<WIDE> 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[4], 0);
    if (!*xstr)
      emit_insn (gen__msbf<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 'u')
      emit_insn (gen__msbfu<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 's')
      emit_insn (gen__msbfsu<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else
      gcc_unreachable ();
    DONE;
  }
)


;; V4HI

(define_insn_and_split "rotlv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (rotate:V4HI (match_operand:V4HI 1 "register_operand" "r")
                     (match_operand:SI 2 "register_operand" "r")))
   (clobber (match_scratch:SI 3 "=&r"))
   (clobber (match_scratch:V4HI 4 "=&r"))
   (clobber (match_scratch:V4HI 5 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3) (neg:SI (match_dup 2)))
   (set (match_dup 4) (ashift:V4HI (match_dup 1) (match_dup 2)))
   (set (match_dup 5) (lshiftrt:V4HI (match_dup 1) (match_dup 3)))
   (set (match_dup 0) (ior:V4HI (match_dup 4) (match_dup 5)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (SImode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (V4HImode);
    if (GET_CODE (operands[5]) == SCRATCH)
      operands[5] = gen_reg_rtx (V4HImode);
  }
)

(define_insn_and_split "rotrv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (rotatert:V4HI (match_operand:V4HI 1 "register_operand" "r")
                       (match_operand:SI 2 "register_operand" "r")))
   (clobber (match_scratch:SI 3 "=&r"))
   (clobber (match_scratch:V4HI 4 "=&r"))
   (clobber (match_scratch:V4HI 5 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3) (neg:SI (match_dup 2)))
   (set (match_dup 4) (lshiftrt:V4HI (match_dup 1) (match_dup 2)))
   (set (match_dup 5) (ashift:V4HI (match_dup 1) (match_dup 3)))
   (set (match_dup 0) (ior:V4HI (match_dup 4) (match_dup 5)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (SImode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (V4HImode);
    if (GET_CODE (operands[5]) == SCRATCH)
      operands[5] = gen_reg_rtx (V4HImode);
  }
)

(define_expand "kvx_zx<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand")
        (unspec:<WIDE> [(match_operand:S64L 1 "register_operand")
                        (match_dup 2) (match_dup 3)] UNSPEC_ZX64))]
  ""
  {
    operands[2] = gen_reg_rtx (DImode);
    rtx valuev8qi_l = GEN_INT (0x0008000400020001);
    rtx valuev4hi_l = GEN_INT (0x0000080400000201);
    emit_insn (gen_rtx_SET (operands[2], value<mode>_l));
    operands[3] = gen_reg_rtx (DImode);
    rtx valuev8qi_m = GEN_INT (0x0080004000200010);
    rtx valuev4hi_m = GEN_INT (0x0000804000002010);
    emit_insn (gen_rtx_SET (operands[3], value<mode>_m));
  }
)

(define_insn "*kvx_zx<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64L 1 "register_operand" "r")
                        (match_operand:DI 2 "register_operand" "r")
                        (match_operand:DI 3 "register_operand" "r")] UNSPEC_ZX64))]
  ""
  "sbmm8 %x0 = %1, %2\n\tsbmm8 %y0 = %1, %3"
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "kvx_sx<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64L 1 "register_operand" "r")] UNSPEC_SX64))]
  ""
  "sxl<hwidenx> %x0 = %1\n\tsxm<hwidenx> %y0 = %1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_qx<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand")
        (unspec:<WIDE> [(match_operand:S64L 1 "register_operand")
                        (match_dup 2) (match_dup 3)] UNSPEC_QX64))]
  ""
  {
    operands[2] = gen_reg_rtx (DImode);
    rtx valuev8qi_l = GEN_INT (0x0800040002000100);
    rtx valuev4hi_l = GEN_INT (0x0804000002010000);
    emit_insn (gen_rtx_SET (operands[2], value<mode>_l));
    operands[3] = gen_reg_rtx (DImode);
    rtx valuev8qi_m = GEN_INT (0x8000400020001000);
    rtx valuev4hi_m = GEN_INT (0x8040000020100000);
    emit_insn (gen_rtx_SET (operands[3], value<mode>_m));
  }
)

(define_insn "*kvx_qx<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64L 1 "register_operand" "r")
                        (match_operand:DI 2 "register_operand" "r")
                        (match_operand:DI 3 "register_operand" "r")] UNSPEC_QX64))]
  ""
  "sbmm8 %x0 = %1, %2\n\tsbmm8 %y0 = %1, %3"
  [(set_attr "type" "alu_lite_x2")]
)

(define_expand "kvx_widen<widenx>"
  [(match_operand:<WIDE> 0 "register_operand" "")
   (match_operand:S64L 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_kvx_sx<widenx> (operands[0], operands[1]));
    else if (xstr[1] == 'z')
      emit_insn (gen_kvx_zx<widenx> (operands[0], operands[1]));
    else if (xstr[1] == 'q')
      emit_insn (gen_kvx_qx<widenx> (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_expand "kvx_trunc<truncx>"
[(parallel
  [(set (match_operand:S64L 0 "register_operand")
        (unspec:S64L [(match_operand:<WIDE> 1 "register_operand")
                      (match_dup 2) (match_dup 3)] UNSPEC_TRUNC64))
   (clobber (match_dup 4))]
)]
  ""
  {
    operands[2] = gen_reg_rtx (DImode);
    rtx valuev8qi_l = GEN_INT (0x0000000040100401);
    rtx valuev4hi_l = GEN_INT (0x0000000020100201);
    emit_insn (gen_rtx_SET (operands[2], value<mode>_l));
    operands[3] = gen_reg_rtx (DImode);
    rtx valuev8qi_m = GEN_INT (0x4010040100000000);
    rtx valuev4hi_m = GEN_INT (0x2010020100000000);
    emit_insn (gen_rtx_SET (operands[3], value<mode>_m));
    operands[4] = gen_rtx_SCRATCH (<WIDE>mode);
  }
)

(define_insn_and_split "*kvx_trunc<truncx>"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (unspec:S64L [(match_operand:<WIDE> 1 "register_operand" "r")
                      (match_operand:DI 2 "register_operand" "r")
                      (match_operand:DI 3 "register_operand" "r")] UNSPEC_TRUNC64))
   (clobber (match_scratch:<WIDE> 4 "=r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 4)
        (unspec:<WIDE> [(match_dup 1) (match_dup 2) (match_dup 3)] UNSPEC_SBMM8XY))
   (set (match_dup 0)
        (unspec:S64L [(subreg:S64L (match_dup 4) 0)
                      (subreg:S64L (match_dup 4) 8)] UNSPEC_XORD))]
  ""
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_fract<truncx>"
[(parallel
  [(set (match_operand:S64L 0 "register_operand")
        (unspec:S64L [(match_operand:<WIDE> 1 "register_operand")
                      (match_dup 2) (match_dup 3)] UNSPEC_FRACT64))
   (clobber (match_dup 4))]
)]
  ""
  {
    operands[2] = gen_reg_rtx (DImode);
    rtx valuev8qi_l = GEN_INT (0x0000000080200802);
    rtx valuev4hi_l = GEN_INT (0x0000000080400804);
    emit_insn (gen_rtx_SET (operands[2], value<mode>_l));
    operands[3] = gen_reg_rtx (DImode);
    rtx valuev8qi_m = GEN_INT (0x8020080200000000);
    rtx valuev4hi_m = GEN_INT (0x8040080400000000);
    emit_insn (gen_rtx_SET (operands[3], value<mode>_m));
    operands[4] = gen_rtx_SCRATCH (<WIDE>mode);
  }
)

(define_insn_and_split "*kvx_fract<truncx>"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (unspec:S64L [(match_operand:<WIDE> 1 "register_operand" "r")
                      (match_operand:DI 2 "register_operand" "r")
                      (match_operand:DI 3 "register_operand" "r")] UNSPEC_FRACT64))
   (clobber (match_scratch:<WIDE> 4 "=r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 4)
        (unspec:<WIDE> [(match_dup 1) (match_dup 2) (match_dup 3)] UNSPEC_SBMM8XY))
   (set (match_dup 0)
        (unspec:S64L [(subreg:S64L (match_dup 4) 0)
                      (subreg:S64L (match_dup 4) 8)] UNSPEC_XORD))]
  ""
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_sat<truncx>"
  [(match_operand:S64L 0 "register_operand")
   (match_operand:<WIDE> 1 "register_operand")]
  ""
  {
    rtx saturated = gen_reg_rtx (<WIDE>mode);
    rtx lshift = GEN_INT (GET_MODE_UNIT_BITSIZE (<MODE>mode));
    emit_insn (gen_ssashl<wide>3 (saturated, operands[1], lshift));
    emit_insn (gen_kvx_fract<truncx> (operands[0], saturated));
    DONE;
  }
)

(define_expand "kvx_satu<truncx>"
  [(match_operand:S64L 0 "register_operand")
   (match_operand:<WIDE> 1 "register_operand")]
  ""
  {
    rtx zero = gen_reg_rtx (<HWIDE>mode);
    rtx lower = gen_reg_rtx (<WIDE>mode);
    rtx upper = gen_reg_rtx (<WIDE>mode);
    rtx maxvalv4hi = gen_rtx_CONST_VECTOR (V4HImode,
                                           gen_rtvec (4, GEN_INT (0xFF), GEN_INT (0xFF),
                                                         GEN_INT (0xFF), GEN_INT (0xFF)));
    rtx maxvalv2si = gen_rtx_CONST_VECTOR (V2SImode,
                                           gen_rtvec (2, GEN_INT (0xFFFF), GEN_INT (0xFFFF)));
    rtx zero_chunk = gen_rtx_UNSPEC (<WIDE>mode, gen_rtvec (1, zero), UNSPEC_DUP128);
    rtx maxval_chunk = gen_rtx_UNSPEC (<WIDE>mode, gen_rtvec (1, maxval<hwide>), UNSPEC_DUP128);
    emit_insn (gen_rtx_SET (zero, CONST0_RTX (<HWIDE>mode)));
    emit_insn (gen_rtx_SET (lower, gen_rtx_SMAX (<WIDE>mode, operands[1], zero_chunk)));
    emit_insn (gen_rtx_SET (upper, gen_rtx_SMIN (<WIDE>mode, lower, maxval_chunk)));
    emit_insn (gen_kvx_trunc<truncx> (operands[0], upper));
    DONE;
  }
)

(define_expand "kvx_narrow<truncx>"
  [(match_operand:S64L 0 "register_operand" "")
   (match_operand:<WIDE> 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_kvx_trunc<truncx> (operands[0], operands[1]));
    else if (xstr[1] == 'q')
      emit_insn (gen_kvx_fract<truncx> (operands[0], operands[1]));
    else if (xstr[1] == 's')
      emit_insn (gen_kvx_sat<truncx> (operands[0], operands[1]));
    else if (xstr[1] == 'u')
      emit_insn (gen_kvx_satu<truncx> (operands[0], operands[1]));
    else
      gcc_unreachable ();
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

(define_insn "rotlv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r")
        (rotate:V2SI (match_operand:V2SI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "rotate_operand" "r,U05")))]
  ""
  "rolwps %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "rotrv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r")
        (rotatert:V2SI (match_operand:V2SI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "rotate_operand" "r,U05")))]
  ""
  "rorwps %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

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

(define_insn "kvx_zxwdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2SI 1 "register_operand" "r")] UNSPEC_ZX64))]
  ""
  "zxwd %x0 = %1\n\tsrld %y0 = %1, 32"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_sxwdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2SI 1 "register_operand" "r")] UNSPEC_SX64))]
  ""
  "sxwd %x0 = %1\n\tsrad %y0 = %1, 32"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_qxwdp"
  [(set (match_operand:V2DI 0 "register_operand")
        (unspec:V2DI [(match_operand:V2SI 1 "register_operand")
        (match_dup 2)] UNSPEC_QX64))]
  ""
  {
    operands[2] = gen_reg_rtx (DImode);
    emit_insn (gen_rtx_SET (operands[2], GEN_INT (0xFFFFFFFF00000000)));
  }
)

(define_insn_and_split "*kvx_qxwdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2SI 1 "register_operand" "r")
                      (match_operand:DI 2 "register_operand" "r")] UNSPEC_QX64))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (unspec:DI [(match_dup 1) (const_int 32)] UNSPEC_SLLD))
   (set (subreg:DI (match_dup 0) 8)
        (unspec:DI [(match_dup 2) (match_dup 1)] UNSPEC_ANDD))]
  ""
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_widenwdp"
  [(match_operand:V2DI 0 "register_operand" "")
   (match_operand:V2SI 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_kvx_sxwdp (operands[0], operands[1]));
    else if (xstr[1] == 'z')
      emit_insn (gen_kvx_zxwdp (operands[0], operands[1]));
    else if (xstr[1] == 'q')
      emit_insn (gen_kvx_qxwdp (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_expand "kvx_truncdwp"
  [(match_operand:V2SI 0 "register_operand")
   (match_operand:V2DI 1 "register_operand")]
  ""
  {
    rtx op0_l = gen_reg_rtx (V2SImode);
    rtx op0_m = gen_reg_rtx (V2SImode);
    rtx op1_l = simplify_gen_subreg (DImode, operands[1], V2DImode, 0);
    rtx op1_m = simplify_gen_subreg (DImode, operands[1], V2DImode, 8);
    emit_insn (gen_kvx_truncldw (op0_l, op1_l));
    emit_insn (gen_kvx_truncmdw (op0_m, op1_m));
    rtx xord = gen_rtx_UNSPEC (V2SImode, gen_rtvec (2, op0_l, op0_m), UNSPEC_XORD);
    emit_insn (gen_rtx_SET (operands[0], xord));
    DONE;
  }
)

(define_expand "kvx_fractdwp"
  [(match_operand:V2SI 0 "register_operand")
   (match_operand:V2DI 1 "register_operand")]
  ""
  {
    rtx op0_l = gen_reg_rtx (V2SImode);
    rtx op0_m = gen_reg_rtx (V2SImode);
    rtx op1_l = simplify_gen_subreg (DImode, operands[1], V2DImode, 0);
    rtx op1_m = simplify_gen_subreg (DImode, operands[1], V2DImode, 8);
    emit_insn (gen_kvx_fractldw (op0_l, op1_l));
    emit_insn (gen_kvx_fractmdw (op0_m, op1_m));
    rtx xord = gen_rtx_UNSPEC (V2SImode, gen_rtvec (2, op0_l, op0_m), UNSPEC_XORD);
    emit_insn (gen_rtx_SET (operands[0], xord));
    DONE;
  }
)

(define_expand "kvx_satdwp"
  [(match_operand:V2SI 0 "register_operand")
   (match_operand:V2DI 1 "register_operand")]
  ""
  {
    rtx op0_l = gen_reg_rtx (V2SImode);
    rtx op0_m = gen_reg_rtx (V2SImode);
    rtx op1_l = simplify_gen_subreg (DImode, operands[1], V2DImode, 0);
    rtx op1_m = simplify_gen_subreg (DImode, operands[1], V2DImode, 8);
    emit_insn (gen_kvx_satldw (op0_l, op1_l));
    emit_insn (gen_kvx_satmdw (op0_m, op1_m));
    rtx xord = gen_rtx_UNSPEC (V2SImode, gen_rtvec (2, op0_l, op0_m), UNSPEC_XORD);
    emit_insn (gen_rtx_SET (operands[0], xord));
    DONE;
  }
)

(define_expand "kvx_satudwp"
  [(match_operand:V2SI 0 "register_operand")
   (match_operand:V2DI 1 "register_operand")]
  ""
  {
    rtx op0_l = gen_reg_rtx (V2SImode);
    rtx op0_m = gen_reg_rtx (V2SImode);
    rtx op1_l = simplify_gen_subreg (DImode, operands[1], V2DImode, 0);
    rtx op1_m = simplify_gen_subreg (DImode, operands[1], V2DImode, 8);
    emit_insn (gen_kvx_satuldw (op0_l, op1_l));
    emit_insn (gen_kvx_satumdw (op0_m, op1_m));
    rtx xord = gen_rtx_UNSPEC (V2SImode, gen_rtvec (2, op0_l, op0_m), UNSPEC_XORD);
    emit_insn (gen_rtx_SET (operands[0], xord));
    DONE;
  }
)

(define_expand "kvx_narrowdwp"
  [(match_operand:V2SI 0 "register_operand" "")
   (match_operand:V2DI 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_kvx_truncdwp (operands[0], operands[1]));
    else if (xstr[1] == 'q')
      emit_insn (gen_kvx_fractdwp (operands[0], operands[1]));
    else if (xstr[1] == 's')
      emit_insn (gen_kvx_satdwp (operands[0], operands[1]));
    else if (xstr[1] == 'u')
      emit_insn (gen_kvx_satudwp (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "kvx_truncldw"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:DI 1 "register_operand" "r")] UNSPEC_ZXWD))]
  ""
  "zxwd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_truncmdw"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:DI 1 "register_operand" "r") (const_int 32)] UNSPEC_SLLD))]
  ""
  "slld %0 = %1, 32"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_fractldw"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:DI 1 "register_operand" "r") (const_int 32)] UNSPEC_SRLD))]
  ""
  "srld %0 = %1, 32"
  [(set_attr "type" "alu_tiny")]
)

(define_expand "kvx_fractmdw"
  [(match_operand:V2SI 0 "register_operand")
   (match_operand:DI 1 "register_operand")]
  ""
  {
    rtx mask = gen_reg_rtx (DImode);
    emit_insn (gen_rtx_SET (mask, GEN_INT (0xFFFFFFFF00000000)));
    rtx opnd1 = simplify_gen_subreg (V2SImode, operands[1], DImode, 0);
    rtx andd = gen_rtx_UNSPEC (V2SImode, gen_rtvec (2, opnd1, mask), UNSPEC_ANDD);
    emit_insn (gen_rtx_SET (operands[0], andd));
    DONE;
  }
)

(define_expand "kvx_satldw"
  [(match_operand:V2SI 0 "register_operand" "=r")
   (match_operand:DI 1 "register_operand" "r")]
  ""
  {
    rtx saturated = gen_reg_rtx (DImode);
    emit_insn (gen_ssashldi3 (saturated, operands[1], GEN_INT (32)));
    emit_insn (gen_kvx_fractldw (operands[0], saturated));
    DONE;
  }
)

(define_expand "kvx_satmdw"
  [(match_operand:V2SI 0 "register_operand" "=r")
   (match_operand:DI 1 "register_operand" "r")]
  ""
  {
    rtx saturated = gen_rtx_SUBREG (DImode, operands[0], 0);
    emit_insn (gen_ssashldi3 (saturated, operands[1], GEN_INT (32)));
    DONE;
  }
)

(define_expand "kvx_satuldw"
  [(match_operand:V2SI 0 "register_operand" "=r")
   (match_operand:DI 1 "register_operand" "r")]
  ""
  {
    rtx maxval = gen_reg_rtx (DImode);
    emit_insn (gen_rtx_SET (maxval, GEN_INT (0xFFFFFFFF)));
    rtx lower = gen_reg_rtx (DImode), upper = gen_rtx_SUBREG (DImode, operands[0], 0);
    emit_insn (gen_smaxdi3 (lower, operands[1], const0_rtx));
    emit_insn (gen_smindi3 (upper, lower, maxval));
    DONE;
  }
)

(define_expand "kvx_satumdw"
  [(match_operand:V2SI 0 "register_operand" "=r")
   (match_operand:DI 1 "register_operand" "r")]
  ""
  {
    rtx maxval = gen_reg_rtx (DImode);
    emit_insn (gen_rtx_SET (maxval, GEN_INT (0xFFFFFFFF)));
    rtx lower = gen_reg_rtx (DImode), upper = gen_reg_rtx (DImode);
    emit_insn (gen_smaxdi3 (lower, operands[1], const0_rtx));
    emit_insn (gen_smindi3 (upper, lower, maxval));
    emit_insn (gen_kvx_truncmdw (operands[0], upper));
    DONE;
  }
)


;; S128I (V8HI V4SI)

(define_insn "*compn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S128I 2 "register_operand" "r")
          (match_operand:S128I 3 "register_operand" "r")]))]
  ""
  "compn<chunkx>.%1 %x0 = %x2, %x3\n\tcompn<chunkx>.%1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*compn<suffix>s"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S128I 2 "register_operand" "r")
          (unspec:S128I [(match_operand:<CHUNK> 3 "nonmemory_operand" "r")] UNSPEC_DUP128)]))]
  ""
  "compn<chunkx>.%1 %x0 = %x2, %3\n\tcompn<chunkx>.%1 %y0 = %y2, %3"
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
  "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1"
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
  "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_select<suffix>"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "0")
                       (match_operand:<MASK> 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECT128))]
  ""
  "cmove<chunkx>%4 %x3? %x0 = %x1\n\tcmove<chunkx>%4 %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "ashl<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (ashift:S128I (match_operand:S128I 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sll<chunkxs> %x0 = %x1, %2\n\tsll<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "ssashl<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (ss_ashift:S128I (match_operand:S128I 1 "register_operand" "r,r")
                         (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sls<chunkxs> %x0 = %x1, %2\n\tsls<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn_and_split "usashl<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (us_ashift:S128I (match_operand:S128I 1 "register_operand" "r,r")
                         (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:S128I 3 "=&r,&r"))
   (clobber (match_scratch:S128I 4 "=&r,&r"))
   (clobber (match_scratch:S128I 5 "=&r,&r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 3)
        (ashift:S128I (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (lshiftrt:S128I (match_dup 3) (match_dup 2)))
   (set (match_dup 5)
        (ne:S128I (match_dup 4) (match_dup 1)))
   (set (match_dup 0)
        (ior:S128I (match_dup 3) (match_dup 5)))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn "ashr<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (ashiftrt:S128I (match_operand:S128I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sra<chunkxs> %x0 = %x1, %2\n\tsra<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "lshr<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (lshiftrt:S128I (match_operand:S128I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srl<chunkxs> %x0 = %x1, %2\n\tsrl<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "kvx_srs<suffix>s"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRS128))]
  ""
  "srs<chunkxs> %x0 = %x1, %2\n\tsrs<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "avg<mode>3_floor"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "r")] UNSPEC_AVGWQ))]
  ""
  "avg<chunkx> %x0 = %x1, %x2\n\tavg<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "avg<mode>3_ceil"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "r")] UNSPEC_AVGRWQ))]
  ""
  "avgr<chunkx> %x0 = %x1, %x2\n\tavgr<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "uavg<mode>3_floor"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "r")] UNSPEC_AVGUWQ))]
  ""
  "avgu<chunkx> %x0 = %x1, %x2\n\tavgu<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "uavg<mode>3_ceil"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "r")] UNSPEC_AVGRUWQ))]
  ""
  "avgru<chunkx> %x0 = %x1, %x2\n\tavgru<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_avg<suffix>"
  [(match_operand:S128I 0 "register_operand" "")
   (match_operand:S128I 1 "register_operand" "")
   (match_operand:S128I 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_avg<mode>3_floor (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r' && !xstr[2])
      emit_insn (gen_avg<mode>3_ceil (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u' && !xstr[2])
      emit_insn (gen_uavg<mode>3_floor (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r' && xstr[2] == 'u')
      emit_insn (gen_uavg<mode>3_ceil (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "_mul<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S128I 1 "register_operand" "r"))
                     (sign_extend:<WIDE> (match_operand:S128I 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (mult:<HWIDE> (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 0))
                      (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 0))))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (mult:<HWIDE> (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 8))
                      (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 8))))]
  ""
  [(set_attr "type" "mau")]
)

(define_insn_and_split "_mulu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (mult:<WIDE> (zero_extend:<WIDE> (match_operand:S128I 1 "register_operand" "r"))
                     (zero_extend:<WIDE> (match_operand:S128I 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (mult:<HWIDE> (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 0))
                      (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 0))))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (mult:<HWIDE> (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 8))
                      (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 8))))]
  ""
  [(set_attr "type" "mau")]
)

(define_insn_and_split "_mulsu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S128I 1 "register_operand" "r"))
                     (zero_extend:<WIDE> (match_operand:S128I 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (mult:<HWIDE> (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 0))
                      (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 0))))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (mult:<HWIDE> (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 8))
                      (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 8))))]
  ""
  [(set_attr "type" "mau")]
)

(define_expand "kvx_mul<widenx>"
  [(match_operand:<WIDE> 0 "register_operand" "")
   (match_operand:S128I 1 "register_operand" "")
   (match_operand:S128I 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen__mul<widenx> (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen__mulu<widenx> (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen__mulsu<widenx> (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "_madd<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (plus:<WIDE> (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S128I 1 "register_operand" "r"))
                                  (sign_extend:<WIDE> (match_operand:S128I 2 "register_operand" "r")))
                      (match_operand:<WIDE> 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (plus:<HWIDE> (mult:<HWIDE> (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 0))
                                    (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 0)))
                      (subreg:<HWIDE> (match_dup 3) 0)))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (plus:<HWIDE> (mult:<HWIDE> (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 8))
                                    (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 8)))
                      (subreg:<HWIDE> (match_dup 3) 16)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "_maddu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (plus:<WIDE> (mult:<WIDE> (zero_extend:<WIDE> (match_operand:S128I 1 "register_operand" "r"))
                                  (zero_extend:<WIDE> (match_operand:S128I 2 "register_operand" "r")))
                     (match_operand:<WIDE> 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (plus:<HWIDE> (mult:<HWIDE> (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 0))
                                    (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 0)))
                      (subreg:<HWIDE> (match_dup 3) 0)))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (plus:<HWIDE> (mult:<HWIDE> (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 8))
                                    (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 8)))
                      (subreg:<HWIDE> (match_dup 3) 16)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "_maddsu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (plus:<WIDE> (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S128I 1 "register_operand" "r"))
                                  (zero_extend:<WIDE> (match_operand:S128I 2 "register_operand" "r")))
                     (match_operand:<WIDE> 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (plus:<HWIDE> (mult:<HWIDE> (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 0))
                                    (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 0)))
                      (subreg:<HWIDE> (match_dup 3) 0)))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (plus:<HWIDE> (mult:<HWIDE> (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 8))
                                    (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 8)))
                      (subreg:<HWIDE> (match_dup 3) 16)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_expand "kvx_madd<widenx>"
  [(match_operand:<WIDE> 0 "register_operand" "")
   (match_operand:S128I 1 "register_operand" "")
   (match_operand:S128I 2 "register_operand" "")
   (match_operand:<WIDE> 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[4], 0);
    if (!*xstr)
      emit_insn (gen__madd<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 'u')
      emit_insn (gen__maddu<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 's')
      emit_insn (gen__maddsu<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "_msbf<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (minus:<WIDE> (match_operand:<WIDE> 3 "register_operand" "0")
                      (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S128I 1 "register_operand" "r"))
                                   (sign_extend:<WIDE> (match_operand:S128I 2 "register_operand" "r")))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (minus:<HWIDE> (subreg:<HWIDE> (match_dup 3) 0)
                       (mult:<HWIDE> (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 0))
                                     (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 0)))))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (minus:<HWIDE> (subreg:<HWIDE> (match_dup 3) 16)
                       (mult:<HWIDE> (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 8))
                                     (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 8)))))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "_msbfu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (minus:<WIDE> (match_operand:<WIDE> 3 "register_operand" "0")
                      (mult:<WIDE> (zero_extend:<WIDE> (match_operand:S128I 1 "register_operand" "r"))
                                   (zero_extend:<WIDE> (match_operand:S128I 2 "register_operand" "r")))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (minus:<HWIDE> (subreg:<HWIDE> (match_dup 3) 0)
                       (mult:<HWIDE> (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 0))
                                     (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 0)))))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (minus:<HWIDE> (subreg:<HWIDE> (match_dup 3) 16)
                       (mult:<HWIDE> (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 8))
                                     (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 8)))))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "_msbfsu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (minus:<WIDE> (match_operand:<WIDE> 3 "register_operand" "0")
                      (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S128I 1 "register_operand" "r"))
                                   (zero_extend:<WIDE> (match_operand:S128I 2 "register_operand" "r")))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (minus:<HWIDE> (subreg:<HWIDE> (match_dup 3) 0)
                       (mult:<HWIDE> (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 0))
                                     (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 0)))))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (minus:<HWIDE> (subreg:<HWIDE> (match_dup 3) 16)
                       (mult:<HWIDE> (sign_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 8))
                                     (zero_extend:<HWIDE> (subreg:<HALF> (match_dup 2) 8)))))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_expand "kvx_msbf<widenx>"
  [(match_operand:<WIDE> 0 "register_operand" "")
   (match_operand:S128I 1 "register_operand" "")
   (match_operand:S128I 2 "register_operand" "")
   (match_operand:<WIDE> 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[4], 0);
    if (!*xstr)
      emit_insn (gen__msbf<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 'u')
      emit_insn (gen__msbfu<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 's')
      emit_insn (gen__msbfsu<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else
      gcc_unreachable ();
    DONE;
  }
)


;; V128J (V8HI V4SI V2DI)

(define_insn "add<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (plus:V128J (match_operand:V128J 1 "register_operand" "r")
                    (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "add<chunkx> %x0 = %x1, %x2\n\tadd<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*add<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (plus:V128J (unspec:V128J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128)
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "add<chunkx> %x0 = %1, %x2\n\tadd<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*add<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (plus:V128J (match_operand:V128J 1 "register_operand" "r")
                    (unspec:V128J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP128)))]
  ""
  "add<chunkx> %x0 = %x1, %2\n\tadd<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "ssadd<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_plus:V128J (match_operand:V128J 1 "register_operand" "r")
                       (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "adds<chunkx> %x0 = %x1, %x2\n\tadds<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*ssadd<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_plus:V128J (unspec:V128J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128)
                       (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "adds<chunkx> %x0 = %1, %x2\n\tadds<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*ssadd<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_plus:V128J (match_operand:V128J 1 "register_operand" "r")
                       (unspec:V128J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP128)))]
  ""
  "adds<chunkx> %x0 = %x1, %2\n\tadds<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "usadd<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_plus:V128J (match_operand:V128J 1 "register_operand" "r")
                       (match_operand:V128J 2 "register_operand" "r")))
   (clobber (match_scratch:V128J 3 "=&r"))
   (clobber (match_scratch:V128J 4 "=&r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 3)
        (plus:V128J (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (ltu:V128J (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (ior:V128J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*usadd<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_plus:V128J (match_operand:V128J 1 "register_operand" "r")
                       (unspec:V128J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP128)))
   (clobber (match_scratch:V128J 3 "=&r"))
   (clobber (match_scratch:V128J 4 "=&r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 3)
        (plus:V128J (match_dup 1) (unspec:V128J [(match_dup 2)] UNSPEC_DUP128)))
   (set (match_dup 4)
        (ltu:V128J (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (ior:V128J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*addx2<suffix>"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (plus:V128J (ashift:V128J (match_operand:V128J 1 "register_operand" "r")
                                  (const_int 1))
                    (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "addx2<chunkx> %x0 = %x1, %x2\n\taddx2<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4<suffix>"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (plus:V128J (ashift:V128J (match_operand:V128J 1 "register_operand" "r")
                                  (const_int 2))
                    (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "addx4<chunkx> %x0 = %x1, %x2\n\taddx4<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8<suffix>"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (plus:V128J (ashift:V128J (match_operand:V128J 1 "register_operand" "r")
                                  (const_int 3))
                    (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "addx8<chunkx> %x0 = %x1, %x2\n\taddx8<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16<suffix>"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (plus:V128J (ashift:V128J (match_operand:V128J 1 "register_operand" "r")
                                  (const_int 4))
                    (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "addx16<chunkx> %x0 = %x1, %x2\n\taddx16<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_add<suffix>"
  [(match_operand:V128J 0 "register_operand" "")
   (match_operand:V128J 1 "register_operand" "")
   (match_operand:V128J 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_add<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssadd<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen_usadd<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "sub<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (minus:V128J (match_operand:V128J 1 "nonmemory_operand" "r")
                     (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "sbf<chunkx> %x0 = %x2, %x1\n\tsbf<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sub<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (minus:V128J (unspec:V128J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128)
                     (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "sbf<chunkx> %x0 = %x2, %1\n\tsbf<chunkx> %y0 = %y2, %1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sub<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (minus:V128J (match_operand:V128J 1 "register_operand" "r")
                     (unspec:V128J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP128)))]
  ""
  "sbf<chunkx> %x0 = %2, %x1\n\tsbf<chunkx> %y0 = %2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "sssub<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_minus:V128J (match_operand:V128J 1 "nonmemory_operand" "r")
                        (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "sbfs<chunkx> %x0 = %x2, %x1\n\tsbfs<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sssub<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_minus:V128J (unspec:V128J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128)
                        (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "sbfs<chunkx> %x0 = %x2, %1\n\tsbfs<chunkx> %y0 = %y2, %1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sssub<mode>3_s"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_minus:V128J (match_operand:V128J 1 "register_operand" "r")
                        (unspec:V128J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP128)))]
  ""
  "sbfs<chunkx> %x0 = %2, %x1\n\tsbfs<chunkx> %y0 = %2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "ussub<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_minus:V128J (match_operand:V128J 1 "register_operand" "r")
                        (match_operand:V128J 2 "register_operand" "r")))
   (clobber (match_scratch:V128J 3 "=&r"))
   (clobber (match_scratch:V128J 4 "=&r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 3)
        (minus:V128J (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (leu:V128J (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (and:V128J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*ussub<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_minus:V128J (unspec:V128J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128)
                        (match_operand:V128J 2 "register_operand" "r")))
   (clobber (match_scratch:V128J 3 "=&r"))
   (clobber (match_scratch:V128J 4 "=&r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 3)
        (minus:V128J (unspec:V128J [(match_dup 1)] UNSPEC_DUP128) (match_dup 2)))
   (set (match_dup 4)
        (leu:V128J (match_dup 3) (unspec:V128J [(match_dup 1)] UNSPEC_DUP128)))
   (set (match_dup 0)
        (and:V128J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*sbfx2<suffix>"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (minus:V128J (match_operand:V128J 1 "nonmemory_operand" "r")
                     (ashift:V128J (match_operand:V128J 2 "register_operand" "r")
                                   (const_int 1))))]
  ""
  "sbfx2<chunkx> %x0 = %x2, %x1\n\tsbfx2<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4<suffix>"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (minus:V128J (match_operand:V128J 1 "nonmemory_operand" "r")
                     (ashift:V128J (match_operand:V128J 2 "register_operand" "r")
                                   (const_int 2))))]
  ""
  "sbfx4<chunkx> %x0 = %x2, %x1\n\tsbfx4<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8<suffix>"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (minus:V128J (match_operand:V128J 1 "nonmemory_operand" "r")
                     (ashift:V128J (match_operand:V128J 2 "register_operand" "r")
                                   (const_int 3))))]
  ""
  "sbfx8<chunkx> %x0 = %x2, %x1\n\tsbfx8<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16<suffix>"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (minus:V128J (match_operand:V128J 1 "nonmemory_operand" "r")
                     (ashift:V128J (match_operand:V128J 2 "register_operand" "r")
                                   (const_int 4))))]
  ""
  "sbfx16<chunkx> %x0 = %x2, %x1\n\tsbfx16<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_sbf<suffix>"
  [(match_operand:V128J 0 "register_operand" "")
   (match_operand:V128J 1 "register_operand" "")
   (match_operand:V128J 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_sub<mode>3 (operands[0], operands[2], operands[1]));
    else if (xstr[1] == 's')
      emit_insn (gen_sssub<mode>3 (operands[0], operands[2], operands[1]));
    else if (xstr[1] == 'u')
      emit_insn (gen_ussub<mode>3 (operands[0], operands[2], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "smin<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (smin:V128J (match_operand:V128J 1 "register_operand" "r")
                    (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "min<chunkx> %x0 = %x1, %x2\n\tmin<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*smin<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (smin:V128J (unspec:V128J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128)
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "min<chunkx> %x0 = %1, %x2\n\tmin<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*smin<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (smin:V128J (match_operand:V128J 1 "register_operand" "r")
                    (unspec:V128J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP128)))]
  ""
  "min<chunkx> %x0 = %x1, %2\n\tmin<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "smax<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (smax:V128J (match_operand:V128J 1 "register_operand" "r")
                    (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "max<chunkx> %x0 = %x1, %x2\n\tmax<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*smax<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (smax:V128J (unspec:V128J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128)
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "max<chunkx> %x0 = %1, %x2\n\tmax<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*smax<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (smax:V128J (match_operand:V128J 1 "register_operand" "r")
                    (unspec:V128J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP128)))]
  ""
  "max<chunkx> %x0 = %x1, %2\n\tmax<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "umin<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (umin:V128J (match_operand:V128J 1 "register_operand" "r")
                    (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "minu<chunkx> %x0 = %x1, %x2\n\tminu<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*umin<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (umin:V128J (unspec:V128J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128)
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "minu<chunkx> %x0 = %1, %x2\n\tminu<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*umin<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (umin:V128J (match_operand:V128J 1 "register_operand" "r")
                    (unspec:V128J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP128)))]
  ""
  "minu<chunkx> %x0 = %x1, %2\n\tminu<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "umax<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (umax:V128J (match_operand:V128J 1 "register_operand" "r")
                    (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "maxu<chunkx> %x0 = %x1, %x2\n\tmaxu<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*umax<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (umax:V128J (unspec:V128J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128)
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "maxu<chunkx> %x0 = %1, %x2\n\tmaxu<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*umax<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (umax:V128J (match_operand:V128J 1 "register_operand" "r")
                    (unspec:V128J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP128)))]
  ""
  "maxu<chunkx> %x0 = %x1, %2\n\tmaxu<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "and<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (and:V128J (match_operand:V128J 1 "register_operand" "r")
                   (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*nand<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ior:V128J (not:V128J (match_operand:V128J 1 "register_operand" "r"))
                   (not:V128J (match_operand:V128J 2 "nonmemory_operand" "r"))))]
  ""
  "nandd %x0 = %x1, %x2\n\tnandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*andn<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (and:V128J (not:V128J (match_operand:V128J 1 "register_operand" "r"))
                   (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "andnd %x0 = %x1, %x2\n\tandnd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "ior<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ior:V128J (match_operand:V128J 1 "register_operand" "r")
                   (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*nior<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (and:V128J (not:V128J (match_operand:V128J 1 "register_operand" "r"))
                   (not:V128J (match_operand:V128J 2 "nonmemory_operand" "r"))))]
  ""
  "nord %x0 = %x1, %x2\n\tnord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*iorn<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ior:V128J (not:V128J (match_operand:V128J 1 "register_operand" "r"))
                   (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "ornd %x0 = %x1, %x2\n\tornd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "xor<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (xor:V128J (match_operand:V128J 1 "register_operand" "r")
                   (match_operand:V128J 2 "nonmemory_operand" "r")))]
  ""
  "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*nxor<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (not:V128J (xor:V128J (match_operand:V128J 1 "register_operand" "r")
                              (match_operand:V128J 2 "nonmemory_operand" "r"))))]
  ""
  "nxord %x0 = %x1, %x2\n\tnxord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "madd<mode><mode>4"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (plus:V128J (mult:V128J (match_operand:V128J 1 "register_operand" "r")
                                (match_operand:V128J 2 "nonmemory_operand" "r"))
                    (match_operand:V128J 3 "register_operand" "0")))]
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
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (minus:V128J (match_operand:V128J 3 "register_operand" "0")
                    (mult:V128J (match_operand:V128J 1 "register_operand" "r")
                                (match_operand:V128J 2 "register_operand" "r"))))]
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

(define_insn "neg<mode>2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (neg:V128J (match_operand:V128J 1 "register_operand" "r")))]
  ""
  "neg<chunkx> %x0 = %x1\n\tneg<chunkx> %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2_x")
   (set_attr "length"          "16")]
)

(define_insn "ssneg<mode>2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_neg:V128J (match_operand:V128J 1 "register_operand" "r")))]
  ""
  "sbfs<chunkx> %x0 = %x1, 0\n\tsbfs<chunkx> %y0 = %y1, 0"
  [(set_attr "type" "alu_lite_x2_x")
   (set_attr "length"          "16")]
)

(define_expand "kvx_neg<suffix>"
  [(match_operand:V128J 0 "register_operand" "")
   (match_operand:V128J 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_neg<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssneg<mode>2 (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "abs<mode>2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (abs:V128J (match_operand:V128J 1 "register_operand" "r")))]
  ""
  "abs<chunkx> %x0 = %x1\n\tabs<chunkx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2_x")
   (set_attr "length"          "16")]
)

(define_insn_and_split "ssabs<mode>2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_abs:V128J (match_operand:V128J 1 "register_operand" "r")))]
  ""
  "#"
  ""
  [(set (match_dup 0)
        (ss_neg:V128J (match_dup 1)))
   (set (match_dup 0)
        (abs:V128J (match_dup 0)))]
  ""
)

(define_expand "kvx_abs<suffix>"
  [(match_operand:V128J 0 "register_operand" "")
   (match_operand:V128J 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_abs<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssabs<mode>2 (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "clrsb<mode>2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (clrsb:V128J (match_operand:V128J 1 "register_operand" "r")))]
  ""
  "cls<chunkx> %x0 = %x1\n\tcls<chunkx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "clz<mode>2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (clz:V128J (match_operand:V128J 1 "register_operand" "r")))]
  ""
  "clz<chunkx> %x0 = %x1\n\tclz<chunkx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "ctz<mode>2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ctz:V128J (match_operand:V128J 1 "register_operand" "r")))]
  ""
  "ctz<chunkx> %x0 = %x1\n\tctz<chunkx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "popcount<mode>2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (popcount:V128J (match_operand:V128J 1 "register_operand" "r")))]
  ""
  "cbs<chunkx> %x0 = %x1\n\tcbs<chunkx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_bitcnt<suffix>"
  [(match_operand:V128J 0 "register_operand" "")
   (match_operand:V128J 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_popcount<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 'l' && xstr[2] == 'z')
      emit_insn (gen_clz<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 'l' && xstr[2] == 's')
      emit_insn (gen_clrsb<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 't')
      emit_insn (gen_ctz<mode>2 (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "one_cmpl<mode>2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (not:V128J (match_operand:V128J 1 "register_operand" "r")))]
  ""
  "notd %x0 = %x1\n\tnotd %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "abd<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (abs:V128J (minus:V128J (match_operand:V128J 1 "register_operand" "r")
                                (match_operand:V128J 2 "register_operand" "r"))))]
  ""
  "abd<chunkx> %x0 = %x1, %x2\n\tabd<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*abd<suffix>_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (abs:V128J (minus:V128J (unspec:V128J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128)
                                (match_operand:V128J 2 "register_operand" "r"))))]
  ""
  "abd<chunkx> %x0 = %1, %x2\n\tabd<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*abd<suffix>_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (abs:V128J (minus:V128J (match_operand:V128J 1 "register_operand" "r")
                                (unspec:V128J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP128))))]
  ""
  "abd<chunkx> %x0 = %x1, %2\n\tabd<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "ssabd<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_abs:V128J (minus:V128J (match_operand:V128J 1 "register_operand" "r")
                                   (match_operand:V128J 2 "register_operand" "r"))))
   (clobber (match_scratch:V128J 3 "=&r"))
   (clobber (match_scratch:V128J 4 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3)
        (ss_minus:V128J (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (ss_minus:V128J (match_dup 2) (match_dup 1)))
   (set (match_dup 0)
        (smax:V128J (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
)

(define_expand "kvx_abd<suffix>"
  [(match_operand:V128J 0 "register_operand" "")
   (match_operand:V128J 1 "register_operand" "")
   (match_operand:V128J 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_abd<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssabd<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)


;; S128K (V16QI V8HI V4SI)

(define_expand "kvx_zx<widenx>"
  [(match_operand:<WIDE> 0 "register_operand")
   (match_operand:S128K 1 "register_operand")]
  ""
  {
    rtx op1_l = gen_rtx_SUBREG (<HALF>mode, operands[1], 0);
    rtx op1_m = gen_rtx_SUBREG (<HALF>mode, operands[1], 8);
    rtx op0_l = gen_rtx_SUBREG (<HWIDE>mode, operands[0], 0);
    rtx op0_m = gen_rtx_SUBREG (<HWIDE>mode, operands[0], 16);
    emit_insn (gen_kvx_zx<hwidenx> (op0_l, op1_l));
    emit_insn (gen_kvx_zx<hwidenx> (op0_m, op1_m));
    DONE;
  }
)

(define_expand "kvx_sx<widenx>"
  [(match_operand:<WIDE> 0 "register_operand")
   (match_operand:S128K 1 "register_operand")]
  ""
  {
    rtx op1_l = gen_rtx_SUBREG (<HALF>mode, operands[1], 0);
    rtx op1_m = gen_rtx_SUBREG (<HALF>mode, operands[1], 8);
    rtx op0_l = gen_rtx_SUBREG (<HWIDE>mode, operands[0], 0);
    rtx op0_m = gen_rtx_SUBREG (<HWIDE>mode, operands[0], 16);
    emit_insn (gen_kvx_sx<hwidenx> (op0_l, op1_l));
    emit_insn (gen_kvx_sx<hwidenx> (op0_m, op1_m));
    DONE;
  }
)

(define_expand "kvx_qx<widenx>"
  [(match_operand:<WIDE> 0 "register_operand")
   (match_operand:S128K 1 "register_operand")]
  ""
  {
    rtx op1_l = gen_rtx_SUBREG (<HALF>mode, operands[1], 0);
    rtx op1_m = gen_rtx_SUBREG (<HALF>mode, operands[1], 8);
    rtx op0_l = gen_rtx_SUBREG (<HWIDE>mode, operands[0], 0);
    rtx op0_m = gen_rtx_SUBREG (<HWIDE>mode, operands[0], 16);
    emit_insn (gen_kvx_qx<hwidenx> (op0_l, op1_l));
    emit_insn (gen_kvx_qx<hwidenx> (op0_m, op1_m));
    DONE;
  }
)

(define_expand "kvx_widen<widenx>"
  [(match_operand:<WIDE> 0 "register_operand" "")
   (match_operand:S128K 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_kvx_sx<widenx> (operands[0], operands[1]));
    else if (xstr[1] == 'z')
      emit_insn (gen_kvx_zx<widenx> (operands[0], operands[1]));
    else if (xstr[1] == 'q')
      emit_insn (gen_kvx_qx<widenx> (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_expand "kvx_trunc<truncx>"
  [(match_operand:S128K 0 "register_operand")
   (match_operand:<WIDE> 1 "register_operand")]
  ""
  {
    rtx op1_l = gen_rtx_SUBREG (<HWIDE>mode, operands[1], 0);
    rtx op1_m = gen_rtx_SUBREG (<HWIDE>mode, operands[1], 16);
    rtx op0_l = gen_rtx_SUBREG (<HALF>mode, operands[0], 0);
    rtx op0_m = gen_rtx_SUBREG (<HALF>mode, operands[0], 8);
    emit_insn (gen_kvx_trunc<htruncx> (op0_l, op1_l));
    emit_insn (gen_kvx_trunc<htruncx> (op0_m, op1_m));
    DONE;
  }
)

(define_expand "kvx_fract<truncx>"
  [(match_operand:S128K 0 "register_operand")
   (match_operand:<WIDE> 1 "register_operand")]
  ""
  {
    rtx op1_l = gen_rtx_SUBREG (<HWIDE>mode, operands[1], 0);
    rtx op1_m = gen_rtx_SUBREG (<HWIDE>mode, operands[1], 16);
    rtx op0_l = gen_rtx_SUBREG (<HALF>mode, operands[0], 0);
    rtx op0_m = gen_rtx_SUBREG (<HALF>mode, operands[0], 8);
    emit_insn (gen_kvx_fract<htruncx> (op0_l, op1_l));
    emit_insn (gen_kvx_fract<htruncx> (op0_m, op1_m));
    DONE;
  }
)

(define_expand "kvx_sat<truncx>"
  [(match_operand:S128K 0 "register_operand")
   (match_operand:<WIDE> 1 "register_operand")]
  ""
  {
    rtx op1_l = gen_rtx_SUBREG (<HWIDE>mode, operands[1], 0);
    rtx op1_m = gen_rtx_SUBREG (<HWIDE>mode, operands[1], 16);
    rtx op0_l = gen_rtx_SUBREG (<HALF>mode, operands[0], 0);
    rtx op0_m = gen_rtx_SUBREG (<HALF>mode, operands[0], 8);
    emit_insn (gen_kvx_sat<htruncx> (op0_l, op1_l));
    emit_insn (gen_kvx_sat<htruncx> (op0_m, op1_m));
    DONE;
  }
)

(define_expand "kvx_satu<truncx>"
  [(match_operand:S128K 0 "register_operand")
   (match_operand:<WIDE> 1 "register_operand")]
  ""
  {
    rtx op1_l = gen_rtx_SUBREG (<HWIDE>mode, operands[1], 0);
    rtx op1_m = gen_rtx_SUBREG (<HWIDE>mode, operands[1], 16);
    rtx op0_l = gen_rtx_SUBREG (<HALF>mode, operands[0], 0);
    rtx op0_m = gen_rtx_SUBREG (<HALF>mode, operands[0], 8);
    emit_insn (gen_kvx_satu<htruncx> (op0_l, op1_l));
    emit_insn (gen_kvx_satu<htruncx> (op0_m, op1_m));
    DONE;
  }
)

(define_expand "kvx_narrow<truncx>"
  [(match_operand:S128K 0 "register_operand" "")
   (match_operand:<WIDE> 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_kvx_trunc<truncx> (operands[0], operands[1]));
    else if (xstr[1] == 'q')
      emit_insn (gen_kvx_fract<truncx> (operands[0], operands[1]));
    else if (xstr[1] == 's')
      emit_insn (gen_kvx_sat<truncx> (operands[0], operands[1]));
    else if (xstr[1] == 'u')
      emit_insn (gen_kvx_satu<truncx> (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)


;; V128I (V8HI V2DI)

(define_insn_and_split "mul<mode>3"
  [(set (match_operand:V128I 0 "register_operand" "=r")
        (mult:V128I (match_operand:V128I 1 "register_operand" "r")
                    (match_operand:V128I 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (mult:<HALF> (subreg:<HALF> (match_dup 1) 0)
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 8)
        (mult:<HALF> (subreg:<HALF> (match_dup 1) 8)
                     (subreg:<HALF> (match_dup 2) 8)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "*mul<mode>3_s1"
  [(set (match_operand:V128I 0 "register_operand" "=r")
        (mult:V128I (unspec:V128I [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128)
                    (match_operand:V128I 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (mult:<HALF> (match_dup 1)
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 8)
        (mult:<HALF> (match_dup 1)
                     (subreg:<HALF> (match_dup 2) 8)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "*mul<mode>3_s2"
  [(set (match_operand:V128I 0 "register_operand" "=r")
        (mult:V128I (match_operand:V128I 1 "register_operand" "r")
                    (unspec:V128I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP128)))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (mult:<HALF> (subreg:<HALF> (match_dup 1) 0)
                     (match_dup 2)))
   (set (subreg:<HALF> (match_dup 0) 8)
        (mult:<HALF> (subreg:<HALF> (match_dup 1) 8)
                     (match_dup 2)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "rotl<mode>3"
  [(set (match_operand:V128I 0 "register_operand" "=r")
        (rotate:V128I (match_operand:V128I 1 "register_operand" "r")
                      (match_operand:SI 2 "register_operand" "r")))
   (clobber (match_scratch:SI 3 "=&r"))
   (clobber (match_scratch:V128I 4 "=&r"))
   (clobber (match_scratch:V128I 5 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3) (neg:SI (match_dup 2)))
   (set (match_dup 4) (ashift:V128I (match_dup 1) (match_dup 2)))
   (set (match_dup 5) (lshiftrt:V128I (match_dup 1) (match_dup 3)))
   (set (match_dup 0) (ior:V128I (match_dup 4) (match_dup 5)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (SImode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[5]) == SCRATCH)
      operands[5] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn_and_split "rotr<mode>3"
  [(set (match_operand:V128I 0 "register_operand" "=r")
        (rotatert:V128I (match_operand:V128I 1 "register_operand" "r")
                        (match_operand:SI 2 "register_operand" "r")))
   (clobber (match_scratch:SI 3 "=&r"))
   (clobber (match_scratch:V128I 4 "=&r"))
   (clobber (match_scratch:V128I 5 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3) (neg:SI (match_dup 2)))
   (set (match_dup 4) (lshiftrt:V128I (match_dup 1) (match_dup 2)))
   (set (match_dup 5) (ashift:V128I (match_dup 1) (match_dup 3)))
   (set (match_dup 0) (ior:V128I (match_dup 4) (match_dup 5)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (SImode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[5]) == SCRATCH)
      operands[5] = gen_reg_rtx (<MODE>mode);
  }
)


;; V128J (V8HI V4SI V2DI)

(define_expand "kvx_shl<suffix>s"
  [(match_operand:V128J 0 "register_operand" "")
   (match_operand:V128J 1 "register_operand" "")
   (match_operand:SI 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    operands[2] = force_reg (SImode, operands[2]);
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_ashl<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssashl<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen_usashl<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r')
      emit_insn (gen_rotl<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_expand "kvx_shr<suffix>s"
  [(match_operand:V128J 0 "register_operand" "")
   (match_operand:V128J 1 "register_operand" "")
   (match_operand:SI 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    operands[2] = force_reg (SImode, operands[2]);
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_lshr<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'a' && !xstr[2])
      emit_insn (gen_ashr<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'a' && xstr[2] == 'r')
      emit_insn (gen_kvx_srs<suffix>s (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r')
      emit_insn (gen_rotr<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
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

(define_insn "rotlv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r,r")
        (rotate:V4SI (match_operand:V4SI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "rotate_operand" "r,U05")))]
  ""
  "rolwps %x0 = %x1, %2\n\trolwps %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "rotrv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r,r")
        (rotatert:V4SI (match_operand:V4SI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "rotate_operand" "r,U05")))]
  ""
  "rorwps %x0 = %x1, %2\n\trorwps %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
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

(define_insn_and_split "*compndps"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "comparison_operator"
         [(match_operand:V2DI 2 "register_operand" "r")
          (unspec:V2DI [(match_operand:DI 3 "nonmemory_operand" "r")] UNSPEC_DUP128)]))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (unspec:V2DI [(match_op_dup 1 [(match_dup 2)
                                       (unspec:V2DI [(match_dup 3)] UNSPEC_DUP128)])] UNSPEC_COMP128))
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

(define_insn "*compdps"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operator:V2DI 1 "comparison_operator"
                       [(match_operand:V2DI 2 "register_operand" "r")
                        (unspec:V2DI [(match_operand:DI 3 "nonmemory_operand" "r")] UNSPEC_DUP128)])] UNSPEC_COMP128))]
  ""
  "compd.%1 %x0 = %x2, %3\n\tcompd.%1 %y0 = %y2, %3"
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
                      (match_operand 4 "" "")] UNSPEC_SELECT128))]
  ""
  "cmoved%4 %x3? %x0 = %x1\n\tcmoved%4 %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
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

(define_insn_and_split "usashlv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r,r")
        (us_ashift:V2DI (match_operand:V2DI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:V2DI 3 "=&r,&r"))
   (clobber (match_scratch:V2DI 4 "=&r,&r"))
   (clobber (match_scratch:V2DI 5 "=&r,&r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 3)
        (ashift:V2DI (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (lshiftrt:V2DI (match_dup 3) (match_dup 2)))
   (set (match_dup 5)
        (ne:V2DI (match_dup 4) (match_dup 1)))
   (set (match_dup 0)
        (ior:V2DI (match_dup 3) (match_dup 5)))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
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
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRS128))]
  ""
  "srsd %x0 = %x1, %2\n\tsrsd %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)


;; S256I (V16HI V8SI)

(define_insn "*compn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S256I 2 "register_operand" "r")
          (match_operand:S256I 3 "register_operand" "r")]))]
  ""
  {
    return "compn<chunkx>.%1 %x0 = %x2, %x3\n\tcompn<chunkx>.%1 %y0 = %y2, %y3\n\t"
           "compn<chunkx>.%1 %z0 = %z2, %z3\n\tcompn<chunkx>.%1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*compn<suffix>s"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S256I 2 "register_operand" "r")
          (unspec:S256I [(match_operand:<CHUNK> 3 "nonmemory_operand" "r")] UNSPEC_DUP256)]))]
  ""
  {
    return "compn<chunkx>.%1 %x0 = %x2, %3\n\tcompn<chunkx>.%1 %y0 = %y2, %3\n\t"
           "compn<chunkx>.%1 %z0 = %z2, %3\n\tcompn<chunkx>.%1 %t0 = %t2, %3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "kvx_select<suffix>"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "0")
                       (match_operand:S256I 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECT256))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)
                        (subreg:<HALF> (match_dup 3) 0)
                        (match_dup 4)] UNSPEC_SELECT128))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)
                        (subreg:<HALF> (match_dup 3) 16)
                        (match_dup 4)] UNSPEC_SELECT128))]
  ""
  [(set_attr "type" "alu_lite_x2")]
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

(define_insn_and_split "usashl<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=r,r")
        (us_ashift:S256I (match_operand:S256I 1 "register_operand" "r,r")
                         (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:S256I 3 "=&r,&r"))
   (clobber (match_scratch:S256I 4 "=&r,&r"))
   (clobber (match_scratch:S256I 5 "=&r,&r"))
   (clobber (match_scratch:SI 6 "=&r,X"))
   (clobber (match_scratch:SI 7 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(parallel [
     (set (match_dup 3)
         (ashift:S256I (match_dup 1) (match_dup 2)))
     (clobber (match_dup 6))])
   (parallel [
    (set (match_dup 4)
         (lshiftrt:S256I (match_dup 3) (match_dup 2)))
    (clobber (match_dup 7))])
   (set (match_dup 5)
        (ne:S256I (match_dup 4) (match_dup 1)))
   (set (match_dup 0)
        (ior:S256I (match_dup 3) (match_dup 5)))]
  ""
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
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRS256))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (match_dup 2)] UNSPEC_SRS128))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (match_dup 2)] UNSPEC_SRS128))]
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

(define_expand "kvx_avg<suffix>"
  [(match_operand:S256I 0 "register_operand" "")
   (match_operand:S256I 1 "register_operand" "")
   (match_operand:S256I 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_avg<mode>3_floor (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r' && !xstr[2])
      emit_insn (gen_avg<mode>3_ceil (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u' && !xstr[2])
      emit_insn (gen_uavg<mode>3_floor (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r' && xstr[2] == 'u')
      emit_insn (gen_uavg<mode>3_ceil (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)


;; V256I (V16HI V4DI)

(define_insn_and_split "mul<mode>3"
  [(set (match_operand:V256I 0 "register_operand" "=r")
        (mult:V256I (match_operand:V256I 1 "register_operand" "r")
                    (match_operand:V256I 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 0)
                      (subreg:<CHUNK> (match_dup 2) 0)))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 8)
                      (subreg:<CHUNK> (match_dup 2) 8)))
   (set (subreg:<CHUNK> (match_dup 0) 16)
        (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 16)
                      (subreg:<CHUNK> (match_dup 2) 16)))
   (set (subreg:<CHUNK> (match_dup 0) 24)
        (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 24)
                      (subreg:<CHUNK> (match_dup 2) 24)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "*mul<mode>3_s1"
  [(set (match_operand:V256I 0 "register_operand" "=r")
        (mult:V256I (unspec:V256I [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256)
                    (match_operand:V256I 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (mult:<CHUNK> (match_dup 1)
                      (subreg:<CHUNK> (match_dup 2) 0)))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (mult:<CHUNK> (match_dup 1)
                      (subreg:<CHUNK> (match_dup 2) 8)))
   (set (subreg:<CHUNK> (match_dup 0) 16)
        (mult:<CHUNK> (match_dup 1)
                      (subreg:<CHUNK> (match_dup 2) 16)))
   (set (subreg:<CHUNK> (match_dup 0) 24)
        (mult:<CHUNK> (match_dup 1)
                      (subreg:<CHUNK> (match_dup 2) 24)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "*mul<mode>3_s2"
  [(set (match_operand:V256I 0 "register_operand" "=r")
        (mult:V256I (match_operand:V256I 1 "register_operand" "r")
                    (unspec:V256I [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP256)))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 0)
                      (match_dup 2)))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 8)
                      (match_dup 2)))
   (set (subreg:<CHUNK> (match_dup 0) 16)
        (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 16)
                      (match_dup 2)))
   (set (subreg:<CHUNK> (match_dup 0) 24)
        (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 24)
                      (match_dup 2)))]
  ""
  [(set_attr "type" "mau_auxr")]
)


;; V256J (V16HI V8SI V4DI)

(define_insn "add<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (plus:V256J (match_operand:V256J 1 "register_operand" "r")
                    (match_operand:V256J 2 "nonmemory_operand" "r")))]
  ""
  {
    return "add<chunkx> %x0 = %x1, %x2\n\tadd<chunkx> %y0 = %y1, %y2\n\t"
           "add<chunkx> %z0 = %z1, %z2\n\tadd<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*add<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (plus:V256J (unspec:V256J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256)
                    (match_operand:V256J 2 "register_operand" "r")))]
  ""
  {
    return "add<chunkx> %x0 = %1, %x2\n\tadd<chunkx> %y0 = %1, %y2\n\t"
           "add<chunkx> %z0 = %1, %z2\n\tadd<chunkx> %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*add<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (plus:V256J (match_operand:V256J 1 "register_operand" "r")
                    (unspec:V256J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP256)))]
  ""
  {
    return "add<chunkx> %x0 = %x1, %2\n\tadd<chunkx> %y0 = %y1, %2\n\t"
           "add<chunkx> %z0 = %z1, %2\n\tadd<chunkx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "ssadd<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_plus:V256J (match_operand:V256J 1 "register_operand" "r")
                       (match_operand:V256J 2 "nonmemory_operand" "r")))]
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

(define_insn_and_split "*ssadd<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_plus:V256J (unspec:V256J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256)
                       (match_operand:V256J 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_plus:<HALF> (unspec:<HALF> [(match_dup 1)] UNSPEC_DUP128)
                        (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_plus:<HALF> (unspec:<HALF> [(match_dup 1)] UNSPEC_DUP128)
                        (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*ssadd<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_plus:V256J (match_operand:V256J 1 "register_operand" "r")
                       (unspec:V256J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP256)))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_plus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                        (unspec:<HALF> [(match_dup 2)] UNSPEC_DUP128)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_plus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                        (unspec:<HALF> [(match_dup 2)] UNSPEC_DUP128)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "usadd<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (us_plus:V256J (match_operand:V256J 1 "register_operand" "r")
                       (match_operand:V256J 2 "nonmemory_operand" "r")))
   (clobber (match_scratch:V256J 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 3)
        (plus:V256J (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (ltu:V256J (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (ior:V256J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*usadd<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (us_plus:V256J (match_operand:V256J 1 "register_operand" "r")
                       (unspec:V256J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP256)))
   (clobber (match_scratch:V256J 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 3)
        (plus:V256J (match_dup 1) (unspec:V256J [(match_dup 2)] UNSPEC_DUP256)))
   (set (match_dup 4)
        (ltu:V256J (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (ior:V256J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx2<suffix>"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (plus:V256J (ashift:V256J (match_operand:V256J 1 "register_operand" "r")
                                  (const_int 1))
                    (match_operand:V256J 2 "nonmemory_operand" "r")))]
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

(define_insn_and_split "*addx4<suffix>"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (plus:V256J (ashift:V256J (match_operand:V256J 1 "register_operand" "r")
                                  (const_int 2))
                    (match_operand:V256J 2 "nonmemory_operand" "r")))]
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

(define_insn_and_split "*addx8<suffix>"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (plus:V256J (ashift:V256J (match_operand:V256J 1 "register_operand" "r")
                                  (const_int 3))
                    (match_operand:V256J 2 "nonmemory_operand" "r")))]
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

(define_insn_and_split "*addx16<suffix>"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (plus:V256J (ashift:V256J (match_operand:V256J 1 "register_operand" "r")
                                  (const_int 4))
                    (match_operand:V256J 2 "nonmemory_operand" "r")))]
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

(define_expand "kvx_add<suffix>"
  [(match_operand:V256J 0 "register_operand" "")
   (match_operand:V256J 1 "register_operand" "")
   (match_operand:V256J 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_add<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssadd<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen_usadd<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "sub<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (minus:V256J (match_operand:V256J 1 "nonmemory_operand" "r")
                     (match_operand:V256J 2 "register_operand" "r")))]
  ""
  {
    return "sbf<chunkx> %x0 = %x2, %x1\n\tsbf<chunkx> %y0 = %y2, %y1\n\t"
           "sbf<chunkx> %z0 = %z2, %z1\n\tsbf<chunkx> %t0 = %t2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*sub<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (minus:V256J (unspec:V256J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256)
                     (match_operand:V256J 2 "register_operand" "r")))]
  ""
  {
    return "sbf<chunkx> %x0 = %x2, %1\n\tsbf<chunkx> %y0 = %y2, %1\n\t"
           "sbf<chunkx> %z0 = %z2, %1\n\tsbf<chunkx> %t0 = %t2, %1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*sub<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (minus:V256J (match_operand:V256J 1 "register_operand" "r")
                     (unspec:V256J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP256)))]
  ""
  {
    return "sbf<chunkx> %x0 = %2, %x1\n\tsbf<chunkx> %y0 = %2, %y1\n\t"
           "sbf<chunkx> %z0 = %2, %z1\n\tsbf<chunkx> %t0 = %2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "sssub<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_minus:V256J (match_operand:V256J 1 "nonmemory_operand" "r")
                        (match_operand:V256J 2 "register_operand" "r")))]
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

(define_insn_and_split "*sssub<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_minus:V256J (unspec:V256J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256)
                        (match_operand:V256J 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_minus:<HALF> (unspec:<HALF> [(match_dup 1)] UNSPEC_DUP128)
                         (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_minus:<HALF> (unspec:<HALF> [(match_dup 1)] UNSPEC_DUP128)
                         (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sssub<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_minus:V256J (match_operand:V256J 1 "register_operand" "r")
                        (unspec:V256J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP256)))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                         (unspec:<HALF> [(match_dup 2)] UNSPEC_DUP128)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                         (unspec:<HALF> [(match_dup 2)] UNSPEC_DUP128)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "ussub<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (us_minus:V256J (match_operand:V256J 1 "nonmemory_operand" "r")
                        (match_operand:V256J 2 "register_operand" "r")))
   (clobber (match_scratch:V256J 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 3)
        (minus:V256J (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (leu:V256J (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (and:V256J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*ussub<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (us_minus:V256J (unspec:V256J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256)
                        (match_operand:V256J 2 "register_operand" "r")))
   (clobber (match_scratch:V256J 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 3)
        (minus:V256J (unspec:V256J [(match_dup 1)] UNSPEC_DUP256) (match_dup 2)))
   (set (match_dup 4)
        (leu:V256J (match_dup 3) (unspec:V256J [(match_dup 1)] UNSPEC_DUP256)))
   (set (match_dup 0)
        (and:V256J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx2<suffix>"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (minus:V256J (match_operand:V256J 1 "nonmemory_operand" "r")
                     (ashift:V256J (match_operand:V256J 2 "register_operand" "r")
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

(define_insn_and_split "*sbfx4<suffix>"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (minus:V256J (match_operand:V256J 1 "nonmemory_operand" "r")
                     (ashift:V256J (match_operand:V256J 2 "register_operand" "r")
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

(define_insn_and_split "*sbfx8<suffix>"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (minus:V256J (match_operand:V256J 1 "nonmemory_operand" "r")
                     (ashift:V256J (match_operand:V256J 2 "register_operand" "r")
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

(define_insn_and_split "*sbfx16<suffix>"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (minus:V256J (match_operand:V256J 1 "nonmemory_operand" "r")
                     (ashift:V256J (match_operand:V256J 2 "register_operand" "r")
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

(define_expand "kvx_sbf<suffix>"
  [(match_operand:V256J 0 "register_operand" "")
   (match_operand:V256J 1 "register_operand" "")
   (match_operand:V256J 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_sub<mode>3 (operands[0], operands[2], operands[1]));
    else if (xstr[1] == 's')
      emit_insn (gen_sssub<mode>3 (operands[0], operands[2], operands[1]));
    else if (xstr[1] == 'u')
      emit_insn (gen_ussub<mode>3 (operands[0], operands[2], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "smin<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (smin:V256J (match_operand:V256J 1 "register_operand" "r")
                    (match_operand:V256J 2 "nonmemory_operand" "r")))]
  ""
  {
    return "min<chunkx> %x0 = %x1, %x2\n\tmin<chunkx> %y0 = %y1, %y2\n\t"
           "min<chunkx> %z0 = %z1, %z2\n\tmin<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*smin<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (smin:V256J (unspec:V256J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256)
                    (match_operand:V256J 2 "register_operand" "r")))]
  ""
  {
    return "min<chunkx> %x0 = %1, %x2\n\tmin<chunkx> %y0 = %1, %y2\n\t"
           "min<chunkx> %z0 = %1, %z2\n\tmin<chunkx> %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*smin<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (smin:V256J (match_operand:V256J 1 "register_operand" "r")
                    (unspec:V256J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP256)))]
  ""
  {
    return "min<chunkx> %x0 = %x1, %2\n\tmin<chunkx> %y0 = %y1, %2\n\t"
           "min<chunkx> %z0 = %z1, %2\n\tmin<chunkx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "smax<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (smax:V256J (match_operand:V256J 1 "register_operand" "r")
                    (match_operand:V256J 2 "nonmemory_operand" "r")))]
  ""
  {
    return "max<chunkx> %x0 = %x1, %x2\n\tmax<chunkx> %y0 = %y1, %y2\n\t"
           "max<chunkx> %z0 = %z1, %z2\n\tmax<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*smax<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (smax:V256J (unspec:V256J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256)
                    (match_operand:V256J 2 "register_operand" "r")))]
  ""
  {
    return "max<chunkx> %x0 = %1, %x2\n\tmax<chunkx> %y0 = %1, %y2\n\t"
           "max<chunkx> %z0 = %1, %z2\n\tmax<chunkx> %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*smax<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (smax:V256J (match_operand:V256J 1 "register_operand" "r")
                    (unspec:V256J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP256)))]
  ""
  {
    return "max<chunkx> %x0 = %x1, %2\n\tmax<chunkx> %y0 = %y1, %2\n\t"
           "max<chunkx> %z0 = %z1, %2\n\tmax<chunkx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "umin<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (umin:V256J (match_operand:V256J 1 "register_operand" "r")
                    (match_operand:V256J 2 "nonmemory_operand" "r")))]
  ""
  {
    return "minu<chunkx> %x0 = %x1, %x2\n\tminu<chunkx> %y0 = %y1, %y2\n\t"
           "minu<chunkx> %z0 = %z1, %z2\n\tminu<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*umin<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (umin:V256J (unspec:V256J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256)
                    (match_operand:V256J 2 "register_operand" "r")))]
  ""
  {
    return "minu<chunkx> %x0 = %1, %x2\n\tminu<chunkx> %y0 = %1, %y2\n\t"
           "minu<chunkx> %z0 = %1, %z2\n\tminu<chunkx> %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*umin<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (umin:V256J (match_operand:V256J 1 "register_operand" "r")
                    (unspec:V256J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP256)))]
  ""
  {
    return "minu<chunkx> %x0 = %x1, %2\n\tminu<chunkx> %y0 = %y1, %2\n\t"
           "minu<chunkx> %z0 = %z1, %2\n\tminu<chunkx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "umax<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (umax:V256J (match_operand:V256J 1 "register_operand" "r")
                    (match_operand:V256J 2 "nonmemory_operand" "r")))]
  ""
  {
    return "maxu<chunkx> %x0 = %x1, %x2\n\tmaxu<chunkx> %y0 = %y1, %y2\n\t"
           "maxu<chunkx> %z0 = %z1, %z2\n\tmaxu<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*umax<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (umax:V256J (unspec:V256J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256)
                    (match_operand:V256J 2 "register_operand" "r")))]
  ""
  {
    return "maxu<chunkx> %x0 = %1, %x2\n\tmaxu<chunkx> %y0 = %1, %y2\n\t"
           "maxu<chunkx> %z0 = %1, %z2\n\tmaxu<chunkx> %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*umax<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (umax:V256J (match_operand:V256J 1 "register_operand" "r")
                    (unspec:V256J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP256)))]
  ""
  {
    return "maxu<chunkx> %x0 = %x1, %2\n\tmaxu<chunkx> %y0 = %y1, %2\n\t"
           "maxu<chunkx> %z0 = %z1, %2\n\tmaxu<chunkx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "and<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (and:V256J (match_operand:V256J 1 "register_operand" "r")
                   (match_operand:V256J 2 "nonmemory_operand" "r")))]
  ""
  {
    return "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2\n\t"
           "andd %z0 = %z1, %z2\n\tandd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*nand<suffix>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ior:V256J (not:V256J (match_operand:V256J 1 "register_operand" "r"))
                   (not:V256J (match_operand:V256J 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nandd %x0 = %x1, %x2\n\tnandd %y0 = %y1, %y2\n\t"
           "nandd %z0 = %z1, %z2\n\tnandd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*andn<suffix>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (and:V256J (not:V256J (match_operand:V256J 1 "register_operand" "r"))
                   (match_operand:V256J 2 "nonmemory_operand" "r")))]
  ""
  {
    return "andnd %x0 = %x1, %x2\n\tandnd %y0 = %y1, %y2\n\t"
           "andnd %z0 = %z1, %z2\n\tandnd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "ior<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ior:V256J (match_operand:V256J 1 "register_operand" "r")
                   (match_operand:V256J 2 "nonmemory_operand" "r")))]
  ""
  {
    return "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2\n\t"
           "ord %z0 = %z1, %z2\n\tord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*nior<suffix>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (and:V256J (not:V256J (match_operand:V256J 1 "register_operand" "r"))
                   (not:V256J (match_operand:V256J 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nord %x0 = %x1, %x2\n\tnord %y0 = %y1, %y2\n\t"
           "nord %z0 = %z1, %z2\n\tnord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*iorn<suffix>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ior:V256J (not:V256J (match_operand:V256J 1 "register_operand" "r"))
                   (match_operand:V256J 2 "nonmemory_operand" "r")))]
  ""
  {
    return "ornd %x0 = %x1, %x2\n\tornd %y0 = %y1, %y2\n\t"
           "ornd %z0 = %z1, %z2\n\tornd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "xor<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (xor:V256J (match_operand:V256J 1 "register_operand" "r")
                   (match_operand:V256J 2 "nonmemory_operand" "r")))]
  ""
  {
    return "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2\n\t"
           "xord %z0 = %z1, %z2\n\txord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*nxor<suffix>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (not:V256J (xor:V256J (match_operand:V256J 1 "register_operand" "r")
                              (match_operand:V256J 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nxord %x0 = %x1, %x2\n\tnxord %y0 = %y1, %y2\n\t"
           "nxord %z0 = %z1, %z2\n\tnxord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "madd<mode><mode>4"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (plus:V256J (mult:V256J (match_operand:V256J 1 "register_operand" "r")
                                (match_operand:V256J 2 "nonmemory_operand" "r"))
                    (match_operand:V256J 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (plus:<CHUNK> (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 0)
                                    (subreg:<CHUNK> (match_dup 2) 0))
                      (subreg:<CHUNK> (match_dup 3) 0)))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (plus:<CHUNK> (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 8)
                                    (subreg:<CHUNK> (match_dup 2) 8))
                      (subreg:<CHUNK> (match_dup 3) 8)))
   (set (subreg:<CHUNK> (match_dup 0) 16)
        (plus:<CHUNK> (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 16)
                                    (subreg:<CHUNK> (match_dup 2) 16))
                      (subreg:<CHUNK> (match_dup 3) 16)))
   (set (subreg:<CHUNK> (match_dup 0) 24)
        (plus:<CHUNK> (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 24)
                                    (subreg:<CHUNK> (match_dup 2) 24))
                      (subreg:<CHUNK> (match_dup 3) 24)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "msub<mode><mode>4"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (minus:V256J (match_operand:V256J 3 "register_operand" "0")
                     (mult:V256J (match_operand:V256J 1 "register_operand" "r")
                                 (match_operand:V256J 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (minus:<CHUNK> (subreg:<CHUNK> (match_dup 3) 0)
                       (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 0)
                                     (subreg:<CHUNK> (match_dup 2) 0))))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (minus:<CHUNK> (subreg:<CHUNK> (match_dup 3) 8)
                       (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 8)
                                     (subreg:<CHUNK> (match_dup 2) 8))))
   (set (subreg:<CHUNK> (match_dup 0) 16)
        (minus:<CHUNK> (subreg:<CHUNK> (match_dup 3) 16)
                       (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 16)
                                     (subreg:<CHUNK> (match_dup 2) 16))))
   (set (subreg:<CHUNK> (match_dup 0) 24)
        (minus:<CHUNK> (subreg:<CHUNK> (match_dup 3) 24)
                       (mult:<CHUNK> (subreg:<CHUNK> (match_dup 1) 24)
                                     (subreg:<CHUNK> (match_dup 2) 24))))]
  ""
  [(set_attr "type" "mau_auxr")
   (set_attr "length"      "8")]
)

(define_insn_and_split "rotl<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (rotate:V256J (match_operand:V256J 1 "register_operand" "r")
                      (match_operand:SI 2 "register_operand" "r")))
   (clobber (match_scratch:SI 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))
   (clobber (match_scratch:V256J 5 "=&r"))
   (clobber (match_scratch:SI 6 "=&r"))
   (clobber (match_scratch:SI 7 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3) (neg:SI (match_dup 2)))
   (parallel [
     (set (match_dup 4) (ashift:V256J (match_dup 1) (match_dup 2)))
     (clobber (match_dup 6))])
   (parallel [
     (set (match_dup 5) (lshiftrt:V256J (match_dup 1) (match_dup 3)))
     (clobber (match_dup 7))])
   (set (match_dup 0) (ior:V256J (match_dup 4) (match_dup 5)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (SImode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[5]) == SCRATCH)
      operands[5] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn_and_split "rotr<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (rotatert:V256J (match_operand:V256J 1 "register_operand" "r")
                        (match_operand:SI 2 "register_operand" "r")))
   (clobber (match_scratch:SI 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))
   (clobber (match_scratch:V256J 5 "=&r"))
   (clobber (match_scratch:SI 6 "=&r"))
   (clobber (match_scratch:SI 7 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3) (neg:SI (match_dup 2)))
   (parallel [
     (set (match_dup 4) (lshiftrt:V256J (match_dup 1) (match_dup 2)))
     (clobber (match_dup 6))])
   (parallel [
     (set (match_dup 5) (ashift:V256J (match_dup 1) (match_dup 3)))
     (clobber (match_dup 7))])
   (set (match_dup 0) (ior:V256J (match_dup 4) (match_dup 5)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (SImode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[5]) == SCRATCH)
      operands[5] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "neg<mode>2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (neg:V256J (match_operand:V256J 1 "register_operand" "r")))]
  ""
  {
    return "neg<chunkx> %x0 = %x1\n\tneg<chunkx> %y0 = %y1\n\t"
           "neg<chunkx> %z0 = %z1\n\tneg<chunkx> %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4_x")
   (set_attr "length"          "32")]
)

(define_insn_and_split "ssneg<mode>2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_neg:V256J (match_operand:V256J 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_neg:<HALF> (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_neg:<HALF> (subreg:<HALF> (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2_x")]
)

(define_expand "kvx_neg<suffix>"
  [(match_operand:V256J 0 "register_operand" "")
   (match_operand:V256J 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_neg<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssneg<mode>2 (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "abs<mode>2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (abs:V256J (match_operand:V256J 1 "register_operand" "r")))]
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

(define_insn_and_split "ssabs<mode>2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_abs:V256J (match_operand:V256J 1 "register_operand" "r")))]
  ""
  "#"
  ""
  [(set (match_dup 0)
        (ss_neg:V256J (match_dup 1)))
   (set (match_dup 0)
        (abs:V256J (match_dup 0)))]
  ""
)

(define_expand "kvx_abs<suffix>"
  [(match_operand:V256J 0 "register_operand" "")
   (match_operand:V256J 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_abs<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssabs<mode>2 (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "clrsb<mode>2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (clrsb:V256J (match_operand:V256J 1 "register_operand" "r")))]
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
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (clz:V256J (match_operand:V256J 1 "register_operand" "r")))]
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
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ctz:V256J (match_operand:V256J 1 "register_operand" "r")))]
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
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (popcount:V256J (match_operand:V256J 1 "register_operand" "r")))]
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

(define_expand "kvx_bitcnt<suffix>"
  [(match_operand:V256J 0 "register_operand" "")
   (match_operand:V256J 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      emit_insn (gen_popcount<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 'l' && xstr[2] == 'z')
      emit_insn (gen_clz<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 'l' && xstr[2] == 's')
      emit_insn (gen_clrsb<mode>2 (operands[0], operands[1]));
    else if (xstr[1] == 't')
      emit_insn (gen_ctz<mode>2 (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "one_cmpl<mode>2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (not:V256J (match_operand:V256J 1 "register_operand" "r")))]
  ""
  {
    return "notd %x0 = %x1\n\tnotd %y0 = %y1\n\t"
           "notd %z0 = %z1\n\tnotd %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "abd<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (abs:V256J (minus:V256J (match_operand:V256J 1 "register_operand" "r")
                                (match_operand:V256J 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (abs:<HALF> (minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                  (subreg:<HALF> (match_dup 2) 0))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (abs:<HALF> (minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                  (subreg:<HALF> (match_dup 2) 16))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*abd<suffix>_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (abs:V256J (minus:V256J (unspec:V256J [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256)
                                (match_operand:V256J 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (abs:<HALF> (minus:<HALF> (unspec:<HALF> [(match_dup 1)] UNSPEC_DUP128)
                                  (subreg:<HALF> (match_dup 2) 0))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (abs:<HALF> (minus:<HALF> (unspec:<HALF> [(match_dup 1)] UNSPEC_DUP128)
                                  (subreg:<HALF> (match_dup 2) 16))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*abd<suffix>_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (abs:V256J (minus:V256J (match_operand:V256J 1 "register_operand" "r")
                                (unspec:V256J [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP256))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (abs:<HALF> (minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                  (unspec:<HALF> [(match_dup 2)] UNSPEC_DUP128))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (abs:<HALF> (minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                  (unspec:<HALF> [(match_dup 2)] UNSPEC_DUP128))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "ssabd<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_abs:V256J (minus:V256J (match_operand:V256J 1 "register_operand" "r")
                                   (match_operand:V256J 2 "register_operand" "r"))))
   (clobber (match_scratch:V256J 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3)
        (ss_minus:V256J (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (ss_minus:V256J (match_dup 2) (match_dup 1)))
   (set (match_dup 0)
        (smax:V256J (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
)

(define_expand "kvx_abd<suffix>"
  [(match_operand:V256J 0 "register_operand" "")
   (match_operand:V256J 1 "register_operand" "")
   (match_operand:V256J 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_abd<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssabd<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_expand "kvx_shl<suffix>s"
  [(match_operand:V256J 0 "register_operand" "")
   (match_operand:V256J 1 "register_operand" "")
   (match_operand:SI 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    operands[2] = force_reg (SImode, operands[2]);
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_ashl<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen_ssashl<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen_usashl<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r')
      emit_insn (gen_rotl<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_expand "kvx_shr<suffix>s"
  [(match_operand:V256J 0 "register_operand" "")
   (match_operand:V256J 1 "register_operand" "")
   (match_operand:SI 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    operands[2] = force_reg (SImode, operands[2]);
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_lshr<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'a' && !xstr[2])
      emit_insn (gen_ashr<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'a' && xstr[2] == 'r')
      emit_insn (gen_kvx_srs<suffix>s (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r')
      emit_insn (gen_rotr<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)


;; V16HI

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


;; V8SI

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

(define_insn_and_split "*compndqn"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (match_operator:V4DI 1 "comparison_operator"
         [(match_operand:V4DI 2 "register_operand" "r")
          (unspec:V4DI [(match_operand:DI 3 "nonmemory_operand" "r")] UNSPEC_DUP256)]))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (unspec:V4DI [(match_op_dup 1 [(match_dup 2)
                                       (unspec:V4DI [(match_dup 3)] UNSPEC_DUP256)])] UNSPEC_COMP256))
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

(define_insn "*compdq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operator:V4DI 1 "comparison_operator"
                       [(match_operand:V4DI 2 "register_operand" "r")
                        (unspec: V4DI [(match_operand:DI 3 "nonmemory_operand" "r")] UNSPEC_DUP256)])] UNSPEC_COMP256))]
  ""
  {
    return "compd.%1 %x0 = %x2, %3\n\tcompd.%1 %y0 = %y2, %3\n\t"
           "compd.%1 %z0 = %z2, %3\n\tcompd.%1 %t0 = %t2, %3";
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
                      (match_operand 4 "" "")] UNSPEC_SELECT256))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 0)
                      (subreg:V2DI (match_dup 2) 0)
                      (subreg:V2DI (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_SELECT128))
   (set (subreg:V2DI (match_dup 0) 16)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 16)
                      (subreg:V2DI (match_dup 2) 16)
                      (subreg:V2DI (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_SELECT128))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "ashlv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r,r")
        (ashift:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
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

(define_insn_and_split "usashlv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r,r")
        (us_ashift:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:V4DI 3 "=&r,&r"))
   (clobber (match_scratch:V4DI 4 "=&r,&r"))
   (clobber (match_scratch:V4DI 5 "=&r,&r"))
   (clobber (match_scratch:SI 6 "=&r,X"))
   (clobber (match_scratch:SI 7 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(parallel [
     (set (match_dup 3)
          (ashift:V4DI (match_dup 1) (match_dup 2)))
     (clobber (match_dup 6))])
   (parallel [
     (set (match_dup 4)
        (lshiftrt:V4DI (match_dup 3) (match_dup 2)))
     (clobber (match_dup 7))])
   (set (match_dup 5)
        (ne:V4DI (match_dup 4) (match_dup 1)))
   (set (match_dup 0)
        (ior:V4DI (match_dup 3) (match_dup 5)))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn "ashrv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r,r")
        (ashiftrt:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
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
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
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
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRS256))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 0)
                      (match_dup 2)] UNSPEC_SRS128))
   (set (subreg:V2DI (match_dup 0) 16)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 16)
                      (match_dup 2)] UNSPEC_SRS128))]
  {
    if (GET_CODE (operands[2]) == REG)
      {
        emit_move_insn (operands[3], operands[2]);
        operands[2] = operands[3];
      }
  }
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)


;; S64F (V4HF V2SF)

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
                      (match_operand 4 "" "")] UNSPEC_SELECT64))]
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
                      (match_operand 3 "" "")] UNSPEC_FADD64))]
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
                      (match_operand 3 "" "")] UNSPEC_FSBF64))]
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
                      (match_operand 3 "" "")] UNSPEC_FMUL64))]
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
                      (match_operand 4 "" "")] UNSPEC_FFMA64))]
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
                      (match_operand 4 "" "")] UNSPEC_FFMS64))]
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

(define_insn "kvx_fmul<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64F 1 "register_operand" "r")
                        (match_operand:S64F 2 "register_operand" "r")
                        (match_operand 3 "" "")] UNSPEC_FMULE64))]
  ""
  "fmul<widenx>%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_ffma<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64F 1 "register_operand" "r")
                        (match_operand:S64F 2 "register_operand" "r")
                        (match_operand:<WIDE> 3 "register_operand" "0")
                        (match_operand 4 "" "")] UNSPEC_FFMAE64))]
  ""
  "ffma<widenx>%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_ffms<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64F 1 "register_operand" "r")
                        (match_operand:S64F 2 "register_operand" "r")
                        (match_operand:<WIDE> 3 "register_operand" "0")
                        (match_operand 4 "" "")] UNSPEC_FFMSE64))]
  ""
  "ffms<widenx>%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_fwiden<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64F 1 "register_operand" "r")
                        (match_operand 2 "" "")] UNSPEC_FWIDEN64))]
  ""
  "fwidenl<hwidenx>%2 %x0 = %1\n\tfwidenm<hwidenx>%2 %y0 = %1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_fnarrow<truncx>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:<WIDE> 1 "register_operand" "r")
                      (match_operand 2 "" "")] UNSPEC_FNARROW64))]
  ""
  "fnarrow<truncx>%2 %0 = %1"
  [(set_attr "type" "<fnarrowt>")]
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
    if (KV3_1)
      {
        emit_insn (gen_kvx_fdot2w (operands[0], operands[1], operands[2], operands[3]));
        DONE;
      }
  }
)

(define_insn "*kvx_ffdmaw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FFDMAW))]
  "KV3_2"
  "ffdmaw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_ffdmsw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand 3 "" "")] UNSPEC_FFDMSW))]
  ""
  {
    if (KV3_1)
      {
        rtx fconj = gen_reg_rtx (V2SFmode);
        emit_insn (gen_kvx_fconjwc (fconj, operands[1]));
        emit_insn (gen_kvx_ffdmaw (operands[0], fconj, operands[2], operands[3]));
        DONE;
      }
  }
)

(define_insn "*kvx_ffdmsw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FFDMSW))]
  "KV3_2"
  "ffdmsw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_ffdmdaw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand:SF 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFDMDAW))]
  ""
  {
    if (KV3_1)
      {
        rtx ffdma = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_ffdmaw (ffdma, operands[1], operands[2], operands[4]));
        emit_insn (gen_kvx_faddw (operands[0], ffdma, operands[3], operands[4]));
        DONE;
      }
  }
)

(define_insn "*kvx_ffdmdaw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFDMDAW))]
  "KV3_2"
  "ffdmdaw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmsaw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand:SF 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFDMSAW))]
  ""
  {
    if (KV3_1)
      {
        rtx ffdmaw = gen_reg_rtx (SFmode);
        rtx fconj = gen_reg_rtx (V2SFmode);
        emit_insn (gen_kvx_fconjwc (fconj, operands[1]));
        emit_insn (gen_kvx_ffdmaw (ffdmaw, fconj, operands[2], operands[4]));
        emit_insn (gen_kvx_fsbfw (operands[0], ffdmaw, operands[3], operands[4]));
        DONE;
      }
  }
)

(define_insn "*kvx_ffdmsaw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFDMSAW))]
  "KV3_2"
  "ffdmsaw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmdsw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand:SF 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFDMDSW))]
  ""
  {
    if (KV3_1)
      {
        rtx ffdma = gen_reg_rtx (SFmode);
        emit_insn (gen_kvx_ffdmaw (ffdma, operands[1], operands[2], operands[4]));
        emit_insn (gen_kvx_fsbfw (operands[0], ffdma, operands[3], operands[4]));
        DONE;
      }
  }
)

(define_insn "*kvx_ffdmdsw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFDMDSW))]
  "KV3_2"
  "ffdmdsw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmasw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand:SF 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFDMASW))]
  ""
  {
    if (KV3_1)
      {
        rtx ffdmaw = gen_reg_rtx (SFmode);
        rtx fconj = gen_reg_rtx (V2SFmode);
        emit_insn (gen_kvx_fconjwc (fconj, operands[1]));
        emit_insn (gen_kvx_ffdmaw (ffdmaw, fconj, operands[2], operands[4]));
        emit_insn (gen_kvx_faddw (operands[0], ffdmaw, operands[3], operands[4]));
        DONE;
      }
  }
)

(define_insn "*kvx_ffdmasw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFDMASW))]
  "KV3_2"
  "ffdmasw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
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
    if (KV3_1)
      {
        rtx product = gen_reg_rtx (V2SFmode);
        emit_insn (gen_kvx_fmulwc (product, operands[2], operands[1], operands[4]));
        emit_insn (gen_kvx_faddwp (operands[0], product, operands[3], operands[4]));
        DONE;
      }
  }
)

(define_insn "*kvx_ffmawc"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMAWC))]
  "KV3_2"
  "ffmawc%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffmswc"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "")
                      (match_operand:V2SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMSWC))]
  ""
  {
    if (KV3_1)
      {
        rtx product = gen_reg_rtx (V2SFmode);
        emit_insn (gen_kvx_fmulwc (product, operands[2], operands[1], operands[4]));
        emit_insn (gen_kvx_fsbfwp (operands[0], product, operands[3], operands[4]));
        DONE;
      }
  }
)

(define_insn "*kvx_ffmswc"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSWC))]
  "KV3_2"
  "ffmswc%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
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

(define_insn "kvx_fconjwc"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FCONJWC))]
  ""
  "fnegd %0 = %1"
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


;; S128F (V8HF V4SF)

(define_insn "*fcompn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(match_operand:S128F 2 "register_operand" "r")
          (match_operand:S128F 3 "register_operand" "r")]))]
  ""
  "fcompn<chunkx>.%f1 %x0 = %x2, %x3\n\tfcompn<chunkx>.%f1 %y0 = %y2, %y3"
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
  "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1"
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
  "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_selectf<suffix>"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "r")
                       (match_operand:S128F 2 "register_operand" "0")
                       (match_operand:<MASK> 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECT128))]
  ""
  "cmove<chunkx>%4 %x3? %x0 = %x1\n\tcmove<chunkx>%4 %y3? %y0 = %y1"
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
                       (match_operand 4 "" "")] UNSPEC_FFMA128))]
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
                       (match_operand 4 "" "")] UNSPEC_FFMA128))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<CHUNK> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMA64))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<CHUNK> (match_dup 3) 8)
                         (match_dup 4)] UNSPEC_FFMA64))]
  ""
)

(define_insn "kvx_ffma<suffix>_2"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "r")
                       (match_operand:S128F 2 "register_operand" "r")
                       (match_operand:S128F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMA128))]
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
                       (match_operand 4 "" "")] UNSPEC_FFMS128))]
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
                       (match_operand 4 "" "")] UNSPEC_FFMS128))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<CHUNK> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMS64))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<CHUNK> (match_dup 3) 8)
                         (match_dup 4)] UNSPEC_FFMS64))]
  ""
)

(define_insn "kvx_ffms<suffix>_2"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "r")
                       (match_operand:S128F 2 "register_operand" "r")
                       (match_operand:S128F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMS128))]
  "KV3_2"
  "ffms<suffix>%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)


;; V128F (V8HF V4SF V2DF)

(define_insn "fmin<mode>3"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (smin:V128F (match_operand:V128F 1 "register_operand" "r")
                    (match_operand:V128F 2 "register_operand" "r")))]
  ""
  "fmin<chunkx> %x0 = %x1, %x2\n\tfmin<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*fmin<mode>3_s1"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (smin:V128F (unspec:V128F [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128)
                    (match_operand:V128F 2 "register_operand" "r")))]
  ""
  "fmin<chunkx> %x0 = %1, %x2\n\tfmin<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*fmin<mode>3_s2"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (smin:V128F (match_operand:V128F 1 "register_operand" "r")
                    (unspec:V128F [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP128)))]
  ""
  "fmin<chunkx> %x0 = %x1, %2\n\tfmin<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "fmax<mode>3"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (smax:V128F (match_operand:V128F 1 "register_operand" "r")
                    (match_operand:V128F 2 "register_operand" "r")))]
  ""
  "fmax<chunkx> %x0 = %x1, %x2\n\tfmax<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*fmax<mode>3_s1"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (smax:V128F (unspec:V128F [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP128)
                    (match_operand:V128F 2 "register_operand" "r")))]
  ""
  "fmax<chunkx> %x0 = %1, %x2\n\tfmax<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*fmax<mode>3_s2"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (smax:V128F (match_operand:V128F 1 "register_operand" "r")
                    (unspec:V128F [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP128)))]
  ""
  "fmax<chunkx> %x0 = %x1, %2\n\tfmax<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "neg<mode>2"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (neg:V128F (match_operand:V128F 1 "register_operand" "r")))]
  ""
  "fneg<chunkx> %x0 = %x1\n\tfneg<chunkx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "abs<mode>2"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (abs:V128F (match_operand:V128F 1 "register_operand" "r")))]
  ""
  "fabs<chunkx> %x0 = %x1\n\tfabs<chunkx> %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)


;; S128F (V8HF V4SF)

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

(define_insn_and_split "kvx_fmul<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S128F 1 "register_operand" "r")
                        (match_operand:S128F 2 "register_operand" "r")
                        (match_operand 3 "" "")] UNSPEC_FMULE128))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (unspec:<HWIDE> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (match_dup 3)] UNSPEC_FMULE64))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (unspec:<HWIDE> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (match_dup 3)] UNSPEC_FMULE64))]
  ""
)

(define_insn_and_split "kvx_ffma<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S128F 1 "register_operand" "r")
                        (match_operand:S128F 2 "register_operand" "r")
                        (match_operand:<WIDE> 3 "register_operand" "0")
                        (match_operand 4 "" "")] UNSPEC_FFMAE128))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (unspec:<HWIDE> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<HWIDE> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMAE64))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (unspec:<HWIDE> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<HWIDE> (match_dup 3) 16)
                         (match_dup 4)] UNSPEC_FFMAE64))]
  ""
)

(define_insn_and_split "kvx_ffms<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S128F 1 "register_operand" "r")
                        (match_operand:S128F 2 "register_operand" "r")
                        (match_operand:<WIDE> 3 "register_operand" "0")
                        (match_operand 4 "" "")] UNSPEC_FFMSE128))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (unspec:<HWIDE> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<HWIDE> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMSE64))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (unspec:<HWIDE> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<HWIDE> (match_dup 3) 16)
                         (match_dup 4)] UNSPEC_FFMSE64))]
  ""
)

(define_expand "kvx_fwiden<widenx>"
  [(match_operand:<WIDE> 0 "register_operand")
   (match_operand:S128F 1 "register_operand")
   (match_operand 2 "")]
  ""
  {
    rtx op1_l = gen_rtx_SUBREG (<HALF>mode, operands[1], 0);
    rtx op1_m = gen_rtx_SUBREG (<HALF>mode, operands[1], 8);
    rtx op0_l = gen_rtx_SUBREG (<HWIDE>mode, operands[0], 0);
    rtx op0_m = gen_rtx_SUBREG (<HWIDE>mode, operands[0], 16);
    emit_insn (gen_kvx_fwiden<hwidenx> (op0_l, op1_l, operands[2]));
    emit_insn (gen_kvx_fwiden<hwidenx> (op0_m, op1_m, operands[2]));
    DONE;
  }
)

(define_expand "kvx_fnarrow<truncx>"
  [(match_operand:S128F 0 "register_operand")
   (match_operand:<WIDE> 1 "register_operand")
   (match_operand 2 "")]
  ""
  {
    rtx op1_l = gen_rtx_SUBREG (<HWIDE>mode, operands[1], 0);
    rtx op1_m = gen_rtx_SUBREG (<HWIDE>mode, operands[1], 16);
    rtx op0_l = gen_rtx_SUBREG (<HALF>mode, operands[0], 0);
    rtx op0_m = gen_rtx_SUBREG (<HALF>mode, operands[0], 8);
    emit_insn (gen_kvx_fnarrow<htruncx> (op0_l, op1_l, operands[2]));
    emit_insn (gen_kvx_fnarrow<htruncx> (op0_m, op1_m, operands[2]));
    DONE;
  }
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
                      (match_operand 3 "" "")] UNSPEC_FADD128))]
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
                      (match_operand 3 "" "")] UNSPEC_FADD128))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADD64))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FADD64))]
  ""
)

(define_insn "kvx_faddho_2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD128))]
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
                      (match_operand 3 "" "")] UNSPEC_FSBF128))]
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
                      (match_operand 3 "" "")] UNSPEC_FSBF128))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBF64))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FSBF64))]
  ""
)

(define_insn "kvx_fsbfho_2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF128))]
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
                      (match_operand 3 "" "")] UNSPEC_FMUL128))]
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
                      (match_operand 3 "" "")] UNSPEC_FMUL128))]
  "KV3_1"
  "#"
  "&& reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMUL64))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FMUL64))]
  ""
)

(define_insn "kvx_fmulho_2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMUL128))]
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
    if (KV3_1)
      {
        rtx modifiers = operands[3];
        const char *xstr = XSTR (modifiers, 0);
        bool matlayout = xstr && xstr[0] == '.' &&
          (xstr[1] == 'n' || xstr[1] == 't') &&
          (xstr[2] == 'n' || xstr[2] == 't');
        if (matlayout)
          {
            if (xstr[1] == 'n')
              {
                rtx operand_1 = gen_reg_rtx (V4SFmode);
                emit_insn (gen_kvx_fmt22w (operand_1, operands[1]));
                operands[1] = operand_1;
              }
            if (xstr[2] == 't')
              {
                rtx operand_2 = gen_reg_rtx (V4SFmode);
                emit_insn (gen_kvx_fmt22w (operand_2, operands[2]));
                operands[2] = operand_2;
              }
            modifiers = gen_rtx_CONST_STRING (VOIDmode, xstr + 3);
          }
        else
          {
            rtx operand_1 = gen_reg_rtx (V4SFmode);
            emit_insn (gen_kvx_fmt22w (operand_1, operands[1]));
            operands[1] = operand_1;
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
  }
)

(define_insn "*kvx_fmm222w"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMM222W))]
  "KV3_2"
  "fmm222w%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_fmma222w"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FMMA222W))]
  ""
  {
    if (KV3_1)
      {
        rtx modifiers = operands[4];
        const char *xstr = XSTR (modifiers, 0);
        bool matlayout = xstr && xstr[0] == '.' &&
          (xstr[1] == 'n' || xstr[1] == 't') &&
          (xstr[2] == 'n' || xstr[2] == 't');
        if (matlayout)
          {
            if (xstr[1] == 'n')
              {
                rtx operand_1 = gen_reg_rtx (V4SFmode);
                emit_insn (gen_kvx_fmt22w (operand_1, operands[1]));
                operands[1] = operand_1;
              }
            if (xstr[2] == 't')
              {
                rtx operand_2 = gen_reg_rtx (V4SFmode);
                emit_insn (gen_kvx_fmt22w (operand_2, operands[2]));
                operands[2] = operand_2;
              }
            modifiers = gen_rtx_CONST_STRING (VOIDmode, xstr + 3);
          }
        else
          {
            rtx operand_1 = gen_reg_rtx (V4SFmode);
            emit_insn (gen_kvx_fmt22w (operand_1, operands[1]));
            operands[1] = operand_1;
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
  }
)

(define_insn "*kvx_fmma222w"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FMMA222W))]
  "KV3_2"
  "fmma222w%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_fmms222w"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FMMS222W))]
  ""
  {
    if (KV3_1)
      {
        rtx modifiers = operands[4];
        const char *xstr = XSTR (modifiers, 0);
        bool matlayout = xstr && xstr[0] == '.' &&
          (xstr[1] == 'n' || xstr[1] == 't') &&
          (xstr[2] == 'n' || xstr[2] == 't');
        if (matlayout)
          {
            if (xstr[1] == 'n')
              {
                rtx operand_1 = gen_reg_rtx (V4SFmode);
                emit_insn (gen_kvx_fmt22w (operand_1, operands[1]));
                operands[1] = operand_1;
              }
            if (xstr[2] == 't')
              {
                rtx operand_2 = gen_reg_rtx (V4SFmode);
                emit_insn (gen_kvx_fmt22w (operand_2, operands[2]));
                operands[2] = operand_2;
              }
            modifiers = gen_rtx_CONST_STRING (VOIDmode, xstr + 3);
          }
        else
          {
            rtx operand_1 = gen_reg_rtx (V4SFmode);
            emit_insn (gen_kvx_fmt22w (operand_1, operands[1]));
            operands[1] = operand_1;
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
  }
)

(define_insn "*kvx_fmms222w"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FMMS222W))]
  "KV3_2"
  "fmms222w%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmawp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FFDMAWP))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffdmawp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FFDMAWP))]
  "KV3_2"
  "ffdmawp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_ffdmswp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FFDMSWP))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffdmswp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FFDMSWP))]
  "KV3_2"
  "ffdmswp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_ffdmdawp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMDAWP))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffdmdawp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMDAWP))]
  "KV3_2"
  "ffdmdawp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmsawp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMSAWP))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffdmsawp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMSAWP))]
  "KV3_2"
  "ffdmsawp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmdswp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMDSWP))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffdmdswp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMDSWP))]
  "KV3_2"
  "ffdmdswp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmaswp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMASWP))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffdmaswp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMASWP))]
  "KV3_2"
  "ffdmaswp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_fmulwcp"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMULWCP))]
  ""
  {
    if (KV3_1)
      emit_insn (gen_kvx_fmulwcp_1 (operands[0], operands[1], operands[2], operands[3]));
    if (KV3_2)
      emit_insn (gen_kvx_fmulwcp_2 (operands[0], operands[1], operands[2], operands[3]));
    DONE;
  }
)

(define_insn_and_split "kvx_fmulwcp_1"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWCP))]
  "KV3_1"
  "#"
  "&& reload_completed"
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

(define_insn "kvx_fmulwcp_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWCP))]
  "KV3_2"
  "fmulwcp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_ffmawcp"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMAWCP))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffmawcp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMAWCP))]
  "KV3_2"
  "ffmawcp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffmswcp"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMSWCP))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffmswcp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSWCP))]
  "KV3_2"
  "ffmswcp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
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
                      (match_operand 3 "" "")] UNSPEC_FADD128))]
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
                      (match_operand 3 "" "")] UNSPEC_FSBF128))]
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
                      (match_operand 3 "" "")] UNSPEC_FMUL128))]
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
                      (match_operand 4 "" "")] UNSPEC_SELECT128))]
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
                      (match_operand 3 "" "")] UNSPEC_FADD128))]
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
                      (match_operand 3 "" "")] UNSPEC_FSBF128))]
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
                      (match_operand 3 "" "")] UNSPEC_FMUL128))]
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
                      (match_operand 4 "" "")] UNSPEC_FFMA128))]
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
                      (match_operand 4 "" "")] UNSPEC_FFMS128))]
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

(define_insn "kvx_fconjdc"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")] UNSPEC_FCONJDC))]
  ""
  "copyd %x0 = %x1\n\tfnegd %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)


;; V256L ()



;; S256F (V16HF V8SF)

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
                       (match_operand 4 "" "")] UNSPEC_SELECT256))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)
                        (subreg:<HMASK> (match_dup 3) 0)
                        (match_dup 4)] UNSPEC_SELECT128))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)
                        (subreg:<HMASK> (match_dup 3) 16)
                        (match_dup 4)] UNSPEC_SELECT128))]
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
                       (match_operand 4 "" "")] UNSPEC_FFMA256))]
  ""
  "#"
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "")
                       (match_operand:S256F 2 "register_operand" "")
                       (match_operand:S256F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMA256))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<CHUNK> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMA64))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<CHUNK> (match_dup 3) 8)
                         (match_dup 4)] UNSPEC_FFMA64))
   (set (subreg:<CHUNK> (match_dup 0) 16)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 16)
                         (subreg:<CHUNK> (match_dup 2) 16)
                         (subreg:<CHUNK> (match_dup 3) 16)
                         (match_dup 4)] UNSPEC_FFMA64))
   (set (subreg:<CHUNK> (match_dup 0) 24)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 24)
                         (subreg:<CHUNK> (match_dup 2) 24)
                         (subreg:<CHUNK> (match_dup 3) 24)
                         (match_dup 4)] UNSPEC_FFMA64))]
  ""
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "")
                       (match_operand:S256F 2 "register_operand" "")
                       (match_operand:S256F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMA256))]
  "KV3_2 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)
                        (subreg:<HALF> (match_dup 3) 0)
                        (match_dup 4)] UNSPEC_FFMA128))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)
                        (subreg:<HALF> (match_dup 3) 16)
                        (match_dup 4)] UNSPEC_FFMA128))]
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
                       (match_operand 4 "" "")] UNSPEC_FFMS256))]
  ""
  "#"
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "")
                       (match_operand:S256F 2 "register_operand" "")
                       (match_operand:S256F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMS256))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<CHUNK> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMS64))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<CHUNK> (match_dup 3) 8)
                         (match_dup 4)] UNSPEC_FFMS64))
   (set (subreg:<CHUNK> (match_dup 0) 16)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 16)
                         (subreg:<CHUNK> (match_dup 2) 16)
                         (subreg:<CHUNK> (match_dup 3) 16)
                         (match_dup 4)] UNSPEC_FFMS64))
   (set (subreg:<CHUNK> (match_dup 0) 24)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 24)
                         (subreg:<CHUNK> (match_dup 2) 24)
                         (subreg:<CHUNK> (match_dup 3) 24)
                         (match_dup 4)] UNSPEC_FFMS64))]
  ""
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "")
                       (match_operand:S256F 2 "register_operand" "")
                       (match_operand:S256F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMS256))]
  "KV3_2 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)
                        (subreg:<HALF> (match_dup 3) 0)
                        (match_dup 4)] UNSPEC_FFMS128))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)
                        (subreg:<HALF> (match_dup 3) 16)
                        (match_dup 4)] UNSPEC_FFMS128))]
  ""
)


;; V256F (V16HF V8SF V4D)

(define_insn_and_split "fmin<mode>3"
  [(set (match_operand:V256F 0 "register_operand" "=r")
        (smin:V256F (match_operand:V256F 1 "register_operand" "r")
                    (match_operand:V256F 2 "register_operand" "r")))]
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

(define_insn_and_split "*fmin<mode>3_s1"
  [(set (match_operand:V256F 0 "register_operand" "=r")
        (smin:V256F (unspec:V256F [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256)
                    (match_operand:V256F 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (smin:<HALF> (unspec:<HALF> [(match_dup 1)] UNSPEC_DUP128)
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (smin:<HALF> (unspec:<HALF> [(match_dup 1)] UNSPEC_DUP128)
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*fmin<mode>3_s2"
  [(set (match_operand:V256F 0 "register_operand" "=r")
        (smin:V256F (match_operand:V256F 1 "register_operand" "r")
                    (unspec:V256F [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP256)))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (smin:<HALF> (subreg:<HALF> (match_dup 1) 0)
                     (unspec:<HALF> [(match_dup 2)] UNSPEC_DUP128)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (smin:<HALF> (subreg:<HALF> (match_dup 1) 16)
                     (unspec:<HALF> [(match_dup 2)] UNSPEC_DUP128)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "fmax<mode>3"
  [(set (match_operand:V256F 0 "register_operand" "=r")
        (smax:V256F (match_operand:V256F 1 "register_operand" "r")
                    (match_operand:V256F 2 "register_operand" "r")))]
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

(define_insn_and_split "*fmax<mode>3_s1"
  [(set (match_operand:V256F 0 "register_operand" "=r")
        (smax:V256F (unspec:V256F [(match_operand:<CHUNK> 1 "nonmemory_operand" "r")] UNSPEC_DUP256)
                    (match_operand:V256F 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (smax:<HALF> (unspec:<HALF> [(match_dup 1)] UNSPEC_DUP128)
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (smax:<HALF> (unspec:<HALF> [(match_dup 1)] UNSPEC_DUP128)
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*fmax<mode>3_s2"
  [(set (match_operand:V256F 0 "register_operand" "=r")
        (smax:V256F (match_operand:V256F 1 "register_operand" "r")
                    (unspec:V256F [(match_operand:<CHUNK> 2 "nonmemory_operand" "r")] UNSPEC_DUP256)))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (smax:<HALF> (subreg:<HALF> (match_dup 1) 0)
                     (unspec:<HALF> [(match_dup 2)] UNSPEC_DUP128)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (smax:<HALF> (subreg:<HALF> (match_dup 1) 16)
                     (unspec:<HALF> [(match_dup 2)] UNSPEC_DUP128)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "neg<mode>2"
  [(set (match_operand:V256F 0 "register_operand" "=r")
        (neg:V256F (match_operand:V256F 1 "register_operand" "r")))]
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
  [(set (match_operand:V256F 0 "register_operand" "=r")
        (abs:V256F (match_operand:V256F 1 "register_operand" "r")))]
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
                      (match_operand 3 "" "")] UNSPEC_FADD256))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FADD256))]
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADD64))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FADD64))
   (set (subreg:V4HF (match_dup 0) 16)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 16)
                      (subreg:V4HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADD64))
   (set (subreg:V4HF (match_dup 0) 24)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 24)
                      (subreg:V4HF (match_dup 2) 24)
                      (match_dup 3)] UNSPEC_FADD64))]
  ""
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FADD256))]
  "KV3_2 && reload_completed"
  [(set (subreg:V8HF (match_dup 0) 0)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 0)
                      (subreg:V8HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADD128))
   (set (subreg:V8HF (match_dup 0) 16)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 16)
                      (subreg:V8HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADD128))]
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
                      (match_operand 3 "" "")] UNSPEC_FSBF256))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FSBF256))]
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBF64))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FSBF64))
   (set (subreg:V4HF (match_dup 0) 16)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 16)
                      (subreg:V4HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBF64))
   (set (subreg:V4HF (match_dup 0) 24)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 24)
                      (subreg:V4HF (match_dup 2) 24)
                      (match_dup 3)] UNSPEC_FSBF64))]
  ""
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FSBF256))]
  "KV3_2 && reload_completed"
  [(set (subreg:V8HF (match_dup 0) 0)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 0)
                      (subreg:V8HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBF128))
   (set (subreg:V8HF (match_dup 0) 16)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 16)
                      (subreg:V8HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBF128))]
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
                      (match_operand 3 "" "")] UNSPEC_FMUL256))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMUL256))]
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMUL64))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FMUL64))
   (set (subreg:V4HF (match_dup 0) 16)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 16)
                      (subreg:V4HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMUL64))
   (set (subreg:V4HF (match_dup 0) 24)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 24)
                      (subreg:V4HF (match_dup 2) 24)
                      (match_dup 3)] UNSPEC_FMUL64))]
  ""
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMUL256))]
  "KV3_2 && reload_completed"
  [(set (subreg:V8HF (match_dup 0) 0)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 0)
                      (subreg:V8HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMUL128))
   (set (subreg:V8HF (match_dup 0) 16)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 16)
                      (subreg:V8HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMUL128))]
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
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffdmawq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FFDMAWQ))]
  "KV3_2"
  "ffdmawq%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_ffdmswq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FFDMSWQ))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffdmswq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FFDMSWQ))]
  "KV3_2"
  "ffdmswq%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_ffdmdawq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMDAWQ))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffdmdawq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMDAWQ))]
  "KV3_2"
  "ffdmdawq%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmsawq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMSAWQ))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffdmsawq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMSAWQ))]
  "KV3_2"
  "ffdmsawq%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmdswq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMDSWQ))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffdmdswq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMDSWQ))]
  "KV3_2"
  "ffdmdswq%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmaswq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMASWQ))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn "*kvx_ffdmaswq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMASWQ))]
  "KV3_2"
  "ffdmaswq%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
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

(define_split
  [(set (match_operand:V8SF 0 "register_operand" "")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMULWCQ))]
  "KV3_2 && reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMULWCP))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMULWCP))]
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
    if (KV3_1)
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
  }
)

(define_insn_and_split "*kvx_ffmawcq"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V8SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMAWCQ))]
  "KV3_2"
  "#"
  "&& reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (subreg:V4SF (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_FFMAWCP))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (subreg:V4SF (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_FFMAWCP))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffmswcq"
  [(set (match_operand:V8SF 0 "register_operand" "")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V8SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMSWCQ))]
  ""
  {
    if (KV3_1)
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
  }
)

(define_insn_and_split "*kvx_ffmswcq"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V8SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSWCQ))]
  "KV3_2"
  "#"
  "&& reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (subreg:V4SF (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_FFMSWCP))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (subreg:V4SF (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_FFMSWCP))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
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
                      (match_operand 3 "" "")] UNSPEC_FADD256))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADD128))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADD128))]
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
                      (match_operand 3 "" "")] UNSPEC_FSBF256))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBF128))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBF128))]
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
                      (match_operand 3 "" "")] UNSPEC_FMUL256))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMUL128))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMUL128))]
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
                      (match_operand 4 "" "")] UNSPEC_SELECT256))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 0)
                      (subreg:V2DF (match_dup 2) 0)
                      (subreg:V2DI (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_SELECT128))
   (set (subreg:V2DF (match_dup 0) 16)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 16)
                      (subreg:V2DF (match_dup 2) 16)
                      (subreg:V2DI (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_SELECT128))]
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
                      (match_operand 3 "" "")] UNSPEC_FADD256))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 0)
                      (subreg:V2DF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADD128))
   (set (subreg:V2DF (match_dup 0) 16)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 16)
                      (subreg:V2DF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADD128))]
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
                      (match_operand 3 "" "")] UNSPEC_FSBF256))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 0)
                      (subreg:V2DF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBF128))
   (set (subreg:V2DF (match_dup 0) 16)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 16)
                      (subreg:V2DF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBF128))]
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
                      (match_operand 3 "" "")] UNSPEC_FMUL256))]
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
                      (match_operand 4 "" "")] UNSPEC_FFMA256))]
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
                      (match_operand 4 "" "")] UNSPEC_FFMS256))]
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

(define_insn "kvx_fconjdcp"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")] UNSPEC_FCONJDC))]
  ""
  "copyd %x0 = %x1\n\tfnegd %y0 = %y1\n\tcopyd %z0 = %z1\n\tfnegd %t0 = %t1"
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; S64A (V8QI V4HI V2SI V4HF V2SF)

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


;; S128A (V16QI V8HI V4SI V8HF V4SF)

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


;; S256A (V32QI V16HI V8SI V16HF V8SF)

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


;; KVX_LD, KVX_SD

(define_insn "kvx_l<SIMD64:lsvs>"
  [(set (match_operand:SIMD64 0 "register_operand" "=r,r,r")
        (unspec:SIMD64 [(match_operand:SIMD64 1 "memory_operand" "a,b,m")
                        (match_operand 2 "" "")] UNSPEC_LD))
   (use (match_dup 1))]
  ""
  "ld%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_s<SIMD64:lsvs>"
  [(unspec_volatile:SIMD64 [(match_operand:SIMD64 0 "memory_operand" "a,b,m")
                            (match_operand:SIMD64 1 "register_operand" "r,r,r")] UNSPEC_SD)
   (clobber (match_dup 0))]
  ""
  "sd%m0 %0 = %1"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

;; KVX_LQ, KVX_SQ

(define_insn "kvx_l<SIMD128:lsvs>"
  [(set (match_operand:SIMD128 0 "register_operand" "=r,r,r")
        (unspec:SIMD128 [(match_operand:SIMD128 1 "memory_operand" "a,b,m")
                         (match_operand 2 "" "")] UNSPEC_LQ))
   (use (match_dup 1))]
  ""
  "lq%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_s<SIMD128:lsvs>"
  [(unspec_volatile:SIMD128 [(match_operand:SIMD128 0 "memory_operand" "a,b,m")
                             (match_operand:SIMD128 1 "register_operand" "r,r,r")] UNSPEC_SQ)
   (clobber (match_dup 0))]
  ""
  "sq%m0 %0 = %1"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

;; KVX_LO, KVX_SO

(define_insn "kvx_l<SIMD256:lsvs>"
  [(set (match_operand:SIMD256 0 "register_operand" "=r,r,r")
        (unspec:SIMD256 [(match_operand:SIMD256 1 "memory_operand" "a,b,m")
                         (match_operand 2 "" "")] UNSPEC_LO))
   (use (match_dup 1))]
  ""
  "lo%2%m1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_s<SIMD256:lsvs>"
  [(unspec_volatile:SIMD256 [(match_operand:SIMD256 0 "memory_operand" "a,b,m")
                             (match_operand:SIMD256 1 "register_operand" "r,r,r")] UNSPEC_SO)
   (clobber (match_dup 0))]
  ""
  "so%m0 %0 = %1"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

;; KVX_LV, KVX_SV

(define_insn "kvx_lv<ALL256:lsvs>"
  [(unspec_volatile:ALL256 [(match_operand 0 "" "")
                            (match_operand:ALL256 1 "memory_operand" "a,b,m")
                            (match_operand 2 "" "")] UNSPEC_LV)
   (use (match_dup 1))]
  ""
  "lv%2%m1 $%0 = %1"
  [(set_attr "type" "lsu_load_uncached,lsu_load_uncached_x,lsu_load_uncached_y")
   (set_attr "length"               "4,                  8,                 12")]
)

(define_insn "kvx_sv<ALL256:lsvs>"
  [(unspec_volatile:ALL256 [(match_operand:ALL256 0 "memory_operand" "a,b,m")
                            (match_operand 1 "" "")] UNSPEC_SV)
   (clobber (match_dup 0))]
  ""
  "sv%m0 %0 = $%1"
  [(set_attr "type" "lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y")
   (set_attr "length"            "4,               8,              12")]
)


;; KVX_MOVETO, KVX_MOVEFO, KVX_SWAPVO

(define_insn "kvx_moveto<ALL256:lsvs>"
  [(unspec_volatile:ALL256 [(match_operand 0 "" "")
                            (match_operand:ALL256 1 "register_operand" "r")] UNSPEC_MOVETO)]
  ""
  "movetq $%0.lo = %x1, %y1\n\tmovetq $%0.hi = %z1, %t1"
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "kvx_movefo<ALL256:lsvs>"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (unspec_volatile:ALL256 [(match_operand 1 "" "")] UNSPEC_MOVEFO))]
  ""
  "movefo %0 = $%1"
  [(set_attr "type" "bcu_tiny_auxw_crrp")]
)

(define_insn "kvx_swapvo<ALL256:lsvs>"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (unspec_volatile:ALL256 [(match_operand 1 "" "")
                                 (match_operand:ALL256 2 "register_operand" "0")] UNSPEC_SWAPVO))]
  ""
  "movetq $%1.lo = %x0, %y0\n\tmovetq $%1.hi = %z0, %t0\n\tmovefo %0 = $%1"
  [(set_attr "type" "all")
   (set_attr "length" "12")]
)

;; KVX_ALIGNO, KVX_ALIGNV

(define_insn "kvx_aligno<ALL256:lsvs>"
  [(set (match_operand:ALL256 0 "register_operand" "=r,r")
        (unspec_volatile:ALL256 [(match_operand 1 "" "")
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


