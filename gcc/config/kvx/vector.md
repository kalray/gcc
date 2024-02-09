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
        (match_operand:SIMD64 1 "general_operand"       "r,Ca,Cb,Cm,Za,Zb,Zm,r,r,r,S16,S43,i"))]
  "(!immediate_operand (operands[1], <MODE>mode) || !memory_operand (operands[0], <MODE>mode))"
  {
    switch (which_alternative)
      {
      case 0:
        return "copyd %0 = %1";
      case 1: case 2: case 3: case 4: case 5: case 6:
        return "ld%V1 %0 = %1";
      case 7: case 8: case 9:
        return "sd%X0 %0 = %1";
      case 10: case 11: case 12:
        return "make %0 = %1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "alu_tiny,load_core,load_core_x,load_core_y,load_core_uncached,load_core_uncached_x,load_core_uncached_y,store_core,store_core_x,store_core_y,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,            4,              8,             12,                     4,                       8,                      12,             4,               8,              12,       4,         8,        12")]
)

(define_insn "*movv1di_imm"
  [(set (match_operand:V1DI 0 "register_operand" "=r,r")
        (subreg:V1DI (match_operand:DI 1 "symbolic_operand" "r,i") 0))]
  ""
  "@
   copyd %0 = %1
   make %0 = %1"
  [(set_attr "type" "alu_tiny,alu_tiny_y")
   (set_attr "length"      "4,        12")]
)


;; 128-bit Vector Moves

(define_expand "mov<mode>"
  [(set (match_operand:ALL128 0 "nonimmediate_operand" "")
        (match_operand:ALL128 1 "general_operand" ""))]
  ""
  {
    bool misaligned_0 = kvx_hardreg_misaligned_p (operands[0], 2);
    bool misaligned_1 = kvx_hardreg_misaligned_p (operands[1], 2);
    if ((misaligned_0 && MEM_P (operands[1])) || (misaligned_1 && MEM_P (operands[0])))
      {
        rtx temp = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (temp, operands[1]));
        emit_insn (gen_rtx_SET (operands[0], temp));
        DONE;
      }

    if (MEM_P (operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn_and_split "*copy<mode>"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (match_operand:ALL128 1 "register_operand" "r"))]
  "kvx_hardreg_misaligned_p (operands[0], 2) || kvx_hardreg_misaligned_p (operands[1], 2)"
  "#"
  "&& reload_completed"
  [(use (const_int 0))]
  {
    kvx_split_128bits_move (operands[0], operands[1]);
    DONE;
  }
)

(define_insn "*mov<mode>"
  [(set (match_operand:ALL128 0 "nonimmediate_operand" "=r, r, r, r, r, r, r,a,b,m")
        (match_operand:ALL128 1 "nonimmediate_operand"  "r,Ca,Cb,Cm,Za,Zb,Zm,r,r,r"))]
  "!kvx_hardreg_misaligned_p (operands[0], 2) && !kvx_hardreg_misaligned_p (operands[1], 2)"
  {
    switch (which_alternative)
      {
      case 0:
        return "#";
      case 1: case 2: case 3: case 4: case 5: case 6:
        return "lq%V1 %0 = %1";
      case 7: case 8: case 9:
        return "sq%X0 %0 = %1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "alu_tiny_x2,load_core,load_core_x,load_core_y,load_core_uncached,load_core_uncached_x,load_core_uncached_y,store_core,store_core_x,store_core_y")
   (set_attr "length"         "8,             4,             8,             12,                     4,                       8,                      12,             4,               8,              12")]
)

(define_split
  [(set (match_operand:ALL128 0 "register_operand" "")
        (match_operand:ALL128 1 "register_operand" ""))]
  "reload_completed"
  [(use (const_int 0))]
  {
    kvx_split_128bits_move (operands[0], operands[1]);
    DONE;
  }
)

(define_insn_and_split "*make<mode>"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (match_operand:ALL128 1 "immediate_operand" "i"))]
  ""
  "#"
  "reload_completed"
  [(use (const_int 0))]
  {
    kvx_make_128bit_const (operands[0], operands[1]);
    DONE;
  }
)


;; 256-bit Vector Moves

(define_expand "mov<mode>"
  [(set (match_operand:ALL256 0 "nonimmediate_operand" "")
        (match_operand:ALL256 1 "general_operand" ""))]
  ""
  {
    bool misaligned_0 = kvx_hardreg_misaligned_p (operands[0], 4);
    bool misaligned_1 = kvx_hardreg_misaligned_p (operands[1], 4);
    if ((misaligned_0 && MEM_P (operands[1])) || (misaligned_1 && MEM_P (operands[0])))
      {
        rtx temp = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (temp, operands[1]));
        emit_insn (gen_rtx_SET (operands[0], temp));
        DONE;
      }

    if (MEM_P (operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn_and_split "*copy<mode>"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (match_operand:ALL256 1 "register_operand" "r"))]
  "kvx_hardreg_misaligned_p (operands[0], 4) || kvx_hardreg_misaligned_p (operands[1], 4)"
  "#"
  "&& reload_completed"
  [(use (const_int 0))]
  {
    kvx_split_256bits_move (operands[0], operands[1]);
    DONE;
  }
)

(define_insn "*mov<mode>"
  [(set (match_operand:ALL256 0 "nonimmediate_operand" "=r, r, r, r, r, r, r,a,b,m")
        (match_operand:ALL256 1 "nonimmediate_operand"  "r,Ca,Cb,Cm,Za,Zb,Zm,r,r,r"))]
  "!kvx_hardreg_misaligned_p (operands[0], 4) && !kvx_hardreg_misaligned_p (operands[1], 4)"
  {
    switch (which_alternative)
      {
      case 0:
        return "#";
      case 1: case 2: case 3: case 4: case 5: case 6:
        return "lo%V1 %0 = %1";
      case 7: case 8: case 9:
        return "so%X0 %0 = %1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "alu_tiny_x4,load_core,load_core_x,load_core_y,load_core_uncached,load_core_uncached_x,load_core_uncached_y,store_core,store_core_x,store_core_y")
   (set_attr "length"        "16,            4,              8,             12,                     4,                       8,                      12,             4,               8,              12")]
)

(define_split
  [(set (match_operand:ALL256 0 "register_operand" "")
        (match_operand:ALL256 1 "register_operand" ""))]
  "reload_completed"
  [(use (const_int 0))]
  {
    kvx_split_256bits_move (operands[0], operands[1]);
    DONE;
  }
)

(define_insn_and_split "*make<mode>"
    [(set (match_operand:ALL256 0 "register_operand" "=r")
          (match_operand:ALL256 1 "immediate_operand" "i"))]
  ""
  "#"
  "reload_completed"
  [(use (const_int 0))]
  {
    kvx_make_256bit_const (operands[0], operands[1]);
    DONE;
  }
)


;; 512-bit Vector Moves

(define_expand "mov<mode>"
  [(set (match_operand:ALL512 0 "nonimmediate_operand" "")
        (match_operand:ALL512 1 "general_operand" ""))]
  ""
  {
    if (MEM_P (operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn_and_split "*mov<mode>"
  [(set (match_operand:ALL512 0 "nonimmediate_operand" "=&r,&r,&r,&r,a,b,m")
        (match_operand:ALL512 1 "nonimmediate_operand"   "r, a, b, m,r,r,r"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0) (subreg:<HALF> (match_dup 1) 0))
   (set (subreg:<HALF> (match_dup 0) 32) (subreg:<HALF> (match_dup 1) 32))]
  {
  }
)

(define_insn_and_split "*make<mode>"
    [(set (match_operand:ALL512 0 "register_operand" "=r")
          (match_operand:ALL512 1 "immediate_operand" "i"))]
  ""
  "#"
  "reload_completed"
  [(use (const_int 0))]
  {
    kvx_make_512bit_const (operands[0], operands[1]);
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

(define_expand "vec_extract<mode><inner>"
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

(define_expand "vec_init<mode><inner>"
  [(match_operand:SIMDALL 0 "register_operand" "")
   (match_operand 1 "" "")]
  ""
  {
    rtx target = operands[0];
    rtx source = operands[1];
    kvx_expand_vector_init (target, source);
    DONE;
  }
)

(define_expand "vec_duplicate<mode>"
  [(match_operand:SIMDALL 0 "register_operand" "")
   (match_operand 1 "" "")]
  ""
  {
    rtx target = operands[0];
    rtx source = operands[1];
    kvx_expand_vector_duplicate (target, source);
    DONE;
  }
)

(define_expand "vec_cmp<mode><mask>"
  [(set (match_operand:<MASK> 0 "register_operand")
        (match_operator 1 "comparison_operator"
         [(match_operand:SIMDCMP 2 "register_operand")
          (match_operand:SIMDCMP 3 "reg_zero_mone_operand")]))]
  ""
  {
    rtx mask = operands[0];
    rtx comp = operands[1];
    kvx_lower_comparison (mask, comp, <MODE>mode);
    DONE;
  }
)

(define_expand "vec_cmpu<mode><mask>"
  [(set (match_operand:<MASK> 0 "register_operand")
        (match_operator 1 "comparison_operator"
         [(match_operand:SIMDCMP 2 "register_operand")
          (match_operand:SIMDCMP 3 "reg_zero_mone_operand")]))]
  ""
  {
    rtx mask = operands[0];
    rtx comp = operands[1];
    kvx_lower_comparison (mask, comp, <MODE>mode);
    DONE;
  }
)

(define_expand "vcond<SIMDALL:mode><SIMDCMP:mode>"
  [(match_operand:SIMDALL 0 "register_operand")
   (match_operand:SIMDALL 1 "nonmemory_operand")
   (match_operand:SIMDALL 2 "nonmemory_operand")
   (match_operator 3 "comparison_operator"
    [(match_operand:SIMDCMP 4 "register_operand")
     (match_operand:SIMDCMP 5 "reg_zero_mone_operand")])]
  "(GET_MODE_NUNITS (<SIMDCMP:MODE>mode) == GET_MODE_NUNITS (<SIMDALL:MODE>mode))"
  {
    rtx target = operands[0];
    rtx select1 = operands[1];
    rtx select2 = operands[2];
    kvx_expand_conditional_move (target, select1, select2, operands[3]);
    DONE;
  }
)

(define_expand "vcondu<SIMDALL:mode><SIMDCMP:mode>"
  [(match_operand:SIMDALL 0 "register_operand")
   (match_operand:SIMDALL 1 "nonmemory_operand")
   (match_operand:SIMDALL 2 "nonmemory_operand")
   (match_operator 3 "comparison_operator"
    [(match_operand:SIMDCMP 4 "register_operand")
     (match_operand:SIMDCMP 5 "reg_zero_mone_operand")])]
  "(GET_MODE_NUNITS (<SIMDCMP:MODE>mode) == GET_MODE_NUNITS (<SIMDALL:MODE>mode))"
  {
    rtx target = operands[0];
    rtx select1 = operands[1];
    rtx select2 = operands[2];
    kvx_expand_conditional_move (target, select1, select2, operands[3]);
    DONE;
  }
)

(define_expand "vcond_mask_<mode><mask>"
  [(match_operand:SIMDALL 0 "register_operand")
   (match_operand:SIMDALL 1 "nonmemory_operand")
   (match_operand:SIMDALL 2 "nonmemory_operand")
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

(define_expand "vec_shl_insert_<mode>"
  [(match_operand:SIMDALL 0 "register_operand" "")
   (match_operand:SIMDALL 1 "register_operand" "")
   (match_operand:<INNER> 2 "register_operand" "")]
  ""
  {
    HOST_WIDE_INT bits = GET_MODE_SIZE (<INNER>mode) * BITS_PER_UNIT;
    kvx_expand_vector_shift (operands[0], operands[1], operands[2], bits, 1);
    DONE;
  }
)

(define_expand "vec_shl_<mode>"
  [(match_operand:SIMDALL 0 "register_operand" "")
   (match_operand:SIMDALL 1 "register_operand" "")
   (match_operand:SI 2 "const_pos32_operand" "")]
  ""
  {
    HOST_WIDE_INT value = INTVAL (operands[2]);
    kvx_expand_vector_shift (operands[0], operands[1], const0_rtx, value, 1);
    DONE;
  }
)

(define_expand "vec_shr_<mode>"
  [(match_operand:SIMDALL 0 "register_operand" "")
   (match_operand:SIMDALL 1 "register_operand" "")
   (match_operand:SI 2 "const_pos32_operand" "")]
  ""
  {
    HOST_WIDE_INT value = INTVAL (operands[2]);
    kvx_expand_vector_shift (operands[0], operands[1], const0_rtx, value, 0);
    DONE;
  }
)


;; Vector Init Helpers

(define_insn "kvx_init0<lsvs>"
  [(set (match_operand:SIMD32X2 0 "register_operand" "=r")
        (unspec:SIMD32X2 [(match_operand:<INNER> 1 "register_operand" "r")] UNSPEC_INITX2A))]
  ""
  "zxwd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_init1<lsvs>"
  [(set (match_operand:SIMD32X2 0 "register_operand" "+r")
        (unspec:SIMD32X2 [(match_operand:<INNER> 1 "register_operand" "r")
                          (match_dup 0)] UNSPEC_INITX2B))]
  ""
  "insf %0 = %1, 63, 32"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_init0<lsvs>"
  [(set (match_operand:SIMD16X4 0 "register_operand" "=r")
        (unspec:SIMD16X4 [(match_operand:<INNER> 1 "register_operand" "r")] UNSPEC_INITX4A))]
  ""
  "zxhd %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_init1<lsvs>"
  [(set (match_operand:SIMD16X4 0 "register_operand" "+r")
        (unspec:SIMD16X4 [(match_operand:<INNER> 1 "register_operand" "r")
                          (match_dup 0)] UNSPEC_INITX4B))]
  ""
  "insf %0 = %1, 31, 16"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_init2<lsvs>"
  [(set (match_operand:SIMD16X4 0 "register_operand" "+r")
        (unspec:SIMD16X4 [(match_operand:<INNER> 1 "register_operand" "r")
                          (match_dup 0)] UNSPEC_INITX4C))]
  ""
  "insf %0 = %1, 47, 32"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_init3<lsvs>"
  [(set (match_operand:SIMD16X4 0 "register_operand" "+r")
        (unspec:SIMD16X4 [(match_operand:<INNER> 1 "register_operand" "r")
                          (match_dup 0)] UNSPEC_INITX4D))]
  ""
  "insf %0 = %1, 63, 48"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_init0<lsvs>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "=r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")] UNSPEC_INITX8A))]
  ""
  "zxbd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_init1<lsvs>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8B))]
  ""
  "insf %0 = %1, 15, 8"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_init2<lsvs>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8C))]
  ""
  "insf %0 = %1, 23, 16"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_init3<lsvs>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8D))]
  ""
  "insf %0 = %1, 31, 24"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_init4<lsvs>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8E))]
  ""
  "insf %0 = %1, 39, 32"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_init5<lsvs>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8F))]
  ""
  "insf %0 = %1, 47, 40"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_init6<lsvs>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8G))]
  ""
  "insf %0 = %1, 55, 48"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_init7<lsvs>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8H))]
  ""
  "insf %0 = %1, 63, 56"
  [(set_attr "type" "alu_thin")]
)


;; Vector Insert/Extract Helpers

(define_insn "*addd"
  [(set (match_operand:ALL64 0 "register_operand" "=r")
        (unspec:ALL64 [(match_operand:SIMD64 1 "register_operand" "r")
                       (match_operand:DI 2 "register_operand" "r")] UNSPEC_ADDD))]
  ""
  "addd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*adddp"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (unspec:ALL128 [(match_operand:SIMD128 1 "register_operand" "r")
                        (match_operand:DI 2 "register_operand" "r")] UNSPEC_ADDD))]
  ""
  "addd %x0 = %x1, %2\n\taddd %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*adddq"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (unspec:ALL256 [(match_operand:SIMD256 1 "register_operand" "r")
                        (match_operand:DI 2 "register_operand" "r")] UNSPEC_ADDD))]
  ""
  {
    return "addd %x0 = %x1, %2\n\taddd %y0 = %y1, %2\n\t"
           "addd %z0 = %z1, %2\n\taddd %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*andd"
  [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
        (unspec:FITGPR [(match_operand:SCALAR 1 "register_operand" "r,r,r,r")
                        (match_operand:WI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")] UNSPEC_ANDD))]
  ""
  "andd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "*andd"
  [(set (match_operand:ALL64 0 "register_operand" "=r,r,r,r")
        (unspec:ALL64 [(match_operand:SIMD64 1 "register_operand" "r,r,r,r")
                       (match_operand:WI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")] UNSPEC_ANDD))]
  ""
  "andd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "*anddp"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (unspec:ALL128 [(match_operand:SIMD128 1 "register_operand" "r")
                        (match_operand:DI 2 "register_operand" "r")] UNSPEC_ANDD))]
  ""
  "andd %x0 = %x1, %2\n\tandd %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*anddq"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (unspec:ALL256 [(match_operand:SIMD256 1 "register_operand" "r")
                        (match_operand:DI 2 "register_operand" "r")] UNSPEC_ANDD))]
  ""
  {
    return "andd %x0 = %x1, %2\n\tandd %y0 = %y1, %2\n\t"
           "andd %z0 = %z1, %2\n\tandd %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*andd"
  [(set (match_operand:ALL64 0 "register_operand" "=r")
        (unspec:ALL64 [(match_operand:ALL64 1 "register_operand" "r")
                       (match_operand:ALL64 2 "register_operand" "r")] UNSPEC_ANDD))]
  ""
  "andd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*anddp"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (unspec:ALL128 [(match_operand:ALL128 1 "register_operand" "r")
                        (match_operand:ALL128 2 "register_operand" "r")] UNSPEC_ANDD))]
  ""
  "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*anddq"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (unspec:ALL256 [(match_operand:ALL256 1 "register_operand" "r")
                        (match_operand:ALL256 2 "register_operand" "r")] UNSPEC_ANDD))]
  ""
  {
    return "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2\n\t"
           "andd %z0 = %z1, %z2\n\tandd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*xord"
  [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
        (unspec:FITGPR [(match_operand:FITGPR 1 "register_operand" "r,r,r,r")
                        (match_operand:SCALAR 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")] UNSPEC_XORD))]
  ""
  "xord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*xord"
  [(set (match_operand:FITGPR 0 "register_operand" "=r")
        (unspec:FITGPR [(match_operand:FITGPR 1 "register_operand" "r")
                        (match_operand:SIMD64 2 "register_operand" "r")] UNSPEC_XORD))]
  ""
  "xord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*xordp"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (unspec:ALL128 [(match_operand:ALL128 1 "register_operand" "r")
                        (match_operand:ALL128 2 "register_operand" "r")] UNSPEC_XORD))]
  ""
  "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*xordq"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (unspec:ALL256 [(match_operand:ALL256 1 "register_operand" "r")
                        (match_operand:ALL256 2 "register_operand" "r")] UNSPEC_XORD))]
  ""
  {
    return "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2\n\t"
           "xord %z0 = %z1, %z2\n\txord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*slld"
  [(set (match_operand:FITGPR 0 "register_operand" "=r")
        (unspec:FITGPR [(match_operand:SIMD64 1 "register_operand" "r")
                        (match_operand 2 "sixbits_unsigned_operand" "i")] UNSPEC_SLLD))]
  ""
  "slld %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*srld"
  [(set (match_operand:FITGPR 0 "register_operand" "=r")
        (unspec:FITGPR [(match_operand:SIMD64 1 "register_operand" "r")
                        (match_operand 2 "sixbits_unsigned_operand" "i")] UNSPEC_SRLD))]
  ""
  "srld %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn_and_split "*dup128"
  [(set (match_operand:SIMD128 0 "register_operand" "=r")
        (vec_duplicate:SIMD128 (match_operand:<CHUNK> 1 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 8) (match_dup 1))]
  ""
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*dup256"
  [(set (match_operand:SIMD256 0 "register_operand" "=r")
        (vec_duplicate:SIMD256 (match_operand:<CHUNK> 1 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 8) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 16) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 24) (match_dup 1))]
  ""
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*dup512"
  [(set (match_operand:SIMD512 0 "register_operand" "=r")
        (vec_duplicate:SIMD512 (match_operand:<CHUNK> 1 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 8) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 16) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 24) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 32) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 40) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 48) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 56) (match_dup 1))]
  ""
)


;; VXQI (V8QI V16QI V32QI)

(define_insn "kvx_oroebo"
  [(set (match_operand:V8QI 0 "register_operand" "=r")
        (unspec:V8QI [(match_operand:V4HI 1 "register_operand" "r")
                      (match_operand:V4HI 2 "register_operand" "r")] UNSPEC_OROE))]
  ""
  "ord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_oroebx"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (unspec:V16QI [(match_operand:V8HI 1 "register_operand" "r")
                       (match_operand:V8HI 2 "register_operand" "r")] UNSPEC_OROE))]
  ""
  "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_oroebv"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (unspec:V32QI [(match_operand:V16HI 1 "register_operand" "r")
                       (match_operand:V16HI 2 "register_operand" "r")] UNSPEC_OROE))]
  ""
  {
    return "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2\n\t"
           "ord %z0 = %z1, %z2\n\tord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "kvx_oroebt"
  [(set (match_operand:V64QI 0 "register_operand" "=r")
        (unspec:V64QI [(match_operand:V32HI 1 "register_operand" "r")
                       (match_operand:V32HI 2 "register_operand" "r")] UNSPEC_OROE))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V32QI (match_dup 0) 0)
        (unspec:V32QI [(subreg:V16HI (match_dup 1) 0)
                       (subreg:V16HI (match_dup 2) 0)] UNSPEC_OROE))
   (set (subreg:V32QI (match_dup 0) 32)
        (unspec:V32QI [(subreg:V16HI (match_dup 1) 32)
                       (subreg:V16HI (match_dup 2) 32)] UNSPEC_OROE))]
  ""
)

(define_insn_and_split "*zxe<hwidenx>_oroe<suffix>_2"
  [(set (match_operand:<HWIDE> 0 "register_operand" "=r")
        (unspec:<HWIDE> [(unspec:VXQI [(match_operand:<HWIDE> 1 "register_operand" "r")
                                       (match_operand:<HWIDE> 2 "register_operand" "r")] UNSPEC_OROE)] UNSPEC_ZXE))]
  ""
  "#"
  ""
  [(set (match_dup 0) (match_dup 2))]
  ""
  [(set_attr "type" "alu_tiny")]
)

(define_insn_and_split "*qxo<hwidenx>_oroe<suffix>_1"
  [(set (match_operand:<HWIDE> 0 "register_operand" "=r")
        (unspec:<HWIDE> [(unspec:VXQI [(match_operand:<HWIDE> 1 "register_operand" "r")
                                       (match_operand:<HWIDE> 2 "register_operand" "r")] UNSPEC_OROE)] UNSPEC_QXO))]
  ""
  "#"
  ""
  [(set (match_dup 0) (match_dup 1))]
  ""
  [(set_attr "type" "alu_tiny")]
)

(define_insn_and_split "*zxo<hwidenx>_oroe<suffix>_1"
  [(set (match_operand:<HWIDE> 0 "register_operand" "=r")
        (unspec:<HWIDE> [(unspec:VXQI [(match_operand:<HWIDE> 1 "register_operand" "r")
                                       (match_operand:<HWIDE> 2 "register_operand" "r")] UNSPEC_OROE)] UNSPEC_ZXO))]
  ""
  "#"
  ""
  [(set (match_dup 0)
        (unspec:<HWIDE> [(match_dup 1)] UNSPEC_ZXO))]
  {
    rtx op1 = gen_reg_rtx (<MODE>mode);
    emit_insn (gen_rtx_SET (op1, simplify_gen_subreg (<MODE>mode, operands[1], <HWIDE>mode, 0)));
    operands[1] = op1;
  }
  [(set_attr "type" "alu_tiny")]
)

(define_insn_and_split "*qxe<hwidenx>_oroe<suffix>_2"
  [(set (match_operand:<HWIDE> 0 "register_operand" "=r")
        (unspec:<HWIDE> [(unspec:VXQI [(match_operand:<HWIDE> 1 "register_operand" "r")
                                       (match_operand:<HWIDE> 2 "register_operand" "r")] UNSPEC_OROE)] UNSPEC_QXE))]
  ""
  "#"
  ""
  [(set (match_dup 0)
        (unspec:<HWIDE> [(match_dup 2)] UNSPEC_QXE))]
  {
    rtx op2 = gen_reg_rtx (<MODE>mode);
    emit_insn (gen_rtx_SET (op2, simplify_gen_subreg (<MODE>mode, operands[2], <HWIDE>mode, 0)));
    operands[2] = op2;
  }
  [(set_attr "type" "alu_tiny")]
)

