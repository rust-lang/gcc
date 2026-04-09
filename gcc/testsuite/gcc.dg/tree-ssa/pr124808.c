/* { dg-do run } */
/* { dg-options "-O2" } */

void abort(void);

/* Macro adapted from builtin-object-size-common.h  */
#define FAIL() \
  do { \
    __builtin_printf ("Failure at line: %d\n", __LINE__);	\
    abort();							\
  } while (0)

#define SHIFTVAL 3
#define CMPVAL 1

long setValue1 (long in)
{
  if (in >> SHIFTVAL < CMPVAL)
    return in += SHIFTVAL;
  return -1;
}

long setValue2 (long in)
{
  if (in >> SHIFTVAL <= CMPVAL)
    return in += SHIFTVAL;
  return -1;
}

long setValue3 (long in)
{
  if (in >> SHIFTVAL > CMPVAL)
    return in += SHIFTVAL;
  return -1;
}

long setValue4 (long in)
{
  if (in >> SHIFTVAL >= CMPVAL)
    return in += SHIFTVAL;
  return -1;
}

int main (void) {
  /* setValue1: in << 3 < 1;  */
  if (setValue1 (7) != 10)
    FAIL ();
  if (setValue1 (8) != -1)
    FAIL ();

  /* setValue2: in << 3 <= 1;  */
  if (setValue2 (7) != 10)
    FAIL ();
  if (setValue2 (8) != 11)
    FAIL ();
  if (setValue2 (15) != 18)
    FAIL ();
  if (setValue2 (16) != -1)
    FAIL ();

  /* setValue3: in << 3 > 1;  */
  if (setValue3 (15) != -1)
    FAIL ();
  if (setValue3 (16) != 19)
    FAIL ();

  /* setValue4: in << 3 >= 1;  */
  if (setValue4 (7) != -1)
    FAIL ();
  if (setValue4 (8) != 11)
    FAIL ();
  if (setValue4 (15) != 18)
    FAIL ();
  if (setValue4 (16) != 19)
    FAIL ();

  return 0;
}
