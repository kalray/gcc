(define_attr "length" ""
         (const_int 4))

;; Only support Coolidge, but keep flexibility as long as it does not
;; cost too much

(define_attr "arch" "coolidge" (const (symbol_ref "kvx_arch_schedule")))

(define_attr "only_64b" "yes,no" (const_string "no"))

;; Unspec numbers
(include "unspec.md")

;; Register numbers
(include "kv3-registers.md")

;; Iterators
(include "iterators.md")

;; Instruction types
(include "types.md")

;; Scheduling classes
(include "coolidge.md")

;; Constraints
(include "constraints.md")

;; Predicates
(include "predicates.md")

;; Atomics
(include "atomics.md")

(define_expand "store_multiple"
  [(match_par_dup 3 [(set (match_operand:DI 0 "" "")
			  (match_operand:DI 1 "" ""))
		     (use (match_operand:DI 2 "" ""))])]
  ""
  "
{
  if (! kvx_expand_store_multiple (operands))
    FAIL;
}")

;; Simple store packing

;; sd+sd+sd+sd -> so
(define_peephole2
 [(set (match_operand:DI 0 "memory_operand" "")
       (match_operand:DI 1 "register_operand" ""))
  (set (match_operand:DI 2 "memory_operand" "")
       (match_operand:DI 3 "register_operand" ""))
  (set (match_operand:DI 4 "memory_operand" "")
       (match_operand:DI 5 "register_operand" ""))
  (set (match_operand:DI 6 "memory_operand" "")
       (match_operand:DI 7 "register_operand" ""))]
 ""
 [(const_int 0)]
 {
   if (kvx_pack_load_store (operands, 4))
     DONE;
   else
     FAIL;
 })

;; sd+sd -> sq
(define_peephole2
 [(set (match_operand:DI 0 "memory_operand" "")
       (match_operand:DI 1 "register_operand" ""))
  (set (match_operand:DI 2 "memory_operand" "")
       (match_operand:DI 3 "register_operand" ""))]
 ""
 [(const_int 0)]
 {
   if (kvx_pack_load_store (operands, 2))
     DONE;
   else
     FAIL;
 })

(define_expand "load_multiple"
  [(match_par_dup 3 [(set (match_operand:DI 0 "" "")
			  (match_operand:DI 1 "" ""))
		     (use (match_operand:DI 2 "" ""))])]
  ""
  "
{
  if (! kvx_expand_load_multiple (operands))
    FAIL;
}")

;; Simple load packing

;; ld+ld+ld+ld -> lo
(define_peephole2
 [(set (match_operand:DI 0 "register_operand" "")
       (match_operand:DI 1 "memory_operand" ""))
  (set (match_operand:DI 2 "register_operand" "")
       (match_operand:DI 3 "memory_operand" ""))
  (set (match_operand:DI 4 "register_operand" "")
       (match_operand:DI 5 "memory_operand" ""))
  (set (match_operand:DI 6 "register_operand" "")
       (match_operand:DI 7 "memory_operand" ""))]
 ""
 [(const_int 0)]
 {
   if (kvx_pack_load_store (operands, 4))
     DONE;
   else
     FAIL;
 })

;; ld+ld -> lq
(define_peephole2
 [(set (match_operand:DI 0 "register_operand" "")
       (match_operand:DI 1 "memory_operand" ""))
  (set (match_operand:DI 2 "register_operand" "")
       (match_operand:DI 3 "memory_operand" ""))]
 ""
 [(const_int 0)]
 {
   if (kvx_pack_load_store (operands, 2))
     DONE;
   else
     FAIL;
 })

(define_insn "*lo_multiple_cached"
  [(match_parallel 0 "load_multiple_operation"
    [(set (match_operand:DI 1 "register_operand" "=r,r,r")
          (mem:DI (plus:P (match_operand:P 2 "register_operand" "r,r,r")
                          (match_operand 3 "const_int_operand" "I10,B37,i"))))
     (set (match_operand:DI 4 "register_operand" "=r,r,r")
          (mem:DI (plus:P (match_dup 2) (match_operand 5 "const_int_operand" "I10,B37,i"))))
     (set (match_operand:DI 6 "register_operand" "=r,r,r")
          (mem:DI (plus:P (match_dup 2) (match_operand 7 "const_int_operand" "I10,B37,i"))))
     (set (match_operand:DI 8 "register_operand" "=r,r,r")
          (mem:DI (plus:P (match_dup 2) (match_operand 9 "const_int_operand" "I10,B37,i"))))])]
  "(XVECLEN (operands[0], 0) == 4)"
  "lo %o1 = %3[%2]"
  [(set_attr "type" "lsu_auxw_load,lsu_auxw_load_x,lsu_auxw_load_y")
   (set_attr "length" "4,8,12")])

(define_insn "*lo_multiple_uncached"
  [(match_parallel 0 "load_multiple_operation_uncached"
    [(set (match_operand:DI 1 "register_operand" "=r,r,r")
          (mem:DI (plus:P (match_operand:P 2 "register_operand" "r,r,r")
                          (match_operand 3 "const_int_operand" ""))))
     (set (match_operand:DI 4 "register_operand" "=r,r,r")
          (mem:DI (plus:P (match_dup 2) (match_operand 5 "const_int_operand" "I10,B37,i"))))
     (set (match_operand:DI 6 "register_operand" "=r,r,r")
          (mem:DI (plus:P (match_dup 2) (match_operand 7 "const_int_operand" "I10,B37,i"))))
     (set (match_operand:DI 8 "register_operand" "=r,r,r")
          (mem:DI (plus:P (match_dup 2) (match_operand 9 "const_int_operand" "I10,B37,i"))))])]
  "(XVECLEN (operands[0], 0) == 4)"
  "lo.u %o1 = %3[%2]"
  [(set_attr "type" "lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxw_load_uncached_y")
   (set_attr "length" "4,8,12")])

(define_insn "*lq_multiple_cached"
  [(match_parallel 0 "load_multiple_operation"
    [(set (match_operand:DI 1 "register_operand" "=r,r,r")
          (mem:DI (plus:P (match_operand:P 2 "register_operand" "r,r,r")
                          (match_operand 3 "const_int_operand" "I10,B37,i"))))
     (set (match_operand:DI 4 "register_operand" "=r,r,r")
          (mem:DI (plus:P (match_dup 2) (match_operand 5 "const_int_operand" "I10,B37,i"))))])]
  "(XVECLEN (operands[0], 0) == 2)"
  "lq %q1 = %3[%2]"
  [(set_attr "type" "lsu_auxw_load,lsu_auxw_load_x,lsu_auxw_load_y")
   (set_attr "length" "4,8,12")])

(define_insn "*lq_multiple_uncached"
  [(match_parallel 0 "load_multiple_operation_uncached"
    [(set (match_operand:DI 1 "register_operand" "=r,r,r")
          (mem:DI (plus:P (match_operand:P 2 "register_operand" "r,r,r")
                          (match_operand 3 "const_int_operand" "I10,B37,i"))))
     (set (match_operand:DI 4 "register_operand" "=r,r,r")
          (mem:DI (plus:P (match_dup 2) (match_operand 5 "const_int_operand" "I10,B37,i"))))])]
  "(XVECLEN (operands[0], 0) == 2)"
  "lq.u %q1 = %3[%2]"
  [(set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")
   (set_attr "length" "4,8,12")])

(define_insn "*sq_multiple"
  [(match_parallel 0 "store_multiple_operation"
    [(set (mem:DI (plus:P (match_operand:P 1 "register_operand" "r,r,r")
                          (match_operand 2 "const_int_operand" "I10,B37,i")))
	  (match_operand:DI 3 "register_operand" "r,r,r"))

     (set (mem:DI (plus:P (match_dup 1) (match_operand 4 "const_int_operand" "I10,B37,i")))
          (match_operand:DI 5 "register_operand" "r,r,r"))])]
  "(XVECLEN (operands[0], 0) == 2)"
  "sq %2[%1] = %q3"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length" "4,8,12")])

(define_insn "*so_multiple"
  [(match_parallel 0 "store_multiple_operation"
    [(set (mem:DI (plus:P (match_operand:P 1 "register_operand" "r,r,r")
                          (match_operand 2 "const_int_operand" "I10,B37,i")))
	  (match_operand:DI 3 "register_operand" "r,r,r"))

     (set (mem:DI (plus:P (match_dup 1) (match_operand 4 "const_int_operand" "I10,B37,i")))
          (match_operand:DI 5 "register_operand" "r,r,r"))

     (set (mem:DI (plus:P (match_dup 1) (match_operand 6 "const_int_operand" "I10,B37,i")))
          (match_operand:DI 7 "register_operand" "r,r,r"))

     (set (mem:DI (plus:P (match_dup 1) (match_operand 8 "const_int_operand" "I10,B37,i")))
          (match_operand:DI 9 "register_operand" "r,r,r"))])]
  "(XVECLEN (operands[0], 0) == 4)"
  "so %2[%1] = %o3"
  [(set_attr "type" "lsu_auxr_store,lsu_auxr_store_x,lsu_auxr_store_y")
   (set_attr "length" "4,8,12")])


;; ========================= move ========================
;;

;; FIXME AUTO: add size info for 'reg[reg]' addressing (currently falling back to lsu.x)
;; FIXME AUTO: reservations for coolidge are not OK

(define_expand "mov<mode>"
   [(set (match_operand:ALLIF 0 "nonimmediate_operand" "")
         (match_operand:ALLIF 1 "kvx_mov_operand"      ""))]
   ""
   "
    if (MEM_P (operands[0]))
        operands[1] = force_reg (<MODE>mode, operands[1]);

    if (CONSTANT_P (operands[1]))
      {
        kvx_expand_mov_constant (operands);
        DONE;
      }
   "
)

(define_expand "movti"
  [(set (match_operand:TI 0 "nonimmediate_operand" "")
        (match_operand:TI 1 "general_operand" ""))]
  ""
  {
    if (MEM_P(operands[0]))
      operands[1] = force_reg (TImode, operands[1]);
  }
)

(define_insn_and_split "*mov_quad_immediate"
    [(set (match_operand:TI 0 "register_operand" "=r")
          (match_operand:TI 1 "immediate_operand" "i" ))]
  ""
  "#"
  "&& reload_completed"
  [(const_int 0)]
  {
   /* We can't have 128bits immediate values, split it */
   kvx_split_128bits_move (operands[0], operands[1], TImode);
   DONE;
  }
)

;; Split what would end-up in a single copyq insn in 2 copyd.
;; Both copyd use 1 TINY each instead of the MAU used by copyq
;; at the cost of an extra word in .text.
(define_split
  [(set (match_operand:TI 0 "register_operand" "")
         (match_operand:TI 1 "register_operand" ""))]
  "!optimize_size && reload_completed"
  [(const_int 0)]
  {
    kvx_split_128bits_move (operands[0], operands[1], TImode);
    DONE;
  }
)

;; This should be used only for argument passing in registers r0-r11
;; where no register alignement is enforced (eg. a TI can be moved in
;; r3 and r4)
(define_insn_and_split "*mov_quad_oddreg"
    [(set (match_operand:TI 0 "nonimmediate_operand" "")
          (match_operand:TI 1 "general_operand" "" ))]
 "(kvx_is_reg_subreg_p (operands[0]) && !kvx_ok_for_paired_reg_p (operands[0]))
   || (kvx_is_reg_subreg_p (operands[1]) && !kvx_ok_for_paired_reg_p (operands[1]))"
  "#"
  "&& reload_completed"
  [(const_int 0)]
  {
   /* This should only happen during function argument preparation */
   kvx_split_128bits_move (operands[0], operands[1], TImode);
   DONE;
  }
)

(define_insn "*mov_quad"
    [(set (match_operand:TI 0 "kvx_nonimmediate_operand_pair" "=r, r,   r,  r,  r,  r,  r, a, b, m")
          (match_operand:TI 1 "kvx_nonimmediate_operand_pair" " r, Ca, Cb, Cm, Za, Zb, Zm, r, r, r" ))]
  "kvx_is_reg_subreg_p (operands[0]) || kvx_is_reg_subreg_p (operands[1])"
{
  switch (which_alternative)
    {
    case 0:
      return kvx_asm_pat_copyq (operands[1]);
    case 1: case 2: case 3: case 4: case 5: case 6:
      return "lq%C1%m1 %0 = %1";
    case 7: case 8: case 9:
      return "sq%m0 %0 = %1";
    default:
      gcc_unreachable ();
    }
}
[(set_attr "type"    "mau, lsu_auxw_load, lsu_auxw_load_x, lsu_auxw_load_y, lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxw_load_uncached_y, lsu_auxr_store, lsu_auxr_store_x, lsu_auxr_store_y")
 (set_attr "length"  "4,   4,             8,               12,              4,                      8,                        12,                       4,              8,                12")])

(define_insn "*mov_octuple"
    [(set (match_operand:OI 0 "kvx_nonimmediate_operand_quad" "=r, r,   r,  r,  r,  r,  r, a, b, m")
          (match_operand:OI 1 "kvx_nonimmediate_operand_quad" " r, Ca, Cb, Cm, Za, Zb, Zm, r, r, r"))]
  "kvx_is_reg_subreg_p (operands[0]) || kvx_is_reg_subreg_p (operands[1])"
{
  switch (which_alternative)
    {
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
[(set_attr "type"    "lsu_auxr_auxw, lsu_auxw_load, lsu_auxw_load_x, lsu_auxw_load_y, lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxw_load_uncached_y, lsu_auxr_store, lsu_auxr_store_x, lsu_auxr_store_y")
 (set_attr "length"  "            4,             4,               8,              12,                      4,                        8,                       12,              4,                8,               12")])

;; Split what would end-up in a single copyo insn in 2 copyq (that
;; will end up as 4 copyd). All copyd use 1 TINY each instead of the
;; LSU used by copyo at the cost of 3 extra words in .text.
(define_split
  [(set (match_operand:OI 0 "register_operand" "")
         (match_operand:OI 1 "register_operand" ""))]
  "!optimize_size && reload_completed"
  [(const_int 0)]
  {
    kvx_split_256bits_move (operands[0], operands[1], OImode);
    DONE;
  }
)

;; FIXME AUTO: refine set insn to bundle it when possible. T7808
(define_insn "*mov<mode>_all"
    [(set (match_operand:ALLIF 0 "nonimmediate_operand" "=r, r,           r,  r, a, b, m, r , r , r , r , r , r , r,  RXX, r, r")
          (match_operand:ALLIF 1 "general_operand"      " r, I16H16, I43H43, nF, r, r, r, Ca, Cb, Cm, Za, Zb, Zm, RXX,  r, T, S"))]
  "register_operand (operands[0], <MODE>mode) || register_operand (operands[1], <MODE>mode)"
{
  switch (which_alternative)
    {
    case 0:
      return "copy<ALLIF:sfx> %0 = %1";
    case 1: case 2: case 3: case 16:
      return "make %0 = %1";
    case 4: case 5: case 6:
      return "s<ALLIF:lsusize>%m0 %0 = %1";
    case 7: case 8: case 9: case 10: case 11: case 12:
      return "l<lsusize><lsusext>%C1%m1 %0 = %1";
    case 13:
      return "get %0 = %1";
    case 14:
      return "set %0 = %1";
    case 15:
      return "pcrel %0 = %T1";

    default:
      gcc_unreachable ();
    }
}
[(set_attr "type"    "alu_tiny, alu_tiny, alu_tiny_x, alu_tiny_y, lsu_auxr_store, lsu_auxr_store_x, lsu_auxr_store_y, lsu_auxw_load, lsu_auxw_load_x, lsu_auxw_load_y, lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxw_load_uncached_y, bcu_get, all,  alu_full_x, alu_tiny<symlen1>")
 (set_attr "length"  "4,        4,        8,          12,         4,              8,                12,               4,             8,               12,              4,                      8,                        12,                       4,       4,    8,          <symlen2>")
 (set (attr "disabled")
      (cond [(and (eq_attr "alternative" "16")
                  (match_test "<MODE>mode != Pmode")) (const_string "yes")
	     (and (eq_attr "alternative" "15")
                  (match_test "!flag_pic")) (const_string "yes")]
	     (const_string "no")))]
)

(define_insn "add_pcrel_<mode>"
  [(set (match_operand:P 0 "register_operand" "=r")
        (const:P (plus:P (pc)
                         (unspec:P [(match_operand 1 "symbolic_operand" "" )
                                    (pc)] UNSPEC_PCREL))))
]
 ""
 "pcrel %0 = %T1"
[(set_attr "type" "alu_full_y")
 (set_attr "length" "12")])

;; ========================= jump ========================
;;

(define_insn "jump"
  [(set (pc) (label_ref (match_operand 0)))]
  ""
  "goto %0"
[(set_attr "type" "bcu")
 (set_attr "class" "branch")]
)

(define_expand "indirect_jump"
  [(set (pc) (match_operand 0 "address_operand"))])

(define_insn "*indirect_jump_<mode>"
  [(set (pc) (match_operand:P 0 "address_operand" "r"))]
  ""
  "igoto %0"
[(set_attr "type" "bcu")
 (set_attr "class" "jump")]
)

(define_expand "tablejump"
  [(set (pc)
	(match_operand:SI 0 "register_operand"))
   (use (label_ref (match_operand 1 "")))]
  "can_create_pseudo_p ()"
{
  kvx_expand_tablejump (operands[0], operands[1]);
  DONE;
})

(define_insn "tablejump_real_<mode>"
  [(parallel [(set (pc)
                   (match_operand:P 0 "register_operand" "r"))
              (use (label_ref (match_operand 1 "")))]
  )]
  "<MODE>mode == Pmode"
  "igoto %0"
[(set_attr "type" "bcu")
 (set_attr "class" "jump")]
)

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop"
[(set_attr "type" "all")]
)

(define_insn "addsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (plus:SI (match_operand:SI 1 "register_operand" "r,r,r")
                 (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "addw %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
 (set_attr "length" "4,4,8")]
)

;; zero-extend version of the above insn
(define_insn "*addsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI
             (plus:SI (match_operand:SI 1 "register_operand" "r,r,r")
	              (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "addw %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
 (set_attr "length" "4,4,8")])

(define_insn "addcd"
  [(set (match_operand:DI 0 "register_operand" "=r")
	(plus:DI (plus:DI (match_operand:DI 1 "register_operand" "r")
		          (match_operand:DI 2 "register_operand" "r"))
                 (unspec:DI [(reg:SI KV3_CS_REGNO)] UNSPEC_GETCARRY)))
   (set (reg:SI KV3_CS_REGNO) (unspec:SI [(reg:SI KV3_CS_REGNO)
                                          (match_dup 1)
                                          (match_dup 2)] UNSPEC_SETCARRY))
  ]
  ""
  "addcd %0 = %1, %2"
[(set_attr "type" "alu_full")
 (set_attr "length" "4")]
)

(define_insn "addcid"
  [(set (match_operand:DI 0 "register_operand" "=r")
	(plus:DI (match_operand:DI 1 "register_operand" "r")
		 (match_operand:DI 2 "register_operand" "r")))
   (set (reg:SI KV3_CS_REGNO) (unspec:SI [(match_dup 1)
                                      (match_dup 2)] UNSPEC_SETCARRY))
  ]
  ""
  "addcid %0 = %1, %2"
[(set_attr "type" "alu_full")
 (set_attr "length" "4")])

(define_insn "adddi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
	(plus:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
		 (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
""
"addd %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
 (set_attr "length" "4,4,8,12")])

(define_insn "sub<mode>3_x2"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
	(minus:SIDI (match_operand:SIDI 1 "register_operand" "r")
                    (ashift:SIDI (match_operand:SIDI 2 "register_operand" "r")
                                 (const_int 1))))]
  ""
  "sbfx2<SIDI:suffix> %0 = %2, %1"
[(set_attr "type" "alu_lite")
 (set_attr "length" "4")])

(define_insn "sub<mode>3_mult_x2"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
	(minus:SIDI (match_operand:SIDI 1 "register_operand" "r")
                    (mult:SIDI (match_operand:SIDI 2 "register_operand" "r")
		               (const_int 2))))]
  ""
  "sbfx2<SIDI:suffix> %0 = %2, %1"
[(set_attr "type" "alu_lite")
 (set_attr "length" "4")])

(define_insn "sub<mode>3_x4"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
	(minus:SIDI (match_operand:SIDI 1 "register_operand" "r")
	            (ashift:SIDI (match_operand:SIDI 2 "register_operand" "r")
		                 (const_int 2))))]
  ""
  "sbfx4<SIDI:suffix> %0 = %2, %1"
[(set_attr "type" "alu_lite")
 (set_attr "length" "4")])

(define_insn "sub<mode>3_mult_x4"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
	(minus:SIDI (match_operand:SIDI 1 "register_operand" "r")
	            (mult:SIDI (match_operand:SIDI 2 "register_operand" "r")
		               (const_int 4))))]
  ""
  "sbfx4<SIDI:suffix> %0 = %2, %1"
