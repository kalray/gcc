/* Copyright (C) 1991-2022 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#define IN_TARGET_CODE 1

#include "config.h"
#include "errors.h"
#include "system.h"
#include "coretypes.h"
#include "opts.h"
#include "tree-pass.h"
#include "backend.h"
#include "memmodel.h"
#include "basic-block.h"
#include "rtl.h"
#include "df.h"
#include "tree.h"
#include "expr.h"
#include "cfghooks.h"
#include "insn-codes.h"
#include "optabs.h"
#include "emit-rtl.h"
#include "print-rtl.h"
#include "cfgrtl.h"
#include "output.h"
#include "recog.h"
#include "insn-attr.h"
#include "rtl.h"
#include "pretty-print.h"
#include "genrtl.h"
#include "insn-flags.h"

#include "kvx-protos.h"

/* The shaker is initialized only once and remember its state */
enum kvx_shaker_state
{
  /* This is the state if the pass has not be called yet. */
  KVX_SHAKER_UNINIT,
  /* At the first invocation of the pass, if the switch to turn on the
   * pass is not present the shaker is disabled */
  KVX_SHAKER_DISABLED,
  /* Otherwise it is enabled and initialized */
  KVX_SHAKER_ENABLED
};
static enum kvx_shaker_state kvx_shaker_state;
static int kvx_shaker_seed;

/* Structure which store the basic information about loads/stores. */
struct GTY (()) kvx_shaker_ls
{
  rtx src;
  rtx dest;
};

static int
shaker_rand ()
{
  kvx_shaker_seed = (kvx_shaker_seed * 1103515245U + 12345U) & 0x7fffffffU;
  return kvx_shaker_seed;
}

static int
load_p (rtx_insn * insn, struct kvx_shaker_ls *ls_info)
{
  if (GET_CODE (PATTERN (insn)) != SET)
    return 0;

  enum attr_type insn_type = get_attr_type (insn);
  if (!(insn_type >= TYPE_LSU_LOAD && insn_type < TYPE_LSU_AUXR_STORE))
    return 0;

  if (ls_info)
    {
      rtx src = SET_SRC (PATTERN (insn));
      rtx dest = SET_DEST (PATTERN (insn));
      if (GET_CODE (src) == UNSPEC || GET_CODE (XEXP (src, 0)) == MEM)
	return 0;
      if (GET_CODE (XEXP (src, 0)) == PLUS &&
	  (GET_CODE (XEXP (XEXP (src, 0), 0)) == MULT ||
	   GET_CODE (XEXP (XEXP (src, 0), 1)) == MULT))
	return 0;
      ls_info->src = copy_rtx (XEXP (src, 0));
      ls_info->dest = copy_rtx (dest);
    }

  return 1;
}

/* Shake offsets of the form PLUS(REG, CONST_INT). */
static inline void
shake_offset (rtx src)
{
  if (GET_CODE (src) != PLUS)
    return;

  switch (GET_CODE (XEXP (src, 1)))
    {
    case CONST_INT:
      {
	int offset = INTVAL (XEXP (src, 1));
	offset += ((shaker_rand () & 1) ? 1 : -1) * 64 * (shaker_rand () % 8);
	XEXP (src, 1) = GEN_INT (offset);
      }
    default:
      break;

    }
}

