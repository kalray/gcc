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

(define_insn "*mov<mode>_real"
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
  [(set (match_operand:SIMD128 0 "nonimmediate_operand" "")
        (match_operand:SIMD128 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (<MODE>mode, operands[1]);
    }
  }
)

(define_insn_and_split "*mov<mode>_oddreg"
  [(set (match_operand:SIMD128 0 "nonimmediate_operand" "")
        (match_operand:SIMD128 1 "general_operand"      ""))]
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

(define_insn "*mov<mode>_real"
  [(set (match_operand:SIMD128 0 "kvx_nonimmediate_operand_pair" "=r, r, r, r, r, r, r,a,m")
        (match_operand:SIMD128 1 "kvx_nonimmediate_operand_pair" " r,Ca,Cb,Cm,Za,Zb,Zm,r,r"))]
  "(!immediate_operand(operands[1], <MODE>mode) || !memory_operand(operands[0], <MODE>mode))"
  {
    switch (which_alternative) {
    case 0:
      return kvx_asm_pat_copyq (operands[1]);
    case 1: case 2: case 3: case 4: case 5: case 6:
      return "lq%C1%m1 %0 = %1";
    case 7: case 8:
      return "sq%m0 %0 = %1";
    default:
      gcc_unreachable ();
    }
  }
  [(set_attr "type"    "mau, lsu_auxw_load, lsu_auxw_load_x, lsu_auxw_load_y, lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxw_load_uncached_y, lsu_auxr_store, lsu_auxr_store_x")
   (set_attr "length"  "  4,              4,              8,              12,                      4,                        8,                       12,              4,                8")]
)

;; Split what would end-up in a copyq in 2 copyd.
;; copyd uses 1 TINY each instead of the MAU used by copyq
;; at the cost of an extra word on insn
(define_split
  [(set (match_operand:SIMD128 0 "register_operand" "")
         (match_operand:SIMD128 1 "register_operand" ""))]
  "!optimize_size && reload_completed"
  [(const_int 0)]
  {
    kvx_split_128bits_move (operands[0], operands[1], <MODE>mode);
    DONE;
  }
)

(define_insn_and_split "*mov<mode>_immediate"
    [(set (match_operand:SIMD128 0 "register_operand" "")
          (match_operand:SIMD128 1 "immediate_operand" "" ))]
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
  [(set (match_operand:SIMD256 0 "nonimmediate_operand" "")
        (match_operand:SIMD256 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (<MODE>mode, operands[1]);
    }
  }
)

(define_insn_and_split "*mov<mode>_misalign_reg"
  [(set (match_operand:SIMD256 0 "nonimmediate_operand" "")
        (match_operand:SIMD256 1 "general_operand"      ""))]
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

(define_insn "*mov<mode>_real"
  [(set (match_operand:SIMD256 0 "kvx_nonimmediate_operand_quad" "=r, r, r, r, r, r, r,a,b,m")
        (match_operand:SIMD256 1 "kvx_nonimmediate_operand_quad"  "r,Ca,Cb,Cm,Za,Zb,Zm,r,r,r"))]
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
  [(set_attr "type"    "lsu_auxr_auxw,lsu_auxw_load,lsu_auxw_load_x,lsu_auxw_load_y,lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y,lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length"  "         4,            4,              8,             12,                     4,                       8,                      12,             4,               8,              12")]
)

;; Split what would end-up in a copyo in 4 copyd.
;; copyd uses 1 TINY each instead of the LSU used by copyo
;; at the cost of 3 extra word on insn
(define_split
  [(set (match_operand:SIMD256 0 "register_operand" "")
         (match_operand:SIMD256 1 "register_operand" ""))]
  "!optimize_size && reload_completed"
  [(const_int 0)]
  {
    kvx_split_256bits_move (operands[0], operands[1], <MODE>mode);
    DONE;
  }
)

(define_insn_and_split "*mov<mode>_immediate"
    [(set (match_operand:SIMD256 0 "register_operand" "")
          (match_operand:SIMD256 1 "immediate_operand" "" ))]
  ""
  "#"
  "&& reload_completed"
  [(const_int 0)]
  {
   kvx_split_256bits_move (operands[0], operands[1], <MODE>mode);
   DONE;
  }
)

;; V4HI

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
  [(set_attr "type" "alu_tiny_x")
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

(define_insn "*addx2wq"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                              (const_int 2))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx2hq %x0 = %x1, %x2\n\taddx2hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx2wq2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (ashift:V8HI (match_operand:V8HI 1 "register_operand" "r")
                                (const_int 1))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx2hq %x0 = %x1, %x2\n\taddx2hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4wq"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                              (const_int 4))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx4hq %x0 = %x1, %x2\n\taddx4hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4wq2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (ashift:V8HI (match_operand:V8HI 1 "register_operand" "r")
                                (const_int 2))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx4hq %x0 = %x1, %x2\n\taddx4hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8wq"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                              (const_int 8))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx8hq %x0 = %x1, %x2\n\taddx8hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8wq2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (ashift:V8HI (match_operand:V8HI 1 "register_operand" "r")
                                (const_int 3))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx8hq %x0 = %x1, %x2\n\taddx8hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16wq"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                              (const_int 16))
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "addx16hq %x0 = %x1, %x2\n\taddx16hq %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16wq2"
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

(define_insn "*sbfx2wq"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (mult:V8HI (match_operand:V8HI 2 "register_operand" "r")
                               (const_int 2))))]
  ""
  "sbfx2hq %x0 = %x2, %x1\n\tsbfx2hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx2wq2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (ashift:V8HI (match_operand:V8HI 2 "register_operand" "r")
                                 (const_int 1))))]
  ""
  "sbfx2hq %x0 = %x2, %x1\n\tsbfx2hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4wq"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (mult:V8HI (match_operand:V8HI 2 "register_operand" "r")
                               (const_int 4))))]
  ""
  "sbfx4hq %x0 = %x2, %x1\n\tsbfx4hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4wq2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (ashift:V8HI (match_operand:V8HI 2 "register_operand" "r")
                                 (const_int 2))))]
  ""
  "sbfx4hq %x0 = %x2, %x1\n\tsbfx4hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8wq"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (mult:V8HI (match_operand:V8HI 2 "register_operand" "r")
                               (const_int 8))))]
  ""
  "sbfx8hq %x0 = %x2, %x1\n\tsbfx8hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8wq2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (ashift:V8HI (match_operand:V8HI 2 "register_operand" "r")
                                 (const_int 3))))]
  ""
  "sbfx8hq %x0 = %x2, %x1\n\tsbfx8hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16wq"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (mult:V8HI (match_operand:V8HI 2 "register_operand" "r")
                               (const_int 16))))]
  ""
  "sbfx16hq %x0 = %x2, %x1\n\tsbfx16hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16wq2"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 1 "nonmemory_operand" "r")
                    (ashift:V8HI (match_operand:V8HI 2 "register_operand" "r")
                                 (const_int 4))))]
  ""
  "sbfx16hq %x0 = %x2, %x1\n\tsbfx16hq %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "mulv8hi3"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                   (match_operand:V8HI 2 "nonmemory_operand" "r")))]
  ""
  "mulhq %x0 = %x1, %x2\n\t;;\n\tmulhq %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr")
   (set_attr "length"      "8")]
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

