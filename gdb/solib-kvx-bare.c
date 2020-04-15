#include "defs.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "gdbcore.h"
#include "remote.h"
#include "inferior.h"
#include "solist.h"
#include "objfiles.h"
#include "solib-kvx-bare.h"

struct target_so_ops kvx_bare_solib_ops;

#define DL_VERSION 1

struct mppa_dl_debug_s
{
  uint64_t version; /* Protocol version */
  uint64_t head;    /* Head of the shared object chain */
  uint64_t brk;     /* Address of the breakpoint function */
  uint64_t valid;   /* The  shared object chain is valid */
} __attribute__ ((packed));

struct mppa_dl_debug_map_s
{
  uint64_t load_addr;     /* Load address for the dynamic object */
  uint64_t file_name;     /* File name of the dynamic object */
  uint64_t file_name_len; /* File name length of the dynamic object */
  uint64_t next;	  /* Chain of loaded objects */
} __attribute__ ((packed));

struct lm_info
{
  CORE_ADDR lm_addr;
};

struct kvx_bare_so_list
{
  struct so_list sl; /* Struct containing the common fields. Must be the first
		      * field of struct kvx_bare_so_list for delete */
  struct lm_info li; /* KVX specific data */
};

struct kvx_bare_solib_info
{
  CORE_ADDR brk;
  CORE_ADDR head_addr;
  CORE_ADDR valid_addr;
  int no_name_shown;
  struct so_list *last_head;
};

/* Per-program-space data key. */
static const struct program_space_data *kvx_bare_solib_pspace_data;

static void
kvx_bare_solib_pspace_data_cleanup (struct program_space *pspace, void *arg)
{
  xfree (arg);
}

/* Get the current KVX data.  If none is found yet, add it now.  This
   function always returns a valid object */
static struct kvx_bare_solib_info *
kvx_bare_solib_get_info (void)
{
  struct kvx_bare_solib_info *info;

  info = (struct kvx_bare_solib_info *)
    program_space_data (current_program_space, kvx_bare_solib_pspace_data);
  if (info != NULL)
    return info;

  info = xzalloc (sizeof (struct kvx_bare_solib_info));
  set_program_space_data (current_program_space, kvx_bare_solib_pspace_data,
			  info);

  return info;
}

void
kvx_bare_solib_load_debug_info (void)
{
  struct bound_minimal_symbol msym;
  CORE_ADDR mppa_dl_debug_addr;
  struct mppa_dl_debug_s dl_debug;
  struct kvx_bare_solib_info *info;

  info = kvx_bare_solib_get_info ();
  if (info->head_addr)
    return;

  msym = lookup_minimal_symbol ("mppa_dl_debug", NULL, NULL);
  if (msym.minsym == NULL)
    return;

  mppa_dl_debug_addr = BMSYMBOL_VALUE_ADDRESS (msym);

  if (target_read_memory (mppa_dl_debug_addr, (gdb_byte *) &dl_debug,
			  sizeof (struct mppa_dl_debug_s)))
    return;

  if (dl_debug.version != DL_VERSION)
    {
      warning ("solib-kvx: incorrect version %lld (expected %d)\n",
	       (unsigned long long) dl_debug.version, DL_VERSION);
      return;
    }

  info->brk = dl_debug.brk;
  info->head_addr
    = (CORE_ADDR) & ((struct mppa_dl_debug_s *) mppa_dl_debug_addr)->head;
  info->valid_addr
    = (CORE_ADDR) & ((struct mppa_dl_debug_s *) mppa_dl_debug_addr)->valid;

  create_solib_event_breakpoint (target_gdbarch (), info->brk);
}

/* The section table is built from bfd sections using bfd VMAs.
   Relocate these VMAs according to solib info */
static void
kvx_bare_solib_relocate_section_addresses (struct so_list *so,
					  struct target_section *sec)
{
  sec->addr += so->lm_info->lm_addr;
  sec->endaddr += so->lm_info->lm_addr;

  // best effort to set addr_high/addr_low, used by 'info sharedlibary'.
  if (so->addr_high == 0)
    {
      so->addr_low = sec->addr;
      so->addr_high = sec->endaddr;
    }
  if (sec->endaddr > so->addr_high)
    so->addr_high = sec->endaddr;
  if (sec->addr < so->addr_low)
    so->addr_low = sec->addr;
}

static void
kvx_bare_solib_free_so (struct so_list *so)
{
}

static void
kvx_bare_solib_clear_solib (void)
{
  struct kvx_bare_solib_info *info = kvx_bare_solib_get_info ();

  info->brk = 0;
  info->head_addr = 0;
  info->valid_addr = 0;
}

/* Shared library startup support.  See documentation in solib-svr4.c */
static void
kvx_bare_solib_solib_create_inferior_hook (int from_tty)
{
  kvx_bare_solib_clear_solib ();
  kvx_bare_solib_load_debug_info ();
}

/* No special symbol handling.  */
static void
kvx_bare_solib_special_symbol_handling (void)
{
}

static struct so_list *
kvx_bare_solib_dup_chain (struct so_list *head)
{
  struct so_list *dup_head = NULL, **p = &dup_head;

  for (; head; head = head->next)
    {
      *p = xzalloc (sizeof (struct kvx_bare_so_list));
      memcpy (*p, head, sizeof (struct kvx_bare_so_list));
      (*p)->next = NULL;
      p = &(*p)->next;
    }

  return dup_head;
}

