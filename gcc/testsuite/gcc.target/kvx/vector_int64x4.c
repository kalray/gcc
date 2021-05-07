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

int64x4_t __attribute ((noinline))
kvx_int64x4_incr(int64x4_t a)
{
    return a + 1;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_addto(int64x4_t a)
{
    int64x4_t b = { 1, 2, 3, 4 };
    return a + b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_subfrom(int64x4_t a)
{
    int64x4_t b = { 1, 2, 3, 4 };
    return b - a;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_addwhere(int64x4_t a, int64x4_t __bypass *b)
{
    return a + *b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_asr(int64x4_t a, int b)
{
    return a >> b;
}
uint64x4_t __attribute ((noinline))
kvx_int64x4_lsr(uint64x4_t a, int b)
{
    return a >> b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_lsl(int64x4_t a, int b)
{
    return a << b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_addx2(int64x4_t a, int64x4_t b)
{
    return a + (b << 1);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_addx4(int64x4_t a, int64x4_t b)
{
    return a + (b << 2);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_addx8(int64x4_t a, int64x4_t b)
{
    return a + (b << 3);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_addx16(int64x4_t a, int64x4_t b)
{
    return a + (b << 4);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_sbfx2(int64x4_t a, int64x4_t b)
{
    return a - b*2;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_sbfx4(int64x4_t a, int64x4_t b)
{
    return a - b*4;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_sbfx8(int64x4_t a, int64x4_t b)
{
    return a - b*8;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_sbfx16(int64x4_t a, int64x4_t b)
{
    return a - b*16;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_mul(int64x4_t a, int64x4_t b)
{
    return a * b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_muls(int64x4_t a, int64_t b)
{
    return a * b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_madd(int64x4_t a, int64x4_t b, int64x4_t c)
{
    return a * b + c;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_msub(int64x4_t a, int64x4_t b, int64x4_t c)
{
    return c - a * b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_subx2(int64x4_t a, int64x4_t b)
{
    return a - (b << 1);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_subx4(int64x4_t a, int64x4_t b)
{
    return a - (b << 2);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_subx8(int64x4_t a, int64x4_t b)
{
    return a - (b << 3);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_subx16(int64x4_t a, int64x4_t b)
{
    return a - (b << 4);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_neg(int64x4_t a, int64x4_t b)
{
    return -a;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_not(int64x4_t a, int64x4_t b)
{
    return ~a;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_and(int64x4_t a, int64x4_t b)
{
    return a & b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_nand(int64x4_t a, int64x4_t b)
{
    return ~(a & b);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_or(int64x4_t a, int64x4_t b)
{
    return a | b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_nor(int64x4_t a, int64x4_t b)
{
    return ~(a | b);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_xor(int64x4_t a, int64x4_t b)
{
    return a ^ b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_nxor(int64x4_t a, int64x4_t b)
{
    return ~(a ^ b);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_andn(int64x4_t a, int64x4_t b)
{
    return ~a & b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_orn(int64x4_t a, int64x4_t b)
{
    return ~a | b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_xorn(int64x4_t a, int64x4_t b)
{
    return ~a ^ b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_ne(int64x4_t a, int64x4_t b)
{
    return a != b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_gt(int64x4_t a, int64x4_t b)
{
    return a > b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_gtu(int64x4_t a, int64x4_t b)
{
    uint64x4_t ua = (uint64x4_t)a;
    uint64x4_t ub = (uint64x4_t)b;
    return ua > ub;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_eqz(int64x4_t a)
{
    return a == 0;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shl(int64x4_t a, int b)
{
    return a << b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shr(int64x4_t a, int b)
{
    return a >> b;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shru(int64x4_t a, int b)
{
    uint64x4_t ua = (uint64x4_t)a;
    return (int64x4_t)(ua >> b);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shl3(int64x4_t a)
{
    return a << 3;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shr5(int64x4_t a)
{
    return a >> 5;
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shru7(int64x4_t a)
{
    uint64x4_t ua = (uint64x4_t)a;
    return (int64x4_t)(ua >> 7);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_load(int64x4_t *p)
{
    return __builtin_kvx_ldq(p, ".s", 1);
}
void __attribute ((noinline))
kvx_int64x4_store(int64x4_t *p, int64x4_t a)
{
    __builtin_kvx_sdq(p, a, 0);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_neg_(int64x4_t a)
{
    return __builtin_kvx_negdq(a, 0);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_neg_s(int64x4_t a)
{
    return __builtin_kvx_negdq(a, ".s");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_abs_(int64x4_t a)
{
    return __builtin_kvx_absdq(a, "");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_abs_s(int64x4_t a)
{
    return __builtin_kvx_absdq(a, ".s");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_abd_(int64x4_t a, int64x4_t b)
{
    return __builtin_kvx_abddq(a, b, 0);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_abd_s(int64x4_t a, int64x4_t b)
{
    return __builtin_kvx_abddq(a, b, ".s");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_abds_(int64x4_t a, int64_t b)
{
    return __builtin_kvx_abddq(a, b-(int64x4_t){0}, 0);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_abd0_(int64x4_t a)
{
    return __builtin_kvx_abddq(a, (int64x4_t){0}, 0);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_abd0_s(int64x4_t a)
{
    return __builtin_kvx_abddq(a, (int64x4_t){0}, ".s");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_add_s(int64x4_t a, int64x4_t b)
{
    return __builtin_kvx_adddq(a, b, ".s");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_add_us(int64x4_t a, int64x4_t b)
{
    return __builtin_kvx_adddq(a, b, ".us");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_adds_(int64x4_t a, int64_t b)
{
    return __builtin_kvx_adddq(a, b-(int64x4_t){0}, 0);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_adds_s(int64x4_t a, int64_t b)
{
    return __builtin_kvx_adddq(a, b-(int64x4_t){0}, ".s");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_adds_us(int64x4_t a, int64_t b)
{
    return __builtin_kvx_adddq(a, b-(int64x4_t){0}, ".us");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_sbf_s(int64x4_t a, int64x4_t b)
{
    return __builtin_kvx_sbfdq(a, b, ".s");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_sbf_us(int64x4_t a, int64x4_t b)
{
    return __builtin_kvx_sbfdq(a, b, ".us");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_sbfs_(int64x4_t a, int64_t b)
{
    return __builtin_kvx_sbfdq(a, b-(int64x4_t){0}, 0);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_sbfs_s(int64x4_t a, int64_t b)
{
    return __builtin_kvx_sbfdq(a, b-(int64x4_t){0}, ".s");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_sbfs_us(int64x4_t a, int64_t b)
{
    return __builtin_kvx_sbfdq(a, b-(int64x4_t){0}, ".us");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_min(int64x4_t a, int64x4_t b)
{
    return __builtin_kvx_mindq(a, b);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_max(int64x4_t a, int64x4_t b)
{
    return __builtin_kvx_maxdq(a, b);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_minu(int64x4_t a, int64x4_t b)
{
    return __builtin_kvx_minudq(a, b);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_maxu(int64x4_t a, int64x4_t b)
{
    return __builtin_kvx_maxudq(a, b);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_mins(int64x4_t a, int64_t b)
{
    return __builtin_kvx_mindq(a, b-(int64x4_t){0});
}
int64x4_t __attribute ((noinline))
kvx_int64x4_maxs(int64x4_t a, int64_t b)
{
    return __builtin_kvx_maxdq(a, b-(int64x4_t){0});
}
int64x4_t __attribute ((noinline))
kvx_int64x4_minus(int64x4_t a, int64_t b)
{
    return __builtin_kvx_minudq(a, b-(int64x4_t){0});
}
int64x4_t __attribute ((noinline))
kvx_int64x4_maxus(int64x4_t a, int64_t b)
{
    return __builtin_kvx_maxudq(a, b-(int64x4_t){0});
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shls_(int64x4_t a, int64_t b)
{
    return __builtin_kvx_shldqs(a, b, 0);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shl3_(int64x4_t a)
{
    return __builtin_kvx_shldqs(a, 3, 0);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shls_s(int64x4_t a, int64_t b)
{
    return __builtin_kvx_shldqs(a, b, ".s");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shl5_s(int64x4_t a)
{
    return __builtin_kvx_shldqs(a, 5, ".s");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shls_us(int64x4_t a, int64_t b)
{
    return __builtin_kvx_shldqs(a, b, ".us");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shl7_us(int64x4_t a)
{
    return __builtin_kvx_shldqs(a, 7, ".us");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shls_r(int64x4_t a, int64_t b)
{
    return __builtin_kvx_shldqs(a, b, ".r");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shl1_r(int64x4_t a)
{
    return __builtin_kvx_shldqs(a, 1, ".r");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shrs_(int64x4_t a, int64_t b)
{
    return __builtin_kvx_shrdqs(a, b, 0);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shr5_(int64x4_t a)
{
    return __builtin_kvx_shrdqs(a, 5, 0);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shrs_a(int64x4_t a, int64_t b)
{
    return __builtin_kvx_shrdqs(a, b, ".a");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shr3_a(int64x4_t a)
{
    return __builtin_kvx_shrdqs(a, 3, ".a");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shrs_ar(int64x4_t a, int64_t b)
{
    return __builtin_kvx_shrdqs(a, b, ".ar");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shr7_ar(int64x4_t a)
{
    return __builtin_kvx_shrdqs(a, 7, ".ar");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shrs_r(int64x4_t a, int64_t b)
{
    return __builtin_kvx_shrdqs(a, b, ".r");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shr1_r(int64x4_t a)
{
    return __builtin_kvx_shrdqs(a, 1, ".r");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_bitcnt_(int64x4_t a)
{
    return __builtin_kvx_bitcntdq(a, 0);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_bitcnt_lz(int64x4_t a)
{
    return __builtin_kvx_bitcntdq(a, ".lz");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_bitcnt_ls(int64x4_t a)
{
    return __builtin_kvx_bitcntdq(a, ".ls");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_bitcnt_tz(int64x4_t a)
{
    return __builtin_kvx_bitcntdq(a, ".tz");
}
int64x4_t __attribute ((noinline))
kvx_int64x4_select(int64x4_t a, int64x4_t b, int64x4_t c)
{
    return __builtin_kvx_selectdq(a, b, c, 0);
}
int64x4_t __attribute ((noinline))
kvx_int64x4_shift(int64x4_t a, int64_t b)
{
    return __builtin_kvx_shiftdq(a, 1, b);
}
