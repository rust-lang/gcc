// PR c++/96496
// { dg-do compile { target c++11 } }
// { dg-options "-Wconversion" }
// Conversion to an enum with a fixed underlying type is well-defined
// ([expr.static.cast]/8, CWG 2338), so -Wconversion should not warn.

enum Flag : bool { kNo, kYes };
enum C2 : unsigned char { C0 };
enum SC : signed char { SC0 };
enum class SI : short { SI0 };

int f1 () { return (Flag) 6; }		 // { dg-bogus "outside the range" }
int f2 () { return (C2) -1; }		 // { dg-bogus "outside the range" }
int f3 () { return (C2) 257; }		 // { dg-bogus "outside the range" }
int f4 () { return (SC) 128; }		 // { dg-bogus "outside the range" }
int f5 () { return (int) (SI) 65537; }	 // { dg-bogus "outside the range" }
int f6 (double d) { return (Flag) d; }	 // { dg-bogus "conversion" }
int f7 () { return (Flag) 2.5; }	 // { dg-bogus "conversion" }

// The conversion gives the same value as converting to the underlying type.
static_assert (static_cast<C2> (257) == static_cast<unsigned char> (257), "");
static_assert (static_cast<SC> (128) == static_cast<signed char> (128), "");
static_assert (static_cast<SI> (65537)
	       == static_cast<SI> (static_cast<short> (65537)), "");
