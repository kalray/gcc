/* { dg-do  compile } */
/* { dg-skip-if "Test valid only on Bostan" { *-*-* }  { "*" } { "-march=k1b" } } */
/* { dg-options "-O3 -save-temps"  } */


#define GEN_SRS_IMM(p2)				\
  int srs_immediate6bits_##p2(int i){		\
    return i/(1<<p2);				\
  }						\
  long long srsd_immediate6bits_##p2(long long i){	\
    return i/(1<<p2);				\
  }

GEN_SRS_IMM(1)
GEN_SRS_IMM(2)
GEN_SRS_IMM(3)
GEN_SRS_IMM(4)
GEN_SRS_IMM(5)
GEN_SRS_IMM(6)

/* { dg-final { scan-assembler "srs \\\$r\[0-9\]+ = \\\$r\[0-9\]+, 1"  } } */
/* { dg-final { scan-assembler "srsd \\\$r\[0-9\]+r\[0-9\]+ = \\\$r\[0-9\]+r\[0-9\]+, 1"  } } */

/* { dg-final { scan-assembler "srs \\\$r\[0-9\]+ = \\\$r\[0-9\]+, 2"  } } */
/* { dg-final { scan-assembler "srsd \\\$r\[0-9\]+r\[0-9\]+ = \\\$r\[0-9\]+r\[0-9\]+, 2"  } } */

/* { dg-final { scan-assembler "srs \\\$r\[0-9\]+ = \\\$r\[0-9\]+, 3"  } } */
/* { dg-final { scan-assembler "srsd \\\$r\[0-9\]+r\[0-9\]+ = \\\$r\[0-9\]+r\[0-9\]+, 3"  } } */

/* { dg-final { scan-assembler "srs \\\$r\[0-9\]+ = \\\$r\[0-9\]+, 4"  } } */
/* { dg-final { scan-assembler "srsd \\\$r\[0-9\]+r\[0-9\]+ = \\\$r\[0-9\]+r\[0-9\]+, 4"  } } */

/* { dg-final { scan-assembler "srs \\\$r\[0-9\]+ = \\\$r\[0-9\]+, 5"  } } */
/* { dg-final { scan-assembler "srsd \\\$r\[0-9\]+r\[0-9\]+ = \\\$r\[0-9\]+r\[0-9\]+, 5"  } } */

/* { dg-final { scan-assembler "srs \\\$r\[0-9\]+ = \\\$r\[0-9\]+, 6"  } } */
/* { dg-final { scan-assembler "srsd \\\$r\[0-9\]+r\[0-9\]+ = \\\$r\[0-9\]+r\[0-9\]+, 6"  } } */
