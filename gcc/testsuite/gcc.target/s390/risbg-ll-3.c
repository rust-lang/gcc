// Test use of RISBG vs RISBGN on zEC12.

/* Tests ported from the Llvm testsuite. */

/* { dg-do compile } */
/* { dg-options "-O3 -march=zEC12 -mzarch -fno-asynchronous-unwind-tables" } */

#define i64 signed long long
#define ui64 unsigned long long

// On zEC12, we generally prefer RISBGN.
i64 f1 (i64 v_a, i64 v_b)
{
/* { dg-final { scan-assembler "f1:\n\trisbgn\t%r2,%r3,60,62,0" } } */
  i64 v_anda = v_a & -15;
  i64 v_andb = v_b & 14;
  i64 v_or = v_anda | v_andb;
  return v_or;
}

// But we may fall back to RISBG if we can use the condition code.
extern i64 f2_foo();
i64 f2 (i64 v_a, i64 v_b)
{
/* { dg-final { scan-assembler "f2:\n\trisbg\t%r2,%r3,60,62,0\n\tbner\t%r14\n\tjg\tf2_foo.*\n" } } */
  i64 v_anda = v_a & -15;
  i64 v_andb = v_b & 14;
  i64 v_or = v_anda | v_andb;
  if (! v_or)
    return f2_foo();
  else
    return v_or;
}

void f2_bar ();
void f2_cconly (i64 v_a, i64 v_b)
{
/* { dg-final { scan-assembler "f2_cconly:\n\trisbg\t%r2,%r3,60,62,0\n\tber\t%r14\n\tjg\tf2_bar(@PLT)?\n" } } */
  if ((v_a & -15) | (v_b & 14))
    f2_bar();
}