[(set_attr "type" "alu_lite")
 (set_attr "length" "4")])

(define_insn "sub<mode>3_x8"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
	(minus:SIDI (match_operand:SIDI 1 "register_operand" "r")
	            (ashift:SIDI (match_operand:SIDI 2 "register_operand" "r")
		                 (const_int 3))))]
  ""
  "sbfx8<SIDI:suffix> %0 = %2, %1"
[(set_attr "type" "alu_lite")
 (set_attr "length" "4")])

(define_insn "sub<mode>3_mult_x8"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
	(minus:SIDI (match_operand:SIDI 1 "register_operand" "r")
	            (mult:SIDI (match_operand:SIDI 2 "register_operand" "r")
		               (const_int 8))))]
  ""
  "sbfx8<SIDI:suffix> %0 = %2, %1"
[(set_attr "type" "alu_lite")
 (set_attr "length" "4")])

(define_insn "sub<mode>3_x16"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
	(minus:SIDI (match_operand:SIDI 1 "register_operand" "r")
	            (ashift:SIDI (match_operand:SIDI 2 "register_operand" "r")
		                 (const_int 4))))]
  ""
  "sbfx16<SIDI:suffix> %0 = %2, %1"
[(set_attr "type" "alu_lite")
 (set_attr "length" "4")])

(define_insn "sub<mode>3_mult_x16"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
	(minus:SIDI (match_operand:SIDI 1 "register_operand" "r")
	            (mult:SIDI (match_operand:SIDI 2 "register_operand" "r")
		               (const_int 16))))]
  ""
  "sbfx16<SIDI:suffix> %0 = %2, %1"
[(set_attr "type" "alu_lite")
 (set_attr "length" "4")])

(define_insn "subsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
	(minus:SI (match_operand:SI 1 "kvx_r_s10_s37_s64_operand" "r,I10,i")
	          (match_operand:SI 2 "register_operand" "r,r,r")))]
  ""
  "sbfw %0 = %2, %1"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
 (set_attr "length" "4,4,8")])

;; zero-extend version of the above insn
(define_insn "*subsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI
             (minus:SI (match_operand:SI 1 "kvx_r_s10_s37_s64_operand" "r,I10,i")
                       (match_operand:SI 2 "register_operand" "r,r,r"))))]
  ""
  "sbfw %0 = %2, %1"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
 (set_attr "length" "4,4,8")])

(define_insn "subdi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
	(minus:DI (match_operand:DI 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
 		  (match_operand:DI 2 "register_operand" "r,r,r,r")))]
  ""
  "sbfd %0 = %2, %1"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
 (set_attr "length" "4,4,8,12")])

