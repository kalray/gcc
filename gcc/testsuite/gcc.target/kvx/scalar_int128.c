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

int128_t __attribute ((noinline))
kvx_int128_incr(int128_t a)
{
    return a + 1;
}
int128_t __attribute ((noinline))
kvx_int128_addto(int128_t a)
{
    int128_t b = 1;
    return a + b;
}
int128_t __attribute ((noinline))
kvx_int128_subfrom(int128_t a)
{
    int128_t b = 1;
    return b - a;
}
int128_t __attribute ((noinline))
kvx_int128_addwhere(int128_t a, int128_t __bypass *b)
{
    return a + *b;
}
int128_t __attribute ((noinline))
kvx_int128_asr(int128_t a, int b)
{
    return a >> b;
}
uint128_t __attribute ((noinline))
kvx_int128_lsr(uint128_t a, int b)
{
    return a >> b;
}
int128_t __attribute ((noinline))
kvx_int128_lsl(int128_t a, int b)
{
    return a << b;
}
int128_t __attribute ((noinline))
kvx_int128_addx2(int128_t a, int128_t b)
{
    return a + (b << 1);
}
int128_t __attribute ((noinline))
kvx_int128_addx4(int128_t a, int128_t b)
{
    return a + (b << 2);
}
int128_t __attribute ((noinline))
kvx_int128_addx8(int128_t a, int128_t b)
{
    return a + (b << 3);
}
int128_t __attribute ((noinline))
kvx_int128_addx16(int128_t a, int128_t b)
{
    return a + (b << 4);
}
int128_t __attribute ((noinline))
kvx_int128_sbfx2(int128_t a, int128_t b)
{
    return a - b*2;
}
int128_t __attribute ((noinline))
kvx_int128_sbfx4(int128_t a, int128_t b)
{
    return a - b*4;
}
int128_t __attribute ((noinline))
kvx_int128_sbfx8(int128_t a, int128_t b)
{
    return a - b*8;
}
int128_t __attribute ((noinline))
kvx_int128_sbfx16(int128_t a, int128_t b)
{
    return a - b*16;
}
int128_t __attribute ((noinline))
kvx_int128_mul(int128_t a, int128_t b)
{
    return a * b;
}
int128_t __attribute ((noinline))
kvx_int128_muls(int128_t a, int128_t b)
{
    return a * b;
}
int128_t __attribute ((noinline))
kvx_int128_madd(int128_t a, int128_t b, int128_t c)
{
    return a * b + c;
}
int128_t __attribute ((noinline))
kvx_int128_msub(int128_t a, int128_t b, int128_t c)
{
    return c - a * b;
}
int128_t __attribute ((noinline))
kvx_int128_subx2(int128_t a, int128_t b)
{
    return a - (b << 1);
}
int128_t __attribute ((noinline))
kvx_int128_subx4(int128_t a, int128_t b)
{
    return a - (b << 2);
}
int128_t __attribute ((noinline))
kvx_int128_subx8(int128_t a, int128_t b)
{
    return a - (b << 3);
}
int128_t __attribute ((noinline))
kvx_int128_subx16(int128_t a, int128_t b)
{
    return a - (b << 4);
}
int128_t __attribute ((noinline))
kvx_int128_neg(int128_t a, int128_t b)
{
    return -a;
}
int128_t __attribute ((noinline))
kvx_int128_not(int128_t a, int128_t b)
{
    return ~a;
}
int128_t __attribute ((noinline))
kvx_int128_and(int128_t a, int128_t b)
{
    return a & b;
}
int128_t __attribute ((noinline))
kvx_int128_nand(int128_t a, int128_t b)
{
    return ~(a & b);
}
int128_t __attribute ((noinline))
kvx_int128_or(int128_t a, int128_t b)
{
    return a | b;
}
int128_t __attribute ((noinline))
kvx_int128_nor(int128_t a, int128_t b)
{
    return ~(a | b);
}
int128_t __attribute ((noinline))
kvx_int128_xor(int128_t a, int128_t b)
{
    return a ^ b;
}
int128_t __attribute ((noinline))
kvx_int128_nxor(int128_t a, int128_t b)
{
    return ~(a ^ b);
}
int128_t __attribute ((noinline))
kvx_int128_andn(int128_t a, int128_t b)
{
    return ~a & b;
}
int128_t __attribute ((noinline))
kvx_int128_orn(int128_t a, int128_t b)
{
    return ~a | b;
}
int128_t __attribute ((noinline))
kvx_int128_xorn(int128_t a, int128_t b)
{
    return ~a ^ b;
}
int128_t __attribute ((noinline))
kvx_int128_ne(int128_t a, int128_t b)
{
    return a != b;
}
int128_t __attribute ((noinline))
kvx_int128_gt(int128_t a, int128_t b)
{
    return a > b;
}
int128_t __attribute ((noinline))
kvx_int128_gtu(int128_t a, int128_t b)
{
    uint128_t ua = (uint128_t)a;
    uint128_t ub = (uint128_t)b;
    return ua > ub;
}
int128_t __attribute ((noinline))
kvx_int128_eqz(int128_t a)
{
    return a == 0;
}
int128_t __attribute ((noinline))
kvx_int128_shl(int128_t a, int b)
{
    return a << b;
}
