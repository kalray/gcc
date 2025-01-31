;; CSTORE

(define_insn "cstoresi4"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (match_operator:SI 1 "comparison_operator"
         [(match_operand:SI 2 "register_operand" "r,r")
          (match_operand:SI 3 "kvx_r_any32_operand" "r,i")]))]
  ""
  "compw.%1 %0 = %2, %3"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length"      "4,         8")]
)
;; zero-extend version of cstoresi4
(define_insn "*cstoresi4_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (match_operator:DI 1 "comparison_operator"
         [(match_operand:SI 2 "register_operand" "r,r")
          (match_operand:SI 3 "kvx_r_any32_operand" "r,i")]))]
  ""
  "compw.%1 %0 = %2, %3"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length"      "4,         8")]
)

(define_insn "cstoredi4"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (match_operator:DI 1 "comparison_operator"
         [(match_operand:DI 2 "register_operand" "r,r,r,r")
          (match_operand:DI 3 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")]))]
  ""
  "compd.%1 %0 = %2, %3"
  [(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length"      "4,       4,         8,        12")]
)
;; any-extend versions of cstoredi4
(define_insn "*extend2_cstoredi4"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (match_operator:DI 1 "comparison_operator"
         [(ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r,r"))
          (match_operand:DI 3 "register_s32_operand" "r,B32")]))]
  ""
  "comp<unsx>wd.%1 %0 = %2, %3"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length"      "4,         8")]
)

(define_insn "*extend3_cstoredi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (match_operator:DI 1 "comparison_operator"
          [(match_operand:DI 2 "register_operand" "r")]))
           (ANY_EXTEND:DI (match_operand:SI 3 "register_operand" "r"))]
  ""
  "comp<unsx>wd.%S1 %0 = %3, %2"
  [(set_attr "type" "alu_thin")
   (set_attr "length"      "4")]
)

(define_expand "cstoreti4"
  [(set (match_operand:DI 0 "register_operand" "")
        (match_operator:DI 1 "comparison_operator"
         [(match_operand:TI 2 "register_operand" "")
          (match_operand:TI 3 "nonmemory_operand" "")]))]
  ""
  {
    /* Commit cc01a27db5411a4fe354a97b7c86703c5bc81243 changed the lowering
       strategies for cstore<mode>4.  In order to get the old behavior we
       fail when comparing against 0 or -1 so that the old logic still
       applies.  */
    if (operands[3] == CONST0_RTX (TImode)
        || operands[3] == CONSTM1_RTX (TImode))
      FAIL;
    else
      {
        operands[3] = force_reg (TImode, operands[3]);
        kvx_lower_comparison (operands[0], operands[1], TImode);
        DONE;
      }
  }
)

(define_expand "cstore<mode>4" 
  [(set (match_operand:DI 0 "register_operand" "")
        (match_operator:DI 1 "comparison_operator"
         [(match_operand:ALLF 2 "register_operand" "")
          (match_operand:ALLF 3 "nonmemory_operand" "")]))]
  ""
  {
    kvx_lower_comparison (operands[0], operands[1], <MODE>mode);
    DONE;
  }
)


;; CBRANCH

(define_expand "cbranch<mode>4"
  [(set (pc)
        (if_then_else (match_operator 0 "comparison_operator"
                       [(match_operand:CBRANCH 1 "register_operand")
                        (match_operand:CBRANCH 2 "nonmemory_operand")])
                      (label_ref (match_operand 3 ""))
                      (pc)))]
  ""
  {
    enum mode_class comp_class = GET_MODE_CLASS (<MODE>mode);
    if (comp_class == MODE_INT && operands[2] == const0_rtx
        && zero_comparison_operator (operands[0], VOIDmode)
        && GET_MODE_SIZE (<MODE>mode) <= UNITS_PER_WORD)
      ;
    else
      {
        rtx pred = gen_reg_rtx (kvx_get_predicate_mode (<MODE>mode));
        kvx_lower_comparison (pred, operands[0], <MODE>mode);
        PUT_CODE (operands[0], NE);
        operands[1] = pred;
        operands[2] = const0_rtx;
      }
  }
)


;; CB

(define_insn "*cb<mode>"
  [(set (pc)
        (if_then_else (match_operator 0 "zero_comparison_operator"
                                      [(match_operand:SIDI 1 "register_operand" "r")
                                       (const_int 0)])
                      (label_ref (match_operand 2 "" ""))
                      (pc)))]
  ""
  "cb.<SIDI:suffix>%0z %1? %2"
  [(set_attr "type" "bcu")]
)

(define_insn "*cb<mode>.<EQNE:evenodd>"
  [(set (pc)
        (if_then_else (EQNE (zero_extract:SIDI (match_operand:SIDI 0 "register_operand" "r")
                                               (const_int 1) (const_int 0))
                            (const_int 0))
                      (label_ref (match_operand 1))
                      (pc)))]
  ""
  "cb.<EQNE:evenodd> %0? %1"
  [(set_attr "type" "bcu")]
)


;; *COMP*

(define_insn "*compdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (neg:V2DI (match_operator:V2DI 1 "comparison_operator"
                   [(match_operand:V2DI 2 "register_operand" "r")
                    (match_operand:V2DI 3 "reg_zero_mone_operand" "rS01")])))]
  ""
  "compd.%1 %x0 = %x2, %x3\n\tcompd.%1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*compdp_s2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (neg:V2DI (match_operator:V2DI 1 "comparison_operator"
                   [(vec_duplicate:V2DI (match_operand:DI 2 "nonmemory_operand" "r"))
                    (match_operand:V2DI 3 "reg_zero_mone_operand" "rS01")])))]
  ""
  "compd.%1 %x0 = %2, %x3\n\tcompd.%1 %y0 = %2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*compdp_s3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (neg:V2DI (match_operator:V2DI 1 "comparison_operator"
                   [(match_operand:V2DI 2 "register_operand" "r")
                    (vec_duplicate:V2DI (match_operand:DI 3 "nonmemory_operand" "r"))])))]
  ""
  "compd.%1 %x0 = %x2, %3\n\tcompd.%1 %y0 = %y2, %3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*compdq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (neg:V4DI (match_operator:V4DI 1 "comparison_operator"
                   [(match_operand:V4DI 2 "register_operand" "r")
                     (match_operand:V4DI 3 "reg_zero_mone_operand" "rS01")])))]
  ""
  {
    return "compd.%1 %x0 = %x2, %x3\n\tcompd.%1 %y0 = %y2, %y3\n\t"
           "compd.%1 %z0 = %z2, %z3\n\tcompd.%1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*compdq_s2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (neg:V4DI (match_operator:V4DI 1 "comparison_operator"
                   [(vec_duplicate:V4DI (match_operand:DI 2 "nonmemory_operand" "r"))
                    (match_operand:V4DI 3 "reg_zero_mone_operand" "rS01")])))]
  ""
  {
    return "compd.%1 %x0 = %2, %x3\n\tcompd.%1 %y0 = %2, %y3\n\t"
           "compd.%1 %z0 = %2, %z3\n\tcompd.%1 %t0 = %2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*compdq_s3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (neg:V4DI (match_operator:V4DI 1 "comparison_operator"
                   [(match_operand:V4DI 2 "register_operand" "r")
                    (vec_duplicate:V4DI (match_operand:DI 3 "nonmemory_operand" "r"))])))]
  ""
  {
    return "compd.%1 %x0 = %x2, %3\n\tcompd.%1 %y0 = %y2, %3\n\t"
           "compd.%1 %z0 = %z2, %3\n\tcompd.%1 %t0 = %t2, %3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; *COMPN*

(define_insn "*compn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r,r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S64K 2 "register_operand" "r,r")
          (match_operand:S64K 3 "reg_zero_mone_operand" "r,S01")]))]
  ""
  "compn<suffix>.%1 %0 = %2, %3"
  [(set_attr "type" "alu_tiny,alu_tiny_x")
   (set_attr "length"      "4,         8")]
)

(define_insn "*compn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r,r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S128K 2 "register_operand" "r,r")
          (match_operand:S128K 3 "reg_zero_mone_operand" "r,S01")]))]
  ""
  "compn<chunkx>.%1 %x0 = %x2, %x3\n\tcompn<chunkx>.%1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2,alu_tiny_x2_x")
   (set_attr "length"         "8,           16")]
)

(define_insn "*compn<suffix>_s2"
  [(set (match_operand:<MASK> 0 "register_operand" "=r,r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(vec_duplicate:S128K (match_operand:<CHUNK> 2 "nonmemory_operand" "r,r"))
          (match_operand:S128K 3 "reg_zero_mone_operand" "r,S01")]))]
  ""
  "compn<chunkx>.%1 %x0 = %2, %x3\n\tcompn<chunkx>.%1 %y0 = %2, %y3"
  [(set_attr "type" "alu_tiny_x2,alu_tiny_x2_x")
   (set_attr "length"         "8,           16")]
)

(define_insn "*compn<suffix>_s3"
  [(set (match_operand:<MASK> 0 "register_operand" "=r,r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S128K 2 "register_operand" "r,r")
          (vec_duplicate:S128K (match_operand:<CHUNK> 3 "nonmemory_operand" "r,r"))]))]
  ""
  "compn<chunkx>.%1 %x0 = %x2, %3\n\tcompn<chunkx>.%1 %y0 = %y2, %3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*compndp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "comparison_operator"
         [(match_operand:V2DI 2 "register_operand" "r")
          (match_operand:V2DI 3 "reg_zero_mone_operand" "rS01")]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 0)
        (neg:V2DI (match_op_dup 1 [(match_dup 2) (match_dup 3)])))
   (set (match_dup 0)
        (neg:V2DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_tiny_x2")]
)

(define_insn "*compndp"
  [(set (match_operand:V2DI 0 "register_operand" "=r,r")
        (match_operator:V2DI 1 "comparison_operator"
         [(match_operand:V2DI 2 "register_operand" "r,r")
          (match_operand:V2DI 3 "reg_zero_mone_operand" "r,S01")]))]
  "(KV3_2||KV4)"
  "compnd.%1 %x0 = %x2, %x3\n\tcompnd.%1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2, alu_tiny_x2_x")
   (set_attr "length"         "8,            16")]
)

