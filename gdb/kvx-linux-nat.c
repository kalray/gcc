/* Kalray KVX GNU/Linux native support.

   Copyright (C) 2007-2020 Free Software Foundation, Inc.

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
#include <elf.h>
#include <sys/ptrace.h>
#include "inferior.h"
#include "linux-nat.h"
#include "kvx-linux-nat.h"
#include "gdb_proc_service.h"

enum
{
  HW_BKP_TYPE = 1,
  HW_WRITE_WP_TYPE = 2
};

struct inferior_data
{
  /* Hardware breakpoints for this process.  */
  struct kvx_linux_hw_pt bpts[MAX_BPTS];
  /* Hardware watchpoints for this process.  */
  struct kvx_linux_hw_pt wpts[MAX_WPTS];
};

struct kvx_linux_hw_pt_cap_s kvx_linux_hw_pt_cap;
static const struct inferior_data *kvx_linux_inferior_data_token;

/* Query Hardware Breakpoint information for the target we are attached to */
static void
kvx_linux_init_hwbp_cap (int pid)
{
  uint64_t val[2];

  if (ptrace (PTRACE_GET_HW_PT_REGS, pid, 0, val) < 0)
    return;

  kvx_linux_hw_pt_cap.bp_count = val[0];
  kvx_linux_hw_pt_cap.wp_count = val[1];

  if (kvx_linux_hw_pt_cap.wp_count > MAX_WPTS)
    internal_error (__FILE__, __LINE__, "Unsupported number of watchpoints");
  if (kvx_linux_hw_pt_cap.bp_count > MAX_BPTS)
    internal_error (__FILE__, __LINE__, "Unsupported number of breakpoints");
}

static struct inferior_data *
kvx_linux_inferior_data (int pid)
{
  struct inferior_data *res;
  struct inferior *inf = find_inferior_pid (pid);

  if (!inf)
    return NULL;

  res = inferior_data (inf, kvx_linux_inferior_data_token);
  if (!res)
    {
      res = xcalloc (1, sizeof (*res));
      set_inferior_data (inf, kvx_linux_inferior_data_token, res);

      kvx_linux_init_hwbp_cap (pid);
    }

  return res;
}

void
fill_gregset (const struct regcache *regcache, gdb_gregset_t *rset, int regnum)
{
  int i;

  if (regnum == -1)
    {
      regcache_raw_collect (regcache, regnum, &(*rset)[regnum]);
      return;
    }

  for (i = R_R0; i < NB_GREGSET_REGS; i++)
    if (regcache_register_status (regcache, i) == REG_VALID)
      regcache_raw_collect (regcache, i, &(*rset)[i]);
}

static void
supply_gregset_reg (struct regcache *regcache, const gdb_gregset_t *rset)
{
  int i;

  for (i = R_R0; i < NB_GREGSET_REGS; i++)
    regcache_raw_supply (regcache, i, &(*rset)[i]);
}

void
supply_gregset (struct regcache *regcache, const gdb_gregset_t *gregsetp)
{
  supply_gregset_reg (regcache, gregsetp);
}

void
fill_fpregset (const struct regcache *regcache, gdb_fpregset_t *fpregsetp,
	       int regnum)
{
  /* Not implemented yet */
}

void
supply_fpregset (struct regcache *regcache, const gdb_fpregset_t *fpregsetp)
{
  /* Not implemented yet */
}

static void
kvx_get_gregs (int tid, gdb_gregset_t *regs)
{
  struct iovec iovec;

  iovec.iov_base = regs;
  iovec.iov_len = sizeof (*regs);
  if (ptrace (PTRACE_GETREGSET, tid, NT_PRSTATUS, &iovec) < 0)
    {
      perror_with_name (_ ("Couldn't get general registers"));
      return;
    }
}

/* Fetch greg-register(s) from process/thread TID
   and store value(s) in GDB's register array. */
static void
fetch_gregs (struct regcache *regcache, int regnum)
{
  int tid = ptid_get_lwp (inferior_ptid);
  gdb_gregset_t regs;

  kvx_get_gregs (tid, &regs);
  supply_gregset_reg (regcache, &regs);
}

/* Store greg-register(s) in GDB's register
   array into the process/thread specified by TID. */
static void
store_gregs (struct regcache *regcache, int regnum)
{
  gdb_gregset_t regs;
  struct iovec iovec;
  int tid = ptid_get_lwp (inferior_ptid);

  kvx_get_gregs (tid, &regs);

  fill_gregset (regcache, &regs, regnum);

  iovec.iov_base = &regs;
  iovec.iov_len = sizeof (regs);
  if (ptrace (PTRACE_SETREGSET, tid, NT_PRSTATUS, &iovec) < 0)
    {
      perror_with_name (_ ("Couldn't write general registers"));
      return;
    }
}

static void
kvx_linux_fetch_inferior_registers (struct target_ops *ops,
				   struct regcache *regcache, int regnum)
{
  fetch_gregs (regcache, regnum);
}

static void
kvx_linux_store_inferior_registers (struct target_ops *ops,
				   struct regcache *regcache, int regnum)
{
  store_gregs (regcache, regnum);
}

static int
kvx_linux_can_use_hw_breakpoint (struct target_ops *self, enum bptype type,
				int cnt, int othertype)
{
  if (type == bp_hardware_watchpoint || type == bp_hardware_breakpoint)
    return 1;

  return 0;
}

static PTRACE_TYPE_ARG3
compute_ptrace_addr_arg (int cmd, int type, int idx)
{
  uint64_t ret = cmd;

  if (type == HW_WRITE_WP_TYPE)
    ret |= 1 << 2;
  ret |= idx << 3;

  return (PTRACE_TYPE_ARG3) ret;
}

static uint64_t *
compute_ptrace_data_arg (CORE_ADDR addr, int len, int enable)
{
  static uint64_t data[2];

  data[0] = addr;
  data[1] = enable | (len << 1);

  return data;
}

static int
kvx_get_hw_point_type_info (int type, struct kvx_linux_hw_pt **pts, int *count)
{
  struct inferior_data *data;
  int pid = ptid_get_pid (inferior_ptid);

  *pts = NULL;
  *count = 0;

  data = kvx_linux_inferior_data (pid);
  if (!data)
    return -1;

  if (type == HW_BKP_TYPE)
    {
      *pts = data->bpts;
      *count = kvx_linux_hw_pt_cap.bp_count;
    }
  else
    {
      *pts = data->wpts;
      *count = kvx_linux_hw_pt_cap.wp_count;
    }

  return 0;
}

static struct arch_lwp_info *
kvx_get_thread_info (struct lwp_info *lwp)
{
  int i;
  struct arch_lwp_info *info = lwp_arch_private_info (lwp);

  if (info)
    return info;

  info = XCNEW (struct arch_lwp_info);

  for (i = 0; i < MAX_BPTS; i++)
    info->bpts_changed[i] = 1;
  for (i = 0; i < MAX_WPTS; i++)
    info->wpts_changed[i] = 1;

  lwp_set_arch_private_info (lwp, info);

  return info;
}

static int
kvx_update_thread_inf_cb (struct lwp_info *lwp, void *arg)
{
  struct update_registers_data *cb_data = (struct update_registers_data *) arg;
  struct arch_lwp_info *info = kvx_get_thread_info (lwp);

  /* The actual update is done later just before resuming the lwp,
  we just mark that the registers need updating. */
  if (cb_data->type == HW_BKP_TYPE)
    info->bpts_changed[cb_data->i] = 1;
  else
    info->wpts_changed[cb_data->i] = 1;

  /* If the lwp isn't stopped, force it to momentarily pause, so
     we can update its breakpoint registers. */
  if (!lwp_is_stopped (lwp))
    linux_stop_lwp (lwp);

  return 0;
}

static int
kvx_linux_insert_hw_point (CORE_ADDR addr, int len, int type)
{
  struct kvx_linux_hw_pt *pts;
  int i, count;
  int pid = ptid_get_pid (inferior_ptid);
  int tid = ptid_get_lwp (inferior_ptid);

  if (kvx_get_hw_point_type_info (type, &pts, &count))
    return -1;

  for (i = 0; i < count; i++)
    {
      if (!pts[i].enabled)
	{
	  struct update_registers_data cb_data = {type, i};

	  if (ptrace (PTRACE_SET_HW_PT_REGS, tid,
		      compute_ptrace_addr_arg (HW_PT_CMD_SET_RESERVE, type, i),
		      compute_ptrace_data_arg (addr, len, 1)))
	    continue;

	  pts[i].address = addr;
	  pts[i].size = len;
	  pts[i].enabled = 1;
	  pts[i].used = 1;

	  iterate_over_lwps (pid_to_ptid (pid), kvx_update_thread_inf_cb,
			     &cb_data);

	  return 0;
	}
    }

  /* We're out of hardware resources */
  return -1;
}

/* Insert a hardware-assisted breakpoint at BP_TGT->reqstd_address.
   Return 0 on success, -1 on failure. */
static int
kvx_linux_insert_hw_breakpoint (struct target_ops *self, struct gdbarch *gdbarch,
			       struct bp_target_info *bp_tgt)
{
  CORE_ADDR addr;
  int len;

  addr = bp_tgt->reqstd_address;
  gdbarch_breakpoint_from_pc (gdbarch, &addr, &len);
  bp_tgt->placed_address = addr;
  bp_tgt->length = len;

  return kvx_linux_insert_hw_point (addr, len, HW_BKP_TYPE);
}

/* Insert a watchpoint to watch a memory region which starts at
   address ADDR and whose length is LEN bytes. Watch memory accesses
   of the type TYPE. Return 0 on success, -1 on failure. */
static int
kvx_linux_insert_watchpoint (struct target_ops *self, CORE_ADDR addr, int len,
			    enum target_hw_bp_type type,
			    struct expression *cond)
{
  if (type != hw_write)
    return -1;

  return kvx_linux_insert_hw_point (addr, len, HW_WRITE_WP_TYPE);
}

static int
kvx_linux_remove_hw_point (CORE_ADDR addr, int len, int type)
{
  struct kvx_linux_hw_pt *pts;
  int i, found, count;
  int pid = ptid_get_pid (inferior_ptid);

  if (kvx_get_hw_point_type_info (type, &pts, &count))
    return -1;

  found = 0;
  for (i = 0; i < count; i++)
    {
      if (!pts[i].enabled)
	continue;

      if (pts[i].address == addr && pts[i].size == len)
	{
	  struct update_registers_data cb_data = {type, i};
	  pts[i].enabled = 0;
	  iterate_over_lwps (pid_to_ptid (pid), kvx_update_thread_inf_cb,
			     &cb_data);
	  found = 1;
	}
    }

  if (found)
    return 0;

  return -1;
}

/* Remove a hardware-assisted breakpoint at BP_TGT->placed_address.
   Return 0 on success, -1 on failure. */
static int
kvx_linux_remove_hw_breakpoint (struct target_ops *self, struct gdbarch *gdbarch,
			       struct bp_target_info *bp_tgt)
{
  return kvx_linux_remove_hw_point (bp_tgt->placed_address, bp_tgt->length,
				   HW_BKP_TYPE);
}

/* Implement the "to_remove_watchpoint" target_ops method.
   Remove a watchpoint that watched the memory region which starts at
   address ADDR, whose length is LEN bytes, and for accesses of the
   type TYPE. Return 0 on success, -1 on failure. */
static int
kvx_linux_remove_watchpoint (struct target_ops *self, CORE_ADDR addr, int len,
			    enum target_hw_bp_type type,
			    struct expression *cond)
{
  return kvx_linux_remove_hw_point (addr, len, HW_WRITE_WP_TYPE);
}

static int
kvx_linux_stopped_data_address (struct target_ops *ops, CORE_ADDR *addr_p)
{
  siginfo_t siginfo;
  int tid = ptid_get_lwp (inferior_ptid);

  /* We must be able to set hardware watchpoints. */
  if (kvx_linux_hw_pt_cap.wp_count == 0)
    return 0;

  /* Retrieve siginfo. */
  errno = 0;
  ptrace (PTRACE_GETSIGINFO, tid, 0, &siginfo);
  if (errno != 0)
    return 0;

  /* This must be a hardware breakpoint. */
  if (siginfo.si_signo != SIGTRAP || (siginfo.si_code & 0xffff) != TRAP_HWBKPT)
    return 0;

  /* If we are in a positive slot then we're looking at a breakpoint and not
     a watchpoint. */
  if (hw_pt_is_bkp (siginfo.si_errno))
    return 0;

  *addr_p = (CORE_ADDR) (uintptr_t) siginfo.si_addr;

  return 1;
}

