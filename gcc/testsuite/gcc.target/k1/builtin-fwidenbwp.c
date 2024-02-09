/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "fwidenlwp \\\$r\[0-9\]+r\[0-9\]+ = \\\$r\[0-9\]+r\[0-9\]+" } } */
/* { dg-skip-if "Test valid only on Andey/Bostan" { *-*-* }  {  "*" } { "-march=k1a" "-march=k1b" } } */

#include "simd.h"

v2sf f_widenbwp(unsigned int packed_half_float){
  return __builtin_k1_fwidenlwp(packed_half_float);
}
