/* { dg-do compile } */
/* { dg-additional-options "-fopenmp-ompt-detailed -fdump-tree-ompexp" } */

/* Check that, with -fopenmp-ompt-detailed, _dispatch builtins are called along
   with the _start and _end variants.  */

#include "for-static.h"

/* { dg-final { scan-tree-dump-times "GOMP_loop_static_worksharing_start \\(" 2 "ompexp" } } */
/* { dg-final { scan-tree-dump-times "GOMP_loop_static_worksharing_dispatch \\(" 2 "ompexp" } } */
/* { dg-final { scan-tree-dump-times "GOMP_loop_static_worksharing_end \\(" 2 "ompexp" } } */
/* { dg-final { scan-tree-dump-times "GOMP_distribute_static_worksharing_start \\(" 2 "ompexp" } } */
/* { dg-final { scan-tree-dump-times "GOMP_distribute_static_worksharing_dispatch \\(" 2 "ompexp" } } */
/* { dg-final { scan-tree-dump-times "GOMP_distribute_static_worksharing_end \\(" 2 "ompexp" } } */
