/* Verify that SFmode loads into FP registers are not ferried through FPUL.  */
/* { dg-do compile { target { any_fpu } } }  */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler-not "fpul" } } */

_Complex float
test_00 (void)
{
  return __builtin_complex (12.3f, 15.4f);
}
