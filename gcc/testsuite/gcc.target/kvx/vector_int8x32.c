/* { dg-do compile } */
/* { dg-options "-O2 -ffp-contract=fast" } */

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
typedef _Float16 float16_t;
typedef float float32_t;
typedef double float64_t;
#define fma(a, b, c) ((a) * (b) + c)

typedef int8_t int8x8_t __attribute((vector_size(8*sizeof(int8_t))));
typedef uint8_t uint8x8_t __attribute((vector_size(8*sizeof(int8_t))));
typedef int8_t int8x16_t __attribute((vector_size(16*sizeof(int8_t))));
typedef uint8_t uint8x16_t __attribute((vector_size(16*sizeof(int8_t))));
typedef int8_t int8x32_t __attribute((vector_size(32*sizeof(int8_t))));
typedef uint8_t uint8x32_t __attribute((vector_size(32*sizeof(int8_t))));

typedef int16_t int16x4_t __attribute((vector_size(4*sizeof(int16_t))));
typedef uint16_t uint16x4_t __attribute((vector_size(4*sizeof(int16_t))));
typedef int16_t int16x8_t __attribute((vector_size(8*sizeof(int16_t))));
typedef uint16_t uint16x8_t __attribute((vector_size(8*sizeof(int16_t))));
typedef int16_t int16x16_t __attribute((vector_size(16*sizeof(int16_t))));
typedef uint16_t uint16x16_t __attribute((vector_size(16*sizeof(int16_t))));

typedef int32_t int32x2_t __attribute((vector_size(2*sizeof(int32_t))));
typedef uint32_t uint32x2_t __attribute((vector_size(2*sizeof(int32_t))));
typedef int32_t int32x4_t __attribute((vector_size(4*sizeof(int32_t))));
typedef uint32_t uint32x4_t __attribute((vector_size(4*sizeof(int32_t))));
typedef int32_t int32x8_t __attribute((vector_size(8*sizeof(int32_t))));
typedef uint32_t uint32x8_t __attribute((vector_size(8*sizeof(int32_t))));

typedef int64_t int64x2_t __attribute((vector_size(2*sizeof(int64_t))));
typedef uint64_t uint64x2_t __attribute((vector_size(2*sizeof(int64_t))));
typedef int64_t int64x4_t __attribute((vector_size(4*sizeof(int64_t))));
typedef uint64_t uint64x4_t __attribute((vector_size(4*sizeof(int64_t))));

typedef float16_t float16x4_t __attribute__((vector_size(4*sizeof(float16_t))));
typedef float16_t float16x8_t __attribute__((vector_size(8*sizeof(float16_t))));
typedef float16_t float16x16_t __attribute__((vector_size(16*sizeof(float16_t))));

typedef float32_t float32x2_t __attribute__((vector_size(2*sizeof(float32_t))));
typedef float32_t float32x4_t __attribute__((vector_size(4*sizeof(float32_t))));
typedef float32_t float32x8_t __attribute__((vector_size(8*sizeof(float32_t))));

typedef float64_t float64x2_t __attribute__((vector_size(2*sizeof(float64_t))));
typedef float64_t float64x4_t __attribute__((vector_size(4*sizeof(float64_t))));

int8x32_t __attribute ((noinline))
kvx_int8x32_incr(int8x32_t a)
{
    return a + 1;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_addto(int8x32_t a)
{
    int8x32_t b = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
    return a + b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_subfrom(int8x32_t a)
{
    int8x32_t b = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
    return b - a;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_addwhere(int8x32_t a, int8x32_t __bypass *b)
{
    return a + *b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_asr(int8x32_t a, int b)
{
    return a >> b;
}
uint8x32_t __attribute ((noinline))
kvx_int8x32_lsr(uint8x32_t a, int b)
{
    return a >> b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_lsl(int8x32_t a, int b)
{
    return a << b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_addx2(int8x32_t a, int8x32_t b)
{
    return a + (b << 1);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_addx4(int8x32_t a, int8x32_t b)
{
    return a + (b << 2);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_addx8(int8x32_t a, int8x32_t b)
{
    return a + (b << 3);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_addx16(int8x32_t a, int8x32_t b)
{
    return a + (b << 4);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_sbfx2(int8x32_t a, int8x32_t b)
{
    return a - b*2;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_sbfx4(int8x32_t a, int8x32_t b)
{
    return a - b*4;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_sbfx8(int8x32_t a, int8x32_t b)
{
    return a - b*8;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_sbfx16(int8x32_t a, int8x32_t b)
{
    return a - b*16;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_mul(int8x32_t a, int8x32_t b)
{
    return a * b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_muls(int8x32_t a, int8_t b)
{
    return a * b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_madd(int8x32_t a, int8x32_t b, int8x32_t c)
{
    return a * b + c;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_msub(int8x32_t a, int8x32_t b, int8x32_t c)
{
    return c - a * b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_subx2(int8x32_t a, int8x32_t b)
{
    return a - (b << 1);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_subx4(int8x32_t a, int8x32_t b)
{
    return a - (b << 2);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_subx8(int8x32_t a, int8x32_t b)
{
    return a - (b << 3);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_subx16(int8x32_t a, int8x32_t b)
{
    return a - (b << 4);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_neg(int8x32_t a, int8x32_t b)
{
    return -a;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_not(int8x32_t a, int8x32_t b)
{
    return ~a;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_and(int8x32_t a, int8x32_t b)
{
    return a & b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_nand(int8x32_t a, int8x32_t b)
{
    return ~(a & b);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_or(int8x32_t a, int8x32_t b)
{
    return a | b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_nor(int8x32_t a, int8x32_t b)
{
    return ~(a | b);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_xor(int8x32_t a, int8x32_t b)
{
    return a ^ b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_nxor(int8x32_t a, int8x32_t b)
{
    return ~(a ^ b);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_andn(int8x32_t a, int8x32_t b)
{
    return ~a & b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_orn(int8x32_t a, int8x32_t b)
{
    return ~a | b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_xorn(int8x32_t a, int8x32_t b)
{
    return ~a ^ b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_ne(int8x32_t a, int8x32_t b)
{
    return a != b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_gt(int8x32_t a, int8x32_t b)
{
    return a > b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_gtu(int8x32_t a, int8x32_t b)
{
    uint8x32_t ua = (uint8x32_t)a;
    uint8x32_t ub = (uint8x32_t)b;
    return ua > ub;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_eqz(int8x32_t a)
{
    return a == 0;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shl(int8x32_t a, int b)
{
    return a << b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shr(int8x32_t a, int b)
{
    return a >> b;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shru(int8x32_t a, int b)
{
    uint8x32_t ua = (uint8x32_t)a;
    return (int8x32_t)(ua >> b);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shl3(int8x32_t a)
{
    return a << 3;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shr5(int8x32_t a)
{
    return a >> 5;
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shru7(int8x32_t a)
{
    uint8x32_t ua = (uint8x32_t)a;
    return (int8x32_t)(ua >> 7);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_load(int8x32_t *p)
{
    return __builtin_kvx_lbv(p, ".s", 1);
}
void __attribute ((noinline))
kvx_int8x32_store(int8x32_t *p, int8x32_t a)
{
    __builtin_kvx_sbv(p, a, 0);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shift(int8x32_t a, int8_t b)
{
    return __builtin_kvx_shiftbv(a, 1, b);
}
