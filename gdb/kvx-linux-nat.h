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

#ifndef _KVX_LINUX_NAT_H_
#define _KVX_LINUX_NAT_H_

#define NB_GPRS 64
enum regnum
{
  R_R0 = 0,
  R_LC = NB_GPRS,
  R_LE,
  R_LS,
  R_RA,
  R_CS,
  R_PC,
  NB_GREGSET_REGS
};

#define R_TLS (R_R0 + 13)

#ifndef PTRACE_GET_HW_PT_REGS
#define PTRACE_GET_HW_PT_REGS 20
#define PTRACE_SET_HW_PT_REGS 21
#endif

#ifndef TRAP_HWBKPT
#define TRAP_HWBKPT 4
#endif

#define get_hw_pt_idx(v) ((v) >> 2)
#define hw_pt_is_bkp(v) (((v) &1) != 0)

#define MAX_WPTS 2
#define MAX_BPTS 2
#define HW_PT_ENABLE 1

#define HW_PT_CMD_GET_CAPS 0
#define HW_PT_CMD_GET_PT 1
#define HW_PT_CMD_SET_RESERVE 0
#define HW_PT_CMD_SET_ENABLE 1

/* Information describing the hardware breakpoint capabilities. */
struct kvx_linux_hw_pt_cap_s
{
  int wp_count;
  int bp_count;
};

/* Structure used to keep track of hardware breakpoints/watchpoints. */
struct kvx_linux_hw_pt
{
  uint64_t address;
  int size;
  int enabled;
  int used;
};

/* Per-thread arch-specific data we want to keep.  */
struct arch_lwp_info
{
  /* Non-zero if our copy differs from what's recorded in the thread.  */
  char bpts_changed[MAX_BPTS];
  char wpts_changed[MAX_WPTS];
  /* Cached stopped data address.  */
  uint64_t stopped_data_address;
};

/* Callback to mark a watch-/breakpoint to be updated in all threads of
   the current process. */
struct update_registers_data
{
  int type;
  int i;
};

#define KVX_TLS_TCB_SIZE 8

#endif // _KVX_LINUX_NAT_H_
