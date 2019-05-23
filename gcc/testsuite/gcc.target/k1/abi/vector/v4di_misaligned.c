typedef long long v4di_t __attribute__ ((vector_size (32)));

int check_args_are_correct (int a0, v4di_t a1234, int a5, v4di_t a6789,
			    v4di_t a1011s01, v4di_t s1234);

int
check_args_are_correct_user (int a0, v4di_t a1234, int a5, v4di_t a6789,
			     v4di_t a1011s01, v4di_t s1234)
{
  return a0 == 0 && a1234[0] == 1 && a1234[1] == 2 && a1234[2] == 3
	 && a1234[3] == 4 && a5 == 5 && a6789[0] == 6 && a6789[1] == 7
	 && a6789[2] == 8 && a6789[3] == 9 && a1011s01[0] == 10
	 && a1011s01[1] == 11 && a1011s01[2] == 12 && a1011s01[3] == 13
	 && s1234[0] == 14 && s1234[1] == 15 && s1234[2] == 16
	 && s1234[3] == 17;
}

int
main (int argc, char **argv)
{
  v4di_t v1234 = {1, 2, 3, 4};
  v4di_t v6789 = {6, 7, 8, 9};
  v4di_t v1011s01 = {10, 11, 12, 13};
  v4di_t s1234 = {14, 15, 16, 17};

  // CHECK r0  0

  // CHECK r1  0x0000000000000001
  // CHECK r2  0x0000000000000002
  // CHECK r3  0x0000000000000003
  // CHECK r4  0x0000000000000004

  // CHECK r5  5

  // CHECK r6  0x0000000000000006
  // CHECK r7  0x0000000000000007
  // CHECK r8  0x0000000000000008
  // CHECK r9  0x0000000000000009

  // CHECK r10     0x000000000000000A
  // CHECK r11     0x000000000000000B
  // CHECK_STACK 0 0x000000000000000C
  // CHECK_STACK 8 0x000000000000000D

  // CHECK_STACK 16 0x000000000000000E
  // CHECK_STACK 24 0x000000000000000F
  // CHECK_STACK 32 0x0000000000000010
  // CHECK_STACK 40 0x0000000000000011

  return !check_args_are_correct (0, v1234, 5, v6789, v1011s01, s1234);
}
