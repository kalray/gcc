typedef int v4si __attribute__ ((vector_size (16)));

int check_args_are_correct (int a0, v4si a12, v4si a34, int a5, v4si a67,
			    int a8, int a9, int a10, int a11, v4si s01, int s02,
			    int s03);

int
check_args_are_correct_user (int a0, v4si a12, v4si a34, int a5, v4si a67,
			     int a8, int a9, int a10, int a11, v4si s01,
			     int s02, int s03)
{
  return a0 == 0 && a12[0] == 1 && a12[1] == 1 && a12[2] == 2 && a12[3] == 2
	 && a34[0] == 3 && a34[1] == 3 && a34[2] == 4 && a34[3] == 4 && a5 == 5
	 && a67[0] == 6 && a67[1] == 6 && a67[2] == 7 && a67[3] == 7 && a8 == 8
	 && a9 == 9 && a10 == 10 && a11 == 11 && s01[0] == 12 && s01[1] == 12
	 && s01[2] == 13 && s01[3] == 13 && s02 == 14 && s03 == 15;
}

int
main (int argc, char **argv)
{
  v4si v12 = {1, 1, 2, 2};
  v4si v34 = {3, 3, 4, 4};
  v4si v67 = {6, 6, 7, 7};
  v4si s01 = {12, 12, 13, 13};

  // CHECK r0  0

  // CHECK r1  0x0000000100000001
  // CHECK r2  0x0000000200000002
  // CHECK r3  0x0000000300000003
  // CHECK r4  0x0000000400000004

  // CHECK r5  5

  // CHECK r6  0x0000000600000006
  // CHECK r7  0x0000000700000007

  // CHECK r8  8
  // CHECK r9  9
  // CHECK r10 10
  // CHECK r11 11

  // CHECK_STACK 0 0x0000000c0000000c
  // CHECK_STACK 8 0x0000000d0000000d
  // CHECK_STACK 16 14
  // CHECK_STACK 24 15
  return !check_args_are_correct (0, v12, v34, 5, v67, 8, 9, 10, 11, s01, 14,
				  15);
}
