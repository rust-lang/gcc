/* PR tree-optimization/102202 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-cdce-details" } */

void
g1 (unsigned int n, int c, char *d)
{
  unsigned int len = n & 1;
  __builtin_memset (d, c, len);
}

char *
g2 (unsigned int n, int c, char *d)
{
  unsigned int len = n & 1;
  return __builtin_memset (d, c, len);
}

/* { dg-final { scan-tree-dump-times "function call is shrink-wrapped into error conditions" 2 "cdce" } } */
