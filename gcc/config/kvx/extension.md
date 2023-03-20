;; XZERO

(define_insn "kvx_xzero256"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (match_operand:X256 1 "const_zero_operand" ""))]
  ""
  {
    if (KV3_1)
      return "#";
    return "xxoro %0 = %0, %0";
  }
  [(set_attr "type" "tca_int")]
)

(define_split
  [(set (match_operand:X256 0 "register_operand" "")
        (match_operand:X256 1 "const_zero_operand" ""))]
  "KV3_1"
  [(set (match_dup 1) (const_int 0))
   (set (match_dup 0)
        (unspec:X256 [(match_dup 1)] UNSPEC_XSPLATD))]
  {
    operands[1] = gen_reg_rtx (DImode);
  }
)

(define_insn "kvx_xzero<bitsize>"
  [(set (match_operand:XBUFF 0 "register_operand" "=x")
        (match_operand:XBUFF 1 "const_zero_operand" ""))]
  ""
  "#"
)

(define_split
  [(set (match_operand:XBUFF 0 "register_operand" "")
        (match_operand:XBUFF 1 "const_zero_operand" ""))]
  "KV3_1"
  [(set (match_dup 1) (const_int 0))
   (set (match_dup 0)
        (unspec:XBUFF [(match_dup 1)] UNSPEC_XSPLATD))]
  {
    operands[1] = gen_reg_rtx (DImode);
  }
)

(define_split
  [(set (match_operand:XBUFF 0 "register_operand" "")
        (match_operand:XBUFF 1 "const_zero_operand" ""))]
  "KV3_2"
  [(use (const_int 0))]
  {
    emit_insn (gen_kvx_xzero256 (gen_rtx_SUBREG (<CHUNK>mode, operands[0], 0), CONST0_RTX (<CHUNK>mode)));
    emit_insn (gen_kvx_xsplato<bitsize> (operands[0], gen_rtx_SUBREG (<CHUNK>mode, operands[0], 0)));
  }
)


;; 256-bit Extension Moves

(define_expand "mov<mode>"
  [(set (match_operand:X256 0 "nonimmediate_operand" "")
        (match_operand:X256 1 "general_operand" ""))]
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
  [(set (match_operand:X256 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x,r")
        (match_operand:X256 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r,r"))]
  "KV3_1"
  {
    switch (which_alternative)
      {
      case 0:
        return "xcopyo %0 = %1";
      case 1: case 2: case 3:
        return "xlo.u%X1 %0 = %1";
      case 4: case 5: case 6:
        return "xso%X0 %0 = %1";
      case 7:
        return "xmovefo %0 = %1";
      case 8:
        return "xmovetq %0.lo = %x1, %y1\n\txmovetq %0.hi = %z1, %t1";
      case 9:
        return "copyo %0 = %1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "bcu_crrp_crwl_crwh,lsu_load_uncached,lsu_load_uncached_x,lsu_load_uncached_y,lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y,bcu_tiny_auxw_crrp,alu_lite_x2_crwl_crwh,lsu_auxr_auxw")
   (set_attr "length"                "4,                4,                  8,                 12,             4,               8,              12,                 4,                    8,            4")]
)

(define_insn "*mov<mode>"
  [(set (match_operand:X256 0 "nonimmediate_operand" "=x, x, x, x, x, x, x,a,b,m,r,x,r")
        (match_operand:X256 1 "nonimmediate_operand"  "x,Ca,Cb,Cm,Za,Zb,Zm,x,x,x,x,r,r"))]
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
      case 12:
        return "copyo %0 = %1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "bcu_crrp_crwl_crwh,lsu_load,lsu_load_x,lsu_load_y,lsu_load_uncached,lsu_load_uncached_x,lsu_load_uncached_y,lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y,bcu_tiny_auxw_crrp,alu_thin_x2_crwl_crwh,lsu_auxr_auxw")
   (set_attr "length"                "4,       4,         8,        12,                4,                  8,                 12,             4,               8,              12,                 4,                    8,            4")]
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
  [(set (match_operand:X512 0 "nonimmediate_operand" "")
        (match_operand:X512 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn "*mov<X512:mode>"
  [(set (match_operand:X512 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:X512 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
)

(define_split
  [(set (match_operand:X512 0 "nonimmediate_operand" "")
        (match_operand:X512 1 "nonimmediate_operand" ""))]
  "reload_completed && (!extension_register_operand (operands[0], VOIDmode)
                        || !extension_register_operand (operands[1], VOIDmode))"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))]
  ""
)

(define_split
  [(set (match_operand:X512 0 "extension_register_operand" "")
        (match_operand:X512 1 "extension_register_operand" ""))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))]
  ""
)

(define_split
  [(set (match_operand:X512 0 "extension_register_operand" "")
        (match_operand:X512 1 "extension_register_operand" ""))]
  "KV3_2 && reload_completed"
  [(set (match_dup 0)
        (unspec:X512 [(match_dup 1) (const_string "")] UNSPEC_XCOPY))]
  ""
)


;; 1024-bit Extension Moves

(define_expand "mov<mode>"
  [(set (match_operand:X1024 0 "nonimmediate_operand" "")
        (match_operand:X1024 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn "*mov<X1024:mode>"
  [(set (match_operand:X1024 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:X1024 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
)

(define_split
  [(set (match_operand:X1024 0 "nonimmediate_operand" "")
        (match_operand:X1024 1 "nonimmediate_operand" ""))]
  "reload_completed && (!extension_register_operand (operands[0], VOIDmode)
                        || !extension_register_operand (operands[1], VOIDmode))"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 64))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 96))]
  ""
)

(define_split
  [(set (match_operand:X1024 0 "extension_register_operand" "")
        (match_operand:X1024 1 "extension_register_operand" ""))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 64))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 96))]
  ""
)

(define_split
  [(set (match_operand:X1024 0 "extension_register_operand" "")
        (match_operand:X1024 1 "extension_register_operand" ""))]
  "KV3_2 && reload_completed"
  [(set (match_dup 0)
        (unspec:X1024 [(match_dup 1) (const_string "")] UNSPEC_XCOPY))]
  ""
)


;; 2048-bit Extension Moves

(define_expand "mov<mode>"
  [(set (match_operand:X2048 0 "nonimmediate_operand" "")
        (match_operand:X2048 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn "*mov<X2048:mode>"
  [(set (match_operand:X2048 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:X2048 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
)

(define_split
  [(set (match_operand:X2048 0 "nonimmediate_operand" "")
        (match_operand:X2048 1 "nonimmediate_operand" ""))]
  "reload_completed && (!extension_register_operand (operands[0], VOIDmode)
                        || !extension_register_operand (operands[1], VOIDmode))"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 64))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 96))
   (set (subreg:<CHUNK> (match_dup 0) 128) (subreg:<CHUNK> (match_dup 1) 128))
   (set (subreg:<CHUNK> (match_dup 0) 160) (subreg:<CHUNK> (match_dup 1) 160))
   (set (subreg:<CHUNK> (match_dup 0) 192) (subreg:<CHUNK> (match_dup 1) 192))
   (set (subreg:<CHUNK> (match_dup 0) 224) (subreg:<CHUNK> (match_dup 1) 224))]
  ""
)

(define_split
  [(set (match_operand:X2048 0 "extension_register_operand" "")
        (match_operand:X2048 1 "extension_register_operand" ""))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 64))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 96))
   (set (subreg:<CHUNK> (match_dup 0) 128) (subreg:<CHUNK> (match_dup 1) 128))
   (set (subreg:<CHUNK> (match_dup 0) 160) (subreg:<CHUNK> (match_dup 1) 160))
   (set (subreg:<CHUNK> (match_dup 0) 192) (subreg:<CHUNK> (match_dup 1) 192))
   (set (subreg:<CHUNK> (match_dup 0) 224) (subreg:<CHUNK> (match_dup 1) 224))]
  ""
)

(define_split
  [(set (match_operand:X2048 0 "extension_register_operand" "")
        (match_operand:X2048 1 "extension_register_operand" ""))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 0)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 128)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 128) (const_string "")] UNSPEC_XCOPY))]
  ""
)


;; 4096-bit Extension Moves

(define_expand "mov<mode>"
  [(set (match_operand:X4096 0 "nonimmediate_operand" "")
        (match_operand:X4096 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn "*mov<X4096:mode>"
  [(set (match_operand:X4096 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:X4096 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
)

(define_split
  [(set (match_operand:X4096 0 "nonimmediate_operand" "")
        (match_operand:X4096 1 "nonimmediate_operand" ""))]
  "reload_completed && (!extension_register_operand (operands[0], VOIDmode)
                        || !extension_register_operand (operands[1], VOIDmode))"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 64))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 96))
   (set (subreg:<CHUNK> (match_dup 0) 128) (subreg:<CHUNK> (match_dup 1) 128))
   (set (subreg:<CHUNK> (match_dup 0) 160) (subreg:<CHUNK> (match_dup 1) 160))
   (set (subreg:<CHUNK> (match_dup 0) 192) (subreg:<CHUNK> (match_dup 1) 192))
   (set (subreg:<CHUNK> (match_dup 0) 224) (subreg:<CHUNK> (match_dup 1) 224))
   (set (subreg:<CHUNK> (match_dup 0) 256) (subreg:<CHUNK> (match_dup 1) 256))
   (set (subreg:<CHUNK> (match_dup 0) 288) (subreg:<CHUNK> (match_dup 1) 288))
   (set (subreg:<CHUNK> (match_dup 0) 320) (subreg:<CHUNK> (match_dup 1) 320))
   (set (subreg:<CHUNK> (match_dup 0) 352) (subreg:<CHUNK> (match_dup 1) 352))
   (set (subreg:<CHUNK> (match_dup 0) 384) (subreg:<CHUNK> (match_dup 1) 384))
   (set (subreg:<CHUNK> (match_dup 0) 416) (subreg:<CHUNK> (match_dup 1) 416))
   (set (subreg:<CHUNK> (match_dup 0) 448) (subreg:<CHUNK> (match_dup 1) 448))
   (set (subreg:<CHUNK> (match_dup 0) 480) (subreg:<CHUNK> (match_dup 1) 480))]
  ""
)

(define_split
  [(set (match_operand:X4096 0 "extension_register_operand" "")
        (match_operand:X4096 1 "extension_register_operand" ""))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 64))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 96))
   (set (subreg:<CHUNK> (match_dup 0) 128) (subreg:<CHUNK> (match_dup 1) 128))
   (set (subreg:<CHUNK> (match_dup 0) 160) (subreg:<CHUNK> (match_dup 1) 160))
   (set (subreg:<CHUNK> (match_dup 0) 192) (subreg:<CHUNK> (match_dup 1) 192))
   (set (subreg:<CHUNK> (match_dup 0) 224) (subreg:<CHUNK> (match_dup 1) 224))
   (set (subreg:<CHUNK> (match_dup 0) 256) (subreg:<CHUNK> (match_dup 1) 256))
   (set (subreg:<CHUNK> (match_dup 0) 288) (subreg:<CHUNK> (match_dup 1) 288))
   (set (subreg:<CHUNK> (match_dup 0) 320) (subreg:<CHUNK> (match_dup 1) 320))
   (set (subreg:<CHUNK> (match_dup 0) 352) (subreg:<CHUNK> (match_dup 1) 352))
   (set (subreg:<CHUNK> (match_dup 0) 384) (subreg:<CHUNK> (match_dup 1) 384))
   (set (subreg:<CHUNK> (match_dup 0) 416) (subreg:<CHUNK> (match_dup 1) 416))
   (set (subreg:<CHUNK> (match_dup 0) 448) (subreg:<CHUNK> (match_dup 1) 448))
   (set (subreg:<CHUNK> (match_dup 0) 480) (subreg:<CHUNK> (match_dup 1) 480))]
  ""
)

(define_split
  [(set (match_operand:X4096 0 "extension_register_operand" "")
        (match_operand:X4096 1 "extension_register_operand" ""))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 0)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 128)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 128) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 256)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 256) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 384)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 384) (const_string "")] UNSPEC_XCOPY))]
  ""
)


