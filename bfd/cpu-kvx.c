#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"

/* This routine is provided two arch_infos and returns if machines
   are compatible.
*/

static const bfd_arch_info_type *
kvx_compatible (const bfd_arch_info_type *a, const bfd_arch_info_type *b)
{
  long amach =  a->mach, bmach =  b->mach;
  /* If a & b are for different architecture we can do nothing.  */
  if (a->arch != b->arch)
    return NULL;

  if (amach == bfd_mach_kv3_1_64 && bmach == bfd_mach_kv3_1_usr)
    return b;
  if (bmach == bfd_mach_kv3_1_64 && amach == bfd_mach_kv3_1_usr)
    return a;

  /* We do not want to transmute some machine into another one */
  if (amach != bmach)
    return NULL;

  /* If a & b are for the same machine then all is well.  */
  if (amach == bmach)
    return a;

  return NULL;
}

static bfd_boolean
scan (const struct bfd_arch_info *info, const char *string)
{
  /* First test for an exact match.  */
  if (strcasecmp (string, info->printable_name) == 0)
    return TRUE;

  /* Finally check for the default architecture.  */
  if (strcasecmp (string, "kvx") == 0)
    return info->the_default;

  return FALSE;
}

#define N(addr_bits, machine, print, default, next)            \
{                                                              \
  32,                          /* 32 bits in a word.  */       \
  addr_bits,                   /* bits in an address.  */      \
  8,                           /* 8 bits in a byte.  */        \
  bfd_arch_kvx,                                                 \
  machine,                     /* Machine number.  */          \
  "kvx",                        /* Architecture name.   */      \
  print,                       /* Printable name.  */          \
  4,                           /* Section align power.  */     \
  default,                     /* Is this the default ?  */    \
  kvx_compatible,					       \
  scan,                                                        \
  bfd_arch_default_fill,                                       \
  next                                                         \
}

const bfd_arch_info_type bfd_kv3_1_usr_arch =
  N (64, bfd_mach_kv3_1_usr,  "kvx:kv3-1:usr", FALSE, NULL);

const bfd_arch_info_type bfd_kv3_1_64_arch =
  N (64, bfd_mach_kv3_1_64,   "kvx:kv3-1:64",  FALSE, & bfd_kv3_1_usr_arch);

const bfd_arch_info_type bfd_kvx_arch =
  N (32, bfd_mach_kv3_1,      "kvx:kv3-1",     TRUE, & bfd_kv3_1_64_arch);
