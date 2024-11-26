#include "malloc.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static heap_t *free_heap = NULL;

size_t align_size(size_t size) {
  return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

void *get_me_blocks(size_t mem_size) {
  void *ptr = sbrk(mem_size);
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
  heap->data[heap->size++] = chunk;
}

int find_suitable_chunk(heap_t *heap, size_t size) {
  for (int i = 0; i < heap->size; i++) {
    if (heap->data[i].size >= size) {
      return i;
    }
  }
  return -1;
}

chunk_on_heap heap_remove(heap_t *heap, int index) {
  assert(index >= 0 && index < heap->size);
  chunk_on_heap chunk = heap->data[index];
  heap->data[index] = heap->data[--heap->size];
  return chunk;
}

void *xmalloc(size_t size) {
  if (!free_heap) {
    init_heap(100);
  }

  size_t aligned_size = align_size(size);
  size_t aligned_metadata = align_size(sizeof(chunk_on_heap));

  int suitable_index = find_suitable_chunk(free_heap, aligned_size);
  if (suitable_index != -1) {
    chunk_on_heap chunk = heap_remove(free_heap, suitable_index);
    return chunk.pointer_to_start;
  }

  size_t total_size = aligned_metadata + aligned_size;
  void *ptr = get_me_blocks(total_size);
  if (!ptr) {
    return NULL;
  }

  chunk_on_heap *new_chunk = (chunk_on_heap *)ptr;
  new_chunk->size = aligned_size;
  new_chunk->pointer_to_start = (void *)((char *)ptr + aligned_metadata);

  return new_chunk->pointer_to_start;
}

void xfree(void *ptr) {
  if (!ptr) return;

  chunk_on_heap *chunk =
      (chunk_on_heap *)((char *)ptr - align_size(sizeof(chunk_on_heap)));
  heap_insert(free_heap, *chunk);
}

void *xrealloc(void *ptr, size_t size) {
  if (!ptr) return xmalloc(size);

  chunk_on_heap *chunk =
      (chunk_on_heap *)((char *)ptr - align_size(sizeof(chunk_on_heap)));

  if (chunk->size >= size) {
    return ptr;
  }

  void *new_ptr = xmalloc(size);
  if (new_ptr) {
    memcpy(new_ptr, ptr, chunk->size);
    xfree(ptr);
  }

  return new_ptr;
}
