;; Abstract the extension modes

(define_mode_iterator EXT256 [ V1OI ])
(define_mode_iterator EXT512 [ V2OI ])
(define_mode_iterator EXT1024 [ V4OI ])
(define_mode_iterator EXT2048 [ V8OI ])
(define_mode_iterator EXT4096 [ V16OI ])
(define_mode_iterator EXT8192 [ V32OI ])

(define_mode_iterator EXTBUFF [
  V2OI V4OI V8OI V16OI V32OI
])

(define_expand "kvx_xundef<mode>"
  [(match_operand:EXT256 0 "register_operand" "")]
  ""
  {
    emit_clobber (operands[0]);
    DONE;
  }
)

(define_expand "kvx_xundef<mode>"
  [(match_operand:EXTBUFF 0 "register_operand" "")]
  ""
  {
    emit_clobber (operands[0]);
    DONE;
  }
)

(define_insn_and_split "*kvx_xundefv1oi"
  [(set (match_operand:EXT256 0 "register_operand" "=x")
        (match_operand:EXT256 1 "const_zero_operand" ""))]
  ""
  "#"
  "reload_completed"
  [(const_int 0)]
  ""
)

(define_insn_and_split "*kvx_xundef<mode>"
  [(set (match_operand:EXTBUFF 0 "register_operand" "=x")
        (match_operand:EXTBUFF 1 "const_zero_operand" ""))]
  ""
  "#"
  "reload_completed"
  [(const_int 0)]
  ""
)


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


;; 2048-bit Extension Moves

