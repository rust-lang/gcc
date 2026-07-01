/* { dg-do link { target offload_target_any } } */
/* { dg-additional-options "-O0 -foffload=-fdump-tree-optimized" } */

/* Check that the static distribute expanders fetch the team id and count
   through a single GOMP_distribute_static_worksharing call rather than separate
   omp_get_team_num and omp_get_num_teams calls, and that this function exists
   on the target side.  */

#include "for-static.h"

/* { dg-final { scan-offload-tree-dump-times "GOMP_distribute_static_worksharing \\(\[^\\)\]" 2 "optimized" } } */
/* { dg-final { scan-offload-tree-dump-not "omp_get_num_teams" "optimized" } } */
/* { dg-final { scan-offload-tree-dump-not "omp_get_team_num" "optimized" } } */
