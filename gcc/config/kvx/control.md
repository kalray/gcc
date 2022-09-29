;; CSTORE

(define_insn "cstoresi4"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (match_operator:SI   1 "comparison_operator"
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
        (match_operator:DI   1 "comparison_operator"
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

;; sign-extend versions of cstoresi4
(define_insn "*sext_cstoredi4"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (match_operator:DI 1 "comparison_operator"
         [(sign_extend:DI (match_operand:SI 2 "register_operand" "r,r"))
          (match_operand:DI 3 "register_s32_operand" "r,B32")]))]
  ""
  "compwd.%1 %0 = %2, %3"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length"      "4,         8")]
)
(define_insn "*sext_swap_cstoredi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (match_operator:DI 1 "comparison_operator"
          [(match_operand:DI 2 "register_operand" "r")]))
           (sign_extend:DI (match_operand:SI 3 "register_operand" "r"))]
  ""
  "compwd.%S1 %0 = %3, %2"
  [(set_attr "type" "alu_thin")
   (set_attr "length"      "4")]
)
;; zero-extend versions of cstoresi4
(define_insn "*zext_cstoredi4"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (match_operator:DI 1 "comparison_operator"
         [(zero_extend:DI (match_operand:SI 2 "register_operand" "r,r"))
          (match_operand:DI 3 "register_s32_operand" "r,B32")]))]
  ""
  "compuwd.%1 %0 = %2, %3"
  [(set_attr "type" "alu_thin,alu_thin_x")
   (set_attr "length"      "4,         8")]
)
(define_insn "*zext_swap_cstoredi4"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (match_operator:DI 1 "comparison_operator"
          [(match_operand:DI 2 "register_operand" "r")]))
           (zero_extend:DI (match_operand:SI 3 "register_operand" "r"))]
  ""
  "compuwd.%S1 %0 = %3, %2"
  [(set_attr "type" "alu_thin")
   (set_attr "length"      "4")]
)

