#include "lightweight_json.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

lightweight_json_err_t
lightweight_json_writer_init(char *buffer, size_t buffer_size,
                             flush_cb_t flush_cb, void *userdata,
                             lightweight_json_writer_ctx_t *ctx) {
  if (NULL == buffer || buffer_size < 2 || NULL == flush_cb || NULL == ctx) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }
  lightweight_json_writer_ctx_t c = {
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

static void check_buffer(lightweight_json_writer_ctx_t *ctx, bool force) {
  if (ctx->offset == ctx->buffer_size || force) {
    ctx->flush_cb(ctx->buffer, ctx->offset, ctx->userdata);
    ctx->offset = 0;
  }
}

static void add_comma(lightweight_json_writer_ctx_t *ctx) {
  if (ctx->objects_in_object[ctx->nesting] > 0) {
    ctx->buffer[ctx->offset++] = ',';
    check_buffer(ctx, false);
  }
}

static void add_key(lightweight_json_writer_ctx_t *ctx, const char *const key) {
  if (NULL != key) {
    ctx->buffer[ctx->offset++] = '\"';
    check_buffer(ctx, false);
    for (int i = 0; i < strlen(key); i++) {
      ctx->buffer[ctx->offset++] = key[i];
      check_buffer(ctx, false);
    }
    ctx->buffer[ctx->offset++] = '\"';
    check_buffer(ctx, false);
    ctx->buffer[ctx->offset++] = ':';
    check_buffer(ctx, false);
  }
}

static void add_str(lightweight_json_writer_ctx_t *ctx,
                    const char *const value) {
  for (int i = 0; i < strlen(value); i++) {
    ctx->buffer[ctx->offset++] = value[i];
    check_buffer(ctx, false);
  }
}

lightweight_json_err_t
lightweight_json_writer_begin(lightweight_json_writer_ctx_t *ctx,
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
  check_buffer(ctx, false);
  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t
lightweight_json_writer_end(lightweight_json_writer_ctx_t *ctx) {
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

  check_buffer(ctx, false);
  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t
lightweight_json_writer_add_string(lightweight_json_writer_ctx_t *ctx,
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
  check_buffer(ctx, false);
  add_str(ctx, value);
  ctx->buffer[ctx->offset++] = '\"';
  check_buffer(ctx, false);

  ctx->objects_in_object[ctx->nesting]++;

  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t
lightweight_json_writer_add_double(lightweight_json_writer_ctx_t *ctx,
                                   const char *const key, double value) {
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

lightweight_json_err_t
lightweight_json_writer_add_int64(lightweight_json_writer_ctx_t *ctx,
                                  const char *const key, int64_t value) {
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

lightweight_json_err_t
lightweight_json_writer_add_uint64(lightweight_json_writer_ctx_t *ctx,
                                   const char *const key, uint64_t value) {
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

lightweight_json_err_t
lightweight_json_writer_add_bool(lightweight_json_writer_ctx_t *ctx,
                                 const char *const key, bool value) {
  if (NULL == ctx) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }
  if (ctx->nesting < 0) {
    // The user didn't begin at least a "main" object
    return LIGHTWEIGHT_JSON_ERR_INVALID_STATE;
  }
  char temp[64] = {0};
  snprintf(temp, sizeof(temp), "%s", value ? "true" : "false");
  add_comma(ctx);
  add_key(ctx, key);
  add_str(ctx, temp);
  ctx->objects_in_object[ctx->nesting]++;
  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t
lightweight_json_writer_flush(lightweight_json_writer_ctx_t *ctx) {
  if (NULL == ctx) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }

  check_buffer(ctx, true);
  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t
lightweight_json_reader_init(const char *buffer, size_t buffer_size,
                             lightweight_json_reader_ctx_t *ctx) {
  if (NULL == buffer || 0 == buffer_size || NULL == ctx) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }
  ctx->buffer = buffer;
  ctx->buffer_size = buffer_size;
  memset(ctx->current_offset, 0, sizeof(ctx->current_offset));
  ctx->nesting = 0;
  memset(ctx->object_type, LIGHTWEIGHT_JSON_NONE, sizeof(ctx->object_type));

  // Find first object offset
  bool found = false;
  for (size_t i = 0; i < buffer_size; i++) {
    if (buffer[i] == '{') {
      ctx->object_type[ctx->nesting] = LIGHTWEIGHT_JSON_OBJECT;
      ctx->current_offset[ctx->nesting] = i;
      found = true;
      break;
    } else if (buffer[i] == '[') {
      ctx->object_type[ctx->nesting] = LIGHTWEIGHT_JSON_ARRAY;
      ctx->current_offset[ctx->nesting] = i;
      found = true;
      break;
    }
  }
  if (!found) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }
  return LIGHTWEIGHT_JSON_ERR_NONE;
}

static size_t find_key(lightweight_json_reader_ctx_t *ctx, const char *key) {
  if (NULL == ctx || NULL == key) {
    return 0;
  }

  size_t offset = ctx->current_offset[ctx->nesting] + 1;
  int nesting = 0;
  bool in_string = false;
  bool is_val = false;
  size_t cur_string_offset = 0;
  size_t cur_string_end = 0;
  for (; offset < ctx->buffer_size; offset++) {
    const char c = ctx->buffer[offset];

    switch (c) {
    case '\"':
      if (nesting == 0 && !is_val) {
        if (!in_string) {
          cur_string_offset = offset + 1;
        } else {
          cur_string_end = offset - 1;
          const char *found_key = &ctx->buffer[cur_string_offset];
          char temp[64] = {0};
          memcpy(temp, found_key, cur_string_end - cur_string_offset + 1);

          if (0 == strcmp(temp, key)) {
            return cur_string_offset - 1;
          }
        }
      }
      in_string = !in_string;
      break;
    case ':':
      if (nesting == 0) {
        is_val = true;
      }
      break;
    case ',':
      if (!in_string && nesting == 0) {
        is_val = false;
      }
      break;
    case '[':
    case '{':
      if (!in_string) {
        nesting++;
      }
      break;
    case ']':
    case '}':
      if (!in_string) {
        nesting--;
        if (nesting < 0) {
          // End of current object
          return 0;
        }
      }
      break;
    case '\\':
      if (in_string) {
        // Skip escape sequence
        offset++;
      }
      break;
    default:

      break;
    }
  }

  return 0;
}

lightweight_json_err_t
lightweight_json_reader_key_exists(lightweight_json_reader_ctx_t *ctx,
                                   const char *key) {
  if (NULL == key || NULL == key) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }

  size_t offset = find_key(ctx, key);
  if (0 == offset) {
    return LIGHTWEIGHT_JSON_ERR_NOT_FOUND;
  }

  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t
lightweight_json_reader_get_string(lightweight_json_reader_ctx_t *ctx,
                                   const char *key, char *buffer,
                                   size_t buffer_len) {
  if (NULL == ctx || NULL == buffer || 0 == buffer_len) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }

  size_t offset = ctx->current_offset[ctx->nesting] +
                  ctx->current_suboffset[ctx->nesting] + 1;

  if (NULL != key) {
    offset = find_key(ctx, key);
    if (0 == offset) {
      return LIGHTWEIGHT_JSON_ERR_NOT_FOUND;
    }

    // Skip key and the two "s and colon
    offset += strlen(key) + 3;
  }

  bool in_string = false;
  size_t string_begin = 0;
  size_t string_end = 0;
  for (; offset < ctx->buffer_size; offset++) {
    const char c = ctx->buffer[offset];

    switch (c) {
    case '\"':
      if (!in_string) {
        in_string = true;
        string_begin = offset;
      } else {
        string_end = offset;
        in_string = false;
        const size_t len = string_end - string_begin - 1;
        memcpy(buffer, &ctx->buffer[string_begin + 1],
               string_end - string_begin - 1);
        buffer[len] = '\0';
        return LIGHTWEIGHT_JSON_ERR_NONE;
      }
      break;
    case '\\':
      // Skip escape
      offset++;
      break;
    default:
      if (!in_string && c != ' ' && c != '\r' && c != '\n' && c != '\t' &&
          c != ':') {
        return LIGHTWEIGHT_JSON_ERR_INVALID_DATATYPE;
      }
      break;
    }
  }

  return LIGHTWEIGHT_JSON_ERR_NOT_FOUND;
}

typedef enum {
  NUMERICAL_TYPE_UINT64,
  NUMERICAL_TYPE_INT64,
  NUMERICAL_TYPE_DOUBLE,
} numerical_type_t;

static lightweight_json_err_t get_numerical(lightweight_json_reader_ctx_t *ctx,
                                            const char *key, void *out_value,
                                            numerical_type_t numerical_type) {
  if (NULL == ctx || NULL == out_value) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }

  size_t offset = ctx->current_offset[ctx->nesting] +
                  ctx->current_suboffset[ctx->nesting] + 1;
  if (NULL != key) {
    offset = find_key(ctx, key);
    if (0 == offset) {
      return LIGHTWEIGHT_JSON_ERR_NOT_FOUND;
    }

    // Skip key and the two "s and colon
    offset += strlen(key) + 3;
  }

  bool in_num = false;
  size_t num_begin = 0;
  size_t num_end = 0;
  int decimal_count = 0;
  for (; offset < ctx->buffer_size; offset++) {
    const char c = ctx->buffer[offset];

    switch (c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (!in_num) {
        in_num = true;
        num_begin = offset;
      }
      break;
    case '-':
      if (!in_num) {
        in_num = true;
        num_begin = offset;
      } else {
        return LIGHTWEIGHT_JSON_ERR_INVALID_JSON;
      }
      break;
    case '.':
      if (in_num && decimal_count == 0) {
        decimal_count++;
      } else {
        return LIGHTWEIGHT_JSON_ERR_INVALID_JSON;
      }
      break;
    case ',':
    case ']':
    case '}':
      if (!in_num) {
        return LIGHTWEIGHT_JSON_ERR_INVALID_JSON;
      }
      in_num = false;
      num_end = offset;

      char temp[64] = {0};
      memcpy(temp, &ctx->buffer[num_begin], num_end - num_begin);

      switch (numerical_type) {
      case NUMERICAL_TYPE_UINT64:
        *(uint64_t *)out_value = strtoull(temp, NULL, 0);
        break;
      case NUMERICAL_TYPE_INT64:
        *(int64_t *)out_value = strtoll(temp, NULL, 0);
        break;
      case NUMERICAL_TYPE_DOUBLE:
        *(double *)out_value = strtod(temp, NULL);
        break;
      default:
        return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
      }
      return LIGHTWEIGHT_JSON_ERR_NONE;
      break;
    default:
      if (c != ' ' && c != '\r' && c != '\n' && c != '\t' && c != ':') {
        return LIGHTWEIGHT_JSON_ERR_INVALID_DATATYPE;
      }
      break;
    }
  }

  return LIGHTWEIGHT_JSON_ERR_NOT_FOUND;
}

lightweight_json_err_t
lightweight_json_reader_get_uint64(lightweight_json_reader_ctx_t *ctx,
                                   const char *key, uint64_t *out_value) {
  return get_numerical(ctx, key, (void *)out_value, NUMERICAL_TYPE_UINT64);
}

lightweight_json_err_t
lightweight_json_reader_get_double(lightweight_json_reader_ctx_t *ctx,
                                   const char *key, double *out_value) {
  return get_numerical(ctx, key, (void *)out_value, NUMERICAL_TYPE_DOUBLE);
}

lightweight_json_err_t
lightweight_json_reader_get_int64(lightweight_json_reader_ctx_t *ctx,
                                  const char *key, int64_t *out_value) {
  return get_numerical(ctx, key, (void *)out_value, NUMERICAL_TYPE_INT64);
}

lightweight_json_err_t
lightweight_json_reader_enter(lightweight_json_reader_ctx_t *ctx,
                              const char *key) {
  if (NULL == ctx) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }

  size_t offset = ctx->current_offset[ctx->nesting] +
                  ctx->current_suboffset[ctx->nesting] + 1;
  if (key) {
    offset = find_key(ctx, key);
    if (0 == offset) {
      return LIGHTWEIGHT_JSON_ERR_NOT_FOUND;
    }

    // Skip key, the two "s and the colon
    offset += strlen(key) + 3;
  }

  bool in_string = false;
  for (; offset < ctx->buffer_size; offset++) {
    const char c = ctx->buffer[offset];

    switch (c) {
    case '\"':
      in_string = !in_string;
      break;
    case '\\':
      // Skip escape
      offset++;
      break;
    case '}':
    case ']':
    case ',':
      if (!in_string) {
        return LIGHTWEIGHT_JSON_ERR_NOT_FOUND;
      }
      break;
    case '[':
    case '{':
      if (!in_string) {
        ctx->nesting++;
        ctx->current_offset[ctx->nesting] = offset;
        if (c == '[') {
          ctx->object_type[ctx->nesting] = LIGHTWEIGHT_JSON_ARRAY;
        } else {
          ctx->object_type[ctx->nesting] = LIGHTWEIGHT_JSON_OBJECT;
        }
        ctx->current_suboffset[ctx->nesting] = 0;
        return LIGHTWEIGHT_JSON_ERR_NONE;
      }
      break;
    default:
      if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
        return LIGHTWEIGHT_JSON_ERR_INVALID_DATATYPE;
      }
      break;
    }
  }
  return LIGHTWEIGHT_JSON_ERR_NOT_FOUND;
}
lightweight_json_err_t
lightweight_json_reader_leave(lightweight_json_reader_ctx_t *ctx) {
  if (NULL == ctx) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }

  if (ctx->nesting > 0) {
    ctx->nesting--;
  }
  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t
lightweight_json_reader_array_next(lightweight_json_reader_ctx_t *ctx) {
  if (NULL == ctx) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }

  if (LIGHTWEIGHT_JSON_ARRAY != ctx->object_type[ctx->nesting]) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_STATE;
  }

  size_t offset = ctx->current_offset[ctx->nesting] +
                  ctx->current_suboffset[ctx->nesting] + 1;
  bool in_string = false;
  int nesting = 0;
  for (; offset < ctx->buffer_size; offset++) {
    const char c = ctx->buffer[offset];
    switch (c) {
    case ',':
      if (!in_string && nesting == 0) {
        ctx->current_suboffset[ctx->nesting] =
            offset - ctx->current_offset[ctx->nesting];
        return LIGHTWEIGHT_JSON_ERR_NONE;
      }
      break;
    case '\"':
      in_string = !in_string;
      break;
    case '\\':
      offset++;
      break;
    case '{':
    case '[':
      if (!in_string) {
        nesting++;
      }
      break;
    case ']':
    case '}':
      if (!in_string && nesting > 0) {
        nesting--;
      } else if (!in_string) {
        return LIGHTWEIGHT_JSON_ERR_NOT_FOUND;
      }
      break;
    default:

      break;
    }
  }

  return LIGHTWEIGHT_JSON_ERR_NONE;
}

