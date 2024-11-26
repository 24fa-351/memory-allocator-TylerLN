#include "malloc.h"

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#define HEAP_SIZE 1024 * 1024

typedef struct {
  size_t size;
  int free;
  void* next;
} Block;

void* heap_start = NULL;
void* heap_end = NULL;

void initialize_heap() {
  if (!heap_start) {
    void* result = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (result == MAP_FAILED) {
      perror("initialize_heap: mmap failed");
      return;
    }
    heap_start = result;
    heap_end = (char*)heap_start + HEAP_SIZE;

    Block* block = (Block*)heap_start;
    block->size = HEAP_SIZE - sizeof(Block);
    block->free = 1;
    block->next = NULL;
  }
}

Block* find_free_block(size_t size) {
  Block* current = (Block*)heap_start;
  while (current && (!current->free || current->size < size)) {
    current = current->next;
  }
  return current;
}

void split_block(Block* block, size_t size) {
  Block* new_block = (Block*)((char*)block + sizeof(Block) + size);
  new_block->size = block->size - size - sizeof(Block);
  new_block->free = 1;
  new_block->next = block->next;

  block->size = size;
  block->free = 0;
  block->next = new_block;
}

void* malloc(size_t size) {
  if (size == 0) return NULL;

  initialize_heap();

  Block* block = find_free_block(size);
  if (block) {
    if (block->size > size + sizeof(Block)) {
      split_block(block, size);
    }
    block->free = 0;
    return (void*)((char*)block + sizeof(Block));
  }

  fprintf(stderr, "Malloc: Out of memory\n");
  return NULL;
}

void free(void* ptr) {
  if (!ptr) return;

  Block* block = (Block*)((char*)ptr - sizeof(Block));
  block->free = 1;

  Block* current = (Block*)heap_start;
  while (current) {
    if (current->free && current->next && ((Block*)current->next)->free) {
      current->size += sizeof(Block) + ((Block*)current->next)->size;
      current->next = ((Block*)current->next)->next;
    } else {
      current = current->next;
    }
  }
}

void* realloc(void* ptr, size_t size) {
  if (!ptr) return malloc(size);
  if (size == 0) {
    free(ptr);
    return NULL;
  }

  Block* block = (Block*)((char*)ptr - sizeof(Block));
  if (block->size >= size) {
    return ptr;
  }

  void* new_ptr = malloc(size);
  if (new_ptr) {
    memcpy(new_ptr, ptr, block->size);
    free(ptr);
  }
  return new_ptr;
}
