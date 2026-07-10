/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized -fno-trapping-math" } */

int foo1 (int a) {
  return (float) a != 16777222.0f;
  /* { dg-final { scan-tree-dump "a_\[0-9]+\\(D\\) != 16777222;" "optimized" } } */
}

int foo2 (unsigned int b) {
  return (float) b != 16777222.0f;
  /* { dg-final { scan-tree-dump "b_\[0-9]+\\(D\\) != 16777222;" "optimized" } } */
}

int foo3 (unsigned long long c) {
  return (float) c != 16777222.0f;
  /* { dg-final { scan-tree-dump "c_\[0-9]+\\(D\\) != 16777222;" "optimized" } } */
}

int foo4 (long long d) {
  return (float) d != 16777222.0f;
  /* { dg-final { scan-tree-dump "d_\[0-9]+\\(D\\) != 16777222;" "optimized" } } */
}

/* { dg-final { scan-tree-dump-not "\\(float\\)" "optimized" } } */
