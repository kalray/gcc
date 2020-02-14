#define SHIFT_64                                                               \
  *(256ULL * 256ULL * 256ULL) * (256ULL * 256ULL * 256ULL * 256ULL * 256ULL)

int check_args_are_correct (__int128 argument0_1, __int128 argument2_3,
			    int argument4, int argument5, int argument6,
			    int argument7, int argument8, int argument9,
			    int argument10, int argument11, int argstack0,
			    int argstack1, int argstack2, int argstack3);

int
check_args_are_correct_user (__int128 argument0_1, __int128 argument2_3,
			     int argument4, int argument5, int argument6,
			     int argument7, int argument8, int argument9,
			     int argument10, int argument11, int argstack0,
			     int argstack1, int argstack2, int argstack3)
{
  return argument0_1
	   == ((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
		+ 0xABCDEF9955555555ULL
	 && argument2_3
	      == ((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
		   + 0xABCDEF9955555555ULL
	 && argument4 == 4 && argument5 == 5 && argument6 == 6 && argument7 == 7
	 && argument8 == 8 && argument9 == 9 && argument10 == 10
	 && argument11 == 11 && argstack0 == 12 && argstack1 == 13
	 && argstack2 == 14 && argstack3 == 15;
}

int
main (int argc, char **argv)
{
  unsigned __int128 a01 = ((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
			  + 0xABCDEF9955555555ULL;
  unsigned __int128 a23 = ((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
			  + 0xABCDEF9955555555ULL;

  // CHECK r0 0xabcdef9955555555
  // CHECK r1 0xdeadbeef12345678
  // CHECK r2 0xabcdef9955555555
  // CHECK r3 0xdeadbeef12345678
  // CHECK r4 4
  // CHECK r5 5
  // CHECK r6 6
  // CHECK r6 6
  // CHECK r7 7
  // CHECK r8 8
  // CHECK r9 9
  // CHECK r10 10
  // CHECK r11 11
  // CHECK_STACK 0 12
  // CHECK_STACK 8 13
  // CHECK_STACK 16 14
  // CHECK_STACK 24 15

  return !check_args_are_correct (a01, a23, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
				  14, 15);
}
