/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized --param logical-op-non-short-circuit=0" } */
/* { dg-additional-options "-fno-unroll-loops" { target riscv*-*-* } } */
/* PR tree-optimization/117973 */
#include "pr111456-1.c"

/* { dg-final { scan-tree-dump-not "foo " "optimized" { xfail *-*-* } } } */
/* { dg-final { scan-tree-dump "return 0;" "optimized" } } */
