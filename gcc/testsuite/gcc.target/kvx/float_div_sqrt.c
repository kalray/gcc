/* { dg-do  compile } */
/* { dg-options "-O2 -ffast-math" } */

#include <math.h>


float k1_fdivw(float a, float b)
{
    return a/b;
}
/* { dg-final { scan-assembler-times "frecw \\\$r1 = \\\$r1" 1 } } */

float k1_frecw(float a)
{
    return 1.0f/a;
}
/* { dg-final { scan-assembler-times "frecw \\\$r0 = \\\$r0" 1 } } */

float k1_fsqrtw(float a)
{
    return sqrtf(a);
}
/* { dg-final { scan-assembler-times "frsrw \\\$r1 = \\\$r0" 1 } } */

float k1_frsrw(float a)
{
    return 1.0f/sqrtf(a);
}
/* { dg-final { scan-assembler-times "frsrw \\\$r0 = \\\$r0" 1 } } */

