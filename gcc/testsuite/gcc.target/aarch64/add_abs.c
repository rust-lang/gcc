/* { dg-do compile } */
/* { dg-additional-options "-O2 -fno-schedule-insns -fno-schedule-insns2" } */
/* { dg-final { check-function-bodies "**" "" {} } } */
#include <arm_neon.h>

/*
** add_abs_q_s32:
**	movi	v[0-9]+.4s, 0
**	saba	v0.4s, v1.4s, v[0-9]+.4s
**	ret
*/
int32x4_t add_abs_q_s32 (int32x4_t a, int32x4_t b) {
  return vaddq_s32( a, vabsq_s32( b ) );
}

/*
** add_abs_q_s16:
**	movi	v[0-9]+.4s, 0
**	saba	v0.8h, v1.8h, v[0-9]+.8h
**	ret
*/
int16x8_t add_abs_q_s16 (int16x8_t a, int16x8_t b) {
  return vaddq_s16( a, vabsq_s16( b ) );
}

/*
** add_abs_q_s8:
**	movi	v[0-9]+.4s, 0
**	saba	v0.16b, v1.16b, v[0-9]+.16b
**	ret
*/
int8x16_t add_abs_q_s8 (int8x16_t a, int8x16_t b) {
  return vaddq_s8( a, vabsq_s8( b ) );
}

/*
** add_abs_s32:
**	movi	v[0-9]+.4s, 0
**	saba	v0.2s, v1.2s, v[0-9]+.2s
**	ret
*/
int32x2_t add_abs_s32 (int32x2_t a, int32x2_t b) {
  return vadd_s32( a, vabs_s32( b ) );
}

/*
** add_abs_s16:
**	movi	v[0-9]+.4s, 0
**	saba	v0.4h, v1.4h, v[0-9]+.4h
**	ret
*/
int16x4_t add_abs_s16 (int16x4_t a, int16x4_t b) {
  return vadd_s16( a, vabs_s16( b ) );
}

/*
** add_abs_s8:
**	movi	v[0-9]+.4s, 0
**	saba	v0.8b, v1.8b, v[0-9]+.8b
**	ret
*/
int8x8_t add_abs_s8 (int8x8_t a, int8x8_t b) {
  return vadd_s8( a, vabs_s8( b ) );
}
/*
** abs_add_q_s32:
**	movi	v[0-9]+.4s, 0
**	saba	v1.4s, v0.4s, v[0-9]+.4s
**	mov	v0.16b, v1.16b
**	ret
*/
int32x4_t abs_add_q_s32 (int32x4_t a, int32x4_t b) {
  return vaddq_s32( b, vabsq_s32( a ) );
}

/*
** abs_add_q_s16:
**	movi	v[0-9]+.4s, 0
**	saba	v1.8h, v0.8h, v[0-9]+.8h
**	mov	v0.16b, v1.16b
**	ret
*/
int16x8_t abs_add_q_s16 (int16x8_t a, int16x8_t b) {
  return vaddq_s16( b, vabsq_s16( a ) );
}

/*
** abs_add_q_s8:
**	movi	v[0-9]+.4s, 0
**	saba	v1.16b, v0.16b, v[0-9]+.16b
**	mov	v0.16b, v1.16b
**	ret
*/
int8x16_t abs_add_q_s8 (int8x16_t a, int8x16_t b) {
  return vaddq_s8( b, vabsq_s8( a ) );
}

/*
** abs_add_s32:
**	movi	v[0-9]+.4s, 0
**	saba	v1.2s, v0.2s, v[0-9]+.2s
**	mov	v0.8b, v1.8b
**	ret
*/
int32x2_t abs_add_s32 (int32x2_t a, int32x2_t b) {
  return vadd_s32( b, vabs_s32( a ) );
}

/*
** abs_add_s16:
**	movi	v[0-9]+.4s, 0
**	saba	v1.4h, v0.4h, v[0-9]+.4h
**	mov	v0.8b, v1.8b
**	ret
*/
int16x4_t abs_add_s16 (int16x4_t a, int16x4_t b) {
  return vadd_s16( b, vabs_s16( a ) );
}

/*
** abs_add_s8:
**	movi	v[0-9]+.4s, 0
**	saba	v1.8b, v0.8b, v[0-9]+.8b
**	mov	v0.8b, v1.8b
**	ret
*/
int8x8_t abs_add_s8 (int8x8_t a, int8x8_t b) {
  return vadd_s8( b, vabs_s8( a ) );
}
