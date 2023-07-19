/* { dg-do compile } */
/* { dg-require-effective-target vect_double } */
/* { dg-add-options arm_v8_3a_complex_neon } */
/* { dg-additional-options "-fno-tree-loop-vectorize -fdump-tree-gimple" } */

#define UNROLL

#define TYPE double
#define N 16
#include "complex-add-template.c"

/* { dg-final { scan-tree-dump-times "stmt.*COMPLEX_ADD_ROT90" 1 "slp1" { target { vect_complex_add_double } } } } */
/* { dg-final { scan-tree-dump-times "stmt.*COMPLEX_ADD_ROT270" 1 "slp1" { target { vect_complex_add_double } } } } */
/* { dg-final { scan-tree-dump "COMPLEX_ROT270" "gimple" } } */
/* { dg-final { scan-tree-dump "COMPLEX_ROT90" "gimple" } } */
/* { dg-final { scan-tree-dump "Found COMPLEX_ADD_ROT270" "slp1" { target { ! kvx-*-* } } } } */
/* { dg-final { scan-tree-dump "Found COMPLEX_ADD_ROT90" "slp1" { target { ! kvx-*-* } } } } */
