/* { dg-do compile } */
/* { dg-options "-Ofast -mcpu=ampere1" } */

/* With issue_info populated for the ampere1 family,
   determine_suggested_unroll_factor picks 4 for a unit-stride FP dot
   product, so the inner reduction emits four independent `fmla v.4s'
   accumulator chains.  */

float
dot (const float *a, const float *b, int n)
{
  float s = 0.0f;
  for (int i = 0; i < n; i++)
    s += a[i] * b[i];
  return s;
}

/* { dg-final { scan-assembler-times {\tfmla\tv[0-9]+\.4s} 4 } } */
