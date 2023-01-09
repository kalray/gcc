;; ADDCD/SBFCD

(define_insn "kvx_addcd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (unspec:DI [(match_operand:DI 1 "register_operand" "r,r")
                    (match_operand:DI 2 "register_s32_operand" "r,B32")
                    (match_operand 3 "" "")
                    (reg:DI KV3_CS_REGNO)] UNSPEC_ADDCD))
   (set (reg:DI KV3_CS_REGNO)
        (unspec:DI [(reg:DI KV3_CS_REGNO) (match_dup 1) (match_dup 2)] UNSPEC_CARRY))]
  ""
  "addcd%3 %0 = %1, %2"
  [(set_attr "type" "alu_full,alu_full_x")
   (set_attr "length" "     4,         8")]
)

(define_insn "kvx_sbfcd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (unspec:DI [(match_operand:DI 1 "register_operand" "r,r")
                    (match_operand:DI 2 "register_s32_operand" "r,B32")
                    (match_operand 3 "" "")
                    (reg:DI KV3_CS_REGNO)] UNSPEC_SBFCD))
   (set (reg:DI KV3_CS_REGNO)
        (unspec:DI [(reg:DI KV3_CS_REGNO) (match_dup 1) (match_dup 2)] UNSPEC_BORROW))]
  ""
  "sbfcd%3 %0 = %1, %2"
  [(set_attr "type" "alu_full,alu_full_x")
   (set_attr "length" "     4,         8")]
)


;; MADDDT, MDSBDT

(define_expand "kvx_madddt"
  [(match_operand:V2DI 0 "register_operand" "")
   (match_operand:DI 1 "register_operand" "")
   (match_operand:DI 2 "register_operand" "")
   (match_operand:V2DI 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    kvx_expand_builtin_maddt (operands, 1);
    DONE;
  }
)

(define_expand "kvx_msbfdt"
  [(match_operand:V2DI 0 "register_operand" "")
   (match_operand:DI 1 "register_operand" "")
   (match_operand:DI 2 "register_operand" "")
   (match_operand:V2DI 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    kvx_expand_builtin_maddt (operands, 0);
    DONE;
  }
)

;; ADD*

(define_expand "kvx_add<suffix>"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
   (match_operand:SIDI 2 "register_operand" "")
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

(define_expand "kvx_add<suffix>"
  [(match_operand:VXQI 0 "register_operand" "")
   (match_operand:VXQI 1 "register_operand" "")
   (match_operand:VXQI 2 "register_operand" "")
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


;; SBF*

(define_expand "kvx_sbf<suffix>"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
   (match_operand:SIDI 2 "register_operand" "")
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

(define_expand "kvx_sbf<suffix>"
  [(match_operand:VXQI 0 "register_operand" "")
   (match_operand:VXQI 1 "register_operand" "")
   (match_operand:VXQI 2 "register_operand" "")
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


;; NEG*

(define_expand "kvx_neg<suffix>"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
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

(define_expand "kvx_neg<suffix>"
  [(match_operand:VXQI 0 "register_operand" "")
   (match_operand:VXQI 1 "register_operand" "")
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


;; ABS*

(define_expand "kvx_abs<suffix>"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
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

(define_expand "kvx_abs<suffix>"
  [(match_operand:VXQI 0 "register_operand" "")
   (match_operand:VXQI 1 "register_operand" "")
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


;; ABD*

(define_expand "kvx_abd<suffix>"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
   (match_operand:SIDI 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_abd<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen_abds<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen_abdu<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_expand "kvx_abd<suffix>"
  [(match_operand:VXQI 0 "register_operand" "")
   (match_operand:VXQI 1 "register_operand" "")
   (match_operand:VXQI 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_abd<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen_abds<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen_abdu<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
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
      emit_insn (gen_abds<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen_abdu<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
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
      emit_insn (gen_abds<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen_abdu<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
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
      emit_insn (gen_abds<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen_abdu<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)


;; AVG*

(define_expand "kvx_avgw"
  [(match_operand:SI 0 "register_operand" "")
   (match_operand:SI 1 "register_operand" "")
   (match_operand:SI 2 "register_s32_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_avgsi3_floor (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r' && !xstr[2])
      emit_insn (gen_avgsi3_ceil (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u' && !xstr[2])
      emit_insn (gen_uavgsi3_floor (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r' && xstr[2] == 'u')
      emit_insn (gen_uavgsi3_ceil (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_expand "kvx_avg<suffix>"
  [(match_operand:VXQI 0 "register_operand" "")
   (match_operand:VXQI 1 "register_operand" "")
   (match_operand:VXQI 2 "register_operand" "")
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


;; MULX*

(define_expand "kvx_mulx<widenx>"
  [(match_operand:<WIDE> 0 "register_operand" "")
   (match_operand:VWXI 1 "register_operand" "")
   (match_operand:VWXI 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (!*xstr)
      emit_insn (gen_kvx_mul<widenx> (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'u')
      emit_insn (gen_kvx_mulu<widenx> (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 's')
      emit_insn (gen_kvx_mulsu<widenx> (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)


;; MADDX*

(define_expand "kvx_maddx<widenx>"
  [(match_operand:<WIDE> 0 "register_operand" "")
   (match_operand:VWXI 1 "register_operand" "")
   (match_operand:VWXI 2 "register_operand" "")
   (match_operand:<WIDE> 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[4], 0);
    if (!*xstr)
      emit_insn (gen_kvx_madd<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 'u')
      emit_insn (gen_kvx_maddu<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 's')
      emit_insn (gen_kvx_maddsu<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else
      gcc_unreachable ();
    DONE;
  }
)


;; MSBFX*

(define_expand "kvx_msbfx<widenx>"
  [(match_operand:<WIDE> 0 "register_operand" "")
   (match_operand:VWXI 1 "register_operand" "")
   (match_operand:VWXI 2 "register_operand" "")
   (match_operand:<WIDE> 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[4], 0);
    if (!*xstr)
      emit_insn (gen_kvx_msbf<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 'u')
      emit_insn (gen_kvx_msbfu<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 's')
      emit_insn (gen_kvx_msbfsu<widenx> (operands[0], operands[1], operands[2], operands[3]));
    else
      gcc_unreachable ();
    DONE;
  }
)


;; SHL*

(define_expand "kvx_shl<suffix>"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
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

(define_expand "kvx_shl<suffix>s"
  [(match_operand:VXQI 0 "register_operand" "")
   (match_operand:VXQI 1 "register_operand" "")
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


;; SHR*

(define_expand "kvx_shr<suffix>"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
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
    else if (xstr[1] == 'a' && xstr[2] == 's')
      emit_insn (gen_sshr<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r')
      emit_insn (gen_rotr<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_expand "kvx_shr<suffix>s"
  [(match_operand:VXQI 0 "register_operand" "")
   (match_operand:VXQI 1 "register_operand" "")
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
    else if (xstr[1] == 'a' && xstr[2] == 's')
      emit_insn (gen_sshr<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r')
      emit_insn (gen_rotr<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
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
    else if (xstr[1] == 'a' && xstr[2] == 's')
      emit_insn (gen_sshr<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r')
      emit_insn (gen_rotr<mode>3 (operands[0], operands[1], operands[2]));
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
    else if (xstr[1] == 'a' && xstr[2] == 's')
      emit_insn (gen_sshr<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r')
      emit_insn (gen_rotr<mode>3 (operands[0], operands[1], operands[2]));
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
    else if (xstr[1] == 'a' && xstr[2] == 's')
      emit_insn (gen_sshr<mode>3 (operands[0], operands[1], operands[2]));
    else if (xstr[1] == 'r')
      emit_insn (gen_rotr<mode>3 (operands[0], operands[1], operands[2]));
    else
      gcc_unreachable ();
    DONE;
  }
)


;; BITCNT*

(define_expand "kvx_bitcnt<suffix>"
  [(match_operand:SIDI 0 "register_operand" "")
   (match_operand:SIDI 1 "register_operand" "")
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


;; WIDEN*, ZX*, SX*, QX*

(define_expand "kvx_widen<widenx>"
  [(match_operand:<WIDE> 0 "register_operand" "")
   (match_operand:WIDENI 1 "register_operand" "")
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

(define_expand "kvx_zx<widenx>"
  [(match_operand:<WIDE> 0 "register_operand")
   (match_operand:WIDENI 1 "register_operand")]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx op1_i = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, offset);
        rtx op0_i = simplify_gen_subreg (<WCHUNK>mode, operands[0], <WIDE>mode, offset * 2);
        emit_insn (gen_kvx_zx<wchunkx>_ (op0_i, op1_i));
      }
    DONE;
  }
)

(define_expand "kvx_zx<widenx>_"
  [(set (match_operand:<WIDE> 0 "register_operand")
        (unspec:<WIDE> [(match_operand:S64M 1 "register_operand")
                        (match_dup 2) (match_dup 3)] UNSPEC_ZX64))]
  ""
  {
    operands[2] = gen_reg_rtx (DImode);
    rtx valuev8qi_l ATTRIBUTE_UNUSED = GEN_INT (0x0008000400020001);
    rtx valuev4hi_l ATTRIBUTE_UNUSED = GEN_INT (0x0000080400000201);
    emit_insn (gen_rtx_SET (operands[2], value<mode>_l));
    operands[3] = gen_reg_rtx (DImode);
    rtx valuev8qi_m ATTRIBUTE_UNUSED = GEN_INT (0x0080004000200010);
    rtx valuev4hi_m ATTRIBUTE_UNUSED = GEN_INT (0x0000804000002010);
    emit_insn (gen_rtx_SET (operands[3], value<mode>_m));
  }
)

(define_insn "*kvx_zx<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64M 1 "register_operand" "r")
                        (match_operand:DI 2 "register_operand" "r")
                        (match_operand:DI 3 "register_operand" "r")] UNSPEC_ZX64))]
  ""
  "sbmm8 %x0 = %1, %2\n\tsbmm8 %y0 = %1, %3"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_zxwdp_"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2SI 1 "register_operand" "r")] UNSPEC_ZXWDP))]
  ""
  "zxwd %x0 = %1\n\tsrld %y0 = %1, 32"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_sx<widenx>"
  [(match_operand:<WIDE> 0 "register_operand")
   (match_operand:WIDENI 1 "register_operand")]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx op1_i = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, offset);
        rtx op0_i = simplify_gen_subreg (<WCHUNK>mode, operands[0], <WIDE>mode, offset * 2);
        emit_insn (gen_kvx_sx<wchunkx>_ (op0_i, op1_i));
      }
    DONE;
  }
)

(define_insn "kvx_sx<widenx>_"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64M 1 "register_operand" "r")] UNSPEC_SX64))]
  ""
  "sxl<hwidenx> %x0 = %1\n\tsxm<hwidenx> %y0 = %1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_sxwdp_"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2SI 1 "register_operand" "r")] UNSPEC_SXWDP))]
  ""
  "sxwd %x0 = %1\n\tsrad %y0 = %1, 32"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_qx<widenx>"
  [(match_operand:<WIDE> 0 "register_operand")
   (match_operand:WIDENI 1 "register_operand")]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx op1_i = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, offset);
        rtx op0_i = simplify_gen_subreg (<WCHUNK>mode, operands[0], <WIDE>mode, offset * 2);
        emit_insn (gen_kvx_qx<wchunkx>_ (op0_i, op1_i));
      }
    DONE;
  }
)

(define_expand "kvx_qx<widenx>_"
  [(set (match_operand:<WIDE> 0 "register_operand")
        (unspec:<WIDE> [(match_operand:S64M 1 "register_operand")
                        (match_dup 2) (match_dup 3)] UNSPEC_QX64))]
  ""
  {
    operands[2] = gen_reg_rtx (DImode);
    rtx valuev8qi_l ATTRIBUTE_UNUSED = GEN_INT (0x0800040002000100);
    rtx valuev4hi_l ATTRIBUTE_UNUSED = GEN_INT (0x0804000002010000);
    emit_insn (gen_rtx_SET (operands[2], value<mode>_l));
    operands[3] = gen_reg_rtx (DImode);
    rtx valuev8qi_m ATTRIBUTE_UNUSED = GEN_INT (0x8000400020001000);
    rtx valuev4hi_m ATTRIBUTE_UNUSED = GEN_INT (0x8040000020100000);
    emit_insn (gen_rtx_SET (operands[3], value<mode>_m));
  }
)

(define_insn "*kvx_qx<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64M 1 "register_operand" "r")
                        (match_operand:DI 2 "register_operand" "r")
                        (match_operand:DI 3 "register_operand" "r")] UNSPEC_QX64))]
  ""
  "sbmm8 %x0 = %1, %2\n\tsbmm8 %y0 = %1, %3"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_qxwdp_"
  [(set (match_operand:V2DI 0 "register_operand")
        (unspec:V2DI [(match_operand:V2SI 1 "register_operand")
                      (match_dup 2)] UNSPEC_QXWDP))]
  ""
  {
    operands[2] = gen_reg_rtx (DImode);
    emit_insn (gen_rtx_SET (operands[2], GEN_INT (0xFFFFFFFF00000000)));
  }
)

(define_insn_and_split "*kvx_qxwdp"
  [(set (match_operand:V2DI 0 "register_operand" "=&r")
        (unspec:V2DI [(match_operand:V2SI 1 "register_operand" "r")
                      (match_operand:DI 2 "register_operand" "r")] UNSPEC_QXWDP))]
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


;; NARROW*, TRUNC*, FRACT*, SAT*, SATU*

(define_expand "kvx_narrow<truncx>"
  [(match_operand:WIDENI 0 "register_operand" "")
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

(define_expand "kvx_trunc<truncx>"
  [(match_operand:WIDENI 0 "register_operand")
   (match_operand:<WIDE> 1 "register_operand")]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx op1_i = simplify_gen_subreg (<WCHUNK>mode, operands[1], <WIDE>mode, offset * 2);
        rtx op0_i = simplify_gen_subreg (<CHUNK>mode, operands[0], <MODE>mode, offset);
        emit_insn (gen_kvx_trunc<nchunkx>_ (op0_i, op1_i));
      }
    DONE;
  }
)

(define_expand "kvx_trunc<truncx>_"
  [(parallel
    [(set (match_operand:S64M 0 "register_operand")
	  (unspec:S64M [(match_operand:<WIDE> 1 "register_operand")
			(match_dup 2) (match_dup 3)] UNSPEC_TRUNC))
     (clobber (match_dup 4))]
  )]
  ""
  {
    operands[2] = gen_reg_rtx (DImode);
    rtx valuev8qi_l ATTRIBUTE_UNUSED = GEN_INT (0x0000000040100401);
    rtx valuev4hi_l ATTRIBUTE_UNUSED = GEN_INT (0x0000000020100201);
    emit_insn (gen_rtx_SET (operands[2], value<mode>_l));
    operands[3] = gen_reg_rtx (DImode);
    rtx valuev8qi_m ATTRIBUTE_UNUSED = GEN_INT (0x4010040100000000);
    rtx valuev4hi_m ATTRIBUTE_UNUSED = GEN_INT (0x2010020100000000);
    emit_insn (gen_rtx_SET (operands[3], value<mode>_m));
    operands[4] = gen_rtx_SCRATCH (<WIDE>mode);
  }
)

(define_insn_and_split "*kvx_trunc<truncx>"
  [(set (match_operand:S64M 0 "register_operand" "=r")
        (unspec:S64M [(match_operand:<WIDE> 1 "register_operand" "r")
                      (match_operand:DI 2 "register_operand" "r")
                      (match_operand:DI 3 "register_operand" "r")] UNSPEC_TRUNC))
   (clobber (match_scratch:<WIDE> 4 "=r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 4)
        (unspec:<WIDE> [(match_dup 1) (match_dup 2) (match_dup 3)] UNSPEC_SBMM8XY))
   (set (match_dup 0)
        (unspec:S64M [(subreg:S64M (match_dup 4) 0)
                      (subreg:S64M (match_dup 4) 8)] UNSPEC_XORD))]
  ""
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_truncdwp_"
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

(define_insn "kvx_truncldw"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:DI 1 "register_operand" "r")] UNSPEC_TRUNCL))]
  ""
  "zxwd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_truncmdw"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:DI 1 "register_operand" "r") (const_int 32)] UNSPEC_TRUNCM))]
  ""
  "slld %0 = %1, 32"
  [(set_attr "type" "alu_tiny")]
)

(define_expand "kvx_fract<truncx>"
  [(match_operand:WIDENI 0 "register_operand")
   (match_operand:<WIDE> 1 "register_operand")]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx op1_i = simplify_gen_subreg (<WCHUNK>mode, operands[1], <WIDE>mode, offset * 2);
        rtx op0_i = simplify_gen_subreg (<CHUNK>mode, operands[0], <MODE>mode, offset);
        emit_insn (gen_kvx_fract<nchunkx>_ (op0_i, op1_i));
      }
    DONE;
  }
)

(define_expand "kvx_fract<truncx>_"
  [(parallel
    [(set (match_operand:S64M 0 "register_operand")
	  (unspec:S64M [(match_operand:<WIDE> 1 "register_operand")
			(match_dup 2) (match_dup 3)] UNSPEC_FRACT))
     (clobber (match_dup 4))]
  )]
  ""
  {
    operands[2] = gen_reg_rtx (DImode);
    rtx valuev8qi_l ATTRIBUTE_UNUSED = GEN_INT (0x0000000080200802);
    rtx valuev4hi_l ATTRIBUTE_UNUSED = GEN_INT (0x0000000080400804);
    emit_insn (gen_rtx_SET (operands[2], value<mode>_l));
    operands[3] = gen_reg_rtx (DImode);
    rtx valuev8qi_m ATTRIBUTE_UNUSED = GEN_INT (0x8020080200000000);
    rtx valuev4hi_m ATTRIBUTE_UNUSED = GEN_INT (0x8040080400000000);
    emit_insn (gen_rtx_SET (operands[3], value<mode>_m));
    operands[4] = gen_rtx_SCRATCH (<WIDE>mode);
  }
)

(define_insn_and_split "*kvx_fract<truncx>"
  [(set (match_operand:S64M 0 "register_operand" "=r")
        (unspec:S64M [(match_operand:<WIDE> 1 "register_operand" "r")
                      (match_operand:DI 2 "register_operand" "r")
                      (match_operand:DI 3 "register_operand" "r")] UNSPEC_FRACT))
   (clobber (match_scratch:<WIDE> 4 "=r"))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 4)
        (unspec:<WIDE> [(match_dup 1) (match_dup 2) (match_dup 3)] UNSPEC_SBMM8XY))
   (set (match_dup 0)
        (unspec:S64M [(subreg:S64M (match_dup 4) 0)
                      (subreg:S64M (match_dup 4) 8)] UNSPEC_XORD))]
  ""
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_fractdwp_"
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

(define_expand "kvx_sat<truncx>"
  [(match_operand:WIDENI 0 "register_operand")
   (match_operand:<WIDE> 1 "register_operand")]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx op1_i = simplify_gen_subreg (<WCHUNK>mode, operands[1], <WIDE>mode, offset * 2);
        rtx op0_i = simplify_gen_subreg (<CHUNK>mode, operands[0], <MODE>mode, offset);
        emit_insn (gen_kvx_sat<nchunkx>_ (op0_i, op1_i));
      }
    DONE;
  }
)

(define_expand "kvx_sat<truncx>_"
  [(match_operand:S64M 0 "register_operand")
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

(define_expand "kvx_satdwp_"
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

(define_expand "kvx_satu<truncx>"
  [(match_operand:WIDENI 0 "register_operand")
   (match_operand:<WIDE> 1 "register_operand")]
  ""
  {
    unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
    for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
      {
        rtx op1_i = simplify_gen_subreg (<WCHUNK>mode, operands[1], <WIDE>mode, offset * 2);
        rtx op0_i = simplify_gen_subreg (<CHUNK>mode, operands[0], <MODE>mode, offset);
        emit_insn (gen_kvx_satu<nchunkx>_ (op0_i, op1_i));
      }
    DONE;
  }
)

(define_expand "kvx_satu<truncx>_"
  [(match_operand:S64M 0 "register_operand")
   (match_operand:<WIDE> 1 "register_operand")]
  ""
  {
    rtx zero = gen_reg_rtx (<HWIDE>mode);
    rtx lower = gen_reg_rtx (<WIDE>mode);
    rtx upper = gen_reg_rtx (<WIDE>mode);
    rtx maxvalv4hi ATTRIBUTE_UNUSED = gen_rtx_CONST_VECTOR (V4HImode,
                                           gen_rtvec (4, GEN_INT (0xFF), GEN_INT (0xFF),
                                                         GEN_INT (0xFF), GEN_INT (0xFF)));
    rtx maxvalv2si ATTRIBUTE_UNUSED = gen_rtx_CONST_VECTOR (V2SImode,
                                           gen_rtvec (2, GEN_INT (0xFFFF), GEN_INT (0xFFFF)));
    rtx zero_chunk = gen_rtx_VEC_DUPLICATE (<WIDE>mode, zero);
    rtx maxval_chunk = gen_rtx_VEC_DUPLICATE (<WIDE>mode, maxval<hwide>);
    emit_insn (gen_rtx_SET (zero, CONST0_RTX (<HWIDE>mode)));
    emit_insn (gen_rtx_SET (lower, gen_rtx_SMAX (<WIDE>mode, operands[1], zero_chunk)));
    emit_insn (gen_rtx_SET (upper, gen_rtx_SMIN (<WIDE>mode, lower, maxval_chunk)));
    emit_insn (gen_kvx_trunc<truncx> (operands[0], upper));
    DONE;
  }
)

(define_expand "kvx_satudwp_"
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


;; WIDENE*, SXE*, ZXE*, QXE*

(define_expand "kvx_widene<hwidenx>"
  [(match_operand:<HWIDE> 0 "register_operand" "")
   (match_operand:WIDENI 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      {
        scalar_mode inner_mode = GET_MODE_INNER (<MODE>mode);
        unsigned inner_mode_size = GET_MODE_SIZE (inner_mode);
        if (inner_mode_size * 2 < UNITS_PER_WORD)
          {
            rtx op1 = simplify_gen_subreg (<HWIDE>mode, operands[1], <MODE>mode, 0);
            rtx bits = GEN_INT (inner_mode_size * BITS_PER_UNIT);
            emit_insn (gen_ashl<hwide>3 (operands[0], op1, bits));
            emit_insn (gen_ashr<hwide>3 (operands[0], operands[0], bits));
          }
        else if (inner_mode == SImode)
          {
            unsigned mode_size = GET_MODE_SIZE (<MODE>mode);
            for (unsigned offset = 0; offset < mode_size; offset += UNITS_PER_WORD)
              {
                rtx op1_i = simplify_gen_subreg (SImode, operands[1], <MODE>mode, offset);
                rtx op0_i = simplify_gen_subreg (DImode, operands[0], <HWIDE>mode, offset);
                emit_insn (gen_extendsidi2 (op0_i, op1_i));
              }
          }
        else
          gcc_unreachable ();
      }
    else if (xstr[1] == 'z')
      emit_insn (gen_kvx_zxe<hwidenx> (operands[0], operands[1]));
    else if (xstr[1] == 'q')
      emit_insn (gen_kvx_qxe<hwidenx> (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "kvx_zxebhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V8QI 1 "register_operand" "r")] UNSPEC_ZXE))]
  ""
  "andd.@ %0 = %1, 0x00FF00FF"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length"        "8")]
)

(define_insn "kvx_zxebho"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (unspec:V8HI [(match_operand:V16QI 1 "register_operand" "r")] UNSPEC_ZXE))]
  ""
  "andd.@ %x0 = %x1, 0x00FF00FF\n\tandd.@ %y0 = %y1, 0x00FF00FF"
  [(set_attr "type" "alu_tiny_x2_x")
   (set_attr "length"          "16")]
)

(define_insn "kvx_zxebhx"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (unspec:V16HI [(match_operand:V32QI 1 "register_operand" "r")] UNSPEC_ZXE))]
  ""
  {
    return "andd.@ %x0 = %x1, 0x00FF00FF\n\tandd.@ %y0 = %y1, 0x00FF00FF\n\t"
           "andd.@ %z0 = %z1, 0x00FF00FF\n\tandd.@ %t0 = %t1, 0x00FF00FF";
  }
  [(set_attr "type" "alu_tiny_x4_x")
   (set_attr "length"          "32")]
)

(define_insn_and_split "kvx_zxebhv"
  [(set (match_operand:V32HI 0 "register_operand" "=r")
        (unspec:V32HI [(match_operand:V64QI 1 "register_operand" "r")] UNSPEC_ZXE))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V16HI (match_dup 0) 0)
        (unspec:V16HI [(subreg:V32QI (match_dup 1) 0)] UNSPEC_ZXE))
   (set (subreg:V16HI (match_dup 0) 32)
        (unspec:V16HI [(subreg:V32QI (match_dup 1) 32)] UNSPEC_ZXE))]
  ""
)

(define_insn "kvx_zxehwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V4HI 1 "register_operand" "r")] UNSPEC_ZXE))]
  ""
  "andd.@ %0 = %1, 0x0000FFFF"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length"        "8")]
)

(define_insn "kvx_zxehwq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unspec:V4SI [(match_operand:V8HI 1 "register_operand" "r")] UNSPEC_ZXE))]
  ""
  "andd.@ %x0 = %x1, 0x0000FFFF\n\tandd.@ %y0 = %y1, 0x0000FFFF"
  [(set_attr "type" "alu_tiny_x2_x")
   (set_attr "length"          "16")]
)

(define_insn "kvx_zxehwo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (unspec:V8SI [(match_operand:V16HI 1 "register_operand" "r")] UNSPEC_ZXE))]
  ""
  {
    return "andd.@ %x0 = %x1, 0x0000FFFF\n\tandd.@ %y0 = %y1, 0x0000FFFF\n\t"
           "andd.@ %z0 = %z1, 0x0000FFFF\n\tandd.@ %t0 = %t1, 0x0000FFFF";
  }
  [(set_attr "type" "alu_tiny_x4_x")
   (set_attr "length"          "32")]
)

(define_insn "kvx_zxewd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI [(match_operand:V2SI 1 "register_operand" "r")] UNSPEC_ZXE))]
  ""
  "zxwd %0 = %1"
  [(set_attr "type" "alu_tiny")
   (set_attr "length"      "4")]
)

(define_insn "kvx_zxewdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V4SI 1 "register_operand" "r")] UNSPEC_ZXE))]
  ""
  "zxwd %x0 = %x1\n\tzxwd %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_zxewdq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operand:V8SI 1 "register_operand" "r")] UNSPEC_ZXE))]
  ""
  {
    return "zxwd %x0 = %x1\n\tzxwd %y0 = %y1\n\t"
           "zxwd %z0 = %z1\n\tzxwd %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "kvx_qxebhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V8QI 1 "register_operand" "r")] UNSPEC_QXE))]
  ""
  "sllhqs %0 = %1, 8"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_qxebho"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (unspec:V8HI [(match_operand:V16QI 1 "register_operand" "r")] UNSPEC_QXE))]
  ""
  "sllhqs %x0 = %x1, 8\n\tsllhqs %y0 = %y1, 8"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "kvx_qxebhx"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (unspec:V16HI [(match_operand:V32QI 1 "register_operand" "r")] UNSPEC_QXE))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (unspec:V8HI [(subreg:V16QI (match_dup 1) 0)] UNSPEC_QXE))
   (set (subreg:V8HI (match_dup 0) 16)
        (unspec:V8HI [(subreg:V16QI (match_dup 1) 16)] UNSPEC_QXE))]
  ""
)

(define_insn_and_split "kvx_qxebhv"
  [(set (match_operand:V32HI 0 "register_operand" "=r")
        (unspec:V32HI [(match_operand:V64QI 1 "register_operand" "r")] UNSPEC_QXE))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (unspec:V8HI [(subreg:V16QI (match_dup 1) 0)] UNSPEC_QXE))
   (set (subreg:V8HI (match_dup 0) 16)
        (unspec:V8HI [(subreg:V16QI (match_dup 1) 16)] UNSPEC_QXE))
   (set (subreg:V8HI (match_dup 0) 32)
        (unspec:V8HI [(subreg:V16QI (match_dup 1) 32)] UNSPEC_QXE))
   (set (subreg:V8HI (match_dup 0) 48)
        (unspec:V8HI [(subreg:V16QI (match_dup 1) 48)] UNSPEC_QXE))]
  ""
)

(define_insn "kvx_qxehwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V4HI 1 "register_operand" "r")] UNSPEC_QXE))]
  ""
  "sllwps %0 = %1, 16"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_qxehwq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unspec:V4SI [(match_operand:V8HI 1 "register_operand" "r")] UNSPEC_QXE))]
  ""
  "sllwps %x0 = %x1, 16\n\tsllwps %y0 = %y1, 16"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "kvx_qxehwo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (unspec:V8SI [(match_operand:V16HI 1 "register_operand" "r")] UNSPEC_QXE))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (unspec:V4SI [(subreg:V8HI (match_dup 1) 0)] UNSPEC_QXE))
   (set (subreg:V4SI (match_dup 0) 16)
        (unspec:V4SI [(subreg:V8HI (match_dup 1) 16)] UNSPEC_QXE))]
  ""
)

(define_insn "kvx_qxewd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI [(match_operand:V2SI 1 "register_operand" "r")] UNSPEC_QXE))]
  ""
  "slld %0 = %1, 32"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_qxewdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V4SI 1 "register_operand" "r")] UNSPEC_QXE))]
  ""
  "slld %x0 = %x1, 32\n\tslld %y0 = %y1, 32"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_qxewdq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operand:V8SI 1 "register_operand" "r")] UNSPEC_QXE))]
  ""
  {
    return "slld %x0 = %x1, 32\n\tslld %y0 = %y1, 32\n\t"
           "slld %z0 = %z1, 32\n\tslld %t0 = %t1, 32";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; WIDENO*, SXO*, ZXO*, QXO*

(define_expand "kvx_wideno<hwidenx>"
  [(match_operand:<HWIDE> 0 "register_operand" "")
   (match_operand:WIDENI 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    if (!*xstr)
      {
        rtx op1 = simplify_gen_subreg (<HWIDE>mode, operands[1], <MODE>mode, 0);
        scalar_mode inner_mode = GET_MODE_INNER (<MODE>mode);
        unsigned inner_mode_size = GET_MODE_SIZE (inner_mode);
        rtx bits = GEN_INT (inner_mode_size * BITS_PER_UNIT);
        emit_insn (gen_ashr<hwide>3 (operands[0], op1, bits));
      }
    else if (xstr[1] == 'z')
      emit_insn (gen_kvx_zxo<hwidenx> (operands[0], operands[1]));
    else if (xstr[1] == 'q')
      emit_insn (gen_kvx_qxo<hwidenx> (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "kvx_zxobhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V8QI 1 "register_operand" "r")] UNSPEC_ZXO))]
  ""
  "srlhqs %0 = %1, 8"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_zxobho"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (unspec:V8HI [(match_operand:V16QI 1 "register_operand" "r")] UNSPEC_ZXO))]
  ""
  "srlhqs %x0 = %x1, 8\n\tsrlhqs %y0 = %y1, 8"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "kvx_zxobhx"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (unspec:V16HI [(match_operand:V32QI 1 "register_operand" "r")] UNSPEC_ZXO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (unspec:V8HI [(subreg:V16QI (match_dup 1) 0)] UNSPEC_ZXO))
   (set (subreg:V8HI (match_dup 0) 16)
        (unspec:V8HI [(subreg:V16QI (match_dup 1) 16)] UNSPEC_ZXO))]
  ""
)

(define_insn_and_split "kvx_zxobhv"
  [(set (match_operand:V32HI 0 "register_operand" "=r")
        (unspec:V32HI [(match_operand:V64QI 1 "register_operand" "r")] UNSPEC_ZXO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V8HI (match_dup 0) 0)
        (unspec:V8HI [(subreg:V16QI (match_dup 1) 0)] UNSPEC_ZXO))
   (set (subreg:V8HI (match_dup 0) 16)
        (unspec:V8HI [(subreg:V16QI (match_dup 1) 16)] UNSPEC_ZXO))
   (set (subreg:V8HI (match_dup 0) 32)
        (unspec:V8HI [(subreg:V16QI (match_dup 1) 32)] UNSPEC_ZXO))
   (set (subreg:V8HI (match_dup 0) 48)
        (unspec:V8HI [(subreg:V16QI (match_dup 1) 48)] UNSPEC_ZXO))]
  ""
)

(define_insn "kvx_zxohwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V4HI 1 "register_operand" "r")] UNSPEC_ZXO))]
  ""
  "srlwps %0 = %1, 16"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_zxohwq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unspec:V4SI [(match_operand:V8HI 1 "register_operand" "r")] UNSPEC_ZXO))]
  ""
  "srlwps %x0 = %x1, 16\n\tsrlwps %y0 = %y1, 16"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "kvx_zxohwo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (unspec:V8SI [(match_operand:V16HI 1 "register_operand" "r")] UNSPEC_ZXO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SI (match_dup 0) 0)
        (unspec:V4SI [(subreg:V8HI (match_dup 1) 0)] UNSPEC_ZXO))
   (set (subreg:V4SI (match_dup 0) 16)
        (unspec:V4SI [(subreg:V8HI (match_dup 1) 16)] UNSPEC_ZXO))]
  ""
)

