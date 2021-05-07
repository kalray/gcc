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

int16x8_t __attribute ((noinline))
kvx_int16x8_incr(int16x8_t a)
{
    return a + 1;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_addto(int16x8_t a)
{
    int16x8_t b = { 1, 2, 3, 4, 5, 6, 7, 8 };
    return a + b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_subfrom(int16x8_t a)
{
    int16x8_t b = { 1, 2, 3, 4, 5, 6, 7, 8 };
    return b - a;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_addwhere(int16x8_t a, int16x8_t __bypass *b)
{
    return a + *b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_asr(int16x8_t a, int b)
{
    return a >> b;
}
uint16x8_t __attribute ((noinline))
kvx_int16x8_lsr(uint16x8_t a, int b)
{
    return a >> b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_lsl(int16x8_t a, int b)
{
    return a << b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_addx2(int16x8_t a, int16x8_t b)
{
    return a + (b << 1);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_addx4(int16x8_t a, int16x8_t b)
{
    return a + (b << 2);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_addx8(int16x8_t a, int16x8_t b)
{
    return a + (b << 3);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_addx16(int16x8_t a, int16x8_t b)
{
    return a + (b << 4);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_sbfx2(int16x8_t a, int16x8_t b)
{
    return a - b*2;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_sbfx4(int16x8_t a, int16x8_t b)
{
    return a - b*4;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_sbfx8(int16x8_t a, int16x8_t b)
{
    return a - b*8;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_sbfx16(int16x8_t a, int16x8_t b)
{
    return a - b*16;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_mul(int16x8_t a, int16x8_t b)
{
    return a * b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_muls(int16x8_t a, int16_t b)
{
    return a * b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_madd(int16x8_t a, int16x8_t b, int16x8_t c)
{
    return a * b + c;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_msub(int16x8_t a, int16x8_t b, int16x8_t c)
{
    return c - a * b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_subx2(int16x8_t a, int16x8_t b)
{
    return a - (b << 1);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_subx4(int16x8_t a, int16x8_t b)
{
    return a - (b << 2);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_subx8(int16x8_t a, int16x8_t b)
{
    return a - (b << 3);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_subx16(int16x8_t a, int16x8_t b)
{
    return a - (b << 4);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_neg(int16x8_t a, int16x8_t b)
{
    return -a;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_not(int16x8_t a, int16x8_t b)
{
    return ~a;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_and(int16x8_t a, int16x8_t b)
{
    return a & b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_nand(int16x8_t a, int16x8_t b)
{
    return ~(a & b);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_or(int16x8_t a, int16x8_t b)
{
    return a | b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_nor(int16x8_t a, int16x8_t b)
{
    return ~(a | b);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_xor(int16x8_t a, int16x8_t b)
{
    return a ^ b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_nxor(int16x8_t a, int16x8_t b)
{
    return ~(a ^ b);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_andn(int16x8_t a, int16x8_t b)
{
    return ~a & b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_orn(int16x8_t a, int16x8_t b)
{
    return ~a | b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_xorn(int16x8_t a, int16x8_t b)
{
    return ~a ^ b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_ne(int16x8_t a, int16x8_t b)
{
    return a != b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_gt(int16x8_t a, int16x8_t b)
{
    return a > b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_gtu(int16x8_t a, int16x8_t b)
{
    uint16x8_t ua = (uint16x8_t)a;
    uint16x8_t ub = (uint16x8_t)b;
    return ua > ub;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_eqz(int16x8_t a)
{
    return a == 0;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shl(int16x8_t a, int b)
{
    return a << b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shr(int16x8_t a, int b)
{
    return a >> b;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shru(int16x8_t a, int b)
{
    uint16x8_t ua = (uint16x8_t)a;
    return (int16x8_t)(ua >> b);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shl3(int16x8_t a)
{
    return a << 3;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shr5(int16x8_t a)
{
    return a >> 5;
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shru7(int16x8_t a)
{
    uint16x8_t ua = (uint16x8_t)a;
    return (int16x8_t)(ua >> 7);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_load(int16x8_t *p)
{
    return __builtin_kvx_lho(p, ".s", 1);
}
void __attribute ((noinline))
kvx_int16x8_store(int16x8_t *p, int16x8_t a)
{
    __builtin_kvx_sho(p, a, 0);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_neg_(int16x8_t a)
{
    return __builtin_kvx_negho(a, 0);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_neg_s(int16x8_t a)
{
    return __builtin_kvx_negho(a, ".s");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_abs_(int16x8_t a)
{
    return __builtin_kvx_absho(a, "");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_abs_s(int16x8_t a)
{
    return __builtin_kvx_absho(a, ".s");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_abd_(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_abdho(a, b, 0);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_abd_s(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_abdho(a, b, ".s");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_abds_(int16x8_t a, int16_t b)
{
    return __builtin_kvx_abdho(a, b-(int16x8_t){0}, 0);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_abd0_(int16x8_t a)
{
    return __builtin_kvx_abdho(a, (int16x8_t){0}, 0);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_abd0_s(int16x8_t a)
{
    return __builtin_kvx_abdho(a, (int16x8_t){0}, ".s");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_add_s(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_addho(a, b, ".s");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_add_us(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_addho(a, b, ".us");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_adds_(int16x8_t a, int16_t b)
{
    return __builtin_kvx_addho(a, b-(int16x8_t){0}, 0);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_adds_s(int16x8_t a, int16_t b)
{
    return __builtin_kvx_addho(a, b-(int16x8_t){0}, ".s");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_adds_us(int16x8_t a, int16_t b)
{
    return __builtin_kvx_addho(a, b-(int16x8_t){0}, ".us");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_sbf_s(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_sbfho(a, b, ".s");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_sbf_us(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_sbfho(a, b, ".us");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_sbfs_(int16x8_t a, int16_t b)
{
    return __builtin_kvx_sbfho(a, b-(int16x8_t){0}, 0);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_sbfs_s(int16x8_t a, int16_t b)
{
    return __builtin_kvx_sbfho(a, b-(int16x8_t){0}, ".s");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_sbfs_us(int16x8_t a, int16_t b)
{
    return __builtin_kvx_sbfho(a, b-(int16x8_t){0}, ".us");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_min(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_minho(a, b);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_max(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_maxho(a, b);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_minu(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_minuho(a, b);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_maxu(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_maxuho(a, b);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_mins(int16x8_t a, int16_t b)
{
    return __builtin_kvx_minho(a, b-(int16x8_t){0});
}
int16x8_t __attribute ((noinline))
kvx_int16x8_maxs(int16x8_t a, int16_t b)
{
    return __builtin_kvx_maxho(a, b-(int16x8_t){0});
}
int16x8_t __attribute ((noinline))
kvx_int16x8_minus(int16x8_t a, int16_t b)
{
    return __builtin_kvx_minuho(a, b-(int16x8_t){0});
}
int16x8_t __attribute ((noinline))
kvx_int16x8_maxus(int16x8_t a, int16_t b)
{
    return __builtin_kvx_maxuho(a, b-(int16x8_t){0});
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shls_(int16x8_t a, int16_t b)
{
    return __builtin_kvx_shlhos(a, b, 0);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shl3_(int16x8_t a)
{
    return __builtin_kvx_shlhos(a, 3, 0);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shls_s(int16x8_t a, int16_t b)
{
    return __builtin_kvx_shlhos(a, b, ".s");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shl5_s(int16x8_t a)
{
    return __builtin_kvx_shlhos(a, 5, ".s");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shls_us(int16x8_t a, int16_t b)
{
    return __builtin_kvx_shlhos(a, b, ".us");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shl7_us(int16x8_t a)
{
    return __builtin_kvx_shlhos(a, 7, ".us");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shls_r(int16x8_t a, int16_t b)
{
    return __builtin_kvx_shlhos(a, b, ".r");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shl1_r(int16x8_t a)
{
    return __builtin_kvx_shlhos(a, 1, ".r");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shrs_(int16x8_t a, int16_t b)
{
    return __builtin_kvx_shrhos(a, b, 0);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shr5_(int16x8_t a)
{
    return __builtin_kvx_shrhos(a, 5, 0);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shrs_a(int16x8_t a, int16_t b)
{
    return __builtin_kvx_shrhos(a, b, ".a");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shr3_a(int16x8_t a)
{
    return __builtin_kvx_shrhos(a, 3, ".a");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shrs_ar(int16x8_t a, int16_t b)
{
    return __builtin_kvx_shrhos(a, b, ".ar");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shr7_ar(int16x8_t a)
{
    return __builtin_kvx_shrhos(a, 7, ".ar");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shrs_r(int16x8_t a, int16_t b)
{
    return __builtin_kvx_shrhos(a, b, ".r");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shr1_r(int16x8_t a)
{
    return __builtin_kvx_shrhos(a, 1, ".r");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_avg(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_avgho(a, b, 0);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_avg_u(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_avgho(a, b, ".u");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_avg_r(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_avgho(a, b, ".r");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_avg_ru(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_avgho(a, b, ".ru");
}
int32x8_t __attribute__ ((noinline))
kvx_int16x8_mul_(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_mulhwo(a, b, 0);
}
int32x8_t __attribute__ ((noinline))
kvx_int16x8_mul_u(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_mulhwo(a, b, ".u");
}
int32x8_t __attribute__ ((noinline))
kvx_int16x8_mul_su(int16x8_t a, int16x8_t b)
{
    return __builtin_kvx_mulhwo(a, b, ".su");
}
int32x8_t __attribute__ ((noinline))
kvx_int16x8_madd_(int16x8_t a, int16x8_t b, int32x8_t c)
{
    return __builtin_kvx_maddhwo(a, b, c, 0);
}
int32x8_t __attribute__ ((noinline))
kvx_int16x8_madd_u(int16x8_t a, int16x8_t b, int32x8_t c)
{
    return __builtin_kvx_maddhwo(a, b, c, ".u");
}
int32x8_t __attribute__ ((noinline))
kvx_int16x8_madd_su(int16x8_t a, int16x8_t b, int32x8_t c)
{
    return __builtin_kvx_maddhwo(a, b, c, ".su");
}
int32x8_t __attribute__ ((noinline))
kvx_int16x8_msbf_(int16x8_t a, int16x8_t b, int32x8_t c)
{
    return __builtin_kvx_msbfhwo(a, b, c, 0);
}
int32x8_t __attribute__ ((noinline))
kvx_int16x8_msbf_u(int16x8_t a, int16x8_t b, int32x8_t c)
{
    return __builtin_kvx_msbfhwo(a, b, c, ".u");
}
int32x8_t __attribute__ ((noinline))
kvx_int16x8_msbf_su(int16x8_t a, int16x8_t b, int32x8_t c)
{
    return __builtin_kvx_msbfhwo(a, b, c, ".su");
}
int32x8_t __attribute__ ((noinline))
kvx_int16x8_sxhwo(int16x8_t a)
{
    return __builtin_kvx_widenhwo(a, 0);
}
int32x8_t __attribute__ ((noinline))
kvx_int16x8_zxhwo(int16x8_t a)
{
    return __builtin_kvx_widenhwo(a, ".z");
}
int32x8_t __attribute__ ((noinline))
kvx_int16x8_qxhwo(int16x8_t a)
{
    return __builtin_kvx_widenhwo(a, ".q");
}
int16x8_t __attribute__ ((noinline))
kvx_int16x8_truncwho(int32x8_t a)
{
    return __builtin_kvx_narrowwho(a, 0);
}
int16x8_t __attribute__ ((noinline))
kvx_int16x8_fractwho(int32x8_t a)
{
    return __builtin_kvx_narrowwho(a, ".q");
}
int16x8_t __attribute__ ((noinline))
kvx_int16x8_satwho(int32x8_t a)
{
    return __builtin_kvx_narrowwho(a, ".s");
}
int16x8_t __attribute__ ((noinline))
kvx_int16x8_satuwho(int32x8_t a)
{
    return __builtin_kvx_narrowwho(a, ".us");
}
int16x8_t __attribute ((noinline))
kvx_int16x8_select(int16x8_t a, int16x8_t b, int16x8_t c)
{
    return __builtin_kvx_selectho(a, b, c, 0);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_shift(int16x8_t a, int16_t b)
{
    return __builtin_kvx_shiftho(a, 1, b);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_consho(int16x4_t a, int16x4_t b) {
    return __builtin_kvx_consho(a, b);
}
int16x8_t __attribute ((noinline))
kvx_int16x8_sconsho(int16x4_t a, int16x4_t b) {
    return __builtin_kvx_consho(b, a);
}
