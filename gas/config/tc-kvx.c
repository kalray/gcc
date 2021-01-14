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

#include "as.h"
#include "obstack.h"
#include "subsegs.h"
#include "tc-kvx.h"
#include "opcode/kv3.h"
#include "libiberty.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#ifdef OBJ_ELF
#include "elf/kv3.h"
#include "dwarf2dbg.h"
#include "dw2gencfi.h"
#endif

#define D(args...) do { if(debug) fprintf(args); }while(0)

static void supported_cores(char buf[], size_t buflen);

#define O_pseudo_fixup (O_max + 1)

#define NELEMS(a)	((int) (sizeof (a)/sizeof ((a)[0])))

#define STREQ(x,y) !strcmp(((x) ? (x) : ""), ((y) ? (y) : ""))
#define STRNEQ(x,y,n) !strncmp(((x) ? (x) : ""), ((y) ? (y) : ""),(n))

#define PARALLEL_BIT (1<<31)

/*TB begin*/
int size_type_function = 1;
/*TB end */

/* Resource usage checking is disabled by default, because it
 * can produce false positives. */
static int check_resource_usage = 1;
/* Generate illegal code : only use for debugging !*/
static int generate_illegal_code = 0;
/* Dump asm tables : for debugging */
static int dump_table = 0;
/* Dump instructions: for documentation */
static int dump_insn = 0;
/* arch string passed as argument with -march option */
char *march= NULL;

/* Used for HW validation: allow all SFR on GET/SET/WFX */
int allow_all_sfr = 0;

/* This string should contains position in string where error occured. */
char *error_str=NULL;

/* Default values used if no assume directive is given */
static const Kvx_Core_Info *kvx_core_info = NULL;
static int subcore_id = 0;

/* Default kvx_registers array. */
static const kvx_Register *kvx_registers = NULL;

/* Default kvx_regfiles array. */
static const int *kvx_regfiles = NULL;

int kvx_abi = ELF_KVX_ABI_UNDEF;
int kvx_osabi = ELFOSABI_NONE;
int kvx_core = -1;
int kvx_core_set = 0;
int kvx_abi_set = 0;
int kvx_osabi_set = 0;

static flagword kvx_pic_flags = 0;

/***********************************************/
/*    Generic Globals for GAS                  */
/***********************************************/

const char comment_chars[] = "#";
const char line_comment_chars[] = "#";
const char line_separator_chars[] = ";";
const char EXP_CHARS[] = "eE";
const char FLT_CHARS[] = "dD";
const int md_short_jump_size = 0;
const int md_long_jump_size = 0;

/***********************************************/
/*           Local Types                       */
/***********************************************/

/* a fix up record                       */

typedef struct {
    expressionS exp;                   /* the expression used            */
    int where;                         /* where (byte) in frag this goes */
    bfd_reloc_code_real_type reloc;
} kvx_fixup_t;

/* a single assembled instruction record */
/* may include immediate extension words  */
typedef struct {
  int written;                          /* written out ? */
  const kv3opc_t *opdef;                 /* Opcode table entry for this insn */
  int len;                              /* length of instruction in words (1 or 2) */
  int immx0;                            /* insn is extended */
  int immx1;                            /* insn has two immx */
  int order;                            /* order to stabilize sort */
  uint32_t words[KVXMAXBUNDLEWORDS];     /* instruction words */
  int nfixups;                          /* the number of fixups [0,2] */
  kvx_fixup_t fixup[2];                  /* the actual fixups */
} kvxinsn_t;

typedef void (*print_insn_t)(kv3opc_t *op);
static print_insn_t print_insn = NULL;

typedef enum {MATCH_NOT_FOUND=0, MATCH_FOUND=1} match_operands_code;

#define NOIMMX -1

static kvxinsn_t insbuf[KVXMAXBUNDLEWORDS]; /* Was KVXMAXBUNDLEISSUE, changed because of NOPs */
static int insncnt = 0;
static kvxinsn_t immxbuf[KVXMAXBUNDLEWORDS];
static int immxcnt = 0;

static void incr_immxcnt(void);
static void incr_immxcnt(void)
{
  immxcnt++;
  if(immxcnt >= KVXMAXBUNDLEWORDS) {
    as_bad("Max immx number exceeded: %d",immxcnt);
  }
}

static void set_byte_counter(asection *sec, int value);
static void set_byte_counter(asection *sec, int value)
{
    sec->target_index = value;
}

static int get_byte_counter(asection *sec);
int get_byte_counter(asection *sec)
{
    return sec->target_index;
}

/* Either 32 or 64.  */
static int kvx_arch_size = 64;

const char *
kvx_target_format (void)
{
  return kvx_arch_size == 64 ? "elf64-kvx" : "elf32-kvx";
}


/****************************************************/
/*             Local Variables                      */
/****************************************************/

static struct hash_control *kvx_opcode_hash;

/****************************************************/
/*  ASSEMBLER Pseudo-ops.  Some of this just        */
/*  extends the default definitions                 */
/*  others are KVX specific                          */
/****************************************************/

enum unwrecord
{
    UNW_HEADER,
    UNW_PROLOGUE,
    UNW_BODY,
    UNW_MEM_STACK_F,
    UNW_MEM_STACK_V,
    UNW_PSP_GR,
    UNW_PSP_SPREL,
    UNW_RP_WHEN,
    UNW_RP_GR,
    UNW_RP_PSPREL,
    UNW_RP_SPREL,
    UNW_GR_MEM_S,
    UNW_GR_MEM_L,
    UNW_SPILL_BASE,
    UNW_SPILL_MASK,
    UNW_EPILOGUE,
    UNW_LABEL_STATE,
    UNW_COPY_STATE,
    UNW_SPILL_PSREL,
    UNW_SPILL_SPREL
};
static int get_regnum_by_name(char *name);
static void kvx_check_resources(int);
static void kvx_proc(int start);
static void kvx_endp(int start);
static void kvx_type(int start);
static void kvx_unwind(int r);

const pseudo_typeS md_pseudo_table[] =
{
     /* override default 2-bytes */
     {"word", cons, 4},

     /* KVX specific */
     {"dword", cons, 8},

     /* Override align directives to have a boundary as argument (and not the
        power of two as in p2align)*/
     {"align", s_align_bytes, 0},

     /* KVX specific, should be deprecated */
     {"data1", cons, 1},
     {"data2", cons, 2},
     {"data4", cons, 4},
     {"real4", cons, 4},
     {"data8", cons, 8},	/* uncertain syntax */
     {"real8", cons, 8},	/* uncertain syntax */

     {"checkresources", kvx_check_resources, 1},
     {"nocheckresources", kvx_check_resources, 0},

     /* unwind descriptor directives */

     {"header", kvx_unwind, (int) UNW_HEADER},
     {"prologue", kvx_unwind, (int) UNW_PROLOGUE},
     {"body", kvx_unwind, (int) UNW_BODY},
     {"mem_stack_f", kvx_unwind, (int) UNW_MEM_STACK_F},
     {"mem_stack_v", kvx_unwind, (int) UNW_MEM_STACK_V},
     {"psp_gr", kvx_unwind, (int) UNW_PSP_GR},
     {"psp_sprel", kvx_unwind, (int) UNW_PSP_SPREL},
     {"rp_when", kvx_unwind, (int) UNW_RP_WHEN},
     {"rp_gr", kvx_unwind, (int) UNW_RP_GR},
     {"rp_psrel", kvx_unwind, (int) UNW_RP_PSPREL},
     {"rp_sprel", kvx_unwind, (int) UNW_RP_SPREL},
     {"gr_mem_s", kvx_unwind, (int) UNW_GR_MEM_S},
     {"gr_mem_l", kvx_unwind, (int) UNW_GR_MEM_L},
     {"spill_base", kvx_unwind, (int) UNW_SPILL_BASE},
     {"spill_mask", kvx_unwind, (int) UNW_SPILL_MASK},
     {"epilogue", kvx_unwind, (int) UNW_EPILOGUE},
     {"label_state", kvx_unwind, (int) UNW_LABEL_STATE},
     {"copy_state", kvx_unwind, UNW_COPY_STATE},
     {"spill_psrel", kvx_unwind, (int) UNW_SPILL_PSREL},
     {"spill_sprel", kvx_unwind, (int) UNW_SPILL_SPREL},

     {"endp", kvx_endp, 0},

     {"proc", kvx_proc, 1},
     {"type", kvx_type, 0},

#ifdef OBJ_ELF
     { "file", (void (*) (int)) dwarf2_directive_file, 0},
     { "loc", dwarf2_directive_loc, 0},
#endif
     {NULL, 0, 0}
};

/* Pseudo functions used to indicate relocation types (these functions
 * start with an at sign (@).  */

struct kvx_pseudo_relocs {
  enum {
        S37_LO10_UP27,
        S43_LO10_UP27_EX6,
        S64_LO10_UP27_EX27,
	S16,
        S32,
        S64,
  } reloc_type;
  int bitsize;

  /* Used when pseudo func should expand to different relocations
     based on the 32/64 bits mode.
     Enum values should match the kvx_arch_size var set by -m32
  */
  enum {
	PSEUDO_ALL = 0,
	PSEUDO_32_ONLY = 32,
	PSEUDO_64_ONLY = 64,
  } avail_modes;

  /* set to 1 when pseudo func does not take an argument */
  int has_no_arg;

  bfd_reloc_code_real_type reloc_lo10, reloc_up27, reloc_ex;
  bfd_reloc_code_real_type single;
  kvx_reloc_t *kreloc;
};

struct pseudo_func_s
{
  const char *name;

  symbolS *sym;
  struct kvx_pseudo_relocs pseudo_relocs;
};

static struct pseudo_func_s pseudo_func[] =
  {
   // reloc pseudo functions:
   {
    .name = "gotoff",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 37,
     .reloc_type = S37_LO10_UP27,
     .reloc_lo10 = BFD_RELOC_KVX_S37_GOTOFF_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S37_GOTOFF_UP27,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_gotoff_signed37_reloc,
    }
   },
   {
    .name = "gotoff",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_32_ONLY,
     .bitsize = 32,
     .reloc_type = S32,
     .single = BFD_RELOC_KVX_32_GOTOFF,
     .kreloc = &kv3_gotoff_32_reloc,
    }
   },
   {
    .name = "got",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 37,
     .reloc_type = S37_LO10_UP27,
     .reloc_lo10 = BFD_RELOC_KVX_S37_GOT_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S37_GOT_UP27,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_got_signed37_reloc,
    }
   },
   {
    .name = "got",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_32_ONLY,
     .bitsize = 32,
     .reloc_type = S32,
     .single = BFD_RELOC_KVX_32_GOT,
     .kreloc = &kv3_got_32_reloc,
    }
   },
   {
    .name = "tlsgd",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 37,
     .reloc_type = S37_LO10_UP27,
     .reloc_lo10 = BFD_RELOC_KVX_S37_TLS_GD_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S37_TLS_GD_UP27,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_tlsgd_signed37_reloc,
    }
   },
   {
    .name = "tlsgd",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 43,
     .reloc_type = S43_LO10_UP27_EX6,
     .reloc_lo10 = BFD_RELOC_KVX_S43_TLS_GD_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S43_TLS_GD_UP27,
     .reloc_ex = BFD_RELOC_KVX_S43_TLS_GD_EX6,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_tlsgd_signed43_reloc,
    }
   },
   {
    .name = "tlsle",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 37,
     .reloc_type = S37_LO10_UP27,
     .reloc_lo10 = BFD_RELOC_KVX_S37_TLS_LE_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S37_TLS_LE_UP27,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_tlsle_signed37_reloc,
    }
   },
   {
    .name = "tlsle",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 43,
     .reloc_type = S43_LO10_UP27_EX6,
     .reloc_lo10 = BFD_RELOC_KVX_S43_TLS_LE_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S43_TLS_LE_UP27,
     .reloc_ex = BFD_RELOC_KVX_S43_TLS_LE_EX6,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_tlsle_signed43_reloc,
    }
   },
   {
    .name = "tlsld",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 37,
     .reloc_type = S37_LO10_UP27,
     .reloc_lo10 = BFD_RELOC_KVX_S37_TLS_LD_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S37_TLS_LD_UP27,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_tlsld_signed37_reloc,
    }
   },
   {
    .name = "tlsld",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 43,
     .reloc_type = S43_LO10_UP27_EX6,
     .reloc_lo10 = BFD_RELOC_KVX_S43_TLS_LD_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S43_TLS_LD_UP27,
     .reloc_ex = BFD_RELOC_KVX_S43_TLS_LD_EX6,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_tlsld_signed43_reloc,
    }
   },
   {
    .name = "dtpoff",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 37,
     .reloc_type = S37_LO10_UP27,
     .reloc_lo10 = BFD_RELOC_KVX_S37_TLS_DTPOFF_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S37_TLS_DTPOFF_UP27,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_dtpoff_signed37_reloc,
    }
   },
   {
    .name = "dtpoff",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 43,
     .reloc_type = S43_LO10_UP27_EX6,
     .reloc_lo10 = BFD_RELOC_KVX_S43_TLS_DTPOFF_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S43_TLS_DTPOFF_UP27,
     .reloc_ex = BFD_RELOC_KVX_S43_TLS_DTPOFF_EX6,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_dtpoff_signed43_reloc,
    }
   },
   {
    .name = "tlsie",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 37,
     .reloc_type = S37_LO10_UP27,
     .reloc_lo10 = BFD_RELOC_KVX_S37_TLS_IE_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S37_TLS_IE_UP27,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_tlsie_signed37_reloc,
    }
   },
   {
    .name = "tlsie",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 43,
     .reloc_type = S43_LO10_UP27_EX6,
     .reloc_lo10 = BFD_RELOC_KVX_S43_TLS_IE_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S43_TLS_IE_UP27,
     .reloc_ex = BFD_RELOC_KVX_S43_TLS_IE_EX6,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_tlsie_signed43_reloc,
    }
   },
   {
    .name = "gotoff",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 43,
     .reloc_type = S43_LO10_UP27_EX6,
     .reloc_lo10 = BFD_RELOC_KVX_S43_GOTOFF_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S43_GOTOFF_UP27,
     .reloc_ex = BFD_RELOC_KVX_S43_GOTOFF_EX6,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_gotoff_signed43_reloc,
    }
   },
   {
    .name = "gotoff",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_64_ONLY,
     .bitsize = 64,
     .reloc_type = S64,
     .single = BFD_RELOC_KVX_64_GOTOFF,
     .kreloc = &kv3_gotoff_64_reloc,
    }
   },
   {
    .name = "got",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 43,
     .reloc_type = S43_LO10_UP27_EX6,
     .reloc_lo10 = BFD_RELOC_KVX_S43_GOT_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S43_GOT_UP27,
     .reloc_ex = BFD_RELOC_KVX_S43_GOT_EX6,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_got_signed43_reloc,
    }
   },
   {
    .name = "got",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_64_ONLY,
     .bitsize = 64,
     .reloc_type = S64,
     .single = BFD_RELOC_KVX_64_GOT,
     .kreloc = &kv3_got_64_reloc,
    }
   },
   {
    .name = "gotaddr",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_32_ONLY,
     .bitsize = 37,
     .has_no_arg = 1,
     .reloc_type = S37_LO10_UP27,
     .reloc_lo10 = BFD_RELOC_KVX_S37_GOTADDR_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S37_GOTADDR_UP27,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_gotaddr_signed37_reloc,
    }
   },
   {
    .name = "gotaddr",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_32_ONLY,
     .bitsize = 43,
     .has_no_arg = 1,
     .reloc_type = S43_LO10_UP27_EX6,
     .reloc_lo10 = BFD_RELOC_KVX_S43_GOTADDR_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S43_GOTADDR_UP27,
     .reloc_ex = BFD_RELOC_KVX_S43_GOTADDR_EX6,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_gotaddr_signed43_reloc,
    }
   },
   {
    .name = "gotaddr",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_64_ONLY,
     .bitsize = 64,
     .has_no_arg = 1,
     .reloc_type = S64_LO10_UP27_EX27,
     .reloc_lo10 = BFD_RELOC_KVX_S64_GOTADDR_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S64_GOTADDR_UP27,
     .reloc_ex = BFD_RELOC_KVX_S64_GOTADDR_EX27,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_gotaddr_signed64_reloc,
    }
   },
   

   // @pcrel()
   {
    // use pcrel16 to force the use of 16bits. This would normally not
    // be selected as symbol would not fit.
    .name = "pcrel16",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_ALL,
     .bitsize = 16,
     .single = BFD_RELOC_KVX_S16_PCREL,
     .reloc_type = S16,
     .kreloc = &kv3_pcrel_signed16_reloc,
    }
   },
   {
    .name = "pcrel",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_32_ONLY,
     .bitsize = 37,
     .reloc_type = S37_LO10_UP27,
     .reloc_lo10 = BFD_RELOC_KVX_S37_PCREL_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S37_PCREL_UP27,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_pcrel_signed37_reloc,
    }
   },
   {
    .name = "pcrel",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_32_ONLY,
     .bitsize = 43,
     .reloc_type = S43_LO10_UP27_EX6,
     .reloc_lo10 = BFD_RELOC_KVX_S43_PCREL_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S43_PCREL_UP27,
     .reloc_ex = BFD_RELOC_KVX_S43_PCREL_EX6,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_pcrel_signed43_reloc,
    }
   },
   {
    .name = "pcrel",
    .pseudo_relocs =
    {
     .avail_modes = PSEUDO_64_ONLY,
     .bitsize = 64,
     .reloc_type = S64_LO10_UP27_EX27,
     .reloc_lo10 = BFD_RELOC_KVX_S64_PCREL_LO10,
     .reloc_up27 = BFD_RELOC_KVX_S64_PCREL_UP27,
     .reloc_ex = BFD_RELOC_KVX_S64_PCREL_EX27,
     .single = BFD_RELOC_UNUSED,
     .kreloc = &kv3_pcrel_signed64_reloc,
    }
   },   
  };

