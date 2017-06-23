#include <sfp-machine.h>

void
__k1_fpu_raise_exceptions (int excepts)
{
  __builtin_k1_hfxb (_K1_SFR_CS, (excepts & _K1_FPU_ALL_EXCEPTS) << 16);
}

enum __k1_fpu_rounding_mode
__k1_fpu_get_rounding_mode (void)
{
  int offset = __builtin_k1_ctz (_K1_MASK_CS_RM);
  return (enum __k1_fpu_rounding_mode) (
    (__builtin_k1_get (_K1_SFR_CS) & _K1_MASK_CS_RM) >> offset);
}
