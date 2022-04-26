
;; KVX_UNDEF

(define_expand "kvx_xundef<bitsize>"
  [(match_operand:X256 0 "register_operand" "")]
  ""
  {
    emit_clobber (operands[0]);
    DONE;
  }
)

(define_expand "kvx_xundef<bitsize>"
  [(match_operand:XBUFF 0 "register_operand" "")]
  ""
  {
    emit_clobber (operands[0]);
    DONE;
  }
)

(define_insn_and_split "*kvx_xundef256"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (match_operand:X256 1 "const_zero_operand" ""))]
  ""
  "#"
  "reload_completed"
  [(const_int 0)]
  ""
)

(define_insn_and_split "*kvx_xundef<bitsize>"
  [(set (match_operand:XBUFF 0 "register_operand" "=x")
        (match_operand:XBUFF 1 "const_zero_operand" ""))]
  ""
  "#"
  "reload_completed"
  [(const_int 0)]
  ""
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
  [(set (match_operand:X256 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:X256 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
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
  [(set (match_operand:X256 0 "nonimmediate_operand" "=x, x, x, x, x, x, x,a,b,m,r,x")
        (match_operand:X256 1 "nonimmediate_operand"  "x,Ca,Cb,Cm,Za,Zb,Zm,x,x,x,x,r"))]
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
  [(set (match_operand:X512 0 "nonimmediate_operand" "")
        (match_operand:X512 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (<MODE>mode, operands[1]);
  }
)

(define_insn_and_split "*mov<X512:mode>"
  [(set (match_operand:X512 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:X512 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 0) 0) (subreg:X256 (match_dup 1) 0))
   (set (subreg:X256 (match_dup 0) 32) (subreg:X256 (match_dup 1) 32))]
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

(define_insn_and_split "*mov<X1024:mode>"
  [(set (match_operand:X1024 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:X1024 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 0) 0) (subreg:X256 (match_dup 1) 0))
   (set (subreg:X256 (match_dup 0) 32) (subreg:X256 (match_dup 1) 32))
   (set (subreg:X256 (match_dup 0) 64) (subreg:X256 (match_dup 1) 64))
   (set (subreg:X256 (match_dup 0) 96) (subreg:X256 (match_dup 1) 96))]
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

(define_insn_and_split "*mov<X2048:mode>"
  [(set (match_operand:X2048 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:X2048 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 0) 0) (subreg:X256 (match_dup 1) 0))
   (set (subreg:X256 (match_dup 0) 32) (subreg:X256 (match_dup 1) 32))
   (set (subreg:X256 (match_dup 0) 64) (subreg:X256 (match_dup 1) 64))
   (set (subreg:X256 (match_dup 0) 96) (subreg:X256 (match_dup 1) 96))
   (set (subreg:X256 (match_dup 0) 128) (subreg:X256 (match_dup 1) 128))
   (set (subreg:X256 (match_dup 0) 160) (subreg:X256 (match_dup 1) 160))
   (set (subreg:X256 (match_dup 0) 192) (subreg:X256 (match_dup 1) 192))
   (set (subreg:X256 (match_dup 0) 224) (subreg:X256 (match_dup 1) 224))]
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

(define_insn_and_split "*mov<X4096:mode>"
  [(set (match_operand:X4096 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:X4096 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 0) 0) (subreg:X256 (match_dup 1) 0))
   (set (subreg:X256 (match_dup 0) 32) (subreg:X256 (match_dup 1) 32))
   (set (subreg:X256 (match_dup 0) 64) (subreg:X256 (match_dup 1) 64))
   (set (subreg:X256 (match_dup 0) 96) (subreg:X256 (match_dup 1) 96))
   (set (subreg:X256 (match_dup 0) 128) (subreg:X256 (match_dup 1) 128))
   (set (subreg:X256 (match_dup 0) 160) (subreg:X256 (match_dup 1) 160))
   (set (subreg:X256 (match_dup 0) 192) (subreg:X256 (match_dup 1) 192))
   (set (subreg:X256 (match_dup 0) 224) (subreg:X256 (match_dup 1) 224))
   (set (subreg:X256 (match_dup 0) 256) (subreg:X256 (match_dup 1) 256))
   (set (subreg:X256 (match_dup 0) 288) (subreg:X256 (match_dup 1) 288))
   (set (subreg:X256 (match_dup 0) 320) (subreg:X256 (match_dup 1) 320))
   (set (subreg:X256 (match_dup 0) 352) (subreg:X256 (match_dup 1) 352))
   (set (subreg:X256 (match_dup 0) 384) (subreg:X256 (match_dup 1) 384))
   (set (subreg:X256 (match_dup 0) 416) (subreg:X256 (match_dup 1) 416))
   (set (subreg:X256 (match_dup 0) 448) (subreg:X256 (match_dup 1) 448))
   (set (subreg:X256 (match_dup 0) 480) (subreg:X256 (match_dup 1) 480))]
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

(define_insn_and_split "*mov<X8192:mode>"
  [(set (match_operand:X8192 0 "nonimmediate_operand" "=x,x,x,x,a,b,m,r,x")
        (match_operand:X8192 1 "nonimmediate_operand"  "x,a,b,m,x,x,x,x,r"))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 0) 0) (subreg:X256 (match_dup 1) 0))
   (set (subreg:X256 (match_dup 0) 32) (subreg:X256 (match_dup 1) 32))
   (set (subreg:X256 (match_dup 0) 64) (subreg:X256 (match_dup 1) 64))
   (set (subreg:X256 (match_dup 0) 96) (subreg:X256 (match_dup 1) 96))
   (set (subreg:X256 (match_dup 0) 128) (subreg:X256 (match_dup 1) 128))
   (set (subreg:X256 (match_dup 0) 160) (subreg:X256 (match_dup 1) 160))
   (set (subreg:X256 (match_dup 0) 192) (subreg:X256 (match_dup 1) 192))
   (set (subreg:X256 (match_dup 0) 224) (subreg:X256 (match_dup 1) 224))
   (set (subreg:X256 (match_dup 0) 256) (subreg:X256 (match_dup 1) 256))
   (set (subreg:X256 (match_dup 0) 288) (subreg:X256 (match_dup 1) 288))
   (set (subreg:X256 (match_dup 0) 320) (subreg:X256 (match_dup 1) 320))
   (set (subreg:X256 (match_dup 0) 352) (subreg:X256 (match_dup 1) 352))
   (set (subreg:X256 (match_dup 0) 384) (subreg:X256 (match_dup 1) 384))
   (set (subreg:X256 (match_dup 0) 416) (subreg:X256 (match_dup 1) 416))
   (set (subreg:X256 (match_dup 0) 448) (subreg:X256 (match_dup 1) 448))
   (set (subreg:X256 (match_dup 0) 480) (subreg:X256 (match_dup 1) 480))
   (set (subreg:X256 (match_dup 0) 512) (subreg:X256 (match_dup 1) 512))
   (set (subreg:X256 (match_dup 0) 544) (subreg:X256 (match_dup 1) 544))
   (set (subreg:X256 (match_dup 0) 576) (subreg:X256 (match_dup 1) 576))
   (set (subreg:X256 (match_dup 0) 608) (subreg:X256 (match_dup 1) 608))
   (set (subreg:X256 (match_dup 0) 640) (subreg:X256 (match_dup 1) 640))
   (set (subreg:X256 (match_dup 0) 672) (subreg:X256 (match_dup 1) 672))
   (set (subreg:X256 (match_dup 0) 704) (subreg:X256 (match_dup 1) 704))
   (set (subreg:X256 (match_dup 0) 736) (subreg:X256 (match_dup 1) 736))
   (set (subreg:X256 (match_dup 0) 768) (subreg:X256 (match_dup 1) 768))
   (set (subreg:X256 (match_dup 0) 800) (subreg:X256 (match_dup 1) 800))
   (set (subreg:X256 (match_dup 0) 832) (subreg:X256 (match_dup 1) 832))
   (set (subreg:X256 (match_dup 0) 864) (subreg:X256 (match_dup 1) 864))
   (set (subreg:X256 (match_dup 0) 896) (subreg:X256 (match_dup 1) 896))
   (set (subreg:X256 (match_dup 0) 928) (subreg:X256 (match_dup 1) 928))
   (set (subreg:X256 (match_dup 0) 960) (subreg:X256 (match_dup 1) 960))
   (set (subreg:X256 (match_dup 0) 992) (subreg:X256 (match_dup 1) 992))]
  ""
)


;; KVX_XCAT*, KVX_XLOW*, KVX_XHIGH*

(define_insn_and_split "kvx_xcat512"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (vec_concat:X512 (match_operand:X256 1 "register_operand" "0")
                         (match_operand:X256 2 "register_operand" "x")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 0) 32)
        (match_dup 2))]
  ""
)

