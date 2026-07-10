/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized -fno-trapping-math -frounding-math" } */

int foo1 (int a) {
  return (float) a != 16777222.0f;
}

int foo2 (unsigned int b) {
  return (float) b != 16777222.0f;
}

int foo3 (unsigned long long c) {
  return (float) c != 16777222.0f;
}

int foo4 (long long d) {
  return (float) d != 16777222.0f;
}

/* { dg-final { scan-tree-dump-times "\\(float\\)" 4 "optimized" } } */
