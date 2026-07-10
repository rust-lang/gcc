/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized -fno-trapping-math" } */

int foo1 (int p) {
  return (float) p != 0;
  /* { dg-final { scan-tree-dump "p_\[0-9]+\\(D\\) != 0;" "optimized" } } */
}

int foo2 (unsigned int q) {
  return (float) q != 0;
  /* { dg-final { scan-tree-dump "q_\[0-9]+\\(D\\) != 0;" "optimized" } } */
}

int foo3 (long long r) {
  return (float) r != 0;
  /* { dg-final { scan-tree-dump "r_\[0-9]+\\(D\\) != 0;" "optimized" } } */
}

int foo4 (unsigned long long s) {
  return (float) s != 0;
  /* { dg-final { scan-tree-dump "s_\[0-9]+\\(D\\) != 0;" "optimized" } } */
}

int foo5 (long long t) {
  return (double) t != 0;
  /* { dg-final { scan-tree-dump "t_\[0-9]+\\(D\\) != 0;" "optimized" } } */
}

int foo6 (unsigned long long u) {
  return (double) u != 0;
  /* { dg-final { scan-tree-dump "u_\[0-9]+\\(D\\) != 0;" "optimized" } } */
}

/* Tests when RHS is within range of integer type.  */

void in_range (unsigned int x)
{
  {
    volatile int in_range_1;
    in_range_1 = (float) x > 100.0f;
    /* { dg-final { scan-tree-dump "x_\[0-9]+\\(D\\) > 100;" "optimized" } } */
  }

  {
    volatile int in_range_2;
    in_range_2 = (float) x < 200.0f;
    /* { dg-final { scan-tree-dump "\[0-9]+\\(D\\) <= 199;" "optimized" } } */
  }

  {
    volatile int in_range_3;
    in_range_3 = (float) x > 300.5f;
    /* { dg-final { scan-tree-dump "\[0-9]+\\(D\\) > 300;" "optimized" } } */
  }

  {
    volatile int in_range_4;
    in_range_4 = (float) x < 400.5f;
    /* { dg-final { scan-tree-dump "\[0-9]+\\(D\\) <= 400;" "optimized" } } */
  }

  {
    volatile int in_range_5;
    in_range_5 = (float) x == 500.0f;
    /* { dg-final { scan-tree-dump "\[0-9]+\\(D\\) == 500;" "optimized" } } */
  }

  {
    volatile int in_range_6;
    in_range_6 = (float) x != 600.0f;
    /* { dg-final { scan-tree-dump "\[0-9]+\\(D\\) != 600;" "optimized" } } */
  }

  {
    volatile int in_range_7;
    in_range_7 = (float) x == 700.5f;
    /* { dg-final { scan-tree-dump "in_range_7 ={v} 0" "optimized" } } */
  }

  {
    volatile int in_range_8;
    in_range_8 = (float) x != 800.5f;
    /* { dg-final { scan-tree-dump "in_range_8 ={v} 1" "optimized" } } */
  }
}

/* Tests for cases where RHS is out of range of integer type.  */

void out_range (unsigned int x)
{
  {
    volatile int out_range_1;
    out_range_1 = (float) x > -100.5f;
    /* { dg-final { scan-tree-dump "out_range_1 ={v} 1" "optimized" } } */
  }

  {
    volatile int out_range_2;
    out_range_2 = (float) x >= -100.5f;
    /* { dg-final { scan-tree-dump "out_range_2 ={v} 1" "optimized" } } */
  }

  {
    volatile int out_range_3;
    out_range_3 = (float) x < -100.5f;
    /* { dg-final { scan-tree-dump "out_range_3 ={v} 0" "optimized" } } */
  }

  {
    volatile int out_range_4;
    out_range_4 = (float) x <= -100.5f;
    /* { dg-final { scan-tree-dump "out_range_4 ={v} 0" "optimized" } } */
  }

  {
    volatile int out_range_5;
    out_range_5 = (float) x == -100.5f;
    /* { dg-final { scan-tree-dump "out_range_5 ={v} 0" "optimized" } } */
  }

  {
    volatile int out_range_6;
    out_range_6 = (float) x != -100.5f;
    /* { dg-final { scan-tree-dump "out_range_6 ={v} 1" "optimized" } } */
  }
}

/* Tests with non-finite float consts.  */

void nonfinite (unsigned int x)
{
#define INFINITY __builtin_inff ()

  {
    volatile int nonfinite_1;
    nonfinite_1 = (float) x > INFINITY;
    /* { dg-final { scan-tree-dump "nonfinite_1 ={v} 0" "optimized" } } */
  }

  {
    volatile int nonfinite_2;
    nonfinite_2 = (float) x >= INFINITY;
    /* { dg-final { scan-tree-dump "nonfinite_2 ={v} 0" "optimized" } } */
  }

  {
    volatile int nonfinite_3;
    nonfinite_3 = (float) x < INFINITY;
    /* { dg-final { scan-tree-dump "nonfinite_3 ={v} 1" "optimized" } } */
  }

  {
    volatile int nonfinite_4;
    nonfinite_4 = (float) x <= INFINITY;
    /* { dg-final { scan-tree-dump "nonfinite_4 ={v} 1" "optimized" } } */
  }

  {
    volatile int nonfinite_5;
    nonfinite_5 = (float) x > -INFINITY;
    /* { dg-final { scan-tree-dump "nonfinite_5 ={v} 1" "optimized" } } */
  }

  {
    volatile int nonfinite_6;
    nonfinite_6 = (float) x >= -INFINITY;
    /* { dg-final { scan-tree-dump "nonfinite_6 ={v} 1" "optimized" } } */
  }

  {
    volatile int nonfinite_7;
    nonfinite_7 = (float) x < -INFINITY;
    /* { dg-final { scan-tree-dump "nonfinite_7 ={v} 0" "optimized" } } */
  }

  {
    volatile int nonfinite_8;
    nonfinite_8 = (float) x <= -INFINITY;
    /* { dg-final { scan-tree-dump "nonfinite_8 ={v} 0" "optimized" } } */
  }

#define QNAN __builtin_nanf ("0")

  /* Even for qNaNs, only == and != are quiet.  */

  {
    volatile int nonfinite_9;
    nonfinite_9 = (float) x == QNAN;
    /* { dg-final { scan-tree-dump "nonfinite_9 ={v} 0" "optimized" } } */
  }

  {
    volatile int nonfinite_10;
    nonfinite_10 = (float) x != QNAN;
    /* { dg-final { scan-tree-dump "nonfinite_10 ={v} 1" "optimized" } } */
  }
}

/* { dg-final { scan-tree-dump-not "\\(float\\)" "optimized" } } */
/* { dg-final { scan-tree-dump-not "\\(double\\)" "optimized" } } */
