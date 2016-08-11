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

  printf("range-based loop example\n");
  for (auto &i : tqdm::tqdm(b))
    // if (i) printf("\r%d", i);
    if (i < 0) printf(" \b");

  return 0;
}
