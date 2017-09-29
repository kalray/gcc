/* Helpers */

#ifndef IN_LIBGCC2
#include "coretypes.h"
#include "statistics.h"
#include "stringpool.h"
#include "vec.h"
#endif

#define K1C_NO_EXT_MASK 0x0
//#define K1C_ARF_EXT_MASK 0x0
#define K1C_PRF_EXT_MASK 0x0
#define K1C_GRF_EXT_MASK 0x0
#define K1C_SRF_EXT_MASK 0x1
#define K1C_SRF32_EXT_MASK 0x1
#define K1C_SRF64_EXT_MASK 0x0
#define K1C_ALL_EXT_MASK 0x1

#ifndef _K1_REGS
#define _K1_REGS
#include "k1c-registers.h"
#endif

//#define TARGET_K1C (TARGET_K1CDP | TARGET_K1CIO)

#define K1C_SCRATCH_AREA_SIZE 16

// FIXME AUTO: aarch64 uses a reg class for stack register. Maybe do the same ?

// FIXME AUTO: Coolidge: what is this MDS+1 register.
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

// FIXME AUTO PRF DISABLED
/* #define IS_PRF_REGNO(num, strict)     (!(num % 2) && (TEST_REGNO(num, <, 64,
 * strict) || TEST_REGNO(num, ==, (K1C_MDS_REGISTERS+1), strict))) */

/* Do not use Transactionnal Memory as it makes the linux
 * build fail */
// FIXME AUTO COOLIDGE disable atypical define
//#define USE_TM_CLONE_REGISTRY 0

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
	  if (TARGET_GPREL)                                                    \
	    builtin_define ("__K1_GPREL__");                                   \
	  if (TARGET_64)                                                       \
	    builtin_define ("__K1_64__");                                      \
	}                                                                      \
    }                                                                          \
  while (0)

/* "%{mcore=k1io: -fstrict-align} ",                             \ */
/* "%{mcore=k1bio: -fstrict-align} ",			      \ */
#define DRIVER_SELF_SPECS_COMMON                                               \
  K1_OS_SELF_SPECS                                                             \
  "%{fpic: %{!fPIC:-fPIC}} %<fpic", "%{fPIC: %<fpic}"

/*
	"%{!mno-gprel: -mgprel}"                        \
*/
/* "%{!mboard*:" K1_DEFAULT_BOARD "} ",                          \ */

//#define CPP_SPEC_COMMON

/* "%{mcluster=*:%:cluster_to_define(%{mcluster=*:%*})} "			\
 */
/*     "%{mcluster=ioddr: -D__iocomm__}"                                   \ */
/*     "%{mcluster=ioeth: -D__iocomm__}"                                   \ */
/*     "%{mcluster=ioddr_ddr: -D__iocomm__}"                                   \
 */
/*     "%{mcluster=node_msd: -D__node__}" */

#define LINK_SPEC_COMMON "%{shared} %{m64:-melf64k1}"

#define CC1_SPEC " %{G*}"

#define ASM_SPEC "%{mcore*} --no-check-resources %{m64}"

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

/* Normal alignment required for function parameters on the stack, in
   bits. All stack parameters receive at least this much alignment
   regardless of data type. On most machines, this is the same as the
   size of an integer. */
#define PARM_BOUNDARY 32

/* Define this macro to the minimum alignment enforced by hardware for
   the stack pointer on this machine. The definition is a C expression
   for the desired alignment (measured in bits). This value is used as
   a default if PREFERRED_STACK_BOUNDARY is not defined. On most
   machines, this should be the same as PARM_BOUNDARY. */
#define STACK_BOUNDARY 32

/* Define this macro if you wish to preserve a certain alignment for
   the stack pointer, greater than what the hardware enforces.  The
   definition is a C expression for the desired alignment (measured
   in bits).  This macro must evaluate to a value equal to or larger
   than `STACK_BOUNDARY'. */
#define PREFERRED_STACK_BOUNDARY 64

/* Alignment required for a function entry point, in bits.  */
#define FUNCTION_BOUNDARY 64

/* Biggest alignment that any data type can require on this machine,
   in bits.  */
#define BIGGEST_ALIGNMENT 64

