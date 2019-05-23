#define SHIFT_64                                                               \
  *(256ULL * 256ULL * 256ULL) * (256ULL * 256ULL * 256ULL * 256ULL * 256ULL)

int check_args_are_correct (int a0, int a1, __int128 argument2_3,
			    __int128 argument4_5, int argument6, int argument7,
			    int argument8, int argument9, int argument10,
			    int argument11);

int
check_args_are_correct_user (int a0, int a1, __int128 argument2_3,
			     __int128 argument4_5, int argument6, int argument7,
			     int argument8, int argument9, int argument10,
			     int argument11)
{
  return a0 == 0 && a1 == 1
	 && argument2_3
	      == ((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
		   + 0xABCDEF9955555555ULL
	 && argument4_5
	      == ((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
		   + 0xABCDEF9955555555ULL
	 && argument6 == 6 && argument7 == 7 && argument8 == 8 && argument9 == 9
	 && argument10 == 10 && argument11 == 11;
}

int
main (int argc, char **argv)
{
  unsigned __int128 a23 = ((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
			  + 0xABCDEF9955555555ULL;
  unsigned __int128 a45 = ((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
			  + 0xABCDEF9955555555ULL;

  // CHECK r0 0
  // CHECK r1 1
  // CHECK r2 0xabcdef9955555555
  // CHECK r3 0xdeadbeef12345678
  // CHECK r4 0xabcdef9955555555
  // CHECK r5 0xdeadbeef12345678
  // CHECK r6 6
  // CHECK r7 7
  // CHECK r8 8
  // CHECK r9 9
  // CHECK r10 10
  // CHECK r11 11

  return !check_args_are_correct (0, 1, a23, a45, 6, 7, 8, 9, 10, 11);
}