;; neg<m>2 ssneg<m>2 abs<m>2 ssabs<m>2
(define_expand "<prefix><mode>2"
  [(set (match_operand:VXQI 0 "register_operand" "")
        (UNARITH:VXQI (match_operand:VXQI 1 "register_operand" "")))]
  ""
  {
    if (KV3_1)
      {
        rtx op1o = gen_reg_rtx (<HWIDE>mode), op1e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op1o, operands[1]));
        emit_insn (gen_kvx_qxe<hwidenx> (op1e, operands[1]));
        rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_<prefix><hwide>2 (op0e, op1e));
        emit_insn (gen_<prefix><hwide>2 (op0o, op1o));
        rtx opto = gen_reg_rtx (<MODE>mode), opte = gen_reg_rtx (<MODE>mode);
        if (<set8lsb>)
          {
            emit_insn (gen_rtx_SET (opto, gen_rtx_SUBREG (<MODE>mode, op0o, 0)));
            emit_insn (gen_kvx_qxo<hwidenx> (op0o, opto));
          }
        emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
        emit_insn (gen_kvx_zxo<hwidenx> (op0e, opte));
        emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));
        DONE;
      }
  }
)
(define_insn "*<prefix>v8qi2_2"
  [(set (match_operand:V8QI 0 "register_operand" "=r")
        (UNARITH:V8QI (match_operand:V8QI 1 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "<stem>bo %0 = %1"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length"        "8")]
)
(define_insn "*<prefix>v16qi2_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (UNARITH:V16QI (match_operand:V16QI 1 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "<stem>bo %x0 = %x1\n\t<stem>bo %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2_x")
   (set_attr "length"          "16")]
)
(define_insn "*<prefix>v32qi2_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (UNARITH:V32QI (match_operand:V32QI 1 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "<stem>bo %x0 = %x1\n\t<stem>bo %y0 = %y1\n\t"
           "<stem>bo %z0 = %z1\n\t<stem>bo %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4_x")
   (set_attr "length"          "32")]
)

;; add<m>3 ssadd<m>3 usass<m>3 sub<m>3 sssub<m>3 ussub<m>3 smin<m>3 smax<m>3 umin<m>3 umax<m>3
(define_expand "<prefix><mode>3"
  [(set (match_operand:VXQI 0 "register_operand" "")
        (BINARITH:VXQI (match_operand:VXQI 1 "register_operand" "")
                       (match_operand:VXQI 2 "register_operand" "")))]
  ""
  {
    if (KV3_1)
      {
        rtx op2o = gen_reg_rtx (<HWIDE>mode), op2e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op2o, operands[2]));
        emit_insn (gen_kvx_qxe<hwidenx> (op2e, operands[2]));
        rtx op1o = gen_reg_rtx (<HWIDE>mode), op1e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op1o, operands[1]));
        emit_insn (gen_kvx_qxe<hwidenx> (op1e, operands[1]));
        rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_<prefix><hwide>3 (op0o, op1o, op2o));
        emit_insn (gen_<prefix><hwide>3 (op0e, op1e, op2e));
        rtx opto = gen_reg_rtx (<MODE>mode), opte = gen_reg_rtx (<MODE>mode);
        if (<set8lsb>)
          {
            emit_insn (gen_rtx_SET (opto, gen_rtx_SUBREG (<MODE>mode, op0o, 0)));
            emit_insn (gen_kvx_qxo<hwidenx> (op0o, opto));
          }
        emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
        emit_insn (gen_kvx_zxo<hwidenx> (op0e, opte));
        emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));

        DONE;
      }
  }
)
(define_insn "*<prefix>v8qi3_2"
  [(set (match_operand:V8QI 0 "register_operand" "=r")
        (BINARITHC:V8QI (match_operand:V8QI 1 "register_operand" "r")
                        (match_operand:V8QI 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "<stem>bo %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)
(define_insn "*<prefix>v16qi3_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINARITHC:V16QI (match_operand:V16QI 1 "register_operand" "r")
                         (match_operand:V16QI 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "<stem>bo %x0 = %x1, %x2\n\t<stem>bo %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<prefix>v16qi3_s1"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINARITHC:V16QI (vec_duplicate:V16QI (match_operand:V8QI 1 "nonmemory_operand" "r"))
                         (match_operand:V16QI 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "<stem>bo %x0 = %1, %x2\n\t<stem>bo %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<prefix>v16qi3_s2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINARITHC:V16QI (match_operand:V16QI 1 "register_operand" "r")
                         (vec_duplicate:V16QI (match_operand:V8QI 2 "nonmemory_operand" "r"))))]
  "(KV3_2||KV4)"
  "<stem>bo %x0 = %x1, %2\n\t<stem>bo %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<prefix>v32qi3_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (BINARITHC:V32QI (match_operand:V32QI 1 "register_operand" "r")
                         (match_operand:V32QI 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "<stem>bo %x0 = %x1, %x2\n\t<stem>bo %y0 = %y1, %y2\n\t"
           "<stem>bo %z0 = %z1, %z2\n\t<stem>bo %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)
(define_insn "*<prefix>v32qi3_s1"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (BINARITHC:V32QI (vec_duplicate:V32QI (match_operand:V8QI 1 "nonmemory_operand" "r"))
                         (match_operand:V32QI 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "<stem>bo %x0 = %1, %x2\n\t<stem>bo %y0 = %1, %y2\n\t"
           "<stem>bo %z0 = %1, %z2\n\t<stem>bo %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)
(define_insn "*<prefix>v32qi3_s2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (BINARITHC:V32QI (match_operand:V32QI 1 "register_operand" "r")
                         (vec_duplicate:V32QI (match_operand:V8QI 2 "nonmemory_operand" "r"))))]
  "(KV3_2||KV4)"
  {
    return "<stem>bo %x0 = %x1, %2\n\t<stem>bo %y0 = %y1, %2\n\t"
           "<stem>bo %z0 = %z1, %2\n\t<stem>bo %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)
(define_insn "*<prefix>v8qi3_2"
  [(set (match_operand:V8QI 0 "register_operand" "=r")
        (BINMINUS:V8QI (match_operand:V8QI 1 "register_operand" "r")
                       (match_operand:V8QI 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "<stem>bo %0 = %2, %1"
  [(set_attr "type" "alu_tiny")]
)
(define_insn "*<prefix>v16qi3_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINMINUS:V16QI (match_operand:V16QI 1 "register_operand" "r")
                        (match_operand:V16QI 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "<stem>bo %x0 = %x2, %x1\n\t<stem>bo %y0 = %y2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<prefix>v16qi3_s1"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINMINUS:V16QI (vec_duplicate:V16QI (match_operand:V8QI 1 "nonmemory_operand" "r"))
                        (match_operand:V16QI 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "<stem>bo %x0 = %x2, %1\n\t<stem>bo %y0 = %y2, %1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<prefix>v16qi3_s2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINMINUS:V16QI (match_operand:V16QI 1 "register_operand" "r")
                        (vec_duplicate:V16QI (match_operand:V8QI 2 "nonmemory_operand" "r"))))]
  "(KV3_2||KV4)"
  "<stem>bo %x0 = %2, %x1\n\t<stem>bo %y0 = %2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<prefix>v32qi3_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (BINMINUS:V32QI (match_operand:V32QI 1 "register_operand" "r")
                        (match_operand:V32QI 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "<stem>bo %x0 = %x2, %x1\n\t<stem>bo %y0 = %y2, %y1\n\t"
           "<stem>bo %z0 = %z2, %z1\n\t<stem>bo %t0 = %t2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)
(define_insn "*<prefix>v32qi3_s1"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (BINMINUS:V32QI (vec_duplicate:V32QI (match_operand:V8QI 1 "nonmemory_operand" "r"))
                        (match_operand:V32QI 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "<stem>bo %x0 = %x2, %1\n\t<stem>bo %y0 = %y2, %1\n\t"
           "<stem>bo %z0 = %z2, %1\n\t<stem>bo %t0 = %t2, %1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)
(define_insn "*<prefix>v32qi3_s2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (BINMINUS:V32QI (match_operand:V32QI 1 "register_operand" "r")
                        (vec_duplicate:V32QI (match_operand:V8QI 2 "nonmemory_operand" "r"))))]
  "(KV3_2||KV4)"
  {
    return "<stem>bo %x0 = %2, %x1\n\t<stem>bo %y0 = %2, %y1\n\t"
           "<stem>bo %z0 = %2, %z1\n\t<stem>bo %t0 = %2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "mul<mode>3"
  [(set (match_operand:VXQI 0 "register_operand" "")
        (mult:VXQI (match_operand:VXQI 1 "register_operand" "")
                   (match_operand:VXQI 2 "register_operand" "")))]
  ""
  {
    rtx op2o = gen_reg_rtx (<HWIDE>mode), op2e = gen_reg_rtx (<HWIDE>mode);
    emit_insn (gen_rtx_SET (op2e, simplify_gen_subreg (<HWIDE>mode, operands[2], <MODE>mode, 0)));
    emit_insn (gen_kvx_zxo<hwidenx> (op2o, operands[2]));
    rtx op1o = gen_reg_rtx (<HWIDE>mode), op1e = gen_reg_rtx (<HWIDE>mode);
    emit_insn (gen_rtx_SET (op1e, simplify_gen_subreg (<HWIDE>mode, operands[1], <MODE>mode, 0)));
    emit_insn (gen_kvx_qxo<hwidenx> (op1o, operands[1]));
    rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
    emit_insn (gen_mul<hwide>3 (op0o, op1o, op2o));
    emit_insn (gen_mul<hwide>3 (op0e, op1e, op2e));
    rtx opte = gen_reg_rtx (<MODE>mode);
    emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
    emit_insn (gen_kvx_zxe<hwidenx> (op0e, opte));
    emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));
    DONE;
  }
)

(define_expand "mulv64qi3"
  [(set (match_operand:V64QI 0 "register_operand" "")
        (mult:V64QI (match_operand:V64QI 1 "register_operand" "")
                    (match_operand:V64QI 2 "register_operand" "")))]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (V64QImode);
    unsigned half_size = GET_MODE_SIZE (V32QImode);
    for (unsigned offset = 0; offset < mode_size; offset += half_size)
      {
        rtx operand0 = gen_reg_rtx (V32QImode);
        rtx operand1 = gen_reg_rtx (V32QImode);
        rtx operand2 = gen_reg_rtx (V32QImode);
        emit_move_insn (operand1, simplify_gen_subreg(V32QImode, operands[1], V64QImode, offset));
        emit_move_insn (operand2, simplify_gen_subreg(V32QImode, operands[2], V64QImode, offset));
        emit_insn (gen_mulv32qi3 (operand0, operand1, operand2));
        emit_move_insn (simplify_gen_subreg(V32QImode, operands[0], V64QImode, offset), operand0);
      }
    DONE;
  }
)

(define_expand "<prefix><mode>3"
  [(set (match_operand:VXQI 0 "register_operand" "")
        (BINDIV:VXQI (match_operand:VXQI 1 "register_operand" "")
                       (match_operand:VXQI 2 "register_operand" "")))]
  ""
  {
    if (KV3_1)
      {
        rtx op2o = gen_reg_rtx (<HWIDE>mode), op2e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op2o, operands[2]));
        emit_insn (gen_kvx_qxe<hwidenx> (op2e, operands[2]));
        rtx op1o = gen_reg_rtx (<HWIDE>mode), op1e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op1o, operands[1]));
        emit_insn (gen_kvx_qxe<hwidenx> (op1e, operands[1]));
        rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_<prefix><hwide>3 (op0o, op1o, op2o));
        emit_insn (gen_<prefix><hwide>3 (op0e, op1e, op2e));
        rtx opto = gen_reg_rtx (<MODE>mode), opte = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (opto, gen_rtx_SUBREG (<MODE>mode, op0o, 0)));
        emit_insn (gen_kvx_qxe<hwidenx> (op0o, opto));
        if (<set8msb>)
          {
            emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
            emit_insn (gen_kvx_zxe<hwidenx> (op0e, opte));
          }
        emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));
      }
    else if ((KV3_2||KV4))
      {
        rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__<prefix><mode>3"),
                                            operands[0], LCT_CONST, <MODE>mode,
                                            operands[1], <MODE>mode, operands[2], <MODE>mode);
        if (dest != operands[0])
          emit_move_insn (operands[0], dest);
      }
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_expand "<prefix><mode>3"
  [(set (match_operand:VXQI 0 "register_operand" "")
        (BINMOD:VXQI (match_operand:VXQI 1 "register_operand" "")
                       (match_operand:VXQI 2 "register_operand" "")))]
  ""
  {
    if (KV3_1)
      {
        rtx op2o = gen_reg_rtx (<HWIDE>mode), op2e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op2o, operands[2]));
        emit_insn (gen_kvx_qxe<hwidenx> (op2e, operands[2]));
        rtx op1o = gen_reg_rtx (<HWIDE>mode), op1e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op1o, operands[1]));
        emit_insn (gen_kvx_qxe<hwidenx> (op1e, operands[1]));
        rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_<prefix><hwide>3 (op0o, op1o, op2o));
        emit_insn (gen_<prefix><hwide>3 (op0e, op1e, op2e));
        rtx opte = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
        emit_insn (gen_kvx_zxo<hwidenx> (op0e, opte));
        emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));
      }
    else if ((KV3_2||KV4))
      {
        rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__<prefix><mode>3"),
                                            operands[0], LCT_CONST, <MODE>mode,
                                            operands[1], <MODE>mode, operands[2], <MODE>mode);
        if (dest != operands[0])
          emit_move_insn (operands[0], dest);
      }
    else
      gcc_unreachable ();
    DONE;
  }
)

;; abd<m>3 abds<m>3
(define_expand "<MINUS:abdm><mode>3_1"
  [(set (match_operand:VXQI 0 "register_operand" "")
        (MINUS:VXQI (smax:VXQI (match_operand:VXQI 1 "register_operand" "")
                               (match_operand:VXQI 2 "register_operand" ""))
                    (smin:VXQI (match_dup 1) (match_dup 2))))]
  ""
  {
    if (KV3_1)
      {
        rtx op2o = gen_reg_rtx (<HWIDE>mode), op2e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op2o, operands[2]));
        emit_insn (gen_kvx_qxe<hwidenx> (op2e, operands[2]));
        rtx op1o = gen_reg_rtx (<HWIDE>mode), op1e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op1o, operands[1]));
        emit_insn (gen_kvx_qxe<hwidenx> (op1e, operands[1]));
        rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_<MINUS:abdm><hwide>3 (op0o, op1o, op2o));
        emit_insn (gen_<MINUS:abdm><hwide>3 (op0e, op1e, op2e));
        rtx opto = gen_reg_rtx (<MODE>mode), opte = gen_reg_rtx (<MODE>mode);
        if (<set8lsb>)
          {
            emit_insn (gen_rtx_SET (opto, gen_rtx_SUBREG (<MODE>mode, op0o, 0)));
            emit_insn (gen_kvx_qxo<hwidenx> (op0o, opto));
          }
        emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
        emit_insn (gen_kvx_zxo<hwidenx> (op0e, opte));
        emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));
        DONE;
      }
  }
)

;; abdu<m>3
(define_expand "abdu<mode>3_1"
  [(set (match_operand:VXQI 0 "register_operand" "")
        (minus:VXQI (umax:VXQI (match_operand:VXQI 1 "register_operand" "")
                               (match_operand:VXQI 2 "register_operand" ""))
                    (umin:VXQI (match_dup 1) (match_dup 2))))]
  ""
  {
    if (KV3_1)
      {
        rtx op2o = gen_reg_rtx (<HWIDE>mode), op2e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op2o, operands[2]));
        emit_insn (gen_kvx_qxe<hwidenx> (op2e, operands[2]));
        rtx op1o = gen_reg_rtx (<HWIDE>mode), op1e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op1o, operands[1]));
        emit_insn (gen_kvx_qxe<hwidenx> (op1e, operands[1]));
        rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_abdu<hwide>3 (op0o, op1o, op2o));
        emit_insn (gen_abdu<hwide>3 (op0e, op1e, op2e));
        rtx opte = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
        emit_insn (gen_kvx_zxo<hwidenx> (op0e, opte));
        emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));
        DONE;
      }
  }
)

;; avg<m>3_floor uavg<m>3_floor avg<m>3_ceil uavg<m>3_ceil
(define_expand "<avgpre><mode><avgpost>"
  [(set (match_operand:VXQI 0 "register_operand" "")
        (unspec:VXQI [(match_operand:VXQI 1 "register_operand" "")
                      (match_operand:VXQI 2 "register_operand" "")] UNSPEC_AVGI))]
  ""
  {
    if (KV3_1)
      {
        rtx op2o = gen_reg_rtx (<HWIDE>mode), op2e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op2o, operands[2]));
        emit_insn (gen_kvx_qxe<hwidenx> (op2e, operands[2]));
        if (<avground>)
          {
            // Add 0xFF to the low bytes so the rounding propagtes to the high bytes.
            rtx bias =  gen_reg_rtx (DImode);
            emit_insn (gen_rtx_SET (bias, GEN_INT (0x00FF00FF00FF00FF)));
            emit_insn (gen_rtx_SET (op2o, gen_rtx_UNSPEC (<HWIDE>mode, gen_rtvec (2, op2o, bias), UNSPEC_ADDD)));
            emit_insn (gen_rtx_SET (op2e, gen_rtx_UNSPEC (<HWIDE>mode, gen_rtvec (2, op2e, bias), UNSPEC_ADDD)));
          }
        rtx op1o = gen_reg_rtx (<HWIDE>mode), op1e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op1o, operands[1]));
        emit_insn (gen_kvx_qxe<hwidenx> (op1e, operands[1]));
        rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_<avgpre><hwide><avgpost> (op0o, op1o, op2o));
        emit_insn (gen_<avgpre><hwide><avgpost> (op0e, op1e, op2e));
        rtx opto = gen_reg_rtx (<MODE>mode), opte = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (opto, gen_rtx_SUBREG (<MODE>mode, op0o, 0)));
        emit_insn (gen_kvx_qxo<hwidenx> (op0o, opto));
        emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
        emit_insn (gen_kvx_zxo<hwidenx> (op0e, opte));
        emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));
        DONE;
      }
  }
)
(define_insn "*<avgpre>v8qi<avgpost>_2"
  [(set (match_operand:V8QI 0 "register_operand" "=r")
        (unspec:V8QI [(match_operand:V8QI 1 "register_operand" "r")
                      (match_operand:V8QI 2 "register_operand" "r")] UNSPEC_AVGI))]
  "(KV3_2||KV4)"
  "<avgm>bo %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)
(define_insn "*<avgpre>v16qi<avgpost>_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (unspec:V16QI [(match_operand:V16QI 1 "register_operand" "r")
                       (match_operand:V16QI 2 "register_operand" "r")] UNSPEC_AVGI))]
  "(KV3_2||KV4)"
  "<avgm>bo %x0 = %x1, %x2\n\t<avgm>bo %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<avgpre>v32qi<avgpost>_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (unspec:V32QI [(match_operand:V32QI 1 "register_operand" "r")
                       (match_operand:V32QI 2 "register_operand" "r")] UNSPEC_AVGI))]
  "(KV3_2||KV4)"
  {
    return "<avgm>bo %x0 = %x1, %x2\n\t<avgm>bo %y0 = %y1, %y2\n\t"
           "<avgm>bo %z0 = %z1, %z2\n\t<avgm>bo %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

;; ashl<m>3 ssashl<m>3 usashl<m>3
(define_expand "<prefix><mode>3"
  [(set (match_operand:VXQI 0 "register_operand" "")
        (BINSHL:VXQI (match_operand:VXQI 1 "register_operand" "")
                     (match_operand:SI 2 "reg_shift_operand" "")))]
  ""
  {
    if (KV3_1)
      {
        rtx op2 = NULL_RTX;
        if (CONST_INT_P (operands[2]))
          op2 = GEN_INT (INTVAL (operands[2]) & 0x7);
        else
          {
            op2 = gen_reg_rtx (SImode);
            emit_insn (gen_andsi3 (op2, operands[2], GEN_INT (0x7)));
          }
        rtx op1o = gen_reg_rtx (<HWIDE>mode), op1e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op1o, operands[1]));
        emit_insn (gen_kvx_qxe<hwidenx> (op1e, operands[1]));
        rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_<prefix><hwide>3 (op0o, op1o, op2));
        emit_insn (gen_<prefix><hwide>3 (op0e, op1e, op2));
        rtx opto = gen_reg_rtx (<MODE>mode), opte = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
        emit_insn (gen_kvx_zxo<hwidenx> (op0e, opte));
        if (<set8lsb>)
          {
            emit_insn (gen_rtx_SET (opto, gen_rtx_SUBREG (<MODE>mode, op0o, 0)));
            emit_insn (gen_kvx_qxo<hwidenx> (op0o, opto));
          }
        emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));
        DONE;
      }
  }
)
(define_insn "*<prefix>v8qi3_2"
  [(set (match_operand:V8QI 0 "register_operand" "=r")
        (BINSHLRT:V8QI (match_operand:V8QI 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  "(KV3_2||KV4)"
  "<stem>bos %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)
(define_insn "*<prefix>v16qi3_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINSHLRT:V16QI (match_operand:V16QI 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  "(KV3_2||KV4)"
  "<stem>bos %x0 = %x1, %2\n\t<stem>bos %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length" "8")]
)
(define_insn "*<prefix>v32qi3_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (BINSHLRT:V32QI (match_operand:V32QI 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  "(KV3_2||KV4)"
  {
    return "<stem>bos %x0 = %x1, %2\n\t<stem>bos %y0 = %y1, %2\n\t"
           "<stem>bos %z0 = %z1, %2\n\t<stem>bos %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length" "16")]
)
;; Special case for LITE.
(define_insn "*<prefix>v8qi3_2"
  [(set (match_operand:V8QI 0 "register_operand" "=r")
        (BINSHLRL:V8QI (match_operand:V8QI 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  "(KV3_2||KV4)"
  "<stem>bos %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)
(define_insn "*<prefix>v16qi3_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINSHLRL:V16QI (match_operand:V16QI 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  "(KV3_2||KV4)"
  "<stem>bos %x0 = %x1, %2\n\t<stem>bos %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length" "8")]
)
(define_insn_and_split "*<prefix>v32qi3_2"
  [(set (match_operand:V32QI 0 "register_operand" "=&r,r")
        (BINSHLRL:V32QI (match_operand:V32QI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "reg_shift_operand" "r,U06")))]
  "(KV3_2||KV4)"
  "#"
  "(KV3_2||KV4) && reload_completed"
  [(set (subreg:V16QI (match_dup 0) 0)
        (BINSHLRL:V16QI (subreg:V16QI (match_dup 1) 0)
                        (match_dup 2)))
   (set (subreg:V16QI (match_dup 0) 16)
        (BINSHLRL:V16QI (subreg:V16QI (match_dup 1) 16)
                        (match_dup 2)))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

;; lshr<m>3 ashr<m>3
(define_expand "<prefix><mode>3"
  [(set (match_operand:VXQI 0 "register_operand" "")
        (BINSHR:VXQI (match_operand:VXQI 1 "register_operand" "")
                     (match_operand:SI 2 "reg_shift_operand" "")))]
  ""
  {
    if (KV3_1)
      {
        rtx op2 = NULL_RTX, op2p8 = NULL_RTX;
        if (CONST_INT_P (operands[2]))
          {
            op2 = GEN_INT (INTVAL (operands[2]) & 0x7);
            op2p8 = GEN_INT ((INTVAL (operands[2]) & 0x7) + 8);
          }
        else
          {
            op2 = gen_reg_rtx (SImode), op2p8 = gen_reg_rtx (SImode);
            emit_insn (gen_andsi3 (op2, operands[2], GEN_INT (0x7)));
            emit_insn (gen_addsi3 (op2p8, op2, GEN_INT (8)));
          }
        rtx op1o = gen_reg_rtx (<HWIDE>mode), op1e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op1o, operands[1]));
        emit_insn (gen_kvx_qxe<hwidenx> (op1e, operands[1]));
        rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_<prefix><hwide>3 (op0o, op1o, op2));
        emit_insn (gen_<prefix><hwide>3 (op0e, op1e, op2p8));
        rtx opto = gen_reg_rtx (<MODE>mode), opte = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (opto, gen_rtx_SUBREG (<MODE>mode, op0o, 0)));
        emit_insn (gen_kvx_qxo<hwidenx> (op0o, opto));
        if (<set8msb>)
          {
            emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
            emit_insn (gen_kvx_zxe<hwidenx> (op0e, opte));
          }
        emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));
        DONE;
      }
  }
)

;; sshr<m>3
(define_expand "sshr<mode>3"
  [(match_operand:VXQI 0 "register_operand" "")
   (match_operand:VXQI 1 "register_operand" "")
   (match_operand:SI 2 "register_operand" "")]
  ""
  {
    if (KV3_1)
      {
        rtx const8 = GEN_INT (8);
        rtx op2 = gen_reg_rtx (SImode), op2p8 = gen_reg_rtx (SImode);
        emit_insn (gen_andsi3 (op2, operands[2], GEN_INT (0x7)));
        emit_insn (gen_addsi3 (op2p8, op2, const8));
        rtx op1o = gen_reg_rtx (<HWIDE>mode), op1e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_kvx_qxo<hwidenx> (op1o, operands[1]));
        emit_insn (gen_kvx_qxe<hwidenx> (op1e, operands[1]));
        rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_sshr<hwide>3 (op0o, op1o, op2p8));
        emit_insn (gen_sshr<hwide>3 (op0e, op1e, op2p8));
        rtx opto = gen_reg_rtx (<MODE>mode), opte = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (opto, gen_rtx_SUBREG (<MODE>mode, op0o, 0)));
        emit_insn (gen_kvx_qxe<hwidenx> (op0o, opto));
        emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
        emit_insn (gen_kvx_zxe<hwidenx> (op0e, opte));
        emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));
      }
    else
      {
        rtvec vec = gen_rtvec (2, operands[1], operands[2]);
        rtx select = gen_rtx_UNSPEC (<MODE>mode, vec, UNSPEC_SRS);
        emit_insn (gen_rtx_SET (operands[0], select));
      }
    DONE;
  }
)
(define_insn "*sshrv8qi3_2"
  [(set (match_operand:V8QI 0 "register_operand" "=r")
        (unspec:V8QI [(match_operand:V8QI 1 "register_operand" "r")
                      (match_operand:SI 2 "reg_shift_operand" "rU06")] UNSPEC_SRS))]
  "(KV3_2||KV4)"
  "srsbos %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)
(define_insn "*sshrv16qi_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (unspec:V16QI [(match_operand:V16QI 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")] UNSPEC_SRS))]
  "(KV3_2||KV4)"
  "srsbos %x0 = %x1, %2\n\tsrsbos %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length" "8")]
)
(define_insn_and_split "*sshrv32qi_2"
  [(set (match_operand:V32QI 0 "register_operand" "=&r,r")
        (unspec:V32QI [(match_operand:V32QI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "reg_shift_operand" "r,U06")] UNSPEC_SRS))]
  "(KV3_2||KV4)"
  "#"
  "(KV3_2||KV4) && reload_completed"
  [(set (subreg:V16QI (match_dup 0) 0)
        (unspec:V16QI [(subreg:V16QI (match_dup 1) 0)
                       (match_dup 2)] UNSPEC_SRS))
   (set (subreg:V16QI (match_dup 0) 16)
        (unspec:V16QI [(subreg:V16QI (match_dup 1) 16)
                       (match_dup 2)] UNSPEC_SRS))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)


;; rotl<m>3
(define_expand "rotl<mode>3"
  [(set (match_operand:VXQI 0 "register_operand" "")
        (rotate:VXQI (match_operand:VXQI 1 "register_operand" "")
                     (match_operand:SI 2 "register_operand" "")))
   (clobber (match_scratch:SI 3 ""))
   (clobber (match_scratch:VXQI 4 ""))
   (clobber (match_scratch:VXQI 5 ""))]
  ""
  {
    if (KV3_1)
      {
        rtx evenbmm = gen_reg_rtx (DImode), oddbmm = gen_reg_rtx (DImode);
        emit_insn (gen_rtx_SET (evenbmm, GEN_INT (0x4040101004040101)));
        emit_insn (gen_rtx_SET (oddbmm, GEN_INT (0x8080202008080202)));
        rtx vevenbmm = evenbmm, voddbmm = oddbmm;
        unsigned nwords = GET_MODE_SIZE (<MODE>mode) / UNITS_PER_WORD;
        if (nwords > 1)
          {
            machine_mode vmode = mode_for_vector (DImode, nwords).require ();
            vevenbmm = gen_rtx_VEC_DUPLICATE (vmode, evenbmm);
            voddbmm = gen_rtx_VEC_DUPLICATE (vmode, oddbmm);
          }
        rtx op2 = gen_reg_rtx (SImode);
        emit_insn (gen_andsi3 (op2, operands[2], GEN_INT (0x7)));
        rtx op1o = gen_reg_rtx (<HWIDE>mode), op1e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (<HWIDE>mode, gen_rtvec (2, operands[1], voddbmm), UNSPEC_SBMM8D)));
        emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (<HWIDE>mode, gen_rtvec (2, operands[1], vevenbmm), UNSPEC_SBMM8D)));
        rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_ashl<hwide>3 (op0o, op1o, op2));
        emit_insn (gen_ashl<hwide>3 (op0e, op1e, op2));
        rtx opto = gen_reg_rtx (<MODE>mode), opte = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (opto, gen_rtx_SUBREG (<MODE>mode, op0o, 0)));
        emit_insn (gen_kvx_qxo<hwidenx> (op0o, opto));
        emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
        emit_insn (gen_kvx_zxo<hwidenx> (op0e, opte));
        emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));
      }
    else
      {
        rtx operands_3 = gen_reg_rtx (SImode);
        rtx operands_4 = gen_reg_rtx (<MODE>mode);
        rtx operands_5 = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (operands_3, gen_rtx_NEG (SImode, operands[2])));
        emit_insn (gen_rtx_SET (operands_4, gen_rtx_ASHIFT (<MODE>mode, operands[1], operands[2])));
        emit_insn (gen_rtx_SET (operands_5, gen_rtx_LSHIFTRT (<MODE>mode, operands[1], operands_3)));
        emit_insn (gen_rtx_SET (operands[0], gen_rtx_IOR (<MODE>mode, operands_4, operands_5)));
      }
    DONE;
  }
)

;; rotr<m>3
(define_expand "rotr<mode>3"
  [(set (match_operand:VXQI 0 "register_operand" "")
        (rotatert:VXQI (match_operand:VXQI 1 "register_operand" "")
                       (match_operand:SI 2 "register_operand" "")))
   (clobber (match_scratch:SI 3 ""))
   (clobber (match_scratch:VXQI 4 ""))
   (clobber (match_scratch:VXQI 5 ""))]
  ""
  {
    if (KV3_1)
      {
        rtx evenbmm = gen_reg_rtx (DImode), oddbmm = gen_reg_rtx (DImode);
        emit_insn (gen_rtx_SET (evenbmm, GEN_INT (0x4040101004040101)));
        emit_insn (gen_rtx_SET (oddbmm, GEN_INT (0x8080202008080202)));
        rtx vevenbmm = evenbmm, voddbmm = oddbmm;
        unsigned nwords = GET_MODE_SIZE (<MODE>mode) / UNITS_PER_WORD;
        if (nwords > 1)
          {
            machine_mode vmode = mode_for_vector (DImode, nwords).require ();
            vevenbmm = gen_rtx_VEC_DUPLICATE (vmode, evenbmm);
            voddbmm = gen_rtx_VEC_DUPLICATE (vmode, oddbmm);
          }
        rtx op2 = gen_reg_rtx (SImode);
        emit_insn (gen_andsi3 (op2, operands[2], GEN_INT (0x7)));
        rtx op1o = gen_reg_rtx (<HWIDE>mode), op1e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (<HWIDE>mode, gen_rtvec (2, operands[1], voddbmm), UNSPEC_SBMM8D)));
        emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (<HWIDE>mode, gen_rtvec (2, operands[1], vevenbmm), UNSPEC_SBMM8D)));
        rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
        emit_insn (gen_lshr<hwide>3 (op0o, op1o, op2));
        emit_insn (gen_lshr<hwide>3 (op0e, op1e, op2));
        rtx opto = gen_reg_rtx (<MODE>mode), opte = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (opto, gen_rtx_SUBREG (<MODE>mode, op0o, 0)));
        emit_insn (gen_kvx_qxe<hwidenx> (op0o, opto));
        emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
        emit_insn (gen_kvx_zxe<hwidenx> (op0e, opte));
        emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));
      }
    else
      {
        rtx operands_3 = gen_reg_rtx (SImode);
        rtx operands_4 = gen_reg_rtx (<MODE>mode);
        rtx operands_5 = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (operands_3, gen_rtx_NEG (SImode, operands[2])));
        emit_insn (gen_rtx_SET (operands_4, gen_rtx_LSHIFTRT (<MODE>mode, operands[1], operands[2])));
        emit_insn (gen_rtx_SET (operands_5, gen_rtx_ASHIFT (<MODE>mode, operands[1], operands_3)));
        emit_insn (gen_rtx_SET (operands[0], gen_rtx_IOR (<MODE>mode, operands_4, operands_5)));
      }
    DONE;
  }
)


