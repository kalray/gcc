#include <omp.h>
#include <stdlib.h>

#define NUM_DEVICES 5

void
test_device (int id)
{
  int is_initial_device = 1;
#pragma omp target map(from:is_initial_device) device(id)
  {
    is_initial_device = omp_is_initial_device ();
  }
  if (is_initial_device)
    abort ();
}

int
main ()
{
  setenv ("OMP_MPPA_MULTIPLE_DEVICES", "1", 0);
  for (int i = 0; i < NUM_DEVICES; i++)
    test_device (i);
  return 0;
}