(define_insn_and_split "kvx_xcat1024"
  [(set (match_operand:X1024 0 "register_operand" "=x")
        (vec_concat:X1024 (match_operand:X512 1 "register_operand" "0")
                          (match_operand:X512 2 "register_operand" "x")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 0) 64)
        (subreg:X256 (match_dup 2) 0))
   (set (subreg:X256 (match_dup 0) 96)
        (subreg:X256 (match_dup 2) 32))]
  ""
)

(define_insn_and_split "kvx_xcat2048"
  [(set (match_operand:X2048 0 "register_operand" "=x")
        (vec_concat:X2048 (match_operand:X1024 1 "register_operand" "0")
                          (match_operand:X1024 2 "register_operand" "x")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 0) 128)
        (subreg:X256 (match_dup 2) 0))
   (set (subreg:X256 (match_dup 0) 160)
        (subreg:X256 (match_dup 2) 32))
   (set (subreg:X256 (match_dup 0) 192)
        (subreg:X256 (match_dup 2) 64))
   (set (subreg:X256 (match_dup 0) 224)
        (subreg:X256 (match_dup 2) 96))]
  ""
)

(define_insn_and_split "kvx_xcat4096"
  [(set (match_operand:X4096 0 "register_operand" "=x")
        (vec_concat:X4096 (match_operand:X2048 1 "register_operand" "0")
                          (match_operand:X2048 2 "register_operand" "x")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 0) 256)
        (subreg:X256 (match_dup 2) 0))
   (set (subreg:X256 (match_dup 0) 288)
        (subreg:X256 (match_dup 2) 32))
   (set (subreg:X256 (match_dup 0) 320)
        (subreg:X256 (match_dup 2) 64))
   (set (subreg:X256 (match_dup 0) 352)
        (subreg:X256 (match_dup 2) 96))
   (set (subreg:X256 (match_dup 0) 384)
        (subreg:X256 (match_dup 2) 128))
   (set (subreg:X256 (match_dup 0) 416)
        (subreg:X256 (match_dup 2) 160))
   (set (subreg:X256 (match_dup 0) 448)
        (subreg:X256 (match_dup 2) 192))
   (set (subreg:X256 (match_dup 0) 480)
        (subreg:X256 (match_dup 2) 224))]
  ""
)

