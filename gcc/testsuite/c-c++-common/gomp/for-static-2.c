/* { dg-do compile } */
/* { dg-additional-options "-fopenmp-ompt -fdump-tree-ompexp" } */

/* Check that, with -fopenmp-ompt, the _start and _end variants are called.  */

#include "for-static.h"

/* { dg-final { scan-tree-dump-times "GOMP_loop_static_worksharing_start \\(" 2 "ompexp" } } */
/* { dg-final { scan-tree-dump-times "GOMP_loop_static_worksharing_end \\(" 2 "ompexp" } } */
/* { dg-final { scan-tree-dump-times "GOMP_distribute_static_worksharing_start \\(" 2 "ompexp" } } */
/* { dg-final { scan-tree-dump-times "GOMP_distribute_static_worksharing_end \\(" 2 "ompexp" } } */
/* { dg-final { scan-tree-dump-not "GOMP_loop_static_worksharing \\(" "ompexp" } } */
/* { dg-final { scan-tree-dump-not "GOMP_loop_static_worksharing_dispatch" "ompexp" } } */
/* { dg-final { scan-tree-dump-not "GOMP_distribute_static_worksharing \\(" "ompexp" } } */
/* { dg-final { scan-tree-dump-not "GOMP_distribute_static_worksharing_dispatch" "ompexp" } } */
