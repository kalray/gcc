/* Builtin function support for the KVX architecture.
   Copyright (C) 2011-2022 Free Software Foundation, Inc.
   Contributed by Kalray SA (bddinechin@kalray.eu).

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#define IN_TARGET_CODE 1

#include "config.h"
#define INCLUDE_STRING
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "function.h"
#include "basic-block.h"
#include "rtl.h"
#include "tree.h"
#include "gimple.h"
#include "memmodel.h"
#include "tm_p.h"
#include "expmed.h"
#include "optabs.h"
#include "recog.h"
#include "diagnostic-core.h"
#include "fold-const.h"
#include "stor-layout.h"
#include "explow.h"
#include "expr.h"
#include "langhooks.h"
#include "emit-rtl.h"

/* Some useful meta-programming macros.  */

#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()
#define EXPAND(...) __VA_ARGS__

#define KVX_FST(A, ...) A
#define KVX_RST(A, ...) __VA_ARGS__

#define CONCAT2_(a,b)          a ## b
#define CONCAT3_(a,b,c)        a ## b ## c
#define CONCAT4_(a,b,c,d)      a ## b ## c ## d
#define QUOTE(a)               #a
#define PROTECT(a)             QUOTE (a)
#define CONCAT2(a,b)           CONCAT2_ (a,b)
#define CONCAT3(a,b,c)         CONCAT3_ (a,b,c)
#define CONCAT4(a,b,c,d)       CONCAT4_ (a,b,c,d)

#define GET_MACRO(_1, _2, _3, _4, _5, _6, x,...) x

#define _ARG16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define HAS_COMMA(...) _ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
#define _TRIGGER_PARENTHESIS_(...) ,

/* This implementation of ISEMPTY comes from:
   https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments */
#define ISEMPTY(...)                                                    \
_ISEMPTY(                                                               \
          /* test if there is just one argument, eventually an empty    \
             one */                                                     \
          HAS_COMMA(__VA_ARGS__),                                       \
          /* test if _TRIGGER_PARENTHESIS_ together with the argument   \
             adds a comma */                                            \
          HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__),                 \
          /* test if the argument together with a parenthesis           \
             adds a comma */                                            \
          HAS_COMMA(__VA_ARGS__ (/*empty*/)),                           \
          /* test if placing it between _TRIGGER_PARENTHESIS_ and the   \
             parenthesis adds a comma */                                \
          HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/))      \
          )

#define PASTE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define _ISEMPTY(_0, _1, _2, _3) HAS_COMMA(PASTE5(_IS_EMPTY_CASE_, _0, _1, _2, _3))
#define _IS_EMPTY_CASE_0001 ,

/* Macros to handle the optional modifier of some builtins.  */

#define KVX_OPT_
#define KVX_OPT__
#define KVX_OPT_ACCESSES  _ACCESSES
#define KVX_OPT_BOOL      _BOOL
#define KVX_OPT_COHERENCY _COHERENCY
#define KVX_OPT_BOOLCASCO _BOOLCASCO
#define KVX_OPT_BOOLCASCO_BOOL _BOOLCASCO_BOOL
#define KVX_GET(OPTIONALS) KVX_OPT_ ## OPTIONALS

struct kvx_modmap
{
  /* The list of valid modifiers in the C source.  */
  const char **imap;

  /* A list mapping IMAP modifiers to OMAP modifiers, i.e., those that will be
   * used by the Machine Description Files or the assembler. */
  const char **omap;
};

/* Accessors for the modifier maps, this is needed by the
 * KVX_BUILTIN_{IN,}DIRECT macros.  */

#define KVX_MOD_VOID       NULL
#define KVX_MOD_VPTR       NULL
#define KVX_MOD_CVPTR      NULL
#define KVX_MOD_BOOL       NULL

#define KVX_MOD_INT8       NULL
#define KVX_MOD_INT16      NULL
#define KVX_MOD_INT32      NULL
#define KVX_MOD_INT64      NULL
#define KVX_MOD_INT128     NULL

#define KVX_MOD_UINT8      NULL
#define KVX_MOD_UINT16     NULL
#define KVX_MOD_UINT32     NULL
#define KVX_MOD_UINT64     NULL
#define KVX_MOD_UINT128    NULL

#define KVX_MOD_FLOAT16    NULL
#define KVX_MOD_FLOAT32    NULL
#define KVX_MOD_FLOAT64    NULL
#define KVX_MOD_COMPLEX64  NULL
#define KVX_MOD_COMPLEX128 NULL

#define KVX_MOD_STRING     NULL

#define KVX_MOD_V8QI       NULL
#define KVX_MOD_V16QI      NULL
#define KVX_MOD_V32QI      NULL
#define KVX_MOD_V64QI      NULL

#define KVX_MOD_V4HI       NULL
#define KVX_MOD_V8HI       NULL
#define KVX_MOD_V16HI      NULL
#define KVX_MOD_V32HI      NULL

#define KVX_MOD_V2SI       NULL
#define KVX_MOD_V4SI       NULL
#define KVX_MOD_V8SI       NULL
#define KVX_MOD_V16SI      NULL

#define KVX_MOD_V2DI       NULL
#define KVX_MOD_V4DI       NULL
#define KVX_MOD_V8DI       NULL

#define KVX_MOD_V4HF       NULL
#define KVX_MOD_V8HF       NULL
#define KVX_MOD_V16HF      NULL
#define KVX_MOD_V32HF      NULL

#define KVX_MOD_V2SF       NULL
#define KVX_MOD_V4SF       NULL
#define KVX_MOD_V8SF       NULL
#define KVX_MOD_V16SF      NULL

#define KVX_MOD_V2DF       NULL
#define KVX_MOD_V4DF       NULL
#define KVX_MOD_V8DF       NULL

#define KVX_MOD_DI         NULL
#define KVX_MOD_V64        NULL
#define KVX_MOD_V128       NULL
#define KVX_MOD_V256       NULL
#define KVX_MOD_V512       NULL

#define KVX_MOD_OI         NULL

#define KVX_MOD_X256       NULL
#define KVX_MOD_X512       NULL
#define KVX_MOD_X1024      NULL
#define KVX_MOD_X2048      NULL
#define KVX_MOD_X4096      NULL
#define KVX_MOD_X8192      NULL

#define KVX_MOD__X256      NULL

#define KVX_MOD_ABDSATUNS  kvx_modmap_abdsatuns
#define KVX_MOD_ACCESSES   kvx_modmap_accesses
#define KVX_MOD_AVERAGE    kvx_modmap_average
#define KVX_MOD_BOOLCASCO  kvx_modmap_boolcasco
#define KVX_MOD_CACHELEV   kvx_modmap_cachelev
#define KVX_MOD_CARRY      kvx_modmap_carry
#define KVX_MOD_COHERENCY  kvx_modmap_coherency
#define KVX_MOD_CONJUGATE  kvx_modmap_conjugate
#define KVX_MOD_COUNTING   kvx_modmap_counting
#define KVX_MOD_EXTENDMUL  kvx_modmap_extendmul
#define KVX_MOD_FLOATINGS  kvx_modmap_floatings
#define KVX_MOD_LANEWISE   kvx_modmap_lanewise
#define KVX_MOD_LOADCOND   kvx_modmap_loadcond
#define KVX_MOD_MOVECOND   kvx_modmap_movecond
#define KVX_MOD_MULXDT     kvx_modmap_mulxdt
#define KVX_MOD_NARROWINT  kvx_modmap_narrowint
#define KVX_MOD_SATURATE   kvx_modmap_saturate
#define KVX_MOD_SHIFTLEFT  kvx_modmap_shiftleft
#define KVX_MOD_SHIFTRIGHT kvx_modmap_shiftright
#define KVX_MOD_SIGNEDSAT  kvx_modmap_signedsat
#define KVX_MOD_SILENT     kvx_modmap_silent
#define KVX_MOD_SIMDCOND   kvx_modmap_simdcond
#define KVX_MOD_SIMDCONDD  kvx_modmap_simdcondd
#define KVX_MOD_STORECOND  kvx_modmap_storecond
#define KVX_MOD_TRANSPOSE  kvx_modmap_transpose
#define KVX_MOD_UNUSED     kvx_modmap_unused
#define KVX_MOD_VARIANT    kvx_modmap_variant
#define KVX_MOD_WIDENINT   kvx_modmap_widenint
#define KVX_MOD_XCHANNEL   kvx_modmap_xchannel
#define KVX_MOD_XCHANNELS  kvx_modmap_xchannels
#define KVX_MOD_XHALF      kvx_modmap_xhalf
#define KVX_MOD_XLOADCOND  kvx_modmap_xloadcond
#define KVX_MOD_XLOADHC    kvx_modmap_xloadhc
#define KVX_MOD_XLOADH     kvx_modmap_xloadh
#define KVX_MOD_XLOADQC    kvx_modmap_xloadqc
#define KVX_MOD_XLOADQ     kvx_modmap_xloadq
#define KVX_MOD_XMATMUL    kvx_modmap_xmatmul
#define KVX_MOD_XPRELOAD   kvx_modmap_xpreload
#define KVX_MOD_XQUARTER   kvx_modmap_xquarter
#define KVX_MOD_XSHUFFLEV  kvx_modmap_xshufflev
#define KVX_MOD_XSHUFFLEX  kvx_modmap_xshufflex
#define KVX_MOD_XVARIANT   kvx_modmap_xvariant

const char *kvx_mod_abdsatuns[] = {
 "", ".s", ".u", NULL
};

const struct kvx_modmap kvx_modmap_abdsatuns_all = {
  kvx_mod_abdsatuns,
  kvx_mod_abdsatuns
};

const struct kvx_modmap *kvx_modmap_abdsatuns = &kvx_modmap_abdsatuns_all;

const char *kvx_mod_accesses_kv3_v1[] = { NULL };
const struct kvx_modmap kvx_modmap_accesses_kv3_v1 = {
  kvx_mod_accesses_kv3_v1,
  kvx_mod_accesses_kv3_v1
};

const char *kvx_mod_accesses_kv3_v2[] = {
 "", ".w", ".r", ".wa", NULL
};
const struct kvx_modmap kvx_modmap_accesses_kv3_v2 = {
  kvx_mod_accesses_kv3_v2,
  kvx_mod_accesses_kv3_v2
};

const struct kvx_modmap *kvx_modmap_accesses = NULL;

const char *kvx_mod_average[] = {
 "", ".r", ".u", ".ru", NULL
};

const struct kvx_modmap kvx_modmap_average_all = {
  kvx_mod_average,
  kvx_mod_average
};

const struct kvx_modmap *kvx_modmap_average = &kvx_modmap_average_all;

const char *kvx_mod_boolcasco_kv3_v1[] = { "", NULL };
const struct kvx_modmap kvx_modmap_boolcasco_kv3_v1 = {
  kvx_mod_boolcasco_kv3_v1,
  kvx_mod_boolcasco_kv3_v1
};
const char *kvx_mod_boolcasco_kv3_v2[] = {
 "", ".g", ".s",
 ".v", ".v.g", ".v.s",
 NULL
};
const struct kvx_modmap kvx_modmap_boolcasco_kv3_v2 = {
  kvx_mod_boolcasco_kv3_v2,
  kvx_mod_boolcasco_kv3_v2
};
const struct kvx_modmap *kvx_modmap_boolcasco = NULL;

const char *kvx_mod_cachelev_kv3_v1[] = { NULL };
const struct kvx_modmap kvx_modmap_cachelev_kv3_v1 = {
  kvx_mod_cachelev_kv3_v1,
  kvx_mod_cachelev_kv3_v1
};
const char *kvx_mod_cachelev_kv3_v2[] = {
 ".l1", ".l2", NULL
};
const struct kvx_modmap kvx_modmap_cachelev_kv3_v2 = {
  kvx_mod_cachelev_kv3_v2,
  kvx_mod_cachelev_kv3_v2
};
const struct kvx_modmap *kvx_modmap_cachelev = NULL;

const char *kvx_mod_carry[] = {
 "", ".i", NULL
};
const struct kvx_modmap kvx_modmap_carry_all = {
  kvx_mod_carry, kvx_mod_carry
};
const struct kvx_modmap *kvx_modmap_carry = &kvx_modmap_carry_all;

const char *kvx_mod_coherency[] = {
 "", ".g", ".s", NULL
};
const struct kvx_modmap kvx_modmap_coherency_all = {
  kvx_mod_coherency, kvx_mod_coherency
};
const struct kvx_modmap *kvx_modmap_coherency = &kvx_modmap_coherency_all;

const char *kvx_mod_conjugate_kv3[] = {
 "", ".rn", ".ru", ".rd", ".rz",
 ".s", ".rn.s", ".ru.s", ".rd.s", ".rz.s",
 ".c", ".c.rn", ".c.ru", ".c.rd", ".c.rz",
 ".c.s", ".c.rn.s", ".c.ru.s", ".c.rd.s", ".c.rz.s",
 NULL
};
const struct kvx_modmap kvx_modmap_conjugate_kv3 = {
  kvx_mod_conjugate_kv3, kvx_mod_conjugate_kv3
};
const char *kvx_mod_conjugate_kv4_in[] = {
 "", ".rn", ".ru", ".rd", ".rz",
 ".s", ".rn.s", ".ru.s", ".rd.s", ".rz.s",
 NULL
};
const char *kvx_mod_conjugate_kv4_out[] = {
 "", ".rn", ".ru", ".rd", ".rz",
 "", ".rn", ".ru", ".rd", ".rz",
 NULL
};
const struct kvx_modmap kvx_modmap_conjugate_kv4 = {
  kvx_mod_conjugate_kv4_in, kvx_mod_conjugate_kv4_out
};
const struct kvx_modmap *kvx_modmap_conjugate;