(define_insn "kvx_zxowd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI [(match_operand:V2SI 1 "register_operand" "r")] UNSPEC_ZXO))]
  ""
  "srld %0 = %1, 32"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_zxowdp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V4SI 1 "register_operand" "r")] UNSPEC_ZXO))]
  ""
  "srld %x0 = %x1, 32\n\tsrld %y0 = %y1, 32"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_zxowdq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operand:V8SI 1 "register_operand" "r")] UNSPEC_ZXO))]
  ""
  {
    return "srld %x0 = %x1, 32\n\tsrld %y0 = %y1, 32\n\t"
           "srld %z0 = %z1, 32\n\tsrld %t0 = %t1, 32";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "kvx_qxobhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V8QI 1 "register_operand" "r")] UNSPEC_QXO))]
  ""
  "andd.@ %0 = %1, 0xFF00FF00"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length"        "8")]
)

(define_insn "kvx_qxobho"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (unspec:V8HI [(match_operand:V16QI 1 "register_operand" "r")] UNSPEC_QXO))]
  ""
  "andd.@ %x0 = %x1, 0xFF00FF00\n\tandd.@ %y0 = %y1, 0xFF00FF00"
  [(set_attr "type" "alu_tiny_x2_x")
   (set_attr "length"          "16")]
)

(define_insn "kvx_qxobhx"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (unspec:V16HI [(match_operand:V32QI 1 "register_operand" "r")] UNSPEC_QXO))]
  ""
  {
    return "andd.@ %x0 = %x1, 0xFF00FF00\n\tandd.@ %y0 = %y1, 0xFF00FF00\n\t"
           "andd.@ %z0 = %z1, 0xFF00FF00\n\tandd.@ %t0 = %t1, 0xFF00FF00";
  }
  [(set_attr "type" "alu_tiny_x4_x")
   (set_attr "length"          "32")]
)

(define_insn_and_split "kvx_qxobhv"
  [(set (match_operand:V32HI 0 "register_operand" "=r")
        (unspec:V32HI [(match_operand:V64QI 1 "register_operand" "r")] UNSPEC_QXO))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V16HI (match_dup 0) 0)
        (unspec:V16HI [(subreg:V32QI (match_dup 1) 0)] UNSPEC_QXO))
   (set (subreg:V16HI (match_dup 0) 32)
        (unspec:V16HI [(subreg:V32QI (match_dup 1) 32)] UNSPEC_QXO))]
  ""
)

(define_insn "kvx_qxohwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V4HI 1 "register_operand" "r")] UNSPEC_QXO))]
  ""
  "andd.@ %0 = %1, 0xFFFF0000"
  [(set_attr "type" "alu_tiny_x")
   (set_attr "length"        "8")]
)

(define_insn "kvx_qxohwq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unspec:V4SI [(match_operand:V8HI 1 "register_operand" "r")] UNSPEC_QXO))]
  ""
  "andd.@ %x0 = %x1, 0xFFFF0000\n\tandd.@ %y0 = %y1, 0xFFFF0000"
  [(set_attr "type" "alu_tiny_x2_x")
   (set_attr "length"          "16")]
)

(define_insn "kvx_qxohwo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (unspec:V8SI [(match_operand:V16HI 1 "register_operand" "r")] UNSPEC_QXO))]
  ""
  {
    return "andd.@ %x0 = %x1, 0xFFFF0000\n\tandd.@ %y0 = %y1, 0xFFFF0000\n\t"
           "andd.@ %z0 = %z1, 0xFFFF0000\n\tandd.@ %t0 = %t1, 0xFFFF0000";
  }
  [(set_attr "type" "alu_tiny_x4_x")
   (set_attr "length"          "32")]
)

(define_insn "kvx_qxowd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI [(match_operand:V2SI 1 "register_operand" "r")] UNSPEC_QXO))]
  ""
  "andd %0 = %1, 0xFFFFFFFF00000000"
  [(set_attr "type" "alu_tiny_y")
   (set_attr "length"       "12")]
)

(define_expand "kvx_qxowdp"
  [(set (match_operand:V2DI 0 "register_operand" "")
        (unspec:V2DI [(match_operand:V4SI 1 "register_operand" "")] UNSPEC_QXO))]
  ""
  {
    rtx mask = gen_reg_rtx (DImode);
    emit_insn (gen_rtx_SET (mask, GEN_INT (0xFFFFFFFF00000000)));
    emit_insn (gen_rtx_SET (operands[0],
                            gen_rtx_UNSPEC (V2DImode, gen_rtvec (2, operands[1], mask), UNSPEC_ANDD)));
    DONE;
  }
)

(define_expand "kvx_qxowdq"
  [(set (match_operand:V4DI 0 "register_operand" "")
        (unspec:V4DI [(match_operand:V8SI 1 "register_operand" "")] UNSPEC_QXO))]
  ""
  {
    rtx mask = gen_reg_rtx (DImode);
    emit_insn (gen_rtx_SET (mask, GEN_INT (0xFFFFFFFF00000000)));
    emit_insn (gen_rtx_SET (operands[0],
                            gen_rtx_UNSPEC (V4DImode, gen_rtvec (2, operands[1], mask), UNSPEC_ANDD)));
    DONE;
  }
)


;; SHIFT*, CAT*, LOW*, HIGH*

(define_expand "kvx_shift<lsvs>"
  [(match_operand:SIMDALL 0 "register_operand" "")
   (match_operand:SIMDALL 1 "register_operand" "")
   (match_operand 2 "const_int_operand" "")
   (match_operand:<INNER> 3 "nonmemory_operand" "")]
  ""
  {
    HOST_WIDE_INT value = INTVAL (operands[2]);
    HOST_WIDE_INT bytes = (value >= 0 ? value : -value) * GET_MODE_SIZE (<INNER>mode);
    bytes %= GET_MODE_SIZE (<MODE>mode);
    if (!bytes)
      emit_insn (gen_rtx_SET (operands[0], operands[1]));
    else
      {
        rtx chunk = const0_rtx;
        unsigned bits = bytes * BITS_PER_UNIT;
        if (operands[3] != CONST0_RTX (<INNER>mode))
          {
            if (bytes == GET_MODE_SIZE (<INNER>mode))
              chunk = force_reg (<INNER>mode, operands[3]);
            else
              {
                chunk = gen_reg_rtx (<CHUNK>mode);
                kvx_expand_chunk_splat (chunk, operands[3], <INNER>mode);
              }
          }
        kvx_expand_vector_shift (operands[0], operands[1], chunk, bits, value < 0);
      }
    DONE;
  }
)

(define_insn "kvx_catwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (vec_concat:V2SI (match_operand:SI 1 "register_operand" "0")
                         (match_operand:SI 2 "register_operand" "r")))]
  ""
  "insf %0 = %2, 63, 32"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_catfwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (vec_concat:V2SF (match_operand:SF 1 "register_operand" "0")
                         (match_operand:SF 2 "register_operand" "r")))]
  ""
  "insf %0 = %2, 63, 32"
  [(set_attr "type" "alu_thin")]
)