;; 8192-bit Extension Moves

(define_expand "mov<mode>"
  [(set (match_operand:X8192 0 "nonimmediate_operand" "")
        (match_operand:X8192 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn "*mov<X8192:mode>"
  [(set (match_operand:X8192 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:X8192 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
)

(define_split
  [(set (match_operand:X8192 0 "nonimmediate_operand" "")
        (match_operand:X8192 1 "nonimmediate_operand" ""))]
  "reload_completed && (!extension_register_operand (operands[0], VOIDmode)
                        || !extension_register_operand (operands[1], VOIDmode))"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 64))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 96))
   (set (subreg:<CHUNK> (match_dup 0) 128) (subreg:<CHUNK> (match_dup 1) 128))
   (set (subreg:<CHUNK> (match_dup 0) 160) (subreg:<CHUNK> (match_dup 1) 160))
   (set (subreg:<CHUNK> (match_dup 0) 192) (subreg:<CHUNK> (match_dup 1) 192))
   (set (subreg:<CHUNK> (match_dup 0) 224) (subreg:<CHUNK> (match_dup 1) 224))
   (set (subreg:<CHUNK> (match_dup 0) 256) (subreg:<CHUNK> (match_dup 1) 256))
   (set (subreg:<CHUNK> (match_dup 0) 288) (subreg:<CHUNK> (match_dup 1) 288))
   (set (subreg:<CHUNK> (match_dup 0) 320) (subreg:<CHUNK> (match_dup 1) 320))
   (set (subreg:<CHUNK> (match_dup 0) 352) (subreg:<CHUNK> (match_dup 1) 352))
   (set (subreg:<CHUNK> (match_dup 0) 384) (subreg:<CHUNK> (match_dup 1) 384))
   (set (subreg:<CHUNK> (match_dup 0) 416) (subreg:<CHUNK> (match_dup 1) 416))
   (set (subreg:<CHUNK> (match_dup 0) 448) (subreg:<CHUNK> (match_dup 1) 448))
   (set (subreg:<CHUNK> (match_dup 0) 480) (subreg:<CHUNK> (match_dup 1) 480))
   (set (subreg:<CHUNK> (match_dup 0) 512) (subreg:<CHUNK> (match_dup 1) 512))
   (set (subreg:<CHUNK> (match_dup 0) 544) (subreg:<CHUNK> (match_dup 1) 544))
   (set (subreg:<CHUNK> (match_dup 0) 576) (subreg:<CHUNK> (match_dup 1) 576))
   (set (subreg:<CHUNK> (match_dup 0) 608) (subreg:<CHUNK> (match_dup 1) 608))
   (set (subreg:<CHUNK> (match_dup 0) 640) (subreg:<CHUNK> (match_dup 1) 640))
   (set (subreg:<CHUNK> (match_dup 0) 672) (subreg:<CHUNK> (match_dup 1) 672))
   (set (subreg:<CHUNK> (match_dup 0) 704) (subreg:<CHUNK> (match_dup 1) 704))
   (set (subreg:<CHUNK> (match_dup 0) 736) (subreg:<CHUNK> (match_dup 1) 736))
   (set (subreg:<CHUNK> (match_dup 0) 768) (subreg:<CHUNK> (match_dup 1) 768))
   (set (subreg:<CHUNK> (match_dup 0) 800) (subreg:<CHUNK> (match_dup 1) 800))
   (set (subreg:<CHUNK> (match_dup 0) 832) (subreg:<CHUNK> (match_dup 1) 832))
   (set (subreg:<CHUNK> (match_dup 0) 864) (subreg:<CHUNK> (match_dup 1) 864))
   (set (subreg:<CHUNK> (match_dup 0) 896) (subreg:<CHUNK> (match_dup 1) 896))
   (set (subreg:<CHUNK> (match_dup 0) 928) (subreg:<CHUNK> (match_dup 1) 928))
   (set (subreg:<CHUNK> (match_dup 0) 960) (subreg:<CHUNK> (match_dup 1) 960))
   (set (subreg:<CHUNK> (match_dup 0) 992) (subreg:<CHUNK> (match_dup 1) 992))]
  ""
)

(define_split
  [(set (match_operand:X8192 0 "extension_register_operand" "")
        (match_operand:X8192 1 "extension_register_operand" ""))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 64))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 96))
   (set (subreg:<CHUNK> (match_dup 0) 128) (subreg:<CHUNK> (match_dup 1) 128))
   (set (subreg:<CHUNK> (match_dup 0) 160) (subreg:<CHUNK> (match_dup 1) 160))
   (set (subreg:<CHUNK> (match_dup 0) 192) (subreg:<CHUNK> (match_dup 1) 192))
   (set (subreg:<CHUNK> (match_dup 0) 224) (subreg:<CHUNK> (match_dup 1) 224))
   (set (subreg:<CHUNK> (match_dup 0) 256) (subreg:<CHUNK> (match_dup 1) 256))
   (set (subreg:<CHUNK> (match_dup 0) 288) (subreg:<CHUNK> (match_dup 1) 288))
   (set (subreg:<CHUNK> (match_dup 0) 320) (subreg:<CHUNK> (match_dup 1) 320))
   (set (subreg:<CHUNK> (match_dup 0) 352) (subreg:<CHUNK> (match_dup 1) 352))
   (set (subreg:<CHUNK> (match_dup 0) 384) (subreg:<CHUNK> (match_dup 1) 384))
   (set (subreg:<CHUNK> (match_dup 0) 416) (subreg:<CHUNK> (match_dup 1) 416))
   (set (subreg:<CHUNK> (match_dup 0) 448) (subreg:<CHUNK> (match_dup 1) 448))
   (set (subreg:<CHUNK> (match_dup 0) 480) (subreg:<CHUNK> (match_dup 1) 480))
   (set (subreg:<CHUNK> (match_dup 0) 512) (subreg:<CHUNK> (match_dup 1) 512))
   (set (subreg:<CHUNK> (match_dup 0) 544) (subreg:<CHUNK> (match_dup 1) 544))
   (set (subreg:<CHUNK> (match_dup 0) 576) (subreg:<CHUNK> (match_dup 1) 576))
   (set (subreg:<CHUNK> (match_dup 0) 608) (subreg:<CHUNK> (match_dup 1) 608))
   (set (subreg:<CHUNK> (match_dup 0) 640) (subreg:<CHUNK> (match_dup 1) 640))
   (set (subreg:<CHUNK> (match_dup 0) 672) (subreg:<CHUNK> (match_dup 1) 672))
   (set (subreg:<CHUNK> (match_dup 0) 704) (subreg:<CHUNK> (match_dup 1) 704))
   (set (subreg:<CHUNK> (match_dup 0) 736) (subreg:<CHUNK> (match_dup 1) 736))
   (set (subreg:<CHUNK> (match_dup 0) 768) (subreg:<CHUNK> (match_dup 1) 768))
   (set (subreg:<CHUNK> (match_dup 0) 800) (subreg:<CHUNK> (match_dup 1) 800))
   (set (subreg:<CHUNK> (match_dup 0) 832) (subreg:<CHUNK> (match_dup 1) 832))
   (set (subreg:<CHUNK> (match_dup 0) 864) (subreg:<CHUNK> (match_dup 1) 864))
   (set (subreg:<CHUNK> (match_dup 0) 896) (subreg:<CHUNK> (match_dup 1) 896))
   (set (subreg:<CHUNK> (match_dup 0) 928) (subreg:<CHUNK> (match_dup 1) 928))
   (set (subreg:<CHUNK> (match_dup 0) 960) (subreg:<CHUNK> (match_dup 1) 960))
   (set (subreg:<CHUNK> (match_dup 0) 992) (subreg:<CHUNK> (match_dup 1) 992))]
  ""
)

