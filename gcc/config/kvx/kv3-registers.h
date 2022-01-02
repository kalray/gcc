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

enum reg_class {
  NO_REGS,
  GPR_REGS,
  SAB_REGS,
  SFR_REGS,
  XCR_REGS,
  ALL_REGS,
  LIM_REG_CLASSES
};

#define REG_CLASS_NAMES { \
  "NO_REGS", \
  "GPR_REGS", \
  "SAB_REGS", \
  "SFR_REGS", \
  "XCR_REGS", \
  "ALL_REGS", \
  "LIM_REG_CLASSES" \
}

#define KV3_GPR_FIRST_REGNO 0
#define KV3_GPR_LAST_REGNO 63
#define KV3_SFR_FIRST_REGNO 64
#define KV3_SFR_LAST_REGNO 255
#define KV3_XCR_FIRST_REGNO 256
#define KV3_XCR_LAST_REGNO 511

#define KV3_MDS_REGISTERS 512
#define FIRST_PSEUDO_REGISTER (KV3_MDS_REGISTERS)
#define KV3_FRAME_POINTER_VIRT_REGNO (KV3_SFR_FIRST_REGNO + 63) // SFR63

#define REG_CLASS_CONTENTS { \
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, }, /* NO_REGS */ \
  { 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, }, /* GPR_REGS */ \
  { 0x00000000, 0x00000000, 0x10000002, 0x00000000, 0x000000f0, 0x0000003f, 0x00000010, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, }, /* SAB_REGS */ \
  { 0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, }, /* SFR_REGS */ \
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, }, /* XCR_REGS */ \
  { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, }, /* ALL_REGS */ \
}

#define REGNO_REG_CLASS(REGNO) ( \
  (((REGNO) <= KV3_GPR_LAST_REGNO)? GPR_REGS: \
    (((REGNO) <= KV3_SFR_LAST_REGNO)? SFR_REGS: \
      (((REGNO) <= KV3_XCR_LAST_REGNO)? XCR_REGS: \
        NO_REGS))))

