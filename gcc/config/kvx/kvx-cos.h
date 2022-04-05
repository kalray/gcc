/* Machine description for KVX MPPA architecture.
   Copyright (C) 2009-2017 Free Software Foundation, Inc.
   Contributed by Kalray.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#ifndef GCC_KVX_MPPA_COS
#define GCC_KVX_MPPA_COS

#undef DRIVER_SELF_SPECS
#define DRIVER_SELF_SPECS						\
  DRIVER_SELF_SPECS_COMMON,						\
    "%{lpthread: -pthread} "						\
    "%{pthread:%<pthread}"


#undef STARTFILE_SPEC
#define STARTFILE_SPEC "%{shared:crtcxa_s%O%s;:crtcxa%O%s}"

#undef ENDFILE_SPEC
#define ENDFILE_SPEC ""

/* The GNU C++ standard library requires that these macros be defined.  */
#undef CPLUSPLUS_CPP_SPEC
#define CPLUSPLUS_CPP_SPEC "-D_GNU_SOURCE %(cpp)"

/* Link against Newlib libraries, because the COS backend assumes Newlib.
   Handle the circular dependence between libc and libgloss.
   Link against MPPA Bare Runtime

   In case of -shared, do not use any library nor linker scripts as nothing is
   provided.
 */
#undef LIB_SPEC
#define LIB_SPEC                                                               \
  "%{!shared:--start-group -lc -lmppacos -lmppa_rsrc -lgloss -lmppa_fdt --end-group " \
  "%{!nostartfiles:%{!nodefaultlibs:%{!nostdlib:%{!T*:-Tmppacos.ld}}}}}"

#undef LINK_SPEC
#define LINK_SPEC                                                              \
  LINK_SPEC_COMMON                                                             \
  "%{pthread:}"								\
  "%{fpie|fPIE|pie:%ePIE (-fpie -pie -fPIE) is not supported in this configuration}"

#define TARGET_OS_CPP_BUILTINS()		\
  do                                                                           \
    {                                                                          \
      builtin_define ("__CLUSTER_OS__");                                       \
      builtin_assert ("system=clusteros");                                     \
    }                                                                          \
  while (0)

/* fixincludes installs include-fixed/limits.h that is problematic.
 * We use limits.h from newlib instead, that will then recurse to it.
 * Unfortunately, this is mostly duplicated from cppdefault.c.
 * Note that Linux also has include dir issues with musl and
 * they do the same to solve their problems.
 */
#ifdef SYSROOT_HEADERS_SUFFIX_SPEC
#define KVX_COS_SYSHDR_SUF_SPEC 1
#else
#define KVX_COS_SYSHDR_SUF_SPEC 0
#endif
#undef INCLUDE_DEFAULTS
#define INCLUDE_DEFAULTS                                                        \
  {                                                                             \
    {GPLUSPLUS_INCLUDE_DIR, "G++", 1, 1, GPLUSPLUS_INCLUDE_DIR_ADD_SYSROOT, 0}, \
      {GPLUSPLUS_TOOL_INCLUDE_DIR,	  "G++", 1, 1,                          \
       GPLUSPLUS_INCLUDE_DIR_ADD_SYSROOT, 1},                                   \
      {GPLUSPLUS_BACKWARD_INCLUDE_DIR,	  "G++", 1, 1,                          \
       GPLUSPLUS_INCLUDE_DIR_ADD_SYSROOT, 0},                                   \
      {GCC_INCLUDE_DIR, "GCC", 0, 0, 0, 0},                                     \
      {CROSS_INCLUDE_DIR, "GCC", 0, 0, 0, 0},                                   \
      {TOOL_INCLUDE_DIR, "BINUTILS", 0, 1, 0, 0},                               \
      {FIXED_INCLUDE_DIR, "GCC", 0, 0, 0, KVX_COS_SYSHDR_SUF_SPEC},             \
    {                                                                           \
      0, 0, 0, 0, 0, 0                                                          \
    }                                                                           \
  }

/* Static library linking does not pull
 * weak symbols in final executable so
 * we force dependencies to be strong (undefined).
 * For instance, libstdc++ and libgcc have
 * weak references on pthread functions and
 * we do not want that as it causes link problems.
 */
#define GTHREAD_USE_WEAK 0

#endif /* GCC_KVX_MPPA_COS */
