#include <cstring>  //memcpy
#include <vector>
#include "tqdm/tqdm.h"

int main() {
  static const size_t N = 1 << 13;

  printf("pointer, pre-increment\n");
  int a[N];
  for (unsigned i = 0; i < N; ++i) a[i] = i;
  // TODO: fix this:
  // for (auto i = tqdm::tqdm(a, a + N); i; ++i)
  for (auto i = tqdm::tqdm(a, a + N); i != i.end(); ++i)
    ;

  printf("iterator, total\n");
  std::vector<int> b(N);
  std::memcpy(b.data(), a, sizeof(int) * N);
  for (auto i = tqdm::tqdm(b.begin(), N); i; ++i)
    ;

  printf("container, post-increment\n");
  for (auto i = tqdm::tqdm(b); i; i++)
    ;

  printf("range-based container\n");
  for (auto &i : tqdm::tqdm(b))
    // if (i) printf("\r%d", i);
    if (i < 0) printf(" \b");

  printf("range-based array\n");
  // for (auto it = tqdm::tqdm(a); *it < a[N - 1]; ++it)
  for (auto &i : tqdm::tqdm(a))
    if (i < 0) printf(" \b");

  printf("iterator-based pythonic range()\n");
  for (auto it = tqdm::range(N); it; ++it)
    ;

  printf("ye moste pythonic range(), auto type inference\n");
  for (auto &i : tqdm::range(float(N)))
    if (i < 0.0f) printf(" \b");

  return 0;
}