#define KV3_REGISTER_NAMES \
	"r0",	"r1",	"r2",	"r3",	"r4",	"r5",	"r6",	"r7", \
	"r8",	"r9",	"r10",	"r11",	"r12",	"r13",	"r14",	"r15", \
	"r16",	"r17",	"r18",	"r19",	"r20",	"r21",	"r22",	"r23", \
	"r24",	"r25",	"r26",	"r27",	"r28",	"r29",	"r30",	"r31", \
	"r32",	"r33",	"r34",	"r35",	"r36",	"r37",	"r38",	"r39", \
	"r40",	"r41",	"r42",	"r43",	"r44",	"r45",	"r46",	"r47", \
	"r48",	"r49",	"r50",	"r51",	"r52",	"r53",	"r54",	"r55", \
	"r56",	"r57",	"r58",	"r59",	"r60",	"r61",	"r62",	"r63", \
	"pc",	"ps",	"pcr",	"ra",	"cs",	"csit",	"aespc",	"ls", \
	"le",	"lc",	"ipe",	"men",	"pmc",	"pm0",	"pm1",	"pm2", \
	"pm3",	"pmsa",	"tcr",	"t0v",	"t1v",	"t0r",	"t1r",	"wdv", \
	"wdr",	"ile",	"ill",	"ilr",	"mmc",	"tel",	"teh",	"ixc", \
	"syo",	"hto",	"ito",	"do",	"mo",	"pso",	"tpcm0",	"tpcm1", \
	"dc",	"dba0",	"dba1",	"dwa0",	"dwa1",	"mes",	"ws",	"dc0", \
	"dc1",	"dc2",	"dc3",	"dba2",	"dba3",	"dwa2",	"dwa3",	"tpcm2", \
	"tpcmc",	"res57",	"res58",	"res59",	"res60",	"res61",	"res62",	"res63", \
	"spc_pl0",	"spc_pl1",	"spc_pl2",	"spc_pl3",	"sps_pl0",	"sps_pl1",	"sps_pl2",	"sps_pl3", \
	"ea_pl0",	"ea_pl1",	"ea_pl2",	"ea_pl3",	"ev_pl0",	"ev_pl1",	"ev_pl2",	"ev_pl3", \
	"sr_pl0",	"sr_pl1",	"sr_pl2",	"sr_pl3",	"es_pl0",	"es_pl1",	"es_pl2",	"es_pl3", \
	"sid_pl0",	"sid_pl1",	"sid_pl2",	"sid_pl3",	"sr1_pl0",	"sr1_pl1",	"sr1_pl2",	"sr1_pl3", \
	"syow",	"htow",	"itow",	"dow",	"mow",	"psow",	"res102",	"res103", \
	"res104",	"res105",	"res106",	"res107",	"res108",	"res109",	"res110",	"res111", \
	"res112",	"res113",	"res114",	"res115",	"res116",	"res117",	"res118",	"res119", \
	"res120",	"res121",	"res122",	"res123",	"res124",	"res125",	"res126",	"res127", \
	"spc",	"res129",	"res130",	"res131",	"sps",	"res133",	"res134",	"res135", \
	"ea",	"res137",	"res138",	"res139",	"ev",	"res141",	"res142",	"res143", \
	"sr",	"res145",	"res146",	"res147",	"es",	"res149",	"res150",	"res151", \
	"sid",	"res153",	"res154",	"res155",	"sr1",	"res157",	"res158",	"res159", \
	"res160",	"res161",	"res162",	"res163",	"res164",	"res165",	"res166",	"res167", \
	"res168",	"res169",	"res170",	"res171",	"res172",	"res173",	"res174",	"res175", \
	"res176",	"res177",	"res178",	"res179",	"res180",	"res181",	"res182",	"res183", \
	"res184",	"res185",	"res186",	"res187",	"res188",	"res189",	"res190",	"res191", \
	"a0_x",	"a0_y",	"a0_z",	"a0_t",	"a1_x",	"a1_y",	"a1_z",	"a1_t", \
	"a2_x",	"a2_y",	"a2_z",	"a2_t",	"a3_x",	"a3_y",	"a3_z",	"a3_t", \
	"a4_x",	"a4_y",	"a4_z",	"a4_t",	"a5_x",	"a5_y",	"a5_z",	"a5_t", \
	"a6_x",	"a6_y",	"a6_z",	"a6_t",	"a7_x",	"a7_y",	"a7_z",	"a7_t", \
	"a8_x",	"a8_y",	"a8_z",	"a8_t",	"a9_x",	"a9_y",	"a9_z",	"a9_t", \
	"a10_x",	"a10_y",	"a10_z",	"a10_t",	"a11_x",	"a11_y",	"a11_z",	"a11_t", \
	"a12_x",	"a12_y",	"a12_z",	"a12_t",	"a13_x",	"a13_y",	"a13_z",	"a13_t", \
	"a14_x",	"a14_y",	"a14_z",	"a14_t",	"a15_x",	"a15_y",	"a15_z",	"a15_t", \
	"a16_x",	"a16_y",	"a16_z",	"a16_t",	"a17_x",	"a17_y",	"a17_z",	"a17_t", \
	"a18_x",	"a18_y",	"a18_z",	"a18_t",	"a19_x",	"a19_y",	"a19_z",	"a19_t", \
	"a20_x",	"a20_y",	"a20_z",	"a20_t",	"a21_x",	"a21_y",	"a21_z",	"a21_t", \
	"a22_x",	"a22_y",	"a22_z",	"a22_t",	"a23_x",	"a23_y",	"a23_z",	"a23_t", \
	"a24_x",	"a24_y",	"a24_z",	"a24_t",	"a25_x",	"a25_y",	"a25_z",	"a25_t", \
	"a26_x",	"a26_y",	"a26_z",	"a26_t",	"a27_x",	"a27_y",	"a27_z",	"a27_t", \
	"a28_x",	"a28_y",	"a28_z",	"a28_t",	"a29_x",	"a29_y",	"a29_z",	"a29_t", \
	"a30_x",	"a30_y",	"a30_z",	"a30_t",	"a31_x",	"a31_y",	"a31_z",	"a31_t", \
	"a32_x",	"a32_y",	"a32_z",	"a32_t",	"a33_x",	"a33_y",	"a33_z",	"a33_t", \
	"a34_x",	"a34_y",	"a34_z",	"a34_t",	"a35_x",	"a35_y",	"a35_z",	"a35_t", \
	"a36_x",	"a36_y",	"a36_z",	"a36_t",	"a37_x",	"a37_y",	"a37_z",	"a37_t", \
	"a38_x",	"a38_y",	"a38_z",	"a38_t",	"a39_x",	"a39_y",	"a39_z",	"a39_t", \
	"a40_x",	"a40_y",	"a40_z",	"a40_t",	"a41_x",	"a41_y",	"a41_z",	"a41_t", \
	"a42_x",	"a42_y",	"a42_z",	"a42_t",	"a43_x",	"a43_y",	"a43_z",	"a43_t", \
	"a44_x",	"a44_y",	"a44_z",	"a44_t",	"a45_x",	"a45_y",	"a45_z",	"a45_t", \
	"a46_x",	"a46_y",	"a46_z",	"a46_t",	"a47_x",	"a47_y",	"a47_z",	"a47_t", \
	"a48_x",	"a48_y",	"a48_z",	"a48_t",	"a49_x",	"a49_y",	"a49_z",	"a49_t", \
	"a50_x",	"a50_y",	"a50_z",	"a50_t",	"a51_x",	"a51_y",	"a51_z",	"a51_t", \
	"a52_x",	"a52_y",	"a52_z",	"a52_t",	"a53_x",	"a53_y",	"a53_z",	"a53_t", \
	"a54_x",	"a54_y",	"a54_z",	"a54_t",	"a55_x",	"a55_y",	"a55_z",	"a55_t", \
	"a56_x",	"a56_y",	"a56_z",	"a56_t",	"a57_x",	"a57_y",	"a57_z",	"a57_t", \
	"a58_x",	"a58_y",	"a58_z",	"a58_t",	"a59_x",	"a59_y",	"a59_z",	"a59_t", \
	"a60_x",	"a60_y",	"a60_z",	"a60_t",	"a61_x",	"a61_y",	"a61_z",	"a61_t", \
	"a62_x",	"a62_y",	"a62_z",	"a62_t",	"a63_x",	"a63_y",	"a63_z",	"a63_t",

