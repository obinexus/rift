/**
 * @file rift_dsl.h
 * @brief Main header for the .rift DSL system
 *
 * This file declares the public API for working with .rift DSL files,
 * integrating the parser, compiler, test runner and I/O components.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "core/bytecode/bytecode.h"
#include "core/errors/regex_error.h"
#ifndef RIFT_DSL_H
#define RIFT_DSL_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure to track system initialization state
 */
typedef struct {
	bool initialized;
	char error_message[256];
	bool has_error;
} rift_dsl_system_state_t;

/* Functions from rift_dsl_parser.c */
void *rift_dsl_parse(const char *source);
void *rift_dsl_load_file(const char *filename);
void rift_dsl_free(void *handle);
const char *rift_dsl_get_error_message(void *handle);
size_t rift_dsl_get_pattern_count(void *handle);
bool rift_dsl_get_pattern(void *handle, size_t index, const char **name, const char **pattern);
bool rift_dsl_get_pattern_flags(void *handle, size_t index, const char ***flags, size_t *count);
size_t rift_dsl_get_test_case_count(void *handle);
bool rift_dsl_get_test_case(void *handle, size_t index, const char **input, bool *expect_match);
bool rift_dsl_get_test_case_groups(void *handle, size_t index, const char ***groups, size_t *count);

/* Functions from rift_dsl_compiler.c */
void *rift_dsl_compile(const char *source);
void rift_dsl_free_compilation(void *handle);
const char *rift_dsl_get_compilation_error(void *handle);
size_t rift_dsl_get_compiled_count(void *handle);
const rift_bytecode_program_t *rift_dsl_get_compiled_program(void *handle, size_t index);
bool rift_dsl_serialize_compilation(void *handle, uint8_t **data, size_t *size);
void *rift_dsl_deserialize_compilation(const uint8_t *data, size_t size);
bool rift_dsl_execute(void *handle, size_t index, const char *input, 
					   size_t input_length, rift_regex_match_t *match);

/* Functions from rift_dsl_test_runner.c */
void *rift_dsl_test_run(const char *source);
void rift_dsl_test_report_free(void *handle);
const char *rift_dsl_test_get_error(void *handle);
bool rift_dsl_test_get_report_text(void *handle, char *buffer, size_t buffer_size);
bool rift_dsl_test_get_stats(void *handle, size_t *total, size_t *passed, size_t *failed);
bool rift_dsl_test_all_passed(void *handle);

/* Functions from rift_dsl_io.c */
void *rift_dsl_compile_to_binary(const char *source);
void *rift_dsl_load_compile_to_binary(const char *filename);
void rift_dsl_binary_free(void *handle);
const char *rift_dsl_binary_get_error(void *handle);
bool rift_dsl_binary_save(void *handle, const char *filename);
void *rift_dsl_binary_load(const char *filename);
bool rift_dsl_binary_get_data(void *handle, const uint8_t **data, size_t *size);
void *rift_dsl_compilation_from_binary(void *handle);
bool rift_dsl_binary_validate(void *handle);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_DSL_H */