(define_insn_and_split "kvx_xcat8192"
  [(set (match_operand:X8192 0 "register_operand" "=x")
        (vec_concat:X8192 (match_operand:X4096 1 "register_operand" "0")
                          (match_operand:X4096 2 "register_operand" "x")))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 0) 512)
        (subreg:X256 (match_dup 2) 0))
   (set (subreg:X256 (match_dup 0) 544)
        (subreg:X256 (match_dup 2) 32))
   (set (subreg:X256 (match_dup 0) 576)
        (subreg:X256 (match_dup 2) 64))
   (set (subreg:X256 (match_dup 0) 608)
        (subreg:X256 (match_dup 2) 96))
   (set (subreg:X256 (match_dup 0) 640)
        (subreg:X256 (match_dup 2) 128))
   (set (subreg:X256 (match_dup 0) 672)
        (subreg:X256 (match_dup 2) 160))
   (set (subreg:X256 (match_dup 0) 704)
        (subreg:X256 (match_dup 2) 192))
   (set (subreg:X256 (match_dup 0) 736)
        (subreg:X256 (match_dup 2) 224))
   (set (subreg:X256 (match_dup 0) 768)
        (subreg:X256 (match_dup 2) 256))
   (set (subreg:X256 (match_dup 0) 800)
        (subreg:X256 (match_dup 2) 288))
   (set (subreg:X256 (match_dup 0) 832)
        (subreg:X256 (match_dup 2) 320))
   (set (subreg:X256 (match_dup 0) 864)
        (subreg:X256 (match_dup 2) 352))
   (set (subreg:X256 (match_dup 0) 896)
        (subreg:X256 (match_dup 2) 384))
   (set (subreg:X256 (match_dup 0) 928)
        (subreg:X256 (match_dup 2) 416))
   (set (subreg:X256 (match_dup 0) 960)
        (subreg:X256 (match_dup 2) 448))
   (set (subreg:X256 (match_dup 0) 992)
        (subreg:X256 (match_dup 2) 480))]
  ""
)

(define_insn_and_split "kvx_xlow256"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (subreg:X256 (match_operand:X512 1 "register_operand" "x") 0))]
  ""
  "#"
  ""
  [(set (match_dup 0) (subreg:X256 (match_dup 1) 0))]
  ""
)

(define_insn_and_split "kvx_xlow512"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (subreg:X512 (match_operand:X1024 1 "register_operand" "x") 0))]
  ""
  "#"
  ""
  [(set (subreg:X256 (match_dup 0) 0) (subreg:X256 (match_dup 1) 0))
   (set (subreg:X256 (match_dup 0) 32) (subreg:X256 (match_dup 1) 32))]
  ""
)

(define_insn_and_split "kvx_xlow1024"
  [(set (match_operand:X1024 0 "register_operand" "=x")
        (subreg:X1024 (match_operand:X2048 1 "register_operand" "x") 0))]
  ""
  "#"
  ""
  [(set (subreg:X256 (match_dup 0) 0) (subreg:X256 (match_dup 1) 0))
   (set (subreg:X256 (match_dup 0) 32) (subreg:X256 (match_dup 1) 32))
   (set (subreg:X256 (match_dup 0) 64) (subreg:X256 (match_dup 1) 64))
   (set (subreg:X256 (match_dup 0) 96) (subreg:X256 (match_dup 1) 96))]
  ""
)

(define_insn_and_split "kvx_xlow2048"
  [(set (match_operand:X2048 0 "register_operand" "=x")
        (subreg:X2048 (match_operand:X4096 1 "register_operand" "x") 0))]
  ""
  "#"
  ""
  [(set (subreg:X256 (match_dup 0) 0) (subreg:X256 (match_dup 1) 0))
   (set (subreg:X256 (match_dup 0) 32) (subreg:X256 (match_dup 1) 32))
   (set (subreg:X256 (match_dup 0) 64) (subreg:X256 (match_dup 1) 64))
   (set (subreg:X256 (match_dup 0) 96) (subreg:X256 (match_dup 1) 96))
   (set (subreg:X256 (match_dup 0) 128) (subreg:X256 (match_dup 1) 128))
   (set (subreg:X256 (match_dup 0) 160) (subreg:X256 (match_dup 1) 160))
   (set (subreg:X256 (match_dup 0) 192) (subreg:X256 (match_dup 1) 192))
   (set (subreg:X256 (match_dup 0) 224) (subreg:X256 (match_dup 1) 224))]
  ""
)