(define_insn "maddv8hiv8hi4"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (plus:V8HI (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                              (match_operand:V8HI 2 "nonmemory_operand" "r"))
                   (match_operand:V8HI 3 "register_operand" "0")))]
  ""
  "maddhq %x0 = %x1, %x2\n\t;;\n\tmaddhq %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr")
   (set_attr "length"      "8")]
)

(define_insn "msubv8hiv8hi4"
  [(set (match_operand:V8HI 0 "register_operand" "=r")
        (minus:V8HI (match_operand:V8HI 3 "register_operand" "0")
                    (mult:V8HI (match_operand:V8HI 1 "register_operand" "r")
                               (match_operand:V8HI 2 "register_operand" "r"))))]
  ""
  "msbfhq %x0 = %x1, %x2\n\t;;\n\tmsbfhq %y0 = %y1, %y2"
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
  [(set_attr "type" "alu_tiny_x2_x")
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


;; V2SI

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

(define_insn "*addx2wp"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                              (const_int 2))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx2wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx2wp2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (ashift:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                                (const_int 1))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx2wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx4wp"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                              (const_int 4))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx4wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx4wp2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (ashift:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                                (const_int 2))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx4wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx8wp"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                              (const_int 8))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx8wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx8wp2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (ashift:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                                (const_int 3))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx8wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx16wp"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (plus:V2SI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r,r,r")
                              (const_int 16))
                   (match_operand:V2SI 2 "nonmemory_operand" "r,v32,vx2")))]
  ""
  "addx16wp %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*addx16wp2"
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

