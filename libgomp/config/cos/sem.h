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

/* This is the default POSIX 1003.1b implementation of a semaphore
   synchronization mechanism for libgomp.  This type is private to
   the library.

   This is a bit heavy weight for what we need, in that we're not
   interested in sem_wait as a cancelation point, but it's not too
   bad for a default.  */

#ifndef GOMP_SEM_H
#define GOMP_SEM_H 1

#ifdef HAVE_ATTRIBUTE_VISIBILITY
#pragma GCC visibility push(default)
#endif

#include <semaphore.h>

#ifdef HAVE_ATTRIBUTE_VISIBILITY
#pragma GCC visibility pop
#endif

#ifdef HAVE_BROKEN_POSIX_SEMAPHORES
#include <pthread.h>

struct gomp_sem
{
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  int value;
};

typedef struct gomp_sem gomp_sem_t;

extern void gomp_sem_init (gomp_sem_t *sem, int value);

extern void gomp_sem_wait (gomp_sem_t *sem);

extern void gomp_sem_post (gomp_sem_t *sem);

extern void gomp_sem_destroy (gomp_sem_t *sem);

#else /* HAVE_BROKEN_POSIX_SEMAPHORES  */

#ifdef HAVE_ATTRIBUTE_VISIBILITY
#pragma GCC visibility push(default)
#endif

#include <hal/cos_apic_mailbox.h>
#include <hal/cos_atomic.h>
#include <hal/cos_bsp.h>
#include <hal/cos_cache.h>

extern void abort (void);

#ifdef HAVE_ATTRIBUTE_VISIBILITY
#pragma GCC visibility pop
#endif

typedef sem_t gomp_sem_t;

static inline void
gomp_sem_write_set (gomp_sem_t *sem, const int value)
{
  volatile uintptr_t *ptr = (void *) sem;

  *ptr = value;
  __builtin_kvx_fence ();
}

static inline void
gomp_sem_init (gomp_sem_t *sem, int value)
{
  gomp_sem_write_set (sem, value);
}

static inline void
gomp_sem_wait (gomp_sem_t *sem)
{
  __cos_swap_t c;

  __builtin_kvx_fence ();

  while (1)
    {
      __uncached volatile uintptr_t *ptr = (void *) sem;

      uintptr_t count = *ptr;

      if (count == 0)
	{
	  if ((uintptr_t) &MPPA_COS_THREAD_PER_CORE_LOG2
	      != 0) /* yield if more than one thread per core */
	    mppa_cos_synchronization_wait (NULL);
	}
      else
	{
#if (__SIZEOF_PTRDIFF_T__ == 8)
	  c.ret = __builtin_kvx_acswapd ((void *) ptr, count - 1, count);
#else
	  c.ret = __builtin_kvx_acswapw ((void *) ptr, count - 1, count);
#endif
	  if (c.test == 1)
	    break;
	}
    }
  MPPA_COS_DINVAL ();
}

static inline void
gomp_sem_post (gomp_sem_t *sem)
{
  __builtin_kvx_fence (); /* consistency before post */
#if (__SIZEOF_PTRDIFF_T__ == 8)
  MPPA_COS_AFADDD ((void *) sem, 1);
#else
  MPPA_COS_AFADDW ((void *) sem, 1);
#endif
  __builtin_kvx_fence (); /* consistency before doorbell */
  mppa_cos_doorbell_all ();
  MPPA_COS_DINVAL ();
}

static inline void
gomp_sem_destroy (gomp_sem_t *sem)
{
  gomp_sem_write_set (sem, 0);
}
#endif /* doesn't HAVE_BROKEN_POSIX_SEMAPHORES  */
#endif /* GOMP_SEM_H  */
