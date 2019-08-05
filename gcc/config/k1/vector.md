
;; V4HI

(define_expand "movv4hi"
  [(set (match_operand:V4HI 0 "nonimmediate_operand" "")
        (match_operand:V4HI 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (V4HImode, operands[1]);
    }
  }
)

(define_insn "*movv4hi_real"
  [(set (match_operand:V4HI 0 "nonimmediate_operand" "=r, r, r, r, r,a,m,r,r,r")
        (match_operand:V4HI 1 "general_operand"       "r,Ca,Cm,Za,Zm,r,r,v16,v43,i"))]
  "(!immediate_operand(operands[1], V4HImode) || !memory_operand(operands[0], V4HImode))"
  {
    switch (which_alternative) {
    case 0: return "copyd %0 = %1";
    case 1:
    case 2:
    case 3:
    case 4: return "ld%C1%m1 %0 = %1";
    case 5:
    case 6: return "sd%m0 %0 = %1";
    case 7:
    case 8:
    case 9: return "make %0 = %1";
    default: gcc_unreachable ();
    }
  }
  [(set_attr "type" "alu_tiny,lsu_auxw_load,lsu_auxw_load_x,lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxr_store,lsu_auxr_store_x,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,            4,              8,                     4,                       8,             4,               8,       4,         8,        12")]
)

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

(define_expand "movv8hi"
  [(set (match_operand:V8HI 0 "nonimmediate_operand" "")
        (match_operand:V8HI 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (V8HImode, operands[1]);
    }
  }
)

(define_insn "*movv8hi_real"
  [(set (match_operand:V8HI 0 "nonimmediate_operand" "=r, r, r, r, r,a,m,r")
        (match_operand:V8HI 1 "general_operand"       "r,Ca,Cm,Za,Zm,r,r,i"))]
  "(!immediate_operand(operands[1], V8HImode) || !memory_operand(operands[0], V8HImode))"
  {
    switch (which_alternative) {
    case 0:
      {
        rtx x = operands[1];
        int regno =  REGNO (x);
        static char instruction[256];
        sprintf (instruction,
                 "copyq %%Q0 = $r%d, $r%d",
                 regno, regno + 1);
        return instruction;
      }
      return "copyo %O0 = %O1";
    case 1: case 2: case 3: case 4:
      return "lq%C1%m1 %Q0 = %1";
    case 5: case 6:
      return "sq%m0 %0 = %Q1";
    case 7:
      {
        rtx x = operands[1];
        int regno =  REGNO (operands[0]);
        HOST_WIDE_INT value_0 = k1_const_vector_value (x, 0);
        HOST_WIDE_INT value_1 = k1_const_vector_value (x, 4);
        static char instruction[256];
        sprintf (instruction,
                 "make $r%d = 0x%llx\n\tmake $r%d = 0x%llx",
                 regno, (long long)value_0, regno + 1, (long long)value_1);
        return instruction;
      }
    default:
      gcc_unreachable ();
    }
  }
  [(set_attr "type"    "lsu_auxr_auxw, lsu_auxw_load, lsu_auxw_load_x, lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxr_store, lsu_auxr_store_x, alu_dual_y")
   (set_attr "length"  "            4,              4,              8,                      4,                        8,              4,                8,         24")]
)


;; V2SI

(define_expand "movv2si"
  [(set (match_operand:V2SI 0 "nonimmediate_operand" "")
        (match_operand:V2SI 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (V2SImode, operands[1]);
    }
  }
)

(define_insn "*movv2si_real"
  [(set (match_operand:V2SI 0 "nonimmediate_operand" "=r, r, r, r, r,a,m,r,r,r")
        (match_operand:V2SI 1 "general_operand"       "r,Ca,Cm,Za,Zm,r,r,v16,v43,i"))]
  "(!immediate_operand(operands[1], V2SImode) || !memory_operand(operands[0], V2SImode))"
  {
    switch (which_alternative) {
    case 0: return "copyd %0 = %1";
    case 1:
    case 2:
    case 3:
    case 4: return "ld%C1%m1 %0 = %1";
    case 5:
    case 6: return "sd%m0 %0 = %1";
    case 7:
    case 8:
    case 9: return "make %0 = %1";
    default: gcc_unreachable ();
    }
  }
  [(set_attr "type" "alu_tiny,lsu_auxw_load,lsu_auxw_load_x,lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxr_store,lsu_auxr_store_x,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,            4,              8,                     4,                       8,             4,               8,       4,         8,        12")]
)

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

(define_insn "maddv2siv2di4"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (plus:V2DI (sign_extend:V2DI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r")
                                                (match_operand:V2SI 2 "nonmemory_operand" "r")))
                   (match_operand:V2DI 3 "register_operand" "0")))]
  "0"
  "maddwdp %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "umaddv2siv2di4"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (plus:V2DI (zero_extend:V2DI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r")
                                                (match_operand:V2SI 2 "nonmemory_operand" "r")))
                   (match_operand:V2DI 3 "register_operand" "0")))]
  "0"
  "madduwdp %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
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