#define KV3_PGR_REGISTER_NAMES \
	"r0r1",	"ERROR",	"r2r3",	"ERROR",	"r4r5",	"ERROR",	"r6r7",	"ERROR", \
	"r8r9",	"ERROR",	"r10r11",	"ERROR",	"r12r13",	"ERROR",	"r14r15",	"ERROR", \
	"r16r17",	"ERROR",	"r18r19",	"ERROR",	"r20r21",	"ERROR",	"r22r23",	"ERROR", \
	"r24r25",	"ERROR",	"r26r27",	"ERROR",	"r28r29",	"ERROR",	"r30r31",	"ERROR", \
	"r32r33",	"ERROR",	"r34r35",	"ERROR",	"r36r37",	"ERROR",	"r38r39",	"ERROR", \
	"r40r41",	"ERROR",	"r42r43",	"ERROR",	"r44r45",	"ERROR",	"r46r47",	"ERROR", \
	"r48r49",	"ERROR",	"r50r51",	"ERROR",	"r52r53",	"ERROR",	"r54r55",	"ERROR", \
	"r56r57",	"ERROR",	"r58r59",	"ERROR",	"r60r61",	"ERROR",	"r62r63",	"ERROR",

#define KV3_QGR_REGISTER_NAMES \
	"r0r1r2r3",	"ERROR",	"ERROR",	"ERROR",	"r4r5r6r7",	"ERROR",	"ERROR",	"ERROR", \
	"r8r9r10r11",	"ERROR",	"ERROR",	"ERROR",	"r12r13r14r15",	"ERROR",	"ERROR",	"ERROR", \
	"r16r17r18r19",	"ERROR",	"ERROR",	"ERROR",	"r20r21r22r23",	"ERROR",	"ERROR",	"ERROR", \
	"r24r25r26r27",	"ERROR",	"ERROR",	"ERROR",	"r28r29r30r31",	"ERROR",	"ERROR",	"ERROR", \
	"r32r33r34r35",	"ERROR",	"ERROR",	"ERROR",	"r36r37r38r39",	"ERROR",	"ERROR",	"ERROR", \
	"r40r41r42r43",	"ERROR",	"ERROR",	"ERROR",	"r44r45r46r47",	"ERROR",	"ERROR",	"ERROR", \
	"r48r49r50r51",	"ERROR",	"ERROR",	"ERROR",	"r52r53r54r55",	"ERROR",	"ERROR",	"ERROR", \
	"r56r57r58r59",	"ERROR",	"ERROR",	"ERROR",	"r60r61r62r63",	"ERROR",	"ERROR",	"ERROR",

