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

int32x2_t __attribute ((noinline))
kvx_int32x2_incr(int32x2_t a)
{
    return a + 1;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_addto(int32x2_t a)
{
    int32x2_t b = { 1, 2 };
    return a + b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_subfrom(int32x2_t a)
{
    int32x2_t b = { 1, 2 };
    return b - a;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_addwhere(int32x2_t a, int32x2_t __bypass *b)
{
    return a + *b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_asr(int32x2_t a, int b)
{
    return a >> b;
}
uint32x2_t __attribute ((noinline))
kvx_int32x2_lsr(uint32x2_t a, int b)
{
    return a >> b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_lsl(int32x2_t a, int b)
{
    return a << b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_addx2(int32x2_t a, int32x2_t b)
{
    return a + (b << 1);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_addx4(int32x2_t a, int32x2_t b)
{
    return a + (b << 2);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_addx8(int32x2_t a, int32x2_t b)
{
    return a + (b << 3);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_addx16(int32x2_t a, int32x2_t b)
{
    return a + (b << 4);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_sbfx2(int32x2_t a, int32x2_t b)
{
    return a - b*2;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_sbfx4(int32x2_t a, int32x2_t b)
{
    return a - b*4;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_sbfx8(int32x2_t a, int32x2_t b)
{
    return a - b*8;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_sbfx16(int32x2_t a, int32x2_t b)
{
    return a - b*16;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_mul(int32x2_t a, int32x2_t b)
{
    return a * b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_muls(int32x2_t a, int32_t b)
{
    return a * b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_madd(int32x2_t a, int32x2_t b, int32x2_t c)
{
    return a * b + c;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_msub(int32x2_t a, int32x2_t b, int32x2_t c)
{
    return c - a * b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_subx2(int32x2_t a, int32x2_t b)
{
    return a - (b << 1);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_subx4(int32x2_t a, int32x2_t b)
{
    return a - (b << 2);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_subx8(int32x2_t a, int32x2_t b)
{
    return a - (b << 3);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_subx16(int32x2_t a, int32x2_t b)
{
    return a - (b << 4);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_neg(int32x2_t a, int32x2_t b)
{
    return -a;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_not(int32x2_t a, int32x2_t b)
{
    return ~a;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_and(int32x2_t a, int32x2_t b)
{
    return a & b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_nand(int32x2_t a, int32x2_t b)
{
    return ~(a & b);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_or(int32x2_t a, int32x2_t b)
{
    return a | b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_nor(int32x2_t a, int32x2_t b)
{
    return ~(a | b);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_xor(int32x2_t a, int32x2_t b)
{
    return a ^ b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_nxor(int32x2_t a, int32x2_t b)
{
    return ~(a ^ b);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_andn(int32x2_t a, int32x2_t b)
{
    return ~a & b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_orn(int32x2_t a, int32x2_t b)
{
    return ~a | b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_xorn(int32x2_t a, int32x2_t b)
{
    return ~a ^ b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_ne(int32x2_t a, int32x2_t b)
{
    return a != b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_gt(int32x2_t a, int32x2_t b)
{
    return a > b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_gtu(int32x2_t a, int32x2_t b)
{
    uint32x2_t ua = (uint32x2_t)a;
    uint32x2_t ub = (uint32x2_t)b;
    return ua > ub;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_eqz(int32x2_t a)
{
    return a == 0;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shl(int32x2_t a, int b)
{
    return a << b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shr(int32x2_t a, int b)
{
    return a >> b;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shru(int32x2_t a, int b)
{
    uint32x2_t ua = (uint32x2_t)a;
    return (int32x2_t)(ua >> b);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shl3(int32x2_t a)
{
    return a << 3;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shr5(int32x2_t a)
{
    return a >> 5;
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shru7(int32x2_t a)
{
    uint32x2_t ua = (uint32x2_t)a;
    return (int32x2_t)(ua >> 7);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_load(int32x2_t *p)
{
    return __builtin_kvx_lwp(p, ".s", 1);
}
void __attribute ((noinline))
kvx_int32x2_store(int32x2_t *p, int32x2_t a)
{
    __builtin_kvx_swp(p, a, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_neg_(int32x2_t a)
{
    return __builtin_kvx_negwp(a, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_neg_s(int32x2_t a)
{
    return __builtin_kvx_negwp(a, ".s");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_abs_(int32x2_t a)
{
    return __builtin_kvx_abswp(a, "");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_abs_s(int32x2_t a)
{
    return __builtin_kvx_abswp(a, ".s");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_abd_(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_abdwp(a, b, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_abd_s(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_abdwp(a, b, ".s");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_abds_(int32x2_t a, int32_t b)
{
    return __builtin_kvx_abdwp(a, b-(int32x2_t){0}, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_abd0_(int32x2_t a)
{
    return __builtin_kvx_abdwp(a, (int32x2_t){0}, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_abd0_s(int32x2_t a)
{
    return __builtin_kvx_abdwp(a, (int32x2_t){0}, ".s");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_add_s(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_addwp(a, b, ".s");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_add_us(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_addwp(a, b, ".us");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_adds_(int32x2_t a, int32_t b)
{
    return __builtin_kvx_addwp(a, b-(int32x2_t){0}, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_adds_s(int32x2_t a, int32_t b)
{
    return __builtin_kvx_addwp(a, b-(int32x2_t){0}, ".s");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_adds_us(int32x2_t a, int32_t b)
{
    return __builtin_kvx_addwp(a, b-(int32x2_t){0}, ".us");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_sbf_s(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_sbfwp(a, b, ".s");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_sbf_us(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_sbfwp(a, b, ".us");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_sbfs_(int32x2_t a, int32_t b)
{
    return __builtin_kvx_sbfwp(a, b-(int32x2_t){0}, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_sbfs_s(int32x2_t a, int32_t b)
{
    return __builtin_kvx_sbfwp(a, b-(int32x2_t){0}, ".s");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_sbfs_us(int32x2_t a, int32_t b)
{
    return __builtin_kvx_sbfwp(a, b-(int32x2_t){0}, ".us");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_min(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_minwp(a, b);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_max(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_maxwp(a, b);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_minu(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_minuwp(a, b);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_maxu(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_maxuwp(a, b);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_mins(int32x2_t a, int32_t b)
{
    return __builtin_kvx_minwp(a, b-(int32x2_t){0});
}
int32x2_t __attribute ((noinline))
kvx_int32x2_maxs(int32x2_t a, int32_t b)
{
    return __builtin_kvx_maxwp(a, b-(int32x2_t){0});
}
int32x2_t __attribute ((noinline))
kvx_int32x2_minus(int32x2_t a, int32_t b)
{
    return __builtin_kvx_minuwp(a, b-(int32x2_t){0});
}
int32x2_t __attribute ((noinline))
kvx_int32x2_maxus(int32x2_t a, int32_t b)
{
    return __builtin_kvx_maxuwp(a, b-(int32x2_t){0});
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shls_(int32x2_t a, int32_t b)
{
    return __builtin_kvx_shlwps(a, b, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shl3_(int32x2_t a)
{
    return __builtin_kvx_shlwps(a, 3, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shls_s(int32x2_t a, int32_t b)
{
    return __builtin_kvx_shlwps(a, b, ".s");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shl5_s(int32x2_t a)
{
    return __builtin_kvx_shlwps(a, 5, ".s");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shls_us(int32x2_t a, int32_t b)
{
    return __builtin_kvx_shlwps(a, b, ".us");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shl7_us(int32x2_t a)
{
    return __builtin_kvx_shlwps(a, 7, ".us");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shls_r(int32x2_t a, int32_t b)
{
    return __builtin_kvx_shlwps(a, b, ".r");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shl1_r(int32x2_t a)
{
    return __builtin_kvx_shlwps(a, 1, ".r");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shrs_(int32x2_t a, int32_t b)
{
    return __builtin_kvx_shrwps(a, b, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shr5_(int32x2_t a)
{
    return __builtin_kvx_shrwps(a, 5, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shrs_a(int32x2_t a, int32_t b)
{
    return __builtin_kvx_shrwps(a, b, ".a");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shr3_a(int32x2_t a)
{
    return __builtin_kvx_shrwps(a, 3, ".a");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shrs_ar(int32x2_t a, int32_t b)
{
    return __builtin_kvx_shrwps(a, b, ".ar");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shr7_ar(int32x2_t a)
{
    return __builtin_kvx_shrwps(a, 7, ".ar");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shrs_r(int32x2_t a, int32_t b)
{
    return __builtin_kvx_shrwps(a, b, ".r");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shr1_r(int32x2_t a)
{
    return __builtin_kvx_shrwps(a, 1, ".r");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_bitcnt_(int32x2_t a)
{
    return __builtin_kvx_bitcntwp(a, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_bitcnt_lz(int32x2_t a)
{
    return __builtin_kvx_bitcntwp(a, ".lz");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_bitcnt_ls(int32x2_t a)
{
    return __builtin_kvx_bitcntwp(a, ".ls");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_bitcnt_tz(int32x2_t a)
{
    return __builtin_kvx_bitcntwp(a, ".tz");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_avg(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_avgwp(a, b, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_avg_u(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_avgwp(a, b, ".u");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_avg_r(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_avgwp(a, b, ".r");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_avg_ru(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_avgwp(a, b, ".ru");
}
int64x2_t __attribute__ ((noinline))
kvx_int32x2_mul_(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_mulwdp(a, b, 0);
}
int64x2_t __attribute__ ((noinline))
kvx_int32x2_mul_u(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_mulwdp(a, b, ".u");
}
int64x2_t __attribute__ ((noinline))
kvx_int32x2_mul_su(int32x2_t a, int32x2_t b)
{
    return __builtin_kvx_mulwdp(a, b, ".su");
}
int64x2_t __attribute__ ((noinline))
kvx_int32x2_madd_(int32x2_t a, int32x2_t b, int64x2_t c)
{
    return __builtin_kvx_maddwdp(a, b, c, 0);
}
int64x2_t __attribute__ ((noinline))
kvx_int32x2_madd_u(int32x2_t a, int32x2_t b, int64x2_t c)
{
    return __builtin_kvx_maddwdp(a, b, c, ".u");
}
int64x2_t __attribute__ ((noinline))
kvx_int32x2_madd_su(int32x2_t a, int32x2_t b, int64x2_t c)
{
    return __builtin_kvx_maddwdp(a, b, c, ".su");
}
int64x2_t __attribute__ ((noinline))
kvx_int32x2_msbf_(int32x2_t a, int32x2_t b, int64x2_t c)
{
    return __builtin_kvx_msbfwdp(a, b, c, 0);
}
int64x2_t __attribute__ ((noinline))
kvx_int32x2_msbf_u(int32x2_t a, int32x2_t b, int64x2_t c)
{
    return __builtin_kvx_msbfwdp(a, b, c, ".u");
}
int64x2_t __attribute__ ((noinline))
kvx_int32x2_msbf_su(int32x2_t a, int32x2_t b, int64x2_t c)
{
    return __builtin_kvx_msbfwdp(a, b, c, ".su");
}
int64x2_t __attribute__ ((noinline))
kvx_int32x2_sxwdp(int32x2_t a)
{
    return __builtin_kvx_widenwdp(a, 0);
}
int64x2_t __attribute__ ((noinline))
kvx_int32x2_zxwdp(int32x2_t a)
{
    return __builtin_kvx_widenwdp(a, ".z");
}
int64x2_t __attribute__ ((noinline))
kvx_int32x2_qxwdp(int32x2_t a)
{
    return __builtin_kvx_widenwdp(a, ".q");
}
int32x2_t __attribute__ ((noinline))
kvx_int32x2_truncdwp(int64x2_t a)
{
    return __builtin_kvx_narrowdwp(a, 0);
}
int32x2_t __attribute__ ((noinline))
kvx_int32x2_fractdwp(int64x2_t a)
{
    return __builtin_kvx_narrowdwp(a, ".q");
}
int32x2_t __attribute__ ((noinline))
kvx_int32x2_satdwp(int64x2_t a)
{
    return __builtin_kvx_narrowdwp(a, ".s");
}
int32x2_t __attribute__ ((noinline))
kvx_int32x2_satudwp(int64x2_t a)
{
    return __builtin_kvx_narrowdwp(a, ".us");
}
int32x2_t __attribute ((noinline))
kvx_int32x2_select(int32x2_t a, int32x2_t b, int32x2_t c)
{
    return __builtin_kvx_selectwp(a, b, c, 0);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_shift(int32x2_t a, int32_t b)
{
    return __builtin_kvx_shiftwp(a, 1, b);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_conswp(int32_t a, int32_t b) {
    return __builtin_kvx_conswp(a, b);
}
int32x2_t __attribute ((noinline))
kvx_int32x2_sconswp(int32_t a, int32_t b) {
    return __builtin_kvx_conswp(b, a);
}
