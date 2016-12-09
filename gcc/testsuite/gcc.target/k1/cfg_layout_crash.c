/* { dg-do compile } */
/* { dg-options "-O2 -g" } */
#include <assert.h>
extern int mppa_aio_read(void *);
extern int mppa_waitpid(int, void *, int);
extern void mppa_exit(int);

int main(int argc, char *argv[])
{
  int  ret, i,status;
  int portal_aiocb;
  status = mppa_aio_read(&portal_aiocb);
  assert(status == 0);
  
  status = 0;

  for(i=0; i < 16; i++)
  {
    ret = mppa_waitpid(i, &status, 0);
    if (ret < 0)
      mppa_exit(ret);	
  }
  return 0;
}
