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

#include "kvx-protos.h"

/**
 * The stack limit register (slr) is setup by ClusterOS and
 * materializes the end of the stack. Since the addresses
 * grow downwards the stack pointer can only decrease.
 * Hence, the following inequality,
 *         sp >= slr
 * has to be verified, otherwise it means that the stack
 * pointer (sp) is below the hard bound set by the stack
 * limit register. This is an overflow!
 */

/* Main entry point for this pass.  */
unsigned int
kvx_prologue_stack_limit (function *fun)
{
  basic_block prologue = single_succ (ENTRY_BLOCK_PTR_FOR_FN (fun));
  rtx (*gen_cbranch) (rtx, rtx, rtx, rtx)
    = TARGET_32 ? gen_cbranchsi4 : gen_cbranchdi4;

  int size = kvx_get_real_frame_size (fun);

  if (size > 0)
    {
      basic_block pre_prologue, stack_overflow_handler;
      rtx_code_label *trap_label = gen_label_rtx ();

      rtx new_stack_pointer_reg = gen_rtx_REG (Pmode, 16);
      rtx stack_limit_reg = gen_rtx_REG (Pmode, 17);

      rtx handler
	= gen_rtx_MEM (FUNCTION_MODE,
		       init_one_libfunc ("__stack_overflow_detected"));

      rtx_insn *insn;
      edge e1, e2;

      /* The pre-prologue performs to actions: first it loads the stack limit
       * either from the dedicated register for our architecture: $sr
       *     get $r17 = $sr
       *     ;;
       * or through the dedicated link time symbol __cos_stack_limit:
       *    make $r17 = __cos_stack_limit
       *    ;;
       * Once the stack limit is in $r17, we perform the following check:
       *     addd $r16 = $r12, -64
       *     ;;
       *     sbfd $r16 = $r16, $r17
       *     ;;
       *     cb.dgtz $r16? .Lstack_overflow_detected
       *     ;;
       */
      if (opt_fstack_limit_register_no == -1 && !opt_fstack_limit_symbol_arg)
	error ("`-fstack-limit-*' not enabled.");

      insn = emit_insn_before (
	gen_move_insn (stack_limit_reg,
		       opt_fstack_limit_symbol_arg
			 ? gen_rtx_SYMBOL_REF (Pmode, "__cos_stack_limit")
			 : stack_limit_rtx),
	NEXT_INSN (BB_HEAD (prologue)));

      insn
	= emit_insn_after (gen_add3_insn (new_stack_pointer_reg,
					  stack_pointer_rtx, GEN_INT (-size)),
			   insn);
      insn = emit_insn_after (gen_sub3_insn (new_stack_pointer_reg,
					     stack_limit_reg,
					     new_stack_pointer_reg),
			      insn);

      insn = emit_jump_insn_after (
	gen_cbranch (gen_rtx_GT (Pmode, new_stack_pointer_reg, const0_rtx),
		     new_stack_pointer_reg, const0_rtx, trap_label),
	insn);
      JUMP_LABEL (insn) = trap_label;
      LABEL_NUSES (trap_label)++;

      e2 = split_block (prologue, insn);
      pre_prologue = e2->src;
      prologue = e2->dest;

      /* This block is only available by a direct jump.
       * Emits the label which catch the jump setup in the pre-prologue,
       * and call __stack_overflow_detected.
       *     .Lstack_overflow_detected
       *     call __stack_overflow_detected
       */
      rtx_insn *label;
      stack_overflow_handler
	= create_empty_bb (EXIT_BLOCK_PTR_FOR_FN (fun)->prev_bb);
      label = insn
	= emit_label_before (trap_label,
			     get_last_bb_insn (stack_overflow_handler));
      insn = emit_call_insn_after (gen_call (handler, const0_rtx),
				   get_last_bb_insn (stack_overflow_handler));
      add_reg_note (insn, REG_ARGS_SIZE, const0_rtx);
      add_reg_note (insn, REG_NORETURN, NULL_RTX);

      e1 = make_edge (pre_prologue, stack_overflow_handler, 0);

      /* Adjust probabilities. */
      e2->probability = profile_probability::always ();
      e1->probability = profile_probability::never ();
    }

  return 0;
}

const pass_data pass_data_prologue_stack_limit = {
  RTL_PASS,		  /* type */
  "prologue_stack_limit", /* name */
  OPTGROUP_NONE,	  /* optinfo_flags */
  TV_NONE,		  /* tv_id */
  0,			  /* properties_required */
  0,			  /* properties_provided */
  0,			  /* properties_destroyed */
  0,			  /* todo_flags_start */
  TODO_df_finish,	  /* todo_flags_finish */
};

class pass_prologue_stack_limit : public rtl_opt_pass
{
public:
  pass_prologue_stack_limit (gcc::context *ctxt)
    : rtl_opt_pass (pass_data_prologue_stack_limit, ctxt)
  {
  }

  /* opt_pass methods: */
  virtual bool
  gate (function *)
  {
    return crtl->limit_stack;
  }

  virtual unsigned int
  execute (function *fun)
  {
    return kvx_prologue_stack_limit (fun);
  }

  opt_pass *
  clone ()
  {
    return new pass_prologue_stack_limit (m_ctxt);
  }

}; // class pass_prologue_stack_limit

rtl_opt_pass *
make_pass_prologue_stack_limit (gcc::context *ctxt)
{
  return new pass_prologue_stack_limit (ctxt);
}
