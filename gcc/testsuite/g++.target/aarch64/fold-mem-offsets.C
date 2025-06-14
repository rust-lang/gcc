/* { dg-do compile } */
/* { dg-options "-O2 -ffold-mem-offsets -fdump-rtl-fold_mem_offsets" } */

typedef int a(void *);
a b;

struct e {
  typedef struct d f;
};

template <typename, typename g = e, typename = typename g::f> struct h;

template <typename i, typename g> struct h<i, g, int> {
  i &operator[](unsigned);
};

template <typename i, typename g> i &h<i, g, int>::operator[](unsigned j) {
  i *k = reinterpret_cast<i *>(1);
  return k[j];
}

template <typename i> struct h<i> {
  i &operator[](unsigned j) { return l[j]; }
  h<i, e, int> l;
};

struct m {
  typedef int aa;
};

template <typename ac, ac> struct n : m { static bool ad(ac); };

template <typename ac, ac o> bool n<ac, o>::ad(ac j) {
  return j == o;
}

template <typename ai> class F {
  typedef typename ai::aa aa;

public:
  F(bool);
  void an() {
    bool ba;
    a r;
    aa *p;
    do {
      aa q = *p;
      ba = ai::ad(q);
      if (ba)
        ;
      else {
        int bj = ai::ao(q);
        aq(bj);
      }
      p++;
    } while (r);
  }
  void aq(unsigned);
};

enum bk {};

struct s {
  int bn;
  bk bo;
  int *bv;
};

h<s> bp;

struct t : n<int, 0> {
  static unsigned ao(int);
};

unsigned t::ao(int j) {
  s *c = &bp[j];
  return b(c->bv) ^ c->bo;
}

void fn3() {
  F<t> bs(5);
  bs.an();
}

/* Check for updated memory offsets.  */
/* { dg-final { scan-rtl-dump "Memory offset changed from 4 to 5" "fold_mem_offsets" } } */
