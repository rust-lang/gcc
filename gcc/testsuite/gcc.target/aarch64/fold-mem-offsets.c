/* { dg-do compile } */
/* { dg-options "-O2 -ffold-mem-offsets" } */

struct a {
  struct a *b;
  int *d;
  int c;
  long ad[];
} e, g;

int f;
long h;
void i() {
  h = g.ad[f] & e.ad[f];
}

/* Check for updated memory offsets.  */
/* { dg-final { scan-assembler "ldr\t.*, \[.*, 64\]" } } */
/* { dg-final { scan-assembler "ldr\t.*, \[.*, 40\]" } } */
