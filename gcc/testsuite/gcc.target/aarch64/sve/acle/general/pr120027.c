/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { check-function-bodies "**" "" } } */

/* Check that the merging form of the unsigned svextb/svexth/svextw intrinsics
   folds to an unpredicated AND when the governing predicate is all-true, and
   that the signed forms keep their sign-extend.  */

#include <arm_sve.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
** uxtb_m_u16_tied:
**	and	z0\.h, z0\.h, #0xff
**	ret
*/
svuint16_t
uxtb_m_u16_tied (svuint16_t x)
{
  return svextb_m (x, svptrue_b16 (), x);
}

/*
** uxtb_m_u16_untied:
**	movprfx	z0, z1
**	and	z0\.h, z0\.h, #0xff
**	ret
*/
svuint16_t
uxtb_m_u16_untied (svuint16_t inactive, svuint16_t x)
{
  return svextb_m (inactive, svptrue_b16 (), x);
}

/*
** uxtb_m_u32_tied:
**	and	z0\.s, z0\.s, #0xff
**	ret
*/
svuint32_t
uxtb_m_u32_tied (svuint32_t x)
{
  return svextb_m (x, svptrue_b32 (), x);
}

/*
** uxtb_m_u32_untied:
**	movprfx	z0, z1
**	and	z0\.s, z0\.s, #0xff
**	ret
*/
svuint32_t
uxtb_m_u32_untied (svuint32_t inactive, svuint32_t x)
{
  return svextb_m (inactive, svptrue_b32 (), x);
}

/*
** uxtb_m_u64_tied:
**	and	z0\.d, z0\.d, #0xff
**	ret
*/
svuint64_t
uxtb_m_u64_tied (svuint64_t x)
{
  return svextb_m (x, svptrue_b64 (), x);
}

/*
** uxtb_m_u64_untied:
**	movprfx	z0, z1
**	and	z0\.d, z0\.d, #0xff
**	ret
*/
svuint64_t
uxtb_m_u64_untied (svuint64_t inactive, svuint64_t x)
{
  return svextb_m (inactive, svptrue_b64 (), x);
}

/*
** uxth_m_u32_tied:
**	and	z0\.s, z0\.s, #0xffff
**	ret
*/
svuint32_t
uxth_m_u32_tied (svuint32_t x)
{
  return svexth_m (x, svptrue_b32 (), x);
}

/*
** uxth_m_u64_untied:
**	movprfx	z0, z1
**	and	z0\.d, z0\.d, #0xffff
**	ret
*/
svuint64_t
uxth_m_u64_untied (svuint64_t inactive, svuint64_t x)
{
  return svexth_m (inactive, svptrue_b64 (), x);
}

/*
** uxtw_m_u64_tied:
**	and	z0\.d, z0\.d, #0xffffffff
**	ret
*/
svuint64_t
uxtw_m_u64_tied (svuint64_t x)
{
  return svextw_m (x, svptrue_b64 (), x);
}

/*
** uxtw_m_u64_untied:
**	movprfx	z0, z1
**	and	z0\.d, z0\.d, #0xffffffff
**	ret
*/
svuint64_t
uxtw_m_u64_untied (svuint64_t inactive, svuint64_t x)
{
  return svextw_m (inactive, svptrue_b64 (), x);
}

/*
** sxtb_m_s32_tied:
**	ptrue	(p[0-7])\.b, all
**	sxtb	z0\.s, \1/m, z0\.s
**	ret
*/
svint32_t
sxtb_m_s32_tied (svint32_t x)
{
  return svextb_m (x, svptrue_b32 (), x);
}

/*
** sxtb_m_s32_untied:
**	ptrue	(p[0-7])\.b, all
**	sxtb	z0\.s, \1/m, z1\.s
**	ret
*/
svint32_t
sxtb_m_s32_untied (svint32_t inactive, svint32_t x)
{
  return svextb_m (inactive, svptrue_b32 (), x);
}

/*
** sxth_m_s64_tied:
**	ptrue	(p[0-7])\.b, all
**	sxth	z0\.d, \1/m, z0\.d
**	ret
*/
svint64_t
sxth_m_s64_tied (svint64_t x)
{
  return svexth_m (x, svptrue_b64 (), x);
}

/*
** sxtw_m_s64_tied:
**	ptrue	(p[0-7])\.b, all
**	sxtw	z0\.d, \1/m, z0\.d
**	ret
*/
svint64_t
sxtw_m_s64_tied (svint64_t x)
{
  return svextw_m (x, svptrue_b64 (), x);
}

#ifdef __cplusplus
}
#endif