(define_insn_and_split "kvx_cat<lsvs>"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (vec_concat:S128F (match_operand:<HALF> 1 "register_operand" "0")
                          (match_operand:<HALF> 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 8)
        (match_dup 2))]
  ""
)

(define_insn_and_split "kvx_catfdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (vec_concat:V2DF (match_operand:DF 1 "register_operand" "0")
                         (match_operand:DF 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 8)
        (match_dup 2))]
  ""
)

(define_insn_and_split "kvx_cat128"
  [(set (match_operand:V128 0 "register_operand" "=r")
        (vec_concat:V128 (match_operand:V64 1 "register_operand" "0")
                         (match_operand:V64 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V64 (match_dup 0) 8)
        (match_dup 2))]
  ""
)

(define_insn_and_split "kvx_cat256"
  [(set (match_operand:V256 0 "register_operand" "=r")
        (vec_concat:V256 (match_operand:V128 1 "register_operand" "0")
                         (match_operand:V128 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V128 (match_dup 0) 16)
        (match_dup 2))]
  ""
)

(define_insn_and_split "kvx_cat512"
  [(set (match_operand:V512 0 "register_operand" "=r")
        (vec_concat:V512 (match_operand:V256 1 "register_operand" "0")
                         (match_operand:V256 2 "register_operand" "r")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V256 (match_dup 0) 32)
        (match_dup 2))]
  ""
)

(define_insn_and_split "kvx_low64"
  [(set (match_operand:V64 0 "register_operand" "=r")
        (subreg:V64 (match_operand:V128 1 "register_operand" "r") 0))]
  ""
  "#"
  ""
  [(set (match_dup 0) (subreg:V64 (match_dup 1) 0))]
  ""
)

(define_insn_and_split "kvx_low128"
  [(set (match_operand:V128 0 "register_operand" "=r")
        (subreg:V128 (match_operand:V256 1 "register_operand" "r") 0))]
  ""
  "#"
  ""
  [(set (match_dup 0) (subreg:V128 (match_dup 1) 0))]
  ""
)

(define_insn_and_split "kvx_low256"
  [(set (match_operand:V256 0 "register_operand" "=r")
        (subreg:V256 (match_operand:V512 1 "register_operand" "r") 0))]
  ""
  "#"
  ""
  [(set (match_dup 0) (subreg:V256 (match_dup 1) 0))]
  ""
)

(define_insn_and_split "kvx_high64"
  [(set (match_operand:V64 0 "register_operand" "=r")
        (subreg:V64 (match_operand:V128 1 "register_operand" "r") 8))]
  ""
  "#"
  ""
  [(set (match_dup 0) (subreg:V64 (match_dup 1) 8))]
  ""
)

(define_insn_and_split "kvx_high128"
  [(set (match_operand:V128 0 "register_operand" "=r")
        (subreg:V128 (match_operand:V256 1 "register_operand" "r") 16))]
  ""
  "#"
  ""
  [(set (match_dup 0) (subreg:V128 (match_dup 1) 16))]
  ""
)

(define_insn_and_split "kvx_high256"
  [(set (match_operand:V256 0 "register_operand" "=r")
        (subreg:V256 (match_operand:V512 1 "register_operand" "r") 32))]
  ""
  "#"
  ""
  [(set (match_dup 0) (subreg:V256 (match_dup 1) 32))]
  ""
)


;; ANY*

(define_expand "kvx_any<suffix>"
  [(match_operand:DI 0 "register_operand" "")
   (match_operand:S64L 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    gcc_assert (xstr[0] == '.');
    if (xstr[1] == 'n')
      emit_insn (gen_kvx_any<suffix>_nez (operands[0], operands[1]));
    else if (xstr[1] == 'e')
      emit_insn (gen_kvx_any<suffix>_eqz (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "kvx_any<suffix>_nez"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (ne:DI (subreg:DI (match_operand:S64L 1 "register_operand" "r") 0)
               (const_int 0)))]
  ""
  "compd.ne %0 = %1, 0"
  [(set_attr "type" "alu_tiny")
   (set_attr "length"      "4")]
)

(define_expand "kvx_any<suffix>_eqz"
  [(match_operand:DI 0 "register_operand" "")
   (match_operand:S64L 1 "register_operand" "")]
  ""
  {
    kvx_expand_any64_eqz (operands[0], operands[1], <MODE>mode);
    emit_insn (gen_kvx_anyd_nez (operands[0], operands[0]));
    DONE;
  }
)

(define_insn "kvx_anyd_nez"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (ne:DI (match_operand:DI 1 "register_operand" "r")
               (const_int 0)))]
  ""
  "compd.ne %0 = %1, 0"
  [(set_attr "type" "alu_tiny")
   (set_attr "length"      "4")]
)

(define_expand "kvx_anyd_eqz"
  [(set (match_operand:DI 0 "register_operand" "")
        (eq:DI (match_operand:DI 1 "register_operand" "")
               (const_int 0)))]
  ""
  ""
)

(define_expand "kvx_any<suffix>"
  [(match_operand:DI 0 "register_operand" "")
   (match_operand:V128L 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    gcc_assert (xstr[0] == '.');
    if (xstr[1] == 'n')
      emit_insn (gen_kvx_any<suffix>_nez (operands[0], operands[1]));
    else if (xstr[1] == 'e')
      emit_insn (gen_kvx_any<suffix>_eqz (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_expand "kvx_any<suffix>_nez"
  [(match_operand:DI 0 "register_operand" "")
   (match_operand:V128L 1 "register_operand" "")]
  ""
  {
    rtx op1_0 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, 0);
    rtx op1_1 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, 8);
    rtx any_0 = gen_reg_rtx (DImode);
    rtx any_1 = gen_reg_rtx (DImode);
    emit_insn (gen_kvx_any<chunkx>_nez (any_0, op1_0));
    emit_insn (gen_kvx_any<chunkx>_nez (any_1, op1_1));
    emit_insn (gen_iordi3 (operands[0], any_0, any_1));
    DONE;
  }
)

(define_expand "kvx_any<suffix>_eqz"
  [(match_operand:DI 0 "register_operand" "")
   (match_operand:V128L 1 "register_operand" "")]
  ""
  {
    rtx op1_0 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, 0);
    rtx op1_1 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, 8);
    rtx any_0 = gen_reg_rtx (DImode);
    rtx any_1 = gen_reg_rtx (DImode);
    kvx_expand_any64_eqz (any_0, op1_0, <CHUNK>mode);
    kvx_expand_any64_eqz (any_1, op1_1, <CHUNK>mode);
    emit_insn (gen_kvx_lord (operands[0], any_0, any_1));
    DONE;
  }
)

(define_expand "kvx_any<suffix>"
  [(match_operand:DI 0 "register_operand" "")
   (match_operand:V256L 1 "register_operand" "")
   (match_operand 2 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[2], 0);
    gcc_assert (xstr[0] == '.');
    if (xstr[1] == 'n')
      emit_insn (gen_kvx_any<suffix>_nez (operands[0], operands[1]));
    else if (xstr[1] == 'e')
      emit_insn (gen_kvx_any<suffix>_eqz (operands[0], operands[1]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_expand "kvx_any<suffix>_nez"
  [(match_operand:DI 0 "register_operand" "")
   (match_operand:V256L 1 "register_operand" "")]
  ""
  {
    rtx op1_0 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, 0);
    rtx op1_1 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, 8);
    rtx op1_2 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, 16);
    rtx op1_3 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, 24);
    rtx any_0 = gen_reg_rtx (DImode);
    rtx any_1 = gen_reg_rtx (DImode);
    rtx any_2 = gen_reg_rtx (DImode);
    rtx any_3 = gen_reg_rtx (DImode);
    rtx temp1 = gen_reg_rtx (DImode);
    rtx temp2 = gen_reg_rtx (DImode);
    emit_insn (gen_kvx_any<chunkx>_nez (any_0, op1_0));
    emit_insn (gen_kvx_any<chunkx>_nez (any_1, op1_1));
    emit_insn (gen_kvx_any<chunkx>_nez (any_2, op1_2));
    emit_insn (gen_kvx_any<chunkx>_nez (any_3, op1_3));
    emit_insn (gen_iordi3 (temp1, any_0, any_1));
    emit_insn (gen_iordi3 (temp2, any_2, any_3));
    emit_insn (gen_iordi3 (operands[0], temp1, temp2));
    DONE;
  }
)

(define_expand "kvx_any<suffix>_eqz"
  [(match_operand:DI 0 "register_operand" "")
   (match_operand:V256L 1 "register_operand" "")]
  ""
  {
    rtx op1_0 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, 0);
    rtx op1_1 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, 8);
    rtx op1_2 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, 16);
    rtx op1_3 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, 24);
    rtx any_0 = gen_reg_rtx (DImode);
    rtx any_1 = gen_reg_rtx (DImode);
    rtx any_2 = gen_reg_rtx (DImode);
    rtx any_3 = gen_reg_rtx (DImode);
    rtx temp1 = gen_reg_rtx (DImode);
    rtx temp2 = gen_reg_rtx (DImode);
    kvx_expand_any64_eqz (any_0, op1_0, <CHUNK>mode);
    kvx_expand_any64_eqz (any_1, op1_1, <CHUNK>mode);
    kvx_expand_any64_eqz (any_2, op1_2, <CHUNK>mode);
    kvx_expand_any64_eqz (any_3, op1_3, <CHUNK>mode);
    emit_insn (gen_kvx_lord (temp1, any_0, any_1));
    emit_insn (gen_kvx_lord (temp2, any_2, any_3));
    emit_insn (gen_iordi3 (operands[0], temp1, temp2));
    DONE;
  }
)


;; SELECT*

(define_insn "kvx_selectw"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r")
                    (match_operand:SI 2 "register_operand" "0")
                    (match_operand:DI 3 "register_operand" "r")
                    (match_operand 4 "" "")] UNSPEC_SELECT))]
  ""
  "cmoved%4 %3? %0 = %1"
  [(set_attr "type" "alu_thin")
   (set_attr "length"      "4")]
)

(define_insn "kvx_selectd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI [(match_operand:DI 1 "register_operand" "r")
                    (match_operand:DI 2 "register_operand" "0")
                    (match_operand:DI 3 "register_operand" "r")
                    (match_operand 4 "" "")] UNSPEC_SELECT))]
  ""
  "cmoved%4 %3? %0 = %1"
  [(set_attr "type" "alu_thin")
   (set_attr "length"      "4")]
)

(define_expand "kvx_select<suffix>"
  [(match_operand:VXQI 0 "register_operand" "")
   (match_operand:VXQI 1 "register_operand" "")
   (match_operand:VXQI 2 "register_operand" "")
   (match_operand:<MASK> 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    if (KV3_1)
      {
        const char *modifier = XSTR (operands[4], 0);
        bool oddeven = !strcmp(modifier, ".odd") || !strcmp(modifier, ".even");
        rtx op3o = gen_reg_rtx (<HWIDE>mode), op3e = gen_reg_rtx (<HWIDE>mode);
        if (oddeven)
          {
            emit_insn (gen_kvx_zxo<hwidenx> (op3o, operands[3]));
            emit_insn (gen_kvx_zxe<hwidenx> (op3e, operands[3]));
          }
        else
          {
            emit_insn (gen_kvx_qxo<hwidenx> (op3o, operands[3]));
            emit_insn (gen_kvx_qxe<hwidenx> (op3e, operands[3]));
          }
        rtx op2 = simplify_gen_subreg (<HWIDE>mode, operands[2], <MODE>mode, 0);
        rtx op1 = simplify_gen_subreg (<HWIDE>mode, operands[1], <MODE>mode, 0);
        rtx op0o = gen_reg_rtx (<HWIDE>mode), op0e = gen_reg_rtx (<HWIDE>mode);
        rtvec veco = gen_rtvec (4, op1, op2, op3o, operands[4]);
        rtvec vece = gen_rtvec (4, op1, op2, op3e, operands[4]);
        rtx selecto = gen_rtx_UNSPEC (<HWIDE>mode, veco, UNSPEC_SELECT);
        rtx selecte = gen_rtx_UNSPEC (<HWIDE>mode, vece, UNSPEC_SELECT);
        emit_insn (gen_rtx_SET (op0o, selecto));
        emit_insn (gen_rtx_SET (op0e, selecte));
        rtx opto = gen_reg_rtx (<MODE>mode), opte = gen_reg_rtx (<MODE>mode);
        emit_insn (gen_rtx_SET (opto, gen_rtx_SUBREG (<MODE>mode, op0o, 0)));
        emit_insn (gen_kvx_qxo<hwidenx> (op0o, opto));
        emit_insn (gen_rtx_SET (opte, gen_rtx_SUBREG (<MODE>mode, op0e, 0)));
        emit_insn (gen_kvx_zxe<hwidenx> (op0e, opte));
        emit_insn (gen_kvx_oroe<suffix> (operands[0], op0o, op0e));
      }
    else
      {
        rtvec vec = gen_rtvec (4, operands[1], operands[2], operands[3], operands[4]);
        rtx select = gen_rtx_UNSPEC (<MODE>mode, vec, UNSPEC_SELECT);
        emit_insn (gen_rtx_SET (operands[0], select));
      }
    DONE;
  }
)
(define_insn "*kvx_selectbo_2"
  [(set (match_operand:V8QI 0 "register_operand" "=r")
        (unspec:V8QI [(match_operand:V8QI 1 "register_operand" "r")
                      (match_operand:V8QI 2 "register_operand" "0")
                      (match_operand:V8QI 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_SELECT))]
  "KV3_2"
  "cmovebo%4 %3? %0 = %1"
  [(set_attr "type" "alu_tiny")]
)
(define_insn "*kvx_selectbx_2"
  [(set (match_operand:V16QI 0 "register_operand" "=r")
        (unspec:V16QI [(match_operand:V16QI 1 "register_operand" "r")
                       (match_operand:V16QI 2 "register_operand" "0")
                       (match_operand:V16QI 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECT))]
  "KV3_2"
  "cmovebo%4 %x3? %x0 = %x1\n\tcmovebo%4 %y3? %y0 = %y1"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)
(define_insn "*kvx_selectbv_2"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (unspec:V32QI [(match_operand:V32QI 1 "register_operand" "r")
                       (match_operand:V32QI 2 "register_operand" "0")
                       (match_operand:V32QI 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECT))]
  "KV3_2"
  {
    return "cmovebo%4 %x3? %x0 = %x1\n\tcmovebo%4 %y3? %y0 = %y1\n\t"
           "cmovebo%4 %z3? %z0 = %z1\n\tcmovebo%4 %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn "kvx_select<suffix>"
  [(set (match_operand:S64I 0 "register_operand" "=r")
        (unspec:S64I [(match_operand:S64I 1 "register_operand" "r")
                      (match_operand:S64I 2 "register_operand" "0")
                      (match_operand:<MASK> 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_SELECT))]
  ""
  "cmove<suffix>%4 %3? %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_select<suffix>"
  [(set (match_operand:V128J 0 "register_operand" "=r")
        (unspec:V128J [(match_operand:V128J 1 "register_operand" "r")
                       (match_operand:V128J 2 "register_operand" "0")
                       (match_operand:<MASK> 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECT))]
  ""
  "cmove<chunkx>%4 %x3? %x0 = %x1\n\tcmove<chunkx>%4 %y3? %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_select<suffix>"
  [(set (match_operand:V256J 0 "register_operand" "")
        (unspec:V256J [(match_operand:V256J 1 "register_operand" "")
                       (match_operand:V256J 2 "register_operand" "")
                       (match_operand:V256J 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_SELECT))]
  ""
  ""
)

(define_insn_and_split "kvx_select<suffix>_1"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (unspec:V256J [(match_operand:V256J 1 "register_operand" "r")
                       (match_operand:V256J 2 "register_operand" "0")
                       (match_operand:V256J 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECT))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)
                        (subreg:<HALF> (match_dup 3) 0)
                        (match_dup 4)] UNSPEC_SELECT))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)
                        (subreg:<HALF> (match_dup 3) 16)
                        (match_dup 4)] UNSPEC_SELECT))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "kvx_select<suffix>_2"
  [(set (match_operand:V256J 0 "register_operand" "=r")
        (unspec:V256J [(match_operand:V256J 1 "register_operand" "r")
                       (match_operand:V256J 2 "register_operand" "0")
                       (match_operand:V256J 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECT))]
  "KV3_2"
  {
    return "cmove<chunkx>%4 %x3? %x0 = %x1\n\tcmove<chunkx>%4 %y3? %y0 = %y1\n\t"
           "cmove<chunkx>%4 %z3? %z0 = %z1\n\tcmove<chunkx>%4 %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; SELECTF*

(define_insn "kvx_selectfw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "0")
                    (match_operand:DI 3 "register_operand" "r")
                    (match_operand 4 "" "")] UNSPEC_SELECT))]
  ""
  "cmoved%4 %3? %0 = %1"
  [(set_attr "type" "alu_thin")
   (set_attr "length"      "4")]
)

(define_insn "kvx_selectfd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:DF 2 "register_operand" "0")
                    (match_operand:DI 3 "register_operand" "r")
                    (match_operand 4 "" "")] UNSPEC_SELECT))]
  ""
  "cmoved%4 %3? %0 = %1"
  [(set_attr "type" "alu_thin")
   (set_attr "length"      "4")]
)

(define_insn "kvx_selectf<suffix>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:S64F 1 "register_operand" "r")
                      (match_operand:S64F 2 "register_operand" "0")
                      (match_operand:<MASK> 3 "register_operand" "r")
                      (match_operand 4 "" "")] UNSPEC_SELECT))]
  ""
  "cmove<suffix>%4 %3? %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_selectf<suffix>"
  [(set (match_operand:V128F 0 "register_operand" "=r")
        (unspec:V128F [(match_operand:V128F 1 "register_operand" "r")
                       (match_operand:V128F 2 "register_operand" "0")
                       (match_operand:<MASK> 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECT))]
  ""
  "cmove<chunkx>%4 %x3? %x0 = %x1\n\tcmove<chunkx>%4 %y3? %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_selectf<suffix>"
  [(set (match_operand:V256F 0 "register_operand" "")
        (unspec:V256F [(match_operand:V256F 1 "register_operand" "")
                       (match_operand:V256F 2 "register_operand" "")
                       (match_operand:<MASK> 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_SELECT))]
  ""
  ""
)

(define_insn_and_split "kvx_selectf<suffix>_1"
  [(set (match_operand:V256F 0 "register_operand" "=r")
        (unspec:V256F [(match_operand:V256F 1 "register_operand" "r")
                       (match_operand:V256F 2 "register_operand" "0")
                       (match_operand:<MASK> 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECT))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)
                        (subreg:<HMASK> (match_dup 3) 0)
                        (match_dup 4)] UNSPEC_SELECT))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)
                        (subreg:<HMASK> (match_dup 3) 16)
                        (match_dup 4)] UNSPEC_SELECT))]
  ""
  [(set_attr "type" "alu_lite_x2")]
)

