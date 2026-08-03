/* --target= reaches code generation: this compiles as m68k.  */
/* { dg-do compile } */

int f (void)
{
  return 42;
}

/* { dg-final { scan-assembler "moveq #42,%d0" } } */
