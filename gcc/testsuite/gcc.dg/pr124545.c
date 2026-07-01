/* PR tree-optimization/124545 */
/* Verify that VN recognizes (T)A + C == (T)(A + C') regardless of
   operand order in the equality comparison.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-fre1" } */

int func1(int *a, int j) {
  int k = j - 1;
  return a[j - 1] == a[k];
}

int func2(int *a, int j) {
  int k = j - 1;
  return a[k] == a[j - 1];
}

int func3(int *a, int j) {
  int k = j - 3;
  return a[k] == a[j - 3];
}

int func4(int *a, int j) {
  int k = j + 2;
  return a[k] == a[j + 2];
}

/* All four functions should fold to return 1 after FRE.  */
/* The pattern is not applied on ilp32 targets (PR116845).  */
/* { dg-final { scan-tree-dump-times "return 1;" 4 "fre1" { xfail { ilp32 } } } } */
