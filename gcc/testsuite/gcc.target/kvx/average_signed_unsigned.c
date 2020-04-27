/* { dg-do  compile } */
/* { dg-options "-O2" } */

signed int k1_avgw(signed int a, signed int b) {
    return (a + b) >> 1;
}

signed int k1_avgw_cst(signed int a) {
    return (a + 0xdeadbeef) >> 1;
}

signed int k1_avgw_builtin(signed int a, signed int b) {
    return __builtin_kvx_avgw(a, b);
}
/* { dg-final { scan-assembler-times "avgw \\\$r0 = \\\$r0, \\\$r1" 1 } } */

signed int k1_avgw_builtin_cst(signed int a) {
    return __builtin_kvx_avgw(a, 0xdeadbeef);
}
/* { dg-final { scan-assembler-times "avgw \\\$r0 = \\\$r0, 0xffffffffdeadbeef" 1 } } */


signed int k1_avgrw(signed int a, signed int b) {
    return (a + b + 1) >> 1;
}

signed int k1_avgrw_builtin(signed int a, signed int b) {
    return __builtin_kvx_avgrw(a, b);
}
/* { dg-final { scan-assembler-times "avgrw \\\$r0 = \\\$r0, \\\$r1" 1 } } */


unsigned int k1_avguw(unsigned int a, unsigned int b) {
    return (a + b) >> 1;
}

unsigned int k1_avguw_cst(unsigned int a) {
    return (a + 0xdeadbeef) >> 1;
}

unsigned int k1_avguw_builtin(unsigned int a, unsigned int b) {
    return __builtin_kvx_avguw(a, b);
}
/* { dg-final { scan-assembler-times "avguw \\\$r0 = \\\$r0, \\\$r1" 1 } } */

unsigned int k1_avguw_builtin_cst(unsigned int a) {
    return __builtin_kvx_avguw(a, 0xdeadbeef);
}
/* { dg-final { scan-assembler-times "avguw \\\$r0 = \\\$r0, 0xffffffffdeadbeef" 1 } } */


unsigned int k1_avgruw(unsigned int a, unsigned int b) {
    return (a + b + 1) >> 1;
}

unsigned int k1_avgruw_builtin(unsigned int a, unsigned int b) {
    return __builtin_kvx_avgruw(a, b);
}
/* { dg-final { scan-assembler-times "avgruw \\\$r0 = \\\$r0, \\\$r1" 1 } } */

