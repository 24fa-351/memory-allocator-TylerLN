#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>

typedef struct chunk_on_heap {
  int size;
  char *pointer_to_start;
} chunk_on_heap;

void *xmalloc(size_t size);
void xfree(void *ptr);
void *realloc(void *ptr, size_t size);

#endif
