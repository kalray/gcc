  /* Plugin for KVX MPPA execution.

   Copyright (C) 2022 Free Software Foundation, Inc.

   Contributed by Kalray Inc.
   Heavily inspired from plugin-gcn.c

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

/* {{{ Includes and defines  */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <stdbool.h>
#include <limits.h>
#include <dlfcn.h>
#include <signal.h>
#include "libgomp-plugin.h"
#include "gomp-constants.h"
#include <elf.h>
#include "oacc-plugin.h"
#include "oacc-int.h"
#include <mppa_offload_host.h>
#include <assert.h>


#if 1
#define MPPA_OFFLOAD_BOOT_MODE (MPPA_OFFLOAD_BOOT_MODE_DEFAULT)
#else
#define MPPA_OFFLOAD_BOOT_MODE (MPPA_OFFLOAD_BOOT_MODE_DAEMON)
#endif

/* The size max of buffers used in this file.  */
#define BUF_MAX 512

/* As of now, we only support one MPPA.  */
#define MPPA_OFFLOAD_NB_ACCELERATOR_DEFAULT (1)
/* Each MPPA has 5 clusters, hence 5 system queues.  */
#define NB_SYSTEM_QUEUE (5)
/* The cluster where COS is actually booted.  */
#define MPPA_OFFLOAD_CC_ID (0)

#define MPPA_GOMP_DEFAULT_BUFFER_ALIGN (64)

// #define OFFLOAD_ALLOC_MODE MPPA_OFFLOAD_ALLOC_DDR
#define OFFLOAD_ALLOC_MODE MPPA_OFFLOAD_ALLOC_LOCALMEM_0

// #define MPPA_OFFLOAD_DEFAULT_QUEUE_TYPE MMIO
#define MPPA_OFFLOAD_DEFAULT_QUEUE_TYPE RPMSG

#define MPPA_MAX_ASYNC_KERNELS 2048

#define MPPA_CV1_SMEM_SIZE 4000000
#define MPPA_CV2_SMEM_SIZE 8000000
#define MPPA_DV1_SMEM_SIZE 8000000

static mppa_offload_host_acc_config_t mppa_offload_host_acc_cfg = {
  .board_id = 0,
  .cluster_id = MPPA_OFFLOAD_CC_ID,
  .nb_sysqueue = NB_SYSTEM_QUEUE,
  .firmware_name = NULL,
};


//TODO move to agent_info (@multiboard feat)
static mppa_offload_host_config_t mppa_offload_cfg_data = {
  .common = {
	     .system_queue_type = MPPA_OFFLOAD_QUEUE_RPMSG,
	     .queue_type = MPPA_OFFLOAD_QUEUE_RPMSG,
	     .rdma_type = MPPA_OFFLOAD_RDMA_PCIE},
  .image_type = MPPA_OFFLOAD_IMAGE_PATH,
  /* file system, no meta data */
  .images = NULL,
  .boot_mode = MPPA_OFFLOAD_BOOT_MODE,
  .nb_accelerators = MPPA_OFFLOAD_NB_ACCELERATOR_DEFAULT,
  .accelerator_configs = &mppa_offload_host_acc_cfg,
};

enum offload_kind { KIND_UNKNOWN, KIND_OPENMP, KIND_OPENACC };
enum queue_type { MMIO, RPMSG };

struct kernel_info
{
  /* Name of the kernel, required to locate it within the GCN object-code
     module.  */
  const char *name;
  /* The specific agent the kernel has been or will be finalized for and run
     on.  */
  struct agent_info *agent;
  /* Mutex enforcing that at most once thread ever initializes a kernel for
     use.  A thread should have locked agent->module_rwlock for reading before
     acquiring it.  */
  pthread_mutex_t init_mutex;
   /* Flag indicating whether the kernel has been initialized and all fields
      below it contain valid data.  */
  bool initialized_p;
  /* Flag indicating that the kernel has a problem that blocks an execution.  */
  bool initialization_failed_p;
  /* The object to be put into the dispatch queue.  */
  uint64_t object;
  /* Set up for OpenMP or OpenACC?  */
  enum offload_kind kind;
  void *async_data;
  /* The kernel's own arguments (variables).  */
  void *vars;
  /* OpenMP runtime arguments (such as num_teams, thread_limit...).  */
  void **args;
  /* The ID of the cluster which the kernel will run on.  */
  int cluster_id;
};

/* An async queue header.

   OpenMP may create one of these.
   OpenACC may create many.  */
struct goacc_asyncqueue
{
  struct goacc_asyncnode *_Atomic front;
  struct goacc_asyncnode *_Atomic rear;
  /* Condition to allow enqueuing if n_kernels <= MAX_ASYNC_KERNELS.  */
  pthread_cond_t *enqueue_cond;
  /* Mutex that should be locked before waiting on enqueue_cond.  */
  pthread_mutex_t *enqueue_mutex;
};

struct goacc_asyncnode
{
  struct kernel_info *kernel;
  struct goacc_asyncnode *_Atomic next;
  /* Anti ABA measure: keep track of how many kernels were popped before it.  */
  atomic_int count;
};

/* Opaque type to represent pointers on the agent.  */
struct mppa_gomp_buffer
{
  /* An address in the virtual address space of the MPPA.  */
  uint64_t vaddr;
  /* Its corresponding physical address for use by DMA accesses.  */
  uint64_t paddr;
  /* The size of the allocated memory on the MPPA.  */
  size_t size;
  /* Where it was allocated on the MPPA.  */
  int alloc_mode;
};

/* A block which has been allocated on the agent and whose address is known to
   the host.  It is used to translate back addresses provided by the host which
   have been offsetted.  */

struct block_node
{
  /* The base address of the block.  */
  uint64_t base_addr;
  /* A pointer to the buffer.  */
  struct mppa_gomp_buffer *buffer;
  /* Pointer to the next block.  */
  struct block_node *nxt;
};

struct async_ctx
{
  /* Init tracking.  */
  int initialized_p;
  /* Number of asynchronous threads. Max size is NB_SYSTEM_QUEUE.  */
  pthread_t *async_ths;
  /* Condition to wake workers when a new kernel is enqueued.  */
  pthread_cond_t *worker_wakeup_cond;
  /* Mutex that should be locked before waiting on worker_wakeup_cond.  */
  pthread_mutex_t *worker_wakeup_mutex;
  /* Counts how many kernels are in the queue.  */
  atomic_int n_kernels;
  /* Boolean to indicate whether the async queue must stop at the next iter.  */
  int async_queue_stop;
  /* The kernel queue, containing none or more nodes.  */
  struct goacc_asyncqueue *async_queue;
};

/* An agent is an offloading device.  */
//TODO extract architecture specific info (smem size, etc) to arch struct
struct agent_info
{
  /* Opaque structure containing the state of the agent.  */
  mppa_offload_host_context_t ctx;
  /* Structure containing async queue data and config variables.  */
  struct async_ctx *async_context;
  /* Is the agent initialized?  */
  int initialized_p;
  /* Is the agent finalized?  */
  int finalized_p;
  /* Opaque handle to the loaded elf.  */
  uint64_t reloc_offset;
  struct block_node *blocks;
  pthread_mutex_t *blocks_mutex;
  /* Offloading queue type (either MMIO or RPROC over sysqueues).  */
  enum queue_type queue_type;
  /* MMIO queue pointers, which store tasks.  */
  mppa_offload_mmio_queue_t *mmio_queues;
  pthread_mutex_t mmio_locks[NB_SYSTEM_QUEUE];
  /* agent number.  */
  int id;
  /* Lock for cluster scheduling. Multiple threads may request a schedule at 
     the same time, this is to make sure they do not reserve 
     the same cluster.  */
  pthread_mutex_t *schedule_mutex;
  /* Round robin counter for scheduling. Its access must be protected by
     locking schedule_mutex. */
  int rr_counter;
  /* Size (in bytes) of a cluster's shared memory.  */
  size_t smem_size;
};

struct kvx_context_info
{
  /* Whether the structure has been initialized.  */
  bool initialized_p;
  /* Number of usable KVX MPPA agents in the system.  */
  int agent_count;
  /* Array of agent_info structures describing the individual HSA agents.  */
  struct agent_info *agents;
};

/* Any changes to the layout of the structures kvx_image,
   kvx_kernel_description, global_var_info and kvx_image_desc should be mirrored
   in mkoffload.cc.  They are used to makes sense of the memory mapped image
   that is meant to be copied to the agent.  */

