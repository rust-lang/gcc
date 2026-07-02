// optional<T&> -*- C++ -*-

// Copyright (C) 2013-2026 Free Software Foundation, Inc.
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

/** @file bits/optional_ref.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{optional}
 */

#ifndef _GLIBCXX_OPTIONAL_REF
#define _GLIBCXX_OPTIONAL_REF 1

#ifdef _GLIBCXX_SYSHDR
#pragma GCC system_header
#endif

#ifdef __glibcxx_optional // C++ >= 17

#include <type_traits>
#include <bits/move.h>
#include <bits/inplace_tags.h>

namespace __gnu_cxx _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  template<typename _Iterator, typename _Container>
    class __normal_iterator;

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace __gnu_cxx

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

   /**
   *  @addtogroup utilities
   *  @{
   */

  template<typename _Tp>
    class optional;

  /// Tag type to disengage optional objects.
  struct nullopt_t
  {
    // Do not user-declare default constructor at all for
    // optional_value = {} syntax to work.
    // nullopt_t() = delete;

    // Used for constructing nullopt.
    enum class _Construct { _Token };

    // Must be constexpr for nullopt_t to be literal.
    explicit constexpr nullopt_t(_Construct) noexcept { }
  };

  /// Tag to disengage optional objects.
  inline constexpr nullopt_t nullopt { nullopt_t::_Construct::_Token };

  template<typename _Fn> struct _Optional_func { _Fn& _M_f; };

  template<typename _Tp>
    inline constexpr bool __is_valid_contained_type_for_optional =
      (
#if __glibcxx_optional >= 202506L
	is_lvalue_reference_v<_Tp> ||
#endif
	(is_object_v<_Tp> && is_destructible_v<_Tp> && !is_array_v<_Tp>)
      )
      && !is_same_v<remove_cv_t<remove_reference_t<_Tp>>, nullopt_t>
      && !is_same_v<remove_cv_t<remove_reference_t<_Tp>>, in_place_t>;

#if __glibcxx_optional >= 202506L // C++26
  template<typename _Tp>
    class optional<_Tp&>;

  template<typename _Tp>
    constexpr bool __is_optional_ref_v = false;

  template<typename _Tp>
    constexpr bool __is_optional_ref_v<optional<_Tp&>> = true;

  template<typename _Tp>
    struct __optional_ref_base
    {};

#ifdef __glibcxx_optional_range_support // >= C++26
  template<typename _Tp>
    struct __optional_ref_base<_Tp[]>
    {};

  template<typename _Tp>
    requires is_object_v<_Tp>
    struct __optional_ref_base<_Tp>
    {
      using iterator = __gnu_cxx::__normal_iterator<_Tp*, optional<_Tp&>>;
    };