(define_insn "msubv2siv2di4"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (minus:V2DI (match_operand:V2DI 3 "register_operand" "0")
                    (sign_extend:V2DI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r")
                                                 (match_operand:V2SI 2 "register_operand" "r")))))]
  "0"
  "msbfwdp %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)

(define_insn "umsubv2siv2di4"
  [(set (match_operand:V2DI 0 "register_operand" "=r")
        (minus:V2DI (match_operand:V2DI 3 "register_operand" "0")
                    (zero_extend:V2DI (mult:V2SI (match_operand:V2SI 1 "register_operand" "r")
                                                 (match_operand:V2SI 2 "register_operand" "r")))))]
  "0"
  "msbfuwdp %0 = %1, %2"
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

(define_expand "movv4si"
  [(set (match_operand:V4SI 0 "nonimmediate_operand" "")
        (match_operand:V4SI 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (V4SImode, operands[1]);
    }
  }
)

(define_insn "*movv4si_real"
  [(set (match_operand:V4SI 0 "nonimmediate_operand" "=r, r, r, r, r,a,m,r")
        (match_operand:V4SI 1 "general_operand"       "r,Ca,Cm,Za,Zm,r,r,i"))]
  "(!immediate_operand(operands[1], V4SImode) || !memory_operand(operands[0], V4SImode))"
  {
    switch (which_alternative) {
    case 0:
      {
        rtx x = operands[1];
        int regno =  REGNO (x);
        static char instruction[256];
        sprintf (instruction,
                 "copyq %%Q0 = $r%d, $r%d",
                 regno, regno + 1);
        return instruction;
      }
      return "copyo %O0 = %O1";
    case 1: case 2: case 3: case 4:
      return "lq%C1%m1 %Q0 = %1";
    case 5: case 6:
      return "sq%m0 %0 = %Q1";
    case 7:
      {
        rtx x = operands[1];
        int regno =  REGNO (operands[0]);
        HOST_WIDE_INT value_0 = k1_const_vector_value (x, 0);
        HOST_WIDE_INT value_1 = k1_const_vector_value (x, 2);
        static char instruction[256];
        sprintf (instruction,
                 "make $r%d = 0x%llx\n\tmake $r%d = 0x%llx",
                 regno, (long long)value_0, regno + 1, (long long)value_1);
        return instruction;
      }
    default:
      gcc_unreachable ();
    }
  }
  [(set_attr "type"    "lsu_auxr_auxw, lsu_auxw_load, lsu_auxw_load_x, lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxr_store, lsu_auxr_store_x, alu_dual_y")
   (set_attr "length"  "            4,              4,              8,                      4,                        8,              4,                8,         24")]
)

(define_insn "mulv4si3"
  [(set (match_operand:V4SI 0 "register_operand" "=r")
        (mult:V4SI (match_operand:V4SI 1 "register_operand" "r")
                   (match_operand:V4SI 2 "nonmemory_operand" "r")))]
  ""
  "mulwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr")]
)


;; V2DI