struct kvx_image
{
  /* Size of the code. */
  size_t size;
  /* Actual ELF image.  */
  void *image;
};

struct kvx_kernel_description
{
  /* Kernel name.  */
  const char *name;
};

struct kvx_async_worker_params
{
  struct agent_info *agent;
  int cluster_id;
};

struct global_var_info
{
  /* The name of the variable as in the original source.  */
  const char *name;
  /* The size of the variable in bytes.  */
  const size_t size;
  /* The address of the variable in the image. (unused)  */
  void *address;
};

struct kvx_image_desc
{
  /* ELF binary which contains code to be offloaded.  */
  const struct kvx_image *kvx_image;
  /* Number of kernels contained in KVX_IMAGE.  */
  unsigned kernel_count;
  /* Internal informations about the kernels.  */
  const struct kvx_kernel_description *kernel_infos;
  /* Number of offloaded global variables.  */
  unsigned global_variable_count;
  /* Internal information about global variables.  */
  const struct global_var_info *global_variables;
};

static struct kvx_context_info kvx_context;

/* {{{ Debug & Diagnostic  */

/* Flag to decide whether print to stderr information about what is going on.
   Set in init_debug depending on environment variables.  */

enum log_level
{
  NONE,
  ERROR,
  WARN,
  INFO,
  TRACE
} static log_level = WARN;

/* Flag to decide whether the MPPA is treated as 5 separated devices. 
 * Is disabled by default. Set in init environment variables.  */

static bool mppa_multiple_devices;

/* Print a message to stderr.  */

#define DEBUG_PRINT(...) fprintf (stderr, __VA_ARGS__)

/* Flush stderr.  */

#define DEBUG_FLUSH() fflush (stderr)

/* Print a logging message with PREFIX to stderr.  */

#define DEBUG_LOG(prefix, ...)			\
do						\
{						\
  DEBUG_PRINT (prefix);			\
  DEBUG_PRINT (__VA_ARGS__);			\
  DEBUG_FLUSH ();				\
} while (false)

/* Print a debugging message to stderr if the severity is above the defined
 * threshold.  */

#define KVX_LOG(level, ...) 					\
do 								\
{ 								\
 if (level <= log_level) 					\
  { 								\
    char prefix[300];						\
								\
    if (level == TRACE)						\
      sprintf (prefix, "%s: ", __func__);			\
    else if (level == INFO)					\
      sprintf (prefix, "\033[1;34m%s:\033[0m ", __func__); 	\
    else if (level == WARN)					\
      sprintf (prefix, "\033[1;33m%s:\033[0m ", __func__); 	\
    else if (level == ERROR)					\
      sprintf (prefix, "\033[1;31m%s:\033[0m ", __func__); 	\
    DEBUG_LOG (prefix, __VA_ARGS__); 				\
  } 								\
} while (false)

static void kvx_async_queue_pop (struct agent_info *agent,
				 struct kernel_info **kernel);
static struct block_node *kvx_find_block (int agent_n, const void *ptr,
					  struct mppa_gomp_buffer **buf,
					  int *offset, bool pop_p);
static void kvx_kernel_exec (int agent_n, void *fn_ptr);
static void kvx_enqueue_async_kernel (int agent_n, void *fn_ptr);
static void *kvx_async_queue_worker (void *q);
static bool init_kvx_context (void);
static void kvx_init_async_worker (struct agent_info *agent,
				   int cluster_id, int queue_id);
static bool kvx_init_async_context (struct agent_info *agent);
static struct agent_info *get_agent_info (int n);
static bool kvx_init_agent (int n, int version, bool mppa_initialized);
static bool kvx_init_kernel (struct kernel_info *kernel,
			     mppa_offload_sysqueue_t * queue);
static int kvx_get_isa_revision (void *elf_image);
static bool parse_target_attributes (struct kernel_info **kernel);
static struct mppa_gomp_buffer* kvx_alloc (int n, size_t size, int alloc_mode);

/* }}}  */


/* @brief: Returns the ID of the next cluster to run a kernel on agent n, 
   according to the scheduling policy. Right now, the scheduling policy is 
   round robin. Only used for tasks which do not require a specific kernel,
   such as free, alloc, synchronous kernels, etc.  */
static int
kvx_schedule_cluster (int agent_n)
{
  // When using the MPPA clusters as devices each agent is tied to a cluster
  if (mppa_multiple_devices)
    return agent_n;

  struct agent_info *agent = get_agent_info (agent_n);

  if (!agent)
    {
      KVX_LOG (ERROR, "failed to retrieve agent %d\n", agent_n);
      return -1;
    }

  pthread_mutex_lock (agent->schedule_mutex);

  agent->rr_counter = (agent->rr_counter + 1) % NB_SYSTEM_QUEUE;
  int cluster_id = agent->rr_counter;

  pthread_mutex_unlock (agent->schedule_mutex);

  KVX_LOG (TRACE, "%d\n", cluster_id);
  return cluster_id;
}

static void
init_environment_variables (void)
{
  const char *env_log_level = getenv ("KVX_DEBUG");
  if (env_log_level)
    {
      if (!strcmp (env_log_level, "TRACE"))
	log_level = TRACE;
      else if (!strcmp (env_log_level, "INFO"))
	log_level = INFO;
      else if (!strcmp (env_log_level, "WARN"))
	log_level = WARN;
      else if (!strcmp (env_log_level, "ERROR"))
	log_level = ERROR;
      else if (!strcmp (env_log_level, "NONE"))
	log_level = NONE;
    }

  if (secure_getenv ("OMP_MPPA_MULTIPLE_DEVICES"))
    {
      mppa_multiple_devices = true;
      KVX_LOG (INFO, "multiple devices mode\n");
    }
  else
    KVX_LOG (INFO, "single device mode\n");
}

/* @brief: Pop a kernel from goacc_asyncqueue into kernel. */
static void
kvx_async_queue_pop (struct agent_info *agent, struct kernel_info **kernel)
{
  KVX_LOG (TRACE, "start\n");
  /* Shorthand for readability.  */
  struct async_ctx *a_ctx = agent->async_context;
  struct goacc_asyncqueue *queue = a_ctx->async_queue;

  while (1)
    {
      struct goacc_asyncnode *_Atomic head = queue->front;
      struct goacc_asyncnode *_Atomic tail = queue->rear;
      struct goacc_asyncnode *_Atomic next = head->next;
      if (head->count != queue->front->count)
	{
	  continue;
	}
      if (head == tail)
	{
	  if (next == NULL)
	    {
	      *kernel = NULL;
	      return;
	    }

	  atomic_fetch_add (&next->count, 1);
	  atomic_compare_exchange_strong (&queue->rear, &tail, next);
	}
      else
	{
	  struct kernel_info *content = next->kernel;
	  atomic_store (&next->count, head->count + 1);
	  if (atomic_compare_exchange_strong (&queue->front, &head, next))
	    {
	      atomic_fetch_sub (&a_ctx->n_kernels, 1);
	      free (head);
	      *kernel = content;
	      if (atomic_load (&a_ctx->n_kernels) <= MPPA_MAX_ASYNC_KERNELS)
		pthread_cond_broadcast (queue->enqueue_cond);
	      KVX_LOG (TRACE, "end\n");
	      return;
	    }
	}
    }
}

/* \brief Retrieve the buffer (holding the vaddr and paddr on the agent) and an
   offset corresponding to ptr.
   \param ptr: An opaque pointer which is exposed to the host.  This pointer is
   resiliant to pointer arithmetic.
   \param buf: A pointer to a buffer which will hold the real allocated
   addresses
   \param offest: A pointer to an int which will hold the offset by which ptr is
   offsetted with respect to buf.  */
static struct block_node *
kvx_find_block (int agent_n, const void *ptr, struct mppa_gomp_buffer **buf,
		int *offset, bool pop_p)
{
  KVX_LOG (TRACE, "start\n");
  struct agent_info *agent = get_agent_info (agent_n);
  struct block_node *cur = agent->blocks;
  struct block_node *prev = NULL;

  uint64_t ptr_addr = (uint64_t) (uintptr_t) ptr;
  while (cur)
    {
      if ((uint64_t) (ptr_addr - cur->base_addr) < (uint64_t) cur->buffer->size)
	{
	  *buf = cur->buffer;
	  if (offset)
	    *offset = ptr_addr - cur->base_addr;
	  KVX_LOG (INFO, "buf: %p, offset: %d\n", *buf, offset ? *offset : 0);

	  if (pop_p)
	    {
	      if (prev)
		prev->nxt = cur->nxt;
	      else
		agent->blocks = cur->nxt;
	      free (cur);
	    }

	  KVX_LOG (TRACE, "end\n");
	  return cur;
	}
      prev = cur;
      cur = cur->nxt;
    }

  KVX_LOG (WARN, "no result\n");
  return NULL;
}

