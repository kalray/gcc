typedef int v4si __attribute__ ((vector_size (16)));

int check_args_are_correct (v4si a01, v4si a23, int a4, int a5, v4si a67,
			    v4si a89, v4si a1011, int s0, int s1);

int
check_args_are_correct_user (v4si a01, v4si a23, int a4, int a5, v4si a67,
			     v4si a89, v4si a1011, int s0, int s1)
{
  return a01[0] == 0 && a01[1] == 0 && a01[2] == 1 && a01[3] == 1 && a23[0] == 2
	 && a23[1] == 2 && a23[2] == 3 && a23[3] == 3 && a4 == 4 && a5 == 5
	 && a67[0] == 6 && a67[1] == 6 && a67[2] == 7 && a67[3] == 7
	 && a89[0] == 8 && a89[1] == 8 && a89[2] == 9 && a89[3] == 9
	 && a1011[0] == 10 && a1011[1] == 10 && a1011[2] == 11 && a1011[3] == 11
	 && s0 == 12 && s1 == 13;
}

int
main (int argc, char **argv)
{
  v4si v01 = {0, 0, 1, 1};
  v4si v23 = {2, 2, 3, 3};
  v4si v67 = {6, 6, 7, 7};
  v4si v89 = {8, 8, 9, 9};
  v4si v1011 = {10, 10, 11, 11};

  // CHECK r0  0x0000000000000000
  // CHECK r1  0x0000000100000001
  // CHECK r2  0x0000000200000002
  // CHECK r3  0x0000000300000003
  // CHECK r4  4
  // CHECK r5  5
  // CHECK r6  0x0000000600000006
  // CHECK r7  0x0000000700000007
  // CHECK r8  0x0000000800000008
  // CHECK r9  0x0000000900000009
  // CHECK r10 0x0000000a0000000a
  // CHECK r11 0x0000000b0000000b

  // CHECK_STACK 0 12
  // CHECK_STACK 8 13

  return !check_args_are_correct (v01, v23, 4, 5, v67, v89, v1011, 12, 13);
}
