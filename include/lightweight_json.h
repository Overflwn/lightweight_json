#ifndef LIGHTWEIGHT_JSON_H
#define LIGHTWEIGHT_JSON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

#define LIGHTWEIGHT_JSON_MAX_NESTING_SIZE 10
/**
 * @brief Flush callback called every time the buffer gets full
 *
 * @param[in] The buffer
 * @param[in] The amount of data to flush
 * @param[in] The userdata passed when the context was initialized
 */
typedef void (*flush_cb_t)(char *, size_t, void *);

typedef struct {
  char *buffer;
  size_t buffer_size;
  flush_cb_t flush_cb;
  int offset;
  int nesting;
  int objects_in_object[LIGHTWEIGHT_JSON_MAX_NESTING_SIZE];
  void *userdata;
} lightweight_json_ctx_t;

lightweight_json_ctx_t lightweight_json_init(char *buffer, size_t buffer_size,
                                             flush_cb_t flush_cb,
                                             void *userdata);

/**
 * @brief Begin a new object ('{')
 *
 * @param[in] ctx The context
 * @param[in] key [Optional] key to use
 * @return 0 on success
 */
int lightweight_json_object_begin(lightweight_json_ctx_t *ctx,
                                  const char *const key);

/**
 * @brief End the current object ('}')
 *
 * @param[in] ctx The context
 * @return 0 on success
 */
int lightweight_json_object_end(lightweight_json_ctx_t *ctx);

/**
 * @brief Begin a new array ('[')
 *
 * @param[in] ctx The context
 * @param[in] key [Optional] key to use
 * @return 0 on success
 */
int lightweight_json_array_begin(lightweight_json_ctx_t *ctx,
                                 const char *const key);

/**
 * @brief End the current array (']')
 *
 * @param[in] ctx The context
 * @return 0 on success
 */
int lightweight_json_array_end(lightweight_json_ctx_t *ctx);

/**
 * @brief Add a string
 *
 * @param[in] ctx The context
 * @param[in] key [Optional] key to use
 * @param[in] value The string to add
 * @return 0 on success
 */
int lightweight_json_add_string(lightweight_json_ctx_t *ctx,
                                const char *const key, const char *const value);

/**
 * @brief Add a double
 *
 * @param[in] ctx The context
 * @param[in] key [Optional] key to use
 * @param[in] value The double to add
 * @return 0 on success
 */
int lightweight_json_add_double(lightweight_json_ctx_t *ctx,
                                const char *const key, double value);

/**
 * @brief Add a uint64_t
 *
 * @param[in] ctx The context
 * @param[in] key [Optional] key to use
 * @param[in] value The uint64_t to add
 * @return 0 on success
 */
int lightweight_json_add_uint64(lightweight_json_ctx_t *ctx,
                                const char *const key, uint64_t value);

/**
 * @brief Add a int64_t
 *
 * @param[in] ctx The context
 * @param[in] key [Optional] key to use
 * @param[in] value The int64_t to add
 * @return 0 on success
 */
int lightweight_json_add_int64(lightweight_json_ctx_t *ctx,
                               const char *const key, int64_t value);

/**
 * @brief Flush the buffer manually, used for when example your object is done
 * but the buffer didn't get filled up completely in the end.
 *
 * @param[in] ctx The context
 *
 * @return 0 on success
 */
int lightweight_json_flush(lightweight_json_ctx_t *ctx);

#ifdef __cplusplus
}
#endif
#endif