(define_insn "kvx_selectf<suffix>_2"
  [(set (match_operand:V256F 0 "register_operand" "=r")
        (unspec:V256F [(match_operand:V256F 1 "register_operand" "r")
                       (match_operand:V256F 2 "register_operand" "0")
                       (match_operand:<MASK> 3 "register_operand" "r")
                       (match_operand 4 "" "")] UNSPEC_SELECT))]
  "KV3_2"
  {
    return "cmove<chunkx>%4 %x3? %x0 = %x1\n\tcmove<chunkx>%4 %y3? %y0 = %y1\n\t"
           "cmove<chunkx>%4 %z3? %z0 = %z1\n\tcmove<chunkx>%4 %t3? %t0 = %t1";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; STSU*

(define_insn "kvx_stsuhq"
  [(set (match_operand:V4HI 0 "register_operand" "=r")
        (unspec:V4HI [(match_operand:V4HI 1 "register_operand" "r")
                      (match_operand:V4HI 2 "register_operand" "r")] UNSPEC_STSU))]
  "KV3_2"
  "stsuhq %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_stsuho"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (unspec:V8HI [(match_operand:V8HI 1 "register_operand" "r")
                      (match_operand:V8HI 2 "register_operand" "r")] UNSPEC_STSU))]
  "KV3_2"
  "stsuhq %x0 = %x1, %x2\n\tstsuhq %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_stsuhx"
  [(set (match_operand:V16HI 0 "register_operand" "=r")
        (unspec:V16HI [(match_operand:V16HI 1 "register_operand" "r")
                       (match_operand:V16HI 2 "register_operand" "r")] UNSPEC_STSU))]
  "KV3_2"
  {
    return "stsuhq %x0 = %x1, %x2\n\tstsuhq %y0 = %y1, %y2\n\t"
           "stsuhq %z0 = %z1, %z2\n\tstsuhq %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "kvx_stsuhv"
  [(set (match_operand:V32HI 0 "register_operand" "=r")
        (unspec:V32HI [(match_operand:V32HI 1 "register_operand" "r")
                       (match_operand:V32HI 2 "register_operand" "r")] UNSPEC_STSU))]
  "KV3_2"
  "#"
  "KV3_2 && reload_completed"
  [(set (subreg:V16HI (match_dup 0) 0)
        (unspec:V16HI [(subreg:V16HI (match_dup 1) 0)
                       (subreg:V16HI (match_dup 2) 0)] UNSPEC_STSU))
   (set (subreg:V16HI (match_dup 0) 32)
        (unspec:V16HI [(subreg:V16HI (match_dup 1) 32)
                       (subreg:V16HI (match_dup 2) 32)] UNSPEC_STSU))]
  ""
  [(set_attr "type" "alu_full")]
)

(define_insn "kvx_stsuwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V2SI 1 "register_operand" "r")
                      (match_operand:V2SI 2 "register_operand" "r")] UNSPEC_STSU))]
  "KV3_2"
  "stsuwp %0 = %1, %2"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "kvx_stsuwq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unspec:V4SI [(match_operand:V4SI 1 "register_operand" "r")
                      (match_operand:V4SI 2 "register_operand" "r")] UNSPEC_STSU))]
  "KV3_2"
  "stsuwp %x0 = %x1, %x2\n\tstsuwp %y0 = %y1, %y2"
  [(set_attr "type" "alu_tiny_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_stsuwo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (unspec:V8SI [(match_operand:V8SI 1 "register_operand" "r")
                      (match_operand:V8SI 2 "register_operand" "r")] UNSPEC_STSU))]
  "KV3_2"
  {
    return "stsuwp %x0 = %x1, %x2\n\tstsuwp %y0 = %y1, %y2\n\t"
           "stsuwp %z0 = %z1, %z2\n\tstsuwp %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "kvx_stsuwx"
  [(set (match_operand:V16SI 0 "register_operand" "=r")
        (unspec:V16SI [(match_operand:V16SI 1 "register_operand" "r")
                       (match_operand:V16SI 2 "register_operand" "r")] UNSPEC_STSU))]
  "KV3_2"
  "#"
  "KV3_2 && reload_completed"
  [(set (subreg:V8SI (match_dup 0) 0)
        (unspec:V8SI [(subreg:V8SI (match_dup 1) 0)
                      (subreg:V8SI (match_dup 2) 0)] UNSPEC_STSU))
   (set (subreg:V8SI (match_dup 0) 32)
        (unspec:V8SI [(subreg:V8SI (match_dup 1) 32)
                      (subreg:V8SI (match_dup 2) 32)] UNSPEC_STSU))]
  ""
  [(set_attr "type" "alu_full")]
)

(define_insn "kvx_stsudp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2DI 1 "register_operand" "r")
                      (match_operand:V2DI 2 "register_operand" "r")] UNSPEC_STSU))]
  ""
  "stsud %x0 = %x1, %x2\n\tstsud %y0 = %y1, %y2"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_expand "kvx_stsudq"
  [(set (match_operand:V4DI 0 "register_operand" "")
        (unspec:V4DI [(match_operand:V4DI 1 "register_operand" "")
                      (match_operand:V4DI 2 "register_operand" "")] UNSPEC_STSU))]
  ""
  ""
)

(define_insn_and_split "kvx_stsudq_1"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operand:V4DI 1 "register_operand" "r")
                      (match_operand:V4DI 2 "register_operand" "r")] UNSPEC_STSU))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:V2DI (match_dup 0) 0)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 0)
                      (subreg:V2DI (match_dup 2) 0)] UNSPEC_STSU))
   (set (subreg:V2DI (match_dup 0) 16)
        (unspec:V2DI [(subreg:V2DI (match_dup 1) 16)
                      (subreg:V2DI (match_dup 2) 16)] UNSPEC_STSU))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "kvx_stsudq_2"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operand:V4DI 1 "register_operand" "r")
                      (match_operand:V4DI 2 "register_operand" "r")] UNSPEC_STSU))]
  "KV3_2"
  {
    return "stsud %x0 = %x1, %x2\n\tstsud %y0 = %y1, %y2\n\t"
           "stsud %z0 = %z1, %z2\n\tstsud %t0 = %t1, %t2";
  }
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)

(define_insn_and_split "kvx_stsudo"
  [(set (match_operand:V8DI 0 "register_operand" "=r")
        (unspec:V8DI [(match_operand:V8DI 1 "register_operand" "r")
                      (match_operand:V8DI 2 "register_operand" "r")] UNSPEC_STSU))]
  ""
  "#"
  ""
  [(set (subreg:V4DI (match_dup 0) 0)
        (unspec:V4DI [(subreg:V4DI (match_dup 1) 0)
                      (subreg:V4DI (match_dup 2) 0)] UNSPEC_STSU))
   (set (subreg:V4DI (match_dup 0) 32)
        (unspec:V4DI [(subreg:V4DI (match_dup 1) 32)
                      (subreg:V4DI (match_dup 2) 32)] UNSPEC_STSU))]
  ""
  [(set_attr "type" "alu_full")]
)


;; FREC*

(define_insn "*kvx_frecw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(subreg:SF (match_operand:V2SF 1 "register_operand" "r") 0)
                    (match_operand 2 "" "")] UNSPEC_FREC))]
  ""
  "frecw%2 %0 = %1"
  [(set_attr "type" "alu_full_sfu")]
)

(define_insn "kvx_frecw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FREC))]
  ""
  "frecw%2 %0 = %1"
  [(set_attr "type" "alu_full_sfu")]
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
    emit_insn (gen_kvx_catfwp (operands[0], op0x, op0y));
    DONE;
  }
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


;; FRSR*

(define_insn "*kvx_frsrw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(subreg:SF (match_operand:V2SF 1 "register_operand" "r") 0)
                    (match_operand 2 "" "")] UNSPEC_FRSR))]
  ""
  "frsrw%2 %0 = %1"
  [(set_attr "type" "alu_full_sfu")]
)

(define_insn "kvx_frsrw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FRSR))]
  ""
  "frsrw%2 %0 = %1"
  [(set_attr "type" "alu_full_sfu")]
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
    emit_insn (gen_kvx_catfwp (operands[0], op0x, op0y));
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

;; FADD*C

(define_insn "kvx_faddwc"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "faddwc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_faddwcp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "faddwcp%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_fadddc"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "fadddc%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fadddcp"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 0)
                      (subreg:V2DF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADD))
   (set (subreg:V2DF (match_dup 0) 16)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 16)
                      (subreg:V2DF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADD))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_faddwcq"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADD))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADD))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

;; FADD*

(define_insn "kvx_faddh"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (unspec:HF [(match_operand:HF 1 "register_operand" "r")
                    (match_operand:HF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "faddhq%3 %0 = %1, %2"
  [(set_attr "type" "mau_fp16")]
)

(define_insn "kvx_faddw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "faddw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_faddd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:DF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "faddd%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fadd<suffix>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:S64F 1 "register_operand" "r")
                      (match_operand:S64F 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "fadd<suffix>%3 %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_fp16") (const_string "mau_fpu")))]
)

(define_expand "kvx_faddho"
  [(set (match_operand:V8HF 0 "register_operand" "")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "")
                      (match_operand:V8HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  ""
)

(define_insn_and_split "kvx_faddho_1"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADD))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FADD))]
  ""
)

(define_insn "kvx_faddho_2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  "KV3_2"
  "faddho%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fp16")]
)

(define_insn "kvx_faddwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "faddwq%3 %0 = %1, %2"
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "mau_auxr_fpu") (const_string "mau_fpu")))]
)

(define_insn "kvx_fadddp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "fadddp%3 %0 = %1, %2"
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "mau_auxr_fpu") (const_string "mau_fpu")))]
)

(define_insn "kvx_faddhx"
  [(set (match_operand:V16HF 0 "register_operand" "=r")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "r")
                      (match_operand:V16HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADD))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FADD))
   (set (subreg:V4HF (match_dup 0) 16)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 16)
                      (subreg:V4HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADD))
   (set (subreg:V4HF (match_dup 0) 24)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 24)
                      (subreg:V4HF (match_dup 2) 24)
                      (match_dup 3)] UNSPEC_FADD))]
  ""
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  "KV3_2 && reload_completed"
  [(set (subreg:V8HF (match_dup 0) 0)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 0)
                      (subreg:V8HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADD))
   (set (subreg:V8HF (match_dup 0) 16)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 16)
                      (subreg:V8HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADD))]
  ""
)

(define_insn_and_split "kvx_faddwo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADD))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADD))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fadddq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADD))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 0)
                      (subreg:V2DF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FADD))
   (set (subreg:V2DF (match_dup 0) 16)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 16)
                      (subreg:V2DF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FADD))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

;; FSBF*C

(define_insn "kvx_fsbfwc"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "fsbfwc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fsbfwcp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "fsbfwcp%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_fsbfdc"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "fsbfdc%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fsbfdcp"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 0)
                      (subreg:V2DF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBF))
   (set (subreg:V2DF (match_dup 0) 16)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 16)
                      (subreg:V2DF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBF))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fsbfwcq"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBF))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBF))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

;; FSBF*

(define_insn "kvx_fsbfh"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (unspec:HF [(match_operand:HF 1 "register_operand" "r")
                    (match_operand:HF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "fsbfhq%3 %0 = %1, %2"
  [(set_attr "type" "mau_fp16")]
)

(define_insn "kvx_fsbfw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "fsbfw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fsbfd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:DF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "fsbfd%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fsbf<suffix>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:S64F 1 "register_operand" "r")
                      (match_operand:S64F 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "fsbf<suffix>%3 %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_fp16") (const_string "mau_fpu")))]
)

(define_expand "kvx_fsbfho"
  [(set (match_operand:V8HF 0 "register_operand" "")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "")
                      (match_operand:V8HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  ""
)

(define_insn_and_split "kvx_fsbfho_1"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBF))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FSBF))]
  ""
)

(define_insn "kvx_fsbfho_2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  "KV3_2"
  "fsbfho%3 %0 = %2, %1"
  [(set_attr "type" "mau_fp16")]
)

(define_insn "kvx_fsbfwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "fsbfwq%3 %0 = %1, %2"
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "mau_auxr_fpu") (const_string "mau_fpu")))]
)

(define_insn "kvx_fsbfdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "fsbfdp%3 %0 = %1, %2"
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "mau_auxr_fpu") (const_string "mau_fpu")))]
)

(define_insn "kvx_fsbfhx"
  [(set (match_operand:V16HF 0 "register_operand" "=r")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "r")
                      (match_operand:V16HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBF))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FSBF))
   (set (subreg:V4HF (match_dup 0) 16)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 16)
                      (subreg:V4HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBF))
   (set (subreg:V4HF (match_dup 0) 24)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 24)
                      (subreg:V4HF (match_dup 2) 24)
                      (match_dup 3)] UNSPEC_FSBF))]
  ""
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  "KV3_2 && reload_completed"
  [(set (subreg:V8HF (match_dup 0) 0)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 0)
                      (subreg:V8HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBF))
   (set (subreg:V8HF (match_dup 0) 16)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 16)
                      (subreg:V8HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBF))]
  ""
)

(define_insn_and_split "kvx_fsbfwo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBF))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBF))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fsbfdq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBF))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2DF (match_dup 0) 0)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 0)
                      (subreg:V2DF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FSBF))
   (set (subreg:V2DF (match_dup 0) 16)
        (unspec:V2DF [(subreg:V2DF (match_dup 1) 16)
                      (subreg:V2DF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FSBF))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

;; FMUL*

(define_insn "kvx_fmulh"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (unspec:HF [(match_operand:HF 1 "register_operand" "r")
                    (match_operand:HF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FMUL))]
  ""
  "fmulhq%3 %0 = %1, %2"
  [(set_attr "type" "mau_fp16")]
)

(define_insn "kvx_fmulw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FMUL))]
  ""
  "fmulw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fmuld"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:DF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FMUL))]
  ""
  "fmuld%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fmul<suffix>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:S64F 1 "register_operand" "r")
                      (match_operand:S64F 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMUL))]
  ""
  "fmul<suffix>%3 %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_fp16") (const_string "mau_fpu")))]
)

(define_expand "kvx_fmulho"
  [(set (match_operand:V8HF 0 "register_operand" "")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "")
                      (match_operand:V8HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMUL))]
  ""
  ""
)

(define_insn_and_split "kvx_fmulho_1"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMUL))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMUL))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FMUL))]
  ""
)

(define_insn "kvx_fmulho_2"
  [(set (match_operand:V8HF 0 "register_operand" "=r")
        (unspec:V8HF [(match_operand:V8HF 1 "register_operand" "r")
                      (match_operand:V8HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMUL))]
  "KV3_2"
  "fmulho%3 %0 = %1, %2"
  [(set_attr "type" "mau_fp16")]
)

(define_insn "kvx_fmulwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMUL))]
  ""
  "fmulwq%3 %0 = %1, %2"
  [(set (attr "type")
        (if_then_else (match_test "KV3_1")
                      (const_string "mau_auxr_fpu") (const_string "mau_fpu")))]
)

(define_insn_and_split "kvx_fmuldp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMUL))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DF (match_dup 1) 0)
                    (subreg:DF (match_dup 2) 0)
                    (match_dup 3)] UNSPEC_FMUL))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DF (match_dup 1) 8)
                    (subreg:DF (match_dup 2) 8)
                    (match_dup 3)] UNSPEC_FMUL))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn"kvx_fmulhx"
  [(set (match_operand:V16HF 0 "register_operand" "=r")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "r")
                      (match_operand:V16HF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMUL))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMUL))]
  "KV3_1 && reload_completed"
  [(set (subreg:V4HF (match_dup 0) 0)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 0)
                      (subreg:V4HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMUL))
   (set (subreg:V4HF (match_dup 0) 8)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 8)
                      (subreg:V4HF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FMUL))
   (set (subreg:V4HF (match_dup 0) 16)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 16)
                      (subreg:V4HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMUL))
   (set (subreg:V4HF (match_dup 0) 24)
        (unspec:V4HF [(subreg:V4HF (match_dup 1) 24)
                      (subreg:V4HF (match_dup 2) 24)
                      (match_dup 3)] UNSPEC_FMUL))]
  ""
)

(define_split
  [(set (match_operand:V16HF 0 "register_operand" "")
        (unspec:V16HF [(match_operand:V16HF 1 "register_operand" "")
                      (match_operand:V16HF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMUL))]
  "KV3_2 && reload_completed"
  [(set (subreg:V8HF (match_dup 0) 0)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 0)
                      (subreg:V8HF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMUL))
   (set (subreg:V8HF (match_dup 0) 16)
        (unspec:V8HF [(subreg:V8HF (match_dup 1) 16)
                      (subreg:V8HF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMUL))]
  ""
)

(define_insn_and_split "kvx_fmulwo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMUL))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMUL))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMUL))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn_and_split "kvx_fmuldq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMUL))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DF (match_dup 1) 0)
                    (subreg:DF (match_dup 2) 0)
                    (match_dup 3)] UNSPEC_FMUL))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DF (match_dup 1) 8)
                    (subreg:DF (match_dup 2) 8)
                    (match_dup 3)] UNSPEC_FMUL))
   (set (subreg:DF (match_dup 0) 16)
        (unspec:DF [(subreg:DF (match_dup 1) 16)
                    (subreg:DF (match_dup 2) 16)
                    (match_dup 3)] UNSPEC_FMUL))
   (set (subreg:DF (match_dup 0) 24)
        (unspec:DF [(subreg:DF (match_dup 1) 24)
                    (subreg:DF (match_dup 2) 24)
                    (match_dup 3)] UNSPEC_FMUL))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)


;; FMUL*C

(define_insn "kvx_fmulwc"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULC))]
  ""
  "fmulwc%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_fmulwcp"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMULC))]
  ""
  ""
)

(define_insn_and_split "kvx_fmulwcp_1"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULC))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 0)
                      (subreg:V2SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMULC))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 8)
                      (subreg:V2SF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FMULC))]
  ""
)

(define_insn "kvx_fmulwcp_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULC))]
  "KV3_2"
  "fmulwcp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_fmuldc"
  [(set (match_operand:V2DF 0 "register_operand")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand")
                      (match_operand:V2DF 2 "register_operand")
                      (match_operand 3 "" "")] UNSPEC_FMULC))]
  ""
  {
    rtx modifiers = operands[3];
    bool conjugate = kvx_modifier_enabled_p (".c", modifiers);
    if (conjugate)
      modifiers = kvx_modifier_rounding (modifiers);
    rtx real_0 = simplify_gen_subreg (DFmode, operands[0], V2DFmode, 0);
    rtx imag_0 = simplify_gen_subreg (DFmode, operands[0], V2DFmode, 8);
    rtx real_1 = simplify_gen_subreg (DFmode, operands[1], V2DFmode, 0);
    rtx imag_1 = simplify_gen_subreg (DFmode, operands[1], V2DFmode, 8);
    rtx real_2 = simplify_gen_subreg (DFmode, operands[2], V2DFmode, 0);
    rtx imag_2 = simplify_gen_subreg (DFmode, operands[2], V2DFmode, 8);
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

(define_insn "kvx_fmulwcq"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULC))]
  ""
  "#"
)

(define_split
  [(set (match_operand:V8SF 0 "register_operand" "")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMULC))]
  "KV3_1 && reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 0)
                      (subreg:V2SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMULC))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 8)
                      (subreg:V2SF (match_dup 2) 8)
                      (match_dup 3)] UNSPEC_FMULC))
   (set (subreg:V2SF (match_dup 0) 16)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 16)
                      (subreg:V2SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMULC))
   (set (subreg:V2SF (match_dup 0) 24)
        (unspec:V2SF [(subreg:V2SF (match_dup 1) 24)
                      (subreg:V2SF (match_dup 2) 24)
                      (match_dup 3)] UNSPEC_FMULC))]
  ""
)

(define_split
  [(set (match_operand:V8SF 0 "register_operand" "")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMULC))]
  "KV3_2 && reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (match_dup 3)] UNSPEC_FMULC))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (match_dup 3)] UNSPEC_FMULC))]
  ""
)

(define_expand "kvx_fmuldcp"
  [(set (match_operand:V4DF 0 "register_operand" "")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "")
                      (match_operand:V4DF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMULC))]
  ""
  {
    rtx modifiers = operands[3];
    bool conjugate = kvx_modifier_enabled_p (".c", modifiers);
    if (conjugate)
      modifiers = kvx_modifier_rounding (modifiers);
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


;; FMULX*

(define_insn "kvx_fmulxhw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:HF 1 "register_operand" "r")
                    (match_operand:HF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FMULX))]
  ""
  "fmulhw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fp16")]
)

(define_insn "kvx_fmulxwd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FMULX))]
  ""
  "fmulwd%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fmulx<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64F 1 "register_operand" "r")
                        (match_operand:S64F 2 "register_operand" "r")
                        (match_operand 3 "" "")] UNSPEC_FMULX))]
  ""
  "fmul<widenx>%3 %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_fp16") (const_string "mau_fpu")))]
)

(define_insn_and_split "kvx_fmulx<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
        (unspec:<WIDE> [(match_operand:S128F 1 "register_operand" "r")
                        (match_operand:S128F 2 "register_operand" "r")
                        (match_operand 3 "" "")] UNSPEC_FMULX))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (unspec:<HWIDE> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (match_dup 3)] UNSPEC_FMULX))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (unspec:<HWIDE> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (match_dup 3)] UNSPEC_FMULX))]
  ""
)

;; FDIV*C

(define_expand "kvx_fdivwc"
  [(match_operand:V2SF 0 "register_operand")
   (match_operand:V2SF 1 "register_operand")
   (match_operand:V2SF 2 "register_operand")
   (match_operand 3 "" "")]
  ""
  {
     bool conjugate = kvx_modifier_enabled_p (".c", operands[3]);
     rtx rnd = kvx_modifier_rounding (operands[3]);
     rtx conj = gen_rtx_CONST_STRING (VOIDmode, ".c");
     rtx conj_rnd = gen_rtx_CONCAT (VOIDmode, conj, rnd);
     rtx bmag = gen_reg_rtx (SFmode);
     rtx bmag_inv = gen_reg_rtx (SFmode);
     rtx bmag_inv_splat = gen_reg_rtx (V2SFmode);
     rtx a_conjb = gen_reg_rtx (V2SFmode);
     if (conjugate)
       {
         rtx cplx1_rtx = gen_reg_rtx (V2SFmode);
         rtx const0f_reg = gen_reg_rtx (SFmode);
         rtx const1f_reg = gen_reg_rtx (SFmode);
         emit_insn (gen_rtx_SET (const0f_reg, CONST0_RTX (SFmode)));
         emit_insn (gen_rtx_SET (const1f_reg, CONST1_RTX (SFmode)));
         emit_insn (gen_kvx_catfwp (cplx1_rtx, const1f_reg, const0f_reg));
         emit_insn (gen_kvx_fmulwc (operands[1], operands[1], cplx1_rtx, conj_rnd));
       }
     if (KV3_1)
       emit_insn (gen_kvx_ffdmaw (bmag, operands[2], operands[2], rnd));
     else
       emit_insn (gen_kvx_ffdmaw_2 (bmag, operands[2], operands[2], rnd));

     emit_insn (gen_kvx_frecw (bmag_inv, bmag, rnd));
     emit_insn (gen_kvx_catfwp (bmag_inv_splat, bmag_inv, bmag_inv));
     emit_insn (gen_kvx_fmulwc (a_conjb, operands[2], operands[1], conj_rnd));
     emit_insn (gen_kvx_fmulwp (operands[0], a_conjb, bmag_inv_splat, rnd));
     DONE;
  })

(define_expand "kvx_fdivwcp"
  [(match_operand:V4SF 0 "register_operand")
   (match_operand:V4SF 1 "register_operand")
   (match_operand:V4SF 2 "register_operand")
   (match_operand 3 "" "")]
  ""
  {
    if (KV3_1)
      emit_insn (gen_kvx_fdivwcp_1 (operands[0], operands[1], operands[2], operands[3]));
    if (KV3_2)
      emit_insn (gen_kvx_fdivwcp_2 (operands[0], operands[1], operands[2], operands[3]));
    DONE;
  })

(define_expand "kvx_fdivwcp_1"
  [(match_operand:V4SF 0 "register_operand")
   (match_operand:V4SF 1 "register_operand")
   (match_operand:V4SF 2 "register_operand")
   (match_operand 3 "" "")]

  "KV3_1"
  {
    rtx high1 = simplify_gen_subreg (V2SFmode, operands[1], V4SFmode, 0);
    rtx high2 = simplify_gen_subreg (V2SFmode, operands[2], V4SFmode, 0);
    rtx high = gen_reg_rtx (V2SFmode);
    emit_insn (gen_kvx_fdivwc (high, high1, high2, operands[3]));
    rtx low1 = simplify_gen_subreg (V2SFmode, operands[1], V4SFmode, 8);
    rtx low2 = simplify_gen_subreg (V2SFmode, operands[2], V4SFmode, 8);
    rtx low = gen_reg_rtx (V2SFmode);
    emit_insn (gen_kvx_fdivwc (low, low1, low2, operands[3]));
    emit_insn (gen_kvx_catfwq (operands[0], high, low));
    DONE;
  })

(define_expand "kvx_fdivwcp_2"
  [(match_operand:V4SF 0 "register_operand")
   (match_operand:V4SF 1 "register_operand")
   (match_operand:V4SF 2 "register_operand")
   (match_operand 3 "" "")]

  "KV3_2"
  {
     bool conjugate = kvx_modifier_enabled_p (".c", operands[3]);
     rtx rnd = kvx_modifier_rounding (operands[3]);
     rtx conj = gen_rtx_CONST_STRING (VOIDmode, ".c");
     rtx conj_rnd = gen_rtx_CONCAT (VOIDmode, conj, rnd);
     rtx b = simplify_gen_subreg (V2SFmode, operands[2], V4SFmode, 0);
     rtx d = simplify_gen_subreg (V2SFmode, operands[2], V4SFmode, 8);
     rtx bmag = gen_reg_rtx (SFmode);
     rtx dmag = gen_reg_rtx (SFmode);
     rtx bdmag = gen_reg_rtx (V2SFmode);
     rtx bdmag_inv = gen_reg_rtx (V2SFmode);
     rtx bdmag_inv_splat = gen_reg_rtx (V4SFmode);
     rtx ac_conjbd = gen_reg_rtx (V4SFmode);
     if (conjugate)
       {
         rtx cplx1_rtx = gen_reg_rtx (V2SFmode);
         rtx cplx1p_rtx = gen_reg_rtx (V4SFmode);
         rtx const0f_reg = gen_reg_rtx (SFmode);
         rtx const1f_reg = gen_reg_rtx (SFmode);
         emit_insn (gen_rtx_SET (const0f_reg, CONST0_RTX (SFmode)));
         emit_insn (gen_rtx_SET (const1f_reg, CONST1_RTX (SFmode)));
         emit_insn (gen_kvx_catfwp (cplx1_rtx, const1f_reg, const0f_reg));
         emit_insn (gen_kvx_catfwq (cplx1p_rtx, cplx1_rtx, cplx1_rtx));
         emit_insn (gen_kvx_fmulwcp (operands[1], operands[1], cplx1p_rtx, conj_rnd));
       }
     emit_insn (gen_kvx_ffdmaw_2 (bmag, b, b, rnd));
     emit_insn (gen_kvx_ffdmaw_2 (dmag, d, d, rnd));
     emit_insn (gen_kvx_catfwp (bdmag, bmag, dmag));

     emit_insn (gen_kvx_frecwp (bdmag_inv, bdmag, rnd));
     emit_insn (gen_kvx_catfwq (bdmag_inv_splat, bdmag_inv, bdmag_inv));
     emit_insn (gen_kvx_fmt22w (bdmag_inv_splat, bdmag_inv_splat));
     emit_insn (gen_kvx_fmulwcp (ac_conjbd, operands[2], operands[1], conj_rnd));
     emit_insn (gen_kvx_fmulwq (operands[0], ac_conjbd, bdmag_inv_splat, rnd));
     DONE;
  })

(define_expand "kvx_fdivwcq"
  [(match_operand:V8SF 0 "register_operand")
   (match_operand:V8SF 1 "register_operand")
   (match_operand:V8SF 2 "register_operand")
   (match_operand 3 "" "")]

  ""
  {
     rtx high1 = simplify_gen_subreg (V4SFmode, operands[1], V8SFmode, 0);
     rtx high2 = simplify_gen_subreg (V4SFmode, operands[2], V8SFmode, 0);
     rtx high = gen_reg_rtx (V4SFmode);
     emit_insn (gen_kvx_fdivwcp (high, high1, high2, operands[3]));
     rtx low1 = simplify_gen_subreg (V4SFmode, operands[1], V8SFmode, 16);
     rtx low2 = simplify_gen_subreg (V4SFmode, operands[2], V8SFmode, 16);
     rtx low = gen_reg_rtx (V4SFmode);
     emit_insn (gen_kvx_fdivwcp (low, low1, low2, operands[3]));
     rtx high0 = simplify_gen_subreg (V4SFmode, operands[0], V8SFmode, 0);
     rtx low0 = simplify_gen_subreg (V4SFmode, operands[0], V8SFmode, 16);
     emit_insn (gen_rtx_SET (high0, high));
     emit_insn (gen_rtx_SET (low0, low));
     DONE;
  })

(define_expand "kvx_fdivwco"
  [(match_operand:V16SF 0 "register_operand")
   (match_operand:V16SF 1 "register_operand")
   (match_operand:V16SF 2 "register_operand")
   (match_operand 3 "" "")]
  ""
  {
     rtx high1 = simplify_gen_subreg (V8SFmode, operands[1], V16SFmode, 0);
     rtx high2 = simplify_gen_subreg (V8SFmode, operands[2], V16SFmode, 0);
     rtx high = gen_reg_rtx (V8SFmode);
     emit_insn (gen_kvx_fdivwcq (high, high1, high2, operands[3]));
     rtx low1 = simplify_gen_subreg (V8SFmode, operands[1], V16SFmode, 32);
     rtx low2 = simplify_gen_subreg (V8SFmode, operands[2], V16SFmode, 32);
     rtx low = gen_reg_rtx (V8SFmode);
     emit_insn (gen_kvx_fdivwcq (low, low1, low2, operands[3]));
     rtx high0 = simplify_gen_subreg (V8SFmode, operands[0], V16SFmode, 0);
     rtx low0 = simplify_gen_subreg (V8SFmode, operands[0], V16SFmode, 32);
     emit_insn (gen_rtx_SET (high0, high));
     emit_insn (gen_rtx_SET (low0, low));
     DONE;
  })

(define_expand "kvx_fdivdc"
  [(match_operand:V2DF 0 "register_operand")
   (match_operand:V2DF 1 "register_operand")
   (match_operand:V2DF 2 "register_operand")
   (match_operand 3 "" "")]
  ""
  {
     bool conjugate = kvx_modifier_enabled_p (".c", operands[3]);
     rtx rnd = kvx_modifier_rounding (operands[3]);
     rtx conj = gen_rtx_CONST_STRING (VOIDmode, ".c");
     rtx conj_rnd = gen_rtx_CONCAT (VOIDmode, conj, rnd);
     rtx cplx1_rtx = gen_reg_rtx (V2DFmode);
     rtx const0f_reg = gen_reg_rtx (DFmode);
     rtx const1f_reg = gen_reg_rtx (DFmode);
     emit_insn (gen_rtx_SET (const0f_reg, CONST0_RTX (DFmode)));
     emit_insn (gen_rtx_SET (const1f_reg, CONST1_RTX (DFmode)));
     rtx real_2 = simplify_gen_subreg (DFmode, operands[2], V2DFmode, 0);
     rtx imag_2 = simplify_gen_subreg (DFmode, operands[2], V2DFmode, 8);
     rtx bmag_inv = gen_reg_rtx (DFmode);
     rtx bmag_inv_splat = gen_reg_rtx (V2DFmode);
     rtx a_conjb = gen_reg_rtx (V2DFmode);
     if (conjugate)
       {
         emit_insn (gen_kvx_catfdp (cplx1_rtx, const1f_reg, const0f_reg));
         emit_insn (gen_kvx_fmuldc (operands[1], operands[1], cplx1_rtx, conj_rnd));
       }
     emit_insn (gen_kvx_fmuld (bmag_inv, real_2, real_2, rnd));
     emit_insn (gen_kvx_ffmad (bmag_inv, imag_2, imag_2, bmag_inv, rnd));
     emit_insn (gen_divdf3 (bmag_inv, const1f_reg, bmag_inv));
     emit_insn (gen_kvx_catfdp (bmag_inv_splat, bmag_inv, bmag_inv));
     emit_insn (gen_kvx_fmuldc (a_conjb, operands[2], operands[1], conj_rnd));
     emit_insn (gen_kvx_fmuldp (operands[0], a_conjb, bmag_inv_splat, rnd));
     DONE;
  })

(define_expand "kvx_fdivdcp"
  [(match_operand:V4DF 0 "register_operand")
   (match_operand:V4DF 1 "register_operand")
   (match_operand:V4DF 2 "register_operand")
   (match_operand 3 "" "")]
  ""
  {
     rtx high1 = simplify_gen_subreg (V2DFmode, operands[1], V4DFmode, 0);
     rtx high2 = simplify_gen_subreg (V2DFmode, operands[2], V4DFmode, 0);
     rtx high = gen_reg_rtx (V2DFmode);
     emit_insn (gen_kvx_fdivdc (high, high1, high2, operands[3]));
     rtx low1 = simplify_gen_subreg (V2DFmode, operands[1], V4DFmode, 16);
     rtx low2 = simplify_gen_subreg (V2DFmode, operands[2], V4DFmode, 16);
     rtx low = gen_reg_rtx (V2DFmode);
     emit_insn (gen_kvx_fdivdc (low, low1, low2, operands[3]));
     rtx high0 = simplify_gen_subreg (V2DFmode, operands[0], V4DFmode, 0);
     rtx low0 = simplify_gen_subreg (V2DFmode, operands[0], V4DFmode, 16);
     emit_insn (gen_rtx_SET (high0, high));
     emit_insn (gen_rtx_SET (low0, low));
     DONE;
  })

(define_expand "kvx_fdivdcq"
  [(match_operand:V8DF 0 "register_operand")
   (match_operand:V8DF 1 "register_operand")
   (match_operand:V8DF 2 "register_operand")
   (match_operand 3 "" "")]
  ""
  {
     rtx high1 = simplify_gen_subreg (V4DFmode, operands[1], V8DFmode, 0);
     rtx high2 = simplify_gen_subreg (V4DFmode, operands[2], V8DFmode, 0);
     rtx high = gen_reg_rtx (V4DFmode);
     emit_insn (gen_kvx_fdivdcp (high, high1, high2, operands[3]));
     rtx low1 = simplify_gen_subreg (V4DFmode, operands[1], V8DFmode, 32);
     rtx low2 = simplify_gen_subreg (V4DFmode, operands[2], V8DFmode, 32);
     rtx low = gen_reg_rtx (V4DFmode);
     emit_insn (gen_kvx_fdivdcp (low, low1, low2, operands[3]));
     rtx high0 = simplify_gen_subreg (V4DFmode, operands[0], V8DFmode, 0);
     rtx low0 = simplify_gen_subreg (V4DFmode, operands[0], V8DFmode, 32);
     emit_insn (gen_rtx_SET (high0, high));
     emit_insn (gen_rtx_SET (low0, low));
     DONE;
  })

;; FFMA*

(define_insn "kvx_ffmah"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (unspec:HF [(match_operand:HF 1 "register_operand" "r")
                    (match_operand:HF 2 "register_operand" "r")
                    (match_operand:HF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMA))]
  ""
  "ffmahq%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fp16")]
)

(define_insn "kvx_ffmaw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMA))]
  ""
  "ffmaw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_ffmad"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:DF 2 "register_operand" "r")
                    (match_operand:DF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMA))]
  ""
  "ffmad%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_ffma<suffix>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:S64F 1 "register_operand" "r")
                      (match_operand:S64F 2 "register_operand" "r")
                      (match_operand:S64F 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMA))]
  ""
  "ffma<suffix>%4 %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_auxr_fp16") (const_string "mau_auxr_fpu")))]
)

(define_expand "kvx_ffma<suffix>"
  [(set (match_operand:S128F 0 "register_operand" "")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "")
                       (match_operand:S128F 2 "register_operand" "")
                       (match_operand:S128F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMA))]
  ""
  ""
)

(define_insn_and_split "kvx_ffma<suffix>_1"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "r")
                       (match_operand:S128F 2 "register_operand" "r")
                       (match_operand:S128F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMA))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<CHUNK> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMA))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<CHUNK> (match_dup 3) 8)
                         (match_dup 4)] UNSPEC_FFMA))]
  ""
)

(define_insn "kvx_ffma<suffix>_2"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "r")
                       (match_operand:S128F 2 "register_operand" "r")
                       (match_operand:S128F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMA))]
  "KV3_2"
  "ffma<suffix>%4 %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_auxr_fp16") (const_string "mau_auxr_fpu")))]
)

(define_insn_and_split "kvx_ffmadp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand:V2DF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMA))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DF (match_dup 1) 0)
                    (subreg:DF (match_dup 2) 0)
                    (subreg:DF (match_dup 3) 0)
                    (match_dup 4)] UNSPEC_FFMA))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DF (match_dup 1) 8)
                    (subreg:DF (match_dup 2) 8)
                    (subreg:DF (match_dup 3) 8)
                    (match_dup 4)] UNSPEC_FFMA))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_ffma<suffix>"
  [(set (match_operand:S256F 0 "register_operand" "=r")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "r")
                       (match_operand:S256F 2 "register_operand" "r")
                       (match_operand:S256F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMA))]
  ""
  "#"
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "")
                       (match_operand:S256F 2 "register_operand" "")
                       (match_operand:S256F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMA))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<CHUNK> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMA))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<CHUNK> (match_dup 3) 8)
                         (match_dup 4)] UNSPEC_FFMA))
   (set (subreg:<CHUNK> (match_dup 0) 16)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 16)
                         (subreg:<CHUNK> (match_dup 2) 16)
                         (subreg:<CHUNK> (match_dup 3) 16)
                         (match_dup 4)] UNSPEC_FFMA))
   (set (subreg:<CHUNK> (match_dup 0) 24)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 24)
                         (subreg:<CHUNK> (match_dup 2) 24)
                         (subreg:<CHUNK> (match_dup 3) 24)
                         (match_dup 4)] UNSPEC_FFMA))]
  ""
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "")
                       (match_operand:S256F 2 "register_operand" "")
                       (match_operand:S256F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMA))]
  "KV3_2 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)
                        (subreg:<HALF> (match_dup 3) 0)
                        (match_dup 4)] UNSPEC_FFMA))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)
                        (subreg:<HALF> (match_dup 3) 16)
                        (match_dup 4)] UNSPEC_FFMA))]
  ""
)

(define_insn_and_split "kvx_ffmadq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand:V4DF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMA))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DF (match_dup 1) 0)
                    (subreg:DF (match_dup 2) 0)
                    (subreg:DF (match_dup 3) 0)
                    (match_dup 4)] UNSPEC_FFMA))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DF (match_dup 1) 8)
                    (subreg:DF (match_dup 2) 8)
                    (subreg:DF (match_dup 3) 8)
                    (match_dup 4)] UNSPEC_FFMA))
   (set (subreg:DF (match_dup 0) 16)
        (unspec:DF [(subreg:DF (match_dup 1) 16)
                    (subreg:DF (match_dup 2) 16)
                    (subreg:DF (match_dup 3) 16)
                    (match_dup 4)] UNSPEC_FFMA))
   (set (subreg:DF (match_dup 0) 24)
        (unspec:DF [(subreg:DF (match_dup 1) 24)
                    (subreg:DF (match_dup 2) 24)
                    (subreg:DF (match_dup 3) 24)
                    (match_dup 4)] UNSPEC_FFMA))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)


;; FFMA*C

(define_expand "kvx_ffmawc"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "")
                      (match_operand:V2SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMAC))]
  ""
  {
    if (KV3_1)
      {
        rtx product = gen_reg_rtx (V2SFmode);
        emit_insn (gen_kvx_fmulwc (product, operands[2], operands[1], operands[4]));
        emit_insn (gen_kvx_faddwc (operands[0], product, operands[3], operands[4]));
        DONE;
      }
  }
)

(define_insn "kvx_ffmawc_2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMAC))]
  "KV3_2"
  "ffmawc%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffmawcp"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMAC))]
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
            emit_insn (gen_kvx_faddwc (opnd0, product, opnd3, operands[4]));
          }
        DONE;
      }
  }
)

(define_insn "kvx_ffmawcp_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMAC))]
  "KV3_2"
  "ffmawcp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffmadc"
  [(set (match_operand:V2DF 0 "register_operand")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand")
                      (match_operand:V2DF 2 "register_operand")
                      (match_operand:V2DF 3 "register_operand")
                      (match_operand 4 "" "")] UNSPEC_FFMAC))]
  ""
  {
    rtx product = gen_reg_rtx (V2DFmode);
    emit_insn (gen_kvx_fmuldc (product, operands[2], operands[1], operands[4]));
    emit_insn (gen_kvx_fadddp (operands[0], product, operands[3], operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffmawcq"
  [(set (match_operand:V8SF 0 "register_operand" "")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V8SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMAC))]
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
            emit_insn (gen_kvx_faddwc (opnd0, product, opnd3, operands[4]));
          }
        DONE;
      }
  }
)

