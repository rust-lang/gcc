/* { dg-do compile } */
/* { dg-options "-O3 -fcall-saved-r4" } */

void test(void) {
    asm volatile("nop" ::: "r4");
}

/* { dg-final { scan-assembler-times "\tstg\t" 1 } } */
/* { dg-final { scan-assembler-times "\tlg\t" 1 } } */
