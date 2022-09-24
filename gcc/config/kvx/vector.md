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
  [(set_attr "type" "alu_tiny,lsu_auxw_load,lsu_auxw_load_x,lsu_auxw_load_y,lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y,lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y,alu_tiny,alu_tiny_x,alu_tiny_y")
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
    if (misaligned_0 && MEM_P (operands[1]) || misaligned_1 && MEM_P (operands[0]))
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
  [(const_int 0)]
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
  [(set_attr "type" "alu_tiny_x2,lsu_auxw_load,lsu_auxw_load_x,lsu_auxw_load_y,lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y,lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"         "8,             4,             8,             12,                     4,                       8,                      12,             4,               8,              12")]
)

(define_split
  [(set (match_operand:ALL128 0 "register_operand" "")
        (match_operand:ALL128 1 "register_operand" ""))]
  "reload_completed"
  [(const_int 0)]
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
  [(const_int 0)]
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
    if (misaligned_0 && MEM_P (operands[1]) || misaligned_1 && MEM_P (operands[0]))
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
  [(const_int 0)]
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
  [(set_attr "type" "alu_tiny_x4,lsu_auxw_load,lsu_auxw_load_x,lsu_auxw_load_y,lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y,lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"        "16,            4,              8,             12,                     4,                       8,                      12,             4,               8,              12")]
)

(define_split
  [(set (match_operand:ALL256 0 "register_operand" "")
        (match_operand:ALL256 1 "register_operand" ""))]
  "reload_completed"
  [(const_int 0)]
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
  [(const_int 0)]
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
  [(set (match_operand:ALL512 0 "nonimmediate_operand" "=r,&r,&r,&r,a,b,m")
        (match_operand:ALL512 1 "nonimmediate_operand"  "r, a, b, m,r,r,r"))]
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
  [(const_int 0)]
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

(define_expand "vec_extract<SIMDALL:mode><SIMDALL:inner>"
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

(define_expand "vec_init<SIMDALL:mode><SIMDALL:inner>"
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

(define_expand "vec_duplicate<SIMDALL:mode>"
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

(define_expand "vcond<SIMDCMP:mode><SIMDALL:mode>"
  [(match_operand:SIMDCMP 0 "register_operand")
   (match_operand:SIMDCMP 1 "nonmemory_operand")
   (match_operand:SIMDCMP 2 "nonmemory_operand")
   (match_operator 3 "comparison_operator"
    [(match_operand:SIMDALL 4 "register_operand")
     (match_operand:SIMDALL 5 "reg_or_zero_operand")])]
  "(GET_MODE_NUNITS (<SIMDCMP:MODE>mode) == GET_MODE_NUNITS (<SIMDALL:MODE>mode))"
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

(define_expand "vcondu<SIMDCMP:mode><SIMDALL:mode>"
  [(match_operand:SIMDCMP 0 "register_operand")
   (match_operand:SIMDCMP 1 "nonmemory_operand")
   (match_operand:SIMDCMP 2 "nonmemory_operand")
   (match_operator 3 "comparison_operator"
    [(match_operand:SIMDALL 4 "register_operand")
     (match_operand:SIMDALL 5 "reg_or_zero_operand")])]
  "(GET_MODE_NUNITS (<SIMDCMP:MODE>mode) == GET_MODE_NUNITS (<SIMDALL:MODE>mode))"
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
   (match_operand 2 "sixbits_unsigned_operand" "")]
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

(define_insn "*sbmm8"
  [(set (match_operand:ALL64 0 "register_operand" "=r")
        (unspec:ALL64 [(match_operand:FITGPR 1 "register_operand" "r")
                       (match_operand:DI 2 "register_operand" "r")] UNSPEC_SBMM8))]
  ""
  "sbmm8 %0 = %1, %2"
  [(set_attr "type" "alu_thin")]
)

(define_insn "*sbmm8s"
  [(set (match_operand:ALL64 0 "register_operand" "=r")
        (unspec:ALL64 [(match_operand:FITGPR 1 "register_operand" "r")
                       (match_operand:DI 2 "register_operand" "r")] UNSPEC_SBMM8S))]
  ""
  "sbmm8.@ %0 = %1, %2"
  [(set_attr "type" "alu_thin_x")
   (set_attr "length"        "8")]
)

(define_insn "*sbmm8xy"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (unspec:ALL128 [(match_operand:SIMD128 1 "register_operand" "r")
                        (match_operand:DI 2 "register_operand" "r")
                        (match_operand:DI 3 "register_operand" "r")] UNSPEC_SBMM8XY))]
  ""
  "sbmm8 %x0 = %x1, %2\n\tsbmm8 %y0 = %y1, %3"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)


;; Vector Insert/Extract Helpers

(define_insn "*addd"
  [(set (match_operand:ALL64 0 "register_operand" "=r")
        (unspec:ALL64 [(match_operand:ALL64 1 "register_operand" "r")
                       (match_operand:FITGPR 2 "register_operand" "r")] UNSPEC_ADDD))]
  ""
  "addd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

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
  [(set (match_operand:SIMD128 0 "register_operand" "=&r")
        (vec_duplicate:SIMD128 (match_operand:<CHUNK> 1 "nonmemory_operand" "r")))]
  ""
  "copyd %x0 = %1\n\tcopyd %y0 = %1"
  ""
  [(set (subreg:<CHUNK> (match_dup 0) 0) (match_dup 1))
   (set (subreg:<CHUNK> (match_dup 0) 8) (match_dup 1))]
  ""
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*dup256"
  [(set (match_operand:SIMD256 0 "register_operand" "=&r")
        (vec_duplicate:SIMD256 (match_operand:<CHUNK> 1 "nonmemory_operand" "r")))]
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


;; VXQI (V8QI V16QI V32QI)

(define_insn "*zxebhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V4HI 1 "register_operand" "r")] UNSPEC_ZXEBHQ))]
  ""
  "andd.@ %0 = %1, 0x00FF00FF"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length"        "8")]
)

(define_insn "*zxobhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V4HI 1 "register_operand" "r")] UNSPEC_ZXOBHQ))]
  ""
  "srlhqs %0 = %1, 8"
  [(set_attr "type" "alu_thin")]
)

(define_insn "*qxebhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V4HI 1 "register_operand" "r")] UNSPEC_QXEBHQ))]
  ""
  "sllhqs %0 = %1, 8"
  [(set_attr "type" "alu_thin")]
)

(define_insn "*qxobhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V4HI 1 "register_operand" "r")] UNSPEC_QXOBHQ))]
  ""
  "andd.@ %0 = %1, 0xFF00FF00"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length"        "8")]
)

(define_insn "*oroebo"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V4HI 1 "register_operand" "r")
                      (match_operand:V4HI 2 "register_operand" "r")] UNSPEC_OROEBO))]
  ""
  "ord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn_and_split "*oroebo2_zxebhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(unspec:V4HI [(match_operand:V4HI 1 "register_operand" "r")
                                    (match_operand:V4HI 2 "register_operand" "r")] UNSPEC_OROEBO)] UNSPEC_ZXEBHQ))]
  ""
  "#"
  ""
  [(set (match_dup 0) (match_dup 2))]
  ""
  [(set_attr "type" "alu_tiny")]
)

(define_insn_and_split "*oroebo1_zxobhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(unspec:V4HI [(match_operand:V4HI 1 "register_operand" "r")
                                    (match_operand:V4HI 2 "register_operand" "r")] UNSPEC_OROEBO)] UNSPEC_ZXOBHQ))]
  ""
  "#"
  ""
  [(set (match_dup 0)
        (unspec:V4HI [(match_dup 1)] UNSPEC_ZXOBHQ))]
  ""
  [(set_attr "type" "alu_tiny")]
)