/*****************************************************/
/*          Unwind information                       */
/*                                                   */
/*     unwind_proc_symbol -- frag address            */
/*                           following proc          */
/*     unwind_info_symbol -- frag address of         */
/*                           unwind info block       */
/*     unwind_bundle_count -- bundles seen since proc */
/*                                                   */
/*****************************************************/

#define UNWIND_HEADER_SECTION_NAME ".unwindh"
#define UNWIND_INFO_SECTION_NAME   ".unwindi"
#define UNWIND_VERSION             1

static int unwind_bundle_count = 0;

static int inside_bundle = 0;

/* Stores the labels inside bundles (typically debug labels) that need
   to be postponed to the next bundle. */
struct label_fix {
    struct label_fix *next;
    symbolS *sym;
} * label_fixes = 0;

/*****************************************************/
/*   OPTIONS PROCESSING                              */
/*****************************************************/

const char *md_shortopts = "hV";	/* catted to std short options */

/* added to std long options */

#define OPTION_HEXFILE	(OPTION_MD_BASE + 0)
#define OPTION_MARCH (OPTION_MD_BASE + 4)
#define OPTION_CHECK_RESOURCES (OPTION_MD_BASE + 5)
#define OPTION_NO_CHECK_RESOURCES (OPTION_MD_BASE + 6)
#define OPTION_GENERATE_ILLEGAL_CODE (OPTION_MD_BASE + 7)
#define OPTION_DUMP_TABLE (OPTION_MD_BASE + 8)
#define OPTION_PIC	(OPTION_MD_BASE + 9)
#define OPTION_BIGPIC	(OPTION_MD_BASE + 10)
#define OPTION_NOPIC    (OPTION_MD_BASE + 12)
#define OPTION_32 (OPTION_MD_BASE + 13)
#define OPTION_DUMPINSN (OPTION_MD_BASE + 15)
#define OPTION_ALL_SFR (OPTION_MD_BASE + 16)

struct option md_longopts[] =
{
     {"march", required_argument, NULL, OPTION_MARCH},
     {"check-resources", no_argument, NULL, OPTION_CHECK_RESOURCES},
     {"no-check-resources", no_argument, NULL, OPTION_NO_CHECK_RESOURCES},
     {"generate-illegal-code", no_argument, NULL, OPTION_GENERATE_ILLEGAL_CODE},
     {"dump-table", no_argument, NULL, OPTION_DUMP_TABLE},
     {"mpic", no_argument, NULL, OPTION_PIC},
     {"mPIC", no_argument, NULL, OPTION_BIGPIC},
     {"mnopic", no_argument,    NULL, OPTION_NOPIC},
     {"m32", no_argument,    NULL, OPTION_32},
     {"dump-insn", no_argument,    NULL, OPTION_DUMPINSN},
     {"all-sfr", no_argument, NULL, OPTION_ALL_SFR},
     {NULL, no_argument, NULL, 0}
};

size_t md_longopts_size = sizeof (md_longopts);

int md_parse_option(int c, const char *arg ATTRIBUTE_UNUSED) {
  int i;
  int find_core = 0;

  switch (c) {
  case 'h':
    md_show_usage(stdout);
    exit(EXIT_SUCCESS);
    break;

    /* -V: SVR4 argument to print version ID.  */
  case 'V':
    print_version_id();
    break;
  case OPTION_MARCH:
    march = strdup(arg);
    i = 0;
    while(i < KVXNUMCORES && ! find_core) {
      subcore_id = 0;
      while(kvx_core_info_table[i]->elf_cores[subcore_id] != -1 && ! find_core) {
        if (strcasecmp(march, kvx_core_info_table[i]->names[subcore_id]) == 0
            && kvx_core_info_table[i]->supported){

          kvx_core_info = kvx_core_info_table[i];
          kvx_registers = kvx_registers_table[i];
          kvx_regfiles = kvx_regfiles_table[i];
        
          find_core = 1;
        }
        else {
          subcore_id++;
        }
      }
      if(find_core) { break; }
      i++;
    }
    if (i == KVXNUMCORES){
      char buf[100];
      supported_cores(buf, sizeof(buf));
      as_fatal("Specified arch not supported [%s]", buf);
    }
    break;
  case OPTION_CHECK_RESOURCES:
    check_resource_usage = 1;
    break;
  case OPTION_NO_CHECK_RESOURCES:
    check_resource_usage = 0;
    break;
  case OPTION_GENERATE_ILLEGAL_CODE:
    generate_illegal_code = 1;
    break;
  case OPTION_DUMP_TABLE:
    dump_table = 1;
    break;
  case OPTION_DUMPINSN:
    dump_insn = 1;
    break;
  case OPTION_ALL_SFR:
    allow_all_sfr = 1;
    break;
  case OPTION_PIC:
  /* fallthrough, for now the same on KVX */
  case OPTION_BIGPIC:
    kvx_pic_flags |= ELF_KVX_ABI_PIC_BIT;
    break;
  case OPTION_NOPIC:
    kvx_pic_flags &= ~(ELF_KVX_ABI_PIC_BIT);
    break;
  case OPTION_32:
    kvx_arch_size = 32;
    break;

  default:
    return 0;
  }
  return 1;
}

void md_show_usage(FILE * stream){
    char buf[100];
    supported_cores(buf, sizeof(buf));

    fprintf(stream, "\nThe options -M, --mri and -f");
    fprintf(stream, " are not supported in this assembler.\n");
    fprintf(stream, "--check-resources \t perform minimal resource checking\n");
    fprintf(stream, "--march [%s] \t select architecture\n", buf);
    fprintf(stream, "-V \t\t\t print assembler version number\n");
}

/**************************************************/
/*              UTILITIES                         */
/**************************************************/

/*
 * Read a value from to the object file
 */

static valueT md_chars_to_number(char *buf, int n);
valueT md_chars_to_number(char *buf, int n){
    valueT val = 0;

    if (n > (int)sizeof (val) || n <= 0)
        abort();

    while (n--){
        val <<= 8;
        val |= (buf[n] & 0xff);
    }

    return val;
}

/* Returns the corresponding pseudo function matching SYM and to be
   used for data section */
static struct pseudo_func_s*
kvx_get_pseudo_func_data_scn(symbolS *sym) {
  int i;

  for (i = 0; i < NELEMS(pseudo_func); i++)
    if (sym == pseudo_func[i].sym
        && pseudo_func[i].pseudo_relocs.single != BFD_RELOC_UNUSED) {
      return &pseudo_func[i];
    }
  return NULL;
}

/* Returns the corresponding pseudo function matching SYM and operand
   format OPND */
static struct pseudo_func_s*
kvx_get_pseudo_func2(symbolS *sym, kvxbfield *opnd) {
  int i;

  for (i = 0; i < NELEMS(pseudo_func); i++)
    if (sym == pseudo_func[i].sym) {
      int relidx;
      for(relidx=0; relidx < opnd->reloc_nb; relidx++) {
	if(opnd->relocs[relidx] == pseudo_func[i].pseudo_relocs.kreloc
           && (kvx_arch_size == (int) pseudo_func[i].pseudo_relocs.avail_modes
	       || pseudo_func[i].pseudo_relocs.avail_modes == PSEUDO_ALL)) {
	  return &pseudo_func[i];
	}
      }
    }

  return NULL;
}

static void
supported_cores(char buf[], size_t buflen) {
  int i, j;
  buf[0] = '\0';
  for (i = 0; i < KVXNUMCORES; i++) {
    j = 0;
    while(kvx_core_info_table[i]->elf_cores[j] != -1) {
      if (kvx_core_info_table[i]->supported) {
        if (buf[0] == '\0') {
          strcpy(buf, kvx_core_info_table[i]->names[j]);
        }
        else {
          int l = strlen(buf);
          if ((l + 1 + strlen(kvx_core_info_table[i]->names[j]) + 1) < buflen) {
            strcat(buf, "|");
            strcat(buf, kvx_core_info_table[i]->names[j]);
          }
        }
      }
    j++;
    }
  }
}

__attribute__((unused))
int
get_regnum_by_name(char *name){
    int i;
    for(i=0; i < kvx_regfiles[KVX_REGFILE_REGISTERS]; i++){
        if(STREQ(kvx_registers[i].name, name)){
            return kvx_registers[i].id;
        }
    }
    return -1;
}

/***************************************************/
/*   ASSEMBLE AN INSTRUCTION                       */
/***************************************************/

/* Parse the arguments to an opcode. STR is a C string containing the
 * arguments. TOK is the output array of tokens, which are assembly
 * expressions. NTOK is the size of the array into which the tokens
 * will be placed. The return value is the number of tokens found if
 * no errors were encountered, otherwise -1 is returned.  */

static int
tokenize_arguments(char *str, expressionS tok[], char *tok_begins[], int ntok) {
    expressionS *end_tok = tok + ntok;
    char *old_input_line_pointer;
    int saw_comma = 0, saw_arg = 0;
    int tokcnt = 0;
    int debug = 0;
    
    memset(tok, 0, sizeof (*tok) * ntok);

    /* Save and restore input_line_pointer around this function */

    old_input_line_pointer = input_line_pointer;

    input_line_pointer = str;

    while ((tok < end_tok) && *input_line_pointer) {
        SKIP_WHITESPACE();
        if (tok_begins)
            tok_begins[tokcnt] = input_line_pointer;

        switch (*input_line_pointer) {
            case '\n':
            case '\0':
                goto fini;
            case ',':
            case '=':
            case '?':
                ++input_line_pointer;
                if (saw_comma || !saw_arg)
                    goto err;
                saw_comma = 1;
                break;
            case '[': {
              /* clarkes: for ldwl/stwl, we allow comma separator
               * before the [, so I have removed this test.
               * if (saw_comma)
               * goto err; */
              expression(tok);
              if (tok->X_op == O_register) {
                saw_comma = 0;
                saw_arg = 1;
                ++tok;
                ++tokcnt;
                break;
              }
              else {
                D(stderr, "tok type == %d\n", tok->X_op);
                as_warn("expected a register");
                break;
              }
            }
            default: {
              if (saw_arg && !saw_comma) {
                goto err;
              }
              expression(tok);
              if (tok->X_op == O_illegal || tok->X_op == O_absent) {
                goto err;
              }
              
              saw_comma = 0;
              saw_arg = 1;
              ++tok;
              ++tokcnt;
              break;
            }
        }
    }

    fini:
    if (saw_comma) {
      goto err;
    }
    if (tok_begins) {
      tok_begins[tokcnt] = input_line_pointer;
    }
    input_line_pointer = old_input_line_pointer;
    return ntok - (end_tok - tok);

    err:
    error_str = input_line_pointer;
    input_line_pointer = old_input_line_pointer;

    return -1;
}

static int
has_relocation_of_size(const kvxbfield *opnd) {
  int i;

  const int symbol_size = kvx_arch_size;

  /*
   * This is a bit hackish: in case of PCREL here, it means we are
   * trying to fit a symbol in the insn, not a pseudo function
   * (eg. @gotaddr, ...).
   * We don't want to use a GOTADDR (pcrel) in any insn that tries to fit a symbol.
   * One way to filter out these is to use the following asumption:
   * - Any insn that accepts a pcrel immediate has only one immediate variant.
   * Example:
   * - call accepts only a pcrel27 -> allow pcrel reloc here
   * - cb accepts only a pcrel17 -> allow pcrel reloc here
   * - addd accepts signed10,37,64 -> deny pcrel reloc here
   *
   * The motivation here is to prevent the function to allow a 64bits
   * symbol in a 37bits variant of any ALU insn (that would match with
   * the GOTADDR 37bits reloc switch case below)
   */
  for(i=0; i<opnd->reloc_nb; i++) {
    switch(opnd->relocs[i]->relative) {
      /* An absolute reloc needs a full size symbol reloc */
    case KVX_REL_ABS:
      if(opnd->relocs[i]->bitsize >= symbol_size) {
        return 1;
      }
      break;

      /* Most likely relative jumps. Let something else check size is
         OK. We don't currently have several relocations for such insns */
    case KVX_REL_PC:
      if (opnd->reloc_nb == 1)
	return 1;
      break;

      /* These relocations should be handled elsewhere with pseudo functions */
    case KVX_REL_GP:
    case KVX_REL_TP:
    case KVX_REL_GOT:
    case KVX_REL_BASE:
      break;
    }
  }
  return 0;
}

/*
 * Check input expressions against required operands
 */

static match_operands_code
match_operands(const kv3opc_t * op, const expressionS * tok,
        int ntok)
{
    int ii;
    int jj;
    int nop;
    kvxbfield *opdef;
    volatile long long min, max;
    volatile unsigned long long mask;

    /* First check that number of operands are the same. */
    for (nop = 0; op && op->format[nop]; nop++);
    if (ntok != nop) {
      return MATCH_NOT_FOUND;
    }

    /* Check enconding space */
    int encoding_space_flags = kvx_arch_size == 32 ? kvxOPCODE_FLAG_MODE32 : kvxOPCODE_FLAG_MODE64;

    for(ii=0; ii < op->wordcount; ii++) {
      if (! (op->codewords[ii].flags & encoding_space_flags))
        return MATCH_NOT_FOUND;
    }

#define IS_KVX_REGFILE_GPR(tok) ((((tok).X_add_number) >= kvx_regfiles[KVX_REGFILE_FIRST_GPR]) \
                                && (((tok).X_add_number) <= kvx_regfiles[KVX_REGFILE_LAST_GPR]))
