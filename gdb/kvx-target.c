/* Target ops to connect to the KVX simulator.

   Copyright (C) 2010, Kalray
 */

#include "config.h"
#include "defs.h"
#include "target.h"

#include <assert.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>

#include "environ.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "gdbthread.h"
#include "inferior.h"
#include "infrun.h"
#include "observer.h"
#include "osdata.h"
#include "main.h"
#include "symfile.h"
#include "top.h"
#include "arch-utils.h"
#include "cli/cli-decode.h"
#include "cli/cli-setshow.h"
#include "cli/cli-utils.h"
#include "event-top.h"
#include "regcache.h"
#include "event-loop.h"
#include "location.h"
#include "regcache.h"
#include "objfiles.h"
#include "thread-fsm.h"

#include "elf/kv3.h"
#include "elf-bfd.h"
#include "elf/kv3.h"
#include "kvx-target.h"
#include "kvx-exception-info.h"
#include "kvx-dump-tlb.h"
#include "solib-kvx-bare.h"

#ifndef MAX
#define MAX(a, b) ((a < b) ? (b) : (a))
#endif

#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY (s)

extern int remote_hw_breakpoint_limit;
extern int remote_hw_watchpoint_limit;
extern int print_stopped_thread;

static cmd_cfunc_ftype *real_run_command;
static struct target_ops kvx_target_ops;
static char *da_options = NULL;

static struct cmd_list_element *kalray_set_cmdlist;
static struct cmd_list_element *kalray_set_traps_cmdlist;
static struct cmd_list_element *kalray_show_traps_cmdlist;
static struct cmd_list_element *kalray_show_cmdlist;
static const char *simulation_vehicles[] = {"kvx-cluster", NULL};
static const char *simulation_vehicle;
static const char *sopts_cluster_stop_all[]
  = {"none", "jtag", "break_mask", NULL};
static const char *sopts_cluster_debug_ring[] = {"all", "current", NULL};
static const char *sopt_cluster_debug_ring;
static const char *sopt_cluster_stop_all;
static const char *traps_name[] = {"opcode",
				   "misalign",
				   "psys",
				   "dsys",
				   "double_ecc",
				   "single_ecc",
				   "nomap",
				   "protection",
				   "write_to_clean",
				   "atomic_to_clean",
				   "double_exception",
				   "vsfr",
				   "pl_owner"};
static int no_traps_name = sizeof (traps_name) / sizeof (traps_name[0]);
char cjtag_over_iss = 'n';
int opt_break_on_spawn = 0;
int opt_cont_os_init_done = 1;
int opt_cluster_stop_all = 0;
int in_info_thread = 0;
int in_attach_mppa = 0;
int new_attach_requested = 0;
static pid_t server_pid;
int after_first_resume = 0;
int opt_trap = 0;
int wait_os_init_done = 0;

static const struct inferior_data *kvx_attached_inf_data;

static void kvx_target_create_inferior (struct target_ops *ops, char *exec_file,
				       char *args, char **env, int from_tty);

static struct inferior_data *
mppa_init_inferior_data (struct inferior *inf)
{
  struct inferior_data *data = xcalloc (1, sizeof (struct inferior_data));
  char *endptr;
  struct osdata *osdata;
  struct osdata_item *item;
  int ix_items;

  data->cluster_break_on_spawn = 0;
  set_inferior_data (inf, kvx_attached_inf_data, data);

  /* Cluster name */
  data->cluster = "Cluster ?";

  osdata = get_osdata (NULL);

  for (ix_items = 0; VEC_iterate (osdata_item_s, osdata->items, ix_items, item);
       ix_items++)
    {
      unsigned long pid
	= strtoul (get_osdata_column (item, "pid"), &endptr, 10);
      const char *cluster = get_osdata_column (item, "cluster");
      const char *unified = get_osdata_column (item, "unified");

      if (pid != inf->pid)
	continue;

      data->cluster = cluster;
      data->unified = unified && !strcmp (unified, "yes");
    }

  return data;
}

struct inferior_data *
mppa_inferior_data (struct inferior *inf)
{
  struct inferior_data *data = inferior_data (inf, kvx_attached_inf_data);

  if (!data)
    data = mppa_init_inferior_data (inf);

  return data;
}

const char *
get_cluster_name (struct inferior *inf)
{
  struct inferior_data *data = mppa_inferior_data (inf);
  if (!data || !data->cluster)
    return "[cluster unknown]";

  return data->cluster;
}

static void
kvx_push_arch_stratum (struct target_ops *ops, int from_tty)
{
  if (find_target_beneath (&current_target) != &kvx_target_ops)
    push_target (&kvx_target_ops);
}

static void
kvx_target_new_thread (struct thread_info *t)
{
  if (!ptid_equal (inferior_ptid, null_ptid))
    current_inferior ()->control.stop_soon = STOP_QUIETLY;

  kvx_push_arch_stratum (NULL, 0);
}

static void
kvx_target_mourn_inferior (struct target_ops *target)
{
  struct target_ops *remote_target = find_target_beneath (target);

  gdb_assert (target == &kvx_target_ops);
  remote_target->to_mourn_inferior (remote_target);
  /* Force disconnect even if we are in extended mode */
  unpush_target (remote_target);
  unpush_target (&kvx_target_ops);

  if (server_pid)
    {
      kill (server_pid, 9);
      server_pid = 0;
    }
}

