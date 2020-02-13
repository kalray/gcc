/* This test ensures that vector in arguments that are not aligned on
   2/4 registers are correctly handled.
 */

/* { dg-do  compile } */
/* { dg-options "-O3 -std=c99 -DNO_MAIN=1" } */

/* { dg-final { scan-assembler-times "so -?\[0-9a-fA-F\]+\\\[\\\$r\[0-9\]+\\\] = \\\$r\[0-9\]r\[0-9\]r\[0-9\]r\[0-9\]" 1 } } */
/* { dg-final { scan-assembler-times "sq -?\[0-9a-fA-F\]+\\\[\\\$r\[0-9\]+\\\] = \\\$r\[0-9\]r\[0-9\]" 1 } } */

#include <stdint.h>

typedef int64_t vect2_t __attribute((vector_size( 2 * sizeof(int64_t))));
typedef int64_t vect4_t __attribute((vector_size( 4 * sizeof(int64_t))));


void __attribute ((noinline))
vect_v2_store(vect2_t *b, vect2_t a)
{
    *b = a;
}

void __attribute ((noinline))
vect_v4_store(vect4_t *b, vect4_t a)
{
    *b = a;
}

#ifndef NO_MAIN

vect2_t a2 = { 1, 2 };
vect4_t a4 = { 1, 2, 3, 4 };

int
main(void)
{
    int i;
    vect4_t b4;
    vect2_t b2;

    vect_v2_store(&b2, a2);
    for (i = 0; i < 2; i++) {
        if (a2[i] != b2[i]) {
            return 1;
        }
    }

    vect_v4_store(&b4, a4);
    for (i = 0; i < 4; i++) {
        if (a4[i] != b4[i]) {
            return 1;
        }
    }

    return 0;
}
#endif