#define KV3_XBR_REGISTER_NAMES \
	"a0_lo",	"ERROR",	"a0_hi",	"ERROR",	"a1_lo",	"ERROR",	"a1_hi",	"ERROR", \
	"a2_lo",	"ERROR",	"a2_hi",	"ERROR",	"a3_lo",	"ERROR",	"a3_hi",	"ERROR", \
	"a4_lo",	"ERROR",	"a4_hi",	"ERROR",	"a5_lo",	"ERROR",	"a5_hi",	"ERROR", \
	"a6_lo",	"ERROR",	"a6_hi",	"ERROR",	"a7_lo",	"ERROR",	"a7_hi",	"ERROR", \
	"a8_lo",	"ERROR",	"a8_hi",	"ERROR",	"a9_lo",	"ERROR",	"a9_hi",	"ERROR", \
	"a10_lo",	"ERROR",	"a10_hi",	"ERROR",	"a11_lo",	"ERROR",	"a11_hi",	"ERROR", \
	"a12_lo",	"ERROR",	"a12_hi",	"ERROR",	"a13_lo",	"ERROR",	"a13_hi",	"ERROR", \
	"a14_lo",	"ERROR",	"a14_hi",	"ERROR",	"a15_lo",	"ERROR",	"a15_hi",	"ERROR", \
	"a16_lo",	"ERROR",	"a16_hi",	"ERROR",	"a17_lo",	"ERROR",	"a17_hi",	"ERROR", \
	"a18_lo",	"ERROR",	"a18_hi",	"ERROR",	"a19_lo",	"ERROR",	"a19_hi",	"ERROR", \
	"a20_lo",	"ERROR",	"a20_hi",	"ERROR",	"a21_lo",	"ERROR",	"a21_hi",	"ERROR", \
	"a22_lo",	"ERROR",	"a22_hi",	"ERROR",	"a23_lo",	"ERROR",	"a23_hi",	"ERROR", \
	"a24_lo",	"ERROR",	"a24_hi",	"ERROR",	"a25_lo",	"ERROR",	"a25_hi",	"ERROR", \
	"a26_lo",	"ERROR",	"a26_hi",	"ERROR",	"a27_lo",	"ERROR",	"a27_hi",	"ERROR", \
	"a28_lo",	"ERROR",	"a28_hi",	"ERROR",	"a29_lo",	"ERROR",	"a29_hi",	"ERROR", \
	"a30_lo",	"ERROR",	"a30_hi",	"ERROR",	"a31_lo",	"ERROR",	"a31_hi",	"ERROR", \
	"a32_lo",	"ERROR",	"a32_hi",	"ERROR",	"a33_lo",	"ERROR",	"a33_hi",	"ERROR", \
	"a34_lo",	"ERROR",	"a34_hi",	"ERROR",	"a35_lo",	"ERROR",	"a35_hi",	"ERROR", \
	"a36_lo",	"ERROR",	"a36_hi",	"ERROR",	"a37_lo",	"ERROR",	"a37_hi",	"ERROR", \
	"a38_lo",	"ERROR",	"a38_hi",	"ERROR",	"a39_lo",	"ERROR",	"a39_hi",	"ERROR", \
	"a40_lo",	"ERROR",	"a40_hi",	"ERROR",	"a41_lo",	"ERROR",	"a41_hi",	"ERROR", \
	"a42_lo",	"ERROR",	"a42_hi",	"ERROR",	"a43_lo",	"ERROR",	"a43_hi",	"ERROR", \
	"a44_lo",	"ERROR",	"a44_hi",	"ERROR",	"a45_lo",	"ERROR",	"a45_hi",	"ERROR", \
	"a46_lo",	"ERROR",	"a46_hi",	"ERROR",	"a47_lo",	"ERROR",	"a47_hi",	"ERROR", \
	"a48_lo",	"ERROR",	"a48_hi",	"ERROR",	"a49_lo",	"ERROR",	"a49_hi",	"ERROR", \
	"a50_lo",	"ERROR",	"a50_hi",	"ERROR",	"a51_lo",	"ERROR",	"a51_hi",	"ERROR", \
	"a52_lo",	"ERROR",	"a52_hi",	"ERROR",	"a53_lo",	"ERROR",	"a53_hi",	"ERROR", \
	"a54_lo",	"ERROR",	"a54_hi",	"ERROR",	"a55_lo",	"ERROR",	"a55_hi",	"ERROR", \
	"a56_lo",	"ERROR",	"a56_hi",	"ERROR",	"a57_lo",	"ERROR",	"a57_hi",	"ERROR", \
	"a58_lo",	"ERROR",	"a58_hi",	"ERROR",	"a59_lo",	"ERROR",	"a59_hi",	"ERROR", \
	"a60_lo",	"ERROR",	"a60_hi",	"ERROR",	"a61_lo",	"ERROR",	"a61_hi",	"ERROR", \
	"a62_lo",	"ERROR",	"a62_hi",	"ERROR",	"a63_lo",	"ERROR",	"a63_hi",	"ERROR",

