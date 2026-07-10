/* PR target/99668 */
/* { dg-do compile } */
/* { dg-options "-O2 -msse2" } */

typedef double v2df __attribute__((vector_size(16)));
v2df foo (_Complex double x)
{
  return *(v2df *)&x;
}

/* { dg-final { scan-assembler-not "\\tmovsd" } } */
/* { dg-final { scan-assembler-not "movupd" } } */
