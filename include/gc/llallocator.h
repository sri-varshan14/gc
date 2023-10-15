#ifndef _GC_LLALLOCATOR_H_
#define _GC_LLALLOCATOR_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct xheap_node {
  size_t size;
  void *mem_ptr;
  struct xheap_node *next;
} xheap_node;

static xheap_node *used_root = NULL;
static xheap_node *unused_root = NULL;
static int cxheap_free_node = 0;

void *xmalloc(size_t size);
void xfree(void *ptr);

xheap_node *__alloc_xheap_node(size_t size, xheap_node *next);
void __internal_ll_front_insert(xheap_node *root, xheap_node *new_node);

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
    node_mem->mem_ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                             MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (node_mem->mem_ptr == NULL)
      return NULL;
  }
  return node_mem;
}

void *xmalloc(size_t size) {
  void *return_ptr = NULL;
  if (size == 0)
    return NULL;
  xheap_node *mem = __alloc_xheap_node(size, used_root);
  if (mem != NULL)
    used_root = mem;
  return used_root->mem_ptr;
}

void xfree(void *ptr) {
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

void __internal_print() {
  xheap_node *cur = used_root;
  while (cur != NULL) {
    printf("%p ", cur);
    cur = cur->next;
  }
  printf("\n");
}

#endif //_GC_LLALLOCATOR_H_
