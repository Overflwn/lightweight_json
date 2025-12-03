#include "lightweight_json.h"
#include <cstdint>
#include <gtest/gtest.h>

extern "C" {
static char output[1024] = {0};
static int output_offset = 0;
static int times_flushed = 0;
static char *last_buffer = NULL;
static size_t last_size = 0;
static char buffer[3] = {0};
static lightweight_json_writer_ctx_t ctx = {0};
void flush_cb(char *buffer, size_t buffer_size, void *userdata) {
  times_flushed++;
  last_buffer = buffer;
  last_size = buffer_size;

  memcpy(&output[output_offset], buffer, buffer_size);
  output_offset += buffer_size;
}
}

TEST(HelloTest, BasicAssertions) {
  EXPECT_STRNE("hello", "world");
  EXPECT_EQ(7 * 6, 42);
}

static void setup() {
  memset(&ctx, 0, sizeof(lightweight_json_writer_ctx_t));
  last_buffer = NULL;
  last_size = 0;
  times_flushed = 0;
  memset(output, 0, sizeof(output));
  output_offset = 0;
}

TEST(LightWeightJson, Initialization) {
  setup();

  // Check fails
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_INVALID_ARGS,
            lightweight_json_writer_init(NULL, sizeof(buffer), flush_cb, NULL, &ctx));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_INVALID_ARGS,
            lightweight_json_writer_init(buffer, 1, flush_cb, NULL, &ctx));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_INVALID_ARGS,
            lightweight_json_writer_init(buffer, sizeof(buffer), NULL, NULL, &ctx));
  EXPECT_EQ(
      LIGHTWEIGHT_JSON_ERR_INVALID_ARGS,
      lightweight_json_writer_init(buffer, sizeof(buffer), flush_cb, NULL, NULL));

  // Check success
  EXPECT_EQ(
      LIGHTWEIGHT_JSON_ERR_NONE,
      lightweight_json_writer_init(buffer, sizeof(buffer), flush_cb, NULL, &ctx));
}

TEST(LightWeightJson, Flush) {
  setup();

  EXPECT_EQ(
      LIGHTWEIGHT_JSON_ERR_NONE,
      lightweight_json_writer_init(buffer, sizeof(buffer), flush_cb, NULL, &ctx));

  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_flush(&ctx));
  EXPECT_EQ(last_buffer, &buffer[0]);
  EXPECT_EQ(last_size, 0);
  EXPECT_GT(times_flushed, 0);
}

TEST(LightWeightJson, AddString) {
  setup();
  EXPECT_EQ(
      LIGHTWEIGHT_JSON_ERR_NONE,
      lightweight_json_writer_init(buffer, sizeof(buffer), flush_cb, NULL, &ctx));

  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_begin(&ctx, NULL, LIGHTWEIGHT_JSON_OBJECT));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_add_string(&ctx, "str", "Hello!"));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_end(&ctx));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_flush(&ctx));

  EXPECT_GE(times_flushed, 1);
  EXPECT_EQ(last_buffer, &buffer[0]);
  EXPECT_STREQ(output, "{\"str\":\"Hello!\"}");
}

TEST(LightWeightJson, AddDouble) {
  setup();
  EXPECT_EQ(
      LIGHTWEIGHT_JSON_ERR_NONE,
      lightweight_json_writer_init(buffer, sizeof(buffer), flush_cb, NULL, &ctx));

  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_begin(&ctx, NULL, LIGHTWEIGHT_JSON_OBJECT));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_add_double(&ctx, "double", 123.456));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_end(&ctx));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_flush(&ctx));
  EXPECT_GE(times_flushed, 1);
  EXPECT_EQ(last_buffer, &buffer[0]);
  EXPECT_STREQ(output, "{\"double\":123.45600000}");
}

TEST(LightWeightJson, AddUint64) {
  setup();
  EXPECT_EQ(
      LIGHTWEIGHT_JSON_ERR_NONE,
      lightweight_json_writer_init(buffer, sizeof(buffer), flush_cb, NULL, &ctx));

  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_begin(&ctx, NULL, LIGHTWEIGHT_JSON_ARRAY));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_add_uint64(&ctx, NULL, 123456));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_end(&ctx));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_flush(&ctx));
  EXPECT_GE(times_flushed, 1);
  EXPECT_EQ(last_buffer, &buffer[0]);
  EXPECT_STREQ(output, "[123456]");
}

TEST(LightWeightJson, AddInt64) {
  setup();
  EXPECT_EQ(
      LIGHTWEIGHT_JSON_ERR_NONE,
      lightweight_json_writer_init(buffer, sizeof(buffer), flush_cb, NULL, &ctx));

  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_begin(&ctx, NULL, LIGHTWEIGHT_JSON_OBJECT));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_add_int64(&ctx, "int64", -123456));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_end(&ctx));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_flush(&ctx));
  EXPECT_GE(times_flushed, 1);
  EXPECT_EQ(last_buffer, &buffer[0]);
  EXPECT_STREQ(output, "{\"int64\":-123456}");
}

TEST(LightWeightJson, Nesting) {
  setup();
  EXPECT_EQ(
      LIGHTWEIGHT_JSON_ERR_NONE,
      lightweight_json_writer_init(buffer, sizeof(buffer), flush_cb, NULL, &ctx));

  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_begin(&ctx, NULL, LIGHTWEIGHT_JSON_OBJECT));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_add_int64(&ctx, "int64", -123456));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_begin(&ctx, "arr", LIGHTWEIGHT_JSON_ARRAY));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_add_int64(&ctx, NULL, 1));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_add_int64(&ctx, NULL, 2));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_add_int64(&ctx, NULL, 3));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_end(&ctx));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_end(&ctx));
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_flush(&ctx));
  EXPECT_GE(times_flushed, 1);
  EXPECT_EQ(last_buffer, &buffer[0]);
  EXPECT_STREQ(output, "{\"int64\":-123456,\"arr\":[1,2,3]}");
}

TEST(LightWeightJson, DeepNesting) {
  setup();
  EXPECT_EQ(
      LIGHTWEIGHT_JSON_ERR_NONE,
      lightweight_json_writer_init(buffer, sizeof(buffer), flush_cb, NULL, &ctx));

  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
            lightweight_json_writer_begin(&ctx, NULL, LIGHTWEIGHT_JSON_OBJECT));

  for (int i = 0; i < LIGHTWEIGHT_JSON_MAX_NESTING_SIZE; i++) {
    if (i < LIGHTWEIGHT_JSON_MAX_NESTING_SIZE - 1) {
      EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE,
                lightweight_json_writer_begin(&ctx, "obj", LIGHTWEIGHT_JSON_OBJECT));
    } else {
      EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_MAX_NESTING_REACHED,
                lightweight_json_writer_begin(&ctx, "obj", LIGHTWEIGHT_JSON_OBJECT));
    }
  }
  for (int i = 0; i < LIGHTWEIGHT_JSON_MAX_NESTING_SIZE; i++) {
    EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_end(&ctx));
  }
  EXPECT_EQ(LIGHTWEIGHT_JSON_ERR_NONE, lightweight_json_writer_flush(&ctx));
  EXPECT_GE(times_flushed, 1);
  EXPECT_EQ(last_buffer, &buffer[0]);
  EXPECT_STREQ(output, "{\"obj\":{\"obj\":{\"obj\":{\"obj\":{\"obj\":{\"obj\":{"
                       "\"obj\":{\"obj\":{\"obj\":{}}}}}}}}}}");
}
