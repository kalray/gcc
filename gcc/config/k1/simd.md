
;; (define_expand "movv2sf"
;;    [(set (match_operand:V2SI 0 "nonimmediate_operand" "")
;;          (match_operand:V2SI 1 "general_operand" " "))]
;;    ""
;; {
;;         if (MEM_P(operands[0]) && can_create_pseudo_p()) {
;;            rtx reg = gen_reg_rtx(V2SFmode);
;;            emit_move_insn (reg, operands[1]);
;;            operands[1] = reg;
;;         }
;; }
;; )
;; 
;; (define_insn "*movv2sf_real"
;;    [(set (match_operand:V2SF 0 "nonimmediate_operand" "=r,=r,=r,=a,=m,=r,=r,=r")
;;          (match_operand:V2SF 1 "general_operand" "r, a, m, r, r,I16,D37,i"))]
;;     "  (!immediate_operand(operands[1], V2SFmode) || !memory_operand(operands[0], V2SFmode))
;;     && !((memory_operand(operands[0], V2SFmode) || memory_operand(operands[1], V2SFmode)) && K1_FORCE_UNCACHED_LSU)"
;; {
;;     switch (which_alternative) {
;;     	   case 0: return "ord   %d0 = %d1, 0";
;; 	   case 1:
;; 	   case 2: return "ld%m1   %0 = %1";
;; 	   case 3:
;; 	   case 4: return "sd%m0   %0 = %1";
;;            case 5:
;;            case 6:
;;            case 7: return "maked %d0 = %d1";
;; 	   default: gcc_unreachable ();
;;     }
;; }
;; [(set_attr "type" "alud,lsu_load,lsu_load_x,lsu_store,lsu_store_x,alud,alud_y,alud_z")
;;  (set_attr "length" "8,4,8,4,8,8,12,16")]
;; )
;; 
;; (define_insn "*movv2sf_uncached"
;;    [(set (match_operand:V2SF 0 "nonimmediate_operand" "=r,r,a,m")
;;          (match_operand:V2SF 1 "nonimmediate_operand" " a,m,r,r"))]
;;     "  !(memory_operand(operands[0], V2SFmode) && memory_operand(operands[1], V2SFmode))
;;     && K1_FORCE_UNCACHED_LSU"
;; {
;;     switch (which_alternative) {
;;     	   case 0:
;; 	   case 1: return "ld%m1   %0 = %1";
;; 	   case 2:
;; 	   case 3: return "sd%m0   %0 = %1";
;; 	   default: gcc_unreachable ();
;;     }
;; }
;; [(set_attr "type" "lsu_load_uncached,lsu_load_uncached_x,lsu_store,lsu_store_x")
;;  (set_attr "length" "4,8,4,8")]
;; )
;; 
;; (define_expand "movv8sf"
;;    [(set (match_operand:V8SF 0 "nonimmediate_operand" "")
;;          (match_operand:V8SF 1 "general_operand" " "))]
;;    ""
;; {
;;         int i;
;;         
;;         if (! MEM_P (operands[0]))
;;                 emit_clobber (operands[0]);
;;         for (i = 0; i < 4; ++i) {
;;             emit_move_insn (simplify_gen_subreg (V2SFmode, operands[0], V8SFmode, i*8),
;;                             simplify_gen_subreg (V2SFmode, operands[1], V8SFmode, i*8));
;;         }
;; 
;;         DONE;
;; }
;; )
;; 
;; (define_insn "fsinvwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;     (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FSINVWP))]
;;    ""
;;    "fsinvwp %0 = %1"
;;    [(set_attr "type" "alud")
;;     (set_attr "length"  "4")]
;; )
;; 
;; 
;; (define_insn "fsinvnwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;     (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FSINVNWP))]
;;    ""
;;    "fsinvnwp %0 = %1"
;;    [(set_attr "type" "alud")
;;     (set_attr "length"  "4")]
;; )
;; 
;; (define_insn "fsisrwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;     (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FSISRWP))]
;;    ""
;;    "fsisrwp %0 = %1"
;;    [(set_attr "type" "alud")
;;     (set_attr "length"  "4")]
;; )
;; 
;; (define_insn "fsdivwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;     (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
;;                   (match_operand:V2SF 2 "register_operand" "r")] UNSPEC_FSDIVWP))]
;;    ""
;;    "fsdivwp %0 = %1"
;;    [(set_attr "type" "alud")
;;     (set_attr "length"  "4")]
;; )
;; 
;; (define_insn "fcdivwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;     (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
;;                   (match_operand:V2SF 2 "register_operand" "r")] UNSPEC_FCDIVWP))]
;;   ""
;;    "fcdivwp %0 = %1"
;;    [(set_attr "type" "alud")
;;     (set_attr "length"  "4")]
;; )
;; 
;; (define_insn "addv2sf3"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;; 	(plus:V2SF (match_operand:V2SF 1 "register_operand" "r")
;; 		   (match_operand:V2SF 2 "register_operand" "r")))]
;;   ""
;;   "faddwp %0 = %1, %2"
;;   [(set_attr "type" "mau_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; (define_insn "faddrnwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;; 	(plus:V2SF (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FPRN)
;; 		   (unspec:V2SF [(match_operand:V2SF 2 "register_operand" "r")] UNSPEC_FPRN)))]
;;   ""
;;   "faddrnwp %0 = %1, %2"
;;   [(set_attr "type" "mau_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; (define_expand "subv2sf3"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;; 	(minus:V2SF (match_operand:V2SF 1 "register_operand" "r")
;; 	            (match_operand:V2SF 2 "register_operand" "r")))]
;;   ""
;; {})
;; 
;; (define_insn "fsbfwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;; 	(minus:V2SF (match_operand:V2SF 2 "register_operand" "r")
;; 	            (match_operand:V2SF 1 "register_operand" "r")))]
;;   ""
;;   "fsbfwp %0 = %1, %2"
;;   [(set_attr "type" "mau_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; (define_insn "fsbfrnwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;; 	(minus:V2SF (unspec:V2SF [(match_operand:V2SF 2 "register_operand" "r")] UNSPEC_FPRN)
;; 	            (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FPRN)))]
;;   ""
;;   "fsbfrnwp %0 = %1, %2"
;;   [(set_attr "type" "mau_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; (define_insn "mulv2sf3"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;; 	(mult:V2SF (match_operand:V2SF 1 "register_operand" "r")
;; 		   (match_operand:V2SF 2 "register_operand" "r")))]
;;   ""
;;   "fmulwp %0 = %1, %2"
;;   [(set_attr "type" "mau_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; (define_insn "fmulrnwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;; 	(mult:V2SF (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FPRN)
;; 		   (unspec:V2SF [(match_operand:V2SF 2 "register_operand" "r")] UNSPEC_FPRN)))]
;;   ""
;;   "fmulrnwp %0 = %1, %2"
;;   [(set_attr "type" "mau_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; (define_expand "fnmsv2sf4"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (neg:V2SF (plus:V2SF (mult:V2SF (match_operand:V2SF 1 "register_operand" "r")
;;                                         (match_operand:V2SF 2 "register_operand" "r"))
;;                              (match_operand:V2SF 3 "register_operand" "r"))))]
;;   ""
;;   {}
;; )
;; 
;; (define_insn "*fnmsv2sf4"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (neg:V2SF (plus:V2SF (mult:V2SF (match_operand:V2SF 2 "register_operand" "r")
;;                                         (match_operand:V2SF 3 "register_operand" "r"))
;;                              (match_operand:V2SF 1 "register_operand" "r"))))]
;;   ""
;;   "ffmanwp %0 = %1, %2, %3"
;;   [(set_attr "type" "mau_lsu_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; (define_expand "ffmanrnwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (minus:V2SF (neg:V2SF (mult:V2SF (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FPRN)
;;                                          (unspec:V2SF [(match_operand:V2SF 2 "register_operand" "r")] UNSPEC_FPRN)))
;;                     (unspec:V2SF [(match_operand:V2SF 3 "register_operand" "r")] UNSPEC_FPRN)))]
;;   ""
;;   {}
;; )
;; 
;; (define_insn "*ffmanrnwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (minus:V2SF (neg:V2SF (mult:V2SF (unspec:V2SF [(match_operand:V2SF 2 "register_operand" "r")] UNSPEC_FPRN)
;;                                          (unspec:V2SF [(match_operand:V2SF 3 "register_operand" "r")] UNSPEC_FPRN)))
;;                     (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FPRN)))]
;;   ""
;;   "ffmanrnwp %0 = %1, %2, %3"
;;   [(set_attr "type" "mau_lsu_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; 
;; (define_expand "fmav2sf4"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (plus:V2SF (mult:V2SF (match_operand:V2SF 1 "register_operand" "r")
;;                               (match_operand:V2SF 2 "register_operand" "r"))
;;                    (match_operand:V2SF 3 "register_operand" "r")))]
;;   ""
;;   {}
;; )
;; 
;; (define_insn "*fmav2sf4"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (plus:V2SF (mult:V2SF (match_operand:V2SF 2 "register_operand" "r")
;;                               (match_operand:V2SF 3 "register_operand" "r"))
;;                    (match_operand:V2SF 1 "register_operand" "r")))]
;;   ""
;;   "ffmawp %0 = %1, %2, %3"
;;   [(set_attr "type" "mau_lsu_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; 
;; (define_expand "ffmarnwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (plus:V2SF (mult:V2SF (unspec: V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FPRN)
;;                               (unspec: V2SF [(match_operand:V2SF 2 "register_operand" "r")] UNSPEC_FPRN))
;;                    (unspec: V2SF [(match_operand:V2SF 3 "register_operand" "r")] UNSPEC_FPRN)))]
;;   ""
;;   {}
;; )
;; 
;; (define_insn "*ffmarnwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (plus:V2SF (mult:V2SF (unspec: V2SF [(match_operand:V2SF 2 "register_operand" "r")] UNSPEC_FPRN)
;;                               (unspec: V2SF [(match_operand:V2SF 3 "register_operand" "r")] UNSPEC_FPRN))
;;                    (unspec: V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FPRN)))]
;;   ""
;;   "ffmarnwp %0 = %1, %2, %3"
;;   [(set_attr "type" "mau_lsu_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; 
;; (define_expand "fmsv2sf4"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (minus:V2SF (mult:V2SF (match_operand:V2SF 1 "register_operand" "r")
;;                                (match_operand:V2SF 2 "register_operand" "r"))
;;                     (match_operand:V2SF 3 "register_operand" "r")))]
;;   ""
;;   {}
;; )
;; 
;; (define_insn "*fmsv2sf4"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (minus:V2SF (mult:V2SF (match_operand:V2SF 2 "register_operand" "r")
;;                                (match_operand:V2SF 3 "register_operand" "r"))
;;                     (match_operand:V2SF 1 "register_operand" "r")))]
;;   ""
;;   "ffmsnwp %0 = %1, %2, %3"
;;   [(set_attr "type" "mau_lsu_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; 
;; (define_expand "ffmsnrnwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (minus:V2SF (mult:V2SF (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FPRN)
;;                                (unspec:V2SF [(match_operand:V2SF 2 "register_operand" "r")] UNSPEC_FPRN))
;;                      (unspec:V2SF [(match_operand:V2SF 3 "register_operand" "r")] UNSPEC_FPRN)))]
;;   ""
;;   {}
;; )
;; 
;; (define_insn "*ffmsnrnwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (minus:V2SF (mult:V2SF (unspec:V2SF [(match_operand:V2SF 2 "register_operand" "r")] UNSPEC_FPRN)
;;                                (unspec:V2SF [(match_operand:V2SF 3 "register_operand" "r")] UNSPEC_FPRN))
;;                     (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FPRN)))]
;;   ""
;;   "ffmsnrnwp %0 = %1, %2, %3"
;;   [(set_attr "type" "mau_lsu_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; 
;; 
;; (define_expand "fnmav2sf4"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (minus:V2SF (match_operand:V2SF 3 "register_operand" "r")
;;                     (mult:V2SF (match_operand:V2SF 1 "register_operand" "r")
;; 		               (match_operand:V2SF 2 "register_operand" "r"))))]
;;   ""
;;   {}
;; )
;; 
;; (define_insn "*fnmav2sf4"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (minus:V2SF (match_operand:V2SF 1 "register_operand" "r")
;;                     (mult:V2SF (match_operand:V2SF 2 "register_operand" "r")
;;                                (match_operand:V2SF 3 "register_operand" "r"))))]
;;   ""
;;   "ffmswp %0 = %1, %2, %3"
;;   [(set_attr "type" "mau_lsu_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; 
;; 
;; (define_expand "ffmsrnwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (minus:V2SF (unspec:V2SF [(match_operand:V2SF 3 "register_operand" "r")] UNSPEC_FPRN)
;;                     (mult:V2SF (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FPRN)
;; 		               (unspec:V2SF [(match_operand:V2SF 2 "register_operand" "r")] UNSPEC_FPRN))))]
;;   ""
;;   {}
;; )
;; 
;; (define_insn "*ffmsrnwp"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (minus:V2SF (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FPRN)
;;                     (mult:V2SF (unspec:V2SF [(match_operand:V2SF 2 "register_operand" "r")] UNSPEC_FPRN)
;;                                (unspec:V2SF [(match_operand:V2SF 3 "register_operand" "r")] UNSPEC_FPRN))))]
;;   ""
;;   "ffmsrnwp %0 = %1, %2, %3"
;;   [(set_attr "type" "mau_lsu_fpu")
;;    (set_attr "length" "4")]
;; )
;; 
;; (define_insn "fix_truncv2sfv2si2"
;;   [(set (match_operand:V2SI 0 "register_operand" "=r")
;;         (fix:V2SI (match_operand:V2SF 1 "register_operand" "r")))]
;;   ""
;;   "fixedwp.rz %0 = %1, 0"
;; [(set_attr "type" "mau_fpu")
;;  (set_attr "length" "4")]
;; )
;; 
;; (define_insn "fixuns_truncv2sfv2si2"
;;   [(set (match_operand:V2SI 0 "register_operand" "=r")
;;         (unsigned_fix:V2SI (match_operand:V2SF 1 "register_operand" "r")))]
;;   ""
;;   "fixeduwp.rz %0 = %1, 0"
;; [(set_attr "type" "mau_fpu")
;;  (set_attr "length" "4")]
;; )
;; 
;; (define_insn "floatv2siv2sf2" 
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (float:V2SF (match_operand:V2SI 1 "register_operand" "r")))]
;;   ""
;;   "floatwp.rn %0 = %1, 0"
;; [(set_attr "type" "mau_fpu")
;;  (set_attr "length" "4")]
;; )
;; 
;; (define_insn "floatunsv2siv2sf2" 
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;;         (unsigned_float:V2SF (match_operand:V2SI 1 "register_operand" "r")))]
;;   ""
;;   "floatuwp.rn %0 = %1, 0"
;; [(set_attr "type" "mau_fpu")
;;  (set_attr "length" "4")]
;; )
;; 
;; 
;; (define_expand "cstorev8sf4"
;;   [(set (match_operand:SI 0 "register_operand" "")
;;         (match_operator:SI 1 "comparison_operator"
;; 	        [(match_operand:V8SF 2 "register_operand" "")
;; 	         (match_operand:V8SF 3 "register_operand" "")]))]
;;   ""
;; {
;;         int i;
;; gcc_unreachable();
;;         if (!reg_mentioned_p (operands[0], operands[2])
;;             && !reg_mentioned_p (operands[0], operands[3]))
;;             emit_clobber (operands[0]);
;; 
;;         for (i = 0; i < 8; ++i) {
;;             emit_insn (gen_cstoresf4 (simplify_gen_subreg (SFmode, operands[0], V8SFmode, i*4),
;;                                       operands[1],
;;                                       simplify_gen_subreg (SFmode, operands[2], V8SFmode, i*4),
;;                                       simplify_gen_subreg (SFmode, operands[2], V8SFmode, i*4)));
;;         }
;; 
;;         DONE;
;; })
;; 
;; 
;; (define_code_iterator v8sf_op [abs neg])
;; (define_code_attr v8sf_name [(abs "abs") (neg "neg")])
;; 
;; (define_expand "<v8sf_name>v8sf2"
;;   [(set (match_operand:V8SF 0 "register_operand" "=r")
;; 	(v8sf_op:V8SF (match_operand:V8SF 1 "register_operand" "r")))]
;;   ""
;; {
;;         int i;
;; 
;;         if (!reg_mentioned_p (operands[0], operands[1]))
;;             emit_clobber (operands[0]);
;; 
;;         for (i = 0; i < 8; ++i) {
;;             emit_insn (gen_<v8sf_name>sf2 (simplify_gen_subreg (SFmode, operands[0], V8SFmode, i*4),
;;                                            simplify_gen_subreg (SFmode, operands[1], V8SFmode, i*4)));
;;         }
;; 
;;         DONE;
;; })
;; 
;; (define_code_iterator v8sf2_op [fix unsigned_fix float unsigned_float])
;; (define_code_attr v8sf2_name [(fix "fix_trunc") (unsigned_fix "fixuns_trunc") (float "float") (unsigned_float "floatuns")])
;; (define_code_attr v8sf2_dest [(fix "si") (unsigned_fix "si") (float "sf") (unsigned_float "sf")])
;; (define_code_attr v8sf2_DEST [(fix "SI") (unsigned_fix "SI") (float "SF") (unsigned_float "SF")])
;; (define_code_attr v8sf2_orig [(fix "sf") (unsigned_fix "sf") (float "si") (unsigned_float "si")])
;; (define_code_attr v8sf2_ORIG [(fix "SF") (unsigned_fix "SF") (float "SI") (unsigned_float "SI")])
;; 
;; (define_expand "<v8sf2_name>v8<v8sf2_orig>v8<v8sf2_dest>2"
;;   [(set (match_operand 0 "register_operand" "=r")
;; 	(v8sf2_op (match_operand 1 "register_operand" "r")))]
;;   ""
;; {
;;         int i;
;; 
;;         if (!reg_mentioned_p (operands[0], operands[1]))
;;             emit_clobber (operands[0]);
;; 
;;         for (i = 0; i < 4; ++i) {
;;             emit_insn (gen_<v8sf2_name>v2<v8sf2_orig>v2<v8sf2_dest>2 (simplify_gen_subreg (V2<v8sf2_DEST>mode, operands[0], V8<v8sf2_DEST>mode, i*8),
;;                                                                       simplify_gen_subreg (V2<v8sf2_ORIG>mode, operands[1], V8<v8sf2_ORIG>mode, i*8)));
;;         }
;; 
;;         DONE;
;; })
;; 
;; 
;; 
;; (define_code_iterator v8sf3_op [plus minus mult])
;; (define_code_attr v8sf3_name [(plus "add") (minus "sub") (mult "mul")])
;; 
;; (define_expand "<v8sf3_name>v8sf3"
;;   [(set (match_operand:V8SF 0 "register_operand" "=r")
;; 	(v8sf3_op:V8SF (match_operand:V8SF 1 "register_operand" "r")
;; 		       (match_operand:V8SF 2 "register_operand" "r")))]
;;   ""
;; {
;;         int i;
;; 
;;         if (!reg_mentioned_p (operands[0], operands[1])
;;             && !reg_mentioned_p (operands[0], operands[2]))
;;             emit_clobber (operands[0]);
;; 
;;         for (i = 0; i < 4; ++i) {
;;             emit_insn (gen_<v8sf3_name>v2sf3 (simplify_gen_subreg (V2SFmode, operands[0], V8SFmode, i*8),
;;                                               simplify_gen_subreg (V2SFmode, operands[1], V8SFmode, i*8),
;;                                               simplify_gen_subreg (V2SFmode, operands[2], V8SFmode, i*8)));
;;         }
;; 
;;         DONE;
;; })
;; 
;; /* These code are meaningless, they just serve as identification token
;;    for the iterator. */
;; (define_code_iterator v8sf4_op [plus minus xor ior])
;; (define_code_attr v8sf4_name [(plus "fma") (minus "fnms") (xor "fms") (ior "fnma")])
;; 
;; (define_expand "<v8sf4_name>v8sf4"
;;   [(use (match_operand:V8SF 0 "register_operand" "=r"))
;;    (use (match_operand:V8SF 1 "register_operand" "r"))
;;    (use (match_operand:V8SF 2 "register_operand" "r"))
;;    (use (match_operand:V8SF 3 "register_operand" "r"))
;;    (use (v8sf4_op:V8SF (const_int 0) (const_int 0)))]
;;   ""
;; {
;;         int i;
;; 
;;         if (!reg_mentioned_p (operands[0], operands[1])
;;             && !reg_mentioned_p (operands[0], operands[2])
;;             && !reg_mentioned_p (operands[0], operands[3]))
;;             emit_clobber (operands[0]);
;; 
;;         for (i = 0; i < 4; ++i) {
;;             emit_insn (gen_<v8sf4_name>v2sf4 (simplify_gen_subreg (V2SFmode, operands[0], V8SFmode, i*8),
;;                                               simplify_gen_subreg (V2SFmode, operands[1], V8SFmode, i*8),
;;                                               simplify_gen_subreg (V2SFmode, operands[2], V8SFmode, i*8),
;;                                               simplify_gen_subreg (V2SFmode, operands[3], V8SFmode, i*8)));
;;         }
;; 
;;         DONE;
;; })
;; 
;; (define_insn "popcountv2si2"
;;   [(set (match_operand:V2SI            0 "register_operand" "=r")
;; 	(popcount:V2SI (match_operand:V2SI 1 "register_operand" "r")))]
;;   ""
;;   "cbswp %0 = %1"
;; [(set_attr "type" "alud_lite")
;;  (set_attr "length" "4")])
;; 