(define_split
  [(set (match_operand:X8192 0 "extension_register_operand" "")
        (match_operand:X8192 1 "extension_register_operand" ""))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 0)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 128)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 128) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 256)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 256) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 384)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 384) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 512)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 512) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 640)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 640) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 768)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 768) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 896)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 1) 896) (const_string "")] UNSPEC_XCOPY))]
  ""
)


;; XCAT*

(define_insn_and_split "kvx_xcat512"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (vec_concat:X512 (match_operand:<CHUNK> 1 "register_operand" "0")
                         (match_operand:<CHUNK> 2 "register_operand" "x")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 32)
        (match_dup 2))]
  ""
)

(define_insn "kvx_xcat1024"
  [(set (match_operand:X1024 0 "register_operand" "=x")
        (vec_concat:X1024 (match_operand:<HALF> 1 "register_operand" "0")
                          (match_operand:<HALF> 2 "register_operand" "x")))]
  ""
  "#"
)

(define_split
  [(set (match_operand:X1024 0 "register_operand" "")
        (vec_concat:X1024 (match_operand:<HALF> 1 "register_operand" "")
                          (match_operand:<HALF> 2 "register_operand" "")))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 64)
        (subreg:<CHUNK> (match_dup 2) 0))
   (set (subreg:<CHUNK> (match_dup 0) 96)
        (subreg:<CHUNK> (match_dup 2) 32))]
  ""
)

(define_split
  [(set (match_operand:X1024 0 "register_operand" "")
        (vec_concat:X1024 (match_operand:<HALF> 1 "register_operand" "")
                          (match_operand:<HALF> 2 "register_operand" "")))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK2> (match_dup 0) 64)
        (subreg:<CHUNK2> (match_dup 2) 0))]
  ""
)

(define_insn "kvx_xcat2048"
  [(set (match_operand:X2048 0 "register_operand" "=x")
        (vec_concat:X2048 (match_operand:<HALF> 1 "register_operand" "0")
                          (match_operand:<HALF> 2 "register_operand" "x")))]
  ""
  "#"
)

(define_split
  [(set (match_operand:X2048 0 "register_operand" "")
        (vec_concat:X2048 (match_operand:<HALF> 1 "register_operand" "")
                          (match_operand:<HALF> 2 "register_operand" "")))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 128)
        (subreg:<CHUNK> (match_dup 2) 0))
   (set (subreg:<CHUNK> (match_dup 0) 160)
        (subreg:<CHUNK> (match_dup 2) 32))
   (set (subreg:<CHUNK> (match_dup 0) 192)
        (subreg:<CHUNK> (match_dup 2) 64))
   (set (subreg:<CHUNK> (match_dup 0) 224)
        (subreg:<CHUNK> (match_dup 2) 96))]
  ""
)

(define_split
  [(set (match_operand:X2048 0 "register_operand" "")
        (vec_concat:X2048 (match_operand:<HALF> 1 "register_operand" "")
                          (match_operand:<HALF> 2 "register_operand" "")))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 128)
        (subreg:<CHUNK4> (match_dup 2) 0))]
  ""
)

(define_insn "kvx_xcat4096"
  [(set (match_operand:X4096 0 "register_operand" "=x")
        (vec_concat:X4096 (match_operand:<HALF> 1 "register_operand" "0")
                          (match_operand:<HALF> 2 "register_operand" "x")))]
  ""
  "#"
)

(define_split
  [(set (match_operand:X4096 0 "register_operand" "")
        (vec_concat:X4096 (match_operand:<HALF> 1 "register_operand" "")
                          (match_operand:<HALF> 2 "register_operand" "")))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 256)
        (subreg:<CHUNK> (match_dup 2) 0))
   (set (subreg:<CHUNK> (match_dup 0) 288)
        (subreg:<CHUNK> (match_dup 2) 32))
   (set (subreg:<CHUNK> (match_dup 0) 320)
        (subreg:<CHUNK> (match_dup 2) 64))
   (set (subreg:<CHUNK> (match_dup 0) 352)
        (subreg:<CHUNK> (match_dup 2) 96))
   (set (subreg:<CHUNK> (match_dup 0) 384)
        (subreg:<CHUNK> (match_dup 2) 128))
   (set (subreg:<CHUNK> (match_dup 0) 416)
        (subreg:<CHUNK> (match_dup 2) 160))
   (set (subreg:<CHUNK> (match_dup 0) 448)
        (subreg:<CHUNK> (match_dup 2) 192))
   (set (subreg:<CHUNK> (match_dup 0) 480)
        (subreg:<CHUNK> (match_dup 2) 224))]
  ""
)

(define_split
  [(set (match_operand:X4096 0 "register_operand" "")
        (vec_concat:X4096 (match_operand:<HALF> 1 "register_operand" "")
                          (match_operand:<HALF> 2 "register_operand" "")))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 256)
        (subreg:<CHUNK4> (match_dup 2) 0))
   (set (subreg:<CHUNK4> (match_dup 0) 384)
        (subreg:<CHUNK4> (match_dup 2) 128))]
  ""
)

(define_insn "kvx_xcat8192"
  [(set (match_operand:X8192 0 "register_operand" "=x")
        (vec_concat:X8192 (match_operand:<HALF> 1 "register_operand" "0")
                          (match_operand:<HALF> 2 "register_operand" "x")))]
  ""
  "#"
)

(define_split
  [(set (match_operand:X8192 0 "register_operand" "")
        (vec_concat:X8192 (match_operand:<HALF> 1 "register_operand" "")
                          (match_operand:<HALF> 2 "register_operand" "")))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 512)
        (subreg:<CHUNK> (match_dup 2) 0))
   (set (subreg:<CHUNK> (match_dup 0) 544)
        (subreg:<CHUNK> (match_dup 2) 32))
   (set (subreg:<CHUNK> (match_dup 0) 576)
        (subreg:<CHUNK> (match_dup 2) 64))
   (set (subreg:<CHUNK> (match_dup 0) 608)
        (subreg:<CHUNK> (match_dup 2) 96))
   (set (subreg:<CHUNK> (match_dup 0) 640)
        (subreg:<CHUNK> (match_dup 2) 128))
   (set (subreg:<CHUNK> (match_dup 0) 672)
        (subreg:<CHUNK> (match_dup 2) 160))
   (set (subreg:<CHUNK> (match_dup 0) 704)
        (subreg:<CHUNK> (match_dup 2) 192))
   (set (subreg:<CHUNK> (match_dup 0) 736)
        (subreg:<CHUNK> (match_dup 2) 224))
   (set (subreg:<CHUNK> (match_dup 0) 768)
        (subreg:<CHUNK> (match_dup 2) 256))
   (set (subreg:<CHUNK> (match_dup 0) 800)
        (subreg:<CHUNK> (match_dup 2) 288))
   (set (subreg:<CHUNK> (match_dup 0) 832)
        (subreg:<CHUNK> (match_dup 2) 320))
   (set (subreg:<CHUNK> (match_dup 0) 864)
        (subreg:<CHUNK> (match_dup 2) 352))
   (set (subreg:<CHUNK> (match_dup 0) 896)
        (subreg:<CHUNK> (match_dup 2) 384))
   (set (subreg:<CHUNK> (match_dup 0) 928)
        (subreg:<CHUNK> (match_dup 2) 416))
   (set (subreg:<CHUNK> (match_dup 0) 960)
        (subreg:<CHUNK> (match_dup 2) 448))
   (set (subreg:<CHUNK> (match_dup 0) 992)
        (subreg:<CHUNK> (match_dup 2) 480))]
  ""
)

(define_split
  [(set (match_operand:X8192 0 "register_operand" "")
        (vec_concat:X8192 (match_operand:<HALF> 1 "register_operand" "")
                          (match_operand:<HALF> 2 "register_operand" "")))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 512)
        (subreg:<CHUNK4> (match_dup 2) 0))
   (set (subreg:<CHUNK4> (match_dup 0) 640)
        (subreg:<CHUNK4> (match_dup 2) 128))
   (set (subreg:<CHUNK4> (match_dup 0) 768)
        (subreg:<CHUNK4> (match_dup 2) 256))
   (set (subreg:<CHUNK4> (match_dup 0) 896)
        (subreg:<CHUNK4> (match_dup 2) 384))]
  ""
)


;; XLOW*

(define_insn_and_split "kvx_xlow256"
  [(set (match_operand:<HALF> 0 "register_operand" "=x")
        (subreg:<HALF> (match_operand:X512 1 "register_operand" "x") 0))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0) (subreg:<CHUNK> (match_dup 1) 0))]
  ""
)