static int
kvx_region_ok_for_hw_watchpoint (struct target_ops *ops, CORE_ADDR addr, int len)
{
  return 1;
}

static void
kvx_target_open (const char *name, int from_tty)
{
}

static void
kvx_target_close (struct target_ops *ops)
{
}

static char *
mppa_pid_to_str (struct target_ops *ops, ptid_t ptid)
{
  struct thread_info *ti = find_thread_ptid (ptid);
  struct target_ops *remote_target = find_target_beneath (ops);

  if (ti)
    {
      const char *name = remote_target->to_thread_name (ops, ti);

      if (in_info_thread)
	{
	  in_info_thread = 0;
	  return (char *) "Thread";
	}

      return (char *) name;
    }

  return find_target_beneath (ops)->to_pid_to_str (find_target_beneath (ops),
						   ptid);
}

static char *
mppa_threads_extra_info (struct target_ops *ops, struct thread_info *tp)
{
  return NULL;
}

static void
kvx_target_attach (struct target_ops *ops, char *args, int from_tty)
{
  const char tar_remote_str[] = "target extended-remote";
  struct observer *new_thread_observer;
  int print_thread_events_save = print_thread_events;
  char *host, *port, *tar_remote_cmd;

  if (!args)
    args = "";

  port = strchr (args, ':');
  if (port)
    {
      *port = 0;
      port++;
      host = args;
    }
  else
    {
      port = args;
      host = "";
    }

  tar_remote_cmd
    = alloca (strlen (host) + strlen (port) + strlen (tar_remote_str) + 4);
  parse_pid_to_attach (port);

  print_thread_events = 0;
  sprintf (tar_remote_cmd, "%s %s:%s", tar_remote_str, host, port);

  /* Load the real debug target by issuing 'target remote'. Of
     course things aren't that simple because it's not meant to be
     used that way. One issue is that connecting to the gdb_stub
     will emit MI stop notifications and will print the initial
     frame at the connection point. The observer
     makes infrun believe that we're in a series of steps and thus
     inhibits the emission of the new_thread observer notification
     that prints the initial MI *stopped message.
   */
  new_thread_observer = observer_attach_new_thread (kvx_target_new_thread);
  async_disable_stdin ();
  TRY { execute_command (tar_remote_cmd, 0); }

  CATCH (ex, RETURN_MASK_ALL)
  {
    observer_detach_new_thread (new_thread_observer);
    print_thread_events = print_thread_events_save;
    throw_exception (ex);
  }
  END_CATCH

  /* We need to tell the debugger to fake a synchronous
     command. This has already been done at the upper level when the
     main loop executes the "run" command, but the execute_command
     call we did just above reseted to async handling when it
     terminated.
   */
  async_disable_stdin ();
  kvx_push_arch_stratum (NULL, 0);

  /* Remove hacks*/
  observer_detach_new_thread (new_thread_observer);
  current_inferior ()->control.stop_soon = NO_STOP_QUIETLY;
  print_thread_events = print_thread_events_save;
}

static void
kvx_target_create_inferior (struct target_ops *ops, char *exec_file, char *args,
			   char **env, int from_tty)
{
  char set_non_stop_cmd[] = "set non-stop";
  char set_pagination_off_cmd[] = "set pagination off";
  char **argv_args = gdb_buildargv (args);
  char **da_args = gdb_buildargv (da_options);
  char **stub_args;
  char **arg;
  int nb_args = 0, nb_da_args = 0;
  int pipefds[2];
  int no_march = 0;
  int no_mcluster = 0;
  int core;
  int argidx = 0;
  int saved_async_execution = !sync_execution;

  if (exec_file == NULL)
    error (_ ("No executable file specified.\nUse the \"file\" or "
	      "\"exec-file\" command."));

  kvx_push_arch_stratum (NULL, 0);
  execute_command (set_non_stop_cmd, 0);
  execute_command (set_pagination_off_cmd, 0);
  remote_hw_breakpoint_limit = 1;
  remote_hw_watchpoint_limit = 1;

  arg = argv_args;
  while (arg && *arg++)
    nb_args++;
  if (nb_args && !*argv_args[nb_args - 1])
    nb_args--;

  arg = da_args;
  while (arg && *arg++)
    nb_da_args++;

  stub_args = xmalloc ((nb_args + nb_da_args + 7) * sizeof (char *));
  stub_args[argidx++] = (char *) simulation_vehicle;

  core = (elf_elfheader (exec_bfd)->e_flags & ELF_KVX_CORE_MASK);

  if (nb_da_args && strlen (da_options))
    {
      arg = da_args;
      while (*arg)
	{
	  if (strncmp (*arg, "--mcluster=", 11) == 0)
	    no_mcluster = 1;
	  if (strncmp (*arg, "--march=", 8) == 0)
	    no_march = 1;

	  stub_args[argidx++] = *arg++;
	}
    }

  if (!no_march)
    {
      switch (core)
	{
	case ELF_KVX_CORE_KV3_1:
	  stub_args[argidx++] = "--march=coolidge";
	  break;
	default:
	  error (_ ("The KVX binary is compiled for an unknown core."));
	}
    }

  stub_args[argidx++] = "--gdb";
  stub_args[argidx++] = "--";
  stub_args[argidx++] = exec_file;
  if (nb_args)
    {
      memcpy (stub_args + argidx, argv_args, nb_args * sizeof (char *));
      argidx += nb_args;
    }
  stub_args[argidx++] = NULL;

  /* Check that we didn't overflow the allocation above. */
  gdb_assert (argidx <= nb_args + nb_da_args + 7);

  if (server_pid != 0)
    {
      kill (server_pid, 9);
      waitpid (server_pid, NULL, 0);
    }

  pipe (pipefds);
  server_pid = fork ();

  if (server_pid < 0)
    error ("Couldn't fork to launch the server.");

  if (server_pid == 0)
    {
      char path[PATH_MAX];
      char tmp[PATH_MAX] = {0};
      char *dir;

      close (pipefds[0]);
      dup2 (pipefds[1], 500);
      close (pipefds[1]);

      setsid ();

      /* Child */
      if (env)
	environ = env;
      execvp (simulation_vehicle, stub_args);

      /* Not in PATH */
      if (readlink ("/proc/self/exe", tmp, PATH_MAX) != -1)
	{
	  dir = dirname (tmp);
	  snprintf (path, PATH_MAX, "%s/%s", dir, simulation_vehicle);
	  execvp (path, stub_args);
	}

      printf_unfiltered ("Could not find %s in you PATH\n", simulation_vehicle);
      exit (1);
    }
  else
    {
      int port;
      char cmd_port[10];

      close (pipefds[1]);
      read (pipefds[0], &port, sizeof (port));
      close (pipefds[0]);

      sprintf (cmd_port, "%i", port);
      print_stopped_thread = 0;
      kvx_target_attach (ops, cmd_port, from_tty);
      print_stopped_thread = 1;
      if (saved_async_execution)
	async_enable_stdin ();
    }
}

