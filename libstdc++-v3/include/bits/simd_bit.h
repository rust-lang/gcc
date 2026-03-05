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

#ifndef _GLIBCXX_SIMD_BIT_H
#define _GLIBCXX_SIMD_BIT_H 1

#ifdef _GLIBCXX_SYSHDR
#pragma GCC system_header
#endif

#if __cplusplus >= 202400L

#include "simd_vec.h"

// psabi warnings are bogus because the ABI of the internal types never leaks into user code
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpsabi"

// [simd.bit] -----------------------------------------------------------------
namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION
namespace simd
{
  template<__simd_integral _Vp>
    [[__gnu__::__always_inline__]]
    constexpr _Vp
    byteswap(const _Vp& __v) noexcept
    {
      if constexpr (sizeof(typename _Vp::value_type) == 1)
	return __v;
      else
	return _Vp([&](int __i) { return std::byteswap(__v[__i]); });
    }

  template<__simd_unsigned_integer _Vp>
    [[__gnu__::__always_inline__]]
    constexpr _Vp
    bit_ceil(const _Vp& __v)
    {
      using _Tp = typename _Vp::value_type;
      constexpr _Tp __max = _Tp(1) << (sizeof(_Tp) * __CHAR_BIT__ - 1);
      __glibcxx_simd_precondition(all_of(__v <= __max), "bit_ceil result is not representable");
      return _Vp([&](int __i) { return std::bit_ceil(__v[__i]); });
    }

  template<__simd_unsigned_integer _Vp>
    [[__gnu__::__always_inline__]]
    constexpr _Vp
    bit_floor(const _Vp& __v) noexcept
    { return _Vp([&](int __i) { return std::bit_floor(__v[__i]); }); }

  template<__simd_unsigned_integer _Vp>
    [[__gnu__::__always_inline__]]
    constexpr typename _Vp::mask_type
    has_single_bit(const _Vp& __v) noexcept
    { return typename _Vp::mask_type([&](int __i) { return std::has_single_bit(__v[__i]); }); }

  template<__simd_unsigned_integer _V0, __simd_integral _V1>
    requires (_V0::size() == _V1::size())
      && (sizeof(typename _V0::value_type) == sizeof(typename _V1::value_type))
    [[__gnu__::__always_inline__]]
    constexpr _V0
    rotl(const _V0& __v, const _V1& __s) noexcept
    { return _V0([&](int __i) { return std::rotl(__v[__i], __s[__i]); }); }

  template<__simd_unsigned_integer _Vp>
    [[__gnu__::__always_inline__]]
    constexpr _Vp
    rotl(const _Vp& __v, int __s) noexcept
    { return _Vp([&](int __i) { return std::rotl(__v[__i], __s); }); }

  template<__simd_unsigned_integer _V0, __simd_integral _V1>
    requires (_V0::size() == _V1::size())
      && (sizeof(typename _V0::value_type) == sizeof(typename _V1::value_type))
    [[__gnu__::__always_inline__]]
    constexpr _V0
    rotr(const _V0& __v, const _V1& __s) noexcept
    { return _V0([&](int __i) { return std::rotr(__v[__i], __s[__i]); }); }

  template<__simd_unsigned_integer _Vp>
    [[__gnu__::__always_inline__]]
    constexpr _Vp
    rotr(const _Vp& __v, int __s) noexcept
    { return _Vp([&](int __i) { return std::rotr(__v[__i], __s); }); }

  template<__simd_unsigned_integer _Vp>
    [[__gnu__::__always_inline__]]
    constexpr rebind_t<make_signed_t<typename _Vp::value_type>, _Vp>
    bit_width(const _Vp& __v) noexcept
    {
      using _Ip = make_signed_t<typename _Vp::value_type>;
      return rebind_t<_Ip, _Vp>([&](int __i) {
	       return static_cast<_Ip>(std::bit_width(__v[__i]));
	     });
    }

  template<__simd_unsigned_integer _Vp>
    [[__gnu__::__always_inline__]]
    constexpr rebind_t<make_signed_t<typename _Vp::value_type>, _Vp>
    countl_zero(const _Vp& __v) noexcept
    {
      using _Ip = make_signed_t<typename _Vp::value_type>;
      return rebind_t<_Ip, _Vp>([&](int __i) {
	       return static_cast<_Ip>(std::countl_zero(__v[__i]));
	     });
    }

  template<__simd_unsigned_integer _Vp>
    [[__gnu__::__always_inline__]]
    constexpr rebind_t<make_signed_t<typename _Vp::value_type>, _Vp>
    countl_one(const _Vp& __v) noexcept
    {
      using _Ip = make_signed_t<typename _Vp::value_type>;
      return rebind_t<_Ip, _Vp>([&](int __i) {
	       return static_cast<_Ip>(std::countl_one(__v[__i]));
	     });
    }

  template<__simd_unsigned_integer _Vp>
    [[__gnu__::__always_inline__]]
    constexpr rebind_t<make_signed_t<typename _Vp::value_type>, _Vp>
    countr_zero(const _Vp& __v) noexcept
    {
      using _Ip = make_signed_t<typename _Vp::value_type>;
      return rebind_t<_Ip, _Vp>([&](int __i) {
	       return static_cast<_Ip>(std::countr_zero(__v[__i]));
	     });
    }

  template<__simd_unsigned_integer _Vp>
    [[__gnu__::__always_inline__]]
    constexpr rebind_t<make_signed_t<typename _Vp::value_type>, _Vp>
    countr_one(const _Vp& __v) noexcept
    {
      using _Ip = make_signed_t<typename _Vp::value_type>;
      return rebind_t<_Ip, _Vp>([&](int __i) {
	       return static_cast<_Ip>(std::countr_one(__v[__i]));
	     });
    }

  template<__simd_unsigned_integer _Vp>
    [[__gnu__::__always_inline__]]
    constexpr rebind_t<make_signed_t<typename _Vp::value_type>, _Vp>
    popcount(const _Vp& __v) noexcept
    {
      using _Ip = make_signed_t<typename _Vp::value_type>;
      return rebind_t<_Ip, _Vp>([&](int __i) {
	       return static_cast<_Ip>(std::popcount(__v[__i]));
	     });
    }
} // namespace simd

  using simd::byteswap;
  using simd::bit_ceil;
  using simd::bit_floor;
  using simd::has_single_bit;
  using simd::rotl;
  using simd::rotr;
  using simd::bit_width;
  using simd::countl_zero;
  using simd::countl_one;
  using simd::countr_zero;
  using simd::countr_one;
  using simd::popcount;
_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std

#pragma GCC diagnostic pop
#endif // C++26
#endif // _GLIBCXX_SIMD_BIT_H
