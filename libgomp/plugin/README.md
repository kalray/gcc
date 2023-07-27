This is the README for the KVX implementation of the GCC OpenMP (GOMP) plugin,
contained in the plugin-kvx.c file. This README intends to be a quickstart guide to
building, understanding, using and contributing to the plugin.

**All paths are relative to your csw root.**

# Prerequisites

## Init CSW
Checkout your csw to coolidge and run:

`git submodule update --init --recursive`

Run `./get_kalrayReq.sh` then run `./get_packages.sh` 
(specify -i if you want to install the packages as well)

## LLVM OpenCL firmwares
Make sure the cache in csw/opencl is clear:

`git submodule foreach --recursive sh -c "git clean -fdxx"`

Then build the firmwares:

`./valid/hudosn/llvm_opencl.rb @opencl_build`

Make sure they are installed in /lib/firmware/kalray.

## libmppaoffload
`./valid/hudson/host_tools.rb @mppa_offload --build_type=Debug--default-toolroot`

## Configuring the host build directory
`mkdir -p gcc/host_host_accel_Debug_build_64`
`cd gcc/host_host_accel_Debug_build_64`
`../configure`


# Using OpenMP Offloading from x86 to the MPPA

## Compiling programs

If everything went well, `accel-gcc` should be part of your environment.
Source `devimage/toolchain_default/toolroot/.switch_env` and check for its
presence. Don't forget the `-fopenmp` flag when compiling. 

## Running the programs

Use `accel-gcc` to compile programs.

### On x86 with a physical MPPA
You can simply run your compiled program.
### On x86 with the simulator
Change YOUR_SOCKET_PATH in all commands to the path you want your simulator to run
on.

Start the simulator: 
`kvx-cluster -p --disable-cache --march=kv3-1 --no-load-elf --sim-server=SOCKET --mmap --mppa-wdir=YOUR_SOCKET_PATH`
Start the program:
`OMP_MPPA_FIRMWARE_NAME=ocl_fw_l1.elf MPPA_RPROC_PLATFORM_MODE=sim MPPA_RPROC_SIM_PATH=YOUR_SOCKET_PATH ./target-critical-1.exe`


## Environment variables

The plugin only logs errors by default. Change this behavior with the
`KVX_DEBUG` environment variable, which can be set to `NONE`, `ERROR`, `WARN`
and `TRACE` at runtime.

The plugin uses RPROC with sysqueues to communicate with the MPPA by default.
MMIO queues are also available by setting `MPPA_OFFLOAD_QUEUE_TYPE` to `MMIO`
(`RPMSG` also works if you change the default to MMIO) at runtime.

The default firmware path is set in kvx_init_agent, but it can be overridden
with `OMP_MPPA_FIRMWARE_NAME`.

## How to build GCC

As GCC binaries are provided by the get_packages script, this is not a required
step. If packages are missing or if you made changes to generic code, you can 
rebuild gcc with the following commands:

`./valid/hudson/toolchain.rb --default-toolroot @gcc_accel --toolchain=cos`
`./valid/hudson/step2.rb @gcc`

## Debugging programs

Debugging on the host side is as simple as firing up gdb. As the plugin runs
on the host side, this makes the process relatively simple most of the time.
But whenever trying to understand what is happening on the MPPA, involving 
`kvx-gdb` is required.

To debug on the MPPA:

Start gdb on the host app. Place a breakpoint on GOMP_OFFLOAD_run (or async_run).

Start the jtag runner:
`kvx-jtag-runner --hot-attach --force-dsu-init -D --sym-file=Cluster0:YOUR_FIRMWARE_PATH`

Then run `kvx-gdb -ex "attach-mppa 10000`. If you have multiple gdb sessions or
a custom configuration, your port might be different. In your kvx gdb session,
place a breakpoint on the kernel function pointer in the run function
(argument tgt_fn or fn_tgt). Resume execution with `c`. 
Resume execution in your gdb session with `c -a`.

You should see the KVX GDB running the offloaded code, and the rest of the host
app, and so on.

# Contributing

Please format your commits using the script located in gcc/contrib/.
Try to make your one-liner similar to other gcc commits.

# Plugin inner workings

Code compiled with accel-gcc produces a hybrid binary, which contains both x86
ELF and KVX ELF (in the .kvx_code section). Whenever a `target` OpenMP region is
encountered, the plugin offloads the KVX code and all necessary data through the
PCI port to the MPPA using calls to the mppa_offload library. On the MPPA, five
independent instances of OpenMP have been started by ClusterOS - one per cluster.
Five sysqueues (or five MMIO queues depending on the selected queue type) are 
used on the host side to communicate with each cluster.

GCC's libgomp expects exposed functions from our plugin, which it uses to perform
various standard tasks (allocating memory, running a kernel, copying memory from
host to device...). The implementation specifics are up to us of course.

In synchronous mode (when GOMP_OFFLOAD_run is called), the host awaits for the
kernel's completion and thus the main thread itself calls mppa_offload's
buffer_exec, which is blocking until the kernel is done running on the MPPA.

In asynchronous mode (GOMP_OFFLOAD_async_run), the kernel is added to a queue
on which five worker threads (one per cluster) wait for work and pop from the
queue whenever a new kernel is added. The call to buffer_exec is still blocking,
but since the main thread is not taking care of it, it can move on. In this
instance, task completion is signaled using GOMP_TARGET_task_completion.

# Known bugs, limitations

The num_teams and thread_limit clauses are difficult to implement properly 
due to the five OpenMP instances on the MPPA having no apparent link. Attempting
to implement num_teams by telling mppa_offload we needed `num_teams` clusters 
resulted in every team executing on each cluster, instead of having one team run
on one cluster. We would have to somehow split the work manually between them. 

As of now, the teams pragma produces numerically correct results, but the teams
run on a single cluster, severely impacting performance.

Occasionally, libgomp will signal a failed host copy (about 1 in 200 runs). 
Cause unknown.

# Troubleshooting

If you're having issues building binutils or newlib, try 
`rm -rf .metabuild newlib/kvx_build* binutils/kvx_build*`