(define_insn_and_split "*compndp_s2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "comparison_operator"
         [(vec_duplicate:V2DI (match_operand:DI 2 "nonmemory_operand" "r"))
          (match_operand:V2DI 3 "reg_zero_mone_operand" "rS01")]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 0)
        (neg:V2DI (match_op_dup 1 [(vec_duplicate:V2DI (match_dup 2)) (match_dup 3)])))
   (set (match_dup 0)
        (neg:V2DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_tiny_x2")]
)

(define_insn "*compndp_s2"
  [(set (match_operand:V2DI 0 "register_operand" "=r,r")
        (match_operator:V2DI 1 "comparison_operator"
         [(vec_duplicate:V2DI (match_operand:DI 2 "nonmemory_operand" "r,r"))
          (match_operand:V2DI 3 "reg_zero_mone_operand" "r,S01")]))]
  "(KV3_2||KV4)"
  "compnd.%1 %x0 = %2, %x3\n\tcompnd.%1 %y0 = %2, %y3"
  [(set_attr "type" "alu_tiny_x2,alu_tiny_x2_x")
   (set_attr "length"         "8,           16")]
)

(define_insn_and_split "*compndp_s3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "comparison_operator"
         [(match_operand:V2DI 2 "register_operand" "r")
          (vec_duplicate:V2DI (match_operand:DI 3 "nonmemory_operand" "r"))]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 0)
        (neg:V2DI (match_op_dup 1 [(match_dup 2) (vec_duplicate:V2DI (match_dup 3))])))
   (set (match_dup 0)
        (neg:V2DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_tiny_x2")]
)

(define_insn "*compndp_s3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "comparison_operator"
         [(match_operand:V2DI 2 "register_operand" "r")
          (vec_duplicate:V2DI (match_operand:DI 3 "nonmemory_operand" "r"))]))]
  "(KV3_2||KV4)"
  "compnd.%1 %x0 = %x2, %3\n\tcompnd.%1 %y0 = %y2, %3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*compn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r,r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S256K 2 "register_operand" "r,r")
          (match_operand:S256K 3 "reg_zero_mone_operand" "r,S01")]))]
  ""
  {
    return "compn<chunkx>.%1 %x0 = %x2, %x3\n\tcompn<chunkx>.%1 %y0 = %y2, %y3\n\t"
           "compn<chunkx>.%1 %z0 = %z2, %z3\n\tcompn<chunkx>.%1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4,alu_tiny_x4_x")
   (set_attr "length"        "16,           32")]
)

(define_insn "*compn<suffix>_s2"
  [(set (match_operand:<MASK> 0 "register_operand" "=r,r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(vec_duplicate:S256K (match_operand:<CHUNK> 2 "nonmemory_operand" "r,r"))
          (match_operand:S256K 3 "reg_zero_mone_operand" "r,S01")]))]
  ""
  {
    return "compn<chunkx>.%1 %x0 = %2, %x3\n\tcompn<chunkx>.%1 %y0 = %2, %y3\n\t"
           "compn<chunkx>.%1 %z0 = %2, %z3\n\tcompn<chunkx>.%1 %t0 = %2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4,alu_tiny_x4_x")
   (set_attr "length"        "16,           32")]
)

(define_insn "*compn<suffix>_s3"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S256K 2 "register_operand" "r")
          (vec_duplicate:S256K (match_operand:<CHUNK> 3 "nonmemory_operand" "r"))]))]
  ""
  {
    return "compn<chunkx>.%1 %x0 = %x2, %3\n\tcompn<chunkx>.%1 %y0 = %y2, %3\n\t"
           "compn<chunkx>.%1 %z0 = %z2, %3\n\tcompn<chunkx>.%1 %t0 = %t2, %3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*compndq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (match_operator:V4DI 1 "comparison_operator"
         [(match_operand:V4DI 2 "register_operand" "r")
          (match_operand:V4DI 3 "reg_zero_mone_operand" "rS01")]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 0)
        (neg:V4DI (match_op_dup 1 [(match_dup 2) (match_dup 3)])))
   (set (match_dup 0)
        (neg:V4DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_tiny_x4")]
)

(define_insn "*compndq"
  [(set (match_operand:V4DI 0 "register_operand" "=r,r")
        (match_operator:V4DI 1 "comparison_operator"
         [(match_operand:V4DI 2 "register_operand" "r,r")
          (match_operand:V4DI 3 "reg_zero_mone_operand" "r,S01")]))]
  "(KV3_2||KV4)"
  {
    return "compnd.%1 %x0 = %x2, %x3\n\tcompnd.%1 %y0 = %y2, %y3\n\t"
           "compnd.%1 %z0 = %z2, %z3\n\tcompnd.%1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4,alu_tiny_x4_x")
   (set_attr "length"        "16,           32")]
)

(define_insn_and_split "*compndq_s2"
  [(set (match_operand:V4DI 0 "register_operand" "=&r")
        (match_operator:V4DI 1 "comparison_operator"
         [(vec_duplicate:V4DI (match_operand:DI 2 "nonmemory_operand" "r"))
          (match_operand:V4DI 3 "reg_zero_mone_operand" "rS01")]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 0)
        (neg:V4DI (match_op_dup 1 [(vec_duplicate:V4DI (match_dup 2)) (match_dup 3)])))
   (set (match_dup 0)
        (neg:V4DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_tiny_x4")]
)

(define_insn "*compndq_s2"
  [(set (match_operand:V4DI 0 "register_operand" "=r,r")
        (match_operator:V4DI 1 "comparison_operator"
         [(vec_duplicate:V4DI (match_operand:DI 2 "nonmemory_operand" "r,r"))
          (match_operand:V4DI 3 "reg_zero_mone_operand" "r,S01")]))]
  "(KV3_2||KV4)"
  {
    return "compnd.%1 %x0 = %2, %x3\n\tcompnd.%1 %y0 = %2, %y3\n\t"
           "compnd.%1 %z0 = %2, %z3\n\tcompnd.%1 %t0 = %2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4,alu_tiny_x4_x")
   (set_attr "length"        "16,           32")]
)

(define_insn_and_split "*compndq_s3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (match_operator:V4DI 1 "comparison_operator"
         [(match_operand:V4DI 2 "register_operand" "r")
          (vec_duplicate:V4DI (match_operand:DI 3 "nonmemory_operand" "r"))]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 0)
        (neg:V4DI (match_op_dup 1 [(match_dup 2) (vec_duplicate:V4DI (match_dup 3))])))
   (set (match_dup 0)
        (neg:V4DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_tiny_x4")]
)

(define_insn "*compndq_s3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (match_operator:V4DI 1 "comparison_operator"
         [(match_operand:V4DI 2 "register_operand" "r")
          (vec_duplicate:V4DI (match_operand:DI 3 "nonmemory_operand" "r"))]))]
  "(KV3_2||KV4)"
  {
    return "compnd.%1 %x0 = %x2, %3\n\tcompnd.%1 %y0 = %y2, %3\n\t"
           "compnd.%1 %z0 = %z2, %3\n\tcompnd.%1 %t0 = %t2, %3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*compn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=&r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:V512L 2 "register_operand" "r")
          (match_operand:V512L 3 "register_operand" "r")]))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QMASK> (match_dup 0) 0)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 0)
          (subreg:<QUART> (match_dup 3) 0)]))
   (set (subreg:<QMASK> (match_dup 0) 16)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 16)
          (subreg:<QUART> (match_dup 3) 16)]))
   (set (subreg:<QMASK> (match_dup 0) 32)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 32)
          (subreg:<QUART> (match_dup 3) 32)]))
   (set (subreg:<QMASK> (match_dup 0) 48)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 48)
          (subreg:<QUART> (match_dup 3) 48)]))]
  ""
)

(define_insn_and_split "*compn<suffix>_s2"
  [(set (match_operand:<MASK> 0 "register_operand" "=&r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(vec_duplicate:V512L (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))
          (match_operand:V512L 3 "register_operand" "r")]))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QMASK> (match_dup 0) 0)
        (match_op_dup:<QMASK> 1
         [(vec_duplicate:<QUART> (match_dup 2))
          (subreg:<QUART> (match_dup 3) 0)]))
   (set (subreg:<QMASK> (match_dup 0) 16)
        (match_op_dup:<QMASK> 1
         [(vec_duplicate:<QUART> (match_dup 2))
          (subreg:<QUART> (match_dup 3) 16)]))
   (set (subreg:<QMASK> (match_dup 0) 32)
        (match_op_dup:<QMASK> 1
         [(vec_duplicate:<QUART> (match_dup 2))
          (subreg:<QUART> (match_dup 3) 32)]))
   (set (subreg:<QMASK> (match_dup 0) 48)
        (match_op_dup:<QMASK> 1
         [(vec_duplicate:<QUART> (match_dup 2))
          (subreg:<QUART> (match_dup 3) 48)]))]
  ""
)

