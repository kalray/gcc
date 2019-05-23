#include <stdarg.h>

#define SHIFT_64                                                               \
  *(256ULL * 256ULL * 256ULL) * (256ULL * 256ULL * 256ULL * 256ULL * 256ULL)

int
check_args_are_correct (int argument0, int lastnamed, ...)
{

  // CHECK_VAR argument0 0

  if (argument0 != 0)
    return 0;
  // CHECK_VAR lastnamed 1

  if (lastnamed != 1)
    return 0;
  // CHECK_VARARG unsigned __int128 (((unsigned __int128)0xdeaddead12345678ULL)
  // SHIFT_64 + 0xAACDFF9955555555ULL)

  va_list ap;
  va_start (ap, lastnamed);

  unsigned __int128 va_arg_var_0 = va_arg (ap, unsigned __int128);
  if (va_arg_var_0
      != (((unsigned __int128) 0xdeaddead12345678ULL) SHIFT_64
	  + 0xAACDFF9955555555ULL))
    return 0;
  // CHECK_VARARG unsigned __int128 (((unsigned __int128)0xdeadbeef12345678ULL)
  // SHIFT_64 + 0xABCDEF9955555555ULL)

  unsigned __int128 va_arg_var_1 = va_arg (ap, unsigned __int128);
  if (va_arg_var_1
      != (((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
	  + 0xABCDEF9955555555ULL))
    return 0;
  // CHECK_VARARG unsigned __int128 (((unsigned __int128)0xbeefbeef12345678ULL)
  // SHIFT_64 + 0xFEEFEE9955555555ULL)

  unsigned __int128 va_arg_var_2 = va_arg (ap, unsigned __int128);
  if (va_arg_var_2
      != (((unsigned __int128) 0xbeefbeef12345678ULL) SHIFT_64
	  + 0xFEEFEE9955555555ULL))
    return 0;
  // CHECK_VARARG int (8)

  int va_arg_var_3 = va_arg (ap, int);
  if (va_arg_var_3 != (8))
    return 0;
  // CHECK_VARARG int (9)

  int va_arg_var_4 = va_arg (ap, int);
  if (va_arg_var_4 != (9))
    return 0;
  // CHECK_VARARG unsigned __int128 (((unsigned __int128)0xdeadeade12345678ULL)
  // SHIFT_64 + 0xFECDEA9955555555ULL)

  unsigned __int128 va_arg_var_5 = va_arg (ap, unsigned __int128);
  if (va_arg_var_5
      != (((unsigned __int128) 0xdeadeade12345678ULL) SHIFT_64
	  + 0xFECDEA9955555555ULL))
    return 0;
  // CHECK_VARARG int (12)

  int va_arg_var_6 = va_arg (ap, int);
  if (va_arg_var_6 != (12))
    return 0;
  // CHECK_VARARG int (13)

  int va_arg_var_7 = va_arg (ap, int);
  if (va_arg_var_7 != (13))
    return 0;
  // CHECK_VARARG int (14)

  int va_arg_var_8 = va_arg (ap, int);
  if (va_arg_var_8 != (14))
    return 0;
  // CHECK_VARARG int (15)

  int va_arg_var_9 = va_arg (ap, int);
  if (va_arg_var_9 != (15))
    return 0;
  return 1;
}

int
main (int argc, char **argv)
{

  unsigned __int128 a23 = ((unsigned __int128) 0xdeaddead12345678ULL) SHIFT_64
			  + 0xAACDFF9955555555ULL;

  unsigned __int128 a45 = ((unsigned __int128) 0xdeadbeef12345678ULL) SHIFT_64
			  + 0xABCDEF9955555555ULL;

  unsigned __int128 a67 = ((unsigned __int128) 0xbeefbeef12345678ULL) SHIFT_64
			  + 0xFEEFEE9955555555ULL;

  unsigned __int128 a1011 = ((unsigned __int128) 0xdeadeade12345678ULL) SHIFT_64
			    + 0xFECDEA9955555555ULL;

  return !check_args_are_correct (0, 1, a23, a45, a67, 8, 9, a1011,

				  12, 13, 14, 15);
}