(define_insn_and_split "kvx_ffmawcq_2"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V8SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMAC))]
  "KV3_2"
  "#"
  "KV3_2 && reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (subreg:V4SF (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_FFMAC))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (subreg:V4SF (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_FFMAC))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffmadcp"
  [(set (match_operand:V4DF 0 "register_operand")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand")
                      (match_operand:V4DF 2 "register_operand")
                      (match_operand:V4DF 3 "register_operand")
                      (match_operand 4 "" "")] UNSPEC_FFMAC))]
  ""
  {
    rtx product = gen_reg_rtx (V4DFmode);
    emit_insn (gen_kvx_fmuldcp (product, operands[2], operands[1], operands[4]));
    emit_insn (gen_kvx_fadddq (operands[0], product, operands[3], operands[4]));
    DONE;
  }
)


;; FFMAX*

(define_insn "kvx_ffmaxhw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:HF 1 "register_operand" "r")
                    (match_operand:HF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMAX))]
  ""
  "ffmahw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fp16")]
)

(define_insn "kvx_ffmaxwd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand:DF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMAX))]
  ""
  "ffmawd%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_ffmax<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64F 1 "register_operand" "r")
                        (match_operand:S64F 2 "register_operand" "r")
                        (match_operand:<WIDE> 3 "register_operand" "0")
                        (match_operand 4 "" "")] UNSPEC_FFMAX))]
  ""
  "ffma<widenx>%4 %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_auxr_fp16") (const_string "mau_auxr_fpu")))]
)

(define_insn_and_split "kvx_ffmax<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
        (unspec:<WIDE> [(match_operand:S128F 1 "register_operand" "r")
                        (match_operand:S128F 2 "register_operand" "r")
                        (match_operand:<WIDE> 3 "register_operand" "0")
                        (match_operand 4 "" "")] UNSPEC_FFMAX))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (unspec:<HWIDE> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<HWIDE> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMAX))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (unspec:<HWIDE> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<HWIDE> (match_dup 3) 16)
                         (match_dup 4)] UNSPEC_FFMAX))]
  ""
)


;; FFMS*

(define_insn "kvx_ffmsh"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (unspec:HF [(match_operand:HF 1 "register_operand" "r")
                    (match_operand:HF 2 "register_operand" "r")
                    (match_operand:HF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMS))]
  ""
  "ffmshq%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fp16")]
)

(define_insn "kvx_ffmsw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMS))]
  ""
  "ffmsw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_ffmsd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:DF 2 "register_operand" "r")
                    (match_operand:DF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMS))]
  ""
  "ffmsd%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_ffms<suffix>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:S64F 1 "register_operand" "r")
                      (match_operand:S64F 2 "register_operand" "r")
                      (match_operand:S64F 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMS))]
  ""
  "ffms<suffix>%4 %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_auxr_fp16") (const_string "mau_auxr_fpu")))]
)

(define_expand "kvx_ffms<suffix>"
  [(set (match_operand:S128F 0 "register_operand" "")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "")
                       (match_operand:S128F 2 "register_operand" "")
                       (match_operand:S128F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMS))]
  ""
  ""
)

(define_insn_and_split "kvx_ffms<suffix>_1"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "r")
                       (match_operand:S128F 2 "register_operand" "r")
                       (match_operand:S128F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMS))]
  "KV3_1"
  "#"
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<CHUNK> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMS))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<CHUNK> (match_dup 3) 8)
                         (match_dup 4)] UNSPEC_FFMS))]
  ""
)

(define_insn "kvx_ffms<suffix>_2"
  [(set (match_operand:S128F 0 "register_operand" "=r")
        (unspec:S128F [(match_operand:S128F 1 "register_operand" "r")
                       (match_operand:S128F 2 "register_operand" "r")
                       (match_operand:S128F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMS))]
  "KV3_2"
  "ffms<suffix>%4 %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_auxr_fp16") (const_string "mau_auxr_fpu")))]
)

(define_insn_and_split "kvx_ffmsdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand:V2DF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMS))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DF (match_dup 1) 0)
                    (subreg:DF (match_dup 2) 0)
                    (subreg:DF (match_dup 3) 0)
                    (match_dup 4)] UNSPEC_FFMS))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DF (match_dup 1) 8)
                    (subreg:DF (match_dup 2) 8)
                    (subreg:DF (match_dup 3) 8)
                    (match_dup 4)] UNSPEC_FFMS))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_ffms<suffix>"
  [(set (match_operand:S256F 0 "register_operand" "=r")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "r")
                       (match_operand:S256F 2 "register_operand" "r")
                       (match_operand:S256F 3 "register_operand" "0")
                       (match_operand 4 "" "")] UNSPEC_FFMS))]
  ""
  "#"
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "")
                       (match_operand:S256F 2 "register_operand" "")
                       (match_operand:S256F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMS))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<CHUNK> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMS))
   (set (subreg:<CHUNK> (match_dup 0) 8)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<CHUNK> (match_dup 3) 8)
                         (match_dup 4)] UNSPEC_FFMS))
   (set (subreg:<CHUNK> (match_dup 0) 16)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 16)
                         (subreg:<CHUNK> (match_dup 2) 16)
                         (subreg:<CHUNK> (match_dup 3) 16)
                         (match_dup 4)] UNSPEC_FFMS))
   (set (subreg:<CHUNK> (match_dup 0) 24)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 24)
                         (subreg:<CHUNK> (match_dup 2) 24)
                         (subreg:<CHUNK> (match_dup 3) 24)
                         (match_dup 4)] UNSPEC_FFMS))]
  ""
)

(define_split
  [(set (match_operand:S256F 0 "register_operand" "")
        (unspec:S256F [(match_operand:S256F 1 "register_operand" "")
                       (match_operand:S256F 2 "register_operand" "")
                       (match_operand:S256F 3 "register_operand" "")
                       (match_operand 4 "" "")] UNSPEC_FFMS))]
  "KV3_2 && reload_completed"
  [(set (subreg:<HALF> (match_dup 0) 0)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 0)
                        (subreg:<HALF> (match_dup 2) 0)
                        (subreg:<HALF> (match_dup 3) 0)
                        (match_dup 4)] UNSPEC_FFMS))
   (set (subreg:<HALF> (match_dup 0) 16)
        (unspec:<HALF> [(subreg:<HALF> (match_dup 1) 16)
                        (subreg:<HALF> (match_dup 2) 16)
                        (subreg:<HALF> (match_dup 3) 16)
                        (match_dup 4)] UNSPEC_FFMS))]
  ""
)

(define_insn_and_split "kvx_ffmsdq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand:V4DF 2 "register_operand" "r")
                      (match_operand:V4DF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMS))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DF (match_dup 1) 0)
                    (subreg:DF (match_dup 2) 0)
                    (subreg:DF (match_dup 3) 0)
                    (match_dup 4)] UNSPEC_FFMS))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DF (match_dup 1) 8)
                    (subreg:DF (match_dup 2) 8)
                    (subreg:DF (match_dup 3) 8)
                    (match_dup 4)] UNSPEC_FFMS))
   (set (subreg:DF (match_dup 0) 16)
        (unspec:DF [(subreg:DF (match_dup 1) 16)
                    (subreg:DF (match_dup 2) 16)
                    (subreg:DF (match_dup 3) 16)
                    (match_dup 4)] UNSPEC_FFMS))
   (set (subreg:DF (match_dup 0) 24)
        (unspec:DF [(subreg:DF (match_dup 1) 24)
                    (subreg:DF (match_dup 2) 24)
                    (subreg:DF (match_dup 3) 24)
                    (match_dup 4)] UNSPEC_FFMS))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)


;; FFMS*C

(define_expand "kvx_ffmswc"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "")
                      (match_operand:V2SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMSC))]
  ""
  {
    if (KV3_1)
      {
        rtx product = gen_reg_rtx (V2SFmode);
        emit_insn (gen_kvx_fmulwc (product, operands[2], operands[1], operands[4]));
        emit_insn (gen_kvx_fsbfwc (operands[0], product, operands[3], operands[4]));
        DONE;
      }
  }
)

(define_insn "kvx_ffmswc_2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSC))]
  "KV3_2"
  "ffmswc%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffmswcp"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMSC))]
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
            emit_insn (gen_kvx_fsbfwc (opnd0, product, opnd3, operands[4]));
          }
        DONE;
      }
  }
)

(define_insn "kvx_ffmswcp_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSC))]
  "KV3_2"
  "ffmswcp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffmsdc"
  [(set (match_operand:V2DF 0 "register_operand")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand")
                      (match_operand:V2DF 2 "register_operand")
                      (match_operand:V2DF 3 "register_operand")
                      (match_operand 4 "" "")] UNSPEC_FFMSC))]
  ""
  {
    rtx product = gen_reg_rtx (V2DFmode);
    emit_insn (gen_kvx_fmuldc (product, operands[2], operands[1], operands[4]));
    emit_insn (gen_kvx_fsbfdp (operands[0], product, operands[3], operands[4]));
    DONE;
  }
)

(define_expand "kvx_ffmswcq"
  [(set (match_operand:V8SF 0 "register_operand" "")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V8SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMSC))]
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
            emit_insn (gen_kvx_fsbfwc (opnd0, product, opnd3, operands[4]));
          }
        DONE;
      }
  }
)

(define_insn_and_split "kvx_ffmswcq_2"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V8SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSC))]
  "KV3_2"
  "#"
  "KV3_2 && reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)
                      (subreg:V4SF (match_dup 2) 0)
                      (subreg:V4SF (match_dup 3) 0)
                      (match_dup 4)] UNSPEC_FFMSC))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)
                      (subreg:V4SF (match_dup 2) 16)
                      (subreg:V4SF (match_dup 3) 16)
                      (match_dup 4)] UNSPEC_FFMSC))]
  ""
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffmsdcp"
  [(set (match_operand:V4DF 0 "register_operand")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand")
                      (match_operand:V4DF 2 "register_operand")
                      (match_operand:V4DF 3 "register_operand")
                      (match_operand 4 "" "")] UNSPEC_FFMSC))]
  ""
  {
    rtx product = gen_reg_rtx (V4DFmode);
    emit_insn (gen_kvx_fmuldcp (product, operands[2], operands[1], operands[4]));
    emit_insn (gen_kvx_fsbfdq (operands[0], product, operands[3], operands[4]));
    DONE;
  }
)


;; FFMSX*

(define_insn "kvx_ffmsxhw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:HF 1 "register_operand" "r")
                    (match_operand:HF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMSX))]
  ""
  "ffmshw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fp16")]
)

(define_insn "kvx_ffmsxwd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")
                    (match_operand:DF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFMSX))]
  ""
  "ffmswd%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "kvx_ffmsx<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64F 1 "register_operand" "r")
                        (match_operand:S64F 2 "register_operand" "r")
                        (match_operand:<WIDE> 3 "register_operand" "0")
                        (match_operand 4 "" "")] UNSPEC_FFMSX))]
  ""
  "ffms<widenx>%4 %0 = %1, %2"
  [(set (attr "type")
     (if_then_else (match_operand 1 "float16_inner_mode") (const_string "mau_auxr_fp16") (const_string "mau_auxr_fpu")))]
)

(define_insn_and_split "kvx_ffmsx<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=&r")
        (unspec:<WIDE> [(match_operand:S128F 1 "register_operand" "r")
                        (match_operand:S128F 2 "register_operand" "r")
                        (match_operand:<WIDE> 3 "register_operand" "0")
                        (match_operand 4 "" "")] UNSPEC_FFMSX))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<HWIDE> (match_dup 0) 0)
        (unspec:<HWIDE> [(subreg:<CHUNK> (match_dup 1) 0)
                         (subreg:<CHUNK> (match_dup 2) 0)
                         (subreg:<HWIDE> (match_dup 3) 0)
                         (match_dup 4)] UNSPEC_FFMSX))
   (set (subreg:<HWIDE> (match_dup 0) 16)
        (unspec:<HWIDE> [(subreg:<CHUNK> (match_dup 1) 8)
                         (subreg:<CHUNK> (match_dup 2) 8)
                         (subreg:<HWIDE> (match_dup 3) 16)
                         (match_dup 4)] UNSPEC_FFMSX))]
  ""
)


;; FMM*

(define_insn "kvx_fmm212w"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMM))]
  ""
  "fmm212w%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_fmm222w"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMM))]
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

(define_insn "kvx_fmm222w_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMM))]
  "KV3_2"
  "fmm222w%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)


;; FMMA*

(define_insn "kvx_fmma212w"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FMMA))]
  ""
  "fmma212w%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_fmma222w"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FMMA))]
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

(define_insn "kvx_fmma222w_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FMMA))]
  "KV3_2"
  "fmma222w%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)


;; FMMS*

(define_insn "kvx_fmms212w"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FMMS))]
  ""
  "fmms212w%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_fmms222w"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FMMS))]
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

(define_insn "kvx_fmms222w_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FMMS))]
  "KV3_2"
  "fmms222w%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)


;; FFDMA*

(define_expand "kvx_ffdmaw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand 3 "" "")] UNSPEC_FFDMA))]
  ""
  {
    if (KV3_1)
      {
        emit_insn (gen_kvx_fdot2w (operands[0], operands[1], operands[2], operands[3]));
        DONE;
      }
  }
)

(define_insn "kvx_ffdmaw_2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FFDMA))]
  "KV3_2"
  "ffdmaw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_ffdmawp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FFDMA))]
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

(define_insn "kvx_ffdmawp_2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FFDMA))]
  "KV3_2"
  "ffdmawp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_ffdmawq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FFDMA))]
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

(define_insn "kvx_ffdmawq_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FFDMA))]
  "KV3_2"
  "ffdmawq%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)


;; FFDMS*

(define_expand "kvx_ffdmsw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand 3 "" "")] UNSPEC_FFDMS))]
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

(define_insn "kvx_ffdmsw_2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand 3 "" "")] UNSPEC_FFDMS))]
  "KV3_2"
  "ffdmsw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_ffdmswp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FFDMS))]
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

(define_insn "kvx_ffdmswp_2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FFDMS))]
  "KV3_2"
  "ffdmswp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_expand "kvx_ffdmswq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FFDMS))]
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

(define_insn "kvx_ffdmswq_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FFDMS))]
  "KV3_2"
  "ffdmswq%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)


;; FFDMDA*

(define_expand "kvx_ffdmdaw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand:SF 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFDMDA))]
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

(define_insn "kvx_ffdmdaw_2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFDMDA))]
  "KV3_2"
  "ffdmdaw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmdawp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMDA))]
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

(define_insn "kvx_ffdmdawp_2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMDA))]
  "KV3_2"
  "ffdmdawp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmdawq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMDA))]
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

(define_insn "kvx_ffdmdawq_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMDA))]
  "KV3_2"
  "ffdmdawq%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)


;; FFDMSA*

(define_expand "kvx_ffdmsaw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand:SF 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFDMSA))]
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

(define_insn "kvx_ffdmsaw_2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFDMSA))]
  "KV3_2"
  "ffdmsaw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmsawp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMSA))]
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

(define_insn "kvx_ffdmsawp_2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMSA))]
  "KV3_2"
  "ffdmsawp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmsawq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMSA))]
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

(define_insn "kvx_ffdmsawq_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMSA))]
  "KV3_2"
  "ffdmsawq%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)


;; FFDMDS*

(define_expand "kvx_ffdmdsw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand:SF 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFDMDS))]
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

(define_insn "kvx_ffdmdsw_2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFDMDS))]
  "KV3_2"
  "ffdmdsw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmdswp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMDS))]
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

(define_insn "kvx_ffdmdswp_2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMDS))]
  "KV3_2"
  "ffdmdswp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmdswq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMDS))]
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

(define_insn "kvx_ffdmdswq_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMDS))]
  "KV3_2"
  "ffdmdswq%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)


;; FFDMAS*

(define_expand "kvx_ffdmasw"
  [(set (match_operand:SF 0 "register_operand" "")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "")
                    (match_operand:V2SF 2 "register_operand" "")
                    (match_operand:SF 3 "register_operand" "")
                    (match_operand 4 "" "")] UNSPEC_FFDMAS))]
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

(define_insn "kvx_ffdmasw_2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V2SF 1 "register_operand" "r")
                    (match_operand:V2SF 2 "register_operand" "r")
                    (match_operand:SF 3 "register_operand" "0")
                    (match_operand 4 "" "")] UNSPEC_FFDMAS))]
  "KV3_2"
  "ffdmasw%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmaswp"
  [(set (match_operand:V2SF 0 "register_operand" "")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V2SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMAS))]
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

(define_insn "kvx_ffdmaswp_2"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V2SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMAS))]
  "KV3_2"
  "ffdmaswp%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_expand "kvx_ffdmaswq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "")
                      (match_operand:V8SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFDMAS))]
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

(define_insn "kvx_ffdmaswq_2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand:V8SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFDMAS))]
  "KV3_2"
  "ffdmaswq%4 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)


;; FLOAT*

(define_insn "kvx_floatw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SI 1 "register_operand" "r")
                    (match_operand 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FLOAT))]
  ""
  "floatw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_floatd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DI 1 "register_operand" "r")
                    (match_operand 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FLOAT))]
  ""
  "floatd%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_floatwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SI 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOAT))]
  ""
  "floatwp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SI 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOAT))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOAT))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOAT))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DI 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOAT))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DI (match_dup 1) 0)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FLOAT))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DI (match_dup 1) 8)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FLOAT))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatwo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SI 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOAT))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOAT))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOAT))
   (set (subreg:V2SF (match_dup 0) 16)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 16)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOAT))
   (set (subreg:V2SF (match_dup 0) 24)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 24)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOAT))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatdq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DI 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOAT))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DI (match_dup 1) 0)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FLOAT))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DI (match_dup 1) 8)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FLOAT))
   (set (subreg:DF (match_dup 0) 16)
        (unspec:DF [(subreg:DI (match_dup 1) 16)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FLOAT))
   (set (subreg:DF (match_dup 0) 24)
        (unspec:DF [(subreg:DI (match_dup 1) 24)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FLOAT))]
  ""
  [(set_attr "type" "mau_fpu")]
)


;; FLOATU*

(define_insn "kvx_floatuw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SI 1 "register_operand" "r")
                    (match_operand 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FLOATU))]
  ""
  "floatuw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_floatud"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DI 1 "register_operand" "r")
                    (match_operand 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FLOATU))]
  ""
  "floatud%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_floatuwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SI 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATU))]
  ""
  "floatuwp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatuwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SI 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATU))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATU))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATU))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatudp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DI 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATU))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DI (match_dup 1) 0)
                    (match_dup 2)
                    (match_operand 3 "" "")] UNSPEC_FLOATU))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DI (match_dup 1) 8)
                    (match_dup 2)
                    (match_operand 3 "" "")] UNSPEC_FLOATU))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatuwo"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SI 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATU))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SF (match_dup 0) 0)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATU))
   (set (subreg:V2SF (match_dup 0) 8)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATU))
   (set (subreg:V2SF (match_dup 0) 16)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 16)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATU))
   (set (subreg:V2SF (match_dup 0) 24)
        (unspec:V2SF [(subreg:V2SI (match_dup 1) 24)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FLOATU))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_floatudq"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DI 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATU))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DF (match_dup 0) 0)
        (unspec:DF [(subreg:DI (match_dup 1) 0)
                    (match_dup 2)
                    (match_operand 3 "" "")] UNSPEC_FLOATU))
   (set (subreg:DF (match_dup 0) 8)
        (unspec:DF [(subreg:DI (match_dup 1) 8)
                    (match_dup 2)
                    (match_operand 3 "" "")] UNSPEC_FLOATU))
   (set (subreg:DF (match_dup 0) 16)
        (unspec:DF [(subreg:DI (match_dup 1) 16)
                    (match_dup 2)
                    (match_operand 3 "" "")] UNSPEC_FLOATU))
   (set (subreg:DF (match_dup 0) 24)
        (unspec:DF [(subreg:DI (match_dup 1) 24)
                    (match_dup 2)
                    (match_operand 3 "" "")] UNSPEC_FLOATU))]
  ""
  [(set_attr "type" "mau_fpu")]
)