;; MAU ISA

(define_insn "mulsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (mult:SI (match_operand:SI 1 "register_operand" "r,r")
                 (match_operand:SI 2 "kvx_r_any32_operand" "r,i")))]
 ""
 "mulw %0 = %1, %2"
[(set_attr "type" "mau, mau_x")
 (set_attr "length" "4, 8")])

(define_insn "muldi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (mult:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                 (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
 ""
 "muld %0 = %1, %2"
[(set_attr "type" "mau, mau, mau_x, mau_y")
 (set_attr "length" "4, 4, 8, 12")])

(define_expand "<ssfx>mulsidi3"
  [(set (match_operand:DI 0 "register_operand" "")
        (mult:DI (ANY_EXTEND:DI (match_operand:SI 1 "register_operand" ""))
                 (ANY_EXTEND:DI (match_operand:SI 2 "register_operand" ""))))]
  ""
)

(define_insn "*<ssfx>mulsidi3_reg"
  [(set (match_operand:DI 0 "register_operand" "=r")
	(mult:DI (ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r"))
		 (ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "mul<ssfx>wd %0 = %1, %2"
[(set_attr "type" "mau")
 (set_attr "length" "4")])

(define_expand "usmulsidi3"
  [(set (match_operand:DI 0 "register_operand" "")
	(mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" ""))
		 (sign_extend:DI (match_operand:SI 2 "register_operand" ""))))]
  ""
)

(define_insn "*usmulsidi3_reg"
  [(set (match_operand:DI 0 "register_operand" "=r")
	(mult:DI (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
		 (sign_extend:DI (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "mulsuwd %0 = %2, %1"
[(set_attr "type" "mau")
 (set_attr "length" "4")]
)

;; multiply/add

(define_insn "madd<mode><mode>4"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (plus:SIDI (mult:SIDI (match_operand:SIDI 1 "register_operand" "r")
                              (match_operand:SIDI 2 "register_operand" "r"))
                   (match_operand:SIDI 3 "register_operand" "0")))]
  ""
  "madd<SIDI:sfx> %0 = %1, %2"
[(set_attr "type" "mau_auxr")
 (set_attr "length" "4")])

(define_expand "<ssfx>maddsidi4"
   [(set (match_operand:DI 0 "register_operand" "")
         (plus:DI (mult:DI (ANY_EXTEND:DI (match_operand:SI 1 "register_operand" ""))
                           (ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "")))
                  (match_operand:DI 3 "register_operand" "")))]
  ""
 )

(define_insn "*<ssfx>maddsidi4_reg"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (plus:DI (mult:DI (ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r"))
                          (ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r")))
                 (match_operand:DI 3 "register_operand" "0")))]
  ""
  "madd<ssfx>wd %0 = %1, %2"
[(set_attr "type" "mau_auxr")
 (set_attr "length" "4")])

;; multiply/subtract

(define_insn "msub<mode><mode>4"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
  	(minus:SIDI (match_operand:SIDI 3 "register_operand" "0")
                    (mult:SIDI (match_operand:SIDI 1 "register_operand" "r")
                               (match_operand:SIDI 2 "register_operand" "r"))))]

  ""
  "msbf<SIDI:sfx> %0 = %1, %2"
[(set_attr "type" "mau_auxr")
 (set_attr "length" "4")])

(define_insn "*<ssfx>msubsidi4_reg"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (minus:DI (match_operand:DI 3 "register_operand" "0")
                  (mult:DI (ANY_EXTEND:DI (match_operand:SI 1 "register_operand" "r"))
                           (ANY_EXTEND:DI (match_operand:SI 2 "register_operand" "r")))))]
  ""
  "msbf<ssfx>wd %0 = %1, %2"
[(set_attr "type" "mau_auxr")
 (set_attr "length" "4")])

;; max/min

(define_insn "<spfx>min<mode>3"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r,r,r")
	(MIN_UMIN:SIDI (match_operand:SIDI 1 "register_operand" "r,r,r,r")
                       (match_operand:SIDI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "min<ssfx><sfx> %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
 (set_attr "length" "4,4,8,12")])

(define_insn "<spfx>max<mode>3"
  [(set (match_operand:SIDI 0 "register_operand" "=r,r,r,r")
	(MAX_UMAX:SIDI (match_operand:SIDI 1 "register_operand" "r,r,r,r")
                       (match_operand:SIDI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "max<ssfx><sfx> %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
 (set_attr "length" "4,4,8,12")])

(define_insn "*zxh_and"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (match_operand:SI 1 "register_operand" "r")
                (const_int 65535) ))]
  ""
  "zxhd %0 = %1"
[(set_attr "type" "alu_lite")]
)

(define_insn "andsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (and:SI (match_operand:SI 1 "register_operand" "r,r,r")
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "andw %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
 (set_attr "length" "4,4,8")]
)

(define_insn "*nandw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (ior:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                (not:SI (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "nandw %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
 (set_attr "length" "4,4,8")]
)

(define_insn "*andnw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (and:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "andnw %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
 (set_attr "length" "4,4,8")]
)

(define_insn "anddi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (and:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "andd %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
 (set_attr "length" "4,4,8,12")]
)

(define_insn "*nandd"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (ior:DI (not:DI (match_operand:DI 1 "register_operand" "r,r,r,r"))
                (not:DI (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i"))))]
  ""
  "nandd %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
 (set_attr "length" "4,4,8,12")]
)

(define_insn "*andnd"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (and:DI (not:DI (match_operand:DI 1 "register_operand" "r,r,r,r"))
                (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "andnd %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
 (set_attr "length" "4,4,8,12")]
)

(define_insn "iorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (ior:SI (match_operand:SI 1 "register_operand" "r,r,r")
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "orw %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
 (set_attr "length" "4,4,8")]
)

(define_insn "*norw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (and:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                (not:SI (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "norw %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
 (set_attr "length" "4,4,8")]
)

(define_insn "*ornw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (ior:SI (not:SI (match_operand:SI 1 "register_operand" "r,r,r"))
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "ornw %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
 (set_attr "length" "4,4,8")]
)

(define_insn "iordi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (ior:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "ord %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
 (set_attr "length" "4,4,8,12")]
)

(define_insn "*nord"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (and:DI (not:DI (match_operand:DI 1 "register_operand" "r,r,r,r"))
                (not:DI (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i"))))]
  ""
  "nord %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
 (set_attr "length" "4,4,8,12")]
)

(define_insn "*ornd"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (ior:DI (not:DI (match_operand:DI 1 "register_operand" "r,r,r,r"))
                (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "ornd %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
 (set_attr "length" "4,4,8,12")]
)

(define_insn "xorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (xor:SI (match_operand:SI 1 "register_operand" "r,r,r")
                (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i")))]
  ""
  "xorw %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
 (set_attr "length" "4,4,8")]
)

(define_insn "*nxorw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (not:SI (xor:SI (match_operand:SI 1 "register_operand" "r,r,r")
                        (match_operand:SI 2 "kvx_r_s10_s37_s64_operand" "r,I10,i"))))]
  ""
  "nxorw %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x")
 (set_attr "length" "4,4,8")]
)

(define_insn "xordi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (xor:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "xord %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
 (set_attr "length" "4,4,8,12")]
)

(define_insn "*nxord"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (not:DI (xor:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                        (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i"))))]
  ""
  "nxord %0 = %1, %2"
[(set_attr "type" "alu_tiny,alu_tiny,alu_tiny_x,alu_tiny_y")
 (set_attr "length" "4,4,8,12")]
)

(define_insn "ashlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (ashift:SI (match_operand:SI 1 "register_operand" "r,r")
                   (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sllw %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)

(define_insn "ssashlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (ss_ashift:SI (match_operand:SI 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "slsw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "     4,       4")]
)

(define_insn "ashrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (ashiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "sraw %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)

(define_insn "lshrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (lshiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srlw %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)

(define_insn "ashldi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (ashift:DI (match_operand:DI 1 "register_operand" "r,r")
                   (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "slld %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)

(define_insn "ssashldi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (ss_ashift:DI (match_operand:DI 1 "register_operand" "r,r")
                      (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "slsd %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)

(define_insn "ashrdi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (ashiftrt:DI (match_operand:DI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srad %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)

(define_insn "lshrdi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (lshiftrt:DI (match_operand:DI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "sat_shift_operand" "r,U06")))]
  ""
  "srld %0 = %1, %2"
  [(set_attr "type" "alu_tiny,alu_tiny")
   (set_attr "length" "     4,       4")]
)

(define_insn "rol"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (rotate:SI (match_operand:SI 1 "register_operand" "r,r")
                   (match_operand:SI 2 "rotate_operand" "r,U05")))]
  ""
  "rolw %0 = %1, %2"
[(set_attr "type" "alu_lite,alu_lite")])

(define_insn "ror"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (rotatert:SI (match_operand:SI 1 "register_operand" "r,r")
                     (match_operand:SI 2 "rotate_operand" "r,U05")))]
  ""
  "rorw %0 = %1, %2"
[(set_attr "type" "alu_lite,alu_lite")
])

(define_expand "rotrsi3"
  [(set (match_operand:SI 0 "register_operand" "")
	(rotatert:SI (match_operand:SI 1 "register_operand" "")
	  	     (match_operand:SI 2 "rotate_operand" "")))]
""
)

(define_expand "rotlsi3"
  [(set (match_operand:SI 0 "register_operand" "")
	(rotate:SI (match_operand:SI 1 "register_operand" "")
	           (match_operand:SI 2 "rotate_operand" "")))]
  ""
)

(define_insn "abs<mode>2"
  [(set (match_operand:SIDI           0 "register_operand" "=r")
        (abs:SIDI (match_operand:SIDI 1 "register_operand" "r")))]
  ""
  "abs<SIDI:suffix> %0 = %1"
  [(set_attr "type" "alu_lite")
   (set_attr "length" "4")]
)

;; ctzw/ctzd clzw/clzd
(define_insn "c<c_tl>z<mode>2"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (ANY_ZERO_COUNT:SIDI (match_operand:SIDI 1 "register_operand" "r")))]
  ""
  "c<c_tl>z<sfx> %0 = %1"
  [(set_attr "type" "alu_lite")
   (set_attr "length" "4")]
)

(define_insn "popcountsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (popcount:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "cbsw %0 = %1"
  [(set_attr "type" "alu_lite")
   (set_attr "length" "4")]
)

(define_insn "popcountdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (popcount:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "cbsd %0 = %1"
  [(set_attr "type" "alu_lite")
   (set_attr "length" "4")]
)

(define_insn "one_cmplsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (not:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "notw %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "one_cmpldi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (not:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "notd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "negsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (neg:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "negw %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "negdi2"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (neg:DI (match_operand:DI 1 "register_operand" "r")))]
  ""
  "negd %0 = %1"
  [(set_attr "type" "alu_tiny")]
)

(define_insn "extv<mode>"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
	(sign_extract:SIDI (match_operand:SIDI 1 "register_operand" "r")
                           (match_operand:SIDI 2 "sixbits_unsigned_operand" "i")
                           (match_operand:SIDI 3 "sixbits_unsigned_operand" "i")))]
  ""
  "extfs %0 = %1, %2+%3-1, %3"
  [(set_attr "type" "alu_lite")
])

(define_insn "extzv<mode>"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
	(zero_extract:SIDI (match_operand:SIDI 1 "register_operand" "r")
                           (match_operand:SIDI 2 "sixbits_unsigned_operand" "i")
                           (match_operand:SIDI 3 "sixbits_unsigned_operand" "i")))]
  ""
  "extfz %0 = %1, %2+%3-1, %3"
  [(set_attr "type" "alu_lite")]
)

