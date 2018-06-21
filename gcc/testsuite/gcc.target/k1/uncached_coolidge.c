/* { dg-do  compile } */
/* { dg-skip-if "Only supported on Coolidge" { *-*-* }  { "*" } { "-march=k1c" } } */
/* { dg-options "-save-temps -Waddress-space-conversion"  } */

/* Conversion warnings. */

int * uc(__uncached int *a) {
    return a;
}
/* { dg-warning "Implicit conversion of uncached pointer to cached one" "" { target *-*-* } 8 } */

__uncached int * cu(int *a) {
    return a;
}
/* { dg-warning "Implicit conversion of cached pointer to uncached one" "" { target *-*-* } 13 } */

int * uca(__uncached int *a) {
    return (__convert int *) a;
}

__uncached int * cua(int *a) {
    return (__convert int *) a;
}


/* Uncached loads. */

char read_1 (__uncached char* a) {
    return *a;
}
/* { dg-final { scan-assembler "lbs.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */

unsigned char read_1u (__uncached unsigned char* a) {
    return *a;
}
/* { dg-final { scan-assembler "lbz.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


short read_2 (__uncached short* a) {
    return *a;
}
/* { dg-final { scan-assembler "lhs.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


unsigned short read_2u (__uncached unsigned short* a) {
    return *a;
}
/* { dg-final { scan-assembler "lhz.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


int read_4 (__uncached int* a) {
    return *a;
}
/* { dg-final { scan-assembler "lws.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


unsigned int read_4u (__uncached unsigned int* a) {
    return *a;
}
/* { dg-final { scan-assembler "lwz.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


long long read_8 (__uncached long long* a) {
    return *a;
}
/* { dg-final { scan-assembler "ld.u \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */


