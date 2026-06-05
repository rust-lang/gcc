// { dg-do run { target c++14 } }

#include <set>
#include <string>
#include <utility>
#include <functional>
#include <testsuite_hooks.h>

struct Z;

struct X {
  std::string s;
  X(std::string str) : s(str) {}
  X(Z&&);
  X(const Z&);
  friend bool operator<(X const& a, X const& b) { return a.s < b.s; }
  friend bool operator==(X const& a, X const& b) { return a.s == b.s; }
};

using cmp = std::less<void>;

struct Z {
  std::string s;
  mutable int compares = 0;

  Z() = default;
  Z(Z&& z) : s(std::move(z.s)) { z.s.clear(); }
  Z(const Z& z) = default;
  Z& operator=(Z&& z) { s = std::move(z.s); z.s.clear(); return *this; }
  Z& operator=(const Z& z) = default;
  Z(std::string sv) : s(sv) {}
  Z(int n) : s(std::string(n, 'a')) {}
  friend bool operator<(Z const& a, Z const& b) { return a.s < b.s; }
  friend bool operator<(X const& a, Z const& b)
    { return ++b.compares, a.s.substr(0, b.s.size()) < b.s; }
  friend bool operator<(Z const& a, X const& b)
    { return ++a.compares, a.s < b.s.substr(0, a.s.size()); }
};

X::X(Z&& z) : s(std::move(z.s)) { z.s.clear(); }
X::X(const Z& z) : s(z.s) {}

// A heterogeneous key type like Z here that compares equal
// if it matches just the first part of the key is allowed.

template <typename T>
T populate(T a)
{
  const std::string vs[] = { "dec", "ded", "dee", "def", "deg", "deh", "dei" };
  for (auto const& v : vs)
    a.insert(X{v});
  return a;
}

void test_equal_range()
{
  std::multiset<X, cmp> aset{cmp{}};
  aset.insert({X{"abc"}, X{"def"}, X{"ghi"}});

  {
    auto a = populate(aset);
    VERIFY(a.size() == 10);
    Z z{"de"};
    auto it = a.equal_range(z);
    VERIFY(it.first != a.end());
    VERIFY(it.second != a.end());
    VERIFY(*it.first == std::string("dec"));
    VERIFY(*it.second == std::string("ghi"));

    VERIFY(z.compares == 7);
  }
  {
    auto a = populate(aset);
    VERIFY(a.size() == 10);
    Z z{"de"};
    auto const& ca = a;
    auto it = ca.equal_range(z);
    VERIFY(it.first  != ca.end());
    VERIFY(it.second != ca.end());
    VERIFY(*it.first  == std::string("dec"));
    VERIFY(*it.second == std::string("ghi"));

    VERIFY(z.compares == 7);
  }
}

int main()
{
  test_equal_range();
}