lightweight_json_err_t
lightweight_json_reader_get_bool(lightweight_json_reader_ctx_t *ctx,
                                 const char *key, bool *out_value) {
  if (NULL == ctx || NULL == out_value) {
    return LIGHTWEIGHT_JSON_ERR_INVALID_ARGS;
  }

  size_t offset = ctx->current_offset[ctx->nesting] +
                  ctx->current_suboffset[ctx->nesting] + 1;

  if (NULL != key) {
    offset = find_key(ctx, key);
    if (0 == offset) {
      return LIGHTWEIGHT_JSON_ERR_NOT_FOUND;
    }

    // Skip key and the two "s and colon
    offset += strlen(key) + 3;
  }

  char temp[16] = {0};
  for (; offset < ctx->buffer_size; offset++) {
    const char c = ctx->buffer[offset];

    switch (c) {
    case 't':
      // Copy "true"
      memcpy(temp, &ctx->buffer[offset], 4);
      break;
    case 'f':
      // Copy "false"
      memcpy(temp, &ctx->buffer[offset], 5);
      break;
    default:
      if (c != ' ' && c != '\r' && c != '\n' && c != '\t' && c != ':') {
        return LIGHTWEIGHT_JSON_ERR_INVALID_DATATYPE;
      }
      break;
    }
  }

  if (0 == strcmp(temp, "true")) {
    *out_value = true;
  } else if (0 == strcmp(temp, "false")) {
    *out_value = false;
  } else {
    return LIGHTWEIGHT_JSON_ERR_INVALID_DATATYPE;
  }

  return LIGHTWEIGHT_JSON_ERR_NOT_FOUND;
}
