/* Definitions for rtems targeting a K1 using ELF.
   Copyright (C) 2010,2011, Kalray, SA

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

/* Target OS builtins.  */

#define K1_RTEMS

#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()                                               \
  do                                                                           \
    {                                                                          \
      builtin_define ("__rtems__");                                            \
      builtin_define ("__USE_INIT_FINI__");                                    \
      builtin_assert ("system=rtems");                                         \
    }                                                                          \
  while (0)

#define TARGET_DIR "k1-rtems"

#define CPP_SPEC                                                               \
  CPP_SPEC_COMMON " -idirafter " TARGET_PREFIX (                               \
    "/%:cluster_board_to_bsp(%{mcluster=*:%*} %{mprocessing=*:%*} "            \
    "/" MCORE_SPEC                                                             \
    ") /k1-rtems/ %:cluster_board_to_bsp(%{mcluster=*:%*} "                    \
    "%{mprocessing=*:%*} /lib/include)") " -idirafter " TARGET_PREFIX ("/%:"   \
								       "clust" \
								       "er_"   \
								       "board" \
								       "_to_"  \
								       "bsp(%" \
								       "{mclu" \
								       "ster=" \
								       "*:%*}" \
								       " %{"   \
								       "mproc" \
								       "essin" \
								       "g=*:%" \
								       "*} / " \
								       ") "    \
								       "/k1-"  \
								       "rtems" \
								       "/incl" \
								       "ude/")

#undef STARTFILE_SPEC
#define STARTFILE_SPEC                                                         \
  "crti%O%s crtbegin%O%s %{qrtems: start%O%s} %{!qrtems:crt0%O%s} \
                        %{!nostdlib:%{msoc=*:%:post_suffix_soc(/soc/%{msoc=*:%*} %D)}}"
#undef ENDFILE_SPEC
#define ENDFILE_SPEC "crtend%O%s crtn%O%s"

/* #define LINK_GCC_C_SEQUENCE_SPEC "%{!nostdlib:-( %G %L %{qrtems: -lnodeos} -)
 * }" */
#define LINK_GCC_C_SEQUENCE_SPEC                                               \
  "%{!nostdlib:-( %G %L %{qrtems: %{mcluster=ioddr: %{!m64: -whole-archive "   \
  "-lk1_debug_server -no-whole-archive}}} %G -) }"
#define BOARD_SPEC "%:board_to_startfile_prefix(%{mboard=*:%*})"
#define CLUSTER_SPEC "%:board_to_startfile_prefix(%{mcluster=*:%*})"
#define CORE_SPEC "%:board_to_startfile_prefix(%{mcore=*:%*})/le/bare/"
#define MCORE_SPEC "%:board_to_startfile_prefix(%{mcore=*:%*})"

/* '/board/ BOARD_SPEC / CORE_SPEC' is necessary because we want
   multilib variations inside a same core to share some libraries
   (eg. the BSP), thus we can't rely on the multilib machinery to put
   the right dirs in the list. */
#define STARTFILE_PREFIX_SPEC                                                  \
  TARGET_PREFIX ("/%:cluster_board_to_bsp(%{mcluster=*:%*} "                   \
		 "%{mprocessing=*:%*} /" MCORE_SPEC ") /k1-rtems/lib/")        \
  TARGET_PREFIX ("/%:cluster_board_to_bsp(%{mcluster=*:%*} "                   \
		 "%{mprocessing=*:%*} /" MCORE_SPEC                            \
		 ") /k1-rtems/ %:cluster_board_to_bsp(%{mcluster=*:%*} "       \
		 "%{mprocessing=*:%*} /lib/)")                                 \
  TARGET_PREFIX ("/board/" BOARD_SPEC)                                         \
  TARGET_PREFIX ("/board/" BOARD_SPEC "/" CORE_SPEC)                           \
  TARGET_PREFIX ("/cluster/" CLUSTER_SPEC "/" CORE_SPEC)                       \
  TARGET_PREFIX ("/core/" CORE_SPEC)                                           \
  TARGET_PREFIX ("/core/")                                                     \
  TARGET_PREFIX ("/")

#define K1_DEFAULT_BOARD                                                       \
  "%{!mprocessing: -mprocessing=single} %{!mboard: -mboard=csp_generic}"

#define DRIVER_SELF_SPECS                                                      \
  DRIVER_SELF_SPECS_COMMON, "%{lpthread|pthread: -qrtems} %<pthread "          \
			    "%<lpthread"

#define DEFAULT_IO_CORE "k1bio"

#define K1_SELECT_IO_CORE                                                      \
  "%{march=k1b:-mcore=k1bio;:-mcore=" DEFAULT_IO_CORE "}"
#define K1_DEFAULT_ARCH "k1b"

#define K1_OS_SELF_SPECS                                                       \
  "%{!march*:%{mcore=k1b*:-march=k1b;:-march=" K1_DEFAULT_ARCH "}} ",          \
    "%{!mcore*:%{mcluster=node:%eOnly IO cluster is "                          \
    "supported;:" K1_SELECT_IO_CORE "}} ",                                     \
    "%{!mcluster*:%{mcore=k1bdp:%eOnly IO cluster is "                         \
    "supported;mcore=k1bio:-mcluster=ioddr;:-mcluster=ioddr}} ",               \
    "%{!fstack-check*: -fstack-check}",

#define GOMP_SELF_SPECS "%{fopenmp|ftree-parallelize-loops=*: -qrtems}"

/* This macro is now poisoned by GCC */
/* #define HANDLE_PRAGMA_PACK_PUSH_POP 1 */

#undef LIB_SPEC
#define LIB_SPEC                                                               \
  "%{!qrtems: " STD_LIB_SPEC "} "                                              \
  "%{!nostdlib: %{qrtems: --start-group -lboard -lmppa_fdt -lrtemsbsp "        \
  "-lrtemscpu %{mprocessing=multi: -lamp} -lc -lgcc --end-group "              \
  "%{!qnolinkcmds: -T linkcmds%s}}}"

#define LINK_SPEC LINK_SPEC_COMMON
