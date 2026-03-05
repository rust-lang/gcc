// Implementation of <simd> -*- C++ -*-

// Copyright The GNU Toolchain Authors.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef _GLIBCXX_SIMD_MATH_H
#define _GLIBCXX_SIMD_MATH_H 1

#ifdef _GLIBCXX_SYSHDR
#pragma GCC system_header
#endif

#if __cplusplus >= 202400L

#include "simd_vec.h"

// psabi warnings are bogus because the ABI of the internal types never leaks into user code
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpsabi"

// [simd.math] ----------------------------------------------------------------
namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION
namespace simd
{
  template <signed_integral T, typename Abi>
    [[__gnu__::__always_inline__]]
    constexpr basic_vec<T, Abi>
    abs(const basic_vec<T, Abi>& __x)
    { return __x._M_abs(); }

  template <__math_floating_point _Vp>
    [[__gnu__::__always_inline__]]
    constexpr typename __deduced_vec_t<_Vp>::mask_type
    isinf(const _Vp& __x)
    { return static_cast<const __deduced_vec_t<_Vp>&>(__x)._M_isinf(); }

  template <__math_floating_point _Vp>
    [[__gnu__::__always_inline__]]
    constexpr typename __deduced_vec_t<_Vp>::mask_type
    isnan(const _Vp& __x)
    { return static_cast<const __deduced_vec_t<_Vp>&>(__x)._M_isnan(); }
} // namespace simd


// [simd.complex.math] --------------------------------------------------------
namespace simd
{
  template <__simd_complex _Vp>
    [[__gnu__::__always_inline__]]
    constexpr rebind_t<__simd_complex_value_type<_Vp>, _Vp>
    real(const _Vp& __x) noexcept
    { return __x.real(); }

  template <__simd_complex _Vp>
    [[__gnu__::__always_inline__]]
    constexpr rebind_t<__simd_complex_value_type<_Vp>, _Vp>
    imag(const _Vp& __x) noexcept
    { return __x.imag(); }

  template <__simd_complex _Vp>
    [[__gnu__::__always_inline__]]
    constexpr rebind_t<__simd_complex_value_type<_Vp>, _Vp>
    abs(const _Vp& __x) noexcept
    { return __x._M_abs(); }

  template <__simd_complex _Vp>
    [[__gnu__::__always_inline__]]
    constexpr rebind_t<__simd_complex_value_type<_Vp>, _Vp>
    arg(const _Vp& __x) noexcept
    { return __x._M_arg(); }

  template <__simd_complex _Vp>
    [[__gnu__::__always_inline__]]
    constexpr rebind_t<__simd_complex_value_type<_Vp>, _Vp>
    norm(const _Vp& __x) noexcept
    { return __x._M_norm(); }

  template <__simd_complex _Vp>
    [[__gnu__::__always_inline__]]
    constexpr _Vp
    conj(const _Vp& __x) noexcept
    { return __x._M_conj(); }

  template <__simd_complex _Vp>
    [[__gnu__::__always_inline__]]
    constexpr _Vp
    proj(const _Vp& __x) noexcept
    { return __x._M_proj(); }
} // namespace simd

  using simd::real;
  using simd::imag;
  using simd::abs;
  using simd::arg;
  using simd::norm;
  using simd::conj;
  using simd::proj;

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std

#pragma GCC diagnostic pop
#endif // C++26
#endif // _GLIBCXX_SIMD_MATH_H
