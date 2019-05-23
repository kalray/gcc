#define SHIFT_64                                                               \
  *(256ULL * 256ULL * 256ULL) * (256ULL * 256ULL * 256ULL * 256ULL * 256ULL)

struct correctly_aligned_int128
{
  int reg0_0;
  int reg0_1;
  int reg1_0;
  int reg1_1;
  long long reg2;
  long long reg3;
  unsigned __int128 reg45;
  unsigned __int128 reg67;
  unsigned __int128 reg89;
};

int check_args_are_correct (struct correctly_aligned_int128 a0123456789,
			    int argument10, int argument11);

int
check_args_are_correct_user (struct correctly_aligned_int128 a0123456789,
			     int argument10, int argument11)
{
  return a0123456789.reg0_0 == 0 && a0123456789.reg0_0 == 0
	 && a0123456789.reg1_0 == 1 && a0123456789.reg1_1 == 1
	 && a0123456789.reg2 == 2 && a0123456789.reg3 == 3
	 && a0123456789.reg45
	      == ((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
		   + 0xCDEFAA9955555555ULL
	 && a0123456789.reg67
	      == ((unsigned __int128) 0xdeaddead12345678ULL) SHIFT_64
		   + 0xBCDEFBB888888888ULL
	 && a0123456789.reg89
	      == ((unsigned __int128) 0xbeefbeef12345678ULL) SHIFT_64
		   + 0xBCDEFCC777777777ULL;
}

int
main (int argc, char **argv)
{
  struct correctly_aligned_int128 mystruct
    = {0,
       0,
       1,
       1,
       2,
       3,
       ((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
	 + 0xCDEFAA9955555555ULL,
       ((unsigned __int128) 0xdeaddead12345678ULL) SHIFT_64
	 + 0xBCDEFBB888888888ULL,
       ((unsigned __int128) 0xbeefbeef12345678ULL) SHIFT_64
	 + 0xBCDEFCC777777777ULL};

  // CHECK_RELATIVE r0:0  0x0000000000000000
  // CHECK_RELATIVE r0:8  0x0000000100000001
  // CHECK_RELATIVE r0:16 2
  // CHECK_RELATIVE r0:24 3
  // CHECK_RELATIVE r0:32 0xcdefaa9955555555
  // CHECK_RELATIVE r0:40 0xdeadbeef12345678
  // CHECK_RELATIVE r0:48 0xbcdefbb888888888
  // CHECK_RELATIVE r0:56 0xdeaddead12345678
  // CHECK_RELATIVE r0:64 0xbcdefcc777777777
  // CHECK_RELATIVE r0:72 0xbeefbeef12345678
  // CHECK r1 1
  // CHECK r2 2

  return !check_args_are_correct (mystruct, 1, 2);
}
