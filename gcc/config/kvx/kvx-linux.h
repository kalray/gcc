/* Machine description for KVX MPPA architecture.
   Copyright (C) 2018 Free Software Foundation, Inc.

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

#ifndef GCC_KVX_MPPA_LINUX
#define GCC_KVX_MPPA_LINUX

#define GLIBC_DYNAMIC_LINKER64 "/lib/ld-linux-kvx.so.1"
#undef MUSL_DYNAMIC_LINKER64
#define MUSL_DYNAMIC_LINKER64 "/lib/ld-musl-kvx.so.1"

#define TARGET_OS_CPP_BUILTINS()                                               \
  do                                                                           \
    {                                                                          \
      GNU_USER_TARGET_OS_CPP_BUILTINS ();                                      \
    }                                                                          \
  while (0)

#define DRIVER_SELF_SPECS DRIVER_SELF_SPECS_COMMON

#define LINK_SPEC                                                              \
  "%{h*}		\
   %{static:-Bstatic}				\
   %{shared:-shared}				\
   %{symbolic:-Bsymbolic}			\
   %{!static:					\
     %{rdynamic:-export-dynamic}		\
     %{!shared:-dynamic-linker " GNU_USER_DYNAMIC_LINKER64 "}} \
   -X"

#undef CPP_SPEC
#define CPP_SPEC "%{pthread:-D_REENTRANT}"
#undef PROFILE_HOOK

#undef FUNCTION_PROFILER
#define FUNCTION_PROFILER(FILE, LABELNO) kvx_output_function_profiler (FILE)
#define PROFILE_REGNO (32)
#define NO_PROFILE_COUNTERS 1

#endif