(define_expand "movv2di"
  [(set (match_operand:V2DI 0 "nonimmediate_operand" "")
        (match_operand:V2DI 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (V2DImode, operands[1]);
    }
  }
)

(define_insn "*movv2di_real"
  [(set (match_operand:V2DI 0 "nonimmediate_operand" "=r, r, r, r, r,a,m,r")
        (match_operand:V2DI 1 "general_operand"       "r,Ca,Cm,Za,Zm,r,r,i"))]
  "(!immediate_operand(operands[1], V2DImode) || !memory_operand(operands[0], V2DImode))"
  {
    switch (which_alternative) {
    case 0:
      {
        rtx x = operands[1];
        int regno =  REGNO (x);
        static char instruction[256];
        sprintf (instruction,
                 "copyq %%Q0 = $r%d, $r%d",
                 regno, regno + 1);
        return instruction;
      }
      return "copyo %O0 = %O1";
    case 1: case 2: case 3: case 4:
      return "lq%C1%m1 %Q0 = %1";
    case 5: case 6:
      return "sq%m0 %0 = %Q1";
    case 7:
      {
        rtx x = operands[1];
        int regno =  REGNO (operands[0]);
        HOST_WIDE_INT value_0 = k1_const_vector_value (x, 0);
        HOST_WIDE_INT value_1 = k1_const_vector_value (x, 1);
        static char instruction[256];
        sprintf (instruction,
                 "make $r%d = 0x%llx\n\tmake $r%d = 0x%llx",
                 regno, (long long)value_0, regno + 1, (long long)value_1);
        return instruction;
      }
    default:
      gcc_unreachable ();
    }
  }
  [(set_attr "type"    "lsu_auxr_auxw, lsu_auxw_load, lsu_auxw_load_x, lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxr_store, lsu_auxr_store_x, alu_dual_y")
   (set_attr "length"  "            4,              4,              8,                      4,                        8,              4,                8,         24")]
)


;; V4DI

(define_expand "movv4di"
  [(set (match_operand:V4DI 0 "nonimmediate_operand" "")
        (match_operand:V4DI 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (V4DImode, operands[1]);
    }
  }
)

(define_insn "*movv4di_real"
  [(set (match_operand:V4DI 0 "nonimmediate_operand" "=r, r, r, r, r,a,m,r")
        (match_operand:V4DI 1 "general_operand"       "r,Ca,Cm,Za,Zm,r,r,i"))]
  "(!immediate_operand(operands[1], V4DImode) || !memory_operand(operands[0], V4DImode))"
  {
    switch (which_alternative) {
    case 0:
      return "copyo %O0 = %O1";
    case 1: case 2: case 3: case 4:
      return "lo%C1%m1 %O0 = %1";
    case 5: case 6:
      return "so%m0 %0 = %O1";
    case 7:
      {
        rtx x = operands[1];
        int regno =  REGNO (operands[0]);
        HOST_WIDE_INT value_0 = k1_const_vector_value (x, 0);
        HOST_WIDE_INT value_1 = k1_const_vector_value (x, 1);
        HOST_WIDE_INT value_2 = k1_const_vector_value (x, 2);
        HOST_WIDE_INT value_3 = k1_const_vector_value (x, 3);
        static char instruction[256];
        sprintf (instruction,
                 "make $r%d = 0x%llx\n\tmake $r%d = 0x%llx\n\t;;\n\t" 
                 "make $r%d = 0x%llx\n\tmake $r%d = 0x%llx",
                 regno, (long long)value_0, regno + 1, (long long)value_1,
                 regno + 2, (long long)value_2, regno + 3, (long long)value_3);
        return instruction;
      }
    default:
      gcc_unreachable ();
    }
  }
  [(set_attr "type"    "lsu_auxr_auxw, lsu_auxw_load, lsu_auxw_load_x, lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxr_store, lsu_auxr_store_x, alu_dual_y")
   (set_attr "length"  "            4,              4,              8,                      4,                        8,              4,                8,         48")]
)


;; V2SF

(define_expand "movv2sf"
  [(set (match_operand:V2SF 0 "nonimmediate_operand" "")
        (match_operand:V2SF 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (V2SFmode, operands[1]);
    }
  }
)

(define_insn "*movv2sf_real"
  [(set (match_operand:V2SF 0 "nonimmediate_operand" "=r, r, r, r, r,a,m,r,r,r")
        (match_operand:V2SF 1 "general_operand"       "r,Ca,Cm,Za,Zm,r,r,v16,v43,i"))]
  "(!immediate_operand(operands[1], V2SFmode) || !memory_operand(operands[0], V2SFmode))"
  {
    switch (which_alternative) {
    case 0: return "copyd %0 = %1";
    case 1:
    case 2:
    case 3:
    case 4: return "ld%C1%m1 %0 = %1";
    case 5:
    case 6: return "sd%m0 %0 = %1";
    case 7:
    case 8:
    case 9: return "make %0 = %1";
    default: gcc_unreachable ();
    }
  }
  [(set_attr "type" "alu_tiny,lsu_auxw_load,lsu_auxw_load_x,lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxr_store,lsu_auxr_store_x,alu_tiny,alu_tiny_x,alu_tiny_y")
   (set_attr "length" "     4,            4,              8,                     4,                       8,             4,               8,       4,         8,        12")]
)

(define_insn "addv2sf3"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (plus:V2SF (match_operand:V2SF 1 "register_operand" "r")
                   (match_operand:V2SF 2 "register_operand" "r")))]
  ""
  "faddwp %0 = %1, %2"
  [(set_attr "type" "mau_fpu")]
)

(define_insn "k1_faddwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDWP))]
  ""
  "faddwp%3 %0 = %1, %2"
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

(define_insn "k1_fsbfwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFWP))]
  ""
  "fsbfwp%3 %0 = %1, %2"
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

(define_insn "k1_fmulwp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (unspec:V2SF [(match_operand:V2SF 1 "register_operand" "r")
                      (match_operand:V2SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWP))]
  ""
  "fmulwp%3 %0 = %1, %2"
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

(define_insn "k1_ffmawp"
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

(define_insn "k1_ffmswp"
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

(define_insn "k1_fminwp"
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

(define_insn "k1_fmaxwp"
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

(define_insn "k1_fnegwp"
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

(define_insn "k1_fabswp"
  [(set (match_operand:V2SF 0 "register_operand" "=r")
        (abs:V2SF (match_operand:V2SF 1 "register_operand" "r")))]
  ""
  "fabswp %0 = %1"
  [(set_attr "type" "alu_lite")]
)


;; V4SF

(define_expand "movv4sf"
  [(set (match_operand:V4SF 0 "nonimmediate_operand" "")
        (match_operand:V4SF 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (V4SFmode, operands[1]);
    }
  }
)

(define_insn "*movv4sf_real"
  [(set (match_operand:V4SF 0 "nonimmediate_operand" "=r, r, r, r, r,a,m,r")
        (match_operand:V4SF 1 "general_operand"       "r,Ca,Cm,Za,Zm,r,r,i"))]
  "(!immediate_operand(operands[1], V4SFmode) || !memory_operand(operands[0], V4SFmode))"
  {
    switch (which_alternative) {
    case 0:
      {
        rtx x = operands[1];
        int regno =  REGNO (x);
        static char instruction[256];
        sprintf (instruction,
                 "copyq %%Q0 = $r%d, $r%d",
                 regno, regno + 1);
        return instruction;
      }
      return "copyo %O0 = %O1";
    case 1: case 2: case 3: case 4:
      return "lq%C1%m1 %Q0 = %1";
    case 5: case 6:
      return "sq%m0 %0 = %Q1";
    case 7:
      {
        rtx x = operands[1];
        int regno =  REGNO (operands[0]);
        HOST_WIDE_INT value_0 = k1_const_vector_value (x, 0);
        HOST_WIDE_INT value_1 = k1_const_vector_value (x, 2);
        static char instruction[256];
        sprintf (instruction,
                 "make $r%d = 0x%llx\n\tmake $r%d = 0x%llx",
                 regno, (long long)value_0, regno + 1, (long long)value_1);
        return instruction;
      }
    default:
      gcc_unreachable ();
    }
  }
  [(set_attr "type"    "lsu_auxr_auxw, lsu_auxw_load, lsu_auxw_load_x, lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxr_store, lsu_auxr_store_x, alu_dual_y")
   (set_attr "length"  "            4,              4,              8,                      4,                        8,              4,                8,         24")]
)

(define_insn "addv4sf3"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (plus:V4SF (match_operand:V4SF 1 "register_operand" "r")
                   (match_operand:V4SF 2 "register_operand" "r")))]
  ""
  "faddwq %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "k1_faddwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDWQ))]
  ""
  "faddwq%3 %0 = %1, %2"
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

(define_insn "k1_fsbfwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFWQ))]
  ""
  "fsbfwq%3 %0 = %1, %2"
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

(define_insn "k1_fmulwq"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "r")
                      (match_operand:V4SF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FMULWQ))]
  ""
  "fmulwq%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

;; (define_expand "fmav4sf4"
;;   [(set (match_operand:V4SF 0 "register_operand" "")
;;         (fma:V4SF (match_operand:V4SF 1 "register_operand" "")
;;                   (match_operand:V4SF 2 "register_operand" "")
;;                   (match_operand:V4SF 3 "register_operand" "")))]
;;   ""
;;   {
;;     rtx empty = gen_rtx_CONST_STRING (VOIDmode, "");
;;     emit_insn (gen_k1_ffmawq (operands[0], operands[1], operands[2], operands[3], empty));
;;     DONE;
;;   }
;; )

;; Expand above leads to unrecognizable (subreg:V2SF (subreg:V4SF (reg:OI ) 0) 0) in fmav8sf4.
(define_insn "fmav4sf4"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (fma:V4SF (match_operand:V4SF 1 "register_operand" "r")
                  (match_operand:V4SF 2 "register_operand" "r")
                  (match_operand:V4SF 3 "register_operand" "0")))]
  ""
  {
    static char instruction[256];
    int regno_0 =  REGNO (operands[0]);
    int regno_1 =  REGNO (operands[1]);
    int regno_2 =  REGNO (operands[2]);
    sprintf (instruction,
             "ffmawp $r%d = $r%d, $r%d\n\t;;\n\tffmawp $r%d = $r%d, $r%d",
             regno_0, regno_1, regno_2, regno_0+1, regno_1+1, regno_2+1);
    return instruction;
  }
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_expand "k1_ffmawq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMAWQ))]
  ""
  {
    rtx operands_0_lo = gen_rtx_SUBREG (V2SFmode, operands[0], 0);
    rtx operands_0_hi = gen_rtx_SUBREG (V2SFmode, operands[0], 8);
    rtx operands_1_lo = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx operands_1_hi = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx operands_2_lo = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    rtx operands_2_hi = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    rtx operands_3_lo = gen_rtx_SUBREG (V2SFmode, operands[3], 0);
    rtx operands_3_hi = gen_rtx_SUBREG (V2SFmode, operands[3], 8);
    emit_insn (gen_k1_ffmawp (operands_0_lo, operands_1_lo, operands_2_lo, operands_3_lo, operands[4]));
    emit_insn (gen_k1_ffmawp (operands_0_hi, operands_1_hi, operands_2_hi, operands_3_hi, operands[4]));
    DONE;
  }
)

;; (define_expand "fnmav4sf4"
;;   [(set (subreg:V2SF (match_operand:V4SF 0 "register_operand" "") 0)
;;         (fma:V2SF (neg:V2SF (subreg:V2SF (match_operand:V4SF 1 "register_operand" "") 0))
;;                   (subreg:V2SF (match_operand:V4SF 2 "register_operand" "") 0)
;;                   (subreg:V2SF (match_operand:V4SF 3 "register_operand" "") 0)))
;;    (set (subreg:V2SF (match_dup 0) 8)
;;         (fma:V2SF (neg:V2SF (subreg:V2SF (match_dup 1) 8))
;;                   (subreg:V2SF (match_dup 2) 8)
;;                   (subreg:V2SF (match_dup 3) 8)))]
;;   ""
;;   {
;;     rtx empty = gen_rtx_CONST_STRING (VOIDmode, "");
;;     emit_insn (gen_k1_ffmswq (operands[0], operands[1], operands[2], operands[3], empty));
;;     DONE;
;;   }
;; )

;; Apply same workaround as for fmav4sf4.
(define_insn "fnmav4sf4"
  [(set (match_operand:V4SF 0 "register_operand" "=r")
        (fma:V4SF (neg:V4SF (match_operand:V4SF 1 "register_operand" "r"))
                  (match_operand:V4SF 2 "register_operand" "r")
                  (match_operand:V4SF 3 "register_operand" "0")))]
  ""
  {
    static char instruction[256];
    int regno_0 =  REGNO (operands[0]);
    int regno_1 =  REGNO (operands[1]);
    int regno_2 =  REGNO (operands[2]);
    sprintf (instruction,
             "ffmswp $r%d = $r%d, $r%d\n\t;;\n\tffmswp $r%d = $r%d, $r%d",
             regno_0, regno_1, regno_2, regno_0+1, regno_1+1, regno_2+1);
    return instruction;
  }
  [(set_attr "type" "mau_auxr_fpu")
   (set_attr "length"          "8")]
)

(define_expand "k1_ffmswq"
  [(set (match_operand:V4SF 0 "register_operand" "")
        (unspec:V4SF [(match_operand:V4SF 1 "register_operand" "")
                      (match_operand:V4SF 2 "register_operand" "")
                      (match_operand:V4SF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMSWQ))]
  ""
  {
    rtx operands_0_lo = gen_rtx_SUBREG (V2SFmode, operands[0], 0);
    rtx operands_0_hi = gen_rtx_SUBREG (V2SFmode, operands[0], 8);
    rtx operands_1_lo = gen_rtx_SUBREG (V2SFmode, operands[1], 0);
    rtx operands_1_hi = gen_rtx_SUBREG (V2SFmode, operands[1], 8);
    rtx operands_2_lo = gen_rtx_SUBREG (V2SFmode, operands[2], 0);
    rtx operands_2_hi = gen_rtx_SUBREG (V2SFmode, operands[2], 8);
    rtx operands_3_lo = gen_rtx_SUBREG (V2SFmode, operands[3], 0);
    rtx operands_3_hi = gen_rtx_SUBREG (V2SFmode, operands[3], 8);
    emit_insn (gen_k1_ffmswp (operands_0_lo, operands_1_lo, operands_2_lo, operands_3_lo, operands[4]));
    emit_insn (gen_k1_ffmswp (operands_0_hi, operands_1_hi, operands_2_hi, operands_3_hi, operands[4]));
    DONE;
  }
)


;; V2DF

(define_expand "movv2df"
  [(set (match_operand:V2DF 0 "nonimmediate_operand" "")
        (match_operand:V2DF 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0])) {
      operands[1] = force_reg (V2DFmode, operands[1]);
    }
  }
)

(define_insn "*movv2df_real"
  [(set (match_operand:V2DF 0 "nonimmediate_operand" "=r, r, r, r, r,a,m,r")
        (match_operand:V2DF 1 "general_operand"       "r,Ca,Cm,Za,Zm,r,r,i"))]
  "(!immediate_operand(operands[1], V2DFmode) || !memory_operand(operands[0], V2DFmode))"
  {
    switch (which_alternative) {
    case 0:
      {
        rtx x = operands[1];
        int regno =  REGNO (x);
        static char instruction[256];
        sprintf (instruction,
                 "copyq %%Q0 = $r%d, $r%d",
                 regno, regno + 1);
        return instruction;
      }
      return "copyo %O0 = %O1";
    case 1: case 2: case 3: case 4:
      return "lq%C1%m1 %Q0 = %1";
    case 5: case 6:
      return "sq%m0 %0 = %Q1";
    case 7:
      {
        rtx x = operands[1];
        int regno =  REGNO (operands[0]);
        HOST_WIDE_INT value_0 = k1_const_vector_value (x, 0);
        HOST_WIDE_INT value_1 = k1_const_vector_value (x, 1);
        static char instruction[256];
        sprintf (instruction,
                 "make $r%d = 0x%llx\n\tmake $r%d = 0x%llx",
                 regno, (long long)value_0, regno + 1, (long long)value_1);
        return instruction;
      }
    default:
      gcc_unreachable ();
    }
  }
  [(set_attr "type"    "lsu_auxr_auxw, lsu_auxw_load, lsu_auxw_load_x, lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxr_store, lsu_auxr_store_x, alu_dual_y")
   (set_attr "length"  "            4,              4,              8,                      4,                        8,              4,                8,         24")]
)

(define_insn "addv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (plus:V2DF (match_operand:V2DF 1 "register_operand" "r")
                   (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "fadddp %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "k1_fadddp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FADDDP))]
  ""
  "fadddp%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "subv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (minus:V2DF (match_operand:V2DF 1 "register_operand" "r")
                    (match_operand:V2DF 2 "register_operand" "r")))]
  ""
  "fsbfdp %0 = %2, %1"
  [(set_attr "type" "mau_auxr_fpu")]
)

