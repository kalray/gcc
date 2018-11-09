(define_predicate "nonmemory64_register32_d_operand"
   (match_code "const,const_int,reg,subreg,mem,symbol_ref,label_ref")
{
	return nonmemory_operand (op,mode);
})

(define_predicate "k1_zero"
  (and (match_code "const_int")
       (match_test "op == const0_rtx")))

;; used for some 32bits ALU
;; register or immediate up to signed 32
(define_predicate "register_s32_operand"
 (ior (match_code "reg")
      (and (match_code "const,const_int")
           (match_test "satisfies_constraint_I32(op)"))))

;; immediate up to signed 32
(define_predicate "s32_operand"
  (and (match_code "const,const_int")
       (match_test "satisfies_constraint_I32(op)")))


;; immediate up to signed 37
(define_predicate "s37_operand"
  (and (match_code "const,const_int")
       (match_test "satisfies_constraint_I37(op)")))

;; register or immediate up to signed 37
(define_predicate "register_s37_operand"
 (ior (match_code "reg")
      (match_operand 0 "s37_operand")))

;; register or immediate up to signed 64
;; Does not really check value fits on 64bits as HOST_WIDE_INT
;; is at most 64bits.
(define_predicate "register_s64_operand"
 (ior (match_code "reg")
      (match_code "const,const_int")))

(define_predicate "nonmemory64_register32_w_operand"
   (match_code "const,const_int,reg,subreg,mem,symbol_ref,label_ref")
{
	return register_operand (op,mode);
})


(define_predicate "jump_operand"
  (match_code "mem")
{
  /* Weak symbols can be resolved to 0 and thus generate long branches that
     don't fit in our 27 bits offsets. Calls to a function which declaration
     has the 'farcall' attribute must also use indirect calls.
     Reject weak symbols and 'farcall's here and handle that case
     in the call expanders to generate indirect calls for weak references. */

  bool farcall = K1_FARCALL;
  if (!farcall && (GET_CODE(XEXP(op, 0)) == SYMBOL_REF
		   && SYMBOL_REF_FUNCTION_P(XEXP(op, 0))
		   && SYMBOL_REF_DECL(XEXP(op,0)) != NULL_TREE))
    farcall = lookup_attribute ("farcall", DECL_ATTRIBUTES(SYMBOL_REF_DECL(XEXP(op,0)))) != NULL;

  return  !farcall && (GET_CODE (XEXP (op, 0)) == LABEL_REF
		       || (GET_CODE (XEXP (op, 0)) == SYMBOL_REF
			   && !SYMBOL_REF_WEAK (XEXP (op, 0))));
})

(define_special_predicate "k1_branch_comparison_operator"
  (match_code "eq,ne,le,lt,ge,gt"))


(define_predicate "float_comparison_operator"
  (match_code "ne,eq,ge,lt,uneq,unge,unlt,ltgt"))

(define_predicate "bitwise_operator"
  (match_code "ior,and,xor"))

(define_predicate "packed_memory_operand"
  (match_code "mem")
{
        (void)op; /* Avoid warnings */
	/* The code that generates packed accesses ensures the corrrectness. */
	return true;
})

;; Immediate suitable for PIC code (insn must have corresponding
;; relocation)
(define_predicate "k1_imm_pic_operand"
  (ior (match_code "const_int")
       (match_test "k1_legitimate_pic_symbolic_ref_p(op)")))

;; Operand valid as the 2nd/2 op of an ALU insn (make, …)
(define_predicate "k1_make_int_operand"
 (ior (and (match_test "!flag_pic")
           (match_operand 0 "immediate_operand"))
      (match_code "const_int")
      (match_test "k1_legitimate_pic_symbolic_ref_p(op)"))
)

;; Operand valid as the 3rd/3 op of an ALU insn (add*, sbf*, …)
(define_predicate "k1_alu_op3_operand"
 (ior (and (match_test "!flag_pic")
           (match_operand 0 "nonmemory_operand"))
      (match_code "const_int")
      (match_operand 0 "register_operand")
      (match_test "k1_legitimate_pic_symbolic_ref_p(op)"))
)

(define_predicate "k1_imm_float_operand"
   (match_code "const_double"))

;; Operand valid as the 3rd op of an MAU insn (mul*, …)
(define_predicate "k1_mau_op3_operand"
 (ior (and (match_test "!flag_pic")
           (match_operand 0 "nonmemory_operand"))
      (match_code "const_int")
      (match_operand 0 "register_operand")
      (match_test "k1_legitimate_pic_symbolic_ref_p(op)")))