;; S64I (V4HI V2SI)

(define_insn "add<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (plus:S64I (match_operand:S64I 1 "register_operand" "r")
                   (match_operand:S64I 2 "register_operand" "r")))]
  ""
  "add<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "ssadd<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ss_plus:S64I (match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")))]
  ""
  "adds<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_expand "usadd<mode>3"
  [(match_operand:S64I 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand:S64I 2 "register_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usadd<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_usadd<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usadd<mode>3_1"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (us_plus:S64I (match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")))
   (clobber (match_scratch:S64I 3 "=&r"))
   (clobber (match_scratch:S64I 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (plus:S64I (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (ltu:S64I (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (ior:S64I (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
  [(set_attr "type" "alu_lite")]
)

(define_insn "usadd<mode>3_2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (us_plus:S64I (match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "addus<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*addx2<suffix>"
  [(set (match_operand:S64K 0 "register_operand" "=r")
        (plus:S64K (ashift:S64K (match_operand:S64K 1 "register_operand" "r")
                                (const_int 1))
                   (match_operand:S64K 2 "register_operand" "r")))]
  ""
  "addx2<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "*addx4<suffix>"
  [(set (match_operand:S64K 0 "register_operand" "=r")
        (plus:S64K (ashift:S64K (match_operand:S64K 1 "register_operand" "r")
                                (const_int 2))
                   (match_operand:S64K 2 "register_operand" "r")))]
  ""
  "addx4<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "*addx8<suffix>"
  [(set (match_operand:S64K 0 "register_operand" "=r")
        (plus:S64K (ashift:S64K (match_operand:S64K 1 "register_operand" "r")
                                (const_int 3))
                   (match_operand:S64K 2 "register_operand" "r")))]
  ""
  "addx8<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "*addx16<suffix>"
  [(set (match_operand:S64K 0 "register_operand" "=r")
        (plus:S64K (ashift:S64K (match_operand:S64K 1 "register_operand" "r")
                                (const_int 4))
                   (match_operand:S64K 2 "register_operand" "r")))]
  ""
  "addx16<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "sub<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (minus:S64I (match_operand:S64I 1 "register_operand" "r")
                    (match_operand:S64I 2 "register_operand" "r")))]
  ""
  "sbf<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "sssub<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ss_minus:S64I (match_operand:S64I 1 "register_operand" "r")
                       (match_operand:S64I 2 "register_operand" "r")))]
  ""
  "sbfs<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_thin")]
)

(define_expand "ussub<mode>3"
  [(match_operand:S64I 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand:S64I 2 "register_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_ussub<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_ussub<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "ussub<mode>3_1"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (us_minus:S64I (match_operand:S64I 1 "register_operand" "r")
                       (match_operand:S64I 2 "register_operand" "r")))
   (clobber (match_scratch:S64I 3 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (umin:S64I (match_dup 1) (match_dup 2)))
   (set (match_dup 0)
        (minus:S64I (match_dup 1) (match_dup 3)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
  }
  [(set_attr "type" "alu_lite")]
)

(define_insn "ussub<mode>3_2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (us_minus:S64I (match_operand:S64I 1 "register_operand" "r")
                       (match_operand:S64I 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "sbfus<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*sbfx2<suffix>"
  [(set (match_operand:S64K 0 "register_operand" "=r")
        (minus:S64K (match_operand:S64K 1 "register_operand" "r")
                    (ashift:S64K (match_operand:S64K 2 "register_operand" "r")
                                 (const_int 1))))]
  ""
  "sbfx2<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "*sbfx4<suffix>"
  [(set (match_operand:S64K 0 "register_operand" "=r")
        (minus:S64K (match_operand:S64K 1 "register_operand" "r")
                    (ashift:S64K (match_operand:S64K 2 "register_operand" "r")
                                 (const_int 2))))]
  ""
  "sbfx4<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "*sbfx8<suffix>"
  [(set (match_operand:S64K 0 "register_operand" "=r")
        (minus:S64K (match_operand:S64K 1 "register_operand" "r")
                    (ashift:S64K (match_operand:S64K 2 "register_operand" "r")
                                 (const_int 3))))]
  ""
  "sbfx8<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "*sbfx16<suffix>"
  [(set (match_operand:S64K 0 "register_operand" "=r")
        (minus:S64K (match_operand:S64K 1 "register_operand" "r")
                    (ashift:S64K (match_operand:S64K 2 "register_operand" "r")
                                 (const_int 4))))]
  ""
  "sbfx16<suffix> %0 = %2, %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "mul<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (mult:S64I (match_operand:S64I 1 "register_operand" "r")
                   (match_operand:S64I 2 "register_operand" "r")))]
  ""
  "mul<suffix> %0 = %1, %2"
  [(set_attr "type" "mult_int")]
)

(define_expand "div<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "")
        (div:S64I (match_operand:S64I 1 "register_operand" "")
                  (match_operand:S64I 2 "register_operand" "")))]
  ""
  {
    rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__div<mode>3"),
                                        operands[0], LCT_CONST, <MODE>mode,
                                        operands[1], <MODE>mode, operands[2], <MODE>mode);
    if (dest != operands[0])
      emit_move_insn (operands[0], dest);
    DONE;
  }
)

(define_expand "mod<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "")
        (mod:S64I (match_operand:S64I 1 "register_operand" "")
                  (match_operand:S64I 2 "register_operand" "")))]
  ""
  {
    rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__mod<mode>3"),
                                        operands[0], LCT_CONST, <MODE>mode,
                                        operands[1], <MODE>mode, operands[2], <MODE>mode);
    if (dest != operands[0])
      emit_move_insn (operands[0], dest);
    DONE;
  }
)

(define_expand "udiv<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "")
        (udiv:S64I (match_operand:S64I 1 "register_operand" "")
                   (match_operand:S64I 2 "register_operand" "")))]
  ""
  {
    rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__udiv<mode>3"),
                                        operands[0], LCT_CONST, <MODE>mode,
                                        operands[1], <MODE>mode, operands[2], <MODE>mode);
    if (dest != operands[0])
      emit_move_insn (operands[0], dest);
    DONE;
  }
)

(define_expand "umod<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "")
        (umod:S64I (match_operand:S64I 1 "register_operand" "")
                   (match_operand:S64I 2 "register_operand" "")))]
  ""
  {
    rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__umod<mode>3"),
                                        operands[0], LCT_CONST, <MODE>mode,
                                        operands[1], <MODE>mode, operands[2], <MODE>mode);
    if (dest != operands[0])
      emit_move_insn (operands[0], dest);
    DONE;
  }
)

(define_insn "smin<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (smin:S64I (match_operand:S64I 1 "register_operand" "r")
                   (match_operand:S64I 2 "register_operand" "r")))]
  ""
  "min<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "smax<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (smax:S64I (match_operand:S64I 1 "register_operand" "r")
                   (match_operand:S64I 2 "register_operand" "r")))]
  ""
  "max<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "umin<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (umin:S64I (match_operand:S64I 1 "register_operand" "r")
                   (match_operand:S64I 2 "register_operand" "r")))]
  ""
  "minu<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "umax<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (umax:S64I (match_operand:S64I 1 "register_operand" "r")
                   (match_operand:S64I 2 "register_operand" "r")))]
  ""
  "maxu<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "madd<mode><mode>4"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (plus:S64I (mult:S64I (match_operand:S64I 1 "register_operand" "r")
                              (match_operand:S64I 2 "register_operand" "r"))
                   (match_operand:S64I 3 "register_operand" "0")))]
  ""
  "madd<suffix> %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "msub<mode><mode>4"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (minus:S64I (match_operand:S64I 3 "register_operand" "0")
                    (mult:S64I (match_operand:S64I 1 "register_operand" "r")
                               (match_operand:S64I 2 "register_operand" "r"))))]
  ""
  "msbf<suffix> %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "ashl<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ashift:S64I (match_operand:S64I 1 "register_operand" "r")
                     (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "sll<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "ssashl<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ss_ashift:S64I (match_operand:S64I 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "sls<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_expand "usashl<mode>3"
  [(match_operand:S64I 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand:SI 2 "reg_shift_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usashl<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_usashl<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usashl<mode>3_1"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (us_ashift:S64I (match_operand:S64I 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))
   (clobber (match_scratch:S64I 3 "=&r"))
   (clobber (match_scratch:S64I 4 "=&r"))
   (clobber (match_scratch:S64I 5 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (ashift:S64I (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (lshiftrt:S64I (match_dup 3) (match_dup 2)))
   (set (match_dup 5)
        (ne:S64I (match_dup 4) (match_dup 1)))
   (set (match_dup 0)
        (ior:S64I (match_dup 3) (match_dup 5)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[5]) == SCRATCH)
      operands[5] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "usashl<mode>3_2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (us_ashift:S64I (match_operand:S64I 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  "(KV3_2||KV4)"
  "slus<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "ashr<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ashiftrt:S64I (match_operand:S64I 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "sra<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "lshr<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (lshiftrt:S64I (match_operand:S64I 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "srl<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "sshr<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r")
                      (match_operand:SI 2 "reg_shift_operand" "rU06")] UNSPEC_SRS))]
  ""
  "srs<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "avg<mode>3_floor"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")] UNSPEC_AVG))]
  ""
  "avg<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "avg<mode>3_ceil"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")] UNSPEC_AVGR))]
  ""
  "avgr<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "uavg<mode>3_floor"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")] UNSPEC_AVGU))]
  ""
  "avgu<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "uavg<mode>3_ceil"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "r")] UNSPEC_AVGRU))]
  ""
  "avgru<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
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
  [(set_attr "type" "alu_thin_x")
   (set_attr "length"        "8")]
)

(define_insn "abs<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (abs:S64I (match_operand:S64I 1 "register_operand" "r")))]
  ""
  "abs<suffix> %0 = %1"
  [(set_attr "type" "alu_thin_x")
   (set_attr "length"        "8")]
)

(define_expand "ssabs<mode>2"
  [(set (match_operand:S64I 0 "register_operand" "")
        (ss_abs:S64I (match_operand:S64I 1 "register_operand" "")))]
  ""
  ""
)

(define_insn_and_split "ssabs<mode>2_1"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ss_abs:S64I (match_operand:S64I 1 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 0)
        (ss_neg:S64I (match_dup 1)))
   (set (match_dup 0)
        (abs:S64I (match_dup 0)))]
  ""
)

(define_insn "ssabs<mode>2_2"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ss_abs:S64I (match_operand:S64I 1 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "abss<suffix> %0 = %1"
  [(set_attr "type" "alu_tiny")]
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


;; S64L

(define_insn "and<mode>3"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (and:S64L (match_operand:S64L 1 "register_operand" "r")
                  (match_operand:S64L 2 "register_operand" "r")))]
  ""
  "andd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "_nand<mode>3"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (ior:S64L (not:S64L (match_operand:S64L 1 "register_operand" "r"))
                  (not:S64L (match_operand:S64L 2 "register_operand" "r"))))]
  ""
  "nandd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "_andn<mode>3"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (and:S64L (not:S64L (match_operand:S64L 1 "register_operand" "r"))
                  (match_operand:S64L 2 "register_operand" "r")))]
  ""
  "andnd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "ior<mode>3"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (ior:S64L (match_operand:S64L 1 "register_operand" "r")
                  (match_operand:S64L 2 "register_operand" "r")))]
  ""
  "ord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "_nior<mode>3"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (and:S64L (not:S64L (match_operand:S64L 1 "register_operand" "r"))
                  (not:S64L (match_operand:S64L 2 "register_operand" "r"))))]
  ""
  "nord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "_iorn<mode>3"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (ior:S64L (not:S64L (match_operand:S64L 1 "register_operand" "r"))
                  (match_operand:S64L 2 "register_operand" "r")))]
  ""
  "ornd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "xor<mode>3"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (xor:S64L (match_operand:S64L 1 "register_operand" "r")
                  (match_operand:S64L 2 "register_operand" "r")))]
  ""
  "xord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "_nxor<mode>3"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (not:S64L (xor:S64L (match_operand:S64L 1 "register_operand" "r")
                            (match_operand:S64L 2 "register_operand" "r"))))]
  ""
  "nxord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "one_cmpl<mode>2"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (not:S64L (match_operand:S64L 1 "register_operand" "r")))]
  ""
  "notd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_expand "abd<mode>3"
  [(match_operand:S64L 0 "register_operand" "")
   (match_operand:S64L 1 "register_operand" "")
   (match_operand:S64L 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_abd<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_abd<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "abd<mode>3_1"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (minus:S64I (smax:S64I (match_operand:S64I 1 "register_operand" "r")
                               (match_operand:S64I 2 "register_operand" "r"))
                    (smin:S64I (match_dup 1) (match_dup 2))))]
  ""
  "abd<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "abd<mode>3_2"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (minus:S64L (smax:S64L (match_operand:S64L 1 "register_operand" "r")
                               (match_operand:S64L 2 "register_operand" "r"))
                    (smin:S64L (match_dup 1) (match_dup 2))))]
  ""
  "abd<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_expand "abds<mode>3"
  [(match_operand:S64L 0 "register_operand" "")
   (match_operand:S64L 1 "register_operand" "")
   (match_operand:S64L 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_abds<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_abds<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "abds<mode>3_1"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (ss_minus:S64I (smax:S64I (match_operand:S64I 1 "register_operand" "r")
                                  (match_operand:S64I 2 "register_operand" "r"))
                       (smin:S64I (match_dup 1) (match_dup 2))))
   (clobber (match_scratch:S64I 3 "=&r"))
   (clobber (match_scratch:S64I 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (smax:S64I (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (smin:S64I (match_dup 1) (match_dup 2)))
   (set (match_dup 0)
        (ss_minus:S64I (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "abds<mode>3_2"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (ss_minus:S64L (smax:S64L (match_operand:S64L 1 "register_operand" "r")
                                  (match_operand:S64L 2 "register_s32_operand" "r"))
                       (smin:S64L (match_dup 1) (match_dup 2))))]
  "(KV3_2||KV4)"
  "abds<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_expand "abdu<mode>3"
  [(match_operand:S64L 0 "register_operand" "")
   (match_operand:S64L 1 "register_operand" "")
   (match_operand:S64L 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_abdu<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_abdu<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "abdu<mode>3_1"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (minus:S64I (umax:S64I (match_operand:S64I 1 "register_operand" "r")
                               (match_operand:S64I 2 "register_operand" "r"))
                    (umin:S64I (match_dup 1) (match_dup 2))))
   (clobber (match_scratch:S64I 3 "=&r"))
   (clobber (match_scratch:S64I 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (umax:S64I (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (umin:S64I (match_dup 1) (match_dup 2)))
   (set (match_dup 0)
        (minus:S64I (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "abdu<mode>3_2"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (minus:S64L (umax:S64L (match_operand:S64L 1 "register_operand" "r")
                               (match_operand:S64L 2 "register_s32_operand" "r"))
                    (umin:S64L (match_dup 1) (match_dup 2))))]
  "(KV3_2||KV4)"
  "abdu<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_expand "trunc<wide><mode>2"
  [(set (match_operand:S64L 0 "register_operand" "")
        (truncate:S64L (match_operand:<WIDE> 1 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_trunc<truncx> (operands[0], operands[1]));
    DONE;
  }
)

(define_expand "extend<mode><wide>2"
  [(set (match_operand:<WIDE> 0 "register_operand" "")
        (sign_extend:<WIDE> (match_operand:S64L 1 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_sx<widenx> (operands[0], operands[1]));
    DONE;
  }
)

(define_expand "zero_extend<mode><wide>2"
  [(set (match_operand:<WIDE> 0 "register_operand" "")
        (zero_extend:<WIDE> (match_operand:S64L 1 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_zx<widenx> (operands[0], operands[1]));
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

(define_insn "*mulv4hiv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (mult:V4SI (sign_extend:V4SI (match_operand:V4HI 1 "register_operand" "r"))
                   (sign_extend:V4SI (match_operand:V4HI 2 "register_operand" "r"))))]
  ""
  "mulhwq %0 = %1, %2"
  [(set_attr "type" "mult_int")]
)

(define_insn "*umulv4hiv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (mult:V4SI (zero_extend:V4SI (match_operand:V4HI 1 "register_operand" "r"))
                   (zero_extend:V4SI (match_operand:V4HI 2 "register_operand" "r"))))]
  ""
  "muluhwq %0 = %1, %2"
  [(set_attr "type" "mult_int")]
)

(define_insn "maddv4hiv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (sign_extend:V4SI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                                                (match_operand:V4HI 2 "register_operand" "r")))
                   (match_operand:V4SI 3 "register_operand" "0")))]
  ""
  "maddhwq %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "umaddv4hiv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (zero_extend:V4SI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                                                (match_operand:V4HI 2 "register_operand" "r")))
                   (match_operand:V4SI 3 "register_operand" "0")))]
  ""
  "madduhwq %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "msubv4hiv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 3 "register_operand" "0")
                    (sign_extend:V4SI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                                                 (match_operand:V4HI 2 "register_operand" "r")))))]
  ""
  "msbfhwq %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "umsubv4hiv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 3 "register_operand" "0")
                    (zero_extend:V4SI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                                                 (match_operand:V4HI 2 "register_operand" "r")))))]
  ""
  "msbfuhwq %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)


;; V2SI

(define_insn "rotlv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (rotate:V2SI (match_operand:V2SI 1 "register_operand" "r")
                     (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "rolwps %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "rotrv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (rotatert:V2SI (match_operand:V2SI 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "rorwps %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*mulv2siv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (mult:V2DI (sign_extend:V2DI (match_operand:V2SI 1 "register_operand" "r"))
                   (sign_extend:V2DI (match_operand:V2SI 2 "register_operand" "r"))))]
  ""
  "mulwdp %0 = %1, %2"
  [(set_attr "type" "mult_int")]
)

(define_insn "*umulv2siv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (mult:V2DI (zero_extend:V2DI (match_operand:V2SI 1 "register_operand" "r"))
                   (zero_extend:V2DI (match_operand:V2SI 2 "register_operand" "r"))))]
  ""
  "muluwdp %0 = %1, %2"
  [(set_attr "type" "mult_int")]
)

;; S128I (V8HI V4SI)

(define_insn "ashl<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (ashift:S128I (match_operand:S128I 1 "register_operand" "r")
                      (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "sll<chunkxs> %x0 = %x1, %2\n\tsll<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length" "8")]
)

(define_insn "ssashl<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (ss_ashift:S128I (match_operand:S128I 1 "register_operand" "r")
                         (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "sls<chunkxs> %x0 = %x1, %2\n\tsls<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length" "8")]
)

(define_expand "usashl<mode>3"
  [(match_operand:S128I 0 "register_operand" "")
   (match_operand:S128I 1 "register_operand" "")
   (match_operand:SI 2 "reg_shift_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usashl<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_usashl<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usashl<mode>3_1"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (us_ashift:S128I (match_operand:S128I 1 "register_operand" "r")
                         (match_operand:SI 2 "reg_shift_operand" "rU06")))
   (clobber (match_scratch:S128I 3 "=&r"))
   (clobber (match_scratch:S128I 4 "=&r"))
   (clobber (match_scratch:S128I 5 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (ashift:S128I (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (lshiftrt:S128I (match_dup 3) (match_dup 2)))
   (set (match_dup 5)
        (ne:S128I (match_dup 4) (match_dup 1)))
   (set (match_dup 0)
        (ior:S128I (match_dup 3) (match_dup 5)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[5]) == SCRATCH)
      operands[5] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "usashl<mode>3_2"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (us_ashift:S128I (match_operand:S128I 1 "register_operand" "r")
                         (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  "(KV3_2||KV4)"
  "slus<chunkxs> %x0 = %x1, %2\n\tslus<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length" "8")]
)

(define_insn "ashr<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (ashiftrt:S128I (match_operand:S128I 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "sra<chunkxs> %x0 = %x1, %2\n\tsra<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length" "8")]
)

(define_insn "lshr<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (lshiftrt:S128I (match_operand:S128I 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "srl<chunkxs> %x0 = %x1, %2\n\tsrl<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length" "8")]
)

(define_insn "sshr<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")] UNSPEC_SRS))]
  ""
  "srs<chunkxs> %x0 = %x1, %2\n\tsrs<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length" "8")]
)

(define_insn "avg<mode>3_floor"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "r")] UNSPEC_AVG))]
  ""
  "avg<chunkx> %x0 = %x1, %x2\n\tavg<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "avg<mode>3_ceil"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "r")] UNSPEC_AVGR))]
  ""
  "avgr<chunkx> %x0 = %x1, %x2\n\tavgr<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "uavg<mode>3_floor"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "r")] UNSPEC_AVGU))]
  ""
  "avgu<chunkx> %x0 = %x1, %x2\n\tavgu<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "uavg<mode>3_ceil"
  [(set (match_operand:S128I 0 "register_operand" "=r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r")
                       (match_operand:S128I 2 "register_operand" "r")] UNSPEC_AVGRU))]
  ""
  "avgru<chunkx> %x0 = %x1, %x2\n\tavgru<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)


;; S128L (V16QI V8HI V4SI)

