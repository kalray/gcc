/* Plugin for KVX MPPA execution.

   Copyright (C) 2022 Free Software Foundation, Inc.

   Contributed by Kalray Inc.

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
#include <mppa_offload_host.h>

/* }}} */

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

#if 1
#define MPPA_OFFLOAD_BOOT_MODE (MPPA_OFFLOAD_BOOT_MODE_DEFAULT)
#else
#define MPPA_OFFLOAD_BOOT_MODE (MPPA_OFFLOAD_BOOT_MODE_DAEMON)
#endif

#define MPPA_OFFLOAD_NB_ACCELERATOR_DEFAULT (1)

#ifndef __kvx__
#define NB_SYSTEM_QUEUE (5)
#define MPPA_OFFLOAD_CC_ID (0)
#else
#define NB_SYSTEM_QUEUE (4)
#define MPPA_OFFLOAD_CC_ID (1)
#endif

#define MPPA_GOMP_DEFAULT_BUFFER_ALIGN (64)

static mppa_offload_host_acc_config_t mppa_offload_host_acc_cfg = {
  .board_id = 0,
  .cluster_id = MPPA_OFFLOAD_CC_ID,
  .nb_sysqueue = NB_SYSTEM_QUEUE,
  .firmware_name = NULL,
};


static mppa_offload_host_config_t mppa_offload_config_data = {
  .common = {
	     .system_queue_type = MPPA_OFFLOAD_QUEUE_RPMSG,
	     .queue_type = MPPA_OFFLOAD_QUEUE_RPMSG,
	     .rdma_type = MPPA_OFFLOAD_RDMA_PCIE,
	     },
  .image_type = MPPA_OFFLOAD_IMAGE_PATH,
  .images = NULL,		/* file system, no meta data */
  .boot_mode = MPPA_OFFLOAD_BOOT_MODE,
  .nb_accelerators = MPPA_OFFLOAD_NB_ACCELERATOR_DEFAULT,
  .accelerator_configs = &mppa_offload_host_acc_cfg,
};

static mppa_offload_host_context_t mppa_offload_context;
static int nb_device = MPPA_OFFLOAD_NB_ACCELERATOR_DEFAULT;
static int is_device_initialized = 0;

struct mppa_gomp_buffer
{
  uint64_t kvx_cpu_virt;
  uint64_t dma_phys;
};

struct kvx_image
{
  size_t size;
  void *image;
};