static void
kvx_kernel_exec (int agent_n, void *fn_ptr)
{
  KVX_LOG (TRACE, "start");
  struct agent_info *agent = get_agent_info (agent_n);
  struct kernel_info *kernel = (struct kernel_info *) fn_ptr;
  mppa_offload_accelerator_t *acc = NULL;
  mppa_offload_sysqueue_t *queue = NULL;

  kernel->agent = agent;
  kernel->kind = KIND_OPENMP;

  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    GOMP_PLUGIN_fatal ("failed to get accelerator.");

  /* get this from kernel */
  uint64_t function_ptr = (uintptr_t) kernel->object;

  /* If the caller is an asynchronous worker, kernel->cluster_id is a valid
     cluster ID and we should use it. Otherwise, schedule a cluster.  */
  int cluster_id = 0;

  if (kernel->cluster_id == -1)
    cluster_id = kvx_schedule_cluster (agent_n);
  else
    cluster_id = kernel->cluster_id;

  /* If the default allocation mode is DDR, send everything to DDR. If not, and
     allocate in the chosen cluster's SMEM.  */
  /* WARN: The allocation mode may be modified by the allocation method
     (such as when the requested size is too big for SMEM). To reaccess the
     same memory space, always use the buffer's alloc_mode field which is set
     after allocation.  */
  int alloc = -1;
  if (OFFLOAD_ALLOC_MODE == MPPA_OFFLOAD_ALLOC_DDR)
    alloc = MPPA_OFFLOAD_ALLOC_DDR;
  else
    alloc = MPPA_OFFLOAD_ALLOC_LOCALMEM_0 + cluster_id;

  /* When a kernel does not have arguments VARS is NULL, this is a problem since
     mppa_offload_exec expects a valid pointer to the argument list.  Thus, we
     allocate a dummy buffer.  */
  struct mppa_gomp_buffer *arg_buffer = NULL;

  int vars_offset = 0;
  if (!kernel->vars)
    arg_buffer = kvx_alloc (agent->id, 32, alloc);
  else
    {
      if (!kvx_find_block (agent_n, kernel->vars, &arg_buffer, &vars_offset,
			   false))
	{
	  KVX_LOG (ERROR, "translation of kernel vars failed!\n");
	  return;
	}
    }

  parse_target_attributes (&kernel);

  int thread_limit = 1;
  int num_teams = 1;

  struct mppa_offload_buffer_exec_cmd *cmd_buffer = NULL;
  if (!(cmd_buffer = calloc (1, sizeof (*cmd_buffer))))
    {
      KVX_LOG (ERROR, "allocation of cmd buffer failed\n");
      return;
    }

  cmd_buffer->virt_func_ptr = (uint64_t) function_ptr;
  cmd_buffer->virt_arg_ptr = (uint64_t) (arg_buffer->vaddr + vars_offset);

  KVX_LOG (INFO, "run: fn_ptr: %lx, vars: %lx\n",
	   cmd_buffer->virt_func_ptr, cmd_buffer->virt_arg_ptr);

  struct mppa_gomp_buffer *cmd_buffer_host = NULL;

  cmd_buffer_host = kvx_alloc (agent->id, sizeof (*cmd_buffer),
			       arg_buffer->alloc_mode);

  GOMP_OFFLOAD_host2dev (agent->id, (void *) cmd_buffer_host->vaddr,
			 cmd_buffer, sizeof (*cmd_buffer));

  free (cmd_buffer);

  int queue_num = cluster_id;

  if (agent->queue_type == RPMSG)
    {
      if (!(queue = mppa_offload_get_sysqueue (acc, queue_num)))
	GOMP_PLUGIN_fatal ("failed to get system queue %d.\n", queue_num);

      if (mppa_offload_buffer_exec (queue, 1, cmd_buffer_host->vaddr, num_teams,
				    1, true, true, NULL))
	{
	  KVX_LOG (ERROR, "[rpmsg] failed\n");
	  return;
	}
      else
	KVX_LOG (INFO, "[rpmsg] success\n");
    }
  else if (agent->queue_type == MMIO)
    {
      if (mppa_multiple_devices)
	queue_num = 0;
      pthread_mutex_lock (&agent->mmio_locks[queue_num]);
      if (mppa_offload_mmio_buffer_exec (&agent->mmio_queues[queue_num], 1,
					 cmd_buffer_host->vaddr, num_teams,
					 thread_limit, true, true, NULL))
	{
	  KVX_LOG (ERROR, "[mmio] failed\n");
	  return;
	}
      else
	KVX_LOG (INFO, "[mmio] success\n");
      pthread_mutex_unlock (&agent->mmio_locks[queue_num]);
    }
  else
    GOMP_PLUGIN_fatal ("couldn't offload kernel, undefined queue type.");

  if (!kernel->vars)
    GOMP_OFFLOAD_free (agent_n, (void *) arg_buffer->vaddr);

  GOMP_OFFLOAD_free (agent_n, (void *) cmd_buffer_host->vaddr);

  KVX_LOG (TRACE, "end\n");
}

/* Add a kernel to the asynchronous execution queue.  */
static void
kvx_enqueue_async_kernel (int agent_n, void *fn_ptr)
{
  KVX_LOG (TRACE, "start\n");
  struct agent_info *agent = get_agent_info (agent_n);
  struct kernel_info *kernel = (struct kernel_info *) fn_ptr;

  /* Shorthand for code clarity.  */
  struct async_ctx *a_ctx = agent->async_context;

  struct goacc_asyncqueue *queue = a_ctx->async_queue;

  struct goacc_asyncnode *_Atomic node =
    GOMP_PLUGIN_malloc_cleared (sizeof (*node));
  node->kernel = kernel;

  while (atomic_load (&a_ctx->n_kernels) >= MPPA_MAX_ASYNC_KERNELS)
    pthread_cond_wait (queue->enqueue_cond, queue->enqueue_mutex);

  while (1)
    {
      struct goacc_asyncnode *_Atomic tail = queue->rear;
      struct goacc_asyncnode *_Atomic next = tail->next;

      if (tail->count != queue->rear->count)
	continue;
      if (next != NULL)
	{
	  atomic_compare_exchange_strong (&queue->rear, &tail, next);
	  continue;
	}
      if (atomic_compare_exchange_strong (&tail->next, &next, node))
	{
	  atomic_compare_exchange_strong (&queue->rear, &tail, node);
	  atomic_fetch_add (&a_ctx->n_kernels, 1);
	  pthread_cond_signal (a_ctx->worker_wakeup_cond);
	  KVX_LOG (TRACE, "end\n");
	  return;
	}
    }
}

/* Execute a queued kernel (from the agent's async kernel queue)
   on sysqueue q.  */
static void *
kvx_async_queue_worker (void *args)
{
  struct kvx_async_worker_params *worker_args = args;
  const int mycluster = worker_args->cluster_id;

  struct agent_info *agent = worker_args->agent;
  struct async_ctx *a_ctx = agent->async_context;

  free (worker_args);

  while (1)
    {
      pthread_mutex_lock (a_ctx->worker_wakeup_mutex);
      pthread_cond_wait (a_ctx->worker_wakeup_cond,
			 a_ctx->worker_wakeup_mutex);
      pthread_mutex_unlock (a_ctx->worker_wakeup_mutex);

      if (a_ctx->async_queue_stop)
	pthread_exit (NULL);

      struct kernel_info *kernel = NULL;
      kvx_async_queue_pop (agent, &kernel);

      if (!kernel)
	{
	  KVX_LOG (INFO, "async kernel is null, skipping\n");
	  continue;
	}

      kernel->cluster_id = mycluster;

      struct agent_info *kernel_agent = kernel->agent;
      kvx_kernel_exec (kernel_agent->id, kernel);

      GOMP_PLUGIN_target_task_completion (kernel->async_data);
      free (kernel);
    }

  return 0;
}

