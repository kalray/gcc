/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "fnarrowhwp \\\$r\[0-9\]+r\[0-9\]+ = \\\$r\[0-9\]+r\[0-9\]+" } } */
/* { dg-skip-if "Test valid only on Andey/Bostan" { *-*-* }  {  "*" } { "-march=k1a" "-march=k1b" } } */

#include "simd.h"

v2si f_narrowhwp(v2sf inputs){
  return __builtin_k1_fnarrowhwp(inputs);
}
