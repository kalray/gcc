/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-additional-options "-fdump-tree-cplxlower-details -fdump-tree-widening_mul" } */

#define INNER_TYPE float

#include "complex_operations.h"

/* KV3-1 */
/* { dg-final { scan-tree-dump-times "Native: " 21 "cplxlower1"   { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times "Split: "   0 "cplxlower1"   { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times "-> But "   0 "cplxlower1"   { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "     0 "widening_mul" { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA "    0 "widening_mul" { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "faddwc "   3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "fsbfwc "   3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "faddwc.c " 1                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "fsbfwc.c " 1                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "fmulwc "   3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "fmulwc.c " 3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "fnegwp "   1                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "fnegd "    1                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "make \\\$r. = 0x" 1         { target { any-opts "-march=kv3-1" } } } } */

/* KV3-2 */
/* { dg-final { scan-tree-dump-times "Native: " 21 "cplxlower1"   { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times "Split: "   0 "cplxlower1"   { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times "-> But "   0 "cplxlower1"   { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "     2 "widening_mul" { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA"     2 "widening_mul" { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "faddwc "   1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fsbfwc "   1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "faddwc.c " 1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fsbfwc.c " 1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fmulwc "   1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fmulwc.c " 1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "ffmawc "   1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "ffmswc "   1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "ffmawc.c " 1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "ffmswc.c " 1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fnegwp "   1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fnegd "    1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "make \\\$r. = 0x" 1         { target { any-opts "-march=kv3-3" } } } } */

/* KV4-1 */
/* { dg-final { scan-tree-dump-times "Native: " 21 "cplxlower1"   { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times "Split: "   0 "cplxlower1"   { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times "-> But "   0 "cplxlower1"   { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "     2 "widening_mul" { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA "    2 "widening_mul" { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "faddwc "   1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fsbfwc "   1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "faddwc.c " 1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fsbfwc.c " 1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fmulwc "   1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fmulwc.c " 1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "ffmawc "   1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "ffmswc "   1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "ffmawc.c " 1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "ffmswc.c " 1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fnegwp "   1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fnegd "    1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "make \\\$r. = 0x" 1         { target { any-opts "-march=kv4-1" } } } } */
