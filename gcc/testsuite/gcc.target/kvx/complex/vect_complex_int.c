/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-additional-options "-fdump-tree-vect-details -fdump-tree-widening_mul" } */

#define N 64
#define INNER_TYPE int

#include "vect_complex_operations.h"

/* KV3-1 */
/* { dg-final { scan-tree-dump-times "vectorized 1 loops in function" 13 "vect"         { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "			       0 "widening_mul" { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA"                           0 "widening_mul" { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "lq"			      26                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "sq"                             13                { target { any-opts "-march=kv3-1" } } } } */

/* KV3-2 */
/* { dg-final { scan-tree-dump-times "vectorized 1 loops in function" 13 "vect"         { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "			       0 "widening_mul" { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA"                           0 "widening_mul" { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "lq"			      26                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "sq"                             13                { target { any-opts "-march=kv3-2" } } } } */

/* KV4-1 */
/* { dg-final { scan-tree-dump-times "vectorized 1 loops in function" 13 "vect"         { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "			       0 "widening_mul" { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA"                           0 "widening_mul" { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "lq"			      26                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "sq"                             13                { target { any-opts "-march=kv4-1" } } } } */