(define_insn "*sbfx2wp"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                               (const_int 2))))]
  ""
  "sbfx2wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx2wp2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                                 (const_int 1))))]
  ""
  "sbfx2wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx4wp"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                               (const_int 4))))]
  ""
  "sbfx4wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx4wp2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                                 (const_int 2))))]
  ""
  "sbfx4wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx8wp"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                               (const_int 8))))]
  ""
  "sbfx8wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx8wp2"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (ashift:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                                 (const_int 3))))]
  ""
  "sbfx8wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx16wp"
  [(set (match_operand:V2SI 0 "register_operand" "=r,r,r")
        (minus:V2SI (match_operand:V2SI 1 "nonmemory_operand" "r,v32,vx2")
                    (mult:V2SI (match_operand:V2SI 2 "register_operand" "r,r,r")
                               (const_int 16))))]
  ""
  "sbfx16wp %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite_x,alu_lite_x")
   (set_attr "length" "     4,         8,         8")]
)

(define_insn "*sbfx16wp2"
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
  [(set_attr "type" "alu_tiny_x")
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

(define_insn "*addx2wq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                              (const_int 2))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx2wp %x0 = %x1, %x2\n\taddx2wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx2wq2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (ashift:V4SI (match_operand:V4SI 1 "register_operand" "r")
                                (const_int 1))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx2wp %x0 = %x1, %x2\n\taddx2wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4wq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                              (const_int 4))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx4wp %x0 = %x1, %x2\n\taddx4wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx4wq2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (ashift:V4SI (match_operand:V4SI 1 "register_operand" "r")
                                (const_int 2))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx4wp %x0 = %x1, %x2\n\taddx4wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8wq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                              (const_int 8))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx8wp %x0 = %x1, %x2\n\taddx8wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx8wq2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (ashift:V4SI (match_operand:V4SI 1 "register_operand" "r")
                                (const_int 3))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx8wp %x0 = %x1, %x2\n\taddx8wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16wq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                              (const_int 16))
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "addx16wp %x0 = %x1, %x2\n\taddx16wp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*addx16wq2"
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

(define_insn "*sbfx2wq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (mult:V4SI (match_operand:V4SI 2 "register_operand" "r")
                               (const_int 2))))]
  ""
  "sbfx2wp %x0 = %x2, %x1\n\tsbfx2wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx2wq2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (ashift:V4SI (match_operand:V4SI 2 "register_operand" "r")
                                 (const_int 1))))]
  ""
  "sbfx2wp %x0 = %x2, %x1\n\tsbfx2wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4wq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (mult:V4SI (match_operand:V4SI 2 "register_operand" "r")
                               (const_int 4))))]
  ""
  "sbfx4wp %x0 = %x2, %x1\n\tsbfx4wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx4wq2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (ashift:V4SI (match_operand:V4SI 2 "register_operand" "r")
                                 (const_int 2))))]
  ""
  "sbfx4wp %x0 = %x2, %x1\n\tsbfx4wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8wq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (mult:V4SI (match_operand:V4SI 2 "register_operand" "r")
                               (const_int 8))))]
  ""
  "sbfx8wp %x0 = %x2, %x1\n\tsbfx8wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx8wq2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (ashift:V4SI (match_operand:V4SI 2 "register_operand" "r")
                                 (const_int 3))))]
  ""
  "sbfx8wp %x0 = %x2, %x1\n\tsbfx8wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16wq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 1 "nonmemory_operand" "r")
                    (mult:V4SI (match_operand:V4SI 2 "register_operand" "r")
                               (const_int 16))))]
  ""
  "sbfx16wp %x0 = %x2, %x1\n\tsbfx16wp %y0 = %y2, %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "*sbfx16wq2"
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