struct kvx_kernel_description
{
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

static void
init_environment_variables (void)
{
  if (secure_getenv ("KVX_DEBUG"))
    debug = true;
  else
    debug = false;
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
/* FIXME: Is a device a cluster or a whole MPPA? */
int
GOMP_OFFLOAD_get_num_devices (void)
{
  KVX_DEBUG ("get_num_devices\n");
  return nb_device;
}

/* Initialize device (agent) number N so that it can be used for computation.
   Return TRUE on success.  */
bool
kvx_init_device (int n, int version)
{
  char *hw_firmware_path = "kalray/opencl/%s%s";

  /* The default firmware used by OpenMP Offload, see
     /lib/firmware/kalray/opencl/ for the full list.  */
  char *default_fw = "ocl_fw_l2_d_1m.elf";

  if (is_device_initialized)
    return true;

  KVX_DEBUG ("(DEVICE) init_device\n");

  /* If MPPA_RPROC_PLATFORM_MODE == sim, this means that we are offloading to
     the ISS simulator. */
  const char *rproc_sim_str = getenv ("MPPA_RPROC_PLATFORM_MODE");
  const bool sim_en = rproc_sim_str && !strcmp (rproc_sim_str, "sim");
  char **fw_name =
    &(mppa_offload_config_data.accelerator_configs->firmware_name);

  mppa_offload_config_data.images =
    malloc (sizeof *(mppa_offload_config_data.images));

  char *fw = getenv ("OMP_MPPA_FIRMWARE_NAME");
  if (!fw)
    fw = default_fw;

  *fw_name = malloc (512 * sizeof (**fw_name));

  /* If the simulator is not enabled use the hardware firmwares.  */
  if (!sim_en)
    {
      if (snprintf (*fw_name, 512, hw_firmware_path,
		    version == 2 ? "kv3-2/" : "", fw) >= 512)
	KVX_DEBUG ("Path to the driver too long. (>= 512)");
    }
  /* Otherwise, lookup the firmware in the kENV of the user.  */
  else
    {
      char *toolchain = getenv ("KALRAY_TOOLCHAIN_DIR");
      if (!toolchain)
	KVX_DEBUG ("KALRAY_TOOLCHAIN_DIR not set");

      if (snprintf (*fw_name, 512, "%s/share/pocl/linux_pcie/%s%s", toolchain,
		    version == 2 ? "kv3-2/" : "", fw) >= 512)
	KVX_DEBUG ("Path to the driver too long. (>= 512)");
    }
  KVX_DEBUG ("firmware: %s\n", *fw_name);

  if (!mppa_offload_create (&mppa_offload_context, &mppa_offload_config_data))
    KVX_DEBUG ("mppa offload create failed\n");
  KVX_DEBUG ("mppa offload create success\n");
  is_device_initialized = 1;
  return true;
}

bool
GOMP_OFFLOAD_init_device (int n)
{
  init_environment_variables ();
  KVX_DEBUG ("(ENV) init_device\n");

  /* The initialization of the device is done in LOAD_IMAGE because we need to
     know the version of the processor we want to offload to before we can load
     the firmware.  */

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

  /* Scan the flag field of the ELF header to scrap the processor version.  */
  int proc_version =
    (0xf0 & ((char *) image_desc->kvx_image->image)[0x31]) >> 4;
  KVX_DEBUG ("Proc ver: %d\n", proc_version);

  kvx_init_device (target_id, proc_version);

  struct addr_pair *pair;
  mppa_offload_accelerator_t *acc =
    mppa_offload_get_accelerator (&mppa_offload_context, target_id);
  assert (acc);
  mppa_offload_sysqueue_t *queue = mppa_offload_get_sysqueue (acc, 0);
  assert (queue);
  uint64_t virt_ptr, phys_ptr;

  /* Size fo the image.  */
  size_t elf_size = image_desc->kvx_image->size;
  /* Pointer to the image location in the host memory.  */
  void *elf_ptr = image_desc->kvx_image->image;
  /* Handle returned by mppa_offload_load on success,
     this can be thought as a dlopen handle.  */
  uint64_t reloc_offset = 0;

  uint32_t nb_offloaded_functions = image_desc->kernel_count;
  uint32_t nb_offloaded_variables = image_desc->global_variable_count;
  uint32_t nb_offloaded_objects =
    nb_offloaded_functions + nb_offloaded_variables;

  pair =
    GOMP_PLUGIN_malloc (nb_offloaded_objects * sizeof (struct addr_pair));
  *target_table = pair;

  if (mppa_offload_alloc
      (queue, elf_size, MPPA_GOMP_DEFAULT_BUFFER_ALIGN,
       MPPA_OFFLOAD_ALLOC_DDR, &virt_ptr, &phys_ptr) != 0)
    KVX_DEBUG ("mppa offload alloc failed\n");
  if (mppa_offload_acc_load
      (acc, elf_ptr, elf_size, 0, virt_ptr, phys_ptr, &reloc_offset) != 0)
    KVX_DEBUG ("mppa offload load failed\n");
  KVX_DEBUG ("reloc_offset: %lx\n", reloc_offset);
  if (mppa_offload_free (queue, MPPA_OFFLOAD_ALLOC_DDR, virt_ptr) != 0)
    KVX_DEBUG ("mppa offload free failed\n");

  /* Extract symbols from the image to offload.  */
  for (uint32_t i = 0; i < nb_offloaded_functions; i++)
    {
      uint64_t function_ptr;
      if (mppa_offload_query_symbol
	  (queue, reloc_offset, image_desc->kernel_infos[i].name,
	   &function_ptr) != 0)
	KVX_DEBUG ("mppa offload query symbol failed\n");
      /* function_ptr now contain the function pointer to be used to next calls/runs */
      /* to be saved somewhere for reuse */
      KVX_DEBUG ("pulled and saved fn '%s' (0x%lx) from image.\n",
		 image_desc->kernel_infos[i].name, function_ptr);
      pair->start = function_ptr;
      pair->end = function_ptr + 1;
      pair++;
    }

  for (uint32_t i = 0; i < nb_offloaded_variables; i++)
    {
      uint64_t var_ptr;
      if (mppa_offload_query_symbol
	  (queue, reloc_offset, image_desc->global_variables[i].name,
	   &var_ptr) != 0)
	KVX_DEBUG ("mppa offload query symbol failed\n");
      /* function_ptr now contain the function pointer to be used to next calls/runs */
      /* to be saved somewhere for reuse */
      KVX_DEBUG
	("pulled and saved var '%s' (0x%lx) from image (%lx - %lx).\n",
	 image_desc->global_variables[i].name, var_ptr,
	 var_ptr + image_desc->global_variables[i].size, var_ptr);
      pair->start = var_ptr;
      pair->end = var_ptr + image_desc->global_variables[i].size;
      pair++;
    }

  KVX_DEBUG ("load_image: end\n");
  return nb_offloaded_objects;
}

/* Unload KVX object-code module described by struct kvx_image_desc in
   TARGET_DATA from agent number N.  Return TRUE on success.  */
bool
GOMP_OFFLOAD_unload_image (int n, unsigned version, const void *target_data)
{
  KVX_DEBUG ("unload_image\n");
  /* TODO: should unload image.  */
  return true;
}

/* Deinitialize all information and status associated with agent number N.  We
   do not attempt any synchronization, assuming the user and libgomp will not
   attempt deinitialization of a device that is in any way being used at the
   same time.  Return TRUE on success.  */
bool
GOMP_OFFLOAD_fini_device (int n)
{
  KVX_DEBUG ("fini_device\n");

  free (mppa_offload_config_data.accelerator_configs->firmware_name);
  free (mppa_offload_config_data.images);

  if (mppa_offload_destroy (&mppa_offload_context) != 0)
    KVX_DEBUG ("mppa offload destroy failed\n");

  KVX_DEBUG ("mppa offload destroy success\n");
  is_device_initialized = 0;
  return true;
}

/* Return true if the KVX runtime can run function FN_PTR.  */
bool
GOMP_OFFLOAD_can_run (void *fn_ptr)
{
  /* TODO: should be a bit more defensive.  */
  KVX_DEBUG ("can_run: start\n");
  return true;
  KVX_DEBUG ("can_run: end\n");
}

/* Allocate memory on device N.  */
void *
GOMP_OFFLOAD_alloc (int n, size_t size)
{
  KVX_DEBUG ("alloc: start\n");
  struct mppa_gomp_buffer *buffer = malloc (sizeof (*buffer));
  mppa_offload_accelerator_t *acc =
    mppa_offload_get_accelerator (&mppa_offload_context, n);
  assert (acc);
  mppa_offload_sysqueue_t *queue = mppa_offload_get_sysqueue (acc, 0);
  assert (queue);
  assert (buffer);

  if (mppa_offload_alloc
      (queue, size, MPPA_GOMP_DEFAULT_BUFFER_ALIGN, MPPA_OFFLOAD_ALLOC_DDR,
       &buffer->kvx_cpu_virt, &buffer->dma_phys) != 0)
    {
      KVX_DEBUG ("mppa offload alloc failed\n");
      return NULL;
    }
  void *virt = (void *) buffer->kvx_cpu_virt;
  KVX_DEBUG
    ("mppa offload alloc success: buffer = (virt: 0x%lx, phys: 0x%lx)\n",
     buffer->kvx_cpu_virt, buffer->dma_phys);
  free (buffer);
  KVX_DEBUG ("alloc: end\n");
  return virt;
}

/* Free memory from device N.  */
bool
GOMP_OFFLOAD_free (int device, void *ptr)
{
  KVX_DEBUG ("free (%d, %p)\n", device, ptr);
  /* FIXME: use mppa_gomp_buffer */
  /*
     struct mppa_gomp_buffer *buffer = ptr;
     assert(buffer);
     assert(buffer->kvx_cpu_virt);
   */
  /* In DDR virt == phys */
  uint64_t virt_addr = (uintptr_t) ptr;
  mppa_offload_accelerator_t *acc =
    mppa_offload_get_accelerator (&mppa_offload_context, device);
  assert (acc);
  mppa_offload_sysqueue_t *queue = mppa_offload_get_sysqueue (acc, 0);
  assert (queue);

  if (mppa_offload_free (queue, MPPA_OFFLOAD_ALLOC_DDR, virt_addr) != 0)
    {
      KVX_DEBUG ("mppa offload free failed\n");
      return false;
    }
  KVX_DEBUG ("mppa offload free success\n");
  return true;
}

/* Copy data from DEVICE to host.  */
bool
GOMP_OFFLOAD_dev2host (int device, void *dst, const void *src, size_t n)
{
  KVX_DEBUG ("dev2host (%d, %p, %p, %ld): start\n", device, dst, src, n);
  /* FIXME: use mppa_gomp_buffer */
  /*
     struct mppa_gomp_buffer *buffer = src;
     assert(buffer);
     assert(buffer->kvx_cpu_virt);
   */
  /* In DDR virt == phys */
  uint64_t phys_addr = (uintptr_t) src;
  assert (src);
  mppa_offload_accelerator_t *acc =
    mppa_offload_get_accelerator (&mppa_offload_context, device);
  assert (acc);

  if (mppa_offload_read (acc, dst, phys_addr, n, NULL) != 0)
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
  KVX_DEBUG ("host2dev(%d, %p, %p, %ld): start\n", device, dst, src, n);
  /* TODO: use mppa_gomp_buffer
     struct mppa_gomp_buffer *buffer = dst;
     assert(buffer);
     assert(buffer->kvx_cpu_virt);
     assert(buffer->dma_phys);
   */
  uint64_t phys_addr = (uintptr_t) dst;	/* FIXME: In the DDR, virt == phys */
  mppa_offload_accelerator_t *acc =
    mppa_offload_get_accelerator (&mppa_offload_context, device);
  assert (acc);

  if (mppa_offload_write (acc, src, phys_addr, n, NULL) != 0)
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
    {
      KVX_DEBUG ("dev2dev: fail to allocate internal buffer\n");
    }
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
  KVX_DEBUG ("run\n");
  int device_id = 0;		/* check which one ? */
  mppa_offload_accelerator_t *acc =
    mppa_offload_get_accelerator (&mppa_offload_context, device_id);
  assert (acc);
  mppa_offload_sysqueue_t *queue = mppa_offload_get_sysqueue (acc, 0);
  assert (queue);
  uint64_t function_ptr = (uintptr_t) fn_ptr;	/* get this from kernel */
  uint64_t arg_offset = (uintptr_t) vars;	/* address of the paramter of the function_ptr  void (function_ptr*)(void*) */

  if (!vars)
    {
      struct mppa_gomp_buffer *arg_buffer = malloc (sizeof (*arg_buffer));
      if (mppa_offload_alloc (queue, 8 * 4, MPPA_GOMP_DEFAULT_BUFFER_ALIGN,
			      MPPA_OFFLOAD_ALLOC_DDR,
			      &arg_buffer->kvx_cpu_virt,
			      &arg_buffer->dma_phys) != 0)
	{
	  KVX_DEBUG ("mppa offload alloc failed\n");
	  return;
	}
      arg_offset = arg_buffer->kvx_cpu_virt;
    }

#if 1
  if (!args)
    GOMP_PLUGIN_fatal ("No target arguments provided");

  while (*args)
    {
      intptr_t id = (intptr_t) *args++, val;
      if (id & GOMP_TARGET_ARG_SUBSEQUENT_PARAM)
	val = (intptr_t) * args++;
      else
	val = id >> GOMP_TARGET_ARG_VALUE_SHIFT;
      if ((id & GOMP_TARGET_ARG_DEVICE_MASK) != GOMP_TARGET_ARG_DEVICE_ALL)
	continue;
      val = val > INT_MAX ? INT_MAX : val;
      id &= GOMP_TARGET_ARG_ID_MASK;
      if (id == GOMP_TARGET_ARG_NUM_TEAMS)
	KVX_DEBUG ("teams: %ld\n", val);
      else if (id == GOMP_TARGET_ARG_THREAD_LIMIT)
	KVX_DEBUG ("threads: %ld\n", val);
    }
#endif

  KVX_DEBUG ("run: fn_ptr: %p, vars: %p\n", fn_ptr, vars);

  if (mppa_offload_exec
      (queue, function_ptr, arg_offset,
       NULL /*null for blocking call */ ) != 0)
    {
      KVX_DEBUG ("mppa offload exec failed\n");
      return;			// false;
    }
  KVX_DEBUG ("mppa offload exec success\n");
}

/* Run an asynchronous OpenMP kernel on DEVICE.  This is similar to
   GOMP_OFFLOAD_run except that the launch is queued and there is a call to
   GOMP_PLUGIN_target_task_completion when it has finished.  */

void
GOMP_OFFLOAD_async_run (int device, void *tgt_fn, void *tgt_vars,
			void **args, void *async_data)
{
  KVX_DEBUG ("async run\n");
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
