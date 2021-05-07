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

int32x4_t __attribute ((noinline))
kvx_int32x4_incr(int32x4_t a)
{
    return a + 1;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_addto(int32x4_t a)
{
    int32x4_t b = { 1, 2, 3, 4 };
    return a + b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_subfrom(int32x4_t a)
{
    int32x4_t b = { 1, 2, 3, 4 };
    return b - a;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_addwhere(int32x4_t a, int32x4_t __bypass *b)
{
    return a + *b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_asr(int32x4_t a, int b)
{
    return a >> b;
}
uint32x4_t __attribute ((noinline))
kvx_int32x4_lsr(uint32x4_t a, int b)
{
    return a >> b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_lsl(int32x4_t a, int b)
{
    return a << b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_addx2(int32x4_t a, int32x4_t b)
{
    return a + (b << 1);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_addx4(int32x4_t a, int32x4_t b)
{
    return a + (b << 2);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_addx8(int32x4_t a, int32x4_t b)
{
    return a + (b << 3);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_addx16(int32x4_t a, int32x4_t b)
{
    return a + (b << 4);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_sbfx2(int32x4_t a, int32x4_t b)
{
    return a - b*2;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_sbfx4(int32x4_t a, int32x4_t b)
{
    return a - b*4;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_sbfx8(int32x4_t a, int32x4_t b)
{
    return a - b*8;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_sbfx16(int32x4_t a, int32x4_t b)
{
    return a - b*16;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_mul(int32x4_t a, int32x4_t b)
{
    return a * b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_muls(int32x4_t a, int32_t b)
{
    return a * b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_madd(int32x4_t a, int32x4_t b, int32x4_t c)
{
    return a * b + c;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_msub(int32x4_t a, int32x4_t b, int32x4_t c)
{
    return c - a * b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_subx2(int32x4_t a, int32x4_t b)
{
    return a - (b << 1);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_subx4(int32x4_t a, int32x4_t b)
{
    return a - (b << 2);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_subx8(int32x4_t a, int32x4_t b)
{
    return a - (b << 3);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_subx16(int32x4_t a, int32x4_t b)
{
    return a - (b << 4);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_neg(int32x4_t a, int32x4_t b)
{
    return -a;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_not(int32x4_t a, int32x4_t b)
{
    return ~a;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_and(int32x4_t a, int32x4_t b)
{
    return a & b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_nand(int32x4_t a, int32x4_t b)
{
    return ~(a & b);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_or(int32x4_t a, int32x4_t b)
{
    return a | b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_nor(int32x4_t a, int32x4_t b)
{
    return ~(a | b);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_xor(int32x4_t a, int32x4_t b)
{
    return a ^ b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_nxor(int32x4_t a, int32x4_t b)
{
    return ~(a ^ b);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_andn(int32x4_t a, int32x4_t b)
{
    return ~a & b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_orn(int32x4_t a, int32x4_t b)
{
    return ~a | b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_xorn(int32x4_t a, int32x4_t b)
{
    return ~a ^ b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_ne(int32x4_t a, int32x4_t b)
{
    return a != b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_gt(int32x4_t a, int32x4_t b)
{
    return a > b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_gtu(int32x4_t a, int32x4_t b)
{
    uint32x4_t ua = (uint32x4_t)a;
    uint32x4_t ub = (uint32x4_t)b;
    return ua > ub;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_eqz(int32x4_t a)
{
    return a == 0;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shl(int32x4_t a, int b)
{
    return a << b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shr(int32x4_t a, int b)
{
    return a >> b;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shru(int32x4_t a, int b)
{
    uint32x4_t ua = (uint32x4_t)a;
    return (int32x4_t)(ua >> b);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shl3(int32x4_t a)
{
    return a << 3;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shr5(int32x4_t a)
{
    return a >> 5;
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shru7(int32x4_t a)
{
    uint32x4_t ua = (uint32x4_t)a;
    return (int32x4_t)(ua >> 7);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_load(int32x4_t *p)
{
    return __builtin_kvx_lwq(p, ".s", 1);
}
void __attribute ((noinline))
kvx_int32x4_store(int32x4_t *p, int32x4_t a)
{
    __builtin_kvx_swq(p, a, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_neg_(int32x4_t a)
{
    return __builtin_kvx_negwq(a, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_neg_s(int32x4_t a)
{
    return __builtin_kvx_negwq(a, ".s");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_abs_(int32x4_t a)
{
    return __builtin_kvx_abswq(a, "");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_abs_s(int32x4_t a)
{
    return __builtin_kvx_abswq(a, ".s");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_abd_(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_abdwq(a, b, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_abd_s(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_abdwq(a, b, ".s");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_abds_(int32x4_t a, int32_t b)
{
    return __builtin_kvx_abdwq(a, b-(int32x4_t){0}, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_abd0_(int32x4_t a)
{
    return __builtin_kvx_abdwq(a, (int32x4_t){0}, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_abd0_s(int32x4_t a)
{
    return __builtin_kvx_abdwq(a, (int32x4_t){0}, ".s");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_add_s(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_addwq(a, b, ".s");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_add_us(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_addwq(a, b, ".us");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_adds_(int32x4_t a, int32_t b)
{
    return __builtin_kvx_addwq(a, b-(int32x4_t){0}, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_adds_s(int32x4_t a, int32_t b)
{
    return __builtin_kvx_addwq(a, b-(int32x4_t){0}, ".s");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_adds_us(int32x4_t a, int32_t b)
{
    return __builtin_kvx_addwq(a, b-(int32x4_t){0}, ".us");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_sbf_s(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_sbfwq(a, b, ".s");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_sbf_us(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_sbfwq(a, b, ".us");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_sbfs_(int32x4_t a, int32_t b)
{
    return __builtin_kvx_sbfwq(a, b-(int32x4_t){0}, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_sbfs_s(int32x4_t a, int32_t b)
{
    return __builtin_kvx_sbfwq(a, b-(int32x4_t){0}, ".s");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_sbfs_us(int32x4_t a, int32_t b)
{
    return __builtin_kvx_sbfwq(a, b-(int32x4_t){0}, ".us");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_min(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_minwq(a, b);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_max(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_maxwq(a, b);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_minu(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_minuwq(a, b);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_maxu(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_maxuwq(a, b);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_mins(int32x4_t a, int32_t b)
{
    return __builtin_kvx_minwq(a, b-(int32x4_t){0});
}
int32x4_t __attribute ((noinline))
kvx_int32x4_maxs(int32x4_t a, int32_t b)
{
    return __builtin_kvx_maxwq(a, b-(int32x4_t){0});
}
int32x4_t __attribute ((noinline))
kvx_int32x4_minus(int32x4_t a, int32_t b)
{
    return __builtin_kvx_minuwq(a, b-(int32x4_t){0});
}
int32x4_t __attribute ((noinline))
kvx_int32x4_maxus(int32x4_t a, int32_t b)
{
    return __builtin_kvx_maxuwq(a, b-(int32x4_t){0});
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shls_(int32x4_t a, int32_t b)
{
    return __builtin_kvx_shlwqs(a, b, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shl3_(int32x4_t a)
{
    return __builtin_kvx_shlwqs(a, 3, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shls_s(int32x4_t a, int32_t b)
{
    return __builtin_kvx_shlwqs(a, b, ".s");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shl5_s(int32x4_t a)
{
    return __builtin_kvx_shlwqs(a, 5, ".s");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shls_us(int32x4_t a, int32_t b)
{
    return __builtin_kvx_shlwqs(a, b, ".us");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shl7_us(int32x4_t a)
{
    return __builtin_kvx_shlwqs(a, 7, ".us");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shls_r(int32x4_t a, int32_t b)
{
    return __builtin_kvx_shlwqs(a, b, ".r");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shl1_r(int32x4_t a)
{
    return __builtin_kvx_shlwqs(a, 1, ".r");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shrs_(int32x4_t a, int32_t b)
{
    return __builtin_kvx_shrwqs(a, b, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shr5_(int32x4_t a)
{
    return __builtin_kvx_shrwqs(a, 5, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shrs_a(int32x4_t a, int32_t b)
{
    return __builtin_kvx_shrwqs(a, b, ".a");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shr3_a(int32x4_t a)
{
    return __builtin_kvx_shrwqs(a, 3, ".a");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shrs_ar(int32x4_t a, int32_t b)
{
    return __builtin_kvx_shrwqs(a, b, ".ar");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shr7_ar(int32x4_t a)
{
    return __builtin_kvx_shrwqs(a, 7, ".ar");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shrs_r(int32x4_t a, int32_t b)
{
    return __builtin_kvx_shrwqs(a, b, ".r");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shr1_r(int32x4_t a)
{
    return __builtin_kvx_shrwqs(a, 1, ".r");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_bitcnt_(int32x4_t a)
{
    return __builtin_kvx_bitcntwq(a, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_bitcnt_lz(int32x4_t a)
{
    return __builtin_kvx_bitcntwq(a, ".lz");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_bitcnt_ls(int32x4_t a)
{
    return __builtin_kvx_bitcntwq(a, ".ls");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_bitcnt_tz(int32x4_t a)
{
    return __builtin_kvx_bitcntwq(a, ".tz");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_avg(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_avgwq(a, b, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_avg_u(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_avgwq(a, b, ".u");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_avg_r(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_avgwq(a, b, ".r");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_avg_ru(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_avgwq(a, b, ".ru");
}
int64x4_t __attribute__ ((noinline))
kvx_int32x4_mul_(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_mulwdq(a, b, 0);
}
int64x4_t __attribute__ ((noinline))
kvx_int32x4_mul_u(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_mulwdq(a, b, ".u");
}
int64x4_t __attribute__ ((noinline))
kvx_int32x4_mul_su(int32x4_t a, int32x4_t b)
{
    return __builtin_kvx_mulwdq(a, b, ".su");
}
int64x4_t __attribute__ ((noinline))
kvx_int32x4_madd_(int32x4_t a, int32x4_t b, int64x4_t c)
{
    return __builtin_kvx_maddwdq(a, b, c, 0);
}
int64x4_t __attribute__ ((noinline))
kvx_int32x4_madd_u(int32x4_t a, int32x4_t b, int64x4_t c)
{
    return __builtin_kvx_maddwdq(a, b, c, ".u");
}
int64x4_t __attribute__ ((noinline))
kvx_int32x4_madd_su(int32x4_t a, int32x4_t b, int64x4_t c)
{
    return __builtin_kvx_maddwdq(a, b, c, ".su");
}
int64x4_t __attribute__ ((noinline))
kvx_int32x4_msbf_(int32x4_t a, int32x4_t b, int64x4_t c)
{
    return __builtin_kvx_msbfwdq(a, b, c, 0);
}
int64x4_t __attribute__ ((noinline))
kvx_int32x4_msbf_u(int32x4_t a, int32x4_t b, int64x4_t c)
{
    return __builtin_kvx_msbfwdq(a, b, c, ".u");
}
int64x4_t __attribute__ ((noinline))
kvx_int32x4_msbf_su(int32x4_t a, int32x4_t b, int64x4_t c)
{
    return __builtin_kvx_msbfwdq(a, b, c, ".su");
}
int64x4_t __attribute__ ((noinline))
kvx_int32x4_sxwdq(int32x4_t a)
{
    return __builtin_kvx_widenwdq(a, 0);
}
int64x4_t __attribute__ ((noinline))
kvx_int32x4_zxwdq(int32x4_t a)
{
    return __builtin_kvx_widenwdq(a, ".z");
}
int64x4_t __attribute__ ((noinline))
kvx_int32x4_qxwdq(int32x4_t a)
{
    return __builtin_kvx_widenwdq(a, ".q");
}
int32x4_t __attribute__ ((noinline))
kvx_int32x4_truncdwq(int64x4_t a)
{
    return __builtin_kvx_narrowdwq(a, 0);
}
int32x4_t __attribute__ ((noinline))
kvx_int32x4_fractdwq(int64x4_t a)
{
    return __builtin_kvx_narrowdwq(a, ".q");
}
int32x4_t __attribute__ ((noinline))
kvx_int32x4_satdwq(int64x4_t a)
{
    return __builtin_kvx_narrowdwq(a, ".s");
}
int32x4_t __attribute__ ((noinline))
kvx_int32x4_satudwq(int64x4_t a)
{
    return __builtin_kvx_narrowdwq(a, ".us");
}
int32x4_t __attribute ((noinline))
kvx_int32x4_select(int32x4_t a, int32x4_t b, int32x4_t c)
{
    return __builtin_kvx_selectwq(a, b, c, 0);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_shift(int32x4_t a, int32_t b)
{
    return __builtin_kvx_shiftwq(a, 1, b);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_conswq(int32x2_t a, int32x2_t b) {
    return __builtin_kvx_conswq(a, b);
}
int32x4_t __attribute ((noinline))
kvx_int32x4_sconswq(int32x2_t a, int32x2_t b) {
    return __builtin_kvx_conswq(b, a);
}