(define_insn_and_split "*oroebo2_qxebhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(unspec:V4HI [(match_operand:V4HI 1 "register_operand" "r")
                                    (match_operand:V4HI 2 "register_operand" "r")] UNSPEC_OROEBO)] UNSPEC_QXEBHQ))]
  ""
  "#"
  ""
  [(set (match_dup 0)
        (unspec:V4HI [(match_dup 2)] UNSPEC_QXEBHQ))]
  ""
  [(set_attr "type" "alu_tiny")]
)

(define_insn_and_split "*oroebo1_qxobhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(unspec:V4HI [(match_operand:V4HI 1 "register_operand" "r")
                                    (match_operand:V4HI 2 "register_operand" "r")] UNSPEC_OROEBO)] UNSPEC_QXOBHQ))]
  ""
  "#"
  ""
  [(set (match_dup 0) (match_dup 1))]
  ""
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
        unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
        for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
          {
            rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MODE>mode, offset);
            rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MODE>mode, offset);
            rtx op1o = gen_reg_rtx (V4HImode), op1e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXEBHQ)));
            rtx op0o = gen_reg_rtx (V4HImode), op0e = gen_reg_rtx (V4HImode);
            emit_insn (gen_<prefix>v4hi2 (op0e, op1e));
            emit_insn (gen_<prefix>v4hi2 (op0o, op1o));
            if (<set8lsb>)
              emit_insn (gen_rtx_SET (op0o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0o), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op0e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0e), UNSPEC_ZXOBHQ)));
            emit_insn (gen_rtx_SET (op0, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op0o, op0e), UNSPEC_OROEBO)));
          }
        DONE;
      }
  }
)
(define_insn "*<prefix>v8qi2_2"
  [(set (match_operand:V8QI 0 "register_operand" "=r")
        (UNARITH:V8QI (match_operand:V8QI 1 "register_operand" "r")))]
  "KV3_2"
  "<stem>bo %0 = %1"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length"        "8")]
)
(define_insn "*<prefix>v16qi2_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (UNARITH:V16QI (match_operand:V16QI 1 "register_operand" "r")))]
  "KV3_2"
  "<stem>bo %x0 = %x1\n\t<stem>bo %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2_x")
   (set_attr "length"          "16")]
)
(define_insn "*<prefix>v32qi2_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (UNARITH:V32QI (match_operand:V32QI 1 "register_operand" "r")))]
  "KV3_2"
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
        unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
        for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
          {
            rtx op2 = simplify_gen_subreg (V4HImode, operands[2], <MODE>mode, offset);
            rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MODE>mode, offset);
            rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MODE>mode, offset);
            rtx op2o = gen_reg_rtx (V4HImode), op2e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op2o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op2), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op2e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op2), UNSPEC_QXEBHQ)));
            rtx op1o = gen_reg_rtx (V4HImode), op1e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXEBHQ)));
            rtx op0o = gen_reg_rtx (V4HImode), op0e = gen_reg_rtx (V4HImode);
            emit_insn (gen_<prefix>v4hi3 (op0o, op1o, op2o));
            emit_insn (gen_<prefix>v4hi3 (op0e, op1e, op2e));
            if (<set8lsb>)
              emit_insn (gen_rtx_SET (op0o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0o), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op0e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0e), UNSPEC_ZXOBHQ)));
            emit_insn (gen_rtx_SET (op0, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op0o, op0e), UNSPEC_OROEBO)));
          }
        DONE;
      }
  }
)
(define_insn "*<prefix>v8qi3_2"
  [(set (match_operand:V8QI 0 "register_operand" "=r")
        (BINARITHC:V8QI (match_operand:V8QI 1 "register_operand" "r")
                        (match_operand:V8QI 2 "register_operand" "r")))]
  "KV3_2"
  "<stem>bo %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)
(define_insn "*<prefix>v16qi3_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINARITHC:V16QI (match_operand:V16QI 1 "register_operand" "r")
                         (match_operand:V16QI 2 "register_operand" "r")))]
  "KV3_2"
  "<stem>bo %x0 = %x1, %x2\n\t<stem>bo %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<prefix>v16qi3_s1"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINARITHC:V16QI (vec_duplicate:V16QI (match_operand:V8QI 1 "nonmemory_operand" "r"))
                         (match_operand:V16QI 2 "register_operand" "r")))]
  "KV3_2"
  "<stem>bo %x0 = %1, %x2\n\t<stem>bo %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<prefix>v16qi3_s2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINARITHC:V16QI (match_operand:V16QI 1 "register_operand" "r")
                         (vec_duplicate:V16QI (match_operand:V8QI 2 "nonmemory_operand" "r"))))]
  "KV3_2"
  "<stem>bo %x0 = %x1, %2\n\t<stem>bo %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<prefix>v32qi3_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (BINARITHC:V32QI (match_operand:V32QI 1 "register_operand" "r")
                         (match_operand:V32QI 2 "register_operand" "r")))]
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
  "<stem>bo %0 = %2, %1"
  [(set_attr "type" "alu_tiny")]
)
(define_insn "*<prefix>v16qi3_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINMINUS:V16QI (match_operand:V16QI 1 "register_operand" "r")
                        (match_operand:V16QI 2 "register_operand" "r")))]
  "KV3_2"
  "<stem>bo %x0 = %x2, %x1\n\t<stem>bo %y0 = %y2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<prefix>v16qi3_s1"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINMINUS:V16QI (vec_duplicate:V16QI (match_operand:V8QI 1 "nonmemory_operand" "r"))
                        (match_operand:V16QI 2 "register_operand" "r")))]
  "KV3_2"
  "<stem>bo %x0 = %x2, %1\n\t<stem>bo %y0 = %y2, %1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<prefix>v16qi3_s2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (BINMINUS:V16QI (match_operand:V16QI 1 "register_operand" "r")
                        (vec_duplicate:V16QI (match_operand:V8QI 2 "nonmemory_operand" "r"))))]
  "KV3_2"
  "<stem>bo %x0 = %2, %x1\n\t<stem>bo %y0 = %2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<prefix>v32qi3_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (BINMINUS:V32QI (match_operand:V32QI 1 "register_operand" "r")
                        (match_operand:V32QI 2 "register_operand" "r")))]
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx op2 = simplify_gen_subreg (V4HImode, operands[2], <MODE>mode, offset);
        rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MODE>mode, offset);
        rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MODE>mode, offset);
        rtx op2o = gen_reg_rtx (V4HImode), op2e = op2;
        emit_insn (gen_rtx_SET (op2o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op2), UNSPEC_ZXOBHQ)));
        rtx op1o = gen_reg_rtx (V4HImode), op1e = op1;
        emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXOBHQ)));
        rtx op0o = gen_reg_rtx (V4HImode), op0e = gen_reg_rtx (V4HImode);
        emit_insn (gen_mulv4hi3 (op0o, op1o, op2o));
        emit_insn (gen_mulv4hi3 (op0e, op1e, op2e));
        emit_insn (gen_rtx_SET (op0e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0e), UNSPEC_ZXEBHQ)));
        emit_insn (gen_rtx_SET (op0, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op0o, op0e), UNSPEC_OROEBO)));
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
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx op2 = simplify_gen_subreg (V4HImode, operands[2], <MODE>mode, offset);
        rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MODE>mode, offset);
        rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MODE>mode, offset);
        rtx op2o = gen_reg_rtx (V4HImode), op2e = gen_reg_rtx (V4HImode);
        emit_insn (gen_rtx_SET (op2o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op2), UNSPEC_QXOBHQ)));
        emit_insn (gen_rtx_SET (op2e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op2), UNSPEC_QXEBHQ)));
        rtx op1o = gen_reg_rtx (V4HImode), op1e = gen_reg_rtx (V4HImode);
        emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXOBHQ)));
        emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXEBHQ)));
        rtx op0o = gen_reg_rtx (V4HImode), op0e = gen_reg_rtx (V4HImode);
        emit_insn (gen_<prefix>v4hi3 (op0o, op1o, op2o));
        emit_insn (gen_<prefix>v4hi3 (op0e, op1e, op2e));
        emit_insn (gen_rtx_SET (op0o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0o), UNSPEC_QXEBHQ)));
        if (<set8msb>)
          emit_insn (gen_rtx_SET (op0e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0e), UNSPEC_ZXEBHQ)));
        emit_insn (gen_rtx_SET (op0, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op0o, op0e), UNSPEC_OROEBO)));
      }
    DONE;
  }
)

