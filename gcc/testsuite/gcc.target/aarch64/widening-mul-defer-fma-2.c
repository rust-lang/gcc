/* { dg-do compile } */
/* { dg-options "-O2 -ffast-math -fno-tree-vectorize --param=avoid-fma-max-bits=512" } */

/* With FMA deferring active (avoid-fma-max-bits > 0) and the default
   widening-mul-defer-fma=1, the widening_mul pass refuses to form an FMA
   whose result feeds the loop-header phi: the reduction stays as a
   separate fmul + fadd.  This is the behaviour the new param decouples
   from the reassoc reorder (also gated by avoid-fma-max-bits).  */

double
dot (const double *a, const double *b, int n)
{
  double s = 0.0;
  for (int i = 0; i < n; i++)
    s += a[i] * b[i];
  return s;
}

/* { dg-final { scan-assembler {\tfmul\t} } } */
/* { dg-final { scan-assembler {\tfadd\t} } } */
/* { dg-final { scan-assembler-not {\tfmadd\t} } } */
