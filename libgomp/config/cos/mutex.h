/* Copyright (C) 2005-2017 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.
   Copyright (C) 2017 Kalray

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

/* This is the default PTHREADS implementation of a mutex synchronization
   mechanism for libgomp.  This type is private to the library.  */

#ifndef GOMP_MUTEX_H
#define GOMP_MUTEX_H 1
extern void abort (void);

#include <hal/cos_apic_mailbox.h>
#include <hal/cos_bsp.h>
#include <hal/cos_cache.h>
#include <pthread.h>

typedef pthread_mutex_t gomp_mutex_t;

#define GOMP_MUTEX_INIT_0 0

extern int MPPA_COS_THREAD_PER_CORE_SHIFT;

static inline void
gomp_mutex_write_set (void *mutex, const int value)
{
  volatile uintptr_t *ptr = (void *) mutex;

  *ptr = value;
}

static inline void
gomp_mutex_init (gomp_mutex_t *mutex)
{
  gomp_mutex_write_set (mutex, 1);
  __builtin_k1_fence ();
}

static inline void
gomp_mutex_lock (gomp_mutex_t *mutex)
{
#if (__SIZEOF_PTRDIFF_T__ == 8)
  while (!(__builtin_k1_alclrd ((void *) mutex) == 1ULL))
#else
  while (!(__builtin_k1_alclrw ((void *) mutex) == 1ULL))
#endif
    {
      if ((uintptr_t) &MPPA_COS_THREAD_PER_CORE_SHIFT
	  != 0) /* yield if more than one thread per core */
	mppa_cos_synchronization_wait (NULL);
    }
  MPPA_COS_DINVAL ();
}

static inline void
gomp_mutex_unlock (gomp_mutex_t *mutex)
{
  __builtin_k1_fence (); /* consitency before unlock */
  gomp_mutex_write_set (mutex, 1);
  mppa_cos_doorbell_all ();
  MPPA_COS_DINVAL ();
}

static inline void
gomp_mutex_destroy (gomp_mutex_t *mutex)
{
  gomp_mutex_write_set (mutex, 0);
  __builtin_k1_fence ();
}

#endif /* GOMP_MUTEX_H */