const char *kvx_mod_counting[] = {
 "", ".lz", ".ls", ".tz", NULL
};
const struct kvx_modmap kvx_modmap_counting_all = {
  kvx_mod_counting, kvx_mod_counting
};
const struct kvx_modmap *kvx_modmap_counting = &kvx_modmap_counting_all;

const char *kvx_mod_extendmul[] = {
 "", ".u", ".su", NULL
};
const struct kvx_modmap kvx_modmap_extendmul_all = {
  kvx_mod_extendmul, kvx_mod_extendmul
};
const struct kvx_modmap *kvx_modmap_extendmul = &kvx_modmap_extendmul_all;

const char *kvx_mod_floatings_all_in[] = {
 "", ".rn", ".ru", ".rd", ".rz",
 ".s", ".rn.s", ".ru.s", ".rd.s", ".rz.s",
 NULL
};
const struct kvx_modmap kvx_modmap_floatings_kv3 = {
  kvx_mod_floatings_all_in, kvx_mod_floatings_all_in
};

const char *kvx_mod_floatings_kv4_out[] = {
 "", ".rn", ".ru", ".rd", ".rz",
 "", ".rn", ".ru", ".rd", ".rz",
 NULL
};
const struct kvx_modmap kvx_modmap_floatings_kv4 = {
  kvx_mod_floatings_all_in, kvx_mod_floatings_kv4_out
};
const struct kvx_modmap *kvx_modmap_floatings = NULL;

const char *kvx_mod_lanewise[] = {
 ".nez", ".eqz", NULL
};
const struct kvx_modmap kvx_modmap_lanewise_all = {
  kvx_mod_lanewise, kvx_mod_lanewise
};
const struct kvx_modmap *kvx_modmap_lanewise = &kvx_modmap_lanewise_all;

const char *kvx_mod_loadcond_kv3_v1[] = {
  /* kv3-1 LD, LQ, LO, XLO and kv3-2 LD, LQ, LO */
  ".dnez", ".deqz", ".dltz", ".dgez", ".dlez", ".dgtz", ".odd",
  ".even", ".wnez", ".weqz", ".wltz", ".wgez", ".wlez", ".wgtz",
  ".s.dnez", ".s.deqz", ".s.dltz", ".s.dgez", ".s.dlez", ".s.dgtz", ".s.odd",
  ".s.even", ".s.wnez", ".s.weqz", ".s.wltz", ".s.wgez", ".s.wlez", ".s.wgtz",
  ".u.dnez", ".u.deqz", ".u.dltz", ".u.dgez", ".u.dlez", ".u.dgtz", ".u.odd",
  ".u.even", ".u.wnez", ".u.weqz", ".u.wltz", ".u.wgez", ".u.wlez", ".u.wgtz",
  ".us.dnez", ".us.deqz", ".us.dltz", ".us.dgez", ".us.dlez", ".us.dgtz", ".us.odd",
  ".us.even", ".us.wnez", ".us.weqz", ".us.wltz", ".us.wgez", ".us.wlez", ".us.wgtz",
  NULL
};
const struct kvx_modmap kvx_modmap_loadcond_kv3_v1 = {
  kvx_mod_loadcond_kv3_v1, kvx_mod_loadcond_kv3_v1
};

const char *kvx_mod_loadcond_kv3_v2[] = {
  /* kv3-2 LO and XLO */
  ".dnez", ".deqz", ".wnez", ".weqz", ".mt", ".mf", ".mtc", ".mfc",
  ".s.dnez", ".s.deqz", ".s.wnez", ".s.weqz", ".s.mt", ".s.mf", ".s.mtc", ".s.mfc",
  ".u.dnez", ".u.deqz", ".u.wnez", ".u.weqz", ".u.mt", ".u.mf", ".u.mtc", ".u.mfc",
  ".us.dnez", ".us.deqz", ".us.wnez", ".us.weqz", ".us.mt", ".us.mf", ".us.mtc", ".us.mfc",
  NULL
};
const struct kvx_modmap kvx_modmap_loadcond_kv3_v2 = {
  kvx_mod_loadcond_kv3_v2, kvx_mod_loadcond_kv3_v2
};
const struct kvx_modmap *kvx_modmap_loadcond = NULL;


const char *kvx_mod_movecond[] = {
 ".wnez", ".weqz", ".wltz", ".wgez", ".wlez", ".wgtz", ".odd", ".even",
 ".dnez", ".deqz", ".dltz", ".dgez", ".dlez", ".dgtz", NULL
};
const struct kvx_modmap kvx_modmap_movecond_all = {
  kvx_mod_movecond, kvx_mod_movecond
};
const struct kvx_modmap *kvx_modmap_movecond = &kvx_modmap_movecond_all;

const char *kvx_mod_mulxdt[] = {
 "", ".u", ".su", ".uz", NULL
};
const struct kvx_modmap kvx_modmap_mulxdt_all = {
  kvx_mod_mulxdt, kvx_mod_mulxdt
};
const struct kvx_modmap *kvx_modmap_mulxdt = &kvx_modmap_mulxdt_all;

const char *kvx_mod_narrowint[] = {
 "", ".q", ".s", ".us", NULL
};
const struct kvx_modmap kvx_modmap_narrowint_all = {
  kvx_mod_narrowint, kvx_mod_narrowint
};
const struct kvx_modmap *kvx_modmap_narrowint = &kvx_modmap_narrowint_all;

const char *kvx_mod_saturate[] = {
 "", ".s", ".us", NULL
};
const struct kvx_modmap kvx_modmap_saturate_all = {
  kvx_mod_saturate, kvx_mod_saturate
};
const struct kvx_modmap *kvx_modmap_saturate = &kvx_modmap_saturate_all;

const char *kvx_mod_shiftleft[] = {
 "", ".s", ".us", ".r", NULL
};
const struct kvx_modmap kvx_modmap_shiftleft_all = {
  kvx_mod_shiftleft, kvx_mod_shiftleft
};
const struct kvx_modmap *kvx_modmap_shiftleft = &kvx_modmap_shiftleft_all;

const char *kvx_mod_shiftright[] = {
 "", ".a", ".as", ".r", NULL
};
const struct kvx_modmap kvx_modmap_shiftright_all = {
  kvx_mod_shiftright, kvx_mod_shiftright
};
const struct kvx_modmap *kvx_modmap_shiftright = &kvx_modmap_shiftright_all;

const char *kvx_mod_signedsat[] = {
 "", ".s", NULL
};
const struct kvx_modmap kvx_modmap_signedsat_all = {
  kvx_mod_signedsat, kvx_mod_signedsat
};
const struct kvx_modmap *kvx_modmap_signedsat = &kvx_modmap_signedsat_all;

const char *kvx_mod_silent_kv3[] = {
 "", ".s", NULL
};
const struct kvx_modmap kvx_modmap_silent_kv3 = {
  kvx_mod_silent_kv3, kvx_mod_silent_kv3,
};
const char *kvx_mod_silent_kv4_in[] = {
 "", ".s", NULL
};
const char *kvx_mod_silent_kv4_out[] = {
 "", "", NULL
};
const struct kvx_modmap kvx_modmap_silent_kv4 = {
  kvx_mod_silent_kv4_in, kvx_mod_silent_kv4_out,
};
const struct kvx_modmap *kvx_modmap_silent = NULL;

const char *kvx_mod_simdcond[] = {
 ".nez", ".eqz", ".ltz", ".gez", ".lez", ".gtz", ".odd", ".even", NULL
};
const struct kvx_modmap kvx_modmap_simdcond_all = {
  kvx_mod_simdcond, kvx_mod_simdcond
};
const struct kvx_modmap *kvx_modmap_simdcond = &kvx_modmap_simdcond_all;

const char *kvx_mod_simdcondd[] = {
    ".dnez", ".deqz", ".dltz", ".dgez", ".dlez", ".dgtz", ".odd", ".even", NULL
};
const struct kvx_modmap kvx_modmap_simdcondd_all = {
  kvx_mod_simdcond, kvx_mod_simdcondd
};
const struct kvx_modmap *kvx_modmap_simdcondd = &kvx_modmap_simdcondd_all;

const char *kvx_mod_storecond_kv3_v1[] = {
  /* kv3-1 SD, SQ, SO, XSO and kv3-2 SD, SQ, SO */
  ".dnez", ".deqz", ".dltz", ".dgez", ".dlez", ".dgtz", ".odd",
  ".even", ".wnez", ".weqz", ".wltz", ".wgez", ".wlez", ".wgtz",
  NULL
};
const struct kvx_modmap kvx_modmap_storecond_kv3_v1 = {
  kvx_mod_storecond_kv3_v1, kvx_mod_storecond_kv3_v1,
};
const char *kvx_mod_storecond_kv3_v2[] = {
  /* kv3-1 SD, SQ, SO, XSO and kv3-2 SD, SQ, SO */
  ".dnez", ".deqz", ".dltz", ".dgez", ".dlez", ".dgtz", ".odd",
  ".even", ".wnez", ".weqz", ".wltz", ".wgez", ".wlez", ".wgtz",
  /* SO and XSO */
  ".dnez", ".deqz", ".wnez", ".weqz", ".mt", ".mf",
  NULL
};
const struct kvx_modmap kvx_modmap_storecond_kv3_v2 = {
  kvx_mod_storecond_kv3_v2, kvx_mod_storecond_kv3_v2,
};
const struct kvx_modmap *kvx_modmap_storecond = NULL;

const char *kvx_mod_transpose_all_in[] = {
  "", ".tn", ".nt", ".tt", ".nn",
  ".tn.rn", ".nt.rn", ".tt.rn", ".nn.rn",
  ".rd", ".tn.rd", ".nt.rd", ".tt.rd", ".nn.rd",
  ".ru", ".tn.ru", ".nt.ru", ".tt.ru", ".nn.ru",
  ".rz", ".tn.rz", ".nt.rz", ".tt.rz", ".nn.rz",
  ".rn", ".tn.rn", ".nt.rn", ".tt.rn", ".nn.rn",
  ".s", ".tn.s", ".nt.s", ".tt.s", ".nn.s",
  ".rn.s", ".tn.rn.s", ".nt.rn.s", ".tt.rn.s", ".nn.rn.s",
  ".rd.s", ".tn.rd.s", ".nt.rd.s", ".tt.rd.s", ".nn.rd.s",
  ".ru.s", ".tn.ru.s", ".nt.ru.s", ".tt.ru.s", ".nn.ru.s",
  ".rz.s", ".tn.rz.s", ".nt.rz.s", ".tt.rz.s", ".nn.rz.s",
  NULL
};
const char *kvx_mod_transpose_all_out[] = {
  "", ".tn", ".nt", ".tt", "",
  ".tn.rn", ".nt.rn", ".tt.rn", ".rn",
  ".rd", ".tn.rd", ".nt.rd", ".tt.rd", ".rd",
  ".ru", ".tn.ru", ".nt.ru", ".tt.ru", ".ru",
  ".rz", ".tn.rz", ".nt.rz", ".tt.rz", ".rz",
  ".rn", ".tn.rn", ".nt.rn", ".tt.rn", ".rn",
  ".s", ".tn.s", ".nt.s", ".tt.s", ".s",
  ".rn.s", ".tn.rn.s", ".nt.rn.s", ".tt.rn.s", ".rn.s",
  ".rd.s", ".tn.rd.s", ".nt.rd.s", ".tt.rd.s", ".rd.s",
  ".ru.s", ".tn.ru.s", ".nt.ru.s", ".tt.ru.s", ".ru.s",
  ".rz.s", ".tn.rz.s", ".nt.rz.s", ".tt.rz.s", ".rz.s",
  NULL
};
const struct kvx_modmap kvx_modmap_transpose_all = {
  kvx_mod_transpose_all_in, kvx_mod_transpose_all_out
};
const struct kvx_modmap *kvx_modmap_transpose = &kvx_modmap_transpose_all;

const char *kvx_mod_variant[] = {
 "", ".s", ".u", ".us", NULL
};
const struct kvx_modmap kvx_modmap_variant_all = {
  kvx_mod_variant, kvx_mod_variant
};
const struct kvx_modmap *kvx_modmap_variant = &kvx_modmap_variant_all;

const char *kvx_mod_widenint[] = {
 "", ".z", ".q", NULL
};
const struct kvx_modmap kvx_modmap_widenint_all = {
  kvx_mod_widenint, kvx_mod_widenint
};
const struct kvx_modmap *kvx_modmap_widenint = &kvx_modmap_widenint_all;

const char *kvx_mod_xchannel[] = {
 ".f", ".b", NULL
};
const struct kvx_modmap kvx_modmap_xchannel_all = {
  kvx_mod_xchannel, kvx_mod_xchannel
};
const struct kvx_modmap *kvx_modmap_xchannel = &kvx_modmap_xchannel_all;

const char *kvx_mod_xchannels[] = {
 ".f.f", ".f.b", ".b.f", ".b.b", NULL
};
const struct kvx_modmap kvx_modmap_xchannels_all = {
  kvx_mod_xchannels, kvx_mod_xchannels
};
const struct kvx_modmap *kvx_modmap_xchannels = &kvx_modmap_xchannels_all;

