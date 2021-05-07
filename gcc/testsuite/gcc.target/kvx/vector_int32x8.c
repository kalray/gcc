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

int32x8_t __attribute ((noinline))
kvx_int32x8_incr(int32x8_t a)
{
    return a + 1;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_addto(int32x8_t a)
{
    int32x8_t b = { 1, 2, 3, 4, 5, 6, 7, 8 };
    return a + b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_subfrom(int32x8_t a)
{
    int32x8_t b = { 1, 2, 3, 4, 5, 6, 7, 8 };
    return b - a;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_addwhere(int32x8_t a, int32x8_t __bypass *b)
{
    return a + *b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_asr(int32x8_t a, int b)
{
    return a >> b;
}
uint32x8_t __attribute ((noinline))
kvx_int32x8_lsr(uint32x8_t a, int b)
{
    return a >> b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_lsl(int32x8_t a, int b)
{
    return a << b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_addx2(int32x8_t a, int32x8_t b)
{
    return a + (b << 1);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_addx4(int32x8_t a, int32x8_t b)
{
    return a + (b << 2);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_addx8(int32x8_t a, int32x8_t b)
{
    return a + (b << 3);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_addx16(int32x8_t a, int32x8_t b)
{
    return a + (b << 4);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_sbfx2(int32x8_t a, int32x8_t b)
{
    return a - b*2;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_sbfx4(int32x8_t a, int32x8_t b)
{
    return a - b*4;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_sbfx8(int32x8_t a, int32x8_t b)
{
    return a - b*8;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_sbfx16(int32x8_t a, int32x8_t b)
{
    return a - b*16;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_mul(int32x8_t a, int32x8_t b)
{
    return a * b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_muls(int32x8_t a, int32_t b)
{
    return a * b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_madd(int32x8_t a, int32x8_t b, int32x8_t c)
{
    return a * b + c;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_msub(int32x8_t a, int32x8_t b, int32x8_t c)
{
    return c - a * b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_subx2(int32x8_t a, int32x8_t b)
{
    return a - (b << 1);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_subx4(int32x8_t a, int32x8_t b)
{
    return a - (b << 2);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_subx8(int32x8_t a, int32x8_t b)
{
    return a - (b << 3);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_subx16(int32x8_t a, int32x8_t b)
{
    return a - (b << 4);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_neg(int32x8_t a, int32x8_t b)
{
    return -a;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_not(int32x8_t a, int32x8_t b)
{
    return ~a;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_and(int32x8_t a, int32x8_t b)
{
    return a & b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_nand(int32x8_t a, int32x8_t b)
{
    return ~(a & b);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_or(int32x8_t a, int32x8_t b)
{
    return a | b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_nor(int32x8_t a, int32x8_t b)
{
    return ~(a | b);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_xor(int32x8_t a, int32x8_t b)
{
    return a ^ b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_nxor(int32x8_t a, int32x8_t b)
{
    return ~(a ^ b);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_andn(int32x8_t a, int32x8_t b)
{
    return ~a & b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_orn(int32x8_t a, int32x8_t b)
{
    return ~a | b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_xorn(int32x8_t a, int32x8_t b)
{
    return ~a ^ b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_ne(int32x8_t a, int32x8_t b)
{
    return a != b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_gt(int32x8_t a, int32x8_t b)
{
    return a > b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_gtu(int32x8_t a, int32x8_t b)
{
    uint32x8_t ua = (uint32x8_t)a;
    uint32x8_t ub = (uint32x8_t)b;
    return ua > ub;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_eqz(int32x8_t a)
{
    return a == 0;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shl(int32x8_t a, int b)
{
    return a << b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shr(int32x8_t a, int b)
{
    return a >> b;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shru(int32x8_t a, int b)
{
    uint32x8_t ua = (uint32x8_t)a;
    return (int32x8_t)(ua >> b);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shl3(int32x8_t a)
{
    return a << 3;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shr5(int32x8_t a)
{
    return a >> 5;
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shru7(int32x8_t a)
{
    uint32x8_t ua = (uint32x8_t)a;
    return (int32x8_t)(ua >> 7);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_load(int32x8_t *p)
{
    return __builtin_kvx_lwo(p, ".s", 1);
}
void __attribute ((noinline))
kvx_int32x8_store(int32x8_t *p, int32x8_t a)
{
    __builtin_kvx_swo(p, a, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_neg_(int32x8_t a)
{
    return __builtin_kvx_negwo(a, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_neg_s(int32x8_t a)
{
    return __builtin_kvx_negwo(a, ".s");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_abs_(int32x8_t a)
{
    return __builtin_kvx_abswo(a, "");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_abs_s(int32x8_t a)
{
    return __builtin_kvx_abswo(a, ".s");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_abd_(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_abdwo(a, b, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_abd_s(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_abdwo(a, b, ".s");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_abds_(int32x8_t a, int32_t b)
{
    return __builtin_kvx_abdwo(a, b-(int32x8_t){0}, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_abd0_(int32x8_t a)
{
    return __builtin_kvx_abdwo(a, (int32x8_t){0}, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_abd0_s(int32x8_t a)
{
    return __builtin_kvx_abdwo(a, (int32x8_t){0}, ".s");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_add_s(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_addwo(a, b, ".s");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_add_us(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_addwo(a, b, ".us");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_adds_(int32x8_t a, int32_t b)
{
    return __builtin_kvx_addwo(a, b-(int32x8_t){0}, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_adds_s(int32x8_t a, int32_t b)
{
    return __builtin_kvx_addwo(a, b-(int32x8_t){0}, ".s");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_adds_us(int32x8_t a, int32_t b)
{
    return __builtin_kvx_addwo(a, b-(int32x8_t){0}, ".us");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_sbf_s(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_sbfwo(a, b, ".s");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_sbf_us(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_sbfwo(a, b, ".us");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_sbfs_(int32x8_t a, int32_t b)
{
    return __builtin_kvx_sbfwo(a, b-(int32x8_t){0}, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_sbfs_s(int32x8_t a, int32_t b)
{
    return __builtin_kvx_sbfwo(a, b-(int32x8_t){0}, ".s");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_sbfs_us(int32x8_t a, int32_t b)
{
    return __builtin_kvx_sbfwo(a, b-(int32x8_t){0}, ".us");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_min(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_minwo(a, b);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_max(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_maxwo(a, b);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_minu(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_minuwo(a, b);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_maxu(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_maxuwo(a, b);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_mins(int32x8_t a, int32_t b)
{
    return __builtin_kvx_minwo(a, b-(int32x8_t){0});
}
int32x8_t __attribute ((noinline))
kvx_int32x8_maxs(int32x8_t a, int32_t b)
{
    return __builtin_kvx_maxwo(a, b-(int32x8_t){0});
}
int32x8_t __attribute ((noinline))
kvx_int32x8_minus(int32x8_t a, int32_t b)
{
    return __builtin_kvx_minuwo(a, b-(int32x8_t){0});
}
int32x8_t __attribute ((noinline))
kvx_int32x8_maxus(int32x8_t a, int32_t b)
{
    return __builtin_kvx_maxuwo(a, b-(int32x8_t){0});
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shls_(int32x8_t a, int32_t b)
{
    return __builtin_kvx_shlwos(a, b, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shl3_(int32x8_t a)
{
    return __builtin_kvx_shlwos(a, 3, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shls_s(int32x8_t a, int32_t b)
{
    return __builtin_kvx_shlwos(a, b, ".s");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shl5_s(int32x8_t a)
{
    return __builtin_kvx_shlwos(a, 5, ".s");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shls_us(int32x8_t a, int32_t b)
{
    return __builtin_kvx_shlwos(a, b, ".us");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shl7_us(int32x8_t a)
{
    return __builtin_kvx_shlwos(a, 7, ".us");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shls_r(int32x8_t a, int32_t b)
{
    return __builtin_kvx_shlwos(a, b, ".r");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shl1_r(int32x8_t a)
{
    return __builtin_kvx_shlwos(a, 1, ".r");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shrs_(int32x8_t a, int32_t b)
{
    return __builtin_kvx_shrwos(a, b, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shr5_(int32x8_t a)
{
    return __builtin_kvx_shrwos(a, 5, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shrs_a(int32x8_t a, int32_t b)
{
    return __builtin_kvx_shrwos(a, b, ".a");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shr3_a(int32x8_t a)
{
    return __builtin_kvx_shrwos(a, 3, ".a");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shrs_ar(int32x8_t a, int32_t b)
{
    return __builtin_kvx_shrwos(a, b, ".ar");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shr7_ar(int32x8_t a)
{
    return __builtin_kvx_shrwos(a, 7, ".ar");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shrs_r(int32x8_t a, int32_t b)
{
    return __builtin_kvx_shrwos(a, b, ".r");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shr1_r(int32x8_t a)
{
    return __builtin_kvx_shrwos(a, 1, ".r");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_bitcnt_(int32x8_t a)
{
    return __builtin_kvx_bitcntwo(a, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_bitcnt_lz(int32x8_t a)
{
    return __builtin_kvx_bitcntwo(a, ".lz");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_bitcnt_ls(int32x8_t a)
{
    return __builtin_kvx_bitcntwo(a, ".ls");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_bitcnt_tz(int32x8_t a)
{
    return __builtin_kvx_bitcntwo(a, ".tz");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_avg(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_avgwo(a, b, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_avg_u(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_avgwo(a, b, ".u");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_avg_r(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_avgwo(a, b, ".r");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_avg_ru(int32x8_t a, int32x8_t b)
{
    return __builtin_kvx_avgwo(a, b, ".ru");
}
int32x8_t __attribute ((noinline))
kvx_int32x8_select(int32x8_t a, int32x8_t b, int32x8_t c)
{
    return __builtin_kvx_selectwo(a, b, c, 0);
}
int32x8_t __attribute ((noinline))
kvx_int32x8_shift(int32x8_t a, int32_t b)
{
    return __builtin_kvx_shiftwo(a, 1, b);
}
