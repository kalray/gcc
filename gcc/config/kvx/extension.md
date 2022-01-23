;; Abstract the extension modes

(define_mode_iterator EXT256 [
  V1OI
])

(define_mode_iterator EXT512 [
  V2OI
])

(define_mode_iterator EXT1024 [
  V4OI
])

(define_mode_iterator EXTBUFF [
  V2OI V4OI V8OI V16OI V32OI V64OI
])

;; 256-bit Extension Moves

(define_expand "mov<mode>"
  [(set (match_operand:EXT256 0 "nonimmediate_operand" "")
        (match_operand:EXT256 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
    if (CONSTANT_P (operands[1]))
      {
        rtx temp = gen_reg_rtx (OImode);
        emit_insn (gen_rtx_SET (temp, CONST_VECTOR_ELT (operands[1], 0)));
        rtx subreg = simplify_gen_subreg (<MODE>mode, temp, OImode, 0);
        emit_insn (gen_rtx_SET (operands[0], subreg));
        DONE;
      }
  }
)

(define_insn "*mov<mode>"
  [(set (match_operand:EXT256 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:EXT256 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  "KV3_1"
  {
    switch (which_alternative)
      {
      case 0:
        return "copyv %0 = %1";
      case 1: case 2: case 3:
        return "xlo.u%X1 %0 = %1";
      case 4: case 5: case 6:
        return "xso%X0 %0 = %1";
      case 7:
        return "xmovefo %0 = %1";
      case 8:
        return "xmovetq %0.lo = %x1, %y1\n\txmovetq %0.hi = %z1, %t1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "bcu_crrp_crwl_crwh,lsu_load_uncached,lsu_load_uncached_x,lsu_load_uncached_y,lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y,bcu_tiny_auxw_crrp,alu_lite_x2_crwl_crwh")
   (set_attr "length"                "4,                4,                  8,                 12,             4,               8,              12,                 4,                    8")]
)

(define_insn "*mov<mode>"
  [(set (match_operand:EXT256 0 "nonimmediate_operand" "=x, x, x, x, x, x, x,a,b,m,r,x")
        (match_operand:EXT256 1 "nonimmediate_operand"  "x,Ca,Cb,Cm,Za,Zb,Zm,x,x,x,x,r"))]
  "KV3_2"
  {
    switch (which_alternative)
      {
      case 0:
        return "xcopyo %0 = %1";
      case 1: case 2: case 3: case 4: case 5: case 6:
        return "xlo%V1 %0 = %1";
      case 7: case 8: case 9:
        return "xso%X0 %0 = %1";
      case 10:
        return "xmovefo %0 = %1";
      case 11:
        return "xmovetq %0.lo = %x1, %y1\n\txmovetq %0.hi = %z1, %t1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "bcu_crrp_crwl_crwh,lsu_load,lsu_load_x,lsu_load_y,lsu_load_uncached,lsu_load_uncached_x,lsu_load_uncached_y,lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y,bcu_tiny_auxw_crrp,alu_thin_x2_crwl_crwh")
   (set_attr "length"                "4,       4,         8,        12,                4,                  8,                 12,             4,               8,              12,                 4,                    8")]
)

(define_insn "*xmovef<mode>"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (match_operand:ALL256 1 "register_operand" "x"))]
  ""
  "xmovefo %0 = %1"
  [(set_attr "type" "bcu_tiny_auxw_crrp")
   (set_attr "length" "4")]
)

(define_insn "*xmovet<mode>"
  [(set (match_operand:ALL256 0 "register_operand" "=x")
        (match_operand:ALL256 1 "register_operand" "r"))]
  ""
  "xmovetq %0.lo = %x1, %y1\n\txmovetq %0.hi = %z1, %t1"
  [(set_attr "type" "alu_thin_x2_crwl_crwh")
   (set_attr "length" "8")]
)

;; 512-bit Extension Moves

(define_expand "mov<mode>"
  [(set (match_operand:EXT512 0 "nonimmediate_operand" "")
        (match_operand:EXT512 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn_and_split "*mov<EXT512:mode>"
  [(set (match_operand:EXT512 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:EXT512 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:EXT256 (match_dup 0) 0) (subreg:EXT256 (match_dup 1) 0))
   (set (subreg:EXT256 (match_dup 0) 32) (subreg:EXT256 (match_dup 1) 32))]
  ""
)


;; 1024-bit Extension Moves

(define_expand "mov<mode>"
  [(set (match_operand:EXT1024 0 "nonimmediate_operand" "")
        (match_operand:EXT1024 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn_and_split "*mov<EXT1024:mode>"
  [(set (match_operand:EXT1024 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:EXT1024 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:EXT256 (match_dup 0) 0) (subreg:EXT256 (match_dup 1) 0))
   (set (subreg:EXT256 (match_dup 0) 32) (subreg:EXT256 (match_dup 1) 32))
   (set (subreg:EXT256 (match_dup 0) 64) (subreg:EXT256 (match_dup 1) 64))
   (set (subreg:EXT256 (match_dup 0) 96) (subreg:EXT256 (match_dup 1) 96))]
  ""
)


;; KVX_XLOAD256, KVX_XLOAD256H, KVX_XLOAD256Q

(define_insn "kvx_xload256"
  [(set (match_operand:EXT256 0 "register_operand" "=x,x,x")
        (unspec:EXT256 [(match_operand:EXT256 1 "memory_operand" "a,b,m")
                        (match_operand 2 "" "")] UNSPEC_XLOAD256))
   (use (match_dup 1))]
  ""
  "xlo%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_xload512h"
  [(set (match_operand:EXT512 0 "register_operand" "=x,x,x")
        (unspec:EXT512 [(match_operand:EXT256 1 "memory_operand" "a,b,m")
                         (match_operand:EXT512 2 "register_operand" "0,0,0")
                         (match_operand 3 "" "")
                         (match_operand 4 "" "")] UNSPEC_XLOAD256))
   (use (match_dup 1))]
  ""
  "xlo.h%4%3%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_xload1024q"
  [(set (match_operand:EXT1024 0 "register_operand" "=x,x,x")
        (unspec:EXT1024 [(match_operand:EXT256 1 "memory_operand" "a,b,m")
                         (match_operand:EXT1024 2 "register_operand" "0,0,0")
                         (match_operand 3 "" "")
                         (match_operand 4 "" "")] UNSPEC_XLOAD256))
   (use (match_dup 1))]
  ""
  "xlo.q%4%3%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)


;; KVX_XLOADC256, KVX_XLOADC256H, KVX_XLOADC256Q

(define_insn "kvx_xloadc256"
  [(set (match_operand:EXT256 0 "register_operand" "=x,x,x")
        (unspec:EXT256 [(match_operand:EXT256 1 "memfoiled_operand" "c,d,e")
                        (match_operand:EXT256 2 "register_operand" "0,0,0")
                        (match_operand:DI 3 "register_operand" "r,r,r")
                        (match_operand 4 "" "")] UNSPEC_XLOAD256))
   (use (match_dup 1))]
  ""
  "xlo%4%X1 %3? %0 = %O1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_xloadc512h"
  [(set (match_operand:EXT512 0 "register_operand" "=x,x,x")
        (unspec:EXT512 [(match_operand:EXT256 1 "memfoiled_operand" "c,d,e")
                         (match_operand:EXT512 2 "register_operand" "0,0,0")
                         (match_operand:DI 3 "register_operand" "r,r,r")
                         (match_operand 4 "" "")
                         (match_operand 5 "" "")] UNSPEC_XLOAD256))
   (use (match_dup 1))]
  ""
  "xlo.h%5%4%X1 %3? %0 = %O1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_xloadc1024q"
  [(set (match_operand:EXT1024 0 "register_operand" "=x,x,x")
        (unspec:EXT1024 [(match_operand:EXT256 1 "memfoiled_operand" "c,d,e")
                         (match_operand:EXT1024 2 "register_operand" "0,0,0")
                         (match_operand:DI 3 "register_operand" "r,r,r")
                         (match_operand 4 "" "")
                         (match_operand 5 "" "")] UNSPEC_XLOAD256))
   (use (match_dup 1))]
  ""
  "xlo.q%5%4%X1 %3? %0 = %O1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)


;; KVX_XSTORE256, KVX_XSTOREC256

(define_insn "kvx_xstore256"
  [(set (match_operand:EXT256 1 "memory_operand"  "=a,b,m")
        (unspec:EXT256 [(match_operand:EXT256 0 "register_operand" "x,x,x")] UNSPEC_XSTORE256))
   (clobber (match_dup 1))]
  ""
  "xso%X1 %1 = %0"
  [(set_attr "type" "lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_xstorec256"
  [(set (match_operand:EXT256 1 "memfoiled_operand"  "=c,d,e")
        (unspec:EXT256 [(match_operand:EXT256 0 "register_operand" "x,x,x")
                        (match_operand:DI 2 "register_operand" "r,r,r")
                        (match_operand 3 "" "")] UNSPEC_XSTORE256))
   (clobber (match_dup 1))]
  ""
  "xso%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y")
   (set_attr "length"            "4,               8,              12")]
)


;; KVX_XPRELOAD256

(define_insn "kvx_xpreload256<EXTBUFF:mode>"
  [(set (match_operand:EXTBUFF 1 "register_operand" "=x,x,x")
        (unspec:EXTBUFF [(match_operand:EXT256 0 "memfoiled_operand" "c,d,e")
                         (match_operand:DI 2 "register_operand" "r,r,r")
                         (match_operand:EXTBUFF 3 "register_operand" "1,1,1")
                         (match_operand 4 "" "")
                         (match_operand 5 "" "")] UNSPEC_XPRELOAD))
   (use (match_dup 0))]
  ""
  "xlo%5%4%X0 %b1, %2 = %O0"
  [(set_attr "type" "lsu,lsu_x,lsu_y")
   (set_attr "length" "4,   8,   12")]
)


;; KVX_XALIGNV256, KVX_XALIGNX256

(define_insn "kvx_xalignv256<EXTBUFF:mode>"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (unspec:V32QI [(match_operand:EXTBUFF 1 "register_operand" "x")
                       (match_operand:DI 2 "register_operand" "r")] UNSPEC_XALIGN256))]
  ""
  "xaligno %0 = %b1, %2"
  [(set_attr "type" "bcu_tiny_auxw_crrp")]
)

(define_insn "kvx_xalignx256<EXTBUFF:mode>"
  [(set (match_operand:EXT256 0 "register_operand" "=x")
        (unspec:EXT256 [(match_operand:EXTBUFF 1 "register_operand" "x")
                        (match_operand:DI 2 "register_operand" "r")] UNSPEC_XALIGN256))]
  ""
  "xaligno %0 = %b1, %2"
  [(set_attr "type" "bcu_crrp_crwl_crwh")]
)


;; KVX_XSWAP256

(define_expand "kvx_xswap256"
  [(match_operand:V32QI 0 "register_operand" "")
   (match_operand:EXT256 1 "memory_operand" "")
   (match_operand:V32QI 2 "register_operand" "")]
  ""
  {
    rtx swapped = gen_reg_rtx (<EXT256:MODE>mode);
    swapped = force_reg (<EXT256:MODE>mode, operands[1]);
    emit_insn (gen_kvx_xswap256v32qi_ (operands[0], swapped, operands[2]));
    emit_move_insn (operands[1], swapped);
    DONE;
  }
)

(define_insn "kvx_xswap256<ALL256:mode>_"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (unspec:ALL256 [(match_operand:EXT256 1 "register_operand" "+x")] UNSPEC_XSWAP256))
   (set (match_dup 1)
        (unspec:EXT256 [(match_operand:ALL256 2 "register_operand" "0")] UNSPEC_XSWAP256))]
  ""
  "xmovefo %0 = %1\n\txmovetq %1.lo = %x2, %y2\n\txmovetq %1.hi = %z2, %t2"
  [(set_attr "type" "all")
   (set_attr "length" "12")]
)


;; KVX_XMT44D

(define_insn "kvx_xmt44d"
  [(set (match_operand:EXT1024 0 "register_operand" "=x")
        (unspec:EXT1024 [(match_operand:EXT1024 1 "register_operand" "x")] UNSPEC_XMT44D))]
  ""
  "xmt44d %0 = %1"
  [(set_attr "type" "tca")]
)


;; KVX_XMMA484BW

(define_expand "kvx_xmma484bw"
  [(match_operand:EXT512 0 "register_operand" "")
   (match_operand:EXT256 1 "register_operand" "")
   (match_operand:EXT256 2 "register_operand" "")
   (match_operand:EXT512 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[4], 0);
    if (KV3_1)
      {
        if (!*xstr)
          emit_insn (gen_kvx_xmma484bw_1 (operands[0], operands[1], operands[2], operands[3]));
        else if (xstr[1] == 'u' && xstr[2] == 0)
          emit_insn (gen_kvx_xmmau484bw_1 (operands[0], operands[1], operands[2], operands[3]));
        else if (xstr[1] == 'u' && xstr[2] == 's')
          emit_insn (gen_kvx_xmmaus484bw_1 (operands[0], operands[1], operands[2], operands[3]));
        else if (xstr[1] == 's' && xstr[2] == 'u')
          emit_insn (gen_kvx_xmmasu484bw_1 (operands[0], operands[1], operands[2], operands[3]));
        else
          gcc_unreachable ();
      }
    if (KV3_2)
      {
        if (!*xstr)
          emit_insn (gen_kvx_xmma484bw_2 (operands[0], operands[1], operands[2], operands[3]));
        else if (xstr[1] == 'u' && xstr[2] == 0)
          emit_insn (gen_kvx_xmmau484bw_2 (operands[0], operands[1], operands[2], operands[3]));
        else if (xstr[1] == 'u' && xstr[2] == 's')
          emit_insn (gen_kvx_xmmaus484bw_2 (operands[0], operands[1], operands[2], operands[3]));
        else if (xstr[1] == 's' && xstr[2] == 'u')
          emit_insn (gen_kvx_xmmasu484bw_2 (operands[0], operands[1], operands[2], operands[3]));
        else
          gcc_unreachable ();
      }
    DONE;
  }
)

(define_insn "kvx_xmma484bw_1"
  [(set (match_operand:EXT512 0 "register_operand" "=x")
        (unspec:EXT512 [(match_operand:EXT256 1 "register_operand" "x")
                        (match_operand:EXT256 2 "register_operand" "x")
                        (match_operand:EXT512 3 "register_operand" "x")] UNSPEC_XMMA484BW))]
  "KV3_1"
  "xmma484bw %0 = %3, %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmau484bw_1"
  [(set (match_operand:EXT512 0 "register_operand" "=x")
        (unspec:EXT512 [(match_operand:EXT256 1 "register_operand" "x")
                        (match_operand:EXT256 2 "register_operand" "x")
                        (match_operand:EXT512 3 "register_operand" "x")] UNSPEC_XMMAU484BW))]
  "KV3_1"
  "xmma484ubw %0 = %3, %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmasu484bw_1"
  [(set (match_operand:EXT512 0 "register_operand" "=x")
        (unspec:EXT512 [(match_operand:EXT256 1 "register_operand" "x")
                        (match_operand:EXT256 2 "register_operand" "x")
                        (match_operand:EXT512 3 "register_operand" "x")] UNSPEC_XMMASU484BW))]
  "KV3_1"
  "xmma484subw %0 = %3, %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmaus484bw_1"
  [(set (match_operand:EXT512 0 "register_operand" "=x")
        (unspec:EXT512 [(match_operand:EXT256 1 "register_operand" "x")
                        (match_operand:EXT256 2 "register_operand" "x")
                        (match_operand:EXT512 3 "register_operand" "x")] UNSPEC_XMMAUS484BW))]
  "KV3_1"
  "xmma484usbw %0 = %3, %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmma484bw_2"
  [(set (match_operand:EXT512 0 "register_operand" "=x")
        (unspec:EXT512 [(match_operand:EXT256 1 "register_operand" "x")
                        (match_operand:EXT256 2 "register_operand" "x")
                        (match_operand:EXT512 3 "register_operand" "0")] UNSPEC_XMMA484BW))]
  "KV3_2"
  "xmma484bw %0 = %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmau484bw_2"
  [(set (match_operand:EXT512 0 "register_operand" "=x")
        (unspec:EXT512 [(match_operand:EXT256 1 "register_operand" "x")
                        (match_operand:EXT256 2 "register_operand" "x")
                        (match_operand:EXT512 3 "register_operand" "0")] UNSPEC_XMMAU484BW))]
  "KV3_2"
  "xmmau484bw %0 = %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmasu484bw_2"
  [(set (match_operand:EXT512 0 "register_operand" "=x")
        (unspec:EXT512 [(match_operand:EXT256 1 "register_operand" "x")
                        (match_operand:EXT256 2 "register_operand" "x")
                        (match_operand:EXT512 3 "register_operand" "0")] UNSPEC_XMMASU484BW))]
  "KV3_2"
  "xmmasu484bw %0 = %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmaus484bw_2"
  [(set (match_operand:EXT512 0 "register_operand" "=x")
        (unspec:EXT512 [(match_operand:EXT256 1 "register_operand" "x")
                        (match_operand:EXT256 2 "register_operand" "x")
                        (match_operand:EXT512 3 "register_operand" "0")] UNSPEC_XMMAUS484BW))]
  "KV3_2"
  "xmmaus484bw %0 = %1, %2"
  [(set_attr "type" "tca")]
)

