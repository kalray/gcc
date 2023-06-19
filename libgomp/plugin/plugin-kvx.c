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
#include <hsa.h>
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

#define OFFLOAD_ALLOC_MODE MPPA_OFFLOAD_ALLOC_DDR
// #define OFFLOAD_ALLOC_MODE MPPA_OFFLOAD_ALLOC_LOCALMEM_0

static mppa_offload_host_acc_config_t mppa_offload_host_acc_cfg = {
  .board_id = 0,
  .cluster_id = MPPA_OFFLOAD_CC_ID,
  .nb_sysqueue = NB_SYSTEM_QUEUE,
  .firmware_name = NULL,
};


/* MPPA_OFFLOAD_QUEUE_TYPE := MPPA_OFFLOAD_QUEUE_RPMSG || MPPA_OFFLOAD_QUEUE_MMIO */
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
//  /* The specific module where the kernel takes place.  */
//  struct module_info *module;
//  /* Information provided by mkoffload associated with the kernel.  */
//  struct hsa_kernel_description *description;
  /* Mutex enforcing that at most once thread ever initializes a kernel for
     use.  A thread should have locked agent->module_rwlock for reading before
     acquiring it.  */
  pthread_mutex_t init_mutex;
//  /* Flag indicating whether the kernel has been initialized and all fields
//     below it contain valid data.  */
  bool initialized;
  /* Flag indicating that the kernel has a problem that blocks an execution.  */
  bool initialization_failed;
  /* The object to be put into the dispatch queue.  */
  uint64_t object;
//  /* Required size of kernel arguments.  */
//  uint32_t kernarg_segment_size;
//  /* Required size of group segment.  */
//  uint32_t group_segment_size;
//  /* Required size of private segment.  */
//  uint32_t private_segment_size;
  /* Set up for OpenMP or OpenACC?  */
  enum offload_kind kind;
  void *async_data;
};

/* An async queue header.

   OpenMP may create one of these.
   OpenACC may create many.  */

struct goacc_asyncqueue
{

  struct kernel_info *kernel;
  void *vars;
  struct goacc_asyncqueue *next;
};

/* Opaque type to represent pointers on the device.  */
struct mppa_gomp_buffer
{
  /* An address in the virtual address space of the MPPA.  */
  uint64_t vaddr;
  /* Its corresponding physical address for use by DMA accesses.  */
  uint64_t paddr;
  /* Its size.  */
  int size;
};

/* A block which has been allocated on the device and whose address is known to
   the host.  It is used to translate back addresses provided by the host which
   have been offsetted.

FIXME: Currently, the index is the virtual address, this should be fixed and
virtual addresses should be used desambiguated by devices and by memory spaces
(DDR, SMEM1/2/.../5).  */
struct block_node
{
  /* The base address of the block.  */
  uint64_t base_addr;
  /* A pointer to the buffer.  */
  struct mppa_gomp_buffer *buffer;
  /* Pointer to the next block.  */
  struct block_node *nxt;
};

/* An agent is an offloading device.  */
struct agent_info
{
  /* Opaque structure containing the state of the device.  */
  mppa_offload_host_context_t ctx;
  /* Is the device initialized?  */
  int initialized_p;
  /* Opaque handle to the loaded elf.  */
  uint64_t reloc_offset;
  /* Mutex garding system queues.  */
  pthread_mutex_t queue_locks[NB_SYSTEM_QUEUE];
  struct block_node *blocks;
  enum queue_type queue_type;
  mppa_offload_mmio_queue_t *mmio_queues;
  int id;
};

struct kvx_context_info
{
  /* Whether the structure has been initialized.  */
  bool initialized_p;
  /* Number of usable KVX MPPA devices in the system.  */
  int agent_count;
  /* Array of agent_info structures describing the individual HSA agents.  */
  struct agent_info *agents;
};

/* Any changes to the layout of the structures kvx_image,
   kvx_kernel_description, global_var_info and kvx_image_desc should be mirrored
   in mkoffload.cc.  They are used to makes sense of the memory mapped image
   that is meant to be copied to the device.  */

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

static bool debug;

/* Print a message to stderr if KVX_DEBUG value is set to true.  */

#define DEBUG_PRINT(...) \
  do \
  { \
    if (debug) \
      { \
	fprintf (stderr, __VA_ARGS__); \
      } \
  } \
  while (false);

/* Flush stderr if KVX_DEBUG value is set to true.  */

#define DEBUG_FLUSH()				\
  do {						\
    if (debug)					\
      fflush (stderr);				\
  } while (false)

/* Print a logging message with PREFIX to stderr if KVX_DEBUG value
   is set to true.  */

