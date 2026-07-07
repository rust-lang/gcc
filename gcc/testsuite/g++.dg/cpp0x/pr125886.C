// PR c++/125886
// PR c++/68604
// PR c++/116385
// { dg-do compile { target c++11 } }

#include <typeinfo>
#include <utility>

// PR c++/125886: a non-polymorphic operand is an unevaluated operand
// ([expr.typeid]/5), so std::declval<A>() is not instantiated and its
// static_assert does not fire.
struct A {};
void
non_poly_declval ()
{
  (void) typeid (std::declval<A> ());
}

// PR c++/125886 (template): the evaluated/unevaluated decision is taken
// at instantiation time in tsubst; a non-polymorphic operand is
// unevaluated, so tmpl<A> is well-formed.
template <typename T>
void
tmpl ()
{
  (void) typeid (std::declval<T> ());
}

template void tmpl<A> ();

// PR c++/68604: an id-expression denoting a non-static data member is
// valid in an unevaluated operand (DR613 / N2253, C++11+).
struct C { int i; };
void
nsm ()
{
  (void) typeid (C::i);
}

// PR c++/116385: function parameters are not odr-used in an unevaluated
// typeid operand, so they need not be captured and may appear in local
// classes and default arguments.
void
params (int n)
{
  [&] { (void) typeid (n); };
  struct Local { void g () { (void) typeid (n); } };
  void g (const std::type_info& = typeid (n));
}

// A final-class polymorphic glvalue resolves to a fixed (static) type,
// so typeid is unevaluated: no vtable lookup, no odr-use.
struct F final { virtual ~F (); };
void
final_glvalue (F& f)
{
  (void) typeid (f);
}

// Polymorphic glvalue in lambda without capture-default must be captured,
// since typeid is evaluated.
struct B { virtual ~B (); };
void
lambda_poly_capture (B& b)
{
  [] { (void) typeid (b); };  // { dg-error "not captured" }
}

// typeid itself inside an unevaluated operand: the polymorphic glvalue
// re-parse must not force evaluation here.
void
nested_unevaluated(B &b)
{
  (void) sizeof (typeid (b)); // no odr-use of b expected
}

// lambda variant of nested_unevaluated.
void
nested_unevaluated_lambda(B &b)
{
  [] { (void) sizeof (typeid (b)); }; // OK: no capture required
}