static int
mppa_mark_clusters_booted (struct inferior *inf, void *_ptid)
{
  struct thread_info *thread;
  ptid_t *ptid = _ptid;

  if (!inf->pid)
    return 0;

  thread = any_live_thread_of_process (inf->pid);

  /* Newer GDBs mark the thread as running before passing it to
     target_resume. However, if we are resuming the thread, it must
     have been stooped before... */
  if (thread && (is_stopped (thread->ptid) || ptid_match (thread->ptid, *ptid)))
    mppa_inferior_data (inf)->booted = 1;

  return 0;
}

static void
mppa_target_resume (struct target_ops *ops, ptid_t ptid, int step,
		    enum gdb_signal siggnal)
{
  struct target_ops *remote_target = find_target_beneath (ops);

  if (!after_first_resume && !wait_os_init_done)
    {
      after_first_resume = 1;
      iterate_over_inferiors (mppa_mark_clusters_booted, &ptid);
    }

  return remote_target->to_resume (remote_target, ptid, step, siggnal);
}

static void
kvx_change_file (const char *file_path, const char *cluster_name)
{
  struct stat st;
  if (stat (file_path, &st))
    {
      printf ("Cannot stat KVX executable file %s\n", file_path);
      return;
    }

  if (st.st_mode & S_IFDIR)
    {
      printf ("%s is a directory, not a KVX executable!\n", file_path);
      return;
    }

  cjtag_over_iss = get_jtag_over_iss ();
  TRY
  {
    exec_file_attach ((char *) file_path, 0);
    symbol_file_add_main (file_path, 0);
    if (cjtag_over_iss == 'i')
      {
	const char *rel_debug_handlers = "/" STRINGIFY (
	  INSTALL_LIB) "/kalray-oce/kv3/v1_node_debug_handlers.u";
	char path[1024], *dn;
	int sz = readlink ("/proc/self/exe", path, sizeof (path) - 1);
	path[sz] = 0;
	dn = dirname (path);
	if (dn != path)
	  strcpy (path, dn);
	dn = dirname (path);
	if (dn != path)
	  sprintf (path, "%s%s", dn, rel_debug_handlers);
	else
	  strcat (path, rel_debug_handlers);

	if (access (path, R_OK))
	  fprintf (stderr, "Warning: cannot find the debug handlers at %s\n",
		   path);
	else
	  {
	    if (!cluster_name)
	      cluster_name = "[unknown cluster name]";
	    fprintf (stderr, "Info: adding %s debug handler symbols from %s\n",
		     cluster_name, path);
	    symbol_file_add (path, 0, NULL, OBJF_USERLOADED | OBJF_SHARED);
	  }
      }

    kvx_bare_solib_load_debug_info ();
  }
  CATCH (ex, RETURN_MASK_ALL)
  {
    // exception
  }
  END_CATCH
}

static void
kvx_new_inferiors_cb (void *arg)
{
  struct osdata_item *item;
  int ix_items, found_new;
  char attach_cmd[25], *endptr;
  struct inferior *inf;
  struct osdata *osdata;
  struct cleanup *old_chain;

  do
    {
      in_attach_mppa = 1;
      old_chain = make_cleanup (null_cleanup, NULL);
      new_attach_requested = 0;
      found_new = 0;

      save_current_space_and_thread ();

      osdata = get_osdata (NULL);
      for (ix_items = 0;
	   VEC_iterate (osdata_item_s, osdata->items, ix_items, item);
	   ix_items++)
	{
	  unsigned long pid
	    = strtoul (get_osdata_column (item, "pid"), &endptr, 10);

	  if (find_inferior_pid (pid))
	    continue;

	  found_new = 1;
	  inf = add_inferior_with_spaces ();
	  set_current_inferior (inf);
	  switch_to_thread (null_ptid);
	  set_current_program_space (inf->pspace);
	  sprintf (attach_cmd, "attach %li&", pid);
	  execute_command (attach_cmd, 0);
	  inf->control.stop_soon = NO_STOP_QUIETLY;
	  inf->removable = 1;
	}

      do_cleanups (old_chain);
      in_attach_mppa = 0;
    }
  while (new_attach_requested || found_new);
}