(define_insn_and_split "*compn<suffix>_s3"
  [(set (match_operand:<MASK> 0 "register_operand" "=&r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:V512L 2 "register_operand" "r")
          (vec_duplicate:V512L (match_operand:<CHUNK> 3 "register_operand" "r"))]))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QMASK> (match_dup 0) 0)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 0)
          (vec_duplicate:<QUART> (match_dup 3))]))
   (set (subreg:<QMASK> (match_dup 0) 16)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 16)
          (vec_duplicate:<QUART> (match_dup 3))]))
   (set (subreg:<QMASK> (match_dup 0) 32)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 32)
          (vec_duplicate:<QUART> (match_dup 3))]))
   (set (subreg:<QMASK> (match_dup 0) 48)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 48)
          (vec_duplicate:<QUART> (match_dup 3))]))]
  ""
)


;; *FCOMP*

(define_insn "*fcomp<ALLF:suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r")
        (match_operator:SIDI 1 "float_comparison_operator"
         [(match_operand:ALLF 2 "register_operand" "r,r")
          (match_operand:ALLF 3 "register_f32_operand" "r,H32")]))]
  ""
  "fcomp<ALLF:suffix>.%F1 %0 = %2, %3"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length"      "4,         8")]
)

(define_insn "*fcompdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (neg:V2DI (match_operator:V2DI 1 "float_comparison_operator"
                   [(match_operand:V2DF 2 "register_operand" "r")
                    (match_operand:V2DF 3 "register_operand" "r")])))]
  ""
  "fcompd.%F1 %x0 = %x2, %x3\n\tfcompd.%F1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*fcompdp_s2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (neg:V2DI (match_operator:V2DI 1 "float_comparison_operator"
                   [(vec_duplicate:V2DF (match_operand:DF 2 "nonmemory_operand" "r"))
                    (match_operand:V2DF 3 "register_operand" "r")])))]
  ""
  "fcompd.%F1 %x0 = %2, %x3\n\tfcompd.%F1 %y0 = %2, %y3"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*fcompdp_s3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (neg:V2DI (match_operator:V2DI 1 "float_comparison_operator"
                   [(match_operand:V2DF 2 "register_operand" "r")
                    (vec_duplicate:V2DF (match_operand:DF 3 "nonmemory_operand" "r"))])))]
  ""
  "fcompd.%F1 %x0 = %x2, %3\n\tfcompd.%F1 %y0 = %y2, %3"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*fcompdq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (neg:V4DI (match_operator:V4DI 1 "float_comparison_operator"
                   [(match_operand:V4DF 2 "register_operand" "r")
                     (match_operand:V4DF 3 "register_operand" "r")])))]
  "(KV3_2||KV4)"
  {
    return "fcompd.%F1 %x0 = %x2, %x3\n\tfcompd.%F1 %y0 = %y2, %y3\n\t"
           "fcompd.%F1 %z0 = %z2, %z3\n\tfcompd.%F1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*fcompdq_s2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (neg:V4DI (match_operator:V4DI 1 "float_comparison_operator"
                   [(vec_duplicate:V4DF (match_operand:DI 2 "nonmemory_operand" "r"))
                    (match_operand:V4DF 3 "register_operand" "r")])))]
  "(KV3_2||KV4)"
  {
    return "fcompd.%F1 %x0 = %2, %x3\n\tfcompd.%F1 %y0 = %2, %y3\n\t"
           "fcompd.%F1 %z0 = %2, %z3\n\tfcompd.%F1 %t0 = %2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*fcompdq_s3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (neg:V4DI (match_operator:V4DI 1 "float_comparison_operator"
                   [(match_operand:V4DF 2 "register_operand" "r")
                    (vec_duplicate:V4DF (match_operand:DI 3 "nonmemory_operand" "r"))])))]
  "(KV3_2||KV4)"
  {
    return "fcompd.%F1 %x0 = %x2, %3\n\tfcompd.%F1 %y0 = %y2, %3\n\t"
           "fcompd.%F1 %z0 = %z2, %3\n\tfcompd.%F1 %t0 = %t2, %3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; *FCOMPN*

(define_insn "*fcompn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(match_operand:S64F 2 "register_operand" "r")
          (match_operand:S64F 3 "register_operand" "r")]))]
  ""
  "fcompn<suffix>.%F1 %0 = %2, %3"
  [(set_attr "type" "alu_thin")]
)

(define_insn "*fcompn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(match_operand:S128F 2 "register_operand" "r")
          (match_operand:S128F 3 "register_operand" "r")]))]
  ""
  "fcompn<chunkx>.%F1 %x0 = %x2, %x3\n\tfcompn<chunkx>.%F1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*fcompn<suffix>_s2"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(vec_duplicate:S128F (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))
          (match_operand:S128F 3 "register_operand" "r")]))]
  ""
  "fcompn<chunkx>.%F1 %x0 = %2, %x3\n\tfcompn<chunkx>.%F1 %y0 = %2, %y3"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*fcompn<suffix>_s3"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(match_operand:S128F 2 "register_operand" "r")
          (vec_duplicate:S128F (match_operand:<CHUNK> 3 "nonmemory_operand" "r"))]))]
  ""
  "fcompn<chunkx>.%F1 %x0 = %x2, %3\n\tfcompn<chunkx>.%F1 %y0 = %y2, %3"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*fcompndp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "float_comparison_operator"
         [(match_operand:V2DF 2 "register_operand" "r")
          (match_operand:V2DF 3 "register_operand" "r")]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 0)
        (neg:V2DI (match_op_dup 1 [(match_dup 2) (match_dup 3)])))
   (set (match_dup 0)
        (neg:V2DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*fcompndp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "float_comparison_operator"
         [(match_operand:V2DF 2 "register_operand" "r")
          (match_operand:V2DF 3 "register_operand" "r")]))]
  "(KV3_2||KV4)"
  "fcompnd.%F1 %x0 = %x2, %x3\n\tfcompnd.%F1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*fcompndp_s2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "float_comparison_operator"
         [(vec_duplicate:V2DF (match_operand:DF 2 "nonmemory_operand" "r"))
          (match_operand:V2DF 3 "register_operand" "r")]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 0)
        (neg:V2DI (match_op_dup 1 [(vec_duplicate:V2DF (match_dup 2)) (match_dup 3)])))
   (set (match_dup 0)
        (neg:V2DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*fcompndp_s2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "float_comparison_operator"
         [(vec_duplicate:V2DF (match_operand:DF 2 "nonmemory_operand" "r"))
          (match_operand:V2DF 3 "register_operand" "r")]))]
  "(KV3_2||KV4)"
  "fcompnd.%F1 %x0 = %2, %x3\n\tfcompnd.%F1 %y0 = %2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*fcompndp_s3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "float_comparison_operator"
         [(match_operand:V2DF 2 "register_operand" "r")
          (vec_duplicate:V2DF (match_operand:DF 3 "nonmemory_operand" "r"))]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (match_dup 0)
        (neg:V2DI (match_op_dup 1 [(match_dup 2) (vec_duplicate:V2DF (match_dup 3))])))
   (set (match_dup 0)
        (neg:V2DI (match_dup 0)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*fcompndp_s3"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "float_comparison_operator"
         [(match_operand:V2DF 2 "register_operand" "r")
          (vec_duplicate:V2DF (match_operand:DF 3 "nonmemory_operand" "r"))]))]
  "(KV3_2||KV4)"
  "fcompnd.%F1 %x0 = %x2, %3\n\tfcompnd.%F1 %y0 = %y2, %3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*fcompn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(match_operand:S256F 2 "register_operand" "r")
          (match_operand:S256F 3 "register_operand" "r")]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
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

