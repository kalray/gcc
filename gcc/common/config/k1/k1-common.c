/*

   Copyright (C) 2009-2014 Kalray SA.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "common/common-target.h"
#include "common/common-target-def.h"
#include "opts.h"
#include "flags.h"
#include "params.h"
#include "hard-reg-set.h"
#include "rtl.h"

/* Set default optimization options.  */
static const struct default_options k1_option_optimization_table[] =
{
    { OPT_LEVELS_ALL, OPT_ftls_model_, "local-exec", 1 },
    { OPT_LEVELS_ALL, OPT_fsched_pressure, NULL, 1 },
    { OPT_LEVELS_ALL, OPT_fomit_frame_pointer, NULL, 1 },
    { OPT_LEVELS_2_PLUS, OPT_fsched2_use_superblocks, NULL, 1 },
    { OPT_LEVELS_2_PLUS, OPT_ftree_loop_if_convert, NULL, 1 },
    { OPT_LEVELS_3_PLUS, OPT_funroll_loops, NULL, 1 },
    { OPT_LEVELS_NONE, 0, NULL, 0 }
};

 

/* Implement TARGET_OPTION_DEFAULT_PARAMS.  */
static void
k1_option_default_params (void)
{
    set_default_param_value (PARAM_PREFETCH_LATENCY, 12);
    set_default_param_value (PARAM_SIMULTANEOUS_PREFETCHES, 10);
    set_default_param_value (PARAM_L1_CACHE_SIZE, 8);
    set_default_param_value (PARAM_L1_CACHE_LINE_SIZE, 32);
    set_default_param_value (PARAM_L2_CACHE_SIZE, 1024);
    set_default_param_value (PARAM_IPA_CP_EVAL_THRESHOLD, 400);
    set_default_param_value (PARAM_MAX_INLINE_INSNS_AUTO, 40); /* Default value */
    set_default_param_value (PARAM_MAX_COMPLETELY_PEELED_INSNS, 400);
    set_default_param_value (PARAM_MAX_PEELED_INSNS, 400);
}

#undef TARGET_OPTION_OPTIMIZATION_TABLE
#define TARGET_OPTION_OPTIMIZATION_TABLE k1_option_optimization_table
#undef TARGET_OPTION_DEFAULT_PARAMS
#define TARGET_OPTION_DEFAULT_PARAMS k1_option_default_params
#undef TARGET_EXCEPT_UNWIND_INFO
#define TARGET_EXCEPT_UNWIND_INFO sjlj_except_unwind_info
#undef TARGET_DEFAULT_TARGET_FLAGS
#define TARGET_DEFAULT_TARGET_FLAGS (MASK_BUNDLING | MASK_HWLOOP)

struct gcc_targetm_common targetm_common = TARGETM_COMMON_INITIALIZER;