/* Alignment, in bits, a C conformant malloc implementation has to
   provide. If not defined, the default value is BITS_PER_WORD.  */
#define MALLOC_ABI_ALIGNMENT 64

/* Define this if instructions will fail to work if given data not
   on the nominal alignment.  If instructions will merely go slower
   in that case, do not define this macro. */
#define STRICT_ALIGNMENT TARGET_STRICT_ALIGN

/* Define this if you wish to imitate the way many other C compilers handle
   alignment of bitfields and the structures that contain them.
   The behavior is that the type written for a bitfield (`int', `short', or
   other integer type) imposes an alignment for the entire structure, as if the
   structure really did contain an ordinary field of that type.  In addition,
   the bitfield is placed within the structure so that it would fit within such
   a field, not crossing a boundary for it. */
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

#define STACK_SLOT_ALIGNMENT(type, mode, align)                                \
  ((mode == BLKmode ? 8 : GET_MODE_SIZE (mode)) > 4 ? BIGGEST_ALIGNMENT        \
						    : BITS_PER_WORD)

/* ********** Type Layout ********** */

#define POINTER_SIZE (TARGET_64 ? 64 : 32)

/* A C expression for the size in bits of the type `int' on the target machine.
   If you don't define this, the default is one word.  */
#define INT_TYPE_SIZE 32

/* A C expression for the size in bits of the type `short' on the target
   machine.  If you don't define this, the default is half a word.  (If this
   would be less than one storage unit, it is rounded up to one unit.)  */
#define SHORT_TYPE_SIZE 16

/* A C expression for the size in bits of the type `long' on the target
   machine.  If you don't define this, the default is one word.  */
#define LONG_TYPE_SIZE (TARGET_64 ? 64 : 32)

/* A C expression for the size in bits of the type `long long' on the target
   machine.  If you don't define this, the default is two words.  If you want
   to support GNU Ada on your machine, the value of macro must be at least 64.
 */
#define LONG_LONG_TYPE_SIZE 64

/* A C expression for the size in bits of the type `char' on the target
   machine.  If you don't define this, the default is one quarter of a word.
   (If this would be less than one storage unit, it is rounded up to one unit.)
 */
#define CHAR_TYPE_SIZE 8

/* A C expression for the size in bits of the type `float' on the target
   machine.  If you don't define this, the default is one word.  */
#define FLOAT_TYPE_SIZE 32

/* A C expression for the size in bits of the type `double' on the target
   machine.  If you don't define this, the default is two words.  */
#define DOUBLE_TYPE_SIZE 64

/* A C expression for the size in bits of the type `long double' on the target
   machine.  If you don't define this, the default is two words.  */
#define LONG_DOUBLE_TYPE_SIZE 64

/* An expression whose value is 1 or 0, according to whether the type `char'
   should be signed or unsigned by default.  The user can always override this
   default with the options `-fsigned-char' and `-funsigned-char'.  */
#define DEFAULT_SIGNED_CHAR 1

/* A C expression for a string describing the name of the data type to use for
   size values.  The typedef name `size_t' is defined using the contents of the
   string.  */
#define SIZE_TYPE (TARGET_64 ? "long unsigned int" : "unsigned int")

/* A C expression for a string describing the name of the data type to use for
   the result of subtracting two pointers.  The typedef name `ptrdiff_t' is
   defined using the contents of the string.  See `SIZE_TYPE' above for more
   information.  */
#define PTRDIFF_TYPE (TARGET_64 ? "long int" : "int")

/* ********** Registers ********** */

/* Number of hardware registers known to the compiler. They receive
   numbers 0 through FIRST_PSEUDO_REGISTER-1; thus, the first pseudo
   register's number really is assigned the number
   FIRST_PSEUDO_REGISTER.  */
#define FIRST_PSEUDO_REGISTER (K1C_MDS_REGISTERS + 1)

/* An initializer that says which registers are used for fixed
   purposes all throughout the compiled code and are therefore not
   available for general allocation. These would include the stack
   pointer, the frame pointer (except on machines where that can be
   used as a general register when no frame pointer is needed), the
   program counter on machines where that is considered one of the
   addressable registers, and any other numbered register with a
   standard use.

    This information is expressed as a sequence of numbers, separated
    by commas and surrounded by braces. The nth number is 1 if
    register n is fixed, 0 otherwise.

    The table initialized from this macro, and the table initialized
    by the following one, may be overridden at run time either
    automatically, by the actions of the macro
    CONDITIONAL_REGISTER_USAGE, or by the user with the command
    options -ffixed-reg, -fcall-used-reg and -fcall-saved-reg.  */
