#define SHIFT_64                                                               \
  *(256ULL * 256ULL * 256ULL) * (256ULL * 256ULL * 256ULL * 256ULL * 256ULL)

struct correctly_aligned_int128
{
  int reg0_0;
  int reg0_1;
  long long reg1;
  long long reg2;
  unsigned __int128 reg34;
  unsigned __int128 reg56;
  unsigned __int128 reg78;
} __attribute__ ((packed));

int check_args_are_correct (struct correctly_aligned_int128 a012345678,
			    int argument1, int argument2);

int
check_args_are_correct_user (struct correctly_aligned_int128 a012345678,
			     int argument1, int argument2)
{

  return a012345678.reg0_0 == 0 && a012345678.reg0_1 == 1
	 && a012345678.reg1 == 1 && a012345678.reg2 == 2
	 && a012345678.reg34
	      == ((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
		   + 0xCDEFAA9955555555ULL
	 && a012345678.reg56
	      == ((unsigned __int128) 0xdeaddead12345678ULL) SHIFT_64
		   + 0xBCDEFBB888888888ULL
	 && a012345678.reg78
	      == ((unsigned __int128) 0xbeefbeef12345678ULL) SHIFT_64
		   + 0xBCDEFCC777777777ULL
	 && argument1 == 1 && argument2 == 2;
}

int
main (int argc, char **argv)
{
  struct correctly_aligned_int128 mystruct
    = {0,
       1,
       1,
       2,
       ((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
	 + 0xCDEFAA9955555555ULL,
       ((unsigned __int128) 0xdeaddead12345678ULL) SHIFT_64
	 + 0xBCDEFBB888888888ULL,
       ((unsigned __int128) 0xbeefbeef12345678ULL) SHIFT_64
	 + 0xBCDEFCC777777777ULL};

  // CHECK_RELATIVE r0:0  0x0000000100000000
  // CHECK_RELATIVE r0:8  0x0000000000000001
  // CHECK_RELATIVE r0:16 0x0000000000000002
  // CHECK_RELATIVE r0:24 0xcdefaa9955555555
  // CHECK_RELATIVE r0:32 0xdeadbeef12345678
  // CHECK_RELATIVE r0:40 0xbcdefbb888888888
  // CHECK_RELATIVE r0:48 0xdeaddead12345678
  // CHECK_RELATIVE r0:56 0xbcdefcc777777777
  // CHECK_RELATIVE r0:64 0xbeefbeef12345678
  // CHECK r1 1
  // CHECK r2 2

  return !check_args_are_correct (mystruct /* in $r0 */, 1 /* in $r1 */,
				  2 /* in $r2 */);
}