static void
kvx_bare_solib_free_chain (struct so_list *head)
{
  struct so_list *p;

  while (head)
    {
      p = head;
      head = head->next;

      kvx_bare_solib_free_so (p);
      xfree (p);
    }
}

/* Build a list of currently loaded shared objects.  See solib-svr4.c */
static struct so_list *
kvx_bare_solib_current_sos (void)
{
  struct kvx_bare_solib_info *info;
  struct mppa_dl_debug_map_s crt_debug_map;
  uint64_t crt_debug_map_addr, is_valid;
  char crt_file_name[SO_NAME_MAX_PATH_SIZE];
  struct kvx_bare_so_list *sl_kvx;
  struct so_list *head = NULL, **ptail = &head;
  const char *cluster_name = NULL;

  info = kvx_bare_solib_get_info ();
  if (!info->brk || !info->head_addr || !info->valid_addr)
    {
      kvx_bare_solib_load_debug_info ();
      if (!info->brk || !info->head_addr || !info->valid_addr)
	return NULL;
    }

  if (ptid_equal (inferior_ptid, null_ptid))
    return NULL;
  cluster_name
    = get_cluster_name (find_inferior_pid (ptid_get_pid (inferior_ptid)));

  if (target_read_memory (info->valid_addr, (gdb_byte *) &is_valid,
			  sizeof (is_valid)))
    return NULL;

  if (!is_valid)
    {
      if (!info->last_head)
	return NULL;

      return kvx_bare_solib_dup_chain (info->last_head);
    }
  kvx_bare_solib_free_chain (info->last_head);

  if (target_read_memory (info->head_addr, (gdb_byte *) &crt_debug_map_addr,
			  sizeof (crt_debug_map_addr)))
    return NULL;
  for (; crt_debug_map_addr; crt_debug_map_addr = crt_debug_map.next)
    {
      if (target_read_memory (crt_debug_map_addr, (gdb_byte *) &crt_debug_map,
			      sizeof (struct mppa_dl_debug_map_s)))
	break;

      if (!crt_debug_map.file_name || !crt_debug_map.file_name_len)
	{
	  if (!info->no_name_shown)
	    {
	      printf_unfiltered ("%s loaded a library without name.\n"
				 "Please add -Wl,-soname=<lib_file.so> to the "
				 "library link flags.\n",
				 cluster_name);
	      info->no_name_shown = 1;
	    }
	  continue;
	}

      if (crt_debug_map.file_name_len >= SO_NAME_MAX_PATH_SIZE)
	continue;

      if (target_read_memory (crt_debug_map.file_name,
			      (gdb_byte *) crt_file_name,
			      crt_debug_map.file_name_len))
	continue;

      sl_kvx = xzalloc (sizeof (struct kvx_bare_so_list));
      sl_kvx->li.lm_addr = crt_debug_map.load_addr;
      sl_kvx->sl.lm_info = &sl_kvx->li;
      strncpy (sl_kvx->sl.so_original_name, crt_file_name,
	       SO_NAME_MAX_PATH_SIZE);
      strncpy (sl_kvx->sl.so_name, crt_file_name, SO_NAME_MAX_PATH_SIZE);

      *ptail = &sl_kvx->sl;
      ptail = &(*ptail)->next;
    }

  info->last_head = head;

  return kvx_bare_solib_dup_chain (head);
}

/* Return 1 if PC lies in the dynamic symbol resolution code of the run time
 * loader */
static int
kvx_bare_solib_in_dynsym_resolve_code (CORE_ADDR pc)
{
  return 0;
}

static struct block_symbol
kvx_bare_solib_lookup_lib_symbol (struct objfile *objfile, const char *name,
				 const domain_enum domain)
{
  return (struct block_symbol){NULL, NULL};
}

/*  Not used */
static int
kvx_bare_solib_open_symbol_file_object (void *from_ttyp)
{
  return 0;
}

extern initialize_file_ftype _initialize_kvx_bare_solib;

void
_initialize_kvx_bare_solib (void)
{
  kvx_bare_solib_pspace_data = register_program_space_data_with_cleanup (
    NULL, kvx_bare_solib_pspace_data_cleanup);

  kvx_bare_solib_ops.relocate_section_addresses
    = kvx_bare_solib_relocate_section_addresses;
  kvx_bare_solib_ops.free_so = kvx_bare_solib_free_so;
  kvx_bare_solib_ops.clear_solib = kvx_bare_solib_clear_solib;
  kvx_bare_solib_ops.solib_create_inferior_hook
    = kvx_bare_solib_solib_create_inferior_hook;
  kvx_bare_solib_ops.special_symbol_handling
    = kvx_bare_solib_special_symbol_handling;
  kvx_bare_solib_ops.current_sos = kvx_bare_solib_current_sos;
  kvx_bare_solib_ops.open_symbol_file_object
    = kvx_bare_solib_open_symbol_file_object;
  kvx_bare_solib_ops.in_dynsym_resolve_code
    = kvx_bare_solib_in_dynsym_resolve_code;
  kvx_bare_solib_ops.lookup_lib_global_symbol = kvx_bare_solib_lookup_lib_symbol;
  kvx_bare_solib_ops.bfd_open = solib_bfd_open;
}