#define IS_KVX_REGFILE_PGR(tok) ((((tok).X_add_number) >= kvx_regfiles[KVX_REGFILE_FIRST_PGR]) \
                                && (((tok).X_add_number) <= kvx_regfiles[KVX_REGFILE_LAST_PGR]))
#define IS_KVX_REGFILE_QGR(tok) ((((tok).X_add_number) >= kvx_regfiles[KVX_REGFILE_FIRST_QGR]) \
                                && (((tok).X_add_number) <= kvx_regfiles[KVX_REGFILE_LAST_QGR]))
#define IS_KVX_REGFILE_SFR(tok) ((((tok).X_add_number) >= kvx_regfiles[KVX_REGFILE_FIRST_SFR]) \
                                 && (((tok).X_add_number) <= kvx_regfiles[KVX_REGFILE_LAST_SFR]))
#define IS_KVX_REGFILE_XCR(tok) ((((tok).X_add_number) >= kvx_regfiles[KVX_REGFILE_FIRST_XCR]) \
                                && (((tok).X_add_number) <= kvx_regfiles[KVX_REGFILE_LAST_XCR]))
#define IS_KVX_REGFILE_XBR(tok) ((((tok).X_add_number) >= kvx_regfiles[KVX_REGFILE_FIRST_XBR]) \
                                && (((tok).X_add_number) <= kvx_regfiles[KVX_REGFILE_LAST_XBR]))
#define IS_KVX_REGFILE_XVR(tok) ((((tok).X_add_number) >= kvx_regfiles[KVX_REGFILE_FIRST_XVR]) \
                                && (((tok).X_add_number) <= kvx_regfiles[KVX_REGFILE_LAST_XVR]))
#define IS_KVX_REGFILE_XWR(tok) ((((tok).X_add_number) >= kvx_regfiles[KVX_REGFILE_FIRST_XWR]) \
                                && (((tok).X_add_number) <= kvx_regfiles[KVX_REGFILE_LAST_XWR]))
#define IS_KVX_REGFILE_XMR(tok) ((((tok).X_add_number) >= kvx_regfiles[KVX_REGFILE_FIRST_XMR]) \
                                && (((tok).X_add_number) <= kvx_regfiles[KVX_REGFILE_LAST_XMR]))

#define MATCH_KVX_REGFILE(tok,is_regfile)                                \
    if (((tok).X_op == O_register) && (is_regfile(tok))) {                \
      break;                                                                \
    }                                                                        \
    else {                                                                \
      return MATCH_NOT_FOUND;                                                \
    }

    /* Now check for compatiblility of each operand. */
    for (jj = 0; jj < ntok; jj++) {
        int operand_type = op->format[jj]->type;
        char *operand_type_name = op->format[jj]->tname;
        // int has_relocation = (op->format[jj]->reloc_nb > 0);
        int is_immediate = (op->format[jj]->reg_nb == 0);

        opdef = op->format[jj];
        int *valid_regs = op->format[jj]->regs;

        /* When operand is a register, check if it is valid. */
        if ((tok[jj].X_op == O_register) &&
            (valid_regs == NULL || !valid_regs[kvx_registers[tok[jj].X_add_number].id])) {
          return MATCH_NOT_FOUND;
        }

        if(is_immediate) {
          if(tok[jj].X_op == O_symbol) {
            if(! has_relocation_of_size(op->format[jj])) {
              return MATCH_NOT_FOUND;
            }
          }
          if (tok[jj].X_op == O_pseudo_fixup) {
            int i;
            for (i = 0; i < NELEMS(pseudo_func); i++){
              if (tok[jj].X_op_symbol == pseudo_func[i].sym){
                if (kvx_get_pseudo_func2(pseudo_func[i].sym, op->format[jj]) != NULL) {
                  break;
                }
              }
            }
            if (i == NELEMS(pseudo_func)) {
              return MATCH_NOT_FOUND;
            }
          }
        }

        switch (operand_type) {

            case RegClass_kv3_singleReg:
                MATCH_KVX_REGFILE(tok[jj],IS_KVX_REGFILE_GPR)
            case RegClass_kv3_pairedReg:
                MATCH_KVX_REGFILE(tok[jj],IS_KVX_REGFILE_PGR)
            case RegClass_kv3_quadReg:
                MATCH_KVX_REGFILE(tok[jj],IS_KVX_REGFILE_QGR)
            case RegClass_kv3_systemReg:
            case RegClass_v2_systemReg:
		 if( ! allow_all_sfr ) {
		      /* If option all-sfr not set, doest not match systemReg for SET/GET/WFX: used only for HW validation. */
		      return MATCH_NOT_FOUND;
		 }
                 /* fallthrough */
            case RegClass_kv3_aloneReg:
            case RegClass_v2_aloneReg:
            case RegClass_kv3_onlyraReg:
            case RegClass_kv3_onlyfxReg:
            case RegClass_v2_onlyfxReg:
            case RegClass_kv3_onlygetReg:
            case RegClass_v2_onlygetReg:
            case RegClass_kv3_onlysetReg:
            case RegClass_v2_onlysetReg:
	    case RegClass_kv3_onlyswapReg:
	    case RegClass_v2_onlyswapReg:
                MATCH_KVX_REGFILE(tok[jj],IS_KVX_REGFILE_SFR)
            case RegClass_kv3_coproReg:
            case RegClass_kv3_coproReg0M4:
            case RegClass_kv3_coproReg1M4:
            case RegClass_kv3_coproReg2M4:
            case RegClass_kv3_coproReg3M4:
                MATCH_KVX_REGFILE(tok[jj],IS_KVX_REGFILE_XCR)
            case RegClass_kv3_blockReg:
            case RegClass_kv3_blockRegE:
            case RegClass_kv3_blockRegO:
            case RegClass_kv3_blockReg0M4:
            case RegClass_kv3_blockReg1M4:
            case RegClass_kv3_blockReg2M4:
            case RegClass_kv3_blockReg3M4:
                MATCH_KVX_REGFILE(tok[jj],IS_KVX_REGFILE_XBR)
            case RegClass_kv3_vectorReg:
            case RegClass_kv3_vectorRegE:
            case RegClass_kv3_vectorRegO:
            case RegClass_kv3_wideReg_0:
            case RegClass_kv3_wideReg_1:
            case RegClass_kv3_matrixReg_0:
            case RegClass_kv3_matrixReg_1:
            case RegClass_kv3_matrixReg_2:
            case RegClass_kv3_matrixReg_3:
                MATCH_KVX_REGFILE(tok[jj],IS_KVX_REGFILE_XVR)
            case RegClass_kv3_wideReg:
                MATCH_KVX_REGFILE(tok[jj],IS_KVX_REGFILE_XWR)
            case RegClass_kv3_matrixReg:
                MATCH_KVX_REGFILE(tok[jj],IS_KVX_REGFILE_XMR)

            case Immediate_kv3_pcrel17:
            case Immediate_kv3_pcrel27:
            case Immediate_kv3_signed6:
            case Immediate_kv3_signed10:
            case Immediate_kv3_signed16:
            case Immediate_kv3_signed27:
            case Immediate_kv3_wrapped32:
            case Immediate_kv3_signed37:
            case Immediate_kv3_signed43:
            case Immediate_kv3_signed54:
            case Immediate_kv3_wrapped64:
            case Immediate_kv3_brknumber:
            case Immediate_kv3_sysnumber:
            case Immediate_kv3_unsigned6:
                if(tok[jj].X_op == O_symbol || tok[jj].X_op == O_pseudo_fixup){
                    break;
                }
                if (tok[jj].X_op == O_constant){
                    long long signed_value = tok[jj].X_add_number;
                    unsigned long long unsigned_value = tok[jj].X_add_number;
                    int match_signed = 0;
                    int match_unsigned = 0;

                    // Operand is not signed, but the token is.
                    if( !((opdef->flags & kvxSIGNED) || (opdef->flags & kvxWRAPPED)) && (tok[jj].X_unsigned == 0)){
                      return MATCH_NOT_FOUND;
                    }

                    // [JV] Special case of both signed and unsigned ranges are accepted because
                    // this immediate;
                    // - is zero extended on the same size of the instruction operation
                    // - sign bit is the highest bit
                    if(opdef->flags & kvxWRAPPED) {
                      signed long long high_mask = 0x8000000000000000LL;
                      int shift = (sizeof(signed long long) * 8) - opdef->width - 1;

                      high_mask = high_mask >> shift;

                      // If high bits set to zero, can perform sign extension.
                      if((signed_value & high_mask) == 0) {
                        signed_value = (signed_value << (64 - opdef->width)) >> (64 - opdef->width);
                      }
                    }

                    max = (1LL << (opdef->width - 1)) - 1;
                    min = (~0ULL << (opdef->width - 1));
                    mask = ~(~0ULL << opdef->width);
                    if(opdef->width == 64) {
                      mask = ~0ULL;
                    }

                    if(opdef->bias != 0) {
                      as_bad("[match_operands] : cannot use bias for encoding operand type %s \n", operand_type_name);
                    }

                    match_signed = (((signed_value >> opdef->shift) >= min) && ((signed_value >> opdef->shift) <= max));
                    match_unsigned = (((unsigned_value >> opdef->shift) & mask) == (unsigned_value >> opdef->shift));

                    if ( !                                                         /* Does not match signed and unsigned variantes */
                         ( ( !(opdef->flags & kvxSIGNED) && match_unsigned ) ||     /* Match unsigned and operand is unsigned or wrapped */           
                           ( ((opdef->flags & kvxSIGNED) || (opdef->flags & kvxWRAPPED))  && match_signed ) ) ) { /* Match signed and operand is signed or wrapped */           
                      return MATCH_NOT_FOUND;
                    }
                    break;
                }

                return MATCH_NOT_FOUND;
            default:
                as_bad("[match_operands] : couldn't find operand type %s \n", operand_type_name);
                return MATCH_NOT_FOUND;
        }
    }
    return MATCH_FOUND;

#undef IS_KVX_REGFILE_GPR
#undef IS_KVX_REGFILE_PGR
#undef IS_KVX_REGFILE_QGR
#undef IS_KVX_REGFILE_SFR
#undef IS_KVX_REGFILE_XCR
#undef IS_KVX_REGFILE_XBR
#undef IS_KVX_REGFILE_XVR
#undef IS_KVX_REGFILE_XWR
#undef IS_KVX_REGFILE_XMR
#undef MATCH_KVX_REGFILE
}

/*
 * Given an initial pointer into the opcode table, OPCODE,
 * find the format that matches the given set of operands. NTOK tells
 * the number of operands in the operand array pointed to by TOK.
 * If a matching format is found, a pointer to it is returned,
 * otherwise a null pointer is returned.
 */

static const kv3opc_t *
find_format(const kv3opc_t * opcode,
        const expressionS * tok,
        int ntok)
{
    char *name = opcode->as_op;
    const kv3opc_t *t = opcode;

    while (STREQ(name, t->as_op)){
        if (match_operands(t, tok, ntok) == MATCH_FOUND){
            return t;
        }
        t++;
    }

    return NULL;
}


/*
 * Insert ARG into the operand described by OPDEF in instruction INSN
 * Returns 1 if the immediate extension (IMMX) has been
 * handled along with relocation, 0 if not.
 */
