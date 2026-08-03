/* m68k long double is the 96-bit motorola extended format: 12
   bytes, exponent word first - not the primary's padded intel
   format.  */
/* { dg-do compile } */

long double d = 2.0L;

int size_check[sizeof (long double) == 12 ? 1 : -1];

/* { dg-final { scan-assembler "\\.size\td, 12" } } */
/* { dg-final { scan-assembler "\\.long\t1073741824" } } */