void custom_notification_cb (char *arg);
void
custom_notification_cb (char *arg)
{
  if (remote_timeout < 10)
    remote_timeout = 10;
  if (in_attach_mppa)
    new_attach_requested = 1;
  else
    create_timer (0, &kvx_new_inferiors_cb, NULL);
}

static void
change_thread_cb (void *p)
{
  struct inferior *inf = (struct inferior *) p;
  struct thread_info *th;

  if (is_stopped (inferior_ptid))
    return;

  th = any_live_thread_of_process (inf->pid);
  if (!is_stopped (th->ptid))
    return;

  switch_to_thread (th->ptid);
}

static int
os_init_done_fsm_should_stop (struct thread_fsm *self)
{
  struct regcache *rc = get_thread_regcache (inferior_ptid);

  return regcache_read_pc (rc) != 0;
}

static void
os_init_done_fsm_clean_up (struct thread_fsm *self)
{
}

static enum async_reply_reason
os_init_done_fsm_async_reply_reason (struct thread_fsm *self)
{
  return EXEC_ASYNC_LOCATION_REACHED;
}

static struct thread_fsm_ops os_init_done_fsm_ops = {
  NULL,			      /* dtor */
  &os_init_done_fsm_clean_up, /* clean_up */
  os_init_done_fsm_should_stop,
  NULL, /* return_value */
  os_init_done_fsm_async_reply_reason,
};

static ptid_t
kvx_target_wait (struct target_ops *target, ptid_t ptid,
		struct target_waitstatus *status, int options)
{
  struct target_ops *remote_target = find_target_beneath (target);
  ptid_t res;
  struct inferior *inferior;
  struct inferior_data *data;
  struct regcache *rc;
  int ix_items;

  res = remote_target->to_wait (remote_target, ptid, status, options);

  inferior = find_inferior_pid (ptid_get_pid (res));
  if (!inferior || !find_thread_ptid (res))
    return res;

  data = mppa_inferior_data (inferior);
  if (!data->booted)
    {
      char *endptr;
      struct osdata *osdata;
      struct osdata_item *last;
      struct osdata_item *item;

      osdata = get_osdata (NULL);

      for (ix_items = 0;
	   VEC_iterate (osdata_item_s, osdata->items, ix_items, item);
	   ix_items++)
	{
	  unsigned long pid
	    = strtoul (get_osdata_column (item, "pid"), &endptr, 10);
	  const char *file = get_osdata_column (item, "command");
	  const char *cluster = get_osdata_column (item, "cluster");
	  struct inferior_data *data;

	  if (pid != ptid_get_pid (res))
	    continue;

	  data = mppa_inferior_data (inferior);
	  data->booted = 1;

	  if (file && file[0] != 0 && !data->sym_file_loaded)
	    {
	      ptid_t save_ptid = inferior_ptid;
	      data->sym_file_loaded = 1;
	      switch_to_thread (res);
	      kvx_change_file (file, data->cluster);
	      switch_to_thread (save_ptid);
	    }

	  if (!after_first_resume)
	    status->value.sig = GDB_SIGNAL_TRAP;
	  else
	    {
	      if (data->cluster_break_on_spawn)
		{
		  status->kind = TARGET_WAITKIND_STOPPED;
		  status->value.sig = GDB_SIGNAL_TRAP;
		}
	      else
		status->kind = TARGET_WAITKIND_SPURIOUS;
	    }
	  break;
	}
    }

  // restore the first syllab of gdb_os_init_done
  if (data->os_init_done)
    {
      int crt_os_init_done = data->os_init_done;

      wait_os_init_done--;
      data->os_init_done = 0;
      rc = get_thread_regcache (res);
      if (regcache_read_pc (rc) == data->gdb_os_init_done_addr)
	{
	  ptid_t save_ptid = inferior_ptid;

	  switch_to_thread (res);
	  write_memory (data->gdb_os_init_done_addr,
			(bfd_byte *) &data->saved_os_init_done_syl, 4);
	  switch_to_thread (save_ptid);
	}

      // change the thread to stopped one after continue to
      // gdb_os_init_done
      if (crt_os_init_done == 1)
	create_timer (0, &change_thread_cb, inferior);
    }
  else
    {
      // for the first inferior, the continue to os_init_done
      // is done from kvx_inferior_created
      if (!after_first_resume && opt_cont_os_init_done
	  && ptid_get_pid (res) != 1)
	{
	  struct thread_info *tp;
	  ptid_t save_ptid = inferior_ptid;

	  switch_to_thread (res);

	  tp = inferior_thread ();
	  if (tp && !tp->thread_fsm && kvx_prepare_os_init_done ())
	    {
	      tp->thread_fsm = XCNEW (struct thread_fsm);
	      thread_fsm_ctor (tp->thread_fsm, &os_init_done_fsm_ops);
	    }

	  switch_to_thread (save_ptid);
	}
    }

