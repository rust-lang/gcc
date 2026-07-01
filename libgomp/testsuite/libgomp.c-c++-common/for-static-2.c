/* { dg-do link { target offload_target_any } } */
/* { dg-additional-options "-O0 -foffload=-fdump-tree-optimized -fopenmp-ompt" } */

/* Check that, with -fopenmp-ompt, the _start and _end variants are called.  */

#include "for-static.h"

/* { dg-final { scan-offload-tree-dump-times "GOMP_distribute_static_worksharing_start \\(\[^\\)\]" 2 "optimized" } } */
/* { dg-final { scan-offload-tree-dump-times "GOMP_distribute_static_worksharing_end \\(\\)" 2 "optimized" } } */
/* { dg-final { scan-offload-tree-dump-not "GOMP_distribute_static_worksharing \\(" "optimized" } } */
/* { dg-final { scan-offload-tree-dump-not "GOMP_distribute_static_worksharing_dispatch" "optimized" } } */
