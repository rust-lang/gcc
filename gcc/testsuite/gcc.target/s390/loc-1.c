/* Test load on condition patterns.  */

/* { dg-do compile } */
/* { dg-options "-O3 -march=z13 -mzarch" } */

unsigned long loc_r (unsigned long rc, unsigned long cond, unsigned long val)
{
  if (cond)
    rc = val;
  return rc;
}
/* { dg-final { scan-assembler "\tlocgrne\t%r2,%r4" } } */

long loc_hi (long rc, long cond)
{
  if (cond)
    rc = (long)-1;
  return rc;
}
/* { dg-final { scan-assembler "\tlocghine\t%r2,-1" } } */
