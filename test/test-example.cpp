#include <cstring>  //memcpy
#include <vector>
#include "tqdm/tqdm.h"

int main() {
  static const int N = 1 << 16;

  // pointer example
  int a[N];
  for (int i = 0; i < N; ++i) a[i] = i;
  for (auto i = tqdm::tqdm(a, a + N); i; ++i)
    ;

  // iterator example
  std::vector<int> b(N);
  std::memcpy(b.data(), a, sizeof(int) * N);
  for (auto i = tqdm::tqdm(b.begin(), b.end()); i; i++)
    ;

  // container example
  for (auto i = tqdm::tqdm(b); i; i++)
    ;

  return 0;  // cat(stdin, stdout);
}