#define FIXED_REGISTERS                                                        \
  {                                                                            \
    K1C_FIXED_REGISTERS                                                        \
    1,                                                                         \
  }

/* Like FIXED_REGISTERS but has 1 for each register that is clobbered
   (in general) by function calls as well as for fixed registers. This
   macro therefore identifies the registers that are not available for
   general allocation of values that must live across function calls.

   If a register has 0 in CALL_USED_REGISTERS, the compiler
   automatically saves it on function entry and restores it on
   function exit, if the register is used within the function.  */
#define CALL_USED_REGISTERS                                                    \
  {                                                                            \
    K1C_CALL_USED_REGISTERS                                                    \
    1,                                                                         \
  }

/* Like CALL_USED_REGISTERS except this macro doesn't require that
   the entire set of FIXED_REGISTERS be
   included. (CALL_USED_REGISTERS must be a superset of
   FIXED_REGISTERS). This macro is optional. If not specified, it
   defaults to the value of CALL_USED_REGISTERS. */
#define CALL_REALLY_USED_REGISTERS                                             \
  {                                                                            \
    K1C_CALL_REALLY_USED_REGISTERS                                             \
    1,                                                                         \
  }

/* If the program counter has a register number, define this as that
   register number. Otherwise, do not define it. */
#define PC_REGNUM K1C_PROGRAM_POINTER_REGNO

/* A C expression for the number of consecutive hard registers,
   starting at register number regno, required to hold a value of mode
   mode. This macro must never return zero, even if a register cannot
   hold the requested mode - indicate that with HARD_REGNO_MODE_OK
   and/or CANNOT_CHANGE_MODE_CLASS instead.  */
#define HARD_REGNO_NREGS(REGNO, MODE)                                          \
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* If defined, a C expression that returns nonzero for a class for
   which a change from mode from to mode to is invalid. */
#define CANNOT_CHANGE_MODE_CLASS(from, to, class)                              \
  k1_cannot_change_mode_class (from, to, class)

/* A C expression that is nonzero if it is permissible to store a
   value of mode mode in hard register number regno (or in several
   registers starting with that one). */
#define HARD_REGNO_MODE_OK(REGNO, MODE) k1_hard_regno_mode_ok (REGNO, MODE)

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

/* use r16..r33 before r10 and r15, because the former are more easily stored by
 * pair */
// FIXME AUTO: Coolidge may need to change this when packing to 128bits ld/st
/* #define REG_ALLOC_ORDER { \ */
/*          0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  11, 12, 13 ,14, 	\ */
/* 	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, \ */
/* 	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, \ */
/* 	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, \ */
/* 	10, 15, \ */
/* 	} */

/* A macro whose definition is the name of the class to which a valid
   base register must belong. A base register is one used in an
   address which is the register value plus a displacement. */
// FIXME AUTO PRF DISABLED
/* #define BASE_REG_CLASS (TARGET_64 ? PRF_REGS : GENERAL_REGS) */
#define BASE_REG_CLASS (GENERAL_REGS)

/* A macro whose definition is the name of the class to which a valid
   index register must belong. An index register is one used in an
   address where its value is either multiplied by a scale factor or
   added to another register (as well as added to a displacement). */
#define INDEX_REG_CLASS GENERAL_REGS

#define SYMBOLIC_CONST(X)                                                      \
  (GET_CODE (X) == SYMBOL_REF || GET_CODE (X) == LABEL_REF                     \
   || (GET_CODE (X) == CONST && symbolic_reference_mentioned_p (X)))

#define LEGITIMIZE_RELOAD_ADDRESS(x, mode, opnum, type, ind_levels, win)       \
  {                                                                            \
    if (k1_legitimize_reload_address (&x, opnum, type))                        \
      goto win;                                                                \
  }

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

// FIXME AUTO PRF DISABLED
/* #ifdef REG_OK_STRICT */
/* #define REGNO_OK_FOR_BASE_P(num) (TARGET_64 ? IS_PRF_REGNO(num, 1) :
 * IS_GENERAL_REGNO(num, 1)) */
