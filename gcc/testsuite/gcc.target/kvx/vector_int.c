/* { dg-do  compile } */
/* { dg-options "-O2" } */

#include <stdint.h>

typedef int16_t int16x4_t __attribute((vector_size(4*sizeof(int16_t))));
typedef uint16_t uint16x4_t __attribute((vector_size(4*sizeof(int16_t))));
int16x4_t __attribute ((noinline))
k1_int16x4_incr(int16x4_t a)
{
    return a + 1;
}
int16x4_t __attribute ((noinline))
k1_int16x4_addto(int16x4_t a)
{
    int16x4_t b = { 1, 2, 3, 4 };
    return a + b;
}
int16x4_t __attribute ((noinline))
k1_int16x4_subfrom(int16x4_t a)
{
    int16x4_t b = { 1, 2, 3, 4 };
    return b - a;
}
int16x4_t __attribute ((noinline))
k1_int16x4_addwhere(int16x4_t a, int16x4_t __bypass *b)
{
    return a + *b;
}
int16x4_t __attribute ((noinline))
k1_int16x4_asr(int16x4_t a, int b)
{
    return a >> b;
}
uint16x4_t __attribute ((noinline))
k1_int16x4_lsr(uint16x4_t a, int b)
{
    return a >> b;
}
int16x4_t __attribute ((noinline))
k1_int16x4_lsl(int16x4_t a, int b)
{
    return a << b;
}
int16x4_t __attribute ((noinline))
k1_int16x4_addx2(int16x4_t a, int16x4_t b)
{
    return a + (b << 1);
}
int16x4_t __attribute ((noinline))
k1_int16x4_addx4(int16x4_t a, int16x4_t b)
{
    return a + (b << 2);
}
int16x4_t __attribute ((noinline))
k1_int16x4_addx8(int16x4_t a, int16x4_t b)
{
    return a + (b << 3);
}
int16x4_t __attribute ((noinline))
k1_int16x4_addx16(int16x4_t a, int16x4_t b)
{
    return a + (b << 4);
}
int16x4_t __attribute ((noinline))
k1_int16x4_muls(int16x4_t a, int16_t b)
{
    return a * b;
}
int16x4_t __attribute ((noinline))
k1_int16x4_mul(int16x4_t a, int16x4_t b)
{
    return a * b;
}
int16x4_t __attribute ((noinline))
k1_int16x4_madd(int16x4_t a, int16x4_t b, int16x4_t c)
{
    return a * b + c;
}
int16x4_t __attribute ((noinline))
k1_int16x4_msub(int16x4_t a, int16x4_t b, int16x4_t c)
{
    return c - a * b;
}
int16x4_t __attribute ((noinline))
k1_int16x4_subx2(int16x4_t a, int16x4_t b)
{
    return a - (b << 1);
}
int16x4_t __attribute ((noinline))
k1_int16x4_subx4(int16x4_t a, int16x4_t b)
{
    return a - (b << 2);
}
int16x4_t __attribute ((noinline))
k1_int16x4_subx8(int16x4_t a, int16x4_t b)
{
    return a - (b << 3);
}
int16x4_t __attribute ((noinline))
k1_int16x4_subx16(int16x4_t a, int16x4_t b)
{
    return a - (b << 4);
}
int16x4_t __attribute ((noinline))
k1_int16x4_neg(int16x4_t a, int16x4_t b)
{
    return -a;
}
int16x4_t __attribute ((noinline))
k1_int16x4_not(int16x4_t a, int16x4_t b)
{
    return ~a;
}
int16x4_t __attribute ((noinline))
k1_int16x4_and(int16x4_t a, int16x4_t b)
{
    return a & b;
}
int16x4_t __attribute ((noinline))
k1_int16x4_nand(int16x4_t a, int16x4_t b)
{
    return ~(a & b);
}
int16x4_t __attribute ((noinline))
k1_int16x4_or(int16x4_t a, int16x4_t b)
{
    return a | b;
}
int16x4_t __attribute ((noinline))
k1_int16x4_nor(int16x4_t a, int16x4_t b)
{
    return ~(a | b);
}
int16x4_t __attribute ((noinline))
k1_int16x4_xor(int16x4_t a, int16x4_t b)
{
    return a ^ b;
}
int16x4_t __attribute ((noinline))
k1_int16x4_nxor(int16x4_t a, int16x4_t b)
{
    return ~(a ^ b);
}
int16x4_t __attribute ((noinline))
k1_int16x4_andn(int16x4_t a, int16x4_t b)
{
    return ~a & b;
}
int16x4_t __attribute ((noinline))
k1_int16x4_orn(int16x4_t a, int16x4_t b)
{
    return ~a | b;
}
int16x4_t __attribute ((noinline))
k1_int16x4_xorn(int16x4_t a, int16x4_t b)
{
    return ~a ^ b;
}

