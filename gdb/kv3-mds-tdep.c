/* Generated target-description for kv3 */
/* (c) Copyright 2010-2018 Kalray SA. */
#include "defs.h"

#include <string.h>

#include "gdbarch.h"
#include "gdbtypes.h"
#include "regcache.h"
#include "reggroups.h"
#include "user-regs.h"
#include "target-descriptions.h"
#include "kvx-common-tdep.h"

struct pseudo_desc {
	const char *name;
	struct type *type;
	int size;
	int nb_components;
	int components[16];
	char *components_names[16];
};

struct dwarf2gdb_desc {
	int gdb_regno;
	const char *name;
};

static struct dwarf2gdb_desc dwarf2gdb[512];

static struct pseudo_desc v1_pseudo_regs[] = {
	{ "r0r1", NULL, 128, 2, { -1, -1, }, { "r0", "r1"}},
	{ "r10r11", NULL, 128, 2, { -1, -1, }, { "r10", "r11"}},
	{ "r12r13", NULL, 128, 2, { -1, -1, }, { "r12", "r13"}},
	{ "r14r15", NULL, 128, 2, { -1, -1, }, { "r14", "r15"}},
	{ "r16r17", NULL, 128, 2, { -1, -1, }, { "r16", "r17"}},
	{ "r18r19", NULL, 128, 2, { -1, -1, }, { "r18", "r19"}},
	{ "r2r3", NULL, 128, 2, { -1, -1, }, { "r2", "r3"}},
	{ "r20r21", NULL, 128, 2, { -1, -1, }, { "r20", "r21"}},
	{ "r22r23", NULL, 128, 2, { -1, -1, }, { "r22", "r23"}},
	{ "r24r25", NULL, 128, 2, { -1, -1, }, { "r24", "r25"}},
	{ "r26r27", NULL, 128, 2, { -1, -1, }, { "r26", "r27"}},
	{ "r28r29", NULL, 128, 2, { -1, -1, }, { "r28", "r29"}},
	{ "r30r31", NULL, 128, 2, { -1, -1, }, { "r30", "r31"}},
	{ "r32r33", NULL, 128, 2, { -1, -1, }, { "r32", "r33"}},
	{ "r34r35", NULL, 128, 2, { -1, -1, }, { "r34", "r35"}},
	{ "r36r37", NULL, 128, 2, { -1, -1, }, { "r36", "r37"}},
	{ "r38r39", NULL, 128, 2, { -1, -1, }, { "r38", "r39"}},
	{ "r4r5", NULL, 128, 2, { -1, -1, }, { "r4", "r5"}},
	{ "r40r41", NULL, 128, 2, { -1, -1, }, { "r40", "r41"}},
	{ "r42r43", NULL, 128, 2, { -1, -1, }, { "r42", "r43"}},
	{ "r44r45", NULL, 128, 2, { -1, -1, }, { "r44", "r45"}},
	{ "r46r47", NULL, 128, 2, { -1, -1, }, { "r46", "r47"}},
	{ "r48r49", NULL, 128, 2, { -1, -1, }, { "r48", "r49"}},
	{ "r50r51", NULL, 128, 2, { -1, -1, }, { "r50", "r51"}},
	{ "r52r53", NULL, 128, 2, { -1, -1, }, { "r52", "r53"}},
	{ "r54r55", NULL, 128, 2, { -1, -1, }, { "r54", "r55"}},
	{ "r56r57", NULL, 128, 2, { -1, -1, }, { "r56", "r57"}},
	{ "r58r59", NULL, 128, 2, { -1, -1, }, { "r58", "r59"}},
	{ "r6r7", NULL, 128, 2, { -1, -1, }, { "r6", "r7"}},
	{ "r60r61", NULL, 128, 2, { -1, -1, }, { "r60", "r61"}},
	{ "r62r63", NULL, 128, 2, { -1, -1, }, { "r62", "r63"}},
	{ "r8r9", NULL, 128, 2, { -1, -1, }, { "r8", "r9"}},
	{ "r0r1r2r3", NULL, 256, 4, { -1, -1, -1, -1, }, { "r0", "r1", "r2", "r3"}},
	{ "r12r13r14r15", NULL, 256, 4, { -1, -1, -1, -1, }, { "r12", "r13", "r14", "r15"}},
	{ "r16r17r18r19", NULL, 256, 4, { -1, -1, -1, -1, }, { "r16", "r17", "r18", "r19"}},
	{ "r20r21r22r23", NULL, 256, 4, { -1, -1, -1, -1, }, { "r20", "r21", "r22", "r23"}},
	{ "r24r25r26r27", NULL, 256, 4, { -1, -1, -1, -1, }, { "r24", "r25", "r26", "r27"}},
	{ "r28r29r30r31", NULL, 256, 4, { -1, -1, -1, -1, }, { "r28", "r29", "r30", "r31"}},
	{ "r32r33r34r35", NULL, 256, 4, { -1, -1, -1, -1, }, { "r32", "r33", "r34", "r35"}},
	{ "r36r37r38r39", NULL, 256, 4, { -1, -1, -1, -1, }, { "r36", "r37", "r38", "r39"}},
	{ "r4r5r6r7", NULL, 256, 4, { -1, -1, -1, -1, }, { "r4", "r5", "r6", "r7"}},
	{ "r40r41r42r43", NULL, 256, 4, { -1, -1, -1, -1, }, { "r40", "r41", "r42", "r43"}},
	{ "r44r45r46r47", NULL, 256, 4, { -1, -1, -1, -1, }, { "r44", "r45", "r46", "r47"}},
	{ "r48r49r50r51", NULL, 256, 4, { -1, -1, -1, -1, }, { "r48", "r49", "r50", "r51"}},
	{ "r52r53r54r55", NULL, 256, 4, { -1, -1, -1, -1, }, { "r52", "r53", "r54", "r55"}},
	{ "r56r57r58r59", NULL, 256, 4, { -1, -1, -1, -1, }, { "r56", "r57", "r58", "r59"}},
	{ "r60r61r62r63", NULL, 256, 4, { -1, -1, -1, -1, }, { "r60", "r61", "r62", "r63"}},
	{ "r8r9r10r11", NULL, 256, 4, { -1, -1, -1, -1, }, { "r8", "r9", "r10", "r11"}},
};

static const int v1_num_pseudo_regs = 48;

static const char *_v1_sp_name = "r12";
static const char *_v1_pc_name = "pc";

