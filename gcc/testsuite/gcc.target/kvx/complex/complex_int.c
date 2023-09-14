/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-additional-options "-fdump-tree-cplxlower-details -fdump-tree-widening_mul" } */

#define INNER_TYPE int

#include "complex_operations.h"

/* KV3-1 */
/* { dg-final { scan-tree-dump-times "Native: " 21 "cplxlower1"  { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times "Split: "  0 "cplxlower1"   { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times "-> But "  0 "cplxlower1"   { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "    0 "widening_mul" { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA "   0 "widening_mul" { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "addwp "   3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "sbfwp "   3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "addwc.c " 2                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "sbfwc.c " 1                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "mulwc "   3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "mulwc.c " 3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "negwp "   1                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "make \\\$r. = 0?x?\\d+" 1  { target { any-opts "-march=kv3-1" } } } } */

/* KV3-2 */
/* { dg-final { scan-tree-dump-times "Native: " 21 "cplxlower1"  { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times "Split: "  0 "cplxlower1"   { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times "-> But "  0 "cplxlower1"   { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "    0 "widening_mul" { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA "   0 "widening_mul" { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "addwp "  10                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "sbfwp "  10                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "addwc.c " 0                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "sbfwc.c " 0                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "mulwc "   0                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "mulwc.c " 0                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "negwp "   7                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "mm212w "  6                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "sbmm8 "   6                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "make \\\$r. = 0?x?\\d+" 1  { target { any-opts "-march=kv3-2" } } } } */

/* KV4-1 */
/* { dg-final { scan-tree-dump-times "Native: " 21 "cplxlower1"  { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times "Split: "  0 "cplxlower1"   { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times "-> But "  0 "cplxlower1"   { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "    0 "widening_mul" { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA "   0 "widening_mul" { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "addwp "  10                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "sbfwp "  10                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "addwc.c " 0                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "sbfwc.c " 0                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "mulwc "   0                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "mulwc.c " 0                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "negwp "   7                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "mm212w "  6                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "sbmm8 "   6                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "make \\\$r. = 0?x?\\d+" 1  { target { any-opts "-march=kv4-1" } } } } */
