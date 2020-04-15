/* Target-dependent code for the Kalray KVX for GDB, the GNU debugger.

   Copyright (C) 2020 Kalray

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>. */

#include "defs.h"

#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

#include "kvx-common-tdep.h"
#include "osabi.h"
#include "linux-tdep.h"
#include "solib-svr4.h"
#include "symtab.h"
#include "gdbthread.h"
#include "top.h"
#include "gdbarch.h"
#include "target-descriptions.h"
#include "frame-unwind.h"
#include "arch-utils.h"
#include "observer.h"
#include "cli/cli-cmds.h"
#include "remote.h"
#include "cli/cli-decode.h"
#include "features/kvx-linux.c"

extern int remote_hw_breakpoint_limit;
extern int remote_hw_watchpoint_limit;

char *sysroot_path = NULL;
uint32_t breakpoint_linux = 0x0;

static const gdb_byte *
kvx_bare_breakpoint_from_pc (struct gdbarch *gdbarch, CORE_ADDR *pc, int *len)
{
  *len = 4;
  return (gdb_byte *) &breakpoint_linux;
}

static struct gdbarch *
kvx_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;
  const struct target_desc *tdesc = info.target_desc;
  struct tdesc_arch_data *tdesc_data = NULL;
  int i;
  unsigned long mach;
  int has_lc = -1, has_le = -1, has_ls = -1;
  int has_ra = -1, has_pc = -1, has_sp = -1, has_local = -1;

  static const char kvx_lc_name[] = "lc";
  static const char kvx_le_name[] = "le";
  static const char kvx_ls_name[] = "ls";
  static const char kvx_ra_name[] = "ra";
  static const char kvx_local_name[] = "r13";

  const char *pc_name;
  const char *sp_name;

  /* If there is already a candidate, use it. */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return arches->gdbarch;
  if (tdesc == NULL)
    tdesc = tdesc_kvx_linux;

  tdep = xzalloc (sizeof (struct gdbarch_tdep));
  gdbarch = gdbarch_alloc (&info, tdep);

  pc_name = kv3_pc_name (gdbarch);
  sp_name = kv3_sp_name (gdbarch);
  mach = gdbarch_bfd_arch_info (gdbarch)->mach;

  /* This could (should?) be extracted from MDS */
  set_gdbarch_short_bit (gdbarch, 16);
  set_gdbarch_int_bit (gdbarch, 32);
  set_gdbarch_long_bit (gdbarch, 64);
  set_gdbarch_long_long_bit (gdbarch, 64);
  set_gdbarch_float_bit (gdbarch, 32);
  set_gdbarch_double_bit (gdbarch, 64);
  set_gdbarch_long_double_bit (gdbarch, 64);
  set_gdbarch_ptr_bit (gdbarch, 64);

  /* Get the kvx target description from INFO. */
  if (tdesc_has_registers (tdesc))
    {
      set_gdbarch_num_regs (gdbarch, 0);
      tdesc_data = tdesc_data_alloc ();
      tdesc_use_registers (gdbarch, tdesc, tdesc_data);

      for (i = 0; i < gdbarch_num_regs (gdbarch); ++i)
	{
	  if (strcmp (tdesc_register_name (gdbarch, i), kvx_lc_name) == 0)
	    has_lc = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_le_name) == 0)
	    has_le = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_ls_name) == 0)
	    has_ls = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_ra_name) == 0)
	    has_ra = i;
	  if (strcmp (tdesc_register_name (gdbarch, i), pc_name) == 0)
	    has_pc = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), sp_name) == 0)
	    has_sp = i;
	  else if (strcmp (tdesc_register_name (gdbarch, i), kvx_local_name)
		   == 0)
	    has_local = i;
	}

      if (has_lc < 0)
	error ("There's no '%s' register!", kvx_lc_name);
      if (has_le < 0)
	error ("There's no '%s' register!", kvx_le_name);
      if (has_ls < 0)
	error ("There's no '%s' register!", kvx_ls_name);
      if (has_ra < 0)
	error ("There's no '%s' register!", kvx_ra_name);
      if (has_pc < 0)
	error ("There's no '%s' register!", pc_name);
      if (has_sp < 0)
	error ("There's no '%s' register!", sp_name);
      if (has_local < 0)
	error ("There's no '%s' register!", kvx_local_name);

      tdep->lc_regnum = has_lc;
      tdep->le_regnum = has_le;
      tdep->ls_regnum = has_ls;
      tdep->ra_regnum = has_ra;
      tdep->local_regnum = has_local;
      set_gdbarch_pc_regnum (gdbarch, has_pc);
      set_gdbarch_sp_regnum (gdbarch, has_sp);

      tdep->uint256 = arch_integer_type (gdbarch, 256, 0, "uint256_t");
      tdep->uint512 = arch_integer_type (gdbarch, 512, 0, "uint512_t");
      tdep->uint1024 = arch_integer_type (gdbarch, 1024, 0, "uint1024_t");
    }
  else
    {
      set_gdbarch_num_regs (gdbarch, 1);
      set_gdbarch_register_name (gdbarch, kvx_dummy_register_name);
      set_gdbarch_register_type (gdbarch, kvx_dummy_register_type);
    }

  set_gdbarch_num_pseudo_regs (gdbarch, kv3_num_pseudos (gdbarch));

  set_tdesc_pseudo_register_name (gdbarch, kv3_pseudo_register_name);
  set_tdesc_pseudo_register_type (gdbarch, kv3_pseudo_register_type);
  set_tdesc_pseudo_register_reggroup_p (gdbarch,
					kv3_pseudo_register_reggroup_p);

  set_gdbarch_pseudo_register_read (gdbarch, kv3_pseudo_register_read);
  set_gdbarch_pseudo_register_write (gdbarch, kv3_pseudo_register_write);
  set_gdbarch_dwarf2_reg_to_regnum (gdbarch, kv3_dwarf2_reg_to_regnum);
  dwarf2_frame_set_init_reg (gdbarch, kvx_dwarf2_frame_init_reg);

  set_gdbarch_return_value (gdbarch, kvx_return_value);
  set_gdbarch_push_dummy_call (gdbarch, kvx_push_dummy_call);
  set_gdbarch_dummy_id (gdbarch, kvx_dummy_id);

  set_gdbarch_skip_prologue (gdbarch, kvx_skip_prologue);
  set_gdbarch_unwind_pc (gdbarch, kvx_unwind_pc);
  dwarf2_append_unwinders (gdbarch);
  frame_unwind_append_unwinder (gdbarch, &kvx_frame_unwind);

  set_gdbarch_breakpoint_from_pc (gdbarch, kvx_bare_breakpoint_from_pc);
  set_gdbarch_adjust_breakpoint_address (gdbarch, kvx_adjust_breakpoint_address);
  /* Settings that should be unnecessary. */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  set_gdbarch_print_insn (gdbarch, kvx_print_insn);

  set_gdbarch_max_insn_length (gdbarch, 8 * 4);

  set_gdbarch_get_longjmp_target (gdbarch, kvx_get_longjmp_target);

  if (tdesc_has_registers (tdesc) && info.bfd_arch_info)
    {
      unsigned long mach = info.bfd_arch_info->mach;

      /* Hook in the ABI-specific overrides, if they have been registered. */
      if (mach == bfd_mach_kv3_1_64 || mach == bfd_mach_kv3_1_usr)
	gdbarch_init_osabi (info, gdbarch);
    }

  return gdbarch;
}

