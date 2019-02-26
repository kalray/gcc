/* { dg-do run } */
/* { dg-options "-Os -save-temps -Wall -Werror" } */
/* { dg-skip-if "Test valid only on Andey/Bostan" { *-*-* }  {  "*" } { "-march=k1a" "-march=k1b" } } */


extern void abort ();

char  byte_values[3] = { -1, 1, 3 };

short short_values[3] = { -1, 1, 3 };

int   int_values[3] = { -1, 1, 3 };

long long long_long_values[3] = { -1, 1, 3 };

void test_sbu_lbsu() {
#ifndef __k1a__
  char value;
  __builtin_k1_sbu(&byte_values[1],byte_values[0]);
  value = __builtin_k1_lbsu(&byte_values[1]);
  if(value != -1) {
    abort();
  }
#endif
}

void test_sbu_lbzu() {
#ifndef __k1a__
  unsigned char value;
  __builtin_k1_sbu(&byte_values[1],byte_values[0]);
  value = __builtin_k1_lbzu(&byte_values[1]);
  if(value != 0xff) {
    abort();
  }
#endif
}

void test_shu_lhsu() {
#ifndef __k1a__
  short value;
  __builtin_k1_shu(&short_values[1],short_values[0]);
  value = __builtin_k1_lhsu(&short_values[1]);
  if(value != -1) {
    abort();
  }
#endif
}

void test_shu_lhzu() {
#ifndef __k1a__
  unsigned short value;
  __builtin_k1_shu(&short_values[1],short_values[0]);
  value = __builtin_k1_lhzu(&short_values[1]);
  if(value != 0xffff) {
    abort();
  }
#endif
}

void test_swu_lbqzu() {
#ifndef __k1a__
  unsigned long long value;
  __builtin_k1_swu(&int_values[1],((int_values[2] << 24) | (int_values[2] << 16) | (int_values[2] << 8) | (int_values[2] << 0)));
  value = __builtin_k1_lbqzu(&int_values[1]);
  if(value != 0x0003000300030003ll) {
    abort();
  }
#endif
}

void test_sw_lbqz() {
  long long value;
  int_values[1] = ((int_values[2] << 24) | (int_values[2] << 16) | (int_values[2] << 8) | (int_values[2] << 0));
  value = __builtin_k1_lbqz(&int_values[1]);
  if(value != 0x0003000300030003ll) {
    abort();
  }
}

void test_swu_lbqsu() {
#ifndef __k1a__
  long long value;
  __builtin_k1_swu(&int_values[1],((int_values[2] << 24) | (int_values[2] << 16) | (int_values[2] << 8) | (int_values[2] << 0)));
  value = __builtin_k1_lbqsu(&int_values[1]);
  if(value != 0x0003000300030003ll) {
    abort();
  }
#endif
}

void test_sw_lbqs() {
#ifndef __k1a__
  long long value;
  int_values[1] = ((int_values[2] << 24) | (int_values[2] << 16) | (int_values[2] << 8) | (int_values[2] << 0));
  value = __builtin_k1_lbqs(&int_values[1]);
  if(value != 0x0003000300030003ll) {
    abort();
  }
#endif
}

void test_acws_ld() {
#ifndef __k1a__
  unsigned long long value;
  int int_value;
  int_values[0] = -1;
  int_values[1] = 1;
  __builtin_k1_wpurge();
  __builtin_k1_fence();
  value = __builtin_k1_acws((void *)&int_values[1], 0xffffffff, 0x1);
  if(value != 0x100000001ULL) {
    abort();
  }

  int_value = (volatile int)int_values[1];
  if(int_value != (volatile int)int_values[0]) {
    abort();
  }
#endif
}

void test_afda_ld() {
#ifndef __k1a__
  long long value;
  long_long_values[0] = -1;
  long_long_values[1] = 1;
  value = long_long_values[1];
  value = __builtin_k1_afda(&long_long_values[0], value);
  if(value != -1LL) {
    abort();
  }

  value = long_long_values[0];
  if(value != 0LL) {
    abort();
  }
#endif
}

int main() {
  test_sbu_lbsu();
  test_sbu_lbzu();
  test_shu_lhsu();
  test_shu_lhzu();
  test_swu_lbqzu();
  test_sw_lbqz();
  test_swu_lbqsu();
  test_sw_lbqs();
  test_aldc_ldu();
  test_acws_ld();
  test_afda_ld();
  return 0;
}
/* { dg-final { scan-assembler-not "call __builtin" } } */
/* { dg-final { cleanup-saved-temps } } */
