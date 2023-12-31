#ifndef _GC_LLALLOCATOR_H_
#define _GC_LLALLOCATOR_H_

#include "gc/gc.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <threads.h>
#include <unistd.h>

typedef struct xheap_node {
  size_t size;
  uintptr_t *mem_ptr;
  struct xheap_node *next;
  bool is_used;
} xheap_node;

static xheap_node *used_root = NULL;
static xheap_node *unused_root = NULL;
static int cxheap_free_node = 0;

uintptr_t *xmalloc(size_t size);
void xfree(uintptr_t *ptr);
void xheap_collect();

xheap_node *__alloc_xheap_node(size_t size, xheap_node *next);
void __internal_print();

xheap_node *__alloc_xheap_node(size_t size, xheap_node *next) {
  xheap_node *node_mem = NULL;
  if (cxheap_free_node > 0) {
    node_mem = unused_root;
    unused_root = unused_root->next;
    cxheap_free_node--;
  } else {
    node_mem =
        (xheap_node *)mmap(NULL, sizeof(xheap_node), PROT_READ | PROT_WRITE,
                           MAP_ANONYMOUS | MAP_SHARED, -1, 0);
  }
  if (node_mem != MAP_FAILED) {
    node_mem->size = size;
    node_mem->next = next;
    node_mem->is_used = true;
    node_mem->mem_ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                             MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (node_mem->mem_ptr == NULL)
      return NULL;
  }
  return node_mem;
}

uintptr_t *xmalloc(size_t size) {
  uintptr_t *return_ptr = NULL;
  if (size == 0)
    return NULL;
  xheap_node *mem = __alloc_xheap_node(size, used_root);
  if (mem != NULL)
    used_root = mem;
  return used_root->mem_ptr;
}

void xfree(uintptr_t *ptr) {
  xheap_node *cur_n = used_root;
  xheap_node *prev_n = NULL;
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
  cxheap_free_node++;
}

void xheap_collect() {
  uintptr_t *start = __builtin_frame_address(0);
  uintptr_t *end = 0;
  end = end + __alignof__(uintptr_t *);
  xheap_node *cur = used_root;
  printf("%p\n", __builtin_frame_address(0));
  printf("%p\n", __builtin_frame_address(1));
  printf("%p\n", __builtin_frame_address(2));
  int count = 1000;
  while (start > end) {
    uintptr_t *p = (uintptr_t *)*start;
    if (used_root->mem_ptr == p) {
      printf("USED ROOT PTR : %p\n", used_root);
      printf("USED ROOT MEM PTR ADDR : %p\n", &used_root->mem_ptr);
      printf("USED ROOT MEM PTR : %p\n", used_root->mem_ptr);
      printf("START : %p\n", start);
      printf("P : %p\n", p);
      printf("FOUND\n");
      int *a = (int *)p;
      printf("Value : %d\n", *a);
      break;
    }
    start = start + __alignof__(uintptr_t *);
  }
  return;
}

void __internal_print() {
  xheap_node *cur = used_root;
  while (cur != NULL) {
    printf("%p ", cur);
    cur = cur->next;
  }
  printf("\n");
}

#endif //_GC_LLALLOCATOR_H_