  return res;
}

static void
mppa_attach (struct target_ops *ops, const char *args, int from_tty)
{
  struct target_ops *remote_target,
    *kvx_ops = find_target_beneath (&current_target);

  if (kvx_ops != &kvx_target_ops)
    error ("Don't know how to attach.  Try \"help target\".");

  remote_target = find_target_beneath (&kvx_target_ops);
  return remote_target->to_attach (remote_target, args, from_tty);
}

static int
kvx_target_can_run (struct target_ops *ops)
{
  return 1;
}

static enum target_xfer_status
kvx_target_xfer_partial (struct target_ops *ops, enum target_object object,
			const char *annex, gdb_byte *readbuf,
			const gdb_byte *writebuf, ULONGEST offset, ULONGEST len,
			ULONGEST *xfered_len)
{
  if (!ops->beneath)
    error (_ ("Don't know how to xfer.  Try \"help target\"."));
  return ops->beneath->to_xfer_partial (ops->beneath, object, annex, readbuf,
					writebuf, offset, len, xfered_len);
}

static int
kvx_target_supports_non_stop (struct target_ops *ops)
{
  return 1;
}

static int
kvx_target_can_async (struct target_ops *ops)
{
  return target_async_permitted;
}

static void
kvx_fetch_registers (struct target_ops *target, struct regcache *regcache,
		    int regnum)
{
  // don't use current_inferior () & current_inferior_
  // our caller (regcache_raw_read) changes only inferior_ptid

  struct target_ops *remote_target;

  // get the registers of the current thread (CPU) in the usual way
  remote_target = find_target_beneath (target);
  remote_target->to_fetch_registers (target, regcache, regnum);

  enable_ps_v64_at_boot (regcache);
}

static void
set_kalray_cmd (char *args, int from_tty)
{
  help_list (kalray_set_cmdlist, "set kalray ", -1, gdb_stdout);
}

static void
set_kalray_traps_cmd (char *args, int from_tty)
{
  help_list (kalray_set_traps_cmdlist, "intercept-traps ", -1, gdb_stdout);
}

static void
show_kalray_traps_cmd (char *args, int from_tty)
{
  help_list (kalray_show_traps_cmdlist, "show kalray ", -1, gdb_stdout);
}

static void
show_kalray_cmd (char *args, int from_tty)
{
  help_list (kalray_show_cmdlist, "show kalray ", -1, gdb_stdout);
}

static void
set_cluster_break_on_spawn (char *args, int from_tty,
			    struct cmd_list_element *c)
{
  struct inferior *inf;
  struct inferior_data *data;

  if (ptid_equal (inferior_ptid, null_ptid))
    error (_ ("Cannot set break on reset without a selected thread."));

  inf = current_inferior ();
  data = mppa_inferior_data (inf);
  data->cluster_break_on_spawn = opt_break_on_spawn;
  send_cluster_break_on_spawn (inf, opt_break_on_spawn);
}

static void
show_cluster_break_on_spawn (struct ui_file *file, int from_tty,
			     struct cmd_list_element *c, const char *value)
{
  struct inferior_data *data;

  if (ptid_equal (inferior_ptid, null_ptid))
    {
      printf (
	_ ("Cannot show break on reset without a live selected thread.\n"));
      return;
    }

  data = mppa_inferior_data (find_inferior_pid (inferior_ptid.pid));
  fprintf_filtered (file, "The cluster break on reset is %d.\n",
		    data->cluster_break_on_spawn);
}

static void
show_cont_os_init_done (struct ui_file *file, int from_tty,
			struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, "Continue until OS initialization is done is %d.\n",
		    opt_cont_os_init_done);
}

static int
get_trap_index (const char *name)
{
  int i;

  for (i = 0; i < no_traps_name; i++)
    if (!strcmp (traps_name[i], name))
      return i;

  return -1;
}

static void
set_intercept_trap (char *args, int from_tty, struct cmd_list_element *c)
{
  struct inferior *inf;
  struct inferior_data *data;
  int trap_idx;

  trap_idx = get_trap_index (c->name);
  if (trap_idx < 0)
    error (_ ("Invalid trap name %s."), c->name);

  if (ptid_equal (inferior_ptid, null_ptid))
    error (_ ("Cannot set intercept trap without a selected thread."));

  inf = current_inferior ();
  data = mppa_inferior_data (inf);
  data->intercept_trap &= ~(1 << trap_idx);
  data->intercept_trap |= opt_trap << trap_idx;

  send_intercept_trap (inf, data->intercept_trap);
}

static void
show_intercept_trap (struct ui_file *file, int from_tty,
		     struct cmd_list_element *c, const char *value)
{
  struct inferior_data *data;
  int trap_idx;

  trap_idx = get_trap_index (c->name);
  if (trap_idx < 0)
    error (_ ("Invalid trap name %s."), c->name);

  if (ptid_equal (inferior_ptid, null_ptid))
    {
      printf (
	_ ("Cannot show intercept trap without a live selected thread.\n"));
      return;
    }

  data = mppa_inferior_data (find_inferior_pid (inferior_ptid.pid));
  fprintf_filtered (file, "Intercept %s trap is %s.\n", c->name,
		    ((data->intercept_trap >> trap_idx) & 1) ? "on" : "off");
}

