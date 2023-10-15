#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define UNIMPLEMENTED                                                          \
  do {                                                                         \
    fprintf(stderr, "%s:%d: Need to Implement {{ %s }} \n", __FILE__,          \
            __LINE__, __func__);                                               \
  } while (false);

typedef struct xheap_tnode {
  size_t size;
  void *mem_ptr;
  struct xheap_tnode *next;
} xheap_tnode;

xheap_tnode *alloc_xheap_tnode(size_t size, xheap_tnode *next) {
  xheap_tnode *node_mem =
      (xheap_tnode *)mmap(NULL, sizeof(xheap_tnode), PROT_READ | PROT_WRITE,
                          MAP_ANONYMOUS | MAP_SHARED, -1, 0);
  if (node_mem == MAP_FAILED) {
    perror("Error mmapping the file");
    exit(EXIT_FAILURE);
  }

  node_mem->size = size;
  node_mem->next = next;
  node_mem->mem_ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                           MAP_ANONYMOUS | MAP_SHARED, -1, 0);

  return node_mem;
}

static xheap_tnode *used_root = NULL;
static xheap_tnode *unused_root = NULL;
static int cxheap_free_tnode = 0;

void *xmalloc(size_t size) {
  void *return_ptr = NULL;
  if (size == 0)
    return NULL;
  if (used_root == NULL) {
    xheap_tnode *mem = alloc_xheap_tnode(size, NULL);
    return_ptr = mem->mem_ptr;
    used_root = mem;
  } else {
    xheap_tnode *cur_ptr = used_root;
    while (cur_ptr->next != NULL) {
      cur_ptr = cur_ptr->next;
    }
    xheap_tnode *mem = alloc_xheap_tnode(size, NULL);
    cur_ptr->next = mem;
    return_ptr = mem->mem_ptr;
  }
  xheap_tnode *cur_ptr = used_root;
  return return_ptr;
}

void xfree(void *ptr) {
  xheap_tnode *cur_n = used_root;
  xheap_tnode *prev_n = NULL;
  if (ptr == NULL || cur_n == NULL)
    return;
  do {
    if (cur_n->mem_ptr == ptr)
      break;
    prev_n = cur_n;
    cur_n = cur_n->next;
  } while (cur_n != NULL);
  if (cur_n == NULL)
    return;

  if (prev_n == NULL) {
    used_root = cur_n->next;
  } else {
    prev_n->next = cur_n->next;
  }
  munmap(cur_n->mem_ptr, cur_n->size);
  cur_n->next = unused_root;
  unused_root = cur_n;
  cxheap_free_tnode++;
}

int main() {
  int n = 10;
  void *arr[n];
  for (int i = 0; i < n; ++i) {
    arr[i] = xmalloc(4096 * (i + 1));
  }
  xheap_tnode *cur = used_root;
  while (cur != NULL) {
    printf("%p ", cur->mem_ptr);
    cur = cur->next;
  }
  printf("\n");
  for (int i = 0; i < n; ++i) {
    xfree(arr[i]);
  }
  cur = used_root;
  while (cur != NULL) {
    printf("%p ", cur->mem_ptr);
    cur = cur->next;
  }
  printf("\n");
  return 0;
}
