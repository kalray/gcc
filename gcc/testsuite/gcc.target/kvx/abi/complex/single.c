int check_args_are_correct (_Complex float, _Complex float, _Complex float);

int
check_args_are_correct_user (_Complex float c0, _Complex float c1,
			     _Complex float c2)
{
  return c0 == (1 + 1i) && c1 == -1 && c2 == -1i;
}

int
main (int argc, char **argv)
{

  // CHECK r0 0x3f8000003f800000
  // CHECK r1 0x00000000bf800000
  // CHECK r2 0xbf80000000000000
  _Complex float c0 = 1 + 1i;
  _Complex float c1 = -1;
  _Complex float c2 = -1i;

  return !check_args_are_correct (c0, c1, c2);
}