static bool
init_kvx_context (void)
{
  if (kvx_context.initialized_p)
    return true;

  init_environment_variables ();

  mppa_rproc_iterator_t *it;
  if (!(it = mppa_rproc_iter_alloc ()))
    GOMP_PLUGIN_fatal ("Could not alloc rproc iterator.");

  if (!mppa_multiple_devices)
    kvx_context.agent_count = mppa_rproc_count_get (it);
  else
    kvx_context.agent_count = mppa_rproc_count_get (it) * NB_SYSTEM_QUEUE;
  KVX_LOG (INFO, "Found %d offloading devices.\n", kvx_context.agent_count);

  //FIXME free kvx_context at program end; causes memory leaks
  kvx_context.agents =
    GOMP_PLUGIN_malloc_cleared (kvx_context.agent_count
				* sizeof (struct agent_info));

  for (int i = 0; i < kvx_context.agent_count; ++i)
    kvx_context.agents[i].id = i;

  mppa_rproc_iter_free (it);

  kvx_context.initialized_p = true;
  return true;
}

/* Init agent number n's async context.  */
static bool
kvx_init_async_context (struct agent_info *agent)
{
  if (agent->async_context && agent->async_context->initialized_p)
    return true;

  KVX_LOG (INFO, "agent id %d\n", agent->id);
  /* initialize the async context.  */
  agent->async_context =
    GOMP_PLUGIN_malloc_cleared (sizeof *agent->async_context);
  struct async_ctx *async_context = agent->async_context;

  async_context->async_queue =
    GOMP_PLUGIN_malloc_cleared (sizeof (struct goacc_asyncqueue));
  async_context->worker_wakeup_cond =
    GOMP_PLUGIN_malloc_cleared (sizeof (pthread_cond_t));
  async_context->worker_wakeup_mutex =
    GOMP_PLUGIN_malloc_cleared (sizeof (pthread_mutex_t));
  async_context->async_queue_stop = 0;

  async_context->async_queue->enqueue_cond =
    GOMP_PLUGIN_malloc_cleared (sizeof (pthread_cond_t));
  async_context->async_queue->enqueue_mutex =
    GOMP_PLUGIN_malloc_cleared (sizeof (pthread_mutex_t));

  struct goacc_asyncnode *dummy = GOMP_PLUGIN_malloc_cleared (sizeof (*dummy));

  async_context->async_queue->front = dummy;
  async_context->async_queue->rear = dummy;
  async_context->n_kernels = 0;

  pthread_cond_init (async_context->async_queue->enqueue_cond, NULL);
  pthread_mutex_init (async_context->async_queue->enqueue_mutex, NULL);

  pthread_cond_init (async_context->worker_wakeup_cond, NULL);
  pthread_mutex_init (async_context->worker_wakeup_mutex, NULL);

  if (!mppa_multiple_devices)
    async_context->async_ths =
      GOMP_PLUGIN_malloc (NB_SYSTEM_QUEUE * sizeof (pthread_t));
  else
    async_context->async_ths = GOMP_PLUGIN_malloc (sizeof (pthread_t));

  async_context->initialized_p = 1;
  return true;
}

/* Verify that kvx_context has already been initialized and return the
   agent_info structure describing agent number N.  Return NULL on error.  */

static struct agent_info *
get_agent_info (int n)
{
  if (!kvx_context.initialized_p)
    {
      GOMP_PLUGIN_error ("Attempt to use uninitialized KVX MPPA context.");
      return NULL;
    }
  if (n >= kvx_context.agent_count)
    {
      GOMP_PLUGIN_error ("Request to operate on non-existent "
			 "KVX MPPA agent %i", n);
      return NULL;
    }
  if (!kvx_context.agents[n].initialized_p)
    {
      GOMP_PLUGIN_error ("Attempt to use an uninitialized KVX MPPA agent.");
      return NULL;
    }
  return &kvx_context.agents[n];
}

/* Initialize asynchronous worker for an agent  */
static void
kvx_init_async_worker (struct agent_info *agent, int cluster_id, int queue_id)
{
  KVX_LOG (TRACE, "Initializing asnyc queue %d\n", queue_id);
  struct kvx_async_worker_params *params =
    GOMP_PLUGIN_malloc (sizeof (struct kvx_async_worker_params));
  params->agent = agent;
  params->cluster_id = cluster_id;
  if (pthread_create (&agent->async_context->async_ths[queue_id],
		      NULL, kvx_async_queue_worker, params))
    KVX_LOG (ERROR, "failed to create async queue %d\n", queue_id);
}

/* Initialize agent (device) number N so that it can be used for computation.
   Return TRUE on success.  */
static bool
kvx_init_agent (int n, int version, bool mppa_initialized)
{
  struct agent_info *agent = &kvx_context.agents[n];

  if (agent->initialized_p)
    return true;

  if (version == 0x14)
    agent->smem_size = MPPA_DV1_SMEM_SIZE;
  else if (version == 0x23)
    agent->smem_size = MPPA_CV2_SMEM_SIZE;
  else if (version == 0x13)
    agent->smem_size = MPPA_CV1_SMEM_SIZE;
  else
    {
      KVX_LOG (ERROR, "invalid version '%d'", version);
      return false;
    }

  /* Only one agent must initialize the firmware */
  if (!mppa_initialized)
    {
      char *hw_firmware_path = "kalray/opencl/%s%s";

      /* The default firmware used by OpenMP Offload, see
         /lib/firmware/kalray/opencl/ for the full list.  */
      char *default_fw = "ocl_fw_l2_d_1m.elf";

      KVX_LOG (TRACE, "start\n");

      /* If MPPA_RPROC_PLATFORM_MODE == sim, 
       * we are offloading to the simulator. */
      const char *rproc_sim_str = getenv ("MPPA_RPROC_PLATFORM_MODE");
      const bool sim_enabled = rproc_sim_str
	&& !strcmp (rproc_sim_str, "sim");
      char **fw_name =
	&(mppa_offload_cfg_data.accelerator_configs->firmware_name);

      mppa_offload_cfg_data.images =
	GOMP_PLUGIN_malloc (sizeof *(mppa_offload_cfg_data.images));

      char *fw = getenv ("OMP_MPPA_FIRMWARE_NAME");
      if (!fw)
	fw = default_fw;

      *fw_name = GOMP_PLUGIN_malloc (BUF_MAX * sizeof (**fw_name));

      /* If the simulator is not enabled use the hardware firmwares.  */
      if (!sim_enabled)
	{
	  if (snprintf (*fw_name, BUF_MAX, hw_firmware_path,
			version == 0x23 ? "kv3-2/" :
			version == 0x14 ? "kv4-1/" : "", fw) >= BUF_MAX)
	    GOMP_PLUGIN_fatal ("Path to the driver too long. (>= 512)");
	}
      /* Otherwise, lookup the firmware in the kENV of the user.  */
      else
	{
	  char *toolchain = getenv ("KALRAY_TOOLCHAIN_DIR");
	  if (!toolchain)
	    KVX_LOG (WARN, "KALRAY_TOOLCHAIN_DIR not set");

	  if (snprintf
	      (*fw_name, BUF_MAX, "%s/share/pocl/linux_pcie/%s%s", toolchain,
	       version == 0x23 ? "kv3-2/" :
	       version == 0x14 ? "kv4-1/" : "", fw) >= BUF_MAX)
	    KVX_LOG (WARN, "Path to the driver too long. (>= 512)");
	}
      KVX_LOG (INFO, "firmware: %s\n", *fw_name);
    }
  char *queue_type = getenv ("MPPA_OFFLOAD_QUEUE_TYPE");

  /* Controls the access to the block list.  */
  agent->blocks_mutex = GOMP_PLUGIN_malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (agent->blocks_mutex, NULL);

  /* Parse offload queue type, use defaults if not found.  */
  if (queue_type)
    {
      if (!strcmp (queue_type, "RPMSG"))
	agent->queue_type = RPMSG;
      else if (!strcmp (queue_type, "MMIO"))
	agent->queue_type = MMIO;
      else
	GOMP_PLUGIN_fatal ("Unrecognized queue type %s\n", queue_type);
    }
  else
    {
      KVX_LOG (INFO, "Could not resolve offload queue type, using defaults.\n");
      agent->queue_type = MPPA_OFFLOAD_DEFAULT_QUEUE_TYPE;
    }

  /* Init cluster schedule control variables.  */
  /* Round robin counter.  */
  agent->rr_counter = 0;
  /* Controls the access to scheduling variables.  */
  agent->schedule_mutex = GOMP_PLUGIN_malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (agent->schedule_mutex, NULL);
  if (!mppa_initialized
      && mppa_offload_create (&agent->ctx, &mppa_offload_cfg_data))
    KVX_LOG (ERROR, "mppa offload create failed\n");
  else if (agent->queue_type == MMIO)
    {
      if (!mppa_multiple_devices)
	agent->mmio_queues =
	  calloc (NB_SYSTEM_QUEUE, sizeof (*agent->mmio_queues));
      else
	agent->mmio_queues = calloc (1, sizeof (*agent->mmio_queues));
      if (!agent->mmio_queues)
	{
	  KVX_LOG (ERROR, "mmio alloc failed\n");
	}
      else if (mppa_multiple_devices)
	{
	  KVX_LOG (INFO, "mmio alloc success\n");
	  mppa_offload_accelerator_t *acc =
	    mppa_offload_get_accelerator (&agent->ctx, 0);
	  mppa_offload_sysqueue_t *queue =
	    mppa_offload_get_sysqueue (acc, n % NB_SYSTEM_QUEUE);
	  assert (queue);
	  enum mppa_offload_alloc_e mode = OFFLOAD_ALLOC_MODE;
	  if (OFFLOAD_ALLOC_MODE == MPPA_OFFLOAD_ALLOC_LOCALMEM_0)
	    mode = mode + n % NB_SYSTEM_QUEUE;
	  if (mppa_offload_create_mmio_queue
	      (queue, 1, mode, &agent->mmio_queues[0]) != 0)
	    GOMP_PLUGIN_fatal ("mmio create failed\n");
	  else
	    KVX_LOG (INFO, "mmio create success\n");
	}
      else
	{
	  KVX_LOG (INFO, "mmio alloc success\n");
	  for (int i = 0; i < NB_SYSTEM_QUEUE; i++)
	    {
	      mppa_offload_accelerator_t *acc =
		mppa_offload_get_accelerator (&agent->ctx, 0);
	      mppa_offload_sysqueue_t *queue =
		mppa_offload_get_sysqueue (acc, i);
	      assert (queue);

	      enum mppa_offload_alloc_e mode = OFFLOAD_ALLOC_MODE;

	      /* If local memory allocation is enabled, offset enum value by cluster n°.  */
	      if (OFFLOAD_ALLOC_MODE == MPPA_OFFLOAD_ALLOC_LOCALMEM_0)
		mode += i;

	      if (mppa_offload_create_mmio_queue
		  (queue, 1, mode, &agent->mmio_queues[i]) != 0)
		GOMP_PLUGIN_fatal ("mmio create failed\n");
	      else
		KVX_LOG (INFO, "mmio create success\n");
	    }
	}
      KVX_LOG (INFO, "mppa offload create success\n");
    }
  else
    KVX_LOG (INFO, "mppa offload create success\n");

  kvx_init_async_context (agent);

  /* In multiple devices mode each agent will create only one worker, that is
   * tied to a cluster.  */
  if (!mppa_multiple_devices)
    {
      int nb_workers = NB_SYSTEM_QUEUE;
      for (int i = 0; i < nb_workers; ++i)
	{
	  kvx_init_async_worker (agent, i, i);
	}
    }
  else
    kvx_init_async_worker (agent, n % NB_SYSTEM_QUEUE, 0);

  agent->initialized_p = 1;
  KVX_LOG (TRACE, "end\n");
  return true;
}


