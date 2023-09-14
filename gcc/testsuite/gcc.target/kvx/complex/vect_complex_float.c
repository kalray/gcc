/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-additional-options "-fdump-tree-vect-details -fdump-tree-widening_mul" } */

#define N 64
#define INNER_TYPE float

#include "vect_complex_operations.h"

/* KV3-1 */
/* { dg-final { scan-tree-dump-times "vectorized 1 loops in function" 12 "vect"         { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "			       0 "widening_mul" { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA"                           0 "widening_mul" { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "lq"			      26                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "sq"                             12                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "faddwcp "			       3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "fsbfwcp "			       3                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "faddwcp.c "		       1                { target { any-opts "-march=kv3-1" } } } } */
/* { dg-final { scan-assembler-times "fsbfwcp.c "		       1                { target { any-opts "-march=kv3-1" } } } } */

/* KV3-2 */
/* { dg-final { scan-tree-dump-times "vectorized 1 loops in function" 12 "vect"         { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "			       2 "widening_mul" { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA"                           2 "widening_mul" { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "lq"			      26                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "sq"                             12                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "faddwcp "			       1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fsbfwcp "			       1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fmulwcp "		               1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "ffmawcp "		               1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "ffmswcp "		               1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "faddwcp.c "		       1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fsbfwcp.c "		       1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "fmulwcp.c "		       1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "ffmawcp.c "		       1                { target { any-opts "-march=kv3-2" } } } } */
/* { dg-final { scan-assembler-times "ffmswcp.c "		       1                { target { any-opts "-march=kv3-2" } } } } */


/* KV4-1 */
/* { dg-final { scan-tree-dump-times "vectorized 1 loops in function" 12 "vect"         { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FMA "			       2 "widening_mul" { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-tree-dump-times ".FNMA"                           2 "widening_mul" { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "lq"			      26                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "sq"                             12                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "faddwcp "			       1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fsbfwcp "			       1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fmulwcp "		               1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "ffmawcp "		               1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "ffmswcp "		               1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "faddwcp.c "		       1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fsbfwcp.c "		       1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "fmulwcp.c "		       1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "ffmawcp.c "		       1                { target { any-opts "-march=kv4-1" } } } } */
/* { dg-final { scan-assembler-times "ffmswcp.c "		       1                { target { any-opts "-march=kv4-1" } } } } */

