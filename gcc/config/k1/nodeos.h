
#define K1_NODEOS

#define LINK_GCC_C_SEQUENCE_SPEC                                               \
  "%{!nostdlib:-( %G %L -lgloss -lnodeos %{pthread*:} %{mhypervisor: "         \
  "-whole-archive -lmOS -no-whole-archive -lvbsp}  -lrt -lboard -lmppa_fdt "   \
  "-lcore %{!T*: -dTnodeos.ld }  %G %L -) }"

#define STARTFILE_SPEC                                                         \
  "crti%O%s crtbegin%O%s crt0%O%s -lcore \
                        %{!nostdlib:%{msoc=*:%:post_suffix_soc(/soc/%{msoc=*:%*} %D)}}"
#define ENDFILE_SPEC "crtend%O%s crtn%O%s"

#define TARGET_DIR "k1-nodeos"

#ifdef GTHREAD_USE_WEAK
#undef GTHREAD_USE_WEAK
#endif

#ifdef _GLIBCXX_GTHREAD_USE_WEAK
#undef _GLIBCXX_GTHREAD_USE_WEAK
#endif

#define _GLIBCXX_GTHREAD_USE_WEAK 0
#define GTHREAD_USE_WEAK 0

#define BOARD_SPEC "%:board_to_startfile_prefix(%{mboard=*:%*})"
#define CLUSTER_SPEC "%:board_to_startfile_prefix(%{mcluster=*:%*})"
#define CORE_SPEC "%:board_to_startfile_prefix(%{mcore=*:%*})/le/bare/"

/* '/board/ BOARD_SPEC / CORE_SPEC' is necessary because we want
   multilib variations inside a same core to share some libraries
   (eg. the BSP), thus we can't rely on the multilib machinery to put
   the right dirs in the list. */
#define STARTFILE_PREFIX_SPEC                                                  \
  TARGET_PREFIX ("/lib/" CORE_SPEC)                                            \
  TARGET_PREFIX ("/board/" BOARD_SPEC)                                         \
  TARGET_PREFIX ("/board/" BOARD_SPEC "/" CORE_SPEC)                           \
  TARGET_PREFIX ("/cluster/" CLUSTER_SPEC "/" CORE_SPEC)                       \
  TARGET_PREFIX ("/core/" CORE_SPEC)                                           \
  TARGET_PREFIX ("/core/")                                                     \
  TARGET_PREFIX ("/")

#define CPP_SPEC " -D__nodeos__ %{mhypervisor: -D__mos__ } " CPP_SPEC_COMMON

/*"%{mcluster=*:%:cluster_to_define(%{mcluster=*:%*})} "		\
  "%{-D__nodeos__}"							\
  "%{mcluster=ioddr: -D__iocomm__}"					\
  "%{mcluster=ioeth: -D__iocomm__}" */

#define K1_DEFAULT_BOARD "%{!mboard: -mboard=csp_generic}"

#define DRIVER_SELF_SPECS DRIVER_SELF_SPECS_COMMON, "%{lpthread: -pthread}"

#define DEFAULT_DP_CORE "k1bdp"

#define K1_SELECT_DP_CORE                                                      \
  "%{march=k1b:-mcore=k1bdp;:-mcore=" DEFAULT_DP_CORE "}"
#define K1_DEFAULT_ARCH "k1b"

#define K1_OS_SELF_SPECS                                                       \
  "%{!march*:%{mcore=k1b*:-march=k1b;:-march=" K1_DEFAULT_ARCH "}} ",          \
    "%{!mcore*:%{mcluster=io*:%eOnly compute cluster (node) is "               \
    "supported;:" K1_SELECT_DP_CORE "}} ",                                     \
    "%{!mcluster*:%{mcore=k1bio:%eOnly compute cluster (node) is "             \
    "supported;mcore=k1bdp:-mcluster=node;:-mcluster=node}} ",                 \
    "%{!mgprel: %{!mno-gprel: -mno-gprel}}",                                   \
    "%{!mno-hypervisor: -mhypervisor}", "%{fpic:-fno-jump-tables} ",           \
    "%{fPIC:-fno-jump-tables} ",                                               \
    "%{!fstack-check*: %{mhypervisor: -fstack-check-use-tls} -fstack-check}",

#define LINK_SPEC LINK_SPEC_COMMON
