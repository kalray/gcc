/* { dg-do  compile } */
/* { dg-options "-O2 -ffast-math" } */

#include <math.h>


float k1_fdivw(float a, float b)
{
    return a/b;
}
/* { dg-final { scan-assembler-times "finvw \\\$r1 = \\\$r1" 1 } } */

float k1_finvw(float a)
{
    return 1.0f/a;
}
/* { dg-final { scan-assembler-times "finvw \\\$r0 = \\\$r0" 1 } } */

float k1_fsqrtw(float a)
{
    return sqrtf(a);
}
/* { dg-final { scan-assembler-times "fisrw \\\$r1 = \\\$r0" 1 } } */

float k1_fisrw(float a)
{
    return 1.0f/sqrtf(a);
}
/* { dg-final { scan-assembler-times "fisrw \\\$r0 = \\\$r0" 1 } } */