#define DEBUG_LOG(prefix, ...)			\
  do						\
    {						\
      DEBUG_PRINT (prefix);			\
      DEBUG_PRINT (__VA_ARGS__);			\
      DEBUG_FLUSH ();				\
    } while (false)

/* Print a debugging message to stderr.  */

#define KVX_DEBUG(...) DEBUG_LOG ("KVX debug: ", __VA_ARGS__)

/* Print a warning message to stderr.  */

#define KVX_WARNING(...) DEBUG_LOG ("KVX warning: ", __VA_ARGS__)

/* }}}  */
static void
init_environment_variables (void)
{
  if (secure_getenv ("KVX_DEBUG"))
    debug = true;
  else
    debug = false;
}

static pthread_t async_ths[NB_SYSTEM_QUEUE];
static pthread_cond_t async_queue_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t async_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static int async_queue_stop = 0;
static struct goacc_asyncqueue *async_queue = NULL;

struct goacc_asyncqueue *
async_queue_enqueue (struct kernel_info *kernel, void *vars,
		     struct goacc_asyncqueue *queue)
{
  struct goacc_asyncqueue *head = GOMP_PLUGIN_malloc (sizeof (*head));
  head->kernel = kernel;
  head->vars = vars;
  head->next = queue;
  return head;
}

void
async_queue_pop (struct goacc_asyncqueue *queue, struct kernel_info **kernel,
		 void **vars)
{
  struct goacc_asyncqueue *to_free = queue;

  *kernel = queue->kernel;
  *vars = queue->vars;

  queue = queue->next;
  free (to_free), to_free = NULL;
}

/* \brief Retrieve the buffer (holding the vaddr and paddr on the device) and an
 * offset corresponding to ptr.
 * \param ptr: An opaque pointer which is exposed to the host.  This pointer is
 * resiliant to pointer arithmetic.
 * \param buf: A pointer to a buffer which will hold the real allocated
 * addresses
 * \param offest: A pointer to an int which will hold the offset by which ptr is
 * offsetted with respect to buf.
 */
static bool
find_block (struct agent_info *agent, const void *ptr,
	    struct mppa_gomp_buffer **buf, int *offset)
{
  struct block_node *cur = agent->blocks;
  uint64_t ptr_addr = (uint64_t) (uintptr_t) ptr;
  while (cur)
    {
      if ((uint64_t) (ptr_addr - cur->base_addr) < (uint64_t) cur->buffer->size)
	{
	  *buf = cur->buffer;
	  if (offset)
	    *offset = ptr_addr - cur->base_addr;
	  KVX_DEBUG ("buf: %p, offset: %d\n", *buf, offset ? *offset : 0);
	  return true;
	}
      cur = cur->nxt;
    }
  return false;
}

