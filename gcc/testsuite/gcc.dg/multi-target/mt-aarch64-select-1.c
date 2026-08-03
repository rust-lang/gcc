/* --target= reaches code generation: this compiles as aarch64.  */
/* { dg-do compile } */

int f (void)
{
  return 42;
}

/* { dg-final { scan-assembler "mov\tw0, 42" } } */
