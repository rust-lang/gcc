/* { dg-do compile } */
/* { dg-options "-O2 -ffast-math -fno-tree-vectorize -mcpu=ampere1" } */

/* The ampere1 family sets AARCH64_EXTRA_TUNE_AVOID_CROSS_LOOP_FMA, which
   sets widening-mul-defer-fma to 0 so that a 2-operand loop-carried
   reduction is contracted to fmadd rather than deferred to fmul + fadd.
   (avoid-fma-max-bits stays 512 to keep the reassoc reorder enabled.)  */

double
dot (const double *a, const double *b, int n)
{
  double s = 0.0;
  for (int i = 0; i < n; i++)
    s += a[i] * b[i];
  return s;
}

/* { dg-final { scan-assembler {\tfmadd\t} } } */
/* { dg-final { scan-assembler-not {\tfmul\t} } } */