(define_expand "<prefix><mode>3"
  [(set (match_operand:VXQI 0 "register_operand" "")
        (BINMOD:VXQI (match_operand:VXQI 1 "register_operand" "")
                       (match_operand:VXQI 2 "register_operand" "")))]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx op2 = simplify_gen_subreg (V4HImode, operands[2], <MODE>mode, offset);
        rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MODE>mode, offset);
        rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MODE>mode, offset);
        rtx op2o = gen_reg_rtx (V4HImode), op2e = gen_reg_rtx (V4HImode);
        emit_insn (gen_rtx_SET (op2o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op2), UNSPEC_QXOBHQ)));
        emit_insn (gen_rtx_SET (op2e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op2), UNSPEC_QXEBHQ)));
        rtx op1o = gen_reg_rtx (V4HImode), op1e = gen_reg_rtx (V4HImode);
        emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXOBHQ)));
        emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXEBHQ)));
        rtx op0o = gen_reg_rtx (V4HImode), op0e = gen_reg_rtx (V4HImode);
        emit_insn (gen_<prefix>v4hi3 (op0o, op1o, op2o));
        emit_insn (gen_<prefix>v4hi3 (op0e, op1e, op2e));
        emit_insn (gen_rtx_SET (op0e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0e), UNSPEC_ZXOBHQ)));
        emit_insn (gen_rtx_SET (op0, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op0o, op0e), UNSPEC_OROEBO)));
      }
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
        unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
        for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
          {
            rtx op2 = simplify_gen_subreg (V4HImode, operands[2], <MODE>mode, offset);
            rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MODE>mode, offset);
            rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MODE>mode, offset);
            rtx op2o = gen_reg_rtx (V4HImode), op2e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op2o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op2), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op2e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op2), UNSPEC_QXEBHQ)));
            rtx op1o = gen_reg_rtx (V4HImode), op1e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXEBHQ)));
            rtx op0o = gen_reg_rtx (V4HImode), op0e = gen_reg_rtx (V4HImode);
            emit_insn (gen_<MINUS:abdm>v4hi3 (op0o, op1o, op2o));
            emit_insn (gen_<MINUS:abdm>v4hi3 (op0e, op1e, op2e));
            if (<set8lsb>)
              emit_insn (gen_rtx_SET (op0o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0o), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op0e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0e), UNSPEC_ZXOBHQ)));
            emit_insn (gen_rtx_SET (op0, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op0o, op0e), UNSPEC_OROEBO)));
          }
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
        unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
        for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
          {
            rtx op2 = simplify_gen_subreg (V4HImode, operands[2], <MODE>mode, offset);
            rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MODE>mode, offset);
            rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MODE>mode, offset);
            rtx op2o = gen_reg_rtx (V4HImode), op2e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op2o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op2), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op2e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op2), UNSPEC_QXEBHQ)));
            rtx op1o = gen_reg_rtx (V4HImode), op1e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXEBHQ)));
            rtx op0o = gen_reg_rtx (V4HImode), op0e = gen_reg_rtx (V4HImode);
            emit_insn (gen_abduv4hi3 (op0o, op1o, op2o));
            emit_insn (gen_abduv4hi3 (op0e, op1e, op2e));
            emit_insn (gen_rtx_SET (op0e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0e), UNSPEC_ZXOBHQ)));
            emit_insn (gen_rtx_SET (op0, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op0o, op0e), UNSPEC_OROEBO)));
          }
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
        unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
        rtx bias =  gen_reg_rtx (DImode);
        if (<avground>)
          emit_insn (gen_rtx_SET (bias, GEN_INT (0x00FF00FF00FF00FF)));
        for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
          {
            rtx op2 = simplify_gen_subreg (V4HImode, operands[2], <MODE>mode, offset);
            rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MODE>mode, offset);
            rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MODE>mode, offset);
            rtx op2o = gen_reg_rtx (V4HImode), op2e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op2o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op2), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op2e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op2), UNSPEC_QXEBHQ)));
            if (<avground>)
              emit_insn (gen_rtx_SET (op2o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op2o, bias), UNSPEC_ADDD)));
            if (<avground>)
              emit_insn (gen_rtx_SET (op2e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op2e, bias), UNSPEC_ADDD)));
            rtx op1o = gen_reg_rtx (V4HImode), op1e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXEBHQ)));
            rtx op0o = gen_reg_rtx (V4HImode), op0e = gen_reg_rtx (V4HImode);
            emit_insn (gen_<avgpre>v4hi<avgpost> (op0o, op1o, op2o));
            emit_insn (gen_<avgpre>v4hi<avgpost> (op0e, op1e, op2e));
            emit_insn (gen_rtx_SET (op0o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0o), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op0e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0e), UNSPEC_ZXOBHQ)));
            emit_insn (gen_rtx_SET (op0, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op0o, op0e), UNSPEC_OROEBO)));
          }
        DONE;
      }
  }
)
(define_insn "*<avgpre>v8qi<avgpost>_2"
  [(set (match_operand:V8QI 0 "register_operand" "=r")
        (unspec:V8QI [(match_operand:V8QI 1 "register_operand" "r")
                      (match_operand:V8QI 2 "register_operand" "r")] UNSPEC_AVGI))]
  "KV3_2"
  "<avgm>bo %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)
(define_insn "*<avgpre>v16qi<avgpost>_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (unspec:V16QI [(match_operand:V16QI 1 "register_operand" "r")
                       (match_operand:V16QI 2 "register_operand" "r")] UNSPEC_AVGI))]
  "KV3_2"
  "<avgm>bo %x0 = %x1, %x2\n\t<avgm>bo %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*<avgpre>v32qi<avgpost>_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (unspec:V32QI [(match_operand:V32QI 1 "register_operand" "r")
                       (match_operand:V32QI 2 "register_operand" "r")] UNSPEC_AVGI))]
  "KV3_2"
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
                     (match_operand:SI 2 "register_operand" "")))]
  ""
  {
    if (KV3_1)
      {
        unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
        for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
          {
            rtx op2 = gen_reg_rtx (SImode);
            emit_insn (gen_andsi3 (op2, operands[2], GEN_INT (0x7)));
            rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MODE>mode, offset);
            rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MODE>mode, offset);
            rtx op1o = gen_reg_rtx (V4HImode), op1e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXEBHQ)));
            rtx op0o = gen_reg_rtx (V4HImode), op0e = gen_reg_rtx (V4HImode);
            emit_insn (gen_<prefix>v4hi3 (op0o, op1o, op2));
            emit_insn (gen_<prefix>v4hi3 (op0e, op1e, op2));
            emit_insn (gen_rtx_SET (op0e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0e), UNSPEC_ZXOBHQ)));
            if (<set8lsb>)
              emit_insn (gen_rtx_SET (op0o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0o), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op0, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op0o, op0e), UNSPEC_OROEBO)));
          }
        DONE;
      }
  }
)
(define_insn "*<prefix>v8qi3_2"
  [(set (match_operand:V8QI 0 "register_operand" "=r,r")
        (BINSHLRT:V8QI (match_operand:V8QI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  "<stem>bos %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)
(define_insn "*<prefix>v16qi3_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r,r")
        (BINSHLRT:V16QI (match_operand:V16QI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  "<stem>bos %x0 = %x1, %2\n\t<stem>bos %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2,alu_tiny_x2")
   (set_attr "length" "        8,          8")]
)
(define_insn "*<prefix>v32qi3_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r,r")
        (BINSHLRT:V32QI (match_operand:V32QI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  {
    return "<stem>bos %x0 = %x1, %2\n\t<stem>bos %y0 = %y1, %2\n\t"
           "<stem>bos %z0 = %z1, %2\n\t<stem>bos %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4,alu_tiny_x4")
   (set_attr "length" "       16,         16")]
)
;; Special case for LITE.
(define_insn "*<prefix>v8qi3_2"
  [(set (match_operand:V8QI 0 "register_operand" "=r,r")
        (BINSHLRL:V8QI (match_operand:V8QI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  "<stem>bos %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)
(define_insn "*<prefix>v16qi3_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r,r")
        (BINSHLRL:V16QI (match_operand:V16QI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  "<stem>bos %x0 = %x1, %2\n\t<stem>bos %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)
(define_insn_and_split "*<prefix>v32qi3_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r,r")
        (BINSHLRL:V32QI (match_operand:V32QI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  "#"
  "KV3_2 && reload_completed"
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
                     (match_operand:SI 2 "register_operand" "")))]
  ""
  {
    if (KV3_1)
      {
        rtx const8 = GEN_INT (8);
        unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
        for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
          {
            rtx op2 = gen_reg_rtx (SImode), op2p8 = gen_reg_rtx (SImode);
            emit_insn (gen_andsi3 (op2, operands[2], GEN_INT (0x7)));
            emit_insn (gen_addsi3 (op2p8, op2, const8));
            rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MODE>mode, offset);
            rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MODE>mode, offset);
            rtx op1o = gen_reg_rtx (V4HImode), op1e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXEBHQ)));
            rtx op0o = gen_reg_rtx (V4HImode), op0e = gen_reg_rtx (V4HImode);
            emit_insn (gen_<prefix>v4hi3 (op0o, op1o, op2));
            emit_insn (gen_<prefix>v4hi3 (op0e, op1e, op2p8));
            emit_insn (gen_rtx_SET (op0o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0o), UNSPEC_QXOBHQ)));
            if (<set8msb>)
              emit_insn (gen_rtx_SET (op0e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0e), UNSPEC_ZXEBHQ)));
            emit_insn (gen_rtx_SET (op0, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op0o, op0e), UNSPEC_OROEBO)));
          }
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
        unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
        for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
          {
            rtx op2 = gen_reg_rtx (SImode), op2p8 = gen_reg_rtx (SImode);
            emit_insn (gen_andsi3 (op2, operands[2], GEN_INT (0x7)));
            emit_insn (gen_addsi3 (op2p8, op2, const8));
            rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MODE>mode, offset);
            rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MODE>mode, offset);
            rtx op1o = gen_reg_rtx (V4HImode), op1e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op1), UNSPEC_QXEBHQ)));
            rtx op0o = gen_reg_rtx (V4HImode), op0e = gen_reg_rtx (V4HImode);
            emit_insn (gen_sshrv4hi3 (op0o, op1o, op2p8));
            emit_insn (gen_sshrv4hi3 (op0e, op1e, op2p8));
            emit_insn (gen_rtx_SET (op0o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0o), UNSPEC_QXEBHQ)));
            emit_insn (gen_rtx_SET (op0e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0e), UNSPEC_ZXEBHQ)));
            emit_insn (gen_rtx_SET (op0, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op0o, op0e), UNSPEC_OROEBO)));
          }
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
  [(set (match_operand:V8QI 0 "register_operand" "=r,r")
        (unspec:V8QI [(match_operand:V8QI 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRS))]
  "KV3_2"
  "srsbos %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)
(define_insn "*sshrv16qi_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r,r")
        (unspec:V16QI [(match_operand:V16QI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRS))]
  "KV3_2"
  "srsbos %x0 = %x1, %2\n\tsrsbos %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)
(define_insn_and_split "*sshrv32qi_2"
  [(set (match_operand:V32QI 0 "register_operand" "=&r,r")
        (unspec:V32QI [(match_operand:V32QI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRS))]
  "KV3_2"
  "#"
  "KV3_2 && reload_completed"
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
        unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
        rtx evenbmm = gen_reg_rtx (DImode), oddbmm = gen_reg_rtx (DImode);
        emit_insn (gen_rtx_SET (evenbmm, GEN_INT (0x4040101004040101)));
        emit_insn (gen_rtx_SET (oddbmm, GEN_INT (0x8080202008080202)));
        for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
          {
            rtx op2 = gen_reg_rtx (SImode);
            emit_insn (gen_andsi3 (op2, operands[2], GEN_INT (0x7)));
            rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MODE>mode, offset);
            rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MODE>mode, offset);
            rtx op1o = gen_reg_rtx (V4HImode), op1e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op1, oddbmm), UNSPEC_SBMM8)));
            emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op1, evenbmm), UNSPEC_SBMM8)));
            rtx op0o = gen_reg_rtx (V4HImode), op0e = gen_reg_rtx (V4HImode);
            emit_insn (gen_ashlv4hi3 (op0o, op1o, op2));
            emit_insn (gen_ashlv4hi3 (op0e, op1e, op2));
            emit_insn (gen_rtx_SET (op0o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0o), UNSPEC_QXOBHQ)));
            emit_insn (gen_rtx_SET (op0e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0e), UNSPEC_ZXOBHQ)));
            emit_insn (gen_rtx_SET (op0, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op0o, op0e), UNSPEC_OROEBO)));
          }
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
        unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
        rtx evenbmm = gen_reg_rtx (DImode), oddbmm = gen_reg_rtx (DImode);
        emit_insn (gen_rtx_SET (evenbmm, GEN_INT (0x4040101004040101)));
        emit_insn (gen_rtx_SET (oddbmm, GEN_INT (0x8080202008080202)));
        for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
          {
            rtx op2 = gen_reg_rtx (SImode);
            emit_insn (gen_andsi3 (op2, operands[2], GEN_INT (0x7)));
            rtx op1 = simplify_gen_subreg (V4HImode, operands[1], <MODE>mode, offset);
            rtx op0 = simplify_gen_subreg (V4HImode, operands[0], <MODE>mode, offset);
            rtx op1o = gen_reg_rtx (V4HImode), op1e = gen_reg_rtx (V4HImode);
            emit_insn (gen_rtx_SET (op1o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op1, oddbmm), UNSPEC_SBMM8)));
            emit_insn (gen_rtx_SET (op1e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op1, evenbmm), UNSPEC_SBMM8)));
            rtx op0o = gen_reg_rtx (V4HImode), op0e = gen_reg_rtx (V4HImode);
            emit_insn (gen_lshrv4hi3 (op0o, op1o, op2));
            emit_insn (gen_lshrv4hi3 (op0e, op1e, op2));
            emit_insn (gen_rtx_SET (op0o, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0o), UNSPEC_QXEBHQ)));
            emit_insn (gen_rtx_SET (op0e, gen_rtx_UNSPEC (V4HImode, gen_rtvec (1, op0e), UNSPEC_ZXEBHQ)));
            emit_insn (gen_rtx_SET (op0, gen_rtx_UNSPEC (V4HImode, gen_rtvec (2, op0o, op0e), UNSPEC_OROEBO)));
          }
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
    else if (KV3_2)
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
  "KV3_2"
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
    else if (KV3_2)
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
  "KV3_2"
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
  [(set_attr "type" "mau")]
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