;; FIXED*

(define_insn "kvx_fixedw"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SF 1 "register_operand" "r")
                    (match_operand 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FIXED))]
  ""
  "fixedw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)
;; zero-extend version of kvx_fixedw
(define_insn "*kvx_fixedw_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (unspec:SI [(match_operand:SF 1 "register_operand" "r")
                                    (match_operand 2 "sixbits_unsigned_operand" "i")
                                    (match_operand 3 "" "")] UNSPEC_FIXED)))]
  ""
  "fixedw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fixedd"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI [(match_operand:DF 1 "register_operand" "r")
                    (match_operand 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FIXED))]
  ""
  "fixedd%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fixedwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXED))]
  ""
  "fixedwp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixedwq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unspec:V4SI [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXED))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXED))
   (set (subreg:V2SI (match_dup 0) 8)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXED))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixeddp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXED))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (unspec:DI [(subreg:DF (match_dup 1) 0)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXED))
   (set (subreg:DI (match_dup 0) 8)
        (unspec:DI [(subreg:DF (match_dup 1) 8)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXED))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixedwo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (unspec:V8SI [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXED))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXED))
   (set (subreg:V2SI (match_dup 0) 8)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXED))
   (set (subreg:V2SI (match_dup 0) 16)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 16)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXED))
   (set (subreg:V2SI (match_dup 0) 24)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 24)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXED))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixeddq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXED))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (unspec:DI [(subreg:DF (match_dup 1) 0)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXED))
   (set (subreg:DI (match_dup 0) 8)
        (unspec:DI [(subreg:DF (match_dup 1) 8)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXED))
   (set (subreg:DI (match_dup 0) 16)
        (unspec:DI [(subreg:DF (match_dup 1) 16)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXED))
   (set (subreg:DI (match_dup 0) 24)
        (unspec:DI [(subreg:DF (match_dup 1) 24)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXED))]
  ""
  [(set_attr "type" "mau_fpu")]
)


;; FIXEDU*

(define_insn "kvx_fixeduw"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SF 1 "register_operand" "r")
                    (match_operand 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FIXEDU))]
  ""
  "fixeduw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)
;; zero-extend version of kvx_fixeduw
(define_insn "*kvx_fixeduw_zext"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (zero_extend:DI (unspec:SI [(match_operand:SF 1 "register_operand" "r")
                                    (match_operand 2 "sixbits_unsigned_operand" "i")
                                    (match_operand 3 "" "")] UNSPEC_FIXEDU)))]
  ""
  "fixeduw%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fixedud"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI [(match_operand:DF 1 "register_operand" "r")
                    (match_operand 2 "sixbits_unsigned_operand" "i")
                    (match_operand 3 "" "")] UNSPEC_FIXEDU))]
  ""
  "fixedud%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "kvx_fixeduwp"
  [(set (match_operand:V2SI 0 "register_operand" "=r")
        (unspec:V2SI [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDU))]
  ""
  "fixeduwp%3 %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixeduwq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unspec:V4SI [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDU))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDU))
   (set (subreg:V2SI (match_dup 0) 8)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDU))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixedudp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDU))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (unspec:DI [(subreg:DF (match_dup 1) 0)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDU))
   (set (subreg:DI (match_dup 0) 8)
        (unspec:DI [(subreg:DF (match_dup 1) 8)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDU))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixeduwo"
  [(set (match_operand:V8SI 0 "register_operand" "=r")
        (unspec:V8SI [(match_operand:V8SF 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDU))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V2SI (match_dup 0) 0)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 0)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDU))
   (set (subreg:V2SI (match_dup 0) 8)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 8)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDU))
   (set (subreg:V2SI (match_dup 0) 16)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 16)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDU))
   (set (subreg:V2SI (match_dup 0) 24)
        (unspec:V2SI [(subreg:V2SF (match_dup 1) 24)
                      (match_dup 2)
                      (match_dup 3)] UNSPEC_FIXEDU))]
  ""
  [(set_attr "type" "mau_fpu")]
)

(define_insn_and_split "kvx_fixedudq"
  [(set (match_operand:V4DI 0 "register_operand" "=r")
        (unspec:V4DI [(match_operand:V4DF 1 "register_operand" "r")
                      (match_operand 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDU))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:DI (match_dup 0) 0)
        (unspec:DI [(subreg:DF (match_dup 1) 0)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDU))
   (set (subreg:DI (match_dup 0) 8)
        (unspec:DI [(subreg:DF (match_dup 1) 8)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDU))
   (set (subreg:DI (match_dup 0) 16)
        (unspec:DI [(subreg:DF (match_dup 1) 16)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDU))
   (set (subreg:DI (match_dup 0) 24)
        (unspec:DI [(subreg:DF (match_dup 1) 24)
                    (match_dup 2)
                    (match_dup 3)] UNSPEC_FIXEDU))]
  ""
  [(set_attr "type" "mau_fpu")]
)


;; FWIDEN*

(define_insn "kvx_fwidenhw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:HF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FWIDEN))]
  ""
  "fwidenlhw%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fwidenwd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FWIDEN))]
  ""
  "fwidenlwd%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fwiden<widenx>"
  [(set (match_operand:<WIDE> 0 "register_operand" "=r")
        (unspec:<WIDE> [(match_operand:S64F 1 "register_operand" "r")
                        (match_operand 2 "" "")] UNSPEC_FWIDEN))]
  ""
  "fwidenl<hwidenx>%2 %x0 = %1\n\tfwidenm<hwidenx>%2 %y0 = %1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
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


;; FNARROW*

(define_insn "kvx_fnarrowwh"
  [(set (match_operand:HF 0 "register_operand" "=r")
        (unspec:HF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FNARROW))]
  ""
  "fnarrowwh%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fnarrowdw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FNARROW))]
  ""
  "fnarrowdw%2 %0 = %1"
  [(set_attr "type" "alu_full")]
)

(define_insn "kvx_fnarrow<truncx>"
  [(set (match_operand:S64F 0 "register_operand" "=r")
        (unspec:S64F [(match_operand:<WIDE> 1 "register_operand" "r")
                      (match_operand 2 "" "")] UNSPEC_FNARROW))]
  ""
  "fnarrow<truncx>%2 %0 = %1"
  [(set_attr "type" "<fnarrowt>")]
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


;; FCONJ*

(define_insn "kvx_fconjwc"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FCONJ))]
  ""
  "fnegd %0 = %1"
  [(set_attr "type" "alu_thin")]
)

(define_insn "kvx_fconjwcp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")] UNSPEC_FCONJ))]
  ""
  "fnegd %x0 = %x1\n\tfnegd %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn "kvx_fconjdc"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")] UNSPEC_FCONJ))]
  ""
  "copyd %x0 = %x1\n\tfnegd %y0 = %y1"
  [(set_attr "type" "alu_thin_x2")
   (set_attr "length"         "8")]
)

(define_insn_and_split "kvx_fconjwcq"
  [(set (match_operand:V8SF 0 "register_operand" "=r")
        (unspec:V8SF [(match_operand:V8SF 1 "register_operand" "r")] UNSPEC_FCONJ))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V4SF (match_dup 0) 0)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 0)] UNSPEC_FCONJ))
   (set (subreg:V4SF (match_dup 0) 16)
        (unspec:V4SF [(subreg:V4SF (match_dup 1) 16)] UNSPEC_FCONJ))]
  ""
  [(set_attr "type" "alu_thin_x2")]
)

(define_insn "kvx_fconjdcp"
  [(set (match_operand:V4DF 0 "register_operand" "=r")
        (unspec:V4DF [(match_operand:V4DF 1 "register_operand" "r")] UNSPEC_FCONJ))]
  ""
  "copyd %x0 = %x1\n\tfnegd %y0 = %y1\n\tcopyd %z0 = %z1\n\tfnegd %t0 = %t1"
  [(set_attr "type" "alu_tiny_x4")
   (set_attr "length"        "16")]
)


;; FCDIV*

(define_expand "kvx_fcdivw"
  [(match_operand:SF 0 "register_operand" "")
   (match_operand:SF 1 "register_operand" "")
   (match_operand:SF 2 "register_operand" "")
   (match_operand 3 "" "")]
 ""
 {
    rtx regpair = gen_reg_rtx (V4SFmode);
    emit_insn (gen_rtx_SET (gen_rtx_SUBREG (SFmode, regpair, 0), operands[1]));
    emit_insn (gen_rtx_SET (gen_rtx_SUBREG (SFmode, regpair, 8), operands[2]));
    emit_insn (gen_kvx_fcdivw_1 (operands[0], regpair, operands[3]));
    DONE;
 }
)

(define_insn "kvx_fcdivw_1"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V4SF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FCDIV))]
  ""
  "fcdivw%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_expand "kvx_fcdivd"
  [(match_operand:DF 0 "register_operand" "")
   (match_operand:DF 1 "register_operand" "")
   (match_operand:DF 2 "register_operand" "")
   (match_operand 3 "" "")]
 ""
 {
    rtx regpair = gen_reg_rtx (V2DFmode);
    emit_insn (gen_kvx_catfdp (regpair, operands[1], operands[2]));
    emit_insn (gen_kvx_fcdivd_1 (operands[0], regpair, operands[3]));
    DONE;
 }
)

(define_insn "kvx_fcdivd_1"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:V2DF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FCDIV))]
  ""
  "fcdivd%2 %0 = %1"
  [(set_attr "type" "alu_lite")
])


;; FSDIV*

(define_expand "kvx_fsdivw"
  [(match_operand:SF 0 "register_operand" "")
   (match_operand:SF 1 "register_operand" "")
   (match_operand:SF 2 "register_operand" "")
   (match_operand 3 "" "")]
 ""
 {
    rtx regpair = gen_reg_rtx (V4SFmode);
    emit_insn (gen_rtx_SET (gen_rtx_SUBREG (SFmode, regpair, 0), operands[1]));
    emit_insn (gen_rtx_SET (gen_rtx_SUBREG (SFmode, regpair, 8), operands[2]));
    emit_insn (gen_kvx_fsdivw_1 (operands[0], regpair, operands[3]));
    DONE;
 }
)

(define_insn "kvx_fsdivw_1"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:V4SF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FSDIV))]
  ""
  "fsdivw%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_expand "kvx_fsdivd"
  [(match_operand:DF 0 "register_operand" "")
   (match_operand:DF 1 "register_operand" "")
   (match_operand:DF 2 "register_operand" "")
   (match_operand 3 "" "")]
 ""
 {
    rtx regpair = gen_reg_rtx (V2DFmode);
    emit_insn (gen_kvx_catfdp (regpair, operands[1], operands[2]));
    emit_insn (gen_kvx_fsdivd_1 (operands[0], regpair, operands[3]));
    DONE;
 }
)

(define_insn "kvx_fsdivd_1"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:V2DF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FSDIV))]
  ""
  "fsdivd%2 %0 = %1"
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
    emit_insn (gen_kvx_catfwq (regpair, operands[1], operands[2]));
    emit_insn (gen_kvx_fsdivwp_1 (operands[0], regpair, operands[3]));
    DONE;
  }
)

(define_insn "kvx_fsdivwp_1"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand 2 "" "")] UNSPEC_FSDIV))]
  ""
  "fsdivwp%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
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


;; FSREC*

(define_insn "kvx_fsrecw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FSREC))]
  ""
  "fsrecw%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fsrecd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FSREC))]
  ""
  "fsrecd%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fsrecwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand 2 "" "")] UNSPEC_FSREC))]
  ""
  "fsrecwp%2 %0 = %1"
  [(set_attr "type" "alu_lite")]
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


;; FSRSR*

(define_insn "kvx_fsrsrw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")] UNSPEC_FSRSR))]
  ""
  "fsrsrw %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fsrsrd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")] UNSPEC_FSRSR))]
  ""
  "fsrsrd %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_insn "kvx_fsrsrwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FSRSR))]
  ""
  "fsrsrwp %0 = %1"
  [(set_attr "type" "alu_lite")]
)

(define_expand "kvx_fsrsrwq"
  [(match_operand:V4SF 0 "register_operand" "")
   (match_operand:V4SF 1 "register_operand" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        emit_insn (gen_kvx_fsrsrwp (opnd0, opnd1));
      }
    DONE;
  }
)

(define_expand "kvx_fsrsrdp"
  [(match_operand:V2DF 0 "register_operand" "")
   (match_operand:V2DF 1 "register_operand" "")]
  ""
  {
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (DFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (DFmode, operands[1], i*8);
        emit_insn (gen_kvx_fsrsrd (opnd0, opnd1));
      }
    DONE;
  }
)

(define_expand "kvx_fsrsrwo"
  [(match_operand:V8SF 0 "register_operand" "")
   (match_operand:V8SF 1 "register_operand" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (V2SFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (V2SFmode, operands[1], i*8);
        emit_insn (gen_kvx_fsrsrwp (opnd0, opnd1));
      }
    DONE;
  }
)

(define_expand "kvx_fsrsrdq"
  [(match_operand:V4DF 0 "register_operand" "")
   (match_operand:V4DF 1 "register_operand" "")]
  ""
  {
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = gen_rtx_SUBREG (DFmode, operands[0], i*8);
        rtx opnd1 = gen_rtx_SUBREG (DFmode, operands[1], i*8);
        emit_insn (gen_kvx_fsrsrd (opnd0, opnd1));
      }
    DONE;
  }
)


;; READY

(define_insn "*kvx_ready"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:DI 1 "register_operand" "r")] UNSPEC_READY))]
  ""
  "ord %0 = %1, %1"
  [(set_attr "type" "alu_tiny_use")
   (set_attr "length"          "4")]
)

(define_insn "*kvx_ready"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:DI 1 "register_operand" "r")
                    (match_operand:DI 2 "register_operand" "r")] UNSPEC_READY))]
  ""
  "ord %0 = %1, %2"
  [(set_attr "type" "alu_tiny_use")
   (set_attr "length"          "4")]
)

(define_insn "*kvx_ready"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:DI 1 "register_operand" "r")
                    (match_operand:DI 2 "register_operand" "r")
                    (match_operand:DI 3 "register_operand" "r")] UNSPEC_READY))]
  ""
  "ord %0 = %1, %2\n\tord %0 = %3, %3"
  [(set_attr "type" "alu_tiny_use_x2")
   (set_attr "length"             "8")]
)

(define_insn "*kvx_ready"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:DI 1 "register_operand" "r")
                    (match_operand:DI 2 "register_operand" "r")
                    (match_operand:DI 3 "register_operand" "r")
                    (match_operand:DI 4 "register_operand" "r")] UNSPEC_READY))]
  ""
  "ord %0 = %1, %2\n\tord %0 = %3, %4"
  [(set_attr "type" "alu_tiny_use_x2")
   (set_attr "length"             "8")]
)


;; KVX_LD, KVX_LO, KVX_LDF

(define_insn "kvx_ld"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:DI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LOAD))]
  ""
  "ld%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_ldf"
 [(set (match_operand:DF 0 "register_operand" "=r,r,r")
       (unspec:DF [(match_operand:DF 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LOAD))]
  ""
  "ld%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_lq"
 [(set (match_operand:TI 0 "register_operand" "=r,r,r")
       (unspec:TI [(match_operand:TI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LOAD))]
  ""
  "lq%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)


;; KVX_L*

(define_insn "kvx_l<SIMD64:lsvs>"
  [(set (match_operand:SIMD64 0 "register_operand" "=r,r,r")
        (unspec:SIMD64 [(match_operand:SIMD64 1 "memory_operand" "a,b,m")
                        (match_operand 2 "" "")] UNSPEC_LOAD))]
  ""
  "ld%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_l<SIMD128:lsvs>"
  [(set (match_operand:SIMD128 0 "register_operand" "=r,r,r")
        (unspec:SIMD128 [(match_operand:SIMD128 1 "memory_operand" "a,b,m")
                         (match_operand 2 "" "")] UNSPEC_LOAD))]
  ""
  "lq%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_l<SIMD256:lsvs>"
  [(set (match_operand:SIMD256 0 "register_operand" "=r,r,r")
        (unspec:SIMD256 [(match_operand:SIMD256 1 "memory_operand" "a,b,m")
                         (match_operand 2 "" "")] UNSPEC_LOAD))]
  ""
  "lo%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)


;; KVX_LBX, KVX_LHX, KVX_LWX

(define_insn "kvx_lbz"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:QI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LOADZ))
   (use (match_dup 1))]
  ""
  "lbz%2%X1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lbs"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:QI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LOADS))
   (use (match_dup 1))]
  ""
  "lbs%2%X1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lhz"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:HI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LOADZ))
   (use (match_dup 1))]
  ""
  "lhz%2%X1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lhs"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:HI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LOADS))
   (use (match_dup 1))]
  ""
  "lhs%2%X1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lwz"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:SI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LOADZ))
   (use (match_dup 1))]
  ""
  "lwz%2%X1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lws"
 [(set (match_operand:DI 0 "register_operand" "=r,r,r")
       (unspec:DI [(match_operand:SI 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LOADS))
   (use (match_dup 1))]
  ""
  "lws%2%X1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lhf"
 [(set (match_operand:HF 0 "register_operand" "=r,r,r")
       (unspec:HF [(match_operand:HF 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LOADZ))
   (use (match_dup 1))]
  ""
  "lhz%2%X1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)

(define_insn "kvx_lwf"
 [(set (match_operand:SF 0 "register_operand" "=r,r,r")
       (unspec:SF [(match_operand:SF 1 "memory_operand" "a,b,m")
                   (match_operand 2 "" "")] UNSPEC_LOADZ))
   (use (match_dup 1))]
  ""
  "lwz%2%X1 %0 = %1"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length"                    "4,                       8,                      12")]
)


;; KVX_LOAD*

(define_insn "kvx_loadbz"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (unspec:QI [(match_operand:QI 1 "memory_operand" "a,b,m")
                                    (match_operand 2 "" "")] UNSPEC_LOADZ)))]
  ""
  "lbz%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadhz"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (unspec:HI [(match_operand:HI 1 "memory_operand" "a,b,m")
                                    (match_operand 2 "" "")] UNSPEC_LOADZ)))]
  ""
  "lhz%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadwz"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (unspec:SI [(match_operand:SI 1 "memory_operand" "a,b,m")
                                    (match_operand 2 "" "")] UNSPEC_LOADZ)))]
  ""
  "lwz%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadd"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (unspec:DI [(match_operand:DI 1 "memory_operand" "a,b,m")
                    (match_operand 2 "" "")] UNSPEC_LOAD))]
  ""
  "ld%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadq"
  [(set (match_operand:TI 0 "register_operand" "=r,r,r")
        (unspec:TI [(match_operand:TI 1 "memory_operand" "a,b,m")
                    (match_operand 2 "" "")] UNSPEC_LOAD))]
  ""
  "lq%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadhf"
  [(set (match_operand:HF 0 "register_operand" "=r,r,r")
        (unspec:HF [(match_operand:HF 1 "memory_operand" "a,b,m")
                    (match_operand 2 "" "")] UNSPEC_LOADZ))]
  ""
  "lhz%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadwf"
  [(set (match_operand:SF 0 "register_operand" "=r,r,r")
        (unspec:SF [(match_operand:SF 1 "memory_operand" "a,b,m")
                    (match_operand 2 "" "")] UNSPEC_LOADZ))]
  ""
  "lwz%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loaddf"
  [(set (match_operand:DF 0 "register_operand" "=r,r,r")
        (unspec:DF [(match_operand:DF 1 "memory_operand" "a,b,m")
                    (match_operand 2 "" "")] UNSPEC_LOAD))]
  ""
  "ld%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_load64"
  [(set (match_operand:V64 0 "register_operand" "=r,r,r")
        (unspec:V64 [(match_operand:V64 1 "memory_operand" "a,b,m")
                     (match_operand 2 "" "")] UNSPEC_LOAD))]
  ""
  "ld%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_load128"
  [(set (match_operand:V128 0 "register_operand" "=r,r,r")
        (unspec:V128 [(match_operand:V128 1 "memory_operand" "a,b,m")
                      (match_operand 2 "" "")] UNSPEC_LOAD))]
  ""
  "lq%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_load256"
  [(set (match_operand:V256 0 "register_operand" "=r,r,r")
        (unspec:V256 [(match_operand:V256 1 "memory_operand" "a,b,m")
                      (match_operand 2 "" "")] UNSPEC_LOAD))]
  ""
  "lo%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn_and_split "kvx_load512"
  [(set (match_operand:V512 0 "register_operand" "=&r,&r,&r")
        (unspec:V512 [(match_operand:V512 1 "memory_operand" "a,b,m")
                      (match_operand 2 "" "")] UNSPEC_LOAD))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:V256 (match_dup 0) 0)
        (unspec:V256 [(subreg:V256 (match_dup 1) 0)
                      (match_dup 2)] UNSPEC_LOAD))
   (set (subreg:V256 (match_dup 0) 32)
        (unspec:V256 [(subreg:V256 (match_dup 1) 32)
                      (match_dup 2)] UNSPEC_LOAD))]
  ""
)


