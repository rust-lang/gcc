/* PR tree-optimization/102202 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-cdce-details" } */

/* The interval [0, 2] is not the exact two-value set {0, 2}.  */

void
g1 (__SIZE_TYPE__ len, int c, char *d)
{
  if (len > 2)
    __builtin_unreachable ();

  __builtin_memset (d, c, len);
}

/* { dg-final { scan-tree-dump-not "function call is shrink-wrapped into error conditions" "cdce" } } */
