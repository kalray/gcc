/* { dg-do  compile } */
/* { dg-skip-if "Only supported on Coolidge" { *-*-* }  { "*" } { "-march=k1c" } } */
/* { dg-options "-Waddress-space-conversion"  } */

/* Conversion warnings. */

int * bc(__bypass int *a) {
    return a;
}
/* { dg-warning "Implicit conversion from uncached pointer to cached one" "" { target *-*-* } 8 } */

__bypass int * cb(int *a) {
    return a;
}
/* { dg-warning "Implicit conversion from cached pointer to uncached one" "" { target *-*-* } 13 } */

int * bca(__bypass int *a) {
    return (__convert int *) a;
}

__bypass int * cba(int *a) {
    return (__convert int *) a;
}

int * pc(__preload int *a) {
    return a;
}
/* { dg-warning "Implicit conversion from uncached pointer to cached one" "" { target *-*-* } 26 } */

__preload int * cp(int *a) {
    return a;
}
/* { dg-warning "Implicit conversion from cached pointer to uncached one" "" { target *-*-* } 31 } */

int * pca(__preload int *a) {
    return (__convert int *) a;
}

__preload int * cpa(int *a) {
    return (__convert int *) a;
}


/* Bypass loads. */

int read_b1 (__bypass signed char* a) {
    return *a;
}
/* { dg-final { scan-assembler "lbs.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */

int read_b1u (__bypass unsigned char* a) {
    return *a;
}
/* { dg-final { scan-assembler "lbz.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


int read_b2 (__bypass short* a) {
    return *a;
}
/* { dg-final { scan-assembler "lhs.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


int read_b2u (__bypass unsigned short* a) {
    return *a;
}
/* { dg-final { scan-assembler "lhz.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


long long read_b4 (__bypass int* a) {
    return *a;
}
/* { dg-final { scan-assembler "lws.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


long long read_b4u (__bypass unsigned int* a) {
    return *a;
}
/* { dg-final { scan-assembler "lwz.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


long long read_b8 (__bypass long long* a) {
    return *a;
}
/* { dg-final { scan-assembler "ld.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


/* Pre-loads. */

int read_p1 (__preload signed char* a) {
    return *a;
}
/* { dg-final { scan-assembler "lbs.us \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */

int read_p1u (__preload unsigned char* a) {
    return *a;
}
/* { dg-final { scan-assembler "lbz.us \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


int read_p2 (__preload short* a) {
    return *a;
}
/* { dg-final { scan-assembler "lhs.us \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


int read_p2u (__preload unsigned short* a) {
    return *a;
}
/* { dg-final { scan-assembler "lhz.us \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


long long read_p4 (__preload int* a) {
    return *a;
}
/* { dg-final { scan-assembler "lws.us \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


long long read_p4u (__preload unsigned int* a) {
    return *a;
}
/* { dg-final { scan-assembler "lwz.us \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


long long read_p8 (__preload long long* a) {
    return *a;
}
/* { dg-final { scan-assembler "ld.us \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


