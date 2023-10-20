#include "gc/llallocator.h"
#include <limits.h>
#include <stdio.h>

int main() {
  int n = 100000;
  void *arr[n];
  for (int i = 0; i < n; ++i) {
    arr[i] = xmalloc(4096 * (i + 1));
  }
  for (int i = n - 1; i >= 0; --i) {
    xfree(arr[i]);
  }
  return 0;
}