static int init_v1_dwarf2gdb(struct gdbarch *gdbarch)
{
	int i;
	memset (dwarf2gdb, -1, sizeof(dwarf2gdb));
	dwarf2gdb[64].name = "pc";
	dwarf2gdb[64].gdb_regno = 0;
	dwarf2gdb[65].name = "ps";
	dwarf2gdb[65].gdb_regno = 1;
	dwarf2gdb[66].name = "pcr";
	dwarf2gdb[66].gdb_regno = 2;
	dwarf2gdb[67].name = "ra";
	dwarf2gdb[67].gdb_regno = 3;
	dwarf2gdb[68].name = "cs";
	dwarf2gdb[68].gdb_regno = 4;
	dwarf2gdb[69].name = "csit";
	dwarf2gdb[69].gdb_regno = 5;
	dwarf2gdb[70].name = "aespc";
	dwarf2gdb[70].gdb_regno = 6;
	dwarf2gdb[71].name = "ls";
	dwarf2gdb[71].gdb_regno = 7;
	dwarf2gdb[72].name = "le";
	dwarf2gdb[72].gdb_regno = 8;
	dwarf2gdb[73].name = "lc";
	dwarf2gdb[73].gdb_regno = 9;
	dwarf2gdb[74].name = "ipe";
	dwarf2gdb[74].gdb_regno = 10;
	dwarf2gdb[75].name = "men";
	dwarf2gdb[75].gdb_regno = 11;
	dwarf2gdb[76].name = "pmc";
	dwarf2gdb[76].gdb_regno = 12;
	dwarf2gdb[77].name = "pm0";
	dwarf2gdb[77].gdb_regno = 13;
	dwarf2gdb[78].name = "pm1";
	dwarf2gdb[78].gdb_regno = 14;
	dwarf2gdb[79].name = "pm2";
	dwarf2gdb[79].gdb_regno = 15;
	dwarf2gdb[80].name = "pm3";
	dwarf2gdb[80].gdb_regno = 16;
	dwarf2gdb[81].name = "pmsa";
	dwarf2gdb[81].gdb_regno = 17;
	dwarf2gdb[82].name = "tcr";
	dwarf2gdb[82].gdb_regno = 18;
	dwarf2gdb[83].name = "t0v";
	dwarf2gdb[83].gdb_regno = 19;
	dwarf2gdb[84].name = "t1v";
	dwarf2gdb[84].gdb_regno = 20;
	dwarf2gdb[85].name = "t0r";
	dwarf2gdb[85].gdb_regno = 21;
	dwarf2gdb[86].name = "t1r";
	dwarf2gdb[86].gdb_regno = 22;
	dwarf2gdb[87].name = "wdv";
	dwarf2gdb[87].gdb_regno = 23;
	dwarf2gdb[88].name = "wdr";
	dwarf2gdb[88].gdb_regno = 24;
	dwarf2gdb[89].name = "ile";
	dwarf2gdb[89].gdb_regno = 25;
	dwarf2gdb[90].name = "ill";
	dwarf2gdb[90].gdb_regno = 26;
	dwarf2gdb[91].name = "ilr";
	dwarf2gdb[91].gdb_regno = 27;
	dwarf2gdb[92].name = "mmc";
	dwarf2gdb[92].gdb_regno = 28;
	dwarf2gdb[93].name = "tel";
	dwarf2gdb[93].gdb_regno = 29;
	dwarf2gdb[94].name = "teh";
	dwarf2gdb[94].gdb_regno = 30;
	dwarf2gdb[95].name = "res31";
	dwarf2gdb[95].gdb_regno = 31;
	dwarf2gdb[96].name = "syo";
	dwarf2gdb[96].gdb_regno = 32;
	dwarf2gdb[97].name = "hto";
	dwarf2gdb[97].gdb_regno = 33;
	dwarf2gdb[98].name = "ito";
	dwarf2gdb[98].gdb_regno = 34;
	dwarf2gdb[99].name = "do";
	dwarf2gdb[99].gdb_regno = 35;
	dwarf2gdb[100].name = "mo";
	dwarf2gdb[100].gdb_regno = 36;
	dwarf2gdb[101].name = "pso";
	dwarf2gdb[101].gdb_regno = 37;
	dwarf2gdb[102].name = "res38";
	dwarf2gdb[102].gdb_regno = 38;
	dwarf2gdb[103].name = "res39";
	dwarf2gdb[103].gdb_regno = 39;
	dwarf2gdb[104].name = "dc";
	dwarf2gdb[104].gdb_regno = 40;
	dwarf2gdb[105].name = "dba0";
	dwarf2gdb[105].gdb_regno = 41;
	dwarf2gdb[106].name = "dba1";
	dwarf2gdb[106].gdb_regno = 42;
	dwarf2gdb[107].name = "dwa0";
	dwarf2gdb[107].gdb_regno = 43;
	dwarf2gdb[108].name = "dwa1";
	dwarf2gdb[108].gdb_regno = 44;
	dwarf2gdb[109].name = "mes";
	dwarf2gdb[109].gdb_regno = 45;
	dwarf2gdb[110].name = "ws";
	dwarf2gdb[110].gdb_regno = 46;
	dwarf2gdb[111].name = "res47";
	dwarf2gdb[111].gdb_regno = 47;
	dwarf2gdb[112].name = "res48";
	dwarf2gdb[112].gdb_regno = 48;
	dwarf2gdb[113].name = "res49";
	dwarf2gdb[113].gdb_regno = 49;
	dwarf2gdb[114].name = "res50";
	dwarf2gdb[114].gdb_regno = 50;
	dwarf2gdb[115].name = "res51";
	dwarf2gdb[115].gdb_regno = 51;
	dwarf2gdb[116].name = "res52";
	dwarf2gdb[116].gdb_regno = 52;
	dwarf2gdb[117].name = "res53";
	dwarf2gdb[117].gdb_regno = 53;
	dwarf2gdb[118].name = "res54";
	dwarf2gdb[118].gdb_regno = 54;
	dwarf2gdb[119].name = "res55";
	dwarf2gdb[119].gdb_regno = 55;
	dwarf2gdb[120].name = "res56";
	dwarf2gdb[120].gdb_regno = 56;
	dwarf2gdb[121].name = "res57";
	dwarf2gdb[121].gdb_regno = 57;
	dwarf2gdb[122].name = "res58";
	dwarf2gdb[122].gdb_regno = 58;
	dwarf2gdb[123].name = "res59";
	dwarf2gdb[123].gdb_regno = 59;
	dwarf2gdb[124].name = "res60";
	dwarf2gdb[124].gdb_regno = 60;
	dwarf2gdb[125].name = "res61";
	dwarf2gdb[125].gdb_regno = 61;
	dwarf2gdb[126].name = "res62";
	dwarf2gdb[126].gdb_regno = 62;
	dwarf2gdb[127].name = "res63";
	dwarf2gdb[127].gdb_regno = 63;
	dwarf2gdb[128].name = "spc_pl0";
	dwarf2gdb[128].gdb_regno = 64;
	dwarf2gdb[129].name = "spc_pl1";
	dwarf2gdb[129].gdb_regno = 65;
	dwarf2gdb[130].name = "spc_pl2";
	dwarf2gdb[130].gdb_regno = 66;
	dwarf2gdb[131].name = "spc_pl3";
	dwarf2gdb[131].gdb_regno = 67;
	dwarf2gdb[132].name = "sps_pl0";
	dwarf2gdb[132].gdb_regno = 68;
	dwarf2gdb[133].name = "sps_pl1";
	dwarf2gdb[133].gdb_regno = 69;
	dwarf2gdb[134].name = "sps_pl2";
	dwarf2gdb[134].gdb_regno = 70;
	dwarf2gdb[135].name = "sps_pl3";
	dwarf2gdb[135].gdb_regno = 71;
	dwarf2gdb[136].name = "ea_pl0";
	dwarf2gdb[136].gdb_regno = 72;
	dwarf2gdb[137].name = "ea_pl1";
	dwarf2gdb[137].gdb_regno = 73;
	dwarf2gdb[138].name = "ea_pl2";
	dwarf2gdb[138].gdb_regno = 74;
	dwarf2gdb[139].name = "ea_pl3";
	dwarf2gdb[139].gdb_regno = 75;
	dwarf2gdb[140].name = "ev_pl0";
	dwarf2gdb[140].gdb_regno = 76;
	dwarf2gdb[141].name = "ev_pl1";
	dwarf2gdb[141].gdb_regno = 77;
	dwarf2gdb[142].name = "ev_pl2";
	dwarf2gdb[142].gdb_regno = 78;
	dwarf2gdb[143].name = "ev_pl3";
	dwarf2gdb[143].gdb_regno = 79;
	dwarf2gdb[144].name = "sr_pl0";
	dwarf2gdb[144].gdb_regno = 80;
	dwarf2gdb[145].name = "sr_pl1";
	dwarf2gdb[145].gdb_regno = 81;
	dwarf2gdb[146].name = "sr_pl2";
	dwarf2gdb[146].gdb_regno = 82;
	dwarf2gdb[147].name = "sr_pl3";
	dwarf2gdb[147].gdb_regno = 83;
	dwarf2gdb[148].name = "es_pl0";
	dwarf2gdb[148].gdb_regno = 84;
	dwarf2gdb[149].name = "es_pl1";
	dwarf2gdb[149].gdb_regno = 85;
	dwarf2gdb[150].name = "es_pl2";
	dwarf2gdb[150].gdb_regno = 86;
	dwarf2gdb[151].name = "es_pl3";
	dwarf2gdb[151].gdb_regno = 87;
	dwarf2gdb[152].name = "res88";
	dwarf2gdb[152].gdb_regno = 88;
	dwarf2gdb[153].name = "res89";
	dwarf2gdb[153].gdb_regno = 89;
	dwarf2gdb[154].name = "res90";
	dwarf2gdb[154].gdb_regno = 90;
	dwarf2gdb[155].name = "res91";
	dwarf2gdb[155].gdb_regno = 91;
	dwarf2gdb[156].name = "res92";
	dwarf2gdb[156].gdb_regno = 92;
	dwarf2gdb[157].name = "res93";
	dwarf2gdb[157].gdb_regno = 93;
	dwarf2gdb[158].name = "res94";
	dwarf2gdb[158].gdb_regno = 94;
	dwarf2gdb[159].name = "res95";
	dwarf2gdb[159].gdb_regno = 95;
	dwarf2gdb[160].name = "syow";
	dwarf2gdb[160].gdb_regno = 96;
	dwarf2gdb[161].name = "htow";
	dwarf2gdb[161].gdb_regno = 97;
	dwarf2gdb[162].name = "itow";
	dwarf2gdb[162].gdb_regno = 98;
	dwarf2gdb[163].name = "dow";
	dwarf2gdb[163].gdb_regno = 99;
	dwarf2gdb[164].name = "mow";
	dwarf2gdb[164].gdb_regno = 100;
	dwarf2gdb[165].name = "psow";
	dwarf2gdb[165].gdb_regno = 101;
	dwarf2gdb[166].name = "res102";
	dwarf2gdb[166].gdb_regno = 102;
	dwarf2gdb[167].name = "res103";
	dwarf2gdb[167].gdb_regno = 103;
	dwarf2gdb[168].name = "res104";
	dwarf2gdb[168].gdb_regno = 104;
	dwarf2gdb[169].name = "res105";
	dwarf2gdb[169].gdb_regno = 105;
	dwarf2gdb[170].name = "res106";
	dwarf2gdb[170].gdb_regno = 106;
	dwarf2gdb[171].name = "res107";
	dwarf2gdb[171].gdb_regno = 107;
	dwarf2gdb[172].name = "res108";
	dwarf2gdb[172].gdb_regno = 108;
	dwarf2gdb[173].name = "res109";
	dwarf2gdb[173].gdb_regno = 109;
	dwarf2gdb[174].name = "res110";
	dwarf2gdb[174].gdb_regno = 110;
	dwarf2gdb[175].name = "res111";
	dwarf2gdb[175].gdb_regno = 111;
	dwarf2gdb[176].name = "res112";
	dwarf2gdb[176].gdb_regno = 112;
	dwarf2gdb[177].name = "res113";
	dwarf2gdb[177].gdb_regno = 113;
	dwarf2gdb[178].name = "res114";
	dwarf2gdb[178].gdb_regno = 114;
	dwarf2gdb[179].name = "res115";
	dwarf2gdb[179].gdb_regno = 115;
	dwarf2gdb[180].name = "res116";
	dwarf2gdb[180].gdb_regno = 116;
	dwarf2gdb[181].name = "res117";
	dwarf2gdb[181].gdb_regno = 117;
	dwarf2gdb[182].name = "res118";
	dwarf2gdb[182].gdb_regno = 118;
	dwarf2gdb[183].name = "res119";
	dwarf2gdb[183].gdb_regno = 119;
	dwarf2gdb[184].name = "res120";
	dwarf2gdb[184].gdb_regno = 120;
	dwarf2gdb[185].name = "res121";
	dwarf2gdb[185].gdb_regno = 121;
	dwarf2gdb[186].name = "res122";
	dwarf2gdb[186].gdb_regno = 122;
	dwarf2gdb[187].name = "res123";
	dwarf2gdb[187].gdb_regno = 123;
	dwarf2gdb[188].name = "res124";
	dwarf2gdb[188].gdb_regno = 124;
	dwarf2gdb[189].name = "res125";
	dwarf2gdb[189].gdb_regno = 125;
	dwarf2gdb[190].name = "res126";
	dwarf2gdb[190].gdb_regno = 126;
	dwarf2gdb[191].name = "res127";
	dwarf2gdb[191].gdb_regno = 127;
	dwarf2gdb[192].name = "spc";
	dwarf2gdb[192].gdb_regno = 128;
	dwarf2gdb[193].name = "res129";
	dwarf2gdb[193].gdb_regno = 129;
	dwarf2gdb[194].name = "res130";
	dwarf2gdb[194].gdb_regno = 130;
	dwarf2gdb[195].name = "res131";
	dwarf2gdb[195].gdb_regno = 131;
	dwarf2gdb[196].name = "sps";
	dwarf2gdb[196].gdb_regno = 132;
	dwarf2gdb[197].name = "res133";
	dwarf2gdb[197].gdb_regno = 133;
	dwarf2gdb[198].name = "res134";
	dwarf2gdb[198].gdb_regno = 134;
	dwarf2gdb[199].name = "res135";
	dwarf2gdb[199].gdb_regno = 135;
	dwarf2gdb[200].name = "ea";
	dwarf2gdb[200].gdb_regno = 136;
	dwarf2gdb[201].name = "res137";
	dwarf2gdb[201].gdb_regno = 137;
	dwarf2gdb[202].name = "res138";
	dwarf2gdb[202].gdb_regno = 138;
	dwarf2gdb[203].name = "res139";
	dwarf2gdb[203].gdb_regno = 139;
	dwarf2gdb[204].name = "ev";
	dwarf2gdb[204].gdb_regno = 140;
	dwarf2gdb[205].name = "res141";
	dwarf2gdb[205].gdb_regno = 141;
	dwarf2gdb[206].name = "res142";
	dwarf2gdb[206].gdb_regno = 142;
	dwarf2gdb[207].name = "res143";
	dwarf2gdb[207].gdb_regno = 143;
	dwarf2gdb[208].name = "sr";
	dwarf2gdb[208].gdb_regno = 144;
	dwarf2gdb[209].name = "res145";
	dwarf2gdb[209].gdb_regno = 145;
	dwarf2gdb[210].name = "res146";
	dwarf2gdb[210].gdb_regno = 146;
	dwarf2gdb[211].name = "res147";
	dwarf2gdb[211].gdb_regno = 147;
	dwarf2gdb[212].name = "es";
	dwarf2gdb[212].gdb_regno = 148;
	dwarf2gdb[213].name = "res149";
	dwarf2gdb[213].gdb_regno = 149;
	dwarf2gdb[214].name = "res150";
	dwarf2gdb[214].gdb_regno = 150;
	dwarf2gdb[215].name = "res151";
	dwarf2gdb[215].gdb_regno = 151;
	dwarf2gdb[216].name = "res152";
	dwarf2gdb[216].gdb_regno = 152;
	dwarf2gdb[217].name = "res153";
	dwarf2gdb[217].gdb_regno = 153;
	dwarf2gdb[218].name = "res154";
	dwarf2gdb[218].gdb_regno = 154;
	dwarf2gdb[219].name = "res155";
	dwarf2gdb[219].gdb_regno = 155;
	dwarf2gdb[220].name = "res156";
	dwarf2gdb[220].gdb_regno = 156;
	dwarf2gdb[221].name = "res157";
	dwarf2gdb[221].gdb_regno = 157;
	dwarf2gdb[222].name = "res158";
	dwarf2gdb[222].gdb_regno = 158;
	dwarf2gdb[223].name = "res159";
	dwarf2gdb[223].gdb_regno = 159;
	dwarf2gdb[224].name = "res160";
	dwarf2gdb[224].gdb_regno = 160;
	dwarf2gdb[225].name = "res161";
	dwarf2gdb[225].gdb_regno = 161;
	dwarf2gdb[226].name = "res162";
	dwarf2gdb[226].gdb_regno = 162;
	dwarf2gdb[227].name = "res163";
	dwarf2gdb[227].gdb_regno = 163;
	dwarf2gdb[228].name = "res164";
	dwarf2gdb[228].gdb_regno = 164;
	dwarf2gdb[229].name = "res165";
	dwarf2gdb[229].gdb_regno = 165;
	dwarf2gdb[230].name = "res166";
	dwarf2gdb[230].gdb_regno = 166;
	dwarf2gdb[231].name = "res167";
	dwarf2gdb[231].gdb_regno = 167;
	dwarf2gdb[232].name = "res168";
	dwarf2gdb[232].gdb_regno = 168;
	dwarf2gdb[233].name = "res169";
	dwarf2gdb[233].gdb_regno = 169;
	dwarf2gdb[234].name = "res170";
	dwarf2gdb[234].gdb_regno = 170;
	dwarf2gdb[235].name = "res171";
	dwarf2gdb[235].gdb_regno = 171;
	dwarf2gdb[236].name = "res172";
	dwarf2gdb[236].gdb_regno = 172;
	dwarf2gdb[237].name = "res173";
	dwarf2gdb[237].gdb_regno = 173;
	dwarf2gdb[238].name = "res174";
	dwarf2gdb[238].gdb_regno = 174;
	dwarf2gdb[239].name = "res175";
	dwarf2gdb[239].gdb_regno = 175;
	dwarf2gdb[240].name = "res176";
	dwarf2gdb[240].gdb_regno = 176;
	dwarf2gdb[241].name = "res177";
	dwarf2gdb[241].gdb_regno = 177;
	dwarf2gdb[242].name = "res178";
	dwarf2gdb[242].gdb_regno = 178;
	dwarf2gdb[243].name = "res179";
	dwarf2gdb[243].gdb_regno = 179;
	dwarf2gdb[244].name = "res180";
	dwarf2gdb[244].gdb_regno = 180;
	dwarf2gdb[245].name = "res181";
	dwarf2gdb[245].gdb_regno = 181;
	dwarf2gdb[246].name = "res182";
	dwarf2gdb[246].gdb_regno = 182;
	dwarf2gdb[247].name = "res183";
	dwarf2gdb[247].gdb_regno = 183;
	dwarf2gdb[248].name = "res184";
	dwarf2gdb[248].gdb_regno = 184;
	dwarf2gdb[249].name = "res185";
	dwarf2gdb[249].gdb_regno = 185;
	dwarf2gdb[250].name = "res186";
	dwarf2gdb[250].gdb_regno = 186;
	dwarf2gdb[251].name = "res187";
	dwarf2gdb[251].gdb_regno = 187;
	dwarf2gdb[252].name = "res188";
	dwarf2gdb[252].gdb_regno = 188;
	dwarf2gdb[253].name = "res189";
	dwarf2gdb[253].gdb_regno = 189;
	dwarf2gdb[254].name = "res190";
	dwarf2gdb[254].gdb_regno = 190;
	dwarf2gdb[255].name = "res191";
	dwarf2gdb[255].gdb_regno = 191;
	dwarf2gdb[0].name = "r0";
	dwarf2gdb[0].gdb_regno = 192;
	dwarf2gdb[1].name = "r1";
	dwarf2gdb[1].gdb_regno = 193;
	dwarf2gdb[2].name = "r2";
	dwarf2gdb[2].gdb_regno = 194;
	dwarf2gdb[3].name = "r3";
	dwarf2gdb[3].gdb_regno = 195;
	dwarf2gdb[4].name = "r4";
	dwarf2gdb[4].gdb_regno = 196;
	dwarf2gdb[5].name = "r5";
	dwarf2gdb[5].gdb_regno = 197;
	dwarf2gdb[6].name = "r6";
	dwarf2gdb[6].gdb_regno = 198;
	dwarf2gdb[7].name = "r7";
	dwarf2gdb[7].gdb_regno = 199;
	dwarf2gdb[8].name = "r8";
	dwarf2gdb[8].gdb_regno = 200;
	dwarf2gdb[9].name = "r9";
	dwarf2gdb[9].gdb_regno = 201;
	dwarf2gdb[10].name = "r10";
	dwarf2gdb[10].gdb_regno = 202;
	dwarf2gdb[11].name = "r11";
	dwarf2gdb[11].gdb_regno = 203;
	dwarf2gdb[12].name = "r12";
	dwarf2gdb[12].gdb_regno = 204;
	dwarf2gdb[13].name = "r13";
	dwarf2gdb[13].gdb_regno = 205;
	dwarf2gdb[14].name = "r14";
	dwarf2gdb[14].gdb_regno = 206;
	dwarf2gdb[15].name = "r15";
	dwarf2gdb[15].gdb_regno = 207;
	dwarf2gdb[16].name = "r16";
	dwarf2gdb[16].gdb_regno = 208;
	dwarf2gdb[17].name = "r17";
	dwarf2gdb[17].gdb_regno = 209;
	dwarf2gdb[18].name = "r18";
	dwarf2gdb[18].gdb_regno = 210;
	dwarf2gdb[19].name = "r19";
	dwarf2gdb[19].gdb_regno = 211;
	dwarf2gdb[20].name = "r20";
	dwarf2gdb[20].gdb_regno = 212;
	dwarf2gdb[21].name = "r21";
	dwarf2gdb[21].gdb_regno = 213;
	dwarf2gdb[22].name = "r22";
	dwarf2gdb[22].gdb_regno = 214;
	dwarf2gdb[23].name = "r23";
	dwarf2gdb[23].gdb_regno = 215;
	dwarf2gdb[24].name = "r24";
	dwarf2gdb[24].gdb_regno = 216;
	dwarf2gdb[25].name = "r25";
	dwarf2gdb[25].gdb_regno = 217;
	dwarf2gdb[26].name = "r26";
	dwarf2gdb[26].gdb_regno = 218;
	dwarf2gdb[27].name = "r27";
	dwarf2gdb[27].gdb_regno = 219;
	dwarf2gdb[28].name = "r28";
	dwarf2gdb[28].gdb_regno = 220;
	dwarf2gdb[29].name = "r29";
	dwarf2gdb[29].gdb_regno = 221;
	dwarf2gdb[30].name = "r30";
	dwarf2gdb[30].gdb_regno = 222;
	dwarf2gdb[31].name = "r31";
	dwarf2gdb[31].gdb_regno = 223;
	dwarf2gdb[32].name = "r32";
	dwarf2gdb[32].gdb_regno = 224;
	dwarf2gdb[33].name = "r33";
	dwarf2gdb[33].gdb_regno = 225;
	dwarf2gdb[34].name = "r34";
	dwarf2gdb[34].gdb_regno = 226;
	dwarf2gdb[35].name = "r35";
	dwarf2gdb[35].gdb_regno = 227;
	dwarf2gdb[36].name = "r36";
	dwarf2gdb[36].gdb_regno = 228;
	dwarf2gdb[37].name = "r37";
	dwarf2gdb[37].gdb_regno = 229;
	dwarf2gdb[38].name = "r38";
	dwarf2gdb[38].gdb_regno = 230;
	dwarf2gdb[39].name = "r39";
	dwarf2gdb[39].gdb_regno = 231;
	dwarf2gdb[40].name = "r40";
	dwarf2gdb[40].gdb_regno = 232;
	dwarf2gdb[41].name = "r41";
	dwarf2gdb[41].gdb_regno = 233;
	dwarf2gdb[42].name = "r42";
	dwarf2gdb[42].gdb_regno = 234;
	dwarf2gdb[43].name = "r43";
	dwarf2gdb[43].gdb_regno = 235;
	dwarf2gdb[44].name = "r44";
	dwarf2gdb[44].gdb_regno = 236;
	dwarf2gdb[45].name = "r45";
	dwarf2gdb[45].gdb_regno = 237;
	dwarf2gdb[46].name = "r46";
	dwarf2gdb[46].gdb_regno = 238;
	dwarf2gdb[47].name = "r47";
	dwarf2gdb[47].gdb_regno = 239;
	dwarf2gdb[48].name = "r48";
	dwarf2gdb[48].gdb_regno = 240;
	dwarf2gdb[49].name = "r49";
	dwarf2gdb[49].gdb_regno = 241;
	dwarf2gdb[50].name = "r50";
	dwarf2gdb[50].gdb_regno = 242;
	dwarf2gdb[51].name = "r51";
	dwarf2gdb[51].gdb_regno = 243;
	dwarf2gdb[52].name = "r52";
	dwarf2gdb[52].gdb_regno = 244;
	dwarf2gdb[53].name = "r53";
	dwarf2gdb[53].gdb_regno = 245;
	dwarf2gdb[54].name = "r54";
	dwarf2gdb[54].gdb_regno = 246;
	dwarf2gdb[55].name = "r55";
	dwarf2gdb[55].gdb_regno = 247;
	dwarf2gdb[56].name = "r56";
	dwarf2gdb[56].gdb_regno = 248;
	dwarf2gdb[57].name = "r57";
	dwarf2gdb[57].gdb_regno = 249;
	dwarf2gdb[58].name = "r58";
	dwarf2gdb[58].gdb_regno = 250;
	dwarf2gdb[59].name = "r59";
	dwarf2gdb[59].gdb_regno = 251;
	dwarf2gdb[60].name = "r60";
	dwarf2gdb[60].gdb_regno = 252;
	dwarf2gdb[61].name = "r61";
	dwarf2gdb[61].gdb_regno = 253;
	dwarf2gdb[62].name = "r62";
	dwarf2gdb[62].gdb_regno = 254;
	dwarf2gdb[63].name = "r63";
	dwarf2gdb[63].gdb_regno = 255;
	dwarf2gdb[256].name = "a0_x";
	dwarf2gdb[256].gdb_regno = 256;
	dwarf2gdb[257].name = "a0_y";
	dwarf2gdb[257].gdb_regno = 257;
	dwarf2gdb[258].name = "a0_z";
	dwarf2gdb[258].gdb_regno = 258;
	dwarf2gdb[259].name = "a0_t";
	dwarf2gdb[259].gdb_regno = 259;
	dwarf2gdb[260].name = "a1_x";
	dwarf2gdb[260].gdb_regno = 260;
	dwarf2gdb[261].name = "a1_y";
	dwarf2gdb[261].gdb_regno = 261;
	dwarf2gdb[262].name = "a1_z";
	dwarf2gdb[262].gdb_regno = 262;
	dwarf2gdb[263].name = "a1_t";
	dwarf2gdb[263].gdb_regno = 263;
	dwarf2gdb[264].name = "a2_x";
	dwarf2gdb[264].gdb_regno = 264;
	dwarf2gdb[265].name = "a2_y";
	dwarf2gdb[265].gdb_regno = 265;
	dwarf2gdb[266].name = "a2_z";
	dwarf2gdb[266].gdb_regno = 266;
	dwarf2gdb[267].name = "a2_t";
	dwarf2gdb[267].gdb_regno = 267;
	dwarf2gdb[268].name = "a3_x";
	dwarf2gdb[268].gdb_regno = 268;
	dwarf2gdb[269].name = "a3_y";
	dwarf2gdb[269].gdb_regno = 269;
	dwarf2gdb[270].name = "a3_z";
	dwarf2gdb[270].gdb_regno = 270;
	dwarf2gdb[271].name = "a3_t";
	dwarf2gdb[271].gdb_regno = 271;
	dwarf2gdb[272].name = "a4_x";
	dwarf2gdb[272].gdb_regno = 272;
	dwarf2gdb[273].name = "a4_y";
	dwarf2gdb[273].gdb_regno = 273;
	dwarf2gdb[274].name = "a4_z";
	dwarf2gdb[274].gdb_regno = 274;
	dwarf2gdb[275].name = "a4_t";
	dwarf2gdb[275].gdb_regno = 275;
	dwarf2gdb[276].name = "a5_x";
	dwarf2gdb[276].gdb_regno = 276;
	dwarf2gdb[277].name = "a5_y";
	dwarf2gdb[277].gdb_regno = 277;
	dwarf2gdb[278].name = "a5_z";
	dwarf2gdb[278].gdb_regno = 278;
	dwarf2gdb[279].name = "a5_t";
	dwarf2gdb[279].gdb_regno = 279;
	dwarf2gdb[280].name = "a6_x";
	dwarf2gdb[280].gdb_regno = 280;
	dwarf2gdb[281].name = "a6_y";
	dwarf2gdb[281].gdb_regno = 281;
	dwarf2gdb[282].name = "a6_z";
	dwarf2gdb[282].gdb_regno = 282;
	dwarf2gdb[283].name = "a6_t";
	dwarf2gdb[283].gdb_regno = 283;
	dwarf2gdb[284].name = "a7_x";
	dwarf2gdb[284].gdb_regno = 284;
	dwarf2gdb[285].name = "a7_y";
	dwarf2gdb[285].gdb_regno = 285;
	dwarf2gdb[286].name = "a7_z";
	dwarf2gdb[286].gdb_regno = 286;
	dwarf2gdb[287].name = "a7_t";
	dwarf2gdb[287].gdb_regno = 287;
	dwarf2gdb[288].name = "a8_x";
	dwarf2gdb[288].gdb_regno = 288;
	dwarf2gdb[289].name = "a8_y";
	dwarf2gdb[289].gdb_regno = 289;
	dwarf2gdb[290].name = "a8_z";
	dwarf2gdb[290].gdb_regno = 290;
	dwarf2gdb[291].name = "a8_t";
	dwarf2gdb[291].gdb_regno = 291;
	dwarf2gdb[292].name = "a9_x";
	dwarf2gdb[292].gdb_regno = 292;
	dwarf2gdb[293].name = "a9_y";
	dwarf2gdb[293].gdb_regno = 293;
	dwarf2gdb[294].name = "a9_z";
	dwarf2gdb[294].gdb_regno = 294;
	dwarf2gdb[295].name = "a9_t";
	dwarf2gdb[295].gdb_regno = 295;
	dwarf2gdb[296].name = "a10_x";
	dwarf2gdb[296].gdb_regno = 296;
	dwarf2gdb[297].name = "a10_y";
	dwarf2gdb[297].gdb_regno = 297;
	dwarf2gdb[298].name = "a10_z";
	dwarf2gdb[298].gdb_regno = 298;
	dwarf2gdb[299].name = "a10_t";
	dwarf2gdb[299].gdb_regno = 299;
	dwarf2gdb[300].name = "a11_x";
	dwarf2gdb[300].gdb_regno = 300;
	dwarf2gdb[301].name = "a11_y";
	dwarf2gdb[301].gdb_regno = 301;
	dwarf2gdb[302].name = "a11_z";
	dwarf2gdb[302].gdb_regno = 302;
	dwarf2gdb[303].name = "a11_t";
	dwarf2gdb[303].gdb_regno = 303;
	dwarf2gdb[304].name = "a12_x";
	dwarf2gdb[304].gdb_regno = 304;
	dwarf2gdb[305].name = "a12_y";
	dwarf2gdb[305].gdb_regno = 305;
	dwarf2gdb[306].name = "a12_z";
	dwarf2gdb[306].gdb_regno = 306;
	dwarf2gdb[307].name = "a12_t";
	dwarf2gdb[307].gdb_regno = 307;
	dwarf2gdb[308].name = "a13_x";
	dwarf2gdb[308].gdb_regno = 308;
	dwarf2gdb[309].name = "a13_y";
	dwarf2gdb[309].gdb_regno = 309;
	dwarf2gdb[310].name = "a13_z";
	dwarf2gdb[310].gdb_regno = 310;
	dwarf2gdb[311].name = "a13_t";
	dwarf2gdb[311].gdb_regno = 311;
	dwarf2gdb[312].name = "a14_x";
	dwarf2gdb[312].gdb_regno = 312;
	dwarf2gdb[313].name = "a14_y";
	dwarf2gdb[313].gdb_regno = 313;
	dwarf2gdb[314].name = "a14_z";
	dwarf2gdb[314].gdb_regno = 314;
	dwarf2gdb[315].name = "a14_t";
	dwarf2gdb[315].gdb_regno = 315;
	dwarf2gdb[316].name = "a15_x";
	dwarf2gdb[316].gdb_regno = 316;
	dwarf2gdb[317].name = "a15_y";
	dwarf2gdb[317].gdb_regno = 317;
	dwarf2gdb[318].name = "a15_z";
	dwarf2gdb[318].gdb_regno = 318;
	dwarf2gdb[319].name = "a15_t";
	dwarf2gdb[319].gdb_regno = 319;
	dwarf2gdb[320].name = "a16_x";
	dwarf2gdb[320].gdb_regno = 320;
	dwarf2gdb[321].name = "a16_y";
	dwarf2gdb[321].gdb_regno = 321;
	dwarf2gdb[322].name = "a16_z";
	dwarf2gdb[322].gdb_regno = 322;
	dwarf2gdb[323].name = "a16_t";
	dwarf2gdb[323].gdb_regno = 323;
	dwarf2gdb[324].name = "a17_x";
	dwarf2gdb[324].gdb_regno = 324;
	dwarf2gdb[325].name = "a17_y";
	dwarf2gdb[325].gdb_regno = 325;
	dwarf2gdb[326].name = "a17_z";
	dwarf2gdb[326].gdb_regno = 326;
	dwarf2gdb[327].name = "a17_t";
	dwarf2gdb[327].gdb_regno = 327;
	dwarf2gdb[328].name = "a18_x";
	dwarf2gdb[328].gdb_regno = 328;
	dwarf2gdb[329].name = "a18_y";
	dwarf2gdb[329].gdb_regno = 329;
	dwarf2gdb[330].name = "a18_z";
	dwarf2gdb[330].gdb_regno = 330;
	dwarf2gdb[331].name = "a18_t";
	dwarf2gdb[331].gdb_regno = 331;
	dwarf2gdb[332].name = "a19_x";
	dwarf2gdb[332].gdb_regno = 332;
	dwarf2gdb[333].name = "a19_y";
	dwarf2gdb[333].gdb_regno = 333;
	dwarf2gdb[334].name = "a19_z";
	dwarf2gdb[334].gdb_regno = 334;
	dwarf2gdb[335].name = "a19_t";
	dwarf2gdb[335].gdb_regno = 335;
	dwarf2gdb[336].name = "a20_x";
	dwarf2gdb[336].gdb_regno = 336;
	dwarf2gdb[337].name = "a20_y";
	dwarf2gdb[337].gdb_regno = 337;
	dwarf2gdb[338].name = "a20_z";
	dwarf2gdb[338].gdb_regno = 338;
	dwarf2gdb[339].name = "a20_t";
	dwarf2gdb[339].gdb_regno = 339;
	dwarf2gdb[340].name = "a21_x";
	dwarf2gdb[340].gdb_regno = 340;
	dwarf2gdb[341].name = "a21_y";
	dwarf2gdb[341].gdb_regno = 341;
	dwarf2gdb[342].name = "a21_z";
	dwarf2gdb[342].gdb_regno = 342;
	dwarf2gdb[343].name = "a21_t";
	dwarf2gdb[343].gdb_regno = 343;
	dwarf2gdb[344].name = "a22_x";
	dwarf2gdb[344].gdb_regno = 344;
	dwarf2gdb[345].name = "a22_y";
	dwarf2gdb[345].gdb_regno = 345;
	dwarf2gdb[346].name = "a22_z";
	dwarf2gdb[346].gdb_regno = 346;
	dwarf2gdb[347].name = "a22_t";
	dwarf2gdb[347].gdb_regno = 347;
	dwarf2gdb[348].name = "a23_x";
	dwarf2gdb[348].gdb_regno = 348;
	dwarf2gdb[349].name = "a23_y";
	dwarf2gdb[349].gdb_regno = 349;
	dwarf2gdb[350].name = "a23_z";
	dwarf2gdb[350].gdb_regno = 350;
	dwarf2gdb[351].name = "a23_t";
	dwarf2gdb[351].gdb_regno = 351;
	dwarf2gdb[352].name = "a24_x";
	dwarf2gdb[352].gdb_regno = 352;
	dwarf2gdb[353].name = "a24_y";
	dwarf2gdb[353].gdb_regno = 353;
	dwarf2gdb[354].name = "a24_z";
	dwarf2gdb[354].gdb_regno = 354;
	dwarf2gdb[355].name = "a24_t";
	dwarf2gdb[355].gdb_regno = 355;
	dwarf2gdb[356].name = "a25_x";
	dwarf2gdb[356].gdb_regno = 356;
	dwarf2gdb[357].name = "a25_y";
	dwarf2gdb[357].gdb_regno = 357;
	dwarf2gdb[358].name = "a25_z";
	dwarf2gdb[358].gdb_regno = 358;
	dwarf2gdb[359].name = "a25_t";
	dwarf2gdb[359].gdb_regno = 359;
	dwarf2gdb[360].name = "a26_x";
	dwarf2gdb[360].gdb_regno = 360;
	dwarf2gdb[361].name = "a26_y";
	dwarf2gdb[361].gdb_regno = 361;
	dwarf2gdb[362].name = "a26_z";
	dwarf2gdb[362].gdb_regno = 362;
	dwarf2gdb[363].name = "a26_t";
	dwarf2gdb[363].gdb_regno = 363;
	dwarf2gdb[364].name = "a27_x";
	dwarf2gdb[364].gdb_regno = 364;
	dwarf2gdb[365].name = "a27_y";
	dwarf2gdb[365].gdb_regno = 365;
	dwarf2gdb[366].name = "a27_z";
	dwarf2gdb[366].gdb_regno = 366;
	dwarf2gdb[367].name = "a27_t";
	dwarf2gdb[367].gdb_regno = 367;
	dwarf2gdb[368].name = "a28_x";
	dwarf2gdb[368].gdb_regno = 368;
	dwarf2gdb[369].name = "a28_y";
	dwarf2gdb[369].gdb_regno = 369;
	dwarf2gdb[370].name = "a28_z";
	dwarf2gdb[370].gdb_regno = 370;
	dwarf2gdb[371].name = "a28_t";
	dwarf2gdb[371].gdb_regno = 371;
	dwarf2gdb[372].name = "a29_x";
	dwarf2gdb[372].gdb_regno = 372;
	dwarf2gdb[373].name = "a29_y";
	dwarf2gdb[373].gdb_regno = 373;
	dwarf2gdb[374].name = "a29_z";
	dwarf2gdb[374].gdb_regno = 374;
	dwarf2gdb[375].name = "a29_t";
	dwarf2gdb[375].gdb_regno = 375;
	dwarf2gdb[376].name = "a30_x";
	dwarf2gdb[376].gdb_regno = 376;
	dwarf2gdb[377].name = "a30_y";
	dwarf2gdb[377].gdb_regno = 377;
	dwarf2gdb[378].name = "a30_z";
	dwarf2gdb[378].gdb_regno = 378;
	dwarf2gdb[379].name = "a30_t";
	dwarf2gdb[379].gdb_regno = 379;
	dwarf2gdb[380].name = "a31_x";
	dwarf2gdb[380].gdb_regno = 380;
	dwarf2gdb[381].name = "a31_y";
	dwarf2gdb[381].gdb_regno = 381;
	dwarf2gdb[382].name = "a31_z";
	dwarf2gdb[382].gdb_regno = 382;
	dwarf2gdb[383].name = "a31_t";
	dwarf2gdb[383].gdb_regno = 383;
	dwarf2gdb[384].name = "a32_x";
	dwarf2gdb[384].gdb_regno = 384;
	dwarf2gdb[385].name = "a32_y";
	dwarf2gdb[385].gdb_regno = 385;
	dwarf2gdb[386].name = "a32_z";
	dwarf2gdb[386].gdb_regno = 386;
	dwarf2gdb[387].name = "a32_t";
	dwarf2gdb[387].gdb_regno = 387;
	dwarf2gdb[388].name = "a33_x";
	dwarf2gdb[388].gdb_regno = 388;
	dwarf2gdb[389].name = "a33_y";
	dwarf2gdb[389].gdb_regno = 389;
	dwarf2gdb[390].name = "a33_z";
	dwarf2gdb[390].gdb_regno = 390;
	dwarf2gdb[391].name = "a33_t";
	dwarf2gdb[391].gdb_regno = 391;
	dwarf2gdb[392].name = "a34_x";
	dwarf2gdb[392].gdb_regno = 392;
	dwarf2gdb[393].name = "a34_y";
	dwarf2gdb[393].gdb_regno = 393;
	dwarf2gdb[394].name = "a34_z";
	dwarf2gdb[394].gdb_regno = 394;
	dwarf2gdb[395].name = "a34_t";
	dwarf2gdb[395].gdb_regno = 395;
	dwarf2gdb[396].name = "a35_x";
	dwarf2gdb[396].gdb_regno = 396;
	dwarf2gdb[397].name = "a35_y";
	dwarf2gdb[397].gdb_regno = 397;
	dwarf2gdb[398].name = "a35_z";
	dwarf2gdb[398].gdb_regno = 398;
	dwarf2gdb[399].name = "a35_t";
	dwarf2gdb[399].gdb_regno = 399;
	dwarf2gdb[400].name = "a36_x";
	dwarf2gdb[400].gdb_regno = 400;
	dwarf2gdb[401].name = "a36_y";
	dwarf2gdb[401].gdb_regno = 401;
	dwarf2gdb[402].name = "a36_z";
	dwarf2gdb[402].gdb_regno = 402;
	dwarf2gdb[403].name = "a36_t";
	dwarf2gdb[403].gdb_regno = 403;
	dwarf2gdb[404].name = "a37_x";
	dwarf2gdb[404].gdb_regno = 404;
	dwarf2gdb[405].name = "a37_y";
	dwarf2gdb[405].gdb_regno = 405;
	dwarf2gdb[406].name = "a37_z";
	dwarf2gdb[406].gdb_regno = 406;
	dwarf2gdb[407].name = "a37_t";
	dwarf2gdb[407].gdb_regno = 407;
	dwarf2gdb[408].name = "a38_x";
	dwarf2gdb[408].gdb_regno = 408;
	dwarf2gdb[409].name = "a38_y";
	dwarf2gdb[409].gdb_regno = 409;
	dwarf2gdb[410].name = "a38_z";
	dwarf2gdb[410].gdb_regno = 410;
	dwarf2gdb[411].name = "a38_t";
	dwarf2gdb[411].gdb_regno = 411;
	dwarf2gdb[412].name = "a39_x";
	dwarf2gdb[412].gdb_regno = 412;
	dwarf2gdb[413].name = "a39_y";
	dwarf2gdb[413].gdb_regno = 413;
	dwarf2gdb[414].name = "a39_z";
	dwarf2gdb[414].gdb_regno = 414;
	dwarf2gdb[415].name = "a39_t";
	dwarf2gdb[415].gdb_regno = 415;
	dwarf2gdb[416].name = "a40_x";
	dwarf2gdb[416].gdb_regno = 416;
	dwarf2gdb[417].name = "a40_y";
	dwarf2gdb[417].gdb_regno = 417;
	dwarf2gdb[418].name = "a40_z";
	dwarf2gdb[418].gdb_regno = 418;
	dwarf2gdb[419].name = "a40_t";
	dwarf2gdb[419].gdb_regno = 419;
	dwarf2gdb[420].name = "a41_x";
	dwarf2gdb[420].gdb_regno = 420;
	dwarf2gdb[421].name = "a41_y";
	dwarf2gdb[421].gdb_regno = 421;
	dwarf2gdb[422].name = "a41_z";
	dwarf2gdb[422].gdb_regno = 422;
	dwarf2gdb[423].name = "a41_t";
	dwarf2gdb[423].gdb_regno = 423;
	dwarf2gdb[424].name = "a42_x";
	dwarf2gdb[424].gdb_regno = 424;
	dwarf2gdb[425].name = "a42_y";
	dwarf2gdb[425].gdb_regno = 425;
	dwarf2gdb[426].name = "a42_z";
	dwarf2gdb[426].gdb_regno = 426;
	dwarf2gdb[427].name = "a42_t";
	dwarf2gdb[427].gdb_regno = 427;
	dwarf2gdb[428].name = "a43_x";
	dwarf2gdb[428].gdb_regno = 428;
	dwarf2gdb[429].name = "a43_y";
	dwarf2gdb[429].gdb_regno = 429;
	dwarf2gdb[430].name = "a43_z";
	dwarf2gdb[430].gdb_regno = 430;
	dwarf2gdb[431].name = "a43_t";
	dwarf2gdb[431].gdb_regno = 431;
	dwarf2gdb[432].name = "a44_x";
	dwarf2gdb[432].gdb_regno = 432;
	dwarf2gdb[433].name = "a44_y";
	dwarf2gdb[433].gdb_regno = 433;
	dwarf2gdb[434].name = "a44_z";
	dwarf2gdb[434].gdb_regno = 434;
	dwarf2gdb[435].name = "a44_t";
	dwarf2gdb[435].gdb_regno = 435;
	dwarf2gdb[436].name = "a45_x";
	dwarf2gdb[436].gdb_regno = 436;
	dwarf2gdb[437].name = "a45_y";
	dwarf2gdb[437].gdb_regno = 437;
	dwarf2gdb[438].name = "a45_z";
	dwarf2gdb[438].gdb_regno = 438;
	dwarf2gdb[439].name = "a45_t";
	dwarf2gdb[439].gdb_regno = 439;
	dwarf2gdb[440].name = "a46_x";
	dwarf2gdb[440].gdb_regno = 440;
	dwarf2gdb[441].name = "a46_y";
	dwarf2gdb[441].gdb_regno = 441;
	dwarf2gdb[442].name = "a46_z";
	dwarf2gdb[442].gdb_regno = 442;
	dwarf2gdb[443].name = "a46_t";
	dwarf2gdb[443].gdb_regno = 443;
	dwarf2gdb[444].name = "a47_x";
	dwarf2gdb[444].gdb_regno = 444;
	dwarf2gdb[445].name = "a47_y";
	dwarf2gdb[445].gdb_regno = 445;
	dwarf2gdb[446].name = "a47_z";
	dwarf2gdb[446].gdb_regno = 446;
	dwarf2gdb[447].name = "a47_t";
	dwarf2gdb[447].gdb_regno = 447;
	dwarf2gdb[448].name = "a48_x";
	dwarf2gdb[448].gdb_regno = 448;
	dwarf2gdb[449].name = "a48_y";
	dwarf2gdb[449].gdb_regno = 449;
	dwarf2gdb[450].name = "a48_z";
	dwarf2gdb[450].gdb_regno = 450;
	dwarf2gdb[451].name = "a48_t";
	dwarf2gdb[451].gdb_regno = 451;
	dwarf2gdb[452].name = "a49_x";
	dwarf2gdb[452].gdb_regno = 452;
	dwarf2gdb[453].name = "a49_y";
	dwarf2gdb[453].gdb_regno = 453;
	dwarf2gdb[454].name = "a49_z";
	dwarf2gdb[454].gdb_regno = 454;
	dwarf2gdb[455].name = "a49_t";
	dwarf2gdb[455].gdb_regno = 455;
	dwarf2gdb[456].name = "a50_x";
	dwarf2gdb[456].gdb_regno = 456;
	dwarf2gdb[457].name = "a50_y";
	dwarf2gdb[457].gdb_regno = 457;
	dwarf2gdb[458].name = "a50_z";
	dwarf2gdb[458].gdb_regno = 458;
	dwarf2gdb[459].name = "a50_t";
	dwarf2gdb[459].gdb_regno = 459;
	dwarf2gdb[460].name = "a51_x";
	dwarf2gdb[460].gdb_regno = 460;
	dwarf2gdb[461].name = "a51_y";
	dwarf2gdb[461].gdb_regno = 461;
	dwarf2gdb[462].name = "a51_z";
	dwarf2gdb[462].gdb_regno = 462;
	dwarf2gdb[463].name = "a51_t";
	dwarf2gdb[463].gdb_regno = 463;
	dwarf2gdb[464].name = "a52_x";
	dwarf2gdb[464].gdb_regno = 464;
	dwarf2gdb[465].name = "a52_y";
	dwarf2gdb[465].gdb_regno = 465;
	dwarf2gdb[466].name = "a52_z";
	dwarf2gdb[466].gdb_regno = 466;
	dwarf2gdb[467].name = "a52_t";
	dwarf2gdb[467].gdb_regno = 467;
	dwarf2gdb[468].name = "a53_x";
	dwarf2gdb[468].gdb_regno = 468;
	dwarf2gdb[469].name = "a53_y";
	dwarf2gdb[469].gdb_regno = 469;
	dwarf2gdb[470].name = "a53_z";
	dwarf2gdb[470].gdb_regno = 470;
	dwarf2gdb[471].name = "a53_t";
	dwarf2gdb[471].gdb_regno = 471;
	dwarf2gdb[472].name = "a54_x";
	dwarf2gdb[472].gdb_regno = 472;
	dwarf2gdb[473].name = "a54_y";
	dwarf2gdb[473].gdb_regno = 473;
	dwarf2gdb[474].name = "a54_z";
	dwarf2gdb[474].gdb_regno = 474;
	dwarf2gdb[475].name = "a54_t";
	dwarf2gdb[475].gdb_regno = 475;
	dwarf2gdb[476].name = "a55_x";
	dwarf2gdb[476].gdb_regno = 476;
	dwarf2gdb[477].name = "a55_y";
	dwarf2gdb[477].gdb_regno = 477;
	dwarf2gdb[478].name = "a55_z";
	dwarf2gdb[478].gdb_regno = 478;
	dwarf2gdb[479].name = "a55_t";
	dwarf2gdb[479].gdb_regno = 479;
	dwarf2gdb[480].name = "a56_x";
	dwarf2gdb[480].gdb_regno = 480;
	dwarf2gdb[481].name = "a56_y";
	dwarf2gdb[481].gdb_regno = 481;
	dwarf2gdb[482].name = "a56_z";
	dwarf2gdb[482].gdb_regno = 482;
	dwarf2gdb[483].name = "a56_t";
	dwarf2gdb[483].gdb_regno = 483;
	dwarf2gdb[484].name = "a57_x";
	dwarf2gdb[484].gdb_regno = 484;
	dwarf2gdb[485].name = "a57_y";
	dwarf2gdb[485].gdb_regno = 485;
	dwarf2gdb[486].name = "a57_z";
	dwarf2gdb[486].gdb_regno = 486;
	dwarf2gdb[487].name = "a57_t";
	dwarf2gdb[487].gdb_regno = 487;
	dwarf2gdb[488].name = "a58_x";
	dwarf2gdb[488].gdb_regno = 488;
	dwarf2gdb[489].name = "a58_y";
	dwarf2gdb[489].gdb_regno = 489;
	dwarf2gdb[490].name = "a58_z";
	dwarf2gdb[490].gdb_regno = 490;
	dwarf2gdb[491].name = "a58_t";
	dwarf2gdb[491].gdb_regno = 491;
	dwarf2gdb[492].name = "a59_x";
	dwarf2gdb[492].gdb_regno = 492;
	dwarf2gdb[493].name = "a59_y";
	dwarf2gdb[493].gdb_regno = 493;
	dwarf2gdb[494].name = "a59_z";
	dwarf2gdb[494].gdb_regno = 494;
	dwarf2gdb[495].name = "a59_t";
	dwarf2gdb[495].gdb_regno = 495;
	dwarf2gdb[496].name = "a60_x";
	dwarf2gdb[496].gdb_regno = 496;
	dwarf2gdb[497].name = "a60_y";
	dwarf2gdb[497].gdb_regno = 497;
	dwarf2gdb[498].name = "a60_z";
	dwarf2gdb[498].gdb_regno = 498;
	dwarf2gdb[499].name = "a60_t";
	dwarf2gdb[499].gdb_regno = 499;
	dwarf2gdb[500].name = "a61_x";
	dwarf2gdb[500].gdb_regno = 500;
	dwarf2gdb[501].name = "a61_y";
	dwarf2gdb[501].gdb_regno = 501;
	dwarf2gdb[502].name = "a61_z";
	dwarf2gdb[502].gdb_regno = 502;
	dwarf2gdb[503].name = "a61_t";
	dwarf2gdb[503].gdb_regno = 503;
	dwarf2gdb[504].name = "a62_x";
	dwarf2gdb[504].gdb_regno = 504;
	dwarf2gdb[505].name = "a62_y";
	dwarf2gdb[505].gdb_regno = 505;
	dwarf2gdb[506].name = "a62_z";
	dwarf2gdb[506].gdb_regno = 506;
	dwarf2gdb[507].name = "a62_t";
	dwarf2gdb[507].gdb_regno = 507;
	dwarf2gdb[508].name = "a63_x";
	dwarf2gdb[508].gdb_regno = 508;
	dwarf2gdb[509].name = "a63_y";
	dwarf2gdb[509].gdb_regno = 509;
	dwarf2gdb[510].name = "a63_z";
	dwarf2gdb[510].gdb_regno = 510;
	dwarf2gdb[511].name = "a63_t";
	dwarf2gdb[511].gdb_regno = 511;

	for (i = 0; i < sizeof(dwarf2gdb)/sizeof(struct dwarf2gdb_desc); ++i) {
		if (dwarf2gdb[i].name == (void *) -1)
			continue;
		dwarf2gdb[i].gdb_regno = user_reg_map_name_to_regnum (gdbarch, dwarf2gdb[i].name, -1);
	}
	return 1;
}
static const char*
find_tdesc_arch (struct gdbarch *gdbarch)
{
	const struct target_desc *tdesc;

	if (gdbarch == NULL) return "kv3-1";
tdesc = gdbarch_target_desc (gdbarch);

	if (tdesc == NULL) return "kv3-1";
	if (tdesc_find_feature (tdesc, "eu.kalray.core.kv3-1"))
		return "kv3-1";
	return "kv3";
}

