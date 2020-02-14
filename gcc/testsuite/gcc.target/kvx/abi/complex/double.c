int check_args_are_correct (_Complex double, _Complex double, _Complex double);

int
check_args_are_correct_user (_Complex double c0, _Complex double c1,
			     _Complex double c2)
{
  return c0 == (1 + 1i) && c1 == -1 && c2 == -1i;
}

int
main (int argc, char **argv)
{

  // CHECK r0 0x3ff0000000000000
  // CHECK r1 0x3ff0000000000000

  // CHECK r2 0xbff0000000000000
  // CHECK r3 0x0000000000000000

  // CHECK r4 0x0000000000000000
  // CHECK r5 0xbff0000000000000
  _Complex double c0 = 1 + 1i;
  _Complex double c1 = -1;
  _Complex double c2 = -1i;

  return !check_args_are_correct (c0, c1, c2);
}