/* #else */
/* #define REGNO_OK_FOR_BASE_P(num) (TARGET_64 ? IS_PRF_REGNO(num, 0) :
 * IS_GENERAL_REGNO(num, 0)) */
/* #endif */
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

/* Define this macro to nonzero value if the addresses of local
   variable slots are at negative offsets from the frame pointer. */
/* FIXME */
#define FRAME_GROWS_DOWNWARD 1

/* Offset from the frame pointer to the first local variable slot to
   be allocated.

   If FRAME_GROWS_DOWNWARD, find the next slot's offset by subtracting
   the first slot's length from STARTING_FRAME_OFFSET. Otherwise, it
   is found by adding the length of the first slot to the value
   STARTING_FRAME_OFFSET. */
#define STARTING_FRAME_OFFSET K1C_SCRATCH_AREA_SIZE

/* Offset from the stack pointer register to the first location at
   which outgoing arguments are placed. If not specified, the default
   value of zero is used. This is the proper value for most machines.

   If ARGS_GROW_DOWNWARD, this is the offset to the location above the
   first location at which outgoing arguments are placed. */
#define STACK_POINTER_OFFSET K1C_SCRATCH_AREA_SIZE

/* Offset from the argument pointer register to the first argument's
   address. On some machines it may depend on the data type of the function.

   If ARGS_GROW_DOWNWARD, this is the offset to the location above the
   first argument's address. */
// FIXME AUTO: Used a more simple value. Maybe wrong.
//#define FIRST_PARM_OFFSET(funcdecl) (((cfun->stdarg && crtl->args.info <
//K1C_ARG_REG_SLOTS)? (UNITS_PER_WORD * ((K1C_ARG_REG_SLOTS - crtl->args.info +
//1) & ~1)): 0) + STARTING_FRAME_OFFSET)
#define FIRST_PARM_OFFSET(funcdecl) (STARTING_FRAME_OFFSET)

/* A C expression whose value is RTL representing the value of the
   return address for the frame count steps up from the current frame,
   after the prologue. frameaddr is the frame pointer of the count
   frame, or the frame pointer of the count − 1 frame if
   RETURN_ADDR_IN_PREVIOUS_FRAME is defined.

   The value of the expression must always be the correct address when
   count is zero, but may be NULL_RTX if there is no way to determine
   the return address of other frames. */
#define RETURN_ADDR_RTX(COUNT, FRAMEADDR) k1_return_addr_rtx (COUNT, FRAMEADDR)

/* FIXME AUTO: Disable FRAME_ADDR_RTX macro. Maybe this is wrong  */
//#define FRAME_ADDR_RTX(rtx) plus_constant (Pmode, rtx, K1C_SCRATCH_AREA_SIZE)

#define DWARF2_UNWIND_INFO 1
#define DWARF2_ASM_LINE_DEBUG_INFO 1

#undef PREFERRED_DEBUGGING_TYPE
#define PREFERRED_DEBUGGING_TYPE DWARF2_DEBUG

/* A C expression whose value is RTL representing the location of the
   incoming return address at the beginning of any function, before
   the prologue. This RTL is either a REG, indicating that the return
   value is saved in `REG', or a MEM representing a location in the
   stack.

   You only need to define this macro if you want to support call
   frame debugging information like that provided by DWARF 2.

   If this RTL is a REG, you should also define
   DWARF_FRAME_RETURN_COLUMN to DWARF_FRAME_REGNUM (REGNO). */
#define INCOMING_RETURN_ADDR_RTX gen_rtx_REG (Pmode, K1C_RETURN_POINTER_REGNO)

#define DBX_REGISTER_NUMBER(NUM) (NUM)

// FIXME AUTO: Add DWARF_FRAME_REGNUM macro, untested, unsure.
#define DWARF_FRAME_REGNUM(REGNO) DBX_REGISTER_NUMBER (REGNO)

#define DWARF_FRAME_RETURN_COLUMN DBX_REGISTER_NUMBER (K1C_RETURN_POINTER_REGNO)

