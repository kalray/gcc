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
    return __builtin_kvx_load256(p, ".s", 1);
}
void __attribute ((noinline))
kvx_int8x32_store(int8x32_t *p, int8x32_t a)
{
    __builtin_kvx_store256(a, p, 0);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_neg_(int8x32_t a)
{
    return __builtin_kvx_negbv(a, 0);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_neg_s(int8x32_t a)
{
    return __builtin_kvx_negbv(a, ".s");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_abs_(int8x32_t a)
{
    return __builtin_kvx_absbv(a, "");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_abs_s(int8x32_t a)
{
    return __builtin_kvx_absbv(a, ".s");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_abd_(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_abdbv(a, b, 0);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_abd_s(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_abdbv(a, b, ".s");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_abds_(int8x32_t a, int8_t b)
{
    return __builtin_kvx_abdbv(a, b-(int8x32_t){0}, 0);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_abd0_(int8x32_t a)
{
    return __builtin_kvx_abdbv(a, (int8x32_t){0}, 0);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_abd0_s(int8x32_t a)
{
    return __builtin_kvx_abdbv(a, (int8x32_t){0}, ".s");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_add_s(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_addbv(a, b, ".s");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_add_us(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_addbv(a, b, ".us");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_adds_(int8x32_t a, int8_t b)
{
    return __builtin_kvx_addbv(a, b-(int8x32_t){0}, 0);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_adds_s(int8x32_t a, int8_t b)
{
    return __builtin_kvx_addbv(a, b-(int8x32_t){0}, ".s");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_adds_us(int8x32_t a, int8_t b)
{
    return __builtin_kvx_addbv(a, b-(int8x32_t){0}, ".us");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_sbf_s(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_sbfbv(a, b, ".s");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_sbf_us(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_sbfbv(a, b, ".us");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_sbfs_(int8x32_t a, int8_t b)
{
    return __builtin_kvx_sbfbv(a, b-(int8x32_t){0}, 0);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_sbfs_s(int8x32_t a, int8_t b)
{
    return __builtin_kvx_sbfbv(a, b-(int8x32_t){0}, ".s");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_sbfs_us(int8x32_t a, int8_t b)
{
    return __builtin_kvx_sbfbv(a, b-(int8x32_t){0}, ".us");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_min(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_minbv(a, b);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_max(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_maxbv(a, b);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_minu(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_minubv(a, b);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_maxu(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_maxubv(a, b);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_mins(int8x32_t a, int8_t b)
{
    return __builtin_kvx_minbv(a, b-(int8x32_t){0});
}
int8x32_t __attribute ((noinline))
kvx_int8x32_maxs(int8x32_t a, int8_t b)
{
    return __builtin_kvx_maxbv(a, b-(int8x32_t){0});
}
int8x32_t __attribute ((noinline))
kvx_int8x32_minus(int8x32_t a, int8_t b)
{
    return __builtin_kvx_minubv(a, b-(int8x32_t){0});
}
int8x32_t __attribute ((noinline))
kvx_int8x32_maxus(int8x32_t a, int8_t b)
{
    return __builtin_kvx_maxubv(a, b-(int8x32_t){0});
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shls_(int8x32_t a, int8_t b)
{
    return __builtin_kvx_shlbvs(a, b, 0);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shl3_(int8x32_t a)
{
    return __builtin_kvx_shlbvs(a, 3, 0);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shls_s(int8x32_t a, int8_t b)
{
    return __builtin_kvx_shlbvs(a, b, ".s");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shl5_s(int8x32_t a)
{
    return __builtin_kvx_shlbvs(a, 5, ".s");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shls_us(int8x32_t a, int8_t b)
{
    return __builtin_kvx_shlbvs(a, b, ".us");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shl7_us(int8x32_t a)
{
    return __builtin_kvx_shlbvs(a, 7, ".us");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shls_r(int8x32_t a, int8_t b)
{
    return __builtin_kvx_shlbvs(a, b, ".r");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shl1_r(int8x32_t a)
{
    return __builtin_kvx_shlbvs(a, 1, ".r");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shrs_(int8x32_t a, int8_t b)
{
    return __builtin_kvx_shrbvs(a, b, 0);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shr5_(int8x32_t a)
{
    return __builtin_kvx_shrbvs(a, 5, 0);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shrs_a(int8x32_t a, int8_t b)
{
    return __builtin_kvx_shrbvs(a, b, ".a");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shr3_a(int8x32_t a)
{
    return __builtin_kvx_shrbvs(a, 3, ".a");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shrs_as(int8x32_t a, int8_t b)
{
    return __builtin_kvx_shrbvs(a, b, ".as");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shr7_as(int8x32_t a)
{
    return __builtin_kvx_shrbvs(a, 7, ".as");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shrs_r(int8x32_t a, int8_t b)
{
    return __builtin_kvx_shrbvs(a, b, ".r");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shr1_r(int8x32_t a)
{
    return __builtin_kvx_shrbvs(a, 1, ".r");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_avg(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_avgbv(a, b, 0);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_avg_u(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_avgbv(a, b, ".u");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_avg_r(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_avgbv(a, b, ".r");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_avg_ru(int8x32_t a, int8x32_t b)
{
    return __builtin_kvx_avgbv(a, b, ".ru");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_select(int8x32_t a, int8x32_t b, int8x32_t c)
{
    return __builtin_kvx_selectbv(a, b, c, ".nez");
}
uint64_t __attribute__ ((noinline))
kvx_int8x32_any_eqz(int8x32_t a)
{
    return __builtin_kvx_anybv(a, ".eqz");
}
uint64_t __attribute__ ((noinline))
kvx_int8x32_any_nez(int8x32_t a)
{
    return __builtin_kvx_anybv(a, ".nez");
}
int8x32_t __attribute ((noinline))
kvx_int8x32_shift(int8x32_t a, int8_t b)
{
    return __builtin_kvx_shiftbv(a, 1, b);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_cat256(int8x16_t a, int8x16_t b) {
    return __builtin_kvx_cat256(a, b);
}
int8x32_t __attribute ((noinline))
kvx_int8x32_scat256(int8x16_t a, int8x16_t b) {
    return __builtin_kvx_cat256(b, a);
}
int8x16_t __attribute ((noinline))
kvx_int8x32_low128(int8x32_t a) {
    return __builtin_kvx_low128(a);
}
int8x16_t __attribute ((noinline))
kvx_int8x32_high128(int8x32_t a) {
    return __builtin_kvx_high128(a);
}
