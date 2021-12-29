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

int8x8_t __attribute ((noinline))
kvx_int8x8_incr(int8x8_t a)
{
    return a + 1;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_addto(int8x8_t a)
{
    int8x8_t b = { 1, 2, 3, 4, 5, 6, 7, 8 };
    return a + b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_subfrom(int8x8_t a)
{
    int8x8_t b = { 1, 2, 3, 4, 5, 6, 7, 8 };
    return b - a;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_addwhere(int8x8_t a, int8x8_t __bypass *b)
{
    return a + *b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_asr(int8x8_t a, int b)
{
    return a >> b;
}
uint8x8_t __attribute ((noinline))
kvx_int8x8_lsr(uint8x8_t a, int b)
{
    return a >> b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_lsl(int8x8_t a, int b)
{
    return a << b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_addx2(int8x8_t a, int8x8_t b)
{
    return a + (b << 1);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_addx4(int8x8_t a, int8x8_t b)
{
    return a + (b << 2);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_addx8(int8x8_t a, int8x8_t b)
{
    return a + (b << 3);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_addx16(int8x8_t a, int8x8_t b)
{
    return a + (b << 4);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_sbfx2(int8x8_t a, int8x8_t b)
{
    return a - b*2;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_sbfx4(int8x8_t a, int8x8_t b)
{
    return a - b*4;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_sbfx8(int8x8_t a, int8x8_t b)
{
    return a - b*8;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_sbfx16(int8x8_t a, int8x8_t b)
{
    return a - b*16;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_mul(int8x8_t a, int8x8_t b)
{
    return a * b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_muls(int8x8_t a, int8_t b)
{
    return a * b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_madd(int8x8_t a, int8x8_t b, int8x8_t c)
{
    return a * b + c;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_msub(int8x8_t a, int8x8_t b, int8x8_t c)
{
    return c - a * b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_subx2(int8x8_t a, int8x8_t b)
{
    return a - (b << 1);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_subx4(int8x8_t a, int8x8_t b)
{
    return a - (b << 2);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_subx8(int8x8_t a, int8x8_t b)
{
    return a - (b << 3);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_subx16(int8x8_t a, int8x8_t b)
{
    return a - (b << 4);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_neg(int8x8_t a, int8x8_t b)
{
    return -a;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_not(int8x8_t a, int8x8_t b)
{
    return ~a;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_and(int8x8_t a, int8x8_t b)
{
    return a & b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_nand(int8x8_t a, int8x8_t b)
{
    return ~(a & b);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_or(int8x8_t a, int8x8_t b)
{
    return a | b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_nor(int8x8_t a, int8x8_t b)
{
    return ~(a | b);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_xor(int8x8_t a, int8x8_t b)
{
    return a ^ b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_nxor(int8x8_t a, int8x8_t b)
{
    return ~(a ^ b);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_andn(int8x8_t a, int8x8_t b)
{
    return ~a & b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_orn(int8x8_t a, int8x8_t b)
{
    return ~a | b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_xorn(int8x8_t a, int8x8_t b)
{
    return ~a ^ b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_ne(int8x8_t a, int8x8_t b)
{
    return a != b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_gt(int8x8_t a, int8x8_t b)
{
    return a > b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_gtu(int8x8_t a, int8x8_t b)
{
    uint8x8_t ua = (uint8x8_t)a;
    uint8x8_t ub = (uint8x8_t)b;
    return ua > ub;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_eqz(int8x8_t a)
{
    return a == 0;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shl(int8x8_t a, int b)
{
    return a << b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shr(int8x8_t a, int b)
{
    return a >> b;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shru(int8x8_t a, int b)
{
    uint8x8_t ua = (uint8x8_t)a;
    return (int8x8_t)(ua >> b);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shl3(int8x8_t a)
{
    return a << 3;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shr5(int8x8_t a)
{
    return a >> 5;
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shru7(int8x8_t a)
{
    uint8x8_t ua = (uint8x8_t)a;
    return (int8x8_t)(ua >> 7);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_load(int8x8_t *p)
{
    return __builtin_kvx_lbo(p, ".s", 1);
}
void __attribute ((noinline))
kvx_int8x8_store(int8x8_t *p, int8x8_t a)
{
    __builtin_kvx_sbo(p, a, 0);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_neg_(int8x8_t a)
{
    return __builtin_kvx_negbo(a, 0);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_neg_s(int8x8_t a)
{
    return __builtin_kvx_negbo(a, ".s");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_abs_(int8x8_t a)
{
    return __builtin_kvx_absbo(a, "");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_abs_s(int8x8_t a)
{
    return __builtin_kvx_absbo(a, ".s");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_abd_(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_abdbo(a, b, 0);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_abd_s(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_abdbo(a, b, ".s");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_abds_(int8x8_t a, int8_t b)
{
    return __builtin_kvx_abdbo(a, b-(int8x8_t){0}, 0);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_abd0_(int8x8_t a)
{
    return __builtin_kvx_abdbo(a, (int8x8_t){0}, 0);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_abd0_s(int8x8_t a)
{
    return __builtin_kvx_abdbo(a, (int8x8_t){0}, ".s");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_add_s(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_addbo(a, b, ".s");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_add_us(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_addbo(a, b, ".us");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_adds_(int8x8_t a, int8_t b)
{
    return __builtin_kvx_addbo(a, b-(int8x8_t){0}, 0);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_adds_s(int8x8_t a, int8_t b)
{
    return __builtin_kvx_addbo(a, b-(int8x8_t){0}, ".s");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_adds_us(int8x8_t a, int8_t b)
{
    return __builtin_kvx_addbo(a, b-(int8x8_t){0}, ".us");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_sbf_s(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_sbfbo(a, b, ".s");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_sbf_us(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_sbfbo(a, b, ".us");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_sbfs_(int8x8_t a, int8_t b)
{
    return __builtin_kvx_sbfbo(a, b-(int8x8_t){0}, 0);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_sbfs_s(int8x8_t a, int8_t b)
{
    return __builtin_kvx_sbfbo(a, b-(int8x8_t){0}, ".s");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_sbfs_us(int8x8_t a, int8_t b)
{
    return __builtin_kvx_sbfbo(a, b-(int8x8_t){0}, ".us");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_min(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_minbo(a, b);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_max(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_maxbo(a, b);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_minu(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_minubo(a, b);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_maxu(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_maxubo(a, b);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_mins(int8x8_t a, int8_t b)
{
    return __builtin_kvx_minbo(a, b-(int8x8_t){0});
}
int8x8_t __attribute ((noinline))
kvx_int8x8_maxs(int8x8_t a, int8_t b)
{
    return __builtin_kvx_maxbo(a, b-(int8x8_t){0});
}
int8x8_t __attribute ((noinline))
kvx_int8x8_minus(int8x8_t a, int8_t b)
{
    return __builtin_kvx_minubo(a, b-(int8x8_t){0});
}
int8x8_t __attribute ((noinline))
kvx_int8x8_maxus(int8x8_t a, int8_t b)
{
    return __builtin_kvx_maxubo(a, b-(int8x8_t){0});
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shls_(int8x8_t a, int8_t b)
{
    return __builtin_kvx_shlbos(a, b, 0);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shl3_(int8x8_t a)
{
    return __builtin_kvx_shlbos(a, 3, 0);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shls_s(int8x8_t a, int8_t b)
{
    return __builtin_kvx_shlbos(a, b, ".s");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shl5_s(int8x8_t a)
{
    return __builtin_kvx_shlbos(a, 5, ".s");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shls_us(int8x8_t a, int8_t b)
{
    return __builtin_kvx_shlbos(a, b, ".us");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shl7_us(int8x8_t a)
{
    return __builtin_kvx_shlbos(a, 7, ".us");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shls_r(int8x8_t a, int8_t b)
{
    return __builtin_kvx_shlbos(a, b, ".r");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shl1_r(int8x8_t a)
{
    return __builtin_kvx_shlbos(a, 1, ".r");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shrs_(int8x8_t a, int8_t b)
{
    return __builtin_kvx_shrbos(a, b, 0);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shr5_(int8x8_t a)
{
    return __builtin_kvx_shrbos(a, 5, 0);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shrs_a(int8x8_t a, int8_t b)
{
    return __builtin_kvx_shrbos(a, b, ".a");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shr3_a(int8x8_t a)
{
    return __builtin_kvx_shrbos(a, 3, ".a");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shrs_as(int8x8_t a, int8_t b)
{
    return __builtin_kvx_shrbos(a, b, ".as");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shr7_as(int8x8_t a)
{
    return __builtin_kvx_shrbos(a, 7, ".as");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shrs_r(int8x8_t a, int8_t b)
{
    return __builtin_kvx_shrbos(a, b, ".r");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shr1_r(int8x8_t a)
{
    return __builtin_kvx_shrbos(a, 1, ".r");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_avg(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_avgbo(a, b, 0);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_avg_u(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_avgbo(a, b, ".u");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_avg_r(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_avgbo(a, b, ".r");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_avg_ru(int8x8_t a, int8x8_t b)
{
    return __builtin_kvx_avgbo(a, b, ".ru");
}
int16x8_t __attribute__ ((noinline))
kvx_int8x8_sxbho(int8x8_t a)
{
    return __builtin_kvx_widenbho(a, 0);
}
int16x8_t __attribute__ ((noinline))
kvx_int8x8_zxbho(int8x8_t a)
{
    return __builtin_kvx_widenbho(a, ".z");
}
int16x8_t __attribute__ ((noinline))
kvx_int8x8_qxbho(int8x8_t a)
{
    return __builtin_kvx_widenbho(a, ".q");
}
int8x8_t __attribute__ ((noinline))
kvx_int8x8_trunchbo(int16x8_t a)
{
    return __builtin_kvx_narrowhbo(a, 0);
}
int8x8_t __attribute__ ((noinline))
kvx_int8x8_fracthbo(int16x8_t a)
{
    return __builtin_kvx_narrowhbo(a, ".q");
}
int8x8_t __attribute__ ((noinline))
kvx_int8x8_sathbo(int16x8_t a)
{
    return __builtin_kvx_narrowhbo(a, ".s");
}
int8x8_t __attribute__ ((noinline))
kvx_int8x8_satuhbo(int16x8_t a)
{
    return __builtin_kvx_narrowhbo(a, ".us");
}
int8x8_t __attribute ((noinline))
kvx_int8x8_select(int8x8_t a, int8x8_t b, int8x8_t c)
{
    return __builtin_kvx_selectbo(a, b, c, 0);
}
int8x8_t __attribute ((noinline))
kvx_int8x8_shift(int8x8_t a, int8_t b)
{
    return __builtin_kvx_shiftbo(a, 1, b);
}
