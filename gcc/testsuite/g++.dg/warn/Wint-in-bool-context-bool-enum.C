// PR c++/96496
// { dg-do compile { target c++11 } }
// { dg-options "-Wint-in-bool-context" }
// For static_cast<Flag>(expr), -Wint-in-bool-context applies similarly
// to static_cast<bool>(expr).

enum Flag : bool { kNo, kYes };

Flag f1 (int x) { return static_cast<Flag> (x << 1); } // { dg-warning "in boolean context" }
Flag f2 (int x, int y) { return static_cast<Flag> (x * y); } // { dg-warning "in boolean context" }
Flag f3 (bool c) { return static_cast<Flag> (c ? 2 : 3); } // { dg-warning "in boolean context" }
Flag f4 (int x) { return static_cast<Flag> (x); } // { dg-bogus "in boolean context" }
Flag f5 (int x) { return static_cast<Flag> (x != 0); } // { dg-bogus "in boolean context" }