static int
insert_operand(kvxinsn_t * insn,
               kvxbfield * opdef,
               const expressionS * arg)
{
    unsigned long long op = 0;
    kvx_bitfield_t *bfields = opdef->bfield;
    int bf_nb = opdef->bitfields;
    int bf_idx;
    int immx_ready = 0;

    if (opdef->width == 0)
        return 0;                        /* syntactic sugar ? */

    /* try to resolve the value */

    /* The cast is used to silence GCC about the abuse done with the enum.
     O_pseudo_fixup is not part of the enum, so enum checks raise an error.
    */
    switch ((int)arg->X_op)
      {
      case O_register:
        op = kvx_registers[arg->X_add_number].id;
        op -= opdef->bias;
        op >>= opdef->shift;
        break;
      case O_pseudo_fixup:
        if (insn->nfixups == 0)
          {
            expressionS reloc_arg;
            reloc_arg = *arg;
            reloc_arg.X_op = O_symbol;
            struct pseudo_func_s *pf = kvx_get_pseudo_func2(arg->X_op_symbol, opdef);

            /* S64 uses LO10/UP27/EX27 format (3 words), with one reloc in each words (3) */
            /* S43 uses LO10/EX6/UP27 format (2 words), with 2 relocs in main syllabes and 1 in extra word */
            /* S37 uses LO10/UP27 format (2 words), with one reloc in each word (2) */

            /* Beware that immxbuf must be filled in the same order as relocs should be emitted. */

            if (   pf->pseudo_relocs.reloc_type == S64_LO10_UP27_EX27
                || pf->pseudo_relocs.reloc_type == S43_LO10_UP27_EX6
                || pf->pseudo_relocs.reloc_type == S37_LO10_UP27) {
              insn->fixup[insn->nfixups].reloc = pf->pseudo_relocs.reloc_lo10;
              insn->fixup[insn->nfixups].exp = reloc_arg;
              insn->fixup[insn->nfixups].where = 0;
              insn->nfixups++;

              insn->immx0 = immxcnt;
              immxbuf[immxcnt].words[0] = 0;
              immxbuf[immxcnt].fixup[0].reloc = pf->pseudo_relocs.reloc_up27;
              immxbuf[immxcnt].fixup[0].exp = reloc_arg;
              immxbuf[immxcnt].fixup[0].where = 0;
              immxbuf[immxcnt].nfixups = 1;
              immxbuf[immxcnt].len = 1;

              insn->len -= 1;
              incr_immxcnt();
              immx_ready = 1;
            } else if (pf->pseudo_relocs.reloc_type == S16) {
              insn->fixup[insn->nfixups].reloc = pf->pseudo_relocs.single;
              insn->fixup[insn->nfixups].exp = reloc_arg;
              insn->fixup[insn->nfixups].where = 0;
              insn->nfixups++;

	    } else {
              as_fatal ("Unexpected fixup");
            }

            if (pf->pseudo_relocs.reloc_type == S64_LO10_UP27_EX27) {
              insn->immx1 = immxcnt;
              immxbuf[immxcnt].words[0] = 0;
              immxbuf[immxcnt].fixup[0].reloc = pf->pseudo_relocs.reloc_ex;
              immxbuf[immxcnt].fixup[0].exp = reloc_arg;
              immxbuf[immxcnt].fixup[0].where = 0;
              immxbuf[immxcnt].nfixups = 1;
              immxbuf[immxcnt].len = 1;

              insn->len -= 1;
              incr_immxcnt();
            } else if (pf->pseudo_relocs.reloc_type == S43_LO10_UP27_EX6) {
              insn->fixup[insn->nfixups].reloc = pf->pseudo_relocs.reloc_ex;
              insn->fixup[insn->nfixups].exp = reloc_arg;
              insn->fixup[insn->nfixups].where = 0;
              insn->nfixups++;
            }

          }
        else
          {
            as_fatal ("No room for fixup ");
          }
        break;
      case O_constant:                /* we had better generate a fixup if > max */
        if (!(arg->X_add_symbol))
          {
            if(opdef->flags & kvxSIGNED){
              op = ((signed long long)arg->X_add_number >> opdef->shift);
            }else{
              op = ((unsigned long long)arg->X_add_number >> opdef->shift);
            }
            break;
          }
        /* fallthrough */
      default:
        {
          if (insn->nfixups == 0)
            {
              switch (opdef->type)
                {
                case Immediate_kv3_pcrel17:
                  insn->fixup[0].reloc = BFD_RELOC_KVX_PCREL17;
                  insn->fixup[0].exp = *arg;
                  insn->fixup[0].where = 0;
                  insn->nfixups = 1;
                  break;

                case Immediate_kv3_pcrel27:
                  insn->fixup[0].reloc = BFD_RELOC_KVX_PCREL27;
                  insn->fixup[0].exp = *arg;
                  insn->fixup[0].where = 0;
                  insn->nfixups = 1;
                  break;

                case Immediate_kv3_wrapped32:
                  insn->fixup[0].reloc = BFD_RELOC_KVX_S32_LO5;
                  insn->fixup[0].exp = *arg;
                  insn->fixup[0].where = 0;
                  insn->nfixups = 1;

                  insn->immx0 = immxcnt;
                  immxbuf[immxcnt].words[0] = 0;
                  immxbuf[immxcnt].fixup[0].reloc = BFD_RELOC_KVX_S32_UP27;
                  immxbuf[immxcnt].fixup[0].exp = *arg;
                  immxbuf[immxcnt].fixup[0].where = 0;
                  immxbuf[immxcnt].nfixups = 1;
                  immxbuf[immxcnt].len = 1;

                  // decrement insn->len: immx part handled separately
                  // from insn and must not be emited twice
                  insn->len -= 1;
                  incr_immxcnt();
                  immx_ready = 1;
                  break;
                  
                case Immediate_kv3_signed10:
                  insn->fixup[0].reloc = BFD_RELOC_KVX_S37_LO10;
                  insn->fixup[0].exp = *arg;
                  insn->fixup[0].where = 0;
                  insn->nfixups = 1;
                  break;

                case Immediate_kv3_signed37:
                  insn->fixup[0].reloc = BFD_RELOC_KVX_S37_LO10;
                  insn->fixup[0].exp = *arg;
                  insn->fixup[0].where = 0;
                  insn->nfixups = 1;

                  insn->immx0 = immxcnt;
                  immxbuf[immxcnt].words[0] = 0;
                  immxbuf[immxcnt].fixup[0].reloc = BFD_RELOC_KVX_S37_UP27;
                  immxbuf[immxcnt].fixup[0].exp = *arg;
                  immxbuf[immxcnt].fixup[0].where = 0;
                  immxbuf[immxcnt].nfixups = 1;
                  immxbuf[immxcnt].len = 1;

                  // decrement insn->len: immx part handled separately
                  // from insn and must not be emited twice
                  insn->len -= 1;
                  incr_immxcnt();
                  immx_ready = 1;
                  break;
                  
                case Immediate_kv3_signed43:
                  insn->fixup[0].reloc = BFD_RELOC_KVX_S43_LO10;
                  insn->fixup[0].exp = *arg;
                  insn->fixup[0].where = 0;
                  insn->fixup[1].reloc = BFD_RELOC_KVX_S43_EX6;
                  insn->fixup[1].exp = *arg;
                  insn->fixup[1].where = 0;
                  insn->nfixups = 2;

                  insn->immx0 = immxcnt;
                  immxbuf[immxcnt].words[0] = insn->words[1];
                  immxbuf[immxcnt].fixup[0].reloc = BFD_RELOC_KVX_S43_UP27;
                  immxbuf[immxcnt].fixup[0].exp = *arg;
                  immxbuf[immxcnt].fixup[0].where = 0;
                  immxbuf[immxcnt].nfixups = 1;
                  immxbuf[immxcnt].len = 1;

                  // decrement insn->len: immx part handled separately
                  // from insn and must not be emited twice
                  incr_immxcnt();
                  insn->len -= 1;
                  immx_ready = 1;
                  break;

                case Immediate_kv3_wrapped64:
                  insn->fixup[0].reloc = BFD_RELOC_KVX_S64_LO10;
                  insn->fixup[0].exp = *arg;
                  insn->fixup[0].where = 0;

                  insn->nfixups = 1;

                  insn->immx0 = immxcnt;
                  immxbuf[immxcnt].words[0] = insn->words[1];
                  immxbuf[immxcnt].fixup[0].reloc = BFD_RELOC_KVX_S64_UP27;
                  immxbuf[immxcnt].fixup[0].exp = *arg;
                  immxbuf[immxcnt].fixup[0].where = 0;
                  immxbuf[immxcnt].nfixups = 1;
                  immxbuf[immxcnt].len = 1;

                  incr_immxcnt();
                  insn->len -= 1;

                  insn->immx1 = immxcnt;
                  immxbuf[immxcnt].words[0] = insn->words[2];
                  immxbuf[immxcnt].fixup[0].reloc = BFD_RELOC_KVX_S64_EX27;
                  immxbuf[immxcnt].fixup[0].exp = *arg;
                  immxbuf[immxcnt].fixup[0].where = 0;
                  immxbuf[immxcnt].nfixups = 1;
                  immxbuf[immxcnt].len = 1;

                  incr_immxcnt();
                  insn->len -= 1;
                  immx_ready = 1;
                  break;

                default:
                  as_fatal("don't know how to generate a fixup record");
                }
              return immx_ready;
            }
          else
            {
              as_fatal("No room for fixup ");
            }
        }
      }

    for(bf_idx=0;bf_idx < bf_nb; bf_idx++) {
      unsigned long long value = ((unsigned long long)op >> bfields[bf_idx].from_offset);
      int j = 0;
      int to_offset = bfields[bf_idx].to_offset;
      value &= (1LL << bfields[bf_idx].size) - 1;
      j = to_offset / 32;
      to_offset = to_offset % 32;
      insn->words[j] |= (value << to_offset) & 0xffffffff;
    }

    return immx_ready;
}

/*
 * Given a set of operands and a matching instruction,
 * assemble it
 *
 */
static void
assemble_insn(const kv3opc_t * opcode,
              const expressionS * tok,
              int ntok,
              kvxinsn_t * insn)
{
    int argidx;
    int i;
    unsigned immx_ready = 0;

    memset(insn, 0, sizeof (*insn));
    insn->opdef = opcode;
    for(i=0; i < opcode->wordcount; i++) {
        insn->words[i] = opcode->codewords[i].opcode;
        insn->len += 1;
    }
    insn->immx0 = NOIMMX;
    insn->immx1 = NOIMMX;
    for (argidx = 0; argidx < ntok; argidx++){
        int ret = insert_operand(insn, opcode->format[argidx], &tok[argidx]);
        immx_ready |= ret;
    }

    // Handle immx if insert_operand did not already take care of that
    if (!immx_ready){
      for(i=0; i < opcode->wordcount; i++){
        if(opcode->codewords[i].flags & kvxOPCODE_FLAG_IMMX0){
          insn->immx0 = immxcnt;
          immxbuf[immxcnt].words[0] = insn->words[i];
          immxbuf[immxcnt].nfixups = 0;
          immxbuf[immxcnt].len = 1;
          insn->len -= 1; 
          incr_immxcnt();
        }
        if(opcode->codewords[i].flags & kvxOPCODE_FLAG_IMMX1){
          insn->immx1 = immxcnt;
          immxbuf[immxcnt].words[0] = insn->words[i];
          immxbuf[immxcnt].nfixups = 0;
          immxbuf[immxcnt].len = 1;
          insn->len -= 1;
          incr_immxcnt();
        }
      }
    }
    return;
}


/* Emit an instruction from the instruction array into the object
 * file. INSN points to an element of the instruction array. STOPFLAG
 * is true if this is the last instruction in the bundle.
 *
 * Only handles main syllables of bundle. Immediate extensions are
 * handled by insert_operand.
 */
static void
emit_insn (kvxinsn_t *insn, int insn_pos, int stopflag)
{
  char *f;
  int i;
  unsigned int image;

  /* if we are listing, attach frag to previous line */
  if (listing)
    {
      listing_prev_line();
    }

  /* Update text size for lane parity checking */
  set_byte_counter(now_seg, (get_byte_counter(now_seg) + (insn->len * 4)) );

  /* allocate space in the fragment      */
  f = frag_more(insn->len * 4);

  /* spit out bits          */
  for (i = 0; i < insn->len; i++) {
    image = insn->words[i];

    /* Handle bundle parallel bit. */ ;
    if ((i == insn->len - 1) && stopflag){
      image &= ~PARALLEL_BIT;
    }else{
      image |= PARALLEL_BIT;
    }

    /* Emit the instruction image. */
    md_number_to_chars(f + (i * 4), image, 4);
  }

  /* generate fixup records */

  for (i = 0; i < insn->nfixups; i++) {
    int size, pcrel;
    reloc_howto_type *reloc_howto = bfd_reloc_type_lookup(stdoutput, insn->fixup[i].reloc);
    assert(reloc_howto);
    size = bfd_get_reloc_size(reloc_howto);
    pcrel = reloc_howto->pc_relative;

    /* In case the PCREL relocation is not for the first insn in the
       bundle, we have to offset it.  The pc used by the hardware
       references a bundle and not separate insn.
    */
    assert (!(insn_pos == -1 && pcrel));
    if (pcrel && insn_pos > 0)
      insn->fixup[i].exp.X_add_number += insn_pos * 4;

    fixS* fixup = fix_new_exp(frag_now,
			      f - frag_now->fr_literal + insn->fixup[i].where,
                              size,
			      &(insn->fixup[i].exp),
			      pcrel,
			      insn->fixup[i].reloc);
    /*
     * Set this bit so that large value can still be
     * handled. Without it, assembler will fail in fixup_segment
     * when it checks there is enough bits to store the value. As we
     * usually split our reloc across different words, it may think
     * that 4 bytes are not enough for large value. This simply
     * skips the tests
     */
    fixup->fx_no_overflow = 1;
  }
}


/* Determines if the expression is constant absolute */
int is_constant_expression(expressionS* exp)
{
    int retval=FALSE;

    if ( exp!=NULL )
    {
        if ( exp->X_op==O_constant )
        {
            retval=TRUE;
        }
        else if ( !(exp->X_add_symbol) )
        {
            retval=TRUE;
        }
        else if ( ! symbol_resolved_p(exp->X_add_symbol) )
        {
            retval=FALSE;
        }
        else
        {
            if ( exp->X_add_symbol )
            {
                if ( bfd_is_abs_section(S_GET_SEGMENT(exp->X_add_symbol)) )
                {
                    retval=TRUE;
                }
                /* FIXME (lc) rather bad to test if we need another symbol */
                if ( exp->X_op>=O_logical_not )
                {
                    if ( bfd_is_abs_section(S_GET_SEGMENT(exp->X_op_symbol)) )
                    {
                        retval=TRUE;
                    }
                    else
                    {
                        retval=FALSE;
                    }
                } /* if (exp->X_op_symbol) */
            } /* if ( exp->X_add_symbol && ...sy_resolved) */
        } /* else if ( !(exp->X_add_symbol) && !(exp->X_op_symbol) ) */
    }

    return retval;
}

/* Called for any expression that can not be recognized.  When the
 * function is called, `input_line_pointer' will point to the start of
 * the expression.  */

void
md_operand(expressionS *e)
{
  /* enum pseudo_type pseudo_type; */
  /* char *name = NULL; */
  size_t len;
  int ch, i;

  switch (*input_line_pointer)
    {
    case '@':
      /* Find what relocation pseudo-function we're dealing with. */
      /* pseudo_type = 0; */
      ch = *++input_line_pointer;
      for (i = 0; i < NELEMS (pseudo_func); ++i)
	if (pseudo_func[i].name && pseudo_func[i].name[0] == ch)
	  {
	    len = strlen (pseudo_func[i].name);
	    if (strncmp (pseudo_func[i].name + 1,
			 input_line_pointer + 1, len - 1) == 0
		&& !is_part_of_name (input_line_pointer[len]))
	      {
		input_line_pointer += len;
		break;
	      }
	  }
      SKIP_WHITESPACE ();
      if (*input_line_pointer != '(')
	{
	  as_bad ("Expected '('");
	  goto err;
	}
      /* Skip '('.  */
      ++input_line_pointer;
      if (!pseudo_func[i].pseudo_relocs.has_no_arg) {
	expression (e);
      }
      if (*input_line_pointer++ != ')')
	{
	  as_bad ("Missing ')'");
	  goto err;
	}
      if (!pseudo_func[i].pseudo_relocs.has_no_arg) {
	if (e->X_op != O_symbol)
          as_fatal ("Illegal combination of relocation functions");
      }
      /* Make sure gas doesn't get rid of local symbols that are used
	 in relocs.  */
      e->X_op = O_pseudo_fixup;
      e->X_op_symbol = pseudo_func[i].sym;
      break;
    default:
      break;
    }
  return;

 err:
  ignore_rest_of_line();
}

/*
 * Return the Bundling type for an insn.
 */
static Bundling find_bundling(const kvxinsn_t *insn)
{
    int insn_bundling = insn->opdef->bundling;
    return insn_bundling;
}

static int find_reservation(const kvxinsn_t *insn)
{
    int insn_reservation = insn->opdef->reservation;
    return insn_reservation;
}

/*
 * Given an opcode name and a pre-tokenized
 * set of arguments, take the
 * opcode all the way through emission
 */
static void
assemble_tokens(const char *opname,
                const expressionS * tok,
                int ntok) {
  const kv3opc_t *opcode;
  kvxinsn_t *insn;
  
  /* make sure there is room in instruction buffer */
  if (insncnt >= KVXMAXBUNDLEWORDS) { /* Was KVXMAXBUNDLEISSUE, changed because of NOPs */
    as_fatal("too many instructions in bundle ");
  }

  insn = insbuf + insncnt;

  /* find the instruction in the opcode table */
  opcode = (kv3opc_t *) hash_find(kvx_opcode_hash, opname);
  if (opcode) {
    if (!(opcode = find_format(opcode, tok, ntok))) {
      as_bad("[assemble_tokens] : couldn't find format %s \n", opname);
    }
    else {
      assemble_insn(opcode, tok, ntok, insn);
      insncnt++;
    }
  }
  else {
    as_bad("[assemble_tokens] : couldn't find op %s\n", opname);
  }
}


/*
 * Write in buf at most buf_size.
 * Returns the number of writen characters.
 */