(define_expand "trunc<wide><mode>2"
  [(set (match_operand:S128L 0 "register_operand" "")
        (truncate:S128L (match_operand:<WIDE> 1 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_trunc<truncx> (operands[0], operands[1]));
    DONE;
  }
)

(define_expand "extend<mode><wide>2"
  [(set (match_operand:<WIDE> 0 "register_operand" "")
        (sign_extend:<WIDE> (match_operand:S128L 1 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_sx<widenx> (operands[0], operands[1]));
    DONE;
  }
)

(define_expand "zero_extend<mode><wide>2"
  [(set (match_operand:<WIDE> 0 "register_operand" "")
        (zero_extend:<WIDE> (match_operand:S128L 1 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_zx<widenx> (operands[0], operands[1]));
    DONE;
  }
)


;; V128I (V8HI V2DI)

(define_insn_and_split "mul<mode>3"
  [(set (match_operand:V128I 0 "register_operand" "=r")
        (mult:V128I (match_operand:V128I 1 "register_operand" "r")
                    (match_operand:V128I 2 "register_operand" "r")))]
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
  [(set_attr "type" "madd_int")]
)

(define_insn_and_split "*mul<mode>3_s1"
  [(set (match_operand:V128I 0 "register_operand" "=&r")
        (mult:V128I (vec_duplicate:V128I (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
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
  [(set_attr "type" "madd_int")]
)

(define_insn_and_split "*mul<mode>3_s2"
  [(set (match_operand:V128I 0 "register_operand" "=&r")
        (mult:V128I (match_operand:V128I 1 "register_operand" "r")
                    (vec_duplicate:V128I (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
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
  [(set_attr "type" "madd_int")]
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

(define_insn "add<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (plus:V128J (match_operand:V128J 1 "register_operand" "r")
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "add<chunkx> %x0 = %x1, %x2\n\tadd<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*add<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (plus:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "add<chunkx> %x0 = %1, %x2\n\tadd<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*add<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (plus:V128J (match_operand:V128J 1 "register_operand" "r")
                    (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  "add<chunkx> %x0 = %x1, %2\n\tadd<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "ssadd<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_plus:V128J (match_operand:V128J 1 "register_operand" "r")
                       (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "adds<chunkx> %x0 = %x1, %x2\n\tadds<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*ssadd<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_plus:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                       (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "adds<chunkx> %x0 = %1, %x2\n\tadds<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*ssadd<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_plus:V128J (match_operand:V128J 1 "register_operand" "r")
                       (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  "adds<chunkx> %x0 = %x1, %2\n\tadds<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_expand "usadd<mode>3"
  [(match_operand:V128J 0 "register_operand" "")
   (match_operand:V128J 1 "register_operand" "")
   (match_operand:V128J 2 "register_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usadd<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_usadd<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usadd<mode>3_1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_plus:V128J (match_operand:V128J 1 "register_operand" "r")
                       (match_operand:V128J 2 "register_operand" "r")))
   (clobber (match_scratch:V128J 3 "=&r"))
   (clobber (match_scratch:V128J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (plus:V128J (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (ltu:V128J (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (ior:V128J (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*usadd<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=&r")
        (us_plus:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                       (match_operand:V128J 2 "register_operand" "r")))
   (clobber (match_scratch:V128J 3 "=&r"))
   (clobber (match_scratch:V128J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 3)
        (plus:V128J (vec_duplicate:V128J (match_dup 1)) (match_dup 2)))
   (set (match_dup 4)
        (ltu:V128J (match_dup 3) (vec_duplicate:V128J (match_dup 1))))
   (set (match_dup 0)
        (ior:V128J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*usadd<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=&r")
        (us_plus:V128J (match_operand:V128J 1 "register_operand" "r")
                       (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))
   (clobber (match_scratch:V128J 3 "=&r"))
   (clobber (match_scratch:V128J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 3)
        (plus:V128J (match_dup 1) (vec_duplicate:V128J (match_dup 2))))
   (set (match_dup 4)
        (ltu:V128J (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (ior:V128J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "usadd<mode>3_2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_plus:V128J (match_operand:V128J 1 "register_operand" "r")
                       (match_operand:V128J 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "addus<chunkx> %x0 = %x1, %x2\n\taddus<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*usadd<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_plus:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                       (match_operand:V128J 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "addus<chunkx> %x0 = %1, %x2\n\taddus<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*usadd<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_plus:V128J (match_operand:V128J 1 "register_operand" "r")
                       (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  "(KV3_2||KV4)"
  "addus<chunkx> %x0 = %x1, %2\n\taddus<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx2<suffix>"
  [(set (match_operand:V128K 0 "register_operand" "=r")
        (plus:V128K (ashift:V128K (match_operand:V128K 1 "register_operand" "r")
                                  (const_int 1))
                    (match_operand:V128K 2 "register_operand" "r")))]
  ""
  "addx2<chunkx> %x0 = %x1, %x2\n\taddx2<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4<suffix>"
  [(set (match_operand:V128K 0 "register_operand" "=r")
        (plus:V128K (ashift:V128K (match_operand:V128K 1 "register_operand" "r")
                                  (const_int 2))
                    (match_operand:V128K 2 "register_operand" "r")))]
  ""
  "addx4<chunkx> %x0 = %x1, %x2\n\taddx4<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8<suffix>"
  [(set (match_operand:V128K 0 "register_operand" "=r")
        (plus:V128K (ashift:V128K (match_operand:V128K 1 "register_operand" "r")
                                  (const_int 3))
                    (match_operand:V128K 2 "register_operand" "r")))]
  ""
  "addx8<chunkx> %x0 = %x1, %x2\n\taddx8<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16<suffix>"
  [(set (match_operand:V128K 0 "register_operand" "=r")
        (plus:V128K (ashift:V128K (match_operand:V128K 1 "register_operand" "r")
                                  (const_int 4))
                    (match_operand:V128K 2 "register_operand" "r")))]
  ""
  "addx16<chunkx> %x0 = %x1, %x2\n\taddx16<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "sub<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (minus:V128J (match_operand:V128J 1 "register_operand" "r")
                     (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "sbf<chunkx> %x0 = %x2, %x1\n\tsbf<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sub<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (minus:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                     (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "sbf<chunkx> %x0 = %x2, %1\n\tsbf<chunkx> %y0 = %y2, %1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sub<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (minus:V128J (match_operand:V128J 1 "register_operand" "r")
                     (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  "sbf<chunkx> %x0 = %2, %x1\n\tsbf<chunkx> %y0 = %2, %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "sssub<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_minus:V128J (match_operand:V128J 1 "register_operand" "r")
                        (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "sbfs<chunkx> %x0 = %x2, %x1\n\tsbfs<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sssub<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_minus:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                        (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "sbfs<chunkx> %x0 = %x2, %1\n\tsbfs<chunkx> %y0 = %y2, %1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sssub<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_minus:V128J (match_operand:V128J 1 "register_operand" "r")
                        (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  "sbfs<chunkx> %x0 = %2, %x1\n\tsbfs<chunkx> %y0 = %2, %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_expand "ussub<mode>3"
  [(match_operand:V128J 0 "register_operand" "")
   (match_operand:V128J 1 "register_operand" "")
   (match_operand:V128J 2 "register_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_ussub<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_ussub<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "ussub<mode>3_1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_minus:V128J (match_operand:V128J 1 "register_operand" "r")
                        (match_operand:V128J 2 "register_operand" "r")))
   (clobber (match_scratch:V128J 3 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (umin:V128J (match_dup 1) (match_dup 2)))
   (set (match_dup 0)
        (minus:V128J (match_dup 1) (match_dup 3)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
  }
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*ussub<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=&r")
        (us_minus:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                        (match_operand:V128J 2 "register_operand" "r")))
   (clobber (match_scratch:V128J 3 "=&r"))
   (clobber (match_scratch:V128J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 3)
        (minus:V128J (vec_duplicate:V128J (match_dup 1)) (match_dup 2)))
   (set (match_dup 4)
        (leu:V128J (match_dup 3) (vec_duplicate:V128J (match_dup 1))))
   (set (match_dup 0)
        (and:V128J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*ussub<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=&r")
        (us_minus:V128J (match_operand:V128J 1 "register_operand" "r")
                        (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))
   (clobber (match_scratch:V128J 3 "=&r"))
   (clobber (match_scratch:V128J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 3)
        (minus:V128J (match_dup 1) (vec_duplicate:V128J (match_dup 2))))
   (set (match_dup 4)
        (leu:V128J (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (and:V128J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "ussub<mode>3_2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_minus:V128J (match_operand:V128J 1 "register_operand" "r")
                        (match_operand:V128J 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "sbfus<chunkx> %x0 = %x2, %x1\n\tsbfus<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*ussub<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_minus:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                        (match_operand:V128J 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "sbfus<chunkx> %x0 = %x2, %1\n\tsbfus<chunkx> %y0 = %y2, %1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*ussub<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_minus:V128J (match_operand:V128J 1 "register_operand" "r")
                        (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  "(KV3_2||KV4)"
  "sbfus<chunkx> %x0 = %2, %x1\n\tsbfus<chunkx> %y0 = %2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx2<suffix>"
  [(set (match_operand:V128K 0 "register_operand" "=r")
        (minus:V128K (match_operand:V128K 1 "register_operand" "r")
                     (ashift:V128K (match_operand:V128K 2 "register_operand" "r")
                                   (const_int 1))))]
  ""
  "sbfx2<chunkx> %x0 = %x2, %x1\n\tsbfx2<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4<suffix>"
  [(set (match_operand:V128K 0 "register_operand" "=r")
        (minus:V128K (match_operand:V128K 1 "register_operand" "r")
                     (ashift:V128K (match_operand:V128K 2 "register_operand" "r")
                                   (const_int 2))))]
  ""
  "sbfx4<chunkx> %x0 = %x2, %x1\n\tsbfx4<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8<suffix>"
  [(set (match_operand:V128K 0 "register_operand" "=r")
        (minus:V128K (match_operand:V128K 1 "register_operand" "r")
                     (ashift:V128K (match_operand:V128K 2 "register_operand" "r")
                                   (const_int 3))))]
  ""
  "sbfx8<chunkx> %x0 = %x2, %x1\n\tsbfx8<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16<suffix>"
  [(set (match_operand:V128K 0 "register_operand" "=r")
        (minus:V128K (match_operand:V128K 1 "register_operand" "r")
                     (ashift:V128K (match_operand:V128K 2 "register_operand" "r")
                                   (const_int 4))))]
  ""
  "sbfx16<chunkx> %x0 = %x2, %x1\n\tsbfx16<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_expand "div<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "")
        (div:V128J (match_operand:V128J 1 "register_operand" "")
                   (match_operand:V128J 2 "register_operand" "")))]
  ""
  {
    rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__div<mode>3"),
                                        operands[0], LCT_CONST, <MODE>mode,
                                        operands[1], <MODE>mode, operands[2], <MODE>mode);
    if (dest != operands[0])
      emit_move_insn (operands[0], dest);
    DONE;
  }
)

(define_expand "mod<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "")
        (mod:V128J (match_operand:V128J 1 "register_operand" "")
                   (match_operand:V128J 2 "register_operand" "")))]
  ""
  {
    rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__mod<mode>3"),
                                        operands[0], LCT_CONST, <MODE>mode,
                                        operands[1], <MODE>mode, operands[2], <MODE>mode);
    if (dest != operands[0])
      emit_move_insn (operands[0], dest);
    DONE;
  }
)

(define_expand "udiv<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "")
        (udiv:V128J (match_operand:V128J 1 "register_operand" "")
                    (match_operand:V128J 2 "register_operand" "")))]
  ""
  {
    rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__udiv<mode>3"),
                                        operands[0], LCT_CONST, <MODE>mode,
                                        operands[1], <MODE>mode, operands[2], <MODE>mode);
    if (dest != operands[0])
      emit_move_insn (operands[0], dest);
    DONE;
  }
)

(define_expand "umod<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "")
        (umod:V128J (match_operand:V128J 1 "register_operand" "")
                    (match_operand:V128J 2 "register_operand" "")))]
  ""
  {
    rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__umod<mode>3"),
                                        operands[0], LCT_CONST, <MODE>mode,
                                        operands[1], <MODE>mode, operands[2], <MODE>mode);
    if (dest != operands[0])
      emit_move_insn (operands[0], dest);
    DONE;
  }
)

(define_insn "smin<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (smin:V128J (match_operand:V128J 1 "register_operand" "r")
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "min<chunkx> %x0 = %x1, %x2\n\tmin<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*smin<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (smin:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "min<chunkx> %x0 = %1, %x2\n\tmin<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*smin<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (smin:V128J (match_operand:V128J 1 "register_operand" "r")
                    (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  "min<chunkx> %x0 = %x1, %2\n\tmin<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "smax<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (smax:V128J (match_operand:V128J 1 "register_operand" "r")
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "max<chunkx> %x0 = %x1, %x2\n\tmax<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*smax<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (smax:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "max<chunkx> %x0 = %1, %x2\n\tmax<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*smax<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (smax:V128J (match_operand:V128J 1 "register_operand" "r")
                    (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  "max<chunkx> %x0 = %x1, %2\n\tmax<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "umin<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (umin:V128J (match_operand:V128J 1 "register_operand" "r")
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "minu<chunkx> %x0 = %x1, %x2\n\tminu<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*umin<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (umin:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "minu<chunkx> %x0 = %1, %x2\n\tminu<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*umin<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (umin:V128J (match_operand:V128J 1 "register_operand" "r")
                    (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  "minu<chunkx> %x0 = %x1, %2\n\tminu<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "umax<mode>3"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (umax:V128J (match_operand:V128J 1 "register_operand" "r")
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "maxu<chunkx> %x0 = %x1, %x2\n\tmaxu<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*umax<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (umax:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                    (match_operand:V128J 2 "register_operand" "r")))]
  ""
  "maxu<chunkx> %x0 = %1, %x2\n\tmaxu<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*umax<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (umax:V128J (match_operand:V128J 1 "register_operand" "r")
                    (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  "maxu<chunkx> %x0 = %x1, %2\n\tmaxu<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "madd<mode><mode>4"
  [(set (match_operand:V128M 0 "register_operand" "=r")
        (plus:V128M (mult:V128M (match_operand:V128M 1 "register_operand" "r")
                                (match_operand:V128M 2 "register_operand" "r"))
                    (match_operand:V128M 3 "register_operand" "0")))]
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
  [(set_attr "type" "madd_int")
   (set_attr "length"      "8")]
)

(define_insn_and_split "msub<mode><mode>4"
  [(set (match_operand:V128M 0 "register_operand" "=r")
        (minus:V128M (match_operand:V128M 3 "register_operand" "0")
                    (mult:V128M (match_operand:V128M 1 "register_operand" "r")
                                (match_operand:V128M 2 "register_operand" "r"))))]
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
  [(set_attr "type" "madd_int")
   (set_attr "length"      "8")]
)


;; V128J (V8HI V4SI V2DI)

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
  [(set_attr "type" "alu_thin_x2_x")
   (set_attr "length"          "16")]
)

(define_insn "abs<mode>2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (abs:V128J (match_operand:V128J 1 "register_operand" "r")))]
  ""
  "abs<chunkx> %x0 = %x1\n\tabs<chunkx> %y0 = %y1"
  [(set_attr "type" "alu_thin_x2_x")
   (set_attr "length"          "16")]
)

(define_expand "ssabs<mode>2"
  [(set (match_operand:V128J 0 "register_operand" "")
        (ss_abs:V128J (match_operand:V128J 1 "register_operand" "")))]
  ""
  ""
)

(define_insn_and_split "ssabs<mode>2_1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_abs:V128J (match_operand:V128J 1 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 0)
        (ss_neg:V128J (match_dup 1)))
   (set (match_dup 0)
        (abs:V128J (match_dup 0)))]
  ""
)

(define_insn "ssabs<mode>2_2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_abs:V128J (match_operand:V128J 1 "register_operand" "r")))]
  "(KV3_2||KV4)"
  "abss<chunkx> %x0 = %x1\n\tabss<chunkx> %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2_x")
   (set_attr "length"          "16")]
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


;; V128L

(define_insn "and<mode>3"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (and:V128L (match_operand:V128L 1 "register_operand" "r")
                   (match_operand:V128L 2 "register_operand" "r")))]
  ""
  "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "_nand<mode>3"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (ior:V128L (not:V128L (match_operand:V128L 1 "register_operand" "r"))
                   (not:V128L (match_operand:V128L 2 "register_operand" "r"))))]
  ""
  "nandd %x0 = %x1, %x2\n\tnandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "_andn<mode>3"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (and:V128L (not:V128L (match_operand:V128L 1 "register_operand" "r"))
                   (match_operand:V128L 2 "register_operand" "r")))]
  ""
  "andnd %x0 = %x1, %x2\n\tandnd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "ior<mode>3"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (ior:V128L (match_operand:V128L 1 "register_operand" "r")
                   (match_operand:V128L 2 "register_operand" "r")))]
  ""
  "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "_nior<mode>3"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (and:V128L (not:V128L (match_operand:V128L 1 "register_operand" "r"))
                   (not:V128L (match_operand:V128L 2 "register_operand" "r"))))]
  ""
  "nord %x0 = %x1, %x2\n\tnord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "_iorn<mode>3"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (ior:V128L (not:V128L (match_operand:V128L 1 "register_operand" "r"))
                   (match_operand:V128L 2 "register_operand" "r")))]
  ""
  "ornd %x0 = %x1, %x2\n\tornd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "xor<mode>3"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (xor:V128L (match_operand:V128L 1 "register_operand" "r")
                   (match_operand:V128L 2 "register_operand" "r")))]
  ""
  "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "_nxor<mode>3"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (not:V128L (xor:V128L (match_operand:V128L 1 "register_operand" "r")
                              (match_operand:V128L 2 "register_operand" "r"))))]
  ""
  "nxord %x0 = %x1, %x2\n\tnxord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "one_cmpl<mode>2"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (not:V128L (match_operand:V128L 1 "register_operand" "r")))]
  ""
  "notd %x0 = %x1\n\tnotd %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_expand "abd<mode>3"
  [(match_operand:V128L 0 "register_operand" "")
   (match_operand:V128L 1 "register_operand" "")
   (match_operand:V128L 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_abd<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_abd<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "abd<mode>3_1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (minus:V128J (smax:V128J (match_operand:V128J 1 "register_operand" "r")
                                 (match_operand:V128J 2 "register_operand" "r"))
                     (smin:V128J (match_dup 1) (match_dup 2))))]
  ""
  "abd<chunkx> %x0 = %x1, %x2\n\tabd<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "abd<mode>3_2"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (minus:V128L (smax:V128L (match_operand:V128L 1 "register_operand" "r")
                                 (match_operand:V128L 2 "register_operand" "r"))
                     (smin:V128L (match_dup 1) (match_dup 2))))]
  ""
  "abd<chunkx> %x0 = %x1, %x2\n\tabd<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*abd<suffix>_s1"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (minus:V128L (smax:V128L (vec_duplicate:V128L (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                                 (match_operand:V128L 2 "register_operand" "r"))
                     (smin:V128L (vec_duplicate:V128L (match_dup 1)) (match_dup 2))))]
  ""
  "abd<chunkx> %x0 = %1, %x2\n\tabd<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*abd<suffix>_s2"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (minus:V128L (smax:V128L (match_operand:V128L 1 "register_operand" "r")
                                 (vec_duplicate:V128L (match_operand:<CHUNK> 2 "nonmemory_operand" "r")))
                     (smin:V128L (match_dup 1) (vec_duplicate:V128L (match_dup 2)))))]
  ""
  "abd<chunkx> %x0 = %x1, %2\n\tabd<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_expand "abds<mode>3"
  [(match_operand:V128L 0 "register_operand" "")
   (match_operand:V128L 1 "register_operand" "")
   (match_operand:V128L 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_abds<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_abds<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "abds<mode>3_1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (ss_minus:V128J (smax:V128J (match_operand:V128J 1 "register_operand" "r")
                                    (match_operand:V128J 2 "register_operand" "r"))
                        (smin:V128J (match_dup 1) (match_dup 2))))
   (clobber (match_scratch:V128J 3 "=&r"))
   (clobber (match_scratch:V128J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (smax:V128J (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (smin:V128J (match_dup 1) (match_dup 2)))
   (set (match_dup 0)
        (ss_minus:V128J (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "abds<mode>3_2"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (ss_minus:V128L (smax:V128L (match_operand:V128L 1 "register_operand" "r")
                                    (match_operand:V128L 2 "register_operand" "r"))
                        (smin:V128L (match_dup 1) (match_dup 2))))]
  "(KV3_2||KV4)"
  "abds<chunkx> %x0 = %x1, %x2\n\tabds<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*abds<suffix>_s1"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (ss_minus:V128L (smax:V128L (vec_duplicate:V128L (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                                    (match_operand:V128L 2 "register_operand" "r"))
                        (smin:V128L (vec_duplicate:V128L (match_dup 1)) (match_dup 2))))]
  "(KV3_2||KV4)"
  "abds<chunkx> %x0 = %1, %x2\n\tabds<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*abds<suffix>_s2"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (ss_minus:V128L (smax:V128L (match_operand:V128L 1 "register_operand" "r")
                                    (vec_duplicate:V128L (match_operand:<CHUNK> 2 "nonmemory_operand" "r")))
                        (smin:V128L (match_dup 1) (vec_duplicate:V128L (match_dup 2)))))]
  "(KV3_2||KV4)"
  "abds<chunkx> %x0 = %x1, %2\n\tabds<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_expand "abdu<mode>3"
  [(match_operand:V128L 0 "register_operand" "")
   (match_operand:V128L 1 "register_operand" "")
   (match_operand:V128L 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_abdu<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_abdu<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "abdu<mode>3_1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (minus:V128J (umax:V128J (match_operand:V128J 1 "register_operand" "r")
                                 (match_operand:V128J 2 "register_operand" "r"))
                     (umin:V128J (match_dup 1) (match_dup 2))))
   (clobber (match_scratch:V128J 3 "=&r"))
   (clobber (match_scratch:V128J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (umax:V128J (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (umin:V128J (match_dup 1) (match_dup 2)))
   (set (match_dup 0)
        (minus:V128J (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "abdu<mode>3_2"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (minus:V128L (umax:V128L (match_operand:V128L 1 "register_operand" "r")
                                 (match_operand:V128L 2 "register_operand" "r"))
                     (umin:V128L (match_dup 1) (match_dup 2))))]
  "(KV3_2||KV4)"
  "abdu<chunkx> %x0 = %x1, %x2\n\tabdu<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*abdu<suffix>_s1"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (minus:V128L (umax:V128L (vec_duplicate:V128L (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                                 (match_operand:V128L 2 "register_operand" "r"))
                     (umin:V128L (vec_duplicate:V128L (match_dup 1)) (match_dup 2))))]
  "(KV3_2||KV4)"
  "abdu<chunkx> %x0 = %1, %x2\n\tabdu<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*abdu<suffix>_s2"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (minus:V128L (umax:V128L (match_operand:V128L 1 "register_operand" "r")
                                 (vec_duplicate:V128L (match_operand:<CHUNK> 2 "nonmemory_operand" "r")))
                     (umin:V128L (match_dup 1) (vec_duplicate:V128L (match_dup 2)))))]
  "(KV3_2||KV4)"
  "abdu<chunkx> %x0 = %x1, %2\n\tabdu<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)


;; V4SI

(define_insn "mulv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                   (match_operand:V4SI 2 "register_operand" "r")))]
  ""
  "mulwq %0 = %1, %2"
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "madd_int") (const_string "mult_int")))]
)

(define_insn_and_split "*maddv4siv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                              (match_operand:V4SI 2 "register_operand" "r"))
                   (match_operand:V4SI 3 "register_operand" "0")))]
  "(KV3_2||KV4)"
  "maddwq %0 = %1, %2"
  "KV4 && reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (plus:V2SI (mult:V2SI (subreg:V2SI (match_dup 1) 0)
                              (subreg:V2SI (match_dup 2) 0))
                   (subreg:V2SI (match_dup 3) 0)))
   (set (subreg:V2SI (match_dup 0) 8)
        (plus:V2SI (mult:V2SI (subreg:V2SI (match_dup 1) 8)
                              (subreg:V2SI (match_dup 2) 8))
                   (subreg:V2SI (match_dup 3) 8)))]
  ""
  [(set_attr "type" "madd_int")]
)

(define_insn "*maddv2siv2si4"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (plus:V2SI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r")
                              (match_operand:V2SI 2 "register_operand" "r"))
                   (match_operand:V2SI 3 "register_operand" "0")))]
  "KV4"
  "maddwp %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn_and_split "*msubv4siv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 3 "register_operand" "0")
                    (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                               (match_operand:V4SI 2 "register_operand" "r"))))]
  "(KV3_2||KV4)"
  "msbfwq %0 = %1, %2"
  "KV4 && reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (minus:V2SI (subreg:V2SI (match_dup 3) 0)
                    (mult:V2SI (subreg:V2SI (match_dup 1) 0)
                               (subreg:V2SI (match_dup 2) 0))))
   (set (subreg:V2SI (match_dup 0) 8)
        (minus:V2SI (subreg:V2SI (match_dup 3) 8)
                    (mult:V2SI (subreg:V2SI (match_dup 1) 8)
                               (subreg:V2SI (match_dup 2) 8))))]
  ""
  [(set_attr "type" "madd_int")]
)

(define_insn "*msubv4siv4si4"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (minus:V2SI (match_operand:V2SI 3 "register_operand" "0")
                    (mult:V2SI (match_operand:V2SI 1 "register_operand" "r")
                               (match_operand:V2SI 2 "register_operand" "r"))))]
  "KV4"
  "msbfwp %0 = %1, %2"
  [(set_attr "type" "madd_int")]
)

(define_insn "rotlv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (rotate:V4SI (match_operand:V4SI 1 "register_operand" "r")
                     (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "rolwps %x0 = %x1, %2\n\trolwps %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length" "8")]
)

(define_insn "rotrv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (rotatert:V4SI (match_operand:V4SI 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "rorwps %x0 = %x1, %2\n\trorwps %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length" "8")]
)


;; V2DI

(define_insn "ashlv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (ashift:V2DI (match_operand:V2DI 1 "register_operand" "r")
                     (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "slld %x0 = %x1, %2\n\tslld %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length" "8")]
)

(define_insn "ssashlv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (ss_ashift:V2DI (match_operand:V2DI 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "slsd %x0 = %x1, %2\n\tslsd %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length" "8")]
)

(define_expand "usashlv2di3"
  [(match_operand:V2DI 0 "register_operand" "")
   (match_operand:V2DI 1 "register_operand" "")
   (match_operand:SI 2 "reg_shift_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usashlv2di3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_usashlv2di3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usashlv2di3_1"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (us_ashift:V2DI (match_operand:V2DI 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))
   (clobber (match_scratch:V2DI 3 "=&r"))
   (clobber (match_scratch:V2DI 4 "=&r"))
   (clobber (match_scratch:V2DI 5 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (ashift:V2DI (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (lshiftrt:V2DI (match_dup 3) (match_dup 2)))
   (set (match_dup 5)
        (ne:V2DI (match_dup 4) (match_dup 1)))
   (set (match_dup 0)
        (ior:V2DI (match_dup 3) (match_dup 5)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (V2DImode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (V2DImode);
    if (GET_CODE (operands[5]) == SCRATCH)
      operands[5] = gen_reg_rtx (V2DImode);
  }
)

(define_insn "usashlv2di3_2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (us_ashift:V2DI (match_operand:V2DI 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  "(KV3_2||KV4)"
  "slusd %x0 = %x1, %2\n\tslusd %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length" "8")]
)

(define_insn "ashrv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (ashiftrt:V2DI (match_operand:V2DI 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "srad %x0 = %x1, %2\n\tsrad %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length" "8")]
)

(define_insn "lshrv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (lshiftrt:V2DI (match_operand:V2DI 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  "srld %x0 = %x1, %2\n\tsrld %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length" "8")]
)

(define_insn "sshrv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2DI 1 "register_operand" "r")
                      (match_operand:SI 2 "reg_shift_operand" "rU06")] UNSPEC_SRS))]
  ""
  "srsd %x0 = %x1, %2\n\tsrsd %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length" "8")]
)


;; S256I (V16HI V8SI)

(define_expand "ashl<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "")
        (ashift:S256I (match_operand:S256I 1 "register_operand" "")
                      (match_operand:SI 2 "reg_shift_operand" "")))]
  ""
  ""
)

(define_insn_and_split "ashl<mode>3_1"
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (ashift:S256I (match_operand:S256I 1 "register_operand" "r,r")
                      (match_operand:SI 2 "reg_shift_operand" "r,U06")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ashift:<HALF> (subreg:<HALF> (match_dup 1) 0)
                       (match_dup 2)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ashift:<HALF> (subreg:<HALF> (match_dup 1) 16)
                       (match_dup 2)))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn "ashl<mode>3_2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (ashift:S256I (match_operand:S256I 1 "register_operand" "r")
                      (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  "(KV3_2||KV4)"
  {
    return "sll<chunkxs> %x0 = %x1, %2\n\tsll<chunkxs> %y0 = %y1, %2\n\t"
           "sll<chunkxs> %z0 = %z1, %2\n\tsll<chunkxs> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length" "16")]
)

(define_insn_and_split "ssashl<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (ss_ashift:S256I (match_operand:S256I 1 "register_operand" "r,r")
                         (match_operand:SI 2 "reg_shift_operand" "r,U06")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_ashift:<HALF> (subreg:<HALF> (match_dup 1) 0)
                          (match_dup 2)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_ashift:<HALF> (subreg:<HALF> (match_dup 1) 16)
                          (match_dup 2)))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_expand "usashl<mode>3"
  [(match_operand:S256I 0 "register_operand" "")
   (match_operand:S256I 1 "register_operand" "")
   (match_operand:SI 2 "reg_shift_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usashl<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_usashl<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usashl<mode>3_1"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (us_ashift:S256I (match_operand:S256I 1 "register_operand" "r")
                         (match_operand:SI 2 "reg_shift_operand" "rU06")))
   (clobber (match_scratch:S256I 3 "=&r"))
   (clobber (match_scratch:S256I 4 "=&r"))
   (clobber (match_scratch:S256I 5 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (ashift:S256I (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (lshiftrt:S256I (match_dup 3) (match_dup 2)))
   (set (match_dup 5)
        (ne:S256I (match_dup 4) (match_dup 1)))
   (set (match_dup 0)
        (ior:S256I (match_dup 3) (match_dup 5)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[5]) == SCRATCH)
      operands[5] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn_and_split "usashl<mode>3_2"
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (us_ashift:S256I (match_operand:S256I 1 "register_operand" "r,r")
                         (match_operand:SI 2 "reg_shift_operand" "r,U06")))]
  "(KV3_2||KV4)"
  "#"
  "(KV3_2||KV4) && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (us_ashift:<HALF> (subreg:<HALF> (match_dup 1) 0)
                          (match_dup 2)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (us_ashift:<HALF> (subreg:<HALF> (match_dup 1) 16)
                          (match_dup 2)))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_expand "ashr<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "")
        (ashiftrt:S256I (match_operand:S256I 1 "register_operand" "")
                        (match_operand:SI 2 "reg_shift_operand" "")))]
  ""
  ""
)

(define_insn_and_split "ashr<mode>3_1"
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (ashiftrt:S256I (match_operand:S256I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "reg_shift_operand" "r,U06")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ashiftrt:<HALF> (subreg:<HALF> (match_dup 1) 0)
                         (match_dup 2)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ashiftrt:<HALF> (subreg:<HALF> (match_dup 1) 16)
                         (match_dup 2)))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn "ashr<mode>3_2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (ashiftrt:S256I (match_operand:S256I 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  "(KV3_2||KV4)"
  {
    return "sra<chunkxs> %x0 = %x1, %2\n\tsra<chunkxs> %y0 = %y1, %2\n\t"
           "sra<chunkxs> %z0 = %z1, %2\n\tsra<chunkxs> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length" "16")]
)

(define_expand "lshr<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "")
        (lshiftrt:S256I (match_operand:S256I 1 "register_operand" "")
                        (match_operand:SI 2 "reg_shift_operand" "")))]
  ""
  ""
)

(define_insn_and_split "lshr<mode>3_1"
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (lshiftrt:S256I (match_operand:S256I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "reg_shift_operand" "r,U06")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (lshiftrt:<HALF> (subreg:<HALF> (match_dup 1) 0)
                         (match_dup 2)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (lshiftrt:<HALF> (subreg:<HALF> (match_dup 1) 16)
                         (match_dup 2)))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn "lshr<mode>3_2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (lshiftrt:S256I (match_operand:S256I 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  "(KV3_2||KV4)"
  {
    return "srl<chunkxs> %x0 = %x1, %2\n\tsrl<chunkxs> %y0 = %y1, %2\n\t"
           "srl<chunkxs> %z0 = %z1, %2\n\tsrl<chunkxs> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length" "16")]
)

(define_insn_and_split "sshr<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r,r")
                       (match_operand:SI 2 "reg_shift_operand" "r,U06")] UNSPEC_SRS))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (match_dup 2)] UNSPEC_SRS))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (match_dup 2)] UNSPEC_SRS))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_expand "avg<mode>3_floor"
  [(set (match_operand:S256I 0 "register_operand" "")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "")
                       (match_operand:S256I 2 "register_operand" "")] UNSPEC_AVG))]
  ""
  ""
)

(define_insn_and_split "avg<mode>3_floor_1"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "r")] UNSPEC_AVG))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)] UNSPEC_AVG))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)] UNSPEC_AVG))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "avg<mode>3_floor_2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "r")] UNSPEC_AVG))]
  "(KV3_2||KV4)"
  {
    return "avg<chunkx> %x0 = %x1, %x2\n\tavg<chunkx> %y0 = %y1, %y2\n\t"
           "avg<chunkx> %z0 = %z1, %z2\n\tavg<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "avg<mode>3_ceil"
  [(set (match_operand:S256I 0 "register_operand" "")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "")
                       (match_operand:S256I 2 "register_operand" "")] UNSPEC_AVGR))]
  ""
  ""
)

(define_insn_and_split "avg<mode>3_ceil_1"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "r")] UNSPEC_AVGR))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)] UNSPEC_AVGR))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)] UNSPEC_AVGR))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "avg<mode>3_ceil_2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "r")] UNSPEC_AVGR))]
  "(KV3_2||KV4)"
  {
    return "avgr<chunkx> %x0 = %x1, %x2\n\tavgr<chunkx> %y0 = %y1, %y2\n\t"
           "avgr<chunkx> %z0 = %z1, %z2\n\tavgr<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "uavg<mode>3_floor"
  [(set (match_operand:S256I 0 "register_operand" "")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "")
                       (match_operand:S256I 2 "register_operand" "")] UNSPEC_AVGU))]
  ""
  ""
)

(define_insn_and_split "uavg<mode>3_floor_1"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "r")] UNSPEC_AVGU))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)] UNSPEC_AVGU))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)] UNSPEC_AVGU))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "uavg<mode>3_floor_2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "r")] UNSPEC_AVGU))]
  "(KV3_2||KV4)"
  {
    return "avgu<chunkx> %x0 = %x1, %x2\n\tavgu<chunkx> %y0 = %y1, %y2\n\t"
           "avgu<chunkx> %z0 = %z1, %z2\n\tavgu<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "uavg<mode>3_ceil"
  [(set (match_operand:S256I 0 "register_operand" "")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "")
                       (match_operand:S256I 2 "register_operand" "")] UNSPEC_AVGRU))]
  ""
  ""
)

