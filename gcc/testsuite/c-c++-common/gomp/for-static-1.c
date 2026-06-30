/* { dg-do compile } */
/* { dg-additional-options "-fdump-tree-ompexp" } */

/* Check that the static loop and distribute expanders fetch the thread/team
   id and count through a single GOMP_loop_static_worksharing /
   GOMP_distribute_static_worksharing call rather than separate
   omp_get_thread_num/omp_get_num_threads or omp_get_team_num/omp_get_num_teams
   calls.  */

#include "for-static.h"

/* { dg-final { scan-tree-dump-times "GOMP_loop_static_worksharing \\(" 2 "ompexp" } } */
/* { dg-final { scan-tree-dump-times "GOMP_distribute_static_worksharing \\(" 2 "ompexp" } } */
/* { dg-final { scan-tree-dump-not "omp_get_num_threads" "ompexp" } } */
/* { dg-final { scan-tree-dump-not "omp_get_thread_num" "ompexp" } } */
/* { dg-final { scan-tree-dump-not "omp_get_num_teams" "ompexp" } } */
/* { dg-final { scan-tree-dump-not "omp_get_team_num" "ompexp" } } */
