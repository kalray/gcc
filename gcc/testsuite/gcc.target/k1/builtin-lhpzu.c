/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "lhpzu \\\$r\[0-9\]+r\[0-9\]+ = " } } */

#include "simd.h"

v2si f_lhpzu(uv2hi *bob){
  return __builtin_k1_lhpzu(bob);
}