/* @brief: Initialize a kernel's fields. 
   \param kernel: The kernel to initialize.
   \param queue: The sysqueue to init the kernel on.  */
static bool
kvx_init_kernel (struct kernel_info *kernel, mppa_offload_sysqueue_t * queue)
{
  struct agent_info *agent = kernel->agent;
  if (mppa_offload_query_symbol (queue, agent->reloc_offset, kernel->name,
				 &kernel->object) != 0)
    {
      KVX_LOG (ERROR, "mppa offload query symbol failed\n");
      return false;
    }
  /* function_ptr now contain the function pointer to be used to next calls/runs */
  /* to be saved somewhere for reuse */
  KVX_LOG (INFO, "pulled and saved fn '%s' (0x%lx) from image.\n",
	     kernel->name, kernel->object);
  kernel->cluster_id = -1;
  kernel->initialized_p = true;
  return true;
}

/* \brief Extract the revision number from the flag field of the elf header.
   \param elf_image: A valid elf blob.  */
static int
kvx_get_isa_revision (void *elf_image)
{
  return ((char *) elf_image)[0x31] & 0xff;
}

static bool
parse_target_attributes (struct kernel_info **kernel)
{
  void **input = (*kernel)->args;

  /* Read target arguments (nb of teams, thread_limit) from ARGS.  */
  if (!input)
    return false;

  while (*input)
    {
      intptr_t id = (intptr_t) * input++, val;
      if (id & GOMP_TARGET_ARG_SUBSEQUENT_PARAM)
	val = (intptr_t) * input++;
      else
	val = id >> GOMP_TARGET_ARG_VALUE_SHIFT;
      if ((id & GOMP_TARGET_ARG_DEVICE_MASK) != GOMP_TARGET_ARG_DEVICE_ALL)
	continue;
      val = val > INT_MAX ? INT_MAX : val;
      id &= GOMP_TARGET_ARG_ID_MASK;
      if (id == GOMP_TARGET_ARG_NUM_TEAMS)
	{
	  // KVX_LOG (TRACE, "nb_teams: %ld\n", val);
	  if (val > NB_SYSTEM_QUEUE - 1)
	    val = NB_SYSTEM_QUEUE - 1;
	  // (*kernel)->num_teams = val;
	}
      else if (id == GOMP_TARGET_ARG_THREAD_LIMIT)
	{
	  //TODO check thread limit
	  // KVX_LOG (TRACE, "thread_limit: %ld\n", val);
	  // (*kernel)->thread_limit = val;
	}
    }
  return true;
}

/* {{{ Generic Plugin API  */

/* Return the name of the accelerator, which is "kvx".  */

const char *
GOMP_OFFLOAD_get_name (void)
{
  return "kvx";
}

/* Return the specific capabilities the HSA accelerator have.  */

unsigned int
GOMP_OFFLOAD_get_caps (void)
{
  /* cf. libgomp-plugin.h
     GOMP_OFFLOAD_CAP_SHARED_MEM
     GOMP_OFFLOAD_CAP_OPENMP_400
     GOMP_OFFLOAD_CAP_OPENACC_200  */
  /* Currently the only supported mode is OpenMP offloadind through
     #pragma omp target ... */
  return GOMP_OFFLOAD_CAP_OPENMP_400;
}

/* Identify as KVX accelerator.  */
int
GOMP_OFFLOAD_get_type (void)
{
  return OFFLOAD_TARGET_TYPE_KVX;
}

/* Return the libgomp version number we're compatible with.  There is
   no requirement for cross-version compatibility.  */
unsigned
GOMP_OFFLOAD_version (void)
{
  KVX_LOG (INFO, "version %d\n", GOMP_VERSION);
  return GOMP_VERSION;
}

/* Return the number of KVX devices (agents) on the system.  */
int
GOMP_OFFLOAD_get_num_devices (void)
{
  KVX_LOG (TRACE, "GOMP_OFFLOAD_get_num_devices\n");
  if (!init_kvx_context ())
    return 0;
  return kvx_context.agent_count;
}

bool
GOMP_OFFLOAD_init_device (int n)
{
  if (!init_kvx_context ())
    return false;

  if (n >= kvx_context.agent_count)
    {
      GOMP_PLUGIN_error ("Request to initialize non-existent "
			 "KVX MPPA device %i", n);
      return false;
    }

  return true;
}

/* Load KVX object-code module described by struct kvx_image_desc in
   TARGET_DATA and return references to kernel descriptors in TARGET_TABLE.
   TODO: If there are any constructors then run them.  */
int
GOMP_OFFLOAD_load_image (int target_id, unsigned version,
			 const void *target_data,
			 struct addr_pair **target_table)
{
  KVX_LOG (TRACE, "load_image: start\n");
  struct kvx_image_desc *image_desc = (struct kvx_image_desc *) target_data;

  int proc_version = kvx_get_isa_revision (image_desc->kvx_image->image);
  KVX_LOG (INFO, "Proc ver: %d\n", proc_version);

  struct agent_info *agent = &kvx_context.agents[target_id];