static void
kvx_kernel_exec (struct agent_info *agent, void *fn_ptr, void *vars,
		 int async_p)
{

  if (async_p)
    {
      async_queue = async_queue_enqueue ((struct kernel_info *) fn_ptr,
					 vars, async_queue);
      pthread_cond_signal (&async_queue_cond);
    }
  else
    {
      mppa_offload_accelerator_t *acc = NULL;
      mppa_offload_sysqueue_t *queue = NULL;

      struct kernel_info *kernel = (struct kernel_info *) fn_ptr;
      kernel->agent = agent;

      int vars_offset = 0;
      kernel->kind = KIND_OPENMP;

      if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
	KVX_DEBUG ("Failed to get accelerator.\n");

      int queue_num = 0;
      while (queue_num < NB_SYSTEM_QUEUE
	     && (pthread_mutex_trylock (&agent->queue_locks[queue_num]) == EBUSY))
	queue_num++;

      /* If all queues are busy, enqueue on queue 0.  */
      if (queue_num == NB_SYSTEM_QUEUE)
	{
	  queue_num = 0;
	  KVX_DEBUG ("No more queues available, waiting on queue 0.\n");
	  pthread_mutex_lock (&agent->queue_locks[queue_num]);
	}

      KVX_DEBUG ("Locked queue %d\n", queue_num);

      if (agent->queue_type == RPMSG)
	if (!(queue = mppa_offload_get_sysqueue (acc, queue_num)))
	  KVX_DEBUG ("Failed to get system queue %d.\n", queue_num);

      /* get this from kernel */
      uint64_t function_ptr = (uintptr_t) kernel->object;

      /* When a kernel does not have arguments VARS is NULL, this is a problem since
       * mppa_offload_exec expects a valid pointer to the argument list.  Thus, we
       * allocate a dummy buffer.  */
      struct mppa_gomp_buffer *arg_buffer = NULL;
      if (!vars)
	find_block (agent, GOMP_OFFLOAD_alloc (agent->id, 32), &arg_buffer,
		    NULL);
      else
	{
	  if (!find_block (agent, vars, &arg_buffer, &vars_offset))
	    {
	      KVX_DEBUG ("translation failed!\n");
	      return;
	    }
	}

      /* Currently, only used for debug purposes.  */
//      parse_target_attributes (args);

      struct mppa_offload_buffer_exec_cmd *cmd_buffer = NULL;
      if (!(cmd_buffer = calloc (1, sizeof (*cmd_buffer))))
	{
	  KVX_DEBUG ("allocation of cmd buffer failed\n");
	  return;
	}
      cmd_buffer->virt_func_ptr = (uint64_t) function_ptr;
      cmd_buffer->virt_arg_ptr = (uint64_t) (arg_buffer->vaddr + vars_offset);

      KVX_DEBUG ("run: fn_ptr: %lx, vars: %lx\n",
		 cmd_buffer->virt_func_ptr, cmd_buffer->virt_arg_ptr);

      struct mppa_gomp_buffer *cmd_buffer_host = NULL;
      find_block (agent, GOMP_OFFLOAD_alloc (agent->id, sizeof (*cmd_buffer)),
		  &cmd_buffer_host, NULL);
      GOMP_OFFLOAD_host2dev (agent->id, (void *) cmd_buffer_host->vaddr,
			     cmd_buffer, sizeof (*cmd_buffer));

      free (cmd_buffer);

      if (agent->queue_type == RPMSG)
	{
	  if (mppa_offload_buffer_exec (queue, 1, cmd_buffer_host->vaddr, 1,
					1, true, true, NULL))
	    {
	      KVX_DEBUG ("[rpmsg] mppa_offload_exec failed\n");
	      return;
	    }
	  else
	    KVX_DEBUG ("[rpmsg] mppa_offload_exec success\n");
	}
      else			/* MMIO */
	{
	  if (mppa_offload_mmio_buffer_exec (&agent->mmio_queues[queue_num],
					     1, cmd_buffer_host->vaddr, 1, 1,
					     true, true, NULL))
	    {
	      KVX_DEBUG ("[rpmsg] mppa_offload_exec failed\n");
	      return;
	    }
	  else
	    KVX_DEBUG ("[rpmsg] mppa_offload_exec success\n");
	}

      if (!vars)
	{
	  if (mppa_offload_free (queue, MPPA_OFFLOAD_ALLOC_DDR,
				 arg_buffer->vaddr) != 0)
	    KVX_DEBUG ("mppa_offload_free: failed.");
	}

      KVX_DEBUG ("unlock queue %d\n", queue_num);
      pthread_mutex_unlock (&agent->queue_locks[queue_num]);
    }

}

static void *
async_queue_worker (void *q)
{
  mppa_offload_sysqueue_t *queue = q;
  int queue_id = queue->cluster_id;

  while (1)
    {
      if (async_queue_stop)
	break;
      /* cond and mutex control the access to the shared async job queue. */
      pthread_cond_wait (&async_queue_cond, &async_queue_mutex);

      struct kernel_info *kernel = NULL;
      void *vars = NULL;
      async_queue_pop (async_queue, &kernel, &vars);

      struct agent_info *agent = kernel->agent;
      pthread_mutex_lock (&agent->queue_locks[queue_id]);
      kvx_kernel_exec (agent, kernel, vars, false);
      GOMP_PLUGIN_target_task_completion (kernel->async_data);
      /* unlock the queue */
      pthread_mutex_unlock (&agent->queue_locks[queue_id]);
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
    KVX_DEBUG ("Could not alloc rproc iterator.");

  kvx_context.agent_count = mppa_rproc_count_get (it);
  KVX_DEBUG ("Found %d offloading devices.\n", kvx_context.agent_count);

  kvx_context.agents =
    GOMP_PLUGIN_malloc_cleared (kvx_context.agent_count
				* sizeof (struct agent_info));

  for (int i = 0; i < kvx_context.agent_count; ++i)
    kvx_context.agents[i].id = i;

  kvx_context.initialized_p = true;
  return true;
}

/* Verify that kvx_context has already been initialized and return the
   agent_info structure describing device number N.  Return NULL on error.  */

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
			 "KVX MPPA device %i", n);
      return NULL;
    }
  if (!kvx_context.agents[n].initialized_p)
    {
      GOMP_PLUGIN_error ("Attempt to use an uninitialized KVX MPPA agent.");
      asm ("int $3");
      return NULL;
    }
  return &kvx_context.agents[n];
}

