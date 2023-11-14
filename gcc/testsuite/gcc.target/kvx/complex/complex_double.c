/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-additional-options "-fdump-tree-cplxlower-details -fdump-tree-widening_mul" } */

#define INNER_TYPE double

#include "complex_operations.h"

/* KV3-1 */
/* { dg-final { scan-tree-dump-times "Native: " 15 "cplxlower1"   { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times "Split: "   6 "cplxlower1"   { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times "-> But "   6 "cplxlower1"   { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "     0 "widening_mul" { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA "    0 "widening_mul" { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "fadddp "   3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "fsbfdp "   3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "fadddc.c " 1                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "fsbfdc.c " 1                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "fnegd "    6                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "make \\\$r. = 0x" 2         { target { any-opts "-march=kv3-1" } } } } */

/* KV3-2 */
/* { dg-final { scan-tree-dump-times "Native: " 15 "cplxlower1"   { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times "Split: "   6 "cplxlower1"   { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times "-> But "   6 "cplxlower1"   { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "     0 "widening_mul" { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA "    0 "widening_mul" { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fadddp "   3                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fsbfdp "   3                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fadddc.c " 1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fsbfdc.c " 1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fnegd "    6                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "make \\\$r. = 0x" 2         { target { any-opts "-march=kv3-2" } } } } */

/* KV4-1 */
/* { dg-final { scan-tree-dump-times "Native: " 15 "cplxlower1"   { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times "Split: "   6 "cplxlower1"   { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times "-> But "   6 "cplxlower1"   { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "     0 "widening_mul" { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA "    0 "widening_mul" { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "faddd "    6                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fsbfd "    6                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fadddc.c " 1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fsbfdc.c " 1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fnegd "    6                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "make \\\$r. = 0x" 2         { target { any-opts "-march=kv4-1" } } } } */
