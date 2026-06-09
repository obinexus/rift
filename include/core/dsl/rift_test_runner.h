/**
 * @file rift_test_runner.h
 * @brief Header for the test runner for .rift DSL files
 *
 * This file defines the public API for running tests defined in .rift DSL files
 * against the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#include "core/dsl/rift_dsl_parser.h"
#include "core/dsl/rift_dsl_compiler.h"
#ifndef RIFT_TEST_RUNNER_H
#define RIFT_TEST_RUNNER_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure representing a test result
 */
typedef struct {
	const char *pattern_name;
	const char *test_input;
	bool expected_match;
	bool actual_match;
	bool passed;
	char **expected_groups;
	char **actual_groups;
	size_t group_count;
} rift_test_result_t;

/**
 * @brief Structure representing a collection of test results
 */
typedef struct {
	rift_test_result_t *results;
	size_t count;
	size_t passed_count;
	size_t failed_count;
} rift_test_results_t;

/**
 * @brief Run tests from a .rift DSL file
 * 
 * @param filename Path to the .rift file
 * @return Opaque handle to test results or NULL on error
 */
void *rift_test_run_file(const char *filename);

/**
 * @brief Run tests from a .rift DSL source string
 * 
 * @param source The .rift DSL source code
 * @return Opaque handle to test results or NULL on error
 */
void *rift_test_run_source(const char *source);

/**
 * @brief Free test results
 * 
 * @param handle The test results handle to free
 */
void rift_test_free_results(void *handle);

/**
 * @brief Get error message from failed test run
 * 
 * @param handle The test results handle
 * @return Error message or NULL if no error
 */
const char *rift_test_get_error(void *handle);

/**
 * @brief Get the number of test results
 * 
 * @param handle The test results handle
 * @return Number of test results
 */
size_t rift_test_get_result_count(void *handle);

/**
 * @brief Get a specific test result
 * 
 * @param handle The test results handle
 * @param index Result index
 * @param result Pointer to result structure to fill
 * @return true if successful, false otherwise
 */
bool rift_test_get_result(void *handle, size_t index, rift_test_result_t *result);

/**
 * @brief Get summary of test results
 * 
 * @param handle The test results handle
 * @param total Pointer to receive total test count
 * @param passed Pointer to receive passed test count
 * @param failed Pointer to receive failed test count
 * @return true if successful, false otherwise
 */
bool rift_test_get_summary(void *handle, size_t *total, size_t *passed, size_t *failed);

/**
 * @brief Print test results to stdout
 * 
 * @param handle The test results handle
 * @param verbose If true, print detailed results for each test
 */
void rift_test_print_results(void *handle, bool verbose);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_TEST_RUNNER_H */