const char* kv3_pc_name (struct gdbarch *gdbarch);
const char*
kv3_pc_name (struct gdbarch *gdbarch)
{
	const char *archname = find_tdesc_arch (gdbarch);
	if (strcmp ("kv3-1", archname) == 0) {
		return _v1_pc_name;
	}
	error ("No PC for architecture %s", archname);
}
const char* kv3_sp_name (struct gdbarch *gdbarch);
const char*
kv3_sp_name (struct gdbarch *gdbarch)
{
	const char *archname = find_tdesc_arch (gdbarch);
	if (strcmp ("kv3-1", archname) == 0) {
		return _v1_sp_name;
	}
	error ("No SP for architecture %s", archname);
}
void init_dwarf2gdb (struct gdbarch *gdbarch);
void
init_dwarf2gdb (struct gdbarch *gdbarch)
{
	const char *archname = find_tdesc_arch (gdbarch);
	if (strcmp ("kv3-1", archname) == 0) {
		init_v1_dwarf2gdb (gdbarch);
		return;
	}
	error ("Unable to find the dwarf2gdb table for processor %s", archname);
}

int kv3_num_pseudos (struct gdbarch *gdbarch);
int
kv3_num_pseudos (struct gdbarch *gdbarch)
{
	const char *archname = find_tdesc_arch (gdbarch);
	if (strcmp ("kv3-1", archname) == 0)
		return v1_num_pseudo_regs;
	error ("Unable to find the num_pseudo_regs table for processor %s", archname);
}

