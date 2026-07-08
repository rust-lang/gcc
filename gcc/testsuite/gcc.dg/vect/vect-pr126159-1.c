/* { dg-do compile } */

typedef union { int i; float f; } RtreeCoord;
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

void cellUnion_f(RtreeCoord *a1, RtreeCoord *a2, int n) {
  int ii = 0;
  do {
    a1[ii].f   = MIN(a1[ii].f,   a2[ii].f);
    a1[ii+1].f = MAX(a1[ii+1].f, a2[ii+1].f);
    ii += 2;
  } while (ii < n);
}

/* { dg-final { scan-tree-dump-times "optimized: loop vectorized" 1 "vect" { target { vect_float } } } } */
