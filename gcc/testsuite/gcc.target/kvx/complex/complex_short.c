/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-additional-options "-fdump-tree-cplxlower-details -fdump-tree-widening_mul" } */

#define INNER_TYPE short

#include "complex_operations.h"

/* KV3-1 */
/* { dg-final { scan-tree-dump-times "Native: " 15 "cplxlower1"  { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times "Split: "  6 "cplxlower1"   { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times "-> But "  0 "cplxlower1"   { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "    0 "widening_mul" { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA "   0 "widening_mul" { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "addhq "   3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "sbfhq "   3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "addhcp.c" 5                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "sbfhcp.c" 1                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "neghq "   1                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "make \\\$r. = 0?x?\\d+" 1  { target { any-opts "-march=kv3-1" } } } } */

/* KV3-2 */
/* { dg-final { scan-tree-dump-times "Native: " 15 "cplxlower1"  { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times "Split: "  6 "cplxlower1"   { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times "-> But "  0 "cplxlower1"   { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "    0 "widening_mul" { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA "   0 "widening_mul" { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "addhq "   4                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "sbfhq "   4                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "addhcp.c" 0                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "sbfhcp.c" 0                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "neghq "   7                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "make \\\$r. = 0?x?\\d+" 1  { target { any-opts "-march=kv3-2" } } } } */

/* KV4-1 */
/* { dg-final { scan-tree-dump-times "Native: " 15 "cplxlower1"  { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times "Split: "  6 "cplxlower1"   { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times "-> But "  0 "cplxlower1"   { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "    0 "widening_mul" { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA "   0 "widening_mul" { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "addhq "   4                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "sbfhq "   4                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "addhcp.c" 0                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "sbfhcp.c" 0                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "neghq "   7                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "make \\\$r. = 0?x?\\d+" 1  { target { any-opts "-march=kv4-1" } } } } */