(define_insn "maddv4siv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (plus:V4SI (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                              (match_operand:V4SI 2 "nonmemory_operand" "r"))
                   (match_operand:V4SI 3 "register_operand" "0")))]
  ""
  "maddwp %x0 = %x1, %x2\n\t;;\n\tmaddwp %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr")
   (set_attr "length"      "8")]
)

(define_insn "msubv4siv4si4"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (minus:V4SI (match_operand:V4SI 3 "register_operand" "0")
                    (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                               (match_operand:V4SI 2 "register_operand" "r"))))]
  ""
  "msbfwp %x0 = %x1, %x2\n\t;;\n\tmsbfwp %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr")
   (set_attr "length"      "8")]
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
  [(set_attr "type" "alu_tiny_x2_x")
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


;; V2DI


;; V4DI


;; V2SF

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

(define_insn "kvx_fminwp"
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

(define_insn "kvx_fmaxwp"
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

(define_insn "kvx_fnegwp"
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

(define_insn "kvx_fabswp"
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

(define_insn "kvx_fmulwcp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWCP))]
  ""
  "fmulwc%3 %x0 = %x1, %x2\n\t;;\n\tfmulwc%3 %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_insn "kvx_fmulcwcp"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULCWCP))]
  ""
  "fmulcwc%3 %x0 = %x1, %x2\n\t;;\n\tfmulcwc%3 %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_insn "fmav4sf4"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (fma:V4SF (match_operand:V4SF 1 "register_operand" "r")
                  (match_operand:V4SF 2 "register_operand" "r")
                  (match_operand:V4SF 3 "register_operand" "0")))]
  ""
  "ffmawp %x0 = %x1, %x2\n\t;;\n\tffmawp %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_insn "kvx_ffmawq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMAWQ))]
  ""
  "ffmawp%4 %x0 = %x1, %x2\n\t;;\n\tffmawp%4 %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_insn "fnmav4sf4"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (fma:V4SF (neg:V4SF (match_operand:V4SF 1 "register_operand" "r"))
                  (match_operand:V4SF 2 "register_operand" "r")
                  (match_operand:V4SF 3 "register_operand" "0")))]
  ""
  "ffmswp %x0 = %x1, %x2\n\t;;\n\tffmswp %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_insn "kvx_ffmswq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand:V4SF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSWQ))]
  ""
  "ffmswp%4 %x0 = %x1, %x2\n\t;;\n\tffmswp%4 %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_expand "fminv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (smin:V4SF (match_operand:V4SF 1 "register_operand" "")
                 (match_operand:V4SF 2 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_fminwq (operands[0], operands[1], operands[2]));
    DONE;
  }
)

(define_insn "kvx_fminwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (smin:V4SF (match_operand:V4SF 1 "register_operand" "r")
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "fminwp %x0 = %x1, %x2\n\tfminwp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "fmaxv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (smax:V4SF (match_operand:V4SF 1 "register_operand" "")
                   (match_operand:V4SF 2 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_fmaxwq (operands[0], operands[1], operands[2]));
    DONE;
  }
)

(define_insn "kvx_fmaxwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (smax:V4SF (match_operand:V4SF 1 "register_operand" "r")
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "fmaxwp %x0 = %x1, %x2\n\tfmaxwp %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "negv4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (neg:V4SF (match_operand:V4SF 1 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_fnegwq (operands[0], operands[1]));
    DONE;
  }
)

(define_insn "kvx_fnegwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (neg:V4SF (match_operand:V4SF 1 "register_operand" "r")))]
  ""
  "fnegwp %x0 = %x1\n\tfnegwp %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "absv4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (abs:V4SF (match_operand:V4SF 1 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_fabswq (operands[0], operands[1]));
    DONE;
  }
)

