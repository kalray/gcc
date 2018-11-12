/* Machine description for K1 MPPA architecture.
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

#ifndef GCC_K1_MPPA_COS
#define GCC_K1_MPPA_COS

#define STARTFILE_SPEC " crti%O%s crtbegin%O%s crt0%O%s"
#define ENDFILE_SPEC " crtend%O%s crtn%O%s"

#define DRIVER_SELF_SPECS                                                      \
  DRIVER_SELF_SPECS_COMMON, "%{lpthread: -pthread} "                           \
			    "%{pthread:%<pthread}"

/* Link against Newlib libraries, because the COS backend assumes Newlib.
   Handle the circular dependence between libc and libgloss.
   Link against MPPA Bare Runtime
 */
#undef LIB_SPEC
#define LIB_SPEC                                                               \
  "--start-group -lmppacos -lc -lgloss --end-group "                           \
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
#endif /* GCC_K1_MPPA_COS */
