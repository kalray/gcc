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

int16x4_t __attribute ((noinline))
kvx_int16x4_incr(int16x4_t a)
{
    return a + 1;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_addto(int16x4_t a)
{
    int16x4_t b = { 1, 2, 3, 4 };
    return a + b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_subfrom(int16x4_t a)
{
    int16x4_t b = { 1, 2, 3, 4 };
    return b - a;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_addwhere(int16x4_t a, int16x4_t __bypass *b)
{
    return a + *b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_asr(int16x4_t a, int b)
{
    return a >> b;
}
uint16x4_t __attribute ((noinline))
kvx_int16x4_lsr(uint16x4_t a, int b)
{
    return a >> b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_lsl(int16x4_t a, int b)
{
    return a << b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_addx2(int16x4_t a, int16x4_t b)
{
    return a + (b << 1);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_addx4(int16x4_t a, int16x4_t b)
{
    return a + (b << 2);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_addx8(int16x4_t a, int16x4_t b)
{
    return a + (b << 3);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_addx16(int16x4_t a, int16x4_t b)
{
    return a + (b << 4);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_sbfx2(int16x4_t a, int16x4_t b)
{
    return a - b*2;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_sbfx4(int16x4_t a, int16x4_t b)
{
    return a - b*4;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_sbfx8(int16x4_t a, int16x4_t b)
{
    return a - b*8;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_sbfx16(int16x4_t a, int16x4_t b)
{
    return a - b*16;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_mul(int16x4_t a, int16x4_t b)
{
    return a * b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_muls(int16x4_t a, int16_t b)
{
    return a * b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_madd(int16x4_t a, int16x4_t b, int16x4_t c)
{
    return a * b + c;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_msub(int16x4_t a, int16x4_t b, int16x4_t c)
{
    return c - a * b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_subx2(int16x4_t a, int16x4_t b)
{
    return a - (b << 1);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_subx4(int16x4_t a, int16x4_t b)
{
    return a - (b << 2);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_subx8(int16x4_t a, int16x4_t b)
{
    return a - (b << 3);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_subx16(int16x4_t a, int16x4_t b)
{
    return a - (b << 4);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_neg(int16x4_t a, int16x4_t b)
{
    return -a;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_not(int16x4_t a, int16x4_t b)
{
    return ~a;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_and(int16x4_t a, int16x4_t b)
{
    return a & b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_nand(int16x4_t a, int16x4_t b)
{
    return ~(a & b);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_or(int16x4_t a, int16x4_t b)
{
    return a | b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_nor(int16x4_t a, int16x4_t b)
{
    return ~(a | b);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_xor(int16x4_t a, int16x4_t b)
{
    return a ^ b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_nxor(int16x4_t a, int16x4_t b)
{
    return ~(a ^ b);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_andn(int16x4_t a, int16x4_t b)
{
    return ~a & b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_orn(int16x4_t a, int16x4_t b)
{
    return ~a | b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_xorn(int16x4_t a, int16x4_t b)
{
    return ~a ^ b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_ne(int16x4_t a, int16x4_t b)
{
    return a != b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_gt(int16x4_t a, int16x4_t b)
{
    return a > b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_gtu(int16x4_t a, int16x4_t b)
{
    uint16x4_t ua = (uint16x4_t)a;
    uint16x4_t ub = (uint16x4_t)b;
    return ua > ub;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_eqz(int16x4_t a)
{
    return a == 0;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shl(int16x4_t a, int b)
{
    return a << b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shr(int16x4_t a, int b)
{
    return a >> b;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shru(int16x4_t a, int b)
{
    uint16x4_t ua = (uint16x4_t)a;
    return (int16x4_t)(ua >> b);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shl3(int16x4_t a)
{
    return a << 3;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shr5(int16x4_t a)
{
    return a >> 5;
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shru7(int16x4_t a)
{
    uint16x4_t ua = (uint16x4_t)a;
    return (int16x4_t)(ua >> 7);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_load(int16x4_t *p)
{
    return __builtin_kvx_lhq(p, ".s", 1);
}
void __attribute ((noinline))
kvx_int16x4_store(int16x4_t *p, int16x4_t a)
{
    __builtin_kvx_shq(p, a, 0);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_neg_(int16x4_t a)
{
    return __builtin_kvx_neghq(a, 0);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_neg_s(int16x4_t a)
{
    return __builtin_kvx_neghq(a, ".s");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_abs_(int16x4_t a)
{
    return __builtin_kvx_abshq(a, "");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_abs_s(int16x4_t a)
{
    return __builtin_kvx_abshq(a, ".s");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_abd_(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_abdhq(a, b, 0);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_abd_s(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_abdhq(a, b, ".s");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_abds_(int16x4_t a, int16_t b)
{
    return __builtin_kvx_abdhq(a, b-(int16x4_t){0}, 0);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_abd0_(int16x4_t a)
{
    return __builtin_kvx_abdhq(a, (int16x4_t){0}, 0);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_abd0_s(int16x4_t a)
{
    return __builtin_kvx_abdhq(a, (int16x4_t){0}, ".s");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_add_s(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_addhq(a, b, ".s");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_add_us(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_addhq(a, b, ".us");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_adds_(int16x4_t a, int16_t b)
{
    return __builtin_kvx_addhq(a, b-(int16x4_t){0}, 0);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_adds_s(int16x4_t a, int16_t b)
{
    return __builtin_kvx_addhq(a, b-(int16x4_t){0}, ".s");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_adds_us(int16x4_t a, int16_t b)
{
    return __builtin_kvx_addhq(a, b-(int16x4_t){0}, ".us");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_sbf_s(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_sbfhq(a, b, ".s");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_sbf_us(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_sbfhq(a, b, ".us");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_sbfs_(int16x4_t a, int16_t b)
{
    return __builtin_kvx_sbfhq(a, b-(int16x4_t){0}, 0);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_sbfs_s(int16x4_t a, int16_t b)
{
    return __builtin_kvx_sbfhq(a, b-(int16x4_t){0}, ".s");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_sbfs_us(int16x4_t a, int16_t b)
{
    return __builtin_kvx_sbfhq(a, b-(int16x4_t){0}, ".us");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_min(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_minhq(a, b);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_max(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_maxhq(a, b);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_minu(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_minuhq(a, b);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_maxu(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_maxuhq(a, b);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_mins(int16x4_t a, int16_t b)
{
    return __builtin_kvx_minhq(a, b-(int16x4_t){0});
}
int16x4_t __attribute ((noinline))
kvx_int16x4_maxs(int16x4_t a, int16_t b)
{
    return __builtin_kvx_maxhq(a, b-(int16x4_t){0});
}
int16x4_t __attribute ((noinline))
kvx_int16x4_minus(int16x4_t a, int16_t b)
{
    return __builtin_kvx_minuhq(a, b-(int16x4_t){0});
}
int16x4_t __attribute ((noinline))
kvx_int16x4_maxus(int16x4_t a, int16_t b)
{
    return __builtin_kvx_maxuhq(a, b-(int16x4_t){0});
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shls_(int16x4_t a, int16_t b)
{
    return __builtin_kvx_shlhqs(a, b, 0);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shl3_(int16x4_t a)
{
    return __builtin_kvx_shlhqs(a, 3, 0);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shls_s(int16x4_t a, int16_t b)
{
    return __builtin_kvx_shlhqs(a, b, ".s");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shl5_s(int16x4_t a)
{
    return __builtin_kvx_shlhqs(a, 5, ".s");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shls_us(int16x4_t a, int16_t b)
{
    return __builtin_kvx_shlhqs(a, b, ".us");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shl7_us(int16x4_t a)
{
    return __builtin_kvx_shlhqs(a, 7, ".us");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shls_r(int16x4_t a, int16_t b)
{
    return __builtin_kvx_shlhqs(a, b, ".r");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shl1_r(int16x4_t a)
{
    return __builtin_kvx_shlhqs(a, 1, ".r");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shrs_(int16x4_t a, int16_t b)
{
    return __builtin_kvx_shrhqs(a, b, 0);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shr5_(int16x4_t a)
{
    return __builtin_kvx_shrhqs(a, 5, 0);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shrs_a(int16x4_t a, int16_t b)
{
    return __builtin_kvx_shrhqs(a, b, ".a");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shr3_a(int16x4_t a)
{
    return __builtin_kvx_shrhqs(a, 3, ".a");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shrs_ar(int16x4_t a, int16_t b)
{
    return __builtin_kvx_shrhqs(a, b, ".ar");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shr7_ar(int16x4_t a)
{
    return __builtin_kvx_shrhqs(a, 7, ".ar");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shrs_r(int16x4_t a, int16_t b)
{
    return __builtin_kvx_shrhqs(a, b, ".r");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shr1_r(int16x4_t a)
{
    return __builtin_kvx_shrhqs(a, 1, ".r");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_avg(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_avghq(a, b, 0);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_avg_u(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_avghq(a, b, ".u");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_avg_r(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_avghq(a, b, ".r");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_avg_ru(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_avghq(a, b, ".ru");
}
int32x4_t __attribute__ ((noinline))
kvx_int16x4_mul_(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_mulhwq(a, b, 0);
}
int32x4_t __attribute__ ((noinline))
kvx_int16x4_mul_u(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_mulhwq(a, b, ".u");
}
int32x4_t __attribute__ ((noinline))
kvx_int16x4_mul_su(int16x4_t a, int16x4_t b)
{
    return __builtin_kvx_mulhwq(a, b, ".su");
}
int32x4_t __attribute__ ((noinline))
kvx_int16x4_madd_(int16x4_t a, int16x4_t b, int32x4_t c)
{
    return __builtin_kvx_maddhwq(a, b, c, 0);
}
int32x4_t __attribute__ ((noinline))
kvx_int16x4_madd_u(int16x4_t a, int16x4_t b, int32x4_t c)
{
    return __builtin_kvx_maddhwq(a, b, c, ".u");
}
int32x4_t __attribute__ ((noinline))
kvx_int16x4_madd_su(int16x4_t a, int16x4_t b, int32x4_t c)
{
    return __builtin_kvx_maddhwq(a, b, c, ".su");
}
int32x4_t __attribute__ ((noinline))
kvx_int16x4_msbf_(int16x4_t a, int16x4_t b, int32x4_t c)
{
    return __builtin_kvx_msbfhwq(a, b, c, 0);
}
int32x4_t __attribute__ ((noinline))
kvx_int16x4_msbf_u(int16x4_t a, int16x4_t b, int32x4_t c)
{
    return __builtin_kvx_msbfhwq(a, b, c, ".u");
}
int32x4_t __attribute__ ((noinline))
kvx_int16x4_msbf_su(int16x4_t a, int16x4_t b, int32x4_t c)
{
    return __builtin_kvx_msbfhwq(a, b, c, ".su");
}
int32x4_t __attribute__ ((noinline))
kvx_int16x4_sxhwq(int16x4_t a)
{
    return __builtin_kvx_widenhwq(a, 0);
}
int32x4_t __attribute__ ((noinline))
kvx_int16x4_zxhwq(int16x4_t a)
{
    return __builtin_kvx_widenhwq(a, ".z");
}
int32x4_t __attribute__ ((noinline))
kvx_int16x4_qxhwq(int16x4_t a)
{
    return __builtin_kvx_widenhwq(a, ".q");
}
int16x4_t __attribute__ ((noinline))
kvx_int16x4_truncwhq(int32x4_t a)
{
    return __builtin_kvx_narrowwhq(a, 0);
}
int16x4_t __attribute__ ((noinline))
kvx_int16x4_fractwhq(int32x4_t a)
{
    return __builtin_kvx_narrowwhq(a, ".q");
}
int16x4_t __attribute__ ((noinline))
kvx_int16x4_satwhq(int32x4_t a)
{
    return __builtin_kvx_narrowwhq(a, ".s");
}
int16x4_t __attribute__ ((noinline))
kvx_int16x4_satuwhq(int32x4_t a)
{
    return __builtin_kvx_narrowwhq(a, ".us");
}
int16x4_t __attribute ((noinline))
kvx_int16x4_select(int16x4_t a, int16x4_t b, int16x4_t c)
{
    return __builtin_kvx_selecthq(a, b, c, 0);
}
int16x4_t __attribute ((noinline))
kvx_int16x4_shift(int16x4_t a, int16_t b)
{
    return __builtin_kvx_shifthq(a, 1, b);
}
