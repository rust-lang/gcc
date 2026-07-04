/* Verify that forwarding a double _Complex argument to a tail-called
   function does not introduce additional reg-reg copies.  */
/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler-times "jmp\t@r" 1 } } */
/* { dg-final { scan-assembler-not "mov\tr" } } */
/* { dg-final { scan-assembler-not "fmov\tr" } } */

double _Complex foo (double _Complex arg);

double _Complex
bar (double _Complex arg)
{
  return foo (arg);
}
