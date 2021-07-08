;; Constant zero operand for scalar or vector MODE
(define_predicate "const_zero_operand"
  (and (match_code "const_int,const_wide_int,const_double,const_vector")
       (match_test "op == CONST0_RTX (mode)")))

;; Constant -1 operand for scalar or vector MODE
(define_predicate "const_mone_operand"
  (and (match_code "const_int, const_wide_int,const_vector")
       (match_test "op == CONSTM1_RTX (mode)")))

;; Constant 1.0 operand for floating-point MODE
(define_predicate "const_float1_operand"
  (and (match_code "const_double,const_vector")
       (match_test "op == CONST1_RTX (mode)")))

;; Register or constant zero.
(define_predicate "reg_or_zero_operand"
  (and (match_code "reg,subreg,const_int,const_vector")
       (ior (match_operand 0 "register_operand")
            (match_test "op == const0_rtx")
            (match_test "op == CONST0_RTX (mode)"))))

;; Register or constant 1.0 (floating-point).
(define_predicate "reg_or_float1_operand"
  (and (match_code "reg,subreg,const_double,const_vector")
       (ior (match_operand 0 "register_operand")
            (match_test "op == CONST1_RTX (mode)"))))

;; Allow for LABELs to be used in the mov expander
;; It will split it using add_pcrel insn.
;; This predicates should only be used in the expander as LABELs
;; are not to be accepted during insn matching.
(define_predicate "kvx_mov_operand"
 (ior (match_operand 0 "general_operand")
      (and (match_test "flag_pic")
           (match_code "label_ref"))))

(define_predicate "rotate_operand"
 (and (match_code "reg,subreg,const_int")
      (ior (match_operand 0 "register_operand")
           (match_test "satisfies_constraint_U05(op)"))))

(define_predicate "sat_shift_operand"
 (and (match_code "reg,subreg,const_int")
      (ior (match_operand 0 "register_operand")
           (match_test "satisfies_constraint_U06(op)"))))

(define_predicate "sixbits_unsigned_operand"
  (match_code "const_int")
{
  return (INTVAL (op) >= 0 && INTVAL (op) < (1<<6));
})

(define_predicate "poweroftwo_6bits_immediate_operand"
  (match_code "const_int")
{
  return (__builtin_popcountll (INTVAL (op)) == 1)
          && (INTVAL (op) >= 0) && (INTVAL(op) <= 64);
})

;; Register or immediate up to signed 32
(define_predicate "register_s32_operand"
 (and (match_code "reg,subreg,const,const_int")
      (ior (match_operand 0 "register_operand")
           (match_test "satisfies_constraint_I32(op)"))))

;; Register or immediate up to float 32
(define_predicate "register_f32_operand"
 (and (match_code "reg,subreg,const_double")
      (ior (match_operand 0 "register_operand")
           (match_test "satisfies_constraint_H32(op)"))))

(define_predicate "jump_operand"
  (match_code "mem")
{
  /* Weak symbols can be resolved to 0 and thus generate long branches that
     don't fit in our 27 bits offsets. Calls to a function which declaration
     has the 'farcall' attribute must also use indirect calls.
     Reject weak symbols and 'farcall's here and handle that case
     in the call expanders to generate indirect calls for weak references. */

  bool farcall = kvx_is_farcall_p (op);

  return  !farcall && (GET_CODE (XEXP (op, 0)) == LABEL_REF
		       || (GET_CODE (XEXP (op, 0)) == SYMBOL_REF
			   && !SYMBOL_REF_WEAK (XEXP (op, 0))));
})

;; Integer comparison operators against integer zero.
(define_predicate "zero_comparison_operator"
  (match_code "eq,ne,le,lt,ge,gt"))

;; Floating-point comparisons operators supported.
(define_predicate "float_comparison_operator"
  (match_code "ne,eq,ge,lt,uneq,unge,unlt,ltgt"))

;; Returns TRUE if op is a register or an immediate suitable for sign
;; extension from the format signed10, upper27_lower10 or
;; extend27_upper27_lower10
(define_predicate "kvx_r_s10_s37_s64_operand"
 (ior (and (match_test "!flag_pic")
           (match_operand 0 "nonmemory_operand"))
      (match_code "const_int")
      (match_code "const_double")
      (match_operand 0 "register_operand")
      (match_test "kvx_legitimate_pic_symbolic_ref_p(op)"))
)

;; Returns TRUE for a register, a 32-bit immediate constant, a symbol
;; reference if 32bit mode and all PIC related symbolic ref
(define_predicate "kvx_r_any32_operand"
  (ior (and (match_test "!flag_pic && (Pmode == SImode)")
            (match_operand 0 "nonmemory_operand"))
       (match_code "const_int")
       (match_operand 0 "register_operand")
       (match_test "kvx_legitimate_pic_symbolic_ref_p(op)"))
 )