static int
insn_syntax(kv3opc_t *op, char *buf, int buf_size)
{
  int chars = snprintf(buf, buf_size, "%s ",op->as_op);
  int i;
  char *fmtp = op->fmtstring;
  char ch = 0;

  for (i = 0; op->format[i]; i++) {
    int type  = op->format[i]->type;
    char *type_name  = op->format[i]->tname;
    int flags = op->format[i]->flags;
    int width = op->format[i]->width;

    /* Print characters in the format string up to the following * % or nul. */
    while((chars < buf_size) && (ch=*fmtp) && ch != '%') {
      buf[chars++] = ch;
      fmtp++;
    }

    /* Skip past %s */
    if(ch == '%') {
      ch=*fmtp++;
      fmtp++;
    }
    
    switch (type) {
    case RegClass_kv3_singleReg:
      chars += snprintf(&buf[chars], buf_size - chars, "grf");
      break;
    case RegClass_kv3_pairedReg:
      chars += snprintf(&buf[chars], buf_size - chars, "prf");
      break;
    case RegClass_kv3_systemReg:
    case RegClass_v2_systemReg:
    case RegClass_kv3_aloneReg:
    case RegClass_v2_aloneReg:
    case RegClass_kv3_onlyraReg:
    case RegClass_kv3_onlyfxReg:
    case RegClass_v2_onlyfxReg:
    case RegClass_kv3_onlygetReg:
    case RegClass_v2_onlygetReg:
    case RegClass_kv3_onlysetReg:
    case RegClass_v2_onlysetReg:
    case RegClass_kv3_onlyswapReg:
    case RegClass_v2_onlyswapReg:
      chars += snprintf(&buf[chars], buf_size - chars, "srf");
      break;
    case RegClass_kv3_coproReg:
    case RegClass_kv3_coproReg0M4:
    case RegClass_kv3_coproReg1M4:
    case RegClass_kv3_coproReg2M4:
    case RegClass_kv3_coproReg3M4:
      chars += snprintf(&buf[chars], buf_size - chars, "crf");
      break;
    case RegClass_kv3_blockReg:
    case RegClass_kv3_blockRegE:
    case RegClass_kv3_blockRegO:
    case RegClass_kv3_blockReg0M4:
    case RegClass_kv3_blockReg1M4:
    case RegClass_kv3_blockReg2M4:
    case RegClass_kv3_blockReg3M4:
      chars += snprintf(&buf[chars], buf_size - chars, "brf");
      break;
    case RegClass_kv3_vectorReg:
    case RegClass_kv3_vectorRegE:
    case RegClass_kv3_vectorRegO:
    case RegClass_kv3_wideReg_0:
    case RegClass_kv3_wideReg_1:
    case RegClass_kv3_matrixReg_0:
    case RegClass_kv3_matrixReg_1:
    case RegClass_kv3_matrixReg_2:
    case RegClass_kv3_matrixReg_3:
      chars += snprintf(&buf[chars], buf_size - chars, "arf");
      break;
    case Immediate_kv3_pcrel17:
    case Immediate_kv3_pcrel27:
    case Immediate_kv3_signed6:
    case Immediate_kv3_signed10:
    case Immediate_kv3_signed16:
    case Immediate_kv3_signed27:
    case Immediate_kv3_wrapped32:
    case Immediate_kv3_signed37:
    case Immediate_kv3_signed43:
    case Immediate_kv3_wrapped64:
    case Immediate_kv3_brknumber:
    case Immediate_kv3_sysnumber:
    case Immediate_kv3_unsigned6:
      if(flags & kvxSIGNED){
        chars += snprintf(&buf[chars], buf_size - chars, "s%d",width);
      }
      else {
        chars += snprintf(&buf[chars], buf_size - chars, "u%d",width);
      }
      break;
    default:
      fprintf(stderr, "error: unexpected operand type (%s)\n", type_name);
      exit(-1);
    }
  }

  /* Print trailing characters in the format string, if any */
  while((chars < buf_size) && (ch=*fmtp)) {
    buf[chars++] = ch;
    fmtp++;
  }

  if(chars < buf_size) {
    buf[chars++] = '\0';
  }
  else {
    buf[buf_size-1] = '\0';
  }

  return chars;
}

#define ASM_CHARS_MAX (48)

static void
kv3_print_insn(kv3opc_t *op) {
  char asm_str[ASM_CHARS_MAX];
  int chars = insn_syntax(op, asm_str, ASM_CHARS_MAX);
  int i;
  const char *insn_type = "UNKNOWN";
  const char *insn_mode = "";

  for(i=chars-1; i<ASM_CHARS_MAX-1; i++)
    asm_str[i] = '-';

  switch(op->bundling) {
  case Bundling_kv3_ALL:
    insn_type="ALL            ";
    break;
  case Bundling_kv3_BCU:
    insn_type="BCU            ";
    break;
  case Bundling_kv3_TCA:
    insn_type="TCA            ";
    break;
  case Bundling_kv3_FULL:
  case Bundling_kv3_FULL_X:
  case Bundling_kv3_FULL_Y:
    insn_type="FULL           ";
    break;
  case Bundling_kv3_LITE:
  case Bundling_kv3_LITE_X:
  case Bundling_kv3_LITE_Y:
    insn_type="LITE           ";
    break;
  case Bundling_kv3_TINY:
  case Bundling_kv3_TINY_X:
  case Bundling_kv3_TINY_Y:
    insn_type="LITE           ";
    break;
  case Bundling_kv3_MAU:
  case Bundling_kv3_MAU_X:
  case Bundling_kv3_MAU_Y:
    insn_type="MAU            ";
    break;
  case Bundling_kv3_LSU:
  case Bundling_kv3_LSU_X:
  case Bundling_kv3_LSU_Y:
    insn_type="LSU            ";
    break;
  case Bundling_kv3_NOP:
    insn_type="NOP            ";
    break;
  default:
    as_fatal("Unhandled Bundling class %d\n", op->bundling);
  }

  if (op->codewords[0].flags & kvxOPCODE_FLAG_MODE64
      && op->codewords[0].flags & kvxOPCODE_FLAG_MODE32)
    insn_mode = "32|64";
  else if(op->codewords[0].flags & kvxOPCODE_FLAG_MODE64)
    insn_mode = "64";
  else if(op->codewords[0].flags & kvxOPCODE_FLAG_MODE32)
    insn_mode = "32";
  else
    as_fatal("Unknown instruction mode.\n");

  printf("%s | syllables: %d | type: %s | mode: %s bits\n", asm_str, op->wordcount, insn_type, insn_mode);
}

static int
kvxinsn_compare(const void *a, const void *b)
{
  kvxinsn_t *kvxinsn_a = *(kvxinsn_t **)a;
  kvxinsn_t *kvxinsn_b = *(kvxinsn_t **)b;
  int bundling_a = find_bundling(kvxinsn_a);
  int bundling_b = find_bundling(kvxinsn_b);
  int order_a = kvxinsn_a->order;
  int order_b = kvxinsn_b->order;
  if (bundling_a != bundling_b)
    return (bundling_b < bundling_a) - (bundling_a < bundling_b);
  return (order_b < order_a) - (order_a < order_b);
}

static void
kv3_reorder_bundle(kvxinsn_t *bundle_insn[], int bundle_insncnt)
{
  enum { EXU_BCU, EXU_TCA, EXU_ALU0, EXU_ALU1, EXU_MAU, EXU_LSU, EXU__ };
  kvxinsn_t *issued[EXU__];
  int i, tag, exu;
  
  /* Sort the bundle_insn in order of bundling. */
  qsort(bundle_insn, bundle_insncnt, sizeof(kvxinsn_t *), kvxinsn_compare);
  
  memset(issued, 0, sizeof(issued));
  for (i = 0; i < bundle_insncnt; i++) {
    kvxinsn_t *kvxinsn = bundle_insn[i];
    tag = -1, exu = -1;
    switch (find_bundling(kvxinsn)) {
    case Bundling_kv3_ALL:
      if (bundle_insncnt > 1)
        as_fatal("Too many ops in a single op bundle\n");
      issued[0] = kvxinsn;
      break;
    case Bundling_kv3_BCU:
      if (!issued[EXU_BCU]) {
        issued[EXU_BCU] = kvxinsn;
      } else
        as_fatal("More than one BCU instruction in bundle\n");
      break;
    case Bundling_kv3_TCA:
      if (!issued[EXU_TCA]) {
        issued[EXU_TCA] = kvxinsn;
      } else
        as_fatal("More than one TCA instruction in bundle\n");
      break;
    case Bundling_kv3_FULL:
    case Bundling_kv3_FULL_X:
    case Bundling_kv3_FULL_Y:
      if (!issued[EXU_ALU0]) {
        issued[EXU_ALU0] = kvxinsn;
        tag = Modifier_kv3_exunum_ALU0;
        exu = EXU_ALU0;
      } else
        as_fatal("More than one ALU FULL instruction in bundle\n");
      break;
    case Bundling_kv3_LITE:
    case Bundling_kv3_LITE_X:
    case Bundling_kv3_LITE_Y:
      if (!issued[EXU_ALU0]) {
        issued[EXU_ALU0] = kvxinsn;
        tag = Modifier_kv3_exunum_ALU0;
        exu = EXU_ALU0;
      } else
      if (!issued[EXU_ALU1]) {
        issued[EXU_ALU1] = kvxinsn;
        tag = Modifier_kv3_exunum_ALU1;
        exu = EXU_ALU1;
      } else
        as_fatal("Too many ALU FULL or LITE instructions in bundle\n");
      break;
    case Bundling_kv3_MAU:
    case Bundling_kv3_MAU_X:
    case Bundling_kv3_MAU_Y:
      if (!issued[EXU_MAU]) {
        issued[EXU_MAU] = kvxinsn;
        tag = Modifier_kv3_exunum_MAU;
        exu = EXU_MAU;
      } else
        as_fatal("More than one MAU instruction in bundle\n");
      break;
    case Bundling_kv3_LSU:
    case Bundling_kv3_LSU_X:
    case Bundling_kv3_LSU_Y:
      if (!issued[EXU_LSU]) {
        issued[EXU_LSU] = kvxinsn;
        tag = Modifier_kv3_exunum_LSU;
        exu = EXU_LSU;
      } else
        as_fatal("More than one LSU instruction in bundle\n");
      break;
    case Bundling_kv3_TINY:
    case Bundling_kv3_TINY_X:
    case Bundling_kv3_TINY_Y:
    case Bundling_kv3_NOP:
      if (!issued[EXU_ALU0]) {
        issued[EXU_ALU0] = kvxinsn;
        tag = Modifier_kv3_exunum_ALU0;
        exu = EXU_ALU0;
      } else
      if (!issued[EXU_ALU1]) {
        issued[EXU_ALU1] = kvxinsn;
        tag = Modifier_kv3_exunum_ALU1;
        exu = EXU_ALU1;
      } else
      if (!issued[EXU_MAU]) {
        issued[EXU_MAU] = kvxinsn;
        tag = Modifier_kv3_exunum_MAU;
        exu = EXU_MAU;
      } else
      if (!issued[EXU_LSU]) {
        issued[EXU_LSU] = kvxinsn;
        tag = Modifier_kv3_exunum_LSU;
        exu = EXU_LSU;
      } else
        as_fatal("Too many ALU instructions in bundle\n");
      break;
    default:
      as_fatal("Unhandled Bundling class %d\n", find_bundling(kvxinsn));
    }
    if (tag >= 0) {
      if (issued[exu]->immx0 != NOIMMX) {
        immxbuf[issued[exu]->immx0].words[0] |= (tag << 27);
      }
      if (issued[exu]->immx1 != NOIMMX) {
        immxbuf[issued[exu]->immx1].words[0] |= (tag << 27);
      }
    }
  }
  
  for (i = 0, exu = 0; exu < EXU__; exu++) {
    if (issued[exu]) {
      bundle_insn[i++] = issued[exu];
    }
  }
  if (i != bundle_insncnt)
    as_fatal("Mismatch between bundle and issued instructions\n");
}

/*
 * Called by core to assemble a single line
 */
void
md_assemble(char *s)
{
    char *t;
    int i, tag,tlen;
    char opname[32];
    expressionS tok[KVXMAXOPERANDS];
    char *tok_begins[2*KVXMAXOPERANDS];
    int ntok;

    if (get_byte_counter(now_seg) & 3)
        as_fatal("code segment not word aligned in md_assemble\n");

    t = s;

    while (t && t[0] && (t[0] == ' '))
        t++;

    /* ;; was converted to "be" by line hook          */
    /* here we look for the bundle end                */
    /* and actually output any instructions in bundle */
    /* also we need to implement the stop bit         */
    /* check for bundle end */
    if (strncmp(t, "be", 2) == 0) {
        int j;
        inside_bundle = 0;
        unwind_bundle_count++;        /* count of bundles in current proc */
        //int sec_align = bfd_get_section_alignment(stdoutput, now_seg);

        {
            kvxinsn_t *bundle_insn[KVXMAXBUNDLEWORDS]; /* Was KVXMAXBUNDLEISSUE, changed because of NOPs */
            int bundle_insncnt = 0;
            int syllables = 0;
            int entry;

            /* retain bundle start adress for error messages */
            //            start_bundle = get_byte_counter(now_seg);

#ifdef OBJ_ELF
            /* Emit Dwarf debug line information */
            dwarf2_emit_insn(0);
#endif
            for (j = 0; j < insncnt; j++) {
                insbuf[j].order = j;
                bundle_insn[bundle_insncnt++] = &insbuf[j];
                syllables += insbuf[j].len;
            }

            if(syllables + immxcnt > KVXMAXBUNDLEWORDS){
                as_fatal("Bundle has too many syllables : %d instead of %d\n", syllables + immxcnt, KVXMAXBUNDLEWORDS);
            }

            /* Check that resources are not oversubscribed.
             * We check only for a single bundle, so resources that are used
             * in multiple cycles will not be fully checked. */

            if (check_resource_usage) {
              const int reservation_table_len = (kv3_reservation_table_lines * kv3_resource_max);
              const int *resources = kvx_core_info->resources;
              int *resources_used = malloc(reservation_table_len * sizeof(int));
              memset(resources_used, 0, reservation_table_len * sizeof(int));

              for (i = 0; i < bundle_insncnt; i++) {
                int insn_reservation = find_reservation(bundle_insn[i]);
                int reservation = insn_reservation & 0xff;
                const int *reservation_table = kv3_reservation_table_table[reservation];
                for (j = 0; j < reservation_table_len; j++)
                  resources_used[j] += reservation_table[j];
              }

              for (i = 0; i < kv3_reservation_table_lines; i++) {
                for (j = 0; j < kv3_resource_max; j++)
                  if (resources_used[(i * kv3_resource_max) + j] > resources[j]) {
                    int v = resources_used[(i * kv3_resource_max) + j];
                    free (resources_used);
                    as_fatal("Resource %s over-used in bundle: %d used, %d available",
                            kv3_resource_names[j], v, resources[j]);
                  }
              }
              free (resources_used);
            }

            if(!generate_illegal_code){
                // reorder and check the bundle
                kv3_reorder_bundle(bundle_insn, bundle_insncnt);
            }

            /* The ordering of the insns has been set correctly in bundle_insn. */
            for (entry = 0; entry < bundle_insncnt; entry++) {
		emit_insn (bundle_insn[entry], entry,
			   (entry == (bundle_insncnt + immxcnt - 1)));
		bundle_insn[entry]->written = 1;
            }

            // Emit immx, ordering them by EXU tags, 0 to 3
            entry = 0;
            for(tag=0; tag < 4; tag++){
                for (j = 0; j < immxcnt; j++) {
                      if(kv3_exunum2_fld(immxbuf[j].words[0]) == tag){
                            assert(immxbuf[j].written == 0);
			    int insn_pos = bundle_insncnt + entry;
			    emit_insn (&(immxbuf[j]), insn_pos,
				       (entry == (immxcnt - 1)));
			    immxbuf[j].written = 1;
                            entry++;
                      }
                }
            }
            if (entry != immxcnt){
                as_fatal("%d IMMX produced, only %d emitted.", immxcnt, entry);
            }
        }

        {
            /* The debug label that appear in the middle of bundles
               had better appear to be attached to the next
               bundle. This is because usually these labels point to
               the first instruction where some condition is met. If
               the label isn't handled this way it will be attached to
               the current bundle which is wrong as the conresponding
               instruction isn't executed yet. */
            while (label_fixes) {
                struct label_fix *fix = label_fixes;

                label_fixes = fix->next;
                symbol_set_value_now (fix->sym);
                free (fix);
            }
        }

        insncnt = 0;
        immxcnt = 0;
        memset(immxbuf, 0, sizeof(immxbuf));

        return;
    }

    /* get opcode info    */
    while (t && t[0] && (t[0] == ' '))
        t++;
    i = strspn(t, "abcdefghijklmnopqrstuvwxyz._0123456789@");
    tlen = (i < 31) ? i : 31;
    memcpy(opname, t, tlen);
    opname[tlen] = '\0';

    t += i;

    /* parse arguments             */
    if ((ntok = tokenize_arguments(t, tok, tok_begins, KVXMAXOPERANDS)) < 0) {
      if(error_str != NULL) {
          as_bad("syntax error at: %s", error_str);
          error_str = NULL;
      }
      else {
          as_bad("syntax error");
      }
    }

    inside_bundle = 1;
    /* build an instruction record */
    assemble_tokens(opname, tok, ntok);
}

