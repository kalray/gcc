/* Helpers */

#ifndef IN_LIBGCC2
#include "coretypes.h"
#include "statistics.h"
#include "stringpool.h"
#include "vec.h"
#endif

#ifndef _K1_REGS
#define _K1_REGS
#include "k1c-registers.h"
#endif

/* Which ABI to use.  */
enum k1_abi_type
{
  K1_ABI_LP64 = 0
};

#ifndef K1_ABI_DEFAULT
#define K1_ABI_DEFAULT K1_ABI_LP64
#endif

#define TEST_REGNO(R, TEST, VALUE, STRICT)                                     \
  ((!(STRICT) && R >= FIRST_PSEUDO_REGISTER) || (R TEST VALUE)                 \
   || (reg_renumber && ((unsigned) reg_renumber[R] TEST VALUE)))

/* (K1C_MDS_REGISTERS+1) is used as part of
   k1_legitimize_reload_address () because reload wants real addresses
   when it reloads MEMs. This register shouldn't be generated anywhere
   else, thus it causes no correctness issue. */
#define IS_GENERAL_REGNO(num, strict)                                          \
  (TEST_REGNO (num, <, 64, strict)                                             \
   || TEST_REGNO (num, ==, (K1C_MDS_REGISTERS + 1), strict))

#define NO_IMPLICIT_EXTERN_C

/* ********** Driver ********** */

#define TARGET_CPU_CPP_BUILTINS()                                              \
  do                                                                           \
    {                                                                          \
      builtin_assert ("cpu=k1");                                               \
      builtin_define ("__K1__");                                               \
      builtin_define ("__k1__");                                               \
      if (TARGET_K1C)                                                          \
	{                                                                      \
	  builtin_define ("__K1C__");                                          \
	  builtin_define ("__k1c__");                                          \
	  builtin_assert ("machine=k1c");                                      \
	  builtin_define ("__k1arch=k1c");                                     \
	  builtin_define ("__k1core=k1c");                                     \
	  if (TARGET_STRICT_ALIGN)                                             \
	    builtin_define ("__STRICT_ALIGN__");                               \
	  if (TARGET_STACK_CHECK_USE_TLS)                                      \
	    builtin_define ("__K1_STACK_LIMIT_TLS");                           \
	  if (!TARGET_32)                                                      \
	    builtin_define ("__K1_64__");                                      \
	}                                                                      \
    }                                                                          \
  while (0)

#ifndef K1_DEFAULT_CORE
#define K1_DEFAULT_CORE "k1c"
#endif

#ifndef K1_OS_SELF_SPECS
#define K1_OS_SELF_SPECS "%{!mcore*:-mcore=" K1_DEFAULT_CORE " } "
#endif

#define DRIVER_SELF_SPECS_COMMON                                               \
  K1_OS_SELF_SPECS                                                             \
  "%{fpic: %{!fPIC:-fPIC}} %<fpic", "%{fPIC: %<fpic}"

#define LINK_SPEC_COMMON "%{shared} %{m32:-melf32k1} %{!m32:-melf64k1} "

#define CC1_SPEC " %{G*}"

#define ASM_SPEC "%{mcore*} --no-check-resources %{m32} "

