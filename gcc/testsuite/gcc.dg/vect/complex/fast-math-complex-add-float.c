/* { dg-do compile } */
/* { dg-require-effective-target vect_float } */
/* { dg-add-options arm_v8_3a_complex_neon } */
/* { dg-additional-options "-fdump-tree-gimple" } */

#define TYPE float
#define N 200
#include "complex-add-template.c"

/* { dg-final { scan-tree-dump-times "stmt.*COMPLEX_ADD_ROT270" 1 "vect" { target { vect_complex_add_float } } } } */
/* { dg-final { scan-tree-dump-times "stmt.*COMPLEX_ADD_ROT90" 1 "vect" { target { vect_complex_add_float } } } } */
/* { dg-final { scan-tree-dump "COMPLEX_ROT270" "gimple" } } */
/* { dg-final { scan-tree-dump "COMPLEX_ROT90" "gimple" } } */
/* { dg-final { scan-tree-dump "Found COMPLEX_ADD_ROT270" "vect" { target { ! kvx-*-* } } } } */
/* { dg-final { scan-tree-dump "Found COMPLEX_ADD_ROT90" "vect" { target { ! kvx-*-* } } } } */
