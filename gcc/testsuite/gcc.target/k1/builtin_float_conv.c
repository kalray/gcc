/* { dg-do  run } */
/* { dg-options "-O2 -save-temps"  } */
/* { dg-skip-if "Test valid only on Andey/Bostan" { *-*-* }  {  "*" } { "-march=k1a" "-march=k1b" } } */

#define TESTSF(rounding) \
        sf = __builtin_k1_float (rounding, 2, 0);   \
        si = __builtin_k1_fixed (rounding, sf, 0);  \
        if (si != 2)                                \
                abort ();                           \
        sf = __builtin_k1_floatu (rounding, 2, 0);  \
        usi = __builtin_k1_fixedu (rounding, sf, 0);\
        if (usi != 2)                               \
                abort ();                           \



#define TESTDF(rounding) \
        df = __builtin_k1_floatd (rounding, 2, 0);   \
	di = __builtin_k1_fixedd (rounding, df, 0);  \
        if (di != 2)                                 \
                abort ();                            \
        df = __builtin_k1_floatud (rounding, 2, 0);  \
        udi = __builtin_k1_fixedud (rounding, df, 0);\
        if (udi != 2)                                \
                abort ();                            \


extern void abort ();

int main()
{
	int rounding;
	float sf;
	double df;
	int si;
	long long di;
	unsigned int usi;
	unsigned long long udi;

	TESTSF(0);
        TESTSF(1);
        TESTSF(2);
        TESTSF(3);

        TESTDF(0);
        TESTDF(1);
        TESTDF(2);
        TESTDF(3);

        return 0;
}


/* { dg-final { scan-assembler-times "float.rn" 1 } } */
/* { dg-final { scan-assembler-times "float.rp" 1 } } */
/* { dg-final { scan-assembler-times "float.rm" 1 } } */
/* { dg-final { scan-assembler-times "float.rz" 1 } } */

/* { dg-final { scan-assembler-times "floatu.rn" 1 } } */
/* { dg-final { scan-assembler-times "floatu.rp" 1 } } */
/* { dg-final { scan-assembler-times "floatu.rm" 1 } } */
/* { dg-final { scan-assembler-times "floatu.rz" 1 } } */

/* { dg-final { scan-assembler-times "floatd.rn" 1 } } */
/* { dg-final { scan-assembler-times "floatd.rp" 1 } } */
/* { dg-final { scan-assembler-times "floatd.rm" 1 } } */
/* { dg-final { scan-assembler-times "floatd.rz" 1 } } */

/* { dg-final { scan-assembler-times "floatud.rn" 1 } } */
/* { dg-final { scan-assembler-times "floatud.rp" 1 } } */
/* { dg-final { scan-assembler-times "floatud.rm" 1 } } */
/* { dg-final { scan-assembler-times "floatud.rz" 1 } } */


/* { dg-final { scan-assembler-times "fixed.rn" 1 } } */
/* { dg-final { scan-assembler-times "fixed.rp" 1 } } */
/* { dg-final { scan-assembler-times "fixed.rm" 1 } } */
/* { dg-final { scan-assembler-times "fixed.rz" 1 } } */

/* { dg-final { scan-assembler-times "fixedu.rn" 1 } } */
/* { dg-final { scan-assembler-times "fixedu.rp" 1 } } */
/* { dg-final { scan-assembler-times "fixedu.rm" 1 } } */
/* { dg-final { scan-assembler-times "fixedu.rz" 1 } } */

/* { dg-final { scan-assembler-times "fixedd.rn" 1 } } */
/* { dg-final { scan-assembler-times "fixedd.rp" 1 } } */
/* { dg-final { scan-assembler-times "fixedd.rm" 1 } } */
/* { dg-final { scan-assembler-times "fixedd.rz" 1 } } */

/* { dg-final { scan-assembler-times "fixedud.rn" 1 } } */
/* { dg-final { scan-assembler-times "fixedud.rp" 1 } } */
/* { dg-final { scan-assembler-times "fixedud.rm" 1 } } */
/* { dg-final { scan-assembler-times "fixedud.rz" 1 } } */

/* { dg-final { cleanup-saved-temps } } */