#define KV3_XVR_REGISTER_NAMES \
	"a0",	"ERROR",	"ERROR",	"ERROR",	"a1",	"ERROR",	"ERROR",	"ERROR", \
	"a2",	"ERROR",	"ERROR",	"ERROR",	"a3",	"ERROR",	"ERROR",	"ERROR", \
	"a4",	"ERROR",	"ERROR",	"ERROR",	"a5",	"ERROR",	"ERROR",	"ERROR", \
	"a6",	"ERROR",	"ERROR",	"ERROR",	"a7",	"ERROR",	"ERROR",	"ERROR", \
	"a8",	"ERROR",	"ERROR",	"ERROR",	"a9",	"ERROR",	"ERROR",	"ERROR", \
	"a10",	"ERROR",	"ERROR",	"ERROR",	"a11",	"ERROR",	"ERROR",	"ERROR", \
	"a12",	"ERROR",	"ERROR",	"ERROR",	"a13",	"ERROR",	"ERROR",	"ERROR", \
	"a14",	"ERROR",	"ERROR",	"ERROR",	"a15",	"ERROR",	"ERROR",	"ERROR", \
	"a16",	"ERROR",	"ERROR",	"ERROR",	"a17",	"ERROR",	"ERROR",	"ERROR", \
	"a18",	"ERROR",	"ERROR",	"ERROR",	"a19",	"ERROR",	"ERROR",	"ERROR", \
	"a20",	"ERROR",	"ERROR",	"ERROR",	"a21",	"ERROR",	"ERROR",	"ERROR", \
	"a22",	"ERROR",	"ERROR",	"ERROR",	"a23",	"ERROR",	"ERROR",	"ERROR", \
	"a24",	"ERROR",	"ERROR",	"ERROR",	"a25",	"ERROR",	"ERROR",	"ERROR", \
	"a26",	"ERROR",	"ERROR",	"ERROR",	"a27",	"ERROR",	"ERROR",	"ERROR", \
	"a28",	"ERROR",	"ERROR",	"ERROR",	"a29",	"ERROR",	"ERROR",	"ERROR", \
	"a30",	"ERROR",	"ERROR",	"ERROR",	"a31",	"ERROR",	"ERROR",	"ERROR", \
	"a32",	"ERROR",	"ERROR",	"ERROR",	"a33",	"ERROR",	"ERROR",	"ERROR", \
	"a34",	"ERROR",	"ERROR",	"ERROR",	"a35",	"ERROR",	"ERROR",	"ERROR", \
	"a36",	"ERROR",	"ERROR",	"ERROR",	"a37",	"ERROR",	"ERROR",	"ERROR", \
	"a38",	"ERROR",	"ERROR",	"ERROR",	"a39",	"ERROR",	"ERROR",	"ERROR", \
	"a40",	"ERROR",	"ERROR",	"ERROR",	"a41",	"ERROR",	"ERROR",	"ERROR", \
	"a42",	"ERROR",	"ERROR",	"ERROR",	"a43",	"ERROR",	"ERROR",	"ERROR", \
	"a44",	"ERROR",	"ERROR",	"ERROR",	"a45",	"ERROR",	"ERROR",	"ERROR", \
	"a46",	"ERROR",	"ERROR",	"ERROR",	"a47",	"ERROR",	"ERROR",	"ERROR", \
	"a48",	"ERROR",	"ERROR",	"ERROR",	"a49",	"ERROR",	"ERROR",	"ERROR", \
	"a50",	"ERROR",	"ERROR",	"ERROR",	"a51",	"ERROR",	"ERROR",	"ERROR", \
	"a52",	"ERROR",	"ERROR",	"ERROR",	"a53",	"ERROR",	"ERROR",	"ERROR", \
	"a54",	"ERROR",	"ERROR",	"ERROR",	"a55",	"ERROR",	"ERROR",	"ERROR", \
	"a56",	"ERROR",	"ERROR",	"ERROR",	"a57",	"ERROR",	"ERROR",	"ERROR", \
	"a58",	"ERROR",	"ERROR",	"ERROR",	"a59",	"ERROR",	"ERROR",	"ERROR", \
	"a60",	"ERROR",	"ERROR",	"ERROR",	"a61",	"ERROR",	"ERROR",	"ERROR", \
	"a62",	"ERROR",	"ERROR",	"ERROR",	"a63",	"ERROR",	"ERROR",	"ERROR",

