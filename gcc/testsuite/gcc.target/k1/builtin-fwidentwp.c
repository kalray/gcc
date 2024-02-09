/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "fwidentwp \\\$r\[0-9\]+r\[0-9\]+ = \\\$r\[0-9\]+r\[0-9\]+" } } */

#include "simd.h"

v2sf f_widentwp(unsigned int packed_half_float){
  return __builtin_k1_fwidentwp(packed_half_float);
}
