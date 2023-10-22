#include "gc/llallocator.h"
#include <limits.h>
#include <stdio.h>

int main() {
  int n = 10;
  void *arr[n];
  for (int i = 0; i < n; ++i) {
    arr[i] = xmalloc(sizeof(int) * n);
    int *a = arr[i];
    a[0] = 69;
  }
  xheap_collect();
  return 0;
}
