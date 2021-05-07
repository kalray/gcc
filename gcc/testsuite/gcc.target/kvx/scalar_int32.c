/* { dg-do compile } */
/* { dg-options "-O2 -ffp-contract=fast" } */

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long int64_t;
typedef __int128 int128_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
typedef unsigned __int128 uint128_t;
typedef _Float16 float16_t;
typedef float float32_t;
typedef double float64_t;
#define fma(a, b, c) ((a) * (b) + c)

int32_t __attribute ((noinline))
kvx_int32_incr(int32_t a)
{
    return a + 1;
}
int32_t __attribute ((noinline))
kvx_int32_addto(int32_t a)
{
    int32_t b = 1;
    return a + b;
}
int32_t __attribute ((noinline))
kvx_int32_subfrom(int32_t a)
{
    int32_t b = 1;
    return b - a;
}
int32_t __attribute ((noinline))
kvx_int32_addwhere(int32_t a, int32_t __bypass *b)
{
    return a + *b;
}
int32_t __attribute ((noinline))
kvx_int32_asr(int32_t a, int b)
{
    return a >> b;
}
uint32_t __attribute ((noinline))
kvx_int32_lsr(uint32_t a, int b)
{
    return a >> b;
}
int32_t __attribute ((noinline))
kvx_int32_lsl(int32_t a, int b)
{
    return a << b;
}
int32_t __attribute ((noinline))
kvx_int32_addx2(int32_t a, int32_t b)
{
    return a + (b << 1);
}
int32_t __attribute ((noinline))
kvx_int32_addx4(int32_t a, int32_t b)
{
    return a + (b << 2);
}
int32_t __attribute ((noinline))
kvx_int32_addx8(int32_t a, int32_t b)
{
    return a + (b << 3);
}
int32_t __attribute ((noinline))
kvx_int32_addx16(int32_t a, int32_t b)
{
    return a + (b << 4);
}
int32_t __attribute ((noinline))
kvx_int32_sbfx2(int32_t a, int32_t b)
{
    return a - b*2;
}
int32_t __attribute ((noinline))
kvx_int32_sbfx4(int32_t a, int32_t b)
{
    return a - b*4;
}
int32_t __attribute ((noinline))
kvx_int32_sbfx8(int32_t a, int32_t b)
{
    return a - b*8;
}
int32_t __attribute ((noinline))
kvx_int32_sbfx16(int32_t a, int32_t b)
{
    return a - b*16;
}
int32_t __attribute ((noinline))
kvx_int32_mul(int32_t a, int32_t b)
{
    return a * b;
}
int32_t __attribute ((noinline))
kvx_int32_muls(int32_t a, int32_t b)
{
    return a * b;
}
int32_t __attribute ((noinline))
kvx_int32_madd(int32_t a, int32_t b, int32_t c)
{
    return a * b + c;
}
int32_t __attribute ((noinline))
kvx_int32_msub(int32_t a, int32_t b, int32_t c)
{
    return c - a * b;
}
int32_t __attribute ((noinline))
kvx_int32_subx2(int32_t a, int32_t b)
{
    return a - (b << 1);
}
int32_t __attribute ((noinline))
kvx_int32_subx4(int32_t a, int32_t b)
{
    return a - (b << 2);
}
int32_t __attribute ((noinline))
kvx_int32_subx8(int32_t a, int32_t b)
{
    return a - (b << 3);
}
int32_t __attribute ((noinline))
kvx_int32_subx16(int32_t a, int32_t b)
{
    return a - (b << 4);
}
int32_t __attribute ((noinline))
kvx_int32_neg(int32_t a, int32_t b)
{
    return -a;
}
int32_t __attribute ((noinline))
kvx_int32_not(int32_t a, int32_t b)
{
    return ~a;
}
int32_t __attribute ((noinline))
kvx_int32_and(int32_t a, int32_t b)
{
    return a & b;
}
int32_t __attribute ((noinline))
kvx_int32_nand(int32_t a, int32_t b)
{
    return ~(a & b);
}
int32_t __attribute ((noinline))
kvx_int32_or(int32_t a, int32_t b)
{
    return a | b;
}
int32_t __attribute ((noinline))
kvx_int32_nor(int32_t a, int32_t b)
{
    return ~(a | b);
}
int32_t __attribute ((noinline))
kvx_int32_xor(int32_t a, int32_t b)
{
    return a ^ b;
}
int32_t __attribute ((noinline))
kvx_int32_nxor(int32_t a, int32_t b)
{
    return ~(a ^ b);
}
int32_t __attribute ((noinline))
kvx_int32_andn(int32_t a, int32_t b)
{
    return ~a & b;
}
int32_t __attribute ((noinline))
kvx_int32_orn(int32_t a, int32_t b)
{
    return ~a | b;
}
int32_t __attribute ((noinline))
kvx_int32_xorn(int32_t a, int32_t b)
{
    return ~a ^ b;
}
int32_t __attribute ((noinline))
kvx_int32_ne(int32_t a, int32_t b)
{
    return a != b;
}
int32_t __attribute ((noinline))
kvx_int32_gt(int32_t a, int32_t b)
{
    return a > b;
}
int32_t __attribute ((noinline))
kvx_int32_gtu(int32_t a, int32_t b)
{
    uint32_t ua = (uint32_t)a;
    uint32_t ub = (uint32_t)b;
    return ua > ub;
}
int32_t __attribute ((noinline))
kvx_int32_eqz(int32_t a)
{
    return a == 0;
}
int32_t __attribute ((noinline))
kvx_int32_shl(int32_t a, int b)
{
    return a << b;
}
int32_t __attribute ((noinline))
kvx_int32_shr(int32_t a, int b)
{
    return a >> b;
}
int32_t __attribute ((noinline))
kvx_int32_shru(int32_t a, int b)
{
    uint32_t ua = (uint32_t)a;
    return (int32_t)(ua >> b);
}
int32_t __attribute ((noinline))
kvx_int32_shl3(int32_t a)
{
    return a << 3;
}
int32_t __attribute ((noinline))
kvx_int32_shr5(int32_t a)
{
    return a >> 5;
}
int32_t __attribute ((noinline))
kvx_int32_shru7(int32_t a)
{
    uint32_t ua = (uint32_t)a;
    return (int32_t)(ua >> 7);
}
int32_t __attribute ((noinline))
kvx_int32_neg_(int32_t a)
{
    return __builtin_kvx_negw(a, 0);
}
int32_t __attribute ((noinline))
kvx_int32_neg_s(int32_t a)
{
    return __builtin_kvx_negw(a, ".s");
}
int32_t __attribute ((noinline))
kvx_int32_abs_(int32_t a)
{
    return __builtin_kvx_absw(a, "");
}
int32_t __attribute ((noinline))
kvx_int32_abs_s(int32_t a)
{
    return __builtin_kvx_absw(a, ".s");
}
int32_t __attribute ((noinline))
kvx_int32_abd_(int32_t a, int32_t b)
{
    return __builtin_kvx_abdw(a, b, 0);
}
int32_t __attribute ((noinline))
kvx_int32_abd_s(int32_t a, int32_t b)
{
    return __builtin_kvx_abdw(a, b, ".s");
}
int32_t __attribute ((noinline))
kvx_int32_abds_(int32_t a, int32_t b)
{
    return __builtin_kvx_abdw(a, b, 0);
}
int32_t __attribute ((noinline))
kvx_int32_abd0_(int32_t a)
{
    return __builtin_kvx_abdw(a, 0, 0);
}
int32_t __attribute ((noinline))
kvx_int32_abd0_s(int32_t a)
{
    return __builtin_kvx_abdw(a, 0, ".s");
}
int32_t __attribute ((noinline))
kvx_int32_add_s(int32_t a, int32_t b)
{
    return __builtin_kvx_addw(a, b, ".s");
}
int32_t __attribute ((noinline))
kvx_int32_add_us(int32_t a, int32_t b)
{
    return __builtin_kvx_addw(a, b, ".us");
}
int32_t __attribute ((noinline))
kvx_int32_adds_(int32_t a, int32_t b)
{
    return __builtin_kvx_addw(a, b, 0);
}
int32_t __attribute ((noinline))
kvx_int32_adds_s(int32_t a, int32_t b)
{
    return __builtin_kvx_addw(a, b, ".s");
}
int32_t __attribute ((noinline))
kvx_int32_adds_us(int32_t a, int32_t b)
{
    return __builtin_kvx_addw(a, b, ".us");
}
int32_t __attribute ((noinline))
kvx_int32_sbf_s(int32_t a, int32_t b)
{
    return __builtin_kvx_sbfw(a, b, ".s");
}
int32_t __attribute ((noinline))
kvx_int32_sbf_us(int32_t a, int32_t b)
{
    return __builtin_kvx_sbfw(a, b, ".us");
}
int32_t __attribute ((noinline))
kvx_int32_sbfs_(int32_t a, int32_t b)
{
    return __builtin_kvx_sbfw(a, b, 0);
}
int32_t __attribute ((noinline))
kvx_int32_sbfs_s(int32_t a, int32_t b)
{
    return __builtin_kvx_sbfw(a, b, ".s");
}
int32_t __attribute ((noinline))
kvx_int32_sbfs_us(int32_t a, int32_t b)
{
    return __builtin_kvx_sbfw(a, b, ".us");
}
int32_t __attribute ((noinline))
kvx_int32_min(int32_t a, int32_t b)
{
    return __builtin_kvx_minw(a, b);
}
int32_t __attribute ((noinline))
kvx_int32_max(int32_t a, int32_t b)
{
    return __builtin_kvx_maxw(a, b);
}
int32_t __attribute ((noinline))
kvx_int32_minu(int32_t a, int32_t b)
{
    return __builtin_kvx_minuw(a, b);
}
int32_t __attribute ((noinline))
kvx_int32_maxu(int32_t a, int32_t b)
{
    return __builtin_kvx_maxuw(a, b);
}
int32_t __attribute ((noinline))
kvx_int32_mins(int32_t a, int32_t b)
{
    return __builtin_kvx_minw(a, b);
}
int32_t __attribute ((noinline))
kvx_int32_maxs(int32_t a, int32_t b)
{
    return __builtin_kvx_maxw(a, b);
}
int32_t __attribute ((noinline))
kvx_int32_minus(int32_t a, int32_t b)
{
    return __builtin_kvx_minuw(a, b);
}
int32_t __attribute ((noinline))
kvx_int32_maxus(int32_t a, int32_t b)
{
    return __builtin_kvx_maxuw(a, b);
}
int32_t __attribute ((noinline))
kvx_int32_shls_(int32_t a, int32_t b)
{
    return __builtin_kvx_shlw(a, b, 0);
}
int32_t __attribute ((noinline))
kvx_int32_shl3_(int32_t a)
{
    return __builtin_kvx_shlw(a, 3, 0);
}
int32_t __attribute ((noinline))
kvx_int32_shls_s(int32_t a, int32_t b)
{
    return __builtin_kvx_shlw(a, b, ".s");
}
int32_t __attribute ((noinline))
kvx_int32_shl5_s(int32_t a)
{
    return __builtin_kvx_shlw(a, 5, ".s");
}
int32_t __attribute ((noinline))
kvx_int32_shls_us(int32_t a, int32_t b)
{
    return __builtin_kvx_shlw(a, b, ".us");
}
int32_t __attribute ((noinline))
kvx_int32_shl7_us(int32_t a)
{
    return __builtin_kvx_shlw(a, 7, ".us");
}
int32_t __attribute ((noinline))
kvx_int32_shls_r(int32_t a, int32_t b)
{
    return __builtin_kvx_shlw(a, b, ".r");
}
int32_t __attribute ((noinline))
kvx_int32_shl1_r(int32_t a)
{
    return __builtin_kvx_shlw(a, 1, ".r");
}
int32_t __attribute ((noinline))
kvx_int32_shrs_(int32_t a, int32_t b)
{
    return __builtin_kvx_shrw(a, b, 0);
}
int32_t __attribute ((noinline))
kvx_int32_shr5_(int32_t a)
{
    return __builtin_kvx_shrw(a, 5, 0);
}
int32_t __attribute ((noinline))
kvx_int32_shrs_a(int32_t a, int32_t b)
{
    return __builtin_kvx_shrw(a, b, ".a");
}
int32_t __attribute ((noinline))
kvx_int32_shr3_a(int32_t a)
{
    return __builtin_kvx_shrw(a, 3, ".a");
}
int32_t __attribute ((noinline))
kvx_int32_shrs_ar(int32_t a, int32_t b)
{
    return __builtin_kvx_shrw(a, b, ".ar");
}
int32_t __attribute ((noinline))
kvx_int32_shr7_ar(int32_t a)
{
    return __builtin_kvx_shrw(a, 7, ".ar");
}
int32_t __attribute ((noinline))
kvx_int32_shrs_r(int32_t a, int32_t b)
{
    return __builtin_kvx_shrw(a, b, ".r");
}
int32_t __attribute ((noinline))
kvx_int32_shr1_r(int32_t a)
{
    return __builtin_kvx_shrw(a, 1, ".r");
}
int32_t __attribute ((noinline))
kvx_int32_bitcnt_(int32_t a)
{
    return __builtin_kvx_bitcntw(a, 0);
}
int32_t __attribute ((noinline))
kvx_int32_bitcnt_lz(int32_t a)
{
    return __builtin_kvx_bitcntw(a, ".lz");
}
int32_t __attribute ((noinline))
kvx_int32_bitcnt_ls(int32_t a)
{
    return __builtin_kvx_bitcntw(a, ".ls");
}
int32_t __attribute ((noinline))
kvx_int32_bitcnt_tz(int32_t a)
{
    return __builtin_kvx_bitcntw(a, ".tz");
}
int32_t __attribute ((noinline))
kvx_int32_avg(int32_t a, int32_t b)
{
    return __builtin_kvx_avgw(a, b, 0);
}
int32_t __attribute ((noinline))
kvx_int32_avg_u(int32_t a, int32_t b)
{
    return __builtin_kvx_avgw(a, b, ".u");
}
int32_t __attribute ((noinline))
kvx_int32_avg_r(int32_t a, int32_t b)
{
    return __builtin_kvx_avgw(a, b, ".r");
}
int32_t __attribute ((noinline))
kvx_int32_avg_ru(int32_t a, int32_t b)
{
    return __builtin_kvx_avgw(a, b, ".ru");
}
