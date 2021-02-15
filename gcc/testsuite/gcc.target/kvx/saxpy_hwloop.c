/* { dg-do  compile } */
/* { dg-options "-O2" } */

void
saxpy (int n, float x[restrict], float y[restrict], float z[restrict],
       float alpha)
{
  for (int i = 0; i < n; i++)
    z[i] = alpha * x[i] + y[i];
}

void
saxpyu (int n, float x[restrict], float y[restrict], float z[restrict],
	float alpha)
{
  for (unsigned i = 0; i < n; i++)
    z[i] = alpha * x[i] + y[i];
}

void
saxpyl (int n, float x[restrict], float y[restrict], float z[restrict],
	float alpha)
{
  for (long long i = 0; i < n; i++)
    z[i] = alpha * x[i] + y[i];
}

void
saxpylu (int n, float x[restrict], float y[restrict], float z[restrict],
	 float alpha)
{
  for (unsigned long long i = 0; i < n; i++)
    z[i] = alpha * x[i] + y[i];
}

/* { dg-final { scan-assembler-times "loopdo \\\$r" 4 } } */