(define_insn "k1_fsbfdp"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "r")
                      (match_operand:V2DF 2 "register_operand" "r")
                      (match_operand 3 "" "")] UNSPEC_FSBFDP))]
  ""
  "fsbfdp%3 %0 = %1, %2"
  [(set_attr "type" "mau_auxr_fpu")]
)

;; (define_insn "mulv2df3"
;;   [(set (match_operand:V2DF 0 "register_operand" "=r")
;;         (mult:V2DF (match_operand:V2DF 1 "register_operand" "r")
;;                    (match_operand:V2DF 2 "register_operand" "r")))]
;;   ""
;;   "FMULDP %0 = %1, %2"
;;   [(set_attr "type" "mau_fpu")]
;; )

(define_expand "mulv2df3"
  [(set (match_operand:V2DF 0 "register_operand" "")
        (mult:V2DF (match_operand:V2DF 1 "register_operand" "")
                   (match_operand:V2DF 2 "register_operand" "")))]
  ""
  {
    rtx empty = gen_rtx_CONST_STRING (VOIDmode, "");
    emit_insn (gen_k1_fmuldp (operands[0], operands[1], operands[2], empty));
    DONE;
  }
)

(define_expand "k1_fmuldp"
  [(set (match_operand:V2DF 0 "register_operand" "")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "")
                      (match_operand:V2DF 2 "register_operand" "")
                      (match_operand 3 "" "")] UNSPEC_FMULDP))]
  ""
  {
    rtx operands_0_lo = gen_rtx_SUBREG (DFmode, operands[0], 0);
    rtx operands_0_hi = gen_rtx_SUBREG (DFmode, operands[0], 8);
    rtx operands_1_lo = gen_rtx_SUBREG (DFmode, operands[1], 0);
    rtx operands_1_hi = gen_rtx_SUBREG (DFmode, operands[1], 8);
    rtx operands_2_lo = gen_rtx_SUBREG (DFmode, operands[2], 0);
    rtx operands_2_hi = gen_rtx_SUBREG (DFmode, operands[2], 8);
    emit_insn (gen_k1_fmuld (operands_0_lo, operands_1_lo, operands_2_lo, operands[3]));
    emit_insn (gen_k1_fmuld (operands_0_hi, operands_1_hi, operands_2_hi, operands[3]));
    DONE;
  }
)