(define_insn "insv<mode>"
  [(set (zero_extract:SIDI (match_operand:SIDI 0 "register_operand" "+r")
                           (match_operand:SIDI 1 "sixbits_unsigned_operand" "i")
                           (match_operand:SIDI 2 "sixbits_unsigned_operand" "i"))
        (match_operand:SIDI 3 "register_operand" "r"))]
  ""
  "insf %0 = %3, %1+%2-1, %2"
  [(set_attr "type" "alu_lite")]
)

;; FIXME AUTO: Add immediate variants. Current ISA only supports 32bit imm.
(define_insn "*addx2<lsusize>"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (plus:SIDI (mult:SIDI (match_operand:SIDI 1 "register_operand" "r")
                              (const_int 2))
                (match_operand:SIDI 2 "register_operand" "r")
                ))]
  ""
  "addx2<lsusize> %0 = %1, %2"
[(set_attr "type" "alu_lite")]
)

;; FIXME AUTO: Add immediate variants. Current ISA only supports 32bit imm.
(define_insn "*addx2<lsusize>_2"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (plus:SIDI (ashift:SIDI (match_operand:SIDI 1 "register_operand" "r")
                                (const_int 1))
                   (match_operand:SIDI 2 "register_operand" "r")
                ))]
  ""
  "addx2<lsusize> %0 = %1, %2"
[(set_attr "type" "alu_lite")]
)

;; FIXME AUTO: Add immediate variants. Current ISA only supports 32bit imm.
(define_insn "*addx4<lsusize>"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (plus:SIDI (mult:SIDI (match_operand:SIDI 1 "register_operand" "r")
                              (const_int 4))
                (match_operand:SIDI 2 "register_operand" "r")
                ))]
  ""
  "addx4<lsusize> %0 = %1, %2"
[(set_attr "type" "alu_lite")]
)

;; FIXME AUTO: Add immediate variants. Current ISA only supports 32bit imm.
(define_insn "*addx4<lsusize>_2"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (plus:SIDI (ashift:SIDI (match_operand:SIDI 1 "register_operand" "r")
                                (const_int 2))
                   (match_operand:SIDI 2 "register_operand" "r")
                ))]
  ""
  "addx4<lsusize> %0 = %1, %2"
[(set_attr "type" "alu_lite")]
)

;; FIXME AUTO: Add immediate variants. Current ISA only supports 32bit imm.
(define_insn "*addx8<lsusize>"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (plus:SIDI (mult:SIDI (match_operand:SIDI 1 "register_operand" "r")
                              (const_int 8))
                (match_operand:SIDI 2 "register_operand" "r")
                ))]
  ""
  "addx8<lsusize> %0 = %1, %2"
[(set_attr "type" "alu_lite")]
)

;; FIXME AUTO: Add immediate variants. Current ISA only supports 32bit imm.
(define_insn "*addx8<lsusize>_2"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (plus:SIDI (ashift:SIDI (match_operand:SIDI 1 "register_operand" "r")
                                (const_int 3))
                   (match_operand:SIDI 2 "register_operand" "r")
                ))]
  ""
  "addx8<lsusize> %0 = %1, %2"
[(set_attr "type" "alu_lite")]
)

;; FIXME AUTO: Add immediate variants. Current ISA only supports 32bit imm.
(define_insn "*addx16<lsusize>"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (plus:SIDI (mult:SIDI (match_operand:SIDI 1 "register_operand" "r")
                              (const_int 16))
                (match_operand:SIDI 2 "register_operand" "r")
                ))]
  ""
  "addx16<lsusize> %0 = %1, %2"
[(set_attr "type" "alu_lite")]
)

;; FIXME AUTO: Add immediate variants. Current ISA only supports 32bit imm.
(define_insn "*addx16<lsusize>_2"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
        (plus:SIDI (ashift:SIDI (match_operand:SIDI 1 "register_operand" "r")
                                (const_int 4))
                   (match_operand:SIDI 2 "register_operand" "r")
                ))]
  ""
  "addx16<lsusize> %0 = %1, %2"
[(set_attr "type" "alu_lite")]
)

(define_insn "*set_<mode>"
   [(set (match_operand:ALLP 0 "system_register_operand" "=RYY,RXX")
         (match_operand:ALLP 1 "register_operand" "r,r"))]
   ""
   "set %0 = %1"
  [(set_attr "type" "all,bcu")]
)

(define_insn "kvx_set"
   [(set (match_operand:DI 0 "system_register_operand" "=RYY,RXX")
         (unspec_volatile:DI [(match_operand:DI 1 "register_operand" "r,r")] UNSPEC_SET))
    (set (match_operand:SI 2 "" "") 
         (unspec:SI [(match_dup 2)] UNSPEC_SYNC))]
   ""
   "set %0 = %1"
  [(set_attr "type" "all,bcu")]
)

;; Provide a 37bits offset for 32bits and 64bits for 64bits.
;; This may be too much in most of the cases but having smaller
;; values may not work in some cases.
;; Changing this requires changes in Assembler @gotaddr() handling.
(define_insn "set_gotp_<mode>"
   [(set (match_operand 0 "pmode_register_operand" "=r")
         (unspec:P [(const_int 0)] UNSPEC_PIC))]
   ""
   "pcrel %0 = @gotaddr()"
[(set_attr "type" "alu_full<symlen1>")
 (set_attr "length" "<symlen2>")]
)

(define_insn "*get_<mode>"
   [(set (match_operand:ALLP 0 "register_operand" "=r") 
         (match_operand:ALLP 1 "system_register_operand" "RXX"))]
   ""
   "get %0 = %1"
  [(set_attr "type" "bcu_get")]
)

(define_insn "kvx_get"
   [(set (match_operand:DI 0 "register_operand" "=r")
         (unspec_volatile:DI [(match_operand:DI 1 "system_register_operand" "RXX")] UNSPEC_GET))
    (set (match_operand:SI 2 "" "") 
         (unspec:SI [(match_dup 2)] UNSPEC_SYNC))]
   ""
   "get %0 = %1"
[(set_attr "type" "bcu_get")]
)

(define_insn "kvx_wfxl"
   [(set (match_operand:DI 0 "system_register_operand" "=RYY,RXX")
         (unspec_volatile:DI [(match_operand:DI 1 "register_operand" "r,r")] UNSPEC_WFXL))
    (set (match_operand:DI 2 "" "") 
         (unspec:DI [(match_dup 2)] UNSPEC_SYNC))]
   ""
   "wfxl %0, %1"
  [(set_attr "type" "all,bcu")]
)

(define_insn "kvx_wfxm"
   [(set (match_operand:DI 0 "system_register_operand" "=RYY,RXX")
         (unspec_volatile:DI [(match_operand:DI 1 "register_operand" "r,r")] UNSPEC_WFXM))
    (set (match_operand:DI 2 "" "") 
         (unspec:DI [(match_dup 2)] UNSPEC_SYNC))]
   ""
   "wfxm %0, %1"
  [(set_attr "type" "all,bcu")]
)

(define_insn "syncgroup"
   [(unspec [(match_operand:DI 0 "register_operand" "r")] UNSPEC_SYNCGROUP)
    (clobber (mem:BLK (scratch)))
    (set (match_operand:DI 1 "" "")
         (unspec:DI [(match_dup 1)] UNSPEC_SYNC))]
   ""
   "syncgroup %0"
  [(set_attr "type" "bcu")]
)

(define_insn "await"
   [(unspec_volatile [(const_int 0)] UNSPEC_AWAIT)
    (set (match_operand:SI 0 "" "") 
         (unspec:SI [(match_dup 0)] UNSPEC_SYNC))]
   ""
   "await"
[(set_attr "type" "all")]
)

(define_insn "barrier"
   [(unspec_volatile [(const_int 0)] UNSPEC_BARRIER)
    (clobber (mem:BLK (scratch)))
    (set (match_operand:SI 0 "" "") 
         (unspec:SI [(match_dup 0)] UNSPEC_SYNC))]
   ""
   "barrier"
[(set_attr "type" "all")]
)

(define_insn "doze"
   [(unspec_volatile [(const_int 0)] UNSPEC_DOZE)
    (set (match_operand:SI 0 "" "") 
         (unspec:SI [(match_dup 0)] UNSPEC_SYNC))]
   ""
   "doze"
[(set_attr "type" "all")]
)

(define_insn "sleep"
   [(unspec_volatile [(const_int 0)] UNSPEC_SLEEP)
    (set (match_operand:SI 0 "" "") 
         (unspec:SI [(match_dup 0)] UNSPEC_SYNC))]
   ""
   "sleep"
[(set_attr "type" "all")]
)

(define_insn "stop"
   [(unspec_volatile [(const_int 0)] UNSPEC_STOP)
    (set (match_operand:SI 0 "" "") 
         (unspec:SI [(match_dup 0)] UNSPEC_SYNC))]
   ""
   "stop"
[(set_attr "type" "all")]
)

(define_insn "waitit"
   [(set (match_operand:SI 0 "register_operand" "=r")
         (unspec_volatile:SI [(match_operand:SI 1 "register_operand" "0")] UNSPEC_WAITIT))
    (set (match_operand:SI 2 "" "")
         (unspec:SI [(match_dup 2)] UNSPEC_SYNC))]
   ""
   "waitit %0"
  [(set_attr "type" "bcu_get")]
)

(define_insn "trap"
[(trap_if (const_int 1) (const_int 0))]
"kvx_have_stack_checking()"
"errop"
[(set_attr "type" "all")]
)

(define_insn "ctrapsi4"
      [(trap_if (match_operator 0 "comparison_operator"
                [(match_operand 1 "register_operand")
                 (match_operand 2 "immediate_operand")])
                (match_operand 3 "const_int_operand" "i"))]
  "kvx_have_stack_checking()"
  {
    return kvx_ctrapsi4();
  }
[(set_attr "type" "bcu")
 (set_attr "class" "branch")]
)

(define_insn "scall_<mode>"
   [(set (reg:SI 0)
         (call 
	    (mem:SI (unspec:SI [(match_operand 0 "immediate_operand" "i")] UNSPEC_SCALL))
	    (const_int 0)))
    
    (use (reg:SI 0))
    (use (reg:SI 1))
    (use (reg:SI 2))
    (use (reg:SI 3))
    (use (reg:SI 4))
    (use (reg:SI 5))
    (use (reg:SI 6))
    (use (reg:SI 7))
    (clobber (match_operand:P 1 "system_register_operand" "=RXX")) /* LINK_REG */
    (clobber (mem:BLK (scratch)))
    (set (match_operand:SI 2 "" "") 
         (unspec:SI [(match_dup 2)] UNSPEC_SYNC))]
   ""
   "scall %0"
[(set_attr "type" "all")]
)

(define_insn "mos_dinval_scall"
   [(unspec_volatile:SI [(const_int 1026)] UNSPEC_SCALL)
    (clobber (reg:SI 0))
    (clobber (reg:SI 1))
    (clobber (reg:SI 2))
    (clobber (reg:SI 3))
    (clobber (reg:SI 4))
    (clobber (reg:SI 5))
    (clobber (reg:SI 6))
    (clobber (reg:SI 7))
    (clobber (reg:SI 8))
    (clobber (reg:SI 9))
    (clobber (reg:SI 11))
    (clobber (reg:SI 32))
    (clobber (reg:SI 33))
    (clobber (reg:SI 34))
    (clobber (reg:SI 35))
    (clobber (mem:BLK (scratch)))]
    ""
   "scall 1056"
 [(set_attr "type" "all")
  ]
 )

(define_insn "mos_dflush_scall"
   [(unspec_volatile:SI [(const_int 1027)] UNSPEC_SCALL)
    (clobber (reg:SI 0))
    (clobber (reg:SI 1))
    (clobber (reg:SI 2))
    (clobber (reg:SI 3))
    (clobber (reg:SI 4))
    (clobber (reg:SI 5))
    (clobber (reg:SI 6))
    (clobber (reg:SI 7))
    (clobber (reg:SI 8))
    (clobber (reg:SI 9))
    (clobber (reg:SI 11))
    (clobber (reg:SI 32))
    (clobber (reg:SI 33))
    (clobber (reg:SI 34))
    (clobber (reg:SI 35))
    (clobber (mem:BLK (scratch)))]
    ""
   "scall 1065"
 [(set_attr "type" "all")
  ]
)