(define_insn "kvx_xlow512"
  [(set (match_operand:<HALF> 0 "register_operand" "=x")
        (subreg:<HALF> (match_operand:X1024 1 "register_operand" "x") 0))]
  ""
  "#"
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X1024 1 "register_operand" "") 0))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))]
  ""
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X1024 1 "register_operand" "") 0))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK2> (match_dup 0) 0) (subreg:<CHUNK2> (match_dup 1) 0))]
  ""
)

(define_insn "kvx_xlow1024"
  [(set (match_operand:<HALF> 0 "register_operand" "=x")
        (subreg:<HALF> (match_operand:X2048 1 "register_operand" "x") 0))]
  ""
  "#"
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X2048 1 "register_operand" "") 0))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 64))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 96))]
  ""
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X2048 1 "register_operand" "") 0))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 0) (subreg:<CHUNK4> (match_dup 1) 0))]
  ""
)

(define_insn "kvx_xlow2048"
  [(set (match_operand:<HALF> 0 "register_operand" "=x")
        (subreg:<HALF> (match_operand:X4096 1 "register_operand" "x") 0))]
  ""
  "#"
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X4096 1 "register_operand" "") 0))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 64))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 96))
   (set (subreg:<CHUNK> (match_dup 0) 128) (subreg:<CHUNK> (match_dup 1) 128))
   (set (subreg:<CHUNK> (match_dup 0) 160) (subreg:<CHUNK> (match_dup 1) 160))
   (set (subreg:<CHUNK> (match_dup 0) 192) (subreg:<CHUNK> (match_dup 1) 192))
   (set (subreg:<CHUNK> (match_dup 0) 224) (subreg:<CHUNK> (match_dup 1) 224))]
  ""
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X4096 1 "register_operand" "") 0))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 0) (subreg:<CHUNK4> (match_dup 1) 0))
   (set (subreg:<CHUNK4> (match_dup 0) 128) (subreg:<CHUNK4> (match_dup 1) 128))]
  ""
)

(define_insn "kvx_xlow4096"
  [(set (match_operand:<HALF> 0 "register_operand" "=x")
        (subreg:<HALF> (match_operand:X8192 1 "register_operand" "x") 0))]
  ""
  "#"
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X8192 1 "register_operand" "") 0))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 0))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 32))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 64))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 96))
   (set (subreg:<CHUNK> (match_dup 0) 128) (subreg:<CHUNK> (match_dup 1) 128))
   (set (subreg:<CHUNK> (match_dup 0) 160) (subreg:<CHUNK> (match_dup 1) 160))
   (set (subreg:<CHUNK> (match_dup 0) 192) (subreg:<CHUNK> (match_dup 1) 192))
   (set (subreg:<CHUNK> (match_dup 0) 224) (subreg:<CHUNK> (match_dup 1) 224))
   (set (subreg:<CHUNK> (match_dup 0) 256) (subreg:<CHUNK> (match_dup 1) 256))
   (set (subreg:<CHUNK> (match_dup 0) 288) (subreg:<CHUNK> (match_dup 1) 288))
   (set (subreg:<CHUNK> (match_dup 0) 320) (subreg:<CHUNK> (match_dup 1) 320))
   (set (subreg:<CHUNK> (match_dup 0) 352) (subreg:<CHUNK> (match_dup 1) 352))
   (set (subreg:<CHUNK> (match_dup 0) 384) (subreg:<CHUNK> (match_dup 1) 384))
   (set (subreg:<CHUNK> (match_dup 0) 416) (subreg:<CHUNK> (match_dup 1) 416))
   (set (subreg:<CHUNK> (match_dup 0) 448) (subreg:<CHUNK> (match_dup 1) 448))
   (set (subreg:<CHUNK> (match_dup 0) 480) (subreg:<CHUNK> (match_dup 1) 480))]
  ""
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X8192 1 "register_operand" "") 0))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 0) (subreg:<CHUNK4> (match_dup 1) 0))
   (set (subreg:<CHUNK4> (match_dup 0) 128) (subreg:<CHUNK4> (match_dup 1) 128))
   (set (subreg:<CHUNK4> (match_dup 0) 256) (subreg:<CHUNK4> (match_dup 1) 256))
   (set (subreg:<CHUNK4> (match_dup 0) 384) (subreg:<CHUNK4> (match_dup 1) 384))]
  ""
)


;; XHIGH*

(define_insn_and_split "kvx_xhigh256"
  [(set (match_operand:<HALF> 0 "register_operand" "=x")
        (subreg:<HALF> (match_operand:X512 1 "register_operand" "x") 32))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0) (subreg:X256 (match_dup 1) 32))]
  ""
)

(define_insn "kvx_xhigh512"
  [(set (match_operand:<HALF> 0 "register_operand" "=x")
        (subreg:<HALF> (match_operand:X1024 1 "register_operand" "x") 64))]
  ""
  "#"
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X1024 1 "register_operand" "") 64))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 64))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 96))]
  ""
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X1024 1 "register_operand" "") 64))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK2> (match_dup 0) 0) (subreg:<CHUNK2> (match_dup 1) 64))]
  ""
)

(define_insn "kvx_xhigh1024"
  [(set (match_operand:<HALF> 0 "register_operand" "=x")
        (subreg:<HALF> (match_operand:X2048 1 "register_operand" "x") 128))]
  ""
  "#"
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X2048 1 "register_operand" "") 128))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 128))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 160))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 192))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 224))]
  ""
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X2048 1 "register_operand" "") 128))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 0) (subreg:<CHUNK4> (match_dup 1) 128))]
  ""
)

(define_insn "kvx_xhigh2048"
  [(set (match_operand:<HALF> 0 "register_operand" "=x")
        (subreg:<HALF> (match_operand:X4096 1 "register_operand" "x") 256))]
  ""
  "#"
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X4096 1 "register_operand" "") 256))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 256))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 288))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 320))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 352))
   (set (subreg:<CHUNK> (match_dup 0) 128) (subreg:<CHUNK> (match_dup 1) 384))
   (set (subreg:<CHUNK> (match_dup 0) 160) (subreg:<CHUNK> (match_dup 1) 416))
   (set (subreg:<CHUNK> (match_dup 0) 192) (subreg:<CHUNK> (match_dup 1) 448))
   (set (subreg:<CHUNK> (match_dup 0) 224) (subreg:<CHUNK> (match_dup 1) 480))]
  ""
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X4096 1 "register_operand" "") 256))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 0) (subreg:<CHUNK4> (match_dup 1) 256))
   (set (subreg:<CHUNK4> (match_dup 0) 128) (subreg:<CHUNK4> (match_dup 1) 384))]
  ""
)

(define_insn "kvx_xhigh4096"
  [(set (match_operand:<HALF> 0 "register_operand" "=x")
        (subreg:<HALF> (match_operand:X8192 1 "register_operand" "x") 512))]
  ""
  "#"
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X8192 1 "register_operand" "") 512))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 512))
   (set (subreg:<CHUNK> (match_dup 0) 32) (subreg:<CHUNK> (match_dup 1) 544))
   (set (subreg:<CHUNK> (match_dup 0) 64) (subreg:<CHUNK> (match_dup 1) 576))
   (set (subreg:<CHUNK> (match_dup 0) 96) (subreg:<CHUNK> (match_dup 1) 608))
   (set (subreg:<CHUNK> (match_dup 0) 128) (subreg:<CHUNK> (match_dup 1) 640))
   (set (subreg:<CHUNK> (match_dup 0) 160) (subreg:<CHUNK> (match_dup 1) 672))
   (set (subreg:<CHUNK> (match_dup 0) 192) (subreg:<CHUNK> (match_dup 1) 704))
   (set (subreg:<CHUNK> (match_dup 0) 224) (subreg:<CHUNK> (match_dup 1) 736))
   (set (subreg:<CHUNK> (match_dup 0) 256) (subreg:<CHUNK> (match_dup 1) 768))
   (set (subreg:<CHUNK> (match_dup 0) 288) (subreg:<CHUNK> (match_dup 1) 800))
   (set (subreg:<CHUNK> (match_dup 0) 320) (subreg:<CHUNK> (match_dup 1) 832))
   (set (subreg:<CHUNK> (match_dup 0) 352) (subreg:<CHUNK> (match_dup 1) 864))
   (set (subreg:<CHUNK> (match_dup 0) 384) (subreg:<CHUNK> (match_dup 1) 896))
   (set (subreg:<CHUNK> (match_dup 0) 416) (subreg:<CHUNK> (match_dup 1) 928))
   (set (subreg:<CHUNK> (match_dup 0) 448) (subreg:<CHUNK> (match_dup 1) 960))
   (set (subreg:<CHUNK> (match_dup 0) 480) (subreg:<CHUNK> (match_dup 1) 992))]
  ""
)

(define_split
  [(set (match_operand:<HALF> 0 "register_operand" "")
        (subreg:<HALF> (match_operand:X8192 1 "register_operand" "") 512))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0) (subreg:<CHUNK> (match_dup 1) 512))
   (set (subreg:<CHUNK> (match_dup 0) 128) (subreg:<CHUNK> (match_dup 1) 640))
   (set (subreg:<CHUNK> (match_dup 0) 256) (subreg:<CHUNK> (match_dup 1) 768))
   (set (subreg:<CHUNK> (match_dup 0) 384) (subreg:<CHUNK> (match_dup 1) 896))]
  ""
)