static struct pseudo_desc *
pseudo_registers (struct gdbarch *gdbarch, int *pseudo_num)
{
	const char *archname = find_tdesc_arch (gdbarch);
	if (strcmp ("kv3-1", archname) == 0) {
		*pseudo_num = v1_num_pseudo_regs;
		return v1_pseudo_regs;
	}
	error ("Unable to find the pseudo_regs table for processor %s", archname);
}



int kv3_dwarf2_reg_to_regnum (struct gdbarch *gdbarch, int reg);
int kv3_dwarf2_reg_to_regnum (struct gdbarch *gdbarch, int reg)
{
	/* Test bith .name and .gdb_regno. The ordering of events might have
	   us passing here while the registers aren't yet available through
	   the user-reg interface.  To be totally honest, what I did is too
	   complicated.  We should have decided on a GDB register order and
	   stuck statically with it in the debugger/simulator/jtag-runner... 
	   Would have been more painful when MDS is in flux, but much
	   simpler for everyday maintainance. */
	if (dwarf2gdb[0].name == 0 || dwarf2gdb[0].gdb_regno == -1) init_dwarf2gdb (gdbarch);

	if (reg < ARRAY_SIZE(dwarf2gdb)) return dwarf2gdb[reg].gdb_regno;

	return -1;
}

