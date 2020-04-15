#ifndef _SOLIB_KVX_BARE_H_
#define _SOLIB_KVX_BARE_H_

#include "solib.h"
#include "inferior.h"

extern struct target_so_ops kvx_bare_solib_ops;

void kvx_bare_solib_load_debug_info (void);
const char *get_cluster_name (struct inferior *inf);

#endif // _SOLIB_KVX_BARE_H_
