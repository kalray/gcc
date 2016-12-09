#define K1_LINUX

#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS() GNU_USER_TARGET_OS_CPP_BUILTINS ()

#undef ASM_APP_OFF
#define ASM_APP_OFF "\n\t;;\n#NO_APP\n"

#define TARGET_DIR "k1-linux"

#define K1_DEFAULT_BOARD "-mboard=csp_generic"

#define DRIVER_SELF_SPECS DRIVER_SELF_SPECS_COMMON

/* #define STARTFILE_PREFIX_SPEC                                           \ */
/*     "/%:concat(%{mcore=*:%*} /usr/lib/)" */

#undef STARTFILE_SPEC
#define STARTFILE_SPEC                                                         \
  "%{!shared: %{pg|p|profile:gcrt1.o%s;pie:Scrt1.o%s;:crt1.o%s} %{mfdpic:crtreloc.o%s} } \
   crti.o%s %{shared|pie:crtbeginS.o%s;:crtbegin.o%s} \
   %{!nostdlib:%{msoc=*:%:post_suffix_soc(/soc/%{msoc=*:%*} %D)}}"

#define CPP_SPEC                                                               \
  CPP_SPEC_COMMON                                                              \
  "%{pthread:-D_REENTRANT} "

#undef MULTILIB_DEFAULT
#define MULTILIB_DEFAULT                                                       \
  {                                                                            \
    ""                                                                         \
  }

#define K1_SELECT_IO_CORE "-mcore=k1bio"

#define K1_OS_SELF_SPECS                                                       \
  "%{!march*:-march=k1b} ",                                                    \
    "%{march=k1a:%eCompiler can only be used with march=k1b} ",                \
    "%{mcore=k1dp|mcore=k1bdp|mcore=k1io:%eCompiler can only be used with "    \
    "mcore=k1bio}",                                                            \
    "%{!mcore*:" K1_SELECT_IO_CORE "} ", "%{!mcluster*:-mcluster=ioddr} ",     \
    "%{fpic:-fno-jump-tables} ", "%{fPIC:-fno-jump-tables} ",                  \
    "%{!mno-fdpic:-mfdpic} ",                                                  \
    "%{mfdpic: %{fno-pic:%efno-pic not available on linux for userspace } } "

/*
 * This is directly dependent on MULTILIB_OSDIRNAMES set in t-linux target
 * specific file
 */
#define CHOOSE_K1_DYNAMIC_LINKER_PATH                                          \
  "%{mfdpic:}"                                                                 \
  "%{mno-fdpic:nofdpic/}"

#undef LINK_SPEC
#define LINK_SPEC                                                              \
  "\
  %{mfdpic: -m elf32k1_linux -z text -z now} \
  %{mno-fdpic: -m elf32k1} \
  %{pthread: -lpthread} \
  %{shared} %{pie} \
  %{!shared: %{!static: \
   %{rdynamic:-export-dynamic} \
   -dynamic-linker /lib/" CHOOSE_K1_DYNAMIC_LINKER_PATH "ld-uClibc.so.0} \
   %{static}}"