const char *kv3_pseudo_register_name (struct gdbarch *gdbarch, int regnr);
const char *kv3_pseudo_register_name (struct gdbarch *gdbarch, int regnr)
{
	int pseudo_num = regnr - gdbarch_num_regs (gdbarch);
	int num_pseudo;
	struct pseudo_desc *pseudo_regs = pseudo_registers (gdbarch, &num_pseudo);
	if (pseudo_num<0 || pseudo_num>=num_pseudo)
		return NULL;

	return pseudo_regs[pseudo_num].name;
}

struct type *kv3_pseudo_register_type (struct gdbarch *gdbarch, 
                                      int regnr);
struct type *kv3_pseudo_register_type (struct gdbarch *gdbarch, 
                                      int regnr)
{
	int pseudo_num = regnr - gdbarch_num_regs (gdbarch);
	int num_pseudo;
	struct pseudo_desc *pseudo_regs = pseudo_registers (gdbarch, &num_pseudo);

	if (pseudo_num<0 || pseudo_num>=num_pseudo)
		return NULL;

	if (pseudo_regs[pseudo_num].type != NULL) 
		return pseudo_regs[pseudo_num].type;

	if (pseudo_regs[pseudo_num].size == gdbarch_long_bit (gdbarch))
		pseudo_regs[pseudo_num].type = builtin_type (gdbarch)->builtin_long;
	else if (pseudo_regs[pseudo_num].size == TARGET_CHAR_BIT)
		pseudo_regs[pseudo_num].type = builtin_type (gdbarch)->builtin_char;
	else if (pseudo_regs[pseudo_num].size == gdbarch_short_bit (gdbarch))
		pseudo_regs[pseudo_num].type = builtin_type (gdbarch)->builtin_short;
	else if (pseudo_regs[pseudo_num].size == gdbarch_int_bit (gdbarch))
		pseudo_regs[pseudo_num].type = builtin_type (gdbarch)->builtin_int;
	else if (pseudo_regs[pseudo_num].size == gdbarch_long_long_bit (gdbarch))
		pseudo_regs[pseudo_num].type = builtin_type (gdbarch)->builtin_long_long;
	else if (pseudo_regs[pseudo_num].size == gdbarch_ptr_bit (gdbarch))
	/* A bit desperate by this point... */
		pseudo_regs[pseudo_num].type = builtin_type (gdbarch)->builtin_data_ptr;
	else if (pseudo_regs[pseudo_num].size == 128)
		pseudo_regs[pseudo_num].type = builtin_type (gdbarch)->builtin_int128;
	else if (pseudo_regs[pseudo_num].size == 256)
		pseudo_regs[pseudo_num].type = gdbarch_tdep (gdbarch)->uint256;
	else if (pseudo_regs[pseudo_num].size == 512)
		pseudo_regs[pseudo_num].type = gdbarch_tdep (gdbarch)->uint512;
	else if (pseudo_regs[pseudo_num].size == 1024)
		pseudo_regs[pseudo_num].type = gdbarch_tdep (gdbarch)->uint1024;
	else {
		warning (_("Register \"%s\" has an unsupported size (%d bits)"),
		pseudo_regs[pseudo_num].name, pseudo_regs[pseudo_num].size);
		pseudo_regs[pseudo_num].type = builtin_type (gdbarch)->builtin_long;
	}
	return pseudo_regs[pseudo_num].type;
}

