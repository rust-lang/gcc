// PR c++/125886
// { dg-do compile { target c++11 } }

#include <typeinfo>
#include <utility>

// [expr.typeid]/4: a glvalue expression of a polymorphic class type is
// evaluated, so std::declval<B>() is instantiated and
// __declval_protector's static_assert fires (the non-template case:
// the operand is parsed evaluated after the unevaluated probe).

struct B { virtual ~B(); };

void
non_template ()
{
  (void) typeid (std::declval<B> ());
}
// { dg-error "static assertion failed: declval" "" { target *-*-* } 0 }
