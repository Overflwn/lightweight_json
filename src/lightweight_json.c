#include "lightweight_json.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

lightweight_json_err_t lightweight_json_init(char *buffer, size_t buffer_size,
                                             flush_cb_t flush_cb,
                                             void *userdata,
                                             lightweight_json_ctx_t *ctx) {
  if (NULL == buffer || buffer_size < 2 || NULL == flush_cb || NULL == ctx) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }
  lightweight_json_ctx_t c = {
      .buffer = buffer,
      .buffer_size = buffer_size,
      .flush_cb = flush_cb,
      .offset = 0,
      .nesting = -1,
      .objects_in_object = {0},
      .object_type = {0},
      .userdata = userdata,
  };
  *ctx = c;
  return LIGHTWEIGHT_JSON_ERR_NONE;
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
    check_buffer(ctx);
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

lightweight_json_err_t lightweight_json_begin(lightweight_json_ctx_t *ctx,
                                              const char *const key,
                                              lightweight_json_type_e type) {
  if (NULL == ctx || (uint8_t)LIGHTWEIGHT_JSON_NONE <= (uint8_t)type) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }
  if (ctx->nesting == LIGHTWEIGHT_JSON_MAX_NESTING_SIZE - 1) {
    return LIGHTWEIGHT_JSON_ERR_MAX_NESTING_REACHED;
  }

  add_comma(ctx);
  add_key(ctx, key);

  switch (type) {
  case LIGHTWEIGHT_JSON_OBJECT:
    ctx->buffer[ctx->offset++] = '{';
    break;
  case LIGHTWEIGHT_JSON_ARRAY:
    ctx->buffer[ctx->offset++] = '[';
    break;
  default:
    return LIGHTWEIGHT_JSON_ERR_INVALID_STATE;
  }
  ctx->nesting++;
  ctx->objects_in_object[ctx->nesting] = 0;
  ctx->object_type[ctx->nesting] = type;
  check_buffer(ctx);
  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t lightweight_json_end(lightweight_json_ctx_t *ctx) {
  if (NULL == ctx) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }
  switch (ctx->object_type[ctx->nesting]) {
  case LIGHTWEIGHT_JSON_OBJECT:
    ctx->buffer[ctx->offset++] = '}';
    break;
  case LIGHTWEIGHT_JSON_ARRAY:
    ctx->buffer[ctx->offset++] = ']';
    break;
  default:
    // Shouldn't be possible unless the ctx is broken
    return LIGHTWEIGHT_JSON_ERR_INVALID_STATE;
  }
  ctx->nesting--;
  ctx->objects_in_object[ctx->nesting]++;

  check_buffer(ctx);
  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t lightweight_json_add_string(lightweight_json_ctx_t *ctx,
                                                   const char *const key,
                                                   const char *const value) {
  if (NULL == ctx || NULL == value) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }
  if (ctx->nesting < 0) {
    // The user didn't begin at least a "main" object
    return LIGHTWEIGHT_JSON_ERR_INVALID_STATE;
  }
  add_comma(ctx);
  add_key(ctx, key);

  ctx->buffer[ctx->offset++] = '\"';
  check_buffer(ctx);
  add_str(ctx, value);
  ctx->buffer[ctx->offset++] = '\"';
  check_buffer(ctx);

  ctx->objects_in_object[ctx->nesting]++;

  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t lightweight_json_add_double(lightweight_json_ctx_t *ctx,
                                                   const char *const key,
                                                   double value) {
  if (NULL == ctx) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }
  if (ctx->nesting < 0) {
    // The user didn't begin at least a "main" object
    return LIGHTWEIGHT_JSON_ERR_INVALID_STATE;
  }
  char temp[64] = {0};
  snprintf(temp, sizeof(temp), "%.8lf", value);
  add_comma(ctx);
  add_key(ctx, key);
  add_str(ctx, temp);
  ctx->objects_in_object[ctx->nesting]++;
  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t lightweight_json_add_int64(lightweight_json_ctx_t *ctx,
                                                  const char *const key,
                                                  int64_t value) {
  if (NULL == ctx) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }
  if (ctx->nesting < 0) {
    // The user didn't begin at least a "main" object
    return LIGHTWEIGHT_JSON_ERR_INVALID_STATE;
  }
  char temp[64] = {0};
  snprintf(temp, sizeof(temp), "%" PRId64, value);
  add_comma(ctx);
  add_key(ctx, key);
  add_str(ctx, temp);
  ctx->objects_in_object[ctx->nesting]++;
  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t lightweight_json_add_uint64(lightweight_json_ctx_t *ctx,
                                                   const char *const key,
                                                   uint64_t value) {
  if (NULL == ctx) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }
  if (ctx->nesting < 0) {
    // The user didn't begin at least a "main" object
    return LIGHTWEIGHT_JSON_ERR_INVALID_STATE;
  }
  char temp[64] = {0};
  snprintf(temp, sizeof(temp), "%" PRIu64, value);
  add_comma(ctx);
  add_key(ctx, key);
  add_str(ctx, temp);
  ctx->objects_in_object[ctx->nesting]++;
  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t lightweight_json_flush(lightweight_json_ctx_t *ctx) {
  if (NULL == ctx) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }

  // terminate string, might be earlier
  ctx->buffer[ctx->offset] = '\0';
  // Force offset to the end so that the check function flushes
  ctx->offset = ctx->buffer_size - 1;
  check_buffer(ctx);
  return LIGHTWEIGHT_JSON_ERR_NONE;
}