#endif // __glibcxx_optional_range_support

  template<typename _Tp>
    class optional<_Tp&> : public __optional_ref_base<_Tp>
    {
      static_assert(__is_valid_contained_type_for_optional<_Tp&>);

    public:
      using value_type = _Tp;

      constexpr static optional
      _S_from_ptr(_Tp* __ptr)
      {
	optional __res;
	__res._M_val = __ptr;
	return __res;
      }

      // Constructors.
      constexpr optional() noexcept = default;
      constexpr optional(nullopt_t) noexcept : optional() {}
      constexpr optional(const optional&) noexcept = default;

      template<typename _Arg>
	requires is_constructible_v<_Tp&, _Arg>
	  && (!reference_constructs_from_temporary_v<_Tp&, _Arg>)
	explicit constexpr
	optional(in_place_t, _Arg&& __arg)
	{
	  __convert_ref_init_val(std::forward<_Arg>(__arg));
	}

      template<typename _Up>
	requires (!is_same_v<remove_cvref_t<_Up>, optional>)
	  && (!is_same_v<remove_cvref_t<_Up>, in_place_t>)
	  && is_constructible_v<_Tp&, _Up>
	  && (!reference_constructs_from_temporary_v<_Tp&, _Up>)
	explicit(!is_convertible_v<_Up, _Tp&>)
	constexpr
	optional(_Up&& __u)
	noexcept(is_nothrow_constructible_v<_Tp&, _Up>)
	{
	  __convert_ref_init_val(std::forward<_Up>(__u));
	}

      template<typename _Up>
	requires (!is_same_v<remove_cvref_t<_Up>, optional>)
	  && (!is_same_v<remove_cvref_t<_Up>, in_place_t>)
	  && is_constructible_v<_Tp&, _Up>
	  && reference_constructs_from_temporary_v<_Tp&, _Up>
	explicit(!is_convertible_v<_Up, _Tp&>)
	constexpr
	optional(_Up&& __u) = delete;

      // optional<U> &
      template<typename _Up>
	requires (!is_same_v<remove_cv_t<_Tp>, optional<_Up>>)
	  && (!is_same_v<_Tp&, _Up>)
	  && is_constructible_v<_Tp&, _Up&>
	  && (!reference_constructs_from_temporary_v<_Tp&, _Up&>)
	explicit(!is_convertible_v<_Up&, _Tp&>)
	constexpr
	optional(optional<_Up>& __rhs)
	noexcept(is_nothrow_constructible_v<_Tp&, _Up&>)
	{
	  if (__rhs)
	    __convert_ref_init_val(__rhs._M_fwd());
	}

      template<typename _Up>
	requires (!is_same_v<remove_cv_t<_Tp>, optional<_Up>>)
	  && (!is_same_v<_Tp&, _Up>)
	  && is_constructible_v<_Tp&, _Up&>
	  && reference_constructs_from_temporary_v<_Tp&, _Up&>
	explicit(!is_convertible_v<_Up&, _Tp&>)
	constexpr
	optional(optional<_Up>& __rhs) = delete;

      // const optional<U>&
      template<typename _Up>
	requires (!is_same_v<remove_cv_t<_Tp>, optional<_Up>>)
	  && (!is_same_v<_Tp&, _Up>)
	  && is_constructible_v<_Tp&, const _Up&>
	  && (!reference_constructs_from_temporary_v<_Tp&, const _Up&>)
	explicit(!is_convertible_v<const _Up&, _Tp&>)
	constexpr
	optional(const optional<_Up>& __rhs)
	noexcept(is_nothrow_constructible_v<_Tp&, _Up&>)
	{
	  if (__rhs)
	    __convert_ref_init_val(__rhs._M_fwd());
	}

      template<typename _Up>
	requires (!is_same_v<remove_cv_t<_Tp>, optional<_Up>>)
	  && (!is_same_v<_Tp&, _Up>)
	  && is_constructible_v<_Tp&, const _Up&>
	  && reference_constructs_from_temporary_v<_Tp&, const _Up&>
	explicit(!is_convertible_v<const _Up&, _Tp&>)
	constexpr
	optional(const optional<_Up>& __rhs) = delete;

      // optional<U>&&
      template<typename _Up>
	requires (!is_same_v<remove_cv_t<_Tp>, optional<_Up>>)
	  && (!is_same_v<_Tp&, _Up>)
	  && is_constructible_v<_Tp&, _Up>
	  && (!reference_constructs_from_temporary_v<_Tp&, _Up>)
	explicit(!is_convertible_v<_Up, _Tp&>)
	constexpr
	optional(optional<_Up>&& __rhs)
	noexcept(is_nothrow_constructible_v<_Tp&, _Up>)
	{
	  if (__rhs)
	    __convert_ref_init_val(std::move(__rhs)._M_fwd());
	}

      template<typename _Up>
	requires (!is_same_v<remove_cv_t<_Tp>, optional<_Up>>)
	  && (!is_same_v<_Tp&, _Up>)
	  && is_constructible_v<_Tp&, _Up>
	  && reference_constructs_from_temporary_v<_Tp&, _Up>
	explicit(!is_convertible_v<_Up, _Tp&>)
	constexpr
	optional(optional<_Up>&& __rhs) = delete;

      // const optional<U>&&
      template<typename _Up>
	requires (!is_same_v<remove_cv_t<_Tp>, optional<_Up>>)
	  && (!is_same_v<_Tp&, _Up>)
	  && is_constructible_v<_Tp&, const _Up>
	  && (!reference_constructs_from_temporary_v<_Tp&, _Up>)
	explicit(!is_convertible_v<const _Up, _Tp&>)
	constexpr
	optional(const optional<_Up>&& __rhs)
	noexcept(is_nothrow_constructible_v<_Tp&, const _Up>)
	{
	  if (__rhs)
	    __convert_ref_init_val(std::move(__rhs)._M_fwd());
	}

      template<typename _Up>
	requires (!is_same_v<remove_cv_t<_Tp>, optional<_Up>>)
	  && (!is_same_v<_Tp&, _Up>)
	  && is_constructible_v<_Tp&, const _Up>
	  && reference_constructs_from_temporary_v<_Tp&, const _Up>
	explicit(!is_convertible_v<const _Up, _Tp&>)
	constexpr
	optional(const optional<_Up>&& __rhs) = delete;

      constexpr ~optional() = default;

      // Assignment.
      constexpr optional& operator=(nullopt_t) noexcept
      {
	_M_val = nullptr;
	return *this;
      }

      constexpr optional& operator=(const optional&) noexcept = default;

      template<typename _Up>
	requires is_constructible_v<_Tp&, _Up>
	  && (!reference_constructs_from_temporary_v<_Tp&, _Up>)
	constexpr _Tp&
	emplace(_Up&& __u)
	noexcept(is_nothrow_constructible_v<_Tp&, _Up>)
	{
	  __convert_ref_init_val(std::forward<_Up>(__u));
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 4300. Missing Returns: element in optional<T&>::emplace
	  return *_M_val;
	}

      // Swap.
      constexpr void swap(optional& __rhs) noexcept
      { std::swap(_M_val, __rhs._M_val); }

#ifdef __glibcxx_optional_range_support // >= C++26
      // Iterator support.
      constexpr auto begin() const noexcept
	requires is_object_v<_Tp> && (!is_unbounded_array_v<_Tp>);

      constexpr auto end() const noexcept
	requires is_object_v<_Tp> && (!is_unbounded_array_v<_Tp>);
#endif // __glibcxx_optional_range_support

      // Observers.
      constexpr _Tp* operator->() const noexcept
      {
	__glibcxx_assert(_M_val); // hardened precondition
	return _M_val;
      }

      constexpr _Tp& operator*() const noexcept
      {
	__glibcxx_assert(_M_val); // hardened precondition
	return *_M_val;
      }

      constexpr explicit operator bool() const noexcept
      {
	return _M_val;
      }

      constexpr bool has_value() const noexcept
      {
	return _M_val;
      }

      constexpr _Tp& value() const;

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 4304. std::optional<NonReturnable&> is ill-formed due to value_or
      template<typename _Up = remove_cv_t<_Tp>>
	requires is_object_v<_Tp> && (!is_array_v<_Tp>)
	constexpr decay_t<_Tp>
	value_or(_Up&& __u) const;

      // Monadic operations.
      template<typename _Fn>
	constexpr auto
	and_then(_Fn&& __f) const;

      template<typename _Fn>
	constexpr
	optional<remove_cv_t<invoke_result_t<_Fn, _Tp&>>>
	transform(_Fn&& __f) const;

      template<typename _Fn>
	requires is_invocable_v<_Fn>
	constexpr
	optional
	or_else(_Fn&& __f) const;

      // Modifiers.
      constexpr void reset() noexcept
      {
	_M_val = nullptr;
      }

    private:
      _Tp *_M_val = nullptr;

      [[__gnu__::__always_inline__]]
      constexpr _Tp&
      _M_fwd() const noexcept
      { return *_M_val; }

      template<typename _Up> friend class optional;

      template<typename _Up>
	constexpr
	void
	__convert_ref_init_val(_Up&& __u)
	noexcept
	{
	  _Tp& __r(std::forward<_Up>(__u));
	  _M_val = std::addressof(__r);
	}

      template<typename _Fn, typename _Value>
	explicit constexpr
	optional(_Optional_func<_Fn> __f, _Value&& __v);
    };
#endif // __glibcxx_optional >= 202506L

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std

#endif // __glibcxx_optional

#endif // _GLIBCXX_OPTIONAL_REF