/* A C expression whose value is an integer giving the offset, in
   bytes, from the value of the stack pointer register to the top of
   the stack frame at the beginning of any function, before the
   prologue. The top of the frame is defined to be the value of the
   stack pointer in the previous frame, just before the call
   instruction.

   You only need to define this macro if you want to support call
   frame debugging information like that provided by DWARF 2. */
#define INCOMING_FRAME_SP_OFFSET K1C_SCRATCH_AREA_SIZE

/* A C expression whose value is an integer giving the offset, in
   bytes, from the argument pointer to the canonical frame address
   (cfa). The final value should coincide with that calculated by
   INCOMING_FRAME_SP_OFFSET. Which is unfortunately not usable during
   virtual register instantiation.

   The default value for this macro is FIRST_PARM_OFFSET (fundecl) +
   crtl->args.pretend_args_size, which is correct for most machines;
   in general, the arguments are found immediately before the stack
   frame. Note that this is not the case on some targets that save
   registers into the caller's frame, such as SPARC and rs6000, and so
   such targets need to define this macro.

   You only need to define this macro if the default is incorrect, and
   you want to support call frame debugging information like that
   provided by DWARF 2. */
// FIXME AUTO: disabled for coolidge
//#define ARG_POINTER_CFA_OFFSET(funcdecl) K1C_SCRATCH_AREA_SIZE +
//crtl->args.pretend_args_size + (cfun->stdarg && crtl->args.info <
//K1C_ARG_REG_SLOTS ? UNITS_PER_WORD * ((K1C_ARG_REG_SLOTS - crtl->args.info +
//1) & ~1) : 0)

/* This macro need only be defined if the target might save registers
   in the function prologue at an offset to the stack pointer that is
   not aligned to UNITS_PER_WORD. The definition should be the
   negative minimum alignment if STACK_GROWS_DOWNWARD is true, and the
   positive minimum alignment otherwise. See SDB and DWARF. Only
   applicable if the target supports DWARF 2 frame unwind
   information. */

#define DWARF_CIE_DATA_ALIGNMENT (-4) //(TARGET_64 ? 8 : 4))

/* If defined, a C expression whose value is an integer giving the
   offset in bytes from the frame pointer to the canonical frame
   address (cfa). The final value should coincide with that calculated
   by INCOMING_FRAME_SP_OFFSET.

   Normally the CFA is calculated as an offset from the argument
   pointer, via ARG_POINTER_CFA_OFFSET, but if the argument pointer is
   variable due to the ABI, this may not be possible. If this macro is
   defined, it implies that the virtual register instantiation should
   be based on the frame pointer instead of the argument pointer. Only
   one of FRAME_POINTER_CFA_OFFSET and ARG_POINTER_CFA_OFFSET should
   be defined. */
/* #define FRAME_POINTER_CFA_OFFSET(func) K1C_SCRATCH_AREA_SIZE +
 * crtl->args.pretend_args_size + (cfun->stdarg && crtl->args.info <
 * K1C_ARG_REG_SLOTS ? UNITS_PER_WORD * ((K1C_ARG_REG_SLOTS - crtl->args.info +
 * 1) & ~1) : 0) */

#define STACK_POINTER_REGNUM K1C_STACK_POINTER_REGNO

#define FRAME_POINTER_REGNUM K1C_FRAME_POINTER_REGNO

/* The register number of the arg pointer register, which is used to
   access the function's argument list. On some machines, this is the
   same as the frame pointer register. On some machines, the hardware
   determines which register this is. On other machines, you can
   choose any register you wish for this purpose. If this is not the
   same register as the frame pointer register, then you must mark it
   as a fixed register according to FIXED_REGISTERS, or arrange to be
   able to eliminate it (see Elimination). */
#define ARG_POINTER_REGNUM FRAME_POINTER_REGNUM

/* Register numbers used for passing a function's static chain
   pointer. If register windows are used, the register number as seen
   by the called function is STATIC_CHAIN_INCOMING_REGNUM, while the
   register number as seen by the calling function is
   STATIC_CHAIN_REGNUM. If these registers are the same,
   STATIC_CHAIN_INCOMING_REGNUM need not be defined.

   The static chain register need not be a fixed register.

   If the static chain is passed in memory, these macros should not be
   defined; instead, the TARGET_STATIC_CHAIN hook should be used. */