static void
kvx_set_cpu(void) {
  if (!kvx_core_info)
    kvx_core_info = &kvx_kv3_v1_core_info;

  if(!kvx_registers)
    kvx_registers = kvx_kv3_v1_registers;

  if(!kvx_regfiles)
    kvx_regfiles = kvx_kv3_v1_regfiles;

  int kvx_bfd_mach;
  print_insn = kv3_print_insn;

  switch(kvx_core_info->elf_cores[subcore_id])
    {
    case ELF_KVX_CORE_KV3_1:
      kvx_bfd_mach = kvx_arch_size == 32 ? bfd_mach_kv3_1 : bfd_mach_kv3_1_64;
      break;
    case ELF_KVX_CORE_KV3_2:
      kvx_bfd_mach = kvx_arch_size == 32 ? bfd_mach_kv3_2 : bfd_mach_kv3_2_64;
      break;
    default:
      as_fatal("Unknown elf core: 0x%x\n",kvx_core_info->elf_cores[subcore_id]);
    }

  if (!bfd_set_arch_mach(stdoutput, TARGET_ARCH, kvx_bfd_mach))
    as_warn(_("could not set architecture and machine"));
}

static int kvxop_compar(const void *a, const void *b)
{
    const kv3opc_t *opa = (const kv3opc_t *)a;
    const kv3opc_t *opb = (const kv3opc_t *)b;
    return strcmp(opa->as_op, opb->as_op);
}

/***************************************************/
/*    INITIALIZE ASSEMBLER                          */
/***************************************************/


static void
print_hash(const char *key,  __attribute__((unused)) PTR val){
  printf("%s\n", key);
}
 

void
md_begin()
{
    int i;
    kvx_set_cpu();


    /*
     * alias register names with symbols
     */
    for(i = 0; i < kvx_regfiles[KVX_REGFILE_REGISTERS]; i++) {
      symbol_table_insert(symbol_create(kvx_registers[i].name, reg_section, i, &zero_address_frag));
    }

    /* Sort optab, so that identical mnemonics appear consecutively */
    {
        int nel;
        for (nel = 0; !STREQ("", kvx_core_info->optab[nel].as_op); nel++) ;
        qsort(kvx_core_info->optab, nel, sizeof(kvx_core_info->optab[0]), kvxop_compar);
    }

    /* The '?' is an operand separator */
    lex_type['?'] = 0;

    /* Create the opcode hash table      */
    /* Each name should appear only once */

    kvx_opcode_hash = hash_new();
    {
        kv3opc_t *op;
        const char *name = 0;
        const char *retval = 0;
        for (op = kvx_core_info->optab; !(STREQ("", op->as_op)) ; op++) {
            /* enter in hash table if this is a new name */

            if (!(STREQ(name, op->as_op))) {
                name = op->as_op;
                retval = hash_insert(kvx_opcode_hash, name, (PTR) op);
                if (retval)
                    as_fatal("internal error: can't hash opcode `%s': %s",
                            name, retval);
            }
        }
    }

    if(dump_table) {
      hash_traverse(kvx_opcode_hash, print_hash);
      exit(0);
    }

    if(dump_insn) {
      kv3opc_t *op;
      for (op = kvx_core_info->optab; !(STREQ("", op->as_op)) ; op++) {
        print_insn(op);
      }
      exit(0);
    }

    /* Here we enforce the minimum section alignment.  Remember, in
     * the linker we can make the boudaries between the linked sections
     * on larger boundaries.  The text segment is aligned to long words
     * because of the odd/even constraint on immediate extensions
     */

    bfd_set_section_alignment(text_section, 3);        /* -- 8 bytes */
    bfd_set_section_alignment(data_section, 2);        /* -- 4 bytes */
    bfd_set_section_alignment(bss_section, 2);        /* -- 4 bytes */
    subseg_set(text_section, 0);

    symbolS *gotoff_sym = symbol_create (".<gotoff>", undefined_section, 0,
                                         &zero_address_frag);
    symbolS *got_sym = symbol_create (".<got>", undefined_section, 0,
                                      &zero_address_frag);
    symbolS *plt_sym = symbol_create (".<plt>", undefined_section, 0,
                                      &zero_address_frag);
    /* symbolS *tprel_sym = symbol_create (".<tprel>", undefined_section, 0, */
    /*                                     &zero_address_frag); */
    symbolS *tlsgd_sym = symbol_create (".<tlsgd>", undefined_section, 0,
                                        &zero_address_frag);
    symbolS *tlsie_sym = symbol_create (".<tlsie>", undefined_section, 0,
                                        &zero_address_frag);
    symbolS *tlsle_sym = symbol_create (".<tlsle>", undefined_section, 0,
                                        &zero_address_frag);
    symbolS *tlsld_sym = symbol_create (".<tlsld>", undefined_section, 0,
                                        &zero_address_frag);
    symbolS *dtpoff_sym = symbol_create (".<dtpoff>", undefined_section, 0,
                                        &zero_address_frag);
    symbolS *plt64_sym = symbol_create (".<plt64>", undefined_section, 0,
					&zero_address_frag);
    symbolS *gotaddr_sym = symbol_create (".<gotaddr>", undefined_section, 0,
					  &zero_address_frag);
    symbolS *pcrel16_sym = symbol_create (".<pcrel16>", undefined_section, 0,
					  &zero_address_frag);
    symbolS *pcrel_sym = symbol_create (".<pcrel>", undefined_section, 0,
					  &zero_address_frag);


    for (i = 0; i < NELEMS (pseudo_func); ++i) {
      symbolS *sym;
      if (!strcmp(pseudo_func[i].name, "gotoff")) {
        sym = gotoff_sym;
      } else if (!strcmp(pseudo_func[i].name, "got")) {
        sym = got_sym;
      } else if (!strcmp(pseudo_func[i].name, "plt")) {
        sym = plt_sym;
      } else if (!strcmp(pseudo_func[i].name, "tlsgd")) {
        sym = tlsgd_sym;
      } else if (!strcmp(pseudo_func[i].name, "tlsle")) {
        sym = tlsle_sym;
      } else if (!strcmp(pseudo_func[i].name, "tlsld")) {
        sym = tlsld_sym;
      } else if (!strcmp(pseudo_func[i].name, "dtpoff")) {
        sym = dtpoff_sym;
      } else if (!strcmp(pseudo_func[i].name, "tlsie")) {
        sym = tlsie_sym;
      } else if (!strcmp(pseudo_func[i].name, "plt64")) {
	sym = plt64_sym;
      } else if (!strcmp(pseudo_func[i].name, "pcrel16")) {
	sym = pcrel16_sym;
      } else if (!strcmp(pseudo_func[i].name, "pcrel")) {
	sym = pcrel_sym;
      } else if (!strcmp(pseudo_func[i].name, "gotaddr")) {
	sym = gotaddr_sym;
      } else {
	as_fatal("internal error: Unknown pseudo func `%s'",
		 pseudo_func[i].name);
      }

      pseudo_func[i].sym = sym;
    }

    /* pseudo_func[FUNC_GOTOFF_RELATIVE].u.sym = */
    /*   symbol_create (".<gotoff>", undefined_section, FUNC_GOTOFF_RELATIVE, */
    /*                      &zero_address_frag); */
    /* pseudo_func[FUNC_GOTOFF64_RELATIVE].u.sym = */
    /*   symbol_create (".<gotoff64>", undefined_section, FUNC_GOTOFF64_RELATIVE, */
    /*                      &zero_address_frag); */
    /* pseudo_func[FUNC_GOT_RELATIVE].u.sym = */
    /*   symbol_create (".<got>", undefined_section, FUNC_GOT_RELATIVE, */
    /*                      &zero_address_frag); */
    /* pseudo_func[FUNC_PLT_RELATIVE].u.sym = */
    /*   symbol_create (".<plt>", undefined_section, FUNC_PLT_RELATIVE, */
    /*                      &zero_address_frag); */
    /* pseudo_func[FUNC_TP_RELATIVE].u.sym = */
    /*   symbol_create (".<tprel>", undefined_section, FUNC_TP_RELATIVE, */
    /*                      &zero_address_frag); */
    /* pseudo_func[FUNC_TP64_RELATIVE].u.sym = */
    /*   symbol_create (".<tprel64>", undefined_section, FUNC_TP64_RELATIVE, */
    /*                      &zero_address_frag); */
    /*  bfd_set_private_flags(stdoutput, 0); *//* default flags */
}

/***************************************************/
/*          ASSEMBLER CLEANUP STUFF                */
/***************************************************/

#if 0
static void
md_after_pass(void)		/* called from md_end */
{
}
#endif

/***************************************************/
/*          ASSEMBLER FIXUP STUFF                  */
/***************************************************/

void
md_apply_fix(fixS * fixP, valueT * valueP,
        segT segmentP ATTRIBUTE_UNUSED)
{
  char *const fixpos = fixP->fx_frag->fr_literal + fixP->fx_where;
  valueT value = *valueP;
  valueT image;
  arelent *rel;

  rel = (arelent *)xmalloc(sizeof(arelent));

  rel->howto = bfd_reloc_type_lookup(stdoutput, fixP->fx_r_type);
  if(rel->howto == NULL){
    as_fatal("[md_apply_fix] unsupported relocation type (can't find howto)");
  }

  /* Note whether this will delete the relocation.  */
  if (fixP->fx_addsy == NULL && fixP->fx_pcrel == 0)
    fixP->fx_done = 1;

  if (fixP->fx_size > 0)
    image = md_chars_to_number(fixpos, fixP->fx_size);
  else
    image = 0;
  if (fixP->fx_addsy != NULL)
    {
      switch (fixP->fx_r_type)
        {
        case BFD_RELOC_KVX_S37_TLS_LE_UP27:
        case BFD_RELOC_KVX_S37_TLS_LE_LO10:

        case BFD_RELOC_KVX_S43_TLS_LE_EX6 :
        case BFD_RELOC_KVX_S43_TLS_LE_UP27 :
        case BFD_RELOC_KVX_S43_TLS_LE_LO10:

	case BFD_RELOC_KVX_S37_TLS_GD_LO10:
	case BFD_RELOC_KVX_S37_TLS_GD_UP27:

	case BFD_RELOC_KVX_S43_TLS_GD_LO10:
	case BFD_RELOC_KVX_S43_TLS_GD_UP27:
	case BFD_RELOC_KVX_S43_TLS_GD_EX6:

	case BFD_RELOC_KVX_S37_TLS_IE_LO10:
	case BFD_RELOC_KVX_S37_TLS_IE_UP27:

	case BFD_RELOC_KVX_S43_TLS_IE_LO10:
	case BFD_RELOC_KVX_S43_TLS_IE_UP27:
	case BFD_RELOC_KVX_S43_TLS_IE_EX6:

	case BFD_RELOC_KVX_S37_TLS_LD_LO10:
	case BFD_RELOC_KVX_S37_TLS_LD_UP27:

	case BFD_RELOC_KVX_S43_TLS_LD_LO10:
	case BFD_RELOC_KVX_S43_TLS_LD_UP27:
	case BFD_RELOC_KVX_S43_TLS_LD_EX6:

          S_SET_THREAD_LOCAL (fixP->fx_addsy);
          break;
        default:
          break;
        }
    }

  /* If relocation has been marked for deletion, apply remaining changes */
  if (fixP->fx_done) {
    switch (fixP->fx_r_type)
      {
      case BFD_RELOC_16:
      case BFD_RELOC_32:
      case BFD_RELOC_64:

      case BFD_RELOC_KVX_GLOB_DAT:
      case BFD_RELOC_KVX_32_GOT:
      case BFD_RELOC_KVX_64_GOT:
      case BFD_RELOC_KVX_64_GOTOFF:
      case BFD_RELOC_KVX_32_GOTOFF:
        image = value;
        md_number_to_chars(fixpos, image, fixP->fx_size);
        break;

      case BFD_RELOC_KVX_S16_PCREL:
      case BFD_RELOC_KVX_PCREL17:
      case BFD_RELOC_KVX_PCREL27:

      case BFD_RELOC_KVX_S64_PCREL_LO10:
      case BFD_RELOC_KVX_S64_PCREL_UP27:
      case BFD_RELOC_KVX_S64_PCREL_EX27:

      case BFD_RELOC_KVX_S43_PCREL_LO10:
      case BFD_RELOC_KVX_S43_PCREL_UP27:
      case BFD_RELOC_KVX_S43_PCREL_EX6:

      case BFD_RELOC_KVX_S37_PCREL_LO10:
      case BFD_RELOC_KVX_S37_PCREL_UP27:

        if (fixP->fx_pcrel || fixP->fx_addsy)
          return;
        value = (((value >> rel->howto->rightshift) << rel->howto->bitpos ) & rel->howto->dst_mask);
        image = (image & ~(rel->howto->dst_mask)) | value;
        md_number_to_chars(fixpos, image, fixP->fx_size);
        break;

      case BFD_RELOC_KVX_S64_GOTADDR_LO10:
      case BFD_RELOC_KVX_S64_GOTADDR_UP27:
      case BFD_RELOC_KVX_S64_GOTADDR_EX27:

      case BFD_RELOC_KVX_S43_GOTADDR_LO10:
      case BFD_RELOC_KVX_S43_GOTADDR_UP27:
      case BFD_RELOC_KVX_S43_GOTADDR_EX6:

      case BFD_RELOC_KVX_S37_GOTADDR_LO10:
      case BFD_RELOC_KVX_S37_GOTADDR_UP27:
	value = 0;
	/* Fallthrough */

      case BFD_RELOC_KVX_S32_UP27:
      case BFD_RELOC_KVX_S37_UP27:
      case BFD_RELOC_KVX_S43_UP27:
      case BFD_RELOC_KVX_S64_UP27:
      case BFD_RELOC_KVX_S64_EX27:
      case BFD_RELOC_KVX_S64_LO10:
      case BFD_RELOC_KVX_S43_TLS_LE_UP27:
      case BFD_RELOC_KVX_S43_TLS_LE_EX6:
      case BFD_RELOC_KVX_S37_TLS_LE_UP27:
      case BFD_RELOC_KVX_S37_GOTOFF_UP27:
      case BFD_RELOC_KVX_S43_GOTOFF_UP27:
      case BFD_RELOC_KVX_S43_GOTOFF_EX6:
      case BFD_RELOC_KVX_S43_GOT_UP27:
      case BFD_RELOC_KVX_S43_GOT_EX6:
      case BFD_RELOC_KVX_S37_GOT_UP27:
      case BFD_RELOC_KVX_S32_LO5:
      case BFD_RELOC_KVX_S37_LO10:
      case BFD_RELOC_KVX_S43_LO10:
      case BFD_RELOC_KVX_S43_EX6:
      case BFD_RELOC_KVX_S43_TLS_LE_LO10:
      case BFD_RELOC_KVX_S37_TLS_LE_LO10:
      case BFD_RELOC_KVX_S37_GOTOFF_LO10:
      case BFD_RELOC_KVX_S43_GOTOFF_LO10:
      case BFD_RELOC_KVX_S43_GOT_LO10:
      case BFD_RELOC_KVX_S37_GOT_LO10:

      default:
        as_fatal("[md_apply_fix] unsupported relocation type (type not handled : %d)", fixP->fx_r_type);
      }
  }
}

void
kvx_validate_fix(fixS *fix)
{
    switch (fix->fx_r_type)
    {
        // case BFD_RELOC_KVX_FPTR32:
        // case BFD_RELOC_KVX_GOTOFF_FPTR_LO9:
        // case BFD_RELOC_KVX_GOTOFF_FPTR_HI23:
        //   if (fix->fx_offset != 0)
        //     as_bad_where (fix->fx_file, fix->fx_line,
        //     	      "No addend allowed in @fptr() relocation");
        //   break;
        default:
            break;
    }

    return;
}

