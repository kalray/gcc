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

int8x16_t __attribute ((noinline))
kvx_int8x16_incr(int8x16_t a)
{
    return a + 1;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_addto(int8x16_t a)
{
    int8x16_t b = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    return a + b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_subfrom(int8x16_t a)
{
    int8x16_t b = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    return b - a;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_addwhere(int8x16_t a, int8x16_t __bypass *b)
{
    return a + *b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_asr(int8x16_t a, int b)
{
    return a >> b;
}
uint8x16_t __attribute ((noinline))
kvx_int8x16_lsr(uint8x16_t a, int b)
{
    return a >> b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_lsl(int8x16_t a, int b)
{
    return a << b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_addx2(int8x16_t a, int8x16_t b)
{
    return a + (b << 1);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_addx4(int8x16_t a, int8x16_t b)
{
    return a + (b << 2);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_addx8(int8x16_t a, int8x16_t b)
{
    return a + (b << 3);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_addx16(int8x16_t a, int8x16_t b)
{
    return a + (b << 4);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_sbfx2(int8x16_t a, int8x16_t b)
{
    return a - b*2;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_sbfx4(int8x16_t a, int8x16_t b)
{
    return a - b*4;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_sbfx8(int8x16_t a, int8x16_t b)
{
    return a - b*8;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_sbfx16(int8x16_t a, int8x16_t b)
{
    return a - b*16;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_mul(int8x16_t a, int8x16_t b)
{
    return a * b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_muls(int8x16_t a, int8_t b)
{
    return a * b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_madd(int8x16_t a, int8x16_t b, int8x16_t c)
{
    return a * b + c;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_msub(int8x16_t a, int8x16_t b, int8x16_t c)
{
    return c - a * b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_subx2(int8x16_t a, int8x16_t b)
{
    return a - (b << 1);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_subx4(int8x16_t a, int8x16_t b)
{
    return a - (b << 2);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_subx8(int8x16_t a, int8x16_t b)
{
    return a - (b << 3);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_subx16(int8x16_t a, int8x16_t b)
{
    return a - (b << 4);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_neg(int8x16_t a, int8x16_t b)
{
    return -a;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_not(int8x16_t a, int8x16_t b)
{
    return ~a;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_and(int8x16_t a, int8x16_t b)
{
    return a & b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_nand(int8x16_t a, int8x16_t b)
{
    return ~(a & b);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_or(int8x16_t a, int8x16_t b)
{
    return a | b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_nor(int8x16_t a, int8x16_t b)
{
    return ~(a | b);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_xor(int8x16_t a, int8x16_t b)
{
    return a ^ b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_nxor(int8x16_t a, int8x16_t b)
{
    return ~(a ^ b);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_andn(int8x16_t a, int8x16_t b)
{
    return ~a & b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_orn(int8x16_t a, int8x16_t b)
{
    return ~a | b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_xorn(int8x16_t a, int8x16_t b)
{
    return ~a ^ b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_ne(int8x16_t a, int8x16_t b)
{
    return a != b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_gt(int8x16_t a, int8x16_t b)
{
    return a > b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_gtu(int8x16_t a, int8x16_t b)
{
    uint8x16_t ua = (uint8x16_t)a;
    uint8x16_t ub = (uint8x16_t)b;
    return ua > ub;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_eqz(int8x16_t a)
{
    return a == 0;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shl(int8x16_t a, int b)
{
    return a << b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shr(int8x16_t a, int b)
{
    return a >> b;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shru(int8x16_t a, int b)
{
    uint8x16_t ua = (uint8x16_t)a;
    return (int8x16_t)(ua >> b);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shl3(int8x16_t a)
{
    return a << 3;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shr5(int8x16_t a)
{
    return a >> 5;
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shru7(int8x16_t a)
{
    uint8x16_t ua = (uint8x16_t)a;
    return (int8x16_t)(ua >> 7);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_load(int8x16_t *p)
{
    return __builtin_kvx_load128(p, ".v.s");
}
void __attribute ((noinline))
kvx_int8x16_store(int8x16_t *p, int8x16_t a)
{
    __builtin_kvx_store128(a, p, 0);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_neg_(int8x16_t a)
{
    return __builtin_kvx_negbx(a, 0);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_neg_s(int8x16_t a)
{
    return __builtin_kvx_negbx(a, ".s");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_abs_(int8x16_t a)
{
    return __builtin_kvx_absbx(a, "");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_abs_s(int8x16_t a)
{
    return __builtin_kvx_absbx(a, ".s");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_abd_(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_abdbx(a, b, 0);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_abd_s(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_abdbx(a, b, ".s");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_abds_(int8x16_t a, int8_t b)
{
    return __builtin_kvx_abdbx(a, b-(int8x16_t){0}, 0);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_abd0_(int8x16_t a)
{
    return __builtin_kvx_abdbx(a, (int8x16_t){0}, 0);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_abd0_s(int8x16_t a)
{
    return __builtin_kvx_abdbx(a, (int8x16_t){0}, ".s");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_add_s(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_addbx(a, b, ".s");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_add_us(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_addbx(a, b, ".us");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_adds_(int8x16_t a, int8_t b)
{
    return __builtin_kvx_addbx(a, b-(int8x16_t){0}, 0);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_adds_s(int8x16_t a, int8_t b)
{
    return __builtin_kvx_addbx(a, b-(int8x16_t){0}, ".s");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_adds_us(int8x16_t a, int8_t b)
{
    return __builtin_kvx_addbx(a, b-(int8x16_t){0}, ".us");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_sbf_s(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_sbfbx(a, b, ".s");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_sbf_us(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_sbfbx(a, b, ".us");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_sbfs_(int8x16_t a, int8_t b)
{
    return __builtin_kvx_sbfbx(a, b-(int8x16_t){0}, 0);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_sbfs_s(int8x16_t a, int8_t b)
{
    return __builtin_kvx_sbfbx(a, b-(int8x16_t){0}, ".s");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_sbfs_us(int8x16_t a, int8_t b)
{
    return __builtin_kvx_sbfbx(a, b-(int8x16_t){0}, ".us");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_min(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_minbx(a, b);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_max(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_maxbx(a, b);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_minu(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_minubx(a, b);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_maxu(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_maxubx(a, b);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_mins(int8x16_t a, int8_t b)
{
    return __builtin_kvx_minbx(a, b-(int8x16_t){0});
}
int8x16_t __attribute ((noinline))
kvx_int8x16_maxs(int8x16_t a, int8_t b)
{
    return __builtin_kvx_maxbx(a, b-(int8x16_t){0});
}
int8x16_t __attribute ((noinline))
kvx_int8x16_minus(int8x16_t a, int8_t b)
{
    return __builtin_kvx_minubx(a, b-(int8x16_t){0});
}
int8x16_t __attribute ((noinline))
kvx_int8x16_maxus(int8x16_t a, int8_t b)
{
    return __builtin_kvx_maxubx(a, b-(int8x16_t){0});
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shls_(int8x16_t a, int8_t b)
{
    return __builtin_kvx_shlbxs(a, b, 0);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shl3_(int8x16_t a)
{
    return __builtin_kvx_shlbxs(a, 3, 0);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shls_s(int8x16_t a, int8_t b)
{
    return __builtin_kvx_shlbxs(a, b, ".s");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shl5_s(int8x16_t a)
{
    return __builtin_kvx_shlbxs(a, 5, ".s");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shls_us(int8x16_t a, int8_t b)
{
    return __builtin_kvx_shlbxs(a, b, ".us");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shl7_us(int8x16_t a)
{
    return __builtin_kvx_shlbxs(a, 7, ".us");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shls_r(int8x16_t a, int8_t b)
{
    return __builtin_kvx_shlbxs(a, b, ".r");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shl1_r(int8x16_t a)
{
    return __builtin_kvx_shlbxs(a, 1, ".r");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shrs_(int8x16_t a, int8_t b)
{
    return __builtin_kvx_shrbxs(a, b, 0);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shr5_(int8x16_t a)
{
    return __builtin_kvx_shrbxs(a, 5, 0);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shrs_a(int8x16_t a, int8_t b)
{
    return __builtin_kvx_shrbxs(a, b, ".a");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shr3_a(int8x16_t a)
{
    return __builtin_kvx_shrbxs(a, 3, ".a");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shrs_as(int8x16_t a, int8_t b)
{
    return __builtin_kvx_shrbxs(a, b, ".as");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shr7_as(int8x16_t a)
{
    return __builtin_kvx_shrbxs(a, 7, ".as");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shrs_r(int8x16_t a, int8_t b)
{
    return __builtin_kvx_shrbxs(a, b, ".r");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shr1_r(int8x16_t a)
{
    return __builtin_kvx_shrbxs(a, 1, ".r");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_avg(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_avgbx(a, b, 0);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_avg_u(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_avgbx(a, b, ".u");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_avg_r(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_avgbx(a, b, ".r");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_avg_ru(int8x16_t a, int8x16_t b)
{
    return __builtin_kvx_avgbx(a, b, ".ru");
}
int16x16_t __attribute__ ((noinline))
kvx_int8x16_sxbhx(int8x16_t a)
{
    return __builtin_kvx_widenbhx(a, 0);
}
int16x16_t __attribute__ ((noinline))
kvx_int8x16_zxbhx(int8x16_t a)
{
    return __builtin_kvx_widenbhx(a, ".z");
}
int16x16_t __attribute__ ((noinline))
kvx_int8x16_qxbhx(int8x16_t a)
{
    return __builtin_kvx_widenbhx(a, ".q");
}
int8x16_t __attribute__ ((noinline))
kvx_int8x16_trunchbx(int16x16_t a)
{
    return __builtin_kvx_narrowhbx(a, 0);
}
int8x16_t __attribute__ ((noinline))
kvx_int8x16_fracthbx(int16x16_t a)
{
    return __builtin_kvx_narrowhbx(a, ".q");
}
int8x16_t __attribute__ ((noinline))
kvx_int8x16_sathbx(int16x16_t a)
{
    return __builtin_kvx_narrowhbx(a, ".s");
}
int8x16_t __attribute__ ((noinline))
kvx_int8x16_satuhbx(int16x16_t a)
{
    return __builtin_kvx_narrowhbx(a, ".us");
}
int8x16_t __attribute ((noinline))
kvx_int8x16_select(int8x16_t a, int8x16_t b, int8x16_t c)
{
    return __builtin_kvx_selectbx(a, b, c, 0);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_shift(int8x16_t a, int8_t b)
{
    return __builtin_kvx_shiftbx(a, 1, b);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_catbx(int8x8_t a, int8x8_t b) {
    return __builtin_kvx_catbx(a, b);
}
int8x16_t __attribute ((noinline))
kvx_int8x16_scatbx(int8x8_t a, int8x8_t b) {
    return __builtin_kvx_catbx(b, a);
}