(define_insn "fence"
   [(unspec_volatile [(const_int 0)] UNSPEC_FENCE)
    (clobber (mem:BLK (scratch)))
    (set (match_operand:SI 0 "" "") 
         (unspec:SI [(match_dup 0)] UNSPEC_SYNC))]
   ""
   "fence"
[(set_attr "type" "lsu")]
)

(define_insn "dinval"
   [(unspec [(const_int 0)] UNSPEC_DINVAL)
    (clobber (mem:BLK (scratch)))
    (set (match_operand:SI 0 "" "") 
         (unspec:SI [(match_dup 0)] UNSPEC_SYNC))]
   ""
   "dinval"
[(set_attr "type" "lsu")]
)

(define_insn "iinval"
   [(unspec_volatile [(const_int 0)] UNSPEC_IINVAL)
    (clobber (mem:BLK (scratch)))
    (set (match_operand:SI 0 "" "") 
         (unspec:SI [(match_dup 0)] UNSPEC_SYNC))]
   ""
   "iinval "
[(set_attr "type" "lsu")]
)

(define_insn "dinvall"
   [(unspec [(match_operand:SI 0 "memory_operand" "a, b, m")] UNSPEC_DINVALL)
    (clobber (mem:BLK (scratch)))
    (set (match_operand:SI 1 "" "")
         (unspec:SI [(match_dup 1)] UNSPEC_SYNC))]
   ""
   "dinvall%m0 %0"
[(set_attr "length" "  4,     8,    12")
 (set_attr "type"   "lsu, lsu_x, lsu_y")]
)

(define_insn "iinvals"
   [(unspec_volatile [(match_operand:SI 0 "memory_operand" "a,b,m")] UNSPEC_IINVALS)
    (clobber (mem:BLK (scratch)))
    (set (match_operand:SI 1 "" "") 
         (unspec:SI [(match_dup 1)] UNSPEC_SYNC))]
   ""
   "iinvals%m0 %0"
[(set_attr "length" "  4,    8,     12")
 (set_attr "type"   "lsu, lsu_x, lsu_y")]
)

(define_insn "dtouchl_<mode>"
  [(prefetch (match_operand:P 0 "address_operand" "Aa,Ab,p")
             (match_operand:SI 1 "" "")
             (match_operand:SI 2 "" ""))]
   ""
   "dtouchl%m0 %a0"
[(set_attr "length" "  4,     8,    12")
(set_attr "type"    "lsu, lsu_x, lsu_y")]
)

(define_expand "prefetch"
  [(prefetch (match_operand    0 "address_operand" "p")
             (match_operand:SI 1 "" "")
             (match_operand:SI 2 "" ""))]
   ""
{
        rtx (*gen_dtouchl) (rtx target, rtx op1, rtx op2) = (!TARGET_32) ? gen_dtouchl_di : gen_dtouchl_si;
	rtx addr = force_reg (Pmode, operands[0]);
	emit_insn (gen_dtouchl(addr, operands[1], operands[2]));
	DONE;
})

(define_insn "dzerol"
   [(unspec [(match_operand:SI 0 "memory_operand" "a,b,m")] UNSPEC_DZEROL)
    (clobber (mem:BLK (scratch)))
    (set (match_operand:SI 1 "" "") 
         (unspec:SI [(match_dup 1)] UNSPEC_SYNC))]
   ""
   "dzerol%m0 %0"
[(set_attr "length" "  4,     8,    12")
 (set_attr "type"   "lsu, lsu_x, lsu_y")]
)

(define_insn "tlbdinval"
   [(unspec_volatile [(const_int 0)] UNSPEC_TLBDINVAL)
    (clobber (mem:BLK (scratch)))
    (set (match_operand:SI 0 "" "") 
         (unspec:SI [(match_dup 0)] UNSPEC_SYNC))]
   ""
   "tlbdinval "
[(set_attr "type" "all")]
)

(define_insn "tlbiinval"
   [(unspec_volatile [(const_int 0)] UNSPEC_TLBIINVAL)
    (clobber (mem:BLK (scratch)))
    (set (match_operand:SI 0 "" "") 
         (unspec:SI [(match_dup 0)] UNSPEC_SYNC))]
   ""
   "tlbiinval "
[(set_attr "type" "all")]
)

(define_insn "tlbprobe"
   [(unspec_volatile [(const_int 0)] UNSPEC_TLBPROBE)
    (clobber (mem:BLK (scratch)))
    (set (match_operand:SI 0 "" "") 
         (unspec_volatile [(match_dup 0)] UNSPEC_SYNC))]
   ""
   "tlbprobe "
[(set_attr "type" "all")]
)

(define_insn "tlbread"
   [(unspec_volatile [(const_int 0)] UNSPEC_TLBREAD)
    (clobber (mem:BLK (scratch)))
    (set (match_operand:SI 0 "" "") 
         (unspec:SI [(match_dup 0)] UNSPEC_SYNC))]
   ""
   "tlbread "
[(set_attr "type" "all")]
)

(define_insn "tlbwrite"
   [(unspec_volatile [(const_int 0)] UNSPEC_TLBWRITE)
    (clobber (mem:BLK (scratch)))
    (set (match_operand:SI 0 "" "") 
         (unspec:SI [(match_dup 0)] UNSPEC_SYNC))]
   ""
   "tlbwrite "
[(set_attr "type" "all")]
)

(define_expand "memory_barrier"
  [(clobber (mem:BLK (scratch)))]
  ""
  {
  	emit_insn (gen_dinval (kvx_sync_reg_rtx));
	emit_insn (gen_fence (kvx_sync_reg_rtx));
  }
)

(define_insn "lqu"
   [(set (match_operand:TI 0 "register_operand" "=r,r,r")
         (unspec:TI [(match_operand:TI 1 "memory_operand" "a,b,m")] UNSPEC_LQU))
    (clobber (mem:BLK (scratch)))
   ]
   ""
   "lq.u%m1 %0 = %1"
[(set_attr "length" "4, 8, 12")
 (set_attr "type"   "lsu_auxw_load_uncached, lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")]
)

(define_insn "ldu"
   [(set (match_operand:DI 0 "register_operand" "=r,r,r")
         (unspec:DI [(match_operand:DI 1 "memory_operand" "a,b,m")] UNSPEC_LDU))
    (clobber (mem:BLK (scratch)))
   ]
   ""
   "ld.u%m1 %0 = %1"
[(set_attr "length" "4, 8, 12")
 (set_attr "type" "lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxw_load_uncached_y")]
)

(define_insn "lwzu"
   [(set (match_operand:SI 0 "register_operand" "=r,r,r")
         (unspec:SI [(match_operand:SI 1 "memory_operand" "a,b,m")] UNSPEC_LWZU))
    (clobber (mem:BLK (scratch)))
   ]
   ""
   "lwz.u%m1 %0 = %1"
[(set_attr "length" "4,8,12")
 (set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")]
)

(define_insn "lhsu"
   [(set (match_operand:HI 0 "register_operand" "=r,r,r")
         (unspec:HI [(match_operand:HI 1 "memory_operand" "a,b,m")] UNSPEC_LHSU))
    (clobber (mem:BLK (scratch)))
   ]
   ""
   "lhs.u%m1 %0 = %1"
[(set_attr "length" "4, 8, 12")
 (set_attr "type" "lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxw_load_uncached_y")]
)

(define_insn "lhzu"
   [(set (match_operand:HI 0 "register_operand" "=r,r,r")
         (unspec:HI [(match_operand:HI 1 "memory_operand" "a,b,m")] UNSPEC_LHZU))
    (clobber (mem:BLK (scratch)))
   ]
   ""
   "lhz.u%m1 %0 = %1"
[(set_attr "length" "4, 8, 12")
 (set_attr "type" "lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxw_load_uncached_y")]
)

;; FIXME AUTO: add size info for 'reg[reg]' addressing (currently falling back to lsu.x)
(define_insn "*l<mode><ANY_EXTEND:lsext>"
   [(set (match_operand:DI 0 "register_operand"                       "=r,  r,  r,  r,  r,  r")
         (ANY_EXTEND:DI (match_operand:ALL_SMALL_I 1 "memory_operand" "Ca, Cb, Cm, Za, Zb, Zm")))]
   ""
   "l<ALL_SMALL_I:lsusize><ANY_EXTEND:lsext>%C1%m1 %0 = %1"
[(set_attr "length" "            4,               8,              12,                      4,                        8,                       12")
 (set_attr "type"   "lsu_auxw_load, lsu_auxw_load_x, lsu_auxw_load_y, lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxw_load_uncached_y")]
)

(define_insn "lbsu"
   [(set (match_operand:QI 0 "register_operand"           "=r, r, r")
         (unspec:QI [(match_operand:QI 1 "memory_operand" " a, b, m")] UNSPEC_LBSU))
    (clobber (mem:BLK (scratch)))
   ]
   ""
   "lbs.u%m1 %0 = %1"
[(set_attr "length" "4,8,12")
 (set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")]
)

(define_insn "lbzu"
   [(set (match_operand:QI 0 "register_operand" "=r,r,r")
         (unspec:QI [(match_operand:QI 1 "memory_operand" "a,b,m")] UNSPEC_LBZU))
    (clobber (mem:BLK (scratch)))
   ]
   ""
   "lbz.u%m1 %0 = %1"
[(set_attr "length" "4,8,12")
 (set_attr "type" "lsu_auxw_load_uncached,lsu_auxw_load_uncached_x,lsu_auxw_load_uncached_y")]
)

;; FIXME AUTO: add size info for 'reg[reg]' addressing (currently falling back to lsu.x)
(define_insn "extend<mode>di2"
  [(set (match_operand:DI 0 "register_operand"                              "=r,  r,  r,  r,  r,  r,  r")
	(sign_extend:DI (match_operand:ALL_SMALL_I 1 "nonimmediate_operand" " r, Ca, Cb, Cm, Za, Zb, Zm")))]
  ""
{
 switch (which_alternative)
   {
   case 0:
     return "sx<lsusize>d %0 = %1";
   case 1: case 2: case 3:
     return "l<lsusize>s%m1 %0 = %1";
   case 4: case 5: case 6:
     return "l<lsusize>s%m1.u %0 = %1";
   default:
     gcc_unreachable ();
   }
}
  [(set_attr "type"   "alu_lite, lsu_auxw_load, lsu_auxw_load_x, lsu_auxw_load_y, lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxw_load_uncached_y")
   (set_attr "length" "       4,             4,               8,              12,                      4,                        8,                       12")])

(define_insn "zero_extend<mode>di2"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r,r,r,r")
	(zero_extend:DI (match_operand:ALL_SMALL_I 1 "nonimmediate_operand" "r,Ca,Cb,Cm,Za,Zb,Zm")))]
  ""
{
 switch (which_alternative)
   {
     case 0:
       return "zx<lsusize>d %0 = %1";
     case 1: case 2: case 3:
       return "l<lsusize>z%m1 %0 = %1";
     case 4: case 5: case 6:
       return "l<lsusize>z%m1.u %0 = %1";
     default:
       gcc_unreachable ();
   }
}
  [(set_attr "type"   "alu_lite, lsu_auxw_load, lsu_auxw_load_x, lsu_auxw_load_y, lsu_auxw_load_uncached, lsu_auxw_load_uncached_x, lsu_auxw_load_uncached_y")
   (set_attr "length" "       4,             4,               8,              12,                      4,                        8,                       12")])

(define_insn "stsuw"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r")
                    (match_operand:SI 2 "register_operand" "r")] UNSPEC_STSUW))]
  ""
  "stsuw %0 = %1, %2"
  [(set_attr "type" "alu_lite")
   (set_attr "length" "4")]
)

