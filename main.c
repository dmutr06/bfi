#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "dyn_arr.h"

#define DEFER() goto defer;

typedef enum {
  OP_NEXT = '>',
  OP_PREV = '<',
  OP_INC = '+',
  OP_DEC = '-',
  OP_PRINT = '.',
  OP_SCAN = ',',
  OP_LOOP_START = '[',
  OP_LOOP_END = ']'
} OpKind;

char *get_src(const char *file_path) {
  FILE *src = fopen(file_path, "r");

  if (!src) {
    perror("Failed to open a file");
    return NULL;
  }

  fseek(src, 0, SEEK_END);

  size_t src_len = ftell(src);

  fseek(src, 0, SEEK_SET);

  char *source = malloc(src_len + 1);

  size_t bytes_read = fread(source, sizeof(char), src_len, src);

  if (bytes_read != src_len) {
    fprintf(stderr, "%zu != %zu", bytes_read, src_len);
    free(source);
    fclose(src);
    return NULL;
  }

  source[src_len] = '\0';
  
  fclose(src);
  return source;
}

bool interpret(const char *src) {
  char mem[30000] = {0};
  int pos = 0;
  bool status = false; 
  DynArr(const char *) stack;
  darr_init(&stack, NULL);

  while (*src != '\0') {
    switch (*src) {
      case OP_INC: {
        mem[pos] += 1;
        break;
      }
      case OP_DEC: {
        mem[pos] -= 1;
        break;
      }
      case OP_NEXT: {
        pos += 1;
        break;
      }
      case '<': {
        pos -= 1;
        break;
      }
      case OP_PRINT: {
        putchar(mem[pos]);
        break;
      }
      case OP_SCAN: {
        mem[pos] = getchar();
        break;
      }
      case OP_LOOP_START: {
        if (mem[pos] == 0) {
          int loop_count = 1;
          while (loop_count > 0 && *++src != '\0') {
            if (*src == OP_LOOP_START) loop_count += 1;
            else if (*src == OP_LOOP_END) loop_count -= 1;
          }
          if (loop_count != 0) {
            DEFER();
          }
        } else {
          darr_push(&stack, src); 
        }
        break;
      }
      case OP_LOOP_END: {
        if (stack.size == 0) {
            DEFER();
        }
        if (mem[pos] != 0) {
          src = *darr_last(&stack); 
        } else {
          darr_pop(&stack); 
        }
        break;
      }
    }
    src += 1;
  }

  status = true;

defer:
  darr_deinit(&stack);
  return status;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Provide a file pls\n");
    return 1;
  }
  
  char *src = get_src(argv[1]); 
  
  bool res = interpret(src);

  free(src);

  if (!res) printf("Some err\n");
  return !res;
}