;; (define_insn "fmav2df4"
;;   [(set (match_operand:V2DF 0 "register_operand" "=r")
;;         (fma:V2DF (match_operand:V2DF 1 "register_operand" "r")
;;                   (match_operand:V2DF 2 "register_operand" "r")
;;                   (match_operand:V2DF 3 "register_operand" "0")))]
;;   ""
;;   "FFMADP %0 = %1, %2"
;;   [(set_attr "type" "mau_auxr_fpu")]
;; )

(define_expand "fmav2df4"
  [(set (match_operand:V2DF 0 "register_operand" "")
        (fma:V2DF (match_operand:V2DF 1 "register_operand" "")
                  (match_operand:V2DF 2 "register_operand" "")
                  (match_operand:V2DF 3 "register_operand" "")))]
  ""
  {
    rtx empty = gen_rtx_CONST_STRING (VOIDmode, "");
    emit_insn (gen_k1_ffmadp (operands[0], operands[1], operands[2], operands[3], empty));
    DONE;
  }
)

(define_expand "k1_ffmadp"
  [(set (match_operand:V2DF 0 "register_operand" "")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "")
                      (match_operand:V2DF 2 "register_operand" "")
                      (match_operand:V2DF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMADP))]
  ""
  {
    rtx operands_0_lo = gen_rtx_SUBREG (DFmode, operands[0], 0);
    rtx operands_0_hi = gen_rtx_SUBREG (DFmode, operands[0], 8);
    rtx operands_1_lo = gen_rtx_SUBREG (DFmode, operands[1], 0);
    rtx operands_1_hi = gen_rtx_SUBREG (DFmode, operands[1], 8);
    rtx operands_2_lo = gen_rtx_SUBREG (DFmode, operands[2], 0);
    rtx operands_2_hi = gen_rtx_SUBREG (DFmode, operands[2], 8);
    rtx operands_3_lo = gen_rtx_SUBREG (DFmode, operands[3], 0);
    rtx operands_3_hi = gen_rtx_SUBREG (DFmode, operands[3], 8);
    emit_insn (gen_k1_ffmad (operands_0_lo, operands_1_lo, operands_2_lo, operands_3_lo, operands[4]));
    emit_insn (gen_k1_ffmad (operands_0_hi, operands_1_hi, operands_2_hi, operands_3_hi, operands[4]));
    DONE;
  }
)

