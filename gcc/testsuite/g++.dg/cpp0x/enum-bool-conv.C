// PR c++/96496
// { dg-do run { target c++11 } }
// Conversion to an enum with a fixed underlying type of bool should convert
// the value to bool first ([expr.static.cast]/8, CWG 2338).

enum Flag : bool { kNo, kYes };
enum class ScopedFlag : bool { kOff, kOn };

struct S {
  constexpr operator Flag () const { return kYes; }
  operator bool() = delete;	// but not using this conversion function
};

template <typename T> struct A
{
  enum E : T { kZero, kOne };
};

static_assert (static_cast<Flag> (2) == kYes, "");
static_assert (static_cast<Flag> (-1) == kYes, "");
static_assert ((Flag) 2 == kYes, "");
static_assert (static_cast<Flag> (true) == kYes, "");
static_assert (static_cast<Flag> (false) == kNo, "");
static_assert (static_cast<Flag> (0) == kNo, "");
static_assert (static_cast<Flag> (1) == kYes, "");
static_assert (static_cast<Flag> (0.5) == kYes, "");
static_assert (static_cast<Flag> (0.0) == kNo, "");
static_assert (static_cast<ScopedFlag> (6) == ScopedFlag::kOn, "");
static_assert (static_cast<Flag> (ScopedFlag::kOn) == kYes, "");
static_assert (static_cast<Flag> (S{}) == kYes, "");
static_assert (static_cast<A<bool>::E> (6) == A<bool>::kOne, "");
static_assert (static_cast<A<char>::E> (6) == 6, "");

// Helpers to keep the operands non-constant and exercise the
// runtime conversion.

Flag
convert (int x)
{
  return static_cast<Flag> (x);
}

Flag
convert (double d)
{
  return static_cast<Flag> (d);
}

Flag
convert_cstyle (int x)
{
  return (Flag) x;
}

Flag
convert_from_bool (bool b)
{
  return static_cast<Flag> (b);
}

ScopedFlag
convert_scoped (int x)
{
  return static_cast<ScopedFlag> (x);
}

A<bool>::E
convert_dep_bool (int x)
{
  return static_cast<A<bool>::E> (x);
}

A<char>::E
convert_dep_char (int x)
{
  return static_cast<A<char>::E> (x);
}

Flag
convert_from_scoped (ScopedFlag f)
{
  return static_cast<Flag> (f);
}

Flag
convert_via_operator (S s)
{
  return static_cast<Flag> (s);
}

int
main ()
{
  if (convert (2) != kYes) __builtin_abort ();
  if (convert (-1) != kYes) __builtin_abort ();
  if (convert (0) != kNo) __builtin_abort ();
  if (convert (1) != kYes) __builtin_abort ();
  if (convert (0.5) != kYes) __builtin_abort ();
  if (convert (0.0) != kNo) __builtin_abort ();
  if (convert_cstyle (2) != kYes) __builtin_abort ();
  if (convert_cstyle (0) != kNo) __builtin_abort ();
  if (convert_from_bool (true) != kYes) __builtin_abort ();
  if (convert_from_bool (false) != kNo) __builtin_abort ();
  if (convert_scoped (6) != ScopedFlag::kOn) __builtin_abort ();
  if (convert_from_scoped (ScopedFlag::kOn) != kYes) __builtin_abort ();
  if (convert_via_operator (S{}) != kYes) __builtin_abort ();
  if (convert_dep_bool (6) != A<bool>::kOne) __builtin_abort ();
  if (convert_dep_char (6) != 6) __builtin_abort ();
}