#ifndef __KVX__
static void
attach_user_command (char *args, int from_tty)
{
  static const char *syntax
    = "Syntax: attach-user <comm> [<path_in_initrd_kvx_linux_program>]\n";
  char *file, *comm, *kvx_comm, *pargs, cmd[PATH_MAX + 50], file_dir[PATH_MAX];
  struct stat vstat;
  int ret;

  if (!ptid_equal (inferior_ptid, null_ptid))
    {
      fprintf (stderr, "Gdb already attached!\n");
      return;
    }

  pargs = args;
  kvx_comm = comm = extract_arg (&pargs);
  if (!comm)
    {
      fprintf (stderr, "Error: the comm was not specified!\n%s", syntax);
      return;
    }
  file = extract_arg (&pargs);
  if (!file)
    {
      fprintf (stderr, "Error: the KVX Linux program was not specified!\n%s",
	       syntax);
      return;
    }
  if (stat (file, &vstat) || S_ISDIR (vstat.st_mode)
      || realpath (file, file_dir) == NULL)
    {
      fprintf (stderr, "Error: Cannot find file %s!\n", file);
      return;
    }

  execute_command ("set pagination off", 0);

  // set file
  sprintf (cmd, "file %s", file);
  execute_command (cmd, 0);

  // set sysroot
  if (sysroot_path)
    {
      free (sysroot_path);
      sysroot_path = NULL;
    }
  strcpy (file_dir, dirname (file_dir));
  while (*file_dir && (file_dir[0] != '/' || file_dir[1] != 0))
    {
      sprintf (cmd, "%s/etc/inittab", file_dir);
      if (stat (cmd, &vstat) == 0)
	{
	  // found the root of kvx filesystem
	  sysroot_path = strdup (file_dir);
	  sprintf (cmd, "set sysroot %s", file_dir);
	  fprintf (stderr, "Set sysroot to %s\n", file_dir);
	  execute_command (cmd, 0);
	  break;
	}

      strcpy (file_dir, dirname (file_dir));
    }

  ret = 0;
  TRY
  {
    sprintf (cmd, "target remote %s", comm);
    execute_command (cmd, 0);
  }
  CATCH (ex, RETURN_MASK_ALL)
  {
    printf ("Error while trying to connect (%s).\n", ex.message ?: "");
    ret = -1;
  }
  END_CATCH

  if (ret)
    return;

  signal_stop_update (GDB_SIGNAL_REALTIME_32, 0);
  signal_print_update (GDB_SIGNAL_REALTIME_32, 0);
  signal_pass_update (GDB_SIGNAL_REALTIME_32, 1);
  signal_stop_update (GDB_SIGNAL_REALTIME_33, 0);
  signal_print_update (GDB_SIGNAL_REALTIME_33, 0);
  signal_pass_update (GDB_SIGNAL_REALTIME_33, 1);

  fprintf (stderr, "Attached to KVX linux user debug using %s.\n", kvx_comm);
}
#endif

