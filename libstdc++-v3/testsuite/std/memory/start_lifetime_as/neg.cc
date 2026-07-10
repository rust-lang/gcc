// { dg-do compile { target c++23 } }

#include <bit>
#include <memory>

#include <testsuite_hooks.h>
#include <testsuite_allocator.h>

struct Incomplete;
struct NonTrivial
{
  NonTrivial() {}
  ~NonTrivial() {}
};

void test(void* p)
{
  (void)std::start_lifetime_as<Incomplete>(p);           // { dg-error "here" }
  (void)std::start_lifetime_as_array<Incomplete>(p, 2);  // { dg-error "here" } 
  (void)std::start_lifetime_as<NonTrivial>(p);           // { dg-error "here" }
  // Array are implicit-lifetime regardless of they element type
  (void)std::start_lifetime_as_array<NonTrivial>(p, 2);

  const void* cp = p;
  (void)std::start_lifetime_as<Incomplete>(cp);           // { dg-error "here" }
  (void)std::start_lifetime_as_array<Incomplete>(cp, 2);  // { dg-error "here" } 
  (void)std::start_lifetime_as<NonTrivial>(cp);           // { dg-error "here" }
  (void)std::start_lifetime_as_array<NonTrivial>(cp, 2);

  volatile void* vp = p;
  (void)std::start_lifetime_as<Incomplete>(vp);           // { dg-error "here" }
  (void)std::start_lifetime_as_array<Incomplete>(vp, 2);  // { dg-error "here" } 
  (void)std::start_lifetime_as<NonTrivial>(vp);           // { dg-error "here" }
  (void)std::start_lifetime_as_array<NonTrivial>(vp, 2);

  const volatile void* cvp = vp;
  (void)std::start_lifetime_as<Incomplete>(cvp);           // { dg-error "here" }
  (void)std::start_lifetime_as_array<Incomplete>(cvp, 2);  // { dg-error "here" } 
  (void)std::start_lifetime_as<NonTrivial>(cvp);           // { dg-error "here" }
  (void)std::start_lifetime_as_array<NonTrivial>(cvp, 2);
}

// { dg-prune-output "incomplete type" }
// { dg-prune-output "template argument must be a complete type" }
// { dg-prune-output "static assertion" }
