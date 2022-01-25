;; Abstract the implentation modes

(define_mode_iterator EXT256 [
  V1OI
])

(define_mode_iterator EXT512 [
  V2OI
])

(define_mode_iterator EXT1024 [
  V4OI
])

;; 256-bit Extension Moves

(define_expand "mov<mode>"
  [(set (match_operand:EXT256 0 "nonimmediate_operand" "")
        (match_operand:EXT256 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn "*mov<mode>"
  [(set (match_operand:EXT256 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:EXT256 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  {
    switch (which_alternative)
      {
      case 0:
        return "copyv %0 = %1";
      case 1: case 2: case 3:
        return "xlo.u%X1 %v0 = %1";
      case 4: case 5: case 6:
        return "xso%X0 %0 = %v1";
      case 7:
        return "xmovefo %0 = %v1";
      case 8:
        return "xmovetq %v0.lo = %x1, %y1\n\txmovetq %v0.hi = %z1, %t1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "bcu_crrp_crwl_crwh,lsu_load_uncached,lsu_load_uncached_x,lsu_load_uncached_y,lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y,bcu_tiny_auxw_crrp,alu_lite_x2_crwl_crwh")
   (set_attr "length"                "4,                4,                  8,                 12,             4,               8,              12,                 4,                    8")]
)

(define_insn "*xmovef<mode>"
  [(set (match_operand:ALL256 0 "general_register_operand"  "=r")
        (match_operand:ALL256 1 "extension_register_operand" "x"))]
  ""
  "xmovefo %0 = %v1"
  [(set_attr "type" "bcu_tiny_auxw_crrp")
   (set_attr "length" "4")]
)

(define_insn "*xmovet<mode>"
  [(set (match_operand:ALL256 0 "extension_register_operand" "=x")
        (match_operand:ALL256 1 "general_register_operand"    "r"))]
  ""
  "xmovetq %v0.lo = %x1, %y1\n\txmovetq %v0.hi = %z1, %t1"
  [(set_attr "type" "alu_lite_x2_crwl_crwh")
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


;; KVX_XLOAD256, KVX_XLOAD256Q

(define_insn "kvx_xload256"
  [(set (match_operand:EXT256 0 "register_operand" "=x,x,x")
        (unspec:EXT256 [(match_operand:EXT256 1 "memory_operand" "a,b,m")
                        (match_operand 2 "" "")] UNSPEC_XLOAD256))
   (use (match_dup 1))]
  ""
  "xlo%2%X1 %v0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_xload256q"
  [(set (match_operand:EXT1024 0 "register_operand" "+x,x,x")
        (unspec:EXT1024 [(match_operand:EXT256 1 "memory_operand"     "a,b,m")
                         (match_operand 2 "" "")
                         (match_operand 3 "" "")] UNSPEC_XLOAD256))
   (use (match_dup 1))]
  ""
  "xlo.q%3%2%X1 %m0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_xloadc256"
  [(set (match_operand:EXT256 0 "register_operand" "=x,x,x")
        (unspec:EXT256 [(match_operand:EXT256 1 "memory_operand" "a,b,m")
                        (match_operand:DI 2 "register_operand" "r,r,r")
                        (match_operand 3 "" "")] UNSPEC_XLOAD256))
   (use (match_dup 1))]
  ""
  "xlo%3%X1 %2? %v0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)


;; KVX_XSTORE256

(define_insn "kvx_xstore256"
  [(set (match_operand:EXT256 1 "memory_operand"  "=a,b,m")
        (unspec:EXT256 [(match_operand:EXT256 0 "register_operand" "x,x,x")] UNSPEC_XSTORE256))
   (clobber (match_dup 1))]
  ""
  "xso%X1 %1 = %v0"
  [(set_attr "type" "lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn "kvx_xstorec256"
  [(set (match_operand:EXT256 1 "memory_operand"  "=a,b,m")
        (unspec:EXT256 [(match_operand:EXT256 0 "register_operand" "x,x,x")
                        (match_operand:DI 2 "register_operand" "r,r,r")
                        (match_operand 3 "" "")] UNSPEC_XSTORE256))
   (clobber (match_dup 1))]
  ""
  "xso%3%X1 %2? %1 = %v0"
  [(set_attr "type" "lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y")
   (set_attr "length"            "4,               8,              12")]
)


;; KVX_XSWAP256

(define_insn "kvx_xswap256"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (unspec:V32QI [(match_operand:EXT256 1 "register_operand" "+x")
                       (match_operand:V32QI 2 "register_operand" "0")] UNSPEC_XSWAP256))]
  ""
  "xmovetq %v1.lo = %x0, %y0\n\txmovetq %v1.hi = %z0, %t0\n\txmovefo %0 = %v1"
  [(set_attr "type" "all")
   (set_attr "length" "12")]
)


;; KVX_XMT44D

(define_insn "kvx_xmt44d"
  [(set (match_operand:EXT1024 0 "register_operand" "=x")
        (unspec [(match_operand:EXT1024 1 "register_operand" "x")] UNSPEC_XMT44D))]
  ""
  "xmt44d %m0 = %m1"
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
    if (!*xstr)
      emit_insn (gen_kvx_xmma484bw_1 (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 'u' && xstr[2] == 0)
      emit_insn (gen_kvx_xmma484ubw_1 (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 'u' && xstr[2] == 's')
      emit_insn (gen_kvx_xmma484usbw_1 (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 's' && xstr[2] == 'u')
      emit_insn (gen_kvx_xmma484subw_1 (operands[0], operands[1], operands[2], operands[3]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "kvx_xmma484bw_1"
  [(set (match_operand:EXT512 0 "register_operand" "=x")
        (unspec:EXT512 [(match_operand:EXT256 1 "register_operand" "x")
                        (match_operand:EXT256 2 "register_operand" "x")
                        (match_operand:EXT512 3 "register_operand" "x")] UNSPEC_XMMA484BW))]
  ""
  "mma484bw %w0 = %w3, %v1, %v2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmma484ubw_1"
  [(set (match_operand:EXT512 0 "register_operand" "=x")
        (unspec:EXT512 [(match_operand:EXT256 1 "register_operand" "x")
                        (match_operand:EXT256 2 "register_operand" "x")
                        (match_operand:EXT512 3 "register_operand" "x")] UNSPEC_XMMA484UBW))]
  ""
  "mma484ubw %w0 = %w3, %v1, %v2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmma484subw_1"
  [(set (match_operand:EXT512 0 "register_operand" "=x")
        (unspec:EXT512 [(match_operand:EXT256 1 "register_operand" "x")
                        (match_operand:EXT256 2 "register_operand" "x")
                        (match_operand:EXT512 3 "register_operand" "x")] UNSPEC_XMMA484SUBW))]
  ""
  "mma484subw %w0 = %w3, %v1, %v2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmma484usbw_1"
  [(set (match_operand:EXT512 0 "register_operand" "=x")
        (unspec:EXT512 [(match_operand:EXT256 1 "register_operand" "x")
                        (match_operand:EXT256 2 "register_operand" "x")
                        (match_operand:EXT512 3 "register_operand" "x")] UNSPEC_XMMA484USBW))]
  ""
  "mma484usbw %w0 = %w3, %v1, %v2"
  [(set_attr "type" "tca")]
)