(define_predicate "symbolic_operand"
  (match_code "const,symbol_ref,label_ref"))

(define_predicate "kvx_symbol_operand"
  (match_code "symbol_ref,label_ref,const,unspec")
{
  rtx base, offset;
  split_const (op, &base, &offset);

  /* Allow for (const (plus (sym) (const_int offset))) */
  switch (GET_CODE (base))
    {
    case SYMBOL_REF:
      /* TLS symbols are not constant.  */
      // if (SYMBOL_REF_TLS_MODEL (op))
      // 	return false;
      return true;

    case LABEL_REF:
      /* For certain code models, the code is near as well.  */
      return true;

    case UNSPEC:
      if (XINT (base, 1) == UNSPEC_GOTOFF
          || XINT (base, 1) == UNSPEC_GOT
          || XINT (base, 1) == UNSPEC_TLS_DTPOFF
          || XINT (base, 1) == UNSPEC_TLS_GD
          || XINT (base, 1) == UNSPEC_TLS_LD
          || XINT (base, 1) == UNSPEC_TLS_IE
          || XINT (base, 1) == UNSPEC_TLS_LE
          || XINT (base, 1) == UNSPEC_PCREL)
	  return true;
      break;

    default:
	gcc_unreachable ();
    }

  return false;
})

;; Reject memory addresses that use the .xs addressing mode.
;; Here .xs addressing may appear as (plus (ashift (reg) (const_int)) (reg)).
;; In that cases addresses will be rejected by address_operand().
(define_predicate "noxsaddr_operand"
  (match_test "address_operand (op, VOIDmode)")
{
  if (GET_CODE (op) == PLUS && GET_CODE (XEXP (op, 0)) == MULT)
    return false;
  return true;
})

;; Used for hw loop pattern where we have an output reload in a jump insn.
;; This is not supported by reload so the insn must handle them.
;; This hack comes from the arc backend.
(define_predicate "shouldbe_register_operand"
  (match_code "reg,subreg,mem")
{
  return ((reload_in_progress || reload_completed)
	  ? general_operand : register_operand) (op, mode);
})

(define_predicate "system_register_operand"
  (match_code "reg")
{
  return (REGNO_REG_CLASS (REGNO (op)) == SFR_REGS);
})

(define_predicate "store_multiple_operation"
  (and (match_code "parallel")
       (match_test "kvx_store_multiple_operation_p (op)")))

;; Return 1 if OP is a load multiple operation, known to be a PARALLEL.
(define_predicate "load_multiple_operation"
  (and (match_code "parallel")
       (match_test "kvx_load_multiple_operation_p (op, false)")))

(define_predicate "load_multiple_operation_uncached"
  (and (match_code "parallel")
       (match_test "kvx_load_multiple_operation_p (op, true)")))

;; Predicates used for register pair for 128-bits.

;; Returns TRUE if OP is suitable for paired-register (pseudo reg are
;; accepted)
(define_predicate "kvx_register_pair_operand"
  (and (match_operand 0 "register_operand")
       (match_test "kvx_ok_for_paired_reg_p (op)")))

;; Returns TRUE if OP is a paired-register or if it is a
;; nonimmediate_operand and not a register
(define_predicate "kvx_nonimmediate_operand_pair"
 (and (match_operand 0 "nonimmediate_operand")
      (ior (not (match_operand 0 "register_operand"))
           (match_operand 0 "kvx_register_pair_operand"))))

;; Returns TRUE if OP is a paired-register or a general_operand and
;; not a register.
(define_predicate "kvx_general_operand_pair"
 (and (match_operand 0 "general_operand")
      (ior (not (match_operand 0 "register_operand"))
           (match_operand 0 "kvx_register_pair_operand"))))

;; TRUE for any immediate, const_vector or subreg of const_vector
(define_predicate "vec_or_scalar_immediate_operand"
    (ior (match_operand 0 "immediate_operand")
         (match_code "const_vector")
         (match_test "kvx_subreg_const_vector_p (op)")))

;; Predicates used for register quad for 256-bits.

;; Returns TRUE if OP is suitable for quad-register (pseudo reg are
;; accepted)
(define_predicate "kvx_register_quad_operand"
  (and (match_operand 0 "register_operand")
       (match_test "kvx_ok_for_quad_reg_p (op)")))

;; Returns TRUE if OP is a quad-register or if it is a
;; nonimmediate_operand and not a register
(define_predicate "kvx_nonimmediate_operand_quad"
 (and (match_operand 0 "nonimmediate_operand")
      (ior (not (match_operand 0 "register_operand"))
           (match_operand 0 "kvx_register_quad_operand"))))

;; Returns TRUE if OP is a quad-register or a general_operand and
;; not a register.
(define_predicate "kvx_general_operand_quad"
 (and (match_operand 0 "general_operand")
      (ior (not (match_operand 0 "register_operand"))
           (match_operand 0 "kvx_register_quad_operand"))))