(define_insn_and_split "kvx_xlow4096"
  [(set (match_operand:X4096 0 "register_operand" "=x")
        (subreg:X4096 (match_operand:X8192 1 "register_operand" "x") 0))]
  ""
  "#"
  ""
  [(set (subreg:X256 (match_dup 0) 0) (subreg:X256 (match_dup 1) 0))
   (set (subreg:X256 (match_dup 0) 32) (subreg:X256 (match_dup 1) 32))
   (set (subreg:X256 (match_dup 0) 64) (subreg:X256 (match_dup 1) 64))
   (set (subreg:X256 (match_dup 0) 96) (subreg:X256 (match_dup 1) 96))
   (set (subreg:X256 (match_dup 0) 128) (subreg:X256 (match_dup 1) 128))
   (set (subreg:X256 (match_dup 0) 160) (subreg:X256 (match_dup 1) 160))
   (set (subreg:X256 (match_dup 0) 192) (subreg:X256 (match_dup 1) 192))
   (set (subreg:X256 (match_dup 0) 224) (subreg:X256 (match_dup 1) 224))
   (set (subreg:X256 (match_dup 0) 256) (subreg:X256 (match_dup 1) 256))
   (set (subreg:X256 (match_dup 0) 288) (subreg:X256 (match_dup 1) 288))
   (set (subreg:X256 (match_dup 0) 320) (subreg:X256 (match_dup 1) 320))
   (set (subreg:X256 (match_dup 0) 352) (subreg:X256 (match_dup 1) 352))
   (set (subreg:X256 (match_dup 0) 384) (subreg:X256 (match_dup 1) 384))
   (set (subreg:X256 (match_dup 0) 416) (subreg:X256 (match_dup 1) 416))
   (set (subreg:X256 (match_dup 0) 448) (subreg:X256 (match_dup 1) 448))
   (set (subreg:X256 (match_dup 0) 480) (subreg:X256 (match_dup 1) 480))]
  ""
)

(define_insn_and_split "kvx_xhigh256"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (subreg:X256 (match_operand:X512 1 "register_operand" "x") 32))]
  ""
  "#"
  ""
  [(set (match_dup 0) (subreg:X256 (match_dup 1) 32))]
  ""
)

(define_insn_and_split "kvx_xhigh512"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (subreg:X512 (match_operand:X1024 1 "register_operand" "x") 64))]
  ""
  "#"
  ""
  [(set (subreg:X256 (match_dup 0) 0) (subreg:X256 (match_dup 1) 64))
   (set (subreg:X256 (match_dup 0) 32) (subreg:X256 (match_dup 1) 96))]
  ""
)

(define_insn_and_split "kvx_xhigh1024"
  [(set (match_operand:X1024 0 "register_operand" "=x")
        (subreg:X1024 (match_operand:X2048 1 "register_operand" "x") 128))]
  ""
  "#"
  ""
  [(set (subreg:X256 (match_dup 0) 0) (subreg:X256 (match_dup 1) 128))
   (set (subreg:X256 (match_dup 0) 32) (subreg:X256 (match_dup 1) 160))
   (set (subreg:X256 (match_dup 0) 64) (subreg:X256 (match_dup 1) 192))
   (set (subreg:X256 (match_dup 0) 96) (subreg:X256 (match_dup 1) 224))]
  ""
)

(define_insn_and_split "kvx_xhigh2048"
  [(set (match_operand:X2048 0 "register_operand" "=x")
        (subreg:X2048 (match_operand:X4096 1 "register_operand" "x") 256))]
  ""
  "#"
  ""
  [(set (subreg:X256 (match_dup 0) 0) (subreg:X256 (match_dup 1) 256))
   (set (subreg:X256 (match_dup 0) 32) (subreg:X256 (match_dup 1) 288))
   (set (subreg:X256 (match_dup 0) 64) (subreg:X256 (match_dup 1) 320))
   (set (subreg:X256 (match_dup 0) 96) (subreg:X256 (match_dup 1) 352))
   (set (subreg:X256 (match_dup 0) 128) (subreg:X256 (match_dup 1) 384))
   (set (subreg:X256 (match_dup 0) 160) (subreg:X256 (match_dup 1) 416))
   (set (subreg:X256 (match_dup 0) 192) (subreg:X256 (match_dup 1) 448))
   (set (subreg:X256 (match_dup 0) 224) (subreg:X256 (match_dup 1) 480))]
  ""
)

