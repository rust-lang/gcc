/* { dg-do compile } */
/* { dg-options "-m4 -mfmovd -O1" } */
/* { dg-final { scan-assembler "lds.l\t@r\[0-9\]+\\+,fpscr" } } */

extern double g;

void __attribute__ ((interrupt_handler))
isr (void)
{
  g += 1.0;
}
