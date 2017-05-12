
#define K1_BARE

// FIXME AUTO: handle kdebug better (use a -m[no-]debug for example
// FIXME AUTO: disabled debug server
/* %{mcluster=ioddr: %{!m64: -whole-archive -lk1_debug_server
 * -no-whole-archive}} */
#define LINK_GCC_C_SEQUENCE_SPEC                                               \
  "%{!nostdlib:-( %G %L -lgloss %{mhypervisor: -whole-archive -lmOS "          \
  "-no-whole-archive -lvbsp -lutask} %{mcluster=node_msd:-lboard_msd "         \
  "-lmppa_fdt;:-lboard -lmppa_fdt} -lcore %{!T*: %{mhypervisor: %Tlink.ld;: "  \
  "%Tplatform.ld}}  %G %L -) }"

#define STARTFILE_SPEC                                                         \
  "crti%O%s crtbegin%O%s crt0%O%s -lcore \
                        %{!nostdlib:%{msoc=*:%:post_suffix_soc(/soc/%{msoc=*:%*} %D)}}"
#define ENDFILE_SPEC "crtend%O%s crtn%O%s"

#define TARGET_DIR "k1-elf"

#define BOARD_SPEC "%:board_to_startfile_prefix(%{mboard=*:%*})"
#define CLUSTER_SPEC "%:board_to_startfile_prefix(%{mcluster=*:%*})"
#define CORE_SPEC "%:board_to_startfile_prefix(%{mcore=*:%*})/le/bare/"

/* '/board/ BOARD_SPEC / CORE_SPEC' is necessary because we want
   multilib variations inside a same core to share some libraries
   (eg. the BSP), thus we can't rely on the multilib machinery to put
   the right dirs in the list. */
#define STARTFILE_PREFIX_SPEC                                                  \
  TARGET_PREFIX ("/board/" BOARD_SPEC)                                         \
  TARGET_PREFIX ("/board/" BOARD_SPEC "/" CORE_SPEC)                           \
  TARGET_PREFIX ("/cluster/" CLUSTER_SPEC)                                     \
  TARGET_PREFIX ("/cluster/" CLUSTER_SPEC "/" CORE_SPEC)                       \
  TARGET_PREFIX ("/core/" CORE_SPEC)                                           \
  TARGET_PREFIX ("/core/")                                                     \
  TARGET_PREFIX ("/")

#define CPP_SPEC                                                               \
  "-D__bare__ %{mhypervisor: -D__mos__ } "                                     \
  "%{pthread: %eThe -pthread option is not supported in bare "                 \
  "mode.}" CPP_SPEC_COMMON

#define K1_DEFAULT_BOARD "-mboard=csp_generic"

#define DRIVER_SELF_SPECS DRIVER_SELF_SPECS_COMMON, "%{lpthread: -pthread}"

/* #define DEFAULT_RM_CORE "k1rm" */
/* #define DEFAULT_PE_CORE "k1pe" */
/* #define K1_SELECT_RM_CORE "%{march=k1c:-mcore=k1rm;:-mcore=" DEFAULT_IO_CORE
 * "}" */
/* #define K1_SELECT_PE_CORE "%{march=k1b:-mcore=k1bdp;:-mcore=" DEFAULT_DP_CORE
 * "}" */

#define K1_DEFAULT_ARCH "k1c"
#define K1_DEFAULT_CORE "k1pe"
#define K1_DEFAULT_CLUSTER "ioddr"

/* "%{!march*:%{mcore=k1b*:-march=k1b;:-march=" K1_DEFAULT_ARCH "}} ",
 * \ */
/* "%{!mcore*:%{mcluster=io*:" K1_SELECT_IO_CORE ";:%{mboard=iocomm_*:"
 * K1_SELECT_IO_CORE ";:" K1_SELECT_DP_CORE "}}} ", \ */

#define K1_OS_SELF_SPECS                                                       \
  "%{!march*:-march=" K1_DEFAULT_ARCH " } "                                    \
  "%{!mcore*:-mcore=" K1_DEFAULT_CORE " } "                                    \
  "%{fpic:-fno-jump-tables} ",                                                 \
    "%{fPIC:-fno-jump-tables} ",                                               \
    "%{!mcluster*:-mcluster=" K1_DEFAULT_CLUSTER "}",

#define LINK_SPEC "%{pthread:-lpthread}" LINK_SPEC_COMMON
