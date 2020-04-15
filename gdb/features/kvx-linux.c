/* THIS FILE IS GENERATED.  -*- buffer-read-only: t -*- vi:set ro:
  Original: kvx/kvx-linux.xml */

#include "defs.h"
#include "osabi.h"
#include "target-descriptions.h"

struct target_desc *tdesc_kvx_linux;
static void
initialize_tdesc_kvx_linux (void)
{
  struct target_desc *result = allocate_target_description ();
  struct tdesc_feature *feature;
  struct tdesc_type *field_type;

  set_tdesc_architecture (result, bfd_scan_arch ("kvx:kv3:usr"));

  feature = tdesc_create_feature (result, "eu.kalray.core.kv3-1");
  field_type = tdesc_create_flags (feature, "cs_type", 8);
  tdesc_add_flag (field_type, 0, "ic");
  tdesc_add_flag (field_type, 1, "io");
  tdesc_add_flag (field_type, 2, "dz");
  tdesc_add_flag (field_type, 3, "ov");
  tdesc_add_flag (field_type, 4, "un");
  tdesc_add_flag (field_type, 5, "in");
  tdesc_add_flag (field_type, 9, "xio");
  tdesc_add_flag (field_type, 10, "xdz");
  tdesc_add_flag (field_type, 11, "xov");
  tdesc_add_flag (field_type, 12, "xun");
  tdesc_add_flag (field_type, 13, "xin");
  tdesc_add_flag (field_type, 16, "rm");
  tdesc_add_flag (field_type, 20, "xrm");
  tdesc_add_flag (field_type, 24, "xmf");
  tdesc_add_flag (field_type, 32, "cc");
  tdesc_add_flag (field_type, 48, "xdrop");
  tdesc_add_flag (field_type, 54, "xpow2");

  tdesc_create_reg (feature, "r0", 0, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r1", 1, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r2", 2, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r3", 3, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r4", 4, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r5", 5, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r6", 6, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r7", 7, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r8", 8, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r9", 9, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r10", 10, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r11", 11, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r12", 12, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r13", 13, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r14", 14, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r15", 15, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r16", 16, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r17", 17, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r18", 18, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r19", 19, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r20", 20, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r21", 21, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r22", 22, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r23", 23, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r24", 24, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r25", 25, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r26", 26, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r27", 27, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r28", 28, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r29", 29, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r30", 30, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r31", 31, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r32", 32, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r33", 33, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r34", 34, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r35", 35, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r36", 36, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r37", 37, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r38", 38, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r39", 39, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r40", 40, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r41", 41, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r42", 42, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r43", 43, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r44", 44, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r45", 45, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r46", 46, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r47", 47, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r48", 48, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r49", 49, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r50", 50, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r51", 51, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r52", 52, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r53", 53, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r54", 54, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r55", 55, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r56", 56, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r57", 57, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r58", 58, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r59", 59, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r60", 60, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r61", 61, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r62", 62, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "r63", 63, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "lc", 64, 1, NULL, 64, "int64");
  tdesc_create_reg (feature, "le", 65, 1, NULL, 64, "code_ptr");
  tdesc_create_reg (feature, "ls", 66, 1, NULL, 64, "code_ptr");
  tdesc_create_reg (feature, "ra", 67, 1, NULL, 64, "code_ptr");
  tdesc_create_reg (feature, "cs", 68, 1, NULL, 64, "cs_type");
  tdesc_create_reg (feature, "pc", 69, 1, NULL, 64, "code_ptr");

  tdesc_kvx_linux = result;
}