(define_insn_and_split "uavg<mode>3_ceil_1"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "r")] UNSPEC_AVGRU))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)] UNSPEC_AVGRU))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)] UNSPEC_AVGRU))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "uavg<mode>3_ceil_2"
  [(set (match_operand:S256I 0 "register_operand" "=r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r")
                       (match_operand:S256I 2 "register_operand" "r")] UNSPEC_AVGRU))]
  "(KV3_2||KV4)"
  {
    return "avgru<chunkx> %x0 = %x1, %x2\n\tavgru<chunkx> %y0 = %y1, %y2\n\t"
           "avgru<chunkx> %z0 = %z1, %z2\n\tavgru<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; S256L (V32QI V16HI V8SI)

(define_expand "trunc<wide><mode>2"
  [(set (match_operand:S256L 0 "register_operand" "")
        (truncate:S256L (match_operand:<WIDE> 1 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_trunc<truncx> (operands[0], operands[1]));
    DONE;
  }
)

(define_expand "extend<mode><wide>2"
  [(set (match_operand:<WIDE> 0 "register_operand" "")
        (sign_extend:<WIDE> (match_operand:S256L 1 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_sx<widenx> (operands[0], operands[1]));
    DONE;
  }
)

(define_expand "zero_extend<mode><wide>2"
  [(set (match_operand:<WIDE> 0 "register_operand" "")
        (zero_extend:<WIDE> (match_operand:S256L 1 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_zx<widenx> (operands[0], operands[1]));
    DONE;
  }
)


;; V256I (V16HI V4DI)

(define_insn_and_split "mul<mode>3"
  [(set (match_operand:V256I 0 "register_operand" "=r")
        (mult:V256I (match_operand:V256I 1 "register_operand" "r")
                    (match_operand:V256I 2 "register_operand" "r")))]
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
  [(set_attr "type" "madd_int")]
)

(define_insn_and_split "*mul<mode>3_s1"
  [(set (match_operand:V256I 0 "register_operand" "=&r")
        (mult:V256I (vec_duplicate:V256I (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
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
  [(set_attr "type" "madd_int")]
)

(define_insn_and_split "*mul<mode>3_s2"
  [(set (match_operand:V256I 0 "register_operand" "=&r")
        (mult:V256I (match_operand:V256I 1 "register_operand" "r")
                    (vec_duplicate:V256I (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
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
  [(set_attr "type" "madd_int")]
)


;; V256J (V16HI V8SI V4DI)

(define_insn "add<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (plus:V256J (match_operand:V256J 1 "register_operand" "r")
                    (match_operand:V256J 2 "register_operand" "r")))]
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
        (plus:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
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
                    (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "add<chunkx> %x0 = %x1, %2\n\tadd<chunkx> %y0 = %y1, %2\n\t"
           "add<chunkx> %z0 = %z1, %2\n\tadd<chunkx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "ssadd<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "")
        (ss_plus:V256J (match_operand:V256J 1 "register_operand" "")
                       (match_operand:V256J 2 "register_operand" "")))]
  ""
  ""
)

(define_insn_and_split "ssadd<mode>3_1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_plus:V256J (match_operand:V256J 1 "register_operand" "r")
                       (match_operand:V256J 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_plus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_plus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "ssadd<mode>3_2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_plus:V256J (match_operand:V256J 1 "register_operand" "r")
                       (match_operand:V256J 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "adds<chunkx> %x0 = %x1, %x2\n\tadds<chunkx> %y0 = %y1, %y2\n\t"
           "adds<chunkx> %z0 = %z1, %z2\n\tadds<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*ssadd<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=&r")
        (ss_plus:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                       (match_operand:V256J 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_plus:<HALF> (vec_duplicate:<HALF> (match_dup 1))
                        (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_plus:<HALF> (vec_duplicate:<HALF> (match_dup 1))
                        (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*ssadd<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (plus:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                    (match_operand:V256J 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "adds<chunkx> %x0 = %1, %x2\n\tadds<chunkx> %y0 = %1, %y2\n\t"
           "adds<chunkx> %z0 = %1, %z2\n\tadds<chunkx> %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*ssadd<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=&r")
        (ss_plus:V256J (match_operand:V256J 1 "register_operand" "r")
                       (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_plus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                        (vec_duplicate:<HALF> (match_dup 2))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_plus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                        (vec_duplicate:<HALF> (match_dup 2))))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*ssadd<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (plus:V256J (match_operand:V256J 1 "register_operand" "r")
                    (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  "(KV3_2||KV4)"
  {
    return "adds<chunkx> %x0 = %x1, %2\n\tadds<chunkx> %y0 = %y1, %2\n\t"
           "adds<chunkx> %z0 = %z1, %2\n\tadds<chunkx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "usadd<mode>3"
  [(match_operand:V256J 0 "register_operand" "")
   (match_operand:V256J 1 "register_operand" "")
   (match_operand:V256J 2 "register_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usadd<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_usadd<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usadd<mode>3_1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (us_plus:V256J (match_operand:V256J 1 "register_operand" "r")
                       (match_operand:V256J 2 "register_operand" "r")))
   (clobber (match_scratch:V256J 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (plus:V256J (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (ltu:V256J (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (ior:V256J (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "usadd<mode>3_2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (us_plus:V256J (match_operand:V256J 1 "register_operand" "r")
                       (match_operand:V256J 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "addus<chunkx> %x0 = %x1, %x2\n\taddus<chunkx> %y0 = %y1, %y2\n\t"
           "addus<chunkx> %z0 = %z1, %z2\n\taddus<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*usadd<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=&r")
        (us_plus:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                       (match_operand:V256J 2 "register_operand" "r")))
   (clobber (match_scratch:V256J 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 3)
        (plus:V256J (vec_duplicate:V256J (match_dup 1)) (match_dup 2)))
   (set (match_dup 4)
        (ltu:V256J (match_dup 3) (vec_duplicate:V256J (match_dup 1))))
   (set (match_dup 0)
        (ior:V256J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*usadd<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (us_plus:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                       (match_operand:V256J 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "addus<chunkx> %x0 = %1, %x2\n\taddus<chunkx> %y0 = %1, %y2\n\t"
           "addus<chunkx> %z0 = %1, %z2\n\taddus<chunkx> %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*usadd<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=&r")
        (us_plus:V256J (match_operand:V256J 1 "register_operand" "r")
                       (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))
   (clobber (match_scratch:V256J 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 3)
        (plus:V256J (match_dup 1) (vec_duplicate:V256J (match_dup 2))))
   (set (match_dup 4)
        (ltu:V256J (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (ior:V256J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*usadd<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (us_plus:V256J (match_operand:V256J 1 "register_operand" "r")
                       (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  "(KV3_2||KV4)"
  {
    return "addus<chunkx> %x0 = %x1, %2\n\taddus<chunkx> %y0 = %y1, %2\n\t"
           "addus<chunkx> %z0 = %z1, %2\n\taddus<chunkx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*addx2<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (plus:V256K (ashift:V256K (match_operand:V256K 1 "register_operand" "r")
                                  (const_int 1))
                    (match_operand:V256K 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                    (const_int 1))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                    (const_int 1))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*addx2<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (plus:V256K (ashift:V256K (match_operand:V256K 1 "register_operand" "r")
                                  (const_int 1))
                    (match_operand:V256K 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "addx2<chunkx> %x0 = %x1, %x2\n\taddx2<chunkx> %y0 = %y1, %y2\n\t"
           "addx2<chunkx> %z0 = %z1, %z2\n\taddx2<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*addx4<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (plus:V256K (ashift:V256K (match_operand:V256K 1 "register_operand" "r")
                                  (const_int 2))
                    (match_operand:V256K 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                    (const_int 2))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                    (const_int 2))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*addx4<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (plus:V256K (ashift:V256K (match_operand:V256K 1 "register_operand" "r")
                                  (const_int 2))
                    (match_operand:V256K 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "addx4<chunkx> %x0 = %x1, %x2\n\taddx4<chunkx> %y0 = %y1, %y2\n\t"
           "addx4<chunkx> %z0 = %z1, %z2\n\taddx4<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*addx8<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (plus:V256K (ashift:V256K (match_operand:V256K 1 "register_operand" "r")
                                  (const_int 3))
                    (match_operand:V256K 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                    (const_int 3))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                    (const_int 3))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*addx8<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (plus:V256K (ashift:V256K (match_operand:V256K 1 "register_operand" "r")
                                  (const_int 3))
                    (match_operand:V256K 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "addx8<chunkx> %x0 = %x1, %x2\n\taddx8<chunkx> %y0 = %y1, %y2\n\t"
           "addx8<chunkx> %z0 = %z1, %z2\n\taddx8<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*addx16<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (plus:V256K (ashift:V256K (match_operand:V256K 1 "register_operand" "r")
                                  (const_int 4))
                    (match_operand:V256K 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                    (const_int 4))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (plus:<HALF> (ashift:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                    (const_int 4))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*addx16<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (plus:V256K (ashift:V256K (match_operand:V256K 1 "register_operand" "r")
                                  (const_int 4))
                    (match_operand:V256K 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "addx16<chunkx> %x0 = %x1, %x2\n\taddx16<chunkx> %y0 = %y1, %y2\n\t"
           "addx16<chunkx> %z0 = %z1, %z2\n\taddx16<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "sub<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (minus:V256J (match_operand:V256J 1 "register_operand" "r")
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
        (minus:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
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
                     (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "sbf<chunkx> %x0 = %2, %x1\n\tsbf<chunkx> %y0 = %2, %y1\n\t"
           "sbf<chunkx> %z0 = %2, %z1\n\tsbf<chunkx> %t0 = %2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "sssub<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "")
        (ss_minus:V256J (match_operand:V256J 1 "register_operand" "")
                        (match_operand:V256J 2 "register_operand" "")))]
  ""
  ""
)

(define_insn_and_split "sssub<mode>3_1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_minus:V256J (match_operand:V256J 1 "register_operand" "r")
                        (match_operand:V256J 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                         (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                         (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "sssub<mode>3_2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_minus:V256J (match_operand:V256J 1 "register_operand" "r")
                        (match_operand:V256J 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "sbfs<chunkx> %x0 = %x2, %x1\n\tsbfs<chunkx> %y0 = %y2, %y1\n\t"
           "sbfs<chunkx> %z0 = %z2, %z1\n\tsbfs<chunkx> %t0 = %t2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*sssub<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=&r")
        (ss_minus:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                        (match_operand:V256J 2 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_minus:<HALF> (vec_duplicate:<HALF> (match_dup 1))
                         (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_minus:<HALF> (vec_duplicate:<HALF> (match_dup 1))
                         (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*sssub<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (minus:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                     (match_operand:V256J 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "sbfs<chunkx> %x0 = %x2, %1\n\tsbfs<chunkx> %y0 = %y2, %1\n\t"
           "sbfs<chunkx> %z0 = %z2, %1\n\tsbfs<chunkx> %t0 = %t2, %1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*sssub<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=&r")
        (ss_minus:V256J (match_operand:V256J 1 "register_operand" "r")
                        (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                         (vec_duplicate:<HALF> (match_dup 2))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                         (vec_duplicate:<HALF> (match_dup 2))))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*sssub<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (minus:V256J (match_operand:V256J 1 "register_operand" "r")
                     (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  "(KV3_2||KV4)"
  {
    return "sbfs<chunkx> %x0 = %2, %x1\n\tsbfs<chunkx> %y0 = %2, %y1\n\t"
           "sbfs<chunkx> %z0 = %2, %z1\n\tsbfs<chunkx> %t0 = %2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "ussub<mode>3"
  [(match_operand:V256J 0 "register_operand" "")
   (match_operand:V256J 1 "register_operand" "")
   (match_operand:V256J 2 "register_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_ussub<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_ussub<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "ussub<mode>3_1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (us_minus:V256J (match_operand:V256J 1 "register_operand" "r")
                        (match_operand:V256J 2 "register_operand" "r")))
   (clobber (match_scratch:V256J 3 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (umin:V256J (match_dup 1) (match_dup 2)))
   (set (match_dup 0)
        (minus:V256J (match_dup 1) (match_dup 3)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
  }
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "ussub<mode>3_2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (us_minus:V256J (match_operand:V256J 1 "register_operand" "r")
                        (match_operand:V256J 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "sbfus<chunkx> %x0 = %x2, %x1\n\tsbfus<chunkx> %y0 = %y2, %y1\n\t"
           "sbfus<chunkx> %z0 = %z2, %z1\n\tsbfus<chunkx> %t0 = %t2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*ussub<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=&r")
        (us_minus:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                        (match_operand:V256J 2 "register_operand" "r")))
   (clobber (match_scratch:V256J 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 3)
        (minus:V256J (vec_duplicate:V256J (match_dup 1)) (match_dup 2)))
   (set (match_dup 4)
        (leu:V256J (match_dup 3) (vec_duplicate:V256J (match_dup 1))))
   (set (match_dup 0)
        (and:V256J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*ussub<mode>3_s1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (us_minus:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                        (match_operand:V256J 2 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "sbfus<chunkx> %x0 = %x2, %1\n\tsbfus<chunkx> %y0 = %y2, %1\n\t"
           "sbfus<chunkx> %z0 = %z2, %1\n\tsbfus<chunkx> %t0 = %t2, %1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*ussub<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=&r")
        (us_minus:V256J (match_operand:V256J 1 "register_operand" "r")
                        (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))
   (clobber (match_scratch:V256J 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 3)
        (minus:V256J (match_dup 1) (vec_duplicate:V256J (match_dup 2))))
   (set (match_dup 4)
        (leu:V256J (match_dup 3) (match_dup 1)))
   (set (match_dup 0)
        (and:V256J (match_dup 3) (match_dup 4)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*ussub<mode>3_s2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (us_minus:V256J (match_operand:V256J 1 "register_operand" "r")
                        (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  "(KV3_2||KV4)"
  {
    return "sbfus<chunkx> %x0 = %2, %x1\n\tsbfus<chunkx> %y0 = %2, %y1\n\t"
           "sbfus<chunkx> %z0 = %2, %z1\n\tsbfus<chunkx> %t0 = %2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*sbfx2<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (minus:V256K (match_operand:V256K 1 "register_operand" "r")
                     (ashift:V256K (match_operand:V256K 2 "register_operand" "r")
                                   (const_int 1))))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 0)
                                     (const_int 1))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 16)
                                     (const_int 1))))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*sbfx2<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (minus:V256K (match_operand:V256K 1 "register_operand" "r")
                     (ashift:V256K (match_operand:V256K 2 "register_operand" "r")
                                   (const_int 1))))]
  "(KV3_2||KV4)"
  {
    return "sbfx2<chunkx> %x0 = %x2, %x1\n\tsbfx2<chunkx> %y0 = %y2, %y1\n\t"
           "sbfx2<chunkx> %z0 = %z2, %z1\n\tsbfx2<chunkx> %t0 = %t2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*sbfx4<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (minus:V256K (match_operand:V256K 1 "register_operand" "r")
                     (ashift:V256K (match_operand:V256K 2 "register_operand" "r")
                                   (const_int 2))))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 0)
                                     (const_int 2))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 16)
                                     (const_int 2))))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*sbfx4<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (minus:V256K (match_operand:V256K 1 "register_operand" "r")
                     (ashift:V256K (match_operand:V256K 2 "register_operand" "r")
                                   (const_int 2))))]
  "(KV3_2||KV4)"
  {
    return "sbfx4<chunkx> %x0 = %x2, %x1\n\tsbfx4<chunkx> %y0 = %y2, %y1\n\t"
           "sbfx4<chunkx> %z0 = %z2, %z1\n\tsbfx4<chunkx> %t0 = %t2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*sbfx8<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (minus:V256K (match_operand:V256K 1 "register_operand" "r")
                     (ashift:V256K (match_operand:V256K 2 "register_operand" "r")
                                   (const_int 3))))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 0)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 0)
                                     (const_int 3))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (subreg:<HALF> (match_dup 1) 16)
                      (ashift:<HALF> (subreg:<HALF> (match_dup 2) 16)
                                     (const_int 3))))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*sbfx8<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (minus:V256K (match_operand:V256K 1 "register_operand" "r")
                     (ashift:V256K (match_operand:V256K 2 "register_operand" "r")
                                   (const_int 3))))]
  "(KV3_2||KV4)"
  {
    return "sbfx8<chunkx> %x0 = %x2, %x1\n\tsbfx8<chunkx> %y0 = %y2, %y1\n\t"
           "sbfx8<chunkx> %z0 = %z2, %z1\n\tsbfx8<chunkx> %t0 = %t2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*sbfx16<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (minus:V256K (match_operand:V256K 1 "register_operand" "r")
                     (ashift:V256K (match_operand:V256K 2 "register_operand" "r")
                                   (const_int 4))))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
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

(define_insn "*sbfx16<suffix>"
  [(set (match_operand:V256K 0 "register_operand" "=r")
        (minus:V256K (match_operand:V256K 1 "register_operand" "r")
                     (ashift:V256K (match_operand:V256K 2 "register_operand" "r")
                                   (const_int 4))))]
  "(KV3_2||KV4)"
  {
    return "sbfx16<chunkx> %x0 = %x2, %x1\n\tsbfx16<chunkx> %y0 = %y2, %y1\n\t"
           "sbfx16<chunkx> %z0 = %z2, %z1\n\tsbfx16<chunkx> %t0 = %t2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "div<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "")
        (div:V256J (match_operand:V256J 1 "register_operand" "")
                   (match_operand:V256J 2 "register_operand" "")))]
  ""
  {
    rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__div<mode>3"),
                                        operands[0], LCT_CONST, <MODE>mode,
                                        operands[1], <MODE>mode, operands[2], <MODE>mode);
    if (dest != operands[0])
      emit_move_insn (operands[0], dest);
    DONE;
  }
)

(define_expand "mod<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "")
        (mod:V256J (match_operand:V256J 1 "register_operand" "")
                   (match_operand:V256J 2 "register_operand" "")))]
  ""
  {
    rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__mod<mode>3"),
                                        operands[0], LCT_CONST, <MODE>mode,
                                        operands[1], <MODE>mode, operands[2], <MODE>mode);
    if (dest != operands[0])
      emit_move_insn (operands[0], dest);
    DONE;
  }
)

(define_expand "udiv<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "")
        (udiv:V256J (match_operand:V256J 1 "register_operand" "")
                    (match_operand:V256J 2 "register_operand" "")))]
  ""
  {
    rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__udiv<mode>3"),
                                        operands[0], LCT_CONST, <MODE>mode,
                                        operands[1], <MODE>mode, operands[2], <MODE>mode);
    if (dest != operands[0])
      emit_move_insn (operands[0], dest);
    DONE;
  }
)

(define_expand "umod<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "")
        (umod:V256J (match_operand:V256J 1 "register_operand" "")
                    (match_operand:V256J 2 "register_operand" "")))]
  ""
  {
    rtx dest = emit_library_call_value (gen_rtx_SYMBOL_REF (Pmode, "__umod<mode>3"),
                                        operands[0], LCT_CONST, <MODE>mode,
                                        operands[1], <MODE>mode, operands[2], <MODE>mode);
    if (dest != operands[0])
      emit_move_insn (operands[0], dest);
    DONE;
  }
)

(define_insn "smin<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (smin:V256J (match_operand:V256J 1 "register_operand" "r")
                    (match_operand:V256J 2 "register_operand" "r")))]
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
        (smin:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
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
                    (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
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
                    (match_operand:V256J 2 "register_operand" "r")))]
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
        (smax:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
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
                    (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
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
                    (match_operand:V256J 2 "register_operand" "r")))]
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
        (umin:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
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
                    (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
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
                    (match_operand:V256J 2 "register_operand" "r")))]
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
        (umax:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
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
                    (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "maxu<chunkx> %x0 = %x1, %2\n\tmaxu<chunkx> %y0 = %y1, %2\n\t"
           "maxu<chunkx> %z0 = %z1, %2\n\tmaxu<chunkx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; V256L

(define_insn "and<mode>3"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (and:V256L (match_operand:V256L 1 "register_operand" "r")
                   (match_operand:V256L 2 "register_operand" "r")))]
  ""
  {
    return "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2\n\t"
           "andd %z0 = %z1, %z2\n\tandd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "_nand<mode>3"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (ior:V256L (not:V256L (match_operand:V256L 1 "register_operand" "r"))
                   (not:V256L (match_operand:V256L 2 "register_operand" "r"))))]
  ""
  {
    return "nandd %x0 = %x1, %x2\n\tnandd %y0 = %y1, %y2\n\t"
           "nandd %z0 = %z1, %z2\n\tnandd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "_andn<mode>3"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (and:V256L (not:V256L (match_operand:V256L 1 "register_operand" "r"))
                   (match_operand:V256L 2 "register_operand" "r")))]
  ""
  {
    return "andnd %x0 = %x1, %x2\n\tandnd %y0 = %y1, %y2\n\t"
           "andnd %z0 = %z1, %z2\n\tandnd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "ior<mode>3"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (ior:V256L (match_operand:V256L 1 "register_operand" "r")
                   (match_operand:V256L 2 "register_operand" "r")))]
  ""
  {
    return "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2\n\t"
           "ord %z0 = %z1, %z2\n\tord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "_nior<mode>3"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (and:V256L (not:V256L (match_operand:V256L 1 "register_operand" "r"))
                   (not:V256L (match_operand:V256L 2 "register_operand" "r"))))]
  ""
  {
    return "nord %x0 = %x1, %x2\n\tnord %y0 = %y1, %y2\n\t"
           "nord %z0 = %z1, %z2\n\tnord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "_iorn<mode>3"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (ior:V256L (not:V256L (match_operand:V256L 1 "register_operand" "r"))
                   (match_operand:V256L 2 "register_operand" "r")))]
  ""
  {
    return "ornd %x0 = %x1, %x2\n\tornd %y0 = %y1, %y2\n\t"
           "ornd %z0 = %z1, %z2\n\tornd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "xor<mode>3"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (xor:V256L (match_operand:V256L 1 "register_operand" "r")
                   (match_operand:V256L 2 "register_operand" "r")))]
  ""
  {
    return "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2\n\t"
           "xord %z0 = %z1, %z2\n\txord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "_nxor<mode>3"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (not:V256L (xor:V256L (match_operand:V256L 1 "register_operand" "r")
                              (match_operand:V256L 2 "register_operand" "r"))))]
  ""
  {
    return "nxord %x0 = %x1, %x2\n\tnxord %y0 = %y1, %y2\n\t"
           "nxord %z0 = %z1, %z2\n\tnxord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; V256M

(define_insn_and_split "madd<mode><mode>4"
  [(set (match_operand:V256M 0 "register_operand" "=r")
        (plus:V256M (mult:V256M (match_operand:V256M 1 "register_operand" "r")
                                (match_operand:V256M 2 "register_operand" "r"))
                    (match_operand:V256M 3 "register_operand" "0")))]
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
  [(set_attr "type" "madd_int")]
)

(define_insn_and_split "msub<mode><mode>4"
  [(set (match_operand:V256M 0 "register_operand" "=r")
        (minus:V256M (match_operand:V256M 3 "register_operand" "0")
                     (mult:V256M (match_operand:V256M 1 "register_operand" "r")
                                 (match_operand:V256M 2 "register_operand" "r"))))]
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
  [(set_attr "type" "madd_int")
   (set_attr "length"      "8")]
)


;; V256J (V16HI V8SI V4DI)

(define_insn_and_split "rotl<mode>3"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (rotate:V256J (match_operand:V256J 1 "register_operand" "r")
                      (match_operand:SI 2 "register_operand" "r")))
   (clobber (match_scratch:SI 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))
   (clobber (match_scratch:V256J 5 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3) (neg:SI (match_dup 2)))
   (set (match_dup 4) (ashift:V256J (match_dup 1) (match_dup 2)))
   (set (match_dup 5) (lshiftrt:V256J (match_dup 1) (match_dup 3)))
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
   (clobber (match_scratch:V256J 5 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 3) (neg:SI (match_dup 2)))
   (set (match_dup 4) (lshiftrt:V256J (match_dup 1) (match_dup 2)))
   (set (match_dup 5) (ashift:V256J (match_dup 1) (match_dup 3)))
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

(define_expand "ssneg<mode>2"
  [(set (match_operand:V256J 0 "register_operand" "")
        (ss_neg:V256J (match_operand:V256J 1 "register_operand" "")))]
  ""
  ""
)

(define_insn_and_split "ssneg<mode>2_1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_neg:V256J (match_operand:V256J 1 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (ss_neg:<HALF> (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (ss_neg:<HALF> (subreg:<HALF> (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2_x")]
)

(define_insn "ssneg<mode>2_2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_neg:V256J (match_operand:V256J 1 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "negs<chunkx> %x0 = %x1\n\tnegs<chunkx> %y0 = %y1\n\t"
           "negs<chunkx> %z0 = %z1\n\tnegs<chunkx> %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4_x")
   (set_attr "length"          "32")]
)

(define_expand "abs<mode>2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (abs:V256J (match_operand:V256J 1 "register_operand" "r")))]
  ""
  ""
)

(define_insn_and_split "abs<mode>2_1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (abs:V256J (match_operand:V256J 1 "register_operand" "r")))]
  "KV3_1"
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (abs:<HALF> (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (abs:<HALF> (subreg:<HALF> (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2_x")]
)

(define_insn "abs<mode>2_2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (abs:V256J (match_operand:V256J 1 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "abs<chunkx> %x0 = %x1\n\tabs<chunkx> %y0 = %y1\n\t"
           "abs<chunkx> %z0 = %z1\n\tabs<chunkx> %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4_x")
   (set_attr "length"          "32")]
)

(define_expand "ssabs<mode>2"
  [(set (match_operand:V256J 0 "register_operand" "")
        (ss_abs:V256J (match_operand:V256J 1 "register_operand" "")))]
  ""
  ""
)

(define_insn_and_split "ssabs<mode>2_1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_abs:V256J (match_operand:V256J 1 "register_operand" "r")))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 0)
        (ss_neg:V256J (match_dup 1)))
   (set (match_dup 0)
        (abs:V256J (match_dup 0)))]
  ""
)

(define_insn "ssabs<mode>2_2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_abs:V256J (match_operand:V256J 1 "register_operand" "r")))]
  "(KV3_2||KV4)"
  {
    return "abss<chunkx> %x0 = %x1\n\tabss<chunkx> %y0 = %y1\n\t"
           "abss<chunkx> %z0 = %z1\n\tabss<chunkx> %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4_x")
   (set_attr "length"          "32")]
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

(define_insn "one_cmpl<mode>2"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (not:V256L (match_operand:V256L 1 "register_operand" "r")))]
  ""
  {
    return "notd %x0 = %x1\n\tnotd %y0 = %y1\n\t"
           "notd %z0 = %z1\n\tnotd %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "abd<mode>3"
  [(match_operand:V256L 0 "register_operand" "")
   (match_operand:V256L 1 "register_operand" "")
   (match_operand:V256L 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_abd<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_abd<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "abd<mode>3_1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (minus:V256J (smax:V256J (match_operand:V256J 1 "register_operand" "r")
                                 (match_operand:V256J 2 "register_operand" "r"))
                     (smin:V256J (match_dup 1) (match_dup 2))))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (smax:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                   (subreg:<HALF> (match_dup 2) 0))
                      (smin:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                   (subreg:<HALF> (match_dup 2) 0))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (smax:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                   (subreg:<HALF> (match_dup 2) 16))
                      (smin:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                   (subreg:<HALF> (match_dup 2) 16))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "abd<mode>3_2"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (minus:V256L (smax:V256L (match_operand:V256L 1 "register_operand" "r")
                                 (match_operand:V256L 2 "register_operand" "r"))
                     (smin:V256L (match_dup 1) (match_dup 2))))]
  "(KV3_2||KV4)"
  {
    return "abd<chunkx> %x0 = %x1, %x2\n\tabd<chunkx> %y0 = %y1, %y2\n\t"
           "abd<chunkx> %z0 = %z1, %z2\n\tabd<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*abd<mode>_s1"
  [(set (match_operand:V256L 0 "register_operand" "=&r")
        (minus:V256L (smax:V256L (vec_duplicate:V256L (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                                 (match_operand:V256L 2 "register_operand" "r"))
                     (smin:V256L (vec_duplicate:V256L (match_dup 1)) (match_dup 2))))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (smax:<HALF> (vec_duplicate:<HALF> (match_dup 1))
                                   (subreg:<HALF> (match_dup 2) 0))
                      (smin:<HALF> (vec_duplicate:<HALF> (match_dup 1))
                                   (subreg:<HALF> (match_dup 2) 0))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (smax:<HALF> (vec_duplicate:<HALF> (match_dup 1))
                                   (subreg:<HALF> (match_dup 2) 16))
                      (smin:<HALF> (vec_duplicate:<HALF> (match_dup 1))
                                   (subreg:<HALF> (match_dup 2) 16))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*abd<mode>_s1"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (minus:V256L (smax:V256L (vec_duplicate:V256L (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                                 (match_operand:V256L 2 "register_operand" "r"))
                     (smin:V256L (vec_duplicate:V256L (match_dup 1)) (match_dup 2))))]
  "(KV3_2||KV4)"
  {
    return "abd<chunkx> %x0 = %1, %x2\n\tabd<chunkx> %y0 = %1, %y2\n\t"
           "abd<chunkx> %z0 = %1, %z2\n\tabd<chunkx> %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*abd<mode>_s2"
  [(set (match_operand:V256L 0 "register_operand" "=&r")
        (minus:V256L (smax:V256L (match_operand:V256L 1 "register_operand" "r")
                                 (vec_duplicate:V256L (match_operand:<CHUNK> 2 "nonmemory_operand" "r")))
                     (smin:V256L (match_dup 1) (vec_duplicate:V256L (match_dup 2)))))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (minus:<HALF> (smax:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                   (vec_duplicate:<HALF> (match_dup 2)))
                      (smin:<HALF> (subreg:<HALF> (match_dup 1) 0)
                                   (vec_duplicate:<HALF> (match_dup 2)))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (minus:<HALF> (smax:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                   (vec_duplicate:<HALF> (match_dup 2)))
                      (smin:<HALF> (subreg:<HALF> (match_dup 1) 16)
                                   (vec_duplicate:<HALF> (match_dup 2)))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*abd<mode>_s2"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (minus:V256L (smax:V256L (match_operand:V256L 1 "register_operand" "r")
                                 (vec_duplicate:V256L (match_operand:<CHUNK> 2 "nonmemory_operand" "r")))
                     (smin:V256L (match_dup 1) (vec_duplicate:V256L (match_dup 2)))))]
  "(KV3_2||KV4)"
  {
    return "abd<chunkx> %x0 = %x1, %2\n\tabd<chunkx> %y0 = %y1, %2\n\t"
           "abd<chunkx> %z0 = %z1, %2\n\tabd<chunkx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "abds<mode>3"
  [(match_operand:V256L 0 "register_operand" "")
   (match_operand:V256L 1 "register_operand" "")
   (match_operand:V256L 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_abds<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_abds<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "abds<mode>3_1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (ss_minus:V256J (smax:V256J (match_operand:V256J 1 "register_operand" "r")
                                    (match_operand:V256J 2 "register_operand" "r"))
                        (smin:V256J (match_dup 1) (match_dup 2))))
   (clobber (match_scratch:V256J 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (smax:V256J (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (smin:V256J (match_dup 1) (match_dup 2)))
   (set (match_dup 0)
        (ss_minus:V256J (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "abds<mode>3_2"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (ss_minus:V256L (smax:V256L (match_operand:V256L 1 "register_operand" "r")
                                    (match_operand:V256L 2 "register_operand" "r"))
                        (smin:V256L (match_dup 1) (match_dup 2))))]
  "(KV3_2||KV4)"
  {
    return "abds<chunkx> %x0 = %x1, %x2\n\tabds<chunkx> %y0 = %y1, %y2\n\t"
           "abds<chunkx> %z0 = %z1, %z2\n\tabds<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*abds<mode>_s1"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (ss_minus:V256L (smax:V256L (vec_duplicate:V256L (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                                    (match_operand:V256L 2 "register_operand" "r"))
                        (smin:V256L (vec_duplicate:V256L (match_dup 1)) (match_dup 2))))]
  "(KV3_2||KV4)"
  {
    return "abds<chunkx> %x0 = %1, %x2\n\tabds<chunkx> %y0 = %1, %y2\n\t"
           "abds<chunkx> %z0 = %1, %z2\n\tabds<chunkx> %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*abds<mode>_s2"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (ss_minus:V256L (smax:V256L (match_operand:V256L 1 "register_operand" "r")
                                    (vec_duplicate:V256L (match_operand:<CHUNK> 2 "nonmemory_operand" "r")))
                        (smin:V256L (match_dup 1) (vec_duplicate:V256L (match_dup 2)))))]
  "(KV3_2||KV4)"
  {
    return "abds<chunkx> %x0 = %x1, %2\n\tabds<chunkx> %y0 = %y1, %2\n\t"
           "abds<chunkx> %z0 = %z1, %2\n\tabds<chunkx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_expand "abdu<mode>3"
  [(match_operand:V256L 0 "register_operand" "")
   (match_operand:V256L 1 "register_operand" "")
   (match_operand:V256L 2 "register_s32_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_abdu<mode>3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_abdu<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "abdu<mode>3_1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (minus:V256J (umax:V256J (match_operand:V256J 1 "register_operand" "r")
                                 (match_operand:V256J 2 "register_operand" "r"))
                     (umin:V256J (match_dup 1) (match_dup 2))))
   (clobber (match_scratch:V256J 3 "=&r"))
   (clobber (match_scratch:V256J 4 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (umax:V256J (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (umin:V256J (match_dup 1) (match_dup 2)))
   (set (match_dup 0)
        (minus:V256J (match_dup 3) (match_dup 4)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (<MODE>mode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (<MODE>mode);
  }
)

(define_insn "abdu<mode>3_2"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (minus:V256L (umax:V256L (match_operand:V256L 1 "register_operand" "r")
                                 (match_operand:V256L 2 "register_operand" "r"))
                     (umin:V256L (match_dup 1) (match_dup 2))))]
  "(KV3_2||KV4)"
  {
    return "abdu<chunkx> %x0 = %x1, %x2\n\tabdu<chunkx> %y0 = %y1, %y2\n\t"
           "abdu<chunkx> %z0 = %z1, %z2\n\tabdu<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*abdu<mode>_s1"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (minus:V256L (umax:V256L (vec_duplicate:V256L (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                                 (match_operand:V256L 2 "register_operand" "r"))
                     (umin:V256L (vec_duplicate:V256L (match_dup 1)) (match_dup 2))))]
  "(KV3_2||KV4)"
  {
    return "abdu<chunkx> %x0 = %1, %x2\n\tabdu<chunkx> %y0 = %1, %y2\n\t"
           "abdu<chunkx> %z0 = %1, %z2\n\tabdu<chunkx> %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*abdu<mode>_s2"
  [(set (match_operand:V256L 0 "register_operand" "=r")
        (minus:V256L (umax:V256L (match_operand:V256L 1 "register_operand" "r")
                                 (vec_duplicate:V256L (match_operand:<CHUNK> 2 "nonmemory_operand" "r")))
                     (umin:V256L (match_dup 1) (vec_duplicate:V256L (match_dup 2)))))]
  "(KV3_2||KV4)"
  {
    return "abdu<chunkx> %x0 = %x1, %2\n\tabdu<chunkx> %y0 = %y1, %2\n\t"
           "abdu<chunkx> %z0 = %z1, %2\n\tabdu<chunkx> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; V8SI

(define_insn_and_split "mulv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (mult:V8SI (match_operand:V8SI 1 "register_operand" "r")
                    (match_operand:V8SI 2 "register_operand" "r")))]
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
  [(set_attr "type" "madd_int")]
)

(define_insn "maddv8siv8si4_2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (plus:V8SI (mult:V8SI (match_operand:V8SI 1 "register_operand" "r")
                              (match_operand:V8SI 2 "register_operand" "r"))
                   (match_operand:V8SI 3 "register_operand" "0")))]
  "(KV3_2||KV4)"
  "#"
  [(set_attr "type" "madd_int")]
)

(define_split
  [(set (match_operand:V8SI 0 "register_operand" "")
        (plus:V8SI (mult:V8SI (match_operand:V8SI 1 "register_operand" "")
                              (match_operand:V8SI 2 "register_operand" ""))
                   (match_operand:V8SI 3 "register_operand" "")))]
  "KV3_2 && reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (plus:V4SI (mult:V4SI (subreg:V4SI (match_dup 1) 0)
                              (subreg:V4SI (match_dup 2) 0))
                   (subreg:V4SI (match_dup 3) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (plus:V4SI (mult:V4SI (subreg:V4SI (match_dup 1) 16)
                              (subreg:V4SI (match_dup 2) 16))
                   (subreg:V4SI (match_dup 3) 16)))]
  ""
)

(define_split
  [(set (match_operand:V8SI 0 "register_operand" "")
        (plus:V8SI (mult:V8SI (match_operand:V8SI 1 "register_operand" "")
                              (match_operand:V8SI 2 "register_operand" ""))
                   (match_operand:V8SI 3 "register_operand" "")))]
  "KV4 && reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (plus:V2SI (mult:V2SI (subreg:V2SI (match_dup 1) 0)
                              (subreg:V2SI (match_dup 2) 0))
                   (subreg:V2SI (match_dup 3) 0)))
   (set (subreg:V2SI (match_dup 0) 8)
        (plus:V2SI (mult:V2SI (subreg:V2SI (match_dup 1) 8)
                              (subreg:V2SI (match_dup 2) 8))
                   (subreg:V2SI (match_dup 3) 8)))
   (set (subreg:V2SI (match_dup 0) 16)
        (plus:V2SI (mult:V2SI (subreg:V2SI (match_dup 1) 16)
                              (subreg:V2SI (match_dup 2) 16))
                   (subreg:V2SI (match_dup 3) 16)))
   (set (subreg:V2SI (match_dup 0) 24)
        (plus:V2SI (mult:V2SI (subreg:V2SI (match_dup 1) 24)
                              (subreg:V2SI (match_dup 2) 24))
                   (subreg:V2SI (match_dup 3) 24)))]
  ""
)

