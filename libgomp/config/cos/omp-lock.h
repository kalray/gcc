/* This header is used during the build process to find the size and
   alignment of the public OpenMP locks, so that we can export data
   structures without polluting the namespace.

   In this default POSIX implementation, we used to map the two locks to the
   same PTHREADS primitive, but for OpenMP 3.0 sem_t needs to be used
   instead, as pthread_mutex_unlock should not be called by different
   thread than the one that called pthread_mutex_lock.  */

#include <pthread.h>
#include <semaphore.h>

typedef pthread_mutex_t omp_lock_25_t;
typedef struct
{
  pthread_mutex_t lock;
  int count;
} omp_nest_lock_25_t;
#ifdef HAVE_BROKEN_POSIX_SEMAPHORES
/* If we don't have working semaphores, we'll make all explicit tasks
   tied to the creating thread.  */
typedef pthread_mutex_t omp_lock_t;
typedef struct
{
  pthread_mutex_t lock;
  int count;
  void *owner;
} omp_nest_lock_t;
#else
typedef union
{
  sem_t sem;
  /* sem_t points to uintptr_t in newlib port for ClusterOS,
     thus, its size is pointer size. Due to the 32-bit mode,
     the generated header omp.h might use the 32-bit flavor
     that is wrong for 64-bit. To fix this we force the struct to
     be 8 bytes long for both 32-bit and 64-bit modes. */
  long long payload;
} omp_lock_t;
typedef union
{
  struct
  {
    sem_t lock;
    int count;
    void *owner;
  };
  /* sem_t points to uintptr_t in newlib port for ClusterOS,
     thus, its size is pointer size. Due to the 32-bit mode,
     the generated header omp.h might use the 32-bit flavor
     that is wrong for 64-bit. To fix this we force the sem_t to
     be 8 bytes long for both 32-bit and 64-bit modes.
     In this case, the entire struct needs to be 24 bytes as C struct
     elements are aligned on their size.
     24 bytes is required for the alignment of the
     last 'void *owner' pointer that is 8 bytes in 64-bit */
  long long payload[3];
} omp_nest_lock_t;
#endif