(define_insn "kvx_fabswq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (abs:V4SF (match_operand:V4SF 1 "register_operand" "r")))]
  ""
  "fabswp %x0 = %x1\n\tfabswp %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "floatv4siv4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (float:V4SF (match_operand:V4SI 1 "register_operand" "r")))]
  ""
  "floatwp.rn %x0 = %x1, 0\n\t;;\n\tfloatwp.rn %y0 = %y1, 0"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "kvx_floatwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATWQ))]
  ""
  "floatwp%3 %x0 = %x1, %2\n\t;;\n\tfloatwp%3 %y0 = %y1, %2"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "floatunsv4siv4sf2"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unsigned_float:V4SF (match_operand:V4SI 1 "register_operand" "r")))]
  ""
  "floatuwp.rn %x0 = %x1, 0\n\t;;\n\tfloatuwp.rn %y0 = %y1, 0"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "kvx_floatuwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATUWQ))]
  ""
  "floatuwp%3 %x0 = %x1, %2\n\t;;\n\tfloatuwp%3 %y0 = %y1, %2"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "fix_truncv4sfv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (fix:V4SI (match_operand:V4SF 1 "register_operand" "r")))]
  ""
  "fixedwp.rz %x0 = %x1, 0\n\t;;\n\tfixedwp.rz %y0 = %y1, 0"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "kvx_fixedwq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unspec:V4SI [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDWQ))]
  ""
  "fixedwp%3 %x0 = %x1, %2\n\t;;\n\tfixedwp%3 %y0 = %y1, %2"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "fixuns_truncv4sfv4si2"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unsigned_fix:V4SI (match_operand:V4SF 1 "register_operand" "r")))]
  ""
  "fixeduwp.rz %x0 = %x1, 0\n\t;;\n\tfixeduwp.rz %y0 = %y1, 0"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "kvx_fixeduwq"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (unspec:V4SI [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDUWQ))]
  ""
  "fixeduwp%3 %x0 = %x1, %2\n\t;;\n\tfixeduwp%3 %y0 = %y1, %2"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)


;; V2DF

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

;; (define_insn "mulv2df3"
;;   [(set (match_operand:V2DF 0 "register_operand" "=r")
;;         (mult:V2DF (match_operand:V2DF 1 "register_operand" "r")
;;                    (match_operand:V2DF 2 "register_operand" "r")))]
;;   ""
;;   "FMULDP %0 = %1, %2"
;;   [(set_attr "type" "mau_fpu")]
;; )

(define_insn "mulv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (mult:V2DF (match_operand:V2DF 1 "register_operand" "r")
                   (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "fmuld %x0 = %x1, %x2\n\t;;\n\tfmuld %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_insn "kvx_fmuldp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULDP))]
  ""
  "fmuld%3 %x0 = %x1, %x2\n\t;;\n\tfmuld%3 %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_expand "kvx_fmuldc"
  [(set (match_operand:V2DF 0 "register_operand" "")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "")
                      (match_operand:V2DF 2 "register_operand" "")
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
  [(set (match_operand:V2DF 0 "register_operand" "")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "")
                      (match_operand:V2DF 2 "register_operand" "")
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

(define_insn "fmav2df4"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (fma:V2DF (match_operand:V2DF 1 "register_operand" "r")
                  (match_operand:V2DF 2 "register_operand" "r")
                  (match_operand:V2DF 3 "register_operand" "0")))]
  ""
  "ffmad %x0 = %x1, %x2\n\t;;\n\tffmad %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_insn "kvx_ffmadp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand:V2DF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMADP))]
  ""
  "ffmad%4 %x0 = %x1, %x2\n\t;;\n\tffmad%4 %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_insn "fnmav2df4"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (fma:V2DF (neg:V2DF (match_operand:V2DF 1 "register_operand" "r"))
                  (match_operand:V2DF 2 "register_operand" "r")
                  (match_operand:V2DF 3 "register_operand" "0")))]
  ""
  "ffmsd %x0 = %x1, %x2\n\t;;\n\tffmsd %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_insn "kvx_ffmsdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand:V2DF 3 "register_operand" "0")
                      (match_operand 4 "" "")] UNSPEC_FFMSDP))]
  ""
  "ffmsd%4 %x0 = %x1, %x2\n\t;;\n\tffmsd%4 %y0 = %y1, %y2"
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_expand "fminv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "")
        (smin:V2DF (match_operand:V2DF 1 "register_operand" "")
                   (match_operand:V2DF 2 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_fmindp (operands[0], operands[1], operands[2]));
    DONE;
  }
)

