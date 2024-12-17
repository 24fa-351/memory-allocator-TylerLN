#include "malloc.h"
#include <assert.h>
#include <stdio.h>
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
    free_heap->size     = 0;
    free_heap->capacity = capacity;
}

void heap_insert(heap_t *heap, chunk_on_heap chunk) {
    assert(heap->size < heap->capacity);
    heap->data[heap->size++] = chunk;
}

int find_suitable_chunk(heap_t *heap, size_t size) {
    for (int i = 0; i < heap->size; i++)
        if (heap->data[i].size >= size)
            return i;
    return -1;
}

chunk_on_heap heap_remove(heap_t *heap, int index) {
    assert(index >= 0 && index < heap->size);
    chunk_on_heap chunk = heap->data[index];
    heap->data[index]   = heap->data[--heap->size];
    return chunk;
}