static void
set_intercept_trap_mask (char *args, int from_tty, struct cmd_list_element *c)
{
  struct inferior *inf;
  struct inferior_data *data;

  if (ptid_equal (inferior_ptid, null_ptid))
    error (_ ("Cannot set intercept trap without a selected thread."));

  opt_trap &= (1 << no_traps_name) - 1;

  inf = current_inferior ();
  data = mppa_inferior_data (inf);
  data->intercept_trap = opt_trap;

  send_intercept_trap (inf, data->intercept_trap);
}

static void
show_intercept_trap_mask (struct ui_file *file, int from_tty,
			  struct cmd_list_element *c, const char *value)
{
  struct inferior_data *data;

  if (ptid_equal (inferior_ptid, null_ptid))
    {
      printf (
	_ ("Cannot show intercept trap without a live selected thread.\n"));
      return;
    }

  data = mppa_inferior_data (find_inferior_pid (inferior_ptid.pid));
  fprintf_filtered (file, "Intercept trap mask is 0x%04x.\n",
		    data->intercept_trap);
}

static int
str_idx_in_opts (const char *s, const char **opts)
{
  int i;
  for (i = 0; opts[i]; i++)
    if (!strcmp (s, opts[i]))
      return i;
  return -1;
}

static void
set_cluster_stop_all (char *args, int from_tty, struct cmd_list_element *c)
{
  struct inferior *inf;
  struct inferior_data *data;

  if (ptid_equal (inferior_ptid, null_ptid))
    error (_ ("Cannot set stop all cluster CPUs without a selected thread."));

  inf = current_inferior ();
  data = mppa_inferior_data (inf);
  data->cluster_stop_all
    = str_idx_in_opts (sopt_cluster_stop_all, sopts_cluster_stop_all);
  send_cluster_stop_all (inf, data->cluster_stop_all);
}

static void
show_cluster_stop_all (struct ui_file *file, int from_tty,
		       struct cmd_list_element *c, const char *value)
{
  struct inferior_data *data;

  if (ptid_equal (inferior_ptid, null_ptid))
    {
      printf (_ (
	"Cannot show stop all cluster CPUs without a live selected thread.\n"));
      return;
    }

  data = mppa_inferior_data (find_inferior_pid (inferior_ptid.pid));
  fprintf_filtered (file, "Stop all cluster CPUs is %s.\n",
		    sopts_cluster_stop_all[data->cluster_stop_all]);
}

static void
set_cluster_debug_ring (char *args, int from_tty, struct cmd_list_element *c)
{
  struct inferior *inf;
  struct inferior_data *data;

  if (ptid_equal (inferior_ptid, null_ptid))
    error (_ ("Cannot set cluster debug ring without a selected thread."));

  inf = current_inferior ();
  data = mppa_inferior_data (inf);
  data->cluster_debug_ring
    = str_idx_in_opts (sopt_cluster_debug_ring, sopts_cluster_debug_ring);
  send_cluster_debug_ring (inf, data->cluster_debug_ring);
}

static void
show_cluster_debug_ring (struct ui_file *file, int from_tty,
			 struct cmd_list_element *c, const char *value)
{
  struct inferior_data *data;

  if (ptid_equal (inferior_ptid, null_ptid))
    {
      printf (
	_ ("Cannot show cluster debug ring without a selected thread.\n"));
      return;
    }

  data = mppa_inferior_data (find_inferior_pid (inferior_ptid.pid));
  fprintf_filtered (file, "Cluster debug ring is %s.\n",
		    sopts_cluster_debug_ring[data->cluster_debug_ring]);
}

