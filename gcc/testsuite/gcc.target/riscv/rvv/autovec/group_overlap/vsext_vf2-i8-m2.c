/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e8m2,
  vint8m2_t,
  vint16m4_t,
  __riscv_vle8_v_i8m2,
  __riscv_vsext_vf2_i16m4,
  __riscv_vse16_v_i16m4,
  vsext_vf,
  LOOP_UNARY_BODY_X8)

/* { dg-final { scan-assembler-times {vsext\.vf2\s+v0,v2([^0-9]|$)} 1 } } */
/* { dg-final { scan-assembler-times {vsext\.vf2\s+v4,v6} 1 } } */
