#include "malloc.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ALIGNMENT 8

typedef struct heap {
  chunk_on_heap *data;
  int size;
  int capacity;
} heap_t;

heap_t *free_heap = NULL;

size_t align_size(size_t size) {
  return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

void *get_me_blocks(size_t how_much) {
  void *ptr = sbrk(how_much);
  if (ptr == (void *)-1) {
    perror("sbrk failed");
    return NULL;
  }
  return ptr;
}

void init_heap(int capacity) {
  free_heap = (heap_t *)get_me_blocks(sizeof(heap_t));
  free_heap->data =
      (chunk_on_heap *)get_me_blocks(capacity * sizeof(chunk_on_heap));
  free_heap->size = 0;
  free_heap->capacity = capacity;
}

void heap_insert(heap_t *heap, chunk_on_heap chunk) {
  assert(heap->size < heap->capacity);
  heap->data[heap->size] = chunk;
  heap->size++;
}

chunk_on_heap heap_remove_min(heap_t *heap) {
  assert(heap->size > 0);
  chunk_on_heap min_chunk = heap->data[0];
  heap->size--;

  heap->data[0] = heap->data[heap->size];
  return min_chunk;
}

void *xmalloc(size_t size) {
  if (!free_heap) {
    init_heap(100);
  }

  size = align_size(size);

  for (int ix = 0; ix < free_heap->size; ix++) {
    if (free_heap->data[ix].size >= size) {
      chunk_on_heap allocated_chunk = free_heap->data[ix];
      free_heap->data[ix] = free_heap->data[free_heap->size - 1];
      free_heap->size--;
      return allocated_chunk.pointer_to_start;
    }
  }

  size_t total_size = size + sizeof(chunk_on_heap);
  void *ptr = get_me_blocks(total_size);
  if (!ptr) {
    return NULL;
  }

  chunk_on_heap new_chunk;
  new_chunk.size = size;
  new_chunk.pointer_to_start = ptr + sizeof(chunk_on_heap);
  return new_chunk.pointer_to_start;
}

void xfree(void *ptr) {
  if (!ptr) return;

  chunk_on_heap *chunk = (chunk_on_heap *)(ptr - sizeof(chunk_on_heap));

  heap_insert(free_heap, *chunk);
}

void *realloc(void *ptr, size_t size) {
  if (!ptr) return malloc(size);

  chunk_on_heap *chunk = (chunk_on_heap *)(ptr - sizeof(chunk_on_heap));

  if (chunk->size >= size) {
    return ptr;
  }

  void *new_ptr = malloc(size);
  if (new_ptr) {
    memcpy(new_ptr, ptr, chunk->size);
    free(ptr);
  }
  return new_ptr;
}
