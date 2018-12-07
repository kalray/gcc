/* FIXME AUTO: Disabling get builtin. Ref T7705 */
/* { d g-do compile } */
/* { d g-options "-O3 -fno-unroll-loops -save-temps"  } */

extern void profiled_func1(void);
extern void profiled_func2(void);

#define PMC     12
#define PM0     13
#define PCR     2

void abort (void) __attribute__((noreturn));

static void TH_InitTimer(void)
{
   /* Use CC0 as cycle counter */
   __builtin_k1_set(PMC,0);
}

/* Reset and start. */
static void TH_StartTimer(void)
{
   __builtin_k1_set(PM0,0);
}

/* Stop and return. */
static int TH_StopTimer(void)
{
   return __builtin_k1_get(PM0);
}

int res[3];

#define NOP "nop\n\t;;\n\t"
#define NOP5 NOP NOP NOP NOP NOP

__attribute__((noinline,noclone))
void
profile(unsigned int iter)
{
    unsigned int i;
    int timer1 = 0;
    int timer2 = 0;
    int timer3 = 0;
    unsigned long pcr;

    for (i = 0; i < iter; ++i) {
        TH_StartTimer ();
        asm(NOP ::: "memory");
        timer1 += TH_StopTimer ();
        
        TH_StartTimer ();
        asm(NOP5 ::: "memory");
        timer2 += TH_StopTimer ();

        TH_StartTimer ();
        asm(NOP5 NOP5 ::: "memory");
        timer3 += TH_StopTimer ();
    }

    pcr = __builtin_k1_get(PCR);
    res[__builtin_k1_get(PCR) + 0 - pcr] = timer1;
    res[__builtin_k1_get(PCR) + 1 - pcr] = timer2;
    res[__builtin_k1_get(PCR) + 2 - pcr] = timer3;
}

#define ITER 10000

int
main ()
{
    TH_InitTimer ();

    profile (ITER);

    if (res[0] != ITER)
        abort ();
    if (res[1] != 5*ITER)
        abort ();
    if (res[2] != 10*ITER)
        abort ();

    return 0;
}

/* { d g-final { scan-assembler-times "set \\\$pmc" 1 } } */
/* { d g-final { scan-assembler-times "set \\\$pm0" 3 } } */
/* { d g-final { scan-assembler-times "get\[^=\]*= \\\$pm0" 3 } } */
/* Accesses to PCR must be CSE'd */
/* { d g-final { scan-assembler-not "get\[^=\]*= \\\$pcr" } } */
/* { d g-final { cleanup-saved-temps } } */