/* Initialize device (agent) number N so that it can be used for computation.
   Return TRUE on success.  */
static bool
kvx_init_device (int n, int version)
{
  struct agent_info *agent = &kvx_context.agents[n];

  if (agent->initialized_p)
    return true;

  char *hw_firmware_path = "kalray/opencl/%s%s";

  /* The default firmware used by OpenMP Offload, see
     /lib/firmware/kalray/opencl/ for the full list.  */
  char *default_fw = "ocl_fw_l2_d_1m.elf";

  KVX_DEBUG ("(DEVICE) init_device\n");

  /* If MPPA_RPROC_PLATFORM_MODE == sim, this means that we are offloading to
     the ISS simulator. */
  const char *rproc_sim_str = getenv ("MPPA_RPROC_PLATFORM_MODE");
  const bool sim_enabled = rproc_sim_str && !strcmp (rproc_sim_str, "sim");
  char **fw_name = &(mppa_offload_cfg_data.accelerator_configs->firmware_name);

  mppa_offload_cfg_data.images = GOMP_PLUGIN_malloc (sizeof *(mppa_offload_cfg_data.images));

  char *fw = getenv ("OMP_MPPA_FIRMWARE_NAME");
  if (!fw)
    fw = default_fw;

  *fw_name = GOMP_PLUGIN_malloc (BUF_MAX * sizeof (**fw_name));

  /* If the simulator is not enabled use the hardware firmwares.  */
  if (!sim_enabled)
    {
      if (snprintf (*fw_name, BUF_MAX, hw_firmware_path,
		    version == 2 ? "kv3-2/" : "", fw) >= BUF_MAX)
	KVX_DEBUG ("Path to the driver too long. (>= 512)");
    }
  /* Otherwise, lookup the firmware in the kENV of the user.  */
  else
    {
      char *toolchain = getenv ("KALRAY_TOOLCHAIN_DIR");
      if (!toolchain)
	KVX_DEBUG ("KALRAY_TOOLCHAIN_DIR not set");

      if (snprintf (*fw_name, BUF_MAX, "%s/share/pocl/linux_pcie/%s%s",
		    toolchain, version == 2 ? "kv3-2/" : "", fw) >= BUF_MAX)
	KVX_DEBUG ("Path to the driver too long. (>= 512)");
    }
  KVX_DEBUG ("firmware: %s\n", *fw_name);

  char *queue_type = getenv ("MPPA_OFFLOAD_QUEUE_TYPE");
  if (queue_type && !strcmp (queue_type, "MMIO"))
    agent->queue_type = MMIO;
  else
    agent->queue_type = RPMSG;

  if (mppa_offload_create (&agent->ctx, &mppa_offload_cfg_data))
    KVX_DEBUG ("mppa offload create failed\n");
  else if (agent->queue_type == MMIO)
    {
      agent->mmio_queues =
	calloc (NB_SYSTEM_QUEUE, sizeof (*agent->mmio_queues));
      if (!agent->mmio_queues)
	{
	  KVX_DEBUG ("mmio alloc failed\n");
	  agent->initialized_p = 0;
	  return false;
	}
      else
	{
	  KVX_DEBUG ("mmio alloc success\n");
	  for (int i = 0; i < NB_SYSTEM_QUEUE; i++)
	    {
	      mppa_offload_accelerator_t *acc =
		mppa_offload_get_accelerator (&agent->ctx, 0);
	      mppa_offload_sysqueue_t *queue =
		mppa_offload_get_sysqueue (acc, i);
	      assert (queue);

	      pthread_mutex_lock (&agent->queue_locks[i]);
	      if (mppa_offload_create_mmio_queue
		  (queue, 1, MPPA_OFFLOAD_ALLOC_LOCALMEM_0 + i,
		   &agent->mmio_queues[i]) != 0)
		KVX_DEBUG ("mmio create failed\n");	//TODO exit?
	      else
		KVX_DEBUG ("mmio create success\n");
	      pthread_mutex_unlock (&agent->queue_locks[i]);
	    }
	}
      KVX_DEBUG ("mppa offload create success\n");
    }
  else
    KVX_DEBUG ("mppa offload create success\n");

  mppa_offload_accelerator_t *acc = mppa_offload_get_accelerator (&agent->ctx,
								  0);

  /* Create the async worker queues.  */
  int nb_workers = NB_SYSTEM_QUEUE;
  for (int i = 0; i < nb_workers; ++i)
    {
      mppa_offload_sysqueue_t *queue = mppa_offload_get_sysqueue (acc, i);
      if (pthread_create (&async_ths[i], NULL, async_queue_worker,
			  (void *) queue))
	KVX_DEBUG ("failed to create async queue %d\n", i);
    }

  agent->initialized_p = 1;
  return true;
}


