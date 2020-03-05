/* This is a copy from pr28982a.c that has a high register pressure to force
   some save/restore in pro/epilogue */

/* { dg-do  compile } */
/* { dg-options "-O3 -fdump-rtl-pro_and_epilogue" } */

/* We have 4 load/store */

/* Match the stores /f */
/* { dg-final { scan-rtl-dump-times "\\(insn/f \[0-9\]+ \[0-9\]+ \[0-9\]+ \[0-9\]+ \\(parallel" 4 "pro_and_epilogue"} } */

/* Match the corresponding loads */
/* { dg-final { scan-rtl-dump-times "\\(insn \[0-9\]+ \[0-9\]+ \[0-9\]+ \[0-9\]+ \\(parallel" 4 "pro_and_epilogue"} } */


#define NITER 4
#define NVARS 20
#define MULTI(X) \
  X( 0), X( 1), X( 2), X( 3), X( 4), X( 5), X( 6), X( 7), X( 8), X( 9), \
  X(10), X(11), X(12), X(13), X(14), X(15), X(16), X(17), X(18), X(19)

#define DECLAREI(INDEX) inc##INDEX = incs[INDEX]
#define DECLAREF(INDEX) *ptr##INDEX = ptrs[INDEX], result##INDEX = 0
#define LOOP(INDEX) result##INDEX += *ptr##INDEX, ptr##INDEX += inc##INDEX
#define COPYOUT(INDEX) results[INDEX] = result##INDEX

float *ptrs[NVARS];
float results[NVARS];
int incs[NVARS];

void __attribute__((noinline))
foo (int n)
{
  int MULTI (DECLAREI);
  float MULTI (DECLAREF);
  while (n--)
    MULTI (LOOP);
  MULTI (COPYOUT);
}

float input[NITER * NVARS];

int
main (void)
{
  int i;

  for (i = 0; i < NVARS; i++)
    ptrs[i] = input + i, incs[i] = i;
  for (i = 0; i < NITER * NVARS; i++)
    input[i] = i;
  foo (NITER);
  for (i = 0; i < NVARS; i++)
    if (results[i] != i * NITER * (NITER + 1) / 2)
      return 1;
  return 0;
}
