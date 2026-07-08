/* { dg-do compile } */

typedef union { int i; float f; } RtreeCoord;
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

void cellUnion_i(RtreeCoord *a1, RtreeCoord *a2, int n) {
  int ii = 0;
  do {
    a1[ii].i   = MIN(a1[ii].i,   a2[ii].i);
    a1[ii+1].i = MAX(a1[ii+1].i, a2[ii+1].i);
    ii += 2;
  } while (ii < n);
}

/* { dg-final { scan-tree-dump-times "optimized: loop vectorized" 1 "vect" { target { vect_int && { ! vect_no_int_min_max } } } } } */
