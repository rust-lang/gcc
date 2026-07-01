/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e8m2,
  vuint8m2_t,
  vuint16m4_t,
  __riscv_vle8_v_u8m2,
  __riscv_vzext_vf2_u16m4,
  __riscv_vse16_v_u16m4,
  vzext_vf,
  LOOP_UNARY_BODY_X8)

/* { dg-final { scan-assembler-times {vzext\.vf2\s+v0,v2} 1 } } */
/* { dg-final { scan-assembler-times {vzext\.vf2\s+v4,v6} 1 } } */
