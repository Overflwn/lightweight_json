#ifndef LIGHTWEIGHT_JSON_H
#define LIGHTWEIGHT_JSON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

// You may re-define this before including
#ifndef LIGHTWEIGHT_JSON_MAX_NESTING_SIZE
#define LIGHTWEIGHT_JSON_MAX_NESTING_SIZE 10
#endif

typedef enum {
  LIGHTWEIGHT_JSON_OBJECT,
  LIGHTWEIGHT_JSON_ARRAY,
  // Used for safety check
  LIGHTWEIGHT_JSON_NONE
} lightweight_json_type_e;

typedef enum {
  LIGHTWEIGHT_JSON_ERR_NONE,
  LIGHTWEIGHT_JSON_ERR_INVALID_ARGS,
  LIGHTWEIGHT_JSON_ERR_MAX_NESTING_REACHED,
  LIGHTWEIGHT_JSON_ERR_INVALID_STATE
} lightweight_json_err_t;

/**
 * @brief Flush callback called every time the buffer gets full
 *
 * @param[in] buffer The buffer. NOTE: The buffer will not be null terminated! Use `amount` for the string length
 * @param[in] amount The amount of data to flush
 * @param[in] userdata The userdata passed when the context was initialized
 */
typedef void (*flush_cb_t)(char *buffer, size_t amount, void *userdata);

typedef struct {
  char *buffer;
  size_t buffer_size;
  flush_cb_t flush_cb;
  int offset;
  int nesting;
  int objects_in_object[LIGHTWEIGHT_JSON_MAX_NESTING_SIZE];
  lightweight_json_type_e object_type[LIGHTWEIGHT_JSON_MAX_NESTING_SIZE];
  void *userdata;
} lightweight_json_ctx_t;

/**
 * @brief Initialize the given context
 *
 * @param[in] buffer The char buffer to stream to, must be at least size 2
 * @param[in] buffer_size The char buffer size, must be >= 2
 * @param[in] flush_cb The flush callback to use
 * @param[in] userdata [Optional] Userdata that gets passed to the flush
callback
 * @param[in] ctx The context to initialize
 */
lightweight_json_err_t lightweight_json_writer_init(char *buffer, size_t buffer_size,
                                             flush_cb_t flush_cb,
                                             void *userdata,
                                             lightweight_json_ctx_t *ctx);

/**
 * @brief Begin a new object ('{') or array ('[')
 *
 * @param[in] ctx The context
 * @param[in] key [Optional] The key to use
 * @param[in] type The type to begin
 *
 * @return `LIGHTWEIGHT_JSON_ERR_NONE` on success
 */
lightweight_json_err_t lightweight_json_writer_begin(lightweight_json_ctx_t *ctx,
                                              const char *const key,
                                              lightweight_json_type_e type);

/**
 * @brief End the current object or array
 *
 * @param[in] ctx The context
 *
 * @return `LIGHTWEIGHT_JSON_ERR_NONE` on success
 */
lightweight_json_err_t lightweight_json_writer_end(lightweight_json_ctx_t *ctx);

/**
 * @brief Add a string
 *
 * @param[in] ctx The context
 * @param[in] key [Optional] key to use
 * @param[in] value The string to add
 * @return `LIGHTWEIGHT_JSON_ERR_NONE` on success
 */
lightweight_json_err_t lightweight_json_writer_add_string(lightweight_json_ctx_t *ctx,
                                                   const char *const key,
                                                   const char *const value);

/**
 * @brief Add a double
 *
 * @param[in] ctx The context
 * @param[in] key [Optional] key to use
 * @param[in] value The double to add
 * @return `LIGHTWEIGHT_JSON_ERR_NONE` on success
 */
lightweight_json_err_t lightweight_json_writer_add_double(lightweight_json_ctx_t *ctx,
                                                   const char *const key,
                                                   double value);

/**
 * @brief Add a uint64_t
 *
 * @param[in] ctx The context
 * @param[in] key [Optional] key to use
 * @param[in] value The uint64_t to add
 * @return `LIGHTWEIGHT_JSON_ERR_NONE` on success
 */
lightweight_json_err_t lightweight_json_writer_add_uint64(lightweight_json_ctx_t *ctx,
                                                   const char *const key,
                                                   uint64_t value);

/**
 * @brief Add a int64_t
 *
 * @param[in] ctx The context
 * @param[in] key [Optional] key to use
 * @param[in] value The int64_t to add
 * @return `LIGHTWEIGHT_JSON_ERR_NONE` on success
 */
lightweight_json_err_t lightweight_json_writer_add_int64(lightweight_json_ctx_t *ctx,
                                                  const char *const key,
                                                  int64_t value);

/**
 * @brief Flush the buffer manually, used for when example your object is done
 * but the buffer didn't get filled up completely in the end.
 *
 * @param[in] ctx The context
 *
 * @return `LIGHTWEIGHT_JSON_ERR_NONE` on success
 */
lightweight_json_err_t lightweight_json_writer_flush(lightweight_json_ctx_t *ctx);

// --- Helper defines ---
// These expect the context to be a static value called "ctx" in the current
// scope

#define LIGHTWEIGHT_JSON_BEGIN(_key, _type)                                    \
  lightweight_json_writer_begin(&ctx, _key, _type)
#define LIGHTWEIGHT_JSON_END() lightweight_json_writer_end(&ctx)
#define LIGHTWEIGHT_JSON_ADD_STRING(_key, _value)                              \
  lightweight_json_writer_add_string(&ctx, _key, _value)
#define LIGHTWEIGHT_JSON_ADD_DOUBLE(_key, _value)                              \
  lightweight_json_writer_add_double(&ctx, _key, _value)
#define LIGHTWEIGHT_JSON_ADD_UINT64(_key, _value)                              \
  lightweight_json_writer_add_uint64(&ctx, _key, _value)
#define LIGHTWEIGHT_JSON_ADD_INT64(_key, _value)                               \
  lightweight_json_writer_add_int64(&ctx, _key, _value)
#define LIGHTWEIGHT_JSON_FLUSH() lightweight_json_writer_flush(&ctx)

#ifdef __cplusplus
}
#endif
#endif
