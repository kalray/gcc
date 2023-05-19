/* Copyright (C) 2005-2022 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU Offloading and Multi Processing Library
   (libgomp).

   Libgomp is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   Libgomp is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

/* This file defines OpenMP API entry points that accelerator targets are
   expected to replace.  */

#include "libgomp.h"
#include <stdio.h>

volatile struct gomp_offload_icvs GOMP_ADDITIONAL_ICVS;
static int is_offloading_device = 0;
static int mppa_multi_device_p = 0;

void
omp_set_default_device (int device_num)
{
  struct gomp_task_icv *icv = gomp_icv (true);
  icv->default_device_var = device_num >= 0 ? device_num : 0;
}

ialias (omp_set_default_device)

int
omp_get_default_device (void)
{
  struct gomp_task_icv *icv = gomp_icv (false);
  return icv->default_device_var;
}

ialias (omp_get_default_device)

int
omp_get_initial_device (void)
{
  return gomp_get_num_devices ();
}

ialias (omp_get_initial_device)

int
omp_get_num_devices (void)
{
  return gomp_get_num_devices ();
}

ialias (omp_get_num_devices)

void
omp_offload_init (int multi_device)
{
  is_offloading_device = 1;
  mppa_multi_device_p = multi_device == 1 ? 1 : 0;
}

/* Should be 1 on host, 0 on device.  */
int
omp_is_initial_device (void)
{
  return !is_offloading_device;
}

ialias (omp_is_initial_device)

int
omp_get_device_num (void)
{
  if (mppa_multi_device_p)
    {
      /* Get the PCR SFR which holds processor config info.  */
      int pcr = __builtin_kvx_get (2);
      pcr &= 0xFFFF;
      return pcr >> 8;
      /* TODO @multiboard feat offset cluster ID by the number of MPPAs */
    }
  else
    {
      return omp_get_num_devices ();
    }
}

ialias (omp_get_device_num)

int
omp_get_max_teams (void)
{
  return GOMP_ADDITIONAL_ICVS.nteams;
}

ialias (omp_get_max_teams)

void
omp_set_num_teams (int num_teams)
{
  if (num_teams >= 0)
    GOMP_ADDITIONAL_ICVS.nteams = num_teams;
}

ialias (omp_set_num_teams)

int
omp_get_teams_thread_limit (void)
{
  return GOMP_ADDITIONAL_ICVS.teams_thread_limit;
}

ialias (omp_get_teams_thread_limit)

void
omp_set_teams_thread_limit (int thread_limit)
{
  if (thread_limit >= 0)
    GOMP_ADDITIONAL_ICVS.teams_thread_limit = thread_limit;
}

ialias (omp_set_teams_thread_limit)