  /* In multiple devices mode only one agent initializes the MPPA and the rest
   * of them have to copy the ctx and reloc_offset. This is required so the elf
   * image is only loaded once.  */
  bool mppa_initialized = false;
  if (mppa_multiple_devices)
    {
      int mppa_id = target_id / NB_SYSTEM_QUEUE;
      for (int i = mppa_id * NB_SYSTEM_QUEUE;
	   i < (mppa_id + 1) * NB_SYSTEM_QUEUE; i++)
	if (i != target_id && kvx_context.agents[i].initialized_p)
	  {
	    mppa_initialized = true;
	    /* Copy reloc_offset from an initialized agent */
	    agent->reloc_offset = kvx_context.agents[i].reloc_offset;
	    /* Copy ctx from an initialized agent */
	    agent->ctx = kvx_context.agents[i].ctx;
	    break;
	  }
    }

  if (!kvx_init_agent (target_id, proc_version, mppa_initialized))
    GOMP_PLUGIN_fatal ("failed to initialize agent");

  struct addr_pair *pair;
  mppa_offload_accelerator_t *acc = NULL;
  mppa_offload_sysqueue_t *queue = NULL;
  struct mppa_gomp_buffer *ptr;

  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    KVX_LOG (WARN, "Failed to get accelerator.");

  /* Size fo the image.  */
  size_t elf_size = image_desc->kvx_image->size;
  /* Pointer to the image location in the host memory.  */
  void *elf_ptr = image_desc->kvx_image->image;

  uint32_t nb_offloaded_functions = image_desc->kernel_count;
  uint32_t nb_offloaded_variables = image_desc->global_variable_count;
  uint32_t nb_offloaded_objects = nb_offloaded_functions + nb_offloaded_variables;

  pair = GOMP_PLUGIN_malloc (nb_offloaded_objects * sizeof (struct addr_pair));
  *target_table = pair;

  if (!mppa_initialized)
    {
      ptr = kvx_alloc (target_id, elf_size, MPPA_OFFLOAD_ALLOC_DDR);

      if (mppa_offload_acc_load
	  (acc, elf_ptr, elf_size, 0, ptr->vaddr, ptr->paddr,
	   &agent->reloc_offset) != 0)
	KVX_LOG (WARN, "mppa offload load failed\n");
      KVX_LOG (INFO, "reloc_offset: %lx\n", agent->reloc_offset);

      GOMP_OFFLOAD_free (target_id, (void *) ptr->vaddr);
    }
  /* Extract symbols from the image to offload.  */
  for (uint32_t i = 0; i < nb_offloaded_functions; i++)
    {
      //FIXME still reachable at program end
      struct kernel_info *kernel = malloc (sizeof (*kernel));
      //NOTE valgrind says there is possibly lost memory here, 
      //but I think it's fine - the kernel pointer is stored in target_table
      //which is reused later on and should be freed appropriately.
      kernel->name = strdup (image_desc->kernel_infos[i].name);
      kernel->initialized_p = false;
      kernel->initialization_failed_p = false;
      kernel->agent = agent;
      pthread_mutex_init (&kernel->init_mutex, NULL);

      pair->start = (uint64_t) (uintptr_t) kernel;
      pair->end = (uint64_t) (uintptr_t) kernel + 1;
      pair++;
    }

  int queue_num = kvx_schedule_cluster (agent->id);
  if (!(queue = mppa_offload_get_sysqueue (acc, queue_num)))
    KVX_LOG (WARN, "Failed to get system queue %d", queue_num);

  for (uint32_t i = 0; i < nb_offloaded_variables; i++)
    {
      struct block_node *block = GOMP_PLUGIN_malloc (sizeof *block);
      block->buffer = GOMP_PLUGIN_malloc_cleared (sizeof (*block->buffer));
      struct mppa_gomp_buffer *var_buf = block->buffer;


      if (mppa_offload_query_symbol (queue, agent->reloc_offset,
				     image_desc->global_variables[i].name,
				     &var_buf->vaddr) != 0)
	KVX_LOG (WARN, "mppa offload query symbol failed\n");
      /* function_ptr now contain the function pointer to be used to next calls/runs */
      /* to be saved somewhere for reuse */
      KVX_LOG (TRACE, "pulled and saved var '%s' "
	       "(0x%lx) from image (%lx - %lx).\n",
	       image_desc->global_variables[i].name, var_buf->vaddr,
	       var_buf->vaddr + image_desc->global_variables[i].size,
	       var_buf->vaddr);
      pair->start = var_buf->vaddr;
      pair->end = var_buf->vaddr + image_desc->global_variables[i].size;
      pair++;
      var_buf->paddr = var_buf->vaddr;
      var_buf->size = image_desc->global_variables[i].size;

      pthread_mutex_lock (agent->blocks_mutex);
      block->base_addr = var_buf->vaddr;
      block->nxt = agent->blocks;
      agent->blocks = block;
      pthread_mutex_unlock (agent->blocks_mutex);
    }

  static bool has_run = false;
  if (!has_run)
    {
      has_run = true;

      /* Special kernel that must run before everything else. */
      struct kernel_info init_kernel = {
	.name = ".omp.offload.device_init",
	.vars = 0,
	.args = 0,
	.initialized_p = false,
	.initialization_failed_p = false,
	.agent = agent,
      };

      /* The MPPA needs to know which device mode is used (single or multiple)
	 in order to exhibit the expected behavior. Thus we pass
	 mppa_multiple_devices to the init kernel.  */

      struct mppa_gomp_buffer *vars_buffer = kvx_alloc (target_id, sizeof (int),
							MPPA_OFFLOAD_ALLOC_DDR);

      if (!vars_buffer)
	KVX_LOG (ERROR, "couldn't alloc init kernel vars\n");

      if (!GOMP_OFFLOAD_host2dev (target_id, (void *) vars_buffer->vaddr,
				  (void *) &mppa_multiple_devices,
				  sizeof (int)))
	KVX_LOG (ERROR, "couldn't host2dev mppa_multiple_devices\n");

      GOMP_OFFLOAD_can_run (&init_kernel);
      GOMP_OFFLOAD_run (target_id, &init_kernel, (void *) vars_buffer->vaddr,
			NULL);
      GOMP_OFFLOAD_free (target_id, (void *) vars_buffer->vaddr);
    }

  KVX_LOG (TRACE, "load_image: end\n");
  return nb_offloaded_objects;
}

/* Unload KVX object-code module described by struct kvx_image_desc in
   TARGET_DATA from agent number N.  Return TRUE on success.  */
bool
GOMP_OFFLOAD_unload_image (int n, unsigned version, const void *target_data)
{
  struct agent_info *agent = get_agent_info (n);
  KVX_LOG (TRACE, "unload_image\n");
  mppa_offload_accelerator_t *acc = NULL;

  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    {
      KVX_LOG (TRACE, "Failed to get accelerator.\n");
      return false;
    }

  if (mppa_offload_acc_unload (acc, (uint64_t) target_data, 0) != 0)
    return false;

  return true;
}

/* Deinitialize all information and status associated with agent number N.  We
   do not attempt any synchronization, assuming the user and libgomp will not
   attempt deinitialization of a agent that is in any way being used at the
   same time. Return TRUE on success.  */