typedef int16_t int16x8_t __attribute((vector_size(8*sizeof(int16_t))));
typedef uint16_t uint16x8_t __attribute((vector_size(8*sizeof(int16_t))));
int16x8_t __attribute ((noinline))
k1_int16x8_incr(int16x8_t a)
{
    return a + 1;
}
int16x8_t __attribute ((noinline))
k1_int16x8_addto(int16x8_t a)
{
    int16x8_t b = { 1, 2, 3, 4, 5, 6, 7, 8 };
    return a + b;
}
int16x8_t __attribute ((noinline))
k1_int16x8_subfrom(int16x8_t a)
{
    int16x8_t b = { 1, 2, 3, 4, 5, 6, 7, 8 };
    return b - a;
}
int16x8_t __attribute ((noinline))
k1_int16x8_addwhere(int16x8_t a, int16x8_t __bypass *b)
{
    return a + *b;
}
int16x8_t __attribute ((noinline))
k1_int16x8_asr(int16x8_t a, int b)
{
    return a >> b;
}
uint16x8_t __attribute ((noinline))
k1_int16x8_lsr(uint16x8_t a, int b)
{
    return a >> b;
}
int16x8_t __attribute ((noinline))
k1_int16x8_lsl(int16x8_t a, int b)
{
    return a << b;
}
int16x8_t __attribute ((noinline))
k1_int16x8_addx2(int16x8_t a, int16x8_t b)
{
    return a + (b << 1);
}
int16x8_t __attribute ((noinline))
k1_int16x8_addx4(int16x8_t a, int16x8_t b)
{
    return a + (b << 2);
}
int16x8_t __attribute ((noinline))
k1_int16x8_addx8(int16x8_t a, int16x8_t b)
{
    return a + (b << 3);
}
int16x8_t __attribute ((noinline))
k1_int16x8_addx16(int16x8_t a, int16x8_t b)
{
    return a + (b << 4);
}
int16x8_t __attribute ((noinline))
k1_int16x8_muls(int16x8_t a, int16_t b)
{
    return a * b;
}
int16x8_t __attribute ((noinline))
k1_int16x8_mul(int16x8_t a, int16x8_t b)
{
    return a * b;
}
int16x8_t __attribute ((noinline))
k1_int16x8_madd(int16x8_t a, int16x8_t b, int16x8_t c)
{
    return a * b + c;
}
int16x8_t __attribute ((noinline))
k1_int16x8_msub(int16x8_t a, int16x8_t b, int16x8_t c)
{
    return c - a * b;
}
int16x8_t __attribute ((noinline))
k1_int16x8_subx2(int16x8_t a, int16x8_t b)
{
    return a - (b << 1);
}
int16x8_t __attribute ((noinline))
k1_int16x8_subx4(int16x8_t a, int16x8_t b)
{
    return a - (b << 2);
}
int16x8_t __attribute ((noinline))
k1_int16x8_subx8(int16x8_t a, int16x8_t b)
{
    return a - (b << 3);
}
int16x8_t __attribute ((noinline))
k1_int16x8_subx16(int16x8_t a, int16x8_t b)
{
    return a - (b << 4);
}
int16x8_t __attribute ((noinline))
k1_int16x8_neg(int16x8_t a, int16x8_t b)
{
    return -a;
}
int16x8_t __attribute ((noinline))
k1_int16x8_not(int16x8_t a, int16x8_t b)
{
    return ~a;
}
int16x8_t __attribute ((noinline))
k1_int16x8_and(int16x8_t a, int16x8_t b)
{
    return a & b;
}
int16x8_t __attribute ((noinline))
k1_int16x8_nand(int16x8_t a, int16x8_t b)
{
    return ~(a & b);
}
int16x8_t __attribute ((noinline))
k1_int16x8_or(int16x8_t a, int16x8_t b)
{
    return a | b;
}
int16x8_t __attribute ((noinline))
k1_int16x8_nor(int16x8_t a, int16x8_t b)
{
    return ~(a | b);
}
int16x8_t __attribute ((noinline))
k1_int16x8_xor(int16x8_t a, int16x8_t b)
{
    return a ^ b;
}
int16x8_t __attribute ((noinline))
k1_int16x8_nxor(int16x8_t a, int16x8_t b)
{
    return ~(a ^ b);
}
int16x8_t __attribute ((noinline))
k1_int16x8_andn(int16x8_t a, int16x8_t b)
{
    return ~a & b;
}
int16x8_t __attribute ((noinline))
k1_int16x8_orn(int16x8_t a, int16x8_t b)
{
    return ~a | b;
}
int16x8_t __attribute ((noinline))
k1_int16x8_xorn(int16x8_t a, int16x8_t b)
{
    return ~a ^ b;
}