(define_insn "stsud"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (unspec:DI [(match_operand:DI 1 "register_operand" "r")
                    (match_operand:DI 2 "register_operand" "r")] UNSPEC_STSUD))]
  ""
  "stsud %0 = %1, %2"
  [(set_attr "type" "alu_lite")
   (set_attr "length" "4")]
)

(define_expand "bswapdi2"
  [(set (match_operand:DI 0 "register_operand")
        (bswap:DI (match_operand:DI 1 "kvx_r_s10_s37_s64_operand")))]
  ""
{
	/* multiply by transposed identity */
	rtx inv_mat = gen_rtx_CONST_INT(VOIDmode, 0x0102040810204080);
	emit_insn(gen_sbmm8(operands[0], operands[1], inv_mat));
	DONE;
})

(define_insn "sbmm8"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
	(unspec:DI [(match_operand:DI 1 "register_operand" "r,r,r,r")
		    (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")] UNSPEC_SBMM8))]
""
"sbmm8 %0 = %1, %2"
[(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
 (set_attr "length" "4,4,8,12")])

(define_insn "sbmmt8"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
	(unspec:DI [(match_operand:DI 1 "register_operand" "r,r,r,r")
		    (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,I37,i")] UNSPEC_SBMMT8))]
""
"sbmmt8 %0 = %1, %2"
[(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
 (set_attr "length" "4,4,8,12")])

(define_insn "*icall_<mode>"
  [(call (mem:P (match_operand:P 0 "register_operand" "r"))
         (match_operand 1 "general_operand" ""))
   (clobber (reg:DI KV3_RA_REGNO))]
  ""
  "icall %0"
[(set_attr "type" "bcu")
 (set_attr "class" "link")]
)

(define_expand "call"
  [(parallel [(call (match_operand 0 "memory_operand" "")
		    (match_operand 1 "general_operand" ""))
	      (clobber (reg:DI KV3_RA_REGNO))])]
  ""
{
  rtx callee = XEXP (operands[0], 0);
  if (!jump_operand (operands[0], Pmode))
      XEXP (operands[0], 0) = force_reg (Pmode, callee);
})

(define_insn "*call_<mode>"
  [(call (match_operand:P 0 "jump_operand" "")
         (match_operand 1 "" ""))
   (clobber (reg:DI KV3_RA_REGNO))]
  ""
  "call %0"
[(set_attr "type" "bcu")
 (set_attr "class" "call")])

(define_expand "call_value"
  [(parallel [(set (match_operand 0 "" "")
		   (call (match_operand 1 "memory_operand" "")
			 (match_operand 2 "general_operand" "")))
	      (clobber (reg:DI KV3_RA_REGNO))])]
  ""
{
  rtx callee = XEXP (operands[1], 0);
  if (!jump_operand (operands[1], Pmode))
      XEXP (operands[1], 0) = force_reg (Pmode, callee);
})

(define_insn "*call_value_<mode>"
  [(set (match_operand 0 "" "")
        (call (match_operand:P 1 "jump_operand" "")
              (match_operand 2 "" "")))
   (clobber (reg:DI KV3_RA_REGNO))]
  ""
  "call %1"
[(set_attr "type" "bcu")
 (set_attr "class" "call")])

(define_expand "sibcall_value"
  [(parallel
    [(set (match_operand 0 "" "")
	  (call (match_operand 1 "memory_operand" "")
	  	(match_operand 2 "general_operand" "")))
     (return)])]
  ""
{
  rtx callee = XEXP (operands[1], 0);
  if (!jump_operand (operands[1], Pmode))
      XEXP (operands[1], 0) = force_reg (Pmode, callee);
})

(define_insn "*sibcall_value_<mode>"
  [(set (match_operand 0 "" "")
        (call (match_operand:P 1 "jump_operand" "")
              (match_operand 2 "" "")))
   (return)]
  ""
  "goto %1"
[(set_attr "type" "bcu")
 (set_attr "class" "branch")])

(define_expand "sibcall"
  [(parallel
    [(call (match_operand 0 "memory_operand" "")
	   (match_operand 1 "general_operand" ""))
     (return)])]
  ""
{
  rtx callee = XEXP (operands[0], 0);
  if (!jump_operand (operands[0], Pmode))
      XEXP (operands[0], 0) = force_reg (Pmode, callee);
})

(define_insn "*sibcall_<mode>"
  [(call (match_operand:P 0 "jump_operand" "")
         (match_operand 1 "" ""))
   (return)]
  ""
  "goto %0"
[(set_attr "type" "bcu")
 (set_attr "class" "branch")])

;;
;; These pattern were used for indirect sibcall
;; but a bug prevent their correct usage
;; and leads to incorrect code (jump to random address)
;; See T1118

;; (define_insn "*isibcall_value_real_<mode>"
;;     [(set (match_operand 0 "" "")
;; 	  (call (mem:P (match_operand:P 1 "register_operand" "Cs"))
;; 	  	(match_operand 2 "" "")))
;;      (return)]
;;   "igoto<P:suffix> %1"
;; [(set_attr "type" "bcu")
;; (set_attr "class" "jump")]
;; )

;; (define_insn "*isibcall_real_<mode>"
;;     [(call (mem:P (match_operand:P 0 "register_operand" "Cs"))
;; 	   (match_operand 1 "" ""))
;;      (return)]
;;   "igoto<P:suffix> %0"
;; [(set_attr "type" "bcu")
;; (set_attr "class" "jump")]
;; )

(define_insn "*icall_value_<mode>"
  [(set (match_operand 0 "" "")
        (call (mem:P (match_operand:P 1 "register_operand" "r"))
              (match_operand 2 "general_operand" "")))
   (clobber (reg:DI KV3_RA_REGNO))]
  ""
  "icall %1"
[(set_attr "type" "bcu")
 (set_attr "class" "link")]
)

(define_insn "*s<ALLI:lsusize><code>"
  [(set (mem:ALLI (match_operand:P 1 "register_operand" "r"))
  	(if_then_else:ALLI
		(cb_cond (match_operand:SIDI 0 "register_operand" "r")
			 (const_int 0))
		(match_operand:ALLI 2 "register_operand" "r")
		(mem:ALLI (match_dup 1))))]
  ""
  "s<ALLI:lsusize>.<SIDI:suffix><code>z %0 ? [%1] = %2"
  [(set_attr "type" "lsu_auxr_store")])

(define_insn "*s<ALLI:lsusize>_<lsu_odd_even>"
  [(set (mem:ALLI (match_operand:P 1 "register_operand" "r"))
  	(if_then_else:ALLI
		(COND_ODD_EVEN (zero_extract (match_operand:ALLI 0 "register_operand" "r")
			       (const_int 1) (const_int 0))
			       (const_int 0))
		(match_operand:ALLI 2 "register_operand" "r")
		(mem:ALLI (match_dup 1))))]
  ""
  "s<ALLI:lsusize>.<lsu_odd_even> %0 ? [%1] = %2"
  [(set_attr "type" "lsu_auxr_store")])

;; Regular loads to ALLI
;; ld, lwz, lhz, lbz.
(define_insn "*l<ALLI:lsusize><code>"
  [(set (match_operand:ALLI 1 "register_operand" "+r")
  	(if_then_else:ALLI
		(cb_cond (match_operand:SIDI 0 "register_operand" "r")
			 (const_int 0))
 		(mem:ALLI (match_operand:P 2 "register_operand" "r"))
               (match_dup 1)))]
  ""
  "l<ALLI:lsusize><ALLI:lsusext>.<SIDI:suffix><cb_cond:code>z %0 ? %1 = [%2]"
  [(set_attr "type" "lsu_auxw_load")])

(define_insn "*l<ALLI:lsusize>_<lsu_odd_even>"
   [(set (match_operand:ALLI 1 "register_operand" "+r")
   	 (if_then_else:ALLI
		(COND_ODD_EVEN (zero_extract (match_operand:SI 0 "register_operand" "r")
			         (const_int 1) (const_int 0))
		               (const_int 0))
		(mem:ALLI (match_operand:P 2 "register_operand" "r"))
		(match_dup 1)))]
  ""
  "l<ALLI:lsusize><ALLI:lsusext>.<COND_ODD_EVEN:lsu_odd_even> %0 ? %1 = [%2]"
  [(set_attr "type" "lsu_auxw_load")])

;; matching sign/zero extend loads.
(define_insn "*l<SHORT:lsusize><ANY_EXTEND:lsext><cb_cond:code>"
  [(set (match_operand:DI 1 "register_operand" "+r")
  	(if_then_else:DI
		(cb_cond (match_operand:SIDI 0 "register_operand" "r")
		         (const_int 0))
		(ANY_EXTEND:DI (mem:SHORT (match_operand:P 2 "register_operand" "r")))
		(match_dup 1)))]
  ""
  "l<SHORT:lsusize><ANY_EXTEND:lsext>.<SIDI:suffix><cb_cond:code>z %0 ? %1 = [%2]"
  [(set_attr "type" "lsu_auxw_load")])

(define_insn "*l<SHORT:lsusize><ANY_EXTEND:lsext>_<lsu_odd_even>"
  [(set (match_operand:DI 1 "register_operand" "+r")
  	(if_then_else:DI
		(COND_ODD_EVEN (zero_extract (match_operand:SIDI 0 "register_operand" "r")
			       (const_int 1) (const_int 0))
		(const_int 0))
		(ANY_EXTEND:DI (mem:SHORT (match_operand:P 2 "register_operand" "r")))
		(match_dup 1)))]
  ""
  "l<SHORT:lsusize><ANY_EXTEND:lsext>.<lsu_odd_even> %0 ? %1 = [%2]"
  [(set_attr "type" "lsu_auxw_load")])

(define_code_iterator gt_comp [gt gtu])
(define_code_iterator lt_comp [lt ltu])
(define_code_iterator ge_comp [ge geu])
(define_code_iterator le_comp [le leu])

(define_insn_and_split "*comp_<code>_incr"
   [(set (match_operand:SI 0 "register_operand" "=r")
         (le_comp:SI (plus:SI (match_operand:SI 1 "register_operand" "r")
                              (const_int 1))
                     (match_operand:SI 2 "register_operand" "r")))
    (set (match_operand:SI 3 "register_operand" "=r")
         (plus:SI (match_dup 1) (const_int 1)))
   ]
   ""
   "comp_<code>_incr not spilt"
   ""
[(set (match_dup 0) (match_op_dup:SI 4 [(match_dup 1) (match_dup 2)]))
 (set (match_dup 3) (plus:SI (match_dup 1) (const_int 1)))]
{
     operands[4] = gen_rtx_fmt_ee(kvx_strict_to_nonstrict_comparison_operator (<CODE>),
                                  SImode, operands[1], operands[2]);
}
)

(define_insn_and_split "*comp_<code>_decr"
   [(set (match_operand:SI 0 "register_operand" "=r")
         (lt_comp:SI (plus:SI (match_operand:SI 1 "register_operand" "r")
                              (const_int -1))
                     (match_operand:SI 2 "register_operand" "r")))
    (set (match_operand:SI 3 "register_operand" "=r")
         (plus:SI (match_dup 1) (const_int -1)))
   ]
   ""
   "comp_<code>_decr not spilt"
   ""
[(set (match_dup 0) (match_op_dup:SI 4 [(match_dup 1) (match_dup 2)]))
 (set (match_dup 3) (plus:SI (match_dup 1) (const_int -1)))]
{
     operands[4] = gen_rtx_fmt_ee(kvx_strict_to_nonstrict_comparison_operator (<CODE>),
                                  SImode, operands[1], operands[2]);
}
)

(define_insn_and_split "*comp_<code>_decr"
   [(set (match_operand:SI 0 "register_operand" "=r")
         (ge_comp:SI (plus:SI (match_operand:SI 1 "register_operand" "r")
                              (const_int -1))
                     (match_operand:SI 2 "register_operand" "r")))
    (set (match_operand:SI 3 "register_operand" "=r")
         (plus:SI (match_dup 1) (const_int -1)))
   ]
   ""
   "comp_<code>_decr not split"
   ""
[(set (match_dup 0) (match_op_dup:SI 4 [(match_dup 1) (match_dup 2)]))
 (set (match_dup 3) (plus:SI (match_dup 1) (const_int -1)))]
{
     operands[4] = gen_rtx_fmt_ee(kvx_strict_to_nonstrict_comparison_operator (<CODE>),
                                  SImode, operands[1], operands[2]);
}
)

(define_insn_and_split "*comp_<code>_incr"
   [(set (match_operand:SI 0 "register_operand" "=r")
         (gt_comp:SI (plus:SI (match_operand:SI 1 "register_operand" "r")
                              (const_int 1))
                     (match_operand:SI 2 "register_operand" "r")))
    (set (match_operand:SI 3 "register_operand" "=r")
         (plus:SI (match_dup 1) (const_int 1)))
   ]
   ""
   "comp_<code>_incr not split"
   ""
[(set (match_dup 0) (match_op_dup:SI 4 [(match_dup 1) (match_dup 2)]))
 (set (match_dup 3) (plus:SI (match_dup 1) (const_int 1)))]
{
     operands[4] = gen_rtx_fmt_ee(kvx_strict_to_nonstrict_comparison_operator (<CODE>),
                                  SImode, operands[1], operands[2]);
}
)

(define_expand "prologue"
  [(const_int 1)]
  ""
  "
{
  kvx_expand_prologue ();
  DONE;
}")

(define_expand "epilogue"
  [(parallel [(return)
              (use (reg:DI KV3_RA_REGNO))])]
  ""
  "
{
	kvx_expand_epilogue ();
}")

(define_expand "sibcall_epilogue"
  [(parallel [(return)
              (use (reg:DI KV3_RA_REGNO))])]
  ""
  "
{
  emit_use (gen_rtx_REG (DImode, KV3_RA_REGNO));
  kvx_expand_epilogue ();
  DONE; /* DO NOT generate the ret in this case! */
}")

(define_insn "ret"
  [(return)
   (use (reg:DI KV3_RA_REGNO))]
  ""
  "ret"
[(set_attr "type" "bcu")
 (set_attr "class" "return")]
) 

(define_expand "untyped_call"
  [(parallel [(call (match_operand 0 "" "")
		    (const_int 0))
	      (match_operand 1 "" "")
	      (match_operand 2 "" "")])]
  ""
  {
    int i;
    rtx reg = gen_rtx_REG (OImode, KV3_ARGUMENT_POINTER_REGNO);

    /* We need to use call_value so the return value registers don't get
     * clobbered. */
    emit_call_insn (gen_call_value (reg, operands[0], const0_rtx));

    for (i = 0; i < XVECLEN (operands[2], 0); i++)
      {
	rtx set = XVECEXP (operands[2], 0, i);
	emit_move_insn (SET_DEST (set), SET_SRC (set));
      }

    DONE;
  })

;; Do not allow any immediate, only numerical ones.
;; (define_expand "div_srs<SIDI:suffix>"
;;   [(set (match_operand:SIDI 0 "register_operand" )
;; 	(div:SIDI (match_operand:SIDI 1 "register_operand" )
;; 	       (match_operand   2 "poweroftwo_6bits_immediate_operand" )))])

(define_insn "div_srs<SIDI:suffix>"
  [(set (match_operand:SIDI 0 "register_operand" "=r")
	(div:SIDI (match_operand:SIDI 1 "register_operand" "r")
	          (match_operand 2 "poweroftwo_6bits_immediate_operand" "n")))]
  ""
  {
  rtx op = operands[2];
  gcc_assert (CONST_INT_P(op));
  long long div = INTVAL(op);
  gcc_assert (__builtin_popcount (div) == 1);
  operands[2] = gen_rtx_CONST_INT(VOIDmode, __builtin_ctzl (div));
  return "srs<SIDI:suffix> %0 = %1, %2";
}
[(set_attr "type" "alu_lite")
])

(define_insn "*land<SIDI:suffix>"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (and:SI (ne:SI (match_operand:SIDI 1 "register_operand" "%r") (const_int 0))
	        (ne:SI (match_operand:SIDI 2 "register_operand" "r") (const_int 0))))]
  ""
  "land<SIDI:suffix> %0 = %1, %2"
[(set_attr "type" "alu_lite")])

(define_insn "*lnand<SIDI:suffix>"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (ior:SI (eq:SI (match_operand:SIDI 1 "register_operand" "%r") (const_int 0))
	        (eq:SI (match_operand:SIDI 2 "register_operand" "r") (const_int 0))))]
  ""
  "lnand<SIDI:suffix> %0 = %1, %2"
[(set_attr "type" "alu_lite")])

(define_insn "*lor<SIDI:suffix>"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (ne:SI (ior:SIDI (match_operand:SIDI 1 "register_operand" "%r")
	              (match_operand:SIDI 2 "register_operand" "r"))
               (const_int 0)))]
  ""
  "lor<SIDI:suffix> %0 = %1, %2"
[(set_attr "type" "alu_lite")])

(define_insn "*lnor<SIDI:suffix>"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (eq:SI (ior:SIDI (match_operand:SIDI 1 "register_operand" "%r")
	              (match_operand:SIDI 2 "register_operand" "r"))
               (const_int 0)))]
  ""
  "lnor<SIDI:suffix> %0 = %1, %2"
[(set_attr "type" "alu_lite")])


;; FIXME AUTO: fix predicate. Incorrectly too wide
(define_insn "satd"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (unspec:DI [(match_operand:DI 1 "register_operand" "r,r")
                    (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SATD))]
  ""
  "satd %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "4,4")]
)

;; FIXME AUTO: fix predicate. Incorrectly too wide
(define_insn "satud"
  [(set (match_operand:DI 0 "register_operand" "=r,r")
        (unspec:DI [(match_operand:DI 1 "register_operand" "r,r")
                    (match_operand:SI 2 "sat_shift_operand" "r,U06")] UNSPEC_SATUD))]
  ""
  "satud %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite")
   (set_attr "length" "4,4")]
)

(define_insn "ssaddsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (ss_plus:SI (match_operand:SI 1 "register_operand" "r,r,r")
                    (match_operand:SI 2 "register_s32_operand" "r,I10,I32")))]
  ""
  "addsw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")])