bool
GOMP_OFFLOAD_fini_device (int n)
{
  struct agent_info *agent = get_agent_info (n);

  if (!agent->initialized_p)
    return 1;

  KVX_LOG (TRACE, "fini_device\n");
  /* In multiple devices mode only one agent has to free 
   * the mppa_offload data.  */
  bool mppa_finalized = false;
  if (mppa_multiple_devices)
    {
      int mppa_id = n / NB_SYSTEM_QUEUE;
      for (int i = mppa_id * NB_SYSTEM_QUEUE;
	   i < (mppa_id + 1) * NB_SYSTEM_QUEUE; i++)
	if (i != n && kvx_context.agents[i].finalized_p)
	  {
	    mppa_finalized = true;
	    break;
	  }
    }

  if (!mppa_finalized)
    {
      free (mppa_offload_cfg_data.accelerator_configs->firmware_name);
      free (mppa_offload_cfg_data.images);
    }
  struct block_node *cur = agent->blocks;
  while (cur)
    {
      struct block_node *tmp = cur->nxt;
      free (cur->buffer);
      free (cur);
      cur = tmp;
    }

  if (agent->async_context && agent->async_context->initialized_p)
    {
      struct async_ctx *a_ctx = agent->async_context;
      a_ctx->async_queue_stop = 1;

      for (int i = 0; i < NB_SYSTEM_QUEUE; i++)
	{
	  pthread_cond_broadcast (a_ctx->worker_wakeup_cond);
	  void *thread_return;
	  if (pthread_join (a_ctx->async_ths[i], &thread_return) != 0)
	    KVX_LOG (TRACE, "Error joinining thread %d.\n", i);
          if (thread_return != NULL)
            KVX_LOG (WARN, "async worker %d exited abnormally\n", i);
	  /* In multiple devices mode only wait for one worker */
	  if (mppa_multiple_devices)
	    break;
	}

      struct goacc_asyncnode *to_free =
	agent->async_context->async_queue->front;
      while (to_free)
	{
	  struct goacc_asyncnode *temp = to_free->next;
	  free (to_free);
	  to_free = temp;
	}

      pthread_mutex_destroy (agent->schedule_mutex);
      pthread_mutex_destroy (agent->blocks_mutex);
      pthread_mutex_destroy (a_ctx->worker_wakeup_mutex);
      pthread_mutex_destroy (a_ctx->async_queue->enqueue_mutex);
      free (agent->schedule_mutex);
      free (agent->blocks_mutex);
      free (a_ctx->async_ths);
      free (a_ctx->worker_wakeup_mutex);
      free (a_ctx->async_queue);
      free (a_ctx->worker_wakeup_cond);
      free (a_ctx->async_queue->enqueue_cond);
      free (a_ctx->async_queue->enqueue_mutex);
      free (a_ctx);
    }

  /* NOTE since precond assumes synchronicity, no need to make free thread safe */
  if (agent->queue_type == MMIO)
    free (agent->mmio_queues);

  if (!mppa_finalized && mppa_offload_destroy (&agent->ctx) != 0)
    KVX_LOG (TRACE, "mppa offload destroy failed\n");

  KVX_LOG (TRACE, "mppa offload destroy success\n");
  agent->initialized_p = 0;
  agent->finalized_p = 1;
  return true;
}

/* Return true if the KVX runtime can run function FN_PTR.
   If the kernel is not initialized, try to initialize it,
   the function can be run as long as the initialization succeeds.  */
bool
GOMP_OFFLOAD_can_run (void *fn_ptr)
{
  KVX_LOG (TRACE, "can_run: start\n");
  struct kernel_info *kernel = (struct kernel_info *) fn_ptr;
  struct agent_info *agent = kernel->agent;

  mppa_offload_accelerator_t *acc = NULL;
  mppa_offload_sysqueue_t *queue = NULL;

  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    KVX_LOG (TRACE, "Failed to get accelerator.");
  int queue_num = kvx_schedule_cluster (agent->id);
  if (!(queue = mppa_offload_get_sysqueue (acc, queue_num)))
    KVX_LOG (TRACE, "Failed to get system queue %d", queue_num);

  pthread_mutex_lock (&kernel->init_mutex);

  if (!kernel->initialized_p)
    kvx_init_kernel (kernel, queue);

  pthread_mutex_unlock (&kernel->init_mutex);

  if (kernel->initialization_failed_p)
    {
      KVX_LOG (TRACE, "Kernel %s initialization failed\n", kernel->name);
      KVX_LOG (TRACE, "can_run: end\n");
      return false;
    }

  KVX_LOG (TRACE, "can_run: end\n");
  return true;
}

static struct mppa_gomp_buffer *
kvx_alloc (int n, size_t size, int alloc_mode)
{
  KVX_LOG (TRACE, "alloc: start\n");
  struct mppa_gomp_buffer *buffer = NULL;
  mppa_offload_accelerator_t *acc = NULL;
  mppa_offload_sysqueue_t *queue = NULL;

  struct agent_info *agent = get_agent_info (n);

  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    KVX_LOG (TRACE, "Failed to get accelerator.\n");

  if (!(buffer = GOMP_PLUGIN_malloc (sizeof (*buffer))))
    KVX_LOG (TRACE, "Failed allocate buffer\n");

  if (alloc_mode != MPPA_OFFLOAD_ALLOC_DDR && size >= agent->smem_size)
    {
      KVX_LOG (WARN, "requested size is biggger than SMEM, forcing allocation "
		     "to DDR. \n");
      buffer->alloc_mode = MPPA_OFFLOAD_ALLOC_DDR;
    }
  else
    buffer->alloc_mode = alloc_mode;

  buffer->size = size;

  int queue_num;
  if (buffer->alloc_mode != MPPA_OFFLOAD_ALLOC_DDR)
    queue_num = buffer->alloc_mode - MPPA_OFFLOAD_ALLOC_LOCALMEM_0;
  else
    queue_num = kvx_schedule_cluster (n);

  if (!(queue = mppa_offload_get_sysqueue (acc, queue_num)))
    KVX_LOG (TRACE, "Failed to get system queue %d", 0);

  if (mppa_offload_alloc (queue, size, MPPA_GOMP_DEFAULT_BUFFER_ALIGN,
			  buffer->alloc_mode, &buffer->vaddr, &buffer->paddr)
      != 0)
    {
      KVX_LOG (WARN, "mppa_offload_alloc of size %ld failed\n", size);
      return NULL;
    }

  buffer->size = size;

  KVX_LOG (TRACE, "mppa_offload_alloc success: buffer (%p) = "
	     "(virt: 0x%lx, phys: 0x%lx), size: %lu\n",
	     buffer, buffer->vaddr, buffer->paddr, buffer->size);
  KVX_LOG (TRACE, "alloc: end\n");

  struct block_node *block = malloc (sizeof *block);
  pthread_mutex_lock (agent->blocks_mutex);
  block->base_addr = buffer->vaddr;
  block->buffer = buffer;
  block->nxt = agent->blocks;

  agent->blocks = block;
  pthread_mutex_unlock (agent->blocks_mutex);

  return buffer;
}

/* Allocate memory on agent N.  */
void *
GOMP_OFFLOAD_alloc (int n, size_t size)
{
  int alloc = MPPA_OFFLOAD_ALLOC_LOCALMEM_0 + kvx_schedule_cluster (n);
  struct mppa_gomp_buffer *buf = kvx_alloc (n, size, alloc);
  return (void *) buf->vaddr;
}

/* Free memory from agent N.  */
bool
GOMP_OFFLOAD_free (int agent_n, void *ptr)
{
  mppa_offload_accelerator_t *acc = NULL;
  mppa_offload_sysqueue_t *queue = NULL;
  struct mppa_gomp_buffer *buf = NULL;
  int offset = 0;
  struct agent_info *agent = get_agent_info (agent_n);

  KVX_LOG (TRACE, "free (%d, %p)\n", agent_n, ptr);

  if (!ptr)
    return true;

  if (!kvx_find_block (agent_n, ptr, &buf, &offset, true))
    {
      KVX_LOG (TRACE, "translation failed!\n");
      return false;
    }

  KVX_LOG (TRACE, "free (%d, %p [virt: %p; phys: %p])\n",
	     agent_n, buf, (void *) buf->vaddr, (void *) buf->paddr);

  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    KVX_LOG (TRACE, "Failed to get accelerator.\n");

  int queue_num;

  if (buf->alloc_mode != MPPA_OFFLOAD_ALLOC_DDR)
    queue_num = buf->alloc_mode - MPPA_OFFLOAD_ALLOC_LOCALMEM_0;
  else
    queue_num = kvx_schedule_cluster (agent_n);

  if (!(queue = mppa_offload_get_sysqueue (acc, queue_num)))
    KVX_LOG (TRACE, "Failed to get system queue %d", queue_num);

  if (mppa_offload_free (queue, buf->alloc_mode, buf->vaddr) != 0)
    {
      KVX_LOG (TRACE, "mppa offload free failed\n");
      return false;
    }

  free (buf);
  KVX_LOG (TRACE, "mppa offload free success\n");
  return true;
}