#define CRT_CALL_STATIC_FUNCTION(SECTION_OP, FUNC)                             \
  asm(SECTION_OP "\ncall " #FUNC "\n;;\n.previous\n");

/* ********** Per-Function Data ********** */

#define INIT_EXPANDERS k1_init_expanders ()

/* ********** Storage Layout ********** */

#define BITS_BIG_ENDIAN 0

#define BYTES_BIG_ENDIAN 0

#define WORDS_BIG_ENDIAN 0

/* Number of storage units in a word; normally 4. */
#define UNITS_PER_WORD 8

#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE)                                    \
  if (GET_MODE_CLASS (MODE) == MODE_INT                                        \
      && GET_MODE_SIZE (MODE) < UNITS_PER_WORD)                                \
    {                                                                          \
      if ((MODE) == QImode || (MODE) == HImode || (MODE) == SImode)            \
	{                                                                      \
	  (MODE) = DImode;                                                     \
	}                                                                      \
    }

#define PARM_BOUNDARY 64

#define STACK_BOUNDARY 128

/* Alignment required for a function entry point, in bits.  */
#define FUNCTION_BOUNDARY 64

#define BIGGEST_ALIGNMENT 128

#define PCC_BITFIELD_TYPE_MATTERS 1

/* Make strings word-aligned so strcpy from constants will be faster.  */
#define CONSTANT_ALIGNMENT(EXP, ALIGN)                                         \
  ((TREE_CODE (EXP) == STRING_CST && !optimize_size                            \
    && (ALIGN) < BITS_PER_WORD)                                                \
     ? BITS_PER_WORD                                                           \
     : (ALIGN))

/* Align definitions of arrays, unions and structures so that
   initializations and copies can be made more efficient.  This is not
   ABI-changing, so it only affects places where we can see the
   definition.  */
#define DATA_ALIGNMENT(EXP, ALIGN)                                             \
  ((ALIGN) < BITS_PER_WORD ? BITS_PER_WORD : (ALIGN))

/* Similarly, make sure that objects on the stack are sensibly aligned.  */
#define LOCAL_ALIGNMENT(EXP, ALIGN) DATA_ALIGNMENT (EXP, ALIGN)

#define STRICT_ALIGNMENT TARGET_STRICT_ALIGN

//#define STACK_SLOT_ALIGNMENT(type, mode, align) ( (mode == BLKmode ? 8 :
//GET_MODE_SIZE (mode)) > 4 ? BIGGEST_ALIGNMENT : BITS_PER_WORD )

/* ********** Type Layout ********** */

#define POINTER_SIZE (TARGET_32 ? 32 : 64)

#define INT_TYPE_SIZE 32

#define SHORT_TYPE_SIZE 16

#define LONG_TYPE_SIZE (TARGET_32 ? 32 : 64)

#define LONG_LONG_TYPE_SIZE 64

#define FLOAT_TYPE_SIZE 32

#define DOUBLE_TYPE_SIZE 64

#define LONG_DOUBLE_TYPE_SIZE 64

#define DEFAULT_SIGNED_CHAR 1

#define SIZE_TYPE "long unsigned int"

#define PTRDIFF_TYPE "long int"

/* ********** Registers ********** */

/* Number of hardware registers known to the compiler. They receive
   numbers 0 through FIRST_PSEUDO_REGISTER-1; thus, the first pseudo
   register's number really is assigned the number
   FIRST_PSEUDO_REGISTER.  */
#define FIRST_PSEUDO_REGISTER (K1C_MDS_REGISTERS + 2)

#define FIXED_REGISTERS                                                        \
  {                                                                            \
    K1C_ABI_REGULAR_FIXED_REGISTERS                                            \
    1,	 /* sync */                                                            \
      1, /* virtual FP */                                                      \
  }

#define CALL_USED_REGISTERS                                                    \
  {                                                                            \
    K1C_ABI_REGULAR_CALL_USED_REGISTERS                                        \
    1,	 /* sync */                                                            \
      1, /* virtual FP */                                                      \
  }

#define CALL_REALLY_USED_REGISTERS                                             \
  {                                                                            \
    K1C_ABI_REGULAR_CALL_REALLY_USED_REGISTERS                                 \
    1,	 /* sync */                                                            \
      1, /* virtual FP */                                                      \
  }

#define PC_REGNUM K1C_PROGRAM_POINTER_REGNO

#define HARD_REGNO_NREGS(REGNO, MODE)                                          \
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* A C expression that is nonzero if it is permissible to store a
   value of mode mode in hard register number regno (or in several
   registers starting with that one). */
#define HARD_REGNO_MODE_OK(REGNO, MODE) k1_hard_regno_mode_ok (REGNO, MODE)

#define HARD_REGNO_RENAME_OK(FROM, TO) k1_hard_regno_rename_ok (FROM, TO)

/* A C expression that is nonzero if a value of mode mode1 is
   accessible in mode mode2 without copying.

   If HARD_REGNO_MODE_OK (r, mode1) and HARD_REGNO_MODE_OK (r, mode2)
   are always the same for any r, then MODES_TIEABLE_P (mode1, mode2)
   should be nonzero. If they differ for any r, you should define this
   macro to return zero unless some other mechanism ensures the
   accessibility of the value in a narrower mode.

   You should define this macro to return nonzero in as many cases as
   possible since doing so will allow GCC to perform better register
   allocation. */
// FIXME AUTO: Check modes tieable is correct. Float part seems suspicious.
#define MODES_TIEABLE_P(MODE1, MODE2)                                          \
  (GET_MODE_CLASS (MODE1) == GET_MODE_CLASS (MODE2))

// This looks a bit problematic for mode with different sizes. A float
// is hardly accessed without a copy when changing its mode.

/* ((mode1 == mode2)					   \ */
/*     || ((!VECTOR_MODE_P (mode1) && !VECTOR_MODE_P (mode2)) \ */
/* 	&& ((INTEGRAL_MODE_P (mode1) && INTEGRAL_MODE_P (mode2))	\ */
/* 	    || (FLOAT_MODE_P (mode1) && FLOAT_MODE_P (mode2))))) */

/* The number of distinct register classes, defined as follows: */

#define N_REG_CLASSES (int) LIM_REG_CLASSES

/* Provide the GENERAL_REGS definitions */
#define GENERAL_REGS GRF_REGS

/* A macro whose definition is the name of the class to which a valid
   base register must belong. A base register is one used in an
   address which is the register value plus a displacement. */
#define BASE_REG_CLASS (GENERAL_REGS)

/* A macro whose definition is the name of the class to which a valid
   index register must belong. An index register is one used in an
   address where its value is either multiplied by a scale factor or
   added to another register (as well as added to a displacement). */
#define INDEX_REG_CLASS GENERAL_REGS

#define SYMBOLIC_CONST(X)                                                      \
  (GET_CODE (X) == SYMBOL_REF || GET_CODE (X) == LABEL_REF                     \
   || (GET_CODE (X) == CONST && symbolic_reference_mentioned_p (X)))

#define LEGITIMATE_PIC_OPERAND_P(X) k1_legitimate_pic_operand_p (X)

/* A C expression which is nonzero if register number num is suitable
   for use as a base register in operand addresses. Like
   TARGET_LEGITIMATE_ADDRESS_P, this macro should also define a strict
   and a non-strict variant. Both variants behave the same for hard
   register; for pseudos, the strict variant will pass only those that
   have been allocated to a valid hard registers, while the non-strict
   variant will pass all pseudos.

   Compiler source files that want to use the strict variant of this
   and other macros define the macro REG_OK_STRICT. You should use an
   #ifdef REG_OK_STRICT conditional to define the strict variant in
   that case and the non-strict variant otherwise. */

#ifdef REG_OK_STRICT
#define REGNO_OK_FOR_BASE_P(num) (IS_GENERAL_REGNO (num, 1))
#else
#define REGNO_OK_FOR_BASE_P(num) (IS_GENERAL_REGNO (num, 0))
#endif

/* A C expression which is nonzero if register number num is suitable
   for use as an index register in operand addresses. It may be either
   a suitable hard register or a pseudo register that has been
   allocated such a hard register.

   The difference between an index register and a base register is
   that the index register may be scaled. If an address involves the
   sum of two registers, neither one of them scaled, then either one
   may be labeled the “base” and the other the “index”; but whichever
   labeling is used must fit the machine's constraints of which
   registers may serve in each capacity. The compiler will try both
   labelings, looking for one that is valid, and will reload one or
   both registers only if neither labeling works.

   This macro also has strict and non-strict variants. */
#ifdef REG_OK_STRICT
#define REGNO_OK_FOR_INDEX_P(num) IS_GENERAL_REGNO (num, 1)
#else
#define REGNO_OK_FOR_INDEX_P(num) IS_GENERAL_REGNO (num, 0)
#endif

/* A C expression that places additional restrictions on the register
   class to use when it is necessary to copy value x into a register
   in class class. The value is a register class; perhaps class, or
   perhaps another, smaller class. On many machines, the following
   definition is safe:

   #define PREFERRED_RELOAD_CLASS(X,CLASS) CLASS

   Sometimes returning a more restrictive class makes better code. For
   example, on the 68000, when x is an integer  constant that is in
   range for a `moveq' instruction, the value of this macro is always
   DATA_REGS as long as class includes the data registers. Requiring a
   data register guarantees that a `moveq' will be used.

   One case where PREFERRED_RELOAD_CLASS must not return class is if x
   is a legitimate constant which cannot be loaded into some register
   class. By returning NO_REGS you can force x into a memory
   location. For example, rs6000 can load immediate values into
   general-purpose registers, but does not have an instruction for
   loading an immediate value into a floating-point register, so
   PREFERRED_RELOAD_CLASS returns NO_REGS when x is a floating-point
   constant. If the constant can't be loaded into any kind of
   register, code generation will be better if LEGITIMATE_CONSTANT_P
   makes the constant illegitimate instead of using
   PREFERRED_RELOAD_CLASS.

   If an insn has pseudos in it after register allocation, reload will
   go through the alternatives and call repeatedly
   PREFERRED_RELOAD_CLASS to find the best one. Returning NO_REGS, in
   this case, makes reload add a ! in front of the constraint: the x86
   back-end uses this feature to discourage usage of 387 registers
   when math is done in the SSE registers (and vice versa). */
#define PREFERRED_RELOAD_CLASS(X, CLASS) CLASS

/* A C expression for the maximum number of consecutive registers of
   class class needed to hold a value of mode mode.

   This is closely related to the macro HARD_REGNO_NREGS. In fact, the
   value of the macro CLASS_MAX_NREGS (class, mode) should be the
   maximum value of HARD_REGNO_NREGS (regno, mode) for all regno
   values in the class class.

   This macro helps control the handling of multiple-word values in
   the reload pass. */
// #define CLASS_MAX_NREGS(CLASS, MODE) HARD_REGNO_NREGS(0, MODE)
/*#define CLASS_MAX_NREGS(CLASS, MODE) (HARD_REGNO_NREGS(0, MODE) / (CLASS ==
 * PRF_REGS ? 2 : 1))*/

/* ********** Frame Layout ********** */

#define STACK_GROWS_DOWNWARD 1

#define FRAME_GROWS_DOWNWARD 1
#undef ARGS_GROWS_DOWNWARD

#define STARTING_FRAME_OFFSET (0)

#define FIRST_PARM_OFFSET(funcdecl) k1_first_parm_offset (funcdecl)

#define RETURN_ADDR_RTX(COUNT, FRAMEADDR) k1_return_addr_rtx (COUNT, FRAMEADDR)

#define DWARF2_UNWIND_INFO 1
#define DWARF2_ASM_LINE_DEBUG_INFO 1

#define INCOMING_RETURN_ADDR_RTX gen_rtx_REG (Pmode, K1C_RETURN_POINTER_REGNO)

#define DBX_REGISTER_NUMBER(REGNO) (REGNO)

#define DWARF_FRAME_RETURN_COLUMN DBX_REGISTER_NUMBER (K1C_RETURN_POINTER_REGNO)

#define STACK_POINTER_REGNUM K1C_STACK_POINTER_REGNO

#define K1C_FRAME_POINTER_VIRT_REGNO (K1C_MDS_REGISTERS + 1)

#define FRAME_POINTER_REGNUM K1C_FRAME_POINTER_VIRT_REGNO
#define HARD_FRAME_POINTER_REGNUM K1C_FRAME_POINTER_REGNO

#define ARG_POINTER_REGNUM FRAME_POINTER_REGNUM

/* ********** Elimination ********** */
#define ELIMINABLE_REGS                                                        \
  {                                                                            \
    {FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM},                              \
      {FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM},                       \
  }

// {ARG_POINTER_REGNUM, STACK_POINTER_REGNUM},
// {ARG_POINTER_REGNUM, FRAME_POINTER_REGNUM},

#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)                           \
  (OFFSET) = k1_initial_elimination_offset (FROM, TO)

/* A C statement to store in the variable depth-var the difference
   between the frame pointer and the stack pointer values immediately
   after the function prologue. The value would be computed from
   information such as the result of get_frame_size () and the tables
   of registers regs_ever_live and call_used_regs.

   If ELIMINABLE_REGS is defined, this macro will be not be used and
   need not be defined. Otherwise, it must be defined even if
   TARGET_FRAME_POINTER_REQUIRED always returns true; in that case,
   you may set depth-var to anything. */
/* #define INITIAL_FRAME_POINTER_OFFSET(depth_var)	\ */
/*     (depth_var) = k1_frame_size() + K1C_SCRATCH_AREA_SIZE -
 * STARTING_FRAME_OFFSET */

/* ********** Stack Arguments ********** */

/* A C expression. If nonzero, push insns will be used to pass
   outgoing arguments. If the target machine does not have a push
   instruction, set it to zero. That directs GCC to use an alternate
   strategy: to allocate the entire argument block and then store the
   arguments into it. When PUSH_ARGS is nonzero, PUSH_ROUNDING must be
   defined too. */
#define PUSH_ARGS 0

/* A C expression. If nonzero, the maximum amount of space required
   for outgoing arguments will be computed and placed into the
   variable current_function_outgoing_args_size. No space will be
   pushed onto the stack for each call; instead, the function prologue
   should increase the stack frame size by this amount.

   Setting both PUSH_ARGS and ACCUMULATE_OUTGOING_ARGS is not proper. */
#define ACCUMULATE_OUTGOING_ARGS 1

/* ********** Register Arguments ********** */

/* Struct used for CUMULATIVE_ARGS.
   Currently keeps track of next argument register to be used.
   If next_arg_reg >= K1C_ARG_REG_SLOTS, then all slots are used.
*/
struct k1_args
{
  int next_arg_reg;
};

#define CUMULATIVE_ARGS struct k1_args

#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, FNDECL, N_NAMED_ARGS)       \
  k1_init_cumulative_args (&CUM, FNTYPE, LIBNAME, FNDECL, N_NAMED_ARGS)

/* A C expression that is nonzero if regno is the number of a hard
   register in which function arguments are sometimes passed. This
   does not include implicit arguments such as the static chain and
   the structure-value address. On many machines, no registers can be
   used for this purpose since all function arguments are pushed on‰
   the stack. */
#define FUNCTION_ARG_REGNO_P(regno)                                            \
  ((int) (regno) >= K1C_ARGUMENT_POINTER_REGNO                                 \
   && (regno) < K1C_ARGUMENT_POINTER_REGNO + K1C_ARG_REG_SLOTS)

/* ********** Scalar Return ********** */

/* A C expression to create an RTX representing the place where a
   library function returns a value of mode mode.

   Note that “library function” in this context means a compiler
   support routine, used to perform arithmetic, whose name is known
   specially by the compiler and was not mentioned in the C code being
   compiled. */
#define LIBCALL_VALUE(MODE) gen_rtx_REG (MODE, K1C_ARGUMENT_POINTER_REGNO)

/* A C expression that is nonzero if REGNO is the number of a hard register in
   which the values of called function may come back.  */
#define FUNCTION_VALUE_REGNO_P(REGNO) FUNCTION_ARG_REGNO_P (REGNO)

/* ********** Aggregate Return ********** */

/* Define this macro to be 1 if all structure and union return values
   must be in memory. Since this results in slower code, this should
   be defined only if needed for compatibility with other compilers or
   with an ABI. If you define this macro to be 0, then the conventions
   used for structure and union return values are decided by the
   TARGET_RETURN_IN_MEMORY target hook.

   If not defined, this defaults to the value 1. */
#define DEFAULT_PCC_STRUCT_RETURN 0

/* ********** Function Entry/Exit ********** */

/* Define this macro as a C expression that is nonzero if the return
   instruction or the function epilogue ignores the value of the stack
   pointer; in other words, if it is safe to delete an instruction to
   adjust the stack pointer before a return from the function. The
   default is 0.

   Note that this macro's value is relevant only for functions for
   which frame pointers are maintained. It is never safe to delete a
   final stack adjustment in a function that has no frame pointer, and
   the compiler knows this regardless of EXIT_IGNORE_STACK. */
#define EXIT_IGNORE_STACK 1

/* Define this macro as a C expression that is nonzero for registers
   that are used by the epilogue or the `return' pattern. The stack
   and frame pointer registers are already assumed to be used as
   needed. */
#define EPILOGUE_USES(regno) (regno == K1C_RETURN_POINTER_REGNO)

/* ********** Tail calls ********** */

/* True if it is ok to do sibling call optimization for the specified
   call expression exp. decl will be the called function, or NULL if
   this is an indirect call.

   It is not uncommon for limitations of calling conventions to
   prevent tail calls to functions outside the current unit of
   translation, or during PIC compilation. The hook is used to enforce
   these restrictions, as the sibcall md pattern can not fail, or fall
   over to a “normal” call. The criteria for successful sibling call
   optimization may vary greatly between different architectures. */
#define FUNCTION_OK_FOR_SIBCALL (decl, exp) true

/* ********** Profiling ********** */

/* Emit rtl for profiling to call "_mcount" for profiling a function entry. */
extern void k1_profile_hook (void);
#define PROFILE_HOOK(LABEL) k1_profile_hook ()

/* A C statement or compound statement to output to file some
   assembler code to call the profiling subroutine mcount.

   The details of how mcount expects to be called are determined by
   your operating system environment, not by GCC. To figure them out,
   compile a small program for profiling using the system's installed
   C compiler and look at the assembler code that results.

   Older implementations of mcount expect the address of a counter
   variable to be loaded into some register. The name of this variable
   is `LP' followed by the number labelno, so you would generate the
   name using `LP%d' in a fprintf.  */
#define FUNCTION_PROFILER(STREAM, LABELNO)                                     \
  do                                                                           \
    {                                                                          \
    }                                                                          \
  while (0)

/* ********** Trampolines ********** */

/* Length in units of the trampoline for entering a nested function.  */
#define TRAMPOLINE_SIZE 12

/* Alignment required for a trampoline in bits .  */
#define TRAMPOLINE_ALIGNMENT 32

/* ********** Addressing Modes ********** */

/* A C expression that is 1 if the RTX x is a constant which is a
   valid address. */
#define CONSTANT_ADDRESS_P(x) (CONSTANT_P (x) && GET_CODE (x) != CONST_DOUBLE)

/* A number, the maximum number of registers that can appear in a
   valid memory address. Note that it is up to you to specify a value
   equal to the maximum number that TARGET_LEGITIMATE_ADDRESS_P would
   ever accept. */
#define MAX_REGS_PER_ADDRESS 2

/* ********** Costs ********** */

/* Define this macro as a C expression which is nonzero if accessing
   less than a word of memory (i.e. a char or a short) is no faster
   than accessing a word of memory, i.e., if such access require more
   than one instruction or if there is no difference in cost between
   byte and (aligned) word loads.

   When this macro is not defined, the compiler will access a field by
   finding the smallest containing object; when it is defined, a
   fullword load will be used if alignment permits. Unless bytes
   accesses are faster than word accesses, using word accesses is
   preferable since it may eliminate subsequent memory access if
   subsequent accesses occur to other fields in the same word of the
   structure, but to different bytes. */
#define SLOW_BYTE_ACCESS 1

/* Define this macro to be the value 1 if memory accesses described by
   the mode and alignment parameters have a cost many times greater
   than aligned accesses, for example if they are emulated in a trap
   handler.

   When this macro is nonzero, the compiler will act as if
   STRICT_ALIGNMENT were nonzero when generating code for block
   moves. This can cause significantly more instructions to be
   produced. Therefore, do not set this macro nonzero if unaligned
   accesses only add a cycle or two to the time for a memory access.

   If the value of this macro is always zero, it need not be
   defined. If this macro is defined, it should produce a nonzero
   value when STRICT_ALIGNMENT is nonzero. */
/* #define SLOW_UNALIGNED_ACCESS(MODE, ALIGN) 1 */

#define NO_FUNCTION_CSE 1

#define MEMORY_MOVE_COST(mode, class, in) (in ? 6 : 4)

#define BRANCH_COST(speed, predictable) 4

/* ********** PIC ********** */

#define GOT_SYMBOL_NAME "*_GLOBAL_OFFSET_TABLE_"

/* ********** Sections ********** */

#define TEXT_SECTION_ASM_OP "\t.text"
#define DATA_SECTION_ASM_OP "\t.data"
#define BSS_SECTION_ASM_OP "\t.section .bss"

/* ********** Assembler Output ********** */

/* Local compiler-generated symbols must have a prefix that the assembler
   understands.   By default, this is $, although some targets (e.g.,
   NetBSD-ELF) need to override this.  */

#define LOCAL_LABEL_PREFIX "."

#define REGISTER_PREFIX "$"

/* /\* Use tags for debug info labels, so that they don't break instruction */
/*    bundles.  This also avoids getting spurious DV warnings from the */
/*    assembler.  This is similar to (*targetm.asm_out.internal_label), except
 * that we */
/*    add brackets around the label.  *\/ */

/* #define ASM_OUTPUT_DEBUG_LABEL(FILE, PREFIX, NUM) \ */
/*   fprintf (FILE, "[.%s%d:]\n", PREFIX, NUM) */

#define ASM_COMMENT_START "#"
#define ASM_APP_ON "\t;;\n#APP\n"
#define ASM_APP_OFF "\n\t;;\n#NO_APP\n"
#define ASM_OUTPUT_ALIGN(FILE, LOG)                                            \
  fprintf (FILE, "\n\t.align %d\n", 1 << (LOG))

#define GLOBAL_ASM_OP "\t.global "

#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE)                                   \
  do                                                                           \
    {                                                                          \
      char label[256];                                                         \
      ASM_GENERATE_INTERNAL_LABEL (label, "L", (VALUE));                       \
      fprintf (FILE, "%s", integer_asm_op (GET_MODE_SIZE (Pmode), TRUE));      \
      assemble_name (FILE, label);                                             \
      fprintf (FILE, "\n");                                                    \
    }                                                                          \
  while (0)

/* Output a relative address table.  */
#define ASM_OUTPUT_ADDR_DIFF_ELT(STREAM, BODY, VALUE, REL)                     \
  do                                                                           \
    {                                                                          \
      char buf[256];                                                           \
      const char *asm_type = (GET_MODE (BODY) == SImode) ? ".word" : ".long";  \
      gcc_assert (flag_pic);                                                   \
      gcc_assert ((GET_MODE (BODY) == DImode) || (GET_MODE (BODY) == SImode)); \
      asm_fprintf ((STREAM), "\t%s ", asm_type);                               \
      ASM_GENERATE_INTERNAL_LABEL (buf, "L", VALUE);                           \
      assemble_name ((STREAM), buf);                                           \
      asm_fprintf ((STREAM), "-");                                             \
      ASM_GENERATE_INTERNAL_LABEL (buf, "L", REL);                             \
      assemble_name ((STREAM), buf);                                           \
      asm_fprintf ((STREAM), "\n");                                            \
    }                                                                          \
  while (0)

/* A C initializer containing the assembler's names for the machine
   registers, each one as a C string constant. This is what translates
   register numbers in the compiler into assembler language. */
#define REGISTER_NAMES                                                         \
  {                                                                            \
    K1C_REGISTER_NAMES                                                         \
    ".sync.", ".virtual-fp.",                                                  \
  }

/* If defined, a C initializer for an array of structures containing a
   name and a register number. This macro defines additional names for
   hard registers, thus allowing the asm option in declarations to
   refer to registers using alternate names. */
#define ADDITIONAL_REGISTER_NAMES                                              \
  {                                                                            \
    {"r0r1", 0}, {"r2r3", 2}, {"r4r5", 4}, {"r6r7", 6}, {"r8r9", 8},           \
      {"r10r11", 10}, {"r12r13", 12}, {"r14r15", 14}, {"r16r17", 16},          \
      {"r18r19", 18}, {"r20r21", 20}, {"r22r23", 22}, {"r24r25", 24},          \
      {"r26r27", 26}, {"r28r29", 28}, {"r30r31", 30}, {"r32r33", 32},          \
      {"r34r35", 34}, {"r36r37", 36}, {"r38r39", 38}, {"r40r41", 40},          \
      {"r42r43", 42}, {"r44r45", 44}, {"r46r47", 46}, {"r48r49", 48},          \
      {"r50r51", 50}, {"r52r53", 52}, {"r54r55", 54}, {"r56r57", 56},          \
      {"r58r59", 58}, {"r60r61", 60}, {"r62r63", 62}, {"r0r1r2r3", 0},         \
      {"r4r5r6r7", 4}, {"r8r9r10r11", 8}, {"r12r13r14r15", 12},                \
      {"r16r17r18r19", 16}, {"r20r21r22r23", 20}, {"r24r25r26r27", 24},        \
      {"r28r29r30r31", 28}, {"r32r33r34r35", 32}, {"r36r37r38r39", 36},        \
      {"r40r41r42r43", 40}, {"r44r45r46r47", 44}, {"r48r49r50r51", 48},        \
      {"r52r53r54r55", 52}, {"r56r57r58r59", 56}, {"r60r61r62r63", 60},        \
      {"s0", 64}, {"s1", 65}, {"s2", 66}, {"s3", 67}, {"s4", 68}, {"s5", 69},  \
      {"s6", 70}, {"s7", 71}, {"s8", 72}, {"s9", 73}, {"s10", 74},             \
      {"s11", 75}, {"s12", 76}, {"s13", 77}, {"s14", 78}, {"s15", 79},         \
      {"s16", 80}, {"s17", 81}, {"s18", 82}, {"s19", 83}, {"s20", 84},         \
      {"s21", 85}, {"s22", 86}, {"s23", 87}, {"s24", 88}, {"s25", 89},         \
      {"s26", 90}, {"s27", 91}, {"s28", 92}, {"s29", 93}, {"s30", 94},         \
      {"s31", 95}, {"s32", 96}, {"s33", 97}, {"s34", 98}, {"s35", 99},         \
      {"s36", 100}, {"s37", 101}, {"s38", 102}, {"s39", 103}, {"s40", 104},    \
      {"s41", 105}, {"s42", 106}, {"s43", 107}, {"s44", 108}, {"s45", 109},    \
      {"s46", 110}, {"s47", 111}, {"s48", 112}, {"s49", 113}, {"s50", 114},    \
      {"s51", 115}, {"s52", 116}, {"s53", 117}, {"s54", 118}, {"s55", 119},    \
      {"s56", 120}, {"s57", 121}, {"s58", 122}, {"s59", 123}, {"s60", 124},    \
      {"s61", 125}, {"s62", 126}, {"s63", 127},                                \
  }

/* ********** Miscellaneous Parameters ********** */

/* When doing PIC jumptable, stores relative address as 32bits.
 */
#define CASE_VECTOR_MODE                                                       \
  ((CASE_VECTOR_PC_RELATIVE || flag_pic) ? SImode : Pmode)

/* Place jump tables next to code for PIC so that PCREL relocations
 are resolved during assembly */
#define JUMP_TABLES_IN_TEXT_SECTION (flag_pic || CASE_VECTOR_PC_RELATIVE)

/* The maximum number of bytes that a single instruction can move quickly from
   memory to memory.  */
#define MOVE_MAX 8

/* A C expression which is nonzero if on this machine it is safe to "convert"
   an integer of INPREC bits to one of OUTPREC bits (where OUTPREC is smaller
   than INPREC) by merely operating on it as if it had only OUTPREC bits.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/* An alias for the machine mode for pointers.  */
#define Pmode (TARGET_32 ? SImode : DImode)

/* An alias for the machine mode used for memory references to
   functions being called, in call RTL expressions. On most CISC
   machines, where an instruction can begin at any byte address, this
   should be QImode. On most RISC machines, where all instructions
   have fixed size and alignment, this should be a mode with the same
   size and alignment as the machine instruction words - typically
   SImode or HImode. */
#define FUNCTION_MODE Pmode

/* A C compound statement to output to stdio stream stream the
   assembler syntax for an instruction operand x. x is an RTL
   expression.

   code is a value that can be used to specify one of several ways of
   printing the operand. It is used when identical operands must be
   printed differently depending on the context. code comes from the
   `%' specification that was used to request printing of the
   operand. If the specification was just `%digit' then code is 0; if
   the specification was `%ltr digit' then code is the ASCII code for
   ltr.

   If x is a register, this macro should print the register's
   name. The names can be found in an array reg_names whose type is
   char *[]. reg_names is initialized from REGISTER_NAMES.

   When the machine description has a specification `%punct' (a `%'
   followed by a punctuation character), this macro is called with a
   null pointer for x and the punctuation character for code. */
#undef PRINT_OPERAND
#define PRINT_OPERAND k1_target_print_operand

/* A C compound statement to output to stdio stream stream the
   assembler syntax for an instruction operand that is a memory
   reference whose address is x. x is an RTL expression.

   On some machines, the syntax for a symbolic address depends on the
   section that the address refers to. On these machines, define the
   hook TARGET_ENCODE_SECTION_INFO to store the information into the
   symbol_ref, and then check for it here. See Assembler Format. */
#undef PRINT_OPERAND_ADDRESS
#define PRINT_OPERAND_ADDRESS k1_target_print_operand_address

/* A C expression which evaluates to true if code is a valid
   punctuation character for use in the PRINT_OPERAND macro. If
   PRINT_OPERAND_PUNCT_VALID_P is not defined, it means that no
   punctuation characters (except for the standard one, `%') are used
   in this way. */
#undef PRINT_OPERAND_PUNCT_VALID_P
#define PRINT_OPERAND_PUNCT_VALID_P k1_target_print_punct_valid_p

#undef STORE_FLAG_VALUE
#define STORE_FLAG_VALUE 1

/* A C expression that indicates whether the architecture defines a
   value for clz or ctz with a zero operand. A result of 0 indicates
   the value is undefined. If the value is defined for only the RTL
   expression, the macro should evaluate to 1; if the value applies
   also to the corresponding optab entry (which is normally the case
   if it expands directly into the corresponding RTL), then the macro
   should evaluate to 2. In the cases where the value is defined,
   value should be set to this value.

   If this macro is not defined, the value of clz or ctz at zero is
   assumed to be undefined.

   This macro must be defined if the target's expansion for ffs relies
   on a particular value to get correct results. Otherwise it is not
   necessary, though it may be used to optimize some corner cases, and
   to provide a default expansion for the ffs optab.

   Note that regardless of this macro the “definedness” of clz and ctz
   at zero do not extend to the builtin functions visible to the
   user. Thus one may be free to adjust the value at will to match the
   target expansion of these operations without fear of breaking the
   API. */
#define CLZ_DEFINED_VALUE_AT_ZERO(mode, value)                                 \
  (mode == SImode ? (value = 32, 1) : (mode == DImode ? (value = 64, 1) : 0))
#define CTZ_DEFINED_VALUE_AT_ZERO(mode, value)                                 \
  CLZ_DEFINED_VALUE_AT_ZERO (mode, value)

/* Copy/paste from cr16 port
   Macros to check the range of integers . These macros are used across
   the port, majorly in constraints.md, predicates.md files. */
#define SIGNED_INT_FITS_N_BITS(imm, N)                                         \
  ((((imm) < ((HOST_WIDE_INT) 1 << ((N) -1)))                                  \
    && ((imm) >= -((HOST_WIDE_INT) 1 << ((N) -1))))                            \
     ? 1                                                                       \
     : 0)

#define UNSIGNED_INT_FITS_N_BITS(imm, N)                                       \
  (((imm) < ((HOST_WIDE_INT) 1 << (N)) && (imm) >= (HOST_WIDE_INT) 0) ? 1 : 0)

#define k1_strict_to_nonstrict_comparison_operator(code)                       \
  __extension__({                                                              \
    enum rtx_code comp;                                                        \
                                                                               \
    switch (code)                                                              \
      {                                                                        \
      case GT:                                                                 \
	comp = GE;                                                             \
	break;                                                                 \
      case GE:                                                                 \
	comp = GT;                                                             \
	break;                                                                 \
      case GTU:                                                                \
	comp = GEU;                                                            \
	break;                                                                 \
      case GEU:                                                                \
	comp = GTU;                                                            \
	break;                                                                 \
      case LT:                                                                 \
	comp = LE;                                                             \
	break;                                                                 \
      case LE:                                                                 \
	comp = LT;                                                             \
	break;                                                                 \
      case LTU:                                                                \
	comp = LEU;                                                            \
	break;                                                                 \
      case LEU:                                                                \
	comp = LTU;                                                            \
	break;                                                                 \
      default:                                                                 \
	gcc_unreachable ();                                                    \
      }                                                                        \
                                                                               \
    comp;                                                                      \
  })

#define STACK_CHECK_BUILTIN 1

/* Address spaces

   The __bypass and __preload address spaces refers to same space as the generic
   one but accesses to __bypass and __preload objects are achieved using
   uncached and uncached speculative load instructions. */
#define K1_ADDR_SPACE_BYPASS 1
#define K1_ADDR_SPACE_PRELOAD 2
#define K1_ADDR_SPACE_CONVERT 3

#define REGISTER_TARGET_PRAGMAS()                                              \
  do                                                                           \
    {                                                                          \
      c_register_addr_space ("__uncached", K1_ADDR_SPACE_BYPASS);              \
      c_register_addr_space ("__bypass", K1_ADDR_SPACE_BYPASS);                \
      c_register_addr_space ("__preload", K1_ADDR_SPACE_PRELOAD);              \
      c_register_addr_space ("__convert", K1_ADDR_SPACE_CONVERT);              \
    }                                                                          \
  while (0)

#ifndef IN_LIBGCC2

extern GTY (()) rtx k1_sync_reg_rtx;
extern GTY (()) rtx k1_link_reg_rtx;

extern rtx k1_get_stack_check_block (void);

#endif