const char *kvx_mod_xloadcond_kv3_v1[] = {
  ".u.dnez", ".u.deqz", ".u.dltz", ".u.dgez", ".u.dlez", ".u.dgtz", ".u.odd",
  ".u.even", ".u.wnez", ".u.weqz", ".u.wltz", ".u.wgez", ".u.wlez", ".u.wgtz",
  ".us.dnez", ".us.deqz", ".us.dltz", ".us.dgez", ".us.dlez", ".us.dgtz", ".us.odd",
  ".us.even", ".us.wnez", ".us.weqz", ".us.wltz", ".us.wgez", ".us.wlez", ".us.wgtz",
  NULL
};
const struct kvx_modmap kvx_modmap_xloadcond_kv3_v1 = {
  kvx_mod_xloadcond_kv3_v1, kvx_mod_xloadcond_kv3_v1,
};
const char *kvx_mod_xloadcond_kv3_v2[] = {
  // kv3-1 LD, LQ, LO, XLO and kv3-2 LD, LQ, LO
  ".dnez", ".deqz", ".dltz", ".dgez", ".dlez", ".dgtz", ".odd",
  ".even", ".wnez", ".weqz", ".wltz", ".wgez", ".wlez", ".wgtz",
  ".s.dnez", ".s.deqz", ".s.dltz", ".s.dgez", ".s.dlez", ".s.dgtz", ".s.odd",
  ".s.even", ".s.wnez", ".s.weqz", ".s.wltz", ".s.wgez", ".s.wlez", ".s.wgtz",
  ".u.dnez", ".u.deqz", ".u.dltz", ".u.dgez", ".u.dlez", ".u.dgtz", ".u.odd",
  ".u.even", ".u.wnez", ".u.weqz", ".u.wltz", ".u.wgez", ".u.wlez", ".u.wgtz",
  ".us.dnez", ".us.deqz", ".us.dltz", ".us.dgez", ".us.dlez", ".us.dgtz", ".us.odd",
  ".us.even", ".us.wnez", ".us.weqz", ".us.wltz", ".us.wgez", ".us.wlez", ".us.wgtz",
  // kv3-2 LO and XLO
  ".dnez", ".deqz", ".wnez", ".weqz", ".mt", ".mf", ".mtc", ".mfc",
  ".s.dnez", ".s.deqz", ".s.wnez", ".s.weqz", ".s.mt", ".s.mf", ".s.mtc", ".s.mfc",
  ".u.dnez", ".u.deqz", ".u.wnez", ".u.weqz", ".u.mt", ".u.mf", ".u.mtc", ".u.mfc",
  ".us.dnez", ".us.deqz", ".us.wnez", ".us.weqz", ".us.mt", ".us.mf", ".us.mtc", ".us.mfc",
  NULL
};
const struct kvx_modmap kvx_modmap_xloadcond_kv3_v2 = {
  kvx_mod_xloadcond_kv3_v2, kvx_mod_xloadcond_kv3_v2,
};
const struct kvx_modmap *kvx_modmap_xloadcond = NULL;

/* This is remapped to .lo .hi */
const char *kvx_mod_xhalf_all_in[] = {
 ".h0", ".h1", NULL
};
const char *kvx_mod_xhalf_all_out[] = {
 ".lo", ".hi", NULL
};
const struct kvx_modmap kvx_modmap_xhalf_all = {
  kvx_mod_xhalf_all_in, kvx_mod_xhalf_all_out
};
const struct kvx_modmap *kvx_modmap_xhalf = &kvx_modmap_xhalf_all;

const char *kvx_mod_xloadqc_kv3_v1[] = {
    ".dnez.q0", ".deqz.q0", ".wnez.q0", ".weqz.q0",
    ".s.dnez.q0", ".s.deqz.q0", ".s.wnez.q0", ".s.weqz.q0",
    ".u.dnez.q0", ".u.deqz.q0", ".u.wnez.q0", ".u.weqz.q0",
    ".us.dnez.q0", ".us.deqz.q0", ".us.wnez.q0", ".us.weqz.q0",
    ".dnez.q1", ".deqz.q1", ".wnez.q1", ".weqz.q1",
    ".s.dnez.q1", ".s.deqz.q1", ".s.wnez.q1", ".s.weqz.q1",
    ".u.dnez.q1", ".u.deqz.q1", ".u.wnez.q1", ".u.weqz.q1",
    ".us.dnez.q1", ".us.deqz.q1", ".us.wnez.q1", ".us.weqz.q1",
    ".dnez.q2", ".deqz.q2", ".wnez.q2", ".weqz.q2",
    ".s.dnez.q2", ".s.deqz.q2", ".s.wnez.q2", ".s.weqz.q2",
    ".u.dnez.q2", ".u.deqz.q2", ".u.wnez.q2", ".u.weqz.q2",
    ".us.dnez.q2", ".us.deqz.q2", ".us.wnez.q2", ".us.weqz.q2",
    ".dnez.q3", ".deqz.q3", ".wnez.q3", ".weqz.q3",
    ".s.dnez.q3", ".s.deqz.q3", ".s.wnez.q3", ".s.weqz.q3",
    ".u.dnez.q3", ".u.deqz.q3", ".u.wnez.q3", ".u.weqz.q3",
    ".us.dnez.q3", ".us.deqz.q3", ".us.wnez.q3", ".us.weqz.q3",
    NULL
};
const struct kvx_modmap kvx_modmap_xloadqc_kv3_v1 = {
  kvx_mod_xloadqc_kv3_v1, kvx_mod_xloadqc_kv3_v1
};
const char *kvx_mod_xloadqc_kv3_v2[] = {
    ".dnez.q0", ".deqz.q0", ".wnez.q0", ".weqz.q0",
    ".s.dnez.q0", ".s.deqz.q0", ".s.wnez.q0", ".s.weqz.q0",
    ".u.dnez.q0", ".u.deqz.q0", ".u.wnez.q0", ".u.weqz.q0",
    ".us.dnez.q0", ".us.deqz.q0", ".us.wnez.q0", ".us.weqz.q0",
    ".dnez.q1", ".deqz.q1", ".wnez.q1", ".weqz.q1",
    ".s.dnez.q1", ".s.deqz.q1", ".s.wnez.q1", ".s.weqz.q1",
    ".u.dnez.q1", ".u.deqz.q1", ".u.wnez.q1", ".u.weqz.q1",
    ".us.dnez.q1", ".us.deqz.q1", ".us.wnez.q1", ".us.weqz.q1",
    ".dnez.q2", ".deqz.q2", ".wnez.q2", ".weqz.q2",
    ".s.dnez.q2", ".s.deqz.q2", ".s.wnez.q2", ".s.weqz.q2",
    ".u.dnez.q2", ".u.deqz.q2", ".u.wnez.q2", ".u.weqz.q2",
    ".us.dnez.q2", ".us.deqz.q2", ".us.wnez.q2", ".us.weqz.q2",
    ".dnez.q3", ".deqz.q3", ".wnez.q3", ".weqz.q3",
    ".s.dnez.q3", ".s.deqz.q3", ".s.wnez.q3", ".s.weqz.q3",
    ".u.dnez.q3", ".u.deqz.q3", ".u.wnez.q3", ".u.weqz.q3",
    ".us.dnez.q3", ".us.deqz.q3", ".us.wnez.q3", ".us.weqz.q3",
    ".mt.q0", ".mf.q0", ".mtc.q0", ".mfc.q0",
    ".s.mt.q0", ".s.mf.q0", ".s.mtc.q0", ".s.mfc.q0",
    ".u.mt.q0", ".u.mf.q0", ".u.mtc.q0", ".u.mfc.q0",
    ".us.mt.q0", ".us.mf.q0", ".us.mtc.q0", ".us.mfc.q0",
    ".mt.q1", ".mf.q1", ".mtc.q1", ".mfc.q1",
    ".s.mt.q1", ".s.mf.q1", ".s.mtc.q1", ".s.mfc.q1",
    ".u.mt.q1", ".u.mf.q1", ".u.mtc.q1", ".u.mfc.q1",
    ".us.mt.q1", ".us.mf.q1", ".us.mtc.q1", ".us.mfc.q1",
    ".mt.q2", ".mf.q2", ".mtc.q2", ".mfc.q2",
    ".s.mt.q2", ".s.mf.q2", ".s.mtc.q2", ".s.mfc.q2",
    ".u.mt.q2", ".u.mf.q2", ".u.mtc.q2", ".u.mfc.q2",
    ".us.mt.q2", ".us.mf.q2", ".us.mtc.q2", ".us.mfc.q2",
    ".mt.q3", ".mf.q3", ".mtc.q3", ".mfc.q3",
    ".s.mt.q3", ".s.mf.q3", ".s.mtc.q3", ".s.mfc.q3",
    ".u.mt.q3", ".u.mf.q3", ".u.mtc.q3", ".u.mfc.q3",
    ".us.mt.q3", ".us.mf.q3", ".us.mtc.q3", ".us.mfc.q3",
    NULL
};
const struct kvx_modmap kvx_modmap_xloadqc_kv3_v2 = {
  kvx_mod_xloadqc_kv3_v2, kvx_mod_xloadqc_kv3_v2
};
const struct kvx_modmap *kvx_modmap_xloadqc = NULL;

const char *kvx_mod_xloadq_kv3_v1[] = {
 ".u.q0", ".us.q0",
 ".u.q1", ".us.q1",
 ".u.q2", ".us.q2",
 ".u.q3", ".us.q3",
 NULL
};
const struct kvx_modmap kvx_modmap_xloadq_kv3_v1 = {
  kvx_mod_xloadq_kv3_v1, kvx_mod_xloadq_kv3_v1
};

const char *kvx_mod_xloadq_kv3_v2[] = {
 ".q0", ".s.q0", ".u.q0", ".us.q0",
 ".q1", ".s.q1", ".u.q1", ".us.q1",
 ".q2", ".s.q2", ".u.q2", ".us.q2",
 ".q3", ".s.q3", ".u.q3", ".us.q3",
 NULL
};
const struct kvx_modmap kvx_modmap_xloadq_kv3_v2 = {
  kvx_mod_xloadq_kv3_v2, kvx_mod_xloadq_kv3_v2
};
const struct kvx_modmap *kvx_modmap_xloadq = NULL;

const char *kvx_mod_xmatmul[] = {
 "", ".u", ".su", ".us", NULL
};
const struct kvx_modmap kvx_modmap_xmatmul_all = {
  kvx_mod_xmatmul, kvx_mod_xmatmul,
};
const struct kvx_modmap *kvx_modmap_xmatmul = &kvx_modmap_xmatmul_all;

/* This is remapped to .x .y .z .t */
const char *kvx_mod_xquarter_all_in[] = {
 ".q0", ".q1", ".q2", ".q3", NULL
};
const char *kvx_mod_xquarter_all_out[] = {
 ".x", ".y", ".z", ".t", NULL
};
const struct kvx_modmap kvx_modmap_xquarter_all = {
  kvx_mod_xquarter_all_in, kvx_mod_xquarter_all_out
};
const struct kvx_modmap *kvx_modmap_xquarter = &kvx_modmap_xquarter_all;

const char *kvx_mod_xshufflev[] = {
 "", ".td", NULL
};
const struct kvx_modmap kvx_modmap_xshufflev_all = {
  kvx_mod_xshufflev, kvx_mod_xshufflev,
};
const struct kvx_modmap *kvx_modmap_xshufflev = &kvx_modmap_xshufflev_all;

const char *kvx_mod_xshufflex[] = {
 "", ".zd", ".ud", ".tq", ".tw", ".zw", ".uw", NULL
};
const struct kvx_modmap kvx_modmap_xshufflex_all = {
  kvx_mod_xshufflex, kvx_mod_xshufflex,
};
const struct kvx_modmap *kvx_modmap_xshufflex = &kvx_modmap_xshufflex_all;

const char *kvx_mod_xvariant_kv3_v1[] = {
 ".u", ".us", NULL
};
const struct kvx_modmap kvx_modmap_xvariant_kv3_v1 = {
  kvx_mod_xvariant_kv3_v1, kvx_mod_xvariant_kv3_v1,
};
const char *kvx_mod_xvariant_kv3_v2[] = {
 "", ".s", ".u", ".us", NULL
};
const struct kvx_modmap kvx_modmap_xvariant_kv3_v2 = {
  kvx_mod_xvariant_kv3_v2, kvx_mod_xvariant_kv3_v2,
};
const struct kvx_modmap *kvx_modmap_xvariant = NULL;

#define V64mode V1DImode
#define V128mode V2DImode
#define V256mode V4DImode
#define V512mode V8DImode
#define X256mode V1OImode
#define X512mode V2OImode
#define X1024mode V4OImode
#define X2048mode V8OImode
#define X4096mode V16OImode
#define X8192mode V32OImode
#define V64mode V1DImode
#define V128mode V2DImode
#define V256mode V4DImode
#define V512mode V8DImode
#define X256mode V1OImode
#define X512mode V2OImode
#define X1024mode V4OImode
#define X2048mode V8OImode
#define X4096mode V16OImode
#define X8192mode V32OImode

enum kvx_builtin_type_index
{
  KVX_BTI_STRING,

  KVX_BTI_V8QI,
  KVX_BTI_V16QI,
  KVX_BTI_V32QI,
  KVX_BTI_V64QI,

  KVX_BTI_V4HI,
  KVX_BTI_V8HI,
  KVX_BTI_V16HI,
  KVX_BTI_V32HI,

  KVX_BTI_V2SI,
  KVX_BTI_V4SI,
  KVX_BTI_V8SI,
  KVX_BTI_V16SI,

  KVX_BTI_V2DI,
  KVX_BTI_V4DI,
  KVX_BTI_V8DI,

  KVX_BTI_V4HF,
  KVX_BTI_V8HF,
  KVX_BTI_V16HF,
  KVX_BTI_V32HF,

  KVX_BTI_V2SF,
  KVX_BTI_V4SF,
  KVX_BTI_V8SF,
  KVX_BTI_V16SF,

  KVX_BTI_V2DF,
  KVX_BTI_V4DF,
  KVX_BTI_V8DF,