/* Copy data from agent (device) to host.  */
bool
GOMP_OFFLOAD_dev2host (int agent_n, void *dst, const void *src, size_t n)
{
  KVX_LOG (TRACE, "dev2host (%d, %p, %p, %ld): start\n", agent_n, dst, src, n);

  mppa_offload_accelerator_t *acc = NULL;
  struct mppa_gomp_buffer *buf = NULL;
  struct agent_info *agent = get_agent_info (agent_n);
  int offset = 0;

  if (!kvx_find_block (agent_n, src, &buf, &offset, false))
    {
      KVX_LOG (TRACE, "translation failed!\n");
      return false;
    }

  if (!buf)
    {
      KVX_LOG (TRACE, "src pointer is corrupted.");
      return false;
    }

  if (!buf->vaddr || !buf->paddr)
    {
      KVX_LOG (TRACE, "Invalid source address [virt: %p, phys: %p]\n",
		 (void *) buf->vaddr, (void *) buf->paddr);
    }
  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    KVX_LOG (TRACE, "Failed to get accelerator.\n");

  if (offset + n > buf->size)
    {
      KVX_LOG (TRACE, "out of bound access.");
      return false;
    }

  if (mppa_offload_read (acc, dst, buf->paddr + offset, n, NULL) != 0)
    {
      KVX_LOG (TRACE, "mppa offload read dev2host failed\n");
      return false;
    }

  KVX_LOG (TRACE, "mppa offload read dev2host success\n");
  KVX_LOG (TRACE, "dev2host: end\n");
  return true;
}


/* Copy data from host to agent (device).  */
bool
GOMP_OFFLOAD_host2dev (int agent_n, void *dst, const void *src, size_t n)
{
  mppa_offload_accelerator_t *acc = NULL;
  struct mppa_gomp_buffer *buf = NULL;
  struct agent_info *agent = get_agent_info (agent_n);
  int offset = 0;
  KVX_LOG (TRACE, "host2dev(%d, %p, %p, %ld): start\n", agent_n, dst, src, n);

  if (!kvx_find_block (agent_n, dst, &buf, &offset, false))
    {
      KVX_LOG (TRACE, "translation failed!\n");
      return false;
    }

  if (!buf)
    {
      KVX_LOG (TRACE, "dst pointer is corrupted.");
      return false;
    }

  if (!buf->vaddr || !buf->paddr)
    {
      KVX_LOG (TRACE, "Invalid destination address [virt: %p, phys: %p]\n",
		 (void *) (buf->vaddr + offset),
		 (void *) (buf->paddr + offset));
      return false;
    }
  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    KVX_LOG (TRACE, "Failed to get accelerator.\n");

  if (offset + n > buf->size)
    {
      KVX_LOG (TRACE, "out of bound access of size %lu at offset %d "
		      "into a buffer of size %lu.\n", n, offset, buf->size);
      return false;
    }

  if (mppa_offload_write (acc, src, buf->paddr + offset, n, NULL) != 0)
    {
      KVX_LOG (TRACE, "mppa offload write host2dev failed\n");
      KVX_LOG (TRACE, "host2dev: end");
      return false;
    }

  KVX_LOG (TRACE, "mppa offload write host2dev success\n");
  KVX_LOG (TRACE, "host2dev: end\n");
  return true;
}

/* Copy data within agent.  Do the copy asynchronously, if appropriate.  */
bool
GOMP_OFFLOAD_dev2dev (int agent_n, void *dst, const void *src, size_t n)
{
  KVX_LOG (TRACE, "dev2dev start\n");
  char *buffer = malloc (n * sizeof (*buffer));
  if (!buffer)
    GOMP_PLUGIN_fatal ("dev2dev: fail to allocate internal buffer\n");
  GOMP_OFFLOAD_dev2host (agent_n, buffer, src, n);
  GOMP_OFFLOAD_host2dev (agent_n, dst, buffer, n);
  free (buffer);
  return true;
}

/* }}}  */
/* {{{ OpenMP Plugin API  */


/* Run a synchronous OpenMP kernel on an agent and pass it an array of pointers
   in VARS as a parameter.  The kernel is identified by FN_PTR which must point
   to a kernel_info structure, and must have previously been loaded to the
   specified agent.  */
void
GOMP_OFFLOAD_run (int agent_n, void *fn_ptr, void *vars, void **args)
{
  KVX_LOG (TRACE, "GOMP_OFFLOAD_run: start\n");
  struct kernel_info *kernel = (struct kernel_info *) fn_ptr;
  kernel->vars = vars;
  kernel->args = args;
  kvx_kernel_exec (agent_n, kernel);
  KVX_LOG (TRACE, "GOMP_OFFLOAD_run: end\n");
}

/* Run an asynchronous OpenMP kernel on agent.  This is similar to
   GOMP_OFFLOAD_run except that the launch is queued and there is a call to
   GOMP_PLUGIN_target_task_completion when it has finished.  */

void
GOMP_OFFLOAD_async_run (int agent_n, void *tgt_fn, void *tgt_vars,
			void **args, void *async_data)
{
  KVX_LOG (TRACE, "GOMP_OFFLOAD_async_run: start\n");
  /* We must allocate kernel_info since multiple different threads may access
     the same kernel with different data.  */
  struct kernel_info *kernel = GOMP_PLUGIN_malloc (sizeof (struct kernel_info));
  memcpy (kernel, tgt_fn, sizeof (*kernel));
  kernel->vars = tgt_vars;
  kernel->args = args;
  kernel->async_data = async_data;
  kvx_enqueue_async_kernel (agent_n, kernel);
  KVX_LOG (TRACE, "GOMP_OFFLOAD_async_run: end\n");
}

/* }}} */
/* {{{ OpenACC Plugin API  */

/* Run a synchronous OpenACC kernel.  The agent number is inferred from the
   already-loaded KERNEL.  */

void
GOMP_OFFLOAD_openacc_exec (void (*fn_ptr) (void *), size_t mapnum,
			   void **hostaddrs, void **devaddrs, unsigned *dims,
			   void *targ_mem_desc)
{
  struct kernel_info *kernel = (struct kernel_info *) fn_ptr;
  kernel->kind = KIND_OPENACC;
}

/* Run an asynchronous OpenACC kernel on the specified queue.  */

void
GOMP_OFFLOAD_openacc_async_exec (void (*fn_ptr) (void *), size_t mapnum,
				 void **hostaddrs, void **devaddrs,
				 unsigned *dims, void *targ_mem_desc,
				 struct goacc_asyncqueue *aq)
{
}

/* Create a new asynchronous thread and queue for running future kernels.  */

struct goacc_asyncqueue *
GOMP_OFFLOAD_openacc_async_construct (int agent)
{
  return NULL;
}

/* Destroy an existing asynchronous thread and queue.  Waits for any
   currently-running task to complete, but cancels any queued tasks.  */

bool
GOMP_OFFLOAD_openacc_async_destruct (struct goacc_asyncqueue * aq)
{
  return true;
}

/* Return true if the specified async queue is currently empty.  */

int
GOMP_OFFLOAD_openacc_async_test (struct goacc_asyncqueue *aq)
{
  return 0;
}

/* Block until the specified queue has executed all its tasks and the
   queue is empty.  */

bool
GOMP_OFFLOAD_openacc_async_synchronize (struct goacc_asyncqueue * aq)
{
  return true;
}

/* Add a serialization point across two async queues. Any new tasks added to
   AQ2, after this call, will not run until all tasks on AQ1, at the time
   of this call, have completed.  */

bool
GOMP_OFFLOAD_openacc_async_serialize (struct goacc_asyncqueue * aq1,
				      struct goacc_asyncqueue * aq2)
{
  return true;
}

/* Add an opaque callback to the given async queue.  */

void
GOMP_OFFLOAD_openacc_async_queue_callback (struct goacc_asyncqueue *aq,
					   void (*fn) (void *), void *data)
{
}

bool
GOMP_OFFLOAD_openacc_async_host2dev (int agent, void *dst, const void *src,
				     size_t n, struct goacc_asyncqueue *aq)
{
  return true;
}

/* Queue up an asynchronous data copy from agent to host.  */

bool
GOMP_OFFLOAD_openacc_async_dev2host (int agent, void *dst, const void *src,
				     size_t n, struct goacc_asyncqueue * aq)
{
  return true;
}

union goacc_property_value
GOMP_OFFLOAD_openacc_get_property (int agent, enum goacc_property prop)
{
  union goacc_property_value a = a;
  return a;
}

/* Set up plugin-specific thread-local-data (host-side).  */

void *
GOMP_OFFLOAD_openacc_create_thread_data (int ord __attribute__ ((unused)))
{
  return NULL;
}

/* Clean up plugin-specific thread-local-data.  */

void
GOMP_OFFLOAD_openacc_destroy_thread_data (void *data)
{
}

/* }}} */
