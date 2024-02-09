;; 64-bit Vector Moves

(define_expand "mov<mode>"
  [(set (match_operand:SIMD64 0 "nonimmediate_operand" "")
        (match_operand:SIMD64 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (<MODE>mode, operands[1]);
    }
  }
)

(define_insn "*mov<mode>"
  [(set (match_operand:SIMD64 0 "nonimmediate_operand" "=r, r, r, r, r, r, r,a,b,m,  r,  r,r")
        (match_operand:SIMD64 1 "general_operand"       "r,Ca,Cb,Cm,Za,Zb,Zm,r,r,r,v16,v43,i"))]
  "(!immediate_operand(operands[1], <MODE>mode) || !memory_operand(operands[0], <MODE>mode))"
  {
    switch (which_alternative) {
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
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (<MODE>mode, operands[1]);
    }
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
    switch (which_alternative) {
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
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (<MODE>mode, operands[1]);
    }
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
    switch (which_alternative) {
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

;; Vector Set/Extract/Init/Perm

(define_insn "kvx_insf_<MODE>"
  [(set (match_operand:ALL64 0 "register_operand" "+r")
        (unspec:ALL64 [(match_operand:<INNER> 1 "register_operand" "r")
                       (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                       (match_operand:SI 3 "sixbits_unsigned_operand" "i")
                       (match_dup 0)] UNSPEC_INSF))]
  ""
  "insf %0 = %1, %2+%3-1, %3"
  [(set_attr "type" "alu_lite")]
)

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

(define_insn "kvx_extfz_<MODE>"
  [(set (match_operand:<INNER> 0 "register_operand" "=r")
        (unspec:<INNER> [(match_operand:ALL64 1 "register_operand" "r")
                         (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                         (match_operand:SI 3 "sixbits_unsigned_operand" "i")] UNSPEC_EXTFZ))]
  ""
  "extfz %0 = %1, %2+%3-1, %3"
  [(set_attr "type" "alu_lite")]
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

(define_insn "kvx_sbmm8_<MODE>"
  [(set (match_operand:ALL64 0 "register_operand" "=r")
        (unspec:ALL64 [(match_operand:<INNER> 1 "register_operand" "r")
                       (match_operand:DI 2 "register_operand" "r")] UNSPEC_SBMM8))]
  ""
  "sbmm8 %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx2a_<MODE>"
  [(set (match_operand:SIMD32X2 0 "register_operand" "=r")
        (unspec:SIMD32X2 [(match_operand:<INNER> 1 "register_operand" "r")] UNSPEC_INITX2A))]
  ""
  "insf %0 = %1, 31, 0"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx2b_<MODE>"
  [(set (match_operand:SIMD32X2 0 "register_operand" "+r")
        (unspec:SIMD32X2 [(match_operand:<INNER> 1 "register_operand" "r")
                          (match_dup 0)] UNSPEC_INITX2B))]
  ""
  "insf %0 = %1, 63, 32"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx4a_<MODE>"
  [(set (match_operand:SIMD16X4 0 "register_operand" "=r")
        (unspec:SIMD16X4 [(match_operand:<INNER> 1 "register_operand" "r")] UNSPEC_INITX4A))]
  ""
  "insf %0 = %1, 15, 0"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx4b_<MODE>"
  [(set (match_operand:SIMD16X4 0 "register_operand" "+r")
        (unspec:SIMD16X4 [(match_operand:<INNER> 1 "register_operand" "r")
                          (match_dup 0)] UNSPEC_INITX4B))]
  ""
  "insf %0 = %1, 31, 16"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx4c_<MODE>"
  [(set (match_operand:SIMD16X4 0 "register_operand" "+r")
        (unspec:SIMD16X4 [(match_operand:<INNER> 1 "register_operand" "r")
                          (match_dup 0)] UNSPEC_INITX4C))]
  ""
  "insf %0 = %1, 47, 32"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx4d_<MODE>"
  [(set (match_operand:SIMD16X4 0 "register_operand" "+r")
        (unspec:SIMD16X4 [(match_operand:<INNER> 1 "register_operand" "r")
                          (match_dup 0)] UNSPEC_INITX4D))]
  ""
  "insf %0 = %1, 63, 48"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx8a_<MODE>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "=r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")] UNSPEC_INITX8A))]
  ""
  "insf %0 = %1, 7, 0"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx8b_<MODE>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8B))]
  ""
  "insf %0 = %1, 15, 8"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx8c_<MODE>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8C))]
  ""
  "insf %0 = %1, 23, 16"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx8d_<MODE>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8D))]
  ""
  "insf %0 = %1, 31, 24"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx8e_<MODE>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8E))]
  ""
  "insf %0 = %1, 39, 32"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx8f_<MODE>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8F))]
  ""
  "insf %0 = %1, 47, 40"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx8g_<MODE>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8G))]
  ""
  "insf %0 = %1, 55, 48"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_initx8h_<MODE>"
  [(set (match_operand:SIMD8X8 0 "register_operand" "+r")
        (unspec:SIMD8X8 [(match_operand:<INNER> 1 "register_operand" "r")
                         (match_dup 0)] UNSPEC_INITX8H))]
  ""
  "insf %0 = %1, 63, 56"
  [(set_attr "type" "alu_lite")]
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
   (match_operand:<PRED> 3 "" "")]
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

(define_expand "vec_cmp<mode><pred>"
  [(set (match_operand:<PRED> 0 "register_operand")
        (match_operator 1 "comparison_operator"
         [(match_operand:SIMDCMP 2 "register_operand")
          (match_operand:SIMDCMP 3 "register_operand")]))]
  ""
  {
    rtx pred = operands[0];
    rtx cmp = operands[1];
    rtx left = operands[2];
    rtx right = operands[3];
    kvx_lower_comparison (pred, GET_CODE (operands[1]), left, right);
    DONE;
  }
)

(define_expand "vec_cmpu<mode><pred>"
  [(set (match_operand:<PRED> 0 "register_operand")
        (match_operator 1 "comparison_operator"
         [(match_operand:SIMDCMP 2 "register_operand")
          (match_operand:SIMDCMP 3 "register_operand")]))]
  ""
  {
    rtx pred = operands[0];
    rtx cmp = operands[1];
    rtx left = operands[2];
    rtx right = operands[3];
    kvx_lower_comparison (pred, GET_CODE (operands[1]), left, right);
    DONE;
  }
)

(define_expand "vcond<SIMDCMP:mode><SIMDCMP2:mode>"
  [(match_operand:SIMDCMP 0 "register_operand")
   (match_operand:SIMDCMP 1 "register_operand")
   (match_operand:SIMDCMP 2 "register_operand")
   (match_operator 3 "comparison_operator"
    [(match_operand:SIMDCMP2 4 "register_operand")
     (match_operand:SIMDCMP2 5 "register_operand")])]
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
   (match_operand:SIMDCMP 1 "register_operand")
   (match_operand:SIMDCMP 2 "register_operand")
   (match_operator 3 "comparison_operator"
    [(match_operand:SIMDCMP2 4 "register_operand")
     (match_operand:SIMDCMP2 5 "register_operand")])]
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

(define_expand "vcond_mask_<mode><pred>"
  [(match_operand:SIMDCMP 0 "register_operand")
   (match_operand:SIMDCMP 1 "register_operand")
   (match_operand:SIMDCMP 2 "register_operand")
   (match_operand:<PRED> 3 "register_operand")]
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

;; (set (reg:V2DI 267)
;;     (if_then_else:V2DI (eq (reg:V2DI 270)
;;             (const_vector:V2DI [
;;                     (const_int 0 [0])
;;                     (const_int 0 [0])
;;                 ]))
;;         (const_vector:V2DI [
;;                 (const_int 0 [0])
;;                 (const_int 0 [0])
;;             ])
;;         (const_vector:V2DI [
;;                 (const_int -1 [0xffffffffffffffff])
;;                 (const_int -1 [0xffffffffffffffff])
;;             ])))

;; (define_insn_and_split "*vec_pred_mode_"
;;   [(set (match_operand:V2DI 0 "register_operand" "=r")
;;         (if_then_else:V2DI (ne (match_operand:V2DI 1 "register_operand" "r")
;;                                (match_operand:V2DI 2 "const_zero_operand"))
;;                            (match_operand:V2DI 3 "const_mone_operand")
;;                            (match_operand:V2DI 4 "const_zero_operand")))]
;;   ""
;;   "#"
;;   ""
;;   [(set (match_dup 0)
;;         (match_dup 1))
;;   ]
;;   ""
;; )

;; (define_split
;;   [(set (match_operand:V2DI 0 "register_operand" "")
;;         (if_then_else:V2DI (ne (match_operand:V2DI 1 "register_operand" "")
;;                                (match_operand:V2DI 2 "const_zero_operand" ""))
;;                            (match_operand:V2DI 3 "const_mone_operand" "")
;;                            (match_operand:V2DI 4 "const_zero_operand" "")))]
;;   ""
;;   [(set (match_dup 0) (match_dup 1))]
;;   ""
;; )

;; ; a = b != c
;; (define_split
;;   [(set (match_operand:V 0 "register_operand" "")
;;         (if_then_else:V
;;          (ne (match_operand:<tointvec> 3 "register_operand" "")
;;              (match_operand:V 4 "const0_operand" ""))
;;          (match_operand:V 1 "all_ones_operand" "")
;;          (match_operand:V 2 "const0_operand" "")))]
;;   "TARGET_VX"
;;   [(set (match_dup 0) (match_dup 3))]
;; {
;;   PUT_MODE (operands[3], <V:MODE>mode);
;; })


;; V4HI

(define_insn "*compnhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (match_operator:V4HI 1 "comparison_operator"
         [(match_operand:V4HI 2 "register_operand" "r")
          (match_operand:V4HI 3 "register_operand" "r")]))]
  ""
  "compnhq.%1 %0 = %2, %3"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*cmovehq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (if_then_else:V4HI (match_operator 2 "signed_comparison_operator"
                                               [(match_operand:V4HI 3 "register_operand" "r")
                                                (const_vector:V4HI [
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)])])
                             (match_operand:V4HI 1 "register_operand" "r")
                             (match_operand:V4HI 4 "register_operand" "0")))]
  ""
  "cmovehq.%2z %3? %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_cmovehq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V4HI 1 "register_operand" "0")
                      (match_operand:V4HI 2 "register_operand" "r")
                      (match_operand:V4HI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_CMOVEHQ))]
  ""
  "cmovehq%4 %3? %0 = %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "addv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (plus:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addhq %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,        8,          8")]
)

(define_insn "ssaddv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (ss_plus:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                      (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addshq %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx2hq"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (plus:V4HI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                              (const_int 2))
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx2hq %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx2hq2"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (plus:V4HI (ashift:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                                (const_int 1))
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx2hq %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx4hq"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (plus:V4HI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                              (const_int 4))
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx4hq %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx4hq2"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (plus:V4HI (ashift:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                                (const_int 2))
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx4hq %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx8hq"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (plus:V4HI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                              (const_int 8))
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx8hq %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx8hq2"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (plus:V4HI (ashift:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                                (const_int 3))
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx8hq %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx16hq"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (plus:V4HI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                              (const_int 16))
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx16hq %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx16hq2"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (plus:V4HI (ashift:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                                (const_int 4))
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx16hq %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "subv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (minus:V4HI (match_operand:V4HI 1 "nonmemory_operand" "r,v32,vx2")
                    (match_operand:V4HI 2 "register_operand" "r,r,r")))]
  ""
  "sbfhq %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "sssubv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (ss_minus:V4HI (match_operand:V4HI 1 "nonmemory_operand" "r,v32,vx2")
                       (match_operand:V4HI 2 "register_operand" "r,r,r")))]
  ""
  "sbfshq %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx2hq"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (minus:V4HI (match_operand:V4HI 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:V4HI (match_operand:V4HI 2 "register_operand" "r,r,r")
                               (const_int 2))))]
  ""
  "sbfx2hq %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx2hq2"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (minus:V4HI (match_operand:V4HI 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:V4HI (match_operand:V4HI 2 "register_operand" "r,r,r")
                                 (const_int 1))))]
  ""
  "sbfx2hq %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx4hq"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (minus:V4HI (match_operand:V4HI 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:V4HI (match_operand:V4HI 2 "register_operand" "r,r,r")
                               (const_int 4))))]
  ""
  "sbfx4hq %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx4hq2"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (minus:V4HI (match_operand:V4HI 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:V4HI (match_operand:V4HI 2 "register_operand" "r,r,r")
                                 (const_int 2))))]
  ""
  "sbfx4hq %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx8hq"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (minus:V4HI (match_operand:V4HI 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:V4HI (match_operand:V4HI 2 "register_operand" "r,r,r")
                               (const_int 8))))]
  ""
  "sbfx8hq %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx8hq2"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (minus:V4HI (match_operand:V4HI 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:V4HI (match_operand:V4HI 2 "register_operand" "r,r,r")
                                 (const_int 3))))]
  ""
  "sbfx8hq %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx16hq"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (minus:V4HI (match_operand:V4HI 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:V4HI (match_operand:V4HI 2 "register_operand" "r,r,r")
                               (const_int 16))))]
  ""
  "sbfx16hq %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx16hq2"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (minus:V4HI (match_operand:V4HI 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:V4HI (match_operand:V4HI 2 "register_operand" "r,r,r")
                                 (const_int 4))))]
  ""
  "sbfx16hq %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "mulv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (mult:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "mulhq %0 = %1, %2"
  [(set_attr "type" "mau,mau_x,mau_x")
   (set_attr "length" "4,    8,    8")]
)

