#define SHIFT_64                                                               \
  *(256ULL * 256ULL * 256ULL) * (256ULL * 256ULL * 256ULL * 256ULL * 256ULL)

int check_args_are_correct (int a0, __int128 argument1_2, __int128 argument3_4,
			    int argument5, int argument6, int argument7,
			    __int128 argument8_9, int argument10,
			    int argument11);

int
check_args_are_correct_user (int a0, __int128 argument1_2, __int128 argument3_4,
			     int argument5, int argument6, int argument7,
			     __int128 argument8_9, int argument10,
			     int argument11)
{
  return a0 == 0
	 && argument1_2
	      == ((unsigned __int128) 0x22adbeef12345678ULL) SHIFT_64
		   + 0x11CDEF9955555555ULL
	 && argument3_4
	      == ((unsigned __int128) 0x44adbeef12345678ULL) SHIFT_64
		   + 0x33CDEF9955555555ULL
	 && argument5 == 5 && argument6 == 6 && argument7 == 7
	 && argument8_9
	      == ((unsigned __int128) 0x99adbeef12345678ULL) SHIFT_64
		   + 0x88CDEF9955555555ULL
	 && argument10 == 10 && argument11 == 11;
}

unsigned __int128 a12 = ((unsigned __int128) 0x22adbeef12345678ULL) SHIFT_64
			+ 0x11CDEF9955555555ULL;
unsigned __int128 a34 = ((unsigned __int128) 0x44adbeef12345678ULL) SHIFT_64
			+ 0x33CDEF9955555555ULL;
unsigned __int128 a89 = ((unsigned __int128) 0x99adbeef12345678ULL) SHIFT_64
			+ 0x88CDEF9955555555ULL;

int
main (int argc, char **argv)
{

  // CHECK r0 0
  // CHECK r1 0x11CDEF9955555555
  // CHECK r2 0x22adbeef12345678
  // CHECK r3 0x33CDEF9955555555
  // CHECK r4 0x44adbeef12345678
  // CHECK r5 5
  // CHECK r6 6
  // CHECK r7 7
  // CHECK r8 0x88CDEF9955555555
  // CHECK r9 0x99adbeef12345678
  // CHECK r10 10
  // CHECK r11 11

  return !check_args_are_correct (0, a12, a34, 5, 6, 7, a89, 10, 11);
}
