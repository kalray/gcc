#ifndef _DIVMOD_TYPES_H
#define _DIVMOD_TYPES_H

typedef __INT16_TYPE__ int16_t;
typedef __UINT16_TYPE__ uint16_t;
typedef int16_t int16x4_t __attribute ((vector_size (4 * sizeof (int16_t))));
typedef int16_t int16x8_t __attribute ((vector_size (8 * sizeof (int16_t))));
typedef int16_t int16x16_t __attribute ((vector_size (16 * sizeof (int16_t))));
typedef int16_t int16x32_t __attribute ((vector_size (32 * sizeof (int16_t))));
typedef uint16_t uint16x4_t __attribute ((vector_size (4 * sizeof (uint16_t))));
typedef uint16_t uint16x8_t __attribute ((vector_size (8 * sizeof (uint16_t))));
typedef uint16_t uint16x16_t __attribute ((vector_size (16 * sizeof (uint16_t))));
typedef uint16_t uint16x32_t __attribute ((vector_size (32 * sizeof (uint16_t))));

typedef __INT32_TYPE__ int32_t;
typedef __UINT32_TYPE__ uint32_t;
typedef int32_t int32x2_t __attribute ((vector_size (2 * sizeof (int32_t))));
typedef int32_t int32x4_t __attribute ((vector_size (4 * sizeof (int32_t))));
typedef int32_t int32x8_t __attribute ((vector_size (8 * sizeof (int32_t))));
typedef int32_t int32x16_t __attribute ((vector_size (16 * sizeof (int32_t))));
typedef uint32_t uint32x2_t __attribute ((vector_size (2 * sizeof (uint32_t))));
typedef uint32_t uint32x4_t __attribute ((vector_size (4 * sizeof (uint32_t))));
typedef uint32_t uint32x8_t __attribute ((vector_size (8 * sizeof (uint32_t))));
typedef uint32_t uint32x16_t __attribute ((vector_size (16 * sizeof (uint32_t))));

typedef __INT64_TYPE__ int64_t;
typedef __UINT64_TYPE__ uint64_t;
typedef int64_t int64x2_t __attribute ((vector_size (2 * sizeof (int64_t))));
typedef int64_t int64x4_t __attribute ((vector_size (4 * sizeof (int64_t))));
typedef int64_t int64x8_t __attribute ((vector_size (8 * sizeof (int64_t))));
typedef uint64_t uint64x2_t __attribute ((vector_size (2 * sizeof (uint64_t))));
typedef uint64_t uint64x4_t __attribute ((vector_size (4 * sizeof (uint64_t))));
typedef uint64_t uint64x8_t __attribute ((vector_size (8 * sizeof (uint64_t))));

typedef float float32_t;
typedef float32_t float32x2_t __attribute ((vector_size (2 * sizeof (float32_t))));
typedef float32_t float32x4_t __attribute ((vector_size (4 * sizeof (float32_t))));
typedef float32_t float32x8_t __attribute ((vector_size (8 * sizeof (float32_t))));

typedef double float64_t;
typedef float64_t float64x2_t __attribute ((vector_size (2 * sizeof (float64_t))));
typedef float64_t float64x4_t __attribute ((vector_size (4 * sizeof (float64_t))));
typedef float64_t float64x8_t __attribute ((vector_size (8 * sizeof (float64_t))));

#ifndef __linux__
/*
 * Setting this symbol non-zero changes the behavior of divmod by zero.
 * The default behavior is to terminate the application with a trap.
 * This feature is needed by the OpenCL-C division where the result is
 * undefined instead of crashing the user application.
 */
extern char *_KVX_NO_DIVMOD0_TRAP __attribute__ ((weak));
#endif

uint64_t __udivdi3 (uint64_t a, uint64_t b);
uint64_t __umoddi3 (uint64_t a, uint64_t b);
uint64_t __udivmoddi4 (uint64_t a, uint64_t b, uint64_t *c);
int64_t __divdi3 (int64_t a, int64_t b);
int64_t __moddi3 (int64_t a, int64_t b);

