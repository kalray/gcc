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

int check_args_are_correct (int a0, int a1, int a2, int a3, int a4, int a5,
			    int a6, int a7, int a8, int a9, int a10, int a11,
			    struct does_not_fit_in_a_reg s0123, int s4, int s5);

int
check_args_are_correct_user (int a0, int a1, int a2, int a3, int a4, int a5,
			     int a6, int a7, int a8, int a9, int a10, int a11,
			     struct does_not_fit_in_a_reg s0123, int s4, int s5)
{
  return s0123.f0_0 == (char) 0xCC && s0123.f1_0 == (char) 0xDD
	 && s0123.f2_0 == (char) 0xEE && s0123.f3_0 == (char) 0xFF
	 && s0123.i4_0 == (int) 0xdeadbeef
	 && s0123.l5_1 == (long long) 0xAABBCCDDEEFF0102ULL
	 && s0123.l6_2 == (long long) 0x0304050607080910ULL
	 && s0123.l7_3 == (long long) 0x1112131415161718ULL
	 && s0123.l8_4 == (long long) 0x4444444444444444ULL && a0 == 0
	 && a1 == 1 && a2 == 2 && a3 == 3 && a4 == 4 && a5 == 5 && a6 == 6
	 && a7 == 7 && a8 == 8 && a9 == 9 && a10 == 10 && a11 == 11 && s4 == 5
	 && s5 == 6;
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
  // CHECK r1 1
  // CHECK r2 2
  // CHECK r3 3
  // CHECK r4 4
  // CHECK r5 5
  // CHECK r6 6
  // CHECK r7 7
  // CHECK r8 8
  // CHECK r9 9
  // CHECK r10 10
  // CHECK r11 11
  // CHECK_RELATIVE_STACK 0:0 0xdeadbeefffeeddcc
  // CHECK_RELATIVE_STACK 0:8 0xAABBCCDDEEFF0102
  // CHECK_RELATIVE_STACK 0:16 0x0304050607080910
  // CHECK_RELATIVE_STACK 0:24 0x1112131415161718
  // CHECK_RELATIVE_STACK 0:32 0x4444444444444444
  // CHECK_STACK 8:4 5
  // CHECK_STACK 16:4 6

  return !check_args_are_correct (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, s1234,
				  5, 6);
}
