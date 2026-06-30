// { dg-do run { target c++20 } }
// { dg-require-effective-target tzdb }
// { dg-require-effective-target cxx11_abi }
// { dg-xfail-run-if "no weak override on AIX" { powerpc-ibm-aix* } }

// Wall-time rules in the same rule set whose effective firing time
// depends on a prior rule's save (Europe/Paris 1945):
//   1945 Apr 2  02:00 wall  save=2  M
//   1945 Sep 16 03:00 wall  save=0  -
// In the (stdoff=1, save=2) frame the September rule fires at
// Sep 16 00:00 UT, not Sep 16 02:00 UT.

#include <chrono>
#include <fstream>
#include <testsuite_hooks.h>

static bool override_used = false;

namespace __gnu_cxx
{
  const char* zoneinfo_dir_override() {
    override_used = true;
    return "./";
  }
}

using namespace std::chrono;

void
test_positive()
{
  // Line 1 ends at "1945 Sep 16 1u" (Universal time, no save shenanigans),
  // so info.begin for line 2 is exactly 1945-09-16 01:00 UT.
  //
  // Two-line zone whose second line begins at 1945 Sep 16 01:00 UT,
  // between the cascaded firing time (Sep 16 00:00 UT) and the
  // non-cascaded firing time (Sep 16 02:00 UT) of the September rule.
  // The seeding must pick the September rule (save=0, CET) at info.begin.
  std::ofstream("tzdata.zi") << R"(# version test_wall_cascade
R Fr 1945 o - Apr 2  2 2 M
R Fr 1945 o - Sep 16 3 0 -
Z Test/Paris 0  -  X     1945 Sep 16 1u
             1  Fr CE%sT
)";

  const auto& db = reload_tzdb();
  VERIFY( override_used ); // If this fails then XFAIL for the target.
  VERIFY( db.version == "test_wall_cascade" );

  auto* tz = locate_zone("Test/Paris");

  // Line 2 begins at exactly 1945-09-16 01:00 UT.  Sample one second
  // after the boundary, well inside line 2's first sys_info.
  auto info = tz->get_info(sys_seconds{
      sys_days(1945y/September/16) + 1h + 1s});
  VERIFY( info.offset == 1h );
  VERIFY( info.save == 0min );
  VERIFY( info.abbrev == "CET" );

  // The boundary instant itself is in the new line.
  auto at_boundary
    = tz->get_info(sys_seconds{sys_days(1945y/September/16) + 1h});
  VERIFY( at_boundary.offset == 1h );
  VERIFY( at_boundary.save == 0min );

  // Sample later still in line 2 (winter): unchanged.
  auto winter = tz->get_info(sys_days(1945y/December/1));
  VERIFY( winter.offset == 1h );
  VERIFY( winter.save == 0min );
}

void
test_negative()
{
  // This is synthetic version of above example, with negative
  // running save.
  //
  // Two-line zone whose second line begins at 1945 Sep 16 01:00 UT,
  // at the cascaded firing time (Sep 16 01:00 UT), but after
  // non-cascaded firing time (Sep 16 00:00 UT) of the September rule.
  // The seeding must pick the April rule (save=-2, CEST) at info.begin.
  std::ofstream("tzdata.zi") << R"(# version test_negative_cascade
R Fr 1945 o - Apr 2  2 -2 M
R Fr 1945 o - Sep 16 0 0 -
Z Test/Negative 0  -  X     1945 Sep 16 1u
             1  Fr CE%sT
)";

  const auto& db = reload_tzdb();
  VERIFY( override_used ); // If this fails then XFAIL for the target.
  VERIFY( db.version == "test_negative_cascade" );

  auto* tz = locate_zone("Test/Negative");

  // Line 2 begins at exactly 1945-09-16 01:00 UT, sample
  // one second after.
  auto info = tz->get_info(sys_seconds{
      sys_days(1945y/September/16) + 1h + 1s});
  VERIFY( info.offset == -1h );
  VERIFY( info.save == -2h );
  VERIFY( info.abbrev == "CEMT" );

  // The boundary instant.
  auto at_boundary
    = tz->get_info(sys_seconds{sys_days(1945y/September/16) + 1h});
  VERIFY( at_boundary.offset == -1h );
  VERIFY( at_boundary.save == -2h );
}

