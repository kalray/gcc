;; (define_predicate "lt_k1b"
;;   (match_test "k1_architecture () < K1B")
;; )

;; (define_predicate "ge_k1b"
;;   (match_test "k1_architecture () >= K1B")
;; )

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


;; immediate up to signed 37
(define_predicate "s37_operand"
  (and (match_code "const,const_int")
       (match_test "satisfies_constraint_I37(op)")))

;; register or immediate up to signed 37
(define_predicate "register_s37_operand"
 (ior (match_code "reg")
      (match_operand 0 "s37_operand")))

;; register or immediate up to signed 64
(define_predicate "register_s64_operand"
 (ior (match_code "reg")
      (and (match_code "const,const_int")
           (match_test "satisfies_constraint_I64(op)"))))

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

        bool farcall = false;
        if (GET_CODE(XEXP(op, 0)) == SYMBOL_REF
	    && SYMBOL_REF_FUNCTION_P(XEXP(op, 0))
            && SYMBOL_REF_DECL(XEXP(op,0)) != NULL_TREE){
          farcall = lookup_attribute ("farcall", DECL_ATTRIBUTES(SYMBOL_REF_DECL(XEXP(op,0)))) != NULL;
        }
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

(define_predicate "k1_symbol_operand"
  (match_code "symbol_ref,label_ref,const,unspec")
{
  switch (GET_CODE (op))
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
      if (XINT (op, 1) == UNSPEC_GOTOFF
       	  && GET_CODE (XVECEXP (op, 0, 0)) == PLUS)
	{
	  rtx op1 = XEXP (XVECEXP (op, 0, 0), 0);
	  rtx op2 = XEXP (XVECEXP (op, 0, 0), 1);

	  if (!CONST_INT_P (op2)){
	       if (k1_symbol_operand(op2, mode) 
	           || !immediate_operand(op2, mode)) {
	           return false;
	       }
	    }

	  if (GET_CODE(op1) == SYMBOL_REF
	      || GET_CODE(op1) == LABEL_REF )
	     return true;
        }
      break;
    case CONST:
      /* We also may accept the offsetted memory references in certain
	 special cases.  */
      if (GET_CODE (XEXP (op, 0)) == UNSPEC
          && XINT (XEXP (op, 0), 1) == UNSPEC_TLS)
	return true;

      if (GET_CODE (XEXP (op, 0)) == UNSPEC
          && (XINT (XEXP (op, 0), 1) == UNSPEC_GOT
              || XINT (XEXP (op, 0), 1) == UNSPEC_GOTOFF)
	  && k1_symbol_operand(XVECEXP (XEXP (op, 0), 0, 0), mode))
	return true;

	// switch ()
	//   {
	//   case UNSPEC_TLS:
	//     return true;
	//   default:
	//     break;
	//   }

      if (GET_CODE (XEXP (op, 0)) == PLUS)
	{
	  rtx op1 = XEXP (XEXP (op, 0), 0);
	  rtx op2 = XEXP (XEXP (op, 0), 1);

	  if (!CONST_INT_P (op2))
	    return false;

//	  offset = trunc_int_for_mode (INTVAL (op2), DImode);
	  if (GET_CODE(op1) == SYMBOL_REF
	      || GET_CODE(op1) == LABEL_REF)
	     return true;
	  // switch (GET_CODE (op1))
	  //   {
	  //   case SYMBOL_REF:
	  //     return true;

	  //   case LABEL_REF:
	  // 	return true;

	  //   default:
	  //     break;
	  //   }
	}
      break;

      default:
	gcc_unreachable ();
    }

  return false;
})


/* When reloading (subreg:SI (reg:DI) xxx) reload will sometimes generate 
   (subreg:SI (concatn:DI ...) xxx). This will get cleaned up in
   cleanup_subreg_operands, but the pattern needs to be accepted as the 
   instruction will be extract_insn'd first.  */
/* FIXME AUTO: should not be needed anymore as we won't play with subreg«
/*(define_predicate "movsi_operand"
   (match_code "const,const_int,reg,subreg,mem,symbol_ref,label_ref")
{
	return general_operand (op,mode) 
               || immediate_operand (op, VOIDmode)
               || (GET_CODE (op) == SUBREG
                   && GET_CODE (SUBREG_REG (op)) == CONCATN);
})
*/


(define_predicate "shift_operand"
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

(define_predicate "fivebits_unsigned_operand"
  (match_code "const_int")
{
	return	CONST_INT_P (op) 
                && (INTVAL (op) >= 0 && INTVAL (op) < (1<<5));
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

(define_predicate "unsigned_mul_reg_or_immediate"
  (ior (match_operand 0 "register_operand")
       (and (match_code "const_int")
            (match_test "INTVAL(op) >= 0"))))

(define_predicate "system_register32_operand"
  (match_code "reg")
{
	return	(!TARGET_64 && (REGNO_REG_CLASS (REGNO (op)) == SRF_REGS)) || (TARGET_64 && ((REGNO_REG_CLASS (REGNO (op)) == SRF32_REGS))) ;
})

(define_predicate "system_register64_operand"
  (match_code "reg")
{
	return	(TARGET_64 && ((REGNO_REG_CLASS (REGNO (op)) == SRF64_REGS))) ;
})

(define_predicate "system_register_operand"
  (match_code "reg")
{
	return	(REGNO_REG_CLASS (REGNO (op)) == SRF_REGS) || (((REGNO_REG_CLASS (REGNO (op)) == SRF32_REGS) ||(REGNO_REG_CLASS (REGNO (op)) == SRF64_REGS))) ;
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
      return (!TARGET_64);

    case LABEL_REF:
      /* For certain code models, the code is near as well.  */
      /* return ix86_cmodel == CM_SMALL || ix86_cmodel == CM_MEDIUM; */
      return (!TARGET_64);

    default:
      gcc_unreachable ();
    }
  return false;
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
  if (count != 2
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