#define KV3_XWR_REGISTER_NAMES \
	"a0a1",	"ERROR",	"a2a3",	"ERROR",	"a4a5",	"ERROR",	"a6a7",	"ERROR", \
	"a8a9",	"ERROR",	"a10a11",	"ERROR",	"a12a13",	"ERROR",	"a14a15",	"ERROR", \
	"a16a17",	"ERROR",	"a18a19",	"ERROR",	"a20a21",	"ERROR",	"a22a23",	"ERROR", \
	"a24a25",	"ERROR",	"a26a27",	"ERROR",	"a28a29",	"ERROR",	"a30a31",	"ERROR", \
	"a32a33",	"ERROR",	"a34a35",	"ERROR",	"a36a37",	"ERROR",	"a38a39",	"ERROR", \
	"a40a41",	"ERROR",	"a42a43",	"ERROR",	"a44a45",	"ERROR",	"a46a47",	"ERROR", \
	"a48a49",	"ERROR",	"a50a51",	"ERROR",	"a52a53",	"ERROR",	"a54a55",	"ERROR", \
	"a56a57",	"ERROR",	"a58a59",	"ERROR",	"a60a61",	"ERROR",	"a62a63",	"ERROR",

#define KV3_XMR_REGISTER_NAMES \
	"a0a1a2a3",	"ERROR",	"ERROR",	"ERROR",	"a4a5a6a7",	"ERROR",	"ERROR",	"ERROR", \
	"a8a9a10a11",	"ERROR",	"ERROR",	"ERROR",	"a12a13a14a15",	"ERROR",	"ERROR",	"ERROR", \
	"a16a17a18a19",	"ERROR",	"ERROR",	"ERROR",	"a20a21a22a23",	"ERROR",	"ERROR",	"ERROR", \
	"a24a25a26a27",	"ERROR",	"ERROR",	"ERROR",	"a28a29a30a31",	"ERROR",	"ERROR",	"ERROR", \
	"a32a33a34a35",	"ERROR",	"ERROR",	"ERROR",	"a36a37a38a39",	"ERROR",	"ERROR",	"ERROR", \
	"a40a41a42a43",	"ERROR",	"ERROR",	"ERROR",	"a44a45a46a47",	"ERROR",	"ERROR",	"ERROR", \
	"a48a49a50a51",	"ERROR",	"ERROR",	"ERROR",	"a52a53a54a55",	"ERROR",	"ERROR",	"ERROR", \
	"a56a57a58a59",	"ERROR",	"ERROR",	"ERROR",	"a60a61a62a63",	"ERROR",	"ERROR",	"ERROR",

#define KV3_ABI_REGULAR 0
#define KV3_PROGRAM_POINTER_REGNO 64
#define KV3_STACK_POINTER_REGNO 12
#define KV3_FRAME_POINTER_REGNO 14
#define KV3_LOCAL_POINTER_REGNO 13
#define KV3_STRUCT_POINTER_REGNO 15
#define KV3_RETURN_POINTER_REGNO 67
#define KV3_ARGUMENT_POINTER_REGNO 0
#define KV3_ARG_REG_SLOTS 12


#define KV3_ABI_REGULAR_FIXED_REGISTERS \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	1,	1,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0,

#define KV3_ABI_REGULAR_CALL_USED_REGISTERS \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	0,	1, \
	1,	1,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0,

#define KV3_ABI_REGULAR_CALL_REALLY_USED_REGISTERS \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	0,	1, \
	1,	1,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	0,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	1,	1,	1,	1,	1,	1,	1,	1, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0, \
	0,	0,	0,	0,	0,	0,	0,	0,

