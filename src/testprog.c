#include "lightweight_json.h"
#include <stdio.h>

void flush_cb(char *buf, size_t len, void *userdata) { printf("%s", buf); }

int main(int argc, char **argv) {
  char buf[3] = {0};
  lightweight_json_ctx_t ctx =
      lightweight_json_init(buf, sizeof(buf), flush_cb, NULL);

  int ret = LIGHTWEIGHT_JSON_BEGIN(NULL, LIGHTWEIGHT_JSON_OBJECT);
  if (ret != 0) {
    printf("Failed to add object!\n");
    return 1;
  }

  ret = LIGHTWEIGHT_JSON_ADD_STRING("string", "Hello, world!");
  if (ret != 0) {
    printf("Failed to add string!\n");
    return 1;
  }

  ret = LIGHTWEIGHT_JSON_ADD_DOUBLE("double", 1234.5678);
  if (ret != 0) {
    printf("Failed to add double!\n");
    return 1;
  }
  ret = LIGHTWEIGHT_JSON_ADD_INT64("int64", -123456789);
  if (ret != 0) {
    printf("Failed to add int64!\n");
    return 1;
  }
  ret = LIGHTWEIGHT_JSON_ADD_UINT64("uint64", 123456789);
  if (ret != 0) {
    printf("Failed to add uint64!\n");
    return 1;
  }

  ret = LIGHTWEIGHT_JSON_BEGIN("array", LIGHTWEIGHT_JSON_ARRAY);
  if (ret != 0) {
    printf("Failed to add array!\n");
    return 1;
  }

  for (uint64_t i = 0; i < 10; i++) {
    ret = LIGHTWEIGHT_JSON_ADD_UINT64(NULL, i);
    if (ret != 0) {
      printf("Failed to add value!\n");
      return 1;
    }
  }

  ret = LIGHTWEIGHT_JSON_BEGIN(NULL, LIGHTWEIGHT_JSON_OBJECT);
  if (ret != 0) {
    printf("Failed to add object!\n");
    return 1;
  }

  // NOTE: Obviously we can add objects without a key to an object even though
  // it makes no sense
  ret = LIGHTWEIGHT_JSON_ADD_UINT64(NULL, 123);
  if (ret != 0) {
    printf("Failed to add value!\n");
    return 1;
  }

  // NOTE: And mix and match too, though again, it makes no sense
  ret = LIGHTWEIGHT_JSON_ADD_UINT64("b", 123);
  if (ret != 0) {
    printf("Failed to add value!\n");
    return 1;
  }

  ret = LIGHTWEIGHT_JSON_END();
  if (ret != 0) {
    printf("Failed to end object!\n");
    return 1;
  }

  ret = LIGHTWEIGHT_JSON_END();
  if (ret != 0) {
    printf("Failed to end array!\n");
    return 1;
  }

  ret = LIGHTWEIGHT_JSON_END();
  if (ret != 0) {
    printf("Failed to end object!\n");
    return 1;
  }

  ret = LIGHTWEIGHT_JSON_FLUSH();
  if (ret != 0) {
    printf("Failed to flush!\n");
    return 1;
  }

  printf("\n");
  return 0;
}
