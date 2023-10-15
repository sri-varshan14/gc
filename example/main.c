#include "gc/llallocator.h"
#include <stdio.h>

int main() {
  int n = 10000;
  void *arr[n];
  for (int i = 0; i < n; ++i) {
    arr[i] = xmalloc(4096 * (i + 1));
    __internal_print();
  }
  for (int i = n - 1; i >= 0; --i) {
    xfree(arr[i]);
    __internal_print();
  }
  return 0;
}
