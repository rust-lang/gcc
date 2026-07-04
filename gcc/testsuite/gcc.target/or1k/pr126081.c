int option_mask32;
void xsocket(int);
enum { OPT_u = 1 << 3 };

void nc_main() {
  int x = option_mask32 & OPT_u ? 2 : 1;
  xsocket(x);
}
