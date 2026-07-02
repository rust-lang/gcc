/* { dg-do compile } */
/* { dg-require-effective-target aarch64_gas_has_dtprel_reloc } */
/* { dg-options "-O0 -g" } */

#include <stdio.h>

__thread unsigned long rage = 1;
__thread unsigned long ire = 2;

void*
increase_rage()
{
  ++rage;
  ++ire;
  printf ("Rage counter for: %lu/%lu\n", rage, ire);
}

int
main ()
{
  increase_rage();
  printf ("Rage counter %lu/%lu\n", rage, ire);
}

/* { dg-final { scan-assembler ".xword\t%dtprel\\(rage\\)" } } */
/* { dg-final { scan-assembler ".xword\t%dtprel\\(ire\\)" } } */