(define_expand "mov<mode>"
  [(set (match_operand:EXT2048 0 "nonimmediate_operand" "")
        (match_operand:EXT2048 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn_and_split "*mov<EXT2048:mode>"
  [(set (match_operand:EXT2048 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:EXT2048 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:EXT256 (match_dup 0) 0) (subreg:EXT256 (match_dup 1) 0))
   (set (subreg:EXT256 (match_dup 0) 32) (subreg:EXT256 (match_dup 1) 32))
   (set (subreg:EXT256 (match_dup 0) 64) (subreg:EXT256 (match_dup 1) 64))
   (set (subreg:EXT256 (match_dup 0) 96) (subreg:EXT256 (match_dup 1) 96))
   (set (subreg:EXT256 (match_dup 0) 128) (subreg:EXT256 (match_dup 1) 128))
   (set (subreg:EXT256 (match_dup 0) 160) (subreg:EXT256 (match_dup 1) 160))
   (set (subreg:EXT256 (match_dup 0) 192) (subreg:EXT256 (match_dup 1) 192))
   (set (subreg:EXT256 (match_dup 0) 224) (subreg:EXT256 (match_dup 1) 224))]
  ""
)


;; 4096-bit Extension Moves

(define_expand "mov<mode>"
  [(set (match_operand:EXT4096 0 "nonimmediate_operand" "")
        (match_operand:EXT4096 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn_and_split "*mov<EXT4096:mode>"
  [(set (match_operand:EXT4096 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:EXT4096 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:EXT256 (match_dup 0) 0) (subreg:EXT256 (match_dup 1) 0))
   (set (subreg:EXT256 (match_dup 0) 32) (subreg:EXT256 (match_dup 1) 32))
   (set (subreg:EXT256 (match_dup 0) 64) (subreg:EXT256 (match_dup 1) 64))
   (set (subreg:EXT256 (match_dup 0) 96) (subreg:EXT256 (match_dup 1) 96))
   (set (subreg:EXT256 (match_dup 0) 128) (subreg:EXT256 (match_dup 1) 128))
   (set (subreg:EXT256 (match_dup 0) 160) (subreg:EXT256 (match_dup 1) 160))
   (set (subreg:EXT256 (match_dup 0) 192) (subreg:EXT256 (match_dup 1) 192))
   (set (subreg:EXT256 (match_dup 0) 224) (subreg:EXT256 (match_dup 1) 224))
   (set (subreg:EXT256 (match_dup 0) 256) (subreg:EXT256 (match_dup 1) 256))
   (set (subreg:EXT256 (match_dup 0) 288) (subreg:EXT256 (match_dup 1) 288))
   (set (subreg:EXT256 (match_dup 0) 320) (subreg:EXT256 (match_dup 1) 320))
   (set (subreg:EXT256 (match_dup 0) 352) (subreg:EXT256 (match_dup 1) 352))
   (set (subreg:EXT256 (match_dup 0) 384) (subreg:EXT256 (match_dup 1) 384))
   (set (subreg:EXT256 (match_dup 0) 416) (subreg:EXT256 (match_dup 1) 416))
   (set (subreg:EXT256 (match_dup 0) 448) (subreg:EXT256 (match_dup 1) 448))
   (set (subreg:EXT256 (match_dup 0) 480) (subreg:EXT256 (match_dup 1) 480))]
  ""
)


;; 8192-bit Extension Moves

(define_expand "mov<mode>"
  [(set (match_operand:EXT8192 0 "nonimmediate_operand" "")
        (match_operand:EXT8192 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn_and_split "*mov<EXT8192:mode>"
  [(set (match_operand:EXT8192 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:EXT8192 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:EXT256 (match_dup 0) 0) (subreg:EXT256 (match_dup 1) 0))
   (set (subreg:EXT256 (match_dup 0) 32) (subreg:EXT256 (match_dup 1) 32))
   (set (subreg:EXT256 (match_dup 0) 64) (subreg:EXT256 (match_dup 1) 64))
   (set (subreg:EXT256 (match_dup 0) 96) (subreg:EXT256 (match_dup 1) 96))
   (set (subreg:EXT256 (match_dup 0) 128) (subreg:EXT256 (match_dup 1) 128))
   (set (subreg:EXT256 (match_dup 0) 160) (subreg:EXT256 (match_dup 1) 160))
   (set (subreg:EXT256 (match_dup 0) 192) (subreg:EXT256 (match_dup 1) 192))
   (set (subreg:EXT256 (match_dup 0) 224) (subreg:EXT256 (match_dup 1) 224))
   (set (subreg:EXT256 (match_dup 0) 256) (subreg:EXT256 (match_dup 1) 256))
   (set (subreg:EXT256 (match_dup 0) 288) (subreg:EXT256 (match_dup 1) 288))
   (set (subreg:EXT256 (match_dup 0) 320) (subreg:EXT256 (match_dup 1) 320))
   (set (subreg:EXT256 (match_dup 0) 352) (subreg:EXT256 (match_dup 1) 352))
   (set (subreg:EXT256 (match_dup 0) 384) (subreg:EXT256 (match_dup 1) 384))
   (set (subreg:EXT256 (match_dup 0) 416) (subreg:EXT256 (match_dup 1) 416))
   (set (subreg:EXT256 (match_dup 0) 448) (subreg:EXT256 (match_dup 1) 448))
   (set (subreg:EXT256 (match_dup 0) 480) (subreg:EXT256 (match_dup 1) 480))
   (set (subreg:EXT256 (match_dup 0) 512) (subreg:EXT256 (match_dup 1) 512))
   (set (subreg:EXT256 (match_dup 0) 544) (subreg:EXT256 (match_dup 1) 544))
   (set (subreg:EXT256 (match_dup 0) 576) (subreg:EXT256 (match_dup 1) 576))
   (set (subreg:EXT256 (match_dup 0) 608) (subreg:EXT256 (match_dup 1) 608))
   (set (subreg:EXT256 (match_dup 0) 640) (subreg:EXT256 (match_dup 1) 640))
   (set (subreg:EXT256 (match_dup 0) 672) (subreg:EXT256 (match_dup 1) 672))
   (set (subreg:EXT256 (match_dup 0) 704) (subreg:EXT256 (match_dup 1) 704))
   (set (subreg:EXT256 (match_dup 0) 736) (subreg:EXT256 (match_dup 1) 736))
   (set (subreg:EXT256 (match_dup 0) 768) (subreg:EXT256 (match_dup 1) 768))
   (set (subreg:EXT256 (match_dup 0) 800) (subreg:EXT256 (match_dup 1) 800))
   (set (subreg:EXT256 (match_dup 0) 832) (subreg:EXT256 (match_dup 1) 832))
   (set (subreg:EXT256 (match_dup 0) 864) (subreg:EXT256 (match_dup 1) 864))
   (set (subreg:EXT256 (match_dup 0) 896) (subreg:EXT256 (match_dup 1) 896))
   (set (subreg:EXT256 (match_dup 0) 928) (subreg:EXT256 (match_dup 1) 928))
   (set (subreg:EXT256 (match_dup 0) 960) (subreg:EXT256 (match_dup 1) 960))
   (set (subreg:EXT256 (match_dup 0) 992) (subreg:EXT256 (match_dup 1) 992))]
  ""
)


;; KVX_XLOAD256, KVX_XLOAD512, KVX_XLOAD1024

(define_insn "kvx_xload256"
  [(set (match_operand:EXT256 0 "register_operand" "=x,x,x")
        (unspec:EXT256 [(match_operand:EXT256 1 "memory_operand" "a,b,m")
                        (match_operand 2 "" "")] UNSPEC_XLOAD256))]
  ""
  "xlo%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn_and_split "kvx_xload512"
  [(set (match_operand:EXT512 0 "register_operand" "=x,x,x")
        (unspec:EXT512 [(match_operand:EXT512 1 "memory_operand" "a,b,m")
                        (match_operand 2 "" "")] UNSPEC_XLOAD512))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:EXT256 (match_dup 0) 0)
        (unspec:EXT256 [(subreg:EXT256 (match_dup 1) 0)
                        (match_dup 2)] UNSPEC_XLOAD256))
   (set (subreg:EXT256 (match_dup 0) 32)
        (unspec:EXT256 [(subreg:EXT256 (match_dup 1) 32)
                        (match_dup 2)] UNSPEC_XLOAD256))]
  ""
)

(define_insn_and_split "kvx_xload1024"
  [(set (match_operand:EXT1024 0 "register_operand" "=x,x,x")
        (unspec:EXT1024 [(match_operand:EXT1024 1 "memory_operand" "a,b,m")
                         (match_operand 2 "" "")] UNSPEC_XLOAD1024))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:EXT256 (match_dup 0) 0)
        (unspec:EXT256 [(subreg:EXT256 (match_dup 1) 0)
                        (match_dup 2)] UNSPEC_XLOAD256))
   (set (subreg:EXT256 (match_dup 0) 32)
        (unspec:EXT256 [(subreg:EXT256 (match_dup 1) 32)
                        (match_dup 2)] UNSPEC_XLOAD256))
   (set (subreg:EXT256 (match_dup 0) 64)
        (unspec:EXT256 [(subreg:EXT256 (match_dup 1) 64)
                        (match_dup 2)] UNSPEC_XLOAD256))
   (set (subreg:EXT256 (match_dup 0) 96)
        (unspec:EXT256 [(subreg:EXT256 (match_dup 1) 96)
                        (match_dup 2)] UNSPEC_XLOAD256))]
  ""
)


;; KVX_XLOADS512, KVX_XLOADSC512

(define_insn "kvx_xloads512"
  [(set (match_operand:EXT512 0 "register_operand" "=x,x,x")
        (unspec:EXT512 [(match_operand:EXT512 1 "register_operand" "0,0,0")
                        (match_operand:EXT256 2 "memory_operand" "a,b,m")
                        (match_operand 3 "" "")] UNSPEC_XLOAD256))]
  ""
  "xlo%3%X2 %0 = %2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_xloadsc512"
  [(set (match_operand:EXT512 0 "register_operand" "=x,x,x")
        (unspec:EXT512 [(match_operand:EXT512 1 "register_operand" "0,0,0")
                        (match_operand:EXT256 2 "memfoiled_operand" "c,d,e")
                        (match_operand:DI 3 "register_operand" "r,r,r")
                        (match_operand 4 "" "")] UNSPEC_XLOAD256))]
  ""
  "xlo%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)


;; KVX_XLOADS1024, KVX_XLOADSC1024

(define_insn "kvx_xloads1024"
  [(set (match_operand:EXT1024 0 "register_operand" "=x,x,x")
        (unspec:EXT1024 [(match_operand:EXT1024 1 "register_operand" "0,0,0")
                         (match_operand:EXT256 2 "memory_operand" "a,b,m")
                         (match_operand 3 "" "")] UNSPEC_XLOAD256))]
  ""
  "xlo%3%X2 %0 = %2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_xloadsc1024"
  [(set (match_operand:EXT1024 0 "register_operand" "=x,x,x")
        (unspec:EXT1024 [(match_operand:EXT1024 1 "register_operand" "0,0,0")
                         (match_operand:EXT256 2 "memfoiled_operand" "c,d,e")
                         (match_operand:DI 3 "register_operand" "r,r,r")
                         (match_operand 4 "" "")] UNSPEC_XLOAD256))]
  ""
  "xlo%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)


;; kvx_xsplatd256, kvx_xsplatd512, kvx_xsplatd1024, kvx_xsplatd2048, kvx_xsplatd4096, kvx_xsplatd8192

(define_insn "kvx_xsplatd256"
  [(set (match_operand:EXT256 0 "register_operand" "=x")
        (unspec:EXT256 [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD256))]
  ""
  "xmovetq %0.lo = %1, %1\n\txmovetq %0.hi = %1, %1";
  [(set_attr "type" "alu_lite_x2_crwl_crwh")
   (set_attr "length"                   "8")]
)

(define_insn_and_split "kvx_xsplatd512"
  [(set (match_operand:EXT512 0 "register_operand" "=x")
        (unspec [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD256))]
  ""
  "#"
  ""
  [(set (subreg:EXT256 (match_dup 0) 0)
        (unspec:EXT256 [(match_dup 1)] UNSPEC_XSPLATD256))
   (set (subreg:EXT256 (match_dup 0) 32)
        (subreg:EXT256 (match_dup 0) 0))]
  ""
)

(define_insn_and_split "kvx_xsplatd1024"
  [(set (match_operand:EXT1024 0 "register_operand" "=x")
        (unspec [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD256))]
  ""
  "#"
  ""
  [(set (subreg:EXT256 (match_dup 0) 0)
        (unspec:EXT256 [(match_dup 1)] UNSPEC_XSPLATD256))
   (set (subreg:EXT256 (match_dup 0) 32)
        (subreg:EXT256 (match_dup 0) 0))
   (set (subreg:EXT256 (match_dup 0) 64)
        (subreg:EXT256 (match_dup 0) 0))
   (set (subreg:EXT256 (match_dup 0) 96)
        (subreg:EXT256 (match_dup 0) 0))]
  ""
)

(define_insn_and_split "kvx_xsplatd2048"
  [(set (match_operand:EXT2048 0 "register_operand" "=x")
        (unspec [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD256))]
  ""
  "#"
  ""
  [(set (subreg:EXT256 (match_dup 0) 0)
        (unspec:EXT256 [(match_dup 1)] UNSPEC_XSPLATD256))
   (set (subreg:EXT256 (match_dup 0) 32)
        (subreg:EXT256 (match_dup 0) 0))
   (set (subreg:EXT256 (match_dup 0) 64)
        (subreg:EXT256 (match_dup 0) 0))
   (set (subreg:EXT256 (match_dup 0) 96)
        (subreg:EXT256 (match_dup 0) 0))
   (set (subreg:EXT1024 (match_dup 0) 128)
        (unspec:EXT1024 [(subreg:EXT1024 (match_dup 0) 0)] UNSPEC_XMT44D))]
  ""
)

(define_insn_and_split "kvx_xsplatd4096"
  [(set (match_operand:EXT4096 0 "register_operand" "=x")
        (unspec [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD256))]
  ""
  "#"
  ""
  [(set (subreg:EXT256 (match_dup 0) 0)
        (unspec:EXT256 [(match_dup 1)] UNSPEC_XSPLATD256))
   (set (subreg:EXT256 (match_dup 0) 32)
        (subreg:EXT256 (match_dup 0) 0))
   (set (subreg:EXT256 (match_dup 0) 64)
        (subreg:EXT256 (match_dup 0) 0))
   (set (subreg:EXT256 (match_dup 0) 96)
        (subreg:EXT256 (match_dup 0) 0))
   (set (subreg:EXT1024 (match_dup 0) 128)
        (unspec:EXT1024 [(subreg:EXT1024 (match_dup 0) 0)] UNSPEC_XMT44D))
   (set (subreg:EXT1024 (match_dup 0) 256)
        (unspec:EXT1024 [(subreg:EXT1024 (match_dup 0) 128)] UNSPEC_XMT44D))
   (set (subreg:EXT1024 (match_dup 0) 384)
        (unspec:EXT1024 [(subreg:EXT1024 (match_dup 0) 256)] UNSPEC_XMT44D))]
  ""
)

(define_insn_and_split "kvx_xsplatd8192"
  [(set (match_operand:EXT8192 0 "register_operand" "=x")
        (unspec [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD256))]
  ""
  "#"
  ""
  [(set (subreg:EXT256 (match_dup 0) 0)
        (unspec:EXT256 [(match_dup 1)] UNSPEC_XSPLATD256))
   (set (subreg:EXT256 (match_dup 0) 32)
        (subreg:EXT256 (match_dup 0) 0))
   (set (subreg:EXT256 (match_dup 0) 64)
        (subreg:EXT256 (match_dup 0) 0))
   (set (subreg:EXT256 (match_dup 0) 96)
        (subreg:EXT256 (match_dup 0) 0))
   (set (subreg:EXT1024 (match_dup 0) 128)
        (unspec:EXT1024 [(subreg:EXT1024 (match_dup 0) 0)] UNSPEC_XMT44D))
   (set (subreg:EXT1024 (match_dup 0) 256)
        (unspec:EXT1024 [(subreg:EXT1024 (match_dup 0) 128)] UNSPEC_XMT44D))
   (set (subreg:EXT1024 (match_dup 0) 384)
        (unspec:EXT1024 [(subreg:EXT1024 (match_dup 0) 256)] UNSPEC_XMT44D))
   (set (subreg:EXT1024 (match_dup 0) 512)
        (unspec:EXT1024 [(subreg:EXT1024 (match_dup 0) 384)] UNSPEC_XMT44D))
   (set (subreg:EXT1024 (match_dup 0) 640)
        (unspec:EXT1024 [(subreg:EXT1024 (match_dup 0) 512)] UNSPEC_XMT44D))
   (set (subreg:EXT1024 (match_dup 0) 768)
        (unspec:EXT1024 [(subreg:EXT1024 (match_dup 0) 640)] UNSPEC_XMT44D))
   (set (subreg:EXT1024 (match_dup 0) 896)
        (unspec:EXT1024 [(subreg:EXT1024 (match_dup 0) 768)] UNSPEC_XMT44D))]

  ""
)


;; KVX_XLOADC256, KVX_XLOADC512, KVX_XLOADC1024

(define_insn "kvx_xloadc256"
  [(set (match_operand:EXT256 0 "register_operand" "=x,x,x")
        (unspec:EXT256 [(match_operand:EXT256 1 "register_operand" "0,0,0")
                        (match_operand:EXT256 2 "memfoiled_operand" "c,d,e")
                        (match_operand:DI 3 "register_operand" "r,r,r")
                        (match_operand 4 "" "")] UNSPEC_XLOAD256))]
  ""
  "xlo%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_expand "kvx_xloadc512"
  [(match_operand:EXT512 0 "register_operand" "")
   (match_operand:EXT512 1 "register_operand" "")
   (match_operand:EXT512 2 "memfoiled_operand" "")
   (match_operand:DI 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    rtx masks[2];
    masks[0] = masks[1] = operands[3];
    if (masked_modifier (operands[4], VOIDmode))
      {
        masks[1] = gen_reg_rtx (DImode);
        emit_move_insn (masks[1], (gen_rtx_LSHIFTRT (DImode, operands[3], GEN_INT (32))));
      }
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = simplify_gen_subreg (V1OImode, operands[0], <MODE>mode, i*32);
        rtx opnd1 = simplify_gen_subreg (V1OImode, operands[1], <MODE>mode, i*32);
        rtx opnd2 = simplify_gen_subreg (V1OImode, operands[2], <MODE>mode, i*32);
        emit_insn (gen_kvx_xloadc256 (opnd0, opnd1, opnd2, masks[i], operands[4]));
      }
    DONE;
  }
)

(define_expand "kvx_xloadc1024"
  [(match_operand:EXT1024 0 "register_operand" "")
   (match_operand:EXT1024 1 "register_operand" "")
   (match_operand:EXT1024 2 "memfoiled_operand" "")
   (match_operand:TI 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    rtx masks[4];
    rtx mask = simplify_gen_subreg (DImode, operands[3], TImode, 0); 
    masks[0] = masks[1] = masks[2] = masks[3] = mask;
    if (masked_modifier (operands[4], VOIDmode))
      {
        masks[1] = gen_reg_rtx (DImode);
        emit_move_insn (masks[1], (gen_rtx_LSHIFTRT (DImode, masks[0], GEN_INT (32))));
        masks[2] = simplify_gen_subreg (DImode, operands[3], TImode, 8);
        masks[3] = gen_reg_rtx (DImode);
        emit_move_insn (masks[3], (gen_rtx_LSHIFTRT (DImode, masks[2], GEN_INT (32))));
      }
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = simplify_gen_subreg (V1OImode, operands[0], <MODE>mode, i*32);
        rtx opnd1 = simplify_gen_subreg (V1OImode, operands[1], <MODE>mode, i*32);
        rtx opnd2 = simplify_gen_subreg (V1OImode, operands[2], <MODE>mode, i*32);
        emit_insn (gen_kvx_xloadc256 (opnd0, opnd1, opnd2, masks[i], operands[4]));
      }
    DONE;
  }
)


;; KVX_XSTORE256, KVX_XSTORE512, KVX_XSTORE1024

(define_insn "kvx_xstore256"
  [(set (match_operand:EXT256 1 "memory_operand"  "=a,b,m")
        (unspec:EXT256 [(match_operand:EXT256 0 "register_operand" "x,x,x")] UNSPEC_XSTORE256))]
  ""
  "xso%X1 %1 = %0"
  [(set_attr "type" "lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn_and_split "kvx_xstore512"
  [(set (match_operand:EXT512 1 "memory_operand"  "=a,b,m")
        (unspec:EXT512 [(match_operand:EXT512 0 "register_operand" "x,x,x")] UNSPEC_XSTORE512))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:EXT256 (match_dup 1) 0)
        (unspec:EXT256 [(subreg:EXT256 (match_dup 0) 0)] UNSPEC_XSTORE256))
   (set (subreg:EXT256 (match_dup 1) 32)
        (unspec:EXT256 [(subreg:EXT256 (match_dup 0) 32)] UNSPEC_XSTORE256))]
  ""
)

(define_insn_and_split "kvx_xstore1024"
  [(set (match_operand:EXT1024 1 "memory_operand"  "=a,b,m")
        (unspec:EXT1024 [(match_operand:EXT1024 0 "register_operand" "x,x,x")] UNSPEC_XSTORE1024))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:EXT256 (match_dup 1) 0)
        (unspec:EXT256 [(subreg:EXT256 (match_dup 0) 0)] UNSPEC_XSTORE256))
   (set (subreg:EXT256 (match_dup 1) 32)
        (unspec:EXT256 [(subreg:EXT256 (match_dup 0) 32)] UNSPEC_XSTORE256))
   (set (subreg:EXT256 (match_dup 1) 64)
        (unspec:EXT256 [(subreg:EXT256 (match_dup 0) 64)] UNSPEC_XSTORE256))
   (set (subreg:EXT256 (match_dup 1) 96)
        (unspec:EXT256 [(subreg:EXT256 (match_dup 0) 96)] UNSPEC_XSTORE256))]
  ""
)


;; KVX_XSTOREC256, KVX_XSTOREC512, KVX_XSTOREC1024

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

(define_expand "kvx_xstorec512"
  [(match_operand:EXT512 0 "register_operand" "")
   (match_operand:EXT512 1 "memfoiled_operand" "")
   (match_operand:DI 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    rtx masks[2];
    masks[0] = masks[1] = operands[2];
    if (masked_modifier (operands[3], VOIDmode))
      {
        masks[1] = gen_reg_rtx (DImode);
        emit_move_insn (masks[1], (gen_rtx_LSHIFTRT (DImode, operands[2], GEN_INT (32))));
      }
    for (int i = 0; i < 2; i++)
      {
        rtx opnd0 = simplify_gen_subreg (V1OImode, operands[0], <MODE>mode, i*32);
        rtx opnd1 = simplify_gen_subreg (V1OImode, operands[1], <MODE>mode, i*32);
        emit_insn (gen_kvx_xstorec256 (opnd0, opnd1, masks[i], operands[3]));
      }
    DONE;
  }
)

(define_expand "kvx_xstorec1024"
  [(match_operand:EXT1024 0 "register_operand" "")
   (match_operand:EXT1024 1 "memfoiled_operand" "")
   (match_operand:TI 2 "register_operand" "")
   (match_operand 3 "" "")]
  ""
  {
    rtx masks[4];
    rtx mask = simplify_gen_subreg (DImode, operands[2], TImode, 0); 
    masks[0] = masks[1] = masks[2] = masks[3] = mask;
    if (masked_modifier (operands[3], VOIDmode))
      {
        masks[1] = gen_reg_rtx (DImode);
        emit_move_insn (masks[1], (gen_rtx_LSHIFTRT (DImode, masks[0], GEN_INT (32))));
        masks[2] = simplify_gen_subreg (DImode, operands[2], TImode, 8);
        masks[3] = gen_reg_rtx (DImode);
        emit_move_insn (masks[3], (gen_rtx_LSHIFTRT (DImode, masks[2], GEN_INT (32))));
      }
    for (int i = 0; i < 4; i++)
      {
        rtx opnd0 = simplify_gen_subreg (V1OImode, operands[0], <MODE>mode, i*32);
        rtx opnd1 = simplify_gen_subreg (V1OImode, operands[1], <MODE>mode, i*32);
        emit_insn (gen_kvx_xstorec256 (opnd0, opnd1, masks[i], operands[3]));
      }
    DONE;
  }
)


;; KVX_XPRELOAD*

(define_insn "kvx_xpreload<EXTBUFF:mode>"
  [(set (match_operand:EXTBUFF 0 "register_operand" "=x,x,x")
        (unspec:EXTBUFF [(match_operand:EXTBUFF 1 "register_operand" "0,0,0")
                         (match_operand:EXT256 2 "memfoiled_operand" "c,d,e")
                         (match_operand:DI 3 "register_operand" "r,r,r")
                         (match_operand 4 "" "")] UNSPEC_XPRELOAD))]
  ""
  "xlo%4%X2 %b0, %3 = %O2"
  [(set_attr "type" "lsu,lsu_x,lsu_y")
   (set_attr "length" "4,   8,   12")]
)


;; KVX_XALIGNO*, KVX_XACCESSO*

(define_insn "kvx_xaligno<EXTBUFF:mode>"
  [(set (match_operand:EXT256 0 "register_operand" "=x")
        (unspec:EXT256 [(match_operand:EXTBUFF 1 "register_operand" "x")
                        (match_operand:DI 2 "register_operand" "r")] UNSPEC_XALIGN256))]
  ""
  "xaligno %0 = %b1, %2"
  [(set_attr "type" "bcu_crrp_crwl_crwh")]
)

(define_insn "kvx_xaccesso<EXTBUFF:mode>"
  [(set (match_operand:V32QI 0 "register_operand" "=r")
        (unspec:V32QI [(match_operand:EXTBUFF 1 "register_operand" "x")
                       (match_operand:DI 2 "register_operand" "r")] UNSPEC_XALIGN256))]
  ""
  "xaccesso %0 = %b1, %2"
  [(set_attr "type" "bcu_tiny_auxw_crrp")]
)


;; KVX_XSWAP256

(define_expand "kvx_xswap256"
  [(match_operand:V32QI 0 "register_operand" "")
   (match_operand:EXT256 1 "memory_operand" "")
   (match_operand:V32QI 2 "register_operand" "")]
  ""
  {
    rtx swapped = force_reg (<EXT256:MODE>mode, operands[1]);
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