typedef int32_t int32x2_t __attribute((vector_size(2*sizeof(int32_t))));
typedef uint32_t uint32x2_t __attribute((vector_size(2*sizeof(int32_t))));
int32x2_t __attribute ((noinline))
k1_int32x2_incr(int32x2_t a)
{
    return a + 1;
}
int32x2_t __attribute ((noinline))
k1_int32x2_addto(int32x2_t a)
{
    int32x2_t b = { 1, 2 };
    return a + b;
}
int32x2_t __attribute ((noinline))
k1_int32x2_subfrom(int32x2_t a)
{
    int32x2_t b = { 1, 2 };
    return b - a;
}
int32x2_t __attribute ((noinline))
k1_int32x2_addwhere(int32x2_t a, int32x2_t __bypass *b)
{
    return a + *b;
}
int32x2_t __attribute ((noinline))
k1_int32x2_asr(int32x2_t a, int b)
{
    return a >> b;
}
uint32x2_t __attribute ((noinline))
k1_int32x2_lsr(uint32x2_t a, int b)
{
    return a >> b;
}
int32x2_t __attribute ((noinline))
k1_int32x2_lsl(int32x2_t a, int b)
{
    return a << b;
}
int32x2_t __attribute ((noinline))
k1_int32x2_addx2(int32x2_t a, int32x2_t b)
{
    return a + (b << 1);
}
int32x2_t __attribute ((noinline))
k1_int32x2_addx4(int32x2_t a, int32x2_t b)
{
    return a + (b << 2);
}
int32x2_t __attribute ((noinline))
k1_int32x2_addx8(int32x2_t a, int32x2_t b)
{
    return a + (b << 3);
}
int32x2_t __attribute ((noinline))
k1_int32x2_addx16(int32x2_t a, int32x2_t b)
{
    return a + (b << 4);
}
int32x2_t __attribute ((noinline))
k1_int32x2_muls(int32x2_t a, int32_t b)
{
    return a * b;
}
int32x2_t __attribute ((noinline))
k1_int32x2_mul(int32x2_t a, int32x2_t b)
{
    return a * b;
}
int32x2_t __attribute ((noinline))
k1_int32x2_madd(int32x2_t a, int32x2_t b, int32x2_t c)
{
    return a * b + c;
}
int32x2_t __attribute ((noinline))
k1_int32x2_msub(int32x2_t a, int32x2_t b, int32x2_t c)
{
    return c - a * b;
}
int32x2_t __attribute ((noinline))
k1_int32x2_subx2(int32x2_t a, int32x2_t b)
{
    return a - (b << 1);
}
int32x2_t __attribute ((noinline))
k1_int32x2_subx4(int32x2_t a, int32x2_t b)
{
    return a - (b << 2);
}
int32x2_t __attribute ((noinline))
k1_int32x2_subx8(int32x2_t a, int32x2_t b)
{
    return a - (b << 3);
}
int32x2_t __attribute ((noinline))
k1_int32x2_subx16(int32x2_t a, int32x2_t b)
{
    return a - (b << 4);
}
int32x2_t __attribute ((noinline))
k1_int32x2_neg(int32x2_t a, int32x2_t b)
{
    return -a;
}
int32x2_t __attribute ((noinline))
k1_int32x2_not(int32x2_t a, int32x2_t b)
{
    return ~a;
}
int32x2_t __attribute ((noinline))
k1_int32x2_and(int32x2_t a, int32x2_t b)
{
    return a & b;
}
int32x2_t __attribute ((noinline))
k1_int32x2_nand(int32x2_t a, int32x2_t b)
{
    return ~(a & b);
}
int32x2_t __attribute ((noinline))
k1_int32x2_or(int32x2_t a, int32x2_t b)
{
    return a | b;
}
int32x2_t __attribute ((noinline))
k1_int32x2_nor(int32x2_t a, int32x2_t b)
{
    return ~(a | b);
}
int32x2_t __attribute ((noinline))
k1_int32x2_xor(int32x2_t a, int32x2_t b)
{
    return a ^ b;
}
int32x2_t __attribute ((noinline))
k1_int32x2_nxor(int32x2_t a, int32x2_t b)
{
    return ~(a ^ b);
}
int32x2_t __attribute ((noinline))
k1_int32x2_andn(int32x2_t a, int32x2_t b)
{
    return ~a & b;
}
int32x2_t __attribute ((noinline))
k1_int32x2_orn(int32x2_t a, int32x2_t b)
{
    return ~a | b;
}
int32x2_t __attribute ((noinline))
k1_int32x2_xorn(int32x2_t a, int32x2_t b)
{
    return ~a ^ b;
}

