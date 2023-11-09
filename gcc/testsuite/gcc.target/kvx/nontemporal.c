/* { dg-do compile } */
/* { dg-options "-O2 -fopenmp -fdump-tree-optimized" } */
/* { dg-skip-if "Only on kvx-cos" { ! "kvx-*-cos" } } */

#define N 13245

void test1(float a[restrict N+42], float b[restrict N], float c[restrict N])
{
    #pragma omp simd nontemporal(a, b)
    for (int i = 0; i < N; i++)
    {
      float *t = &a[42];
      c[i] += t[i] + b[i];
    }
}

/* { dg-final { scan-tree-dump-times "\\{nt\\}" 6 "optimized" } } */
/* { dg-final { scan-assembler-times "lq.u"  2 } } */
/* { dg-final { scan-assembler-times "lwz.u" 4 } } */