(define_predicate "k1_symbol_operand"
  (match_code "symbol_ref,label_ref,const,unspec")
{
  rtx base, offset;
  split_const (op, &base, &offset);

  /* Allow for (const (plus (sym) (const_int offset)) */
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
          || XINT (base, 1) == UNSPEC_PCREL
          || XINT (base, 1) == UNSPEC_TLS)
	  return true;
      break;

    default:
	gcc_unreachable ();
    }

  return false;
})

(define_predicate "sat_shift_operand"
  (match_code "const_int,reg,subreg")
{
	return	nonmemory_operand(op, mode)
	   && (!CONST_INT_P (op)
                || (INTVAL (op) >= 0 && INTVAL (op) < (1<<6)));
})

(define_predicate "shiftd_operand"
  (match_code "const_int,reg,subreg")
{
	return	nonmemory_operand(op, mode) 
	   && (!CONST_INT_P (op) 
                || (INTVAL (op) >= 0 && INTVAL (op) < (1<<7)));
})

;; (define_predicate "non_modulo_memory_operand"
;;   (match_code "mem")
;; {
;; 	return memory_operand(op, mode) 
;; 	   && !k1_legitimate_modulo_addressing_p(XEXP (op, 0), reload_completed);
;; })

;; (define_predicate "non_modulo_address_operand"
;;   (match_code "subreg,reg,plus")
;; {
;; 	return address_operand(op, mode) 
;; 	   && !k1_legitimate_modulo_addressing_p(op, reload_completed);
;; })

(define_predicate "small_operand"
  (match_code "const_int,reg,subreg")
{
	return	register_operand(op, mode) 
	   || (CONST_INT_P (op) 
                && (INTVAL (op) >= -(1<<9) && INTVAL (op) < (1<<9)));
})

(define_predicate "twobits_unsigned_operand"
  (match_code "const_int")
{
	return	CONST_INT_P (op) 
                && (INTVAL (op) >= 0 && INTVAL (op) < (1<<2));
})

(define_predicate "sixbits_unsigned_operand"
  (match_code "const_int")
{
	return	CONST_INT_P (op) 
                && (INTVAL (op) >= 0 && INTVAL (op) < (1<<6));
})

/*
 * (define_predicate "immediate_float_43bits_operand"
 *  (and (match_code "const_double")
 *       (match_test "k1_float_fits_bits(CONST_DOUBLE_REAL_VALUE(op),43,mode)")))
 */

(define_predicate "reg_or_s32_operand"
 ( ior (match_operand 0 "register_operand")
       (and (match_code "const_int")
            (match_test "satisfies_constraint_I32(op)"))))

(define_predicate "immediate_unsigned_32bits_operand"
  (and (match_code "const_int")
       (match_test "INTVAL (op) >= 0 && INTVAL (op) < (1LL << 32)")))

(define_predicate "immediate_unsigned_37bits_operand"
  (and (match_code "const_int")
       (match_test "INTVAL (op) >= 0 && INTVAL (op) < (1LL << 37)")))

(define_predicate "poweroftwo_6bits_immediate_operand"
  (match_code "const_int")
{
        return (__builtin_popcount (INTVAL (op)) == 1) &&
	       (INTVAL(op) <= 64);
})

(define_predicate "rotate_operand"
  (match_code "const_int,reg,subreg")
{
	return	register_operand (op, mode)
		|| (CONST_INT_P (op) 
                    && (INTVAL (op) >= 0 && INTVAL (op) < (1<<5)));
})

(define_predicate "unsigned_mul_immediate_37"
 (ior (match_test "satisfies_constraint_J10(op)")
      (match_test "satisfies_constraint_U37(op)")))

;; Used for hw loop pattern where we have an output reload in a jump insn.
;; This is not supported by reload so the insn must handle them.
;; This hack comes from the arc backend.
(define_predicate "shouldbe_register_operand"
  (match_code "reg,subreg,mem")
{
  return ((reload_in_progress || reload_completed)
	  ? general_operand : register_operand) (op, mode);
})

;; (define_predicate "reg_or_unsigned_mul_immediate_37"
;;  (ior (match_operand 0 "register_operand")
;;       (match_test "satisfies_constraint_J10(op)")
;;       (match_test "satisfies_constraint_U37(op)")))

(define_predicate "uimmediate_operand"
  (and (match_code "const_int")
       (match_test "INTVAL (op) >= 0")))

(define_predicate "u32immediate_operand"
  (match_test "satisfies_constraint_U32(op)"))

(define_predicate "register_or_u32immediate"
  (ior (match_operand 0 "register_operand")
       (match_test "satisfies_constraint_U32(op)")))

(define_predicate "system_register_operand"
  (match_code "reg")
{
  return (REGNO_REG_CLASS (REGNO (op)) == SRF_REGS)
          || REGNO_REG_CLASS (REGNO (op)) == SRF32_REGS
	  || REGNO_REG_CLASS (REGNO (op)) == SRF64_REGS ;
})

