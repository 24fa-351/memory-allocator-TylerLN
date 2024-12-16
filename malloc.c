#include "malloc.h"

#include "heap.c"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void *xmalloc(size_t size) {
    if (!free_heap)
        init_heap(100);

    size_t aligned_size     = align_size(size);
    size_t aligned_metadata = align_size(sizeof(chunk_on_heap));

    int suitable_index      = find_suitable_chunk(free_heap, aligned_size);
    if (suitable_index != -1) {
        chunk_on_heap chunk = heap_remove(free_heap, suitable_index);
        return chunk.pointer_to_start;
    }

    size_t total_size = aligned_metadata + aligned_size;
    void *ptr         = get_me_blocks(total_size);
    if (!ptr)
        return NULL;

    chunk_on_heap *new_chunk    = (chunk_on_heap *)ptr;
    new_chunk->size             = aligned_size;
    new_chunk->pointer_to_start = (void *)((char *)ptr + aligned_metadata);

    return new_chunk->pointer_to_start;
}

void xfree(void *ptr) {
    if (!ptr)
        return;

    chunk_on_heap *chunk =
        (chunk_on_heap *)((char *)ptr - align_size(sizeof(chunk_on_heap)));
    heap_insert(free_heap, *chunk);
}

void *xrealloc(void *ptr, size_t size) {
    if (!ptr)
        return xmalloc(size);

    chunk_on_heap *chunk =
        (chunk_on_heap *)((char *)ptr - align_size(sizeof(chunk_on_heap)));

    if (chunk->size >= size)
        return ptr;

    void *new_ptr = xmalloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, chunk->size);
        xfree(ptr);
    }

    return new_ptr;
}
