#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>

#define ALIGNMENT 8

size_t align_size(size_t size);

typedef struct {
  size_t size;
  void *pointer_to_start;
} chunk_on_heap;

typedef struct {
  chunk_on_heap *data;
  int size;
  int capacity;
} heap_t;

void init_heap(int capacity);
void *xmalloc(size_t size);
void xfree(void *ptr);
void *xrealloc(void *ptr, size_t size);

#endif