;; XLOAD256, XLOAD512, XLOAD1024

(define_insn "kvx_xload256"
  [(set (match_operand:X256 0 "register_operand" "=x,x,x")
        (unspec:X256 [(match_operand:X256 1 "memory_operand" "a,b,m")
                        (match_operand 2 "" "")] UNSPEC_XLOAD))]
  ""
  "xlo%2%X1 %0 = %1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 2 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn_and_split "kvx_xload512"
  [(set (match_operand:X512 0 "register_operand" "=x,x,x")
        (unspec:X512 [(match_operand:X512 1 "memsimple_operand" "c,d,e")
                        (match_operand 2 "" "")] UNSPEC_XLOAD))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (match_dup 2)] UNSPEC_XLOAD))
   (set (subreg:<CHUNK> (match_dup 0) 32)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 32)
                         (match_dup 2)] UNSPEC_XLOAD))]
  ""
)

(define_insn_and_split "kvx_xload1024"
  [(set (match_operand:X1024 0 "register_operand" "=x,x,x")
        (unspec:X1024 [(match_operand:X1024 1 "memsimple_operand" "c,d,e")
                         (match_operand 2 "" "")] UNSPEC_XLOAD))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 0)
                         (match_dup 2)] UNSPEC_XLOAD))
   (set (subreg:<CHUNK> (match_dup 0) 32)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 32)
                         (match_dup 2)] UNSPEC_XLOAD))
   (set (subreg:<CHUNK> (match_dup 0) 64)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 64)
                         (match_dup 2)] UNSPEC_XLOAD))
   (set (subreg:<CHUNK> (match_dup 0) 96)
        (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 1) 96)
                         (match_dup 2)] UNSPEC_XLOAD))]
  ""
)


;; XLOADS1024, XLOADSC1024

(define_insn "kvx_xloads1024"
  [(set (match_operand:X1024 0 "register_operand" "=x,x,x")
        (unspec:X1024 [(match_operand:X1024 1 "register_operand" "0,0,0")
                         (match_operand:<CHUNK> 2 "memsimple_operand" "c,d,e")
                         (match_operand 3 "" "")] UNSPEC_XLOADS))]
  ""
  "xlo%3%X2 %0 = %2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_xloadsc1024"
  [(set (match_operand:X1024 0 "register_operand" "=x,x,x")
        (unspec:X1024 [(match_operand:X1024 1 "register_operand" "0,0,0")
                         (match_operand:<CHUNK> 2 "memsimple_operand" "c,d,e")
                         (match_operand:DI 3 "register_operand" "r,r,r")
                         (match_operand 4 "" "")] UNSPEC_XLOADS))]
  ""
  "xlo%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)


;; XSPLATD

(define_insn "kvx_xsplatd256"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD))]
  ""
  "xmovetq %0.lo = %1, %1\n\txmovetq %0.hi = %1, %1";
  [(set_attr "type" "alu_thin_x2_crwl_crwh")
   (set_attr "length"                   "8")]
)

(define_insn "kvx_xsplatd<bitsize>"
  [(set (match_operand:XBUFF 0 "register_operand" "=x")
        (unspec:XBUFF [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD))]
  ""
  "#"
)

(define_split
  [(set (match_operand:X512 0 "register_operand" "")
        (unspec:X512 [(match_operand:DI 1 "register_operand" "")] UNSPEC_XSPLATD))]
  "reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(match_dup 1)] UNSPEC_XSPLATD))
   (set (subreg:<CHUNK> (match_dup 0) 32)
        (subreg:<CHUNK> (match_dup 0) 0))]
  ""
)

(define_split
  [(set (match_operand:X1024 0 "register_operand" "")
        (unspec:X1024 [(match_operand:DI 1 "register_operand" "")] UNSPEC_XSPLATD))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(match_dup 1)] UNSPEC_XSPLATD))
   (set (subreg:<CHUNK> (match_dup 0) 32)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK> (match_dup 0) 64)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK> (match_dup 0) 96)
        (subreg:<CHUNK> (match_dup 0) 0))]
  ""
)

(define_split
  [(set (match_operand:X1024 0 "register_operand" "")
        (unspec:X1024 [(match_operand:DI 1 "register_operand" "")] UNSPEC_XSPLATD))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(match_dup 1)] UNSPEC_XSPLATD))
   (set (subreg:<CHUNK> (match_dup 0) 32)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK2> (match_dup 0) 64)
        (unspec:<CHUNK2> [(subreg:<CHUNK2> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))]
  ""
)

(define_split
  [(set (match_operand:X2048 0 "register_operand" "")
        (unspec:X2048 [(match_operand:DI 1 "register_operand" "")] UNSPEC_XSPLATD))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(match_dup 1)] UNSPEC_XSPLATD))
   (set (subreg:<CHUNK> (match_dup 0) 32)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK> (match_dup 0) 64)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK> (match_dup 0) 96)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK4> (match_dup 0) 128)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0)] UNSPEC_XMT44D))]
  ""
)

(define_split
  [(set (match_operand:X2048 0 "register_operand" "")
        (unspec:X2048 [(match_operand:DI 1 "register_operand" "")] UNSPEC_XSPLATD))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(match_dup 1)] UNSPEC_XSPLATD))
   (set (subreg:<CHUNK> (match_dup 0) 32)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK2> (match_dup 0) 64)
        (unspec:<CHUNK2> [(subreg:<CHUNK2> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 128)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))]
  ""
)

(define_split
  [(set (match_operand:X4096 0 "register_operand" "")
        (unspec:X4096 [(match_operand:DI 1 "register_operand" "")] UNSPEC_XSPLATD))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(match_dup 1)] UNSPEC_XSPLATD))
   (set (subreg:<CHUNK> (match_dup 0) 32)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK> (match_dup 0) 64)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK> (match_dup 0) 96)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK4> (match_dup 0) 128)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0)] UNSPEC_XMT44D))
   (set (subreg:<CHUNK4> (match_dup 0) 256)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 128)] UNSPEC_XMT44D))
   (set (subreg:<CHUNK4> (match_dup 0) 384)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 256)] UNSPEC_XMT44D))]
  ""
)

(define_split
  [(set (match_operand:X4096 0 "register_operand" "")
        (unspec:X4096 [(match_operand:DI 1 "register_operand" "")] UNSPEC_XSPLATD))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(match_dup 1)] UNSPEC_XSPLATD))
   (set (subreg:<CHUNK> (match_dup 0) 32)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK2> (match_dup 0) 64)
        (unspec:<CHUNK2> [(subreg:<CHUNK2> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 128)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 256)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 384)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))]
  ""
)

(define_split
  [(set (match_operand:X8192 0 "register_operand" "")
        (unspec:X8192 [(match_operand:DI 1 "register_operand" "")] UNSPEC_XSPLATD))]
  "KV3_1 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(match_dup 1)] UNSPEC_XSPLATD))
   (set (subreg:<CHUNK> (match_dup 0) 32)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK> (match_dup 0) 64)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK> (match_dup 0) 96)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK4> (match_dup 0) 128)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0)] UNSPEC_XMT44D))
   (set (subreg:<CHUNK4> (match_dup 0) 256)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 128)] UNSPEC_XMT44D))
   (set (subreg:<CHUNK4> (match_dup 0) 384)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 256)] UNSPEC_XMT44D))
   (set (subreg:<CHUNK4> (match_dup 0) 512)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 384)] UNSPEC_XMT44D))
   (set (subreg:<CHUNK4> (match_dup 0) 640)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 512)] UNSPEC_XMT44D))
   (set (subreg:<CHUNK4> (match_dup 0) 768)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 640)] UNSPEC_XMT44D))
   (set (subreg:<CHUNK4> (match_dup 0) 896)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 768)] UNSPEC_XMT44D))]
  ""
)

(define_split
  [(set (match_operand:X8192 0 "register_operand" "")
        (unspec:X8192 [(match_operand:DI 1 "register_operand" "")] UNSPEC_XSPLATD))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK> (match_dup 0) 0)
        (unspec:<CHUNK> [(match_dup 1)] UNSPEC_XSPLATD))
   (set (subreg:<CHUNK> (match_dup 0) 32)
        (subreg:<CHUNK> (match_dup 0) 0))
   (set (subreg:<CHUNK2> (match_dup 0) 64)
        (unspec:<CHUNK2> [(subreg:<CHUNK2> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 128)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 256)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 384)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 512)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 640)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 768)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))
   (set (subreg:<CHUNK4> (match_dup 0) 896)
        (unspec:<CHUNK4> [(subreg:<CHUNK4> (match_dup 0) 0) (const_string "")] UNSPEC_XCOPY))]
  ""
)


;; XSPLATO

(define_insn "kvx_xsplato<bitsize>"
  [(set (match_operand:XBUFF 0 "register_operand" "=x")
        (unspec:XBUFF [(match_operand:<CHUNK> 1 "register_operand" "x")] UNSPEC_XSPLATO))]
  ""
  "#"
)

(define_split
  [(set (match_operand:X512 0 "register_operand" "")
        (unspec:X512 [(match_operand:<CHUNK> 1 "register_operand" "")] UNSPEC_XSPLATO))]
  "KV3_2"
  [(set (match_dup 0)
        (unspec:X512 [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))]
  ""
)

(define_split
  [(set (match_operand:X1024 0 "register_operand" "")
        (unspec:X1024 [(match_operand:<CHUNK> 1 "register_operand" "")] UNSPEC_XSPLATO))]
  "KV3_2"
  [(set (match_dup 0)
        (unspec:X1024 [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))]
  ""
)

(define_split
  [(set (match_operand:X2048 0 "register_operand" "")
        (unspec:X2048 [(match_operand:<CHUNK> 1 "register_operand" "")] UNSPEC_XSPLATO))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 0)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))
   (set (subreg:<CHUNK4> (match_dup 0) 128)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))]
  ""
)

