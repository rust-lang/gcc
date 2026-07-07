/* { dg-do compile } */
/* { dg-options "-fdump-rtl-loop2_unroll-details -fno-tree-vectorize" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" "-Os" "-Oz" "-funroll-loops" } { "" } } */

/* With the default -O2 small loop unrolling (-munroll-only-small-loops),
   a loop whose body exceeds the small-loop instruction threshold is NOT
   unrolled.  */

void
large_loop (int *a, int *b, int *c, int *d, int n)
{
  int i;
  for (i = 0; i < n; i++)
    {
      a[i] = b[i] + c[i];
      d[i] = a[i] * b[i] - c[i];
      b[i] = c[i] + d[i] + a[i];
      c[i] = a[i] - d[i] + b[i];
    }
}

/* { dg-final { scan-rtl-dump-not "Unrolled loop" "loop2_unroll" } } */