/*
 * Warning: Can be called only in fixup_segment() after fx_addsy field
 * has been updated by calling symbol_get_value_expression(...->X_add_symbol)
 */
int
kvx_validate_sub_fix(fixS *fixP)
{
    segT add_symbol_segment, sub_symbol_segment;

    switch (fixP->fx_r_type)
    {
        case BFD_RELOC_16:
        case BFD_RELOC_32:
            if (fixP->fx_addsy != NULL)
                add_symbol_segment = S_GET_SEGMENT(fixP->fx_addsy);
            else
                return 0;
            if (fixP->fx_subsy != NULL)
                sub_symbol_segment = S_GET_SEGMENT(fixP->fx_subsy);
            else
                return 0;

            if ((strcmp(S_GET_NAME(fixP->fx_addsy),
                    S_GET_NAME(fixP->fx_subsy)) == 0) &&
                    (add_symbol_segment == sub_symbol_segment)) {
                return 1;
            }

            break;
        default:
            break;
    }

    return 0;
}

/* This is called whenever some data item (not an instruction) needs a
 * fixup.  */
void
kvx_cons_fix_new(fragS *f, int where, int nbytes, expressionS *exp, bfd_reloc_code_real_type code)
{
  if (exp->X_op == O_pseudo_fixup)
    {
      exp->X_op = O_symbol;
      /* real_kvx_reloc_type(exp->X_op_symbol, 0, 0, 0, &code); */
      struct pseudo_func_s *pf = kvx_get_pseudo_func_data_scn(exp->X_op_symbol);
      assert(pf != NULL);
      code = pf->pseudo_relocs.single;

      if (code == BFD_RELOC_UNUSED)
        as_fatal("Unsupported relocation");
    }
  else
    {
      switch (nbytes)
        {
          /* [SC] We have no relocation for BFD_RELOC_8, but accept it
           * here in case we can later eliminate the fixup (in md_apply_fix).
           * This is required to pass the gas test forward.s.
           */
        case 1:
          code = BFD_RELOC_8;
          break;
        case 2:
          code = BFD_RELOC_16;
          break;
        case 4:
          code = BFD_RELOC_32;
          break;
        case 8:
          code = BFD_RELOC_64;
          break;
        default:
          as_fatal("unsupported BFD relocation size %u", nbytes);
          break;
        }
    }
  fix_new_exp(f, where, nbytes, exp, 0, code);
}

/*
 * generate a relocation record
 */

arelent *
tc_gen_reloc(asection * sec ATTRIBUTE_UNUSED, fixS * fixp)
{
    arelent *reloc;
    bfd_reloc_code_real_type code;

    reloc = (arelent *) xmalloc(sizeof (arelent));

    reloc->sym_ptr_ptr = (asymbol **) xmalloc(sizeof (asymbol *));
    *reloc->sym_ptr_ptr = symbol_get_bfdsym(fixp->fx_addsy);
    reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;

    code = fixp->fx_r_type;
    if (code == BFD_RELOC_32 && fixp->fx_pcrel)
    {
        code = BFD_RELOC_32_PCREL;
    }
    reloc->howto = bfd_reloc_type_lookup(stdoutput, code);

    if (reloc->howto == NULL)
    {
        as_bad_where(fixp->fx_file, fixp->fx_line,
                "cannot represent `%s' relocation in object file",
                bfd_get_reloc_code_name(code));
        return NULL;
    }

    if (!fixp->fx_pcrel != !reloc->howto->pc_relative)
    {
        as_fatal("internal error? cannot generate `%s' relocation",
                bfd_get_reloc_code_name(code));
    }
    assert(!fixp->fx_pcrel == !reloc->howto->pc_relative);

    reloc->addend = fixp->fx_offset;

    /*
     * Ohhh, this is ugly.  The problem is that if this is a local global
     * symbol, the relocation will entirely be performed at link time, not
     * at assembly time.  bfd_perform_reloc doesn't know about this sort
     * of thing, and as a result we need to fake it out here.
     */

    /* GD I'm not sure what this is used for in the kvx case but it sure  */
    /* messes up the relocs when emit_all_relocs is used as they are not */
    /* resolved with respect to a global sysmbol (e.g. .text), and hence */
    /* they are ALWAYS resolved at link time                             */
    /* FIXME FIXME                                                       */

    /* clarkes: 030827:  This code (and the other half of the fix in write.c)
     * have caused problems with the PIC relocations.
     * The root problem is that bfd_install_relocation adds in to the reloc
     * addend the section offset of a symbol defined in the current object.
     * This causes problems on numerous other targets too, and there are
     * several different methods used to get around it:
     *   1.  In tc_gen_reloc, subtract off the value that bfd_install_relocation
     *       added.  That is what we do here, and it is also done the
     *       same way for alpha.
     *   2.  In md_apply_fix, subtract off the value that bfd_install_relocation
     *       will add.  This is done on SH (non-ELF) and sparc targets.
     *   3.  In the howto structure for the relocations, specify a
     *       special function that does not return bfd_reloc_continue.
     *       This causes bfd_install_relocaion to terminate before it
     *       adds in the symbol offset.  This is done on SH ELF targets.
     *       Note that on ST200 we specify bfd_elf_generic_reloc as
     *       the special function.  This will return bfd_reloc_continue
     *       only in some circumstances, but in particular if the reloc
     *       is marked as partial_inplace in the bfd howto structure, then
     *       bfd_elf_generic_reloc will return bfd_reloc_continue.
     *       Some ST200 relocations are marked as partial_inplace
     *       (this is an error in my opinion because ST200 always uses
     *       a separate addend), but some are not.  The PIC relocations
     *       are not marked as partial_inplace, so for them,
     *       bfd_elf_generic_reloc returns bfd_reloc_ok, and the addend
     *       is not modified by bfd_install_relocation.   The relocations
     *       R_KVX_16 and R_KVX_32 are marked partial_inplace, and so for
     *       these we need to correct the addend.
     * In the code below, the condition in the emit_all_relocs branch
     * (now moved to write.c) is the inverse of the condition that
     * bfd_elf_generic_reloc uses to short-circuit the code in
     * bfd_install_relocation that modifies the addend.  The condition
     * in the else branch match the condition used in the alpha version
     * of tc_gen_reloc (see tc-alpha.c).
     * I do not know why we need to use different conditions in these
     * two branches, it seems to me that the condition should be the same
     * whether or not emit_all_relocs is true.
     * I also do not understand why it was necessary to move the emit_all_relocs
     * condition to write.c.
     */

    if (S_IS_EXTERNAL(fixp->fx_addsy)  &&
        !S_IS_COMMON(fixp->fx_addsy) &&
        reloc->howto->partial_inplace)
      reloc->addend -= symbol_get_bfdsym(fixp->fx_addsy)->value;

    return reloc;
}

/* Round up segment to appropriate boundary */

valueT
md_section_align(asection * seg ATTRIBUTE_UNUSED, valueT size)
{
#ifndef OBJ_ELF
    /* This is not right for ELF; a.out wants it, and COFF will force
     * the alignment anyways.  */
    int align = bfd_get_section_alignment(stdoutput, seg);
    valueT mask = ((valueT) 1 << align) - 1;
    return (size + mask) & ~mask;
#else
    return size;
#endif
}

int
md_estimate_size_before_relax(register fragS * fragP ATTRIBUTE_UNUSED,
        segT segtype ATTRIBUTE_UNUSED)
{
    as_fatal("estimate_size_before_relax called\n");
}

void
md_convert_frag(bfd * abfd ATTRIBUTE_UNUSED,
        asection * sec ATTRIBUTE_UNUSED,
        fragS * fragp ATTRIBUTE_UNUSED)
{
    as_fatal("kvx convert_frag\n");
}

symbolS *
md_undefined_symbol(char *name ATTRIBUTE_UNUSED)
{
    return 0;
}

const char *
md_atof(int type ATTRIBUTE_UNUSED,
        char *litp ATTRIBUTE_UNUSED,
        int *sizep ATTRIBUTE_UNUSED)
{
    /* we'll need this for reading floating point constants */
    return _("floating-point literals are not supported");
}

/*
 * calculate the base for a pcrel fixup
 * -- for relocation, we might need to add addend ?
 */

long
md_pcrel_from(fixS * fixP)
{
    return (fixP->fx_where + fixP->fx_frag->fr_address);
}

/************************************************************/
/*   Hooks into standard processing -- we hook into label   */
/*   handling code to detect double ':' and we hook before  */
/*   a line of code is processed to do some simple sed style */
/*   edits.                                                 */
/************************************************************/

static symbolS *last_proc_sym = NULL;
static int update_last_proc_sym = 0;

void
kvx_frob_label(symbolS * sym) {
    if (input_line_pointer[1] == ':')        /* second colon => global symbol */ {
        S_SET_EXTERNAL(sym);
        input_line_pointer++;
    }

    if (update_last_proc_sym) {
        last_proc_sym = sym;
        update_last_proc_sym = 0;
    }

    if (inside_bundle) {
        struct label_fix *fix;
        fix = malloc (sizeof (*fix));
        fix->next = label_fixes;
        fix->sym = sym;
        label_fixes = fix;
    }
}

/*  edit out some syntactic sugar that confuses GAS       */
/*  input_line_pointer is guaranteed to point to the      */
/*  the current line but may include text from following  */
/*  lines.  Thus, '\n' must be scanned for as well as '\0' */

void
kvx_md_start_line_hook(void) {
    char *t;

    for (t = input_line_pointer; t && t[0] == ' '; t++);

    /* Detect illegal syntax patterns:
     * - two bundle ends on the same line: ;; ;;
     * - illegal token: ;;;
     */
    if (t && (t[0] == ';') && (t[1] == ';')) {
        char *tmp_t;
        bfd_boolean newline_seen = FALSE;

        if (t[2] == ';') {
            as_fatal("Syntax error: Illegal ;;; token");
        }

        tmp_t = t + 2;

        while (tmp_t && tmp_t[0]) {
            while (tmp_t && tmp_t[0] &&
                    ((tmp_t[0] == ' ') || (tmp_t[0] == '\n'))) {
                if (tmp_t[0] == '\n') {
                    newline_seen = TRUE;
                }
                tmp_t++;
            }
            if (tmp_t[0] == ';' && tmp_t[1] == ';') {
                /* if there's no newline between the two bundle stops
                 * then raise a syntax error now, otherwise a strange error
                 * message from read.c will be raised: "junk at end of line..."
                 */
                if (tmp_t[2] == ';') {
                    as_fatal("Syntax error: Illegal ;;; token");
                }

                if (!newline_seen) {
                    as_fatal("Syntax error: More than one bundle stop on a line");
                }
                newline_seen = FALSE; /* reset */

                /* this is an empty bundle, transform it into an
                 * empty statement */
                tmp_t[0] = ';';
                tmp_t[1] = ' ';

                tmp_t += 2;
            } else {
                break;
            }
        }
    }

    /* check for bundle end                             */
    /* we transform these into a special opcode BE      */
    /* because gas has ';' hardwired as a statement end */
    if (t && (t[0] == ';') && (t[1] == ';'))
    {
        t[0] = 'B';
        t[1] = 'E';
        return;
    }
}

static void
kvx_check_resources(int f)
{
    check_resource_usage = f;
}

/** called before write_object_file */
void
kvx_end(void)
{
    int newflags;
    Elf_Internal_Ehdr * i_ehdrp;

    if (! kvx_core_set)
        kvx_core = kvx_core_info->elf_cores[subcore_id];

    /* (pp) the flags must be set at once */
    newflags = kvx_core | kvx_abi | kvx_pic_flags;

    if (kvx_arch_size == 64) {
      newflags |= ELF_KVX_ABI_64B_ADDR_BIT;
    }

    bfd_set_private_flags(stdoutput, newflags);

    i_ehdrp = elf_elfheader(stdoutput);
    i_ehdrp->e_ident[EI_ABIVERSION] = kvx_abi;
    i_ehdrp->e_ident[EI_OSABI] = kvx_osabi;
}

static void
kvx_type(int start ATTRIBUTE_UNUSED)
{
    char *name;
    char c;
    int type;
    char *typename = NULL;
    symbolS *sym;
    elf_symbol_type *elfsym;

    /* name = input_line_pointer; */
    /* c = get_symbol_end(); */
    c = get_symbol_name(&name);
    sym = symbol_find_or_make(name);
    elfsym = (elf_symbol_type *) symbol_get_bfdsym(sym);
    *input_line_pointer = c;

    SKIP_WHITESPACE();
    if (*input_line_pointer == ',')
        ++input_line_pointer;

    SKIP_WHITESPACE();
    if (   *input_line_pointer == '#'
            || *input_line_pointer == '@'
            || *input_line_pointer == '"'
            || *input_line_pointer == '%')
        ++input_line_pointer;

    /* typename = input_line_pointer; */
    /* c = get_symbol_end(); */
    c = get_symbol_name(&typename);

    type = 0;
    if (strcmp(typename, "function") == 0
            || strcmp(typename, "STT_FUNC") == 0)
        type = BSF_FUNCTION;
    else if (strcmp(typename, "object") == 0
            || strcmp(typename, "STT_OBJECT") == 0)
        type = BSF_OBJECT;
    else if (strcmp(typename, "tls_object") == 0
            || strcmp(typename, "STT_TLS") == 0)
        type = BSF_OBJECT | BSF_THREAD_LOCAL;
    else if (strcmp(typename, "notype") == 0
            || strcmp(typename, "STT_NOTYPE") == 0)
        ;
#ifdef md_elf_symbol_type
    else if ((type = md_elf_symbol_type(typename, sym, elfsym)) != -1)
        ;
#endif
    else
        as_fatal(_("unrecognized symbol type \"%s\""), typename);

    *input_line_pointer = c;

    if (*input_line_pointer == '"')
        ++input_line_pointer;

    elfsym->symbol.flags |= type;
    symbol_get_bfdsym(sym)->flags |= type;

/* FIXME: do we need that? */
/*
    while (*input_line_pointer == ',') {
        ++input_line_pointer;

        SKIP_WHITESPACE();

        typename = input_line_pointer;
        c = get_symbol_end();
        if (strcmp(typename, "moveable") == 0) {
            if (emit_all_relocs) {
                S_SET_OTHER(sym, S_GET_OTHER(sym) | STO_MOVEABLE);
            }
        } else if (strcmp(typename, "used") == 0) {
            if (emit_all_relocs) {
                S_SET_OTHER(sym, S_GET_OTHER(sym) | STO_USED);
            }
        } else
            as_bad(_("unrecognized symbol type \"%s\""), typename);
        *input_line_pointer = c;
    }
*/
    demand_empty_rest_of_line();
}

#define ENDPROCEXTENSION	"$endproc"
#define MINUSEXPR		".-"

static int proc_endp_status = 0;