  KVX_BTI_DI,
  KVX_BTI_V64,
  KVX_BTI_V128,
  KVX_BTI_V256,
  KVX_BTI_V512,

  KVX_BTI_OI,

  KVX_BTI_X256,
  KVX_BTI_X512,
  KVX_BTI_X1024,
  KVX_BTI_X2048,
  KVX_BTI_X4096,
  KVX_BTI_X8192,

  KVX_BTI_PX256,

  KVX_BTI_FLOATINGS,

  KVX_BTI_MAX
};

tree kvx_builtin_types[KVX_BTI_MAX];

#define KVX_FTYPE_NAME0(A) KVX_##A##_FTYPE
#define KVX_FTYPE_NAME1(A, B) KVX_##A##_FTYPE_##B
#define KVX_FTYPE_NAME2(A, B, C) KVX_##A##_FTYPE_##B##_##C
#define KVX_FTYPE_NAME3(A, B, C, D) KVX_##A##_FTYPE_##B##_##C##_##D
#define KVX_FTYPE_NAME4(A, B, C, D, E) KVX_##A##_FTYPE_##B##_##C##_##D##_##E
#define KVX_FTYPE_NAME5(A, B, C, D, E, F) KVX_##A##_FTYPE_##B##_##C##_##D##_##E##_##F

/* KVX_FTYPE_ATYPESN takes N KVX_FTYPES-like type codes and lists
   their associated KVX_ATYPEs.  */
#define KVX_FTYPE_ATYPES0(A) \
  KVX_ATYPE_##A
#define KVX_FTYPE_ATYPES1(A, B) \
  KVX_ATYPE_##A, KVX_ATYPE_##B
#define KVX_FTYPE_ATYPES2(A, B, C) \
  KVX_ATYPE_##A, KVX_ATYPE_##B, KVX_ATYPE_##C
#define KVX_FTYPE_ATYPES3(A, B, C, D) \
  KVX_ATYPE_##A, KVX_ATYPE_##B, KVX_ATYPE_##C, KVX_ATYPE_##D
#define KVX_FTYPE_ATYPES4(A, B, C, D, E) \
  KVX_ATYPE_##A, KVX_ATYPE_##B, KVX_ATYPE_##C, KVX_ATYPE_##D, KVX_ATYPE_##E
#define KVX_FTYPE_ATYPES5(A, B, C, D, E, F) \
  KVX_ATYPE_##A, KVX_ATYPE_##B, KVX_ATYPE_##C, KVX_ATYPE_##D, KVX_ATYPE_##E, KVX_ATYPE_##F

#define KVX_FTYPE_MOD0(A) KVX_MOD_##A
#define KVX_FTYPE_MOD1(A, B) KVX_MOD_##B
#define KVX_FTYPE_MOD2(A, B, C) KVX_MOD_##C
#define KVX_FTYPE_MOD3(A, B, C, D) KVX_MOD_##D
#define KVX_FTYPE_MOD4(A, B, C, D, E) KVX_MOD_##E
#define KVX_FTYPE_MOD5(A, B, C, D, E, F) KVX_MOD_##F

#define KVX_ATYPE_VOID void_type_node
#define KVX_ATYPE_VPTR ptr_type_node
#define KVX_ATYPE_CVPTR const_ptr_type_node
#define KVX_ATYPE_BOOL boolean_type_node

#define KVX_ATYPE_INT8 intQI_type_node
#define KVX_ATYPE_INT16 intHI_type_node
#define KVX_ATYPE_INT32 intSI_type_node
#define KVX_ATYPE_INT64 intDI_type_node
#define KVX_ATYPE_INT128 intTI_type_node

#define KVX_ATYPE_UINT8 unsigned_intQI_type_node
#define KVX_ATYPE_UINT16 unsigned_intHI_type_node
#define KVX_ATYPE_UINT32 unsigned_intSI_type_node
#define KVX_ATYPE_UINT64 unsigned_intDI_type_node
#define KVX_ATYPE_UINT128 unsigned_intTI_type_node

#define KVX_ATYPE_FLOAT16 float16_type_node
#define KVX_ATYPE_FLOAT32 float_type_node
#define KVX_ATYPE_FLOAT64 double_type_node
#define KVX_ATYPE_COMPLEX64 complex_float_type_node
#define KVX_ATYPE_COMPLEX128 complex_double_type_node

#define KVX_ATYPE_STRING (kvx_builtin_types[KVX_BTI_STRING])

#define KVX_ATYPE_V8QI (kvx_builtin_types[KVX_BTI_V8QI])
#define KVX_ATYPE_V16QI (kvx_builtin_types[KVX_BTI_V16QI])
#define KVX_ATYPE_V32QI (kvx_builtin_types[KVX_BTI_V32QI])
#define KVX_ATYPE_V64QI (kvx_builtin_types[KVX_BTI_V64QI])

#define KVX_ATYPE_V4HI (kvx_builtin_types[KVX_BTI_V4HI])
#define KVX_ATYPE_V8HI (kvx_builtin_types[KVX_BTI_V8HI])
#define KVX_ATYPE_V16HI (kvx_builtin_types[KVX_BTI_V16HI])
#define KVX_ATYPE_V32HI (kvx_builtin_types[KVX_BTI_V32HI])

#define KVX_ATYPE_V2SI (kvx_builtin_types[KVX_BTI_V2SI])
#define KVX_ATYPE_V4SI (kvx_builtin_types[KVX_BTI_V4SI])
#define KVX_ATYPE_V8SI (kvx_builtin_types[KVX_BTI_V8SI])
#define KVX_ATYPE_V16SI (kvx_builtin_types[KVX_BTI_V16SI])

#define KVX_ATYPE_V2DI (kvx_builtin_types[KVX_BTI_V2DI])
#define KVX_ATYPE_V4DI (kvx_builtin_types[KVX_BTI_V4DI])
#define KVX_ATYPE_V8DI (kvx_builtin_types[KVX_BTI_V8DI])

#define KVX_ATYPE_V4HF (kvx_builtin_types[KVX_BTI_V4HF])
#define KVX_ATYPE_V8HF (kvx_builtin_types[KVX_BTI_V8HF])
#define KVX_ATYPE_V16HF (kvx_builtin_types[KVX_BTI_V16HF])
#define KVX_ATYPE_V32HF (kvx_builtin_types[KVX_BTI_V32HF])

#define KVX_ATYPE_V2SF (kvx_builtin_types[KVX_BTI_V2SF])
#define KVX_ATYPE_V4SF (kvx_builtin_types[KVX_BTI_V4SF])
#define KVX_ATYPE_V8SF (kvx_builtin_types[KVX_BTI_V8SF])
#define KVX_ATYPE_V16SF (kvx_builtin_types[KVX_BTI_V16SF])

#define KVX_ATYPE_V2DF (kvx_builtin_types[KVX_BTI_V2DF])
#define KVX_ATYPE_V4DF (kvx_builtin_types[KVX_BTI_V4DF])
#define KVX_ATYPE_V8DF (kvx_builtin_types[KVX_BTI_V8DF])

#define KVX_ATYPE_DI  (kvx_builtin_types[KVX_BTI_DI])
#define KVX_ATYPE_V64 (kvx_builtin_types[KVX_BTI_V64])
#define KVX_ATYPE_V128 (kvx_builtin_types[KVX_BTI_V128])
#define KVX_ATYPE_V256 (kvx_builtin_types[KVX_BTI_V256])
#define KVX_ATYPE_V512 (kvx_builtin_types[KVX_BTI_V512])

#define KVX_ATYPE_OI (kvx_builtin_types[KVX_BTI_OI])

#define KVX_ATYPE_X256 (kvx_builtin_types[KVX_BTI_X256])
#define KVX_ATYPE_X512 (kvx_builtin_types[KVX_BTI_X512])
#define KVX_ATYPE_X1024 (kvx_builtin_types[KVX_BTI_X1024])
#define KVX_ATYPE_X2048 (kvx_builtin_types[KVX_BTI_X2048])
#define KVX_ATYPE_X4096 (kvx_builtin_types[KVX_BTI_X4096])
#define KVX_ATYPE_X8192 (kvx_builtin_types[KVX_BTI_X8192])

#define KVX_ATYPE_PX256 (kvx_builtin_types[KVX_BTI_PX256])

#define KVX_ATYPE_ABDSATUNS KVX_ATYPE_STRING
#define KVX_ATYPE_ACCESSES KVX_ATYPE_STRING
#define KVX_ATYPE_AVERAGE KVX_ATYPE_STRING
#define KVX_ATYPE_BOOLCASCO KVX_ATYPE_STRING
#define KVX_ATYPE_CACHELEV KVX_ATYPE_STRING
#define KVX_ATYPE_CARRY KVX_ATYPE_STRING
#define KVX_ATYPE_COHERENCY KVX_ATYPE_STRING
#define KVX_ATYPE_CONJUGATE KVX_ATYPE_STRING
#define KVX_ATYPE_COUNTING KVX_ATYPE_STRING
#define KVX_ATYPE_EXTENDMUL KVX_ATYPE_STRING
#define KVX_ATYPE_FLOATINGS KVX_ATYPE_STRING
#define KVX_ATYPE_LANEWISE KVX_ATYPE_STRING
#define KVX_ATYPE_LOADCOND KVX_ATYPE_STRING
#define KVX_ATYPE_MOVECOND KVX_ATYPE_STRING
#define KVX_ATYPE_MULXDT KVX_ATYPE_STRING
#define KVX_ATYPE_NARROWINT KVX_ATYPE_STRING
#define KVX_ATYPE_SATURATE KVX_ATYPE_STRING
#define KVX_ATYPE_SHIFTLEFT KVX_ATYPE_STRING
#define KVX_ATYPE_SHIFTRIGHT KVX_ATYPE_STRING
#define KVX_ATYPE_SIGNEDSAT KVX_ATYPE_STRING
#define KVX_ATYPE_SILENT KVX_ATYPE_STRING
#define KVX_ATYPE_SIMDCOND KVX_ATYPE_STRING
#define KVX_ATYPE_SIMDCONDD KVX_ATYPE_STRING
#define KVX_ATYPE_STORECOND KVX_ATYPE_STRING
#define KVX_ATYPE_TRANSPOSE KVX_ATYPE_STRING
#define KVX_ATYPE_UNUSED KVX_ATYPE_STRING
#define KVX_ATYPE_VARIANT KVX_ATYPE_STRING
#define KVX_ATYPE_WIDENINT KVX_ATYPE_STRING
#define KVX_ATYPE_XCHANNEL KVX_ATYPE_STRING
#define KVX_ATYPE_XCHANNELS KVX_ATYPE_STRING
#define KVX_ATYPE_XHALF KVX_ATYPE_STRING
#define KVX_ATYPE_XLOADCOND KVX_ATYPE_STRING
#define KVX_ATYPE_XLOADHC KVX_ATYPE_STRING
#define KVX_ATYPE_XLOADH KVX_ATYPE_STRING
#define KVX_ATYPE_XLOADQC KVX_ATYPE_STRING
#define KVX_ATYPE_XLOADQ KVX_ATYPE_STRING
#define KVX_ATYPE_XMATMUL KVX_ATYPE_STRING
#define KVX_ATYPE_XPRELOAD KVX_ATYPE_STRING
#define KVX_ATYPE_XQUARTER KVX_ATYPE_STRING
#define KVX_ATYPE_XSHUFFLEV KVX_ATYPE_STRING
#define KVX_ATYPE_XSHUFFLEX KVX_ATYPE_STRING
#define KVX_ATYPE_XVARIANT KVX_ATYPE_STRING

/* Classifies the prototype of a built-in function.  */
enum kvx_function_type {
#define DEF_KVX_FTYPE(NARGS, LIST) KVX_FTYPE_NAME##NARGS LIST,
#include "config/kvx/kvx-ftypes.def"
#undef DEF_KVX_FTYPE
  KVX_MAX_FTYPE_MAX
};

static const kvx_modmap*
kvx_get_modifiers (enum kvx_function_type type)
{
  switch (type)
  {
#define DEF_KVX_FTYPE(NUM, ARGS)					\
  case KVX_FTYPE_NAME##NUM ARGS:					\
    return KVX_FTYPE_MOD##NUM ARGS;
#include "config/kvx/kvx-ftypes.def"
#undef DEF_KVX_FTYPE
  default:
    return NULL;
  }
}

static tree
kvx_build_function_type (enum kvx_function_type type, int variadic_p)
{
  static tree types[(int) KVX_MAX_FTYPE_MAX];

  // TODO: Check if no memory leak, when reassigning the type.
  // Maybe we would like to store the va_types and the types in 2 different
  // tables.
  if (types[(int) type] == NULL_TREE || variadic_p)
    switch (type)
      {
#define DEF_KVX_FTYPE(NUM, ARGS)                                        \
  case KVX_FTYPE_NAME##NUM ARGS:                                        \
    types[(int) type] = variadic_p                                      \
	? build_varargs_function_type_list (KVX_FTYPE_ATYPES##NUM ARGS, \
				    NULL_TREE)                          \
	: build_function_type_list (KVX_FTYPE_ATYPES##NUM ARGS,         \
					    NULL_TREE);                 \
    break;
#include "config/kvx/kvx-ftypes.def"
#undef DEF_KVX_FTYPE
      default:
	gcc_unreachable ();
      }

  return types[(int) type];
}

/* Specifies how a built-in function should be converted into rtl.  */
enum kvx_builtin_type
{
  /* The function corresponds directly to an .md pattern.  */
  KVX_BLTN_DIRECT,

  /* The function does not map directly to an .md pattern and need a custom
     expander. */
  KVX_BLTN_INDIRECT,
  KVX_BLTN_TYPE_MAX
};

