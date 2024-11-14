#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifndef DYN_ARR_H_
#define DYN_ARR_H_

static const size_t INIT_CAP = 16;

#define DynArr(type...) \
  struct { \
    size_t size; \
    size_t cap; \
    type *items; \
    void (*deinit_item)(type *item); \
  }

#define darr_init(type, deinit) { \
    .cap = INIT_CAP, \
    .size = 0, \
    .items = malloc(INIT_CAP * sizeof(type)), \
    .deinit_item = deinit ? deinit : NULL \
}

#define darr_push(arr, item...) { \
  if ((arr)->size >= (arr)->cap) { \
    (arr)->cap *= 2; \
    (arr)->items = realloc((arr)->items, (arr)->cap * sizeof(*(arr)->items)); \
  } \
  (arr)->items[(arr)->size++] = item; \
} 

#define darr_get(arr, idx...) (((arr)->size > (idx) || (idx) < 0) ? ((arr)->items + (idx)) : NULL)

#define darr_deinit(arr) do { \
  if ((arr)->deinit_item) \
    darr_foreach(arr, item) (arr)->deinit_item(item); \
  free((arr)->items); \
} while (0) \

#define darr_pop(arr) do { \
  if ((arr)->size <= 0) break; \
  (arr)->size -= 1; \
  if ((arr)->deinit_item) \
    (arr)->deinit_item((arr)->items + (arr)->size); \
} while (0)

#define darr_remove(arr, idx...) do { \
  if ((idx) < 0) break; \
  if ((idx) == (arr)->size - 1) { \
    darr_pop(arr); \
    break; \
  } \
  if ((idx) >= (arr)->size) break; \
  if ((arr)->deinit_item) \
    (arr)->deinit_item((arr)->items + (idx)); \
  memmove((arr)->items + (idx), (arr)->items + (idx) + 1, (--(arr)->size - (idx)) * sizeof(*(arr)->items)); \
} while (0); 

#define darr_first(arr) darr_get(arr, 0)

#define darr_last(arr) darr_get(arr, (arr)->size - 1)

#define darr_foreach(arr, item) \
  for (typeof(*((arr)->items)) *item = (arr)->items; item < (arr)->items + (arr)->size; item++)

#endif // DYN_ARR_H_
