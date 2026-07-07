/* PR tree-optimization/102202 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-cdce-details" } */

/* An exact two-value range {0, 2}.  Use a PHI so that Ranger retains
   the two singleton values at the call.  */

void
g1 (unsigned int x, int c, char *d)
{
  __SIZE_TYPE__ len;

  if (x)
    len = 2;
  else
    len = 0;

  __builtin_memset (d, c, len);
}

/* { dg-final { scan-tree-dump-times "function call is shrink-wrapped into error conditions" 1 "cdce" } } */