(define_insn "msubv8siv8si4_2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (minus:V8SI (match_operand:V8SI 3 "register_operand" "0")
                    (mult:V8SI (match_operand:V8SI 1 "register_operand" "r")
                               (match_operand:V8SI 2 "register_operand" "r"))))]
  "(KV3_2||KV4)"
  "#"
  [(set_attr "type" "madd_int")]
)

(define_split
  [(set (match_operand:V8SI 0 "register_operand" "")
        (minus:V8SI (match_operand:V8SI 3 "register_operand" "")
                    (mult:V8SI (match_operand:V8SI 1 "register_operand" "")
                               (match_operand:V8SI 2 "register_operand" ""))))]
  "KV3_2 && reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (minus:V4SI (subreg:V4SI (match_dup 3) 0)
                    (mult:V4SI (subreg:V4SI (match_dup 1) 0)
                               (subreg:V4SI (match_dup 2) 0))))
   (set (subreg:V4SI (match_dup 0) 16)
        (minus:V4SI (subreg:V4SI (match_dup 3) 16)
                    (mult:V4SI (subreg:V4SI (match_dup 1) 16)
                               (subreg:V4SI (match_dup 2) 16))))]
  ""
)

(define_split
  [(set (match_operand:V8SI 0 "register_operand" "")
        (minus:V8SI (match_operand:V8SI 3 "register_operand" "")
                    (mult:V8SI (match_operand:V8SI 1 "register_operand" "")
                               (match_operand:V8SI 2 "register_operand" ""))))]
  "KV4 && reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (minus:V2SI (subreg:V2SI (match_dup 3) 0)
                    (mult:V2SI (subreg:V2SI (match_dup 1) 0)
                               (subreg:V2SI (match_dup 2) 0))))
   (set (subreg:V2SI (match_dup 0) 8)
        (minus:V2SI (subreg:V2SI (match_dup 3) 8)
                    (mult:V2SI (subreg:V2SI (match_dup 1) 8)
                               (subreg:V2SI (match_dup 2) 8))))
   (set (subreg:V2SI (match_dup 0) 16)
        (minus:V2SI (subreg:V2SI (match_dup 3) 16)
                    (mult:V2SI (subreg:V2SI (match_dup 1) 16)
                               (subreg:V2SI (match_dup 2) 16))))
   (set (subreg:V2SI (match_dup 0) 24)
        (minus:V2SI (subreg:V2SI (match_dup 3) 24)
                    (mult:V2SI (subreg:V2SI (match_dup 1) 24)
                               (subreg:V2SI (match_dup 2) 24))))]
  ""
)


;; V4DI

(define_insn "ashlv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (ashift:V4DI (match_operand:V4DI 1 "register_operand" "r")
                     (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  {
    return "slld %x0 = %x1, %2\n\tslld %y0 = %y1, %2\n\t"
           "slld %z0 = %z1, %2\n\tslld %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length" "16")]
)

(define_insn_and_split "ssashlv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=&r,r")
        (ss_ashift:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "reg_shift_operand" "r,U06")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (ss_ashift:V2DI (subreg:V2DI (match_dup 1) 0)
                        (match_dup 2)))
   (set (subreg:V2DI (match_dup 0) 16)
        (ss_ashift:V2DI (subreg:V2DI (match_dup 1) 16)
                        (match_dup 2)))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_expand "usashlv4di3"
  [(match_operand:V4DI 0 "register_operand" "")
   (match_operand:V4DI 1 "register_operand" "")
   (match_operand:SI 2 "reg_shift_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usashlv4di3_1 (operands[0], operands[1], operands[2]));
    else if ((KV3_2||KV4))
      emit_insn (gen_usashlv4di3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usashlv4di3_1"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (us_ashift:V4DI (match_operand:V4DI 1 "register_operand" "r")
                        (match_operand:SI 2 "reg_shift_operand" "rU06")))
   (clobber (match_scratch:V4DI 3 "=&r"))
   (clobber (match_scratch:V4DI 4 "=&r"))
   (clobber (match_scratch:V4DI 5 "=&r"))]
  "KV3_1"
  "#"
  "KV3_1"
  [(set (match_dup 3)
        (ashift:V4DI (match_dup 1) (match_dup 2)))
   (set (match_dup 4)
        (lshiftrt:V4DI (match_dup 3) (match_dup 2)))
   (set (match_dup 5)
        (ne:V4DI (match_dup 4) (match_dup 1)))
   (set (match_dup 0)
        (ior:V4DI (match_dup 3) (match_dup 5)))]
  {
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (V4DImode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (V4DImode);
    if (GET_CODE (operands[5]) == SCRATCH)
      operands[5] = gen_reg_rtx (V4DImode);
  }
)

(define_insn_and_split "usashlv4di3_2"
  [(set (match_operand:V4DI 0 "register_operand" "=&r,r")
        (us_ashift:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "reg_shift_operand" "r,U06")))]
  "(KV3_2||KV4)"
  "#"
  "(KV3_2||KV4) && reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (us_ashift:V2DI (subreg:V2DI (match_dup 1) 0)
                        (match_dup 2)))
   (set (subreg:V2DI (match_dup 0) 16)
        (us_ashift:V2DI (subreg:V2DI (match_dup 1) 16)
                        (match_dup 2)))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn "ashrv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (ashiftrt:V4DI (match_operand:V4DI 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  {
    return "srad %x0 = %x1, %2\n\tsrad %y0 = %y1, %2\n\t"
           "srad %z0 = %z1, %2\n\tsrad %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length" "16")]
)

(define_insn "lshrv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (lshiftrt:V4DI (match_operand:V4DI 1 "register_operand" "r")
                       (match_operand:SI 2 "reg_shift_operand" "rU06")))]
  ""
  {
    return "srld %x0 = %x1, %2\n\tsrld %y0 = %y1, %2\n\t"
           "srld %z0 = %z1, %2\n\tsrld %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length" "16")]
)

(define_insn_and_split "sshrv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=&r,r")
        (unspec:V4DI [(match_operand:V4DI 1 "register_operand" "r,r")
                      (match_operand:SI 2 "reg_shift_operand" "r,U06")] UNSPEC_SRS))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 0)
                      (match_dup 2)] UNSPEC_SRS))
   (set (subreg:V2DI (match_dup 0) 16)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 16)
                      (match_dup 2)] UNSPEC_SRS))]
  ""
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)


;; S64F (V4HF V2SF)

(define_insn "add<mode>3"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (plus:S64F (match_operand:S64F 1 "register_operand" "r")
                   (match_operand:S64F 2 "register_operand" "r")))]
  ""
  "fadd<suffix> %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mult_fp3") (const_string "mult_fp4")))]
)

(define_insn "sub<mode>3"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (minus:S64F (match_operand:S64F 1 "register_operand" "r")
                    (match_operand:S64F 2 "register_operand" "r")))]
  ""
  "fsbf<suffix> %0 = %2, %1"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mult_fp3") (const_string "mult_fp4")))]
)

(define_insn "mul<mode>3"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (mult:S64F (match_operand:S64F 1 "register_operand" "r")
                   (match_operand:S64F 2 "register_operand" "r")))]
  ""
  "fmul<suffix> %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mult_fp3") (const_string "mult_fp4")))]
)

(define_insn "fma<mode>4"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (fma:S64F (match_operand:S64F 1 "register_operand" "r")
                  (match_operand:S64F 2 "register_operand" "r")
                  (match_operand:S64F 3 "register_operand" "0")))]
  ""
  "ffma<suffix> %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "madd_fp3") (const_string "madd_fp4")))]
)

(define_insn "fnma<mode>4"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (fma:S64F (neg:S64F (match_operand:S64F 1 "register_operand" "r"))
                  (match_operand:S64F 2 "register_operand" "r")
                  (match_operand:S64F 3 "register_operand" "0")))]
  ""
  "ffms<suffix> %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "madd_fp3") (const_string "madd_fp4")))]
)

(define_insn "fmin<mode>3"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (smin:S64F (match_operand:S64F 1 "register_operand" "r")
                   (match_operand:S64F 2 "register_operand" "r")))]
  ""
  "fmin<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "fmax<mode>3"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (smax:S64F (match_operand:S64F 1 "register_operand" "r")
                   (match_operand:S64F 2 "register_operand" "r")))]
  ""
  "fmax<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "neg<mode>2"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (neg:S64F (match_operand:S64F 1 "register_operand" "r")))]
  ""
  "fneg<suffix> %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "abs<mode>2"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (abs:S64F (match_operand:S64F 1 "register_operand" "r")))]
  ""
  "fabs<suffix> %0 = %1"
  [(set_attr "type" "alu_thin")]
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

(define_expand "xorsign<mode>3"
  [(match_operand:S64F 0 "register_operand")
   (match_operand:S64F 1 "register_operand")
   (match_operand:S64F 2 "register_operand")]
  ""
  {
    rtx maskv4hf = GEN_INT (0x8000800080008000);
    rtx maskv2sf = GEN_INT (0x8000000080000000);
    rtx sign2 = gen_reg_rtx (<MODE>mode);
    emit_insn (gen_rtx_SET (sign2, gen_rtx_UNSPEC (<MODE>mode, gen_rtvec (2, operands[2], mask<mode>), UNSPEC_ANDD)));
    emit_insn (gen_rtx_SET (operands[0], gen_rtx_UNSPEC (<MODE>mode, gen_rtvec (2, operands[1], sign2), UNSPEC_XORD)));
    DONE;
  }
)


;; V4HF

(define_insn "truncv4sfv4hf2"
  [(set (match_operand:V4HF 0 "register_operand" "=r")
        (float_truncate:V4HF (match_operand:V4SF 1 "register_operand" "r")))]
  ""
  "fnarrowwhq %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "extendv4hfv4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (float_extend:V4SF (match_operand:V4HF 1 "register_operand" "r")))]
  ""
  "fwidenlhwp %x0 = %1\n\tfwidenmhwp %y0 = %1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "floatv4siv4hf2"
  [(set (match_operand:V4HF 0 "register_operand" "=r")
        (float:V4HF (match_operand:V4SI 1 "register_operand" "r")))
   (clobber (match_scratch:V4SF 2 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float:V4SF (match_dup 1)))
   (set (match_dup 0)
        (float_truncate:V4HF (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V4SFmode);
  }
)

(define_insn_and_split "floatunsv4siv4hf2"
  [(set (match_operand:V4HF 0 "register_operand" "=r")
        (unsigned_float:V4HF (match_operand:V4SI 1 "register_operand" "r")))
   (clobber (match_scratch:V4SF 2 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (unsigned_float:V4SF (match_dup 1)))
   (set (match_dup 0)
        (float_truncate:V4HF (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V4SFmode);
  }
)

(define_insn_and_split "fix_truncv4hfv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (fix:V4SI (match_operand:V4HF 1 "register_operand" "r")))
   (clobber (match_scratch:V4SF 2 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float_extend:V4SF (match_dup 1)))
   (set (match_dup 0)
        (fix:V4SI (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V4SFmode);
  }
)

(define_insn_and_split "fixuns_truncv4hfv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unsigned_fix:V4SI (match_operand:V4HF 1 "register_operand" "r")))
   (clobber (match_scratch:V4SF 2 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float_extend:V4SF (match_dup 1)))
   (set (match_dup 0)
        (unsigned_fix:V4SI (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V4SFmode);
  }
)


;; V2SF

(define_insn "kvx_fdot2w"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FDOT2))]
  "KV3"
  "fdot2w%3 %0 = %1, %2"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "kvx_fdot2wd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FDOT2))]
  "KV3"
  "fdot2wd%3 %0 = %1, %2"
  [(set_attr "type" "mult_fp4")]
)

(define_insn "kvx_fdot2wdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FDOT2))]
  "KV3"
  "fdot2wdp%3 %0 = %1, %2"
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_insn "kvx_fdot2wzp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FDOT2))]
  "KV3"
  "fdot2wzp%3 %0 = %1, %2"
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_insn "floatv2siv2sf2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (float:V2SF (match_operand:V2SI 1 "register_operand" "r")))]
  ""
  {
    if (KV3)
      return "floatwp.rn %0 = %1, 0";
    return "floatwp.rn %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)

(define_insn "floatunsv2siv2sf2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unsigned_float:V2SF (match_operand:V2SI 1 "register_operand" "r")))]
  ""
  {
    if (KV3)
      return "floatuwp.rn %0 = %1, 0";
    return "floatuwp.rn %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)

(define_insn "fix_truncv2sfv2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (fix:V2SI (match_operand:V2SF 1 "register_operand" "r")))]
  ""
  {
    if (KV3)
      return "fixedwp.rz %0 = %1, 0";
    return "fixedwp.rz %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)

(define_insn "fixuns_truncv2sfv2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unsigned_fix:V2SI (match_operand:V2SF 1 "register_operand" "r")))]
  ""
  {
    if (KV3)
      return "fixeduwp.rz %0 = %1, 0";
    return "fixeduwp.rz %0 = %1";
  }
  [(set_attr "type" "conv_fp4")]
)

(define_insn "truncv2dfv2sf2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (float_truncate:V2SF (match_operand:V2DF 1 "register_operand" "r")))]
  ""
  "fnarrowdwp %0 = %1"
  [(set_attr "type" "alu_full")]
)

(define_insn "extendv2sfv2df2"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (float_extend:V2DF (match_operand:V2SF 1 "register_operand" "r")))]
  ""
  "fwidenlwd %x0 = %1\n\tfwidenmwd %y0 = %1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "floatv2div2sf2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (float:V2SF (match_operand:V2DI 1 "register_operand" "r")))
   (clobber (match_scratch:V2DF 2 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float:V2DF (match_dup 1)))
   (set (match_dup 0)
        (float_truncate:V2SF (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V2DFmode);
  }
)

(define_insn_and_split "floatunsv2div2sf2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unsigned_float:V2SF (match_operand:V2DI 1 "register_operand" "r")))
   (clobber (match_scratch:V2DF 2 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (unsigned_float:V2DF (match_dup 1)))
   (set (match_dup 0)
        (float_truncate:V2SF (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V2DFmode);
  }
)

(define_insn_and_split "fix_truncv2sfv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (fix:V2DI (match_operand:V2SF 1 "register_operand" "r")))
   (clobber (match_scratch:V2DF 2 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float_extend:V2DF (match_dup 1)))
   (set (match_dup 0)
        (fix:V2DI (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V2DFmode);
  }
)

(define_insn_and_split "fixuns_truncv2sfv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unsigned_fix:V2DI (match_operand:V2SF 1 "register_operand" "r")))
   (clobber (match_scratch:V2DF 2 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float_extend:V2DF (match_dup 1)))
   (set (match_dup 0)
        (unsigned_fix:V2DI (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V2DFmode);
  }
)


;; VXHF

(define_expand "div<mode>3"
  [(set (match_operand:VXHF 0 "register_operand" "")
        (div:VXHF (match_operand:VXHF 1 "register_operand" "")
                  (match_operand:VXHF 2 "register_operand" "")))]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx temp0 = gen_reg_rtx (V4SFmode);
        rtx temp1 = gen_reg_rtx (V4SFmode);
        rtx temp2 = gen_reg_rtx (V4SFmode);
        rtx op0 = simplify_gen_subreg (V4HFmode, operands[0], <MODE>mode, offset);
        rtx op1 = simplify_gen_subreg (V4HFmode, operands[1], <MODE>mode, offset);
        rtx op2 = simplify_gen_subreg (V4HFmode, operands[2], <MODE>mode, offset);
        emit_insn (gen_extendv4hfv4sf2 (temp1, op1));
        emit_insn (gen_extendv4hfv4sf2 (temp2, op2));
        emit_insn (gen_divv4sf3 (temp0, temp1, temp2));
        emit_insn (gen_truncv4sfv4hf2 (op0, temp0));
      }
    DONE;
  }
)

(define_expand "float<mask><mode>2"
  [(set (match_operand:VXHF 0 "register_operand" "")
        (float:VXHF (match_operand:<MASK> 1 "register_operand" "")))]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx temp0 = gen_reg_rtx (V4SFmode);
        rtx temp1 = gen_reg_rtx (V4SImode);
        rtx op0 = simplify_gen_subreg (V4HFmode, operands[0], <MODE>mode, offset);
        rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MASK>mode, offset);
        emit_insn (gen_extendv4hiv4si2 (temp1, op1));
        emit_insn (gen_floatv4siv4sf2 (temp0, temp1));
        emit_insn (gen_truncv4sfv4hf2 (op0, temp0));
      }
    DONE;
  }
)

(define_expand "floatuns<mask><mode>2"
  [(set (match_operand:VXHF 0 "register_operand" "")
        (unsigned_float:VXHF (match_operand:<MASK> 1 "register_operand" "")))]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx temp0 = gen_reg_rtx (V4SFmode);
        rtx temp1 = gen_reg_rtx (V4SImode);
        rtx op0 = simplify_gen_subreg (V4HFmode, operands[0], <MODE>mode, offset);
        rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MASK>mode, offset);
        emit_insn (gen_zero_extendv4hiv4si2 (temp1, op1));
        emit_insn (gen_floatunsv4siv4sf2 (temp0, temp1));
        emit_insn (gen_truncv4sfv4hf2 (op0, temp0));
      }
    DONE;
  }
)

(define_expand "fix_trunc<mode><mask>2"
  [(set (match_operand:<MASK> 0 "register_operand" "")
        (fix:<MASK> (match_operand:VXHF 1 "register_operand" "")))]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx temp0 = gen_reg_rtx (V4SImode);
        rtx temp1 = gen_reg_rtx (V4SFmode);
        rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MASK>mode, offset);
        rtx op1 = simplify_gen_subreg (V4HFmode, operands[1], <MODE>mode, offset);
        emit_insn (gen_extendv4hfv4sf2 (temp1, op1));
        emit_insn (gen_fix_truncv4sfv4si2 (temp0, temp1));
        emit_insn (gen_truncv4siv4hi2 (op0, temp0));
      }
    DONE;
  }
)

(define_expand "fixuns_trunc<mode><mask>2"
  [(set (match_operand:<MASK> 0 "register_operand" "")
        (unsigned_fix:<MASK> (match_operand:VXHF 1 "register_operand" "")))]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx temp0 = gen_reg_rtx (V4SImode);
        rtx temp1 = gen_reg_rtx (V4SFmode);
        rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MASK>mode, offset);
        rtx op1 = simplify_gen_subreg (V4HFmode, operands[1], <MODE>mode, offset);
        emit_insn (gen_extendv4hfv4sf2 (temp1, op1));
        emit_insn (gen_fixuns_truncv4sfv4si2 (temp0, temp1));
        emit_insn (gen_truncv4siv4hi2 (op0, temp0));
      }
    DONE;
  }
)


;; VXSF