(define_insn_and_split "kvx_xhigh4096"
  [(set (match_operand:X4096 0 "register_operand" "=x")
        (subreg:X4096 (match_operand:X8192 1 "register_operand" "x") 512))]
  ""
  "#"
  ""
  [(set (subreg:X256 (match_dup 0) 0) (subreg:X256 (match_dup 1) 512))
   (set (subreg:X256 (match_dup 0) 32) (subreg:X256 (match_dup 1) 544))
   (set (subreg:X256 (match_dup 0) 64) (subreg:X256 (match_dup 1) 576))
   (set (subreg:X256 (match_dup 0) 96) (subreg:X256 (match_dup 1) 608))
   (set (subreg:X256 (match_dup 0) 128) (subreg:X256 (match_dup 1) 640))
   (set (subreg:X256 (match_dup 0) 160) (subreg:X256 (match_dup 1) 672))
   (set (subreg:X256 (match_dup 0) 192) (subreg:X256 (match_dup 1) 704))
   (set (subreg:X256 (match_dup 0) 224) (subreg:X256 (match_dup 1) 736))
   (set (subreg:X256 (match_dup 0) 256) (subreg:X256 (match_dup 1) 768))
   (set (subreg:X256 (match_dup 0) 288) (subreg:X256 (match_dup 1) 800))
   (set (subreg:X256 (match_dup 0) 320) (subreg:X256 (match_dup 1) 832))
   (set (subreg:X256 (match_dup 0) 352) (subreg:X256 (match_dup 1) 864))
   (set (subreg:X256 (match_dup 0) 384) (subreg:X256 (match_dup 1) 896))
   (set (subreg:X256 (match_dup 0) 416) (subreg:X256 (match_dup 1) 928))
   (set (subreg:X256 (match_dup 0) 448) (subreg:X256 (match_dup 1) 960))
   (set (subreg:X256 (match_dup 0) 480) (subreg:X256 (match_dup 1) 992))]
  ""
)


;; KVX_XLOAD256, KVX_XLOAD512, KVX_XLOAD1024

(define_insn "kvx_xload256"
  [(set (match_operand:X256 0 "register_operand" "=x,x,x")
        (unspec:X256 [(match_operand:X256 1 "memory_operand" "a,b,m")
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
  [(set (match_operand:X512 0 "register_operand" "=x,x,x")
        (unspec:X512 [(match_operand:X512 1 "memory_operand" "a,b,m")
                        (match_operand 2 "" "")] UNSPEC_XLOAD512))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 0) 0)
        (unspec:X256 [(subreg:X256 (match_dup 1) 0)
                        (match_dup 2)] UNSPEC_XLOAD256))
   (set (subreg:X256 (match_dup 0) 32)
        (unspec:X256 [(subreg:X256 (match_dup 1) 32)
                        (match_dup 2)] UNSPEC_XLOAD256))]
  ""
)

(define_insn_and_split "kvx_xload1024"
  [(set (match_operand:X1024 0 "register_operand" "=x,x,x")
        (unspec:X1024 [(match_operand:X1024 1 "memory_operand" "a,b,m")
                         (match_operand 2 "" "")] UNSPEC_XLOAD1024))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 0) 0)
        (unspec:X256 [(subreg:X256 (match_dup 1) 0)
                        (match_dup 2)] UNSPEC_XLOAD256))
   (set (subreg:X256 (match_dup 0) 32)
        (unspec:X256 [(subreg:X256 (match_dup 1) 32)
                        (match_dup 2)] UNSPEC_XLOAD256))
   (set (subreg:X256 (match_dup 0) 64)
        (unspec:X256 [(subreg:X256 (match_dup 1) 64)
                        (match_dup 2)] UNSPEC_XLOAD256))
   (set (subreg:X256 (match_dup 0) 96)
        (unspec:X256 [(subreg:X256 (match_dup 1) 96)
                        (match_dup 2)] UNSPEC_XLOAD256))]
  ""
)


;; KVX_XLOADS512, KVX_XLOADSC512

