/* { dg-do  compile } */
/* { dg-options "-O3 -save-temps -fdump-rtl-reload"  } */

char *data;

char foo_10bits (void) {
    return data[10];
}

char foo_10bits_hi (void) {
    return data[511];
}

char foo_10bits_lo (void) {
    return data[-512];
}

/* { dg-final { scan-rtl-dump-times "Choosing alt 7 in insn \[0-9\]+:  \\\(0\\\) r  \\\(1\\\) Ca \\\{\\\*movqi_all\\\}" 3 "reload"} } */

char foo_37bits_hi_lo (void) {
    return data[512];
}

char foo_37bits_lo_hi (void) {
    return data[-513];
}

char foo_37bits_hi_hi (void) {
    return data[68719476735];
}

char foo_37bits_lo_lo (void) {
    return data[-68719476736];
}

/* { dg-final { scan-rtl-dump-times "Choosing alt 8 in insn \[0-9\]+:  \\\(0\\\) r  \\\(1\\\) Cb \\\{\\\*movqi_all\\\}" 4 "reload"} } */

char foo_64bits (void) {
    return data[68719476735+1];
}
/* { dg-final { scan-rtl-dump-times "Choosing alt 9 in insn \[0-9\]+:  \\\(0\\\) r  \\\(1\\\) Cm \\\{\\\*movqi_all\\\}" 1 "reload"} } */


/* { dg-final { scan-assembler-times "lbz \\\$r0 = \[0-9-\]+\\\[\\\$r\[0-9\]+\\\]" 8 } } */

extern struct { long long params[5]; float array[0]; } buffer;
float access_sym(long i) { return buffer.array[i]; }

/* { dg-final { scan-rtl-dump-times "Choosing alt 9 in insn \[0-9\]+:  \\\(0\\\) r  \\\(1\\\) Cm \\\{\\\*movsf_all\\\}" 1 "reload"} } */