uint32_t __udivsi3 (uint32_t a, uint32_t b);
uint32_t __umodsi3 (uint32_t a, uint32_t b);
uint32_t __udivmodsi4 (uint32_t a, uint32_t b, uint32_t *c);
int32_t __divsi3 (int32_t a, int32_t b);
int32_t __modsi3 (int32_t a, int32_t b);

uint64x2_t __udivv2di3 (uint64x2_t a, uint64x2_t b);
uint64x2_t __umodv2di3 (uint64x2_t a, uint64x2_t b);
uint64x2_t __udivmodv2di4 (uint64x2_t a, uint64x2_t b, uint64x2_t *c);
int64x2_t __divv2di3 (int64x2_t a, int64x2_t b);
int64x2_t __modv2di3 (int64x2_t a, int64x2_t b);

uint64x4_t __udivv4di3 (uint64x4_t a, uint64x4_t b);
uint64x4_t __umodv4di3 (uint64x4_t a, uint64x4_t b);
uint64x4_t __udivmodv4di4 (uint64x4_t a, uint64x4_t b, uint64x4_t *c);
int64x4_t __divv4di3 (int64x4_t a, int64x4_t b);
int64x4_t __modv4di3 (int64x4_t a, int64x4_t b);

uint32x2_t __udivv2si3 (uint32x2_t a, uint32x2_t b);
uint32x2_t __umodv2si3 (uint32x2_t a, uint32x2_t b);
uint32x2_t __udivmodv2si4 (uint32x2_t a, uint32x2_t b, uint32x2_t *c);
int32x2_t __divv2si3 (int32x2_t a, int32x2_t b);
int32x2_t __modv2si3 (int32x2_t a, int32x2_t b);

uint32x4_t __udivv4si3 (uint32x4_t a, uint32x4_t b);
uint32x4_t __umodv4si3 (uint32x4_t a, uint32x4_t b);
uint32x4_t __udivmodv4si4 (uint32x4_t a, uint32x4_t b, uint32x4_t *c);
int32x4_t __divv4si3 (int32x4_t a, int32x4_t b);
int32x4_t __modv4si3 (int32x4_t a, int32x4_t b);

uint32x8_t __udivv8si3 (uint32x8_t a, uint32x8_t b);
uint32x8_t __umodv8si3 (uint32x8_t a, uint32x8_t b);
uint32x8_t __udivmodv8si4 (uint32x8_t a, uint32x8_t b, uint32x8_t *c);
int32x8_t __divv8si3 (int32x8_t a, int32x8_t b);
int32x8_t __modv8si3 (int32x8_t a, int32x8_t b);

uint16x4_t __udivv4hi3 (uint16x4_t a, uint16x4_t b);
uint16x4_t __umodv4hi3 (uint16x4_t a, uint16x4_t b);
uint16x4_t __udivmodv4hi4 (uint16x4_t a, uint16x4_t b, uint16x4_t *c);
int16x4_t __divv4hi3 (int16x4_t a, int16x4_t b);
int16x4_t __modv4hi3 (int16x4_t a, int16x4_t b);

uint16x8_t __udivv8hi3 (uint16x8_t a, uint16x8_t b);
uint16x8_t __umodv8hi3 (uint16x8_t a, uint16x8_t b);
uint16x8_t __udivmodv8hi4 (uint16x8_t a, uint16x8_t b, uint16x8_t *c);
int16x8_t __divv8hi3 (int16x8_t a, int16x8_t b);
int16x8_t __modv8hi3 (int16x8_t a, int16x8_t b);

uint16x16_t __udivv16hi3 (uint16x16_t a, uint16x16_t b);
uint16x16_t __umodv16hi3 (uint16x16_t a, uint16x16_t b);
uint16x16_t __udivmodv16hi4 (uint16x16_t a, uint16x16_t b, uint16x16_t *c);
int16x16_t __divv16hi3 (int16x16_t a, int16x16_t b);
int16x16_t __modv16hi3 (int16x16_t a, int16x16_t b);

//#define TEST_V2SI
#define TEST_V4SI
//#define TEST_V2DI
#define TEST_V4DI

#endif//_DIVMOD_TYPES_H