;; S64L

(define_insn "and<mode>3"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (and:S64L (match_operand:S64L 1 "register_operand" "r")
                  (match_operand:S64L 2 "register_operand" "r")))]
  ""
  "andd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*nand<mode>3"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (ior:S64L (not:S64L (match_operand:S64L 1 "register_operand" "r"))
                  (not:S64L (match_operand:S64L 2 "register_operand" "r"))))]
  ""
  "nandd %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*andn<mode>3"
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

(define_insn "*nior<mode>3"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (and:S64L (not:S64L (match_operand:S64L 1 "register_operand" "r"))
                  (not:S64L (match_operand:S64L 2 "register_operand" "r"))))]
  ""
  "nord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*iorn<mode>3"
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

(define_insn "*nxor<mode>3"
  [(set (match_operand:S64L 0 "register_operand" "=r")
        (not:S64L (xor:S64L (match_operand:S64L 1 "register_operand" "r")
                            (match_operand:S64L 2 "register_operand" "r"))))]
  ""
  "nxord %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)


;; S64I

(define_insn "madd<mode><mode>4"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (plus:S64I (mult:S64I (match_operand:S64I 1 "register_operand" "r")
                              (match_operand:S64I 2 "register_operand" "r"))
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
  [(set_attr "type" "alu_thin,alu_thin")
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

(define_expand "usashl<mode>3"
  [(match_operand:S64I 0 "register_operand" "")
   (match_operand:S64I 1 "register_operand" "")
   (match_operand:SI 2 "sat_shift_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usashl<mode>3_1 (operands[0], operands[1], operands[2]));
    else if (KV3_2)
      emit_insn (gen_usashl<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usashl<mode>3_1"
  [(set (match_operand:S64I 0 "register_operand" "=r,r")
        (us_ashift:S64I (match_operand:S64I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:S64I 3 "=&r,&r"))
   (clobber (match_scratch:S64I 4 "=&r,&r"))
   (clobber (match_scratch:S64I 5 "=&r,&r"))]
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
  [(set_attr "type" "alu_lite,alu_lite")]
)

(define_insn "usashl<mode>3_2"
  [(set (match_operand:S64I 0 "register_operand" "=r,r")
        (us_ashift:S64I (match_operand:S64I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  "slus<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "ashr<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r")
        (ashiftrt:S64I (match_operand:S64I 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sra<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin")
   (set_attr "length" "     4,       4")]
)

(define_insn "lshr<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r")
        (lshiftrt:S64I (match_operand:S64I 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srl<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_thin,alu_thin")
   (set_attr "length" "     4,       4")]
)

(define_insn "sshr<mode>3"
  [(set (match_operand:S64I 0 "register_operand" "=r,r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRS))]
  ""
  "srs<suffix>s %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
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
  "KV3_2"
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
    else if (KV3_2)
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
    else if (KV3_2)
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
  "KV3_2"
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
    else if (KV3_2)
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
  "KV3_2"
  "abdu<suffix> %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_mul<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                     (sign_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r"))))]
  ""
  "mul<widenx> %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "kvx_mulu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (mult:<WIDE> (zero_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                     (zero_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r"))))]
  ""
  "mulu<widenx> %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "kvx_mulsu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                     (zero_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r"))))]
  ""
  "mulsu<widenx> %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "kvx_madd<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (plus:<WIDE> (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                                  (sign_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r")))
                     (match_operand:<WIDE> 3 "register_operand" "0")))]
  ""
  "madd<widenx> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "kvx_maddu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (plus:<WIDE> (mult:<WIDE> (zero_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                                  (zero_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r")))
                     (match_operand:<WIDE> 3 "register_operand" "0")))]
  ""
  "maddu<widenx> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "kvx_maddsu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (plus:<WIDE> (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                                  (zero_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r")))
                     (match_operand:<WIDE> 3 "register_operand" "0")))]
  ""
  "maddsu<widenx> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "kvx_msbf<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (minus:<WIDE> (match_operand:<WIDE> 3 "register_operand" "0")
                      (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                                   (sign_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r")))))]
  ""
  "msbf<widenx> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "kvx_msbfu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (minus:<WIDE> (match_operand:<WIDE> 3 "register_operand" "0")
                      (mult:<WIDE> (zero_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                                   (zero_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r")))))]
  ""
  "msbfu<widenx> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "kvx_msbfsu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (minus:<WIDE> (match_operand:<WIDE> 3 "register_operand" "0")
                      (mult:<WIDE> (sign_extend:<WIDE> (match_operand:S64I 1 "register_operand" "r"))
                                   (zero_extend:<WIDE> (match_operand:S64I 2 "register_operand" "r")))))]
  ""
  "msbfsu<widenx> %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
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
                                                (match_operand:V4HI 2 "register_operand" "r")))
                   (match_operand:V4SI 3 "register_operand" "0")))]
  ""
  "maddhwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "umaddv4hiv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (zero_extend:V4SI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                                                (match_operand:V4HI 2 "register_operand" "r")))
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


;; S128I (V8HI V4SI)

(define_insn "ashl<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (ashift:S128I (match_operand:S128I 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sll<chunkxs> %x0 = %x1, %2\n\tsll<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_thin_x2,alu_thin_x2")
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

(define_expand "usashl<mode>3"
  [(match_operand:S128I 0 "register_operand" "")
   (match_operand:S128I 1 "register_operand" "")
   (match_operand:SI 2 "sat_shift_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usashl<mode>3_1 (operands[0], operands[1], operands[2]));
    else if (KV3_2)
      emit_insn (gen_usashl<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usashl<mode>3_1"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (us_ashift:S128I (match_operand:S128I 1 "register_operand" "r,r")
                         (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:S128I 3 "=&r,&r"))
   (clobber (match_scratch:S128I 4 "=&r,&r"))
   (clobber (match_scratch:S128I 5 "=&r,&r"))]
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
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn "usashl<mode>3_2"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (us_ashift:S128I (match_operand:S128I 1 "register_operand" "r,r")
                         (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  "slus<chunkxs> %x0 = %x1, %2\n\tslus<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "ashr<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (ashiftrt:S128I (match_operand:S128I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sra<chunkxs> %x0 = %x1, %2\n\tsra<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_thin_x2,alu_thin_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "lshr<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (lshiftrt:S128I (match_operand:S128I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srl<chunkxs> %x0 = %x1, %2\n\tsrl<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_thin_x2,alu_thin_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "sshr<mode>3"
  [(set (match_operand:S128I 0 "register_operand" "=r,r")
        (unspec:S128I [(match_operand:S128I 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRS))]
  ""
  "srs<chunkxs> %x0 = %x1, %2\n\tsrs<chunkxs> %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
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

(define_insn_and_split "kvx_mul<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
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

(define_insn_and_split "kvx_mulu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
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

(define_insn_and_split "kvx_mulsu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
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

(define_insn_and_split "kvx_madd<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
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

(define_insn_and_split "kvx_maddu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
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

(define_insn_and_split "kvx_maddsu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
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

(define_insn_and_split "kvx_msbf<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
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

(define_insn_and_split "kvx_msbfu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
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

(define_insn_and_split "kvx_msbfsu<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
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
    else if (KV3_2)
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
  "KV3_2"
  "addus<chunkx> %x0 = %x1, %x2\n\taddus<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*usadd<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_plus:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                       (match_operand:V128J 2 "register_operand" "r")))]
  "KV3_2"
  "addus<chunkx> %x0 = %1, %x2\n\taddus<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*usadd<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_plus:V128J (match_operand:V128J 1 "register_operand" "r")
                       (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  "KV3_2"
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
    else if (KV3_2)
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
  "KV3_2"
  "sbfus<chunkx> %x0 = %x2, %x1\n\tsbfus<chunkx> %y0 = %y2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*ussub<mode>3_s1"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_minus:V128J (vec_duplicate:V128J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                        (match_operand:V128J 2 "register_operand" "r")))]
  "KV3_2"
  "sbfus<chunkx> %x0 = %x2, %1\n\tsbfus<chunkx> %y0 = %y2, %1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*ussub<mode>3_s2"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (us_minus:V128J (match_operand:V128J 1 "register_operand" "r")
                        (vec_duplicate:V128J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  "KV3_2"
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

(define_insn "*nand<mode>3"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (ior:V128L (not:V128L (match_operand:V128L 1 "register_operand" "r"))
                   (not:V128L (match_operand:V128L 2 "register_operand" "r"))))]
  ""
  "nandd %x0 = %x1, %x2\n\tnandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*andn<mode>3"
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

(define_insn "*nior<mode>3"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (and:V128L (not:V128L (match_operand:V128L 1 "register_operand" "r"))
                   (not:V128L (match_operand:V128L 2 "register_operand" "r"))))]
  ""
  "nord %x0 = %x1, %x2\n\tnord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*iorn<mode>3"
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

(define_insn "*nxor<mode>3"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (not:V128L (xor:V128L (match_operand:V128L 1 "register_operand" "r")
                              (match_operand:V128L 2 "register_operand" "r"))))]
  ""
  "nxord %x0 = %x1, %x2\n\tnxord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)


;; V128J

(define_insn_and_split "madd<mode><mode>4"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (plus:V128J (mult:V128J (match_operand:V128J 1 "register_operand" "r")
                                (match_operand:V128J 2 "register_operand" "r"))
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
  "KV3_2"
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
    else if (KV3_2)
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
    else if (KV3_2)
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
  "KV3_2"
  "abds<chunkx> %x0 = %x1, %x2\n\tabds<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*abds<suffix>_s1"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (ss_minus:V128L (smax:V128L (vec_duplicate:V128L (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                                    (match_operand:V128L 2 "register_operand" "r"))
                        (smin:V128L (vec_duplicate:V128L (match_dup 1)) (match_dup 2))))]
  "KV3_2"
  "abds<chunkx> %x0 = %1, %x2\n\tabds<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*abds<suffix>_s2"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (ss_minus:V128L (smax:V128L (match_operand:V128L 1 "register_operand" "r")
                                    (vec_duplicate:V128L (match_operand:<CHUNK> 2 "nonmemory_operand" "r")))
                        (smin:V128L (match_dup 1) (vec_duplicate:V128L (match_dup 2)))))]
  "KV3_2"
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
    else if (KV3_2)
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
  "KV3_2"
  "abdu<chunkx> %x0 = %x1, %x2\n\tabdu<chunkx> %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*abdu<suffix>_s1"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (minus:V128L (umax:V128L (vec_duplicate:V128L (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                                 (match_operand:V128L 2 "register_operand" "r"))
                     (umin:V128L (vec_duplicate:V128L (match_dup 1)) (match_dup 2))))]
  "KV3_2"
  "abdu<chunkx> %x0 = %1, %x2\n\tabdu<chunkx> %y0 = %1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*abdu<suffix>_s2"
  [(set (match_operand:V128L 0 "register_operand" "=r")
        (minus:V128L (umax:V128L (match_operand:V128L 1 "register_operand" "r")
                                 (vec_duplicate:V128L (match_operand:<CHUNK> 2 "nonmemory_operand" "r")))
                     (umin:V128L (match_dup 1) (vec_duplicate:V128L (match_dup 2)))))]
  "KV3_2"
  "abdu<chunkx> %x0 = %x1, %2\n\tabdu<chunkx> %y0 = %y1, %2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
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
  [(set_attr "type" "mau_auxr")]
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
  [(set_attr "type" "mau_auxr")]
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


;; V4SI

(define_insn "mulv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                   (match_operand:V4SI 2 "register_operand" "r")))]
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

(define_expand "usashlv2di3"
  [(match_operand:V2DI 0 "register_operand" "")
   (match_operand:V2DI 1 "register_operand" "")
   (match_operand:SI 2 "sat_shift_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usashlv2di3_1 (operands[0], operands[1], operands[2]));
    else if (KV3_2)
      emit_insn (gen_usashlv2di3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usashlv2di3_1"
  [(set (match_operand:V2DI 0 "register_operand" "=r,r")
        (us_ashift:V2DI (match_operand:V2DI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:V2DI 3 "=&r,&r"))
   (clobber (match_scratch:V2DI 4 "=&r,&r"))
   (clobber (match_scratch:V2DI 5 "=&r,&r"))]
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
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn "usashlv2di3_2"
  [(set (match_operand:V2DI 0 "register_operand" "=r,r")
        (us_ashift:V2DI (match_operand:V2DI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  "slusd %x0 = %x1, %2\n\tslusd %y0 = %y1, %2"
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

(define_insn "sshrv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r,r")
        (unspec:V2DI [(match_operand:V2DI 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRS))]
  ""
  "srsd %x0 = %x1, %2\n\tsrsd %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)


;; S256I (V16HI V8SI)

(define_expand "ashl<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "")
        (ashift:S256I (match_operand:S256I 1 "register_operand" "")
                      (match_operand:SI 2 "sat_shift_operand" "")))]
  ""
  ""
)

(define_insn_and_split "ashl<mode>3_1"
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (ashift:S256I (match_operand:S256I 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
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
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (ashift:S256I (match_operand:S256I 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  {
    return "sll<chunkxs> %x0 = %x1, %2\n\tsll<chunkxs> %y0 = %y1, %2\n\t"
           "sll<chunkxs> %z0 = %z1, %2\n\tsll<chunkxs> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4,alu_tiny_x4")
   (set_attr "length" "       16,         16")]
)

(define_insn_and_split "ssashl<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (ss_ashift:S256I (match_operand:S256I 1 "register_operand" "r,r")
                         (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
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
   (match_operand:SI 2 "sat_shift_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usashl<mode>3_1 (operands[0], operands[1], operands[2]));
    else if (KV3_2)
      emit_insn (gen_usashl<mode>3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usashl<mode>3_1"
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (us_ashift:S256I (match_operand:S256I 1 "register_operand" "r,r")
                         (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:S256I 3 "=&r,&r"))
   (clobber (match_scratch:S256I 4 "=&r,&r"))
   (clobber (match_scratch:S256I 5 "=&r,&r"))]
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
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn_and_split "usashl<mode>3_2"
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (us_ashift:S256I (match_operand:S256I 1 "register_operand" "r,r")
                         (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  "#"
  "KV3_2 && reload_completed"
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
                        (match_operand:SI 2 "sat_shift_operand" "")))]
  ""
  ""
)

(define_insn_and_split "ashr<mode>3_1"
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (ashiftrt:S256I (match_operand:S256I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
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
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (ashiftrt:S256I (match_operand:S256I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  {
    return "sra<chunkxs> %x0 = %x1, %2\n\tsra<chunkxs> %y0 = %y1, %2\n\t"
           "sra<chunkxs> %z0 = %z1, %2\n\tsra<chunkxs> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4,alu_tiny_x4")
   (set_attr "length" "       16,         16")]
)

(define_expand "lshr<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "")
        (lshiftrt:S256I (match_operand:S256I 1 "register_operand" "")
                        (match_operand:SI 2 "sat_shift_operand" "")))]
  ""
  ""
)

(define_insn_and_split "lshr<mode>3_1"
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (lshiftrt:S256I (match_operand:S256I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
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
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (lshiftrt:S256I (match_operand:S256I 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  {
    return "srl<chunkxs> %x0 = %x1, %2\n\tsrl<chunkxs> %y0 = %y1, %2\n\t"
           "srl<chunkxs> %z0 = %z1, %2\n\tsrl<chunkxs> %t0 = %t1, %2";
  }
  [(set_attr "type" "alu_tiny_x4,alu_tiny_x4")
   (set_attr "length" "       16,         16")]
)

(define_insn_and_split "sshr<mode>3"
  [(set (match_operand:S256I 0 "register_operand" "=&r,r")
        (unspec:S256I [(match_operand:S256I 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRS))]
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
  {
    return "avgru<chunkx> %x0 = %x1, %x2\n\tavgru<chunkx> %y0 = %y1, %y2\n\t"
           "avgru<chunkx> %z0 = %z1, %z2\n\tavgru<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
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
  [(set_attr "type" "mau_auxr")]
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
  [(set_attr "type" "mau_auxr")]
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
  [(set_attr "type" "mau_auxr")]
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
    else if (KV3_2)
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
  "KV3_2"
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
  [(set (match_operand:V256J 0 "register_operand" "=&r")
        (us_plus:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                       (match_operand:V256J 2 "register_operand" "r")))]
  "KV3_2"
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
  [(set (match_operand:V256J 0 "register_operand" "=&r")
        (us_plus:V256J (match_operand:V256J 1 "register_operand" "r")
                       (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
    else if (KV3_2)
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
  "KV3_2"
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
  [(set (match_operand:V256J 0 "register_operand" "=&r")
        (us_minus:V256J (vec_duplicate:V256J (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                        (match_operand:V256J 2 "register_operand" "r")))]
  "KV3_2"
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
  [(set (match_operand:V256J 0 "register_operand" "=&r")
        (us_minus:V256J (match_operand:V256J 1 "register_operand" "r")
                        (vec_duplicate:V256J (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))))]
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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

(define_insn "*nand<suffix>3"
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

(define_insn "*andn<suffix>3"
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

(define_insn "*nior<suffix>3"
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

(define_insn "*iorn<suffix>3"
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

(define_insn "*nxor<suffix>3"
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


;; V256J

(define_insn_and_split "madd<mode><mode>4"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (plus:V256J (mult:V256J (match_operand:V256J 1 "register_operand" "r")
                                (match_operand:V256J 2 "register_operand" "r"))
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
    else if (KV3_2)
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
  "KV3_2"
  {
    return "abd<chunkx> %x0 = %x1, %x2\n\tabd<chunkx> %y0 = %y1, %y2\n\t"
           "abd<chunkx> %z0 = %z1, %z2\n\tabd<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*abd<suffix>_s1"
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

(define_insn "*abd<suffix>_s1"
  [(set (match_operand:V256L 0 "register_operand" "=&r")
        (minus:V256L (smax:V256L (vec_duplicate:V256L (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                                 (match_operand:V256L 2 "register_operand" "r"))
                     (smin:V256L (vec_duplicate:V256L (match_dup 1)) (match_dup 2))))]
  "KV3_2"
  {
    return "abd<chunkx> %x0 = %1, %x2\n\tabd<chunkx> %y0 = %1, %y2\n\t"
           "abd<chunkx> %z0 = %1, %z2\n\tabd<chunkx> %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*abd<suffix>_s2"
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

(define_insn "*abd<suffix>_s2"
  [(set (match_operand:V256L 0 "register_operand" "=&r")
        (minus:V256L (smax:V256L (match_operand:V256L 1 "register_operand" "r")
                                 (vec_duplicate:V256L (match_operand:<CHUNK> 2 "nonmemory_operand" "r")))
                     (smin:V256L (match_dup 1) (vec_duplicate:V256L (match_dup 2)))))]
  "KV3_2"
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
    else if (KV3_2)
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
  "KV3_2"
  {
    return "abds<chunkx> %x0 = %x1, %x2\n\tabds<chunkx> %y0 = %y1, %y2\n\t"
           "abds<chunkx> %z0 = %z1, %z2\n\tabds<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*abds<suffix>_s1"
  [(set (match_operand:V256L 0 "register_operand" "=&r")
        (ss_minus:V256L (smax:V256L (vec_duplicate:V256L (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                                    (match_operand:V256L 2 "register_operand" "r"))
                        (smin:V256L (vec_duplicate:V256L (match_dup 1)) (match_dup 2))))]
  "KV3_2"
  {
    return "abds<chunkx> %x0 = %1, %x2\n\tabds<chunkx> %y0 = %1, %y2\n\t"
           "abds<chunkx> %z0 = %1, %z2\n\tabds<chunkx> %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*abds<suffix>_s2"
  [(set (match_operand:V256L 0 "register_operand" "=&r")
        (ss_minus:V256L (smax:V256L (match_operand:V256L 1 "register_operand" "r")
                                    (vec_duplicate:V256L (match_operand:<CHUNK> 2 "nonmemory_operand" "r")))
                        (smin:V256L (match_dup 1) (vec_duplicate:V256L (match_dup 2)))))]
  "KV3_2"
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
    else if (KV3_2)
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
  "KV3_2"
  {
    return "abdu<chunkx> %x0 = %x1, %x2\n\tabdu<chunkx> %y0 = %y1, %y2\n\t"
           "abdu<chunkx> %z0 = %z1, %z2\n\tabdu<chunkx> %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*abdu<suffix>_s1"
  [(set (match_operand:V256L 0 "register_operand" "=&r")
        (minus:V256L (umax:V256L (vec_duplicate:V256L (match_operand:<CHUNK> 1 "nonmemory_operand" "r"))
                                 (match_operand:V256L 2 "register_operand" "r"))
                     (umin:V256L (vec_duplicate:V256L (match_dup 1)) (match_dup 2))))]
  "KV3_2"
  {
    return "abdu<chunkx> %x0 = %1, %x2\n\tabdu<chunkx> %y0 = %1, %y2\n\t"
           "abdu<chunkx> %z0 = %1, %z2\n\tabdu<chunkx> %t0 = %1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*abdu<suffix>_s2"
  [(set (match_operand:V256L 0 "register_operand" "=&r")
        (minus:V256L (umax:V256L (match_operand:V256L 1 "register_operand" "r")
                                 (vec_duplicate:V256L (match_operand:<CHUNK> 2 "nonmemory_operand" "r")))
                     (umin:V256L (match_dup 1) (vec_duplicate:V256L (match_dup 2)))))]
  "KV3_2"
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
  [(set_attr "type" "mau_auxr")]
)


;; V4DI

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
  [(set (match_operand:V4DI 0 "register_operand" "=&r,r")
        (ss_ashift:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
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
   (match_operand:SI 2 "sat_shift_operand" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_usashlv4di3_1 (operands[0], operands[1], operands[2]));
    else if (KV3_2)
      emit_insn (gen_usashlv4di3_2 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn_and_split "usashlv4di3_1"
  [(set (match_operand:V4DI 0 "register_operand" "=&r,r")
        (us_ashift:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:V4DI 3 "=&r,&r"))
   (clobber (match_scratch:V4DI 4 "=&r,&r"))
   (clobber (match_scratch:V4DI 5 "=&r,&r"))]
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
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")]
)

(define_insn_and_split "usashlv4di3_2"
  [(set (match_operand:V4DI 0 "register_operand" "=&r,r")
        (us_ashift:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  "KV3_2"
  "#"
  "KV3_2 && reload_completed"
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

(define_insn_and_split "sshrv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=&r,r")
        (unspec:V4DI [(match_operand:V4DI 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SRS))]
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
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_fp16") (const_string "mau_fpu")))]
)

(define_insn "sub<mode>3"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (minus:S64F (match_operand:S64F 1 "register_operand" "r")
                    (match_operand:S64F 2 "register_operand" "r")))]
  ""
  "fsbf<suffix> %0 = %2, %1"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_fp16") (const_string "mau_fpu")))]
)

(define_insn "mul<mode>3"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (mult:S64F (match_operand:S64F 1 "register_operand" "r")
                   (match_operand:S64F 2 "register_operand" "r")))]
  ""
  "fmul<suffix> %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_fp16") (const_string "mau_fpu")))]
)

(define_insn "fma<mode>4"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (fma:S64F (match_operand:S64F 1 "register_operand" "r")
                  (match_operand:S64F 2 "register_operand" "r")
                  (match_operand:S64F 3 "register_operand" "0")))]
  ""
  "ffma<suffix> %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_auxr_fp16") (const_string "mau_auxr_fpu")))]
)

(define_insn "fnma<mode>4"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (fma:S64F (neg:S64F (match_operand:S64F 1 "register_operand" "r"))
                  (match_operand:S64F 2 "register_operand" "r")
                  (match_operand:S64F 3 "register_operand" "0")))]
  ""
  "ffms<suffix> %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_auxr_fp16") (const_string "mau_auxr_fpu")))]
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


;; V2SF

(define_insn "kvx_fdot2w"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FDOT2))]
  ""
  "fdot2w%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fdot2wd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FDOT2))]
  ""
  "fdot2wd%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fdot2wdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FDOT2))]
  ""
  "fdot2wdp%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_fdot2wzp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FDOT2))]
  ""
  "fdot2wzp%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "floatv2siv2sf2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (float:V2SF (match_operand:V2SI 1 "register_operand" "r")))]
  ""
  "floatwp.rn %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "floatunsv2siv2sf2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unsigned_float:V2SF (match_operand:V2SI 1 "register_operand" "r")))]
  ""
  "floatuwp.rn %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "fix_truncv2sfv2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (fix:V2SI (match_operand:V2SF 1 "register_operand" "r")))]
  ""
  "fixedwp.rz %0 = %1, 0"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "fixuns_truncv2sfv2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unsigned_fix:V2SI (match_operand:V2SF 1 "register_operand" "r")))]
  ""
  "fixeduwp.rz %0 = %1, 0"
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

(define_expand "kvx_fcdivwp"
  [(match_operand:V2SF 0 "register_operand" "")
   (match_operand:V2SF 1 "register_operand" "")
   (match_operand:V2SF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    rtx regpair = gen_reg_rtx (V4SFmode);
    emit_insn (gen_kvx_catfwq (regpair, operands[1], operands[2]));
    emit_insn (gen_kvx_fcdivwp_insn (operands[0], regpair, operands[3]));
    DONE;
  }
)

(define_insn "kvx_fcdivwp_insn"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand 2 "" "")] UNSPEC_FCDIV))]
  ""
  "fcdivwp%2 %0 = %1"
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
        rtx t = gen_reg_rtx (<MODE>mode);
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


;; S128F (V8HF V4SF)

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
  "KV3_1 && reload_completed"
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
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_auxr_fp16") (const_string "mau_auxr_fpu")))]
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
  "KV3_1 && reload_completed"
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
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_auxr_fp16") (const_string "mau_auxr_fpu")))]
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
  "KV3_1 && reload_completed"
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
  [(set_attr "type" "mau_auxr_fp16")]
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
  "KV3_1 && reload_completed"
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
  [(set_attr "type" "mau_fp16")]
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
  "KV3_1 && reload_completed"
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
  [(set_attr "type" "mau_fp16")]
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

(define_insn "addv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (plus:V4SF (match_operand:V4SF 1 "register_operand" "r")
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "faddwq %0 = %1, %2"
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

(define_insn "mulv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (mult:V4SF (match_operand:V4SF 1 "register_operand" "r")
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "fmulwq %0 = %1, %2"
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

(define_expand "kvx_fcdivwq"
  [(match_operand:V4SF 0 "register_operand" "")
   (match_operand:V4SF 1 "register_operand" "")
   (match_operand:V4SF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = simplify_gen_subreg (V2SFmode, operands[0], V4SFmode, i*8);
        rtx opnd1 = simplify_gen_subreg (V2SFmode, operands[1], V4SFmode, i*8);
        rtx opnd2 = simplify_gen_subreg (V2SFmode, operands[2], V4SFmode, i*8);
        emit_insn (gen_kvx_fcdivwp (opnd0, opnd1, opnd2, operands[3]));
      }
    DONE;
  }
)


;; V2DF

(define_insn "addv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (plus:V2DF (match_operand:V2DF 1 "register_operand" "r")
                   (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "fadddp %0 = %1, %2"
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


(define_expand "kvx_fcdivdp"
  [(match_operand:V2DF 0 "register_operand" "")
   (match_operand:V2DF 1 "register_operand" "")
   (match_operand:V2DF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = simplify_gen_subreg (DFmode, operands[0], V2DFmode, i*8);
        rtx opnd1 = simplify_gen_subreg (DFmode, operands[1], V2DFmode, i*8);
        rtx opnd2 = simplify_gen_subreg (DFmode, operands[2], V2DFmode, i*8);
        emit_insn (gen_kvx_fcdivd (opnd0, opnd1, opnd2, operands[3]));
      }
    DONE;
  }
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
  [(match_operand:S256F 0 "register_operand")
   (match_operand:S256F 1 "register_operand")
   (match_operand:S256F 2 "register_operand")]
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


;; V8SF

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

(define_expand "kvx_fcdivwo"
  [(match_operand:V8SF 0 "register_operand" "")
   (match_operand:V8SF 1 "register_operand" "")
   (match_operand:V8SF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = simplify_gen_subreg (V2SFmode, operands[0], V8SFmode, i*8);
        rtx opnd1 = simplify_gen_subreg (V2SFmode, operands[1], V8SFmode, i*8);
        rtx opnd2 = simplify_gen_subreg (V2SFmode, operands[2], V8SFmode, i*8);
        emit_insn (gen_kvx_fcdivwp (opnd0, opnd1, opnd2, operands[3]));
      }
    DONE;
  }
)


;; V4DF

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

(define_expand "copysignv4df3"
  [(match_operand:V4DF 0 "register_operand")
   (match_operand:V4DF 1 "register_operand")
   (match_operand:V4DF 2 "register_operand")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = simplify_gen_subreg (V2DFmode, operands[0], V4DFmode, i*16);
        rtx opnd1 = simplify_gen_subreg (V2DFmode, operands[1], V4DFmode, i*16);
        rtx opnd2 = simplify_gen_subreg (V2DFmode, operands[2], V4DFmode, i*16);
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

(define_expand "kvx_fcdivdq"
  [(match_operand:V4DF 0 "register_operand" "")
   (match_operand:V4DF 1 "register_operand" "")
   (match_operand:V4DF 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = simplify_gen_subreg (DFmode, operands[0], V4DFmode, i*8);
        rtx opnd1 = simplify_gen_subreg (DFmode, operands[1], V4DFmode, i*8);
        rtx opnd2 = simplify_gen_subreg (DFmode, operands[2], V4DFmode, i*8);
        emit_insn (gen_kvx_fcdivd (opnd0, opnd1, opnd2, operands[3]));
      }
    DONE;
  }
)