(define_expand "div<mode>3"
  [(set (match_operand:VXSF 0 "register_operand" "")
        (div:VXSF (match_operand:VXSF 1 "register_float1_operand" "")
                  (match_operand:VXSF 2 "register_operand" "")))]
  ""
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
        rtx t = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_kvx_frec<suffix> (t, b, rm));
        emit_insn (gen_kvx_fmul<suffix> (operands[0], a, t, rm));
      }
    else if (flag_unsafe_math_optimizations)
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
     else
       {
         emit_library_call_value
           (gen_rtx_SYMBOL_REF (Pmode, "__div<mode>3"),
            operands[0], LCT_CONST, <MODE>mode,
            operands[1], <MODE>mode, operands[2], <MODE>mode);
       }
    DONE;
  }
)

(define_expand "sqrt<mode>2"
  [(match_operand:VXSF 0 "register_operand" "")
   (match_operand:VXSF 1 "register_operand" "")]
  "flag_reciprocal_math"
  {
    rtx temp = gen_reg_rtx (<MODE>mode);
    rtx rm = gen_rtx_CONST_STRING (VOIDmode, "");
    emit_insn (gen_kvx_frsr<suffix> (temp, operands[1], rm));
    emit_insn (gen_mul<mode>3 (operands[0], operands[1], temp));
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


;; VXDF

(define_expand "div<mode>3"
  [(set (match_operand:VXDF 0 "register_operand" "")
        (div:VXDF (match_operand:VXDF 1 "register_operand" "")
                  (match_operand:VXDF 2 "register_operand" "")))]
  ""
  {
    emit_library_call_value
      (gen_rtx_SYMBOL_REF (Pmode, "__div<mode>3"),
       operands[0], LCT_CONST, <MODE>mode,
       operands[1], <MODE>mode, operands[2], <MODE>mode);
    DONE;
  }
)


;; S128F (V8HF V4SF)

(define_insn_and_split "fma<mode>4"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (fma:S128F (match_operand:S128F 1 "register_operand" "r")
                   (match_operand:S128F 2 "register_operand" "r")
                   (match_operand:S128F 3 "register_operand" "0")))]
  ""
  "ffma<suffix> %0 = %1, %2"
  "(KV3_1||KV4) && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (fma:<CHUNK> (subreg:<CHUNK> (match_dup 1) 0)
                     (subreg:<CHUNK> (match_dup 2) 0)
                     (subreg:<CHUNK> (match_dup 3) 0)))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (fma:<CHUNK> (subreg:<CHUNK> (match_dup 1) 8)
                     (subreg:<CHUNK> (match_dup 2) 8)
                     (subreg:<CHUNK> (match_dup 3) 8)))]
  ""
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "madd_fp3") (const_string "madd_fp4")))]
)

(define_insn_and_split "fnma<mode>4"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (fma:S128F (neg:S128F (match_operand:S128F 1 "register_operand" "r"))
                   (match_operand:S128F 2 "register_operand" "r")
                   (match_operand:S128F 3 "register_operand" "0")))]
  ""
  "ffms<suffix> %0 = %1, %2"
  "(KV3_1||KV4) && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (fma:<CHUNK> (neg:<CHUNK> (subreg:<CHUNK> (match_dup 1) 0))
                     (subreg:<CHUNK> (match_dup 2) 0)
                     (subreg:<CHUNK> (match_dup 3) 0)))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (fma:<CHUNK> (neg:<CHUNK> (subreg:<CHUNK> (match_dup 1) 8))
                     (subreg:<CHUNK> (match_dup 2) 8)
                     (subreg:<CHUNK> (match_dup 3) 8)))]
  ""
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "madd_fp3") (const_string "madd_fp4")))]
)

(define_insn_and_split "trunc<wide><mode>2"
  [(set (match_operand:S128F 0 "register_operand" "=&r")
        (float_truncate:S128F (match_operand:<WIDE> 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (float_truncate:<HALF> (subreg:<HWIDE> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 8)
        (float_truncate:<HALF> (subreg:<HWIDE> (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_full")]
)

(define_insn_and_split "extend<mode><wide>2"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
        (float_extend:<WIDE> (match_operand:S128F 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (float_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (float_extend:<HWIDE> (subreg:<HALF> (match_dup 1) 8)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)


;; V128F (V8HF V4SF V2DF)

(define_insn "fmin<mode>3"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (smin:V128F (match_operand:V128F 1 "register_operand" "r")
                    (match_operand:V128F 2 "register_operand" "r")))]
  ""
  "fmin<chunkx> %x0 = %x1, %x2\n\tfmin<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*fmin<mode>3_s1"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (smin:V128F (vec_duplicate:V128F (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                    (match_operand:V128F 2 "register_operand" "r")))]
  ""
  "fmin<chunkx> %x0 = %1, %x2\n\tfmin<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*fmin<mode>3_s2"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (smin:V128F (match_operand:V128F 1 "register_operand" "r")
                    (vec_duplicate:V128F (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  "fmin<chunkx> %x0 = %x1, %2\n\tfmin<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "fmax<mode>3"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (smax:V128F (match_operand:V128F 1 "register_operand" "r")
                    (match_operand:V128F 2 "register_operand" "r")))]
  ""
  "fmax<chunkx> %x0 = %x1, %x2\n\tfmax<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*fmax<mode>3_s1"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (smax:V128F (vec_duplicate:V128F (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                    (match_operand:V128F 2 "register_operand" "r")))]
  ""
  "fmax<chunkx> %x0 = %1, %x2\n\tfmax<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*fmax<mode>3_s2"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (smax:V128F (match_operand:V128F 1 "register_operand" "r")
                    (vec_duplicate:V128F (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  "fmax<chunkx> %x0 = %x1, %2\n\tfmax<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "neg<mode>2"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (neg:V128F (match_operand:V128F 1 "register_operand" "r")))]
  ""
  "fneg<chunkx> %x0 = %x1\n\tfneg<chunkx> %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "abs<mode>2"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (abs:V128F (match_operand:V128F 1 "register_operand" "r")))]
  ""
  "fabs<chunkx> %x0 = %x1\n\tfabs<chunkx> %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_expand "copysign<mode>3"
  [(match_operand:V128F 0 "register_operand")
   (match_operand:V128F 1 "register_operand")
   (match_operand:V128F 2 "register_operand")]
  ""
  {
    rtx fabs1 = gen_reg_rtx (<MODE>mode);
    emit_insn (gen_abs<mode>2 (fabs1, operands[1]));
    rtx fneg1 = gen_reg_rtx (<MODE>mode);
    emit_insn (gen_neg<mode>2 (fneg1, fabs1));
    rtx sign2 = gen_reg_rtx (<MASK>mode);
    convert_move (sign2, operands[2], 0);
    scalar_mode inner_mode = GET_MODE_INNER (<MODE>mode);
    rtx ltz = GET_MODE_SIZE (inner_mode) == UNITS_PER_WORD
            ? gen_rtx_CONST_STRING (VOIDmode, ".dltz")
            : gen_rtx_CONST_STRING (VOIDmode, ".ltz");
    emit_insn (gen_kvx_selectf<suffix> (operands[0], fneg1, fabs1, sign2, ltz));
    DONE;
  }
)

(define_expand "xorsign<mode>3"
  [(match_operand:V128F 0 "register_operand")
   (match_operand:V128F 1 "register_operand")
   (match_operand:V128F 2 "register_operand")]
  ""
  {
    rtx maskv8hf = GEN_INT (0x8000800080008000);
    rtx maskv4sf = GEN_INT (0x8000000080000000);
    rtx maskv2df = GEN_INT (0x8000000000000000);
    rtx mask = gen_reg_rtx (DImode);
    emit_move_insn (mask, mask<mode>);
    rtx sign2 = gen_reg_rtx (<MODE>mode);
    emit_insn (gen_rtx_SET (sign2, gen_rtx_UNSPEC (<MODE>mode, gen_rtvec (2, operands[2], mask), UNSPEC_ANDD)));
    emit_insn (gen_rtx_SET (operands[0], gen_rtx_UNSPEC (<MODE>mode, gen_rtvec (2, operands[1], sign2), UNSPEC_XORD)));
    DONE;
  }
)


;; V128G (V4SF V2DF)

(define_insn_and_split "float<mask><mode>2"
  [(set (match_operand:V128G 0 "register_operand" "=r")
        (float:V128G (match_operand:<MASK> 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (float:<HALF> (subreg:<HMASK> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 8)
        (float:<HALF> (subreg:<HMASK> (match_dup 1) 8)))]
  ""
)

(define_insn_and_split "floatuns<mask><mode>2"
  [(set (match_operand:V128G 0 "register_operand" "=r")
        (unsigned_float:V128G (match_operand:<MASK> 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unsigned_float:<HALF> (subreg:<HMASK> (match_dup 1) 0)))
   (set (subreg:<HALF> (match_dup 0) 8)
        (unsigned_float:<HALF> (subreg:<HMASK> (match_dup 1) 8)))]
  ""
)

(define_insn_and_split "fix_trunc<mode><mask>2"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (fix:<MASK> (match_operand:V128G 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HMASK> (match_dup 0) 0)
        (fix:<HMASK> (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HMASK> (match_dup 0) 8)
        (fix:<HMASK> (subreg:<HALF> (match_dup 1) 8)))]
  ""
)

(define_insn_and_split "fixuns_trunc<mode><mask>2"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (unsigned_fix:<MASK> (match_operand:V128G 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HMASK> (match_dup 0) 0)
        (unsigned_fix:<HMASK> (subreg:<HALF> (match_dup 1) 0)))
   (set (subreg:<HMASK> (match_dup 0) 8)
        (unsigned_fix:<HMASK> (subreg:<HALF> (match_dup 1) 8)))]
  ""
)


;; V8HF

(define_insn_and_split "addv8hf3"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (plus:V8HF (match_operand:V8HF 1 "register_operand" "r")
                   (match_operand:V8HF 2 "register_operand" "r")))]
  ""
  "faddho %0 = %1, %2"
  "(KV3_1||KV4) && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (plus:V4HF (subreg:V4HF (match_dup 1) 0)
                   (subreg:V4HF (match_dup 2) 0)))
   (set (subreg:V4HF (match_dup 0) 8)
        (plus:V4HF (subreg:V4HF (match_dup 1) 8)
                   (subreg:V4HF (match_dup 2) 8)))]
  ""
  [(set_attr "type" "madd_fp3")]
)

(define_insn_and_split "subv8hf3"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (minus:V8HF (match_operand:V8HF 1 "register_operand" "r")
                    (match_operand:V8HF 2 "register_operand" "r")))]
  ""
  "fsbfho %0 = %2, %1"
  "(KV3_1||KV4) && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (minus:V4HF (subreg:V4HF (match_dup 1) 0)
                    (subreg:V4HF (match_dup 2) 0)))
   (set (subreg:V4HF (match_dup 0) 8)
        (minus:V4HF (subreg:V4HF (match_dup 1) 8)
                    (subreg:V4HF (match_dup 2) 8)))]
  ""
  [(set_attr "type" "mult_fp3")]
)

(define_insn_and_split "mulv8hf3"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (mult:V8HF (match_operand:V8HF 1 "register_operand" "r")
                   (match_operand:V8HF 2 "register_operand" "r")))]
  ""
  "fmulho %0 = %1, %2"
  "(KV3_1||KV4) && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (mult:V4HF (subreg:V4HF (match_dup 1) 0)
                   (subreg:V4HF (match_dup 2) 0)))
   (set (subreg:V4HF (match_dup 0) 8)
        (mult:V4HF (subreg:V4HF (match_dup 1) 8)
                   (subreg:V4HF (match_dup 2) 8)))]
  ""
  [(set_attr "type" "mult_fp3")]
)

;;(define_insn "truncv8sfv8hf2"
;;  [(set (match_operand:V8HF 0 "register_operand" "=r")
;;        (float_truncate:V8HF (match_operand:V8SF 1 "register_operand" "r")))]
;;  ""
;;  "fnarrowwhq %x0 = %x1\n\tfnarrowwhq %y0 = %y1"
;;  [(set_attr "type" "alu_lite_x2")
;;   (set_attr "length"         "8")]
;;)

;;(define_insn_and_split "extendv8hfv8sf2"
;;  [(set (match_operand:V8SF 0 "register_operand" "=r")
;;        (float_extend:V8SF (match_operand:V8HF 1 "register_operand" "r")))]
;;  ""
;;  "#"
;;  "reload_completed"
;;  [(set (subreg:V4SF (match_dup 0) 0)
;;        (float_extend:V4SF (subreg:V4HF (match_dup 1) 0)))
;;   (set (subreg:V4SF (match_dup 0) 16)
;;        (float_extend:V4SF (subreg:V4HF (match_dup 1) 8)))]
;;  ""
;;)

(define_insn_and_split "floatv8siv8hf2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (float:V8HF (match_operand:V8SI 1 "register_operand" "r")))
   (clobber (match_scratch:V8SF 2 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float:V8SF (match_dup 1)))
   (set (match_dup 0)
        (float_truncate:V8HF (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V8SFmode);
  }
)

(define_insn_and_split "floatunsv8siv8hf2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unsigned_float:V8HF (match_operand:V8SI 1 "register_operand" "r")))
   (clobber (match_scratch:V8SF 2 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (unsigned_float:V8SF (match_dup 1)))
   (set (match_dup 0)
        (float_truncate:V8HF (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V8SFmode);
  }
)

(define_insn_and_split "fix_truncv8hfv8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (fix:V8SI (match_operand:V8HF 1 "register_operand" "r")))
   (clobber (match_scratch:V8SF 2 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float_extend:V8SF (match_dup 1)))
   (set (match_dup 0)
        (fix:V8SI (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V8SFmode);
  }
)

(define_insn_and_split "fixuns_truncv8hfv8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (unsigned_fix:V8SI (match_operand:V8HF 1 "register_operand" "r")))
   (clobber (match_scratch:V8SF 2 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float_extend:V8SF (match_dup 1)))
   (set (match_dup 0)
        (unsigned_fix:V8SI (match_dup 2)))]
  {
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V8SFmode);
  }
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

(define_insn_and_split "addv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (plus:V4SF (match_operand:V4SF 1 "register_operand" "r")
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "faddwq %0 = %1, %2"
  "KV4 && reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (plus:V2SF (subreg:V2SF (match_dup 1) 0)
                   (subreg:V2SF (match_dup 2) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (plus:V2SF (subreg:V2SF (match_dup 1) 8)
                   (subreg:V2SF (match_dup 2) 8)))]
  ""
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_insn_and_split "subv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (minus:V4SF (match_operand:V4SF 1 "register_operand" "r")
                    (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "fsbfwq %0 = %2, %1"
  "KV4 && reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (minus:V2SF (subreg:V2SF (match_dup 1) 0)
                    (subreg:V2SF (match_dup 2) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (minus:V2SF (subreg:V2SF (match_dup 1) 8)
                    (subreg:V2SF (match_dup 2) 8)))]
  ""
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_insn_and_split "mulv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (mult:V4SF (match_operand:V4SF 1 "register_operand" "r")
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "fmulwq %0 = %1, %2"
  "KV4 && reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (mult:V2SF (subreg:V2SF (match_dup 1) 0)
                   (subreg:V2SF (match_dup 2) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (mult:V2SF (subreg:V2SF (match_dup 1) 8)
                   (subreg:V2SF (match_dup 2) 8)))]
  ""
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_expand "floatv4hiv4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (float:V4SF (match_operand:V4HI 1 "register_operand" "")))
   (clobber (match_dup 2))]
  ""
  {
    operands[2] = gen_reg_rtx (V4SImode);
    emit_insn (gen_kvx_sxhwq (operands[2], operands[1]));
    emit_insn (gen_floatv4siv4sf2 (operands[0], operands[2]));
    DONE;
  }
)

(define_expand "floatunsv4hiv4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unsigned_float:V4SF (match_operand:V4HI 1 "register_operand" "")))
   (clobber (match_dup 2))]
  ""
  {
    operands[2] = gen_reg_rtx (V4SImode);
    emit_insn (gen_kvx_zxhwq (operands[2], operands[1]));
    emit_insn (gen_floatunsv4siv4sf2 (operands[0], operands[2]));
    DONE;
  }
)

(define_expand "fix_truncv4sfv4hi2"
  [(set (match_operand:V4HI 0 "register_operand" "")
        (truncate:V4HI (fix:V4SI (match_operand:V4SF 1 "register_operand" ""))))
   (clobber (match_dup 2))]
  ""
  {
    operands[2] = gen_reg_rtx (V4SImode);
    emit_insn (gen_fix_truncv4sfv4si2 (operands[2], operands[1]));
    emit_insn (gen_kvx_truncwhq (operands[0], operands[2]));
    DONE;
  }
)

(define_expand "fixuns_truncv4sfv4hi2"
  [(set (match_operand:V4HI 0 "register_operand" "")
        (truncate:V4HI (unsigned_fix:V4SI (match_operand:V4SF 1 "register_operand" ""))))
   (clobber (match_dup 2))]
  ""
  {
    operands[2] = gen_reg_rtx (V4SImode);
    emit_insn (gen_fixuns_truncv4sfv4si2 (operands[2], operands[1]));
    emit_insn (gen_kvx_truncwhq (operands[0], operands[2]));
    DONE;
  }
)

(define_insn_and_split "floatv4div4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (float:V4SF (match_operand:V4DI 1 "register_operand" "r")))
   (clobber (match_scratch:V2DF 2 "=&r"))
   (clobber (match_scratch:V2DF 3 "=&r"))
   (clobber (match_scratch:V4SF 4 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float:V2DF (subreg:V2DI (match_dup 1) 0)))
   (set (subreg:V2SF (match_dup 4) 0)
        (float_truncate:V2SF (match_dup 2)))
   (set (match_dup 3)
        (float:V2DF (subreg:V2DI (match_dup 1) 16)))
   (set (subreg:V2SF (match_dup 4) 8)
        (float_truncate:V2SF (match_dup 3)))
   (set (match_dup 0) (match_dup 4))]
  {
    rtx operand1 = gen_reg_rtx (V4DImode);
    emit_move_insn (operand1, operands[1]);
    operands[1] = operand1;
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (V4SFmode);
  }
)

(define_insn_and_split "floatunsv4div4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unsigned_float:V4SF (match_operand:V4DI 1 "register_operand" "r")))
   (clobber (match_scratch:V2DF 2 "=&r"))
   (clobber (match_scratch:V2DF 3 "=&r"))
   (clobber (match_scratch:V4SF 4 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (unsigned_float:V2DF (subreg:V2DI (match_dup 1) 0)))
   (set (subreg:V2SF (match_dup 4) 0)
        (float_truncate:V2SF (match_dup 2)))
   (set (match_dup 3)
        (unsigned_float:V2DF (subreg:V2DI (match_dup 1) 16)))
   (set (subreg:V2SF (match_dup 4) 8)
        (float_truncate:V2SF (match_dup 3)))
   (set (match_dup 0) (match_dup 4))]
  {
    rtx operand1 = gen_reg_rtx (V4DImode);
    emit_move_insn (operand1, operands[1]);
    operands[1] = operand1;
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (V4SFmode);
  }
)

(define_insn_and_split "fix_truncv4sfv4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (fix:V4DI (match_operand:V4SF 1 "register_operand" "r")))
   (clobber (match_scratch:V2DF 2 "=&r"))
   (clobber (match_scratch:V2DF 3 "=&r"))
   (clobber (match_scratch:V4DI 4 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float_extend:V2DF (subreg:V2SF (match_dup 1) 0)))
   (set (subreg:V2DI (match_dup 4) 0)
        (fix:V2DI (match_dup 2)))
   (set (match_dup 3)
        (float_extend:V2DF (subreg:V2SF (match_dup 1) 8)))
   (set (subreg:V2DI (match_dup 4) 16)
        (fix:V2DI (match_dup 3)))
   (set (match_dup 0) (match_dup 4))]
  {
    rtx operand1 = gen_reg_rtx (V4SFmode);
    emit_move_insn (operand1, operands[1]);
    operands[1] = operand1;
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (V4DImode);
  }
)

(define_insn_and_split "fixuns_truncv4sfv4di2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unsigned_fix:V4DI (match_operand:V4SF 1 "register_operand" "r")))
   (clobber (match_scratch:V2DF 2 "=&r"))
   (clobber (match_scratch:V2DF 3 "=&r"))
   (clobber (match_scratch:V4DI 4 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float_extend:V2DF (subreg:V2SF (match_dup 1) 0)))
   (set (subreg:V2DI (match_dup 4) 0)
        (unsigned_fix:V2DI (match_dup 2)))
   (set (match_dup 3)
        (float_extend:V2DF (subreg:V2SF (match_dup 1) 8)))
   (set (subreg:V2DI (match_dup 4) 16)
        (unsigned_fix:V2DI (match_dup 3)))
   (set (match_dup 0) (match_dup 4))]
  {
    rtx operand1 = gen_reg_rtx (V4SFmode);
    emit_move_insn (operand1, operands[1]);
    operands[1] = operand1;
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (V4DImode);
  }
)


;; V2DF

(define_insn_and_split "addv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (plus:V2DF (match_operand:V2DF 1 "register_operand" "r")
                   (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "fadddp %0 = %1, %2"
  "KV4 && reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (plus:DF (subreg:DF (match_dup 1) 0)
                 (subreg:DF (match_dup 2) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (plus:DF (subreg:DF (match_dup 1) 8)
                 (subreg:DF (match_dup 2) 8)))]
  ""
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "madd_fp4") (const_string "mult_fp4")))]
)

(define_insn_and_split "subv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (minus:V2DF (match_operand:V2DF 1 "register_operand" "r")
                    (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "fsbfdp %0 = %2, %1"
  "KV4 && reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (minus:DF (subreg:DF (match_dup 1) 0)
                  (subreg:DF (match_dup 2) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (minus:DF (subreg:DF (match_dup 1) 8)
                  (subreg:DF (match_dup 2) 8)))]
  ""
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "madd_fp4") (const_string "mult_fp4")))]
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
  [(set_attr "type" "madd_fp4")]
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
  [(set_attr "type" "madd_fp4")]
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
  [(set_attr "type" "madd_fp4")]
)

;; S256F (V16HF V8SF)

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
  "(KV3_1||KV4) && reload_completed"
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
  "(KV3_1||KV4) && reload_completed"
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