/* Return whether current thread is stopped due to a watchpoint. */
static int
kvx_linux_stopped_by_watchpoint (struct target_ops *ops)
{
  CORE_ADDR addr;

  return kvx_linux_stopped_data_address (ops, &addr);
}

/* Function to call when a new thread is detected. */
static void
kvx_linux_new_thread (struct lwp_info *lwp)
{
  kvx_get_thread_info (lwp);
}

/* Called when resuming a thread LWP.
   The hardware debug registers are updated when there is any change. */
static void
kvx_linux_prepare_to_resume (struct lwp_info *lwp)
{
  struct arch_lwp_info *info = kvx_get_thread_info (lwp);
  int i, j, ret;
  int tid = ptid_get_lwp (ptid_of_lwp (lwp));
  int hw_points_types[] = {HW_BKP_TYPE, HW_WRITE_WP_TYPE};
  int nb_hw_points_types
    = sizeof (hw_points_types) / sizeof (hw_points_types[0]);

  for (i = 0; i < nb_hw_points_types; i++)
    {
      struct kvx_linux_hw_pt *pts, *pt;
      int count, type = hw_points_types[i];
      char *changed = !i ? info->bpts_changed : info->wpts_changed;

      if (kvx_get_hw_point_type_info (type, &pts, &count))
	break;

      for (j = 0; j < count; j++)
	{
	  pt = &pts[j];
	  if (changed[j] && pt->used)
	    {
	      errno = 0;
	      ret = ptrace (PTRACE_SET_HW_PT_REGS, tid,
			    compute_ptrace_addr_arg (HW_PT_CMD_SET_ENABLE, type,
						     j),
			    compute_ptrace_data_arg (pt->address, pt->size,
						     pt->enabled));
	      if (ret < 0)
		perror_with_name ("Error setting breakpoint");
	    }

	  changed[j] = 0;
	}
    }
}

/* Called by libthread_db. */
ps_err_e
ps_get_thread_area (struct ps_prochandle *ph, lwpid_t lwpid, int idx,
		    void **base)
{
  gdb_gregset_t regs;

  kvx_get_gregs (lwpid, &regs);

  /* IDX is the bias from the thread pointer to the beginning of the
   thread descriptor.  It has to be subtracted due to implementation
   quirks in libthread_db. */
  *base = (void *) (uintptr_t) (regs[R_TLS] - idx - KVX_TLS_TCB_SIZE);

  return 0;
}

static void
kvx_cleanup_linux_inferior_data (struct inferior *inf, void *data)
{
  xfree (data);
}

void _initialize_kvx_linux_nat (void);
void
_initialize_kvx_linux_nat (void)
{
  struct target_ops *t;

  /* Fill in the generic GNU/Linux methods. */
  t = linux_target ();

  /* Add our register access methods. */
  t->to_fetch_registers = kvx_linux_fetch_inferior_registers;
  t->to_store_registers = kvx_linux_store_inferior_registers;
  t->to_can_use_hw_breakpoint = kvx_linux_can_use_hw_breakpoint;
  t->to_insert_hw_breakpoint = kvx_linux_insert_hw_breakpoint;
  t->to_remove_hw_breakpoint = kvx_linux_remove_hw_breakpoint;
  t->to_insert_watchpoint = kvx_linux_insert_watchpoint;
  t->to_remove_watchpoint = kvx_linux_remove_watchpoint;
  t->to_stopped_by_watchpoint = kvx_linux_stopped_by_watchpoint;
  t->to_stopped_data_address = kvx_linux_stopped_data_address;

  linux_nat_add_target (t);
  linux_nat_set_new_thread (t, kvx_linux_new_thread);
  linux_nat_set_prepare_to_resume (t, kvx_linux_prepare_to_resume);

  // override the dummy_target->to_can_use_hw_breakpoint function
  t = find_target_beneath (&current_target);
  if (t->to_stratum == dummy_stratum)
    t->to_can_use_hw_breakpoint = kvx_linux_can_use_hw_breakpoint;

  kvx_linux_inferior_data_token
    = register_inferior_data_with_cleanup (NULL,
					   kvx_cleanup_linux_inferior_data);
}
