#ifndef __SIMD_TEST_H_
#define __SIMD_TEST_H_

typedef float v2sf __attribute__ ((vector_size(8)));
typedef float v2sf_elt;

typedef short v2hi __attribute__ ((vector_size (4)));
typedef short v4hi __attribute__ ((vector_size (8)));
typedef int v2si __attribute__ ((vector_size (8)));

typedef short v2hi_elt;
typedef short v4hi_elt;
typedef int v2si_elt;

typedef unsigned short uv2hi __attribute__ ((vector_size (4)));
typedef unsigned short uv4hi __attribute__ ((vector_size (8)));
typedef unsigned int uv2si __attribute__ ((vector_size (8)));

typedef unsigned short uv2hi_elt;
typedef unsigned short uv4hi_elt;
typedef unsigned int uv2si_elt;

#endif
