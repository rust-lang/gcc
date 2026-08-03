/* x86-64 long double is the intel extended format padded to 16
   bytes, exponent last.  */
/* { dg-do compile } */

long double d = 2.0L;

int size_check[sizeof (long double) == 16 ? 1 : -1];

/* { dg-final { scan-assembler "\\.size\td, 16" } } */
/* { dg-final { scan-assembler "\\.long\t16384" } } */
