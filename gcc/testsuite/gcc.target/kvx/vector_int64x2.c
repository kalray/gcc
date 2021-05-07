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

int64x2_t __attribute ((noinline))
kvx_int64x2_incr(int64x2_t a)
{
    return a + 1;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_addto(int64x2_t a)
{
    int64x2_t b = { 1, 2 };
    return a + b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_subfrom(int64x2_t a)
{
    int64x2_t b = { 1, 2 };
    return b - a;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_addwhere(int64x2_t a, int64x2_t __bypass *b)
{
    return a + *b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_asr(int64x2_t a, int b)
{
    return a >> b;
}
uint64x2_t __attribute ((noinline))
kvx_int64x2_lsr(uint64x2_t a, int b)
{
    return a >> b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_lsl(int64x2_t a, int b)
{
    return a << b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_addx2(int64x2_t a, int64x2_t b)
{
    return a + (b << 1);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_addx4(int64x2_t a, int64x2_t b)
{
    return a + (b << 2);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_addx8(int64x2_t a, int64x2_t b)
{
    return a + (b << 3);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_addx16(int64x2_t a, int64x2_t b)
{
    return a + (b << 4);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_sbfx2(int64x2_t a, int64x2_t b)
{
    return a - b*2;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_sbfx4(int64x2_t a, int64x2_t b)
{
    return a - b*4;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_sbfx8(int64x2_t a, int64x2_t b)
{
    return a - b*8;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_sbfx16(int64x2_t a, int64x2_t b)
{
    return a - b*16;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_mul(int64x2_t a, int64x2_t b)
{
    return a * b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_muls(int64x2_t a, int64_t b)
{
    return a * b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_madd(int64x2_t a, int64x2_t b, int64x2_t c)
{
    return a * b + c;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_msub(int64x2_t a, int64x2_t b, int64x2_t c)
{
    return c - a * b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_subx2(int64x2_t a, int64x2_t b)
{
    return a - (b << 1);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_subx4(int64x2_t a, int64x2_t b)
{
    return a - (b << 2);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_subx8(int64x2_t a, int64x2_t b)
{
    return a - (b << 3);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_subx16(int64x2_t a, int64x2_t b)
{
    return a - (b << 4);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_neg(int64x2_t a, int64x2_t b)
{
    return -a;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_not(int64x2_t a, int64x2_t b)
{
    return ~a;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_and(int64x2_t a, int64x2_t b)
{
    return a & b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_nand(int64x2_t a, int64x2_t b)
{
    return ~(a & b);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_or(int64x2_t a, int64x2_t b)
{
    return a | b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_nor(int64x2_t a, int64x2_t b)
{
    return ~(a | b);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_xor(int64x2_t a, int64x2_t b)
{
    return a ^ b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_nxor(int64x2_t a, int64x2_t b)
{
    return ~(a ^ b);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_andn(int64x2_t a, int64x2_t b)
{
    return ~a & b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_orn(int64x2_t a, int64x2_t b)
{
    return ~a | b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_xorn(int64x2_t a, int64x2_t b)
{
    return ~a ^ b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_ne(int64x2_t a, int64x2_t b)
{
    return a != b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_gt(int64x2_t a, int64x2_t b)
{
    return a > b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_gtu(int64x2_t a, int64x2_t b)
{
    uint64x2_t ua = (uint64x2_t)a;
    uint64x2_t ub = (uint64x2_t)b;
    return ua > ub;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_eqz(int64x2_t a)
{
    return a == 0;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shl(int64x2_t a, int b)
{
    return a << b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shr(int64x2_t a, int b)
{
    return a >> b;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shru(int64x2_t a, int b)
{
    uint64x2_t ua = (uint64x2_t)a;
    return (int64x2_t)(ua >> b);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shl3(int64x2_t a)
{
    return a << 3;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shr5(int64x2_t a)
{
    return a >> 5;
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shru7(int64x2_t a)
{
    uint64x2_t ua = (uint64x2_t)a;
    return (int64x2_t)(ua >> 7);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_load(int64x2_t *p)
{
    return __builtin_kvx_ldp(p, ".s", 1);
}
void __attribute ((noinline))
kvx_int64x2_store(int64x2_t *p, int64x2_t a)
{
    __builtin_kvx_sdp(p, a, 0);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_neg_(int64x2_t a)
{
    return __builtin_kvx_negdp(a, 0);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_neg_s(int64x2_t a)
{
    return __builtin_kvx_negdp(a, ".s");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_abs_(int64x2_t a)
{
    return __builtin_kvx_absdp(a, "");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_abs_s(int64x2_t a)
{
    return __builtin_kvx_absdp(a, ".s");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_abd_(int64x2_t a, int64x2_t b)
{
    return __builtin_kvx_abddp(a, b, 0);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_abd_s(int64x2_t a, int64x2_t b)
{
    return __builtin_kvx_abddp(a, b, ".s");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_abds_(int64x2_t a, int64_t b)
{
    return __builtin_kvx_abddp(a, b-(int64x2_t){0}, 0);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_abd0_(int64x2_t a)
{
    return __builtin_kvx_abddp(a, (int64x2_t){0}, 0);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_abd0_s(int64x2_t a)
{
    return __builtin_kvx_abddp(a, (int64x2_t){0}, ".s");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_add_s(int64x2_t a, int64x2_t b)
{
    return __builtin_kvx_adddp(a, b, ".s");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_add_us(int64x2_t a, int64x2_t b)
{
    return __builtin_kvx_adddp(a, b, ".us");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_adds_(int64x2_t a, int64_t b)
{
    return __builtin_kvx_adddp(a, b-(int64x2_t){0}, 0);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_adds_s(int64x2_t a, int64_t b)
{
    return __builtin_kvx_adddp(a, b-(int64x2_t){0}, ".s");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_adds_us(int64x2_t a, int64_t b)
{
    return __builtin_kvx_adddp(a, b-(int64x2_t){0}, ".us");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_sbf_s(int64x2_t a, int64x2_t b)
{
    return __builtin_kvx_sbfdp(a, b, ".s");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_sbf_us(int64x2_t a, int64x2_t b)
{
    return __builtin_kvx_sbfdp(a, b, ".us");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_sbfs_(int64x2_t a, int64_t b)
{
    return __builtin_kvx_sbfdp(a, b-(int64x2_t){0}, 0);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_sbfs_s(int64x2_t a, int64_t b)
{
    return __builtin_kvx_sbfdp(a, b-(int64x2_t){0}, ".s");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_sbfs_us(int64x2_t a, int64_t b)
{
    return __builtin_kvx_sbfdp(a, b-(int64x2_t){0}, ".us");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_min(int64x2_t a, int64x2_t b)
{
    return __builtin_kvx_mindp(a, b);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_max(int64x2_t a, int64x2_t b)
{
    return __builtin_kvx_maxdp(a, b);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_minu(int64x2_t a, int64x2_t b)
{
    return __builtin_kvx_minudp(a, b);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_maxu(int64x2_t a, int64x2_t b)
{
    return __builtin_kvx_maxudp(a, b);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_mins(int64x2_t a, int64_t b)
{
    return __builtin_kvx_mindp(a, b-(int64x2_t){0});
}
int64x2_t __attribute ((noinline))
kvx_int64x2_maxs(int64x2_t a, int64_t b)
{
    return __builtin_kvx_maxdp(a, b-(int64x2_t){0});
}
int64x2_t __attribute ((noinline))
kvx_int64x2_minus(int64x2_t a, int64_t b)
{
    return __builtin_kvx_minudp(a, b-(int64x2_t){0});
}
int64x2_t __attribute ((noinline))
kvx_int64x2_maxus(int64x2_t a, int64_t b)
{
    return __builtin_kvx_maxudp(a, b-(int64x2_t){0});
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shls_(int64x2_t a, int64_t b)
{
    return __builtin_kvx_shldps(a, b, 0);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shl3_(int64x2_t a)
{
    return __builtin_kvx_shldps(a, 3, 0);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shls_s(int64x2_t a, int64_t b)
{
    return __builtin_kvx_shldps(a, b, ".s");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shl5_s(int64x2_t a)
{
    return __builtin_kvx_shldps(a, 5, ".s");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shls_us(int64x2_t a, int64_t b)
{
    return __builtin_kvx_shldps(a, b, ".us");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shl7_us(int64x2_t a)
{
    return __builtin_kvx_shldps(a, 7, ".us");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shls_r(int64x2_t a, int64_t b)
{
    return __builtin_kvx_shldps(a, b, ".r");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shl1_r(int64x2_t a)
{
    return __builtin_kvx_shldps(a, 1, ".r");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shrs_(int64x2_t a, int64_t b)
{
    return __builtin_kvx_shrdps(a, b, 0);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shr5_(int64x2_t a)
{
    return __builtin_kvx_shrdps(a, 5, 0);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shrs_a(int64x2_t a, int64_t b)
{
    return __builtin_kvx_shrdps(a, b, ".a");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shr3_a(int64x2_t a)
{
    return __builtin_kvx_shrdps(a, 3, ".a");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shrs_ar(int64x2_t a, int64_t b)
{
    return __builtin_kvx_shrdps(a, b, ".ar");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shr7_ar(int64x2_t a)
{
    return __builtin_kvx_shrdps(a, 7, ".ar");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shrs_r(int64x2_t a, int64_t b)
{
    return __builtin_kvx_shrdps(a, b, ".r");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shr1_r(int64x2_t a)
{
    return __builtin_kvx_shrdps(a, 1, ".r");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_bitcnt_(int64x2_t a)
{
    return __builtin_kvx_bitcntdp(a, 0);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_bitcnt_lz(int64x2_t a)
{
    return __builtin_kvx_bitcntdp(a, ".lz");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_bitcnt_ls(int64x2_t a)
{
    return __builtin_kvx_bitcntdp(a, ".ls");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_bitcnt_tz(int64x2_t a)
{
    return __builtin_kvx_bitcntdp(a, ".tz");
}
int64x2_t __attribute ((noinline))
kvx_int64x2_select(int64x2_t a, int64x2_t b, int64x2_t c)
{
    return __builtin_kvx_selectdp(a, b, c, 0);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_shift(int64x2_t a, int64_t b)
{
    return __builtin_kvx_shiftdp(a, 1, b);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_consdp(int64_t a, int64_t b) {
    return __builtin_kvx_consdp(a, b);
}
int64x2_t __attribute ((noinline))
kvx_int64x2_sconsdp(int64_t a, int64_t b) {
    return __builtin_kvx_consdp(b, a);
}