#define STATIC_CHAIN_REGNUM K1C_STATIC_POINTER_REGNO

/* ********** Elimination ********** */
#define ELIMINABLE_REGS                                                        \
  {                                                                            \
    {ARG_POINTER_REGNUM, STACK_POINTER_REGNUM},                                \
      {ARG_POINTER_REGNUM, FRAME_POINTER_REGNUM},                              \
    {                                                                          \
      FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM                               \
    }                                                                          \
  }

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

/* A C type for declaring a variable that is used as the first argument of
   `FUNCTION_ARG' and other related values.  For some target machines, the type
   `int' suffices and can hold the number of bytes of argument so far.  */
#define CUMULATIVE_ARGS int

/* A C statement (sans semicolon) for initializing the variable cum
   for the state at the beginning of the argument list. The variable
   has type CUMULATIVE_ARGS. The value of fntype is the tree node for
   the data type of the function which will receive the args, or 0 if
   the args are to a compiler support library function. For direct
   calls that are not libcalls, fndecl contain the declaration node of
   the function. fndecl is also set when INIT_CUMULATIVE_ARGS is used
   to find arguments for the function being compiled. n_named_args is
   set to the number of named arguments, including a structure return
   address if it is passed as a parameter, when making a call. When
   processing incoming arguments, n_named_args is set to −1.

   When processing a call to a compiler support library function,
   libname identifies which one. It is a symbol_ref rtx which contains
   the name of the function, as a string. libname is 0 when an
   ordinary C function call is being processed. Thus, each time this
   macro is called, either libname or fntype is nonzero, but never
   both of them at once. */
#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, FNDECL, N_NAMED_ARGS)       \
  (CUM) = 0

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
#define EPILOGUE_USES(regno)                                                   \
  (reload_completed && regno == K1C_RETURN_POINTER_REGNO)

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

#define NO_FUNCTION_CSE

#define ADJUST_INSN_LENGTH(insn, length)                                       \
  (length) = k1_adjust_insn_length ((insn), (length))
int k1_adjust_insn_length (rtx insn, int length);

#define MEMORY_MOVE_COST(mode, class, in) (in ? 6 : 4)

#define BRANCH_COST(speed, predictable) 4

/* ********** PIC ********** */

/* The register number of the register used to address a table of
   static data addresses in memory. In some cases this register is
   defined by a processor's “application binary interface” (ABI). When
   this macro is defined, RTL is generated for this register once, as
   with the stack pointer and frame pointer registers. If this macro
   is not defined, it is up to the machine-dependent files to allocate
   such a register (if necessary). Note that this register must be
   fixed when in use (e.g. when flag_pic is true). */
#define PIC_OFFSET_TABLE_REGNUM                                                \
  (!flag_pic ? INVALID_REGNUM                                                  \
	     : reload_completed ? REGNO (pic_offset_table_rtx)                 \
				: K1C_GLOBAL_POINTER_REGNO)

/* Define this macro if the register defined by
   PIC_OFFSET_TABLE_REGNUM is clobbered by calls. Do not define this
   macro if PIC_OFFSET_TABLE_REGNUM is not defined. */
#define PIC_OFFSET_TABLE_REG_CALL_CLOBBERED 1

#define GOT_SYMBOL_NAME "*_GLOBAL_OFFSET_TABLE_"

/* ********** Sections ********** */

#define TEXT_SECTION_ASM_OP "\t.text"
#define DATA_SECTION_ASM_OP "\t.data"
#define BSS_SECTION_ASM_OP "\t.section .bss"
#define SBSS_SECTION_ASM_OP "\t.section .sbss"
/* A C expression whose value is a string containing the assembler operation to
   switch to the fixup section that records all initialized pointers in a -fpic
   program so they can be changed program startup time if the program is loaded
   at a different address than linked for.  */
#define FIXUP_SECTION_ASM_OP "\t.section .rofixup,\"a\""

/* ********** Assembler Output ********** */

/* Local compiler-generated symbols must have a prefix that the assembler
   understands.   By default, this is $, although some targets (e.g.,
   NetBSD-ELF) need to override this.  */

#define LOCAL_LABEL_PREFIX "."

#define REGISTER_PREFIX "$"

