/* { dg-do run } */
/* { dg-require-effective-target riscv_v_ok } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O2 -ftree-vectorize" } */

#define NAN (__builtin_nan(""))

__attribute__ ((noipa)) float
maxx (float *arr, unsigned int sz)
{
  float val = NAN;
  unsigned int i;

  for (i = 0; i < sz; i++)
    val = __builtin_fmaxf (val, arr[i]);

  return val;
}

int
main ()
{
  float arr[] = { NAN, NAN, NAN, NAN };
  float res = maxx (arr, 4);
  if (!__builtin_isnan (res))
    __builtin_abort ();
}