struct kvx_builtin_desc
{
  /* The name of the built-in function.  */
  const char *name;

  /* The code of the main .md file instruction or -1 for KVX_BLTN_INDIRECT. */
  enum insn_code icode;

  /* Specifies how the function should be expanded.  */
  enum kvx_builtin_type type;

  /* The function's prototype.  */
  enum kvx_function_type prototype;

  /* The expander function.  */
  rtx (*expand)(rtx, tree);

  /* Predicate telling whether the built-in function can take a variable
     number of arguments.  */
  unsigned int variadic_p;

  /* The function's prototype.  */
  enum kvx_function_type variadic_prototype;

  /* Whether the function is available.  */
  unsigned int (*avail) (void);
};

/* Declare an availability predicate for built-in functions.  */
#define AVAIL(NAME, COND)		\
 static unsigned int			\
 kvx_builtin_avail_##NAME (void)	\
 {					\
   return (COND);			\
 }

AVAIL (true, true)
AVAIL (kv3_1, KV3_1)
AVAIL (kv3, (KV3_1 || KV3_2))
AVAIL (kv3_2, (KV4 || KV3_2))
AVAIL (kv4_1, KV4)

static inline const char *
kvx_tree_string_constant (tree arg, const char *name)
{
  tree offset_tree = 0;
  arg = string_constant (arg, &offset_tree, 0, 0);
  if (!arg)
    error ("%<__builtin_kvx_%s%> requires a constant string modifier", name);
  return TREE_STRING_POINTER (arg);
}

static rtx
build_arg (tree arg, const char *name, const struct kvx_modmap *mod_table)
{
  const char *modifier = kvx_tree_string_constant (arg, name);
  for (int i = 0; mod_table->imap[i]; ++i)
    if (!strcmp (modifier, mod_table->imap[i]))
      return gen_rtx_CONST_STRING (VOIDmode, mod_table->omap[i]);

  char buffer[1024] = { 0 };
  char *btmp = buffer;
  for (int i = 0; mod_table->imap[i]; ++i)
    {
      btmp = stpcpy (btmp, mod_table->imap[i]);
      if (mod_table->imap[i+1])
        btmp = stpcpy (btmp, ", ");
    }
  error ("%<__builtin_kvx_%s%> modifier %<%s%> not recognized", name, modifier);
  inform (input_location, "modifier list: %s", buffer);
  return 0;
}

static rtx
verify_const_bool_arg (rtx arg, const char *name, const char *where)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned long long tmp = INTVAL (arg);
      if (tmp == 0LL || tmp == 1LL)
	return arg;
    }
  error ("%<__builtin_kvx_%s%> expects a boolean immediate in %s argument", name,
	 where);
  return 0;
}

static rtx
verify_const_ready_arg (rtx arg, const char *name, const char *where)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned long long tmp = INTVAL (arg);
      if (tmp == 0LL || tmp == 1LL)
	return arg;
    }
  if (GET_MODE (arg) == SImode)
    return force_not_mem (arg);
  error ("%<__builtin_kvx_%s%> expects a boolean value in %s argument", name,
	 where);
  return 0;
}

__attribute__ ((unused))
static rtx
verify_const_int_arg (rtx arg, int bits, const char *name, const char *where)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned shift = 64 - bits;
      long long tmp = INTVAL (arg);
      signed long long stmp = tmp;
      if (tmp == (stmp << shift) >> shift)
	return arg;
    }
  error ("%<__builtin_kvx_%s%> expects a %d-bit signed immediate in %s argument",
	 name, bits, where);
  return 0;
}

static rtx
verify_const_uint_arg (rtx arg, int bits, const char *name,
	const char *where, bool strict)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned shift = 64 - bits;
      unsigned long long utmp = INTVAL (arg);
      if (utmp == (utmp << shift) >> shift)
	return arg;
    }
  if (strict)
    error ("%<__builtin_kvx_%s%> expects a %d-bit unsigned immediate "
	   "in %s argument", name, bits, where);
  return NULL_RTX;
}

__attribute__ ((unused))
static rtx
verify_const_field_arg (rtx arg, int bits, const char *name, const char *where)
{
  if (GET_CODE (arg) == CONST_INT && GET_MODE (arg) == VOIDmode)
    {
      unsigned shift = 64 - bits;
      long long tmp = INTVAL (arg);
      signed long long stmp = tmp;
      unsigned long long utmp = tmp;
      if (tmp == (stmp << shift) >> shift)
	return arg;
      if (utmp == (utmp << shift) >> shift)
	return arg;
    }
  error ("%<__builtin_kvx_%s%> expects a %d-bit signed or unsigned immediate in %s "
	 "argument",
	 name, bits, where);
  return 0;
}

static int
verify_sfr_regno (int regno, const char *name, const char *where)
{
  int gcc_regno = KVX_SFR_FIRST_REGNO + regno;
  if (gcc_regno  > KVX_SFR_LAST_REGNO) {
    error ("%<__builtin_kvx_%s%> passed %d as %s argument: expects a SFR register index between 0 and %d",
	   name, regno, where, KVX_SFR_LAST_REGNO - KVX_SFR_FIRST_REGNO);
  }

  return gcc_regno;
}

unsigned long kvx_xundef_counter;

#define KVX_EXPAND_BUILTIN_0_VOID(name, name2, ...)                            \
  static rtx kvx_expand_builtin_##name (rtx target ATTRIBUTE_UNUSED,           \
                                        tree args ATTRIBUTE_UNUSED)            \
  {                                                                            \
    emit_insn (CONCAT2 (gen_, name2) ());                                                \
    return NULL_RTX;                                                           \
  }

#define KVX_EXPAND_BUILTIN_1_VOID(name, name2, smode)                          \
  static rtx kvx_expand_builtin_##name (rtx target ATTRIBUTE_UNUSED, tree args)\
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    arg1 = force_reg (smode, arg1);                                            \
    emit_insn (CONCAT2 (gen_, name2) (arg1));                                            \
    return NULL_RTX;                                                           \
  }

#define KVX_EXPAND_BUILTIN_2_VOID(name, name2, smode)                          \
  static rtx kvx_expand_builtin_##name (rtx target ATTRIBUTE_UNUSED, tree args)\
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    arg1 = force_reg (smode, arg1);                                            \
    arg2 = force_reg (smode, arg2);                                            \
    emit_insn (gen_##name2 (arg1, arg2));                                      \
    return NULL_RTX;                                                           \
  }

#define KVX_EXPAND_BUILTIN_3_CACHELEVEL(name, name2, smode)                    \
  static rtx kvx_expand_builtin_##name (rtx target ATTRIBUTE_UNUSED, tree args)\
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = build_arg (CALL_EXPR_ARG (args, 2), #name, kvx_modmap_cachelev);\
    arg1 = force_reg (smode, arg1);                                            \
    arg2 = force_reg (smode, arg2);                                            \
    emit_insn (gen_##name2 (arg1, arg2, arg3));                                \
    return NULL_RTX;                                                           \
  }

#define KVX_EXPAND_BUILTIN_SHIFT(name, name2, tmode, smode)                    \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    int bits = __builtin_ctz (GET_MODE_NUNITS (tmode));                        \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    arg1 = force_reg (tmode, arg1);                                            \
    arg2 = verify_const_int_arg (arg2, bits + 1, #name, "second");             \
    if (arg3 != CONST0_RTX (smode))                                            \
      {                                                                        \
        if (immediate_operand (arg3, VOIDmode))                                \
          arg3 = force_reg (smode, arg3);                                      \
        else                                                                   \
          arg3 = simplify_gen_subreg (smode, arg3, GET_MODE (arg3), 0);        \
      }                                                                        \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_##name2 (target, arg1, arg2, arg3));                        \
    return target;                                                             \
  }

static rtx
kvx_expand_builtin_get (rtx target, tree args)
{
  int regno = -1;
  rtx sys_reg = NULL_RTX;
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  if (verify_const_uint_arg (arg1, 9, "get", "first", false) != NULL_RTX)
      {
	arg1 = verify_const_uint_arg (arg1, 9, "get", "first", false);
	regno = verify_sfr_regno (INTVAL (arg1), "get", "first");
	sys_reg = gen_rtx_REG (DImode, regno);
      }
  else
    {
      sys_reg = force_reg (GET_MODE (arg1), arg1);
      sys_reg = simplify_gen_subreg (DImode, sys_reg, SImode, 0);
    }
  if (!target)
    target = gen_reg_rtx (DImode);
  else
    target = force_reg (DImode, target);
  if (regno == KVX_PCR_REGNO)
    emit_move_insn (target, sys_reg);
  else
    emit_insn (gen_kvx_get (target, sys_reg));
  return target;
}

static rtx
kvx_expand_builtin_set (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = verify_const_uint_arg (arg1, 9, "set", "first", true);
  arg2 = force_reg (DImode, arg2);
  int regno = verify_sfr_regno (INTVAL (arg1), "set", "first");
  rtx sys_reg = gen_rtx_REG (DImode, regno);
  emit_insn (gen_kvx_set (sys_reg, arg2));
  return NULL_RTX;
}

static rtx
kvx_expand_builtin_wfxl (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = verify_const_uint_arg (arg1, 9, "wfxl", "first", true);
  arg2 = force_reg (DImode, arg2);
  int regno = verify_sfr_regno (INTVAL (arg1), "wfxl", "first");
  rtx sys_reg = gen_rtx_REG (DImode, regno);
  emit_insn (gen_kvx_wfxl (sys_reg, arg2));
  return NULL_RTX;
}

static rtx
kvx_expand_builtin_wfxm (rtx target ATTRIBUTE_UNUSED, tree args)
{
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = verify_const_uint_arg (arg1, 9, "wfxm", "first", true);
  arg2 = force_reg (DImode, arg2);
  int regno = verify_sfr_regno (INTVAL (arg1), "wfxm", "first");
  rtx sys_reg = gen_rtx_REG (DImode, regno);
  emit_insn (gen_kvx_wfxm (sys_reg, arg2));
  return NULL_RTX;
}

static rtx
kvx_expand_builtin_scall (rtx target ATTRIBUTE_UNUSED, tree args)
{
  int nb_args = call_expr_nargs (args) - 1;
  if (nb_args > 11)
    error ("%<__builtin_scall%> can't have more than 11 arguments.");

  rtx scall_no = expand_normal (CALL_EXPR_ARG (args, 0));

  if (GET_CODE (scall_no) != CONST_INT)
    {
      scall_no = force_reg (SImode, scall_no);
      emit_insn (gen_rtx_USE (DImode, scall_no));
      emit_insn (gen_rtx_CLOBBER (DImode, scall_no));
    }
  for (int i = 0; i < nb_args; ++i)
    {
      rtx argi = expand_normal (CALL_EXPR_ARG (args, i + 1));
      machine_mode modi = GET_MODE (argi);
      rtx regi = gen_rtx_REG (modi == VOIDmode ? SImode : modi, i);
      emit_move_insn (regi, argi);
    }
  for (int i = 0; i < nb_args; ++i)
    {
      rtx regi = gen_rtx_REG (DImode, i);
      emit_insn (gen_rtx_USE (DImode, regi));
      emit_insn (gen_rtx_CLOBBER (DImode, regi));
    }
  emit_insn (gen_kvx_scall (scall_no));
  for (int i = 0; i < nb_args + 1; ++i)
    {
      rtx regi = gen_rtx_REG (DImode, i);
      emit_insn (gen_rtx_CLOBBER (DImode, regi));
    }
  emit_insn (gen_nop_volatile ());
  return gen_rtx_REG (DImode, 0);
}

void
kvx_expand_builtin_maddt (rtx operands[], int add)
{
  operands[0] = simplify_gen_subreg (TImode, operands[0], V2DImode, 0);
  operands[3] = simplify_gen_subreg (TImode, operands[3], V2DImode, 0);
  const char *xstr = XSTR (operands[4], 0);
  rtx pattern = NULL_RTX;

  if (!*xstr)
    {
      rtx mulx = gen_rtx_MULT (TImode, gen_rtx_SIGN_EXTEND (TImode, operands[1]),
				       gen_rtx_SIGN_EXTEND (TImode, operands[2]));
      pattern = add ? gen_rtx_PLUS (TImode, mulx, operands[3]) :
		      gen_rtx_MINUS (TImode, operands[3], mulx);
    }
  else if (xstr[1] == 'u' && xstr[2] == 0)
    {
      rtx mulx = gen_rtx_MULT (TImode, gen_rtx_ZERO_EXTEND (TImode, operands[1]),
				       gen_rtx_ZERO_EXTEND (TImode, operands[2]));
      pattern = add ? gen_rtx_PLUS (TImode, mulx, operands[3]) :
		      gen_rtx_MINUS (TImode, operands[3], mulx);
    }
  else if (xstr[1] == 'u' && xstr[2] == 'z')
    {
      rtx mulx = gen_rtx_MULT (TImode, gen_rtx_ZERO_EXTEND (TImode, operands[1]),
				       gen_rtx_ZERO_EXTEND (TImode, operands[2]));
      rtx sl64 = gen_rtx_LSHIFTRT (TImode, operands[3], GEN_INT (64));
      pattern = add ? gen_rtx_PLUS (TImode, mulx, sl64) :
		      gen_rtx_MINUS (TImode, sl64, mulx);
    }
  else if (xstr[1] == 's' && xstr[2] == 'u')
    {
      rtx mulx = gen_rtx_MULT (TImode, gen_rtx_ZERO_EXTEND (TImode, operands[2]),
				       gen_rtx_SIGN_EXTEND (TImode, operands[1]));
      pattern = add ? gen_rtx_PLUS (TImode, mulx, operands[3]) :
		      gen_rtx_MINUS (TImode, operands[3], mulx);
    }
  else
    gcc_unreachable ();

  emit_insn (gen_rtx_SET (operands[0], pattern));
}

void
kvx_expand_builtin_fmuldc (rtx operands[], int lanes)
{
  rtx modifiers = operands[3];
  machine_mode mode = GET_MODE (operands[0]);
  bool conjugate = kvx_modifier_enabled_p (".c", modifiers);
  if (conjugate)
    modifiers = kvx_modifier_rounding (modifiers);
  for (int i = 0; i < lanes; i++)
    {
      rtx real_0 = simplify_gen_subreg (DFmode, operands[0], mode, i*16+0);
      rtx imag_0 = simplify_gen_subreg (DFmode, operands[0], mode, i*16+8);
      rtx real_1 = simplify_gen_subreg (DFmode, operands[1], mode, i*16+0);
      rtx imag_1 = simplify_gen_subreg (DFmode, operands[1], mode, i*16+8);
      rtx real_2 = simplify_gen_subreg (DFmode, operands[2], mode, i*16+0);
      rtx imag_2 = simplify_gen_subreg (DFmode, operands[2], mode, i*16+8);
      rtx real_t = gen_reg_rtx (DFmode), imag_t = gen_reg_rtx (DFmode);
      if (conjugate)
	{
	  emit_insn (gen_kvx_fmuld (real_t, real_1, real_2, modifiers));
	  emit_insn (gen_kvx_ffmad (real_0, imag_1, imag_2, real_t, modifiers));
	  emit_insn (gen_kvx_fmuld (imag_t, real_1, imag_2, modifiers));
	  emit_insn (gen_kvx_ffmsd (imag_0, real_2, imag_1, imag_t, modifiers));
	}
      else
	{
	  emit_insn (gen_kvx_fmuld (real_t, real_1, real_2, modifiers));
	  emit_insn (gen_kvx_ffmsd (real_0, imag_1, imag_2, real_t, modifiers));
	  emit_insn (gen_kvx_fmuld (imag_t, real_1, imag_2, modifiers));
	  emit_insn (gen_kvx_ffmad (imag_0, real_2, imag_1, imag_t, modifiers));
	}
    }
}


#define KVX_EXPAND_BUILTIN_LOADU(name, name2, tmode, mmode)                    \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    arg1 = gen_rtx_MEM (mmode, force_reg (Pmode, arg1));                       \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_##name2 (target, arg1));                                    \
    return target;                                                             \
  }