;; (define_insn "fnmav2df4"
;;   [(set (match_operand:V2DF 0 "register_operand" "=r")
;;         (fma:V2DF (neg:V2DF (match_operand:V2DF 1 "register_operand" "r"))
;;                   (match_operand:V2DF 2 "register_operand" "r")
;;                   (match_operand:V2DF 3 "register_operand" "0")))]
;;   ""
;;   "FFMSDP %0 = %1, %2"
;;   [(set_attr "type" "mau_auxr_fpu")]
;; )

(define_expand "fnmav2df4"
  [(set (match_operand:V2DF 0 "register_operand" "=r")
        (fma:V2DF (neg:V2DF (match_operand:V2DF 1 "register_operand" ""))
                  (match_operand:V2DF 2 "register_operand" "")
                  (match_operand:V2DF 3 "register_operand" "")))]
  ""
  {
    rtx empty = gen_rtx_CONST_STRING (VOIDmode, "");
    emit_insn (gen_k1_ffmsdp (operands[0], operands[1], operands[2], operands[3], empty));
    DONE;
  }
)

(define_expand "k1_ffmsdp"
  [(set (match_operand:V2DF 0 "register_operand" "")
        (unspec:V2DF [(match_operand:V2DF 1 "register_operand" "")
                      (match_operand:V2DF 2 "register_operand" "")
                      (match_operand:V2DF 3 "register_operand" "")
                      (match_operand 4 "" "")] UNSPEC_FFMSDP))]
  ""
  {
    rtx operands_0_lo = gen_rtx_SUBREG (DFmode, operands[0], 0);
    rtx operands_0_hi = gen_rtx_SUBREG (DFmode, operands[0], 8);
    rtx operands_1_lo = gen_rtx_SUBREG (DFmode, operands[1], 0);
    rtx operands_1_hi = gen_rtx_SUBREG (DFmode, operands[1], 8);
    rtx operands_2_lo = gen_rtx_SUBREG (DFmode, operands[2], 0);
    rtx operands_2_hi = gen_rtx_SUBREG (DFmode, operands[2], 8);
    rtx operands_3_lo = gen_rtx_SUBREG (DFmode, operands[3], 0);
    rtx operands_3_hi = gen_rtx_SUBREG (DFmode, operands[3], 8);
    emit_insn (gen_k1_ffmsd (operands_0_lo, operands_1_lo, operands_2_lo, operands_3_lo, operands[4]));
    emit_insn (gen_k1_ffmsd (operands_0_hi, operands_1_hi, operands_2_hi, operands_3_hi, operands[4]));
    DONE;
  }
)