(define_insn "kvx_fmindp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (smin:V2DF (match_operand:V2DF 1 "register_operand" "r")
                   (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "fmind %x0 = %x1, %x2\n\tfmind %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "fmaxv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "")
        (smax:V2DF (match_operand:V2DF 1 "register_operand" "")
                   (match_operand:V2DF 2 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_fmaxdp (operands[0], operands[1], operands[2]));
    DONE;
  }
)

(define_insn "kvx_fmaxdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (smax:V2DF (match_operand:V2DF 1 "register_operand" "r")
                   (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "fmaxd %x0 = %x1, %x2\n\tfmaxd %y0 = %y1, %y2"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "negv2df2"
  [(set (match_operand:V2DF 0 "register_operand" "")
        (neg:V2DF (match_operand:V2DF 1 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_fnegdp (operands[0], operands[1]));
    DONE;
  }
)

(define_insn "kvx_fnegdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (neg:V2DF (match_operand:V2DF 1 "register_operand" "r")))]
  ""
  "fnegd %x0 = %x1\n\tfnegd %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_expand "absv2df2"
  [(set (match_operand:V2DF 0 "register_operand" "")
        (abs:V2DF (match_operand:V2DF 1 "register_operand" "")))]
  ""
  {
    emit_insn (gen_kvx_fabsdp (operands[0], operands[1]));
    DONE;
  }
)

(define_insn "kvx_fabsdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (abs:V2DF (match_operand:V2DF 1 "register_operand" "r")))]
  ""
  "fabsd %x0 = %x1\n\tfabsd %y0 = %y1"
  [(set_attr "type" "alu_lite_x2")
   (set_attr "length"         "8")]
)

(define_insn "floatv2div2df2"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (float:V2DF (match_operand:V2DI 1 "register_operand" "r")))]
  ""
  "floatd.rn %x0 = %x1, 0\n\t;;\n\tfloatd.rn %y0 = %y1, 0"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "kvx_floatdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATDP))]
  ""
  "floatd%3 %x0 = %x1, %2\n\t;;\n\tfloatd%3 %y0 = %y1, %2"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "floatunsv2div2df2"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unsigned_float:V2DF (match_operand:V2DI 1 "register_operand" "r")))]
  ""
  "floatud.rn %x0 = %x1, 0\n\t;;\n\tfloatud.rn %y0 = %y1, 0"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "kvx_floatudp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DI 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FLOATUDP))]
  ""
  "floatud%3 %x0 = %x1, %2\n\t;;\n\tfloatud%3 %y0 = %y1, %2"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "fix_truncv2dfv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (fix:V2DI (match_operand:V2DF 1 "register_operand" "r")))]
  ""
  "fixedd.rz %x0 = %x1, 0\n\t;;\n\tfixedd.rz %y0 = %y1, 0"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "kvx_fixeddp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDDP))]
  ""
  "fixedd%3 %x0 = %x1, %2\n\t;;\n\tfixedd%3 %y0 = %y1, %2"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "fixuns_truncv2dfv2di2"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unsigned_fix:V2DI (match_operand:V2DF 1 "register_operand" "r")))]
  ""
  "fixedud.rz %x0 = %x1, 0\n\t;;\n\tfixedud.rz %y0 = %y1, 0"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)

(define_insn "kvx_fixedudp"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (unspec:V2DI [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:SI 2 "sixbits_unsigned_operand" "i")
                      (match_operand 3 "" "")] UNSPEC_FIXEDUDP))]
  ""
  "fixedud%3 %x0 = %x1, %2\n\t;;\n\tfixedud%3 %y0 = %y1, %2"
  [(set_attr "type" "mau_fpu")
   (set_attr "length"     "8")]
)


