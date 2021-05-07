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

int16x16_t __attribute ((noinline))
kvx_int16x16_incr(int16x16_t a)
{
    return a + 1;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_addto(int16x16_t a)
{
    int16x16_t b = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    return a + b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_subfrom(int16x16_t a)
{
    int16x16_t b = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    return b - a;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_addwhere(int16x16_t a, int16x16_t __bypass *b)
{
    return a + *b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_asr(int16x16_t a, int b)
{
    return a >> b;
}
uint16x16_t __attribute ((noinline))
kvx_int16x16_lsr(uint16x16_t a, int b)
{
    return a >> b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_lsl(int16x16_t a, int b)
{
    return a << b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_addx2(int16x16_t a, int16x16_t b)
{
    return a + (b << 1);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_addx4(int16x16_t a, int16x16_t b)
{
    return a + (b << 2);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_addx8(int16x16_t a, int16x16_t b)
{
    return a + (b << 3);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_addx16(int16x16_t a, int16x16_t b)
{
    return a + (b << 4);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_sbfx2(int16x16_t a, int16x16_t b)
{
    return a - b*2;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_sbfx4(int16x16_t a, int16x16_t b)
{
    return a - b*4;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_sbfx8(int16x16_t a, int16x16_t b)
{
    return a - b*8;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_sbfx16(int16x16_t a, int16x16_t b)
{
    return a - b*16;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_mul(int16x16_t a, int16x16_t b)
{
    return a * b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_muls(int16x16_t a, int16_t b)
{
    return a * b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_madd(int16x16_t a, int16x16_t b, int16x16_t c)
{
    return a * b + c;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_msub(int16x16_t a, int16x16_t b, int16x16_t c)
{
    return c - a * b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_subx2(int16x16_t a, int16x16_t b)
{
    return a - (b << 1);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_subx4(int16x16_t a, int16x16_t b)
{
    return a - (b << 2);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_subx8(int16x16_t a, int16x16_t b)
{
    return a - (b << 3);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_subx16(int16x16_t a, int16x16_t b)
{
    return a - (b << 4);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_neg(int16x16_t a, int16x16_t b)
{
    return -a;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_not(int16x16_t a, int16x16_t b)
{
    return ~a;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_and(int16x16_t a, int16x16_t b)
{
    return a & b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_nand(int16x16_t a, int16x16_t b)
{
    return ~(a & b);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_or(int16x16_t a, int16x16_t b)
{
    return a | b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_nor(int16x16_t a, int16x16_t b)
{
    return ~(a | b);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_xor(int16x16_t a, int16x16_t b)
{
    return a ^ b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_nxor(int16x16_t a, int16x16_t b)
{
    return ~(a ^ b);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_andn(int16x16_t a, int16x16_t b)
{
    return ~a & b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_orn(int16x16_t a, int16x16_t b)
{
    return ~a | b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_xorn(int16x16_t a, int16x16_t b)
{
    return ~a ^ b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_ne(int16x16_t a, int16x16_t b)
{
    return a != b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_gt(int16x16_t a, int16x16_t b)
{
    return a > b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_gtu(int16x16_t a, int16x16_t b)
{
    uint16x16_t ua = (uint16x16_t)a;
    uint16x16_t ub = (uint16x16_t)b;
    return ua > ub;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_eqz(int16x16_t a)
{
    return a == 0;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shl(int16x16_t a, int b)
{
    return a << b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shr(int16x16_t a, int b)
{
    return a >> b;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shru(int16x16_t a, int b)
{
    uint16x16_t ua = (uint16x16_t)a;
    return (int16x16_t)(ua >> b);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shl3(int16x16_t a)
{
    return a << 3;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shr5(int16x16_t a)
{
    return a >> 5;
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shru7(int16x16_t a)
{
    uint16x16_t ua = (uint16x16_t)a;
    return (int16x16_t)(ua >> 7);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_load(int16x16_t *p)
{
    return __builtin_kvx_lhx(p, ".s", 1);
}
void __attribute ((noinline))
kvx_int16x16_store(int16x16_t *p, int16x16_t a)
{
    __builtin_kvx_shx(p, a, 0);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_neg_(int16x16_t a)
{
    return __builtin_kvx_neghx(a, 0);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_neg_s(int16x16_t a)
{
    return __builtin_kvx_neghx(a, ".s");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_abs_(int16x16_t a)
{
    return __builtin_kvx_abshx(a, "");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_abs_s(int16x16_t a)
{
    return __builtin_kvx_abshx(a, ".s");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_abd_(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_abdhx(a, b, 0);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_abd_s(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_abdhx(a, b, ".s");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_abds_(int16x16_t a, int16_t b)
{
    return __builtin_kvx_abdhx(a, b-(int16x16_t){0}, 0);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_abd0_(int16x16_t a)
{
    return __builtin_kvx_abdhx(a, (int16x16_t){0}, 0);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_abd0_s(int16x16_t a)
{
    return __builtin_kvx_abdhx(a, (int16x16_t){0}, ".s");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_add_s(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_addhx(a, b, ".s");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_add_us(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_addhx(a, b, ".us");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_adds_(int16x16_t a, int16_t b)
{
    return __builtin_kvx_addhx(a, b-(int16x16_t){0}, 0);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_adds_s(int16x16_t a, int16_t b)
{
    return __builtin_kvx_addhx(a, b-(int16x16_t){0}, ".s");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_adds_us(int16x16_t a, int16_t b)
{
    return __builtin_kvx_addhx(a, b-(int16x16_t){0}, ".us");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_sbf_s(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_sbfhx(a, b, ".s");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_sbf_us(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_sbfhx(a, b, ".us");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_sbfs_(int16x16_t a, int16_t b)
{
    return __builtin_kvx_sbfhx(a, b-(int16x16_t){0}, 0);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_sbfs_s(int16x16_t a, int16_t b)
{
    return __builtin_kvx_sbfhx(a, b-(int16x16_t){0}, ".s");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_sbfs_us(int16x16_t a, int16_t b)
{
    return __builtin_kvx_sbfhx(a, b-(int16x16_t){0}, ".us");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_min(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_minhx(a, b);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_max(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_maxhx(a, b);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_minu(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_minuhx(a, b);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_maxu(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_maxuhx(a, b);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_mins(int16x16_t a, int16_t b)
{
    return __builtin_kvx_minhx(a, b-(int16x16_t){0});
}
int16x16_t __attribute ((noinline))
kvx_int16x16_maxs(int16x16_t a, int16_t b)
{
    return __builtin_kvx_maxhx(a, b-(int16x16_t){0});
}
int16x16_t __attribute ((noinline))
kvx_int16x16_minus(int16x16_t a, int16_t b)
{
    return __builtin_kvx_minuhx(a, b-(int16x16_t){0});
}
int16x16_t __attribute ((noinline))
kvx_int16x16_maxus(int16x16_t a, int16_t b)
{
    return __builtin_kvx_maxuhx(a, b-(int16x16_t){0});
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shls_(int16x16_t a, int16_t b)
{
    return __builtin_kvx_shlhxs(a, b, 0);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shl3_(int16x16_t a)
{
    return __builtin_kvx_shlhxs(a, 3, 0);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shls_s(int16x16_t a, int16_t b)
{
    return __builtin_kvx_shlhxs(a, b, ".s");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shl5_s(int16x16_t a)
{
    return __builtin_kvx_shlhxs(a, 5, ".s");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shls_us(int16x16_t a, int16_t b)
{
    return __builtin_kvx_shlhxs(a, b, ".us");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shl7_us(int16x16_t a)
{
    return __builtin_kvx_shlhxs(a, 7, ".us");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shls_r(int16x16_t a, int16_t b)
{
    return __builtin_kvx_shlhxs(a, b, ".r");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shl1_r(int16x16_t a)
{
    return __builtin_kvx_shlhxs(a, 1, ".r");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shrs_(int16x16_t a, int16_t b)
{
    return __builtin_kvx_shrhxs(a, b, 0);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shr5_(int16x16_t a)
{
    return __builtin_kvx_shrhxs(a, 5, 0);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shrs_a(int16x16_t a, int16_t b)
{
    return __builtin_kvx_shrhxs(a, b, ".a");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shr3_a(int16x16_t a)
{
    return __builtin_kvx_shrhxs(a, 3, ".a");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shrs_ar(int16x16_t a, int16_t b)
{
    return __builtin_kvx_shrhxs(a, b, ".ar");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shr7_ar(int16x16_t a)
{
    return __builtin_kvx_shrhxs(a, 7, ".ar");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shrs_r(int16x16_t a, int16_t b)
{
    return __builtin_kvx_shrhxs(a, b, ".r");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shr1_r(int16x16_t a)
{
    return __builtin_kvx_shrhxs(a, 1, ".r");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_avg(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_avghx(a, b, 0);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_avg_u(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_avghx(a, b, ".u");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_avg_r(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_avghx(a, b, ".r");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_avg_ru(int16x16_t a, int16x16_t b)
{
    return __builtin_kvx_avghx(a, b, ".ru");
}
int16x16_t __attribute ((noinline))
kvx_int16x16_select(int16x16_t a, int16x16_t b, int16x16_t c)
{
    return __builtin_kvx_selecthx(a, b, c, 0);
}
int16x16_t __attribute ((noinline))
kvx_int16x16_shift(int16x16_t a, int16_t b)
{
    return __builtin_kvx_shifthx(a, 1, b);
}
