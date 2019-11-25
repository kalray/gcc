/* { dg-do  compile } */
/* { dg-options "-O2 -c -save-temps"  } */

long access_hi(short *p, int i) { return ++p[i]; }
long access_si(int *p, int i) { return ++p[i]; }
long access_sim(int *p, long i) { return ++*(int *)((char *)p + 4*i); }
long access_sis(int *p, long i) { return ++*(int *)((char *)p + (i<<2)); }
long access_regxs(int *p, long i) { return ++p[i]; }
long access_di(long long *p, int i) { return ++p[i]; }
__int128 access_ti(__int128 *p, int i) { return ++p[i]; }
typedef float float32_t;
typedef float32_t float32x4_t __attribute__((vector_size(4*sizeof(float32_t))));
float32x4_t access_v4sf(float32x4_t *p, int i) { return ++p[i]; }
typedef long long int64_t;
typedef int64_t int64x4_t __attribute__((vector_size(4*sizeof(int64_t))));
int64x4_t access_v4di(int64x4_t *p, int i) { return ++p[i]; }

/* { dg-final { scan-assembler-times "lhz.xs \\\$" 1 } } */
/* { dg-final { scan-assembler-times "sh.xs \\\$" 1 } } */

/* { dg-final { scan-assembler-times "lwz.xs \\\$" 4 } } */
/* { dg-final { scan-assembler-times "sw.xs \\\$" 4 } } */

/* { dg-final { scan-assembler-times "ld.xs \\\$" 1 } } */
/* { dg-final { scan-assembler-times "sd.xs \\\$" 1 } } */

/* { dg-final { scan-assembler-times "lq.xs \\\$" 1 } } */
/* { dg-final { scan-assembler-times "sq.xs \\\$" 1 } } */

/* { dg-final { scan-assembler-times "lo.xs \\\$" 0 } } */
/* { dg-final { scan-assembler-times "so.xs \\\$" 1 } } */

