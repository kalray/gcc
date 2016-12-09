/* { dg-do run } */
/* { dg-options "-O2 -save-temps" } */

#include <stdlib.h>

typedef struct {
 unsigned int x[3];
 unsigned int y[3];
} state;

static void
div2mul(state *s)
{
 long long x3 = (long long) 1403580ll * (long long) s->x[1] + (long long) -810728ll * (long long) s->x[0];
 int kx = x3 / 4294967087u;
 x3 -= kx * 4294967087u;
 if (x3 < 0) {
  x3 += 4294967087u;
 }
 s->x[0] = s->x[1];
 s->x[1] = s->x[2];
 s->x[2] = x3;

 long long y3 = (long long) 527612ll * (long long) s->y[2] + (long long) -1370589ll * (long long) s->y[0];
 int ky = y3 / 4294944443u;
 y3 -= ky * 4294944443u;
 if (y3 < 0) {
  y3 += 4294944443u;
 }
 s->y[0] = s->y[1];
 s->y[1] = s->y[2];
 s->y[2] = y3;
}

__attribute__((noinline))
static void
gen(state *state, double *value)
{
    div2mul(state);
    if (state->x[2] <= state->y[2]) {
        *value = ((long long) state->x[2] - (long long) state->y[2] + (long long) 4294967087u) * 2.328306549295728e-10;
    } else {
        *value = ((long long) state->x[2] - (long long) state->y[2]) * 2.328306549295728e-10;
    }
}


int main()
{
    state s = { { 0xa99eb4c3, 0xa415e660, 0xb0e4c094 },
                { 0xb29c278, 0x70b0008c, 0xc896be0e } };
    double d;

    gen (&s, &d);

    if (*(unsigned long long*)&d != 0x3fe06fc75bdad1fb)
        abort();

    return 0;
}

/* { dg-final { scan-assembler-not "call __div" } } */
/* { dg-final { cleanup-saved-temps } } */

