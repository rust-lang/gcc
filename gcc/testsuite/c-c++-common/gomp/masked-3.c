/* { dg-do compile } */
/* { dg-additional-options "-fopenmp-ompt -fdump-tree-omplower" } */

void bar (void);

void
foo (void)
{
  #pragma omp masked
  bar ();
}

/* { dg-final { scan-tree-dump-times "GOMP_has_masked_thread_num" 1 "omplower" } } */
/* { dg-final { scan-tree-dump-times "GOMP_masked_end" 1 "omplower" } } */
/* { dg-final { scan-tree-dump-not "omp_get_thread_num" "omplower" } } */
