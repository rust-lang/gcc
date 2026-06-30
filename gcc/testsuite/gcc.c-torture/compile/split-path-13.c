/* PR tree-optimization/112402 */
/* This is similar to split-path-2.c but instead of the add
   being inside both sides, we have a constant. */

int
foo(signed char *p, int n)
{
  int s = 0;
  int i;

  for (i = 0; i < n; i++) {
    int t;
    if (p[i] >= 0)
      t = 1;
    else
      t = -1;
    s += t;
  }

  return s;
}