void
test_next_year()
{
  // The NZ 1946 rule triggers at 1946 Jan 1 00:00:00
  // local time, which correspond to 1946 Dec 13 12:00:00 UT.
  std::ofstream("tzdata.zi") << R"(# version test_next_year
R NZ 1934 1940 - Ap lastSu 2 0 M
R NZ 1934 1940 - S lastSu 2 0:30 S
R NZ 1946 o - Ja 1 0 0 S
Z Pacific/Auckland 11:39:4 - LMT 1868 N 2
11:30 NZ NZ%sT 1946
12 NZ NZ%sT
Z Pacific/AucklandUT 11:39:4 - LMT 1868 N 2
11:30 NZ NZ%sT 1945 Dec 31 13u
12 NZ NZ%sT
)";

  const auto& db = reload_tzdb();
  VERIFY( override_used ); // If this fails then XFAIL for the target.
  VERIFY( db.version == "test_next_year" );

  // Pacific/Auckland requires both PR124854 and PR116110 to work
  // correctly. TODO test it once implemented.
  // The UT version uses 1945-12-31 13:00:00 UT after
  // the rule application change.
  auto* utz = locate_zone("Pacific/AucklandUT");

  // Before the change
  auto before_utboundary
   = utz->get_info(sys_seconds{sys_days(1945y/December/31) + 11h});
  VERIFY( before_utboundary.offset == 12h );
  VERIFY( before_utboundary.save == 30min );
  VERIFY( before_utboundary.abbrev == "NZST" );

  auto at_utboundary
    = utz->get_info(sys_seconds{sys_days(1945y/December/31) + 13h});
  VERIFY( at_utboundary.offset == 12h );
  VERIFY( at_utboundary.save == 0h );
  VERIFY( at_utboundary.abbrev == "NZST" );

  auto after_utboundary
    = utz->get_info(sys_seconds{sys_days(1945y/December/31) + 14h});
  VERIFY( after_utboundary.offset == 12h );
  VERIFY( after_utboundary.save == 0h );
  VERIFY( after_utboundary.abbrev == "NZST" );
}

void
test_prev_year()
{
  // The synthetic version of above, where the local
  // time for rule is moved to previous year.
  // The NZ 1946 rule triggers at 1946 Dec 31 22:00:00
  // local time, which correspond to 1947 Jan 1 10:00:00 UT.
  std::ofstream("tzdata.zi") << R"(# version test_prev_year
R PY 1934 1940 - Ap lastSu 2 0 M
R PY 1934 1940 - S lastSu 2 0:30 S
R PY 1946 o - D 31 22 0 S
Z Test/PrevYear -11:39:4 - LMT 1868 N 2
-12:30 PY PY%sT 1947 Jan 1 11u
-12 PY PY%sT
)";

  const auto& db = reload_tzdb();
  VERIFY( override_used ); // If this fails then XFAIL for the target.
  VERIFY( db.version == "test_prev_year" );

  // The UT version uses 1945-12-31 13:00:00 UT after
  // the rule application change.
  auto* tz = locate_zone("Test/PrevYear");

  // Before the change
  auto before_boundary
   = tz->get_info(sys_seconds{sys_days(1947y/January/1) + 9h});
  VERIFY( before_boundary.offset == -12h );
  VERIFY( before_boundary.save == 30min );
  VERIFY( before_boundary.abbrev == "PYST" );

  auto at_boundary
    = tz->get_info(sys_seconds{sys_days(1947y/January/1) + 11h});
  VERIFY( at_boundary.offset == -12h );
  VERIFY( at_boundary.save == 0h );
  VERIFY( at_boundary.abbrev == "PYST" );

  auto after_boundary
    = tz->get_info(sys_seconds{sys_days(1947y/January/1) + 12h});
  VERIFY( after_boundary.offset == -12h );
  VERIFY( after_boundary.save == 0h );
  VERIFY( after_boundary.abbrev == "PYST" );
}

void
test_earlier_year()
{
  // Synthetic example where PY 1941 rule is still running.
  std::ofstream("tzdata.zi") << R"(# version test_earlier_year
R EY 1934 1941 - Ap lastSu 2 0 M
R EY 1934 1940 - S lastSu 2 0:30 S
Z Test/EarielYear 11:39:4 - LMT 1868 N 2
11:30 EY EY%sT 1943 Jan 1 12u
12 EY EY%sT
   )";

  const auto& db = reload_tzdb();
  VERIFY( override_used ); // If this fails then XFAIL for the target.
  VERIFY( db.version == "test_earlier_year" );

  auto* utz = locate_zone("Test/EarielYear");
  auto at_boundary
    = utz->get_info(sys_seconds{sys_days(1943y/January/1) + 12h});
  VERIFY( at_boundary.offset == 12h );
  VERIFY( at_boundary.save == 0min );
  VERIFY( at_boundary.abbrev == "EYMT" );
}

int
main()
{
  test_positive();
  test_negative();
  test_next_year();
  test_prev_year();
  test_earlier_year();
}