(define_insn "*mulv4hiv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (mult:V4SI (sign_extend:V4SI (match_operand:V4HI 1 "register_operand" "r"))
                   (sign_extend:V4SI (match_operand:V4HI 2 "register_operand" "r"))))]
  "0"
  "mulhwq %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "*umulv4hiv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (mult:V4SI (zero_extend:V4SI (match_operand:V4HI 1 "register_operand" "r"))
                   (zero_extend:V4SI (match_operand:V4HI 2 "register_operand" "r"))))]
  "0"
  "muluhwq %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "sminv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (smin:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "minhq %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "smaxv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (smax:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "maxhq %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "uminv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (umin:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "minuhq %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "umaxv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r")
        (umax:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r")
                   (match_operand:V4HI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "maxuhq %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "andv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r,r")
        (and:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r,r")
                  (match_operand:V4HI 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "andd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*nandv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r,r")
        (ior:V4HI (not:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r,r"))
                  (not:V4HI (match_operand:V4HI 2 "nonmemory_operand" "r,v10,v37,i"))))]
  ""
  "nandd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*andnv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r,r")
        (and:V4HI (not:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r,r"))
                  (match_operand:V4HI 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "andnd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "iorv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r,r")
        (ior:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r,r")
                  (match_operand:V4HI 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "ord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*niorv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r,r")
        (and:V4HI (not:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r,r"))
                  (not:V4HI (match_operand:V4HI 2 "nonmemory_operand" "r,v10,v37,i"))))]
  ""
  "nord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*iornv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r,r")
        (ior:V4HI (not:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r,r"))
                  (match_operand:V4HI 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "ornd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "xorv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r,r")
        (xor:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r,r")
                  (match_operand:V4HI 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "xord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*nxorv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r,r")
        (not:V4HI (xor:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r,r")
                            (match_operand:V4HI 2 "nonmemory_operand" "r,v10,v37,i"))))]
  ""
  "nxord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "maddv4hiv4hi4"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r,r,r")
        (plus:V4HI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r,r,r,r")
                              (match_operand:V4HI 2 "nonmemory_operand" "r,v10,v37,i"))
                   (match_operand:V4HI 3 "register_operand" "0,0,0,0")))]
  ""
  "maddhq %0 = %1, %2"
  [(set_attr "type" "mau_auxr,mau_auxr,mau_auxr_x,mau_auxr_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "maddv4hiv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (sign_extend:V4SI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                                                (match_operand:V4HI 2 "nonmemory_operand" "r")))
                   (match_operand:V4SI 3 "register_operand" "0")))]
  "0"
  "maddhwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "umaddv4hiv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (zero_extend:V4SI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                                                (match_operand:V4HI 2 "nonmemory_operand" "r")))
                   (match_operand:V4SI 3 "register_operand" "0")))]
  "0"
  "madduhwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "msubv4hiv4hi4"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (minus:V4HI (match_operand:V4HI 3 "register_operand" "0")
                    (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                               (match_operand:V4HI 2 "register_operand" "r"))))]
  ""
  "msbfhq %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "msubv4hiv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 3 "register_operand" "0")
                    (sign_extend:V4SI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                                                 (match_operand:V4HI 2 "register_operand" "r")))))]
  "0"
  "msbfhwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "umsubv4hiv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 3 "register_operand" "0")
                    (zero_extend:V4SI (mult:V4HI (match_operand:V4HI 1 "register_operand" "r")
                                                 (match_operand:V4HI 2 "register_operand" "r")))))]
  "0"
  "msbfuhwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "ashlv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r")
        (ashift:V4HI (match_operand:V4HI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sllhqs %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "ssashlv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r")
        (ss_ashift:V4HI (match_operand:V4HI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "slshqs %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "ashrv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r")
        (ashiftrt:V4HI (match_operand:V4HI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srahqs %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "lshrv4hi3"
  [(set (match_operand:V4HI 0 "register_operand" "=r,r")
        (lshiftrt:V4HI (match_operand:V4HI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srlhqs %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "negv4hi2"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (neg:V4HI (match_operand:V4HI 1 "register_operand" "r")))]
  ""
  "neghq %0 = %1"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length" "8")]
)

(define_insn "absv4hi2"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (abs:V4HI (match_operand:V4HI 1 "register_operand" "r")))]
  ""
  "abshq %0 = %1"
  [(set_attr "type" "alu_lite_x")
   (set_attr "length" "8")]
)

(define_insn "clrsbv4hi2"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (clrsb:V4HI (match_operand:V4HI 1 "register_operand" "r")))]
  "0"
  "clshq %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "clzv4hi2"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (clz:V4HI (match_operand:V4HI 1 "register_operand" "r")))]
  "0"
  "clzhq %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "ctzv4hi2"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (ctz:V4HI (match_operand:V4HI 1 "register_operand" "r")))]
  "0"
  "ctzhq %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "popcountv4hi2"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (popcount:V4HI (match_operand:V4HI 1 "register_operand" "r")))]
  "0"
  "cbshq %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "one_cmplv4hi2"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (not:V4HI (match_operand:V4HI 1 "register_operand" "r")))]
  ""
  "notd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "extendv4hiv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (sign_extend:V4SI (match_operand:V4HI 1 "register_operand" "r")))]
  "0"
  "sxhwq %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "zero_extendv4hiv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (zero_extend:V4SI (match_operand:V4HI 1 "register_operand" "r")))]
  "0"
  "zxhwq %0 = %1"
  [(set_attr "type" "alu_tiny")]
)


;; V8HI

(define_insn "*compnho"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (match_operator:V8HI 1 "comparison_operator"
         [(match_operand:V8HI 2 "register_operand" "r")
          (match_operand:V8HI 3 "register_operand" "r")]))]
  ""
  "compnhq.%1 %x0 = %x2, %x3\n\tcompnhq.%1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*cmoveho"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (if_then_else:V8HI (match_operator 2 "signed_comparison_operator"
                                               [(match_operand:V8HI 3 "register_operand" "r")
                                                (const_vector:V8HI [
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)])])
                             (match_operand:V8HI 1 "register_operand" "r")
                             (match_operand:V8HI 4 "register_operand" "0")))]
  ""
  "cmovehq.%2z %x3? %x0 = %x1\n\tcmovehq.%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_cmoveho"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (unspec:V8HI [(match_operand:V8HI 1 "register_operand" "0")
                      (match_operand:V8HI 2 "register_operand" "r")
                      (match_operand:V8HI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_CMOVEHO))]
  ""
  "cmovehq%4 %x3? %x0 = %x2\n\tcmovehq%4 %y3? %y0 = %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "addv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (match_operand:V8HI 1 "register_operand" "r")
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addhq %x0 = %x1, %x2\n\taddhq %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "ssaddv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (ss_plus:V8HI (match_operand:V8HI 1 "register_operand" "r")
                      (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addshq %x0 = %x1, %x2\n\taddshq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx2v8hi"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                              (const_int 2))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx2hq %x0 = %x1, %x2\n\taddx2hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx2v8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (ashift:V8HI (match_operand:V8HI 1 "register_operand" "r")
                                (const_int 1))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx2hq %x0 = %x1, %x2\n\taddx2hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4v8hi"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                              (const_int 4))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx4hq %x0 = %x1, %x2\n\taddx4hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4v8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (ashift:V8HI (match_operand:V8HI 1 "register_operand" "r")
                                (const_int 2))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx4hq %x0 = %x1, %x2\n\taddx4hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8v8hi"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                              (const_int 8))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx8hq %x0 = %x1, %x2\n\taddx8hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8v8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (ashift:V8HI (match_operand:V8HI 1 "register_operand" "r")
                                (const_int 3))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx8hq %x0 = %x1, %x2\n\taddx8hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16v8hi"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                              (const_int 16))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx16hq %x0 = %x1, %x2\n\taddx16hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16v8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (ashift:V8HI (match_operand:V8HI 1 "register_operand" "r")
                                (const_int 4))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx16hq %x0 = %x1, %x2\n\taddx16hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "subv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (match_operand:V8HI 2 "register_operand" "r")))]
  ""
  "sbfhq %x0 = %x2, %x1\n\tsbfhq %y0 = %y2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "sssubv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (ss_minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                       (match_operand:V8HI 2 "register_operand" "r")))]
  ""
  "sbfshq %x0 = %x2, %x1\n\tsbfshq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx2v8hi"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (mult:V8HI (match_operand:V8HI 2 "register_operand" "r")
                               (const_int 2))))]
  ""
  "sbfx2hq %x0 = %x2, %x1\n\tsbfx2hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx2v8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (ashift:V8HI (match_operand:V8HI 2 "register_operand" "r")
                                 (const_int 1))))]
  ""
  "sbfx2hq %x0 = %x2, %x1\n\tsbfx2hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4v8hi"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (mult:V8HI (match_operand:V8HI 2 "register_operand" "r")
                               (const_int 4))))]
  ""
  "sbfx4hq %x0 = %x2, %x1\n\tsbfx4hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4v8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (ashift:V8HI (match_operand:V8HI 2 "register_operand" "r")
                                 (const_int 2))))]
  ""
  "sbfx4hq %x0 = %x2, %x1\n\tsbfx4hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8v8hi"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (mult:V8HI (match_operand:V8HI 2 "register_operand" "r")
                               (const_int 8))))]
  ""
  "sbfx8hq %x0 = %x2, %x1\n\tsbfx8hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8v8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (ashift:V8HI (match_operand:V8HI 2 "register_operand" "r")
                                 (const_int 3))))]
  ""
  "sbfx8hq %x0 = %x2, %x1\n\tsbfx8hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16v8hi"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (mult:V8HI (match_operand:V8HI 2 "register_operand" "r")
                               (const_int 16))))]
  ""
  "sbfx16hq %x0 = %x2, %x1\n\tsbfx16hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16v8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (ashift:V8HI (match_operand:V8HI 2 "register_operand" "r")
                                 (const_int 4))))]
  ""
  "sbfx16hq %x0 = %x2, %x1\n\tsbfx16hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

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

(define_insn "sminv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (smin:V8HI (match_operand:V8HI 1 "register_operand" "r")
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "minhq %x0 = %x1, %x2\n\tminhq %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "smaxv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (smax:V8HI (match_operand:V8HI 1 "register_operand" "r")
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "maxhq %x0 = %x1, %x2\n\tmaxhq %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "uminv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (umin:V8HI (match_operand:V8HI 1 "register_operand" "r")
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "minuhq %x0 = %x1, %x2\n\tminuhq %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "umaxv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (umax:V8HI (match_operand:V8HI 1 "register_operand" "r")
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "maxuhq %x0 = %x1, %x2\n\tmaxuhq %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "andv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (and:V8HI (match_operand:V8HI 1 "register_operand" "r")
                  (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*nandv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (ior:V8HI (not:V8HI (match_operand:V8HI 1 "register_operand" "r"))
                  (not:V8HI (match_operand:V8HI 2 "nonmemory_operand" "r"))))]
  ""
  "nandd %x0 = %x1, %x2\n\tnandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*andnv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (and:V8HI (not:V8HI (match_operand:V8HI 1 "register_operand" "r"))
                  (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "andnd %x0 = %x1, %x2\n\tandnd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "iorv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (ior:V8HI (match_operand:V8HI 1 "register_operand" "r")
                  (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*niorv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (and:V8HI (not:V8HI (match_operand:V8HI 1 "register_operand" "r"))
                  (not:V8HI (match_operand:V8HI 2 "nonmemory_operand" "r"))))]
  ""
  "nord %x0 = %x1, %x2\n\tnord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*iornv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (ior:V8HI (not:V8HI (match_operand:V8HI 1 "register_operand" "r"))
                  (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "ornd %x0 = %x1, %x2\n\tornd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "xorv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (xor:V8HI (match_operand:V8HI 1 "register_operand" "r")
                  (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*nxorv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (not:V8HI (xor:V8HI (match_operand:V8HI 1 "register_operand" "r")
                            (match_operand:V8HI 2 "nonmemory_operand" "r"))))]
  ""
  "nxord %x0 = %x1, %x2\n\tnxord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "maddv8hiv8hi4"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                              (match_operand:V8HI 2 "nonmemory_operand" "r"))
                   (match_operand:V8HI 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4HI (match_dup 0) 0)
        (plus:V4HI (mult:V4HI (subreg:V4HI (match_dup 1) 0)
                              (subreg:V4HI (match_dup 2) 0))
                   (subreg:V4HI (match_dup 3) 0)))
   (set (subreg:V4HI (match_dup 0) 8)
        (plus:V4HI (mult:V4HI (subreg:V4HI (match_dup 1) 8)
                              (subreg:V4HI (match_dup 2) 8))
                   (subreg:V4HI (match_dup 3) 8)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "msubv8hiv8hi4"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 3 "register_operand" "0")
                    (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                               (match_operand:V8HI 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4HI (match_dup 0) 0)
        (minus:V4HI (subreg:V4HI (match_dup 3) 0)
                    (mult:V4HI (subreg:V4HI (match_dup 1) 0)
                               (subreg:V4HI (match_dup 2) 0))))
   (set (subreg:V4HI (match_dup 0) 8)
        (minus:V4HI (subreg:V4HI (match_dup 3) 8)
                    (mult:V4HI (subreg:V4HI (match_dup 1) 8)
                               (subreg:V4HI (match_dup 2) 8))))]
  ""
  [(set_attr "type" "mau_auxr")
   (set_attr "length"      "8")]
)

(define_insn "ashlv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r,r")
        (ashift:V8HI (match_operand:V8HI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sllhqs %x0 = %x1, %2\n\tsllhqs %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "ssashlv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r,r")
        (ss_ashift:V8HI (match_operand:V8HI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "slshqs %x0 = %x1, %2\n\tslshqs %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "ashrv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r,r")
        (ashiftrt:V8HI (match_operand:V8HI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srahqs %x0 = %x1, %2\n\tsrahqs %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "lshrv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r,r")
        (lshiftrt:V8HI (match_operand:V8HI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srlhqs %x0 = %x1, %2\n\tsrlhqs %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "negv8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (neg:V8HI (match_operand:V8HI 1 "register_operand" "r")))]
  ""
  "neghq %x0 = %x1\n\tneghq %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2_x")
   (set_attr "length"        "16")]
)

(define_insn "absv8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (abs:V8HI (match_operand:V8HI 1 "register_operand" "r")))]
  ""
  "abshq %x0 = %x1\n\tabshq %y0 = %y1"
  [(set_attr "type" "alu_lite_x2_x")
   (set_attr "length"          "16")]
)

(define_insn "clrsbv8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (clrsb:V8HI (match_operand:V8HI 1 "register_operand" "r")))]
  ""
  "clshq %x0 = %x1\n\tclshq %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "clzv8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (clz:V8HI (match_operand:V8HI 1 "register_operand" "r")))]
  ""
  "clzhq %x0 = %x1\n\tclzhq %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "ctzv8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (ctz:V8HI (match_operand:V8HI 1 "register_operand" "r")))]
  ""
  "ctzhq %x0 = %x1\n\tctzhq %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "popcountv8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (popcount:V8HI (match_operand:V8HI 1 "register_operand" "r")))]
  ""
  "cbshq %x0 = %x1\n\tcbshq %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "one_cmplv8hi2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (not:V8HI (match_operand:V8HI 1 "register_operand" "r")))]
  ""
  "notd %x0 = %x1\n\tnotd %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
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