static void
attach_mppa_command (char *args, int from_tty)
{
  char set_non_stop_cmd[] = "set non-stop";
  char set_pagination_off_cmd[] = "set pagination off";
  struct osdata *osdata;
  struct osdata_item *item;
  int ix_items, cur_pid, bstopped, bcur_inf_stopped, new_attached;
  struct inferior *cur_inf;
  ptid_t cur_ptid, stopped_ptid;
  int saved_async_execution = !sync_execution;

  dont_repeat ();

  print_thread_events = 0;
  after_first_resume = 0;
  in_attach_mppa = 1;

  kvx_push_arch_stratum (NULL, 0);
  execute_command (set_non_stop_cmd, 0);
  execute_command (set_pagination_off_cmd, 0);

  kvx_target_attach (&current_target, args, from_tty);
  cjtag_over_iss = get_jtag_over_iss ();

  remote_hw_breakpoint_limit = 1;
  remote_hw_watchpoint_limit = 1;

  bstopped = 0;
  bcur_inf_stopped = 0;
  osdata = get_osdata (NULL);

  cur_inf = current_inferior ();
  cur_ptid = inferior_ptid;
  cur_pid = cur_inf->pid;

  for (new_attached = 1; new_attached;)
    {
      new_attached = 0;
      for (ix_items = 0;
	   VEC_iterate (osdata_item_s, osdata->items, ix_items, item);
	   ix_items++)
	{
	  char *endptr;
	  unsigned long pid
	    = strtoul (get_osdata_column (item, "pid"), &endptr, 10);
	  struct inferior *inf;
	  char attach_cmd[25];

	  if (pid == cur_inf->pid || find_inferior_pid (pid))
	    continue;

	  inf = add_inferior_with_spaces ();
	  set_current_inferior (inf);
	  switch_to_thread (null_ptid);
	  set_current_program_space (inf->pspace);
	  sprintf (attach_cmd, "attach %li&", pid);
	  execute_command (attach_cmd, 0);
	  inf->control.stop_soon = NO_STOP_QUIETLY;
	  inf->removable = 1;
	}

      bstopped = 0;
      bcur_inf_stopped = 0;
      osdata = get_osdata (NULL);

      for (ix_items = 0;
	   VEC_iterate (osdata_item_s, osdata->items, ix_items, item);
	   ix_items++)
	{
	  char *endptr;
	  struct thread_info *live_th;
	  unsigned long pid
	    = strtoul (get_osdata_column (item, "pid"), &endptr, 10);
	  const char *file = get_osdata_column (item, "command");
	  const char *running = get_osdata_column (item, "running");

	  if (strcmp (running, "yes"))
	    continue;

	  if (!find_inferior_pid (pid)) // new cluster not attached yet
	    {
	      new_attached = 1;
	      continue;
	    }

	  live_th = any_live_thread_of_process (pid);
	  if (live_th && !live_th->stop_requested)
	    set_stop_requested (live_th->ptid, 1);

	  if (pid == cur_pid)
	    bcur_inf_stopped = 1;

	  if (!bstopped)
	    {
	      bstopped = 1;
	      stopped_ptid = any_live_thread_of_process (pid)->ptid;
	    }

	  if (file && file[0])
	    {
	      struct inferior_data *data;

	      switch_to_thread (any_live_thread_of_process (pid)->ptid);
	      data = mppa_inferior_data (current_inferior ());
	      if (!data->sym_file_loaded)
		{
		  data->sym_file_loaded = 1;
		  kvx_change_file (file, data->cluster);
		}
	    }
	}
    }
  if (!bstopped)
    async_enable_stdin ();

  if (!bstopped || bcur_inf_stopped)
    switch_to_thread (cur_ptid);
  else
    switch_to_thread (stopped_ptid);

  /* This is a hack to populate the dwarf mapping tables now that we
     have the architecture at hand. Having these tables initialized
     from the debug reader routines will break as objfile_arch won't
     have the register descriptions */
  gdbarch_dwarf2_reg_to_regnum (get_current_arch (), 0);

  if (saved_async_execution)
    async_enable_stdin ();

  in_attach_mppa = 0;
  if (new_attach_requested)
    kvx_new_inferiors_cb (NULL);
}

static void
run_mppa_command (char *args, int from_tty)
{
  char set_non_stop_cmd[] = "set non-stop";
  char set_pagination_off_cmd[] = "set pagination off";
  char run_cmd[] = "run";

  dont_repeat ();

  kvx_push_arch_stratum (NULL, 0);
  execute_command (set_non_stop_cmd, 0);
  execute_command (set_pagination_off_cmd, 0);
  execute_command (run_cmd, 0);
}

static void
mppa_inferior_data_cleanup (struct inferior *inf, void *data)
{
  xfree (data);
}

static int
kvx_filesystem_is_local (struct target_ops *ops)
{
  return 1;
}