(define_expand "cstoreti4"
  [(set (match_operand:DI 0 "register_operand" "")
        (match_operator:DI 1 "comparison_operator"
         [(match_operand:TI 2 "register_operand" "")
          (match_operand:TI 3 "register_operand" "")]))]
  ""
  {
    kvx_lower_comparison (operands[0], operands[1], TImode);
    DONE;
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

(define_insn "*cb<mode>.odd"
  [(set (pc)
        (if_then_else (ne (zero_extract:SIDI (match_operand:SIDI 0 "register_operand" "r")
                                             (const_int 1) (const_int 0))
                          (const_int 0))
                      (label_ref (match_operand 1))
                      (pc)))]
  ""
  "cb.odd %0? %1"
  [(set_attr "type" "bcu")]
)

(define_insn "*cb<mode>.even"
  [(set (pc)
        (if_then_else (eq (zero_extract:SIDI (match_operand:SIDI 0 "register_operand" "r")
                                             (const_int 1) (const_int 0))
                          (const_int 0))
                      (label_ref (match_operand 1))
                      (pc)))]
  ""
  "cb.even %0? %1"
  [(set_attr "type" "bcu")]
)


;; *COMP*

(define_insn "*compdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (neg:V2DI (match_operator:V2DI 1 "comparison_operator"
                   [(match_operand:V2DI 2 "register_operand" "r")
                    (match_operand:V2DI 3 "register_operand" "r")])))]
  ""
  "compd.%1 %x0 = %x2, %x3\n\tcompd.%1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*compdp_s2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (neg:V2DI (match_operator:V2DI 1 "comparison_operator"
                   [(vec_duplicate:V2DI (match_operand:DI 2 "nonmemory_operand" "r"))
                    (match_operand:V2DI 3 "register_operand" "r")])))]
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
                     (match_operand:V4DI 3 "register_operand" "r")])))]
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
                    (match_operand:V4DI 3 "register_operand" "r")])))]
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
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S64K 2 "register_operand" "r")
          (match_operand:S64K 3 "register_operand" "r")]))]
  ""
  "compn<suffix>.%1 %0 = %2, %3"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "*compn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S128K 2 "register_operand" "r")
          (match_operand:S128K 3 "register_operand" "r")]))]
  ""
  "compn<chunkx>.%1 %x0 = %x2, %x3\n\tcompn<chunkx>.%1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*compn<suffix>_s2"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(vec_duplicate:S128K (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))
          (match_operand:S128K 3 "register_operand" "r")]))]
  ""
  "compn<chunkx>.%1 %x0 = %2, %x3\n\tcompn<chunkx>.%1 %y0 = %2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*compn<suffix>_s3"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S128K 2 "register_operand" "r")
          (vec_duplicate:S128K (match_operand:<CHUNK> 3 "nonmemory_operand" "r"))]))]
  ""
  "compn<chunkx>.%1 %x0 = %x2, %3\n\tcompn<chunkx>.%1 %y0 = %y2, %3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*compndp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "comparison_operator"
         [(match_operand:V2DI 2 "register_operand" "r")
          (match_operand:V2DI 3 "register_operand" "r")]))]
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
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "comparison_operator"
         [(match_operand:V2DI 2 "register_operand" "r")
          (match_operand:V2DI 3 "register_operand" "r")]))]
  "KV3_2"
  "compnd.%1 %x0 = %x2, %x3\n\tcompnd.%1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*compndp_s2"
  [(set (match_operand:V2DI 0 "register_operand" "=&r")
        (match_operator:V2DI 1 "comparison_operator"
         [(vec_duplicate:V2DI (match_operand:DI 2 "nonmemory_operand" "r"))
          (match_operand:V2DI 3 "register_operand" "r")]))]
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
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (match_operator:V2DI 1 "comparison_operator"
         [(vec_duplicate:V2DI (match_operand:DI 2 "nonmemory_operand" "r"))
          (match_operand:V2DI 3 "register_operand" "r")]))]
  "KV3_2"
  "compnd.%1 %x0 = %2, %x3\n\tcompnd.%1 %y0 = %2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*compndp_s3"
  [(set (match_operand:V2DI 0 "register_operand" "=&r")
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
  "KV3_2"
  "compnd.%1 %x0 = %x2, %3\n\tcompnd.%1 %y0 = %y2, %3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "*compn<suffix>"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(match_operand:S256K 2 "register_operand" "r")
          (match_operand:S256K 3 "register_operand" "r")]))]
  ""
  {
    return "compn<chunkx>.%1 %x0 = %x2, %x3\n\tcompn<chunkx>.%1 %y0 = %y2, %y3\n\t"
           "compn<chunkx>.%1 %z0 = %z2, %z3\n\tcompn<chunkx>.%1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*compn<suffix>_s2"
  [(set (match_operand:<MASK> 0 "register_operand" "=r")
        (match_operator:<MASK> 1 "comparison_operator"
         [(vec_duplicate:S256K (match_operand:<CHUNK> 2 "nonmemory_operand" "r"))
          (match_operand:S256K 3 "register_operand" "r")]))]
  ""
  {
    return "compn<chunkx>.%1 %x0 = %2, %x3\n\tcompn<chunkx>.%1 %y0 = %2, %y3\n\t"
           "compn<chunkx>.%1 %z0 = %2, %z3\n\tcompn<chunkx>.%1 %t0 = %2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
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
          (match_operand:V4DI 3 "register_operand" "r")]))]
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
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (match_operator:V4DI 1 "comparison_operator"
         [(match_operand:V4DI 2 "register_operand" "r")
          (match_operand:V4DI 3 "register_operand" "r")]))]
  "KV3_2"
  {
    return "compnd.%1 %x0 = %x2, %x3\n\tcompnd.%1 %y0 = %y2, %y3\n\t"
           "compnd.%1 %z0 = %z2, %z3\n\tcompnd.%1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*compndq_s2"
  [(set (match_operand:V4DI 0 "register_operand" "=&r")
        (match_operator:V4DI 1 "comparison_operator"
         [(vec_duplicate:V4DI (match_operand:DI 2 "nonmemory_operand" "r"))
          (match_operand:V4DI 3 "register_operand" "r")]))]
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
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (match_operator:V4DI 1 "comparison_operator"
         [(vec_duplicate:V4DI (match_operand:DI 2 "nonmemory_operand" "r"))
          (match_operand:V4DI 3 "register_operand" "r")]))]
  "KV3_2"
  {
    return "compnd.%1 %x0 = %2, %x3\n\tcompnd.%1 %y0 = %2, %y3\n\t"
           "compnd.%1 %z0 = %2, %z3\n\tcompnd.%1 %t0 = %2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*compndq_s3"
  [(set (match_operand:V4DI 0 "register_operand" "=&r")
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
  "KV3_2"
  {
    return "compnd.%1 %x0 = %x2, %3\n\tcompnd.%1 %y0 = %y2, %3\n\t"
           "compnd.%1 %z0 = %z2, %3\n\tcompnd.%1 %t0 = %t2, %3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
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
        (neg:V4DI (match_operator:V4DI 1 "comparison_operator"
                   [(match_operand:V4DI 2 "register_operand" "r")
                     (match_operand:V4DI 3 "register_operand" "r")])))]
  "KV3_2"
  {
    return "fcompd.%F1 %x0 = %x2, %x3\n\tfcompd.%F1 %y0 = %y2, %y3\n\t"
           "fcompd.%F1 %z0 = %z2, %z3\n\tfcompd.%F1 %t0 = %t2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*fcompdq_s2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (neg:V4DI (match_operator:V4DI 1 "comparison_operator"
                   [(vec_duplicate:V4DI (match_operand:DI 2 "nonmemory_operand" "r"))
                    (match_operand:V4DI 3 "register_operand" "r")])))]
  "KV3_2"
  {
    return "fcompd.%F1 %x0 = %2, %x3\n\tfcompd.%F1 %y0 = %2, %y3\n\t"
           "fcompd.%F1 %z0 = %2, %z3\n\tfcompd.%F1 %t0 = %2, %t3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*fcompdq_s3"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (neg:V4DI (match_operator:V4DI 1 "comparison_operator"
                   [(match_operand:V4DI 2 "register_operand" "r")
                    (vec_duplicate:V4DI (match_operand:DI 3 "nonmemory_operand" "r"))])))]
  "KV3_2"
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
  "KV3_2"
  "fcompnd.%F1 %x0 = %x2, %x3\n\tfcompnd.%F1 %y0 = %y2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*fcompndp_s2"
  [(set (match_operand:V2DI 0 "register_operand" "=&r")
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
  "KV3_2"
  "fcompnd.%F1 %x0 = %2, %x3\n\tfcompnd.%F1 %y0 = %2, %y3"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*fcompndp_s3"
  [(set (match_operand:V2DI 0 "register_operand" "=&r")
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
  "KV3_2"
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
         [(vec_duplicate:V4DI (match_operand:DI 2 "nonmemory_operand" "r"))
          (match_operand:V4DI 3 "register_operand" "r")]))]
  "KV3_2"
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
         [(match_operand:V4DI 2 "register_operand" "r")
          (vec_duplicate:V4DI (match_operand:DI 3 "nonmemory_operand" "r"))]))]
  "KV3_2"
  {
    return "fcompnd.%F1 %x0 = %x2, %3\n\tfcompnd.%F1 %y0 = %y2, %3\n\t"
           "fcompnd.%F1 %z0 = %z2, %3\n\tfcompnd.%F1 %t0 = %t2, %3";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
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

(define_insn "*cmov<SIDI:mode>.<FITGPR:mode>.odd"
  [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
        (if_then_else:FITGPR (ne (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r,r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                             (match_operand:FITGPR 3 "register_operand" "0,0,0,0")))]
  ""
  "cmoved.odd %2? %0 = %1"
  [(set_attr "type" "alu_thin,alu_thin,alu_thin_x,alu_thin_y")
   (set_attr "length"      "4,       4,         8,        12")]
)

(define_insn "*cmov<SIDI:mode>.<FITGPR:mode>.even"
  [(set (match_operand:FITGPR 0 "register_operand" "=r,r,r,r")
        (if_then_else:FITGPR (eq (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r,r,r,r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:FITGPR 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                             (match_operand:FITGPR 3 "register_operand" "0,0,0,0")))]
  ""
  "cmoved.even %2? %0 = %1"
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

(define_insn "*cmov<SIDI:mode>.<ALL128:mode>.odd"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (if_then_else:ALL128 (ne (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:ALL128 1 "register_operand" "r")
                             (match_operand:ALL128 3 "register_operand" "0")))]
  ""
  "cmoved.odd %2? %x0 = %x1\n\tcmoved.odd %2? %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*cmov<SIDI:mode>.<ALL128:mode>.even"
  [(set (match_operand:ALL128 0 "register_operand" "=r")
        (if_then_else:ALL128 (eq (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:ALL128 1 "register_operand" "r")
                             (match_operand:ALL128 3 "register_operand" "0")))]
  ""
  "cmoved.even %2? %x0 = %x1\n\tcmoved.even %2? %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "*cmov<SIDI:mode>.<ALL256:mode>"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (if_then_else:ALL256 (match_operator 2 "zero_comparison_operator"
                                                [(match_operand:SIDI 3 "register_operand" "r")
                                                 (const_int 0)])
                             (match_operand:ALL256 1 "register_operand" "r")
                             (match_operand:ALL256 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:TI (match_dup 0) 0)
        (if_then_else:TI (match_op_dup 2 [(match_dup 3) (const_int 0)])
                         (subreg:TI (match_dup 1) 0)
                         (subreg:TI (match_dup 4) 0)))
   (set (subreg:TI (match_dup 0) 16)
        (if_then_else:TI (match_op_dup 2 [(match_dup 3) (const_int 0)])
                         (subreg:TI (match_dup 1) 16)
                         (subreg:TI (match_dup 4) 16)))]
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
  "KV3_2"
  {
    return "cmoved.<SIDI:suffix>%2z %3? %x0 = %x1\n\tcmoved.<SIDI:suffix>%2z %3? %y0 = %y1\n\t"
           "cmoved.<SIDI:suffix>%2z %3? %z0 = %z1\n\tcmoved.<SIDI:suffix>%2z %3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*cmov<SIDI:mode>.<ALL256:mode>.odd"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (if_then_else:ALL256 (ne (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:ALL256 1 "register_operand" "r")
                             (match_operand:ALL256 3 "register_operand" "0")))]
  "KV3_1"
  "#"
  "reload_completed"
  [(set (subreg:TI (match_dup 0) 0)
        (if_then_else:TI (ne (zero_extract:SIDI (match_dup 2)
                                                (const_int 1) (const_int 0))
                             (const_int 0))
                         (subreg:TI (match_dup 1) 0)
                         (subreg:TI (match_dup 3) 0)))
   (set (subreg:TI (match_dup 0) 16)
        (if_then_else:TI (ne (zero_extract:SIDI (match_dup 2)
                                                (const_int 1) (const_int 0))
                             (const_int 0))
                         (subreg:TI (match_dup 1) 16)
                         (subreg:TI (match_dup 3) 16)))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*cmov<SIDI:mode>.<ALL256:mode>.odd"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (if_then_else:ALL256 (ne (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:ALL256 1 "register_operand" "r")
                             (match_operand:ALL256 3 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmoved.odd %2? %x0 = %x1\n\tcmoved.odd %2? %y0 = %y1\n\t"
           "cmoved.odd %2? %z0 = %z1\n\tcmoved.odd %2? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*cmov<SIDI:mode>.<ALL256:mode>.even"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (if_then_else:ALL256 (eq (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:ALL256 1 "register_operand" "r")
                             (match_operand:ALL256 3 "register_operand" "0")))]
  "KV3_1"
  "#"
  "reload_completed"
  [(set (subreg:TI (match_dup 0) 0)
        (if_then_else:TI (eq (zero_extract:SIDI (match_dup 2)
                                                (const_int 1) (const_int 0))
                             (const_int 0))
                         (subreg:TI (match_dup 1) 0)
                         (subreg:TI (match_dup 3) 0)))
   (set (subreg:TI (match_dup 0) 16)
        (if_then_else:TI (eq (zero_extract:SIDI (match_dup 2)
                                                (const_int 1) (const_int 0))
                             (const_int 0))
                         (subreg:TI (match_dup 1) 16)
                         (subreg:TI (match_dup 3) 16)))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "*cmov<SIDI:mode>.<ALL256:mode>.even"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (if_then_else:ALL256 (eq (zero_extract:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                                    (const_int 1) (const_int 0))
                                 (const_int 0))
                             (match_operand:ALL256 1 "register_operand" "r")
                             (match_operand:ALL256 3 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmoved.even %2? %x0 = %x1\n\tcmoved.even %2? %y0 = %y1\n\t"
           "cmoved.even %2? %z0 = %z1\n\tcmoved.even %2? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; MOV*CC

(define_expand "mov<mode>cc"
  [(set (match_operand:ALLIFV 0 "register_operand" "")
        (if_then_else:ALLIFV (match_operand 1 "comparison_operator" "")
                             (match_operand:ALLIFV 2 "nonmemory_operand" "")
                             (match_operand:ALLIFV 3 "nonmemory_operand" "")))]
  ""
  {
    rtx target = operands[0];
    rtx select1 = operands[2];
    rtx select2 = operands[3];
    machine_mode mode = GET_MODE (XEXP (operands[1], 0));
    kvx_expand_conditional_move (target, select1, select2, operands[1], mode);
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
  [(set (match_operand:S128B 0 "register_operand" "=r")
        (if_then_else:S128B (match_operator 2 "zero_comparison_operator"
                                            [(match_operand:<MASK> 3 "register_operand" "r")
                                             (match_operand:<MASK> 5 "const_zero_operand" "")])
                            (match_operand:S128B 1 "register_operand" "r")
                            (match_operand:S128B 4 "register_operand" "0")))]
  ""
  "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*select<suffix>_nez"
  [(set (match_operand:S128B 0 "register_operand" "=r")
        (if_then_else:S128B (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                                [(match_operand:<MASK> 3 "register_operand" "r")
                                                 (match_operand:<MASK> 5 "const_zero_operand" "")])
                                (match_operand:<MASK> 6 "const_zero_operand" ""))
                            (match_operand:S128B 1 "register_operand" "r")
                            (match_operand:S128B 4 "register_operand" "0")))]
  ""
  "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*select<suffix>_nez_eqz"
  [(set (match_operand:S128B 0 "register_operand" "=r")
        (if_then_else:S128B (ne (eq:<MASK> (match_operator:<MASK> 2 "zero_comparison_operator"
                                                           [(match_operand:<MASK> 3 "register_operand" "r")
                                                            (match_operand:<MASK> 5 "const_zero_operand" "")])
                                           (match_operand:<MASK> 6 "const_zero_operand" ""))
                                (match_operand:<MASK> 7 "const_zero_operand" ""))
                            (match_operand:S128B 1 "register_operand" "r")
                            (match_operand:S128B 4 "register_operand" "0")))]
  ""
  "cmove<chunkx>.%R2z %x3? %x0 = %x1\n\tcmove<chunkx>.%R2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*selectdp"
  [(set (match_operand:V2DA 0 "register_operand" "=r")
        (if_then_else:V2DA (match_operator 2 "zero_comparison_operator"
                                             [(match_operand:<MASK> 3 "register_operand" "r")
                                              (match_operand:<MASK> 5 "const_zero_operand" "")])
                           (match_operand:V2DA 1 "register_operand" "r")
                           (match_operand:V2DA 4 "register_operand" "0")))]
  ""
  "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*selectdp_nez"
  [(set (match_operand:V2DA 0 "register_operand" "=r")
        (if_then_else:V2DA (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                               (match_operand:<MASK> 6 "const_zero_operand" ""))
                           (match_operand:V2DA 1 "register_operand" "r")
                           (match_operand:V2DA 4 "register_operand" "0")))]
  ""
  "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*selectdp_nez_eqz"
  [(set (match_operand:V2DA 0 "register_operand" "=r")
        (if_then_else:V2DA (ne (eq:<MASK> (match_operator:<MASK> 2 "zero_comparison_operator"
                                                          [(match_operand:<MASK> 3 "register_operand" "r")
                                                           (match_operand:<MASK> 5 "const_zero_operand" "")])
                                        (match_operand:<MASK> 6 "const_zero_operand" ""))
                               (match_operand:<MASK> 7 "const_zero_operand" ""))
                           (match_operand:V2DA 1 "register_operand" "r")
                           (match_operand:V2DA 4 "register_operand" "0")))]
  ""
  "cmoved.d%R2z %x3? %x0 = %x1\n\tcmoved.d%R2z %y3? %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "*selectbv_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (if_then_else:V32QI (match_operator 2 "zero_comparison_operator"
                                            [(match_operand:V32QI 3 "register_operand" "r")
                                             (match_operand:V32QI 5 "const_zero_operand" "")])
                            (match_operand:V32QI 1 "register_operand" "r")
                            (match_operand:V32QI 4 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmovebo.%2z %x3? %x0 = %x1\n\tcmovebo.%2z %y3? %y0 = %y1\n\t"
           "cmovebo.%2z %z3? %z0 = %z1\n\tcmovebo.%2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*selectbv_nez_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (if_then_else:V32QI (ne (match_operator:V32QI 2 "zero_comparison_operator"
                                                [(match_operand:V32QI 3 "register_operand" "r")
                                                 (match_operand:V32QI 5 "const_zero_operand" "")])
                                (match_operand:V32QI 6 "const_zero_operand" ""))
                            (match_operand:V32QI 1 "register_operand" "r")
                            (match_operand:V32QI 4 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmovebo.%2z %x3? %x0 = %x1\n\tcmovebo.%2z %y3? %y0 = %y1\n\t"
           "cmovebo.%2z %z3? %z0 = %z1\n\tcmovebo.%2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "*selectbv_nez_eqz_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (if_then_else:V32QI (ne (eq:V32QI (match_operator:V32QI 2 "zero_comparison_operator"
                                                          [(match_operand:V32QI 3 "register_operand" "r")
                                                           (match_operand:V32QI 5 "const_zero_operand" "")])
                                          (match_operand:V32QI 6 "const_zero_operand" ""))
                                (match_operand:V32QI 7 "const_zero_operand" ""))
                            (match_operand:V32QI 1 "register_operand" "r")
                            (match_operand:V32QI 4 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmovebo.%R2z %x3? %x0 = %x1\n\tcmovebo.%R2z %y3? %y0 = %y1\n\t"
           "cmovebo.%R2z %z3? %z0 = %z1\n\tcmovebo.%R2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*select<suffix>_1"
  [(set (match_operand:V16HA 0 "register_operand" "=r")
        (if_then_else:V16HA (match_operator 2 "zero_comparison_operator"
                                            [(match_operand:<MASK> 3 "register_operand" "r")
                                             (match_operand:<MASK> 5 "const_zero_operand" "")])
                            (match_operand:V16HA 1 "register_operand" "r")
                            (match_operand:V16HA 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 0)
                                              (const_vector:<HMASK> [
                                                (const_int 0) (const_int 0) (const_int 0) (const_int 0)
                                                (const_int 0) (const_int 0) (const_int 0) (const_int 0)])])
                             (subreg:<HALF> (match_dup 1) 0)
                             (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 16)
                                              (const_vector:<HMASK> [
                                                (const_int 0) (const_int 0) (const_int 0) (const_int 0)
                                                (const_int 0) (const_int 0) (const_int 0) (const_int 0)])])
                             (subreg:<HALF> (match_dup 1) 16)
                             (subreg:<HALF> (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*select<suffix>_2"
  [(set (match_operand:V16HA 0 "register_operand" "=r")
        (if_then_else:V16HA (match_operator 2 "zero_comparison_operator"
                                            [(match_operand:<MASK> 3 "register_operand" "r")
                                             (match_operand:<MASK> 5 "const_zero_operand" "")])
                            (match_operand:V16HA 1 "register_operand" "r")
                            (match_operand:V16HA 4 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1\n\t"
           "cmove<chunkx>.%2z %z3? %z0 = %z1\n\tcmove<chunkx>.%2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*select<suffix>_nez_1"
  [(set (match_operand:V16HA 0 "register_operand" "=r")
        (if_then_else:V16HA (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                                [(match_operand:<MASK> 3 "register_operand" "r")
                                                 (match_operand:<MASK> 5 "const_zero_operand" "")])
                                (match_operand:<MASK> 6 "const_zero_operand" ""))
                            (match_operand:V16HA 1 "register_operand" "r")
                            (match_operand:V16HA 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 0)
                                              (const_vector:<HMASK> [
                                                (const_int 0) (const_int 0) (const_int 0) (const_int 0)
                                                (const_int 0) (const_int 0) (const_int 0) (const_int 0)])])
                             (subreg:<HALF> (match_dup 1) 0)
                             (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 16)
                                              (const_vector:<HMASK> [
                                                (const_int 0) (const_int 0) (const_int 0) (const_int 0)
                                                (const_int 0) (const_int 0) (const_int 0) (const_int 0)])])
                             (subreg:<HALF> (match_dup 1) 16)
                             (subreg:<HALF> (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*select<suffix>_nez_2"
  [(set (match_operand:V16HA 0 "register_operand" "=r")
        (if_then_else:V16HA (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                                [(match_operand:<MASK> 3 "register_operand" "r")
                                                 (match_operand:<MASK> 5 "const_zero_operand" "")])
                                (match_operand:<MASK> 6 "const_zero_operand" ""))
                            (match_operand:V16HA 1 "register_operand" "r")
                            (match_operand:V16HA 4 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1\n\t"
           "cmove<chunkx>.%2z %z3? %z0 = %z1\n\tcmove<chunkx>.%2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*select<suffix>_nez_eqz_1"
  [(set (match_operand:V16HA 0 "register_operand" "=r")
        (if_then_else:V16HA (ne (eq:<MASK> (match_operator:<MASK> 2 "zero_comparison_operator"
                                                           [(match_operand:<MASK> 3 "register_operand" "r")
                                                            (match_operand:<MASK> 5 "const_zero_operand" "")])
                                           (match_operand:<MASK> 6 "const_zero_operand" ""))
                                (match_operand:<MASK> 7 "const_zero_operand" ""))
                            (match_operand:V16HA 1 "register_operand" "r")
                            (match_operand:V16HA 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (ne (eq:<HMASK> (match_op_dup:<HMASK> 2 [(subreg:<HMASK> (match_dup 3) 0)
                                                                      (const_vector:<HMASK> [
                                                                        (const_int 0) (const_int 0) (const_int 0) (const_int 0)
                                                                        (const_int 0) (const_int 0) (const_int 0) (const_int 0)])])
                                             (const_vector:<HMASK> [
                                               (const_int 0) (const_int 0) (const_int 0) (const_int 0)
                                               (const_int 0) (const_int 0) (const_int 0) (const_int 0)]))
                                 (const_vector:<HMASK> [
                                   (const_int 0) (const_int 0) (const_int 0) (const_int 0)
                                   (const_int 0) (const_int 0) (const_int 0) (const_int 0)]))
                           (subreg:<HALF> (match_dup 1) 0)
                           (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (ne (eq:<HMASK> (match_op_dup:<HMASK> 2 [(subreg:<HMASK> (match_dup 3) 16)
                                                                      (const_vector:<HMASK> [
                                                                        (const_int 0) (const_int 0) (const_int 0) (const_int 0)
                                                                        (const_int 0) (const_int 0) (const_int 0) (const_int 0)])])
                                             (const_vector:<HMASK> [
                                               (const_int 0) (const_int 0) (const_int 0) (const_int 0)
                                               (const_int 0) (const_int 0) (const_int 0) (const_int 0)]))
                                 (const_vector:<HMASK> [
                                   (const_int 0) (const_int 0) (const_int 0) (const_int 0)
                                   (const_int 0) (const_int 0) (const_int 0) (const_int 0)]))
                           (subreg:<HALF> (match_dup 1) 16)
                           (subreg:<HALF> (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*select<suffix>_nez_eqz_2"
  [(set (match_operand:V16HA 0 "register_operand" "=r")
        (if_then_else:V16HA (ne (eq:<MASK> (match_operator:<MASK> 2 "zero_comparison_operator"
                                                           [(match_operand:<MASK> 3 "register_operand" "r")
                                                            (match_operand:<MASK> 5 "const_zero_operand" "")])
                                           (match_operand:<MASK> 6 "const_zero_operand" ""))
                                (match_operand:<MASK> 7 "const_zero_operand" ""))
                            (match_operand:V16HA 1 "register_operand" "r")
                            (match_operand:V16HA 4 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmove<chunkx>.%R2z %x3? %x0 = %x1\n\tcmove<chunkx>.%R2z %y3? %y0 = %y1\n\t"
           "cmove<chunkx>.%R2z %z3? %z0 = %z1\n\tcmove<chunkx>.%R2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*select<suffix>_1"
  [(set (match_operand:V8SA 0 "register_operand" "=r")
        (if_then_else:V8SA (match_operator 2 "zero_comparison_operator"
                                             [(match_operand:<MASK> 3 "register_operand" "r")
                                              (match_operand:<MASK> 5 "const_zero_operand" "")])
                           (match_operand:V8SA 1 "register_operand" "r")
                           (match_operand:V8SA 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 0)
                                              (const_vector:<HMASK> [
                                                (const_int 0) (const_int 0)
                                                (const_int 0) (const_int 0)])])
                             (subreg:<HALF> (match_dup 1) 0)
                             (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 16)
                                              (const_vector:<HMASK> [
                                                (const_int 0) (const_int 0)
                                                (const_int 0) (const_int 0)])])
                             (subreg:<HALF> (match_dup 1) 16)
                             (subreg:<HALF> (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*select<suffix>_2"
  [(set (match_operand:V8SA 0 "register_operand" "=r")
        (if_then_else:V8SA (match_operator 2 "zero_comparison_operator"
                                             [(match_operand:<MASK> 3 "register_operand" "r")
                                              (match_operand:<MASK> 5 "const_zero_operand" "")])
                           (match_operand:V8SA 1 "register_operand" "r")
                           (match_operand:V8SA 4 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1\n\t"
           "cmove<chunkx>.%2z %z3? %z0 = %z1\n\tcmove<chunkx>.%2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*select<suffix>_nez_1"
  [(set (match_operand:V8SA 0 "register_operand" "=r")
        (if_then_else:V8SA (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                               (match_operand:<MASK> 6 "const_zero_operand" ""))
                           (match_operand:V8SA 1 "register_operand" "r")
                           (match_operand:V8SA 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 0)
                                              (const_vector:<HMASK> [
                                                (const_int 0) (const_int 0)
                                                (const_int 0) (const_int 0)])])
                             (subreg:<HALF> (match_dup 1) 0)
                             (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 16)
                                              (const_vector:<HMASK> [
                                                (const_int 0) (const_int 0)
                                                (const_int 0) (const_int 0)])])
                             (subreg:<HALF> (match_dup 1) 16)
                             (subreg:<HALF> (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*select<suffix>_nez_2"
  [(set (match_operand:V8SA 0 "register_operand" "=r")
        (if_then_else:V8SA (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                               (match_operand:<MASK> 6 "const_zero_operand" ""))
                           (match_operand:V8SA 1 "register_operand" "r")
                           (match_operand:V8SA 4 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmove<chunkx>.%2z %x3? %x0 = %x1\n\tcmove<chunkx>.%2z %y3? %y0 = %y1\n\t"
           "cmove<chunkx>.%2z %z3? %z0 = %z1\n\tcmove<chunkx>.%2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*select<suffix>_nez_eqz_1"
  [(set (match_operand:V8SA 0 "register_operand" "=r")
        (if_then_else:V8SA (ne (eq:<MASK> (match_operator:<MASK> 2 "zero_comparison_operator"
                                                          [(match_operand:<MASK> 3 "register_operand" "r")
                                                           (match_operand:<MASK> 5 "const_zero_operand" "")])
                                          (match_operand:<MASK> 6 "const_zero_operand" ""))
                               (match_operand:<MASK> 7 "const_zero_operand" ""))
                           (match_operand:V8SA 1 "register_operand" "r")
                           (match_operand:V8SA 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (ne (eq:<HMASK> (match_op_dup:<HMASK> 2 [(subreg:<HMASK> (match_dup 3) 0)
                                                                      (const_vector:<HMASK> [
                                                                        (const_int 0) (const_int 0)
                                                                        (const_int 0) (const_int 0)])])
                                             (const_vector:<HMASK> [
                                               (const_int 0) (const_int 0)
                                               (const_int 0) (const_int 0)]))
                                 (const_vector:<HMASK> [
                                   (const_int 0) (const_int 0)
                                   (const_int 0) (const_int 0)]))
                             (subreg:<HALF> (match_dup 1) 0)
                             (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (ne (eq:<HMASK> (match_op_dup:<HMASK> 2 [(subreg:<HMASK> (match_dup 3) 16)
                                                                      (const_vector:<HMASK> [
                                                                        (const_int 0) (const_int 0)
                                                                        (const_int 0) (const_int 0)])])
                                             (const_vector:<HMASK> [
                                               (const_int 0) (const_int 0)
                                               (const_int 0) (const_int 0)]))
                                 (const_vector:<HMASK> [
                                   (const_int 0) (const_int 0)
                                   (const_int 0) (const_int 0)]))
                             (subreg:<HALF> (match_dup 1) 16)
                             (subreg:<HALF> (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*select<suffix>_nez_eqz_2"
  [(set (match_operand:V8SA 0 "register_operand" "=r")
        (if_then_else:V8SA (ne (eq:<MASK> (match_operator:<MASK> 2 "zero_comparison_operator"
                                                          [(match_operand:<MASK> 3 "register_operand" "r")
                                                           (match_operand:<MASK> 5 "const_zero_operand" "")])
                                          (match_operand:<MASK> 6 "const_zero_operand" ""))
                               (match_operand:<MASK> 7 "const_zero_operand" ""))
                           (match_operand:V8SA 1 "register_operand" "r")
                           (match_operand:V8SA 4 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmove<chunkx>.%R2z %x3? %x0 = %x1\n\tcmove<chunkx>.%R2z %y3? %y0 = %y1\n\t"
           "cmove<chunkx>.%R2z %z3? %z0 = %z1\n\tcmove<chunkx>.%R2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*select<suffix>_1"
  [(set (match_operand:V4DA 0 "register_operand" "=r")
        (if_then_else:V4DA (match_operator 2 "zero_comparison_operator"
                                             [(match_operand:<MASK> 3 "register_operand" "r")
                                              (match_operand:<MASK> 5 "const_zero_operand" "")])
                           (match_operand:V4DA 1 "register_operand" "r")
                           (match_operand:V4DA 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 0)
                                              (const_vector:<HMASK> [
                                                (const_int 0)
                                                (const_int 0)])])
                             (subreg:<HALF> (match_dup 1) 0)
                             (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 16)
                                              (const_vector:<HMASK> [
                                                (const_int 0)
                                                (const_int 0)])])
                             (subreg:<HALF> (match_dup 1) 16)
                             (subreg:<HALF> (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*select<suffix>_2"
  [(set (match_operand:V4DA 0 "register_operand" "=r")
        (if_then_else:V4DA (match_operator 2 "zero_comparison_operator"
                                             [(match_operand:<MASK> 3 "register_operand" "r")
                                              (match_operand:<MASK> 5 "const_zero_operand" "")])
                           (match_operand:V4DA 1 "register_operand" "r")
                           (match_operand:V4DA 4 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1\n\t"
           "cmoved.d%2z %z3? %z0 = %z1\n\tcmoved.d%2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*select<suffix>_nez_1"
  [(set (match_operand:V4DA 0 "register_operand" "=r")
        (if_then_else:V4DA (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                               (match_operand:<MASK> 6 "const_zero_operand" ""))
                           (match_operand:V4DA 1 "register_operand" "r")
                           (match_operand:V4DA 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 0)
                                              (const_vector:<HMASK> [
                                                (const_int 0)
                                                (const_int 0)])])
                             (subreg:<HALF> (match_dup 1) 0)
                             (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (match_op_dup 2 [(subreg:<HMASK> (match_dup 3) 16)
                                              (const_vector:<HMASK> [
                                                (const_int 0)
                                                (const_int 0)])])
                             (subreg:<HALF> (match_dup 1) 16)
                             (subreg:<HALF> (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*select<suffix>_nez_2"
  [(set (match_operand:V4DA 0 "register_operand" "=r")
        (if_then_else:V4DA (ne (match_operator:<MASK> 2 "zero_comparison_operator"
                                               [(match_operand:<MASK> 3 "register_operand" "r")
                                                (match_operand:<MASK> 5 "const_zero_operand" "")])
                               (match_operand:<MASK> 6 "const_zero_operand" ""))
                           (match_operand:V4DA 1 "register_operand" "r")
                           (match_operand:V4DA 4 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmoved.d%2z %x3? %x0 = %x1\n\tcmoved.d%2z %y3? %y0 = %y1\n\t"
           "cmoved.d%2z %z3? %z0 = %z1\n\tcmoved.d%2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "*select<suffix>_nez_eqz_1"
  [(set (match_operand:V4DA 0 "register_operand" "=r")
        (if_then_else:V4DA (ne (eq:<MASK> (match_operator:<MASK> 2 "zero_comparison_operator"
                                                          [(match_operand:<MASK> 3 "register_operand" "r")
                                                           (match_operand:<MASK> 5 "const_zero_operand" "")])
                                          (match_operand:<MASK> 6 "const_zero_operand" ""))
                               (match_operand:<MASK> 7 "const_zero_operand" ""))
                           (match_operand:V4DA 1 "register_operand" "r")
                           (match_operand:V4DA 4 "register_operand" "0")))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (if_then_else:<HALF> (ne (eq:<HMASK> (match_op_dup:<HMASK> 2 [(subreg:<HMASK> (match_dup 3) 0)
                                                                      (const_vector:<HMASK> [
                                                                        (const_int 0)
                                                                        (const_int 0)])])
                                             (const_vector:<HMASK> [
                                               (const_int 0)
                                               (const_int 0)]))
                                 (const_vector:<HMASK> [
                                   (const_int 0)
                                   (const_int 0)]))
                             (subreg:<HALF> (match_dup 1) 0)
                             (subreg:<HALF> (match_dup 4) 0)))
   (set (subreg:<HALF> (match_dup 0) 16)
        (if_then_else:<HALF> (ne (eq:<HMASK> (match_op_dup:<HMASK> 2 [(subreg:<HMASK> (match_dup 3) 16)
                                                                      (const_vector:<HMASK> [
                                                                        (const_int 0)
                                                                        (const_int 0)])])
                                             (const_vector:<HMASK> [
                                               (const_int 0)
                                               (const_int 0)]))
                                 (const_vector:<HMASK> [
                                   (const_int 0)
                                   (const_int 0)]))
                             (subreg:<HALF> (match_dup 1) 16)
                             (subreg:<HALF> (match_dup 4) 16)))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "*select<suffix>_nez_eqz_2"
  [(set (match_operand:V4DA 0 "register_operand" "=r")
        (if_then_else:V4DA (ne (eq:<MASK> (match_operator:<MASK> 2 "zero_comparison_operator"
                                                        [(match_operand:<MASK> 3 "register_operand" "r")
                                                         (match_operand:<MASK> 5 "const_zero_operand" "")])
                                        (match_operand:<MASK> 6 "const_zero_operand" ""))
                              (match_operand:<MASK> 7 "const_zero_operand" ""))
                            (match_operand:V4DA 1 "register_operand" "r")
                            (match_operand:V4DA 4 "register_operand" "0")))]
  "KV3_2"
  {
    return "cmoved.d%R2z %x3? %x0 = %x1\n\tcmoved.d%R2z %y3? %y0 = %y1\n\t"
           "cmoved.d%R2z %z3? %z0 = %z1\n\tcmoved.d%R2z %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