#define ASM_COMMENT_START "#"
#define ASM_APP_ON "\t;;\n#APP\n"
#define ASM_APP_OFF "\n\t;;\n#NO_APP\n"
#define ASM_OUTPUT_ALIGN(FILE, LOG)                                            \
  fprintf (FILE, "\n\t.align %d\n", 1 << (LOG))

/* Override default implementation in elfos.h to support -G.  */
#undef ASM_OUTPUT_ALIGNED_LOCAL
#define ASM_OUTPUT_ALIGNED_LOCAL(FILE, NAME, SIZE, ALIGN)                      \
  do                                                                           \
    {                                                                          \
      if (TARGET_LOCAL_SDATA                                                   \
	  && (SIZE) <= (unsigned HOST_WIDE_INT) g_switch_value)                \
	switch_to_section (sbss_section);                                      \
      else                                                                     \
	switch_to_section (bss_section);                                       \
      ASM_OUTPUT_TYPE_DIRECTIVE (FILE, NAME, "object");                        \
      if (!flag_inhibit_size_directive)                                        \
	ASM_OUTPUT_SIZE_DIRECTIVE (FILE, NAME, SIZE);                          \
      ASM_OUTPUT_ALIGN ((FILE), exact_log2 ((ALIGN) / BITS_PER_UNIT));         \
      ASM_OUTPUT_LABEL (FILE, NAME);                                           \
      ASM_OUTPUT_SKIP ((FILE), (SIZE) ? (SIZE) : 1);                           \
    }                                                                          \
  while (0)

/* Override default implementation in elfos.h to support -G.  */
#undef ASM_OUTPUT_ALIGNED_COMMON
#define ASM_OUTPUT_ALIGNED_COMMON(FILE, NAME, SIZE, ALIGN)                     \
  do                                                                           \
    {                                                                          \
      if (TARGET_LOCAL_SDATA                                                   \
	  && (SIZE) <= (unsigned HOST_WIDE_INT) g_switch_value)                \
	{                                                                      \
	  switch_to_section (sbss_section);                                    \
	  (*targetm.asm_out.globalize_label) (FILE, NAME);                     \
	  ASM_OUTPUT_TYPE_DIRECTIVE (FILE, NAME, "object");                    \
	  if (!flag_inhibit_size_directive)                                    \
	    ASM_OUTPUT_SIZE_DIRECTIVE (FILE, NAME, SIZE);                      \
	  ASM_OUTPUT_ALIGN ((FILE), exact_log2 ((ALIGN) / BITS_PER_UNIT));     \
	  ASM_OUTPUT_LABEL (FILE, NAME);                                       \
	  ASM_OUTPUT_SKIP ((FILE), (SIZE) ? (SIZE) : 1);                       \
	}                                                                      \
      else                                                                     \
	{                                                                      \
	  switch_to_section (bss_section);                                     \
	  fprintf ((FILE), "%s", COMMON_ASM_OP);                               \
	  assemble_name ((FILE), (NAME));                                      \
	  fprintf ((FILE), "," HOST_WIDE_INT_PRINT_UNSIGNED ",%u\n", (SIZE),   \
		   (ALIGN) / BITS_PER_UNIT);                                   \
	}                                                                      \
    }                                                                          \
  while (0)

#undef ASM_OUTPUT_ALIGNED_DECL_LOCAL
#define ASM_OUTPUT_ALIGNED_DECL_LOCAL(FILE, DECL, NAME, SIZE, ALIGN)           \
  do                                                                           \
    {                                                                          \
      if (TARGET_LOCAL_SDATA                                                   \
	  && (SIZE) <= (unsigned HOST_WIDE_INT) g_switch_value)                \
	switch_to_section (sbss_section);                                      \
      else                                                                     \
	switch_to_section (bss_section);                                       \
      ASM_OUTPUT_TYPE_DIRECTIVE (FILE, NAME, "object");                        \
      if (!flag_inhibit_size_directive)                                        \
	ASM_OUTPUT_SIZE_DIRECTIVE (FILE, NAME, SIZE);                          \
      ASM_OUTPUT_ALIGN ((FILE), exact_log2 ((ALIGN) / BITS_PER_UNIT));         \
      ASM_OUTPUT_LABEL (FILE, NAME);                                           \
      ASM_OUTPUT_SKIP ((FILE), (SIZE) ? (SIZE) : 1);                           \
    }                                                                          \
  while (0)

#define GLOBAL_ASM_OP "\t.globl "

/* #define ASM_OUTPUT_ADDR_VEC_ELT(STREAM, VALUE)				\
 */
/*   fprintf (STREAM, "\t.word\t%sL%d\n",					\
 */
/* 	   LOCAL_LABEL_PREFIX,						\ */
/* 	   VALUE) */
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
    ".sync.",                                                                  \
  }

/* If defined, a C initializer for an array of structures containing a
   name and a register number. This macro defines additional names for
   hard registers, thus allowing the asm option in declarations to
   refer to registers using alternate names. */
#define ADDITIONAL_REGISTER_NAMES                                              \
  {                                                                            \
    {"p0", 0}, {"p2", 2}, {"p4", 4}, {"p6", 6}, {"p8", 8}, {"p10", 10},        \
      {"p12", 12}, {"p14", 14}, {"p16", 16}, {"p18", 18}, {"p20", 20},         \
      {"p22", 22}, {"p24", 24}, {"p26", 26}, {"p28", 28}, {"p30", 30},         \
      {"p32", 32}, {"p34", 34}, {"p36", 36}, {"p38", 38}, {"p40", 40},         \
      {"p42", 42}, {"p44", 44}, {"p46", 46}, {"p48", 48}, {"p50", 50},         \
      {"p52", 52}, {"p54", 54}, {"p56", 56}, {"p58", 58}, {"p60", 60},         \
      {"p62", 62}, {"r0r1", 0}, {"r2r3", 2}, {"r4r5", 4}, {"r6r7", 6},         \
      {"r8r9", 8}, {"r10r11", 10}, {"r12r13", 12}, {"r14r15", 14},             \
      {"r16r17", 16}, {"r18r19", 18}, {"r20r21", 20}, {"r22r23", 22},          \
      {"r24r25", 24}, {"r26r27", 26}, {"r28r29", 28}, {"r30r31", 30},          \
      {"r32r33", 32}, {"r34r35", 34}, {"r36r37", 36}, {"r38r39", 38},          \
      {"r40r41", 40}, {"r42r43", 42}, {"r44r45", 44}, {"r46r47", 46},          \
      {"r48r49", 48}, {"r50r51", 50}, {"r52r53", 52}, {"r54r55", 54},          \
      {"r56r57", 56}, {"r58r59", 58}, {"r60r61", 60}, {"r62r63", 62},          \
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

/* Default Small Data treshold to -G 8 */
#ifndef K1C_DEFAULT_GVALUE
#define K1C_DEFAULT_GVALUE 8
#endif

/* An alias for a machine mode name.  This is the machine mode that elements of
   a jump-table should have.  */
#define CASE_VECTOR_MODE Pmode

#define CASE_VECTOR_PC_RELATIVE (flag_pic)

/* FIXME: when storing jumptables in rodata, relocation seem to be
   wronlgy computed along the chain, leading to incorrect jumps */
#define JUMP_TABLES_IN_TEXT_SECTION (flag_pic)

/* The maximum number of bytes that a single instruction can move quickly from
   memory to memory.  */
#define MOVE_MAX 8

/* A C expression which is nonzero if on this machine it is safe to "convert"
   an integer of INPREC bits to one of OUTPREC bits (where OUTPREC is smaller
   than INPREC) by merely operating on it as if it had only OUTPREC bits.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/* An alias for the machine mode for pointers.  */
#define Pmode (TARGET_64 ? DImode : SImode)

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

#define FINAL_PRESCAN_INSN(insn, ops, nops)                                    \
  k1_final_prescan_insn (insn, ops, nops)

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

/* ABI requires 16-byte alignment. */
#define K1_STACK_ALIGN(LOC) (((LOC) + 7) & -8)

#ifndef IN_LIBGCC2

typedef struct GTY (()) fake_SC
{
  rtx sc;
  rtx low, low_insn;
  rtx high, high_insn;
} fake_SC_t;

extern GTY (()) rtx k1_sync_reg_rtx;
extern GTY (()) rtx k1_link_reg_rtx;

extern rtx k1_get_stack_check_block (void);

#endif