(define_insn "*fcompn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(match_operand:S256F 2 "register_operand" "r")
          (match_operand:S256F 3 "register_operand" "r")]))]
  "(KV3_2||KV4)"
  {
    return "fcompn<chunkx>.%F1 %x0 = %x2, %x3\n\tfcompn<chunkx>.%F1 %y0 = %y2, %y3\n\t"
           "fcompn<chunkx>.%F1 %z0 = %z2, %z3\n\tfcompn<chunkx>.%F1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*fcompn<suffix>_s2"
  [(set (match_operand:<MASK> 0 "register_operand" "=&r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(vec_duplicate:S256F (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))
          (match_operand:S256F 3 "register_operand" "r")]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HMASK> (match_dup 0) 0)
        (match_op_dup:<HMASK> 1
         [(vec_duplicate:<HALF> (match_dup 2))
          (subreg:<HALF> (match_dup 3) 0)]))
   (set (subreg:<HMASK> (match_dup 0) 16)
        (match_op_dup:<HMASK> 1
         [(vec_duplicate:<HALF> (match_dup 2))
          (subreg:<HALF> (match_dup 3) 16)]))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*fcompn<suffix>_s2"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(vec_duplicate:S256F (match_operand:<CHUNK> 2 "register_operand" "r"))
          (match_operand:S256F 3 "register_operand" "r")]))]
  "(KV3_2||KV4)"
  {
    return "fcompn<chunkx>.%F1 %x0 = %2, %x3\n\tfcompn<chunkx>.%F1 %y0 = %2, %y3\n\t"
           "fcompn<chunkx>.%F1 %z0 = %2, %z3\n\tfcompn<chunkx>.%F1 %t0 = %2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*fcompn<suffix>_s3"
  [(set (match_operand:<MASK> 0 "register_operand" "=&r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(match_operand:S256F 2 "register_operand" "r")
          (vec_duplicate:S256F (match_operand:<CHUNK> 3 "register_operand" "r"))]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HMASK> (match_dup 0) 0)
        (match_op_dup:<HMASK> 1
         [(subreg:<HALF> (match_dup 2) 0)
          (vec_duplicate:<HALF> (match_dup 3))]))
   (set (subreg:<HMASK> (match_dup 0) 16)
        (match_op_dup:<HMASK> 1
         [(subreg:<HALF> (match_dup 2) 16)
          (vec_duplicate:<HALF> (match_dup 3))]))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*fcompn<suffix>_s3"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(match_operand:S256F 2 "register_operand" "r")
          (vec_duplicate:S256F (match_operand:<CHUNK> 3 "register_operand" "r"))]))]
  "(KV3_2||KV4)"
  {
    return "fcompn<chunkx>.%F1 %x0 = %x2, %3\n\tfcompn<chunkx>.%F1 %y0 = %y2, %3\n\t"
           "fcompn<chunkx>.%F1 %z0 = %z2, %3\n\tfcompn<chunkx>.%F1 %t0 = %t2, %3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*fcompndq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (match_operator:V4DI 1 "float_comparison_operator"
         [(match_operand:V4DF 2 "register_operand" "r")
          (match_operand:V4DF 3 "register_operand" "r")]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
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

(define_insn "*fcompndq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (match_operator:V4DI 1 "float_comparison_operator"
         [(match_operand:V4DF 2 "register_operand" "r")
          (match_operand:V4DF 3 "register_operand" "r")]))]
  "(KV3_2||KV4)"
  {
    return "fcompnd.%F1 %x0 = %x2, %x3\n\tfcompnd.%F1 %y0 = %y2, %y3\n\t"
           "fcompnd.%F1 %z0 = %z2, %z3\n\tfcompnd.%F1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*fcompndq_s2"
  [(set (match_operand:V4DI 0 "register_operand" "=&r")
        (match_operator:V4DI 1 "float_comparison_operator"
         [(vec_duplicate:V4DF (match_operand:DF 2 "nonmemory_operand" "r"))
          (match_operand:V4DF 3 "register_operand" "r")]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (match_op_dup:V2DI 1
         [(vec_duplicate:V2DF (match_dup 2))
          (subreg:V2DF (match_dup 3) 0)]))
   (set (subreg:V2DI (match_dup 0) 16)
        (match_op_dup:V2DI 1
         [(vec_duplicate:V2DF (match_dup 2))
          (subreg:V2DF (match_dup 3) 16)]))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*fcompndq_s2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (match_operator:V4DI 1 "comparison_operator"
         [(vec_duplicate:V4DF (match_operand:DF 2 "nonmemory_operand" "r"))
          (match_operand:V4DF 3 "register_operand" "r")]))]
  "(KV3_2||KV4)"
  {
    return "fcompnd.%F1 %x0 = %2, %x3\n\tfcompnd.%F1 %y0 = %2, %y3\n\t"
           "fcompnd.%F1 %z0 = %2, %z3\n\tfcompnd.%F1 %t0 = %2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*fcompndq_s3"
  [(set (match_operand:V4DI 0 "register_operand" "=&r")
        (match_operator:V4DI 1 "float_comparison_operator"
         [(match_operand:V4DF 2 "register_operand" "r")
          (vec_duplicate:V4DF (match_operand:DF 3 "nonmemory_operand" "r"))]))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (match_op_dup:V2DI 1
         [(subreg:V2DF (match_dup 2) 0)
          (vec_duplicate:V2DF (match_dup 3))]))
   (set (subreg:V2DI (match_dup 0) 16)
        (match_op_dup:V2DI 1
         [(subreg:V2DF (match_dup 2) 16)
          (vec_duplicate:V2DF (match_dup 3))]))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*fcompndq_s3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (match_operator:V4DI 1 "comparison_operator"
         [(match_operand:V4DF 2 "register_operand" "r")
          (vec_duplicate:V4DF (match_operand:DF 3 "nonmemory_operand" "r"))]))]
  "(KV3_2||KV4)"
  {
    return "fcompnd.%F1 %x0 = %x2, %3\n\tfcompnd.%F1 %y0 = %y2, %3\n\t"
           "fcompnd.%F1 %z0 = %z2, %3\n\tfcompnd.%F1 %t0 = %t2, %3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*fcompn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=&r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(match_operand:V512F 2 "register_operand" "r")
          (match_operand:V512F 3 "register_operand" "r")]))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QMASK> (match_dup 0) 0)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 0)
          (subreg:<QUART> (match_dup 3) 0)]))
   (set (subreg:<QMASK> (match_dup 0) 16)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 16)
          (subreg:<QUART> (match_dup 3) 16)]))
   (set (subreg:<QMASK> (match_dup 0) 32)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 32)
          (subreg:<QUART> (match_dup 3) 32)]))
   (set (subreg:<QMASK> (match_dup 0) 48)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 48)
          (subreg:<QUART> (match_dup 3) 48)]))]
  ""
)

(define_insn_and_split "*fcompn<suffix>_s2"
  [(set (match_operand:<MASK> 0 "register_operand" "=&r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(vec_duplicate:V512F (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))
          (match_operand:V512F 3 "register_operand" "r")]))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QMASK> (match_dup 0) 0)
        (match_op_dup:<QMASK> 1
         [(vec_duplicate:<QUART> (match_dup 2))
          (subreg:<QUART> (match_dup 3) 0)]))
   (set (subreg:<QMASK> (match_dup 0) 16)
        (match_op_dup:<QMASK> 1
         [(vec_duplicate:<QUART> (match_dup 2))
          (subreg:<QUART> (match_dup 3) 16)]))
   (set (subreg:<QMASK> (match_dup 0) 32)
        (match_op_dup:<QMASK> 1
         [(vec_duplicate:<QUART> (match_dup 2))
          (subreg:<QUART> (match_dup 3) 32)]))
   (set (subreg:<QMASK> (match_dup 0) 48)
        (match_op_dup:<QMASK> 1
         [(vec_duplicate:<QUART> (match_dup 2))
          (subreg:<QUART> (match_dup 3) 48)]))]
  ""
)

(define_insn_and_split "*fcompn<suffix>_s3"
  [(set (match_operand:<MASK> 0 "register_operand" "=&r")
        (match_operator:<MASK> 1 "float_comparison_operator"
         [(match_operand:V512F 2 "register_operand" "r")
          (vec_duplicate:V512F (match_operand:<CHUNK> 3 "register_operand" "r"))]))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QMASK> (match_dup 0) 0)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 0)
          (vec_duplicate:<QUART> (match_dup 3))]))
   (set (subreg:<QMASK> (match_dup 0) 16)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 16)
          (vec_duplicate:<QUART> (match_dup 3))]))
   (set (subreg:<QMASK> (match_dup 0) 32)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 32)
          (vec_duplicate:<QUART> (match_dup 3))]))
   (set (subreg:<QMASK> (match_dup 0) 48)
        (match_op_dup:<QMASK> 1
         [(subreg:<QUART> (match_dup 2) 48)
          (vec_duplicate:<QUART> (match_dup 3))]))]
  ""
)


;; CMOVED

(define_insn "*cmov<SIDI:mode>.<FITGPR:mode>"
  [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
        (if_then_else:FITGPR (match_operator 2 "zero_comparison_operator"
                                               [(match_operand:SIDI 3 "register_operand" "r,r,r,r")
                                                (const_int 0)])
                             (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                             (match_operand:FITGPR 4 "register_operand" "0,0,0,0")))]
  ""
  "cmoved.<SIDI:suffix>%2z %3? %0 = %1"
  [(set_attr "type" "alu_thin,alu_thin,alu_thin_x,alu_thin_y")
   (set_attr "length"      "4,       4,         8,        12")]
)

(define_insn "*cmov<SIDI:mode>.<FITGPR:mode>.<EQNE:evenodd>"
  [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
        (if_then_else:FITGPR (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r,r")
                                                      (const_int 1) (const_int 0))
                                   (const_int 0))
                             (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                             (match_operand:FITGPR 3 "register_operand" "0,0,0,0")))]
  ""
  "cmoved.<EQNE:evenodd> %2? %0 = %1"
  [(set_attr "type" "alu_thin,alu_thin,alu_thin_x,alu_thin_y")
   (set_attr "length"      "4,       4,         8,        12")]
)

