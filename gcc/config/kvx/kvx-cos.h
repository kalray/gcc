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
 */
#undef LIB_SPEC
#define LIB_SPEC                                                               \
  "--start-group -lc -lmppacos -lmppa_rsrc -lgloss -lmppa_fdt --end-group "    \
  "%{!nostartfiles:%{!nodefaultlibs:%{!nostdlib:%{!T*:-Tmppacos.ld}}}}"

#undef LINK_SPEC
#define LINK_SPEC                                                              \
  LINK_SPEC_COMMON                                                             \
  "%{pthread:}"

#define TARGET_OS_CPP_BUILTINS()                                               \
  do                                                                           \
    {                                                                          \
      builtin_define ("__CLUSTER_OS__");                                       \
      builtin_assert ("system=clusteros");                                     \
    }                                                                          \
  while (0)
#endif /* GCC_KVX_MPPA_COS */
