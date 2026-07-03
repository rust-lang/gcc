/* PR tree-optimization/102202 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

/* A [0, 2] length is not a boolean range, so the call must be left alone.  */

void
g1 (int a, int c, char *d)
{
  if (a < 0 || a > 2)
    __builtin_unreachable ();

  __builtin_memset (d, c, a);
}

/* { dg-final { scan-tree-dump "__builtin_memset" "optimized" } } */
