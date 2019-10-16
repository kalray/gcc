/*
 * Definitions of target machine for GNU compiler, for Kalray k1c.
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
  ALL_REGS,
  LIM_REG_CLASSES
};

#define REG_CLASS_NAMES                                                        \
  {                                                                            \
    "NO_REGS", "GPR_REGS", "PGR_REGS", "QGR_REGS", "SFR_REGS", "ALL_REGS",     \
      "LIM_REG_CLASSES"                                                        \
  }

#define K1C_GPR_FIRST_REGNO 0
#define K1C_GPR_LAST_REGNO 63
#define K1C_SFR_FIRST_REGNO 64
#define K1C_SFR_LAST_REGNO 255

#define K1C_MDS_REGISTERS 256

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
      {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,             \
       0xffffffff, 0xffffffff, 0xffffffff, 0x3}, /* ALL_REGS */                \
  }

#define REGNO_REG_CLASS(REGNO)                                                 \
  ((((REGNO) <= K1C_GPR_LAST_REGNO)                                            \
      ? GPR_REGS                                                               \
      : (((REGNO) >= K1C_SFR_FIRST_REGNO && (REGNO) <= K1C_SFR_LAST_REGNO)     \
	   ? SFR_REGS                                                          \
	   : ((REGNO) == K1C_MDS_REGISTERS                                     \
		? SFR_REGS                                                     \
		: ((REGNO) == K1C_MDS_REGISTERS + 1 ? GPR_REGS : NO_REGS)))))

#define K1C_REGISTER_NAMES                                                     \
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

#define K1C_GPR_REGISTER_NAMES                                                 \
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11",    \
    "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19", "r20", "r21",      \
    "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",      \
    "r32", "r33", "r34", "r35", "r36", "r37", "r38", "r39", "r40", "r41",      \
    "r42", "r43", "r44", "r45", "r46", "r47", "r48", "r49", "r50", "r51",      \
    "r52", "r53", "r54", "r55", "r56", "r57", "r58", "r59", "r60", "r61",      \
    "r62", "r63",

#define K1C_PGR_REGISTER_NAMES                                                 \
  "r0r1", "ERROR", "r2r3", "ERROR", "r4r5", "ERROR", "r6r7", "ERROR", "r8r9",  \
    "ERROR", "r10r11", "ERROR", "r12r13", "ERROR", "r14r15", "ERROR",          \
    "r16r17", "ERROR", "r18r19", "ERROR", "r20r21", "ERROR", "r22r23",         \
    "ERROR", "r24r25", "ERROR", "r26r27", "ERROR", "r28r29", "ERROR",          \
    "r30r31", "ERROR", "r32r33", "ERROR", "r34r35", "ERROR", "r36r37",         \
    "ERROR", "r38r39", "ERROR", "r40r41", "ERROR", "r42r43", "ERROR",          \
    "r44r45", "ERROR", "r46r47", "ERROR", "r48r49", "ERROR", "r50r51",         \
    "ERROR", "r52r53", "ERROR", "r54r55", "ERROR", "r56r57", "ERROR",          \
    "r58r59", "ERROR", "r60r61", "ERROR", "r62r63", "ERROR",

#define K1C_QGR_REGISTER_NAMES                                                 \
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

