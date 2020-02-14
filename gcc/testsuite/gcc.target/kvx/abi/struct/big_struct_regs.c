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

int check_args_are_correct (struct does_not_fit_in_a_reg s0123, int a4, int a5,
			    int a6);

int
check_args_are_correct_user (struct does_not_fit_in_a_reg s0123, int a4, int a5,
			     int a6)
{
  return s0123.f0_0 == (char) 0xCC && s0123.f1_0 == (char) 0xDD
	 && s0123.f2_0 == (char) 0xEE && s0123.f3_0 == (char) 0xFF
	 && s0123.i4_0 == (int) 0xdeadbeef
	 && s0123.l5_1 == (long long) 0xAABBCCDDEEFF0102ULL
	 && s0123.l6_2 == (long long) 0x0304050607080910ULL
	 && s0123.l7_3 == (long long) 0x1112131415161718ULL
	 && s0123.l8_4 == (long long) 0x4444444444444444ULL && a4 == 4
	 && a5 == 5 && a6 == 6;
}

int
main (int argc, char **argv)
{
  struct does_not_fit_in_a_reg s0123 = {0xCC,
					0xDD,
					0xEE,
					0xFF,
					0xdeadbeef,
					0xAABBCCDDEEFF0102ULL,
					0x0304050607080910ULL,
					0x1112131415161718ULL,
					0x4444444444444444ULL};

  // CHECK_RELATIVE r0:0  0xdeadbeefffeeddcc
  // CHECK_RELATIVE r0:8  0xAABBCCDDEEFF0102
  // CHECK_RELATIVE r0:16 0x0304050607080910
  // CHECK_RELATIVE r0:24 0x1112131415161718
  // CHECK_RELATIVE r0:32 0x4444444444444444
  // CHECK r1 4
  // CHECK r2 5
  // CHECK r3 6

  return !check_args_are_correct (s0123, 4, 5, 6);
}
