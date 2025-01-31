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

;; Register or constant 0/-1.
(define_predicate "reg_zero_mone_operand"
  (and (match_code "reg,subreg,const_int,const_vector")
       (ior (match_operand 0 "register_operand")
            (match_test "op == const0_rtx")
            (match_test "op == constm1_rtx")
            (match_test "op == CONST0_RTX (mode)")
            (match_test "op == CONSTM1_RTX (mode)"))))

;; Register or constant 1.0 (floating-point).
(define_predicate "register_float1_operand"
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

(define_predicate "reg_shift_operand"
 (and (match_code "reg,subreg,const_int")
      (ior (match_operand 0 "register_operand")
           (match_test "satisfies_constraint_U06(op)"))))

(define_predicate "sixbits_unsigned_operand"
  (match_code "const_int")
{
  return (INTVAL (op) >= 0 && INTVAL (op) < (1<<6));
})

(define_predicate "const_pow2lt64_operand"
  (match_code "const_int")
{
  return (__builtin_popcountll (INTVAL (op)) == 1)
         && (__builtin_ctzll (INTVAL (op)) < 64);
})

(define_predicate "const_ge64_operand"
  (match_code "const_int")
{
  return (INTVAL (op) >= 64);
})

(define_predicate "const_pos32_operand"
  (and (match_code "const_int")
       (match_test "satisfies_constraint_I32(op)"))
{
  return INTVAL (op) >= 0;
})

(define_predicate "const_neg32_operand"
  (and (match_code "const_int")
       (match_test "satisfies_constraint_I32(op)"))
{
  return INTVAL (op) < 0;
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

(define_predicate "syscall_operand"
  (match_code "mem")
{
    return (GET_CODE (XEXP (op, 0)) == REG
            || GET_CODE(XEXP (op, 0)) == CONST_INT)
        && kvx_syscall_addrspace_p (op);
})

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

;; Return 1 if this is predicable unary operator.
(define_predicate "pred_unary_operator"
  (ior
    (match_code "neg,ss_neg,abs,ss_abs,not")
    (match_code "sign_extend,zero_extend,truncate,bswap,ffs,clrsb,ctz,popcount")
    (match_code "float_extend,float_truncate,float,fix,unsigned_float,unsigned_fix")))

;; Return 1 if this is predicable shift/rotate operator.
(define_predicate "pred_shift_operator"
  (match_code "ashift,ss_ashift,us_ashift,ashiftrt,lshiftrt,rotate,rotatert"))

;; Return 1 if this is predicable binary arithmetic operator (no mult).
(define_predicate "pred_binarith_operator"
  (ior
    (match_code "plus,ss_plus,us_plus,minus,ss_minus,us_minus")
    (match_code "and,ior,xor,smin,smax,umin,umax")))

;; Return 1 if this is predicable multiply operator.
(define_predicate "pred_multiply_operator"
  (match_code "mult"))

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
      //   return false;
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
;; Here .xs addressing may appear as (plus (mult (reg) (const_int)) (reg)).
;; In that cases addresses will be rejected by address_operand().
(define_predicate "noxsaddr_operand"
  (match_test "address_operand (op, mode)")
{
  if (GET_CODE (op) == PLUS && GET_CODE (XEXP (op, 0)) == MULT)
    return false;
  return true;
})

;; Used to filter the addressing mode of masked memory instructions.
(define_predicate "memsimple_operand"
  (match_code "mem")
{
  return indirect_operand (op, mode)
         || kvx_has_27bit_immediate_p (op)
         || kvx_has_54bit_immediate_p (op);
})

;; Used to filter the addressing mode of atomic memory instructions.
(define_predicate "mematomic_operand"
  (match_code "mem")
{
  if (KV3_1)
    return memory_operand (op, mode);
  return indirect_operand (op, mode)
         || kvx_has_27bit_immediate_p (op)
         || kvx_has_54bit_immediate_p (op);
})

;; Used for do loop pattern where we have an output reload in a jump insn.
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
  unsigned regno = REGNO (op);
  return REGNO_REG_CLASS (regno) == SFR_REGS;
})

(define_predicate "general_register_operand"
  (match_code "reg,subreg")
{
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  unsigned regno = REGNO (op);
  return REGNO_REG_CLASS (regno) == GENERAL_REGS;
})

(define_predicate "extension_register_operand"
  (match_code "reg,subreg")
{
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  unsigned regno = REGNO (op);
  return REGNO_REG_CLASS (regno) == XCR_REGS;
})

(define_predicate "float16_inner_mode"
  (match_code "reg,subreg")
{
  machine_mode inner_mode = GET_MODE_INNER (GET_MODE (op));
  return inner_mode == HFmode;
})

(define_predicate "uncached_modifier"
  (match_code "const_string")
{
  const char *modifier = XSTR (op, 0);
  if (modifier[0] == '.' && modifier[1] == 'u')
    return true;
  for (modifier++; *modifier; modifier++)
    if (*modifier == '.') break;
  return modifier[0] == '.' && modifier[1] == 'u';
})

(define_predicate "masked_modifier"
  (match_code "const_string")
{
  const char *modifier = XSTR (op, 0);
  if (modifier[0] == '.' && modifier[1] == 'm')
    return true;
  for (modifier++; *modifier; modifier++)
    if (*modifier == '.') break;
  return modifier[0] == '.' && modifier[1] == 'm';
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

