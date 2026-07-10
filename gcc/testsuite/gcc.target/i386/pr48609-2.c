/* PR target/48609 */
/* { dg-do compile { target { ! ia32 } } } */
/* { dg-options "-O2" } */

_Complex float mem;

_Complex float foo(_Complex float x)
{
  return x;
}

_Complex float bar()
{
  return mem;
}

/* { dg-final { scan-assembler-not "%rsp" } } */