(define_split
  [(set (match_operand:X4096 0 "register_operand" "")
        (unspec:X4096 [(match_operand:<CHUNK> 1 "register_operand" "")] UNSPEC_XSPLATO))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 0)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))
   (set (subreg:<CHUNK4> (match_dup 0) 128)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))
   (set (subreg:<CHUNK4> (match_dup 0) 256)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))
   (set (subreg:<CHUNK4> (match_dup 0) 384)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))]
  ""
)

(define_split
  [(set (match_operand:X8192 0 "register_operand" "")
        (unspec:X8192 [(match_operand:<CHUNK> 1 "register_operand" "")] UNSPEC_XSPLATO))]
  "KV3_2 && reload_completed"
  [(set (subreg:<CHUNK4> (match_dup 0) 0)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))
   (set (subreg:<CHUNK4> (match_dup 0) 128)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))
   (set (subreg:<CHUNK4> (match_dup 0) 256)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))
   (set (subreg:<CHUNK4> (match_dup 0) 384)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))
   (set (subreg:<CHUNK4> (match_dup 0) 512)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))
   (set (subreg:<CHUNK4> (match_dup 0) 640)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))
   (set (subreg:<CHUNK4> (match_dup 0) 768)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))
   (set (subreg:<CHUNK4> (match_dup 0) 896)
        (unspec:<CHUNK4> [(match_dup 1) (const_string "")] UNSPEC_XSPLATO))]
  ""
)


;; XLOADC256, XLOADC512, XLOADC1024

(define_expand "kvx_xloadc256"
  [(match_operand:X256 0 "register_operand" "")
   (match_operand:X256 1 "reg_zero_mone_operand" "")
   (match_operand:X256 2 "memsimple_operand" "")
   (match_operand:DI 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    if (!const_zero_operand (operands[1], <MODE>mode))
      emit_insn (gen_kvx_xloadc_ (operands[0], operands[1], operands[2], operands[3], operands[4]));
    else
      emit_insn (gen_kvx_xloadc__ (operands[0], operands[2], operands[3], operands[4]));
    DONE;
  }
)

(define_insn "kvx_xloadc_"
  [(set (match_operand:X256 0 "register_operand" "=x,x,x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "0,0,0")
                      (match_operand:X256 2 "memsimple_operand" "c,d,e")
                      (match_operand:DI 3 "register_operand" "r,r,r")
                      (match_operand 4 "" "")] UNSPEC_XLOADC))]
  ""
  "xlo%4%X2 %3? %0 = %O2"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 4 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_insn "kvx_xloadc__"
  [(set (match_operand:X256 0 "register_operand" "=x,x,x")
        (unspec:X256 [(match_operand:X256 1 "memsimple_operand" "c,d,e")
                      (match_operand:DI 2 "register_operand" "r,r,r")
                      (match_operand 3 "" "")] UNSPEC_XLOADC))]
  ""
  "xlo%3%X1 %2? %0 = %O1"
  [(set_attr_alternative "type"
    [(if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached") (const_string "lsu_load"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_x") (const_string "lsu_load_x"))
     (if_then_else (match_operand 3 "uncached_modifier") (const_string "lsu_load_uncached_y") (const_string "lsu_load_y"))])
   (set_attr "length" "4, 8, 12")]
)

(define_expand "kvx_xloadc512"
  [(match_operand:X512 0 "register_operand" "")
   (match_operand:X512 1 "reg_zero_mone_operand" "")
   (match_operand:X512 2 "memsimple_operand" "")
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
    if (!const_zero_operand (operands[1], <MODE>mode))
      for (int i = 0; i < 2; i++)
        {
          rtx opnd0 = simplify_gen_subreg (<CHUNK>mode, operands[0], <MODE>mode, i*32);
          rtx opnd1 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, i*32);
          rtx opnd2 = simplify_gen_subreg (<CHUNK>mode, operands[2], <MODE>mode, i*32);
          emit_insn (gen_kvx_xloadc_ (opnd0, opnd1, opnd2, masks[i], operands[4]));
        }
    else
      for (int i = 0; i < 2; i++)
        {
          rtx opnd0 = simplify_gen_subreg (<CHUNK>mode, operands[0], <MODE>mode, i*32);
          rtx opnd2 = simplify_gen_subreg (<CHUNK>mode, operands[2], <MODE>mode, i*32);
          emit_insn (gen_kvx_xloadc__ (opnd0, opnd2, masks[i], operands[4]));
        }
    DONE;
  }
)

(define_expand "kvx_xloadc1024"
  [(match_operand:X1024 0 "register_operand" "")
   (match_operand:X1024 1 "reg_zero_mone_operand" "")
   (match_operand:X1024 2 "memsimple_operand" "")
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
    if (!const_zero_operand (operands[1], <MODE>mode))
      for (int i = 0; i < 4; i++)
        {
          rtx opnd0 = simplify_gen_subreg (<CHUNK>mode, operands[0], <MODE>mode, i*32);
          rtx opnd1 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, i*32);
          rtx opnd2 = simplify_gen_subreg (<CHUNK>mode, operands[2], <MODE>mode, i*32);
          emit_insn (gen_kvx_xloadc_ (opnd0, opnd1, opnd2, masks[i], operands[4]));
        }
    else
      for (int i = 0; i < 4; i++)
        {
          rtx opnd0 = simplify_gen_subreg (<CHUNK>mode, operands[0], <MODE>mode, i*32);
          rtx opnd2 = simplify_gen_subreg (<CHUNK>mode, operands[2], <MODE>mode, i*32);
          emit_insn (gen_kvx_xloadc__ (opnd0, opnd2, masks[i], operands[4]));
        }
    DONE;
  }
)


;; XSTORE256, XSTORE512, XSTORE1024

(define_insn "kvx_xstore256"
  [(set (match_operand:X256 1 "memory_operand"  "=a,b,m")
        (unspec:X256 [(match_operand:X256 0 "register_operand" "x,x,x")] UNSPEC_XSTORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "xso%X1 %1 = %0"
  [(set_attr "type" "lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn_and_split "kvx_xstore512"
  [(set (match_operand:X512 1 "memsimple_operand" "=c,d,e")
        (unspec:X512 [(match_operand:X512 0 "register_operand" "x,x,x")] UNSPEC_XSTORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "#"
  "reload_completed"
  [(parallel
    [(set (subreg:<CHUNK> (match_dup 1) 0)
          (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 0) 0)] UNSPEC_XSTORE))
     (use (match_operand:SI 2 "nonmemory_operand" ""))])
   (parallel
    [(set (subreg:<CHUNK> (match_dup 1) 32)
          (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 0) 32)] UNSPEC_XSTORE))
     (use (match_operand:SI 2 "nonmemory_operand" ""))])]
  ""
)

(define_insn_and_split "kvx_xstore1024"
  [(set (match_operand:X1024 1 "memsimple_operand" "=c,d,e")
        (unspec:X1024 [(match_operand:X1024 0 "register_operand" "x,x,x")] UNSPEC_XSTORE))
   (use (match_operand:SI 2 "nonmemory_operand" ""))]
  ""
  "#"
  "reload_completed"
  [(parallel
    [(set (subreg:<CHUNK> (match_dup 1) 0)
          (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 0) 0)] UNSPEC_XSTORE))
     (use (match_operand:SI 2 "nonmemory_operand" ""))])
   (parallel
    [(set (subreg:<CHUNK> (match_dup 1) 32)
          (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 0) 32)] UNSPEC_XSTORE))
     (use (match_operand:SI 2 "nonmemory_operand" ""))])
   (parallel
    [(set (subreg:<CHUNK> (match_dup 1) 64)
          (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 0) 64)] UNSPEC_XSTORE))
     (use (match_operand:SI 2 "nonmemory_operand" ""))])
   (parallel
    [(set (subreg:<CHUNK> (match_dup 1) 96)
          (unspec:<CHUNK> [(subreg:<CHUNK> (match_dup 0) 96)] UNSPEC_XSTORE))
     (use (match_operand:SI 2 "nonmemory_operand" ""))])]
  ""
)


;; XSTOREC256, XSTOREC512, XSTOREC1024

(define_insn "kvx_xstorec256"
  [(set (match_operand:X256 1 "memsimple_operand"  "=c,d,e")
        (unspec:X256 [(match_operand:X256 0 "register_operand" "x,x,x")
                        (match_operand:DI 2 "register_operand" "r,r,r")
                        (match_operand 3 "" "")] UNSPEC_XSTOREC))
   (use (match_operand:SI 4 "nonmemory_operand" ""))
   (clobber (match_dup 1))]
  ""
  "xso%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_expand "kvx_xstorec512"
  [(match_operand:X512 0 "register_operand" "")
   (match_operand:X512 1 "memsimple_operand" "")
   (match_operand:DI 2 "register_operand" "")
   (match_operand 3 "" "")
   (match_operand:SI 4 "nonmemory_operand" "")]
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
        rtx opnd0 = simplify_gen_subreg (<CHUNK>mode, operands[0], <MODE>mode, i*32);
        rtx opnd1 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, i*32);
        emit_insn (gen_kvx_xstorec256 (opnd0, opnd1, masks[i], operands[3], operands[4]));
      }
    DONE;
  }
)

