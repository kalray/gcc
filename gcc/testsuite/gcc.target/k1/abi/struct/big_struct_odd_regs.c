struct does_not_fit_in_a_reg
{
  char f0_0;
  char f1_0;
  char f2_0;
  char f3_0;
  int i4_0;
  long long l5_1;
  long long l6_2;
  long long l7_3;
  long long l8_4;
};

int check_args_are_correct (int a0, struct does_not_fit_in_a_reg s1234, int a5,
			    int a6);

int
check_args_are_correct_user (int a0, struct does_not_fit_in_a_reg s1234, int a5,
			     int a6)
{
  return a0 == 0 && s1234.f0_0 == (char) 0xCC && s1234.f1_0 == (char) 0xDD
	 && s1234.f2_0 == (char) 0xEE && s1234.f3_0 == (char) 0xFF
	 && s1234.i4_0 == (int) 0xdeadbeef
	 && s1234.l5_1 == (long long) 0xAABBCCDDEEFF0102ULL
	 && s1234.l6_2 == (long long) 0x0304050607080910ULL
	 && s1234.l7_3 == (long long) 0x1112131415161718ULL
	 && s1234.l8_4 == (long long) 0x4444444444444444ULL && a5 == 2
	 && a6 == 3;
}

int
main (int argc, char **argv)
{
  struct does_not_fit_in_a_reg s1234 = {0xCC,
					0xDD,
					0xEE,
					0xFF,
					0xdeadbeef,
					0xAABBCCDDEEFF0102ULL,
					0x0304050607080910ULL,
					0x1112131415161718ULL,
					0x4444444444444444ULL};

  // CHECK r0 0
  // CHECK_RELATIVE r1:0  0xdeadbeefffeeddcc
  // CHECK_RELATIVE r1:8  0xAABBCCDDEEFF0102
  // CHECK_RELATIVE r1:16 0x0304050607080910
  // CHECK_RELATIVE r1:24 0x1112131415161718
  // CHECK_RELATIVE r1:32 0x4444444444444444
  // CHECK r2 2
  // CHECK r3 3

  return !check_args_are_correct (0, s1234, 2, 3);
}
