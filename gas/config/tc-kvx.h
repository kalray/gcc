/**
*** (c) Copyright Hewlett-Packard Company 1999-2003
*** (c) Copyright Kalray 2010-2018
***
*** This program is free software; you can redistribute it and/or
*** modify it under the terms of the GNU General Public License
*** as published by the Free Software Foundation; either version
*** 2 of the License, or (at your option) any later version.
***
*** This program is distributed in the hope that it will be useful,
*** but WITHOUT ANY WARRANTY; without even the implied warranty of
*** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
*** General Public License for more details.
***
*** You should have received a copy of the GNU General Public License
*** along with this program; if not, write to the Free Software
*** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**/

#define TC_KVX
#define TARGET_ARCH bfd_arch_kvx

#define KV3_RA_REGNO (67)
#define KV3_SP_REGNO (12)

extern const char * kvx_target_format (void);
#define TARGET_FORMAT kvx_target_format ()

/* Uncomment this if the compiler prepends an _ to global names */
/*
 * #define STRIP_UNDERSCORE 
 */

/* default little endian */

#define TARGET_BYTES_BIG_ENDIAN 0    

/* for now we have no BFD target */

/* lexing macros */

#define LEX_QM (LEX_BEGIN_NAME | LEX_NAME)
#define LEX_DOLLAR (LEX_BEGIN_NAME | LEX_NAME)
#define WORKING_DOT_WORD

#define md_end kvx_end
extern void kvx_end (void);

extern void kvx_handle_align (fragS *);
#define HANDLE_ALIGN kvx_handle_align

#define md_number_to_chars number_to_chars_littleendian

#define TC_FIX_TYPE struct _symbol_struct *
#define TC_SYMFILED_TYPE struct list_info_struct *
#define REPEAT_CONS_EXPRESSIONS
#define TC_INIT_FIX_DATA(FIXP) ((FIXP)->tc_fix_data = NULL)

#define tc_frob_label(sym) kvx_frob_label(sym)
extern void kvx_frob_label (struct symbol *);
/* listings */


int is_constant_expression(expressionS*);

/*
 *  Make sure that absolute symbols and imported symbols
 *  that are never used do not get written to .o file
 */

#ifdef OBJ_ELF
#define tc_frob_symbol(sym,punt) \
  { \
    if ((S_GET_SEGMENT (sym) == undefined_section \
        && ! symbol_used_p (sym)\
        && ELF_ST_VISIBILITY (S_GET_OTHER (sym)) == STV_DEFAULT) \
        || (S_GET_SEGMENT (sym) == absolute_section \
            && ! S_IS_EXTERNAL (sym))) \
      punt = 1; \
  }
#endif


#define LISTING_HEADER "KVX GAS LISTING"
#define LISTING_LHS_CONT_LINES 100
#define md_start_line_hook kvx_md_start_line_hook
extern void kvx_md_start_line_hook (void);

/* Values passed to md_apply_fix3 don't include the symbol value.  */
#define MD_APPLY_SYM_VALUE(FIX) 0

extern int kvx_force_reloc (struct fix *);
#define TC_FORCE_RELOCATION(fixP)			\
		kvx_force_reloc(fixP)

#define DIFF_EXPR_OK 1

#define TC_KVX_KVX
extern int kvx_force_reloc_sub_same (struct fix *, segT);
#define TC_FORCE_RELOCATION_SUB_SAME(FIX, SEC)		\
  (! SEG_NORMAL (S_GET_SEGMENT((FIX)->fx_addsy))	\
   || kvx_force_reloc_sub_same(FIX, SEC))

/* This expression evaluates to true if the relocation is for a local object
   for which we still want to do the relocation at runtime.  False if we
   are willing to perform this relocation while building the .o file.

   We can't resolve references to the GOT or the PLT when creating the
   object file, since these tables are only created by the linker.
   Also, if the symbol is global, weak, common or not defined, the
   assembler can't compute the appropriate reloc, since its location
   can only be determined at link time.  */

#define TC_FORCE_RELOCATION_LOCAL(FIX)			\
  (!(FIX)->fx_pcrel					\
   || TC_FORCE_RELOCATION (FIX))

/* This expression evaluates to false if the relocation is for a local object
   for which we still want to do the relocation at runtime.  True if we
   are willing to perform this relocation while building the .o file.
   This is only used for pcrel relocations.
   Use this to ensure that a branch to a preemptible symbol is not
   resolved by the assembler. */

#define TC_RELOC_RTSYM_LOC_FIXUP(FIX)				\
  ((FIX)->fx_r_type != BFD_RELOC_KVX_23_PCREL                    \
   || (FIX)->fx_addsy == NULL					\
   || (! S_IS_EXTERNAL ((FIX)->fx_addsy)			\
       && ! S_IS_WEAK ((FIX)->fx_addsy)			        \
       && S_IS_DEFINED ((FIX)->fx_addsy)			\
       && ! S_IS_COMMON ((FIX)->fx_addsy)))

# define EXTERN_FORCE_RELOC 1
#define tc_fix_adjustable(fixP) 1

/* This arranges for gas/write.c to not apply a relocation if
   tc_fix_adjustable() says it is not adjustable.
   The "! symbol_used_in_reloc_p" test is there specifically to cover
   the case of non-global symbols in linkonce sections.  It's the
   generally correct thing to do though;  If a reloc is going to be
   emitted against a symbol then we don't want to adjust the fixup by
   applying the reloc during assembly.  The reloc will be applied by
   the linker during final link.  */
#define TC_FIX_ADJUSTABLE(fixP) \
  (! symbol_used_in_reloc_p ((fixP)->fx_addsy) && tc_fix_adjustable (fixP))

extern void kvx_validate_fix (struct fix *);
#define TC_VALIDATE_FIX(fix,seg,skip)	kvx_validate_fix (fix)

/* Force this to avoid -g to fail because of dwarf2 expression .L0 - .L0 */
extern int kvx_validate_sub_fix (struct fix *fix);
#define TC_VALIDATE_FIX_SUB(FIX, SEG)			\
  (((FIX)->fx_r_type == BFD_RELOC_32			\
    || (FIX)->fx_r_type == BFD_RELOC_16) &&		\
   kvx_validate_sub_fix((FIX)))

#define TC_CONS_FIX_NEW(FRAG,OFF,LEN,EXP,RELOC) kvx_cons_fix_new(FRAG,OFF,LEN,EXP,RELOC)
extern void kvx_cons_fix_new (fragS *f, int where, int nbytes,
			     expressionS *exp, bfd_reloc_code_real_type);

/* No post-alignment of sections */
#define SUB_SEGMENT_ALIGN(SEG, FRCHAIN) 0

#  define MAX_MEM_FOR_RS_ALIGN_CODE  ((1 << alignment) - 1)

#ifdef OBJ_ELF
#define TARGET_USE_CFIPOP 1
extern void kvx_cfi_frame_initial_instructions (void);

#define tc_cfi_frame_initial_instructions kvx_cfi_frame_initial_instructions

#define tc_regname_to_dw2regnum kvx_regname_to_dw2regnum
extern int kvx_regname_to_dw2regnum (const char *regname);

/* All KVX instructions are multiples of 32 bits.  */
#define DWARF2_LINE_MIN_INSN_LENGTH 1
#define DWARF2_DEFAULT_RETURN_COLUMN (KV3_RA_REGNO)
#define DWARF2_CIE_DATA_ALIGNMENT -4
#endif