(define_insn "kvx_xloads512"
  [(set (match_operand:X512 0 "register_operand" "=x,x,x")
        (unspec:X512 [(match_operand:X512 1 "register_operand" "0,0,0")
                        (match_operand:X256 2 "memory_operand" "a,b,m")
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
  [(set (match_operand:X512 0 "register_operand" "=x,x,x")
        (unspec:X512 [(match_operand:X512 1 "register_operand" "0,0,0")
                        (match_operand:X256 2 "memfoiled_operand" "c,d,e")
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
  [(set (match_operand:X1024 0 "register_operand" "=x,x,x")
        (unspec:X1024 [(match_operand:X1024 1 "register_operand" "0,0,0")
                         (match_operand:X256 2 "memory_operand" "a,b,m")
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
  [(set (match_operand:X1024 0 "register_operand" "=x,x,x")
        (unspec:X1024 [(match_operand:X1024 1 "register_operand" "0,0,0")
                         (match_operand:X256 2 "memfoiled_operand" "c,d,e")
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
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD256))]
  ""
  "xmovetq %0.lo = %1, %1\n\txmovetq %0.hi = %1, %1";
  [(set_attr "type" "alu_lite_x2_crwl_crwh")
   (set_attr "length"                   "8")]
)

(define_insn_and_split "kvx_xsplatd512"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD256))]
  ""
  "#"
  ""
  [(set (subreg:X256 (match_dup 0) 0)
        (unspec:X256 [(match_dup 1)] UNSPEC_XSPLATD256))
   (set (subreg:X256 (match_dup 0) 32)
        (subreg:X256 (match_dup 0) 0))]
  ""
)

(define_insn_and_split "kvx_xsplatd1024"
  [(set (match_operand:X1024 0 "register_operand" "=x")
        (unspec [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD256))]
  ""
  "#"
  ""
  [(set (subreg:X256 (match_dup 0) 0)
        (unspec:X256 [(match_dup 1)] UNSPEC_XSPLATD256))
   (set (subreg:X256 (match_dup 0) 32)
        (subreg:X256 (match_dup 0) 0))
   (set (subreg:X256 (match_dup 0) 64)
        (subreg:X256 (match_dup 0) 0))
   (set (subreg:X256 (match_dup 0) 96)
        (subreg:X256 (match_dup 0) 0))]
  ""
)

(define_insn_and_split "kvx_xsplatd2048"
  [(set (match_operand:X2048 0 "register_operand" "=x")
        (unspec [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD256))]
  ""
  "#"
  ""
  [(set (subreg:X256 (match_dup 0) 0)
        (unspec:X256 [(match_dup 1)] UNSPEC_XSPLATD256))
   (set (subreg:X256 (match_dup 0) 32)
        (subreg:X256 (match_dup 0) 0))
   (set (subreg:X256 (match_dup 0) 64)
        (subreg:X256 (match_dup 0) 0))
   (set (subreg:X256 (match_dup 0) 96)
        (subreg:X256 (match_dup 0) 0))
   (set (subreg:X1024 (match_dup 0) 128)
        (unspec:X1024 [(subreg:X1024 (match_dup 0) 0)] UNSPEC_XMT44D))]
  ""
)

(define_insn_and_split "kvx_xsplatd4096"
  [(set (match_operand:X4096 0 "register_operand" "=x")
        (unspec [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD256))]
  ""
  "#"
  ""
  [(set (subreg:X256 (match_dup 0) 0)
        (unspec:X256 [(match_dup 1)] UNSPEC_XSPLATD256))
   (set (subreg:X256 (match_dup 0) 32)
        (subreg:X256 (match_dup 0) 0))
   (set (subreg:X256 (match_dup 0) 64)
        (subreg:X256 (match_dup 0) 0))
   (set (subreg:X256 (match_dup 0) 96)
        (subreg:X256 (match_dup 0) 0))
   (set (subreg:X1024 (match_dup 0) 128)
        (unspec:X1024 [(subreg:X1024 (match_dup 0) 0)] UNSPEC_XMT44D))
   (set (subreg:X1024 (match_dup 0) 256)
        (unspec:X1024 [(subreg:X1024 (match_dup 0) 128)] UNSPEC_XMT44D))
   (set (subreg:X1024 (match_dup 0) 384)
        (unspec:X1024 [(subreg:X1024 (match_dup 0) 256)] UNSPEC_XMT44D))]
  ""
)

(define_insn_and_split "kvx_xsplatd8192"
  [(set (match_operand:X8192 0 "register_operand" "=x")
        (unspec [(match_operand:DI 1 "register_operand" "r")] UNSPEC_XSPLATD256))]
  ""
  "#"
  ""
  [(set (subreg:X256 (match_dup 0) 0)
        (unspec:X256 [(match_dup 1)] UNSPEC_XSPLATD256))
   (set (subreg:X256 (match_dup 0) 32)
        (subreg:X256 (match_dup 0) 0))
   (set (subreg:X256 (match_dup 0) 64)
        (subreg:X256 (match_dup 0) 0))
   (set (subreg:X256 (match_dup 0) 96)
        (subreg:X256 (match_dup 0) 0))
   (set (subreg:X1024 (match_dup 0) 128)
        (unspec:X1024 [(subreg:X1024 (match_dup 0) 0)] UNSPEC_XMT44D))
   (set (subreg:X1024 (match_dup 0) 256)
        (unspec:X1024 [(subreg:X1024 (match_dup 0) 128)] UNSPEC_XMT44D))
   (set (subreg:X1024 (match_dup 0) 384)
        (unspec:X1024 [(subreg:X1024 (match_dup 0) 256)] UNSPEC_XMT44D))
   (set (subreg:X1024 (match_dup 0) 512)
        (unspec:X1024 [(subreg:X1024 (match_dup 0) 384)] UNSPEC_XMT44D))
   (set (subreg:X1024 (match_dup 0) 640)
        (unspec:X1024 [(subreg:X1024 (match_dup 0) 512)] UNSPEC_XMT44D))
   (set (subreg:X1024 (match_dup 0) 768)
        (unspec:X1024 [(subreg:X1024 (match_dup 0) 640)] UNSPEC_XMT44D))
   (set (subreg:X1024 (match_dup 0) 896)
        (unspec:X1024 [(subreg:X1024 (match_dup 0) 768)] UNSPEC_XMT44D))]

  ""
)


;; KVX_XLOADC256, KVX_XLOADC512, KVX_XLOADC1024

(define_insn "kvx_xloadc256"
  [(set (match_operand:X256 0 "register_operand" "=x,x,x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "0,0,0")
                        (match_operand:X256 2 "memfoiled_operand" "c,d,e")
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
  [(match_operand:X512 0 "register_operand" "")
   (match_operand:X512 1 "register_operand" "")
   (match_operand:X512 2 "memfoiled_operand" "")
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
  [(match_operand:X1024 0 "register_operand" "")
   (match_operand:X1024 1 "register_operand" "")
   (match_operand:X1024 2 "memfoiled_operand" "")
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
  [(set (match_operand:X256 1 "memory_operand"  "=a,b,m")
        (unspec:X256 [(match_operand:X256 0 "register_operand" "x,x,x")] UNSPEC_XSTORE256))]
  ""
  "xso%X1 %1 = %0"
  [(set_attr "type" "lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_insn_and_split "kvx_xstore512"
  [(set (match_operand:X512 1 "memory_operand"  "=a,b,m")
        (unspec:X512 [(match_operand:X512 0 "register_operand" "x,x,x")] UNSPEC_XSTORE512))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 1) 0)
        (unspec:X256 [(subreg:X256 (match_dup 0) 0)] UNSPEC_XSTORE256))
   (set (subreg:X256 (match_dup 1) 32)
        (unspec:X256 [(subreg:X256 (match_dup 0) 32)] UNSPEC_XSTORE256))]
  ""
)

(define_insn_and_split "kvx_xstore1024"
  [(set (match_operand:X1024 1 "memory_operand"  "=a,b,m")
        (unspec:X1024 [(match_operand:X1024 0 "register_operand" "x,x,x")] UNSPEC_XSTORE1024))]
  ""
  "#"
  "reload_completed"
  [(set (subreg:X256 (match_dup 1) 0)
        (unspec:X256 [(subreg:X256 (match_dup 0) 0)] UNSPEC_XSTORE256))
   (set (subreg:X256 (match_dup 1) 32)
        (unspec:X256 [(subreg:X256 (match_dup 0) 32)] UNSPEC_XSTORE256))
   (set (subreg:X256 (match_dup 1) 64)
        (unspec:X256 [(subreg:X256 (match_dup 0) 64)] UNSPEC_XSTORE256))
   (set (subreg:X256 (match_dup 1) 96)
        (unspec:X256 [(subreg:X256 (match_dup 0) 96)] UNSPEC_XSTORE256))]
  ""
)


;; KVX_XSTOREC256, KVX_XSTOREC512, KVX_XSTOREC1024

(define_insn "kvx_xstorec256"
  [(set (match_operand:X256 1 "memfoiled_operand"  "=c,d,e")
        (unspec:X256 [(match_operand:X256 0 "register_operand" "x,x,x")
                        (match_operand:DI 2 "register_operand" "r,r,r")
                        (match_operand 3 "" "")] UNSPEC_XSTORE256))
   (clobber (match_dup 1))]
  ""
  "xso%3%X1 %2? %O1 = %0"
  [(set_attr "type" "lsu_crrp_store,lsu_crrp_store_x,lsu_crrp_store_y")
   (set_attr "length"            "4,               8,              12")]
)

(define_expand "kvx_xstorec512"
  [(match_operand:X512 0 "register_operand" "")
   (match_operand:X512 1 "memfoiled_operand" "")
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
  [(match_operand:X1024 0 "register_operand" "")
   (match_operand:X1024 1 "memfoiled_operand" "")
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

(define_insn "kvx_xpreload<XBUFF:bitsize>"
  [(set (match_operand:XBUFF 0 "register_operand" "=x,x,x")
        (unspec:XBUFF [(match_operand:XBUFF 1 "register_operand" "0,0,0")
                         (match_operand:X256 2 "memfoiled_operand" "c,d,e")
                         (match_operand:DI 3 "register_operand" "r,r,r")
                         (match_operand 4 "" "")] UNSPEC_XPRELOAD))]
  "KV3_2"
  "xlo%4%X2 %b0, %3 = %O2"
  [(set_attr "type" "lsu,lsu_x,lsu_y")
   (set_attr "length" "4,   8,   12")]
)


;; KVX_XALIGN*O, KVX_XACCESS*O

(define_insn "kvx_xalign<XBUFF:bitsize>o"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:XBUFF 1 "register_operand" "x")
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


;; KVX_XFSCALEWO

(define_insn "kvx_xfscalewo"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:DI 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_XFSCALEWO))]
  "KV3_2"
  "xfscalewo%3 %0 = %1, %2"
  [(set_attr "type" "bcu_crrp_crwl_crwh")]
)


;; KVX_XMMA484BW

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
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmau484bw_1"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "x")] UNSPEC_XMMAU484BW))]
  "KV3_1"
  "xmma484ubw %0 = %3, %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmasu484bw_1"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "x")] UNSPEC_XMMASU484BW))]
  "KV3_1"
  "xmma484subw %0 = %3, %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmaus484bw_1"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "x")] UNSPEC_XMMAUS484BW))]
  "KV3_1"
  "xmma484usbw %0 = %3, %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmma484bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMA484BW))]
  "KV3_2"
  "xmma484bw %0 = %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmau484bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMAU484BW))]
  "KV3_2"
  "xmmau484bw %0 = %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmasu484bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMASU484BW))]
  "KV3_2"
  "xmmasu484bw %0 = %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmaus484bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMAUS484BW))]
  "KV3_2"
  "xmmaus484bw %0 = %1, %2"
  [(set_attr "type" "tca")]
)


;; KVX_XMMA4164BW

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
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmau4164bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X512 1 "register_operand" "x")
                      (match_operand:X512 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMAU4164BW))]
  "KV3_2"
  "xmmau4164bw %0 = %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmasu4164bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X512 1 "register_operand" "x")
                      (match_operand:X512 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMASU4164BW))]
  "KV3_2"
  "xmmasu4164bw %0 = %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmmaus4164bw_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X512 1 "register_operand" "x")
                      (match_operand:X512 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMMAUS4164BW))]
  "KV3_2"
  "xmmaus4164bw %0 = %1, %2"
  [(set_attr "type" "tca")]
)


