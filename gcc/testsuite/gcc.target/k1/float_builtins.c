/* { dg-do  run } */
/* { dg-options "-O2 -std=c99" } */
/* { dg-skip-if "Test valid only on Andey/Bostan" { *-*-* }  {  "*" } { "-march=k1a" "-march=k1b" } } */

#include <fenv.h>

void abort() __attribute__((noreturn)); 

#ifndef __k1a__
typedef float __v2sf __attribute__ ((__vector_size__ (8)));
#endif

#define TEST_2F(insn, val1, val2) \
{ \
__typeof__(__builtin_k1_##insn (val1, val2)) res_builtin = __builtin_k1_##insn (val1, val2); \
__typeof__(__builtin_k1_##insn (val1, val2)) res_asm; \
__asm__ (#insn " %0 = %1, %2\n;;" : "=r"(res_asm) : "r"((float)val1), "r"((float)val2)); \
if (res_asm != res_builtin) abort (); \
}

#define TEST_3F(insn, val1, val2, val3) \
{ \
__typeof__(__builtin_k1_##insn (val1, val2, val3)) res_builtin = __builtin_k1_##insn (val1, val2, val3); \
__typeof__(__builtin_k1_##insn (val1, val2, val3)) res_asm; \
__asm__ (#insn " %0 = %1, %2, %3\n;;" : "=r"(res_asm) : "r"((float)val1), "r"((float)val2), "r"((float)val3)); \
if (res_asm != res_builtin) abort (); \
}

#define TEST_2V(insn, val1, val2) \
{ \
__typeof__(__builtin_k1_##insn (val1, val2)) res_builtin = __builtin_k1_##insn (val1, val2); \
__typeof__(__builtin_k1_##insn (val1, val2)) res_asm; \
__asm__ (#insn " %0 = %1, %2\n;;" : "=r"(res_asm) : "r"((__v2sf)val1), "r"((__v2sf)val2)); \
 if (*(unsigned long long*)&res_asm != *(unsigned long long*)&res_builtin) abort ();            \
}

#define TEST_3V(insn, val1, val2, val3) \
{ \
__typeof__(__builtin_k1_##insn (val1, val2, val3)) res_builtin = __builtin_k1_##insn (val1, val2, val3); \
__typeof__(__builtin_k1_##insn (val1, val2, val3)) res_asm; \
__asm__ (#insn " %0 = %1, %2, %3\n;;" : "=r"(res_asm) : "r"((__v2sf)val1), "r"((__v2sf)val2), "r"((__v2sf)val3)); \
if (*(unsigned long long*)&res_asm != *(unsigned long long*)&res_builtin) abort (); \
}

#define TEST_1D2F(insn, val1, val2, val3) \
{ \
__typeof__(__builtin_k1_##insn (val1, val2, val3)) res_builtin = __builtin_k1_##insn (val1, val2, val3); \
__typeof__(__builtin_k1_##insn (val1, val2, val3)) res_asm; \
__asm__ (#insn " %0 = %1, %2, %3\n;;" : "=r"(res_asm) : "r"((double)val1), "r"((float)val2), "r"((float)val3)); \
if (res_asm != res_builtin) abort (); \
}

#define TEST_2D(insn, val1, val2) \
{ \
__typeof__(__builtin_k1_##insn (val1, val2)) res_builtin = __builtin_k1_##insn (val1, val2); \
__typeof__(__builtin_k1_##insn (val1, val2)) res_asm; \
__asm__ (#insn " %0 = %1, %2\n;;" : "=r"(res_asm) : "r"((double)val1), "r"((double)val2)); \
if (res_asm != res_builtin) abort (); \
}

#define TEST_3D(insn, val1, val2, val3) \
{ \
__typeof__(__builtin_k1_##insn (val1, val2, val3)) res_builtin = __builtin_k1_##insn (val1, val2, val3); \
__typeof__(__builtin_k1_##insn (val1, val2, val3)) res_asm; \
__asm__ (#insn " %0 = %1, %2, %3\n;;" : "=r"(res_asm) : "r"((double)val1), "r"((double)val2), "r"((double)val3)); \
if (res_asm != res_builtin) abort (); \
}


#define TEST_4F(insn, val1, val2, val3, val4) \
{ \
__typeof__(__builtin_k1_##insn (val1, val2, val3, val4)) res_builtin = __builtin_k1_##insn (val1, val2, val3, val4); \
__typeof__(__builtin_k1_##insn (val1, val2, val3, val4)) res_asm; \
union { unsigned long raw; float f; } v1, v2, v3, v4; \
v1.f = val1; \
v2.f = val2; \
v3.f = val3; \
v4.f = val4; \
__asm__ (#insn " %0 = %1, %2\n;;" : "=r"(res_asm) \
                                  : "r"((unsigned long long)v1.raw | ((unsigned long long)v2.raw << 32)), \
                                    "r"((unsigned long long)v3.raw | ((unsigned long long)v4.raw << 32))); \
if (res_asm != res_builtin) abort (); \
}


float snan;
double snand;
fenv_t cleanenv;

#define VAL1 1.0
#define VAL2 2.0
#define VAL3 3.0
#define VAL4 4.0
#define VAL5 5.0
#define VAL6 6.0

#define TEST_RN_VARIANT_2F(insn)                \
    {                                                         \
        volatile float res = __builtin_k1_##insn(snan, snan); \
        if (fetestexcept (FE_ALL_EXCEPT))                     \
            abort ();                                         \
    }

#define TEST_RN_VARIANT_2V(insn)                \
    {                                                         \
        volatile __v2sf res = __builtin_k1_##insn((__v2sf){snan, snan}, (__v2sf){snan, snan}); \
        if (fetestexcept (FE_ALL_EXCEPT))                     \
            abort ();                                         \
    }

#define TEST_RN_VARIANT_2D(insn)                \
    {                                                           \
        volatile double res = __builtin_k1_##insn(snand, snand); \
        if (fetestexcept (FE_ALL_EXCEPT))                       \
            abort ();                                           \
    }

#define TEST_RN_VARIANT_1D2F(insn)                                      \
    {                                                                   \
        volatile double res = __builtin_k1_##insn(snand, snan, snan);    \
        if (fetestexcept (FE_ALL_EXCEPT))                               \
            abort ();                                                   \
    }


#define TEST_RN_VARIANT_3F(insn)                                        \
    {                                                                   \
        volatile float res = __builtin_k1_##insn(snan, snan, snan);     \
        if (fetestexcept (FE_ALL_EXCEPT))                               \
            abort ();                                                   \
    }

#define TEST_RN_VARIANT_3V(insn)                                        \
    {                                                                   \
        volatile __v2sf res = __builtin_k1_##insn((__v2sf){snan, snan}, (__v2sf){snan, snan}, (__v2sf){snan, snan});     \
        if (fetestexcept (FE_ALL_EXCEPT))                               \
            abort ();                                                   \
    }


#define TEST_RN_VARIANT_3D(insn)                                        \
    {                                                                   \
        volatile double res = __builtin_k1_##insn(snand, snand, snand);  \
        if (fetestexcept (FE_ALL_EXCEPT))                               \
            abort ();                                                   \
    }


int main ()
{
        snan = __builtin_nansf("");
        snand = __builtin_nans("");

        TEST_RN_VARIANT_2F (faddrn);
        TEST_RN_VARIANT_3F (ffmanrn);
        TEST_RN_VARIANT_3F (ffmarn);
        TEST_RN_VARIANT_3F (ffmsnrn);
        TEST_RN_VARIANT_3F (ffmsrn);
        TEST_RN_VARIANT_2F (fmulrn);
        TEST_RN_VARIANT_2F (fsbfrn);
#ifndef __k1a__
        TEST_RN_VARIANT_2D (faddrnd);
        TEST_RN_VARIANT_3D (ffmanrnd);
        TEST_RN_VARIANT_1D2F (ffmanrnwd);
        TEST_RN_VARIANT_3D (ffmarnd);
        TEST_RN_VARIANT_1D2F (ffmarnwd);
        TEST_RN_VARIANT_3D (ffmsnrnd);
        TEST_RN_VARIANT_1D2F (ffmsrnwd);
        TEST_RN_VARIANT_1D2F (ffmsnrnwd);
        TEST_RN_VARIANT_3D (ffmsrnd);
        TEST_RN_VARIANT_2D (fsbfrnd);
        TEST_RN_VARIANT_2D (fmulrnd);
        TEST_RN_VARIANT_2F (fmulrnwd);
#endif

#ifndef __k1a__
        TEST_RN_VARIANT_2V (faddrnwp);
        TEST_RN_VARIANT_3V (ffmarnwp);
        TEST_RN_VARIANT_3V (ffmanrnwp); 
        TEST_RN_VARIANT_3V (ffmsrnwp);
        TEST_RN_VARIANT_3V (ffmsnrnwp);
        TEST_RN_VARIANT_2V (fmulrnwp);
        TEST_RN_VARIANT_2V (fsbfrnwp);
#endif
        TEST_4F(fcma, VAL1, VAL2, VAL3, VAL4);
        TEST_4F(fcmawd, VAL1, VAL2, VAL3, VAL4);
        TEST_4F(fcms, VAL1, VAL2, VAL3, VAL4);
        TEST_4F(fcmswd, VAL1, VAL2, VAL3, VAL4);
        TEST_4F(fdma, VAL1, VAL2, VAL3, VAL4);
        TEST_4F(fdmawd, VAL1, VAL2, VAL3, VAL4);
        TEST_4F(fdms, VAL1, VAL2, VAL3, VAL4);
        TEST_4F(fdmswd, VAL1, VAL2, VAL3, VAL4);

        TEST_3F(ffma, VAL1, VAL2, VAL3);
        TEST_3F(ffman, VAL1, VAL2, VAL3);
        TEST_3F(ffmanrn, VAL1, VAL2, VAL3);
        TEST_1D2F(ffmanwd, VAL1, VAL2, VAL3);
        TEST_3F(ffmarn, VAL1, VAL2, VAL3);
        TEST_1D2F(ffmawd, VAL1, VAL2, VAL3);
#ifndef __k1a__
        TEST_1D2F(ffmanrnwd, VAL1, VAL2, VAL3);
        TEST_1D2F(ffmarnwd, VAL1, VAL2, VAL3);
        TEST_3D(ffmad, VAL1, VAL2, VAL3);
        TEST_3D(ffmand, VAL1, VAL2, VAL3);
        TEST_3D(ffmanrnd, VAL1, VAL2, VAL3);
        TEST_3D(ffmarnd, VAL1, VAL2, VAL3);
#endif

        TEST_3F(ffms, VAL1, VAL2, VAL3);
        TEST_3F(ffmsn, VAL1, VAL2, VAL3);
        TEST_3F(ffmsnrn, VAL1, VAL2, VAL3);
        TEST_1D2F(ffmsnwd, VAL1, VAL2, VAL3);
        TEST_3F(ffmsrn, VAL1, VAL2, VAL3);
        TEST_1D2F(ffmswd, VAL1, VAL2, VAL3);
#ifndef __k1a__
        TEST_1D2F(ffmsnrnwd, VAL1, VAL2, VAL3);
        TEST_1D2F(ffmsrnwd, VAL1, VAL2, VAL3);
        TEST_3D(ffmsd, VAL1, VAL2, VAL3);
        TEST_3D(ffmsnd, VAL1, VAL2, VAL3);
        TEST_3D(ffmsnrnd, VAL1, VAL2, VAL3);
        TEST_3D(ffmsrnd, VAL1, VAL2, VAL3);
#endif

        TEST_2F (fmulrn, VAL1, VAL2);
#ifndef __k1a__
        TEST_2F (fmulwd, VAL1, VAL2);
        TEST_2F (fmulrnwd, VAL1, VAL2);
        TEST_2D (fmulrnd, VAL1, VAL2);
#endif

        TEST_2F (faddrn, VAL1, VAL2);
#ifndef __k1a__
        TEST_2D (faddrnd, VAL1, VAL2);
#endif

        TEST_2F (fsbfrn, VAL1, VAL2);
#ifndef __k1a__
        TEST_2D (fsbfrnd, VAL1, VAL2);
#endif

#ifndef __k1a__
        TEST_2V (faddwp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}));
        TEST_2V (faddrnwp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}));
        TEST_3V (ffmawp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}), ((__v2sf){VAL5, VAL6}));
        TEST_3V (ffmarnwp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}), ((__v2sf){VAL5, VAL6}));
        TEST_3V (ffmanwp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}), ((__v2sf){VAL5, VAL6}));
        TEST_3V (ffmanrnwp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}), ((__v2sf){VAL5, VAL6})); 
        TEST_3V (ffmswp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}), ((__v2sf){VAL5, VAL6}));
        TEST_3V (ffmsrnwp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}), ((__v2sf){VAL5, VAL6}));
        TEST_3V (ffmsnwp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}), ((__v2sf){VAL5, VAL6}));
        TEST_3V (ffmsnrnwp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}), ((__v2sf){VAL5, VAL6}));
        TEST_2V (fmulwp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}));
        TEST_2V (fmulrnwp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}));
        TEST_2V (fsbfwp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}));
        TEST_2V (fsbfrnwp, ((__v2sf){VAL1, VAL2}), ((__v2sf){VAL3, VAL4}));
#endif
        return 0;	
}