static void
kvx_inferior_created (struct target_ops *target, int from_tty)
{
  kvx_current_arch = KVX_NUM_ARCHES;
}

/* OS specific initialization of gdbarch. */
static void
kvx_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  linux_init_abi (info, gdbarch);

  set_solib_svr4_fetch_link_map_offsets (gdbarch,
					 svr4_lp64_fetch_link_map_offsets);

  /* Enable TLS support. */
  set_gdbarch_fetch_tls_load_module_address (gdbarch,
					     svr4_fetch_objfile_link_map);
}

static void
add_kvx_commands (void)
{
#ifndef __KVX__
  add_com (
    "attach-user", class_run, attach_user_command,
    "Connect to gdbserver running on MPPA.\n"
    "Usage is 'attach-user <comm> [<path_in_initrd_kvx_linux_program>]'.");
#endif
}

extern initialize_file_ftype _initialize_kvx_linux_tdep;
void
_initialize_kvx_linux_tdep (void)
{
  remote_hw_breakpoint_limit = 2;
  remote_hw_watchpoint_limit = 1;

  gdbarch_register_osabi (bfd_arch_kvx, bfd_mach_kv3_1_64, GDB_OSABI_LINUX,
			  kvx_linux_init_abi);
  gdbarch_register_osabi (bfd_arch_kvx, bfd_mach_kv3_1_usr, GDB_OSABI_LINUX,
			  kvx_linux_init_abi);
  add_kvx_commands ();
}

extern initialize_file_ftype _initialize_kvx_tdep;
void
_initialize_kvx_tdep (void)
{
  kvx_look_for_insns ();
  gdbarch_register (bfd_arch_kvx, kvx_gdbarch_init, NULL);
  observer_attach_inferior_created (kvx_inferior_created);

  initialize_tdesc_kvx_linux ();
}
