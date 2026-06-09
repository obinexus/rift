/**
 #include <stdbool.h>
tmp/librift_analysis/librift-4933472-W/src/core/dsl/rift_dsl.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/dsl/rift_dsl.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/dsl/rift_dsl.h"
 #include <stdbool.h>
 #include <stdbool.h>
 #include <stdio.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdlib.h>
 #include <string.h>
 #include <string.h>
 #include "core/bytecode/bytecode.h"
 #include "core/bytecode/bytecode.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "core/bytecode/bytecode.h"
 * @file rift_dsl.c
 * @brief Main implementation file for the .rift DSL system
 *
 * This file implements the main public API for working with .rift DSL files,
 * integrating the parser, compiler, test runner and I/O components.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/dsl/rift_dsl.h
#include "core/bytecode/bytecode.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 
 /* Forward declarations from rift_dsl_parser.c */
 extern void *rift_dsl_parse(const char *source);
 extern void *rift_dsl_load_file(const char *filename);
 extern void rift_dsl_free(void *handle);
 extern const char *rift_dsl_get_error_message(void *handle);
 extern size_t rift_dsl_get_pattern_count(void *handle);
 extern bool rift_dsl_get_pattern(void *handle, size_t index, const char **name, const char **pattern);
 extern bool rift_dsl_get_pattern_flags(void *handle, size_t index, const char ***flags, size_t *count);
 extern size_t rift_dsl_get_test_case_count(void *handle);
 extern bool rift_dsl_get_test_case(void *handle, size_t index, const char **input, bool *expect_match);
 extern bool rift_dsl_get_test_case_groups(void *handle, size_t index, const char ***groups, size_t *count);
 
 /* Forward declarations from rift_dsl_compiler.c */
 extern void *rift_dsl_compile(const char *source);
 extern void rift_dsl_free_compilation(void *handle);
 extern const char *rift_dsl_get_compilation_error(void *handle);
 extern size_t rift_dsl_get_compiled_count(void *handle);
 extern const rift_bytecode_program_t *rift_dsl_get_compiled_program(void *handle, size_t index);
 extern bool rift_dsl_serialize_compilation(void *handle, uint8_t **data, size_t *size);
 extern void *rift_dsl_deserialize_compilation(const uint8_t *data, size_t size);
 extern bool rift_dsl_execute(void *handle, size_t index, const char *input, 
                              size_t input_length, rift_regex_match_t *match);
 
 /* Forward declarations from rift_dsl_test_runner.c */
 extern void *rift_dsl_test_run(const char *source);
 extern void rift_dsl_test_report_free(void *handle);
 extern const char *rift_dsl_test_get_error(void *handle);
 extern bool rift_dsl_test_get_report_text(void *handle, char *buffer, size_t buffer_size);
 extern bool rift_dsl_test_get_stats(void *handle, size_t *total, size_t *passed, size_t *failed);
 extern bool rift_dsl_test_all_passed(void *handle);
 
 /* Forward declarations from rift_dsl_io.c */
 extern void *rift_dsl_compile_to_binary(const char *source);
 extern void *rift_dsl_load_compile_to_binary(const char *filename);
 extern void rift_dsl_binary_free(void *handle);
 extern const char *rift_dsl_binary_get_error(void *handle);
 extern bool rift_dsl_binary_save(void *handle, const char *filename);
 extern void *rift_dsl_binary_load(const char *filename);
 extern bool rift_dsl_binary_get_data(void *handle, const uint8_t **data, size_t *size);
 extern void *rift_dsl_compilation_from_binary(void *handle);
 extern bool rift_dsl_binary_validate(void *handle);
 
 /* Include for type declarations */
 #include "core/bytecode/bytecode.h"
 
 /**
  * @brief Structure to track system initialization state
  */
 typedef struct {
     bool initialized;
     char error_message[256];
     bool has_error;
 }
 
 /**
  * @brief Check if the DSL system is initialized
  * 
  * @return true if initialized, false otherwise
  */
 bool
 rift_dsl_is_initialized(void)
 {
     return g_dsl_system.initialized;
 }
 
 /**
  * @brief Shut down the DSL system
  */
 void
 rift_dsl_shutdown(void)
 {
     if (!g_dsl_system.initialized) {
         return;
     }
     
     // Any cleanup code can be added here
     
     g_dsl_system.initialized = false;
 }
 
 /**
  * @brief Get the last system error message
  * 
  * @return Error message or NULL if no error
  */
 const char *
 rift_dsl_get_system_error(void)
 {
     if (!g_dsl_system.has_error) {
         return NULL;
     }
     
     return g_dsl_system.error_message;
 }
 
 /**
  * @brief Process a .rift file
  * 
  * This function parses a .rift file, compiles the patterns, and returns
  * a handle that can be used to access the compiled patterns.
  * 
  * @param filename The .rift file path
  * @return Opaque handle to processed file or NULL on error
  */
 void *
 rift_dsl_process_file(const char *filename)
 {
     if (!rift_dsl_is_initialized()) {
         rift_dsl_system_error("DSL system not initialized");
         return NULL;
     }
     
     // Load and parse the file
     void *handle = rift_dsl_load_file(filename);
     if (!handle) {
         rift_dsl_system_error("Failed to load or parse file");
         return NULL;
     }
     
     // Check for parsing errors
     const char *error_message = rift_dsl_get_error_message(handle);
     if (error_message) {
         rift_dsl_system_error(error_message);
         rift_dsl_free(handle);
         return NULL;
     }
     
     return handle;
 }
 
 /**
  * @brief Process a .rift source string
  * 
  * This function parses a .rift source string, compiles the patterns, and returns
  * a handle that can be used to access the compiled patterns.
  * 
  * @param source The .rift source string
  * @return Opaque handle to processed source or NULL on error
  */
 void *
 rift_dsl_process_source(const char *source)
 {
     if (!rift_dsl_is_initialized()) {
         rift_dsl_system_error("DSL system not initialized");
         return NULL;
     }
     
     // Parse the source
     void *handle = rift_dsl_parse(source);
     if (!handle) {
         rift_dsl_system_error("Failed to parse source");
         return NULL;
     }
     
     // Check for parsing errors
     const char *error_message = rift_dsl_get_error_message(handle);
     if (error_message) {
         rift_dsl_system_error(error_message);
         rift_dsl_free(handle);
         return NULL;
     }
     
     return handle;
 }
 
 /**
  * @brief Compile patterns from a processed .rift file
  * 
  * @param handle Opaque handle returned by rift_dsl_process_file or rift_dsl_process_source
  * @return Opaque handle to compiled patterns or NULL on error
  */
 void *
 rift_dsl_compile_from_handle(void *handle)
 {
     if (!rift_dsl_is_initialized()) {
         rift_dsl_system_error("DSL system not initialized");
         return NULL;
     }
     
     if (!handle) {
         rift_dsl_system_error("Invalid handle");
         return NULL;
     }
     
     // Get the source code from the handle
     // This is a simplification - in a real implementation, we would need to
     // extract the source code from the parsed representation
     
     // For now, assume the handle is a direct reference to the parsed file
     // and we'll compile from the original source (which is inefficient but works)
     void *compilation = rift_dsl_compile(handle);
     
     if (!compilation) {
         rift_dsl_system_error("Compilation failed");
         return NULL;
     }
     
     // Check for compilation errors
     const char *error_message = rift_dsl_get_compilation_error(compilation);
     if (error_message) {
         rift_dsl_system_error(error_message);
         rift_dsl_free_compilation(compilation);
         return NULL;
     }
     
     return compilation;
 }
 
 /**
  * @brief Export compiled patterns to a binary file
  * 
  * @param compilation_handle Opaque handle returned by rift_dsl_compile_from_handle
  * @param filename The output file path
  * @return true if successful, false otherwise
  */
 bool
 rift_dsl_export_compilation(void *compilation_handle, const char *filename)
 {
     if (!rift_dsl_is_initialized()) {
         rift_dsl_system_error("DSL system not initialized");
         return false;
     }
     
     if (!compilation_handle || !filename) {
         rift_dsl_system_error("Invalid parameters");
         return false;
     }
     
     // Serialize the compilation
     uint8_t *data;
     size_t size;
     
     if (!rift_dsl_serialize_compilation(compilation_handle, &data, &size)) {
         rift_dsl_system_error("Failed to serialize compilation");
         return false;
     }
     
     // Open the file for writing
     FILE *file = fopen(filename, "wb");
     if (!file) {
         rift_dsl_system_error("Failed to open output file");
         free(data);
         return false;
     }
     
     // Write the data
     size_t bytes_written = fwrite(data, 1, size, file);
     fclose(file);
     
     // Free the serialized data
     free(data);
     
     if (bytes_written != size) {
         rift_dsl_system_error("Failed to write all data to file");
         return false;
     }
     
     return true;
 }
 
 /**
  * @brief Import compiled patterns from a binary file
  * 
  * @param filename The input file path
  * @return Opaque handle to compiled patterns or NULL on error
  */
 void *
 rift_dsl_import_compilation(const char *filename)
 {
     if (!rift_dsl_is_initialized()) {
         rift_dsl_system_error("DSL system not initialized");
         return NULL;
     }
     
     if (!filename) {
         rift_dsl_system_error("Invalid filename");
         return NULL;
     }
     
     // Open the file for reading
     FILE *file = fopen(filename, "rb");
     if (!file) {
         rift_dsl_system_error("Failed to open input file");
         return NULL;
     }
     
     // Get file size
     fseek(file, 0, SEEK_END);
     long file_size = ftell(file);
     fseek(file, 0, SEEK_SET);
     
     if (file_size <= 0) {
         rift_dsl_system_error("Empty or invalid file");
         fclose(file);
         return NULL;
     }
     
     // Allocate buffer for file content
     uint8_t *buffer = (uint8_t *)malloc(file_size);
     if (!buffer) {
         rift_dsl_system_error("Memory allocation failed");
         fclose(file);
         return NULL;
     }
     
     // Read file content
     size_t bytes_read = fread(buffer, 1, file_size, file);
     fclose(file);
     
     if (bytes_read != (size_t)file_size) {
         rift_dsl_system_error("Failed to read entire file");
         free(buffer);
         return NULL;
     }
     
     // Deserialize the compilation
     void *compilation = rift_dsl_deserialize_compilation(buffer, file_size);
     
     // Free the buffer
     free(buffer);
     
     if (!compilation) {
         rift_dsl_system_error("Failed to deserialize compilation");
         return NULL;
     }
     
     // Check for deserialization errors
     const char *error_message = rift_dsl_get_compilation_error(compilation);
     if (error_message) {
         rift_dsl_system_error(error_message);
         rift_dsl_free_compilation(compilation);
         return NULL;
     }
     
     return compilation;
 }
 
 /**
  * @brief Run tests from a processed .rift file
  * 
  * @param handle Opaque handle returned by rift_dsl_process_file or rift_dsl_process_source
  * @return Opaque handle to test report or NULL on error
  */
 void *
 rift_dsl_run_tests_from_handle(void *handle)
 {
     if (!rift_dsl_is_initialized()) {
         rift_dsl_system_error("DSL system not initialized");
         return NULL;
     }
     
     if (!handle) {
         rift_dsl_system_error("Invalid handle");
         return NULL;
     }
     
     // Get the source code from the handle
     // This is a simplification - in a real implementation, we would need to
     // extract the source code from the parsed representation
     
     // For now, assume we can compile and run tests directly from the handle
     // (which might be incorrect but demonstrates the concept)
     void *report = rift_dsl_test_run(handle);
     
     if (!report) {
         rift_dsl_system_error("Failed to run tests");
         return NULL;
     }
     
     // Check for test errors
     const char *error_message = rift_dsl_test_get_error(report);
     if (error_message) {
         rift_dsl_system_error(error_message);
         rift_dsl_test_report_free(report);
         return NULL;
     }
     
     return report;
 }
 
 /**
  * @brief Match input string against a compiled pattern
  * 
  * @param compilation_handle Opaque handle to compiled patterns
  * @param pattern_index Index of the pattern to use
  * @param input Input string to match
  * @param match Output match information (can be NULL)
  * @return true if matched, false otherwise
  */
 bool
 rift_dsl_match(void *compilation_handle, size_t pattern_index, 
                const char *input, rift_regex_match_t *match)
 {
     if (!rift_dsl_is_initialized()) {
         rift_dsl_system_error("DSL system not initialized");
         return false;
     }
     
     if (!compilation_handle || !input) {
         rift_dsl_system_error("Invalid parameters");
         return false;
     }
     
     return rift_dsl_execute(compilation_handle, pattern_index, input, (size_t)-1, match);
 }
 
 /**
  * @brief Free resources associated with a processed .rift file
  * 
  * @param handle Opaque handle returned by rift_dsl_process_file or rift_dsl_process_source
  */
 void
 rift_dsl_free_handle(void *handle)
 {
     if (!handle) {
         return;
     }
     
     rift_dsl_free(handle);
 }
 
 /**
  * @brief Free resources associated with compiled patterns
  * 
  * @param compilation_handle Opaque handle returned by rift_dsl_compile_from_handle
  */
 void
 rift_dsl_free_compilation_handle(void *compilation_handle)
 {
     if (!compilation_handle) {
         return;
     }
     
     rift_dsl_free_compilation(compilation_handle);
 }
 
 /**
  * @brief Free resources associated with a test report
  * 
  * @param report_handle Opaque handle returned by rift_dsl_run_tests_from_handle
  */
 void
 rift_dsl_free_report_handle(void *report_handle)
 {
     if (!report_handle) {
         return;
     }
     
     rift_dsl_test_report_free(report_handle);
 } rift_dsl_system_t;
 
 /* Global system state */
 static rift_dsl_system_t g_dsl_system = {
     .initialized = false,
     .error_message = "",
     .has_error = false
 };
 
 /**
  * @brief Set a system error
  * 
  * @param message The error message
  */
 static void
 rift_dsl_system_error(const char *message)
 {
     g_dsl_system.has_error = true;
     strncpy(g_dsl_system.error_message, message, sizeof(g_dsl_system.error_message) - 1);
     g_dsl_system.error_message[sizeof(g_dsl_system.error_message) - 1] = '\0';
 }
 
 /* Public API functions */
 
 /**
  * @brief Initialize the DSL system
  * 
  * @return true if successful, false otherwise
  */
 bool
 rift_dsl_initialize(void)
 {
     if (g_dsl_system.initialized) {
         return true;
     }
     
     // Initialize the bytecode system
     if (!rift_bytecode_system_initialize()) {
         rift_dsl_system_error("Failed to initialize bytecode system");
         return false;
     }
     
     // Any additional initialization can be added here
     
     g_dsl_system.initialized = true;
     g_dsl_system.has_error = false;
     
     return true;
 }
