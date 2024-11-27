#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef SYSTEM_MALLOC
#define xfree free
#define xmalloc malloc
#define xrealloc realloc
#else
#include "malloc.h"
#endif

int rand_between(int min, int max) { return rand() % (max - min + 1) + min; }

#define TEST_SIZE 30
#define LARGE_MIN 1024
#define LARGE_MAX (1024 * 1024)

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main(int argc, char *argv[]) {
  srand(time(NULL));

  char *test_string =
      "Now is the time for all good people to come to the aid "
      "of their country.";

  if (argc > 1) {
    test_string = argv[1];
  }

  char *ptrs[TEST_SIZE] = {0};

  for (int ix = 0; ix < TEST_SIZE; ix++) {
    int size;
    if (rand() % 5 == 0) {
      size = rand_between(LARGE_MIN, LARGE_MAX);
    } else {
      size = rand_between(1, strlen(test_string) + 1);
    }

    fprintf(stderr, "\n[%d] Allocating size: %d\n", ix, size);
    ptrs[ix] = xmalloc(size);
    if (!ptrs[ix]) {
      fprintf(stderr, "[%d] malloc failed\n", ix);
      exit(1);
    }

    int len_to_copy = MIN(strlen(test_string), size - 1);
    strncpy(ptrs[ix], test_string, len_to_copy);
    ptrs[ix][len_to_copy] = '\0';
    fprintf(stderr, "[%d] Allocated: %p, Content: '%s'\n", ix, ptrs[ix],
            ptrs[ix]);

    if (rand() % 3 == 0) {
      int new_size = rand_between(1, size * 2);
      fprintf(stderr, "[%d] Reallocating %p to new size: %d\n", ix, ptrs[ix],
              new_size);
      char *new_ptr = xrealloc(ptrs[ix], new_size);
      if (new_ptr) {
        ptrs[ix] = new_ptr;
        int new_len_to_copy = MIN(strlen(test_string), new_size - 1);
        strncpy(ptrs[ix], test_string, new_len_to_copy);
        ptrs[ix][new_len_to_copy] = '\0';
        fprintf(stderr, "[%d] Reallocated: %p, Content: '%s'\n", ix, ptrs[ix],
                ptrs[ix]);
      } else {
        fprintf(stderr, "[%d] Realloc failed\n", ix);
      }
    }

    int index_to_free = rand_between(0, ix);
    if (ptrs[index_to_free]) {
      fprintf(stderr, "[%d] Randomly freeing %p ('%s')\n", index_to_free,
              ptrs[index_to_free], ptrs[index_to_free]);
      xfree(ptrs[index_to_free]);
      ptrs[index_to_free] = NULL;
    }
  }

  for (int ix = 0; ix < TEST_SIZE; ix++) {
    if (ptrs[ix]) {
      fprintf(stderr, "[%d] Freeing %p ('%s')\n", ix, ptrs[ix], ptrs[ix]);
      xfree(ptrs[ix]);
      ptrs[ix] = NULL;
    } else {
      fprintf(stderr, "[%d] Already freed\n", ix);
    }
  }

  return 0;
}