;; zero extend version of above.
(define_insn "*ssaddsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (ss_plus:SI (match_operand:SI 1 "register_operand" "r,r,r")
                                    (match_operand:SI 2 "register_s32_operand" "r,I10,I32"))))]
  ""
  "addsw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")])

(define_insn "ssadddi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (ss_plus:DI (match_operand:DI 1 "register_operand" "r,r,r,r")
                    (match_operand:DI 2 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")))]
  ""
  "addsd %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
   (set_attr "length" "4,4,8,12")]
)

(define_insn "sssubsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (ss_minus:SI (match_operand:SI 1 "register_operand" "r,I10,I32")
                     (match_operand:SI 2 "register_s32_operand" "r,r,r")))]
  ""
  "sbfsw %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")])

;; zero extend version of above.
(define_insn "*sssubsi3_zext"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r")
        (zero_extend:DI (ss_minus:SI (match_operand:SI 1 "register_operand" "r,I10,I32")
                                     (match_operand:SI 2 "register_s32_operand" "r,r,r"))))]
  ""
  "sbfsw %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")])

(define_insn "sssubdi3"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (ss_minus:DI (match_operand:DI 1 "kvx_r_s10_s37_s64_operand" "r,I10,B37,i")
                     (match_operand:DI 2 "register_operand" "r,r,r,r")))]
  ""
  "sbfsd %0 = %2, %1"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
   (set_attr "length" "4,4,8,12")]
)

/*************** FLOATING POINT **************/

(define_insn "builtin_extendhfsf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SI 1 "register_operand" "r")] UNSPEC_FWIDENLHW))]
  ""
  "fwidenlhw %0 = %1"
[(set_attr "type" "alu_lite")])

;; (define_insn "builtin_extendv2hfv2sf2"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;; 	(unspec:V2SF [(match_operand:DI 1 "register_operand" "r")] UNSPEC_FWIDENLHWP))]
;;   ""
;;   "fwidenlhwp %0 = %1"
;; [(set_attr "type" "alud")
;;  (set_attr "length" "4")]
;; )
;; 
(define_insn "builtin_extendhfsf2_tophalf"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SI 1 "register_operand" "r")] UNSPEC_FWIDENMHW))]
  ""
  "fwidenmhw %0 = %1"
[(set_attr "type" "alu_lite")])

;; (define_insn "builtin_extendv2hfv2sf2_tophalf"
;;   [(set (match_operand:V2SF 0 "register_operand" "=r")
;; 	(unspec:V2SF [(match_operand:DI 1 "register_operand" "r")] UNSPEC_FWIDENMHWP))]
;;   ""
;;   "fwidenmhwp %0 = %1"
;; [(set_attr "type" "alud")
;;  (set_attr "length" "4")]
;; )

(define_mode_attr int_type [(SF "SI") (DF "DI")])
(define_mode_attr float_type [(SI "SF") (DI "DF")])

(define_insn "truncdfsf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (float_truncate:SF
         (match_operand:DF 1 "register_operand" "r")))]
  ""
  "fnarrowdw %0 = %1"
[(set_attr "type" "alu_full")
])

(define_insn "builtin_truncsfhf2"
  [(set (match_operand:HI 0 "register_operand" "=r")
        (unspec:HI [(match_operand:SF 1 "register_operand" "r")] UNSPEC_FNARROWWH))]
  ""
  "fnarrowwh %0 = %1"
[(set_attr "type" "alu_lite")
])

;; Beware that the half float are stored in 2 32bits words. They are /not/ packed.
;; (define_insn "builtin_truncv2sf_2hf2"
;;   [(set (match_operand:V2SI 0 "register_operand" "=r")
;;         (unspec [(match_operand:V2SF 1 "register_operand" "r")] UNSPEC_FNARROWDWP))]
;;   ""
;;   "fnarrowdwp %0 = %1"
;; [(set_attr "type" "alu_full")
;;  (set_attr "length" "4")]
;; )


(define_expand "divsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (div:SF (match_operand:SF 1 "register_operand" "r")
                (match_operand:SF 2 "register_operand" "r")))]
  ""
  {
    rtx temp = gen_reg_rtx(SFmode);
    emit_insn (gen_recipsf2_insn (temp, CONST1_RTX (SFmode), operands[2]));
    emit_insn (gen_mulsf3 (operands[0], operands[1], temp));
    DONE;
  }
)

(define_expand "sqrtsf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (sqrt:SF (match_operand:SF 1 "register_operand" "r")))]
  ""
  {
    rtx temp = gen_reg_rtx(SFmode);
    emit_insn (gen_rsqrtsf2_insn (temp, CONST1_RTX (SFmode), operands[1]));
    emit_insn (gen_mulsf3 (operands[0], operands[1], temp));
    DONE;
  }
)

;; Jeff Law - [committed] Fix v850e3v5 recipf and rsqrt issues
;; Generic code demands that the recip and rsqrt named patterns
;; have precisely one operand.  So that what is exposed in the
;; expander via the strange UNSPEC.  However, those expanders
;; generate normal looking recip and rsqrt patterns.

(define_expand "recipsf2"
  [(set (match_operand:SF 0 "register_operand" "")
   (unspec:SF [(match_operand:SF 1 "register_operand" "")]
                UNSPEC_FINVW_))]
  ""
  {
    emit_insn (gen_recipsf2_insn (operands[0], CONST1_RTX (SFmode), operands[1]));
    DONE;
  }
)