void
_initialize__kvx_target (void)
{
  int i;

  simulation_vehicle = simulation_vehicles[0];
  cjtag_over_iss = 'n';

  kvx_target_ops.to_shortname = "mppa";
  kvx_target_ops.to_longname = "Kalray MPPA connection";
  kvx_target_ops.to_doc = "Connect to a Kalray MPPA execution vehicle.";
  kvx_target_ops.to_stratum = arch_stratum;

  kvx_target_ops.to_open = kvx_target_open;
  kvx_target_ops.to_close = kvx_target_close;

  kvx_target_ops.to_create_inferior = kvx_target_create_inferior;
  kvx_target_ops.to_attach = mppa_attach;
  kvx_target_ops.to_mourn_inferior = kvx_target_mourn_inferior;
  kvx_target_ops.to_wait = kvx_target_wait;
  kvx_target_ops.to_resume = mppa_target_resume;

  kvx_target_ops.to_supports_non_stop = kvx_target_supports_non_stop;
  kvx_target_ops.to_can_async_p = kvx_target_can_async;
  kvx_target_ops.to_can_run = kvx_target_can_run;
  kvx_target_ops.to_xfer_partial = kvx_target_xfer_partial;
  kvx_target_ops.to_attach_no_wait = 0;
  kvx_target_ops.to_region_ok_for_hw_watchpoint = kvx_region_ok_for_hw_watchpoint;

  kvx_target_ops.to_pid_to_str = mppa_pid_to_str;
  kvx_target_ops.to_extra_thread_info = mppa_threads_extra_info;

  kvx_target_ops.to_fetch_registers = kvx_fetch_registers;
  kvx_target_ops.to_filesystem_is_local = kvx_filesystem_is_local;

  kvx_target_ops.to_magic = OPS_MAGIC;

  add_target (&kvx_target_ops);

  print_stopped_thread = 1;

  add_prefix_cmd ("kalray", class_maintenance, set_kalray_cmd,
		  _ ("Kalray specific variables\n            Configure various "
		     "Kalray specific variables."),
		  &kalray_set_cmdlist, "set kalray ", 0 /* allow-unknown */,
		  &setlist);

  add_prefix_cmd ("kalray", class_maintenance, show_kalray_cmd,
		  _ ("Kalray specific variables\n            Configure various "
		     "Kalray specific variables."),
		  &kalray_show_cmdlist, "show kalray ", 0 /* allow-unknown */,
		  &showlist);

  add_setshow_string_noescape_cmd (
    "debug_agent_options", class_maintenance, &da_options,
    _ ("Set the options passed to the debug agent."),
    _ ("Show the options passed to the debug agent."), NULL, NULL, NULL,
    &kalray_set_cmdlist, &kalray_show_cmdlist);

  add_setshow_enum_cmd ("simulation_vehicle", class_maintenance,
			simulation_vehicles, &simulation_vehicle,
			_ ("Set the simulation vehicle to use for execution."),
			_ ("Show the simulation vehicle to use for execution."),
			NULL, NULL, NULL, &kalray_set_cmdlist,
			&kalray_show_cmdlist);

  add_setshow_boolean_cmd ("break_on_spawn", class_maintenance,
			   &opt_break_on_spawn, _ ("Set break on reset."),
			   _ ("Show break on reset."), NULL,
			   set_cluster_break_on_spawn,
			   show_cluster_break_on_spawn, &kalray_set_cmdlist,
			   &kalray_show_cmdlist);

  add_setshow_boolean_cmd ("cont_os_init_done", class_maintenance,
			   &opt_cont_os_init_done,
			   _ ("Set continue until OS initialization is done."),
			   _ ("Show continue until OS initialization is done."),
			   NULL, NULL, show_cont_os_init_done,
			   &kalray_set_cmdlist, &kalray_show_cmdlist);

  add_com ("attach-mppa", class_run, attach_mppa_command,
	   _ ("Connect to a MPPA TLM platform and start debugging it.\nUsage "
	      "is `attach-mppa PORT'."));

  add_com ("mppa-cpu-status", class_run, mppa_cpu_status_command,
	   _ ("Show information of the current processor."));

  add_com (
    "mppa-cpu-debug-sfr-regs", class_run, mppa_cpu_debug_sfr_regs_command,
    _ ("Read the current processor debug SFR registers from the magic bus"));

  add_setshow_enum_cmd ("stop-all", class_maintenance, sopts_cluster_stop_all,
			&sopt_cluster_stop_all, _ ("Set stop all cluster CPUs"),
			_ ("Show stop all cluster CPUs"), NULL,
			set_cluster_stop_all, show_cluster_stop_all,
			&kalray_set_cmdlist, &kalray_show_cmdlist);

  add_setshow_enum_cmd ("cluster-debug-ring", class_maintenance,
			sopts_cluster_debug_ring, &sopt_cluster_debug_ring,
			_ ("Set cluster debug ring"),
			_ ("Show cluster debug ring"), NULL,
			set_cluster_debug_ring, show_cluster_debug_ring,
			&kalray_set_cmdlist, &kalray_show_cmdlist);

  add_prefix_cmd (
    "intercept-trap", class_maintenance, set_kalray_traps_cmd,
    _ ("Configure what traps should be intercepted for the current cluster."),
    &kalray_set_traps_cmdlist, "set kalray intercept-traps ",
    0 /* allow-unknown */, &kalray_set_cmdlist);

  add_prefix_cmd (
    "intercept-trap", class_maintenance, show_kalray_traps_cmd,
    _ ("Configure what traps should be intercepted for the current cluster."),
    &kalray_show_traps_cmdlist, "show kalray intercept-traps ",
    0 /* allow-unknown */, &kalray_show_cmdlist);

  for (i = 0; i < no_traps_name; i++)
    {
      char trap_set_doc[100], trap_show_doc[100];

      sprintf (trap_set_doc, _ ("Set intercept %s trap."), traps_name[i]);
      sprintf (trap_show_doc, _ ("Show intercept %s trap."), traps_name[i]);
      add_setshow_boolean_cmd (traps_name[i], class_maintenance, &opt_trap,
			       trap_set_doc, trap_show_doc, NULL,
			       set_intercept_trap, show_intercept_trap,
			       &kalray_set_traps_cmdlist,
			       &kalray_show_traps_cmdlist);
    }
  add_setshow_uinteger_cmd ("mask", class_maintenance,
			    (unsigned int *) &opt_trap,
			    _ ("Set intercept traps mask."),
			    _ ("Show intercept traps mask."), NULL,
			    set_intercept_trap_mask, show_intercept_trap_mask,
			    &kalray_set_traps_cmdlist,
			    &kalray_show_traps_cmdlist);

  add_com ("run-mppa", class_run, run_mppa_command,
	   _ ("Connect to a MPPA TLM platform and start debugging it."));

  add_com ("mppa-dump-tlb", class_run, mppa_dump_tlb_command,
	   _ ("Dump TLB. Syntax:\nmppa-dump-tlb [--jtlb] [--ltlb] "
	      "[--valid-only] [--global] [--asn=<asn>]\nIf none of "
	      "--jtlb and --ltlb are given, the both are dumped."));

  add_com ("mppa-lookup-addr", class_run, mppa_lookup_addr_command,
	   _ ("Translate virtual address to/from physical address using the "
	      "TLB entries. "
	      "Syntax:\nmppa-lookup-addr --phys=<addr>|--virt=<addr> "
	      "[--asn=<asn>]\nIf --asn is not specified, "
	      "display all matching entries."));

  observer_attach_inferior_created (kvx_push_arch_stratum);
  kvx_attached_inf_data
    = register_inferior_data_with_cleanup (NULL, mppa_inferior_data_cleanup);
}
