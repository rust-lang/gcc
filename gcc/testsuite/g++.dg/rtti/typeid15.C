// PR c++/125886
// { dg-do compile { target c++11 } }

#include <typeinfo>
#include <utility>

// [expr.typeid]/4 for a template-dependent operand: the evaluated/
// unevaluated decision is taken at instantiation time in tsubst.  For
// a polymorphic operand the operand is re-substituted evaluated, so
// std::declval<C>() is instantiated and __declval_protector's
// static_assert fires.

struct C { virtual ~C(); };

template <typename T>
void
tmpl ()
{
  (void) typeid (std::declval<T> ());
}

template void tmpl<C> ();
// { dg-error "static assertion failed: declval" "" { target *-*-* } 0 }
