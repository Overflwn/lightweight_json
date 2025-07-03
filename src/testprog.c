#include "lightweight_json.h"
#include <stdio.h>

void flush_cb(char *buf, size_t len, void *userdata) { printf("%s", buf); }

int main(int argc, char **argv) {
  char buf[3] = {0};
  lightweight_json_ctx_t ctx = {0};
  lightweight_json_err_t err =
      lightweight_json_init(buf, sizeof(buf), flush_cb, NULL, &ctx);
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("failed to initialize context!\n");
    return 1;
  }

  err = LIGHTWEIGHT_JSON_BEGIN(NULL, LIGHTWEIGHT_JSON_OBJECT);
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("Failed to add object!\n");
    return 1;
  }

  err = LIGHTWEIGHT_JSON_ADD_STRING("string", "Hello, world!");
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("Failed to add string!\n");
    return 1;
  }

  err = LIGHTWEIGHT_JSON_ADD_DOUBLE("double", 1234.5678);
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("Failed to add double!\n");
    return 1;
  }
  err = LIGHTWEIGHT_JSON_ADD_INT64("int64", -123456789);
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("Failed to add int64!\n");
    return 1;
  }
  err = LIGHTWEIGHT_JSON_ADD_UINT64("uint64", 123456789);
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("Failed to add uint64!\n");
    return 1;
  }

  err = LIGHTWEIGHT_JSON_BEGIN("array", LIGHTWEIGHT_JSON_ARRAY);
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("Failed to add array!\n");
    return 1;
  }

  for (uint64_t i = 0; i < 10; i++) {
    err = LIGHTWEIGHT_JSON_ADD_UINT64(NULL, i);
    if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
      printf("Failed to add value!\n");
      return 1;
    }
  }

  err = LIGHTWEIGHT_JSON_BEGIN(NULL, LIGHTWEIGHT_JSON_OBJECT);
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("Failed to add object!\n");
    return 1;
  }

  // NOTE: Obviously we can add objects without a key to an object even though
  // it makes no sense
  err = LIGHTWEIGHT_JSON_ADD_UINT64(NULL, 123);
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("Failed to add value!\n");
    return 1;
  }

  // NOTE: And mix and match too, though again, it makes no sense
  err = LIGHTWEIGHT_JSON_ADD_UINT64("b", 123);
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("Failed to add value!\n");
    return 1;
  }

  err = LIGHTWEIGHT_JSON_END();
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("Failed to end object!\n");
    return 1;
  }

  err = LIGHTWEIGHT_JSON_END();
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("Failed to end array!\n");
    return 1;
  }

  err = LIGHTWEIGHT_JSON_END();
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("Failed to end object!\n");
    return 1;
  }

  err = LIGHTWEIGHT_JSON_FLUSH();
  if (LIGHTWEIGHT_JSON_ERR_NONE != err) {
    printf("Failed to flush!\n");
    return 1;
  }

  printf("\n");
  return 0;
}
