/* { dg-do compile } */


void use(unsigned);
bool f(unsigned x, unsigned y) {
    return x < 1111 + (y <= 2222);
}
void test_f(unsigned x, unsigned y) {
    for (unsigned i = 0; i < 3333; ++i)
        use(f(x++, y++));
}


