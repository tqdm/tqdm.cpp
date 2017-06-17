#include "../src/stdafx.h"
#include <cstring>  // memcpy
#include <vector>
#include "tqdm/tqdm.h"

int main() {
  static const size_t N = 1 << 13;

  printf("pointer, pre-increment\n");
  int a[N];
  for (unsigned i = 0; i < N; ++i)
    a[i] = i;
  for (auto i = tqdm::tqdm(a, a + N); !i.ended(); ++i)
    ;

  printf("iterator, total\n");
  std::vector<int> b(N);
  std::memcpy(b.data(), a, sizeof(int) * N);
  for (auto i = tqdm::tqdm(b.begin(), N); !i.ended(); ++i)
    ;

  printf("container, post-increment\n");
  for (auto i = tqdm::tqdm(b); !i.ended(); i++)
    ;

  printf("range-based container\n");
  for (auto &i : tqdm::tqdm(b))
    // if (i) printf("\r%d", i);
    if (i < 0)
      printf(" \b");
  {
  tqdm::Params p;
  p.desc = "range-based array";
  p.miniters = 1;
  p.mininterval = 0;
  p.unit_scale = true;
  p.dynamic_ncols = true;
  // for (auto it = tqdm::tqdm(a); *it < a[N - 1]; ++it)
  for (auto &i : tqdm::tqdm(a, p))
    if (i < 0)
      printf(" \b");
  }
  tqdm::Params p;
  p.desc = "range-based array (unicode)";
  p.ascii = "";
  p.dynamic_ncols = true;
  for (auto &i : tqdm::tqdm(a, p)) {
    if (i < 0)
      printf(" \b");
  }

  printf("iterator-based pythonic range()\n");
  for (auto it = tqdm::range(N); !it.ended(); ++it)
      ;

  printf("ye moste pythonic range(), auto type inference\n");
  float m = 2, n = float(N), s = 2;
  for (auto &i : tqdm::range(m, n, s))
    if (i < 0.0f)
      printf(" \b");

  printf("demonstrate modifiable references\n");
  std::vector<float> foo = {0, 1, 2, 3, 4, 5};
  for (float &i : tqdm::tqdm(foo))
    for (float &j : tqdm::tqdm(foo.begin(), foo.end()))
      i += j;
  for (float &i : foo)
    printf("%.5f ", i);
  printf("\n");

  return 0;
}
