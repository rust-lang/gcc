/* { dg-do compile } */
/* { dg-options "-O2 -ffast-math -fno-tree-vectorize --param=avoid-fma-max-bits=512 --param=widening-mul-defer-fma=0" } */

/* Companion to widening-mul-defer-fma-2.c: the same reduction with FMA
   avoidance still requested for the reassoc reorder (avoid-fma-max-bits=512)
   but --param=widening-mul-defer-fma=0 now suppresses the widening_mul
   deferring, so the loop-carried multiply-add is contracted to a single
   fmadd.  This proves the new param decouples the deferring from
   avoid-fma-max-bits.  */

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
