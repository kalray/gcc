/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "lhpzun \\\$r\[0-9\]+r\[0-9\]+ = " } } */

#include "simd.h"

v2si f_lhpzun(uv2hi *bob){
  return __builtin_k1_lhpzun(bob);
}