static void
kvx_endp(int start ATTRIBUTE_UNUSED)
{
    char c;
    char *name;

    if(inside_bundle){
      as_warn(".endp directive inside a bundle.");
    }
    /* function name is optionnal and is ignored */
    /* there may be several names separated by commas... */
    while (1)
    {
        SKIP_WHITESPACE();
        c = get_symbol_name(&name);
        (void)restore_line_pointer(c);
        /* c = get_symbol_end(); */
        /* *input_line_pointer = c; */
        SKIP_WHITESPACE();
        if (*input_line_pointer != ',')
            break;
        ++input_line_pointer;
    }
    demand_empty_rest_of_line();

    if (!proc_endp_status)
    {
        as_warn(".endp directive doesn't follow .proc -- ignoring ");
        return;
    }

    proc_endp_status = 0;

    /* TB begin : add BSF_FUNCTION attribute to last_proc_sym symbol */
    if (size_type_function) {
        if (!last_proc_sym) {
            as_bad("Cannot set function attributes (bad symbol)");
            return;
        }

        /*    last_proc_sym->symbol.flags |= BSF_FUNCTION; */
        symbol_get_bfdsym(last_proc_sym)->flags |= BSF_FUNCTION;
        /* Add .size funcname,.-funcname in order to add size
         * attribute to the current function */
        {
            const int newdirective_sz = strlen(S_GET_NAME(last_proc_sym)) + strlen(MINUSEXPR) + 1;
            char *newdirective = malloc(newdirective_sz);
            char *savep = input_line_pointer;
            expressionS exp;

            memset(newdirective, 0, newdirective_sz);

            /* BUILD :".-funcname" expression */
            strcat(newdirective, MINUSEXPR);
            strcat(newdirective, S_GET_NAME(last_proc_sym));
            input_line_pointer = newdirective;
            expression(&exp);

            if (exp.X_op == O_constant)
            {
                S_SET_SIZE(last_proc_sym, exp.X_add_number);
                if (symbol_get_obj(last_proc_sym)->size)
                {
                    xfree(symbol_get_obj(last_proc_sym)->size);
                    symbol_get_obj(last_proc_sym)->size = NULL;
                }
            }
            else
            {
                symbol_get_obj(last_proc_sym)->size =
                        (expressionS *) xmalloc(sizeof (expressionS));
                *symbol_get_obj(last_proc_sym)->size = exp;
            }

            /* just restore the real input pointer */
            input_line_pointer = savep;
            free (newdirective);
        }
    }
    /* TB end */

    last_proc_sym = NULL;
}

static void
kvx_proc(int start ATTRIBUTE_UNUSED)
{
    char c;
    char *name;
    /* there may be several names separated by commas... */
    while (1)
    {
        SKIP_WHITESPACE();
        c = get_symbol_name(&name);
        (void)restore_line_pointer(c);

        /* c = get_symbol_end(); */
        /* *input_line_pointer = c; */
        SKIP_WHITESPACE();
        if (*input_line_pointer != ',')
            break;
        ++input_line_pointer;
    }
    demand_empty_rest_of_line();

    if (proc_endp_status)
    {
        as_warn(".proc follows .proc -- ignoring");
        return;
    }

    proc_endp_status = 1;

    /* this code emit a global symbol to mark the end of each function    */
    /* the symbol emitted has a name formed by the original function name */
    /* concatenated with $endproc so if _foo is a function name the symbol */
    /* marking the end of it is _foo$endproc                              */
    /* It is also required for generation of .size directive in kvx_endp() */

    if (size_type_function)
    {
        update_last_proc_sym = 1;
    }
}

int
kvx_force_reloc(fixS * fixP)
{
    symbolS *sym;
    asection *symsec;

    if (generic_force_reloc(fixP))
        return 1;

    switch (fixP->fx_r_type)
      {
      case BFD_RELOC_KVX_32_GOTOFF:
      case BFD_RELOC_KVX_S37_GOTOFF_UP27:
      case BFD_RELOC_KVX_S37_GOTOFF_LO10:

      case BFD_RELOC_KVX_64_GOTOFF:
      case BFD_RELOC_KVX_S43_GOTOFF_UP27:
      case BFD_RELOC_KVX_S43_GOTOFF_LO10:
      case BFD_RELOC_KVX_S43_GOTOFF_EX6:

      case BFD_RELOC_KVX_32_GOT:
      case BFD_RELOC_KVX_64_GOT:
      case BFD_RELOC_KVX_S37_GOT_UP27:
      case BFD_RELOC_KVX_S37_GOT_LO10:

      case BFD_RELOC_KVX_GLOB_DAT:
        return 1;
      default:
        return 0;
      }

    sym = fixP->fx_addsy;
    if (sym)
    {
        symsec = S_GET_SEGMENT(sym);
        /* if (bfd_is_abs_section (symsec)) return 0; */
        if (!SEG_NORMAL(symsec))
            return 0;
    }
    return 1;
}

int
kvx_force_reloc_sub_same(fixS * fixP, segT sec)
{
    symbolS *sym;
    asection *symsec;
    const char *sec_name = NULL;

    if (generic_force_reloc(fixP))
        return 1;

    switch (fixP->fx_r_type)
      {
      case BFD_RELOC_KVX_32_GOTOFF:
      case BFD_RELOC_KVX_S37_GOTOFF_UP27:
      case BFD_RELOC_KVX_S37_GOTOFF_LO10:

      case BFD_RELOC_KVX_64_GOTOFF:
      case BFD_RELOC_KVX_S43_GOTOFF_UP27:
      case BFD_RELOC_KVX_S43_GOTOFF_LO10:
      case BFD_RELOC_KVX_S43_GOTOFF_EX6:

      case BFD_RELOC_KVX_32_GOT:
      case BFD_RELOC_KVX_64_GOT:
      case BFD_RELOC_KVX_S37_GOT_UP27:
      case BFD_RELOC_KVX_S37_GOT_LO10:

      case BFD_RELOC_KVX_GLOB_DAT:
        return 1;

      default:
        return 0;
      }

    sym = fixP->fx_addsy;
    if (sym)
    {
        symsec = S_GET_SEGMENT(sym);
        /* if (bfd_is_abs_section (symsec)) return 0; */
        if (!SEG_NORMAL(symsec))
            return 0;

        /*
         * for .debug_arrange, .debug_frame, .eh_frame sections, containing
         * expressions of the form "sym2 - sym1 + addend", solve them even when
         * --emit-all-relocs is set. Otherwise, a relocation on two symbols
         * is necessary and fails at elf level. Binopt should not be impacted by
         * the resolution of this relocatable expression on symbols inside a
         * function.
         */
        sec_name =  segment_name(sec);
        if ((strcmp(sec_name, ".eh_frame") == 0) ||
                (strcmp(sec_name, ".except_table") == 0) ||
                (strncmp(sec_name, ".debug_", strlen(".debug_")) == 0)) {
            return 0;
        }
    }
    return 1;
}

/*
 *    Support for unwind descriptors
 */

#define KVXMAXUNWINDARGS 4

static int
kvx_get_constant(const expressionS arg)
{
    if (arg.X_op && !arg.X_add_symbol)
    {
        return arg.X_add_number;
    }
    else
    {
        as_warn("expected constant argument");
        return 0;
    }
}

static void
kvx_emit_uleb128(int i)
{
    do
    {
        int tmp = i & 127;
        i = (i & 0x1fffffff) >> 7;
        FRAG_APPEND_1_CHAR((i != 0) ? (tmp + 1) : tmp);
    }
    while (i != 0);
}

/* Implement HANDLE_ALIGN.  */

static void
kvx_make_nops (char *buf, bfd_vma bytes)
{
  bfd_vma i = 0;
  unsigned int j;

  static unsigned int nop_single = 0;

  if (!nop_single)
    {
      const kv3opc_t *opcode = (kv3opc_t *) hash_find(kvx_opcode_hash, "nop");

      if (opcode == NULL)
        as_fatal("internal error: could not find opcode for 'nop' during padding");

      nop_single = opcode->codewords[0].opcode;
    }

  /* KVX instructions are always 4-bytes aligned. If we are at a position */
  /* that is not 4 bytes aligned, it means this is not part of an instruction, */
  /* so it is safe to use a zero byte for padding. */

  for (j = bytes % 4; j > 0; j--)
      buf[i++] = 0;

  for (j = 0; j < (bytes-i); j+=4 )
    {
      unsigned nop = nop_single;

      // nop has bundle end only if #4 nop or last padding nop.
      // Sets the parallel bit when neither conditions are matched.
      // 4*4 = biggest nop bundle we can get
      // 12 = offset when writting the last nop possible in a 4 nops bundle
      // bytes-i-4 = offset for the last 4-words in the padding
      if (j%(4*4) != 12 && j != (bytes-i-4))
        nop |= PARALLEL_BIT;

      memcpy(buf + i + j, &nop, sizeof(nop));
    }
}

/* Pads code section with bundle of nops when possible, 0 if not. */
void
kvx_handle_align (fragS *fragP)
{
  switch (fragP->fr_type)
    {
    case rs_align_code:
      {
        bfd_signed_vma bytes = (fragP->fr_next->fr_address
                                - fragP->fr_address - fragP->fr_fix);
        char *p = fragP->fr_literal + fragP->fr_fix;

        if (bytes <= 0)
          break;

        /* Insert zeros or nops to get 4 byte alignment.  */
        kvx_make_nops (p, bytes);
        fragP->fr_fix += bytes;
      }
      break;

    default:
      break;
    }
}

static void
kvx_unwind(int r)
{
    int ntok;
    expressionS tok[KVXMAXUNWINDARGS];
    char *f;

    int tmp;

    if (strcmp(segment_name(now_seg), ".KVX.unwind_info") != 0)
    {
        as_warn("unwind directive not in .KVX.unwind_info segment\n");
        return;
    }

    ntok = tokenize_arguments(input_line_pointer, tok, NULL, KVXMAXUNWINDARGS);
    while (input_line_pointer && (input_line_pointer[0] != '\n'))
        input_line_pointer++;

#define KVXGETCONST(i) (((i)<ntok) ? \
		       kvx_get_constant(tok[(i)]) :  \
                        (as_warn("too few operands"), 0))

    switch ((enum unwrecord) r)
    {
        case UNW_HEADER:		/* postpone fixup work for now */
            tmp = (KVXGETCONST(0) << 8);
            tmp += (KVXGETCONST(1) & 1);
            tmp += ((KVXGETCONST(2) & 1) << 1);
            emit_expr(tok + 3, 2);
            f = frag_more(2);
            md_number_to_chars(f, tmp, 2);
            break;

        case UNW_PROLOGUE:
            tmp = KVXGETCONST(0);
            if (tmp < 32)
            {
                FRAG_APPEND_1_CHAR(tmp);
            }
            else
            {
                FRAG_APPEND_1_CHAR(0x40);
                kvx_emit_uleb128(tmp);
            }
            break;

        case UNW_BODY:
            tmp = KVXGETCONST(0);
            if (tmp < 32)
            {
                FRAG_APPEND_1_CHAR(tmp + 0x20);
            }
            else
            {
                FRAG_APPEND_1_CHAR(0x41);
                kvx_emit_uleb128(tmp);
            }
            break;

        case UNW_MEM_STACK_F:
            FRAG_APPEND_1_CHAR(0xe0);
            kvx_emit_uleb128(KVXGETCONST(0));
            kvx_emit_uleb128(KVXGETCONST(1));
            break;

        case UNW_MEM_STACK_V:
            FRAG_APPEND_1_CHAR(0xe1);
            kvx_emit_uleb128(KVXGETCONST(0));
            break;

        case UNW_PSP_SPREL:
            FRAG_APPEND_1_CHAR(0xe2);
            kvx_emit_uleb128(KVXGETCONST(0));
            break;

        case UNW_RP_WHEN:
            FRAG_APPEND_1_CHAR(0xe3);
            kvx_emit_uleb128(KVXGETCONST(0));
            break;

        case UNW_RP_PSPREL:
            FRAG_APPEND_1_CHAR(0xe4);
            kvx_emit_uleb128(KVXGETCONST(0));
            break;

        case UNW_RP_SPREL:
            FRAG_APPEND_1_CHAR(0xe5);
            kvx_emit_uleb128(KVXGETCONST(0));
            break;

        case UNW_SPILL_BASE:
            FRAG_APPEND_1_CHAR(0xe6);
            kvx_emit_uleb128(KVXGETCONST(0));
            break;

        case UNW_PSP_GR:
            tmp = KVXGETCONST(0);
            FRAG_APPEND_1_CHAR(0xb0);
            FRAG_APPEND_1_CHAR(tmp & 0x3f);
            break;

        case UNW_RP_GR:
            tmp = KVXGETCONST(0);
            FRAG_APPEND_1_CHAR(0xb1);
            FRAG_APPEND_1_CHAR(tmp & 0x3f);
            break;

        case UNW_GR_MEM_S:
            tmp = KVXGETCONST(0);
            FRAG_APPEND_1_CHAR(0xc0 + (tmp & 0x0f));
            break;

        case UNW_GR_MEM_L:
            tmp = KVXGETCONST(0);
            FRAG_APPEND_1_CHAR(0x90 + (tmp & 7));
            kvx_emit_uleb128(KVXGETCONST(1));
            break;

        case UNW_SPILL_MASK:
            as_warn("unwind spill mask not interpreted yet \n");
            break;

        case UNW_EPILOGUE:
            tmp = KVXGETCONST(1);
            if (tmp < 32)
            {
                FRAG_APPEND_1_CHAR(0xc0 + tmp);
                kvx_emit_uleb128(KVXGETCONST(0));
            }
            else
            {
                FRAG_APPEND_1_CHAR(0xe0);
                kvx_emit_uleb128(KVXGETCONST(0));
                kvx_emit_uleb128(tmp);
            }
            break;

        case UNW_LABEL_STATE:
            tmp = KVXGETCONST(0);
            if (tmp > 32)
            {
                FRAG_APPEND_1_CHAR(0xe1);
                kvx_emit_uleb128(tmp);
            }
            else
            {
                FRAG_APPEND_1_CHAR(0x80 + tmp);
            }
            break;

        case UNW_COPY_STATE:
            tmp = KVXGETCONST(0);
            if (tmp > 32)
            {
                FRAG_APPEND_1_CHAR(0xe9);
                kvx_emit_uleb128(tmp);
            }
            else
            {
                FRAG_APPEND_1_CHAR(0xa0 + tmp);
            }
            break;

        case UNW_SPILL_PSREL:
            as_warn("unw_spill_psrel unimplemented\n");
            break;
        case UNW_SPILL_SPREL:
            as_warn("unw_spill_sprel unimplemented\n");
            break;
        default:
            as_warn("unrecognized unwind descriptor\n");
    }
}

static void
print_operand(expressionS * e, FILE * out) ATTRIBUTE_UNUSED;

/*
 * This is just used for debugging
 */

static void
print_operand(expressionS * e, FILE * out)
{
    if (e)
    {
        switch (e->X_op)
        {
            case O_register:
                fprintf(out, "%s",  kvx_registers[e->X_add_number].name);
                break;

            case O_constant:
                if (e->X_add_symbol)
                {
                    if (e->X_add_number)
                        fprintf(out, "(%s + %d)", S_GET_NAME(e->X_add_symbol),
                                (int) e->X_add_number);
                    else
                        fprintf(out, "%s", S_GET_NAME(e->X_add_symbol));
                }
                else
                    fprintf(out, "%d", (int) e->X_add_number);
                break;

            case O_symbol:
                if (e->X_add_symbol)
                {
                    if (e->X_add_number)
                        fprintf(out, "(%s + %d)", S_GET_NAME(e->X_add_symbol),
                                (int) e->X_add_number);
                    else
                        fprintf(out, "%s", S_GET_NAME(e->X_add_symbol));
                }
                else
                    fprintf(out, "%d", (int) e->X_add_number);
                break;

            default:
                fprintf(out, "o,ptype-%d", e->X_op);
        }
    }
}

void
kvx_cfi_frame_initial_instructions(void)
{
  cfi_add_CFA_def_cfa (KV3_SP_REGNO, 0);
}

int
kvx_regname_to_dw2regnum(const char *regname)
{
    unsigned int regnum = -1;
    const char *p;
    char *q;

    if (regname[0] == 'r')
    {
        p = regname + 1;
        regnum = strtoul(p, &q, 10);
        if (p == q || *q || regnum >= 64)
            return -1;
    }
    return regnum;
}
