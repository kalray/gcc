struct does_not_fit_in_a_reg
{
  char f0_0;
  char f1_0;
  char f2_0;
  char f3_0;
  int i4_0;	  // 64bit
  long long l5_0; // 128bit
  long long l6_0; // 192bit
  long long l7_0; // 256bit
  long long l8_0;
};

int check_args_are_correct (int a0, int a1, int a2, int a3, int a4, int a5,
			    int a6, int a7, int a8, int a9,
			    struct does_not_fit_in_a_reg a1011s01, int s2,
			    int s3);

int
check_args_are_correct_user (int a0, int a1, int a2, int a3, int a4, int a5,
			     int a6, int a7, int a8, int a9,
			     struct does_not_fit_in_a_reg a1011s01, int s2,
			     int s3)
{
  return a0 == 0 && a1 == 1 && a2 == 2 && a3 == 3 && a4 == 4 && a5 == 5
	 && a6 == 6 && a7 == 7 && a8 == 8 && a9 == 9
	 && a1011s01.f0_0 == (char) 0xCC && a1011s01.f1_0 == (char) 0xDD
	 && a1011s01.f2_0 == (char) 0xEE && a1011s01.f3_0 == (char) 0xFF
	 && a1011s01.i4_0 == (int) 0xdeadbeef
	 && a1011s01.l5_0 == (long long) 0xAABBCCDDEEFF0102ULL
	 && a1011s01.l6_0 == (long long) 0x0304050607080910ULL
	 && a1011s01.l7_0 == (long long) 0x1112131415161718ULL
	 && a1011s01.l8_0 == (long long) 0x4444444444444444ULL && s2 == 5
	 && s3 == 6;
}

int
main (int argc, char **argv)
{
  struct does_not_fit_in_a_reg a1011s01 = {
    0xCC,
    0xDD,
    0xEE,
    0xFF,
    0xdeadbeef,		   // 64bit
    0xAABBCCDDEEFF0102ULL, // 128
    0x0304050607080910ULL, // 192
    0x1112131415161718ULL, // 256
    0x4444444444444444ULL  // should trigger reference passing
  };

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

  // CHECK_RELATIVE r10:0  0xdeadbeefffeeddcc
  // CHECK_RELATIVE r10:8  0xAABBCCDDEEFF0102
  // CHECK_RELATIVE r10:16 0x0304050607080910
  // CHECK_RELATIVE r10:24 0x1112131415161718
  // CHECK_RELATIVE r10:32 0x4444444444444444
  // CHECK r11 5
  // CHECK_STACK 0 6

  return !check_args_are_correct (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, a1011s01, 5, 6);
}