int kv3_pseudo_register_reggroup_p (struct gdbarch *gdbarch, 
				   int regnum, 
				   struct reggroup *reggroup);
int kv3_pseudo_register_reggroup_p (struct gdbarch *gdbarch, 
				   int regnum, 
				   struct reggroup *reggroup)
{
	return reggroup == general_reggroup;
}

static void kv3_init_pseudo_register (struct gdbarch *gdbarch, 
			            struct pseudo_desc *reg)
{
	int i;

	for (i = 0; i < reg->nb_components; ++i) {
		reg->components[i] = user_reg_map_name_to_regnum (gdbarch, reg->components_names[i], -1);
		if (reg->components[i] < 0) error ("Can't find register '%s' for pseudo reg '%s'\
", reg->components_names[i], reg->name);
	}
}

enum register_status kv3_pseudo_register_read (struct gdbarch *gdbarch, 
                              struct regcache *regcache,
                              int regnum, gdb_byte *buf);
enum register_status kv3_pseudo_register_read (struct gdbarch *gdbarch, 
                              struct regcache *regcache,
                              int regnum, gdb_byte *buf)
{
	int pseudo_num = regnum - gdbarch_num_regs (gdbarch);
	int num_pseudo;
	struct pseudo_desc *pseudo_regs = pseudo_registers (gdbarch, &num_pseudo);
	struct pseudo_desc *reg = &pseudo_regs[pseudo_num];
	int i;

	if (pseudo_num<0 || pseudo_num>=num_pseudo)
		error ("Register %i is not a pseudo register!", regnum);

	if (reg->components[0] < 0) kv3_init_pseudo_register (gdbarch, reg);

	for (i = 0; i < reg->nb_components; ++i) {
		enum register_status status;
		status = regcache_raw_read (regcache, reg->components[i], buf);
		if (status != REG_VALID)
			return status;
		buf += register_size (gdbarch, reg->components[i]);
	}
	return REG_VALID;
}

void kv3_pseudo_register_write (struct gdbarch *gdbarch, 
                               struct regcache *regcache,
                               int regnum, const gdb_byte *buf)
{
	int pseudo_num = regnum - gdbarch_num_regs (gdbarch);
	int num_pseudo;
	struct pseudo_desc *pseudo_regs = pseudo_registers (gdbarch, &num_pseudo);
	struct pseudo_desc *reg = &pseudo_regs[pseudo_num];
	int i;

	if (pseudo_num<0 || pseudo_num>=num_pseudo)
		error ("Register %i is not a pseudo register!", regnum);

	if (reg->components[0] < 0) kv3_init_pseudo_register (gdbarch, reg);

	for (i = 0; i < reg->nb_components; ++i) {
		regcache_raw_write (regcache, reg->components[i], buf);
		buf += register_size (gdbarch, reg->components[i]);
	}
}

void _initialize_kv3_mds_tdep (void);
void
_initialize_kv3_mds_tdep (void)
{
}
