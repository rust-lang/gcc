/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -fno-vect-cost-model -fdump-rtl-expand-details" } */

#include "strided_ld_st.h"

DEF_STRIDED_LD_ST_FORM_1(int64_t)

/* { dg-final { scan-rtl-dump-times ".MASK_LEN_STRIDED_LOAD " 4 "expand" { xfail { any-opts "-O2" } } } } */
/* { dg-final { scan-rtl-dump-times ".MASK_LEN_STRIDED_STORE " 4 "expand" { xfail { any-opts "-O2" } } } } */
/* { dg-final { scan-rtl-dump-times ".MASK_LEN_STRIDED_LOAD " 2 "expand" { xfail { any-opts "-O3" } } } } */
/* { dg-final { scan-rtl-dump-times ".MASK_LEN_STRIDED_STORE " 2 "expand" { xfail { any-opts "-O3" } } } } */
/* { dg-final { scan-assembler-times {vlse64.v} 1 } } */
/* { dg-final { scan-assembler-times {vsse64.v} 1 } } */
