/* { dg-do compile } */
/* { dg-options "-Ofast -msve-vector-bits=256 -mautovec-preference=sve-only -fdump-tree-vect-details" } */

#define N 512
#define START 1
#define END 505

int x[N] __attribute__((aligned(32)));

int __attribute__((noipa))
foo (void)
{
  for (int *p = x + START; p < x + END; ++p)
    if (*p == 0)
      return START;

  return -1;
}

/* { dg-final { scan-tree-dump "Alignment of access forced using peeling" "vect" } } */
/* { dg-final { scan-tree-dump-times {Value numbering stmt = vectp_x\.[0-9]+_[0-9]+ = &x \+ 4;} 1 "vect" } } */
/* { dg-final { scan-tree-dump-times {Value numbering stmt = vectp_x\.[0-9]+_[0-9]+ = vectp_x\.[0-9]+_[0-9]+ \+ 18446744073709551612;} 1 "vect" } } */
