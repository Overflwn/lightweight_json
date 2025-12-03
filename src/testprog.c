#include "lightweight_json.h"
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 3

static int times_flushed = 0;

void flush_cb(char *buf, size_t len, void *userdata) {
  char temp[BUFFER_SIZE+1] = {0};
  memcpy(temp, buf, len);
  printf("%s", temp);
  times_flushed++;
}

int main(int argc, char **argv) {
  printf("lightweight_json test with BUFFER_SIZE = %d\n", BUFFER_SIZE);
  char buf[BUFFER_SIZE] = {0};
  lightweight_json_writer_ctx_t ctx = {0};
  lightweight_json_err_t err =
      lightweight_json_writer_init(buf, BUFFER_SIZE, flush_cb, NULL, &ctx);
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

  printf("flush_cb was called %d times!\n", times_flushed);

  lightweight_json_reader_ctx_t ctx2;
  const char *testinput = "{\"hello\": \"wo\\\"rld\", \"test\": {\"hello2\": 5, \"array1\": [1, 2, 3, 4, 5], \"array2\": [{\"some\": \"thing\"}, {\"thing\": \"some\"}]}, \"hello2\": 3.54529387, \"hello3\": 69}";
  lightweight_json_reader_init(testinput, strlen(testinput), &ctx2);

  err = lightweight_json_reader_key_exists(&ctx2, "hello");
  err = lightweight_json_reader_key_exists(&ctx2, "hello2");

  char testoutput[64] = {0};
  err = lightweight_json_reader_get_string(&ctx2, "hello", testoutput, sizeof(testoutput));
  uint64_t testval = 0;
  double testval2 = 0.0;
  err = lightweight_json_reader_get_uint64(&ctx2, "hello2", &testval);
  err = lightweight_json_reader_get_double(&ctx2, "hello2", &testval2);
  err = lightweight_json_reader_enter(&ctx2, "test");
  err = lightweight_json_reader_key_exists(&ctx2, "hello2");
  err = lightweight_json_reader_get_uint64(&ctx2, "hello2", &testval);
  // Should not exist
  err = lightweight_json_reader_key_exists(&ctx2, "hello3");
  err = lightweight_json_reader_key_exists(&ctx2, "array1");
  err = lightweight_json_reader_enter(&ctx2, "array1");
  err = lightweight_json_reader_get_uint64(&ctx2, NULL, &testval);
  err = lightweight_json_reader_array_next(&ctx2);
  err = lightweight_json_reader_get_uint64(&ctx2, NULL, &testval);
  err = lightweight_json_reader_array_next(&ctx2);
  err = lightweight_json_reader_get_uint64(&ctx2, NULL, &testval);
  err = lightweight_json_reader_array_next(&ctx2);
  err = lightweight_json_reader_get_uint64(&ctx2, NULL, &testval);
  err = lightweight_json_reader_array_next(&ctx2);
  err = lightweight_json_reader_get_uint64(&ctx2, NULL, &testval);
  // Expect NOT_FOUND here
  err = lightweight_json_reader_array_next(&ctx2);
  // The last value is read again (5)
  err = lightweight_json_reader_get_uint64(&ctx2, NULL, &testval);
  err = lightweight_json_reader_leave(&ctx2);

  // Enter array2, check if iterating through objects works
  err = lightweight_json_reader_enter(&ctx2, "array2");
  err = lightweight_json_reader_enter(&ctx2, NULL);
  err = lightweight_json_reader_key_exists(&ctx2, "some");
  // Should not exist
  err = lightweight_json_reader_key_exists(&ctx2, "thing");
  err = lightweight_json_reader_leave(&ctx2);
  err = lightweight_json_reader_array_next(&ctx2);
  err = lightweight_json_reader_enter(&ctx2, NULL);
  err = lightweight_json_reader_key_exists(&ctx2, "thing");
  // Should not exist
  err = lightweight_json_reader_key_exists(&ctx2, "some");
  err = lightweight_json_reader_leave(&ctx2);
  err = lightweight_json_reader_leave(&ctx2);

  err = lightweight_json_reader_leave(&ctx2);
  // We're back in the main object
  err = lightweight_json_reader_key_exists(&ctx2, "hello3");
  err = lightweight_json_reader_get_uint64(&ctx2, "hello3", &testval);
  return 0;
}
