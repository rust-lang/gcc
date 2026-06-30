// { dg-do run { target c++20 } }

// Test stream state semantics for chrono operator<<
#include <chrono>
#include <format>
#include <iomanip>
#include <sstream>
#include <testsuite_hooks.h>

bool
check_prefix(std::string_view s, size_t n, char c)
{
  for (char v : s)
    if (v != c)
      return !n;
    else if (!n)
      return false;
    else
      --n; 
  return false;
}

void
test_sentry_badbit()
{
  using namespace std::chrono;
  std::ostringstream os;
  os.setstate(std::ios::badbit);
  VERIFY( !os.good() );
  os << sys_time<seconds>{1'700'000'000s};
  VERIFY( os.bad() );
  VERIFY( os.str().empty() );
}

void
test_sentry_tied()
{
  using namespace std::chrono;
  struct testbuf : std::streambuf
  { bool flushed = false; int sync() override { flushed = true; return 0; } };
  testbuf tied_buf; std::ostream tied(&tied_buf); std::ostringstream os;
  os.tie(&tied);
  os << sys_time<seconds>{1'700'000'000s};
  VERIFY( tied_buf.flushed == true );
}

void
test_width_fill()
{
  using namespace std::chrono;
  std::ostringstream os;
  os << std::setw(30) << std::setfill('*') << sys_time<seconds>{1'700'000'000s};
  auto s = os.view();
  VERIFY( s.size() == 30 );
  VERIFY( check_prefix(s, 11, '*') );
  VERIFY( s.substr(11) == "2023-11-14 22:13:20" );
  VERIFY( os.width() == 0 );
}

void
test_width_left()
{
  using namespace std::chrono;
  std::ostringstream os;
  os << std::setw(30) << std::left << std::setfill('.') << sys_time<seconds>{1'700'000'000s};
  auto s = os.view();
  VERIFY( s.size() == 30 );
  VERIFY( s.find("2023-11-14") == 0 );
  VERIFY( s.back() == '.' );
  VERIFY( os.width() == 0 );
}

void
test_width_internal()
{
  using namespace std::chrono;
  std::ostringstream os;
  os << std::setw(30) << std::internal << std::setfill('_') << day(15);
  auto s = os.view();
  VERIFY( s.size() == 30 );
  VERIFY( check_prefix(s, 28, '_') );
  VERIFY( s.substr(28) == "15" );
  VERIFY( os.width() == 0 );
}

void
test_width_reset()
{
  using namespace std::chrono;
  std::ostringstream os;
  os << std::setw(20) << sys_time<seconds>{1'700'000'000s};
  VERIFY( os.width() == 0 );
  os << hh_mm_ss{seconds(45296)};
  VERIFY( os.width() == 0 );
  os << std::setw(15) << day(15);
  VERIFY( os.width() == 0 );
}

void
test_width_zero()
{
  using namespace std::chrono;
  std::ostringstream os;
  os << std::setw(0) << sys_time<seconds>{1'700'000'000s};
  auto s = os.view();
  VERIFY( s.find("2023-11-14") != std::string::npos );
  VERIFY( s.size() < 20 );
  VERIFY( os.width() == 0 );
}

void
test_multiple_chrono_types()
{
  using namespace std::chrono;
  std::ostringstream os;
  os << std::setw(25) << std::setfill('-') << sys_time<seconds>{1'700'000'000s};
  VERIFY( check_prefix(os.view(), 6, '-') );
  VERIFY( os.width() == 0 );
  os << std::setw(15) << hh_mm_ss{seconds(45296)};
  VERIFY( check_prefix(os.view().substr(25), 7, '-') );
  VERIFY( os.width() == 0 );
  os << std::setw(10) << day(15);
  VERIFY( check_prefix(os.view().substr(40), 8, '-') );
  VERIFY( os.width() == 0 );
  os << std::setw(10) << month(7);
  VERIFY( check_prefix(os.view().substr(50), 7, '-') );
  VERIFY( os.width() == 0 );
  os << std::setw(10) << year(2026);
  VERIFY( check_prefix(os.view().substr(60), 6, '-') );
  VERIFY( os.width() == 0 );
  os << std::setw(12) << Monday;
  VERIFY( check_prefix(os.view().substr(70), 9, '-') );
  VERIFY( os.width() == 0 );
}

void
test_locale_aware()
{
  using namespace std::chrono;
  std::ostringstream os;
  os.imbue(std::locale::classic());
  os << std::setw(12) << Monday;
  auto s = os.view();
  VERIFY( s.size() >= 12 );
  VERIFY( os.width() == 0 );
}

void
test_wchar_t()
{
  using namespace std::chrono;
  std::wostringstream wos;
  wos << std::chrono::sys_time<std::chrono::seconds>{1'700'000'000s};
  VERIFY( wos.view() == L"2023-11-14 22:13:20" );
}

void
test_weekday_indexed()
{
  using namespace std::chrono;
  std::ostringstream os;
  os << Monday[1];
  VERIFY( os.view() == "Mon[1]" );
  os.str("");
  os << Monday[6];  // invalid index
  VERIFY( os.view().find("Mon[") != std::string::npos );
}

int
main()
{
  test_sentry_badbit();
  test_sentry_tied();
  test_width_fill();
  test_width_left();
  test_width_internal();
  test_width_reset();
  test_width_zero();
  test_multiple_chrono_types();
  test_locale_aware();
  test_wchar_t();
  test_weekday_indexed();
}