(define_insn_and_split "trunc<wide><mode>2"
  [(set (match_operand:S256F 0 "register_operand" "=&r")
        (float_truncate:S256F (match_operand:<WIDE> 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QUART> (match_dup 0) 0)
        (float_truncate:<QUART> (subreg:<QWIDE> (match_dup 1) 0)))
   (set (subreg:<QUART> (match_dup 0) 8)
        (float_truncate:<QUART> (subreg:<QWIDE> (match_dup 1) 16)))
   (set (subreg:<QUART> (match_dup 0) 16)
        (float_truncate:<QUART> (subreg:<QWIDE> (match_dup 1) 32)))
   (set (subreg:<QUART> (match_dup 0) 24)
        (float_truncate:<QUART> (subreg:<QWIDE> (match_dup 1) 48)))]
  ""
  [(set_attr "type" "alu_full")]
)

(define_insn_and_split "extend<mode><wide>2"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
        (float_extend:<WIDE> (match_operand:S256F 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QWIDE> (match_dup 0) 0)
        (float_extend:<QWIDE> (subreg:<QUART> (match_dup 1) 0)))
   (set (subreg:<QWIDE> (match_dup 0) 16)
        (float_extend:<QWIDE> (subreg:<QUART> (match_dup 1) 8)))
   (set (subreg:<QWIDE> (match_dup 0) 32)
        (float_extend:<QWIDE> (subreg:<QUART> (match_dup 1) 16)))
   (set (subreg:<QWIDE> (match_dup 0) 48)
        (float_extend:<QWIDE> (subreg:<QUART> (match_dup 1) 24)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
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
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn_and_split "*fmin<mode>3_s1"
  [(set (match_operand:V256F 0 "register_operand" "=&r")
        (smin:V256F (vec_duplicate:V256F (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                    (match_operand:V256F 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (smin:<HALF> (vec_duplicate:<HALF> (match_dup 1))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (smin:<HALF> (vec_duplicate:<HALF> (match_dup 1))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn_and_split "*fmin<mode>3_s2"
  [(set (match_operand:V256F 0 "register_operand" "=&r")
        (smin:V256F (match_operand:V256F 1 "register_operand" "r")
                    (vec_duplicate:V256F (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (smin:<HALF> (subreg:<HALF> (match_dup 1) 0)
                     (vec_duplicate:<HALF> (match_dup 2))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (smin:<HALF> (subreg:<HALF> (match_dup 1) 16)
                     (vec_duplicate:<HALF> (match_dup 2))))]
  ""
  [(set_attr "type" "alu_thin_x2")]
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
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn_and_split "*fmax<mode>3_s1"
  [(set (match_operand:V256F 0 "register_operand" "=&r")
        (smax:V256F (vec_duplicate:V256F (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                    (match_operand:V256F 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (smax:<HALF> (vec_duplicate:<HALF> (match_dup 1))
                     (subreg:<HALF> (match_dup 2) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (smax:<HALF> (vec_duplicate:<HALF> (match_dup 1))
                     (subreg:<HALF> (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn_and_split "*fmax<mode>3_s2"
  [(set (match_operand:V256F 0 "register_operand" "=&r")
        (smax:V256F (match_operand:V256F 1 "register_operand" "r")
                    (vec_duplicate:V256F (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (smax:<HALF> (subreg:<HALF> (match_dup 1) 0)
                     (vec_duplicate:<HALF> (match_dup 2))))
   (set (subreg:<HALF> (match_dup 0) 16)
        (smax:<HALF> (subreg:<HALF> (match_dup 1) 16)
                     (vec_duplicate:<HALF> (match_dup 2))))]
  ""
  [(set_attr "type" "alu_thin_x2")]
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
  [(set_attr "type" "alu_thin_x2")]
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
  [(set_attr "type" "alu_thin_x2")]
)

(define_expand "copysign<mode>3"
  [(match_operand:V256F 0 "register_operand")
   (match_operand:V256F 1 "register_operand")
   (match_operand:V256F 2 "register_operand")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = simplify_gen_subreg (<HALF>mode, operands[0], <MODE>mode, i*16);
        rtx opnd1 = simplify_gen_subreg (<HALF>mode, operands[1], <MODE>mode, i*16);
        rtx opnd2 = simplify_gen_subreg (<HALF>mode, operands[2], <MODE>mode, i*16);
        emit_insn (gen_copysign<half>3 (opnd0, opnd1, opnd2));
      }
    DONE;
  }
)

(define_expand "xorsign<mode>3"
  [(match_operand:V256F 0 "register_operand")
   (match_operand:V256F 1 "register_operand")
   (match_operand:V256F 2 "register_operand")]
  ""
  {
    rtx maskv16hf = GEN_INT (0x8000800080008000);
    rtx maskv8sf = GEN_INT (0x8000000080000000);
    rtx maskv4df = GEN_INT (0x8000000000000000);
    rtx mask = gen_reg_rtx (DImode);
    emit_move_insn (mask, mask<mode>);
    rtx sign2 = gen_reg_rtx (<MODE>mode);
    emit_insn (gen_rtx_SET (sign2, gen_rtx_UNSPEC (<MODE>mode, gen_rtvec (2, operands[2], mask), UNSPEC_ANDD)));
    emit_insn (gen_rtx_SET (operands[0], gen_rtx_UNSPEC (<MODE>mode, gen_rtvec (2, operands[1], sign2), UNSPEC_XORD)));
    DONE;
  }
)


;; V256G (V8SF V4DF)

(define_insn_and_split "float<mask><mode>2"
  [(set (match_operand:V256G 0 "register_operand" "=r")
        (float:V256G (match_operand:<MASK> 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QUART> (match_dup 0) 0)
        (float:<QUART> (subreg:<QMASK> (match_dup 1) 0)))
   (set (subreg:<QUART> (match_dup 0) 8)
        (float:<QUART> (subreg:<QMASK> (match_dup 1) 8)))
   (set (subreg:<QUART> (match_dup 0) 16)
        (float:<QUART> (subreg:<QMASK> (match_dup 1) 16)))
   (set (subreg:<QUART> (match_dup 0) 24)
        (float:<QUART> (subreg:<QMASK> (match_dup 1) 24)))]
  ""
)

(define_insn_and_split "floatuns<mask><mode>2"
  [(set (match_operand:V256G 0 "register_operand" "=r")
        (unsigned_float:V256G (match_operand:<MASK> 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QUART> (match_dup 0) 0)
        (unsigned_float:<QUART> (subreg:<QMASK> (match_dup 1) 0)))
   (set (subreg:<QUART> (match_dup 0) 8)
        (unsigned_float:<QUART> (subreg:<QMASK> (match_dup 1) 8)))
   (set (subreg:<QUART> (match_dup 0) 16)
        (unsigned_float:<QUART> (subreg:<QMASK> (match_dup 1) 16)))
   (set (subreg:<QUART> (match_dup 0) 24)
        (unsigned_float:<QUART> (subreg:<QMASK> (match_dup 1) 24)))]
  ""
)

(define_insn_and_split "fix_trunc<mode><mask>2"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (fix:<MASK> (match_operand:V256G 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QMASK> (match_dup 0) 0)
        (fix:<QMASK> (subreg:<QUART> (match_dup 1) 0)))
   (set (subreg:<QMASK> (match_dup 0) 8)
        (fix:<QMASK> (subreg:<QUART> (match_dup 1) 8)))
   (set (subreg:<QMASK> (match_dup 0) 16)
        (fix:<QMASK> (subreg:<QUART> (match_dup 1) 16)))
   (set (subreg:<QMASK> (match_dup 0) 24)
        (fix:<QMASK> (subreg:<QUART> (match_dup 1) 24)))]
  ""
)

(define_insn_and_split "fixuns_trunc<mode><mask>2"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (unsigned_fix:<MASK> (match_operand:V256G 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QMASK> (match_dup 0) 0)
        (unsigned_fix:<QMASK> (subreg:<QUART> (match_dup 1) 0)))
   (set (subreg:<QMASK> (match_dup 0) 8)
        (unsigned_fix:<QMASK> (subreg:<QUART> (match_dup 1) 8)))
   (set (subreg:<QMASK> (match_dup 0) 16)
        (unsigned_fix:<QMASK> (subreg:<QUART> (match_dup 1) 16)))
   (set (subreg:<QMASK> (match_dup 0) 24)
        (unsigned_fix:<QMASK> (subreg:<QUART> (match_dup 1) 24)))]
  ""
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
  "(KV3_1||KV4) && reload_completed"
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
  "(KV3_1||KV4) && reload_completed"
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
  "(KV3_1||KV4) && reload_completed"
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


;; V8SF

(define_insn "addv8sf3"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (plus:V8SF (match_operand:V8SF 1 "register_operand" "r")
                   (match_operand:V8SF 2 "register_operand" "r")))]
  ""
  "#"
  [(set_attr "type" "madd_fp4")]
)

(define_split
  [(set (match_operand:V8SF 0 "register_operand" "")
        (plus:V8SF (match_operand:V8SF 1 "register_operand" "")
                   (match_operand:V8SF 2 "register_operand" "")))]
  "KV3 && reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (plus:V4SF (subreg:V4SF (match_dup 1) 0)
                   (subreg:V4SF (match_dup 2) 0)))
   (set (subreg:V4SF (match_dup 0) 16)
        (plus:V4SF (subreg:V4SF (match_dup 1) 16)
                   (subreg:V4SF (match_dup 2) 16)))]
  ""
)

(define_split
  [(set (match_operand:V8SF 0 "register_operand" "")
        (plus:V8SF (match_operand:V8SF 1 "register_operand" "")
                   (match_operand:V8SF 2 "register_operand" "")))]
  "KV4 && reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (plus:V2SF (subreg:V2SF (match_dup 1) 0)
                   (subreg:V2SF (match_dup 2) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (plus:V2SF (subreg:V2SF (match_dup 1) 8)
                   (subreg:V2SF (match_dup 2) 8)))
   (set (subreg:V2SF (match_dup 0) 16)
        (plus:V2SF (subreg:V2SF (match_dup 1) 16)
                   (subreg:V2SF (match_dup 2) 16)))
   (set (subreg:V2SF (match_dup 0) 24)
        (plus:V2SF (subreg:V2SF (match_dup 1) 24)
                   (subreg:V2SF (match_dup 2) 24)))]
  ""
)

(define_insn "subv8sf3"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (minus:V8SF (match_operand:V8SF 1 "register_operand" "r")
                    (match_operand:V8SF 2 "register_operand" "r")))]
  ""
  "#"
  [(set_attr "type" "madd_fp4")]
)

(define_split
  [(set (match_operand:V8SF 0 "register_operand" "")
        (minus:V8SF (match_operand:V8SF 1 "register_operand" "")
                    (match_operand:V8SF 2 "register_operand" "")))]
  "KV3 && reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (minus:V4SF (subreg:V4SF (match_dup 1) 0)
                    (subreg:V4SF (match_dup 2) 0)))
   (set (subreg:V4SF (match_dup 0) 16)
        (minus:V4SF (subreg:V4SF (match_dup 1) 16)
                    (subreg:V4SF (match_dup 2) 16)))]
  ""
)

(define_split
  [(set (match_operand:V8SF 0 "register_operand" "")
        (minus:V8SF (match_operand:V8SF 1 "register_operand" "")
                    (match_operand:V8SF 2 "register_operand" "")))]
  "KV4 && reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (minus:V2SF (subreg:V2SF (match_dup 1) 0)
                    (subreg:V2SF (match_dup 2) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (minus:V2SF (subreg:V2SF (match_dup 1) 8)
                    (subreg:V2SF (match_dup 2) 8)))
   (set (subreg:V2SF (match_dup 0) 16)
        (minus:V2SF (subreg:V2SF (match_dup 1) 16)
                    (subreg:V2SF (match_dup 2) 16)))
   (set (subreg:V2SF (match_dup 0) 24)
        (minus:V2SF (subreg:V2SF (match_dup 1) 24)
                    (subreg:V2SF (match_dup 2) 24)))]
  ""
)

(define_insn "mulv8sf3"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (mult:V8SF (match_operand:V8SF 1 "register_operand" "r")
                   (match_operand:V8SF 2 "register_operand" "r")))]
  ""
  "#"
  [(set_attr "type" "madd_fp4")]
)

(define_split
  [(set (match_operand:V8SF 0 "register_operand" "")
        (mult:V8SF (match_operand:V8SF 1 "register_operand" "")
                   (match_operand:V8SF 2 "register_operand" "")))]
  "KV3 && reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (mult:V4SF (subreg:V4SF (match_dup 1) 0)
                   (subreg:V4SF (match_dup 2) 0)))
   (set (subreg:V4SF (match_dup 0) 16)
        (mult:V4SF (subreg:V4SF (match_dup 1) 16)
                   (subreg:V4SF (match_dup 2) 16)))]
  ""
)

(define_split
  [(set (match_operand:V8SF 0 "register_operand" "")
        (mult:V8SF (match_operand:V8SF 1 "register_operand" "")
                   (match_operand:V8SF 2 "register_operand" "")))]
  "KV4 && reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (mult:V2SF (subreg:V2SF (match_dup 1) 0)
                   (subreg:V2SF (match_dup 2) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (mult:V2SF (subreg:V2SF (match_dup 1) 8)
                   (subreg:V2SF (match_dup 2) 8)))
   (set (subreg:V2SF (match_dup 0) 16)
        (mult:V2SF (subreg:V2SF (match_dup 1) 16)
                   (subreg:V2SF (match_dup 2) 16)))
   (set (subreg:V2SF (match_dup 0) 24)
        (mult:V2SF (subreg:V2SF (match_dup 1) 24)
                   (subreg:V2SF (match_dup 2) 24)))]
  ""
)

(define_expand "floatv8hiv8sf2"
  [(set (match_operand:V8SF 0 "register_operand" "")
        (float:V8SF (match_operand:V8HI 1 "register_operand" "")))
   (clobber (match_dup 2))]
  ""
  {
    operands[2] = gen_reg_rtx (V8SImode);
    emit_insn (gen_kvx_sxhwo (operands[2], operands[1]));
    emit_insn (gen_floatv8siv8sf2 (operands[0], operands[2]));
    DONE;
  }
)

(define_expand "floatunsv8hiv8sf2"
  [(set (match_operand:V8SF 0 "register_operand" "")
        (unsigned_float:V8SF (match_operand:V8HI 1 "register_operand" "")))
   (clobber (match_dup 2))]
  ""
  {
    operands[2] = gen_reg_rtx (V8SImode);
    emit_insn (gen_kvx_zxhwo (operands[2], operands[1]));
    emit_insn (gen_floatunsv8siv8sf2 (operands[0], operands[2]));
    DONE;
  }
)

(define_expand "fix_truncv8sfv8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "")
        (truncate:V8HI (fix:V8SI (match_operand:V8SF 1 "register_operand" ""))))
   (clobber (match_dup 2))]
  ""
  {
    operands[2] = gen_reg_rtx (V8SImode);
    emit_insn (gen_fix_truncv8sfv8si2 (operands[2], operands[1]));
    emit_insn (gen_kvx_truncwho (operands[0], operands[2]));
    DONE;
  }
)

(define_expand "fixuns_truncv8sfv8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "")
        (truncate:V8HI (unsigned_fix:V8SI (match_operand:V8SF 1 "register_operand" ""))))
   (clobber (match_dup 2))]
  ""
  {
    operands[2] = gen_reg_rtx (V8SImode);
    emit_insn (gen_fixuns_truncv8sfv8si2 (operands[2], operands[1]));
    emit_insn (gen_kvx_truncwho (operands[0], operands[2]));
    DONE;
  }
)

(define_insn_and_split "floatv8div8sf2"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (float:V8SF (match_operand:V8DI 1 "register_operand" "r")))
   (clobber (match_scratch:V2DF 2 "=&r"))
   (clobber (match_scratch:V2DF 3 "=&r"))
   (clobber (match_scratch:V8SF 4 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float:V2DF (subreg:V2DI (match_dup 1) 0)))
   (set (subreg:V2SF (match_dup 4) 0)
        (float_truncate:V2SF (match_dup 2)))
   (set (match_dup 3)
        (float:V2DF (subreg:V2DI (match_dup 1) 16)))
   (set (subreg:V2SF (match_dup 4) 8)
        (float_truncate:V2SF (match_dup 3)))
   (set (match_dup 2)
        (float:V2DF (subreg:V2DI (match_dup 1) 32)))
   (set (subreg:V2SF (match_dup 4) 16)
        (float_truncate:V2SF (match_dup 2)))
   (set (match_dup 3)
        (float:V2DF (subreg:V2DI (match_dup 1) 48)))
   (set (subreg:V2SF (match_dup 4) 24)
        (float_truncate:V2SF (match_dup 3)))
   (set (match_dup 0) (match_dup 4))]
  {
    rtx operand1 = gen_reg_rtx (V8DImode);
    emit_move_insn (operand1, operands[1]);
    operands[1] = operand1;
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (V8SFmode);
  }
)

(define_insn_and_split "floatunsv8div8sf2"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unsigned_float:V8SF (match_operand:V8DI 1 "register_operand" "r")))
   (clobber (match_scratch:V2DF 2 "=&r"))
   (clobber (match_scratch:V2DF 3 "=&r"))
   (clobber (match_scratch:V8SF 4 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (unsigned_float:V2DF (subreg:V2DI (match_dup 1) 0)))
   (set (subreg:V2SF (match_dup 4) 0)
        (float_truncate:V2SF (match_dup 2)))
   (set (match_dup 3)
        (unsigned_float:V2DF (subreg:V2DI (match_dup 1) 16)))
   (set (subreg:V2SF (match_dup 4) 8)
        (float_truncate:V2SF (match_dup 3)))
   (set (match_dup 2)
        (unsigned_float:V2DF (subreg:V2DI (match_dup 1) 32)))
   (set (subreg:V2SF (match_dup 4) 16)
        (float_truncate:V2SF (match_dup 2)))
   (set (match_dup 3)
        (unsigned_float:V2DF (subreg:V2DI (match_dup 1) 48)))
   (set (subreg:V2SF (match_dup 4) 24)
        (float_truncate:V2SF (match_dup 3)))
   (set (match_dup 0) (match_dup 4))]
  {
    rtx operand1 = gen_reg_rtx (V8DImode);
    emit_move_insn (operand1, operands[1]);
    operands[1] = operand1;
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (V8SFmode);
  }
)

(define_insn_and_split "fix_truncv8sfv8di2"
  [(set (match_operand:V8DI 0 "register_operand" "=r")
        (fix:V8DI (match_operand:V8SF 1 "register_operand" "r")))
   (clobber (match_scratch:V2DF 2 "=&r"))
   (clobber (match_scratch:V2DF 3 "=&r"))
   (clobber (match_scratch:V8DI 4 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float_extend:V2DF (subreg:V2SF (match_dup 1) 0)))
   (set (subreg:V2DI (match_dup 4) 0)
        (fix:V2DI (match_dup 2)))
   (set (match_dup 3)
        (float_extend:V2DF (subreg:V2SF (match_dup 1) 8)))
   (set (subreg:V2DI (match_dup 4) 16)
        (fix:V2DI (match_dup 3)))
   (set (match_dup 2)
        (float_extend:V2DF (subreg:V2SF (match_dup 1) 16)))
   (set (subreg:V2DI (match_dup 4) 32)
        (fix:V2DI (match_dup 2)))
   (set (match_dup 3)
        (float_extend:V2DF (subreg:V2SF (match_dup 1) 24)))
   (set (subreg:V2DI (match_dup 4) 48)
        (fix:V2DI (match_dup 3)))
   (set (match_dup 0) (match_dup 4))]
  {
    rtx operand1 = gen_reg_rtx (V8SFmode);
    emit_move_insn (operand1, operands[1]);
    operands[1] = operand1;
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (V8DImode);
  }
)

(define_insn_and_split "fixuns_truncv8sfv8di2"
  [(set (match_operand:V8DI 0 "register_operand" "=r")
        (unsigned_fix:V8DI (match_operand:V8SF 1 "register_operand" "r")))
   (clobber (match_scratch:V2DF 2 "=&r"))
   (clobber (match_scratch:V2DF 3 "=&r"))
   (clobber (match_scratch:V8DI 4 "=&r"))]
  ""
  "#"
  ""
  [(set (match_dup 2)
        (float_extend:V2DF (subreg:V2SF (match_dup 1) 0)))
   (set (subreg:V2DI (match_dup 4) 0)
        (unsigned_fix:V2DI (match_dup 2)))
   (set (match_dup 3)
        (float_extend:V2DF (subreg:V2SF (match_dup 1) 8)))
   (set (subreg:V2DI (match_dup 4) 16)
        (unsigned_fix:V2DI (match_dup 3)))
   (set (match_dup 2)
        (float_extend:V2DF (subreg:V2SF (match_dup 1) 16)))
   (set (subreg:V2DI (match_dup 4) 32)
        (unsigned_fix:V2DI (match_dup 2)))
   (set (match_dup 3)
        (float_extend:V2DF (subreg:V2SF (match_dup 1) 24)))
   (set (subreg:V2DI (match_dup 4) 48)
        (unsigned_fix:V2DI (match_dup 3)))
   (set (match_dup 0) (match_dup 4))]
  {
    rtx operand1 = gen_reg_rtx (V8SFmode);
    emit_move_insn (operand1, operands[1]);
    operands[1] = operand1;
    if (GET_CODE (operands[2]) == SCRATCH)
      operands[2] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[3]) == SCRATCH)
      operands[3] = gen_reg_rtx (V2DFmode);
    if (GET_CODE (operands[4]) == SCRATCH)
      operands[4] = gen_reg_rtx (V8DImode);
  }
)


;; V4DF

(define_insn "addv4df3"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (plus:V4DF (match_operand:V4DF 1 "register_operand" "r")
                   (match_operand:V4DF 2 "register_operand" "r")))]
  ""
  "#"
  [(set_attr "type" "madd_fp4")]
)

(define_split
  [(set (match_operand:V4DF 0 "register_operand" "")
        (plus:V4DF (match_operand:V4DF 1 "register_operand" "")
                   (match_operand:V4DF 2 "register_operand" "")))]
  "KV3 && reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (plus:V2DF (subreg:V2DF (match_dup 1) 0)
                   (subreg:V2DF (match_dup 2) 0)))
   (set (subreg:V2DF (match_dup 0) 16)
        (plus:V2DF (subreg:V2DF (match_dup 1) 16)
                   (subreg:V2DF (match_dup 2) 16)))]
  ""
)

(define_split
  [(set (match_operand:V4DF 0 "register_operand" "")
        (plus:V4DF (match_operand:V4DF 1 "register_operand" "")
                   (match_operand:V4DF 2 "register_operand" "")))]
  "KV4 && reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (plus:DF (subreg:DF (match_dup 1) 0)
                 (subreg:DF (match_dup 2) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (plus:DF (subreg:DF (match_dup 1) 8)
                 (subreg:DF (match_dup 2) 8)))
   (set (subreg:DF (match_dup 0) 16)
        (plus:DF (subreg:DF (match_dup 1) 16)
                 (subreg:DF (match_dup 2) 16)))
   (set (subreg:DF (match_dup 0) 24)
        (plus:DF (subreg:DF (match_dup 1) 24)
                 (subreg:DF (match_dup 2) 24)))]
  ""
)

(define_insn "subv4df3"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (minus:V4DF (match_operand:V4DF 1 "register_operand" "r")
                    (match_operand:V4DF 2 "register_operand" "r")))]
  ""
  "#"
  [(set_attr "type" "madd_fp4")]
)

(define_split
  [(set (match_operand:V4DF 0 "register_operand" "")
        (minus:V4DF (match_operand:V4DF 1 "register_operand" "")
                    (match_operand:V4DF 2 "register_operand" "")))]
  "KV3 && reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (minus:V2DF (subreg:V2DF (match_dup 1) 0)
                    (subreg:V2DF (match_dup 2) 0)))
   (set (subreg:V2DF (match_dup 0) 16)
        (minus:V2DF (subreg:V2DF (match_dup 1) 16)
                    (subreg:V2DF (match_dup 2) 16)))]
  ""
)

(define_split
  [(set (match_operand:V4DF 0 "register_operand" "")
        (minus:V4DF (match_operand:V4DF 1 "register_operand" "")
                    (match_operand:V4DF 2 "register_operand" "")))]
  "KV4 && reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (minus:DF (subreg:DF (match_dup 1) 0)
                  (subreg:DF (match_dup 2) 0)))
   (set (subreg:DF (match_dup 0) 8)
        (minus:DF (subreg:DF (match_dup 1) 8)
                  (subreg:DF (match_dup 2) 8)))
   (set (subreg:DF (match_dup 0) 16)
        (minus:DF (subreg:DF (match_dup 1) 16)
                  (subreg:DF (match_dup 2) 16)))
   (set (subreg:DF (match_dup 0) 24)
        (minus:DF (subreg:DF (match_dup 1) 24)
                  (subreg:DF (match_dup 2) 24)))]
  ""
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
  [(set_attr "type" "madd_fp4")]
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
  [(set_attr "type" "madd_fp4")]
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
  [(set_attr "type" "madd_fp4")]
)


;; PACK / UNPACK

(define_insn "kvx_sxmbhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V8QI 1 "register_operand" "r")]
                       UNSPEC_SXMBHQ))]
  ""
  "sxmbhq %0 = %1"
  [(set_attr "type" "alu_thin")
   (set_attr "length" "4")]
)

(define_insn "kvx_sxlbhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V8QI 1 "register_operand" "r")]
                       UNSPEC_SXLBHQ))]
  ""
  "sxlbhq %0 = %1"
  [(set_attr "type" "alu_thin")
   (set_attr "length" "4")]
)

(define_insn "kvx_zxmbhq_2"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V8QI 1 "register_operand" "r")]
                       UNSPEC_ZXMBHQ))]
  "(KV3_2||KV4)"
  "zxmbhq %0 = %1"
  [(set_attr "type" "alu_tiny")
   (set_attr "length" "4")]
)

(define_expand "kvx_zxmbhq"
  [(match_operand:V4HI 0 "register_operand")
   (match_operand:V8QI 1 "register_operand")]
  ""
  {
    if (KV3_1)
      {
        rtx expand_msb = GEN_INT (0x0008000400020001ULL << 4);
        operands[0] = simplify_gen_subreg (DImode, operands[0], V4HImode, 0);
        operands[1] = simplify_gen_subreg (DImode, operands[1], V8QImode, 0);
        emit_insn (gen_kvx_sbmm8d (operands[0], operands[1], expand_msb));
      }
    else
      emit_insn (gen_kvx_zxmbhq_2 (operands[0], operands[1]));
    DONE;
  }
)

(define_insn "kvx_zxlbhq_2"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V8QI 1 "register_operand" "r")]
                       UNSPEC_ZXLBHQ))]
  "(KV3_2||KV4)"
  "zxlbhq %0 = %1"
  [(set_attr "type" "alu_tiny")
   (set_attr "length" "4")]
)

(define_expand "kvx_zxlbhq"
  [(match_operand:V4HI 0 "register_operand")
   (match_operand:V8QI 1 "register_operand")]
  ""
  {
    if (KV3_1)
      {
        rtx expand_lsb = GEN_INT (0x0008000400020001ULL);
        operands[0] = simplify_gen_subreg (DImode, operands[0], V4HImode, 0);
        operands[1] = simplify_gen_subreg (DImode, operands[1], V8QImode, 0);
        emit_insn (gen_kvx_sbmm8d (operands[0], operands[1], expand_lsb));
      }
    else
      emit_insn (gen_kvx_zxlbhq_2 (operands[0], operands[1]));
    DONE;
  }
)

(define_insn "kvx_sxmhwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V4HI 1 "register_operand" "r")]
                       UNSPEC_SXMHWP))]
  ""
  "sxmhwp %0 = %1"
  [(set_attr "type" "alu_thin")
   (set_attr "length" "4")]
)

(define_insn "kvx_sxlhwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V4HI 1 "register_operand" "r")]
                       UNSPEC_SXLHWP))]
  ""
  "sxlhwp %0 = %1"
  [(set_attr "type" "alu_thin")
   (set_attr "length" "4")]
)

(define_insn "kvx_zxmhwp_2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V4HI 1 "register_operand" "r")]
                       UNSPEC_ZXMHWP))]
  "(KV3_2||KV4)"
  "zxmhwp %0 = %1"
  [(set_attr "type" "alu_tiny")
   (set_attr "length" "4")]
)

(define_expand "kvx_zxmhwp"
  [(match_operand:V2SI 0 "register_operand")
   (match_operand:V4HI 1 "register_operand")]
  ""
  {
    if (KV3_1)
      {
        rtx expand_msb = GEN_INT (0x0000080400000201ULL << 4);
        operands[0] = simplify_gen_subreg (DImode, operands[0], V2SImode, 0);
        operands[1] = simplify_gen_subreg (DImode, operands[1], V4HImode, 0);
        emit_insn (gen_kvx_sbmm8d (operands[0], operands[1], expand_msb));
      }
    else
      emit_insn (gen_kvx_zxmhwp_2 (operands[0], operands[1]));
    DONE;
  }
)

(define_insn "kvx_zxlhwp_2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V4HI 1 "register_operand" "r")]
                       UNSPEC_ZXLHWP))]
  "(KV3_2||KV4)"
  "zxlhwp %0 = %1"
  [(set_attr "type" "alu_tiny")
   (set_attr "length" "4")]
)

(define_expand "kvx_zxlhwp"
  [(match_operand:V2SI 0 "register_operand")
   (match_operand:V4HI 1 "register_operand")]
  ""
  {
    if (KV3_1)
      {
        rtx expand_lsb = GEN_INT (0x0000080400000201ULL);
        operands[0] = simplify_gen_subreg (DImode, operands[0], V2SImode, 0);
        operands[1] = simplify_gen_subreg (DImode, operands[1], V4HImode, 0);
        emit_insn (gen_kvx_sbmm8d (operands[0], operands[1], expand_lsb));
      }
    else
      emit_insn (gen_kvx_zxlhwp_2 (operands[0], operands[1]));
    DONE;
  }
)

(define_expand "vec_unpacks_hi_<packi>"
  [(set (match_operand:UNPACKI 0 "register_operand")
        (match_operand:<PACKI> 1 "register_operand"))]
  ""
  {
    kvx_expand_unpack (operands[0], operands[1], /*signed_p*/1, /*hi_p*/1);
    DONE;
  }
)

(define_expand "vec_unpacks_lo_<packi>"
  [(set (match_operand:UNPACKI 0 "register_operand")
        (match_operand:<PACKI> 1 "register_operand"))]
  ""
  {
    kvx_expand_unpack (operands[0], operands[1], /*signed_p*/1, /*hi_p*/0);
    DONE;
  }
)

(define_expand "vec_unpacku_hi_<packi>"
  [(match_operand:UNPACKI 0 "register_operand")
   (match_operand:<PACKI> 1 "register_operand")]
  ""
  {
    kvx_expand_unpack (operands[0], operands[1], /*signed_p*/0, /*hi_p*/1);
    DONE;
  }
)

(define_expand "vec_unpacku_lo_<packi>"
  [(match_operand:UNPACKI 0 "register_operand")
   (match_operand:<PACKI> 1 "register_operand")]
  ""
  {
    kvx_expand_unpack (operands[0], operands[1], /*signed_p*/0, /*hi_p*/0);
    DONE;
  }
)


;; V512G

(define_expand "float<mask><mode>2"
  [(set (match_operand:V512G 0 "register_operand" "")
        (float:V512G (match_operand:<MASK> 1 "register_operand" "")))]
  ""
  {
    rtx operand1 = gen_reg_rtx (<MASK>mode);
    rtx operand0 = gen_reg_rtx (<MODE>mode);
    emit_move_insn (operand1, operands[1]);
    emit_insn (gen_float<hmask><half>2 (gen_rtx_SUBREG (<HALF>mode, operand0, 0),
                                        gen_rtx_SUBREG (<HMASK>mode, operand1, 0)));
    emit_insn (gen_float<hmask><half>2 (gen_rtx_SUBREG (<HALF>mode, operand0, 32),
                                        gen_rtx_SUBREG (<HMASK>mode, operand1, 32)));
    emit_move_insn (operands[0], operand0);
    DONE;
  }
)

(define_expand "floatuns<mask><mode>2"
  [(set (match_operand:V512G 0 "register_operand" "")
        (float:V512G (match_operand:<MASK> 1 "register_operand" "")))]
  ""
  {
    rtx operand1 = gen_reg_rtx (<MASK>mode);
    rtx operand0 = gen_reg_rtx (<MODE>mode);
    emit_move_insn (operand1, operands[1]);
    emit_insn (gen_floatuns<hmask><half>2 (gen_rtx_SUBREG (<HALF>mode, operand0, 0),
                                           gen_rtx_SUBREG (<HMASK>mode, operand1, 0)));
    emit_insn (gen_floatuns<hmask><half>2 (gen_rtx_SUBREG (<HALF>mode, operand0, 32),
                                           gen_rtx_SUBREG (<HMASK>mode, operand1, 32)));
    emit_move_insn (operands[0], operand0);
    DONE;
  }
)

(define_expand "fix_trunc<mode><mask>2"
  [(set (match_operand:<MASK> 0 "register_operand" "")
        (fix:<MASK> (match_operand:V512G 1 "register_operand" "")))]
  ""
  {
    rtx operand1 = gen_reg_rtx (<MODE>mode);
    rtx operand0 = gen_reg_rtx (<MASK>mode);
    emit_move_insn (operand1, operands[1]);
    emit_insn (gen_fix_trunc<half><hmask>2 (gen_rtx_SUBREG (<HMASK>mode, operand0, 0),
                                            gen_rtx_SUBREG (<HALF>mode, operand1, 0)));
    emit_insn (gen_fix_trunc<half><hmask>2 (gen_rtx_SUBREG (<HMASK>mode, operand0, 32),
                                            gen_rtx_SUBREG (<HALF>mode, operand1, 32)));
    emit_move_insn (operands[0], operand0);
    DONE;
  }
)

(define_expand "fixuns_trunc<mode><mask>2"
  [(set (match_operand:<MASK> 0 "register_operand" "")
        (fix:<MASK> (match_operand:V512G 1 "register_operand" "")))]
  ""
  {
    rtx operand1 = gen_reg_rtx (<MODE>mode);
    rtx operand0 = gen_reg_rtx (<MASK>mode);
    emit_move_insn (operand1, operands[1]);
    emit_insn (gen_fixuns_trunc<half><hmask>2 (gen_rtx_SUBREG (<HMASK>mode, operand0, 0),
                                               gen_rtx_SUBREG (<HALF>mode, operand1, 0)));
    emit_insn (gen_fixuns_trunc<half><hmask>2 (gen_rtx_SUBREG (<HMASK>mode, operand0, 32),
                                               gen_rtx_SUBREG (<HALF>mode, operand1, 32)));
    emit_move_insn (operands[0], operand0);
    DONE;
  }
)

