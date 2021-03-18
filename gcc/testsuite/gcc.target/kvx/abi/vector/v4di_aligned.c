typedef long long v4di_t __attribute__ ((vector_size (32)));

int check_args_are_correct (v4di_t v0123, v4di_t v4567, v4di_t v891011,
			    int stack1, v4di_t stack2);

int
check_args_are_correct_user (v4di_t v0123, v4di_t v4567, v4di_t v891011,
			     int stack1, v4di_t stack2)
{
  return v0123[0] == 0 && v0123[1] == 1 && v0123[2] == 2 && v0123[3] == 3
	 && v4567[0] == 4 && v4567[1] == 5 && v4567[2] == 6 && v4567[3] == 7
	 && v891011[0] == 8 && v891011[1] == 9 && v891011[2] == 10
	 && v891011[3] == 11 && stack1 == 123456 && stack2[0] == 0xdead
	 && stack2[1] == 0xbeef && stack2[2] == 0xcafe && stack2[3] == 0xbebe;
}

int
main (int argc, char **argv)
{
  v4di_t v0123 = {0, 1, 2, 3};
  v4di_t v4567 = {4, 5, 6, 7};
  v4di_t v891011 = {8, 9, 10, 11};
  v4di_t vstack = {0xdead, 0xbeef, 0xcafe, 0xbebe};

  // CHECK r0  0x0000000000000000
  // CHECK r1  0x0000000000000001
  // CHECK r2  0x0000000000000002
  // CHECK r3  0x0000000000000003

  // CHECK r4  0x0000000000000004
  // CHECK r5  0x0000000000000005
  // CHECK r6  0x0000000000000006
  // CHECK r7  0x0000000000000007

  // CHECK r8  0x0000000000000008
  // CHECK r9  0x0000000000000009
  // CHECK r10 0x000000000000000A
  // CHECK r11 0x000000000000000B

  // CHECK_STACK:8 0 123456
  // CHECK_STACK:8 8 0xdead
  // CHECK_STACK:8 16 0xbeef
  // CHECK_STACK:8 24 0xcafe
  // CHECK_STACK:8 32 0xbebe

  return !check_args_are_correct (v0123, v4567, v891011, 123456, vstack);
}
