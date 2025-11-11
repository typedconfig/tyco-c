#ifndef TYCO_C_API_H
#define TYCO_C_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef enum tyco_status {
    TYCO_OK = 0,
    TYCO_ERROR_INVALID_ARGUMENT = 1,
    TYCO_ERROR_PARSING = 2,
    TYCO_ERROR_INTERNAL = 3
} tyco_status;

typedef struct tyco_context tyco_context;

/**
 * Parses a Tyco file from disk.
 *
 * @param path      Absolute or relative path to a .tyco file.
 * @param out_ctx   Receives the resulting context on success (caller owns).
 * @param out_error Receives an error message on failure (call tyco_free_string).
 */
tyco_status tyco_load_file(const char* path, tyco_context** out_ctx, char** out_error);

/**
 * Parses Tyco content from memory.
 *
 * @param source        Raw Tyco document.
 * @param source_name   Optional identifier used in diagnostics.
 * @param out_ctx       Receives resulting context.
 * @param out_error     Receives error message on failure.
 */
tyco_status tyco_load_string(const char* source, const char* source_name, tyco_context** out_ctx, char** out_error);

/**
 * Serializes the context to the canonical JSON representation. Caller must free
 * the returned buffer via tyco_free_string.
 */
tyco_status tyco_context_to_json(const tyco_context* ctx, char** out_json, char** out_error);

/**
 * Releases the context and any owned resources.
 */
void tyco_context_free(tyco_context* ctx);

/**
 * Utility to free buffers returned by the API (error strings, JSON output).
 */
void tyco_free_string(char* str);

typedef struct tyco_json_result {
    tyco_status status;
    char* json;
    char* error;
} tyco_json_result;

/**
 * Convenience helpers that parse the document and return the JSON string directly.
 * Call tyco_json_result_free once you are done with the result.
 */
tyco_json_result tyco_parse_file_json(const char* path);
tyco_json_result tyco_parse_string_json(const char* source, const char* source_name);
void tyco_json_result_free(tyco_json_result* result);

#ifdef __cplusplus
}
#endif

#endif // TYCO_C_API_H
