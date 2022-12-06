// { dg-do compile { target { kvx-*-* } } }
// { dg-final { scan-assembler "_Z1fPU8__bypassVi" } }

int f (int volatile __bypass *a)
{
  return *a;
}

int main () {}