(define_insn "*cmov<SIDI:mode>.<ALL128:mode>"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (if_then_else:ALL128 (match_operator 2 "zero_comparison_operator"
                                               [(match_operand:SIDI 3 "register_operand" "r")
                                                (const_int 0)])
                             (match_operand:ALL128 1 "register_operand" "r")
                             (match_operand:ALL128 4 "register_operand" "0")))]
  ""
  "cmoved.<SIDI:suffix>%2z %3? %x0 = %x1\n\tcmoved.<SIDI:suffix>%2z %3? %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*cmov<SIDI:mode>.<ALL128:mode>.<EQNE:evenodd>"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (if_then_else:ALL128 (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                      (const_int 1) (const_int 0))
                                   (const_int 0))
                             (match_operand:ALL128 1 "register_operand" "r")
                             (match_operand:ALL128 3 "register_operand" "0")))]
  ""
  "cmoved.<EQNE:evenodd> %2? %x0 = %x1\n\tcmoved.<EQNE:evenodd> %2? %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*cmov<SIDI:mode>.<ALL256:mode>"
  [(set (match_operand:ALL256 0 "register_operand" "=&r")
        (if_then_else:ALL256 (match_operator 2 "zero_comparison_operator"
                                                [(match_operand:SIDI 3 "register_operand" "r")
                                                 (const_int 0)])
                             (match_operand:ALL256 1 "register_operand" "r")
                             (match_operand:ALL256 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (match_op_dup 2 [(match_dup 3) (const_int 0)])
                             (subreg:<HALF> (match_dup 1) 0)
                             (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (match_op_dup 2 [(match_dup 3) (const_int 0)])
                             (subreg:<HALF> (match_dup 1) 16)
                             (subreg:<HALF> (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*cmov<SIDI:mode>.<ALL256:mode>"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (if_then_else:ALL256 (match_operator 2 "zero_comparison_operator"
                                                [(match_operand:SIDI 3 "register_operand" "r")
                                                 (const_int 0)])
                             (match_operand:ALL256 1 "register_operand" "r")
                             (match_operand:ALL256 4 "register_operand" "0")))]
  "(KV3_2||KV4)"
  {
    return "cmoved.<SIDI:suffix>%2z %3? %x0 = %x1\n\tcmoved.<SIDI:suffix>%2z %3? %y0 = %y1\n\t"
           "cmoved.<SIDI:suffix>%2z %3? %z0 = %z1\n\tcmoved.<SIDI:suffix>%2z %3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*cmov<SIDI:mode>.<ALL256:mode>.<EQNE:evenodd>"
  [(set (match_operand:ALL256 0 "register_operand" "=&r")
        (if_then_else:ALL256 (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                      (const_int 1) (const_int 0))
                                   (const_int 0))
                             (match_operand:ALL256 1 "register_operand" "r")
                             (match_operand:ALL256 3 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (EQNE (zero_extract:SIDI (match_dup 2)
                                                      (const_int 1) (const_int 0))
                                   (const_int 0))
                             (subreg:<HALF> (match_dup 1) 0)
                             (subreg:<HALF> (match_dup 3) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (EQNE (zero_extract:SIDI (match_dup 2)
                                                      (const_int 1) (const_int 0))
                                   (const_int 0))
                             (subreg:<HALF> (match_dup 1) 16)
                             (subreg:<HALF> (match_dup 3) 16)))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*cmov<SIDI:mode>.<ALL256:mode>.<EQNE:evenodd>"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (if_then_else:ALL256 (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                      (const_int 1) (const_int 0))
                                   (const_int 0))
                             (match_operand:ALL256 1 "register_operand" "r")
                             (match_operand:ALL256 3 "register_operand" "0")))]
  "(KV3_2||KV4)"
  {
    return "cmoved.<EQNE:evenodd> %2? %x0 = %x1\n\tcmoved.<EQNE:evenodd> %2? %y0 = %y1\n\t"
           "cmoved.<EQNE:evenodd> %2? %z0 = %z1\n\tcmoved.<EQNE:evenodd> %2? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*cmov<SIDI:mode>.<ALL512:mode>"
  [(set (match_operand:ALL512 0 "register_operand" "=&r")
        (if_then_else:ALL512 (match_operator 2 "zero_comparison_operator"
                                                [(match_operand:SIDI 3 "register_operand" "r")
                                                 (const_int 0)])
                             (match_operand:ALL512 1 "register_operand" "r")
                             (match_operand:ALL512 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QUART> (match_dup 0) 0)
        (if_then_else:<QUART> (match_op_dup 2 [(match_dup 3) (const_int 0)])
                              (subreg:<QUART> (match_dup 1) 0)
                              (subreg:<QUART> (match_dup 4) 0)))
   (set (subreg:<QUART> (match_dup 0) 16)
        (if_then_else:<QUART> (match_op_dup 2 [(match_dup 3) (const_int 0)])
                              (subreg:<QUART> (match_dup 1) 16)
                              (subreg:<QUART> (match_dup 4) 16)))
   (set (subreg:<QUART> (match_dup 0) 32)
        (if_then_else:<QUART> (match_op_dup 2 [(match_dup 3) (const_int 0)])
                              (subreg:<QUART> (match_dup 1) 32)
                              (subreg:<QUART> (match_dup 4) 32)))
   (set (subreg:<QUART> (match_dup 0) 48)
        (if_then_else:<QUART> (match_op_dup 2 [(match_dup 3) (const_int 0)])
                              (subreg:<QUART> (match_dup 1) 48)
                              (subreg:<QUART> (match_dup 4) 48)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*cmov<SIDI:mode>.<ALL512:mode>.<EQNE:evenodd>"
  [(set (match_operand:ALL512 0 "register_operand" "=&r")
        (if_then_else:ALL512 (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                      (const_int 1) (const_int 0))
                                   (const_int 0))
                             (match_operand:ALL512 1 "register_operand" "r")
                             (match_operand:ALL512 3 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QUART> (match_dup 0) 0)
        (if_then_else:<QUART> (EQNE (zero_extract:SIDI (match_dup 2)
                                                       (const_int 1) (const_int 0))
                                    (const_int 0))
                              (subreg:<QUART> (match_dup 1) 0)
                              (subreg:<QUART> (match_dup 3) 0)))
   (set (subreg:<QUART> (match_dup 0) 16)
        (if_then_else:<QUART> (EQNE (zero_extract:SIDI (match_dup 2)
                                                       (const_int 1) (const_int 0))
                                    (const_int 0))
                              (subreg:<QUART> (match_dup 1) 16)
                              (subreg:<QUART> (match_dup 3) 16)))
   (set (subreg:<QUART> (match_dup 0) 32)
        (if_then_else:<QUART> (EQNE (zero_extract:SIDI (match_dup 2)
                                                       (const_int 1) (const_int 0))
                                    (const_int 0))
                              (subreg:<QUART> (match_dup 1) 32)
                              (subreg:<QUART> (match_dup 3) 32)))
   (set (subreg:<QUART> (match_dup 0) 48)
        (if_then_else:<QUART> (EQNE (zero_extract:SIDI (match_dup 2)
                                                       (const_int 1) (const_int 0))
                                    (const_int 0))
                              (subreg:<QUART> (match_dup 1) 48)
                              (subreg:<QUART> (match_dup 3) 48)))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)


;; MOV*CC, ADD*CC

(define_expand "mov<mode>cc"
  [(match_operand:ALLIFV 0 "register_operand" "")
   (match_operand 1 "comparison_operator" "")
   (match_operand:ALLIFV 2 "nonmemory_operand" "")
   (match_operand:ALLIFV 3 "nonmemory_operand" "")]
  ""
  {
    rtx target = operands[0];
    rtx select1 = operands[2];
    rtx select2 = operands[3];
    kvx_expand_conditional_move (target, select1, select2, operands[1]);
    DONE;
  }
)

(define_expand "add<mode>cc"
  [(match_operand:ALLIV 0 "register_operand" "")
   (match_operand 1 "comparison_operator" "")
   (match_operand:ALLIV 2 "register_operand" "")
   (match_operand:ALLIV 3 "register_operand" "")]
  ""
  {
    rtx target = operands[0];
    rtx select1 = gen_reg_rtx (<MODE>mode);
    rtx select2 = operands[2];
    emit_insn (gen_rtx_SET (select1, gen_rtx_PLUS (<MODE>mode, operands[2], operands[3])));
    kvx_expand_conditional_move (target, select1, select2, operands[1]);
    DONE;
  }
)


;; *SELECT*

(define_insn "*select<suffix>"
  [(set (match_operand:S64B 0 "register_operand" "=r")
        (if_then_else:S64B (match_operator 2 "zero_comparison_operator"
                                           [(match_operand:<MASK> 3 "register_operand" "r")
                                            (match_operand:<MASK> 5 "const_zero_operand" "")])
                           (match_operand:S64B 1 "register_operand" "r")
                           (match_operand:S64B 4 "register_operand" "0")))]
  ""
  "cmove<suffix>.%2z %3? %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "*select<suffix>_nez"
  [(set (match_operand:S64B 0 "register_operand" "=r")
        (if_then_else:S64B (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                               (match_operand:<MASK> 6 "const_zero_operand" ""))
                           (match_operand:S64B 1 "register_operand" "r")
                           (match_operand:S64B 4 "register_operand" "0")))]
  ""
  "cmove<suffix>.%2z %3? %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "*select<suffix>_nez_eqz"
  [(set (match_operand:S64B 0 "register_operand" "=r")
        (if_then_else:S64B (ne (eq:<MASK> (match_operator:<MASK> 2 "zero_comparison_operator"
                                                          [(match_operand:<MASK> 3 "register_operand" "r")
                                                           (match_operand:<MASK> 5 "const_zero_operand" "")])
                                          (match_operand:<MASK> 6 "const_zero_operand" ""))
                               (match_operand:<MASK> 7 "const_zero_operand" ""))
                           (match_operand:S64B 1 "register_operand" "r")
                           (match_operand:S64B 4 "register_operand" "0")))]
  ""
  "cmove<suffix>.%R2z %3? %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "*select<suffix>"
  [(set (match_operand:V128B 0 "register_operand" "=r")
        (if_then_else:V128B (match_operator 2 "zero_comparison_operator"
                                            [(match_operand:<MASK> 3 "register_operand" "r")
                                             (match_operand:<MASK> 5 "const_zero_operand" "")])
                            (match_operand:V128B 1 "register_operand" "r")
                            (match_operand:V128B 4 "register_operand" "0")))]
  ""
  {
    if (GET_MODE_SIZE (GET_MODE_INNER (<MODE>mode)) == UNITS_PER_WORD)
      return "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1";
    return "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1";
  }
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*select<suffix>_nez"
  [(set (match_operand:V128B 0 "register_operand" "=r")
        (if_then_else:V128B (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                                [(match_operand:<MASK> 3 "register_operand" "r")
                                                 (match_operand:<MASK> 5 "const_zero_operand" "")])
                                (match_operand:<MASK> 6 "const_zero_operand" ""))
                            (match_operand:V128B 1 "register_operand" "r")
                            (match_operand:V128B 4 "register_operand" "0")))]
  ""
  {
    if (GET_MODE_SIZE (GET_MODE_INNER (<MODE>mode)) == UNITS_PER_WORD)
      return "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1";
    return "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1";
  }
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*select<suffix>_nez_eqz"
  [(set (match_operand:V128B 0 "register_operand" "=r")
        (if_then_else:V128B (ne (eq:<MASK> (match_operator:<MASK> 2 "zero_comparison_operator"
                                                           [(match_operand:<MASK> 3 "register_operand" "r")
                                                            (match_operand:<MASK> 5 "const_zero_operand" "")])
                                           (match_operand:<MASK> 6 "const_zero_operand" ""))
                                (match_operand:<MASK> 7 "const_zero_operand" ""))
                            (match_operand:V128B 1 "register_operand" "r")
                            (match_operand:V128B 4 "register_operand" "0")))]
  ""
  {
    if (GET_MODE_SIZE (GET_MODE_INNER (<MODE>mode)) == UNITS_PER_WORD)
      return "cmoved.d%R2z %x3? %x0 = %x1\n\tcmoved.d%R2z %y3? %y0 = %y1";
    return "cmove<chunkx>.%R2z %x3? %x0 = %x1\n\tcmove<chunkx>.%R2z %y3? %y0 = %y1";
  }
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*select<suffix>_1"
  [(set (match_operand:V256B 0 "register_operand" "=r")
        (if_then_else:V256B (match_operator 2 "zero_comparison_operator"
                                            [(match_operand:<MASK> 3 "register_operand" "r")
                                             (match_operand:<MASK> 5 "const_zero_operand" "")])
                            (match_operand:V256B 1 "register_operand" "r")
                            (match_operand:V256B 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 0)
                                              (match_dup 5)])
                             (subreg:<HALF> (match_dup 1) 0)
                             (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 16)
                                              (match_dup 5)])
                             (subreg:<HALF> (match_dup 1) 16)
                             (subreg:<HALF> (match_dup 4) 16)))]
  {
    operands[5] = CONST0_RTX (<HMASK>mode);
  }
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*select<suffix>_2"
  [(set (match_operand:V256B 0 "register_operand" "=r")
        (if_then_else:V256B (match_operator 2 "zero_comparison_operator"
                                            [(match_operand:<MASK> 3 "register_operand" "r")
                                             (match_operand:<MASK> 5 "const_zero_operand" "")])
                            (match_operand:V256B 1 "register_operand" "r")
                            (match_operand:V256B 4 "register_operand" "0")))]
  "(KV3_2||KV4)"
  {
    if (GET_MODE_SIZE (GET_MODE_INNER (<MODE>mode)) == UNITS_PER_WORD)
      return "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1\n\t"
             "cmoved.d%2z %z3? %z0 = %z1\n\tcmoved.d%2z %t3? %t0 = %t1";
    return "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1\n\t"
           "cmove<chunkx>.%2z %z3? %z0 = %z1\n\tcmove<chunkx>.%2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*select<suffix>_nez_1"
  [(set (match_operand:V256B 0 "register_operand" "=r")
        (if_then_else:V256B (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                                [(match_operand:<MASK> 3 "register_operand" "r")
                                                 (match_operand:<MASK> 5 "const_zero_operand" "")])
                                (match_operand:<MASK> 6 "const_zero_operand" ""))
                            (match_operand:V256B 1 "register_operand" "r")
                            (match_operand:V256B 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 0)
                                              (match_dup 5)])
                             (subreg:<HALF> (match_dup 1) 0)
                             (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 16)
                                              (match_dup 5)])
                             (subreg:<HALF> (match_dup 1) 16)
                             (subreg:<HALF> (match_dup 4) 16)))]
  {
    operands[5] = CONST0_RTX (<HMASK>mode);
  }
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*select<suffix>_nez_2"
  [(set (match_operand:V256B 0 "register_operand" "=r")
        (if_then_else:V256B (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                                [(match_operand:<MASK> 3 "register_operand" "r")
                                                 (match_operand:<MASK> 5 "const_zero_operand" "")])
                                (match_operand:<MASK> 6 "const_zero_operand" ""))
                            (match_operand:V256B 1 "register_operand" "r")
                            (match_operand:V256B 4 "register_operand" "0")))]
  "(KV3_2||KV4)"
  {
    if (GET_MODE_SIZE (GET_MODE_INNER (<MODE>mode)) == UNITS_PER_WORD)
      return "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1\n\t"
             "cmoved.d%2z %z3? %z0 = %z1\n\tcmoved.d%2z %t3? %t0 = %t1";
    return "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1\n\t"
           "cmove<chunkx>.%2z %z3? %z0 = %z1\n\tcmove<chunkx>.%2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*select<suffix>_nez_eqz_1"
  [(set (match_operand:V256B 0 "register_operand" "=r")
        (if_then_else:V256B (ne (eq:<MASK> (match_operator:<MASK> 2 "zero_comparison_operator"
                                                           [(match_operand:<MASK> 3 "register_operand" "r")
                                                            (match_operand:<MASK> 5 "const_zero_operand" "")])
                                           (match_operand:<MASK> 6 "const_zero_operand" ""))
                                (match_operand:<MASK> 7 "const_zero_operand" ""))
                            (match_operand:V256B 1 "register_operand" "r")
                            (match_operand:V256B 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (ne (eq:<HMASK> (match_op_dup:<HMASK> 2 [(subreg:<HMASK> (match_dup 3) 0)
                                                                      (match_dup 5)])
                                             (match_dup 5))
                                 (match_dup 5))
                           (subreg:<HALF> (match_dup 1) 0)
                           (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (ne (eq:<HMASK> (match_op_dup:<HMASK> 2 [(subreg:<HMASK> (match_dup 3) 16)
                                                                      (match_dup 5)])
                                             (match_dup 5))
                                 (match_dup 5))
                           (subreg:<HALF> (match_dup 1) 16)
                           (subreg:<HALF> (match_dup 4) 16)))]
  {
    operands[5] = CONST0_RTX (<HMASK>mode);
  }
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*select<suffix>_nez_eqz_2"
  [(set (match_operand:V256B 0 "register_operand" "=r")
        (if_then_else:V256B (ne (eq:<MASK> (match_operator:<MASK> 2 "zero_comparison_operator"
                                                           [(match_operand:<MASK> 3 "register_operand" "r")
                                                            (match_operand:<MASK> 5 "const_zero_operand" "")])
                                           (match_operand:<MASK> 6 "const_zero_operand" ""))
                                (match_operand:<MASK> 7 "const_zero_operand" ""))
                            (match_operand:V256B 1 "register_operand" "r")
                            (match_operand:V256B 4 "register_operand" "0")))]
  "(KV3_2||KV4)"
  {
    if (GET_MODE_SIZE (GET_MODE_INNER (<MODE>mode)) == UNITS_PER_WORD)
      return "cmoved.d%R2z %x3? %x0 = %x1\n\tcmoved.d%R2z %y3? %y0 = %y1\n\t"
             "cmoved.d%R2z %z3? %z0 = %z1\n\tcmoved.d%R2z %t3? %t0 = %t1";
    return "cmove<chunkx>.%R2z %x3? %x0 = %x1\n\tcmove<chunkx>.%R2z %y3? %y0 = %y1\n\t"
           "cmove<chunkx>.%R2z %z3? %z0 = %z1\n\tcmove<chunkx>.%R2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*select<suffix>"
  [(set (match_operand:V512B 0 "register_operand" "=r")
        (if_then_else:V512B (match_operator 2 "zero_comparison_operator"
                                            [(match_operand:<MASK> 3 "register_operand" "r")
                                             (match_operand:<MASK> 5 "const_zero_operand" "")])
                            (match_operand:V512B 1 "register_operand" "r")
                            (match_operand:V512B 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QUART> (match_dup 0) 0)
        (if_then_else:<QUART> (match_op_dup 2 [(subreg:<QMASK> (match_dup 3) 0)
                                               (match_dup 5)])
                              (subreg:<QUART> (match_dup 1) 0)
                              (subreg:<QUART> (match_dup 4) 0)))
   (set (subreg:<QUART> (match_dup 0) 16)
        (if_then_else:<QUART> (match_op_dup 2 [(subreg:<QMASK> (match_dup 3) 16)
                                               (match_dup 5)])
                              (subreg:<QUART> (match_dup 1) 16)
                              (subreg:<QUART> (match_dup 4) 16)))
   (set (subreg:<QUART> (match_dup 0) 32)
        (if_then_else:<QUART> (match_op_dup 2 [(subreg:<QMASK> (match_dup 3) 32)
                                               (match_dup 5)])
                              (subreg:<QUART> (match_dup 1) 32)
                              (subreg:<QUART> (match_dup 4) 32)))
   (set (subreg:<QUART> (match_dup 0) 48)
        (if_then_else:<QUART> (match_op_dup 2 [(subreg:<QMASK> (match_dup 3) 48)
                                               (match_dup 5)])
                              (subreg:<QUART> (match_dup 1) 48)
                              (subreg:<QUART> (match_dup 4) 48)))]
  {
    operands[5] = CONST0_RTX (<QMASK>mode);
  }
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*select<suffix>_nez"
  [(set (match_operand:V512B 0 "register_operand" "=&r")
        (if_then_else:V512B (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                                [(match_operand:<MASK> 3 "register_operand" "r")
                                                 (match_operand:<MASK> 5 "const_zero_operand" "")])
                                (match_operand:<MASK> 6 "const_zero_operand" ""))
                            (match_operand:V512B 1 "register_operand" "r")
                            (match_operand:V512B 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QUART> (match_dup 0) 0)
        (if_then_else:<QUART> (match_op_dup 2 [(subreg:<QMASK> (match_dup 3) 0)
                                               (match_dup 5)])
                              (subreg:<QUART> (match_dup 1) 0)
                              (subreg:<QUART> (match_dup 4) 0)))
   (set (subreg:<QUART> (match_dup 0) 16)
        (if_then_else:<QUART> (match_op_dup 2 [(subreg:<QMASK> (match_dup 3) 16)
                                               (match_dup 5)])
                              (subreg:<QUART> (match_dup 1) 16)
                              (subreg:<QUART> (match_dup 4) 16)))
   (set (subreg:<QUART> (match_dup 0) 32)
        (if_then_else:<QUART> (match_op_dup 2 [(subreg:<QMASK> (match_dup 3) 32)
                                               (match_dup 5)])
                              (subreg:<QUART> (match_dup 1) 32)
                              (subreg:<QUART> (match_dup 4) 32)))
   (set (subreg:<QUART> (match_dup 0) 48)
        (if_then_else:<QUART> (match_op_dup 2 [(subreg:<QMASK> (match_dup 3) 48)
                                               (match_dup 5)])
                              (subreg:<QUART> (match_dup 1) 48)
                              (subreg:<QUART> (match_dup 4) 48)))]
  {
    operands[5] = CONST0_RTX (<QMASK>mode);
  }
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn_and_split "*select<suffix>_nez_eqz"
  [(set (match_operand:V512B 0 "register_operand" "=&r")
        (if_then_else:V512B (ne (eq:<MASK> (match_operator:<MASK> 2 "zero_comparison_operator"
                                                           [(match_operand:<MASK> 3 "register_operand" "r")
                                                            (match_operand:<MASK> 5 "const_zero_operand" "")])
                                           (match_operand:<MASK> 6 "const_zero_operand" ""))
                                (match_operand:<MASK> 7 "const_zero_operand" ""))
                            (match_operand:V512B 1 "register_operand" "r")
                            (match_operand:V512B 4 "register_operand" "0")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<QUART> (match_dup 0) 0)
        (if_then_else:<QUART> (ne (eq:<QMASK> (match_op_dup:<QMASK> 2 [(subreg:<QMASK> (match_dup 3) 0)
                                                                      (match_dup 5)])
                                             (match_dup 5))
                                 (match_dup 5))
                           (subreg:<QUART> (match_dup 1) 0)
                           (subreg:<QUART> (match_dup 4) 0)))
   (set (subreg:<QUART> (match_dup 0) 16)
        (if_then_else:<QUART> (ne (eq:<QMASK> (match_op_dup:<QMASK> 2 [(subreg:<QMASK> (match_dup 3) 16)
                                                                      (match_dup 5)])
                                             (match_dup 5))
                                 (match_dup 5))
                           (subreg:<QUART> (match_dup 1) 16)
                           (subreg:<QUART> (match_dup 4) 16)))
   (set (subreg:<QUART> (match_dup 0) 32)
        (if_then_else:<QUART> (ne (eq:<QMASK> (match_op_dup:<QMASK> 2 [(subreg:<QMASK> (match_dup 3) 32)
                                                                      (match_dup 5)])
                                             (match_dup 5))
                                 (match_dup 5))
                           (subreg:<QUART> (match_dup 1) 32)
                           (subreg:<QUART> (match_dup 4) 32)))
   (set (subreg:<QUART> (match_dup 0) 48)
        (if_then_else:<QUART> (ne (eq:<QMASK> (match_op_dup:<QMASK> 2 [(subreg:<QMASK> (match_dup 3) 48)
                                                                      (match_dup 5)])
                                             (match_dup 5))
                                 (match_dup 5))
                           (subreg:<QUART> (match_dup 1) 48)
                           (subreg:<QUART> (match_dup 4) 48)))]
  {
    operands[5] = CONST0_RTX (<QMASK>mode);
  }
  [(set_attr "type" "alu_lite_x2")]
)


;; COND_EXEC MOVE

(define_insn "*cond_exec_move<FITGPR:mode>"
  [(cond_exec
     (match_operator 2 "zero_comparison_operator"
      [(match_operand:SIDI 3 "register_operand" "r,r,r,r")
       (const_int 0)])
     (set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
          (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "cmoved.<SIDI:suffix>%2z %3? %0 = %1"
  [(set_attr "type" "alu_thin,alu_thin,alu_thin_x,alu_thin_y")
   (set_attr "length"      "4,       4,         8,        12")]
)

(define_insn "*cond_exec_move<FITGPR:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
          (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "cmoved.<EQNE:evenodd> %2? %0 = %1"
  [(set_attr "type" "alu_thin,alu_thin,alu_thin_x,alu_thin_y")
   (set_attr "length"      "4,       4,         8,        12")]
)


;; COND_EXEC CMOVE

(define_insn_and_split "*cond_exec_cmove_<FITGPR:mode>"
  [(cond_exec
     (match_operator 6 "zero_comparison_operator"
      [(match_operand:DISI 5 "register_operand" "r,r,r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
            (if_then_else:FITGPR (match_operator 2 "zero_comparison_operator"
                                                   [(match_operand:SIDI 3 "register_operand" "r,r,r,r")
                                                    (const_int 0)])
                                 (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                                 (match_operand:FITGPR 4 "register_operand" "0,0,0,0")))
       (use (match_operand:FITGPR 7 "register_operand" "r,r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 7) (match_dup 4))
   (cond_exec
     (match_op_dup 2 [(match_dup 3) (const_int 0)])
     (set (match_dup 7) (match_dup 1)))
   (cond_exec
     (match_op_dup 6 [(match_dup 5) (const_int 0)])
     (set (match_dup 0) (match_dup 7)))]
)

(define_insn_and_split "*wrapped_cmove_<FITGPR:mode>"
  [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
        (if_then_else:FITGPR (match_operator 2 "zero_comparison_operator"
                                               [(match_operand:SIDI 3 "register_operand" "r,r,r,r")
                                                (const_int 0)])
                             (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                             (match_operand:FITGPR 4 "register_operand" "0,0,0,0")))
   (use (match_operand:FITGPR 5 "register_operand" "r,r,r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (if_then_else:FITGPR (match_op_dup 2 [(match_dup 3) (const_int 0)])
                             (match_dup 1)
                             (match_dup 4)))]
)

(define_insn_and_split "*cond_exec_cmove_<FITGPR:mode>.<EQNE:evenodd>"
  [(cond_exec
     (match_operator 5 "zero_comparison_operator"
      [(match_operand:DISI 4 "register_operand" "r,r,r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
            (if_then_else:FITGPR (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r,r")
                                                          (const_int 1) (const_int 0))
                                       (const_int 0))
                                 (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                                 (match_operand:FITGPR 3 "register_operand" "0,0,0,0")))

       (use (match_operand:FITGPR 6 "register_operand" "r,r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 6) (match_dup 3))
   (cond_exec
     (EQNE (zero_extract:SIDI (match_dup 2) (const_int 1) (const_int 0))
           (const_int 0))
     (set (match_dup 6) (match_dup 1)))
   (cond_exec
     (match_op_dup 5 [(match_dup 4) (const_int 0)])
     (set (match_dup 0) (match_dup 6)))]
)

(define_insn_and_split "*wrapped_cmove_<FITGPR:mode>.<EQNE:evenodd>"
  [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
        (if_then_else:FITGPR (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r,r")
                                                      (const_int 1) (const_int 0))
                                   (const_int 0))
                             (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                             (match_operand:FITGPR 3 "register_operand" "0,0,0,0")))

       (use (match_operand:FITGPR 4 "register_operand" "r,r,r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0)
        (if_then_else:FITGPR (EQNE (zero_extract:SIDI (match_dup 2) (const_int 1) (const_int 0))
                                   (const_int 0))
                             (match_dup 1)
                             (match_dup 3)))]
)


;; COND_EXEC STORE

(define_insn "*cond_exec_store<ALLIFV:mode>"
  [(cond_exec
     (match_operator 2 "zero_comparison_operator"
      [(match_operand:SIDI 3 "register_operand" "r,r,r")
       (const_int 0)])
     (set (match_operand:ALLIFV 0 "memsimple_operand" "=c,d,e")
          (match_operand:ALLIFV 1 "register_operand" "r,r,r")))]
  ""
  "s<ALLIFV:lsusize>%X0.<SIDI:suffix>%2z %3? %O0 = %1"
  [(set_attr "type" "store_core,store_core_x,store_core_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn_and_split "*cond_exec_store<ALLIFV:mode>"
  [(cond_exec
     (match_operator 2 "zero_comparison_operator"
      [(match_operand:SIDI 3 "register_operand" "r,r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:ALLIFV 0 "memory_operand" "=a,b,m")
            (match_operand:ALLIFV 1 "register_operand" "r,r,r"))
       (use (match_operand:P 4 "register_operand" "r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(cond_exec
     (match_op_dup 2
      [(match_dup 3) (const_int 0)])
     (set (match_dup 0) (match_dup 1)))]
  {
    if (!memsimple_operand (operands[0], VOIDmode))
      {
        rtx address = copy_rtx (XEXP (operands[0], 0));
        emit_insn (gen_rtx_SET (operands[4], address));
        operands[0] = gen_rtx_MEM (<ALLIFV:MODE>mode, operands[4]);
        gcc_checking_assert (memsimple_operand (operands[0], VOIDmode));
      }
  }
)

(define_insn "*cond_exec_store<ALLIFV:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (set (match_operand:ALLIFV 0 "memsimple_operand" "=c,d,e")
          (match_operand:ALLIFV 1 "register_operand" "r,r,r")))]
  ""
  "s<ALLIFV:lsusize>%X0.<EQNE:evenodd> %2? %O0 = %1"
  [(set_attr "type" "store_core,store_core_x,store_core_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn_and_split "*cond_exec_store<ALLIFV:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:ALLIFV 0 "memory_operand" "=a,b,m")
            (match_operand:ALLIFV 1 "register_operand" "r,r,r"))
       (use (match_operand:P 3 "register_operand" "r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_dup 2) (const_int 1) (const_int 0))
           (const_int 0))
     (set (match_dup 0) (match_dup 1)))]
  {
    if (!memsimple_operand (operands[0], VOIDmode))
      {
        rtx address = copy_rtx (XEXP (operands[0], 0));
        emit_insn (gen_rtx_SET (operands[3], address));
        operands[0] = gen_rtx_MEM (<ALLIFV:MODE>mode, operands[3]);
        gcc_checking_assert (memsimple_operand (operands[0], VOIDmode));
      }
  }
)

(define_insn_and_split "*wrapped_store<ALLIFV:mode>"
  [(set (match_operand:ALLIFV 0 "memory_operand" "=a,b,m")
        (match_operand:ALLIFV 1 "register_operand" "r,r,r"))
   (use (match_operand:P 2 "register_operand" "r,r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0) (match_dup 1))]
)


;; COND_EXEC LOAD

(define_insn "*cond_exec_load<ALLIFV:mode>"
  [(cond_exec
     (match_operator 2 "zero_comparison_operator"
      [(match_operand:SIDI 3 "register_operand" "r,r,r,r,r,r")
       (const_int 0)])
     (set (match_operand:ALLIFV 0 "register_operand" "=r,r,r,r,r,r")
          (match_operand:ALLIFV 1 "memsimple_operand" "Cc,Cd,Ce,Zc,Zd,Ze")))]
  ""
  "l<ALLIFV:lsusizezx>%V1.<SIDI:suffix>%2z %3? %0 = %O1"
  [(set_attr "type" "load_core, load_core_x, load_core_y, load_core_uncached, load_core_uncached_x, load_core_uncached_y")
   (set_attr "length"           "4,               8,              12,                      4,                        8,                       12")]
)

(define_insn_and_split "*cond_exec_load<ALLIFV:mode>"
  [(cond_exec
     (match_operator 2 "zero_comparison_operator"
      [(match_operand:SIDI 3 "register_operand" "r,r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:ALLIFV 0 "register_operand" "=r,r,r")
            (match_operand:ALLIFV 1 "memory_operand" "a,b,m"))
       (use (match_operand:P 4 "register_operand" "r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(cond_exec
     (match_op_dup 2
      [(match_dup 3) (const_int 0)])
     (set (match_dup 0) (match_dup 1)))]
  {
    if (!memsimple_operand (operands[1], VOIDmode))
      {
        rtx address = copy_rtx (XEXP (operands[1], 0));
        emit_insn (gen_rtx_SET (operands[4], address));
        operands[1] = gen_rtx_MEM (<ALLIFV:MODE>mode, operands[4]);
        gcc_checking_assert (memsimple_operand (operands[1], VOIDmode));
      }
  }
)

(define_insn "*cond_exec_load<ALLIFV:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r,r,r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (set (match_operand:ALLIFV 0 "register_operand" "=r,r,r,r,r,r")
          (match_operand:ALLIFV 1 "memsimple_operand" "Cc,Cd,Ce,Zc,Zd,Ze")))]
  ""
  "l<ALLIFV:lsusizezx>%V1.<EQNE:evenodd> %2? %0 = %O1"
  [(set_attr "type" "load_core, load_core_x, load_core_y, load_core_uncached, load_core_uncached_x, load_core_uncached_y")
   (set_attr "length"           "4,               8,              12,                      4,                        8,                       12")]
)

(define_insn_and_split "*cond_exec_load<ALLIFV:mode>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:ALLIFV 0 "register_operand" "=r,r,r")
            (match_operand:ALLIFV 1 "memory_operand" "a,b,m"))
       (use (match_operand:P 3 "register_operand" "r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_dup 2) (const_int 1) (const_int 0))
           (const_int 0))
     (set (match_dup 0) (match_dup 1)))]
  {
    if (!memsimple_operand (operands[1], VOIDmode))
      {
        rtx address = copy_rtx (XEXP (operands[1], 0));
        emit_insn (gen_rtx_SET (operands[3], address));
        operands[1] = gen_rtx_MEM (<ALLIFV:MODE>mode, operands[3]);
        gcc_checking_assert (memsimple_operand (operands[1], VOIDmode));
      }
  }
)

(define_insn_and_split "*wrapped_load<ALLIFV:mode>"
  [(set (match_operand:ALLIFV 0 "register_operand" "=r,r,r")
        (match_operand:ALLIFV 1 "memory_operand" "a,b,m"))
   (use (match_operand:P 2 "register_operand" "r,r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0) (match_dup 1))]
  ""
)


;; COND_EXEC LOAD EXTEND

(define_insn "*cond_exec_load<SHORT:mode><ANY_EXTEND:lsux>"
  [(cond_exec
     (match_operator 2 "zero_comparison_operator"
      [(match_operand:SIDI 3 "register_operand" "r,r,r,r,r,r")
       (const_int 0)])
     (set (match_operand:DI 0 "register_operand" "=r,r,r,r,r,r")
          (ANY_EXTEND:DI (match_operand:SHORT 1 "memsimple_operand" "Cc,Cd,Ce,Zc,Zd,Ze"))))]
  ""
  "l<SHORT:lsusize><ANY_EXTEND:lsux>%V1.<SIDI:suffix>%2z %3? %0 = %O1"
  [(set_attr "type" "load_core, load_core_x, load_core_y, load_core_uncached, load_core_uncached_x, load_core_uncached_y")
   (set_attr "length"           "4,               8,              12,                      4,                        8,                       12")]
)

(define_insn_and_split "*cond_exec_load<SHORT:mode>_ext<ANY_EXTEND:lsux>"
  [(cond_exec
     (match_operator 2 "zero_comparison_operator"
      [(match_operand:SIDI 3 "register_operand" "r,r,r")
       (const_int 0)])
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r,r,r")
            (ANY_EXTEND:DI (match_operand:SHORT 1 "memory_operand" "a,b,m")))
       (use (match_operand:P 4 "register_operand" "r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(cond_exec
     (match_op_dup 2
      [(match_dup 3) (const_int 0)])
     (set (match_dup 0) (ANY_EXTEND:DI (match_dup 1))))]
  {
    if (!memsimple_operand (operands[1], VOIDmode))
      {
        rtx address = copy_rtx (XEXP (operands[1], 0));
        emit_insn (gen_rtx_SET (operands[4], address));
        operands[1] = gen_rtx_MEM (<SHORT:MODE>mode, operands[4]);
        gcc_checking_assert (memsimple_operand (operands[1], VOIDmode));
      }
  }
)

(define_insn "*cond_exec_load<SHORT:mode><ANY_EXTEND:lsux>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r,r,r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (set (match_operand:DI 0 "register_operand" "=r,r,r,r,r,r")
          (ANY_EXTEND:DI (match_operand:SHORT 1 "memsimple_operand" "Cc,Cd,Ce,Zc,Zd,Ze"))))]
  ""
  "l<SHORT:lsusize><ANY_EXTEND:lsux>%V1.<EQNE:evenodd> %2? %0 = %O1"
  [(set_attr "type" "load_core, load_core_x, load_core_y, load_core_uncached, load_core_uncached_x, load_core_uncached_y")
   (set_attr "length"           "4,               8,              12,                      4,                        8,                       12")]
)

(define_insn_and_split "*cond_exec_load<SHORT:mode>_ext<ANY_EXTEND:lsux>.<EQNE:evenodd>"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r")
                              (const_int 1) (const_int 0))
           (const_int 0))
     (parallel
      [(set (match_operand:DI 0 "register_operand" "=r,r,r")
            (ANY_EXTEND:DI (match_operand:SHORT 1 "memory_operand" "a,b,m")))
       (use (match_operand:P 3 "register_operand" "r,r,r"))]))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(cond_exec
     (EQNE (zero_extract:SIDI (match_dup 2) (const_int 1) (const_int 0))
           (const_int 0))
     (set (match_dup 0) (ANY_EXTEND:DI (match_dup 1))))]
  {
    if (!memsimple_operand (operands[1], VOIDmode))
      {
        rtx address = copy_rtx (XEXP (operands[1], 0));
        emit_insn (gen_rtx_SET (operands[3], address));
        operands[1] = gen_rtx_MEM (<SHORT:MODE>mode, operands[3]);
        gcc_checking_assert (memsimple_operand (operands[1], VOIDmode));
      }
  }
)

(define_insn_and_split "*wrapped_load<SHORT:mode>_ext<ANY_EXTEND:lsux>"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (ANY_EXTEND:DI (match_operand:SHORT 1 "memory_operand" "a,b,m")))
   (use (match_operand:P 2 "register_operand" "r,r,r"))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(set (match_dup 0) (ANY_EXTEND:DI (match_dup 1)))]
  ""
)


;; UNSPEC_DEF

(define_insn_and_split "*unspec_def"
  [(set (match_operand:FITGPR 0 "register_operand" "=r")
        (unspec:FITGPR [(match_operand 1)] UNSPEC_DEF))]
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE2"
  "#"
  "kvx_ifcvt_ce_level >= KVX_IFCVT_CE3"
  [(use (match_dup 1))]
)