static rtx
kvx_expand_builtin_fence (rtx ARG_UNUSED (target), tree args)
{
  int nargs = call_expr_nargs (args);
  rtx arg1 = nargs >= 1?
	     build_arg (CALL_EXPR_ARG (args, 0), "fence", kvx_modmap_accesses):
	     gen_rtx_CONST_STRING (VOIDmode, "");
  emit_insn (gen_kvx_fence (arg1));
  return NULL_RTX;
}

static rtx
kvx_expand_builtin_ready (rtx target, tree args)
{
  rtx arguments[4];
  int nargs = call_expr_nargs (args);
  for (int i = 0; i < nargs; i++)
    {
      arguments[i] = expand_normal (CALL_EXPR_ARG (args, i));
      machine_mode mode = GET_MODE (arguments[i]);
      arguments[i] = force_reg (mode, arguments[i]);
      arguments[i] = simplify_gen_subreg (DImode, arguments[i], mode, 0);
    }
  if (!target)
    target = gen_reg_rtx (SImode);
  else
    target = force_reg (SImode, target);
  rtvec vec = 0;
  if (nargs == 0)
    return target;
  if (nargs == 1)
    vec = gen_rtvec (1, arguments[0]);
  else if (nargs == 2)
    vec = gen_rtvec (2, arguments[0], arguments[1]);
  else if (nargs == 3)
    vec = gen_rtvec (3, arguments[0], arguments[1], arguments[2]);
  else if (nargs == 4)
    vec = gen_rtvec (4, arguments[0], arguments[1], arguments[2], arguments[3]);
  else
    error ("%<__builtin_kvx_ready%> must have one to four arguments");
  emit_insn (gen_rtx_SET (target, gen_rtx_UNSPEC (SImode, vec, UNSPEC_READY)));
  return target;
}


#define KVX_EXPAND_BUILTIN_ALOAD(name, name2, tmode, mmode)                    \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    int nargs = call_expr_nargs (args);                                        \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = nargs >= 2?                                                     \
               build_arg (CALL_EXPR_ARG (args, 1), #name, kvx_modmap_coherency):\
               gen_rtx_CONST_STRING (VOIDmode, "");                            \
    arg1 = gen_rtx_MEM (mmode, force_reg (Pmode, arg1));                       \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_##name2 (target, arg1, arg2));                              \
    return target;                                                             \
  }


#define KVX_EXPAND_BUILTIN_ALADD(name, name2, tmode, mmode)                    \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    int nargs = call_expr_nargs (args);                                        \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = nargs >= 3?                                                     \
               build_arg (CALL_EXPR_ARG (args, 2), #name, kvx_modmap_coherency):\
               gen_rtx_CONST_STRING (VOIDmode, "");                            \
    arg1 = gen_rtx_MEM (mmode, force_reg (Pmode, arg1));                       \
    arg2 = force_reg (tmode, arg2);                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_##name2 (target, arg1, arg2, arg3));                        \
    return target;                                                             \
  }