(define_expand "kvx_xstorec1024"
  [(match_operand:X1024 0 "register_operand" "")
   (match_operand:X1024 1 "memsimple_operand" "")
   (match_operand:TI 2 "register_operand" "")
   (match_operand 3 "" "")
   (match_operand:SI 4 "nonmemory_operand" "")]
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
        rtx opnd0 = simplify_gen_subreg (<CHUNK>mode, operands[0], <MODE>mode, i*32);
        rtx opnd1 = simplify_gen_subreg (<CHUNK>mode, operands[1], <MODE>mode, i*32);
        emit_insn (gen_kvx_xstorec256 (opnd0, opnd1, masks[i], operands[3], operands[4]));
      }
    DONE;
  }
)


;; XPRELOAD*

(define_insn "kvx_xpreload<XBUFF:bitsize>"
  [(set (match_operand:XBUFF 0 "register_operand" "=x,x,x")
        (unspec:XBUFF [(match_operand:XBUFF 1 "register_operand" "0,0,0")
                       (match_operand:<CHUNK> 2 "memsimple_operand" "c,d,e")
                       (match_operand:DI 3 "register_operand" "r,r,r")
                       (match_operand 4 "" "")] UNSPEC_XPRELOAD))]
  "KV3_2"
  "xlo%4%X2 %b0, %3 = %O2"
  [(set_attr "type" "lsu,lsu_x,lsu_y")
   (set_attr "length" "4,   8,   12")]
)


;; XALIGN*O, XACCESS*O

(define_insn "kvx_xalign<XBUFF:bitsize>o"
  [(set (match_operand:<CHUNK> 0 "register_operand" "=x")
        (unspec:<CHUNK> [(match_operand:XBUFF 1 "register_operand" "x")
                         (match_operand:DI 2 "register_operand" "r")] UNSPEC_XALIGN256))]
  "KV3_2"
  "xaligno %0 = %b1, %2"
  [(set_attr "type" "bcu_crrp_crwl_crwh")]
)

(define_insn "kvx_xaccess<XBUFF:bitsize>o"
  [(set (match_operand:V256 0 "register_operand" "=r")
        (unspec:V256 [(match_operand:XBUFF 1 "register_operand" "x")
                      (match_operand:DI 2 "register_operand" "r")] UNSPEC_XALIGN256))]
  "KV3_2"
  "xaccesso %0 = %b1, %2"
  [(set_attr "type" "bcu_tiny_auxw_crrp")]
)


;; XFSCALEWO

(define_insn "kvx_xfscalewo"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:DI 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_XFSCALEWO))]
  "KV3_2"
  "xfscalewo%3 %0 = %1, %2"
  [(set_attr "type" "bcu_crrp_crwl_crwh")]
)


;; XMMA484BW

(define_expand "kvx_xmma484bw"
  [(match_operand:X512 0 "register_operand" "")
   (match_operand:X256 1 "register_operand" "")
   (match_operand:X256 2 "register_operand" "")
   (match_operand:X512 3 "register_operand" "")
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
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "x")] UNSPEC_XMMA484BW))]
  "KV3_1"
  "xmma484bw %0 = %3, %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmmau484bw_1"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "x")] UNSPEC_XMMAU484BW))]
  "KV3_1"
  "xmma484ubw %0 = %3, %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmmasu484bw_1"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "x")] UNSPEC_XMMASU484BW))]
  "KV3_1"
  "xmma484subw %0 = %3, %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmmaus484bw_1"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "x")] UNSPEC_XMMAUS484BW))]
  "KV3_1"
  "xmma484usbw %0 = %3, %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmma484bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMA484BW))]
  "KV3_2"
  "xmma484bw %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmmau484bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMAU484BW))]
  "KV3_2"
  "xmmau484bw %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmmasu484bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMASU484BW))]
  "KV3_2"
  "xmmasu484bw %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmmaus484bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMAUS484BW))]
  "KV3_2"
  "xmmaus484bw %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)


;; XMMA4164BW

(define_expand "kvx_xmma4164bw"
  [(match_operand:X512 0 "register_operand" "")
   (match_operand:X512 1 "register_operand" "")
   (match_operand:X512 2 "register_operand" "")
   (match_operand:X512 3 "register_operand" "")
   (match_operand 4 "" "")]
  "KV3_2"
  {
    const char *xstr = XSTR (operands[4], 0);
    if (!*xstr)
      emit_insn (gen_kvx_xmma4164bw_2 (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 'u' && xstr[2] == 0)
      emit_insn (gen_kvx_xmmau4164bw_2 (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 'u' && xstr[2] == 's')
      emit_insn (gen_kvx_xmmaus4164bw_2 (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 's' && xstr[2] == 'u')
      emit_insn (gen_kvx_xmmasu4164bw_2 (operands[0], operands[1], operands[2], operands[3]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "kvx_xmma4164bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X512 1 "register_operand" "x")
                      (match_operand:X512 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMA4164BW))]
  "KV3_2"
  "xmma4164bw %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmmau4164bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X512 1 "register_operand" "x")
                      (match_operand:X512 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMAU4164BW))]
  "KV3_2"
  "xmmau4164bw %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmmasu4164bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X512 1 "register_operand" "x")
                      (match_operand:X512 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMASU4164BW))]
  "KV3_2"
  "xmmasu4164bw %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmmaus4164bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X512 1 "register_operand" "x")
                      (match_operand:X512 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMAUS4164BW))]
  "KV3_2"
  "xmmaus4164bw %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)


;; XMADD44BW0, XMADD44BW1

(define_expand "kvx_xmadd44bw0"
  [(match_operand:X512 0 "register_operand" "")
   (match_operand:X256 1 "register_operand" "")
   (match_operand:X256 2 "register_operand" "")
   (match_operand:X512 3 "register_operand" "")
   (match_operand 4 "" "")]
  "KV3_2"
  {
    const char *xstr = XSTR (operands[4], 0);
    if (!*xstr)
      emit_insn (gen_kvx_xmadd44bw0_2 (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 'u' && xstr[2] == 0)
      emit_insn (gen_kvx_xmaddu44bw0_2 (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 's' && xstr[2] == 'u')
      emit_insn (gen_kvx_xmaddsu44bw0_2 (operands[0], operands[1], operands[2], operands[3]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "kvx_xmadd44bw0_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMADD44BW0))]
  "KV3_2"
  "xmadd44bw0 %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmaddu44bw0_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMADDU44BW0))]
  "KV3_2"
  "xmaddu44bw0 %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmaddsu44bw0_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMADDSU44BW0))]
  "KV3_2"
  "xmaddsu44bw0 %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_expand "kvx_xmadd44bw1"
  [(match_operand:X512 0 "register_operand" "")
   (match_operand:X256 1 "register_operand" "")
   (match_operand:X256 2 "register_operand" "")
   (match_operand:X512 3 "register_operand" "")
   (match_operand 4 "" "")]
  ""
  {
    const char *xstr = XSTR (operands[4], 0);
    if (!*xstr)
      emit_insn (gen_kvx_xmadd44bw1_2 (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 'u' && xstr[2] == 0)
      emit_insn (gen_kvx_xmaddu44bw1_2 (operands[0], operands[1], operands[2], operands[3]));
    else if (xstr[1] == 's' && xstr[2] == 'u')
      emit_insn (gen_kvx_xmaddsu44bw1_2 (operands[0], operands[1], operands[2], operands[3]));
    else
      gcc_unreachable ();
    DONE;
  }
)

(define_insn "kvx_xmadd44bw1_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMADD44BW1))]
  "KV3_2"
  "xmadd44bw1 %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmaddu44bw1_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMADDU44BW1))]
  "KV3_2"
  "xmaddu44bw1 %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmaddsu44bw1_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMADDSU44BW1))]
  "KV3_2"
  "xmaddsu44bw1 %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)


;; XMADDIFWO, XMSBFIFWO, XFFMA44HW

(define_insn "kvx_xmaddifwo"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X256 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_XMADDIFWO))]
  "KV3_2"
  "xmaddifwo%4 %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xmsbfifwo"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X256 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_XMSBFIFWO))]
  "KV3_2"
  "xmsbfifwo%4 %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xffma44hw"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_XFFMA44HW))]
  "KV3_2"
  "xffma44hw%4 %0 = %1, %2"
  [(set_attr "type" "tca_float")]
)


;; XFMMA444HW, XFMMA484HW

(define_insn "kvx_xfmma242hw01"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "x")] UNSPEC_XFMMA242HW01))]
  "KV3_1"
  "fmma242hw0 %0.lo = %3, %1, %2\n\t;;\n\tfmma242hw1 %0.hi = %3, %1, %2"
  [(set_attr "type" "tca_float")
   (set_attr "length" "12")])

(define_insn "kvx_xfmma242hw23"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "x")] UNSPEC_XFMMA242HW23))]
  "KV3_1"
  "fmma242hw2 %0.lo = %3, %1, %2\n\t;;\n\tfmma242hw3 %0.hi = %3, %1, %2"
  [(set_attr "type" "tca_float")
   (set_attr "length" "12")])