(define_insn "recipsf2_insn"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (div:SF (match_operand:SF 1 "const_float_1_operand" "")
                (match_operand:SF 2 "register_operand" "r")))]
  ""
  "finvw %0 = %2"
  [(set_attr "type" "alu_full_copro")]
)

(define_insn "kvx_finvw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FINVW))]
  ""
  "finvw%2 %0 = %1"
  [(set_attr "type" "alu_full_copro")]
)

(define_expand "rsqrtsf2"
  [(set (match_operand:SF 0 "register_operand" "=")
   (unspec:SF [(match_operand:SF 1 "register_operand" "")]
                UNSPEC_FISRW_))]
  ""
  {
    emit_insn (gen_rsqrtsf2_insn (operands[0], CONST1_RTX (SFmode), operands[1]));
    DONE;
  }
)

(define_insn "rsqrtsf2_insn"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (div:SF (match_operand:SF 1 "const_float_1_operand" "")
                (sqrt:SF (match_operand:SF 2 "register_operand" "r"))))]
  ""
  "fisrw %0 = %2"
  [(set_attr "type" "alu_full_copro")]
)

(define_insn "kvx_fisrw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand 2 "" "")] UNSPEC_FISRW))]
  ""
  "fisrw%2 %0 = %1"
  [(set_attr "type" "alu_full_copro")]
)

(define_insn "fcdivw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")
                    (match_operand:SF 2 "register_operand" "r")] UNSPEC_FCDIVW))]
  ""
  "fcdivw %0 = %1, %2"
  [(set_attr "type" "alu_lite")
])

(define_insn "fcdivd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")
                    (match_operand:DF 2 "register_operand" "r")] UNSPEC_FCDIVD))]
  ""
  "fcdivd %0 = %1, %2"
  [(set_attr "type" "alu_lite")
])

(define_insn "fsinvw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")] UNSPEC_FSINVW))]
  ""
  "fsinvw %0 = %1"
  [(set_attr "type" "alu_lite")
])

(define_insn "fsinvd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")] UNSPEC_FSINVD))]
  ""
  "fsinvd %0 = %1"
  [(set_attr "type" "alu_lite")
])

(define_insn "fsisrw"
  [(set (match_operand:SF 0 "register_operand" "=r")
        (unspec:SF [(match_operand:SF 1 "register_operand" "r")] UNSPEC_FSISRW))]
  ""
  "fsisrw %0 = %1"
  [(set_attr "type" "alu_lite")
])

(define_insn "fsisrd"
  [(set (match_operand:DF 0 "register_operand" "=r")
        (unspec:DF [(match_operand:DF 1 "register_operand" "r")] UNSPEC_FSISRD))]
  ""
  "fsisrd %0 = %1"
  [(set_attr "type" "alu_lite")
])

(define_insn "fsdivw"
  [(set (match_operand:SF 0 "register_operand" "=r")
	(unspec:SF [(match_operand:SF 1 "register_operand" "r")
		    (match_operand:SF 2 "register_operand" "r")] UNSPEC_FSDIVW))]
  ""
  "fsdivw %0 = %1, %2"
  [(set_attr "type" "alu_lite")
])

(define_insn "fsdivd"
  [(set (match_operand:DF 0 "register_operand" "=r")
	(unspec:DF [(match_operand:DF 1 "register_operand" "r")
		    (match_operand:DF 2 "register_operand" "r")] UNSPEC_FSDIVD))]
  ""
  "fsdivd %0 = %1, %2"
  [(set_attr "type" "alu_lite")
])


/******* ALU instructions **********/

(define_insn "kvx_abdw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (abs:SI (minus:SI  (match_operand:SI 2 "register_s32_operand" "r,I10,I32")
                           (match_operand:SI 1 "register_operand" "r,r,r"))))]
  ""
  "abdw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")]
)

(define_insn "kvx_abdd"
  [(set (match_operand:DI 0 "register_operand" "=r,r,r,r")
        (abs:DI (minus:DI  (match_operand:DI 2 "register_s64_operand" "r,I10,I37,i")
                           (match_operand:DI 1 "register_operand" "r,r,r,r"))))]
  ""
  "abdd %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x,alu_lite_y")
   (set_attr "length" "4,4,8,12")]
)

(define_expand "avgsi3_floor"
  [(set (match_operand:SI 0 "register_operand" "")
        (ashiftrt:SI (plus:SI (match_operand:SI 1 "register_operand" "")
                              (match_operand:SI 2 "register_s32_operand" ""))
                     (const_int 1)))
   (clobber (match_scratch:SI 3 ""))]
  ""
  {
    emit_insn (gen_kvx_avgw (operands[3], operands[1], operands[2]));
    operands[0] = gen_rtx_SIGN_EXTEND (SImode, operands[3]);
  }
)

(define_insn "kvx_avgw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r,r,r")
                    (match_operand:SI 2 "register_s32_operand" "r,I10,I32")] UNSPEC_AVGW))]
  ""
  "avgw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")]
)

(define_expand "uavgsi3_floor"
  [(set (match_operand:SI 0 "register_operand" "")
        (lshiftrt:SI (plus:SI (match_operand:SI 1 "register_operand" "")
                              (match_operand:SI 2 "register_s32_operand" ""))
                     (const_int 1)))
   (clobber (match_scratch:SI 3 ""))]
  ""
  {
    emit_insn (gen_kvx_avguw (operands[3], operands[1], operands[2]));
    operands[0] = gen_rtx_ZERO_EXTEND (SImode, operands[3]);
  }
)

(define_insn "kvx_avguw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r,r,r")
                    (match_operand:SI 2 "register_s32_operand" "r,I10,I32")] UNSPEC_AVGUW))]
  ""
  "avguw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")]
)

(define_expand "avgsi3_ceil"
  [(set (match_operand:SI 0 "register_operand" "")
        (ashiftrt:SI (plus:SI (plus:SI (match_operand:SI 1 "register_operand" "")
                                       (match_operand:SI 2 "register_s32_operand" ""))
                              (const_int 1))
                     (const_int 1)))
   (clobber (match_scratch:SI 3 ""))]
  ""
  {
    emit_insn (gen_kvx_avgrw (operands[3], operands[1], operands[2]));
    operands[0] = gen_rtx_SIGN_EXTEND (SImode, operands[3]);
  }
)

(define_insn "kvx_avgrw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r,r,r")
                    (match_operand:SI 2 "register_s32_operand" "r,I10,I32")] UNSPEC_AVGRW))]
  ""
  "avgrw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")]
)

(define_expand "uavgsi3_ceil"
  [(set (match_operand:SI 0 "register_operand" "")
        (lshiftrt:SI (plus:SI (plus:SI (match_operand:SI 1 "register_operand" "")
                                       (match_operand:SI 2 "register_s32_operand" ""))
                              (const_int 1))
                     (const_int 1)))
   (clobber (match_scratch:SI 3 ""))]
  ""
  {
    emit_insn (gen_kvx_avgruw (operands[3], operands[1], operands[2]));
    operands[0] = gen_rtx_ZERO_EXTEND (SImode, operands[3]);
  }
)

(define_insn "kvx_avgruw"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "r,r,r")
                    (match_operand:SI 2 "register_s32_operand" "r,I10,I32")] UNSPEC_AVGRUW))]
  ""
  "avgruw %0 = %1, %2"
  [(set_attr "type" "alu_lite,alu_lite,alu_lite_x")
   (set_attr "length" "4,4,8")]
)

(define_insn "abdhq"
  [(set (match_operand:DI 0 "register_operand" "=r")
	(unspec:DI [(match_operand:DI 1 "register_operand" "r")
		    (match_operand:DI 2 "register_operand" "r")] UNSPEC_ABDHQ))]
  ""
  "abdhq %0 = %1, %2"
  [(set_attr "type" "alu_lite")
])

/********** Hardware loops **************/

;; operand 0 is the loop count pseudo register
;; operand 1 is the label to jump to at the top of the loop
(define_expand "doloop_end"
  [(parallel [(set (pc) (if_then_else
                          (ne (match_operand:SI 0 "" "")
                              (const_int 1))
                          (label_ref (match_operand 1 "" ""))
                          (pc)))
              (set (match_dup 0)
                   (plus:SI (match_dup 0)
                            (const_int -1)))
              (unspec [(const_int 0)] UNSPEC_LOOPDO_END)
              (clobber (match_scratch:SI 2))])]
  ""
{
  /* The loop optimizer doesn't check the predicates... */
  if (GET_MODE (operands[0]) != SImode)
    FAIL;
  operands[2] = gen_rtx_SCRATCH (SImode);
})

;; Here, we let a symbol even if there are only 17bits of immediate value.
;; FIXME AUTO: we should let gcc know operand 0 is unused after insn.
(define_insn "loop_start"
  [(set (pc)
        (if_then_else (ne (match_operand:SI 1 "register_operand" "0")
                          (const_int 1))
                      (label_ref (match_operand 2 "" ""))
                      (pc)))
   (set (match_operand:SI 0 "register_operand" "=r")
        (plus (match_dup 0)
              (const_int -1)))
   (unspec [(const_int 0)] UNSPEC_LOOPDO)]
  ""
  "loopdo %0, %2"
  [(set_attr "type" "all")
   (set_attr "length" "4")])

;; jump_insn with output reloads: we must take care of our reloads.
(define_insn_and_split "loop_end"
  [(set (pc)
        (if_then_else (ne (match_operand:SI 0 "shouldbe_register_operand" "+r,*m")
                          (const_int 1))
                      (label_ref (match_operand 1 "" ""))
                      (pc)))
   (set (match_dup 0)
        (plus (match_dup 0)
              (const_int -1)))
   (unspec [(const_int 0)] UNSPEC_LOOPDO_END)
   (clobber (match_scratch:SI 2 "=X,&r"))]
  ""
  " # HW loop end"
  "&& memory_operand(operands[0], SImode)"
   [(pc)]
{
  emit_move_insn (operands[2], operands[0]);
  emit_jump_insn (gen_loop_end_withreload (operands[2], operands[1], operands[0]));
  DONE;
}
  [(set_attr "length" "0")])

; reload can't make output reloads for jump insns, so we have to do this by hand.
(define_insn "loop_end_withreload"
  [(set (pc) (if_then_else (ne (match_operand:SI 0 "register_operand" "+&r,&r,&r")
				(const_int 1))
			   (label_ref (match_operand 1 "" ""))
			   (pc)))
   (set (match_dup 0) (plus:SI (match_dup 0) (const_int -1)))
   (set (match_operand:SI 2 "memory_operand" "=a,b,m")
	(plus:SI (match_dup 0) (const_int -1)))
   (unspec [(const_int 0)] UNSPEC_LOOPDO_END)]
  "reload_completed"
  "# End of hwloop\n\tsd%m0 %0 = %2"
  [(set_attr "type" "lsu_auxr_store, lsu_auxr_store_x, lsu_auxr_store_y")
   (set_attr "length" "4, 8, 12")])

/* ====================================================================== */
/*                            Reload stuff                                */

(define_expand "reload_in_gotoff_<mode>"
  [(parallel [(set (match_operand:P 0 "register_operand" "=&r")
                   (match_operand:P 1 "immediate_operand" "i"))
              (clobber (match_operand:P 2 "register_operand" "=&r"))])]
  "flag_pic"
    {
        emit_insn (gen_set_gotp_<mode>(operands[0]));
        emit_insn (gen_add<mode>3 (operands[0], operands[2], operands[0]));
        emit_insn (gen_add<mode>3 (operands[0], operands[0], gen_rtx_CONST (<MODE>mode,gen_rtx_UNSPEC (<MODE>mode, gen_rtvec (1, operands[1]), UNSPEC_GOTOFF))));
  DONE;
})

(include "scalar.md")

(include "vector.md")

(include "control.md")