#define KVX_EXPAND_BUILTIN_ACSWAP(name, name2, tmode, mmode)                   \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    int nargs = call_expr_nargs (args);                                        \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    rtx arg4 = nargs >= 4?                                                     \
               build_arg (CALL_EXPR_ARG (args, 3), #name, kvx_modmap_ ## boolcasco):\
               gen_rtx_CONST_STRING (VOIDmode, "");                            \
    rtx arg5 = const0_rtx;                                                     \
    if (nargs > 4)                                                             \
      {                                                                        \
        arg5 = expand_normal (CALL_EXPR_ARG (args, 4));                        \
        arg5 = verify_const_ready_arg (arg5, #name, "fifth");                  \
      }                                                                        \
    arg1 = gen_rtx_MEM (mmode, force_reg (Pmode, arg1));                       \
    rtx new_old = gen_reg_rtx (TImode);                                        \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_move_insn (gen_rtx_SUBREG (tmode, new_old, 0), arg2);                 \
    emit_move_insn (gen_rtx_SUBREG (tmode, new_old, 8), arg3);                 \
    emit_insn (gen_##name2 (new_old, arg1, arg4, arg5));                       \
    rtx result = gen_lowpart_SUBREG (tmode, new_old);                          \
    emit_move_insn (target, result);                                           \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_ASTORE(name, name2, tmode, mmode)                   \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    int nargs = call_expr_nargs (args);                                        \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = nargs >= 3?                                                     \
               build_arg (CALL_EXPR_ARG (args, 2), #name, kvx_modmap_coherency):\
               gen_rtx_CONST_STRING (VOIDmode, "");                            \
    arg1 = force_reg (tmode, arg1);                                            \
    arg2 = gen_rtx_MEM (mmode, force_reg (Pmode, arg2));                       \
    emit_insn (gen_##name2 (arg1, arg2, arg3));                                \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_LOAD(name, name2, tmode, mmode)                     \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    int nargs = call_expr_nargs (args), volatile_p = 0;                        \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2;                                                                  \
    if (#name[0] == 'x')                                                       \
      arg2 = build_arg (CALL_EXPR_ARG (args, 1), #name, kvx_modmap_xvariant);  \
    else                                                                       \
      arg2 = build_arg (CALL_EXPR_ARG (args, 1), #name, kvx_modmap_variant);   \
    arg1 = gen_rtx_MEM (mmode, force_reg (Pmode, arg1));                       \
    if (nargs > 2)                                                             \
      {                                                                        \
        rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                    \
        arg3 = verify_const_bool_arg (arg3, #name, "third");                   \
        volatile_p |= (INTVAL (arg3) != 0);                                    \
      }                                                                        \
    if (GET_MODE_SIZE (mmode) > 32 && volatile_p)                              \
      error ("volatile not allowed for memory access sizes greater than 32");  \
    MEM_VOLATILE_P (arg1) = volatile_p;                                        \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_##name2 (target, arg1, arg2));                              \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_LOADQ(name, name2, tmode, mmode)                    \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    int nargs = call_expr_nargs (args), volatile_p = 0;                        \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = build_arg (CALL_EXPR_ARG (args, 2), #name, kvx_modmap_variant); \
    arg1 = force_reg (tmode, arg1);                                            \
    arg2 = gen_rtx_MEM (mmode, force_reg (Pmode, arg2));                       \
    if (nargs > 3)                                                             \
      {                                                                        \
        rtx arg4 = expand_normal (CALL_EXPR_ARG (args, 3));                    \
        arg4 = verify_const_bool_arg (arg4, #name, "fourth");                  \
        volatile_p |= (INTVAL (arg4) != 0);                                    \
      }                                                                        \
    if (GET_MODE_SIZE (mmode) > 32 && volatile_p)                              \
      error ("volatile not allowed for memory access sizes greater than 32");  \
    MEM_VOLATILE_P (arg2) = volatile_p;                                        \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_##name2 (target, arg1, arg2, arg3));                        \
    return target;                                                             \
  }


#define KVX_EXPAND_BUILTIN_LOADC(name, name2, tmode, mmode)                    \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    int nargs = call_expr_nargs (args), volatile_p = 0, clear = 0;             \
    machine_mode cmode = GET_MODE_SIZE (mmode) > 64 ? TImode : DImode;         \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    rtx arg4 = NULL_RTX;                                                       \
    const char *modifier = kvx_tree_string_constant (CALL_EXPR_ARG (args, 3), #name); \
    if (#name[0] == 'x')                                                       \
      arg4 = build_arg (CALL_EXPR_ARG (args, 3), #name, kvx_modmap_xloadcond); \
    else                                                                       \
      arg4 = build_arg (CALL_EXPR_ARG (args, 3), #name, kvx_modmap_loadcond);  \
    arg1 = force_reg (tmode, arg1);                                            \
    arg2 = gen_rtx_MEM (mmode, force_reg (Pmode, arg2));                       \
    arg3 = force_reg (cmode, arg3);                                            \
    if (nargs > 4)                                                             \
      {                                                                        \
        rtx arg5 = expand_normal (CALL_EXPR_ARG (args, 4));                    \
        arg5 = verify_const_bool_arg (arg5, #name, "fifth");                   \
        volatile_p |= (INTVAL (arg5) != 0);                                    \
      }                                                                        \
    if (GET_MODE_SIZE (mmode) > 32 && volatile_p)                              \
      error ("volatile not allowed for memory access sizes greater than 32");  \
    MEM_VOLATILE_P (arg2) = volatile_p;                                        \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    if (clear && (strstr (modifier, ".mtc") || strstr (modifier, ".mfc")))    \
      clear = true;                                                           \
    if (clear && !strstr (#name, "1024q"))                                     \
      arg1 = CONST0_RTX (tmode);                                               \
    emit_insn (gen_##name2 (target, arg1, arg2, arg3, arg4));               \
    return target;                                                             \
  }

#define KVX_EXPAND_BUILTIN_STORE(name, name2, tmode, mmode)                    \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    int nargs = call_expr_nargs (args), volatile_p = 0;                        \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = const0_rtx;                                                     \
    arg1 = force_reg (tmode, arg1);                                            \
    arg2 = gen_rtx_MEM (mmode, force_reg (Pmode, arg2));                       \
    if (nargs > 2)                                                             \
      {                                                                        \
        arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
        arg3 = verify_const_ready_arg (arg3, #name, "third");                  \
        if (GET_CODE (arg3) == CONST_INT)                                      \
          volatile_p |= (INTVAL (arg3) != 0);                                  \
      }                                                                        \
    if (nargs > 3)                                                             \
      {                                                                        \
        rtx arg4 = expand_normal (CALL_EXPR_ARG (args, 3));                    \
        arg4 = verify_const_bool_arg (arg4, #name, "fourth");                  \
        volatile_p |= (INTVAL (arg4) != 0);                                    \
      }                                                                        \
    if (GET_MODE_SIZE (mmode) > 32 && volatile_p)                              \
      error ("volatile not allowed for memory access sizes greater than 32");  \
    MEM_VOLATILE_P (arg2) = volatile_p;                                        \
    emit_insn (gen_##name2 (arg1, arg2, arg3));                                \
    return target;                                                             \
  }


#define KVX_EXPAND_BUILTIN_STOREC(name, name2, tmode, mmode)                   \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    int nargs = call_expr_nargs (args), volatile_p = 0;                        \
    machine_mode cmode = GET_MODE_SIZE (mmode) > 64 ? TImode : DImode;         \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    rtx arg4 = build_arg (CALL_EXPR_ARG (args, 3), #name, kvx_modmap_storecond);\
    rtx arg5 = const0_rtx;                                                     \
    arg1 = force_reg (tmode, arg1);                                            \
    arg2 = gen_rtx_MEM (mmode, force_reg (Pmode, arg2));                       \
    arg3 = force_reg (cmode, arg3);                                            \
    if (nargs > 4)                                                             \
      {                                                                        \
        arg5 = expand_normal (CALL_EXPR_ARG (args, 4));                        \
        arg5 = verify_const_ready_arg (arg5, #name, "fifth");                  \
        if (GET_CODE (arg5) == CONST_INT)                                      \
          volatile_p |= (INTVAL (arg5) != 0);                                  \
      }                                                                        \
    if (nargs > 5)                                                             \
      {                                                                        \
        rtx arg6 = expand_normal (CALL_EXPR_ARG (args, 5));                    \
        arg6 = verify_const_bool_arg (arg6, #name, "sixth");                   \
        volatile_p |= (INTVAL (arg6) != 0);                                    \
      }                                                                        \
    if (GET_MODE_SIZE (mmode) > 32 && volatile_p)                              \
      error ("volatile not allowed for memory access sizes greater than 32");  \
    MEM_VOLATILE_P (arg2) = volatile_p;                                        \
    emit_insn (gen_kvx_##name (arg1, arg2, arg3, arg4, arg5));                 \
    return target;                                                             \
  }


#define KVX_EXPAND_BUILTIN_XPRELOAD(name, name2, bmode, mmode)                 \
  static rtx kvx_expand_builtin_##name (rtx target, tree args)                 \
  {                                                                            \
    int nargs = call_expr_nargs (args), volatile_p = 0;                        \
    rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));                        \
    rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));                        \
    rtx arg3 = expand_normal (CALL_EXPR_ARG (args, 2));                        \
    rtx arg4 = build_arg (CALL_EXPR_ARG (args, 3), #name, kvx_modmap_xvariant);\
    arg1 = force_reg (bmode, arg1);                                            \
    arg2 = gen_rtx_MEM (mmode, force_reg (Pmode, arg2));                       \
    arg3 = force_reg (DImode, arg3);                                           \
    if (nargs > 4)                                                             \
      {                                                                        \
        rtx arg5 = expand_normal (CALL_EXPR_ARG (args, 4));                    \
        arg5 = verify_const_bool_arg (arg5, #name, "fifth");                   \
        volatile_p |= (INTVAL (arg5) != 0);                                    \
      }                                                                        \
    MEM_VOLATILE_P (arg2) = volatile_p;                                        \
    if (!target)                                                               \
      target = gen_reg_rtx (bmode);                                            \
    else                                                                       \
      target = force_reg (bmode, target);                                      \
    emit_insn (gen_##name2 (target, arg1, arg2, arg3, arg4));                  \
    return target;                                                             \
  }


#define KVX_EXPAND_BUILTIN_XUNDEF(name, name2, tmode)                          \
  static rtx kvx_expand_builtin_##name (rtx target, tree args ATTRIBUTE_UNUSED)\
  {                                                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    rtx arg1 = GEN_INT (kvx_xundef_counter++);                                 \
    emit_insn (gen_##name2 (target, arg1));                                    \
    return target;                                                             \
  }


#define KVX_EXPAND_BUILTIN_XZERO(name, name2, tmode)                           \
  static rtx kvx_expand_builtin_##name (rtx target, tree args ATTRIBUTE_UNUSED)\
  {                                                                            \
    if (!target)                                                               \
      target = gen_reg_rtx (tmode);                                            \
    else                                                                       \
      target = force_reg (tmode, target);                                      \
    emit_insn (gen_##name2 (target, CONST0_RTX (tmode)));                      \
    return target;                                                             \
  }


static rtx
kvx_expand_builtin_xswapo256 (rtx target, tree args)
{
  machine_mode bmode = X256mode;
  machine_mode tmode = V256mode;
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  arg1 = gen_rtx_MEM (bmode, force_reg (Pmode, arg1));
  arg2 = force_reg (tmode, arg2);
  if (!target)
    target = gen_reg_rtx (tmode);
  else
    target = force_reg (tmode, target);
  emit_insn (gen_kvx_xswapo256 (target, arg1, arg2));
  return target;
}

static inline rtx
kvx_expand_builtin_xswap256 (rtx target, tree args)
{
  return kvx_expand_builtin_xswapo256 (target, args);
}

static rtx
kvx_expand_builtin_xswapq256 (rtx target, tree args)
{
  machine_mode bmode = X256mode;
  machine_mode tmode = V2DImode;
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  rtx arg3 = build_arg (CALL_EXPR_ARG (args, 2), "xswapq256", kvx_modmap_xhalf);
  arg1 = gen_rtx_MEM (bmode, force_reg (Pmode, arg1));
  arg2 = force_reg (tmode, arg2);
  if (!target)
    target = gen_reg_rtx (tmode);
  else
    target = force_reg (tmode, target);
  emit_insn (gen_kvx_xswapq256 (target, arg1, arg2, arg3));
  return target;
}

static rtx
kvx_expand_builtin_xswapd256 (rtx target, tree args)
{
  machine_mode bmode = X256mode;
  machine_mode tmode = DImode;
  rtx arg1 = expand_normal (CALL_EXPR_ARG (args, 0));
  rtx arg2 = expand_normal (CALL_EXPR_ARG (args, 1));
  rtx arg3 = build_arg (CALL_EXPR_ARG (args, 2), "xswapd256", kvx_modmap_xquarter);
  arg1 = gen_rtx_MEM (bmode, force_reg (Pmode, arg1));
  arg2 = force_reg (tmode, arg2);
  if (!target)
    target = gen_reg_rtx (tmode);
  else
    target = force_reg (tmode, target);
  emit_insn (gen_kvx_xswapd256 (target, arg1, arg2, arg3));
  return target;
}

#define KVX_BUILTIN_DIRECT_(NAME, FTYPE, OPTIONALS, AVAIL)
#define KVX_BUILTIN_DIRECT_INSN_(NAME, INSN, FTYPE, OPTIONALS, AVAIL)
#define KVX_BUILTIN_INDIRECT_(NAME, FTYPE, OPTIONALS, AVAIL)
#define KVX_BUILTIN_INDIRECT_INSN_CUSTOM_ARGS(NAME, INSN, ...) (NAME, INSN, __VA_ARGS__)
#define KVX_BUILTIN_INDIRECT_INSN_CUSTOM_(NAME, INSN, FTYPE, OPTIONALS, AVAIL, CUSTOM) \
  EXPAND(DEFER(CONCAT2 (KVX_EXPAND_BUILTIN_, KVX_FST CUSTOM) \
               KVX_BUILTIN_INDIRECT_INSN_CUSTOM_ARGS(NAME, INSN, KVX_RST CUSTOM)))
#define KVX_BUILTIN_INDIRECT_CUSTOM_ARGS(NAME, ...) (NAME, CONCAT2 (kvx_, NAME), __VA_ARGS__)
#define KVX_BUILTIN_INDIRECT_CUSTOM_(NAME, FTYPE, OPTIONALS, AVAIL, CUSTOM) \
  EXPAND(DEFER(CONCAT2 (KVX_EXPAND_BUILTIN_, KVX_FST CUSTOM) \
	       KVX_BUILTIN_INDIRECT_CUSTOM_ARGS(NAME, KVX_RST CUSTOM)))

#define KVX_BUILTIN_DIRECT(...) \
  GET_MACRO(__VA_ARGS__,, \
      KVX_BUILTIN_DIRECT_INSN_, \
      KVX_BUILTIN_DIRECT_,,,)(__VA_ARGS__)
#define KVX_BUILTIN_INDIRECT(...) \
  GET_MACRO(__VA_ARGS__, \
      KVX_BUILTIN_INDIRECT_INSN_CUSTOM_, \
      KVX_BUILTIN_INDIRECT_CUSTOM_, \
      KVX_BUILTIN_INDIRECT_,,,)(__VA_ARGS__)
#include "config/kvx/kvx-builtins.def"
#undef KVX_BUILTIN_DIRECT_
#undef KVX_BUILTIN_DIRECT_INSN_
#undef KVX_BUILTIN_INDIRECT_
#undef KVX_BUILTIN_INDIRECT_CUSTOM_ARGS
#undef KVX_BUILTIN_INDIRECT_CUSTOM_
#undef KVX_BUILTIN_INDIRECT_INSN_CUSTOM_ARGS
#undef KVX_BUILTIN_INDIRECT_INSN_CUSTOM_
#undef KVX_BUILTIN_DIRECT
#undef KVX_BUILTIN_INDIRECT

#define KVX_BUILTIN_DIRECT_(NAME, FTYPE, OPTIONALS, AVAIL) \
  { "__builtin_kvx_" #NAME, CODE_FOR_kvx_ ## NAME, KVX_BLTN_DIRECT, \
    KVX_ ## FTYPE, NULL, !ISEMPTY(OPTIONALS), \
    CONCAT3(KVX_,FTYPE,KVX_GET(OPTIONALS)), kvx_builtin_avail_ ## AVAIL },

#define KVX_BUILTIN_DIRECT_INSN_(NAME, INSN, FTYPE, OPTIONALS, AVAIL) \
  { "__builtin_kvx_" #NAME, CODE_FOR_ ## INSN, KVX_BLTN_DIRECT, \
    KVX_ ## FTYPE, NULL, !ISEMPTY(OPTIONALS), \
    CONCAT3(KVX_,FTYPE,KVX_GET(OPTIONALS)), kvx_builtin_avail_ ## AVAIL },

#define KVX_BUILTIN_INDIRECT_(NAME, FTYPE, OPTIONALS, AVAIL) \
  { "__builtin_kvx_" #NAME, CODE_FOR_nothing, KVX_BLTN_INDIRECT, \
    KVX_ ## FTYPE, kvx_expand_builtin_ ## NAME, !ISEMPTY(OPTIONALS), \
    CONCAT3(KVX_,FTYPE,KVX_GET(OPTIONALS)), kvx_builtin_avail_ ## AVAIL },

#define KVX_BUILTIN_INDIRECT_CUSTOM_(NAME, FTYPE, OPTIONALS, AVAIL, CUSTOM) \
  { "__builtin_kvx_" #NAME, CODE_FOR_nothing, KVX_BLTN_INDIRECT, \
    KVX_ ## FTYPE, kvx_expand_builtin_ ## NAME, !ISEMPTY(OPTIONALS), \
    CONCAT3(KVX_,FTYPE,KVX_GET(OPTIONALS)), kvx_builtin_avail_ ## AVAIL },

#define KVX_BUILTIN_INDIRECT_INSN_CUSTOM_(NAME, INSN, FTYPE, OPTIONALS, AVAIL, CUSTOM) \
  { "__builtin_kvx_" #NAME, CODE_FOR_nothing, KVX_BLTN_INDIRECT, \
    KVX_ ## FTYPE, kvx_expand_builtin_ ## NAME, !ISEMPTY(OPTIONALS), \
    CONCAT3(KVX_,FTYPE,KVX_GET(OPTIONALS)), kvx_builtin_avail_ ## AVAIL },

#define KVX_BUILTIN_DIRECT(...) \
  GET_MACRO(__VA_ARGS__,, \
      KVX_BUILTIN_DIRECT_INSN_, \
      KVX_BUILTIN_DIRECT_,,,) (__VA_ARGS__)
#define KVX_BUILTIN_INDIRECT(...) \
  GET_MACRO(__VA_ARGS__, \
      KVX_BUILTIN_INDIRECT_INSN_CUSTOM_, \
      KVX_BUILTIN_INDIRECT_CUSTOM_, \
      KVX_BUILTIN_INDIRECT_,,,)(__VA_ARGS__)

static const struct kvx_builtin_desc kvx_builtins[] = {
#include "config/kvx/kvx-builtins.def"
};
#undef KVX_BUILTIN_DIRECT
#undef KVX_BUILTIN_DIRECT_INSN
#undef KVX_BUILTIN_INDIRECT
#undef KVX_BUILTIN_INDIRECT_CUSTOM


static tree kvx_builtin_decls[ARRAY_SIZE (kvx_builtins)];
//static GTY(()) int kvx_builtin_decl_index[NUM_INSN_CODES];

void
kvx_init_builtins (void)
{
  KVX_ATYPE_STRING = build_pointer_type (
    build_qualified_type (char_type_node, TYPE_QUAL_CONST));

  KVX_ATYPE_V8QI  = build_opaque_vector_type (KVX_ATYPE_INT8, 8);
  KVX_ATYPE_V16QI = build_opaque_vector_type (KVX_ATYPE_INT8, 16);
  KVX_ATYPE_V32QI = build_opaque_vector_type (KVX_ATYPE_INT8, 32);
  KVX_ATYPE_V64QI = build_opaque_vector_type (KVX_ATYPE_INT8, 64);

  KVX_ATYPE_V4HI  = build_opaque_vector_type (KVX_ATYPE_INT16, 4);
  KVX_ATYPE_V8HI  = build_opaque_vector_type (KVX_ATYPE_INT16, 8);
  KVX_ATYPE_V16HI = build_opaque_vector_type (KVX_ATYPE_INT16, 16);
  KVX_ATYPE_V32HI = build_opaque_vector_type (KVX_ATYPE_INT16, 32);

  KVX_ATYPE_V2SI  = build_opaque_vector_type (KVX_ATYPE_INT32, 2);
  KVX_ATYPE_V4SI  = build_opaque_vector_type (KVX_ATYPE_INT32, 4);
  KVX_ATYPE_V8SI  = build_opaque_vector_type (KVX_ATYPE_INT32, 8);
  KVX_ATYPE_V16SI = build_opaque_vector_type (KVX_ATYPE_INT32, 16);

  KVX_ATYPE_V2DI  = build_opaque_vector_type (KVX_ATYPE_INT64, 2);
  KVX_ATYPE_V4DI  = build_opaque_vector_type (KVX_ATYPE_INT64, 4);
  KVX_ATYPE_V8DI  = build_opaque_vector_type (KVX_ATYPE_INT64, 8);

  KVX_ATYPE_V4HF  = build_opaque_vector_type (KVX_ATYPE_FLOAT16, 4);
  KVX_ATYPE_V8HF  = build_opaque_vector_type (KVX_ATYPE_FLOAT16, 8);
  KVX_ATYPE_V16HF = build_opaque_vector_type (KVX_ATYPE_FLOAT16, 16);
  KVX_ATYPE_V32HF = build_opaque_vector_type (KVX_ATYPE_FLOAT16, 32);

  KVX_ATYPE_V2SF  = build_opaque_vector_type (KVX_ATYPE_FLOAT32, 2);
  KVX_ATYPE_V4SF  = build_opaque_vector_type (KVX_ATYPE_FLOAT32, 4);
  KVX_ATYPE_V8SF  = build_opaque_vector_type (KVX_ATYPE_FLOAT32, 8);
  KVX_ATYPE_V16SF = build_opaque_vector_type (KVX_ATYPE_FLOAT32, 16);

  KVX_ATYPE_V2DF  = build_opaque_vector_type (KVX_ATYPE_FLOAT64, 2);
  KVX_ATYPE_V4DF  = build_opaque_vector_type (KVX_ATYPE_FLOAT64, 4);
  KVX_ATYPE_V8DF  = build_opaque_vector_type (KVX_ATYPE_FLOAT64, 8);

  add_builtin_type ("__kvx_v8qi", KVX_ATYPE_V8QI);
  add_builtin_type ("__kvx_v16qi", KVX_ATYPE_V16QI);
  add_builtin_type ("__kvx_v32qi", KVX_ATYPE_V32QI);
  add_builtin_type ("__kvx_v64qi", KVX_ATYPE_V64QI);

  add_builtin_type ("__kvx_v4hi", KVX_ATYPE_V4HI);
  add_builtin_type ("__kvx_v8hi", KVX_ATYPE_V8HI);
  add_builtin_type ("__kvx_v16hi", KVX_ATYPE_V16HI);
  add_builtin_type ("__kvx_v32hi", KVX_ATYPE_V32HI);

  add_builtin_type ("__kvx_v2si", KVX_ATYPE_V2SI);
  add_builtin_type ("__kvx_v4si", KVX_ATYPE_V4SI);
  add_builtin_type ("__kvx_v8si", KVX_ATYPE_V8SI);
  add_builtin_type ("__kvx_v16si", KVX_ATYPE_V16SI);

  add_builtin_type ("__kvx_v2di", KVX_ATYPE_V2DI);
  add_builtin_type ("__kvx_v4di", KVX_ATYPE_V4DI);
  add_builtin_type ("__kvx_v8di", KVX_ATYPE_V8DI);

  add_builtin_type ("__kvx_v4hf", KVX_ATYPE_V4HF);
  add_builtin_type ("__kvx_v8hf", KVX_ATYPE_V8HF);
  add_builtin_type ("__kvx_v16hf", KVX_ATYPE_V16HF);
  add_builtin_type ("__kvx_v32hf", KVX_ATYPE_V32HF);

  add_builtin_type ("__kvx_v2sf", KVX_ATYPE_V2SF);
  add_builtin_type ("__kvx_v4sf", KVX_ATYPE_V4SF);
  add_builtin_type ("__kvx_v8sf", KVX_ATYPE_V8SF);
  add_builtin_type ("__kvx_v16sf", KVX_ATYPE_V16SF);

  add_builtin_type ("__kvx_v2df", KVX_ATYPE_V2DF);
  add_builtin_type ("__kvx_v4df", KVX_ATYPE_V4DF);
  add_builtin_type ("__kvx_v8df", KVX_ATYPE_V8DF);

  KVX_ATYPE_DI = make_unsigned_type (64);
  KVX_ATYPE_V64  = build_opaque_vector_type (KVX_ATYPE_DI, 1);
  KVX_ATYPE_V128 = build_opaque_vector_type (KVX_ATYPE_DI, 2);
  KVX_ATYPE_V256 = build_opaque_vector_type (KVX_ATYPE_DI, 4);
  KVX_ATYPE_V512 = build_opaque_vector_type (KVX_ATYPE_DI, 8);

  add_builtin_type ("__kvx_v64", KVX_ATYPE_V64);
  add_builtin_type ("__kvx_v128", KVX_ATYPE_V128);
  add_builtin_type ("__kvx_v256", KVX_ATYPE_V256);
  add_builtin_type ("__kvx_v512", KVX_ATYPE_V512);

  KVX_ATYPE_OI = make_unsigned_type (256);
  SET_TYPE_ALIGN (KVX_ATYPE_OI, 256);
  SET_TYPE_MODE (KVX_ATYPE_OI, OImode);
  layout_type (KVX_ATYPE_OI);
  lang_hooks.types.register_builtin_type (KVX_ATYPE_OI, "__oi");

  /* Support for _Float16.  */
  tree kvx_float16_type_node = make_node (REAL_TYPE);
  TYPE_PRECISION (kvx_float16_type_node) = 16;
  SET_TYPE_MODE (kvx_float16_type_node, HFmode);
  layout_type (kvx_float16_type_node);

  if (!maybe_get_identifier ("_Float16"))
    lang_hooks.types.register_builtin_type (kvx_float16_type_node,
					    "_Float16");

  KVX_ATYPE_X256  = build_vector_type (KVX_ATYPE_OI, 1);
  KVX_ATYPE_X512  = build_vector_type (KVX_ATYPE_OI, 2);
  KVX_ATYPE_X1024 = build_vector_type (KVX_ATYPE_OI, 4);
  KVX_ATYPE_X2048 = build_vector_type (KVX_ATYPE_OI, 8);
  KVX_ATYPE_X4096 = build_vector_type (KVX_ATYPE_OI, 16);
  KVX_ATYPE_X8192 = build_vector_type (KVX_ATYPE_OI, 32);

  KVX_ATYPE_PX256 = build_pointer_type (KVX_ATYPE_X256);

  add_builtin_type ("__kvx_x256", KVX_ATYPE_X256);
  add_builtin_type ("__kvx_x512", KVX_ATYPE_X512);
  add_builtin_type ("__kvx_x1024", KVX_ATYPE_X1024);
  add_builtin_type ("__kvx_x2048", KVX_ATYPE_X2048);
  add_builtin_type ("__kvx_x4096", KVX_ATYPE_X4096);
  add_builtin_type ("__kvx_x8192", KVX_ATYPE_X8192);

  for (int i = 0; i < (int) ARRAY_SIZE (kvx_builtins); ++i)
    {
      const struct kvx_builtin_desc *d = &kvx_builtins[i];
      if (d->avail ())
        {
          tree type = kvx_build_function_type (d->prototype, d->variadic_p);
          kvx_builtin_decls[i] = add_builtin_function (d->name, type, i, BUILT_IN_MD, NULL, NULL);
          //	kvx_builtin_decl_index[d->icode] = i;
        }
    }
  if (KV3_1)
    {
      kvx_modmap_loadcond  = &kvx_modmap_loadcond_kv3_v1;
      kvx_modmap_xloadcond = &kvx_modmap_xloadcond_kv3_v1;
      kvx_modmap_storecond = &kvx_modmap_storecond_kv3_v1;
      kvx_modmap_silent    = &kvx_modmap_silent_kv3;
      kvx_modmap_xvariant  = &kvx_modmap_xvariant_kv3_v1;
      kvx_modmap_xloadq    = &kvx_modmap_xloadq_kv3_v1;
      kvx_modmap_boolcasco = &kvx_modmap_boolcasco_kv3_v1;
      kvx_modmap_accesses  = &kvx_modmap_accesses_kv3_v1;
      kvx_modmap_cachelev  = &kvx_modmap_cachelev_kv3_v1;
      kvx_modmap_xloadcond = &kvx_modmap_xloadcond_kv3_v1;
      kvx_modmap_xloadqc   = &kvx_modmap_xloadqc_kv3_v1;
      kvx_modmap_floatings = &kvx_modmap_floatings_kv3;
      kvx_modmap_conjugate = &kvx_modmap_conjugate_kv3;
    }
  else if (KV3_2)
    {
      kvx_modmap_loadcond  = &kvx_modmap_loadcond_kv3_v2;
      kvx_modmap_xloadcond = &kvx_modmap_xloadcond_kv3_v2;
      kvx_modmap_storecond = &kvx_modmap_storecond_kv3_v2;
      kvx_modmap_silent    = &kvx_modmap_silent_kv3;
      kvx_modmap_xvariant  = &kvx_modmap_xvariant_kv3_v2;
      kvx_modmap_xloadq    = &kvx_modmap_xloadq_kv3_v2;
      kvx_modmap_boolcasco = &kvx_modmap_boolcasco_kv3_v2;
      kvx_modmap_accesses  = &kvx_modmap_accesses_kv3_v2;
      kvx_modmap_cachelev  = &kvx_modmap_cachelev_kv3_v2;
      kvx_modmap_xloadcond = &kvx_modmap_xloadcond_kv3_v2;
      kvx_modmap_xloadqc   = &kvx_modmap_xloadqc_kv3_v2;
      kvx_modmap_floatings = &kvx_modmap_floatings_kv3;
      kvx_modmap_conjugate = &kvx_modmap_conjugate_kv3;
    }
  else if (KV4_1 && !KV3_2)
    {
      kvx_modmap_loadcond  = &kvx_modmap_loadcond_kv3_v2;
      kvx_modmap_xloadcond = &kvx_modmap_xloadcond_kv3_v2;
      kvx_modmap_storecond = &kvx_modmap_storecond_kv3_v2;
      kvx_modmap_silent    = &kvx_modmap_silent_kv4;
      kvx_modmap_xvariant  = &kvx_modmap_xvariant_kv3_v2;
      kvx_modmap_xloadq    = &kvx_modmap_xloadq_kv3_v2;
      kvx_modmap_boolcasco = &kvx_modmap_boolcasco_kv3_v2;
      kvx_modmap_accesses  = &kvx_modmap_accesses_kv3_v2;
      kvx_modmap_cachelev  = &kvx_modmap_cachelev_kv3_v2;
      kvx_modmap_xloadcond = &kvx_modmap_xloadcond_kv3_v2;
      kvx_modmap_xloadqc   = &kvx_modmap_xloadqc_kv3_v2;
      kvx_modmap_floatings = &kvx_modmap_floatings_kv4;
      kvx_modmap_conjugate = &kvx_modmap_conjugate_kv4;
    }
  else
    internal_error ("unsupported architecture");
}

tree
kvx_builtin_decl (unsigned code, bool initialize_p ATTRIBUTE_UNUSED)
{
  if (code >= ARRAY_SIZE (kvx_builtins))
    return error_mark_node;
  return kvx_builtin_decls[code];
}


rtx
kvx_expand_builtin (tree exp, rtx target, rtx subtarget ATTRIBUTE_UNUSED,
		    enum machine_mode mode ATTRIBUTE_UNUSED,
		    int ignore ATTRIBUTE_UNUSED)
{
  tree fndecl = TREE_OPERAND (CALL_EXPR_FN (exp), 0);
  int nops = call_expr_nargs (exp);
  unsigned int fcode = DECL_FUNCTION_CODE (fndecl);
  int has_target_p = true;

//  int args_idx = 0;
//  tree t = NULL_TREE;
//  function_args_iterator it;
  const struct kvx_modmap *mod = kvx_get_modifiers (kvx_builtins[fcode].variadic_prototype);
  if (TREE_TYPE (TREE_TYPE (kvx_builtin_decls[fcode])) == KVX_ATYPE_VOID)
      has_target_p = false;
//  FOREACH_FUNCTION_ARGS (TREE_TYPE (kvx_builtin_decls[fcode]), t, it)
//  {
//
//    if (args_idx >= nops)
//      break;
//
//    tree cur_arg = CALL_EXPR_ARG (exp, args_idx);
//    if (TYPE_MODE (TREE_TYPE (cur_arg)) != TYPE_MODE (t))
//        error ("type error");
//
//    tree offset_tree = 0;
//    tree maybe_mod = string_constant (cur_arg, &offset_tree, 0, 0);
//    if (maybe_mod)
//      build_arg (cur_arg, kvx_builtins[fcode].name, mod);
//
//    args_idx += 1;
//  }

  if (kvx_builtins[fcode].type == KVX_BLTN_INDIRECT)
    return kvx_builtins[fcode].expand (target, exp);
  else if (kvx_builtins[fcode].type == KVX_BLTN_DIRECT)
    {
      struct expand_operand ops[MAX_RECOG_OPERANDS];
      int opno = 0;
      enum insn_code icode = kvx_builtins[fcode].icode;
      if (has_target_p)
	create_output_operand (&ops[opno++], target, TYPE_MODE (TREE_TYPE (exp)));

      nops += !!has_target_p;
      gcc_assert (opno + call_expr_nargs (exp)
		  == insn_data[icode].n_generator_args);
      for (int argno = 0; argno < call_expr_nargs (exp); argno++)
	{
	  tree arg = CALL_EXPR_ARG (exp, argno);
	  tree offset_tree = 0;
	  tree maybe_mod = string_constant (arg, &offset_tree, 0, 0);
	  rtx thing;
	  if (maybe_mod)
	    thing = build_arg (arg, kvx_builtins[fcode].name, mod);
	  else
	    thing = expand_normal (arg);

	  create_input_operand (&ops[opno++], thing, TYPE_MODE (TREE_TYPE (arg)));
	}
      maybe_expand_insn (icode, nops, ops);

      return has_target_p ? ops[0].value : const0_rtx;
    }
  else
    {
      gcc_unreachable ();
    }

  //internal_error ("unknown builtin code");
  return NULL_RTX;
}

tree
kvx_resolve_overloaded_builtin (location_t ARG_UNUSED (loc),
				tree ob_fndecl,
				void *passed_arglist)
{
  (void) loc;
  (void) ob_fndecl;
  (void) passed_arglist;
   return NULL_TREE;
}

