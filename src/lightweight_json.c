#include "lightweight_json.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

lightweight_json_ctx_t lightweight_json_init(char *buffer, size_t buffer_size,
                                             flush_cb_t flush_cb,
                                             void *userdata) {
  lightweight_json_ctx_t ctx = {
      .buffer = buffer,
      .buffer_size = buffer_size,
      .flush_cb = flush_cb,
      .offset = 0,
      .nesting = -1,
      .objects_in_object = {0},
      .userdata = userdata,
  };
  return ctx;
}

static void check_buffer(lightweight_json_ctx_t *ctx) {
  if (ctx->offset == ctx->buffer_size - 1) {
    ctx->buffer[ctx->offset] = '\0';
    ctx->flush_cb(ctx->buffer, ctx->buffer_size, ctx->userdata);
    ctx->offset = 0;
  }
}

static void add_comma(lightweight_json_ctx_t *ctx) {
  if (ctx->objects_in_object[ctx->nesting] > 0) {
    ctx->buffer[ctx->offset++] = ',';
  }
}

static void add_key(lightweight_json_ctx_t *ctx, const char *const key) {
  if (NULL != key) {
    ctx->buffer[ctx->offset++] = '\"';
    check_buffer(ctx);
    for (int i = 0; i < strlen(key); i++) {
      ctx->buffer[ctx->offset++] = key[i];
      check_buffer(ctx);
    }
    ctx->buffer[ctx->offset++] = '\"';
    check_buffer(ctx);
    ctx->buffer[ctx->offset++] = ':';
    check_buffer(ctx);
  }
}

static void add_str(lightweight_json_ctx_t *ctx, const char *const value) {
  for (int i = 0; i < strlen(value); i++) {
    ctx->buffer[ctx->offset++] = value[i];
    check_buffer(ctx);
  }
}

int lightweight_json_object_begin(lightweight_json_ctx_t *ctx,
                                  const char *const key) {
  if (NULL == ctx) {
    return -1;
  }
  add_comma(ctx);
  add_key(ctx, key);

  ctx->buffer[ctx->offset++] = '{';
  ctx->nesting++;
  ctx->objects_in_object[ctx->nesting] = 0;

  check_buffer(ctx);
  return 0;
}

int lightweight_json_object_end(lightweight_json_ctx_t *ctx) {
  if (NULL == ctx) {
    return -1;
  }
  ctx->buffer[ctx->offset++] = '}';
  ctx->nesting--;
  ctx->objects_in_object[ctx->nesting]++;

  check_buffer(ctx);
  return 0;
}

int lightweight_json_array_begin(lightweight_json_ctx_t *ctx,
                                 const char *const key) {
  if (NULL == ctx) {
    return -1;
  }
  add_comma(ctx);
  add_key(ctx, key);

  ctx->buffer[ctx->offset++] = '{';
  ctx->nesting++;
  ctx->objects_in_object[ctx->nesting] = 0;

  check_buffer(ctx);
  return 0;
}

int lightweight_json_array_end(lightweight_json_ctx_t *ctx) {
  if (NULL == ctx) {
    return -1;
  }
  ctx->buffer[ctx->offset++] = '}';
  ctx->nesting--;
  ctx->objects_in_object[ctx->nesting]++;

  check_buffer(ctx);
  return 0;
}

int lightweight_json_add_string(lightweight_json_ctx_t *ctx,
                                const char *const key,
                                const char *const value) {
  if (NULL == ctx || NULL == value) {
    return -1;
  }
  add_comma(ctx);
  add_key(ctx, key);

  ctx->buffer[ctx->offset++] = '\"';
  check_buffer(ctx);
  add_str(ctx, value);
  ctx->buffer[ctx->offset++] = '\"';
  check_buffer(ctx);

  ctx->objects_in_object[ctx->nesting]++;

  return 0;
}

int lightweight_json_add_double(lightweight_json_ctx_t *ctx,
                                const char *const key, double value) {
  char temp[64] = {0};
  snprintf(temp, sizeof(temp), "%.8lf", value);
  add_comma(ctx);
  add_key(ctx, key);
  add_str(ctx, temp);
  ctx->objects_in_object[ctx->nesting]++;
  return 0;
}

int lightweight_json_add_int64(lightweight_json_ctx_t *ctx,
                               const char *const key, int64_t value) {
  char temp[64] = {0};
  snprintf(temp, sizeof(temp), "%" PRId64, value);
  add_comma(ctx);
  add_key(ctx, key);
  add_str(ctx, temp);
  ctx->objects_in_object[ctx->nesting]++;
  return 0;
}

int lightweight_json_add_uint64(lightweight_json_ctx_t *ctx,
                                const char *const key, uint64_t value) {
  char temp[64] = {0};
  snprintf(temp, sizeof(temp), "%" PRIu64, value);
  add_comma(ctx);
  add_key(ctx, key);
  add_str(ctx, temp);
  ctx->objects_in_object[ctx->nesting]++;
  return 0;
}

int lightweight_json_flush(lightweight_json_ctx_t *ctx) {
  if (NULL == ctx) {
    return -1;
  }

  // terminate string, might be earlier
  ctx->buffer[ctx->offset] = '\0';
  // Force offset to the end so that the check function flushes
  ctx->offset = ctx->buffer_size - 1;
  check_buffer(ctx);
  return 0;
}
