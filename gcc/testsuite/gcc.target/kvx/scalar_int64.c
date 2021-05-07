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

int64_t __attribute ((noinline))
kvx_int64_incr(int64_t a)
{
    return a + 1;
}
int64_t __attribute ((noinline))
kvx_int64_addto(int64_t a)
{
    int64_t b = 1;
    return a + b;
}
int64_t __attribute ((noinline))
kvx_int64_subfrom(int64_t a)
{
    int64_t b = 1;
    return b - a;
}
int64_t __attribute ((noinline))
kvx_int64_addwhere(int64_t a, int64_t __bypass *b)
{
    return a + *b;
}
int64_t __attribute ((noinline))
kvx_int64_asr(int64_t a, int b)
{
    return a >> b;
}
uint64_t __attribute ((noinline))
kvx_int64_lsr(uint64_t a, int b)
{
    return a >> b;
}
int64_t __attribute ((noinline))
kvx_int64_lsl(int64_t a, int b)
{
    return a << b;
}
int64_t __attribute ((noinline))
kvx_int64_addx2(int64_t a, int64_t b)
{
    return a + (b << 1);
}
int64_t __attribute ((noinline))
kvx_int64_addx4(int64_t a, int64_t b)
{
    return a + (b << 2);
}
int64_t __attribute ((noinline))
kvx_int64_addx8(int64_t a, int64_t b)
{
    return a + (b << 3);
}
int64_t __attribute ((noinline))
kvx_int64_addx16(int64_t a, int64_t b)
{
    return a + (b << 4);
}
int64_t __attribute ((noinline))
kvx_int64_sbfx2(int64_t a, int64_t b)
{
    return a - b*2;
}
int64_t __attribute ((noinline))
kvx_int64_sbfx4(int64_t a, int64_t b)
{
    return a - b*4;
}
int64_t __attribute ((noinline))
kvx_int64_sbfx8(int64_t a, int64_t b)
{
    return a - b*8;
}
int64_t __attribute ((noinline))
kvx_int64_sbfx16(int64_t a, int64_t b)
{
    return a - b*16;
}
int64_t __attribute ((noinline))
kvx_int64_mul(int64_t a, int64_t b)
{
    return a * b;
}
int64_t __attribute ((noinline))
kvx_int64_muls(int64_t a, int64_t b)
{
    return a * b;
}
int64_t __attribute ((noinline))
kvx_int64_madd(int64_t a, int64_t b, int64_t c)
{
    return a * b + c;
}
int64_t __attribute ((noinline))
kvx_int64_msub(int64_t a, int64_t b, int64_t c)
{
    return c - a * b;
}
int64_t __attribute ((noinline))
kvx_int64_subx2(int64_t a, int64_t b)
{
    return a - (b << 1);
}
int64_t __attribute ((noinline))
kvx_int64_subx4(int64_t a, int64_t b)
{
    return a - (b << 2);
}
int64_t __attribute ((noinline))
kvx_int64_subx8(int64_t a, int64_t b)
{
    return a - (b << 3);
}
int64_t __attribute ((noinline))
kvx_int64_subx16(int64_t a, int64_t b)
{
    return a - (b << 4);
}
int64_t __attribute ((noinline))
kvx_int64_neg(int64_t a, int64_t b)
{
    return -a;
}
int64_t __attribute ((noinline))
kvx_int64_not(int64_t a, int64_t b)
{
    return ~a;
}
int64_t __attribute ((noinline))
kvx_int64_and(int64_t a, int64_t b)
{
    return a & b;
}
int64_t __attribute ((noinline))
kvx_int64_nand(int64_t a, int64_t b)
{
    return ~(a & b);
}
int64_t __attribute ((noinline))
kvx_int64_or(int64_t a, int64_t b)
{
    return a | b;
}
int64_t __attribute ((noinline))
kvx_int64_nor(int64_t a, int64_t b)
{
    return ~(a | b);
}
int64_t __attribute ((noinline))
kvx_int64_xor(int64_t a, int64_t b)
{
    return a ^ b;
}
int64_t __attribute ((noinline))
kvx_int64_nxor(int64_t a, int64_t b)
{
    return ~(a ^ b);
}
int64_t __attribute ((noinline))
kvx_int64_andn(int64_t a, int64_t b)
{
    return ~a & b;
}
int64_t __attribute ((noinline))
kvx_int64_orn(int64_t a, int64_t b)
{
    return ~a | b;
}
int64_t __attribute ((noinline))
kvx_int64_xorn(int64_t a, int64_t b)
{
    return ~a ^ b;
}
int64_t __attribute ((noinline))
kvx_int64_ne(int64_t a, int64_t b)
{
    return a != b;
}
int64_t __attribute ((noinline))
kvx_int64_gt(int64_t a, int64_t b)
{
    return a > b;
}
int64_t __attribute ((noinline))
kvx_int64_gtu(int64_t a, int64_t b)
{
    uint64_t ua = (uint64_t)a;
    uint64_t ub = (uint64_t)b;
    return ua > ub;
}
int64_t __attribute ((noinline))
kvx_int64_eqz(int64_t a)
{
    return a == 0;
}
int64_t __attribute ((noinline))
kvx_int64_shl(int64_t a, int b)
{
    return a << b;
}
int64_t __attribute ((noinline))
kvx_int64_shr(int64_t a, int b)
{
    return a >> b;
}
int64_t __attribute ((noinline))
kvx_int64_shru(int64_t a, int b)
{
    uint64_t ua = (uint64_t)a;
    return (int64_t)(ua >> b);
}
int64_t __attribute ((noinline))
kvx_int64_shl3(int64_t a)
{
    return a << 3;
}
int64_t __attribute ((noinline))
kvx_int64_shr5(int64_t a)
{
    return a >> 5;
}
int64_t __attribute ((noinline))
kvx_int64_shru7(int64_t a)
{
    uint64_t ua = (uint64_t)a;
    return (int64_t)(ua >> 7);
}
int64_t __attribute ((noinline))
kvx_int64_neg_(int64_t a)
{
    return __builtin_kvx_negd(a, 0);
}
int64_t __attribute ((noinline))
kvx_int64_neg_s(int64_t a)
{
    return __builtin_kvx_negd(a, ".s");
}
int64_t __attribute ((noinline))
kvx_int64_abs_(int64_t a)
{
    return __builtin_kvx_absd(a, "");
}
int64_t __attribute ((noinline))
kvx_int64_abs_s(int64_t a)
{
    return __builtin_kvx_absd(a, ".s");
}
int64_t __attribute ((noinline))
kvx_int64_abd_(int64_t a, int64_t b)
{
    return __builtin_kvx_abdd(a, b, 0);
}
int64_t __attribute ((noinline))
kvx_int64_abd_s(int64_t a, int64_t b)
{
    return __builtin_kvx_abdd(a, b, ".s");
}
int64_t __attribute ((noinline))
kvx_int64_abds_(int64_t a, int64_t b)
{
    return __builtin_kvx_abdd(a, b, 0);
}
int64_t __attribute ((noinline))
kvx_int64_abd0_(int64_t a)
{
    return __builtin_kvx_abdd(a, 0, 0);
}
int64_t __attribute ((noinline))
kvx_int64_abd0_s(int64_t a)
{
    return __builtin_kvx_abdd(a, 0, ".s");
}
int64_t __attribute ((noinline))
kvx_int64_add_s(int64_t a, int64_t b)
{
    return __builtin_kvx_addd(a, b, ".s");
}
int64_t __attribute ((noinline))
kvx_int64_add_us(int64_t a, int64_t b)
{
    return __builtin_kvx_addd(a, b, ".us");
}
int64_t __attribute ((noinline))
kvx_int64_adds_(int64_t a, int64_t b)
{
    return __builtin_kvx_addd(a, b, 0);
}
int64_t __attribute ((noinline))
kvx_int64_adds_s(int64_t a, int64_t b)
{
    return __builtin_kvx_addd(a, b, ".s");
}
int64_t __attribute ((noinline))
kvx_int64_adds_us(int64_t a, int64_t b)
{
    return __builtin_kvx_addd(a, b, ".us");
}
int64_t __attribute ((noinline))
kvx_int64_sbf_s(int64_t a, int64_t b)
{
    return __builtin_kvx_sbfd(a, b, ".s");
}
int64_t __attribute ((noinline))
kvx_int64_sbf_us(int64_t a, int64_t b)
{
    return __builtin_kvx_sbfd(a, b, ".us");
}
int64_t __attribute ((noinline))
kvx_int64_sbfs_(int64_t a, int64_t b)
{
    return __builtin_kvx_sbfd(a, b, 0);
}
int64_t __attribute ((noinline))
kvx_int64_sbfs_s(int64_t a, int64_t b)
{
    return __builtin_kvx_sbfd(a, b, ".s");
}
int64_t __attribute ((noinline))
kvx_int64_sbfs_us(int64_t a, int64_t b)
{
    return __builtin_kvx_sbfd(a, b, ".us");
}
int64_t __attribute ((noinline))
kvx_int64_min(int64_t a, int64_t b)
{
    return __builtin_kvx_mind(a, b);
}
int64_t __attribute ((noinline))
kvx_int64_max(int64_t a, int64_t b)
{
    return __builtin_kvx_maxd(a, b);
}
int64_t __attribute ((noinline))
kvx_int64_minu(int64_t a, int64_t b)
{
    return __builtin_kvx_minud(a, b);
}
int64_t __attribute ((noinline))
kvx_int64_maxu(int64_t a, int64_t b)
{
    return __builtin_kvx_maxud(a, b);
}
int64_t __attribute ((noinline))
kvx_int64_mins(int64_t a, int64_t b)
{
    return __builtin_kvx_mind(a, b);
}
int64_t __attribute ((noinline))
kvx_int64_maxs(int64_t a, int64_t b)
{
    return __builtin_kvx_maxd(a, b);
}
int64_t __attribute ((noinline))
kvx_int64_minus(int64_t a, int64_t b)
{
    return __builtin_kvx_minud(a, b);
}
int64_t __attribute ((noinline))
kvx_int64_maxus(int64_t a, int64_t b)
{
    return __builtin_kvx_maxud(a, b);
}
int64_t __attribute ((noinline))
kvx_int64_shls_(int64_t a, int64_t b)
{
    return __builtin_kvx_shld(a, b, 0);
}
int64_t __attribute ((noinline))
kvx_int64_shl3_(int64_t a)
{
    return __builtin_kvx_shld(a, 3, 0);
}
int64_t __attribute ((noinline))
kvx_int64_shls_s(int64_t a, int64_t b)
{
    return __builtin_kvx_shld(a, b, ".s");
}
int64_t __attribute ((noinline))
kvx_int64_shl5_s(int64_t a)
{
    return __builtin_kvx_shld(a, 5, ".s");
}
int64_t __attribute ((noinline))
kvx_int64_shls_us(int64_t a, int64_t b)
{
    return __builtin_kvx_shld(a, b, ".us");
}
int64_t __attribute ((noinline))
kvx_int64_shl7_us(int64_t a)
{
    return __builtin_kvx_shld(a, 7, ".us");
}
int64_t __attribute ((noinline))
kvx_int64_shls_r(int64_t a, int64_t b)
{
    return __builtin_kvx_shld(a, b, ".r");
}
int64_t __attribute ((noinline))
kvx_int64_shl1_r(int64_t a)
{
    return __builtin_kvx_shld(a, 1, ".r");
}
int64_t __attribute ((noinline))
kvx_int64_shrs_(int64_t a, int64_t b)
{
    return __builtin_kvx_shrd(a, b, 0);
}
int64_t __attribute ((noinline))
kvx_int64_shr5_(int64_t a)
{
    return __builtin_kvx_shrd(a, 5, 0);
}
int64_t __attribute ((noinline))
kvx_int64_shrs_a(int64_t a, int64_t b)
{
    return __builtin_kvx_shrd(a, b, ".a");
}
int64_t __attribute ((noinline))
kvx_int64_shr3_a(int64_t a)
{
    return __builtin_kvx_shrd(a, 3, ".a");
}
int64_t __attribute ((noinline))
kvx_int64_shrs_ar(int64_t a, int64_t b)
{
    return __builtin_kvx_shrd(a, b, ".ar");
}
int64_t __attribute ((noinline))
kvx_int64_shr7_ar(int64_t a)
{
    return __builtin_kvx_shrd(a, 7, ".ar");
}
int64_t __attribute ((noinline))
kvx_int64_shrs_r(int64_t a, int64_t b)
{
    return __builtin_kvx_shrd(a, b, ".r");
}
int64_t __attribute ((noinline))
kvx_int64_shr1_r(int64_t a)
{
    return __builtin_kvx_shrd(a, 1, ".r");
}
int64_t __attribute ((noinline))
kvx_int64_bitcnt_(int64_t a)
{
    return __builtin_kvx_bitcntd(a, 0);
}
int64_t __attribute ((noinline))
kvx_int64_bitcnt_lz(int64_t a)
{
    return __builtin_kvx_bitcntd(a, ".lz");
}
int64_t __attribute ((noinline))
kvx_int64_bitcnt_ls(int64_t a)
{
    return __builtin_kvx_bitcntd(a, ".ls");
}
int64_t __attribute ((noinline))
kvx_int64_bitcnt_tz(int64_t a)
{
    return __builtin_kvx_bitcntd(a, ".tz");
}
