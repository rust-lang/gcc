/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

int
aa_div_bb_eq_0 (unsigned short a, unsigned short b)
{
  int aa = a;
  int bb = b;
  int c = aa / bb;
  return (c == 0) == (a < b);
}

int
aa_div_bb_ne_0 (unsigned short a, unsigned short b)
{
  int aa = a;
  int bb = b;
  int c = aa / bb;
  return (c != 0) == (a >= b);
}

/* { dg-final { scan-tree-dump-times "return 1;" 2 "optimized" } } */