;; Return true if VALUE can be stored in the zero extended 32bits immediate field.
(define_predicate "k1_unsigned32_immediate_operand"
  (match_code "const_double,const_int,symbol_ref,label_ref")
{
  switch (GET_CODE (op))
    {
    case CONST_DOUBLE:
      if (HOST_BITS_PER_WIDE_INT == 32)
	return (GET_MODE (op) == VOIDmode && !CONST_DOUBLE_HIGH (op));
      else
	return false;

    case CONST_INT:
      if (HOST_BITS_PER_WIDE_INT == 32)
	return INTVAL (op) >= 0;
      else{
	return !(INTVAL (op) & ~(HOST_WIDE_INT) 0xffffffff);
      }

    case SYMBOL_REF:
      /* For certain code models, the symbolic references are known to fit.  */
      /* TLS symbols are not constant.  */
      if (SYMBOL_REF_TLS_MODEL (op))
	return false;
      return (TARGET_32);

    case LABEL_REF:
      /* For certain code models, the code is near as well.  */
      /* return ix86_cmodel == CM_SMALL || ix86_cmodel == CM_MEDIUM; */
      return (TARGET_32);

    default:
      gcc_unreachable ();
    }
  return false;
})

(define_predicate "store_multiple_operation"
  (match_code "parallel")
{
  int count = XVECLEN (op, 0) /* - 1 */;
  unsigned int src_regno;
  rtx dest_addr;
  int i;

  /* Perform a quick check so we don't blow up below.  */
  if ((count != 2 && count != 4)
      || GET_CODE (XVECEXP (op, 0, 0)) != SET
      || GET_CODE (SET_DEST (XVECEXP (op, 0, 0))) != MEM
      || GET_CODE (SET_SRC (XVECEXP (op, 0, 0))) != REG)
    return 0;

  src_regno = REGNO (SET_SRC (XVECEXP (op, 0, 0)));
  dest_addr = XEXP (SET_DEST (XVECEXP (op, 0, 0)), 0);

  for (i = 1; i < count; i++)
    {
      rtx elt = XVECEXP (op, 0, i /*+ 1*/);

      if (GET_CODE (elt) != SET
	  || GET_CODE (SET_SRC (elt)) != REG
	  || GET_MODE (SET_SRC (elt)) != DImode
	  || REGNO (SET_SRC (elt)) != src_regno + i
	  || GET_CODE (SET_DEST (elt)) != MEM
	  || GET_MODE (SET_DEST (elt)) != DImode
	  || GET_CODE (XEXP (SET_DEST (elt), 0)) != PLUS
	  || ! rtx_equal_p (XEXP (XEXP (SET_DEST (elt), 0), 0), dest_addr)
	  || GET_CODE (XEXP (XEXP (SET_DEST (elt), 0), 1)) != CONST_INT
	  || INTVAL (XEXP (XEXP (SET_DEST (elt), 0), 1)) != i * 8)
	return 0;

    }

  return 1;
})


;; Return 1 if OP is a load multiple operation, known to be a PARALLEL.
(define_predicate "load_multiple_operation"
  (match_code "parallel")
{
  int count = XVECLEN (op, 0);
  unsigned int dest_regno;
  rtx src_addr;
  int i;

  /* Perform a quick check so we don't blow up below.  */
  if ((count != 2 && count != 4)
      || GET_CODE (XVECEXP (op, 0, 0)) != SET
      || GET_CODE (SET_DEST (XVECEXP (op, 0, 0))) != REG
      || GET_CODE (SET_SRC (XVECEXP (op, 0, 0))) != MEM)
    return 0;

  dest_regno = REGNO (SET_DEST (XVECEXP (op, 0, 0)));
  src_addr = XEXP (SET_SRC (XVECEXP (op, 0, 0)), 0);

  for (i = 1; i < count; i++)
    {
      rtx elt = XVECEXP (op, 0, i);

      if (GET_CODE (elt) != SET
	  || GET_CODE (SET_DEST (elt)) != REG
	  || GET_MODE (SET_DEST (elt)) != DImode
	  || REGNO (SET_DEST (elt)) != dest_regno + i
	  || GET_CODE (SET_SRC (elt)) != MEM
	  || GET_MODE (SET_SRC (elt)) != DImode
	  || GET_CODE (XEXP (SET_SRC (elt), 0)) != PLUS
	  || ! rtx_equal_p (XEXP (XEXP (SET_SRC (elt), 0), 0), src_addr)
	  || GET_CODE (XEXP (XEXP (SET_SRC (elt), 0), 1)) != CONST_INT
	  || INTVAL (XEXP (XEXP (SET_SRC (elt), 0), 1)) != i * 8)
	return 0;
    }

  return 1;
})


