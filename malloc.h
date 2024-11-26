#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>

void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);

#define LOGGING_ENABLED 1

#endif