(define_insn_and_split "kvx_xfmma444hw"
  [(set (match_operand:X512 0 "register_operand" "=&x")
   (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                 (match_operand:X256 2 "register_operand" "x")
                 (match_operand:X512 3 "register_operand" "0")
                 (match_operand 4 "" "")] UNSPEC_XFMMA444HW))]
  "KV3_1"
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 0) 0)
        (unspec:X256 [(match_dup 1) (match_dup 2) (match_dup 3)] UNSPEC_XFMMA242HW01))
   (set (subreg:X256 (match_dup 0) 32)
        (unspec:X256 [(match_dup 1) (match_dup 2) (match_dup 3)] UNSPEC_XFMMA242HW23))]
  {
     operands[0] = simplify_subreg (V2OImode, operands[0], V2OImode, 0);
  })

(define_insn "kvx_xfmma484hw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X512 1 "register_operand" "x")
                      (match_operand:X512 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_XFMMA484HW))]
  "KV3_2"
  "xfmma484hw%4 %0 = %1, %2"
  [(set_attr "type" "tca_float")])

(define_expand "kvx_xfmma484hw"
  [(match_operand:X512 0 "register_operand")
   (match_operand:X512 1 "register_operand")
   (match_operand:X512 2 "register_operand")
   (match_operand:X512 3 "register_operand")
   (match_operand 4 "" "")]
  ""
  {
    if (KV3_1)
      {
        rtx lo256_1 = gen_reg_rtx (<CHUNK>mode);
        rtx lo256_2 = gen_reg_rtx (<CHUNK>mode);
        rtx hi256_1 = gen_reg_rtx (<CHUNK>mode);
        rtx hi256_2 = gen_reg_rtx (<CHUNK>mode);
        emit_insn (gen_kvx_xlow256 (lo256_1, operands[1]));
        emit_insn (gen_kvx_xlow256 (lo256_2, operands[2]));
        emit_insn (gen_kvx_xhigh256 (hi256_1, operands[1]));
        emit_insn (gen_kvx_xhigh256 (hi256_2, operands[2]));
        rtx tmp = gen_reg_rtx (V2OImode);
        emit_insn (gen_kvx_xfmma444hw (tmp, lo256_1, lo256_2, operands[3], operands[4]));
        emit_insn (gen_kvx_xfmma444hw (operands[0], hi256_1, hi256_2, tmp, operands[4]));
      }
    if (KV3_2)
      emit_insn (gen_kvx_xfmma484hw_2 (operands[0], operands[1], operands[2], operands[3], operands[4])) ;
    DONE;
  })


;; XFNARROW44WH, XCLAMPWO

(define_insn "kvx_xfnarrow44wh"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X512 1 "register_operand" "x")
                      (match_operand 2 "" "")] UNSPEC_XFNARROW44WH))]
  "KV3_2"
  "xfnarrow44wh%2 %0 = %1"
  [(set_attr "type" "tca_float")]
)

(define_insn "kvx_xclampwo"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X256 3 "register_operand" "0")] UNSPEC_XCLAMPWO))]
  "KV3_2"
  "xclampwo %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)


;; XTRUNC48WB, XSX48BW, XZX48BW

(define_insn "kvx_xtrunc48wb"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X1024 1 "register_operand" "x")] UNSPEC_XTRUNC48WB))]
  "KV3_2"
  "xtrunc48wb %0 = %1"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xsx48bw"
  [(set (match_operand:X1024 0 "register_operand" "=x")
        (unspec:X1024 [(match_operand:X256 1 "register_operand" "x")] UNSPEC_XSX48BW))]
  "KV3_2"
  "xsx48bw %0 = %1"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xzx48bw"
  [(set (match_operand:X1024 0 "register_operand" "=x")
        (unspec:X1024 [(match_operand:X256 1 "register_operand" "x")] UNSPEC_XZX48BW))]
  "KV3_2"
  "xzx48bw %0 = %1"
  [(set_attr "type" "tca_int")]
)


;; XSENDO, XRECVO, XSENDRECVO

(define_insn "kvx_xsendo"
  [(unspec_volatile [(match_operand:X256 0 "register_operand" "x")
                     (match_operand 1 "" "")] UNSPEC_XSENDO)]
  "KV3_2"
  "xsendo%1 %0"
  [(set_attr "type" "alu_tiny_crrp")]
)

(define_insn "kvx_xrecvo"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec_volatile:X256 [(match_operand 1 "" "")] UNSPEC_XRECVO))]
  "KV3_2"
  "xrecvo%1 %0"
  [(set_attr "type" "alu_tiny_crwl_crwh")]
)

(define_insn "kvx_xsendrecvo"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec_volatile:X256 [(match_operand:X256 1 "register_operand" "x")
                               (match_operand 2 "" "")] UNSPEC_XSENDRECVO))]
  "KV3_2"
  "xsendrecvo%2 %0, %1"
  [(set_attr "type" "alu_tiny_crrp_crwl_crwh")]
)


;; XSPLATOX, XSPLATOV

(define_insn "kvx_xsplatox"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand 2 "" "")] UNSPEC_XSPLATO))]
  ""
  "xsplatox%2 %0 = %1"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xsplatov"
  [(set (match_operand:X1024 0 "register_operand" "=x")
        (unspec:X1024 [(match_operand:X256 1 "register_operand" "x")
                       (match_operand 2 "" "")] UNSPEC_XSPLATO))]
  ""
  "xsplatov%2 %0 = %1"
  [(set_attr "type" "tca_int")]
)


;; XCOPYX, XCOPYV

(define_insn "kvx_xcopyx"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X512 1 "register_operand" "x")
                      (match_operand 2 "" "")] UNSPEC_XCOPY))]
  ""
  "xcopyx%2 %0 = %1"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xcopyv"
  [(set (match_operand:X1024 0 "register_operand" "=x")
        (unspec:X1024 [(match_operand:X1024 1 "register_operand" "x")
                       (match_operand 2 "" "")] UNSPEC_XCOPY))]
  ""
  "xcopyv%2 %0 = %1"
  [(set_attr "type" "tca_int")]
)


;; XMT44D

(define_insn "kvx_xmt44d"
  [(set (match_operand:X1024 0 "register_operand" "=x")
        (unspec:X1024 [(match_operand:X1024 1 "register_operand" "x")] UNSPEC_XMT44D))]
  ""
  "xmt44d %0 = %1"
  [(set_attr "type" "tca_int")]
)


;; XANDO, XNANDO, XANDNO, XIORO, XNIORO, XIORNO, XXORO, XNXORO, XSBMM8DQ, XSBMMT8DQ

(define_insn "kvx_xando"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (and:X256 (match_operand:X256 1 "register_operand" "x")
                  (match_operand:X256 2 "register_operand" "x")))]
  "KV3_2"
  "xando %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xnando"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (ior:X256 (not:X256 (match_operand:X256 1 "register_operand" "x"))
                  (not:X256 (match_operand:X256 2 "register_operand" "x"))))]
  "KV3_2"
  "xnando %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xandno"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (and:X256 (not:X256 (match_operand:X256 1 "register_operand" "x"))
                  (match_operand:X256 2 "register_operand" "x")))]
  "KV3_2"
  "xandno %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xioro"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (ior:X256 (match_operand:X256 1 "register_operand" "x")
                  (match_operand:X256 2 "register_operand" "x")))]
  "KV3_2"
  "xoro %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xnioro"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (and:X256 (not:X256 (match_operand:X256 1 "register_operand" "x"))
                  (not:X256 (match_operand:X256 2 "register_operand" "x"))))]
  "KV3_2"
  "xnoro %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xiorno"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (ior:X256 (not:X256 (match_operand:X256 1 "register_operand" "x"))
                  (match_operand:X256 2 "register_operand" "x")))]
  "KV3_2"
  "xorno %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xxoro"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (xor:X256 (match_operand:X256 1 "register_operand" "x")
                  (match_operand:X256 2 "register_operand" "x")))]
  "KV3_2"
  "xxoro %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xnxoro"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (not:X256 (xor:X256 (match_operand:X256 1 "register_operand" "x")
                            (match_operand:X256 2 "register_operand" "x"))))]
  "KV3_2"
  "xnxoro %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xsbmm8dq"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")] UNSPEC_SBMM8D))]
  ""
  "xsbmm8dq %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)

(define_insn "kvx_xsbmmt8dq"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")] UNSPEC_SBMMT8D))]
  ""
  "xsbmmt8dq %0 = %1, %2"
  [(set_attr "type" "tca_int")]
)


;; XSWAP256

(define_expand "kvx_xswap256"
  [(match_operand:V256 0 "register_operand" "")
   (match_operand:X256 1 "memory_operand" "")
   (match_operand:V256 2 "register_operand" "")]
  ""
  {
    rtx swapped = force_reg (<X256:MODE>mode, operands[1]);
    emit_insn (gen_kvx_xswap256v4di_ (operands[0], swapped, operands[2]));
    emit_move_insn (operands[1], swapped);
    DONE;
  }
)

(define_insn "kvx_xswap256<ALL256:mode>_"
  [(set (match_operand:ALL256 0 "register_operand" "=r")
        (unspec:ALL256 [(match_operand:X256 1 "register_operand" "+x")] UNSPEC_XSWAP256))
   (set (match_dup 1)
        (unspec:X256 [(match_operand:ALL256 2 "register_operand" "0")] UNSPEC_XSWAP256))]
  ""
  "xmovefo %0 = %1\n\txmovetq %1.lo = %x2, %y2\n\txmovetq %1.hi = %z2, %t2"
  [(set_attr "type" "all")
   (set_attr "length" "12")]
)

