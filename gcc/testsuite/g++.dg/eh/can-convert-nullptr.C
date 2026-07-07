// { dg-do compile { target c++11 } }

int main() {
  try { throw nullptr; }
  catch (void *) {}
  catch (decltype(nullptr)) {} // { dg-warning "will be caught by earlier handler" }
}
