/*
 * Definitions of target machine for GNU compiler, for Kalray kv3.
 * Copyright (C) 2009-2019 Kalray S.A.
 *
 * This file is part of GCC.
 *
 * GCC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GCC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GCC; see the file COPYING3.  If not see
 * <http://www.gnu.org/licenses/>.
 */

enum reg_class
{
  NO_REGS,
  GPR_REGS,
  PGR_REGS,
  QGR_REGS,
  SFR_REGS,
  AIB_REGS,
  ALL_REGS,
  LIM_REG_CLASSES
};

#define REG_CLASS_NAMES                                                        \
  {                                                                            \
    "NO_REGS", "GPR_REGS", "PGR_REGS", "QGR_REGS", "SFR_REGS", "AIB_REGS",     \
      "ALL_REGS", "LIM_REG_CLASSES"                                            \
  }

#define KV3_GPR_FIRST_REGNO 0
#define KV3_GPR_LAST_REGNO 63
#define KV3_SFR_FIRST_REGNO 64
#define KV3_SFR_LAST_REGNO 255

#define KV3_MDS_REGISTERS 256
#define KV3_SYNC_REG_REGNO (KV3_MDS_REGISTERS + 0)
#define KV3_FRAME_POINTER_VIRT_REGNO (KV3_MDS_REGISTERS + 1)
#define FIRST_PSEUDO_REGISTER (KV3_MDS_REGISTERS + 2)

#define REG_CLASS_CONTENTS                                                     \
  {                                                                            \
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,               \
     0x00000000, 0x00000000, 0x00000000, 0x0}, /* NO_REGS */                   \
      {0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000,             \
       0x00000000, 0x00000000, 0x00000000, 0x2}, /* GPR_REGS */                \
      {0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000,             \
       0x00000000, 0x00000000, 0x00000000, 0x0}, /* PGR_REGS */                \
      {0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000,             \
       0x00000000, 0x00000000, 0x00000000, 0x0}, /* QGR_REGS */                \
      {0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff,             \
       0xffffffff, 0xffffffff, 0xffffffff, 0x1}, /* SFR_REGS */                \
      {0x00000000, 0x00000000, 0x10000002, 0x00000000, 0x000000f0,             \
       0x0000003f, 0x00000010, 0x00000000, 0x0}, /* AIB_REGS */                \
      {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,             \
       0xffffffff, 0xffffffff, 0xffffffff, 0x3}, /* ALL_REGS */                \
  }

#define REGNO_REG_CLASS(REGNO)                                                 \
  ((((REGNO) <= KV3_GPR_LAST_REGNO)                                            \
      ? GPR_REGS                                                               \
      : (((REGNO) >= KV3_SFR_FIRST_REGNO && (REGNO) <= KV3_SFR_LAST_REGNO)     \
	   ? SFR_REGS                                                          \
	   : ((REGNO) == KV3_SYNC_REG_REGNO                                    \
		? SFR_REGS                                                     \
		: ((REGNO) == KV3_FRAME_POINTER_VIRT_REGNO ? GPR_REGS          \
							   : NO_REGS)))))

#define KV3_REGISTER_NAMES                                                     \
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11",    \
    "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19", "r20", "r21",      \
    "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",      \
    "r32", "r33", "r34", "r35", "r36", "r37", "r38", "r39", "r40", "r41",      \
    "r42", "r43", "r44", "r45", "r46", "r47", "r48", "r49", "r50", "r51",      \
    "r52", "r53", "r54", "r55", "r56", "r57", "r58", "r59", "r60", "r61",      \
    "r62", "r63", "pc", "ps", "pcr", "ra", "cs", "csit", "aespc", "ls", "le",  \
    "lc", "ipe", "men", "pmc", "pm0", "pm1", "pm2", "pm3", "pmsa", "tcr",      \
    "t0v", "t1v", "t0r", "t1r", "wdv", "wdr", "ile", "ill", "ilr", "mmc",      \
    "tel", "teh", "res31", "syo", "hto", "ito", "do", "mo", "pso", "res38",    \
    "res39", "dc", "dba0", "dba1", "dwa0", "dwa1", "mes", "ws", "res47",       \
    "res48", "res49", "res50", "res51", "res52", "res53", "res54", "res55",    \
    "res56", "res57", "res58", "res59", "res60", "res61", "res62", "res63",    \
    "spc_pl0", "spc_pl1", "spc_pl2", "spc_pl3", "sps_pl0", "sps_pl1",          \
    "sps_pl2", "sps_pl3", "ea_pl0", "ea_pl1", "ea_pl2", "ea_pl3", "ev_pl0",    \
    "ev_pl1", "ev_pl2", "ev_pl3", "sr_pl0", "sr_pl1", "sr_pl2", "sr_pl3",      \
    "es_pl0", "es_pl1", "es_pl2", "es_pl3", "res88", "res89", "res90",         \
    "res91", "res92", "res93", "res94", "res95", "syow", "htow", "itow",       \
    "dow", "mow", "psow", "res102", "res103", "res104", "res105", "res106",    \
    "res107", "res108", "res109", "res110", "res111", "res112", "res113",      \
    "res114", "res115", "res116", "res117", "res118", "res119", "res120",      \
    "res121", "res122", "res123", "res124", "res125", "res126", "res127",      \
    "spc", "res129", "res130", "res131", "sps", "res133", "res134", "res135",  \
    "ea", "res137", "res138", "res139", "ev", "res141", "res142", "res143",    \
    "sr", "res145", "res146", "res147", "es", "res149", "res150", "res151",    \
    "res152", "res153", "res154", "res155", "res156", "res157", "res158",      \
    "res159", "res160", "res161", "res162", "res163", "res164", "res165",      \
    "res166", "res167", "res168", "res169", "res170", "res171", "res172",      \
    "res173", "res174", "res175", "res176", "res177", "res178", "res179",      \
    "res180", "res181", "res182", "res183", "res184", "res185", "res186",      \
    "res187", "res188", "res189", "res190", "res191",