typedef int32_t int32x4_t __attribute((vector_size(4*sizeof(int32_t))));
typedef uint32_t uint32x4_t __attribute((vector_size(4*sizeof(int32_t))));
int32x4_t __attribute ((noinline))
k1_int32x4_incr(int32x4_t a)
{
    return a + 1;
}
int32x4_t __attribute ((noinline))
k1_int32x4_addto(int32x4_t a)
{
    int32x4_t b = { 1, 2, 3, 4 };
    return a + b;
}
int32x4_t __attribute ((noinline))
k1_int32x4_subfrom(int32x4_t a)
{
    int32x4_t b = { 1, 2, 3, 4 };
    return b - a;
}
int32x4_t __attribute ((noinline))
k1_int32x4_addwhere(int32x4_t a, int32x4_t __bypass *b)
{
    return a + *b;
}
int32x4_t __attribute ((noinline))
k1_int32x4_asr(int32x4_t a, int b)
{
    return a >> b;
}
uint32x4_t __attribute ((noinline))
k1_int32x4_lsr(uint32x4_t a, int b)
{
    return a >> b;
}
int32x4_t __attribute ((noinline))
k1_int32x4_lsl(int32x4_t a, int b)
{
    return a << b;
}
int32x4_t __attribute ((noinline))
k1_int32x4_addx2(int32x4_t a, int32x4_t b)
{
    return a + (b << 1);
}
int32x4_t __attribute ((noinline))
k1_int32x4_addx4(int32x4_t a, int32x4_t b)
{
    return a + (b << 2);
}
int32x4_t __attribute ((noinline))
k1_int32x4_addx8(int32x4_t a, int32x4_t b)
{
    return a + (b << 3);
}
int32x4_t __attribute ((noinline))
k1_int32x4_addx16(int32x4_t a, int32x4_t b)
{
    return a + (b << 4);
}
int32x4_t __attribute ((noinline))
k1_int32x4_muls(int32x4_t a, int32_t b)
{
    return a * b;
}
int32x4_t __attribute ((noinline))
k1_int32x4_mul(int32x4_t a, int32x4_t b)
{
    return a * b;
}
int32x4_t __attribute ((noinline))
k1_int32x4_madd(int32x4_t a, int32x4_t b, int32x4_t c)
{
    return a * b + c;
}
int32x4_t __attribute ((noinline))
k1_int32x4_msub(int32x4_t a, int32x4_t b, int32x4_t c)
{
    return c - a * b;
}
int32x4_t __attribute ((noinline))
k1_int32x4_subx2(int32x4_t a, int32x4_t b)
{
    return a - (b << 1);
}
int32x4_t __attribute ((noinline))
k1_int32x4_subx4(int32x4_t a, int32x4_t b)
{
    return a - (b << 2);
}
int32x4_t __attribute ((noinline))
k1_int32x4_subx8(int32x4_t a, int32x4_t b)
{
    return a - (b << 3);
}
int32x4_t __attribute ((noinline))
k1_int32x4_subx16(int32x4_t a, int32x4_t b)
{
    return a - (b << 4);
}
int32x4_t __attribute ((noinline))
k1_int32x4_neg(int32x4_t a, int32x4_t b)
{
    return -a;
}
int32x4_t __attribute ((noinline))
k1_int32x4_not(int32x4_t a, int32x4_t b)
{
    return ~a;
}
int32x4_t __attribute ((noinline))
k1_int32x4_and(int32x4_t a, int32x4_t b)
{
    return a & b;
}
int32x4_t __attribute ((noinline))
k1_int32x4_nand(int32x4_t a, int32x4_t b)
{
    return ~(a & b);
}
int32x4_t __attribute ((noinline))
k1_int32x4_or(int32x4_t a, int32x4_t b)
{
    return a | b;
}
int32x4_t __attribute ((noinline))
k1_int32x4_nor(int32x4_t a, int32x4_t b)
{
    return ~(a | b);
}
int32x4_t __attribute ((noinline))
k1_int32x4_xor(int32x4_t a, int32x4_t b)
{
    return a ^ b;
}
int32x4_t __attribute ((noinline))
k1_int32x4_nxor(int32x4_t a, int32x4_t b)
{
    return ~(a ^ b);
}
int32x4_t __attribute ((noinline))
k1_int32x4_andn(int32x4_t a, int32x4_t b)
{
    return ~a & b;
}
int32x4_t __attribute ((noinline))
k1_int32x4_orn(int32x4_t a, int32x4_t b)
{
    return ~a | b;
}
int32x4_t __attribute ((noinline))
k1_int32x4_xorn(int32x4_t a, int32x4_t b)
{
    return ~a ^ b;
}

typedef int64_t int64x2_t __attribute((vector_size(2*sizeof(int64_t))));
typedef uint64_t uint64x2_t __attribute((vector_size(2*sizeof(int64_t))));
int64x2_t __attribute ((noinline))
k1_int64x2_incr(int64x2_t a)
{
    return a + 1;
}
int64x2_t __attribute ((noinline))
k1_int64x2_addto(int64x2_t a)
{
    int64x2_t b = { 1, 2 };
    return a + b;
}
int64x2_t __attribute ((noinline))
k1_int64x2_subfrom(int64x2_t a)
{
    int64x2_t b = { 1, 2 };
    return b - a;
}
int64x2_t __attribute ((noinline))
k1_int64x2_addwhere(int64x2_t a, int64x2_t __bypass *b)
{
    return a + *b;
}
int64x2_t __attribute ((noinline))
k1_int64x2_asr(int64x2_t a, int b)
{
    return a >> b;
}
uint64x2_t __attribute ((noinline))
k1_int64x2_lsr(uint64x2_t a, int b)
{
    return a >> b;
}
int64x2_t __attribute ((noinline))
k1_int64x2_lsl(int64x2_t a, int b)
{
    return a << b;
}
int64x2_t __attribute ((noinline))
k1_int64x2_addx2(int64x2_t a, int64x2_t b)
{
    return a + (b << 1);
}
int64x2_t __attribute ((noinline))
k1_int64x2_addx4(int64x2_t a, int64x2_t b)
{
    return a + (b << 2);
}
int64x2_t __attribute ((noinline))
k1_int64x2_addx8(int64x2_t a, int64x2_t b)
{
    return a + (b << 3);
}
int64x2_t __attribute ((noinline))
k1_int64x2_addx16(int64x2_t a, int64x2_t b)
{
    return a + (b << 4);
}
int64x2_t __attribute ((noinline))
k1_int64x2_muls(int64x2_t a, int64_t b)
{
    return a * b;
}
int64x2_t __attribute ((noinline))
k1_int64x2_mul(int64x2_t a, int64x2_t b)
{
    return a * b;
}
int64x2_t __attribute ((noinline))
k1_int64x2_madd(int64x2_t a, int64x2_t b, int64x2_t c)
{
    return a * b + c;
}
int64x2_t __attribute ((noinline))
k1_int64x2_msub(int64x2_t a, int64x2_t b, int64x2_t c)
{
    return c - a * b;
}
int64x2_t __attribute ((noinline))
k1_int64x2_subx2(int64x2_t a, int64x2_t b)
{
    return a - (b << 1);
}
int64x2_t __attribute ((noinline))
k1_int64x2_subx4(int64x2_t a, int64x2_t b)
{
    return a - (b << 2);
}
int64x2_t __attribute ((noinline))
k1_int64x2_subx8(int64x2_t a, int64x2_t b)
{
    return a - (b << 3);
}
int64x2_t __attribute ((noinline))
k1_int64x2_subx16(int64x2_t a, int64x2_t b)
{
    return a - (b << 4);
}
int64x2_t __attribute ((noinline))
k1_int64x2_neg(int64x2_t a, int64x2_t b)
{
    return -a;
}
int64x2_t __attribute ((noinline))
k1_int64x2_not(int64x2_t a, int64x2_t b)
{
    return ~a;
}
int64x2_t __attribute ((noinline))
k1_int64x2_and(int64x2_t a, int64x2_t b)
{
    return a & b;
}
int64x2_t __attribute ((noinline))
k1_int64x2_nand(int64x2_t a, int64x2_t b)
{
    return ~(a & b);
}
int64x2_t __attribute ((noinline))
k1_int64x2_or(int64x2_t a, int64x2_t b)
{
    return a | b;
}
int64x2_t __attribute ((noinline))
k1_int64x2_nor(int64x2_t a, int64x2_t b)
{
    return ~(a | b);
}
int64x2_t __attribute ((noinline))
k1_int64x2_xor(int64x2_t a, int64x2_t b)
{
    return a ^ b;
}
int64x2_t __attribute ((noinline))
k1_int64x2_nxor(int64x2_t a, int64x2_t b)
{
    return ~(a ^ b);
}
int64x2_t __attribute ((noinline))
k1_int64x2_andn(int64x2_t a, int64x2_t b)
{
    return ~a & b;
}
int64x2_t __attribute ((noinline))
k1_int64x2_orn(int64x2_t a, int64x2_t b)
{
    return ~a | b;
}
int64x2_t __attribute ((noinline))
k1_int64x2_xorn(int64x2_t a, int64x2_t b)
{
    return ~a ^ b;
}

/* { dg-final { scan-assembler-times "mulwq \\\$r0r1" 2 } } */