(define_insn_and_split "*cmovehx"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (if_then_else:V16HI (match_operator 2 "signed_comparison_operator"
                                               [(match_operand:V16HI 3 "register_operand" "r")
                                                (const_vector:V16HI [
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)])])
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

(define_insn_and_split "kvx_cmovehx"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (unspec:V16HI [(match_operand:V16HI 1 "register_operand" "0")
                       (match_operand:V16HI 2 "register_operand" "r")
                       (match_operand:V16HI 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_CMOVEHX))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (unspec:V8HI [(subreg:V8HI (match_dup 1) 0)
                      (subreg:V8HI (match_dup 2) 0)
                      (subreg:V8HI (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_CMOVEHO))
   (set (subreg:V8HI (match_dup 0) 16)
        (unspec:V8HI [(subreg:V8HI (match_dup 1) 16)
                      (subreg:V8HI (match_dup 2) 16)
                      (subreg:V8HI (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_CMOVEHO))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "addv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (plus:V16HI (match_operand:V16HI 1 "register_operand" "r")
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "addhq %x0 = %x1, %x2\n\taddhq %y0 = %y1, %y2\n\t"
           "addhq %z0 = %z1, %z2\n\taddhq %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "ssaddv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (ss_plus:V16HI (match_operand:V16HI 1 "register_operand" "r")
                       (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (ss_plus:V8HI (subreg:V8HI (match_dup 1) 0)
                      (subreg:V8HI (match_dup 2) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (ss_plus:V8HI (subreg:V8HI (match_dup 1) 16)
                      (subreg:V8HI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx2v16hi"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (plus:V16HI (mult:V16HI (match_operand:V16HI 1 "register_operand" "r")
                                (const_int 2))
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (plus:V8HI (mult:V8HI (subreg:V8HI (match_dup 1) 0)
                              (const_int 2))
                   (subreg:V8HI (match_dup 2) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (plus:V8HI (mult:V8HI (subreg:V8HI (match_dup 1) 16)
                              (const_int 2))
                   (subreg:V8HI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx2v16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (plus:V16HI (ashift:V16HI (match_operand:V16HI 1 "register_operand" "r")
                                  (const_int 1))
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (plus:V8HI (ashift:V8HI (subreg:V8HI (match_dup 1) 0)
                                (const_int 1))
                   (subreg:V8HI (match_dup 2) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (plus:V8HI (ashift:V8HI (subreg:V8HI (match_dup 1) 16)
                                (const_int 1))
                   (subreg:V8HI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx4v16hi"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (plus:V16HI (mult:V16HI (match_operand:V16HI 1 "register_operand" "r")
                                (const_int 4))
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (plus:V8HI (mult:V8HI (subreg:V8HI (match_dup 1) 0)
                              (const_int 4))
                   (subreg:V8HI (match_dup 2) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (plus:V8HI (mult:V8HI (subreg:V8HI (match_dup 1) 16)
                              (const_int 4))
                   (subreg:V8HI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx4v16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (plus:V16HI (ashift:V16HI (match_operand:V16HI 1 "register_operand" "r")
                                  (const_int 2))
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (plus:V8HI (ashift:V8HI (subreg:V8HI (match_dup 1) 0)
                                (const_int 2))
                   (subreg:V8HI (match_dup 2) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (plus:V8HI (ashift:V8HI (subreg:V8HI (match_dup 1) 16)
                                (const_int 2))
                   (subreg:V8HI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx8v16hi"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (plus:V16HI (mult:V16HI (match_operand:V16HI 1 "register_operand" "r")
                                (const_int 8))
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (plus:V8HI (mult:V8HI (subreg:V8HI (match_dup 1) 0)
                              (const_int 8))
                   (subreg:V8HI (match_dup 2) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (plus:V8HI (mult:V8HI (subreg:V8HI (match_dup 1) 16)
                              (const_int 8))
                   (subreg:V8HI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx8v16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (plus:V16HI (ashift:V16HI (match_operand:V16HI 1 "register_operand" "r")
                                  (const_int 3))
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (plus:V8HI (ashift:V8HI (subreg:V8HI (match_dup 1) 0)
                                (const_int 3))
                   (subreg:V8HI (match_dup 2) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (plus:V8HI (ashift:V8HI (subreg:V8HI (match_dup 1) 16)
                                (const_int 3))
                   (subreg:V8HI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx16v16hi"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (plus:V16HI (mult:V16HI (match_operand:V16HI 1 "register_operand" "r")
                                (const_int 16))
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (plus:V8HI (mult:V8HI (subreg:V8HI (match_dup 1) 0)
                              (const_int 16))
                   (subreg:V8HI (match_dup 2) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (plus:V8HI (mult:V8HI (subreg:V8HI (match_dup 1) 16)
                              (const_int 16))
                   (subreg:V8HI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx16v16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (plus:V16HI (ashift:V16HI (match_operand:V16HI 1 "register_operand" "r")
                                  (const_int 4))
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (plus:V8HI (ashift:V8HI (subreg:V8HI (match_dup 1) 0)
                                (const_int 4))
                   (subreg:V8HI (match_dup 2) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (plus:V8HI (ashift:V8HI (subreg:V8HI (match_dup 1) 16)
                                (const_int 4))
                   (subreg:V8HI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "subv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (minus:V16HI (match_operand:V16HI 1 "nonmemory_operand" "r")
                     (match_operand:V16HI 2 "register_operand" "r")))]
  ""
  {
    return "sbfhq %x0 = %x2, %x1\n\tsbfhq %y0 = %y2, %y1\n\t"
           "sbfhq %z0 = %z2, %z1\n\tsbfhq %t0 = %t2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "sssubv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (ss_minus:V16HI (match_operand:V16HI 1 "nonmemory_operand" "r")
                        (match_operand:V16HI 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (ss_minus:V8HI (subreg:V8HI (match_dup 1) 0)
                       (subreg:V8HI (match_dup 2) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (ss_minus:V8HI (subreg:V8HI (match_dup 1) 16)
                       (subreg:V8HI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx2v16hi"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (minus:V16HI (match_operand:V16HI 1 "nonmemory_operand" "r")
                     (mult:V16HI (match_operand:V16HI 2 "register_operand" "r")
                                 (const_int 2))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (minus:V8HI (subreg:V8HI (match_dup 1) 0)
                    (mult:V8HI (subreg:V8HI (match_dup 2) 0)
                               (const_int 2))))
   (set (subreg:V8HI (match_dup 0) 16)
        (minus:V8HI (subreg:V8HI (match_dup 1) 16)
                    (mult:V8HI (subreg:V8HI (match_dup 2) 16)
                               (const_int 2))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx2v16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (minus:V16HI (match_operand:V16HI 1 "nonmemory_operand" "r")
                     (ashift:V16HI (match_operand:V16HI 2 "register_operand" "r")
                                   (const_int 1))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (minus:V8HI (subreg:V8HI (match_dup 1) 0)
                    (ashift:V8HI (subreg:V8HI (match_dup 2) 0)
                                 (const_int 1))))
   (set (subreg:V8HI (match_dup 0) 16)
        (minus:V8HI (subreg:V8HI (match_dup 1) 16)
                    (ashift:V8HI (subreg:V8HI (match_dup 2) 16)
                                 (const_int 1))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx4v16hi"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (minus:V16HI (match_operand:V16HI 1 "nonmemory_operand" "r")
                     (mult:V16HI (match_operand:V16HI 2 "register_operand" "r")
                                 (const_int 4))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (minus:V8HI (subreg:V8HI (match_dup 1) 0)
                    (mult:V8HI (subreg:V8HI (match_dup 2) 0)
                               (const_int 4))))
   (set (subreg:V8HI (match_dup 0) 16)
        (minus:V8HI (subreg:V8HI (match_dup 1) 16)
                    (mult:V8HI (subreg:V8HI (match_dup 2) 16)
                               (const_int 4))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx4v16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (minus:V16HI (match_operand:V16HI 1 "nonmemory_operand" "r")
                     (ashift:V16HI (match_operand:V16HI 2 "register_operand" "r")
                                   (const_int 2))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (minus:V8HI (subreg:V8HI (match_dup 1) 0)
                    (ashift:V8HI (subreg:V8HI (match_dup 2) 0)
                                 (const_int 2))))
   (set (subreg:V8HI (match_dup 0) 16)
        (minus:V8HI (subreg:V8HI (match_dup 1) 16)
                    (ashift:V8HI (subreg:V8HI (match_dup 2) 16)
                                 (const_int 2))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx8v16hi"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (minus:V16HI (match_operand:V16HI 1 "nonmemory_operand" "r")
                     (mult:V16HI (match_operand:V16HI 2 "register_operand" "r")
                                 (const_int 8))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (minus:V8HI (subreg:V8HI (match_dup 1) 0)
                    (mult:V8HI (subreg:V8HI (match_dup 2) 0)
                               (const_int 8))))
   (set (subreg:V8HI (match_dup 0) 16)
        (minus:V8HI (subreg:V8HI (match_dup 1) 16)
                    (mult:V8HI (subreg:V8HI (match_dup 2) 16)
                               (const_int 8))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx8v16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (minus:V16HI (match_operand:V16HI 1 "nonmemory_operand" "r")
                     (ashift:V16HI (match_operand:V16HI 2 "register_operand" "r")
                                   (const_int 3))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (minus:V8HI (subreg:V8HI (match_dup 1) 0)
                    (ashift:V8HI (subreg:V8HI (match_dup 2) 0)
                                 (const_int 3))))
   (set (subreg:V8HI (match_dup 0) 16)
        (minus:V8HI (subreg:V8HI (match_dup 1) 16)
                    (ashift:V8HI (subreg:V8HI (match_dup 2) 16)
                                 (const_int 3))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx16v16hi"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (minus:V16HI (match_operand:V16HI 1 "nonmemory_operand" "r")
                     (mult:V16HI (match_operand:V16HI 2 "register_operand" "r")
                                 (const_int 16))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (minus:V8HI (subreg:V8HI (match_dup 1) 0)
                    (mult:V8HI (subreg:V8HI (match_dup 2) 0)
                               (const_int 16))))
   (set (subreg:V8HI (match_dup 0) 16)
        (minus:V8HI (subreg:V8HI (match_dup 1) 16)
                    (mult:V8HI (subreg:V8HI (match_dup 2) 16)
                               (const_int 16))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx16v16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (minus:V16HI (match_operand:V16HI 1 "nonmemory_operand" "r")
                     (ashift:V16HI (match_operand:V16HI 2 "register_operand" "r")
                                   (const_int 4))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (minus:V8HI (subreg:V8HI (match_dup 1) 0)
                    (ashift:V8HI (subreg:V8HI (match_dup 2) 0)
                                 (const_int 4))))
   (set (subreg:V8HI (match_dup 0) 16)
        (minus:V8HI (subreg:V8HI (match_dup 1) 16)
                    (ashift:V8HI (subreg:V8HI (match_dup 2) 16)
                                 (const_int 4))))]
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

(define_insn "sminv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (smin:V16HI (match_operand:V16HI 1 "register_operand" "r")
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "minhq %x0 = %x1, %x2\n\tminhq %y0 = %y1, %y2\n\t"
           "minhq %z0 = %z1, %z2\n\tminhq %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "smaxv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (smax:V16HI (match_operand:V16HI 1 "register_operand" "r")
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "maxhq %x0 = %x1, %x2\n\tmaxhq %y0 = %y1, %y2\n\t"
           "maxhq %z0 = %z1, %z2\n\tmaxhq %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "uminv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (umin:V16HI (match_operand:V16HI 1 "register_operand" "r")
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "minuhq %x0 = %x1, %x2\n\tminuhq %y0 = %y1, %y2\n\t"
           "minuhq %z0 = %z1, %z2\n\tminuhq %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "umaxv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (umax:V16HI (match_operand:V16HI 1 "register_operand" "r")
                    (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "maxuhq %x0 = %x1, %x2\n\tmaxuhq %y0 = %y1, %y2\n\t"
           "maxuhq %z0 = %z1, %z2\n\tmaxuhq %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "andv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (and:V16HI (match_operand:V16HI 1 "register_operand" "r")
                   (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2\n\t"
           "andd %z0 = %z1, %z2\n\tandd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*nandv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (ior:V16HI (not:V16HI (match_operand:V16HI 1 "register_operand" "r"))
                   (not:V16HI (match_operand:V16HI 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nandd %x0 = %x1, %x2\n\tnandd %y0 = %y1, %y2\n\t"
           "nandd %z0 = %z1, %z2\n\tnandd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*andnv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (and:V16HI (not:V16HI (match_operand:V16HI 1 "register_operand" "r"))
                   (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "andnd %x0 = %x1, %x2\n\tandnd %y0 = %y1, %y2\n\t"
           "andnd %z0 = %z1, %z2\n\tandnd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "iorv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (ior:V16HI (match_operand:V16HI 1 "register_operand" "r")
                   (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2\n\t"
           "ord %z0 = %z1, %z2\n\tord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*niorv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (and:V16HI (not:V16HI (match_operand:V16HI 1 "register_operand" "r"))
                   (not:V16HI (match_operand:V16HI 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nord %x0 = %x1, %x2\n\tnord %y0 = %y1, %y2\n\t"
           "nord %z0 = %z1, %z2\n\tnord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*iornv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (ior:V16HI (not:V16HI (match_operand:V16HI 1 "register_operand" "r"))
                   (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "ornd %x0 = %x1, %x2\n\tornd %y0 = %y1, %y2\n\t"
           "ornd %z0 = %z1, %z2\n\tornd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "xorv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (xor:V16HI (match_operand:V16HI 1 "register_operand" "r")
                   (match_operand:V16HI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2\n\t"
           "xord %z0 = %z1, %z2\n\txord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*nxorv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (not:V16HI (xor:V16HI (match_operand:V16HI 1 "register_operand" "r")
                              (match_operand:V16HI 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nxord %x0 = %x1, %x2\n\tnxord %y0 = %y1, %y2\n\t"
           "nxord %z0 = %z1, %z2\n\tnxord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "maddv16hiv16hi4"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (plus:V16HI (mult:V16HI (match_operand:V16HI 1 "register_operand" "r")
                                (match_operand:V16HI 2 "nonmemory_operand" "r"))
                    (match_operand:V16HI 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4HI (match_dup 0) 0)
        (plus:V4HI (mult:V4HI (subreg:V4HI (match_dup 1) 0)
                              (subreg:V4HI (match_dup 2) 0))
                   (subreg:V4HI (match_dup 3) 0)))
   (set (subreg:V4HI (match_dup 0) 8)
        (plus:V4HI (mult:V4HI (subreg:V4HI (match_dup 1) 8)
                              (subreg:V4HI (match_dup 2) 8))
                   (subreg:V4HI (match_dup 3) 8)))
   (set (subreg:V4HI (match_dup 0) 16)
        (plus:V4HI (mult:V4HI (subreg:V4HI (match_dup 1) 16)
                              (subreg:V4HI (match_dup 2) 16))
                   (subreg:V4HI (match_dup 3) 16)))
   (set (subreg:V4HI (match_dup 0) 24)
        (plus:V4HI (mult:V4HI (subreg:V4HI (match_dup 1) 24)
                              (subreg:V4HI (match_dup 2) 24))
                   (subreg:V4HI (match_dup 3) 24)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "msubv16hiv16hi4"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (minus:V16HI (match_operand:V16HI 3 "register_operand" "0")
                     (mult:V16HI (match_operand:V16HI 1 "register_operand" "r")
                                 (match_operand:V16HI 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4HI (match_dup 0) 0)
        (minus:V4HI (subreg:V4HI (match_dup 3) 0)
                    (mult:V4HI (subreg:V4HI (match_dup 1) 0)
                               (subreg:V4HI (match_dup 2) 0))))
   (set (subreg:V4HI (match_dup 0) 8)
        (minus:V4HI (subreg:V4HI (match_dup 3) 8)
                    (mult:V4HI (subreg:V4HI (match_dup 1) 8)
                               (subreg:V4HI (match_dup 2) 8))))
   (set (subreg:V4HI (match_dup 0) 16)
        (minus:V4HI (subreg:V4HI (match_dup 3) 16)
                    (mult:V4HI (subreg:V4HI (match_dup 1) 16)
                               (subreg:V4HI (match_dup 2) 16))))
   (set (subreg:V4HI (match_dup 0) 24)
        (minus:V4HI (subreg:V4HI (match_dup 3) 24)
                    (mult:V4HI (subreg:V4HI (match_dup 1) 24)
                               (subreg:V4HI (match_dup 2) 24))))]
  ""
  [(set_attr "type" "mau_auxr")
   (set_attr "length"      "8")]
)

(define_insn_and_split "ashlv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r,r")
        (ashift:V16HI (match_operand:V16HI 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (ashift:V8HI (subreg:V8HI (match_dup 1) 0)
                     (match_dup 2)))
   (set (subreg:V8HI (match_dup 0) 16)
        (ashift:V8HI (subreg:V8HI (match_dup 1) 16)
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

(define_insn_and_split "ssashlv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r,r")
        (ss_ashift:V16HI (match_operand:V16HI 1 "register_operand" "r,r")
                         (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (ss_ashift:V8HI (subreg:V8HI (match_dup 1) 0)
                        (match_dup 2)))
   (set (subreg:V8HI (match_dup 0) 16)
        (ss_ashift:V8HI (subreg:V8HI (match_dup 1) 16)
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

(define_insn_and_split "ashrv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r,r")
        (ashiftrt:V16HI (match_operand:V16HI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (ashiftrt:V8HI (subreg:V8HI (match_dup 1) 0)
                       (match_dup 2)))
   (set (subreg:V8HI (match_dup 0) 16)
        (ashiftrt:V8HI (subreg:V8HI (match_dup 1) 16)
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

(define_insn_and_split "lshrv16hi3"
  [(set (match_operand:V16HI 0 "register_operand" "=r,r")
        (lshiftrt:V16HI (match_operand:V16HI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (lshiftrt:V8HI (subreg:V8HI (match_dup 1) 0)
                       (match_dup 2)))
   (set (subreg:V8HI (match_dup 0) 16)
        (lshiftrt:V8HI (subreg:V8HI (match_dup 1) 16)
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

(define_insn "negv16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (neg:V16HI (match_operand:V16HI 1 "register_operand" "r")))]
  ""
  {
    return "neghq %x0 = %x1\n\tneghq %y0 = %y1\n\t"
           "neghq %z0 = %z1\n\tneghq %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4_x")
   (set_attr "length"          "32")]
)

(define_insn_and_split "absv16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (abs:V16HI (match_operand:V16HI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (abs:V8HI (subreg:V8HI (match_dup 1) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (abs:V8HI (subreg:V8HI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2_x")]
)

(define_insn_and_split "clrsbv16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (clrsb:V16HI (match_operand:V16HI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (clrsb:V8HI (subreg:V8HI (match_dup 1) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (clrsb:V8HI (subreg:V8HI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "clzv16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (clz:V16HI (match_operand:V16HI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (clz:V8HI (subreg:V8HI (match_dup 1) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (clz:V8HI (subreg:V8HI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "ctzv16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (ctz:V16HI (match_operand:V16HI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (ctz:V8HI (subreg:V8HI (match_dup 1) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (ctz:V8HI (subreg:V8HI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "popcountv16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (popcount:V16HI (match_operand:V16HI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (popcount:V8HI (subreg:V8HI (match_dup 1) 0)))
   (set (subreg:V8HI (match_dup 0) 16)
        (popcount:V8HI (subreg:V8HI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "one_cmplv16hi2"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (not:V16HI (match_operand:V16HI 1 "register_operand" "r")))]
  ""
  {
    return "notd %x0 = %x1\n\tnotd %y0 = %y1\n\t"
           "notd %z0 = %z1\n\tnotd %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; V2SI

(define_insn "*compnwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (match_operator:V2SI 1 "comparison_operator"
         [(match_operand:V2SI 2 "register_operand" "r")
          (match_operand:V2SI 3 "register_operand" "r")]))]
  ""
  "compnwp.%1 %0 = %2, %3"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*cmovewp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (if_then_else:V2SI (match_operator 2 "signed_comparison_operator"
                                               [(match_operand:V2SI 3 "register_operand" "r")
                                                (const_vector:V2SI [
                                                  (const_int 0)
                                                  (const_int 0)])])
                             (match_operand:V2SI 1 "register_operand" "r")
                             (match_operand:V2SI 4 "register_operand" "0")))]
  ""
  "cmovewp.%2z %3? %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_cmovewp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V2SI 1 "register_operand" "0")
                      (match_operand:V2SI 2 "register_operand" "r")
                      (match_operand:V2SI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_CMOVEWP))]
  ""
  "cmovewp%4 %3? %0 = %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "addv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addwp %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,        8,          8")]
)

(define_insn "ssaddv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (ss_plus:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                      (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addswp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx2v2si"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                              (const_int 2))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx2wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx2v2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (ashift:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                                (const_int 1))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx2wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx4v2si"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                              (const_int 4))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx4wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx4v2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (ashift:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                                (const_int 2))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx4wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx8v2si"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                              (const_int 8))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx8wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx8v2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (ashift:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                                (const_int 3))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx8wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx16v2si"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                              (const_int 16))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx16wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx16v2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (ashift:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                                (const_int 4))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx16wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "subv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (match_operand:V2SI 2 "register_operand" "r,r,r")))]
  ""
  "sbfwp %0 = %2, %1"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "sssubv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (ss_minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                       (match_operand:V2SI 2 "register_operand" "r,r,r")))]
  ""
  "sbfswp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx2v2si"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                               (const_int 2))))]
  ""
  "sbfx2wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx2v2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                                 (const_int 1))))]
  ""
  "sbfx2wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx4v2si"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                               (const_int 4))))]
  ""
  "sbfx4wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx4v2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                                 (const_int 2))))]
  ""
  "sbfx4wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx8v2si"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                               (const_int 8))))]
  ""
  "sbfx8wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx8v2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                                 (const_int 3))))]
  ""
  "sbfx8wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx16v2si"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                               (const_int 16))))]
  ""
  "sbfx16wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx16v2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                                 (const_int 4))))]
  ""
  "sbfx16wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "mulv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (mult:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "mulwp %0 = %1, %2"
  [(set_attr "type" "mau,mau_x,mau_x")
   (set_attr "length" "4,    8,    8")]
)

(define_insn "*mulv2siv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (mult:V2DI (sign_extend:V2DI (match_operand:V2SI 1 "register_operand" "r"))
                   (sign_extend:V2DI (match_operand:V2SI 2 "register_operand" "r"))))]
  "0"
  "mulwdp %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "*umulv2siv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (mult:V2DI (zero_extend:V2DI (match_operand:V2SI 1 "register_operand" "r"))
                   (zero_extend:V2DI (match_operand:V2SI 2 "register_operand" "r"))))]
  "0"
  "muluwdp %0 = %1, %2"
  [(set_attr "type" "mau")]
)

(define_insn "sminv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (smin:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "minwp %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "smaxv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (smax:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "maxwp %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "uminv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (umin:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "minuwp %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "umaxv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (umax:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "maxuwp %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny_x,alu_tiny_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "andv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r,r")
        (and:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r,r")
                  (match_operand:V2SI 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "andd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*nandv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r,r")
        (ior:V2SI (not:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r,r"))
                  (not:V2SI (match_operand:V2SI 2 "nonmemory_operand" "r,v10,v37,i"))))]
  ""
  "nandd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*andnv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r,r")
        (and:V2SI (not:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r,r"))
                  (match_operand:V2SI 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "andnd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "iorv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r,r")
        (ior:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r,r")
                  (match_operand:V2SI 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "ord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*niorv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r,r")
        (and:V2SI (not:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r,r"))
                  (not:V2SI (match_operand:V2SI 2 "nonmemory_operand" "r,v10,v37,i"))))]
  ""
  "nord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*iornv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r,r")
        (ior:V2SI (not:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r,r"))
                  (match_operand:V2SI 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "ornd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "xorv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r,r")
        (xor:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r,r")
                  (match_operand:V2SI 2 "nonmemory_operand" "r,v10,v37,i")))]
  ""
  "xord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "*nxorv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r,r")
        (not:V2SI (xor:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r,r")
                            (match_operand:V2SI 2 "nonmemory_operand" "r,v10,v37,i"))))]
  ""
  "nxord %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "maddv2siv2si4"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r,r")
        (plus:V2SI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r,r")
                              (match_operand:V2SI 2 "nonmemory_operand" "r,v10,v37,i"))
                   (match_operand:V2SI 3 "register_operand" "0,0,0,0")))]
  ""
  "maddwp %0 = %1, %2"
  [(set_attr "type" "mau_auxr,mau_auxr,mau_auxr_x,mau_auxr_y")
   (set_attr "length" "     4,       4,         8,        12")]
)

(define_insn "msubv2siv2si4"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (minus:V2SI (match_operand:V2SI 3 "register_operand" "0")
                    (mult:V2SI (match_operand:V2SI 1 "register_operand" "r")
                               (match_operand:V2SI 2 "register_operand" "r"))))]
  ""
  "msbfwp %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "ashlv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r")
        (ashift:V2SI (match_operand:V2SI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sllwps %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "ssashlv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r")
        (ss_ashift:V2SI (match_operand:V2SI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "slswps %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "ashrv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r")
        (ashiftrt:V2SI (match_operand:V2SI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srawps %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "lshrv2si3"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r")
        (lshiftrt:V2SI (match_operand:V2SI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srlwps %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "negv2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (neg:V2SI (match_operand:V2SI 1 "register_operand" "r")))]
  ""
  "negwp %0 = %1"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length" "8")]
)

(define_insn "absv2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (abs:V2SI (match_operand:V2SI 1 "register_operand" "r")))]
  ""
  "abswp %0 = %1"
  [(set_attr "type" "alu_lite_x")
   (set_attr "length" "8")]
)

(define_insn "clrsbv2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (clrsb:V2SI (match_operand:V2SI 1 "register_operand" "r")))]
  ""
  "clswp %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "clzv2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (clz:V2SI (match_operand:V2SI 1 "register_operand" "r")))]
  ""
  "clzwp %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "ctzv2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (ctz:V2SI (match_operand:V2SI 1 "register_operand" "r")))]
  ""
  "ctzwp %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "popcountv2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (popcount:V2SI (match_operand:V2SI 1 "register_operand" "r")))]
  ""
  "cbswp %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "one_cmplv2si2"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (not:V2SI (match_operand:V2SI 1 "register_operand" "r")))]
  ""
  "notd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "extendv2siv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (sign_extend:V2DI (match_operand:V2SI 1 "register_operand" "r")))]
  "0"
  "sxwdp %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "zero_extendv2siv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (zero_extend:V2DI (match_operand:V2SI 1 "register_operand" "r")))]
  "0"
  "zxwdp %0 = %1"
  [(set_attr "type" "alu_tiny")]
)


;; V4SI

(define_insn "*compnwq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (match_operator:V4SI 1 "comparison_operator"
         [(match_operand:V4SI 2 "register_operand" "r")
          (match_operand:V4SI 3 "register_operand" "r")]))]
  ""
  "compnwp.%1 %x0 = %x2, %x3\n\tcompnwp.%1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*cmovewq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (if_then_else:V4SI (match_operator 2 "signed_comparison_operator"
                                               [(match_operand:V4SI 3 "register_operand" "r")
                                                (const_vector:V4SI [
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)])])
                             (match_operand:V4SI 1 "register_operand" "r")
                             (match_operand:V4SI 4 "register_operand" "0")))]
  ""
  "cmovewp.%2z %x3? %x0 = %x1\n\tcmovewp.%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_cmovewq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unspec:V4SI [(match_operand:V4SI 1 "register_operand" "0")
                      (match_operand:V4SI 2 "register_operand" "r")
                      (match_operand:V4SI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_CMOVEWQ))]
  ""
  "cmovewp%4 %x3? %x0 = %x2\n\tcmovewp%4 %y3? %y0 = %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "addv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (match_operand:V4SI 1 "register_operand" "r")
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addwp %x0 = %x1, %x2\n\taddwp %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "ssaddv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (ss_plus:V4SI (match_operand:V4SI 1 "register_operand" "r")
                      (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addswp %x0 = %x1, %x2\n\taddswp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx2v4si"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                              (const_int 2))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx2wp %x0 = %x1, %x2\n\taddx2wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx2v4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (ashift:V4SI (match_operand:V4SI 1 "register_operand" "r")
                                (const_int 1))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx2wp %x0 = %x1, %x2\n\taddx2wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4v4si"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                              (const_int 4))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx4wp %x0 = %x1, %x2\n\taddx4wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4v4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (ashift:V4SI (match_operand:V4SI 1 "register_operand" "r")
                                (const_int 2))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx4wp %x0 = %x1, %x2\n\taddx4wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8v4si"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                              (const_int 8))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx8wp %x0 = %x1, %x2\n\taddx8wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8v4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (ashift:V4SI (match_operand:V4SI 1 "register_operand" "r")
                                (const_int 3))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx8wp %x0 = %x1, %x2\n\taddx8wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16v4si"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                              (const_int 16))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx16wp %x0 = %x1, %x2\n\taddx16wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16v4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (ashift:V4SI (match_operand:V4SI 1 "register_operand" "r")
                                (const_int 4))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx16wp %x0 = %x1, %x2\n\taddx16wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "subv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (match_operand:V4SI 2 "register_operand" "r")))]
  ""
  "sbfwp %x0 = %x2, %x1\n\tsbfwp %y0 = %y2, %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "sssubv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (ss_minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                       (match_operand:V4SI 2 "register_operand" "r")))]
  ""
  "sbfswp %x0 = %x2, %x1\n\tsbfswp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx2v4si"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (mult:V4SI (match_operand:V4SI 2 "register_operand" "r")
                               (const_int 2))))]
  ""
  "sbfx2wp %x0 = %x2, %x1\n\tsbfx2wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx2v4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (ashift:V4SI (match_operand:V4SI 2 "register_operand" "r")
                                 (const_int 1))))]
  ""
  "sbfx2wp %x0 = %x2, %x1\n\tsbfx2wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4v4si"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (mult:V4SI (match_operand:V4SI 2 "register_operand" "r")
                               (const_int 4))))]
  ""
  "sbfx4wp %x0 = %x2, %x1\n\tsbfx4wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4v4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (ashift:V4SI (match_operand:V4SI 2 "register_operand" "r")
                                 (const_int 2))))]
  ""
  "sbfx4wp %x0 = %x2, %x1\n\tsbfx4wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8v4si"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (mult:V4SI (match_operand:V4SI 2 "register_operand" "r")
                               (const_int 8))))]
  ""
  "sbfx8wp %x0 = %x2, %x1\n\tsbfx8wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8v4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (ashift:V4SI (match_operand:V4SI 2 "register_operand" "r")
                                 (const_int 3))))]
  ""
  "sbfx8wp %x0 = %x2, %x1\n\tsbfx8wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16v4si"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (mult:V4SI (match_operand:V4SI 2 "register_operand" "r")
                               (const_int 16))))]
  ""
  "sbfx16wp %x0 = %x2, %x1\n\tsbfx16wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16v4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (ashift:V4SI (match_operand:V4SI 2 "register_operand" "r")
                                 (const_int 4))))]
  ""
  "sbfx16wp %x0 = %x2, %x1\n\tsbfx16wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "mulv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "mulwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "sminv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (smin:V4SI (match_operand:V4SI 1 "register_operand" "r")
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "minwp %x0 = %x1, %x2\n\tminwp %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "smaxv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (smax:V4SI (match_operand:V4SI 1 "register_operand" "r")
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "maxwp %x0 = %x1, %x2\n\tmaxwp %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "uminv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (umin:V4SI (match_operand:V4SI 1 "register_operand" "r")
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "minuwp %x0 = %x1, %x2\n\tminuwp %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "umaxv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (umax:V4SI (match_operand:V4SI 1 "register_operand" "r")
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "maxuwp %x0 = %x1, %x2\n\tmaxuwp %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "andv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (and:V4SI (match_operand:V4SI 1 "register_operand" "r")
                  (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*nandv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (ior:V4SI (not:V4SI (match_operand:V4SI 1 "register_operand" "r"))
                  (not:V4SI (match_operand:V4SI 2 "nonmemory_operand" "r"))))]
  ""
  "nandd %x0 = %x1, %x2\n\tnandd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*andnv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (and:V4SI (not:V4SI (match_operand:V4SI 1 "register_operand" "r"))
                  (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "andnd %x0 = %x1, %x2\n\tandnd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "iorv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (ior:V4SI (match_operand:V4SI 1 "register_operand" "r")
                  (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*niorv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (and:V4SI (not:V4SI (match_operand:V4SI 1 "register_operand" "r"))
                  (not:V4SI (match_operand:V4SI 2 "nonmemory_operand" "r"))))]
  ""
  "nord %x0 = %x1, %x2\n\tnord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*iornv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (ior:V4SI (not:V4SI (match_operand:V4SI 1 "register_operand" "r"))
                  (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "ornd %x0 = %x1, %x2\n\tornd %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "xorv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (xor:V4SI (match_operand:V4SI 1 "register_operand" "r")
                  (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*nxorv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (not:V4SI (xor:V4SI (match_operand:V4SI 1 "register_operand" "r")
                            (match_operand:V4SI 2 "nonmemory_operand" "r"))))]
  ""
  "nxord %x0 = %x1, %x2\n\tnxord %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "maddv4siv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                              (match_operand:V4SI 2 "nonmemory_operand" "r"))
                   (match_operand:V4SI 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (plus:V2SI (mult:V2SI (subreg:V2SI (match_dup 1) 0)
                              (subreg:V2SI (match_dup 2) 0))
                   (subreg:V2SI (match_dup 3) 0)))
   (set (subreg:V2SI (match_dup 0) 8)
        (plus:V2SI (mult:V2SI (subreg:V2SI (match_dup 1) 8)
                              (subreg:V2SI (match_dup 2) 8))
                   (subreg:V2SI (match_dup 3) 8)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "msubv4siv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 3 "register_operand" "0")
                    (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                               (match_operand:V4SI 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (minus:V2SI (subreg:V2SI (match_dup 3) 0)
                    (mult:V2SI (subreg:V2SI (match_dup 1) 0)
                               (subreg:V2SI (match_dup 2) 0))))
   (set (subreg:V2SI (match_dup 0) 8)
        (minus:V2SI (subreg:V2SI (match_dup 3) 8)
                    (mult:V2SI (subreg:V2SI (match_dup 1) 8)
                               (subreg:V2SI (match_dup 2) 8))))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn "ashlv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r,r")
        (ashift:V4SI (match_operand:V4SI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sllwps %x0 = %x1, %2\n\tsllwps %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "ssashlv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r,r")
        (ss_ashift:V4SI (match_operand:V4SI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "slswps %x0 = %x1, %2\n\tslswps %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "ashrv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r,r")
        (ashiftrt:V4SI (match_operand:V4SI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srawps %x0 = %x1, %2\n\tsrawps %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "lshrv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r,r")
        (lshiftrt:V4SI (match_operand:V4SI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srlwps %x0 = %x1, %2\n\tsrlwps %y0 = %y1, %2"
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "negv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (neg:V4SI (match_operand:V4SI 1 "register_operand" "r")))]
  ""
  "negwp %x0 = %x1\n\tnegwp %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2_x")
   (set_attr "length"        "16")]
)

(define_insn "absv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (abs:V4SI (match_operand:V4SI 1 "register_operand" "r")))]
  ""
  "abswp %x0 = %x1\n\tabswp %y0 = %y1"
  [(set_attr "type" "alu_lite_x2_x")
   (set_attr "length"          "16")]
)

(define_insn "clrsbv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (clrsb:V4SI (match_operand:V4SI 1 "register_operand" "r")))]
  ""
  "clswp %x0 = %x1\n\tclswp %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "clzv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (clz:V4SI (match_operand:V4SI 1 "register_operand" "r")))]
  ""
  "clzwp %x0 = %x1\n\tclzwp %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "ctzv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (ctz:V4SI (match_operand:V4SI 1 "register_operand" "r")))]
  ""
  "ctzwp %x0 = %x1\n\tctzwp %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "popcountv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (popcount:V4SI (match_operand:V4SI 1 "register_operand" "r")))]
  ""
  "cbswp %x0 = %x1\n\tcbswp %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "one_cmplv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (not:V4SI (match_operand:V4SI 1 "register_operand" "r")))]
  ""
  "notd %x0 = %x1\n\tnotd %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
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

(define_insn_and_split "*cmovewo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (if_then_else:V8SI (match_operator 2 "signed_comparison_operator"
                                             [(match_operand:V8SI 3 "register_operand" "r")
                                              (const_vector:V8SI [
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)])])
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

(define_insn_and_split "kvx_cmovewo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (unspec:V8SI [(match_operand:V8SI 1 "register_operand" "0")
                      (match_operand:V8SI 2 "register_operand" "r")
                      (match_operand:V8SI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_CMOVEWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (unspec:V4SI [(subreg:V4SI (match_dup 1) 0)
                      (subreg:V4SI (match_dup 2) 0)
                      (subreg:V4SI (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_CMOVEWQ))
   (set (subreg:V4SI (match_dup 0) 16)
        (unspec:V4SI [(subreg:V4SI (match_dup 1) 16)
                      (subreg:V4SI (match_dup 2) 16)
                      (subreg:V4SI (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_CMOVEWQ))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "addv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (plus:V8SI (match_operand:V8SI 1 "register_operand" "r")
                    (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "addwp %x0 = %x1, %x2\n\taddwp %y0 = %y1, %y2\n\t"
           "addwp %z0 = %z1, %z2\n\taddwp %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "ssaddv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (ss_plus:V8SI (match_operand:V8SI 1 "register_operand" "r")
                       (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (ss_plus:V4SI (subreg:V4SI (match_dup 1) 0)
                      (subreg:V4SI (match_dup 2) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (ss_plus:V4SI (subreg:V4SI (match_dup 1) 16)
                      (subreg:V4SI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx2v8si"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (plus:V8SI (mult:V8SI (match_operand:V8SI 1 "register_operand" "r")
                              (const_int 2))
                    (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (plus:V4SI (mult:V4SI (subreg:V4SI (match_dup 1) 0)
                              (const_int 2))
                   (subreg:V4SI (match_dup 2) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (plus:V4SI (mult:V4SI (subreg:V4SI (match_dup 1) 16)
                              (const_int 2))
                   (subreg:V4SI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx2v8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (plus:V8SI (ashift:V8SI (match_operand:V8SI 1 "register_operand" "r")
                                (const_int 1))
                    (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (plus:V4SI (ashift:V4SI (subreg:V4SI (match_dup 1) 0)
                                (const_int 1))
                   (subreg:V4SI (match_dup 2) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (plus:V4SI (ashift:V4SI (subreg:V4SI (match_dup 1) 16)
                                (const_int 1))
                   (subreg:V4SI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx4v8si"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (plus:V8SI (mult:V8SI (match_operand:V8SI 1 "register_operand" "r")
                              (const_int 4))
                    (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (plus:V4SI (mult:V4SI (subreg:V4SI (match_dup 1) 0)
                              (const_int 4))
                   (subreg:V4SI (match_dup 2) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (plus:V4SI (mult:V4SI (subreg:V4SI (match_dup 1) 16)
                              (const_int 4))
                   (subreg:V4SI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx4v8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (plus:V8SI (ashift:V8SI (match_operand:V8SI 1 "register_operand" "r")
                                (const_int 2))
                    (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (plus:V4SI (ashift:V4SI (subreg:V4SI (match_dup 1) 0)
                                (const_int 2))
                   (subreg:V4SI (match_dup 2) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (plus:V4SI (ashift:V4SI (subreg:V4SI (match_dup 1) 16)
                                (const_int 2))
                   (subreg:V4SI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx8v8si"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (plus:V8SI (mult:V8SI (match_operand:V8SI 1 "register_operand" "r")
                              (const_int 8))
                    (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (plus:V4SI (mult:V4SI (subreg:V4SI (match_dup 1) 0)
                              (const_int 8))
                   (subreg:V4SI (match_dup 2) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (plus:V4SI (mult:V4SI (subreg:V4SI (match_dup 1) 16)
                              (const_int 8))
                   (subreg:V4SI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx8v8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (plus:V8SI (ashift:V8SI (match_operand:V8SI 1 "register_operand" "r")
                                (const_int 3))
                    (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (plus:V4SI (ashift:V4SI (subreg:V4SI (match_dup 1) 0)
                                (const_int 3))
                   (subreg:V4SI (match_dup 2) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (plus:V4SI (ashift:V4SI (subreg:V4SI (match_dup 1) 16)
                                (const_int 3))
                   (subreg:V4SI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx16v8si"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (plus:V8SI (mult:V8SI (match_operand:V8SI 1 "register_operand" "r")
                              (const_int 16))
                    (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (plus:V4SI (mult:V4SI (subreg:V4SI (match_dup 1) 0)
                              (const_int 16))
                   (subreg:V4SI (match_dup 2) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (plus:V4SI (mult:V4SI (subreg:V4SI (match_dup 1) 16)
                              (const_int 16))
                   (subreg:V4SI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*addx16v8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (plus:V8SI (ashift:V8SI (match_operand:V8SI 1 "register_operand" "r")
                                (const_int 4))
                    (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (plus:V4SI (ashift:V4SI (subreg:V4SI (match_dup 1) 0)
                                (const_int 4))
                   (subreg:V4SI (match_dup 2) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (plus:V4SI (ashift:V4SI (subreg:V4SI (match_dup 1) 16)
                                (const_int 4))
                   (subreg:V4SI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "subv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (minus:V8SI (match_operand:V8SI 1 "nonmemory_operand" "r")
                     (match_operand:V8SI 2 "register_operand" "r")))]
  ""
  {
    return "sbfwp %x0 = %x2, %x1\n\tsbfwp %y0 = %y2, %y1\n\t"
           "sbfwp %z0 = %z2, %z1\n\tsbfwp %t0 = %t2, %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "sssubv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (ss_minus:V8SI (match_operand:V8SI 1 "nonmemory_operand" "r")
                        (match_operand:V8SI 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (ss_minus:V4SI (subreg:V4SI (match_dup 1) 0)
                       (subreg:V4SI (match_dup 2) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (ss_minus:V4SI (subreg:V4SI (match_dup 1) 16)
                       (subreg:V4SI (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx2v8si"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (minus:V8SI (match_operand:V8SI 1 "nonmemory_operand" "r")
                     (mult:V8SI (match_operand:V8SI 2 "register_operand" "r")
                                (const_int 2))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (minus:V4SI (subreg:V4SI (match_dup 1) 0)
                    (mult:V4SI (subreg:V4SI (match_dup 2) 0)
                               (const_int 2))))
   (set (subreg:V4SI (match_dup 0) 16)
        (minus:V4SI (subreg:V4SI (match_dup 1) 16)
                    (mult:V4SI (subreg:V4SI (match_dup 2) 16)
                               (const_int 2))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx2v8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (minus:V8SI (match_operand:V8SI 1 "nonmemory_operand" "r")
                     (ashift:V8SI (match_operand:V8SI 2 "register_operand" "r")
                                  (const_int 1))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (minus:V4SI (subreg:V4SI (match_dup 1) 0)
                    (ashift:V4SI (subreg:V4SI (match_dup 2) 0)
                                 (const_int 1))))
   (set (subreg:V4SI (match_dup 0) 16)
        (minus:V4SI (subreg:V4SI (match_dup 1) 16)
                    (ashift:V4SI (subreg:V4SI (match_dup 2) 16)
                                 (const_int 1))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx4v8si"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (minus:V8SI (match_operand:V8SI 1 "nonmemory_operand" "r")
                     (mult:V8SI (match_operand:V8SI 2 "register_operand" "r")
                                (const_int 4))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (minus:V4SI (subreg:V4SI (match_dup 1) 0)
                    (mult:V4SI (subreg:V4SI (match_dup 2) 0)
                               (const_int 4))))
   (set (subreg:V4SI (match_dup 0) 16)
        (minus:V4SI (subreg:V4SI (match_dup 1) 16)
                    (mult:V4SI (subreg:V4SI (match_dup 2) 16)
                               (const_int 4))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx4v8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (minus:V8SI (match_operand:V8SI 1 "nonmemory_operand" "r")
                     (ashift:V8SI (match_operand:V8SI 2 "register_operand" "r")
                                  (const_int 2))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (minus:V4SI (subreg:V4SI (match_dup 1) 0)
                    (ashift:V4SI (subreg:V4SI (match_dup 2) 0)
                                 (const_int 2))))
   (set (subreg:V4SI (match_dup 0) 16)
        (minus:V4SI (subreg:V4SI (match_dup 1) 16)
                    (ashift:V4SI (subreg:V4SI (match_dup 2) 16)
                                 (const_int 2))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx8v8si"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (minus:V8SI (match_operand:V8SI 1 "nonmemory_operand" "r")
                     (mult:V8SI (match_operand:V8SI 2 "register_operand" "r")
                                (const_int 8))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (minus:V4SI (subreg:V4SI (match_dup 1) 0)
                    (mult:V4SI (subreg:V4SI (match_dup 2) 0)
                               (const_int 8))))
   (set (subreg:V4SI (match_dup 0) 16)
        (minus:V4SI (subreg:V4SI (match_dup 1) 16)
                    (mult:V4SI (subreg:V4SI (match_dup 2) 16)
                               (const_int 8))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx8v8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (minus:V8SI (match_operand:V8SI 1 "nonmemory_operand" "r")
                     (ashift:V8SI (match_operand:V8SI 2 "register_operand" "r")
                                  (const_int 3))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (minus:V4SI (subreg:V4SI (match_dup 1) 0)
                    (ashift:V4SI (subreg:V4SI (match_dup 2) 0)
                                 (const_int 3))))
   (set (subreg:V4SI (match_dup 0) 16)
        (minus:V4SI (subreg:V4SI (match_dup 1) 16)
                    (ashift:V4SI (subreg:V4SI (match_dup 2) 16)
                                 (const_int 3))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx16v8si"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (minus:V8SI (match_operand:V8SI 1 "nonmemory_operand" "r")
                     (mult:V8SI (match_operand:V8SI 2 "register_operand" "r")
                                (const_int 16))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (minus:V4SI (subreg:V4SI (match_dup 1) 0)
                    (mult:V4SI (subreg:V4SI (match_dup 2) 0)
                               (const_int 16))))
   (set (subreg:V4SI (match_dup 0) 16)
        (minus:V4SI (subreg:V4SI (match_dup 1) 16)
                    (mult:V4SI (subreg:V4SI (match_dup 2) 16)
                               (const_int 16))))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*sbfx16v8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (minus:V8SI (match_operand:V8SI 1 "nonmemory_operand" "r")
                     (ashift:V8SI (match_operand:V8SI 2 "register_operand" "r")
                                  (const_int 4))))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (minus:V4SI (subreg:V4SI (match_dup 1) 0)
                    (ashift:V4SI (subreg:V4SI (match_dup 2) 0)
                                 (const_int 4))))
   (set (subreg:V4SI (match_dup 0) 16)
        (minus:V4SI (subreg:V4SI (match_dup 1) 16)
                    (ashift:V4SI (subreg:V4SI (match_dup 2) 16)
                                 (const_int 4))))]
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
  [(set (subreg:V2SI (match_dup 0) 0)
        (mult:V2SI (subreg:V2SI (match_dup 1) 0)
                   (subreg:V2SI (match_dup 2) 0)))
   (set (subreg:V2SI (match_dup 0) 8)
        (mult:V2SI (subreg:V2SI (match_dup 1) 8)
                   (subreg:V2SI (match_dup 2) 8)))
   (set (subreg:V2SI (match_dup 0) 16)
        (mult:V2SI (subreg:V2SI (match_dup 1) 16)
                   (subreg:V2SI (match_dup 2) 16)))
   (set (subreg:V2SI (match_dup 0) 24)
        (mult:V2SI (subreg:V2SI (match_dup 1) 24)
                   (subreg:V2SI (match_dup 2) 24)))]
  ""
  [(set_attr "type" "mau_auxr")]
)

(define_insn "sminv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (smin:V8SI (match_operand:V8SI 1 "register_operand" "r")
                   (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "minwp %x0 = %x1, %x2\n\tminwp %y0 = %y1, %y2\n\t"
           "minwp %z0 = %z1, %z2\n\tminwp %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "smaxv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (smax:V8SI (match_operand:V8SI 1 "register_operand" "r")
                   (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "maxwp %x0 = %x1, %x2\n\tmaxwp %y0 = %y1, %y2\n\t"
           "maxwp %z0 = %z1, %z2\n\tmaxwp %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "uminv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (umin:V8SI (match_operand:V8SI 1 "register_operand" "r")
                   (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "minuwp %x0 = %x1, %x2\n\tminuwp %y0 = %y1, %y2\n\t"
           "minuwp %z0 = %z1, %z2\n\tminuwp %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "umaxv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (umax:V8SI (match_operand:V8SI 1 "register_operand" "r")
                   (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "maxuwp %x0 = %x1, %x2\n\tmaxuwp %y0 = %y1, %y2\n\t"
           "maxuwp %z0 = %z1, %z2\n\tmaxuwp %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "andv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (and:V8SI (match_operand:V8SI 1 "register_operand" "r")
                   (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "andd %x0 = %x1, %x2\n\tandd %y0 = %y1, %y2\n\t"
           "andd %z0 = %z1, %z2\n\tandd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*nandv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (ior:V8SI (not:V8SI (match_operand:V8SI 1 "register_operand" "r"))
                   (not:V8SI (match_operand:V8SI 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nandd %x0 = %x1, %x2\n\tnandd %y0 = %y1, %y2\n\t"
           "nandd %z0 = %z1, %z2\n\tnandd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*andnv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (and:V8SI (not:V8SI (match_operand:V8SI 1 "register_operand" "r"))
                   (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "andnd %x0 = %x1, %x2\n\tandnd %y0 = %y1, %y2\n\t"
           "andnd %z0 = %z1, %z2\n\tandnd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "iorv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (ior:V8SI (match_operand:V8SI 1 "register_operand" "r")
                   (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "ord %x0 = %x1, %x2\n\tord %y0 = %y1, %y2\n\t"
           "ord %z0 = %z1, %z2\n\tord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*niorv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (and:V8SI (not:V8SI (match_operand:V8SI 1 "register_operand" "r"))
                   (not:V8SI (match_operand:V8SI 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nord %x0 = %x1, %x2\n\tnord %y0 = %y1, %y2\n\t"
           "nord %z0 = %z1, %z2\n\tnord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*iornv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (ior:V8SI (not:V8SI (match_operand:V8SI 1 "register_operand" "r"))
                   (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "ornd %x0 = %x1, %x2\n\tornd %y0 = %y1, %y2\n\t"
           "ornd %z0 = %z1, %z2\n\tornd %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "xorv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (xor:V8SI (match_operand:V8SI 1 "register_operand" "r")
                   (match_operand:V8SI 2 "nonmemory_operand" "r")))]
  ""
  {
    return "xord %x0 = %x1, %x2\n\txord %y0 = %y1, %y2\n\t"
           "xord %z0 = %z1, %z2\n\txord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*nxorv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (not:V8SI (xor:V8SI (match_operand:V8SI 1 "register_operand" "r")
                              (match_operand:V8SI 2 "nonmemory_operand" "r"))))]
  ""
  {
    return "nxord %x0 = %x1, %x2\n\tnxord %y0 = %y1, %y2\n\t"
           "nxord %z0 = %z1, %z2\n\tnxord %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "maddv8siv8si4"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (plus:V8SI (mult:V8SI (match_operand:V8SI 1 "register_operand" "r")
                                (match_operand:V8SI 2 "nonmemory_operand" "r"))
                    (match_operand:V8SI 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
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
  [(set_attr "type" "mau_auxr")]
)

(define_insn_and_split "msubv8siv8si4"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (minus:V8SI (match_operand:V8SI 3 "register_operand" "0")
                     (mult:V8SI (match_operand:V8SI 1 "register_operand" "r")
                                 (match_operand:V8SI 2 "register_operand" "r"))))]
  ""
  "#"
  "reload_completed"
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
  [(set_attr "type" "mau_auxr")
   (set_attr "length"      "8")]
)

(define_insn_and_split "ashlv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r,r")
        (ashift:V8SI (match_operand:V8SI 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (ashift:V4SI (subreg:V4SI (match_dup 1) 0)
                     (match_dup 2)))
   (set (subreg:V4SI (match_dup 0) 16)
        (ashift:V4SI (subreg:V4SI (match_dup 1) 16)
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

(define_insn_and_split "ssashlv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r,r")
        (ss_ashift:V8SI (match_operand:V8SI 1 "register_operand" "r,r")
                         (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (ss_ashift:V4SI (subreg:V4SI (match_dup 1) 0)
                        (match_dup 2)))
   (set (subreg:V4SI (match_dup 0) 16)
        (ss_ashift:V4SI (subreg:V4SI (match_dup 1) 16)
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

(define_insn_and_split "ashrv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r,r")
        (ashiftrt:V8SI (match_operand:V8SI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (ashiftrt:V4SI (subreg:V4SI (match_dup 1) 0)
                       (match_dup 2)))
   (set (subreg:V4SI (match_dup 0) 16)
        (ashiftrt:V4SI (subreg:V4SI (match_dup 1) 16)
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

(define_insn_and_split "lshrv8si3"
  [(set (match_operand:V8SI 0 "register_operand" "=r,r")
        (lshiftrt:V8SI (match_operand:V8SI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (lshiftrt:V4SI (subreg:V4SI (match_dup 1) 0)
                       (match_dup 2)))
   (set (subreg:V4SI (match_dup 0) 16)
        (lshiftrt:V4SI (subreg:V4SI (match_dup 1) 16)
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

(define_insn "negv8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (neg:V8SI (match_operand:V8SI 1 "register_operand" "r")))]
  ""
  {
    return "negwp %x0 = %x1\n\tnegwp %y0 = %y1\n\t"
           "negwp %z0 = %z1\n\tnegwp %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4_x")
   (set_attr "length"          "32")]
)

(define_insn_and_split "absv8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (abs:V8SI (match_operand:V8SI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (abs:V4SI (subreg:V4SI (match_dup 1) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (abs:V4SI (subreg:V4SI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2_x")]
)

(define_insn_and_split "clrsbv8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (clrsb:V8SI (match_operand:V8SI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (clrsb:V4SI (subreg:V4SI (match_dup 1) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (clrsb:V4SI (subreg:V4SI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "clzv8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (clz:V8SI (match_operand:V8SI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (clz:V4SI (subreg:V4SI (match_dup 1) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (clz:V4SI (subreg:V4SI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "ctzv8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (ctz:V8SI (match_operand:V8SI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (ctz:V4SI (subreg:V4SI (match_dup 1) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (ctz:V4SI (subreg:V4SI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "popcountv8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (popcount:V8SI (match_operand:V8SI 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (popcount:V4SI (subreg:V4SI (match_dup 1) 0)))
   (set (subreg:V4SI (match_dup 0) 16)
        (popcount:V4SI (subreg:V4SI (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "one_cmplv8si2"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (not:V8SI (match_operand:V8SI 1 "register_operand" "r")))]
  ""
  {
    return "notd %x0 = %x1\n\tnotd %y0 = %y1\n\t"
           "notd %z0 = %z1\n\tnotd %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
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
                                       (match_dup 3)])] UNSPEC_COMP))
   (set (match_dup 0)
        (neg:V2DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_tiny_x2")]
)

(define_insn "*compdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operator:V2DI 1 "comparison_operator"
                       [(match_operand:V2DI 2 "register_operand" "r")
                        (match_operand:V2DI 3 "register_operand" "r")])] UNSPEC_COMP))]
  ""
  "compd.%1 %x0 = %x2, %x3\n\tcompd.%1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*cmovedp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (if_then_else:V2DI (match_operator 2 "signed_comparison_operator"
                                             [(match_operand:V2DI 3 "register_operand" "r")
                                              (const_vector:V2DI [
                                                (const_int 0)
                                                (const_int 0)])])
                           (match_operand:V2DI 1 "register_operand" "r")
                           (match_operand:V2DI 4 "register_operand" "0")))]
  ""
  "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_cmovedp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2DI 1 "register_operand" "0")
                      (match_operand:V2DI 2 "register_operand" "r")
                      (match_operand:V2DI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_CMOVEDP))]
  ""
  "cmoved%4 %x3? %x0 = %x2\n\tcmoved%4 %y3? %y0 = %y2"
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
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
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
  [(set_attr "type" "alu_lite_x2,alu_lite_x2")
   (set_attr "length" "        8,          8")]
)

(define_insn "lshrv2di3"
  [(set (match_operand:V2DI 0 "register_operand" "=r,r")
        (lshiftrt:V2DI (match_operand:V2DI 1 "register_operand" "r,r")
                       (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srld %x0 = %x1, %2\n\tsrld %y0 = %y1, %2"
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
                                       (match_dup 3)])] UNSPEC_COMP))
   (set (match_dup 0)
        (neg:V4DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_tiny_x4")]
)

(define_insn "*compdq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operator:V4DI 1 "comparison_operator"
                       [(match_operand:V4DI 2 "register_operand" "r")
                        (match_operand:V4DI 3 "register_operand" "r")])] UNSPEC_COMP))]
  ""
  {
    return "compd.%1 %x0 = %x2, %x3\n\tcompd.%1 %y0 = %y2, %y3\n\t"
           "compd.%1 %z0 = %z2, %z3\n\tcompd.%1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*cmovedq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (if_then_else:V4DI (match_operator 2 "signed_comparison_operator"
                                             [(match_operand:V4DI 3 "register_operand" "r")
                                              (const_vector:V4DI [
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)])])
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

(define_insn_and_split "kvx_cmovedq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operand:V4DI 1 "register_operand" "0")
                      (match_operand:V4DI 2 "register_operand" "r")
                      (match_operand:V4DI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_CMOVEDQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 0)
                      (subreg:V2DI (match_dup 2) 0)
                      (subreg:V2DI (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_CMOVEDP))
   (set (subreg:V2DI (match_dup 0) 16)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 16)
                      (subreg:V2DI (match_dup 2) 16)
                      (subreg:V2DI (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_CMOVEDP))]
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

(define_insn_and_split "ashlv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r,r")
        (ashift:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (ashift:V2DI (subreg:V2DI (match_dup 1) 0)
                     (match_dup 2)))
   (set (subreg:V2DI (match_dup 0) 16)
        (ashift:V2DI (subreg:V2DI (match_dup 1) 16)
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

(define_insn_and_split "ashrv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r,r")
        (ashiftrt:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (ashiftrt:V2DI (subreg:V2DI (match_dup 1) 0)
                       (match_dup 2)))
   (set (subreg:V2DI (match_dup 0) 16)
        (ashiftrt:V2DI (subreg:V2DI (match_dup 1) 16)
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

(define_insn_and_split "lshrv4di3"
  [(set (match_operand:V4DI 0 "register_operand" "=r,r")
        (lshiftrt:V4DI (match_operand:V4DI 1 "register_operand" "r,r")
                        (match_operand:SI 2 "sat_shift_operand" "r,U06")))
   (clobber (match_scratch:SI 3 "=&r,X"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (lshiftrt:V2DI (subreg:V2DI (match_dup 1) 0)
                       (match_dup 2)))
   (set (subreg:V2DI (match_dup 0) 16)
        (lshiftrt:V2DI (subreg:V2DI (match_dup 1) 16)
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


;; V2SF

(define_insn "*fcompnwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (match_operator:V2SI 1 "float_comparison_operator"
         [(match_operand:V2SF 2 "register_operand" "r")
          (match_operand:V2SF 3 "register_operand" "r")]))]
  ""
  "fcompnwp.%f1 %0 = %2, %3"
  [(set_attr "type" "alu_lite")]
)

(define_insn "*cmovefwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (if_then_else:V2SF (match_operator 2 "signed_comparison_operator"
                                               [(match_operand:V2SI 3 "register_operand" "r")
                                                (const_vector:V2SI [
                                                  (const_int 0)
                                                  (const_int 0)])])
                             (match_operand:V2SF 1 "register_operand" "r")
                             (match_operand:V2SF 4 "register_operand" "0")))]
  ""
  "cmovewp.%2z %3? %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_cmovefwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "0")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand:V2SI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_CMOVEWP))]
  ""
  "cmovewp%4 %3? %0 = %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "addv2sf3"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (plus:V2SF (match_operand:V2SF 1 "register_operand" "r")
                   (match_operand:V2SF 2 "register_operand" "r")))]
  ""
  "faddwp %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_faddwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDWP))]
  ""
  "faddwp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_faddcwc"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDCWC))]
  ""
  "faddcwc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "subv2sf3"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (minus:V2SF (match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")))]
  ""
  "fsbfwp %0 = %2, %1"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fsbfwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFWP))]
  ""
  "fsbfwp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fsbfcwc"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFCWC))]
  ""
  "fsbfcwc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "mulv2sf3"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (mult:V2SF (match_operand:V2SF 1 "register_operand" "r")
                   (match_operand:V2SF 2 "register_operand" "r")))]
  ""
  "fmulwp %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fmulwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWP))]
  ""
  "fmulwp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
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

(define_insn "kvx_fmulcwc"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULCWC))]
  ""
  "fmulcwc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "fmav2sf4"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (fma:V2SF (match_operand:V2SF 1 "register_operand" "r")
                  (match_operand:V2SF 2 "register_operand" "r")
                  (match_operand:V2SF 3 "register_operand" "0")))]
  ""
  "ffmawp %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_ffmawp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMAWP))]
  ""
  "ffmawp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "fnmav2sf4"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (fma:V2SF (neg:V2SF (match_operand:V2SF 1 "register_operand" "r"))
                  (match_operand:V2SF 2 "register_operand" "r")
                  (match_operand:V2SF 3 "register_operand" "0")))]
  ""
  "ffmswp %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_ffmswp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSWP))]
  ""
  "ffmswp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "fminv2sf3"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (smin:V2SF (match_operand:V2SF 1 "register_operand" "r")
                 (match_operand:V2SF 2 "register_operand" "r")))]
  ""
  "fminwp %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "fmaxv2sf3"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (smax:V2SF (match_operand:V2SF 1 "register_operand" "r")
                   (match_operand:V2SF 2 "register_operand" "r")))]
  ""
  "fmaxwp %0 = %1, %2"
  [(set_attr "type" "alu_lite")]
)

(define_insn "negv2sf2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (neg:V2SF (match_operand:V2SF 1 "register_operand" "r")))]
  ""
  "fnegwp %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "absv2sf2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (abs:V2SF (match_operand:V2SF 1 "register_operand" "r")))]
  ""
  "fabswp %0 = %1"
  [(set_attr "type" "alu_lite")]
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

(define_insn "kvx_fmm2wq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMM2WQ))]
  ""
  "fmm2wq%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fmm2awq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FMM2AWQ))]
  ""
  "fmm2awq%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_fmm2swq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FMM2SWQ))]
  ""
  "fmm2swq%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)


;; V4SF

(define_insn "*fcompnwq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (match_operator:V4SI 1 "float_comparison_operator"
         [(match_operand:V4SF 2 "register_operand" "r")
          (match_operand:V4SF 3 "register_operand" "r")]))]
  ""
  "fcompnwp.%f1 %x0 = %x2, %x3\n\tfcompnwp.%f1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*cmovefwo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (if_then_else:V8SF (match_operator 2 "signed_comparison_operator"
                                             [(match_operand:V8SI 3 "register_operand" "r")
                                              (const_vector:V8SI [
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)])])
                             (match_operand:V8SF 1 "register_operand" "r")
                             (match_operand:V8SF 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (if_then_else:V4SF (match_op_dup 2 [(subreg:V4SI (match_dup 3) 0)
                                            (const_vector:V4SI [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V4SF (match_dup 1) 0)
                           (subreg:V4SF (match_dup 4) 0)))
   (set (subreg:V4SF (match_dup 0) 16)
        (if_then_else:V4SF (match_op_dup 2 [(subreg:V4SI (match_dup 3) 16)
                                            (const_vector:V4SI [
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)
                                              (const_int 0)])])
                           (subreg:V4SF (match_dup 1) 16)
                           (subreg:V4SF (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*cmovefwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (if_then_else:V4SF (match_operator 2 "signed_comparison_operator"
                                               [(match_operand:V4SI 3 "register_operand" "r")
                                                (const_vector:V4SI [
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)
                                                  (const_int 0)])])
                             (match_operand:V4SF 1 "register_operand" "r")
                             (match_operand:V4SF 4 "register_operand" "0")))]
  ""
  "cmovewp.%2z %x3? %x0 = %x1\n\tcmovewp.%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_cmovefwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "0")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V4SI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_CMOVEWQ))]
  ""
  "cmovewp%4 %x3? %x0 = %x2\n\tcmovewp%4 %y3? %y0 = %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
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

(define_insn "kvx_faddcwcp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDCWCP))]
  ""
  "faddcwcp%3 %0 = %1, %2"
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

(define_insn "kvx_fsbfcwcp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFCWCP))]
  ""
  "fsbfcwcp%3 %0 = %1, %2"
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

(define_insn_and_split "kvx_fmulwcp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWCP))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 0)
                      (subreg:V2SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMULWC))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 8)
                      (subreg:V2SF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FMULWC))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fmulcwcp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULCWCP))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 0)
                      (subreg:V2SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMULCWC))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 8)
                      (subreg:V2SF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FMULCWC))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "fmav4sf4"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (fma:V4SF (match_operand:V4SF 1 "register_operand" "r")
                  (match_operand:V4SF 2 "register_operand" "r")
                  (match_operand:V4SF 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (fma:V2SF (subreg:V2SF (match_dup 1) 0)
                  (subreg:V2SF (match_dup 2) 0)
                  (subreg:V2SF (match_dup 3) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (fma:V2SF (subreg:V2SF (match_dup 1) 8)
                  (subreg:V2SF (match_dup 2) 8)
                  (subreg:V2SF (match_dup 3) 8)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_ffmawq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMAWQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 0)
                      (subreg:V2SF (match_dup 2) 0)
                      (subreg:V2SF (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_FFMAWP))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 8)
                      (subreg:V2SF (match_dup 2) 8)
                      (subreg:V2SF (match_dup 3) 8)
                      (match_dup 4)] UNSPEC_FFMAWP))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "fnmav4sf4"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (fma:V4SF (neg:V4SF (match_operand:V4SF 1 "register_operand" "r"))
                  (match_operand:V4SF 2 "register_operand" "r")
                  (match_operand:V4SF 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (fma:V2SF (neg:V2SF (subreg:V2SF (match_dup 1) 0))
                  (subreg:V2SF (match_dup 2) 0)
                  (subreg:V2SF (match_dup 3) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (fma:V2SF (neg:V2SF (subreg:V2SF (match_dup 1) 8))
                  (subreg:V2SF (match_dup 2) 8)
                  (subreg:V2SF (match_dup 3) 8)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_ffmswq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSWQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 0)
                      (subreg:V2SF (match_dup 2) 0)
                      (subreg:V2SF (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_FFMSWP))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 8)
                      (subreg:V2SF (match_dup 2) 8)
                      (subreg:V2SF (match_dup 3) 8)
                      (match_dup 4)] UNSPEC_FFMSWP))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "fminv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (smin:V4SF (match_operand:V4SF 1 "register_operand" "r")
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "fminwp %x0 = %x1, %x2\n\tfminwp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "fmaxv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (smax:V4SF (match_operand:V4SF 1 "register_operand" "r")
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "fmaxwp %x0 = %x1, %x2\n\tfmaxwp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "negv4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (neg:V4SF (match_operand:V4SF 1 "register_operand" "r")))]
  ""
  "fnegwp %x0 = %x1\n\tfnegwp %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "absv4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (abs:V4SF (match_operand:V4SF 1 "register_operand" "r")))]
  ""
  "fabswp %x0 = %x1\n\tfabswp %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
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


;; V8SF

(define_insn_and_split "*fcompnwo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (match_operator:V8SI 1 "float_comparison_operator"
         [(match_operand:V8SF 2 "register_operand" "r")
          (match_operand:V8SF 3 "register_operand" "r")]))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (match_op_dup:V4SI 1
         [(subreg:V4SF (match_dup 2) 0)
          (subreg:V4SF (match_dup 3) 0)]))
   (set (subreg:V4SI (match_dup 0) 16)
        (match_op_dup:V4SI 1
         [(subreg:V4SF (match_dup 2) 16)
          (subreg:V4SF (match_dup 3) 16)]))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "kvx_cmovefwo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "0")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V8SI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_CMOVEWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (subreg:V4SI (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_CMOVEWQ))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (subreg:V4SI (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_CMOVEWQ))]
  ""
  [(set_attr "type" "alu_lite_x2")]
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

(define_insn_and_split "kvx_faddcwcq"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDCWCQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADDCWCP))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADDCWCP))]
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

(define_insn_and_split "kvx_fsbfcwcq"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFCWCQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBFCWCP))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBFCWCP))]
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

(define_insn_and_split "kvx_fmulwcq"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWCQ))]
  ""
  "#"
  "reload_completed"
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
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fmulcwcq"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULCWCQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 0)
                      (subreg:V2SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMULCWC))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 8)
                      (subreg:V2SF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FMULCWC))
   (set (subreg:V2SF (match_dup 0) 16)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 16)
                      (subreg:V2SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMULCWC))
   (set (subreg:V2SF (match_dup 0) 24)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 24)
                      (subreg:V2SF (match_dup 2) 24)
                      (match_dup 3)] UNSPEC_FMULCWC))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "fmav8sf4"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (fma:V8SF (match_operand:V8SF 1 "register_operand" "r")
                  (match_operand:V8SF 2 "register_operand" "r")
                  (match_operand:V8SF 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (fma:V2SF (subreg:V2SF (match_dup 1) 0)
                  (subreg:V2SF (match_dup 2) 0)
                  (subreg:V2SF (match_dup 3) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (fma:V2SF (subreg:V2SF (match_dup 1) 8)
                  (subreg:V2SF (match_dup 2) 8)
                  (subreg:V2SF (match_dup 3) 8)))
   (set (subreg:V2SF (match_dup 0) 16)
        (fma:V2SF (subreg:V2SF (match_dup 1) 16)
                  (subreg:V2SF (match_dup 2) 16)
                  (subreg:V2SF (match_dup 3) 16)))
   (set (subreg:V2SF (match_dup 0) 24)
        (fma:V2SF (subreg:V2SF (match_dup 1) 24)
                  (subreg:V2SF (match_dup 2) 24)
                  (subreg:V2SF (match_dup 3) 24)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_ffmawo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V8SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMAWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 0)
                      (subreg:V2SF (match_dup 2) 0)
                      (subreg:V2SF (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_FFMAWP))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 8)
                      (subreg:V2SF (match_dup 2) 8)
                      (subreg:V2SF (match_dup 3) 8)
                      (match_dup 4)] UNSPEC_FFMAWP))
   (set (subreg:V2SF (match_dup 0) 16)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 16)
                      (subreg:V2SF (match_dup 2) 16)
                      (subreg:V2SF (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_FFMAWP))
   (set (subreg:V2SF (match_dup 0) 24)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 24)
                      (subreg:V2SF (match_dup 2) 24)
                      (subreg:V2SF (match_dup 3) 24)
                      (match_dup 4)] UNSPEC_FFMAWP))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "fnmav8sf4"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (fma:V8SF (neg:V8SF (match_operand:V8SF 1 "register_operand" "r"))
                  (match_operand:V8SF 2 "register_operand" "r")
                  (match_operand:V8SF 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (fma:V2SF (neg:V2SF (subreg:V2SF (match_dup 1) 0))
                  (subreg:V2SF (match_dup 2) 0)
                  (subreg:V2SF (match_dup 3) 0)))
   (set (subreg:V2SF (match_dup 0) 8)
        (fma:V2SF (neg:V2SF (subreg:V2SF (match_dup 1) 8))
                  (subreg:V2SF (match_dup 2) 8)
                  (subreg:V2SF (match_dup 3) 8)))
   (set (subreg:V2SF (match_dup 0) 16)
        (fma:V2SF (neg:V2SF (subreg:V2SF (match_dup 1) 16))
                  (subreg:V2SF (match_dup 2) 16)
                  (subreg:V2SF (match_dup 3) 16)))
   (set (subreg:V2SF (match_dup 0) 24)
        (fma:V2SF (neg:V2SF (subreg:V2SF (match_dup 1) 24))
                  (subreg:V2SF (match_dup 2) 24)
                  (subreg:V2SF (match_dup 3) 24)))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_ffmswo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V8SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSWO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 0)
                      (subreg:V2SF (match_dup 2) 0)
                      (subreg:V2SF (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_FFMSWP))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 8)
                      (subreg:V2SF (match_dup 2) 8)
                      (subreg:V2SF (match_dup 3) 8)
                      (match_dup 4)] UNSPEC_FFMSWP))
   (set (subreg:V2SF (match_dup 0) 16)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 16)
                      (subreg:V2SF (match_dup 2) 16)
                      (subreg:V2SF (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_FFMSWP))
   (set (subreg:V2SF (match_dup 0) 24)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 24)
                      (subreg:V2SF (match_dup 2) 24)
                      (subreg:V2SF (match_dup 3) 24)
                      (match_dup 4)] UNSPEC_FFMSWP))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "fminv8sf3"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (smin:V8SF (match_operand:V8SF 1 "register_operand" "r")
                   (match_operand:V8SF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (smin:V4SF (subreg:V4SF (match_dup 1) 0)
                   (subreg:V4SF (match_dup 2) 0)))
   (set (subreg:V4SF (match_dup 0) 16)
        (smin:V4SF (subreg:V4SF (match_dup 1) 16)
                   (subreg:V4SF (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "fmaxv8sf3"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (smax:V8SF (match_operand:V8SF 1 "register_operand" "r")
                   (match_operand:V8SF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (smax:V4SF (subreg:V4SF (match_dup 1) 0)
                   (subreg:V4SF (match_dup 2) 0)))
   (set (subreg:V4SF (match_dup 0) 16)
        (smax:V4SF (subreg:V4SF (match_dup 1) 16)
                   (subreg:V4SF (match_dup 2) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "negv8sf2"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (neg:V8SF (match_operand:V8SF 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (neg:V4SF (subreg:V4SF (match_dup 1) 0)))
   (set (subreg:V4SF (match_dup 0) 16)
        (neg:V4SF (subreg:V4SF (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "absv8sf2"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (abs:V8SF (match_operand:V8SF 1 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (abs:V4SF (subreg:V4SF (match_dup 1) 0)))
   (set (subreg:V4SF (match_dup 0) 16)
        (abs:V4SF (subreg:V4SF (match_dup 1) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
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
                                       (match_dup 3)])] UNSPEC_FCOMP))
   (set (match_dup 0)
        (neg:V2DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*fcompdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operator:V2DI 1 "float_comparison_operator"
                       [(match_operand:V2DF 2 "register_operand" "r")
                        (match_operand:V2DF 3 "register_operand" "r")])] UNSPEC_FCOMP))]
  ""
  "fcompd.%f1 %x0 = %x2, %x3\n\tfcompd.%f1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*cmovefdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (if_then_else:V2DF (match_operator 2 "signed_comparison_operator"
                                             [(match_operand:V2DI 3 "register_operand" "r")
                                              (const_vector:V2DI [
                                                (const_int 0)
                                                (const_int 0)])])
                           (match_operand:V2DF 1 "register_operand" "r")
                           (match_operand:V2DF 4 "register_operand" "0")))]
  ""
  "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_cmovefdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "0")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand:V2DI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_CMOVEDP))]
  ""
  "cmoved%4 %x3? %x0 = %x2\n\tcmoved%4 %y3? %y0 = %y2"
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

(define_insn "kvx_faddcdc"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDCWC))]
  ""
  "faddcdc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
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

(define_insn "kvx_fsbfcdc"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFCWC))]
  ""
  "fsbfcdc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
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
    rtx real_0 = gen_rtx_SUBREG (DFmode, operands[0], 0);
    rtx imag_0 = gen_rtx_SUBREG (DFmode, operands[0], 8);
    rtx real_1 = gen_rtx_SUBREG (DFmode, operands[1], 0);
    rtx imag_1 = gen_rtx_SUBREG (DFmode, operands[1], 8);
    rtx real_2 = gen_rtx_SUBREG (DFmode, operands[2], 0);
    rtx imag_2 = gen_rtx_SUBREG (DFmode, operands[2], 8);
    rtx real_t = gen_reg_rtx (DFmode), imag_t = gen_reg_rtx (DFmode);
    emit_insn (gen_kvx_fmuld (real_t, real_1, real_2, operands[3]));
    emit_insn (gen_kvx_ffmsd (real_0, imag_1, imag_2, real_t, operands[3]));
    emit_insn (gen_kvx_fmuld (imag_t, real_1, imag_2, operands[3]));
    emit_insn (gen_kvx_ffmad (imag_0, real_2, imag_1, imag_t, operands[3]));
    DONE;
  }
)

(define_expand "kvx_fmulcdc"
  [(set (match_operand:V2DF 0 "register_operand")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand")
                      (match_operand:V2DF 2 "register_operand")
                      (match_operand 3 "" "")] UNSPEC_FMULCDC))]
  ""
  {
    rtx real_0 = gen_rtx_SUBREG (DFmode, operands[0], 0);
    rtx imag_0 = gen_rtx_SUBREG (DFmode, operands[0], 8);
    rtx real_1 = gen_rtx_SUBREG (DFmode, operands[1], 0);
    rtx imag_1 = gen_rtx_SUBREG (DFmode, operands[1], 8);
    rtx real_2 = gen_rtx_SUBREG (DFmode, operands[2], 0);
    rtx imag_2 = gen_rtx_SUBREG (DFmode, operands[2], 8);
    rtx real_t = gen_reg_rtx (DFmode), imag_t = gen_reg_rtx (DFmode);
    emit_insn (gen_kvx_fmuld (real_t, real_1, real_2, operands[3]));
    emit_insn (gen_kvx_ffmad (real_0, real_t, imag_1, imag_2, operands[3]));
    emit_insn (gen_kvx_fmuld (imag_t, real_2, imag_1, operands[3]));
    emit_insn (gen_kvx_ffmsd (imag_0, imag_t, real_1, imag_2, operands[3]));
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

(define_insn_and_split "*cmovefdq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (if_then_else:V4DF (match_operator 2 "signed_comparison_operator"
                                             [(match_operand:V4DI 3 "register_operand" "r")
                                              (const_vector:V4DI [
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)
                                                (const_int 0)])])
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

(define_insn_and_split "kvx_cmovefdq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "0")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand:V4DI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_CMOVEDQ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 0)
                      (subreg:V2DF (match_dup 2) 0)
                      (subreg:V2DI (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_CMOVEDP))
   (set (subreg:V2DF (match_dup 0) 16)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 16)
                      (subreg:V2DF (match_dup 2) 16)
                      (subreg:V2DI (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_CMOVEDP))]
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

(define_insn_and_split "kvx_faddcdcp"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDCDCP))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 0)
                      (subreg:V2DF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADDCWC))
   (set (subreg:V2DF (match_dup 0) 16)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 16)
                      (subreg:V2DF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADDCWC))]
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

(define_insn_and_split "kvx_fsbfcdcp"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFCDCP))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 0)
                      (subreg:V2DF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBFCWC))
   (set (subreg:V2DF (match_dup 0) 16)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 16)
                      (subreg:V2DF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBFCWC))]
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
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULDCP))]
  ""
  {
    rtx real_0 = gen_rtx_SUBREG (V2DFmode, operands[0], 0);
    rtx imag_0 = gen_rtx_SUBREG (V2DFmode, operands[0], 16);
    rtx real_1 = gen_rtx_SUBREG (V2DFmode, operands[1], 0);
    rtx imag_1 = gen_rtx_SUBREG (V2DFmode, operands[1], 16);
    rtx real_2 = gen_rtx_SUBREG (V2DFmode, operands[2], 0);
    rtx imag_2 = gen_rtx_SUBREG (V2DFmode, operands[2], 16);
    rtx real_t = gen_reg_rtx (V2DFmode), imag_t = gen_reg_rtx (V2DFmode);
    emit_insn (gen_kvx_fmuldp (real_t, real_1, real_2, operands[3]));
    emit_insn (gen_kvx_ffmsdp (real_0, imag_1, imag_2, real_t, operands[3]));
    emit_insn (gen_kvx_fmuldp (imag_t, real_1, imag_2, operands[3]));
    emit_insn (gen_kvx_ffmadp (imag_0, real_2, imag_1, imag_t, operands[3]));
    DONE;
  }
)

(define_expand "kvx_fmulcdcp"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULCDCP))]
  ""
  {
    rtx real_0 = gen_rtx_SUBREG (V2DFmode, operands[0], 0);
    rtx imag_0 = gen_rtx_SUBREG (V2DFmode, operands[0], 16);
    rtx real_1 = gen_rtx_SUBREG (V2DFmode, operands[1], 0);
    rtx imag_1 = gen_rtx_SUBREG (V2DFmode, operands[1], 16);
    rtx real_2 = gen_rtx_SUBREG (V2DFmode, operands[2], 0);
    rtx imag_2 = gen_rtx_SUBREG (V2DFmode, operands[2], 16);
    rtx real_t = gen_reg_rtx (V2DFmode), imag_t = gen_reg_rtx (V2DFmode);
    emit_insn (gen_kvx_fmuldp (real_t, real_1, real_2, operands[3]));
    emit_insn (gen_kvx_ffmadp (real_0, real_t, imag_1, imag_2, operands[3]));
    emit_insn (gen_kvx_fmuldp (imag_t, real_2, imag_1, operands[3]));
    emit_insn (gen_kvx_ffmsdp (imag_0, imag_t, real_1, imag_2, operands[3]));
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


;; KVX_MOVETO, KVX_MOVEFO

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