/*
 * \param kernel:
 * \param queue:
 */
static bool
kvx_init_kernel (struct kernel_info *kernel, mppa_offload_sysqueue_t * queue)
{
  struct agent_info *agent = kernel->agent;
  if (mppa_offload_query_symbol (queue, agent->reloc_offset, kernel->name,
				 &kernel->object) != 0)
    {
      KVX_DEBUG ("mppa offload query symbol failed\n");
      return false;
    }
  /* function_ptr now contain the function pointer to be used to next calls/runs */
  /* to be saved somewhere for reuse */
  KVX_DEBUG ("pulled and saved fn '%s' (0x%lx) from image.\n",
	     kernel->name, kernel->object);
  return true;
}

/* \brief Extract the revision number from the flag field of the elf header.
   \param elf_image: A valid elf blob.  */
static int
kvx_get_isa_revision (void *elf_image)
{
  return (((char *) elf_image)[0x31] & 0xf0) >> 4;
}

static bool __attribute__ ((unused))
parse_target_attributes (void **input)
{
  /* Read target arguments (nb of teams, thread_limit) from ARGS.  */
  if (!input)
    GOMP_PLUGIN_fatal ("No target arguments provided");

  while (*input)
    {
      intptr_t id = (intptr_t) *input++, val;
      if (id & GOMP_TARGET_ARG_SUBSEQUENT_PARAM)
	val = (intptr_t) * input++;
      else
	val = id >> GOMP_TARGET_ARG_VALUE_SHIFT;
      if ((id & GOMP_TARGET_ARG_DEVICE_MASK) != GOMP_TARGET_ARG_DEVICE_ALL)
	continue;
      val = val > INT_MAX ? INT_MAX : val;
      id &= GOMP_TARGET_ARG_ID_MASK;
      if (id == GOMP_TARGET_ARG_NUM_TEAMS)
	KVX_DEBUG ("nb_teams: %ld\n", val);
      else if (id == GOMP_TARGET_ARG_THREAD_LIMIT)
	KVX_DEBUG ("thread_limit: %ld\n", val);
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
  KVX_DEBUG ("version %d\n", GOMP_VERSION);
  return GOMP_VERSION;
}

/* Return the number of KVX devices on the system.  */
int
GOMP_OFFLOAD_get_num_devices (void)
{
  KVX_DEBUG ("GOMP_OFFLOAD_get_num_devices\n");
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
  KVX_DEBUG ("load_image: start\n");
  struct kvx_image_desc *image_desc = (struct kvx_image_desc *) target_data;

  int proc_version = kvx_get_isa_revision (image_desc->kvx_image->image);
  KVX_DEBUG ("Proc ver: %d\n", proc_version);

  kvx_init_device (target_id, proc_version);
  struct agent_info *agent = get_agent_info (target_id);

  struct addr_pair *pair;
  mppa_offload_accelerator_t *acc = NULL;
  mppa_offload_sysqueue_t *queue = NULL;
  struct mppa_gomp_buffer *ptr;

  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    KVX_DEBUG ("Failed to get accelerator.");
  if (!(queue = mppa_offload_get_sysqueue (acc, 0)))
    KVX_DEBUG ("Failed to get system queue 0");

  /* Size fo the image.  */
  size_t elf_size = image_desc->kvx_image->size;
  /* Pointer to the image location in the host memory.  */
  void *elf_ptr = image_desc->kvx_image->image;

  uint32_t nb_offloaded_functions = image_desc->kernel_count;
  uint32_t nb_offloaded_variables = image_desc->global_variable_count;
  uint32_t nb_offloaded_objects = nb_offloaded_functions + nb_offloaded_variables;

  pair = GOMP_PLUGIN_malloc (nb_offloaded_objects * sizeof (struct addr_pair));
  *target_table = pair;

  find_block (agent, GOMP_OFFLOAD_alloc (target_id, elf_size), &ptr, NULL);
  if (mppa_offload_acc_load (acc, elf_ptr, elf_size, 0, ptr->vaddr,
			     ptr->paddr, &agent->reloc_offset) != 0)
    KVX_DEBUG ("mppa offload load failed\n");
  KVX_DEBUG ("reloc_offset: %lx\n", agent->reloc_offset);

  GOMP_OFFLOAD_free (0, (void *) ptr->vaddr);

  /* Extract symbols from the image to offload.  */
  for (uint32_t i = 0; i < nb_offloaded_functions; i++)
    {
      struct kernel_info *kernel = malloc (sizeof (*kernel));
      kernel->name = strdup (image_desc->kernel_infos[i].name);
      kernel->initialized = false;
      kernel->initialization_failed = false;
      kernel->agent = agent;
      pthread_mutex_init (&kernel->init_mutex, NULL);

      pair->start = (uint64_t) (uintptr_t) kernel;
      pair->end = (uint64_t) (uintptr_t) kernel + 1;
      pair++;
    }

  for (uint32_t i = 0; i < nb_offloaded_variables; i++)
    {
      struct mppa_gomp_buffer *var_buf = malloc (sizeof (*var_buf));
      if (mppa_offload_query_symbol (queue, agent->reloc_offset,
				     image_desc->global_variables[i].name,
				     &var_buf->vaddr) != 0)
	KVX_DEBUG ("mppa offload query symbol failed\n");
      /* function_ptr now contain the function pointer to be used to next calls/runs */
      /* to be saved somewhere for reuse */
      KVX_DEBUG ("pulled and saved var '%s' (0x%lx) from image "
		 "(%lx - %lx).\n", image_desc->global_variables[i].name,
		 var_buf->vaddr,
		 var_buf->vaddr + image_desc->global_variables[i].size,
		 var_buf->vaddr);
      pair->start = var_buf->vaddr;
      pair->end = var_buf->vaddr + image_desc->global_variables[i].size;
      pair++;
      var_buf->paddr = var_buf->vaddr;
      var_buf->size = image_desc->global_variables[i].size;

      struct block_node *block = malloc (sizeof *block);
      block->base_addr = var_buf->vaddr;
      block->buffer = var_buf;
      block->nxt = agent->blocks;

      agent->blocks = block;
    }

  /* Special kernel that must run before everything else. */
  struct kernel_info init_kernel = {
    .name = ".omp.offload.device_init",
    .vars = 0,
    .args = 0,
    .initialized = false,
    .initialization_failed = false,
    .agent = agent,
  };

  GOMP_OFFLOAD_can_run (&init_kernel);
  GOMP_OFFLOAD_run (target_id, &init_kernel, NULL, NULL);

  KVX_DEBUG ("load_image: end\n");
  return nb_offloaded_objects;
}

/* Unload KVX object-code module described by struct kvx_image_desc in
   TARGET_DATA from agent number N.  Return TRUE on success.  */
bool
GOMP_OFFLOAD_unload_image (int n, unsigned version, const void *target_data)
{
  struct agent_info *agent = get_agent_info (n);
  KVX_DEBUG ("unload_image\n");
  mppa_offload_accelerator_t *acc = NULL;

  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    {
      KVX_DEBUG ("Failed to get accelerator.\n");
      return false;
    }

  if (mppa_offload_acc_unload (acc, (uint64_t) target_data, 0) != 0)
    return false;

  return true;
}

/* Deinitialize all information and status associated with agent number N.  We
   do not attempt any synchronization, assuming the user and libgomp will not
   attempt deinitialization of a device that is in any way being used at the
   same time.  Return TRUE on success.  */
bool
GOMP_OFFLOAD_fini_device (int n)
{
  struct agent_info *agent = get_agent_info (n);

  if (!agent->initialized_p)
    return 1;

  KVX_DEBUG ("fini_device\n");

  free (mppa_offload_cfg_data.accelerator_configs->firmware_name);
  free (mppa_offload_cfg_data.images);

  struct block_node *cur = agent->blocks;
  while (cur)
    {
      struct block_node *tmp = cur->nxt;
      free (cur);
      cur = tmp;
    }

  /* NOTE since precond assumes synchronicity, no need to make free thread safe */
  if (agent->queue_type == MMIO)
    free (agent->mmio_queues);

  if (mppa_offload_destroy (&agent->ctx) != 0)
    KVX_DEBUG ("mppa offload destroy failed\n");

  KVX_DEBUG ("mppa offload destroy success\n");
  agent->initialized_p = 0;
  return true;
}

/* Return true if the KVX runtime can run function FN_PTR.
  If the kernel is not initialized, try to initialize it,
  the function can be run as long as the initialization succeeds.
 */
bool
GOMP_OFFLOAD_can_run (void *fn_ptr)
{
  KVX_DEBUG ("can_run: start\n");
  struct kernel_info *kernel = (struct kernel_info *) fn_ptr;
  struct agent_info *agent = kernel->agent;

  mppa_offload_accelerator_t *acc = NULL;
  mppa_offload_sysqueue_t *queue = NULL;
  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    KVX_DEBUG ("Failed to get accelerator.");

  if (!(queue = mppa_offload_get_sysqueue (acc, 0)))
    KVX_DEBUG ("Failed to get system queue 0");

  pthread_mutex_lock (&kernel->init_mutex);
  if (!kernel->initialized)
    kvx_init_kernel (kernel, queue);
  pthread_mutex_unlock (&kernel->init_mutex);

  if (kernel->initialization_failed)
    {
      KVX_DEBUG ("Kernel %s initialization failed\n", kernel->name);
      KVX_DEBUG ("can_run: end\n");
      return false;
    }

  KVX_DEBUG ("can_run: end\n");
  return true;
}

/* Allocate memory on device N.  */
void *
GOMP_OFFLOAD_alloc (int n, size_t size)
{
  KVX_DEBUG ("alloc: start\n");
  struct mppa_gomp_buffer *buffer = NULL;
  mppa_offload_accelerator_t *acc = NULL;
  mppa_offload_sysqueue_t *queue = NULL;

  struct agent_info *agent = get_agent_info (n);

  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    KVX_DEBUG ("Failed to get accelerator.\n");
  if (!(queue = mppa_offload_get_sysqueue (acc, 0)))
    KVX_DEBUG ("Failed to get queue %d.\n", 0);
  if (!(buffer = GOMP_PLUGIN_malloc (sizeof (*buffer))))
    KVX_DEBUG ("Failed allocate buffer\n");

  if (mppa_offload_alloc (queue, size, MPPA_GOMP_DEFAULT_BUFFER_ALIGN,
			  OFFLOAD_ALLOC_MODE, &buffer->vaddr,
			  &buffer->paddr) != 0)
    {
      KVX_DEBUG ("mppa_offload_alloc failed\n");
      return NULL;
    }

  buffer->size = size;

  KVX_DEBUG ("mppa_offload_alloc success: buffer (%p) = "
	     "(virt: 0x%lx, phys: 0x%lx), size: %lu\n", buffer,
	     buffer->vaddr, buffer->paddr, size);
  KVX_DEBUG ("alloc: end\n");

  struct block_node *block = malloc (sizeof *block);
  block->base_addr = buffer->vaddr;
  block->buffer = buffer;
  block->nxt = agent->blocks;

  agent->blocks = block;

  return (void *) buffer->vaddr;
}

/* Free memory from device N.  */
bool
GOMP_OFFLOAD_free (int device, void *ptr)
{
  mppa_offload_accelerator_t *acc = NULL;
  mppa_offload_sysqueue_t *queue = NULL;
  struct mppa_gomp_buffer *buf = NULL;
  int offset = 0;
  struct agent_info *agent = get_agent_info (device);

  KVX_DEBUG ("free (%d, %p)\n", device, ptr);

  if (!ptr)
    return true;

  if (!find_block (agent, ptr, &buf, &offset))
    {
      KVX_DEBUG ("translation failed!\n");
      return false;
    }

  KVX_DEBUG ("free (%d, %p [virt: %p; phys: %p])\n",
	     device, buf, (void *) buf->vaddr, (void *) buf->paddr);

  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    KVX_DEBUG ("Failed to get accelerator.\n");
  if (!(queue = mppa_offload_get_sysqueue (acc, 0)))
    KVX_DEBUG ("Failed to get queue %d.\n", 0);

  if (mppa_offload_free (queue, OFFLOAD_ALLOC_MODE, buf->vaddr) != 0)
    {
      KVX_DEBUG ("mppa offload free failed\n");
      return false;
    }

  free (buf);
  KVX_DEBUG ("mppa offload free success\n");
  return true;
}

/* Copy data from DEVICE to host.  */
bool
GOMP_OFFLOAD_dev2host (int device, void *dst, const void *src, size_t n)
{
  KVX_DEBUG ("dev2host (%d, %p, %p, %ld): start\n", device, dst, src, n);

  mppa_offload_accelerator_t *acc = NULL;
  struct mppa_gomp_buffer *buf = NULL;
  struct agent_info *agent = get_agent_info (device);
  int offset = 0;

  if (!find_block (agent, src, &buf, &offset))
    {
      KVX_DEBUG ("translation failed!\n");
      return false;
    }

  if (!buf)
    {
      KVX_DEBUG ("src pointer is corrupted.");
      return false;
    }

  if (!buf->vaddr || !buf->paddr)
    {
      KVX_DEBUG ("Invalid source address [virt: %p, phys: %p]\n",
		 (void *) buf->vaddr, (void *) buf->paddr);
    }
  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    KVX_DEBUG ("Failed to get accelerator.\n");

  if (offset + n >= buf->size)
    {
      KVX_DEBUG ("out of bound access.");
      return false;
    }

  if (mppa_offload_read (acc, dst, buf->paddr + offset, n, NULL) != 0)
    {
      KVX_DEBUG ("mppa offload read dev2host failed\n");
      return false;
    }

  KVX_DEBUG ("mppa offload read dev2host success\n");
  KVX_DEBUG ("dev2host: end\n");
  return true;
}


/* Copy data from host to DEVICE.  */

bool
GOMP_OFFLOAD_host2dev (int device, void *dst, const void *src, size_t n)
{
  mppa_offload_accelerator_t *acc = NULL;
  struct mppa_gomp_buffer *buf = NULL;
  struct agent_info *agent = get_agent_info (device);
  int offset = 0;
  KVX_DEBUG ("host2dev(%d, %p, %p, %ld): start\n", device, dst, src, n);

  if (!find_block (agent, dst, &buf, &offset))
    {
      KVX_DEBUG ("translation failed!\n");
      return false;
    }

  if (!buf)
    {
      KVX_DEBUG ("dst pointer is corrupted.");
      return false;
    }

  if (!buf->vaddr || !buf->paddr)
    {
      KVX_DEBUG ("Invalid destination address [virt: %p, phys: %p]\n",
		 (void *) (buf->vaddr + offset),
		 (void *) (buf->paddr + offset));
      return false;
    }
  if (!(acc = mppa_offload_get_accelerator (&agent->ctx, 0)))
    KVX_DEBUG ("Failed to get accelerator.\n");

  if (offset + n > buf->size)
    {
      KVX_DEBUG ("out of bound access of size %lu at offset %d into a "
		 "buffer of size %d.\n", n, offset, buf->size);
      return false;
    }

  if (mppa_offload_write (acc, src, buf->paddr + offset, n, NULL) != 0)
    {
      KVX_DEBUG ("mppa offload write host2dev failed\n");
      KVX_DEBUG ("host2dev: end");
      return false;
    }

  KVX_DEBUG ("mppa offload write host2dev success\n");
  KVX_DEBUG ("host2dev: end\n");
  return true;
}

/* Copy data within DEVICE.  Do the copy asynchronously, if appropriate.  */
bool
GOMP_OFFLOAD_dev2dev (int device, void *dst, const void *src, size_t n)
{
  KVX_DEBUG ("dev2dev\n");
  char *buffer = malloc (n * sizeof (*buffer));
  if (!buffer)
    KVX_DEBUG ("dev2dev: fail to allocate internal buffer\n");
  GOMP_OFFLOAD_dev2host (device, buffer, src, n);
  GOMP_OFFLOAD_host2dev (device, dst, buffer, n);
  free (buffer);
  return true;
}

/* }}}  */
/* {{{ OpenMP Plugin API  */


/* Run a synchronous OpenMP kernel on DEVICE and pass it an array of pointers
   in VARS as a parameter.  The kernel is identified by FN_PTR which must point
   to a kernel_info structure, and must have previously been loaded to the
   specified device.  */
void
GOMP_OFFLOAD_run (int device, void *fn_ptr, void *vars, void **args)
{
  KVX_DEBUG ("GOMP_OFFLOAD_run: start\n");
  struct agent_info *agent = get_agent_info (device);
  kvx_kernel_exec (agent, fn_ptr, vars, false);
}

/* Run an asynchronous OpenMP kernel on DEVICE.  This is similar to
   GOMP_OFFLOAD_run except that the launch is queued and there is a call to
   GOMP_PLUGIN_target_task_completion when it has finished.  */

void
GOMP_OFFLOAD_async_run (int device, void *tgt_fn, void *tgt_vars,
			void **args, void *async_data)
{
  KVX_DEBUG ("async run\n");
  struct kernel_info *kernel = tgt_fn;
  struct agent_info *agent = get_agent_info (device);
  kernel->async_data = async_data;
  kvx_kernel_exec (agent, kernel, tgt_vars, true);
}

/* }}} */
/* {{{ OpenACC Plugin API  */

/* Run a synchronous OpenACC kernel.  The device number is inferred from the
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
GOMP_OFFLOAD_openacc_async_construct (int device)
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
GOMP_OFFLOAD_openacc_async_host2dev (int device, void *dst, const void *src,
				     size_t n, struct goacc_asyncqueue *aq)
{
  return true;
}

/* Queue up an asynchronous data copy from DEVICE to host.  */

bool
GOMP_OFFLOAD_openacc_async_dev2host (int device, void *dst, const void *src,
				     size_t n, struct goacc_asyncqueue * aq)
{
  return true;
}

union goacc_property_value
GOMP_OFFLOAD_openacc_get_property (int device, enum goacc_property prop)
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
