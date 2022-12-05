// { dg-do compile { target { i?86-*-* x86_64-*-* } } }
// { dg-final { scan-assembler "_Z1fPU8__seg_fsVi" } }

int f (int volatile __seg_fs *a)
{
  return *a;
}

int main () {}