;; KVX_XMADD44BW0, KVX_XMADD44BW1

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
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmaddu44bw0_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMADDU44BW0))]
  "KV3_2"
  "xmaddu44bw0 %0 = %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmaddsu44bw0_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMADDSU44BW0))]
  "KV3_2"
  "xmaddsu44bw0 %0 = %1, %2"
  [(set_attr "type" "tca")]
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
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmaddu44bw1_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMADDU44BW1))]
  "KV3_2"
  "xmaddu44bw1 %0 = %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmaddsu44bw1_2"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")] UNSPEC_XMADDSU44BW1))]
  "KV3_2"
  "xmaddsu44bw1 %0 = %1, %2"
  [(set_attr "type" "tca")]
)


;; KVX_XMADDIFWO, KVX_XMSBFIFWO, KVX_XFFMA44HW

(define_insn "kvx_xmaddifwo"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X256 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_XMADDIFWO))]
  "KV3_2"
  "xmaddifwo%4 %0 = %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xmsbfifwo"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X256 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_XMSBFIFWO))]
  "KV3_2"
  "xmsbfifwo%4 %0 = %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xffma44hw"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_XFFMA44HW))]
  "KV3_2"
  "xffma44hw%4 %0 = %1, %2"
  [(set_attr "type" "tca")]
)