;; KVX_LOADC*

(define_insn "kvx_loadcbz"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (unspec:QI [(match_operand:DI 1 "register_operand" "0,0,0")
                                    (match_operand:QI 2 "memsimple_operand" "c,d,e")
                                    (match_operand:DI 3 "register_operand" "r,r,r")
                                    (match_operand 4 "" "")] UNSPEC_LOADC)))]
  ""
  "lbz%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadchz"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (unspec:HI [(match_operand:DI 1 "register_operand" "0,0,0")
                                    (match_operand:HI 2 "memsimple_operand" "c,d,e")
                                    (match_operand:DI 3 "register_operand" "r,r,r")
                                    (match_operand 4 "" "")] UNSPEC_LOADC)))]
  ""
  "lhz%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadcwz"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (unspec:SI [(match_operand:DI 1 "register_operand" "0,0,0")
                                    (match_operand:SI 2 "memsimple_operand" "c,d,e")
                                    (match_operand:DI 3 "register_operand" "r,r,r")
                                    (match_operand 4 "" "")] UNSPEC_LOADC)))]
  ""
  "lwz%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadcd"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (unspec:DI [(match_operand:DI 1 "register_operand" "0,0,0")
                    (match_operand:DI 2 "memsimple_operand" "c,d,e")
                    (match_operand:DI 3 "register_operand" "r,r,r")
                    (match_operand 4 "" "")] UNSPEC_LOADC))]
  ""
  "ld%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadcq"
  [(set (match_operand:TI 0 "register_operand" "=r,r,r")
        (unspec:TI [(match_operand:TI 1 "register_operand" "0,0,0")
                    (match_operand:TI 2 "memsimple_operand" "c,d,e")
                    (match_operand:DI 3 "register_operand" "r,r,r")
                    (match_operand 4 "" "")] UNSPEC_LOADC))]
  ""
  "lq%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadchf"
  [(set (match_operand:HF 0 "register_operand" "=r,r,r")
        (unspec:HF [(match_operand:HF 1 "register_operand" "0,0,0")
                    (match_operand:HF 2 "memsimple_operand" "c,d,e")
                    (match_operand:DI 3 "register_operand" "r,r,r")
                    (match_operand 4 "" "")] UNSPEC_LOADC))]
  ""
  "lhz%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadcwf"
  [(set (match_operand:SF 0 "register_operand" "=r,r,r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "0,0,0")
                    (match_operand:SF 2 "memsimple_operand" "c,d,e")
                    (match_operand:DI 3 "register_operand" "r,r,r")
                    (match_operand 4 "" "")] UNSPEC_LOADC))]
  ""
  "lwz%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadcdf"
  [(set (match_operand:DF 0 "register_operand" "=r,r,r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "0,0,0")
                    (match_operand:DF 2 "memsimple_operand" "c,d,e")
                    (match_operand:DI 3 "register_operand" "r,r,r")
                    (match_operand 4 "" "")] UNSPEC_LOADC))]
  ""
  "ld%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_expand "kvx_loadc64"
  [(match_operand:V64 0 "register_operand" "")
   (match_operand:V64 1 "reg_zero_mone_operand" "")
   (match_operand:V64 2 "memsimple_operand" "")
   (match_operand:DI 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    if (masked_modifier (operands[4], VOIDmode))
      {
        rtx loaded = gen_reg_rtx (<TMODE>mode);
        rtx address = XEXP (operands[2], 0);
        rtx memory = gen_rtx_MEM (<TMODE>mode, address);
        MEM_COPY_ATTRIBUTES (memory, operands[2]);
        if (!const_zero_operand (operands[1], <MODE>mode))
          {
            emit_insn (gen_rtx_SET (gen_rtx_SUBREG (<MODE>mode, loaded, 0), operands[1]));
            emit_insn (gen_kvx_loadc_ (loaded, loaded, memory, operands[3], operands[4]));
          }
        else
          emit_insn (gen_kvx_loadc__ (loaded, memory, operands[3], operands[4]));
        emit_insn (gen_rtx_SET (operands[0], gen_rtx_SUBREG (<MODE>mode, loaded, 0)));
      }
    else
      emit_insn (gen_kvx_loadc64_ (operands[0], operands[1], operands[2], operands[3], operands[4]));
    DONE;
  }
)

(define_insn "kvx_loadc64_"
  [(set (match_operand:V64 0 "register_operand" "=r,r,r")
        (unspec:V64 [(match_operand:V64 1 "register_operand" "0,0,0")
                     (match_operand:V64 2 "memsimple_operand" "c,d,e")
                     (match_operand:DI 3 "register_operand" "r,r,r")
                     (match_operand 4 "" "")] UNSPEC_LOADC))]
  ""
  "ld%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_expand "kvx_loadc128"
  [(match_operand:V128 0 "register_operand" "")
   (match_operand:V128 1 "reg_zero_mone_operand" "")
   (match_operand:V128 2 "memsimple_operand" "")
   (match_operand:DI 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    if (masked_modifier (operands[4], VOIDmode))
      {
        rtx loaded = gen_reg_rtx (<DMODE>mode);
        rtx address = XEXP (operands[2], 0);
        rtx memory = gen_rtx_MEM (<DMODE>mode, address);
        MEM_COPY_ATTRIBUTES (memory, operands[2]);
        if (!const_zero_operand (operands[1], <MODE>mode))
          {
            emit_insn (gen_rtx_SET (gen_rtx_SUBREG (<MODE>mode, loaded, 0), operands[1]));
            emit_insn (gen_kvx_loadc_ (loaded, loaded, memory, operands[3], operands[4]));
          }
        else
          emit_insn (gen_kvx_loadc__ (loaded, memory, operands[3], operands[4]));
        emit_insn (gen_rtx_SET (operands[0], gen_rtx_SUBREG (<MODE>mode, loaded, 0)));
      }
    else
      emit_insn (gen_kvx_loadc128_ (operands[0], operands[1], operands[2], operands[3], operands[4]));
    DONE;
  }
)

(define_insn "kvx_loadc128_"
  [(set (match_operand:V128 0 "register_operand" "=r,r,r")
        (unspec:V128 [(match_operand:V128 1 "register_operand" "0,0,0")
                      (match_operand:V128 2 "memsimple_operand" "c,d,e")
                      (match_operand:DI 3 "register_operand" "r,r,r")
                      (match_operand 4 "" "")] UNSPEC_LOADC))]
  ""
  "lq%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_expand "kvx_loadc256"
  [(match_operand:V256 0 "register_operand" "")
   (match_operand:V256 1 "reg_zero_mone_operand" "")
   (match_operand:V256 2 "memsimple_operand" "")
   (match_operand:DI 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    if (!const_zero_operand (operands[1], <MODE>mode))
      emit_insn (gen_kvx_loadc_ (operands[0], operands[1], operands[2], operands[3], operands[4]));
    else
      emit_insn (gen_kvx_loadc__ (operands[0], operands[2], operands[3], operands[4]));
    DONE;
  }
)

(define_insn "kvx_loadc_"
  [(set (match_operand:V256 0 "register_operand" "=r,r,r")
        (unspec:V256 [(match_operand:V256 1 "reg_zero_mone_operand" "0,0,0")
                      (match_operand:V256 2 "memsimple_operand" "c,d,e")
                      (match_operand:DI 3 "register_operand" "r,r,r")
                      (match_operand 4 "" "")] UNSPEC_LOADC))]
  ""
  "lo%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_loadc__"
  [(set (match_operand:V256 0 "register_operand" "=r,r,r")
        (unspec:V256 [(match_operand:V256 1 "memsimple_operand" "c,d,e")
                      (match_operand:DI 2 "register_operand" "r,r,r")
                      (match_operand 3 "" "")] UNSPEC_LOADC))]
  ""
  "lo%3%X1 %2? %0 = %O1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_auxw_load_uncached") (const_string "lsu_auxw_load"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_auxw_load_uncached_x") (const_string "lsu_auxw_load_x"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_auxw_load_uncached_y") (const_string "lsu_auxw_load_y"))])
   (set_attr "length" "4, 8, 12")]
)


;; KVX_STORE*

(define_insn "kvx_storeb"
  [(set (match_operand:QI 1 "memory_operand"  "=a,b,m")
        (unspec:QI [(match_operand:DI 0 "register_operand" "r,r,r")] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "sb%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)
(define_insn "*kvx_storeb"
  [(set (match_operand:QI 1 "memory_operand"  "=a,b,m")
        (unspec:QI [(ANY_EXTEND:DI (match_operand:QI 0 "register_operand" "r,r,r"))] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "sb%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storeh"
  [(set (match_operand:HI 1 "memory_operand"  "=a,b,m")
        (unspec:HI [(match_operand:DI 0 "register_operand" "r,r,r")] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "sh%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)
(define_insn "*kvx_storeh"
  [(set (match_operand:HI 1 "memory_operand"  "=a,b,m")
        (unspec:HI [(ANY_EXTEND:DI (match_operand:HI 0 "register_operand" "r,r,r"))] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "sh%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storew"
  [(set (match_operand:SI 1 "memory_operand"  "=a,b,m")
        (unspec:SI [(match_operand:DI 0 "register_operand" "r,r,r")] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "sw%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)
(define_insn "*kvx_storew"
  [(set (match_operand:SI 1 "memory_operand"  "=a,b,m")
        (unspec:SI [(ANY_EXTEND:DI (match_operand:SI 0 "register_operand" "r,r,r"))] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "sw%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_stored"
  [(set (match_operand:DI 1 "memory_operand"  "=a,b,m")
        (unspec:DI [(match_operand:DI 0 "register_operand" "r,r,r")] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "sd%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storeq"
  [(set (match_operand:TI 1 "memory_operand"  "=a,b,m")
        (unspec:TI [(match_operand:TI 0 "register_operand" "r,r,r")] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "sq%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storehf"
  [(set (match_operand:HF 1 "memory_operand"  "=a,b,m")
        (unspec:HF [(match_operand:HF 0 "register_operand" "r,r,r")] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "sh%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storewf"
  [(set (match_operand:SF 1 "memory_operand"  "=a,b,m")
        (unspec:SF [(match_operand:SF 0 "register_operand" "r,r,r")] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "sw%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storedf"
  [(set (match_operand:DF 1 "memory_operand"  "=a,b,m")
        (unspec:DF [(match_operand:DF 0 "register_operand" "r,r,r")] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "sd%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_store64"
  [(set (match_operand:V64 1 "memory_operand"  "=a,b,m")
        (unspec:V64 [(match_operand:V64 0 "register_operand" "r,r,r")] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "sd%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_store128"
  [(set (match_operand:V128 1 "memory_operand"  "=a,b,m")
        (unspec:V128 [(match_operand:V128 0 "register_operand" "r,r,r")] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "sq%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_store256"
  [(set (match_operand:V256 1 "memory_operand"  "=a,b,m")
        (unspec:V256 [(match_operand:V256 0 "register_operand" "r,r,r")] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "so%X1 %1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn_and_split "kvx_store512"
  [(set (match_operand:V512 1 "memory_operand"  "=a,b,m")
        (unspec:V512 [(match_operand:V512 0 "register_operand" "r,r,r")] UNSPEC_STORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "#"
  "reload_completed"
  [(parallel
    [(set (subreg:V256 (match_dup 1) 0)
          (unspec:V256 [(subreg:V256 (match_dup 0) 0)] UNSPEC_STORE))
     (use (match_operand:SI 2 "nonmemory_operand" ""))])
   (parallel
    [(set (subreg:V256 (match_dup 1) 32)
          (unspec:V256 [(subreg:V256 (match_dup 0) 32)] UNSPEC_STORE))
     (use (match_operand:SI 2 "nonmemory_operand" ""))])]
  ""
)


;; KVX_STOREC*

(define_insn "kvx_storecb"
  [(set (match_operand:QI 1 "memsimple_operand"  "=c,d,e")
        (unspec:QI [(match_operand:DI 0 "register_operand" "r,r,r")
                    (match_operand:DI 2 "register_operand" "r,r,r")
                    (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "sb%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)
(define_insn "*kvx_storecb"
  [(set (match_operand:QI 1 "memsimple_operand"  "=c,d,e")
        (unspec:QI [(ANY_EXTEND:DI (match_operand:QI 0 "register_operand" "r,r,r"))
                    (match_operand:DI 2 "register_operand" "r,r,r")
                    (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "sb%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storech"
  [(set (match_operand:HI 1 "memsimple_operand"  "=c,d,e")
        (unspec:HI [(match_operand:DI 0 "register_operand" "r,r,r")
                    (match_operand:DI 2 "register_operand" "r,r,r")
                    (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "sh%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)
(define_insn "*kvx_storech"
  [(set (match_operand:HI 1 "memsimple_operand"  "=c,d,e")
        (unspec:HI [(ANY_EXTEND:DI (match_operand:HI 0 "register_operand" "r,r,r"))
                    (match_operand:DI 2 "register_operand" "r,r,r")
                    (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "sh%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storecw"
  [(set (match_operand:SI 1 "memsimple_operand"  "=c,d,e")
        (unspec:SI [(match_operand:DI 0 "register_operand" "r,r,r")
                    (match_operand:DI 2 "register_operand" "r,r,r")
                    (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "sw%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)
(define_insn "*kvx_storecw"
  [(set (match_operand:SI 1 "memsimple_operand"  "=c,d,e")
        (unspec:SI [(ANY_EXTEND:DI (match_operand:SI 0 "register_operand" "r,r,r"))
                    (match_operand:DI 2 "register_operand" "r,r,r")
                    (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "sw%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storecd"
  [(set (match_operand:DI 1 "memsimple_operand"  "=c,d,e")
        (unspec:DI [(match_operand:DI 0 "register_operand" "r,r,r")
                    (match_operand:DI 2 "register_operand" "r,r,r")
                    (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "sd%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storecq"
  [(set (match_operand:TI 1 "memsimple_operand"  "=c,d,e")
        (unspec:TI [(match_operand:TI 0 "register_operand" "r,r,r")
                    (match_operand:DI 2 "register_operand" "r,r,r")
                    (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "sq%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storechf"
  [(set (match_operand:HF 1 "memsimple_operand"  "=c,d,e")
        (unspec:HF [(match_operand:HF 0 "register_operand" "r,r,r")
                    (match_operand:DI 2 "register_operand" "r,r,r")
                    (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "sh%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storecwf"
  [(set (match_operand:SF 1 "memsimple_operand"  "=c,d,e")
        (unspec:SF [(match_operand:SF 0 "register_operand" "r,r,r")
                    (match_operand:DI 2 "register_operand" "r,r,r")
                    (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "sw%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storecdf"
  [(set (match_operand:DF 1 "memsimple_operand"  "=c,d,e")
        (unspec:DF [(match_operand:DF 0 "register_operand" "r,r,r")
                    (match_operand:DI 2 "register_operand" "r,r,r")
                    (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "sd%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_expand "kvx_storec64"
  [(match_operand:V64 1 "memsimple_operand"  "")
   (match_operand:V64 0 "register_operand" "")
   (match_operand:DI 2 "register_operand" "")
   (match_operand 3 "" "")
   (match_operand:SI 4 "nonmemory_operand" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (*xstr && xstr[1] == 'm')
      {
        rtx stored = gen_reg_rtx (<TMODE>mode);
        rtx address = XEXP (operands[1], 0);
        rtx memory = gen_rtx_MEM (<TMODE>mode, address);
        MEM_COPY_ATTRIBUTES (memory, operands[1]);
        rtx mask = gen_reg_rtx (DImode);
        if (xstr[2] == 't')
          emit_insn (gen_andsi3 (gen_rtx_SUBREG (SImode, mask, 0),
                                 gen_rtx_SUBREG (SImode, operands[2], 0), GEN_INT (0xFF)));
        else if (xstr[2] == 'f')
          emit_insn (gen_iorsi3 (gen_rtx_SUBREG (SImode, mask, 0),
                                 gen_rtx_SUBREG (SImode, operands[2], 0), GEN_INT (0xFFFFFF00)));
        else
          gcc_unreachable ();
        emit_insn (gen_rtx_SET (gen_rtx_SUBREG (<MODE>mode, stored, 0), operands[0]));
        emit_insn (gen_kvx_storec256 (stored, memory, mask, operands[3], operands[4]));
      }
    else
      emit_insn (gen_kvx_storec64_ (operands[0], operands[1], operands[2], operands[3], operands[4]));
    DONE;
  }
)

(define_insn "kvx_storec64_"
  [(set (match_operand:V64 1 "memsimple_operand"  "=c,d,e")
        (unspec:V64 [(match_operand:V64 0 "register_operand" "r,r,r")
                     (match_operand:DI 2 "register_operand" "r,r,r")
                     (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "sd%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_expand "kvx_storec128"
  [(match_operand:V128 1 "memsimple_operand"  "")
   (match_operand:V128 0 "register_operand" "")
   (match_operand:DI 2 "register_operand" "")
   (match_operand 3 "" "")
   (match_operand:SI 4 "nonmemory_operand" "")]
  ""
  {
    const char *xstr = XSTR (operands[3], 0);
    if (*xstr && xstr[1] == 'm')
      {
        rtx stored = gen_reg_rtx (<DMODE>mode);
        rtx address = XEXP (operands[1], 0);
        rtx memory = gen_rtx_MEM (<DMODE>mode, address);
        MEM_COPY_ATTRIBUTES (memory, operands[1]);
        rtx mask = gen_reg_rtx (DImode);
        if (xstr[2] == 't')
          emit_insn (gen_andsi3 (gen_rtx_SUBREG (SImode, mask, 0),
                                 gen_rtx_SUBREG (SImode, operands[2], 0), GEN_INT (0xFFFF)));
        else if (xstr[2] == 'f')
          emit_insn (gen_iorsi3 (gen_rtx_SUBREG (SImode, mask, 0),
                                 gen_rtx_SUBREG (SImode, operands[2], 0), GEN_INT (0xFFFF0000)));
        else
          gcc_unreachable ();
        emit_insn (gen_rtx_SET (gen_rtx_SUBREG (<MODE>mode, stored, 0), operands[0]));
        emit_insn (gen_kvx_storec256 (stored, memory, mask, operands[3], operands[4]));
      }
    else
      emit_insn (gen_kvx_storec128_ (operands[0], operands[1], operands[2], operands[3], operands[4]));
    DONE;
  }
)

(define_insn "kvx_storec128_"
  [(set (match_operand:V128 1 "memsimple_operand"  "=c,d,e")
        (unspec:V128 [(match_operand:V128 0 "register_operand" "r,r,r")
                      (match_operand:DI 2 "register_operand" "r,r,r")
                      (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "sq%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_storec256"
  [(set (match_operand:V256 1 "memsimple_operand"  "=c,d,e")
        (unspec:V256 [(match_operand:V256 0 "register_operand" "r,r,r")
                      (match_operand:DI 2 "register_operand" "r,r,r")
                      (match_operand 3 "" "")] UNSPEC_STOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "so%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"            "4,               8,              12")]
)

