struct fit_in_a_reg
{
  char f0;
  char f1;
  char f2;
  char f3;
  int i4;
};

int check_args_are_correct (struct fit_in_a_reg s0, int a1, int a2, int a3,
			    int a4, int a5, int a6);

int
check_args_are_correct_user (struct fit_in_a_reg s0, int a1, int a2, int a3,
			     int a4, int a5, int a6)
{
  return s0.f0 == (char) 0xCC && s0.f1 == (char) 0xDD && s0.f2 == (char) 0xEE
	 && s0.f3 == (char) 0xFF && s0.i4 == (int) 0xdeadbeef && a1 == 1
	 && a2 == 2 && a3 == 3 && a4 == 4 && a5 == 5 && a6 == 6;
}

int
main (int argc, char **argv)
{
  struct fit_in_a_reg s0 = {0xCC, 0xDD, 0xEE, 0xFF, 0xdeadbeef};

  // CHECK r0 0xdeadbeefffeeddcc
  // CHECK r1 1
  // CHECK r2 2
  // CHECK r3 3
  // CHECK r4 4
  // CHECK r5 5
  // CHECK r6 6

  return !check_args_are_correct (s0, 1, 2, 3, 4, 5, 6);
}