#define K1C_SFR_REGISTER_NAMES                                                 \
  "pc", "ps", "pcr", "ra", "cs", "csit", "aespc", "ls", "le", "lc", "ipe",     \
    "men", "pmc", "pm0", "pm1", "pm2", "pm3", "pmsa", "tcr", "t0v", "t1v",     \
    "t0r", "t1r", "wdv", "wdr", "ile", "ill", "ilr", "mmc", "tel", "teh",      \
    "res31", "syo", "hto", "ito", "do", "mo", "pso", "res38", "res39", "dc",   \
    "dba0", "dba1", "dwa0", "dwa1", "mes", "ws", "res47", "res48", "res49",    \
    "res50", "res51", "res52", "res53", "res54", "res55", "res56", "res57",    \
    "res58", "res59", "res60", "res61", "res62", "res63", "spc_pl0",           \
    "spc_pl1", "spc_pl2", "spc_pl3", "sps_pl0", "sps_pl1", "sps_pl2",          \
    "sps_pl3", "ea_pl0", "ea_pl1", "ea_pl2", "ea_pl3", "ev_pl0", "ev_pl1",     \
    "ev_pl2", "ev_pl3", "sr_pl0", "sr_pl1", "sr_pl2", "sr_pl3", "es_pl0",      \
    "es_pl1", "es_pl2", "es_pl3", "res88", "res89", "res90", "res91", "res92", \
    "res93", "res94", "res95", "syow", "htow", "itow", "dow", "mow", "psow",   \
    "res102", "res103", "res104", "res105", "res106", "res107", "res108",      \
    "res109", "res110", "res111", "res112", "res113", "res114", "res115",      \
    "res116", "res117", "res118", "res119", "res120", "res121", "res122",      \
    "res123", "res124", "res125", "res126", "res127", "spc", "res129",         \
    "res130", "res131", "sps", "res133", "res134", "res135", "ea", "res137",   \
    "res138", "res139", "ev", "res141", "res142", "res143", "sr", "res145",    \
    "res146", "res147", "es", "res149", "res150", "res151", "res152",          \
    "res153", "res154", "res155", "res156", "res157", "res158", "res159",      \
    "res160", "res161", "res162", "res163", "res164", "res165", "res166",      \
    "res167", "res168", "res169", "res170", "res171", "res172", "res173",      \
    "res174", "res175", "res176", "res177", "res178", "res179", "res180",      \
    "res181", "res182", "res183", "res184", "res185", "res186", "res187",      \
    "res188", "res189", "res190", "res191", "res192", "res193", "res194",      \
    "res195", "res196", "res197", "res198", "res199", "res200", "res201",      \
    "res202", "res203", "res204", "res205", "res206", "res207", "res208",      \
    "res209", "res210", "res211", "res212", "res213", "res214", "res215",      \
    "res216", "res217", "res218", "res219", "res220", "res221", "res222",      \
    "res223", "res224", "res225", "res226", "res227", "res228", "res229",      \
    "res230", "res231", "res232", "res233", "res234", "res235", "res236",      \
    "res237", "res238", "res239", "res240", "res241", "res242", "res243",      \
    "res244", "res245", "res246", "res247", "res248", "res249", "res250",      \
    "res251", "res252", "res253", "res254", "res255", "vsfr0", "vsfr1",        \
    "vsfr2", "vsfr3", "vsfr4", "vsfr5", "vsfr6", "vsfr7", "vsfr8", "vsfr9",    \
    "vsfr10", "vsfr11", "vsfr12", "vsfr13", "vsfr14", "vsfr15", "vsfr16",      \
    "vsfr17", "vsfr18", "vsfr19", "vsfr20", "vsfr21", "vsfr22", "vsfr23",      \
    "vsfr24", "vsfr25", "vsfr26", "vsfr27", "vsfr28", "vsfr29", "vsfr30",      \
    "vsfr31", "vsfr32", "vsfr33", "vsfr34", "vsfr35", "vsfr36", "vsfr37",      \
    "vsfr38", "vsfr39", "vsfr40", "vsfr41", "vsfr42", "vsfr43", "vsfr44",      \
    "vsfr45", "vsfr46", "vsfr47", "vsfr48", "vsfr49", "vsfr50", "vsfr51",      \
    "vsfr52", "vsfr53", "vsfr54", "vsfr55", "vsfr56", "vsfr57", "vsfr58",      \
    "vsfr59", "vsfr60", "vsfr61", "vsfr62", "vsfr63", "vsfr64", "vsfr65",      \
    "vsfr66", "vsfr67", "vsfr68", "vsfr69", "vsfr70", "vsfr71", "vsfr72",      \
    "vsfr73", "vsfr74", "vsfr75", "vsfr76", "vsfr77", "vsfr78", "vsfr79",      \
    "vsfr80", "vsfr81", "vsfr82", "vsfr83", "vsfr84", "vsfr85", "vsfr86",      \
    "vsfr87", "vsfr88", "vsfr89", "vsfr90", "vsfr91", "vsfr92", "vsfr93",      \
    "vsfr94", "vsfr95", "vsfr96", "vsfr97", "vsfr98", "vsfr99", "vsfr100",     \
    "vsfr101", "vsfr102", "vsfr103", "vsfr104", "vsfr105", "vsfr106",          \
    "vsfr107", "vsfr108", "vsfr109", "vsfr110", "vsfr111", "vsfr112",          \
    "vsfr113", "vsfr114", "vsfr115", "vsfr116", "vsfr117", "vsfr118",          \
    "vsfr119", "vsfr120", "vsfr121", "vsfr122", "vsfr123", "vsfr124",          \
    "vsfr125", "vsfr126", "vsfr127", "vsfr128", "vsfr129", "vsfr130",          \
    "vsfr131", "vsfr132", "vsfr133", "vsfr134", "vsfr135", "vsfr136",          \
    "vsfr137", "vsfr138", "vsfr139", "vsfr140", "vsfr141", "vsfr142",          \
    "vsfr143", "vsfr144", "vsfr145", "vsfr146", "vsfr147", "vsfr148",          \
    "vsfr149", "vsfr150", "vsfr151", "vsfr152", "vsfr153", "vsfr154",          \
    "vsfr155", "vsfr156", "vsfr157", "vsfr158", "vsfr159", "vsfr160",          \
    "vsfr161", "vsfr162", "vsfr163", "vsfr164", "vsfr165", "vsfr166",          \
    "vsfr167", "vsfr168", "vsfr169", "vsfr170", "vsfr171", "vsfr172",          \
    "vsfr173", "vsfr174", "vsfr175", "vsfr176", "vsfr177", "vsfr178",          \
    "vsfr179", "vsfr180", "vsfr181", "vsfr182", "vsfr183", "vsfr184",          \
    "vsfr185", "vsfr186", "vsfr187", "vsfr188", "vsfr189", "vsfr190",          \
    "vsfr191", "vsfr192", "vsfr193", "vsfr194", "vsfr195", "vsfr196",          \
    "vsfr197", "vsfr198", "vsfr199", "vsfr200", "vsfr201", "vsfr202",          \
    "vsfr203", "vsfr204", "vsfr205", "vsfr206", "vsfr207", "vsfr208",          \
    "vsfr209", "vsfr210", "vsfr211", "vsfr212", "vsfr213", "vsfr214",          \
    "vsfr215", "vsfr216", "vsfr217", "vsfr218", "vsfr219", "vsfr220",          \
    "vsfr221", "vsfr222", "vsfr223", "vsfr224", "vsfr225", "vsfr226",          \
    "vsfr227", "vsfr228", "vsfr229", "vsfr230", "vsfr231", "vsfr232",          \
    "vsfr233", "vsfr234", "vsfr235", "vsfr236", "vsfr237", "vsfr238",          \
    "vsfr239", "vsfr240", "vsfr241", "vsfr242", "vsfr243", "vsfr244",          \
    "vsfr245", "vsfr246", "vsfr247", "vsfr248", "vsfr249", "vsfr250",          \
    "vsfr251", "vsfr252", "vsfr253", "vsfr254", "vsfr255",

#define K1C_ABI_REGULAR 0
#define K1C_PROGRAM_POINTER_REGNO 64
#define K1C_STACK_POINTER_REGNO 12
#define K1C_FRAME_POINTER_REGNO 14
#define K1C_LOCAL_POINTER_REGNO 13
#define K1C_STRUCT_POINTER_REGNO 15
#define K1C_RETURN_POINTER_REGNO 67
#define K1C_ARGUMENT_POINTER_REGNO 0
#define K1C_ARG_REG_SLOTS 12

#define K1C_ABI_REGULAR_FIXED_REGISTERS                                        \
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   \
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

#define K1C_ABI_REGULAR_CALL_USED_REGISTERS                                    \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,   \
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

#define K1C_ABI_REGULAR_CALL_REALLY_USED_REGISTERS                             \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,   \
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
