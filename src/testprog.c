#include "lightweight_json.h"
#include <stdio.h>

void flush_cb(char *buf, size_t len, void *userdata) { printf("%s", buf); }

int main(int argc, char **argv) {
  char buf[3] = {0};
  lightweight_json_ctx_t ctx =
      lightweight_json_init(buf, sizeof(buf), flush_cb, NULL);

  int ret = lightweight_json_object_begin(&ctx, NULL);
  if (ret != 0) {
    printf("Failed to add object!\n");
    return 1;
  }

  ret = lightweight_json_add_string(&ctx, "string", "Hello, world!");
  if (ret != 0) {
    printf("Failed to add string!\n");
    return 1;
  }

  ret = lightweight_json_add_double(&ctx, "double", 1234.5678);
  if (ret != 0) {
    printf("Failed to add double!\n");
    return 1;
  }
  ret = lightweight_json_add_int64(&ctx, "int64", -123456789);
  if (ret != 0) {
    printf("Failed to add int64!\n");
    return 1;
  }
  ret = lightweight_json_add_uint64(&ctx, "uint64", 123456789);
  if (ret != 0) {
    printf("Failed to add uint64!\n");
    return 1;
  }

  ret = lightweight_json_array_begin(&ctx, "array");
  if (ret != 0) {
    printf("Failed to add array!\n");
    return 1;
  }

  for (uint64_t i = 0; i < 10; i++) {
    ret = lightweight_json_add_uint64(&ctx, NULL, i);
    if (ret != 0) {
      printf("Failed to add value!\n");
      return 1;
    }
  }

  // ret = lightweight_json_object_begin(&ctx, NULL);
  // if (ret != 0) {
  //   printf("Failed to add object!\n");
  //   return 1;
  // }
  // ret = lightweight_json_add_uint64(&ctx, NULL, 123);
  // if (ret != 0) {
  //   printf("Failed to add value!\n");
  //   return 1;
  // }
  // ret = lightweight_json_add_uint64(&ctx, "b", 123);
  // if (ret != 0) {
  //   printf("Failed to add value!\n");
  //   return 1;
  // }
  // ret = lightweight_json_object_end(&ctx);
  // if (ret != 0) {
  //   printf("Failed to end object!\n");
  //   return 1;
  // }

  ret = lightweight_json_array_end(&ctx);
  if (ret != 0) {
    printf("Failed to end array!\n");
    return 1;
  }
  ret = lightweight_json_object_end(&ctx);
  if (ret != 0) {
    printf("Failed to end object!\n");
    return 1;
  }

  ret = lightweight_json_flush(&ctx);
  if (ret != 0) {
    printf("Failed to flush!\n");
    return 1;
  }

  printf("\n");
  return 0;
}
