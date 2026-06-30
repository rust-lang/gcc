/* PR77283 */

void
foo (double *x, double *a, double *b, long n, double limit)
{
  long i;
  for (i=0; i < n; i++)
    if (a[i] < limit)
      x[i] = b[i];
}

