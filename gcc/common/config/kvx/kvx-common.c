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
#include "hard-reg-set.h"
#include "rtl.h"

/* Set default optimization options.  */
static const struct default_options kvx_option_optimization_table[] =
  {
    {OPT_LEVELS_ALL, OPT_ftls_model_, "local-exec", 1},
    {OPT_LEVELS_ALL, OPT_fsched_pressure, NULL, 1},
    {OPT_LEVELS_ALL, OPT_fomit_frame_pointer, NULL, 1},

    {OPT_LEVELS_ALL, OPT__param_prefetch_latency_, NULL, 12 },
    {OPT_LEVELS_ALL, OPT__param_simultaneous_prefetches_, NULL, 10},
    {OPT_LEVELS_ALL, OPT__param_l1_cache_size_, NULL, 8},
    {OPT_LEVELS_ALL, OPT__param_l1_cache_line_size_, NULL, 32},
    {OPT_LEVELS_ALL, OPT__param_l2_cache_size_, NULL, 1024},
    {OPT_LEVELS_ALL, OPT__param_ipa_cp_eval_threshold_, NULL, 400},
    {OPT_LEVELS_ALL, OPT__param_max_inline_insns_auto_, NULL, 40}, /* Default value */
    {OPT_LEVELS_ALL, OPT__param_max_completely_peeled_insns_, NULL, 400},
    {OPT_LEVELS_ALL, OPT__param_max_peeled_insns_, NULL, 400},

    {OPT_LEVELS_2_PLUS, OPT_fsched2_use_superblocks, NULL, 1},
    {OPT_LEVELS_2_PLUS, OPT_ftree_loop_if_convert, NULL, 1},

    {OPT_LEVELS_3_PLUS, OPT_funroll_loops, NULL, 1},

    {OPT_LEVELS_NONE, 0, NULL, 0}
  };

#undef TARGET_OPTION_OPTIMIZATION_TABLE
#define TARGET_OPTION_OPTIMIZATION_TABLE kvx_option_optimization_table
#undef TARGET_EXCEPT_UNWIND_INFO
#define TARGET_EXCEPT_UNWIND_INFO sjlj_except_unwind_info
#undef TARGET_DEFAULT_TARGET_FLAGS
#define TARGET_DEFAULT_TARGET_FLAGS (MASK_HWLOOP | MASK_SCHED2_DATES)

struct gcc_targetm_common targetm_common = TARGETM_COMMON_INITIALIZER;
