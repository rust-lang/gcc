// { dg-do run { target c++26 } }

// Copyright (C) 2025-2026 Free Software Foundation, Inc.
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

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

// exception_ptr_cast.

#include <exception>
#include <testsuite_hooks.h>

#if __cpp_lib_exception_ptr_cast != 202603L
# error "__cpp_lib_exception_ptr_cast != 202606"
#endif

struct A { int a; };
struct B : A {};
struct C : B {};
struct D {};
struct E : virtual C { int e; constexpr virtual ~E () {} };
struct F : virtual E, virtual C { int f; };
struct G : virtual F, virtual C, virtual E {
  constexpr G () : g (4) { a = 1; e = 2; f = 3; } int g;
};

// Convertible to optional<const Y&>
struct Y
{
  constexpr operator std::optional<const Y&>() const
  { return std::nullopt; }
};

constexpr bool test01(bool x)
{
  auto a = std::make_exception_ptr(C{ 42 });
  auto b = std::exception_ptr_cast<C>(a);
  auto bp = &*b;
  VERIFY( b );
  VERIFY( b->a == 42 );
  auto c = std::exception_ptr_cast<B>(a);
  VERIFY( c );
  VERIFY( &*c == static_cast<const B*>(bp) );
  auto d = std::exception_ptr_cast<A>(a);
  VERIFY( d );
  VERIFY( &*d == static_cast<const A*>(bp) );
  auto e = std::exception_ptr_cast<D>(a);
  VERIFY( !e );
  auto f = std::make_exception_ptr(42L);
  auto g = std::exception_ptr_cast<long>(f);
  VERIFY( g );
  VERIFY( *g == 42L );

  try
    {
      throw G ();
    }
  catch (...)
    {
#if __has_builtin(__builtin_current_exception)
      auto h = __builtin_current_exception();
#else
      auto h = std::current_exception();
#endif
      auto i = std::exception_ptr_cast<G>(h);
      VERIFY( i );
      VERIFY( i->a == 1 && i->e == 2 && i->f == 3 && i->g == 4 );
      auto ip = &*i;
      auto j = std::exception_ptr_cast<A>(h);
      VERIFY( j );
      VERIFY( &*j == static_cast<const A*>(ip) );
      auto k = std::exception_ptr_cast<C>(h);
      VERIFY( k );
      VERIFY( &*k == static_cast<const C*>(ip) );
      auto l = std::exception_ptr_cast<E>(h);
      VERIFY( l );
      VERIFY( &*l == static_cast<const E*>(ip) );
      auto m = std::exception_ptr_cast<F>(h);
      VERIFY( m );
      VERIFY( &*m == static_cast<const F*>(ip) );
      auto n = std::exception_ptr_cast<G>(a);
      VERIFY( !n );
    }
  
  auto o = std::make_exception_ptr(Y{});
  auto p = std::exception_ptr_cast<Y>(o);
  VERIFY( p );

  if (x)
    throw 1;
  return true;
}

#if _GLIBCXX_USE_CXX11_ABI && __has_builtin(__builtin_current_exception)
static_assert(test01(false));
#endif

int main()
{
  try
    {
      test01(true);
    }
  catch (...)
    {
    }
}