#define KV3_PGR_REGISTER_NAMES                                                 \
  "r0r1", "ERROR", "r2r3", "ERROR", "r4r5", "ERROR", "r6r7", "ERROR", "r8r9",  \
    "ERROR", "r10r11", "ERROR", "r12r13", "ERROR", "r14r15", "ERROR",          \
    "r16r17", "ERROR", "r18r19", "ERROR", "r20r21", "ERROR", "r22r23",         \
    "ERROR", "r24r25", "ERROR", "r26r27", "ERROR", "r28r29", "ERROR",          \
    "r30r31", "ERROR", "r32r33", "ERROR", "r34r35", "ERROR", "r36r37",         \
    "ERROR", "r38r39", "ERROR", "r40r41", "ERROR", "r42r43", "ERROR",          \
    "r44r45", "ERROR", "r46r47", "ERROR", "r48r49", "ERROR", "r50r51",         \
    "ERROR", "r52r53", "ERROR", "r54r55", "ERROR", "r56r57", "ERROR",          \
    "r58r59", "ERROR", "r60r61", "ERROR", "r62r63", "ERROR",

#define KV3_QGR_REGISTER_NAMES                                                 \
  "r0r1r2r3", "ERROR", "ERROR", "ERROR", "r4r5r6r7", "ERROR", "ERROR",         \
    "ERROR", "r8r9r10r11", "ERROR", "ERROR", "ERROR", "r12r13r14r15", "ERROR", \
    "ERROR", "ERROR", "r16r17r18r19", "ERROR", "ERROR", "ERROR",               \
    "r20r21r22r23", "ERROR", "ERROR", "ERROR", "r24r25r26r27", "ERROR",        \
    "ERROR", "ERROR", "r28r29r30r31", "ERROR", "ERROR", "ERROR",               \
    "r32r33r34r35", "ERROR", "ERROR", "ERROR", "r36r37r38r39", "ERROR",        \
    "ERROR", "ERROR", "r40r41r42r43", "ERROR", "ERROR", "ERROR",               \
    "r44r45r46r47", "ERROR", "ERROR", "ERROR", "r48r49r50r51", "ERROR",        \
    "ERROR", "ERROR", "r52r53r54r55", "ERROR", "ERROR", "ERROR",               \
    "r56r57r58r59", "ERROR", "ERROR", "ERROR", "r60r61r62r63", "ERROR",        \
    "ERROR", "ERROR",

#define KV3_ABI_REGULAR 0
#define KV3_PROGRAM_POINTER_REGNO 64
#define KV3_STACK_POINTER_REGNO 12
#define KV3_FRAME_POINTER_REGNO 14
#define KV3_LOCAL_POINTER_REGNO 13
#define KV3_STRUCT_POINTER_REGNO 15
#define KV3_RETURN_POINTER_REGNO 67
#define KV3_ARGUMENT_POINTER_REGNO 0
#define KV3_ARG_REG_SLOTS 12

#define KV3_ABI_REGULAR_FIXED_REGISTERS                                        \
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1,

#define KV3_ABI_REGULAR_CALL_USED_REGISTERS                                    \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,   \
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1,

#define KV3_ABI_REGULAR_CALL_REALLY_USED_REGISTERS                             \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,   \
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1,
