/* { dg-do  compile } */
/* { dg-options "-O2"  } */

void exec_control_engine( void ) {
  while(1)
    {
      if (__builtin_k1_ldu(0))
          {
            break;
          }
      __builtin_k1_waitclr1(0);
    }
}