;; KVX_XFMMA444HW, KVX_XFMMA484HW

(define_insn "kvx_xfmma444hw"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_XFMMA444HW))]
  "KV3_2"
  "xfmma444hw%4 %0 = %1, %2"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xfmma484hw"
  [(set (match_operand:X512 0 "register_operand" "=x")
        (unspec:X512 [(match_operand:X512 1 "register_operand" "x")
                      (match_operand:X512 2 "register_operand" "x")
                      (match_operand:X512 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_XFMMA484HW))]
  "KV3_2"
  "xfmma484hw%4 %0 = %1, %2"
  [(set_attr "type" "tca")]
)


;; KVX_XFNARROW44WH, KVX_XCLAMPWO

(define_insn "kvx_xfnarrow44wh"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X512 1 "register_operand" "x")
                      (match_operand 2 "" "")] UNSPEC_XFNARROW44WH))]
  "KV3_2"
  "xfnarrow44wh%2 %0 = %1"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xclampwo"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X256 1 "register_operand" "x")
                      (match_operand:X256 2 "register_operand" "x")] UNSPEC_XCLAMPWO))]
  "KV3_2"
  "xclampwo %0 = %1, %2"
  [(set_attr "type" "tca")]
)


;; KVX_XTRUNC48WB, KVX_XSX48BW, KVX_XZX48BW

(define_insn "kvx_xtrunc48wb"
  [(set (match_operand:X256 0 "register_operand" "=x")
        (unspec:X256 [(match_operand:X1024 1 "register_operand" "x")] UNSPEC_XTRUNC48WB))]
  "KV3_2"
  "xtrunc48wb %0 = %1"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xsx48bw"
  [(set (match_operand:X1024 0 "register_operand" "=x")
        (unspec:X1024 [(match_operand:X256 1 "register_operand" "x")] UNSPEC_XSX48BW))]
  "KV3_2"
  "xsx48bw %0 = %1"
  [(set_attr "type" "tca")]
)

(define_insn "kvx_xzx48bw"
  [(set (match_operand:X1024 0 "register_operand" "=x")
        (unspec:X1024 [(match_operand:X256 1 "register_operand" "x")] UNSPEC_XZX48BW))]
  "KV3_2"
  "xzx48bw %0 = %1"
  [(set_attr "type" "tca")]
)


;; KVX_XSWAP256

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


;; KVX_XMT44D

(define_insn "kvx_xmt44d"
  [(set (match_operand:X1024 0 "register_operand" "=x")
        (unspec:X1024 [(match_operand:X1024 1 "register_operand" "x")] UNSPEC_XMT44D))]
  ""
  "xmt44d %0 = %1"
  [(set_attr "type" "tca")]
)

