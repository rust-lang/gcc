// PR c++/116162
// { dg-do compile { target c++11 } }
// { dg-additional-options "-Wno-defaulted-function-deleted" }

struct S
{
  S& operator=(S &&) = default;
};

struct T
{
  T& operator=(volatile T &&) = default;	// { dg-error "does not match the expected signature" "" { target c++29 } }
};

struct U
{
  U& operator=(const volatile U &&) = default;	// { dg-error "does not match the expected signature" "" { target c++29 } }
};

struct V
{
  V& operator=(const V &&) = default;		// { dg-error "does not match the expected signature" "" { target c++29 } }
};
