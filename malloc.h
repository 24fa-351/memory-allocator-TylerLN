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

size_t align_size(size_t size);
void *get_me_blocks(size_t mem_size);
void init_heap(int capacity);
void heap_insert(heap_t *heap, chunk_on_heap chunk);
int find_suitable_chunk(heap_t *heap, size_t size);
chunk_on_heap heap_remove(heap_t *heap, int index);

#endif
