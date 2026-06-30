void bar (int a)
{}

/* Distribute without a chunk size goes through
   expand_omp_for_static_nochunk.  */

void
f3 (int n)
{
  int i;
#pragma omp target
#pragma omp teams
#pragma omp distribute
  for (i = 0; i < n; ++i)
    bar (i);
}

/* Distribute with a chunk size goes through
   expand_omp_for_static_chunk.  */

void
f4 (int n)
{
  int i;
#pragma omp target
#pragma omp teams
#pragma omp distribute dist_schedule(static, 4)
  for (i = 0; i < n; ++i)
    bar (i);
}

int main (void)
{
  f3(0);
  f4(1);
  return 0;
}