/* Main entry point for this pass.  */
static unsigned int
kvx_shaker (function * fun)
{
  basic_block bb;
  FOR_EACH_BB_FN (bb, fun)
  {
    for (rtx_insn * cur_insn = BB_HEAD (bb); cur_insn != BB_END (bb);
	 cur_insn = NEXT_INSN (cur_insn))
      {
	int shake_p = shaker_rand ();
	if (shake_p % 5)
	  continue;
	switch (GET_CODE (cur_insn))
	  {
	  case INSN:
	    {
	      struct kvx_shaker_ls ls_info;
	      int is_load = load_p (cur_insn, &ls_info);
	      if (shake_p % 51 == 0)
		emit_insn_before (gen_nop (), cur_insn);
	      if (is_load && KV3_2)
		{
		  shake_offset (ls_info.src);
		  switch (shake_p % 17)
		    {
		    case 0:
		      emit_insn_before (gen_nop (), cur_insn);
		    case 1:
		    case 2:
		    case 3:
		    case 4:
		      emit_insn_before (gen_kvx_dtouchl (ls_info.src),
					cur_insn);
		    case 5:
		      if (shake_p % 5 == 0)
			emit_insn_before (gen_kvx_dflushl (ls_info.src),
					  cur_insn);
		      else if (shake_p % 5 < 3)
			emit_insn_before (gen_kvx_dpurgel (ls_info.src),
					  cur_insn);
		      break;
		    case 6:
		      if (shake_p % 3 == 0)
			emit_insn_before (gen_kvx_fence
					  (gen_rtx_CONST_STRING
					   (VOIDmode, ".r")), cur_insn);
		      else if (shake_p % 3 == 1)
			emit_insn_before (gen_kvx_fence
					  (gen_rtx_CONST_STRING
					   (VOIDmode, ".w")), cur_insn);
		      if (shake_p % 5 == 0)
			emit_insn_before (gen_kvx_dflushl (ls_info.src),
					  cur_insn);
		      else if (shake_p % 5 < 3)
			emit_insn_before (gen_kvx_dpurgel (ls_info.src),
					  cur_insn);
		      break;
		    case 7:
		      emit_insn_before (gen_kvx_barrier (), cur_insn);
		      break;
		    case 9:
		    case 10:
		      emit_insn_before (gen_kvx_d1inval (), cur_insn);
		      break;
		    case 11:
		    case 12:
		      emit_insn_before (gen_kvx_i1inval (), cur_insn);
		      break;
		    case 13:
		      // case 14:
		      //   emit_insn_before (gen_kvx_dinvall (ls_info.src),
		      //                     cur_insn);
		      //   break;
		    case 15:
		      emit_insn_before (gen_kvx_i1invals (ls_info.src),
					cur_insn);
		      break;
		      // case ??:
		      //   emit_insn_before (gen_kvx_tlbiinval (), cur_insn);
		      //   break;
		      // case ??:
		      //   emit_insn_before (gen_kvx_tlbdinval (), cur_insn);
		      //   break;
		      // case ??:
		      //   emit_insn_before (gen_kvx_tlbprobe (), cur_insn);
		      //   break;
		    default:
		      break;
		    }
		}
	    }
	  default:
	    break;
	  }
      }
    insn_locations_finalize ();
    init_insn_lengths ();
  }

  return 0;
}

const pass_data pass_data_kvx_shaker = {
  RTL_PASS,			/* type */
  "kvx_shaker",			/* name */
  OPTGROUP_NONE,		/* optinfo_flags */
  TV_NONE,			/* tv_id */
  0,				/* properties_required */
  0,				/* properties_provided */
  0,				/* properties_destroyed */
  0,				/* todo_flags_start */
  0,				/* todo_flags_finish */
};

class pass_kvx_shaker:public rtl_opt_pass
{
public:
  pass_kvx_shaker (gcc::context * ctxt):rtl_opt_pass (pass_data_kvx_shaker,
						      ctxt)
  {
  }

  /* opt_pass methods: */
  virtual bool gate (function *)
  {
    if (kvx_shaker_state == KVX_SHAKER_UNINIT)
      {
	int i;
	cl_deferred_option *opt;
	vec < cl_deferred_option > *v
	  = (vec < cl_deferred_option > *)kvx_deferred_options;

	if (v)
	  FOR_EACH_VEC_ELT (*v, i, opt)
	  {
	    switch (opt->opt_index)
	      {
	      case OPT_fshaker_seed_:
		kvx_shaker_state = KVX_SHAKER_ENABLED;
		kvx_shaker_seed = atoi (opt->arg);
		break;
	      default:
		break;
	      }
	    if (!kvx_shaker_state)
	      kvx_shaker_state = KVX_SHAKER_DISABLED;
	  }
      }

    return kvx_shaker_state == KVX_SHAKER_ENABLED;
  }

  virtual unsigned int execute (function * fun)
  {
    return kvx_shaker (fun);
  }

  opt_pass *clone ()
  {
    return new pass_kvx_shaker (m_ctxt);
  }

};				// class pass_kvx_shaker

rtl_opt_pass *
make_pass_kvx_shaker (gcc::context * ctxt)
{
  return new pass_kvx_shaker (ctxt);
}
