/**
 #include <stdbool.h>
tmp/librift_analysis/librift-4933472-W/src/core/dsl/rift_dsl_compiler.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/dsl/rift_dsl_compiler.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/dsl/rift_dsl_compiler.h"
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
 #include "core/bytecode/bytecode_system.h"
 #include "core/bytecode/bytecode_system.h"
 #include "core/errors/regex_error.h"
 #include "core/errors/regex_error.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "core/bytecode/bytecode.h"
 #include "core/bytecode/bytecode_system.h"
 #include "core/errors/regex_error.h"
 * @file rift_dsl_compiler.c
 * @brief Implementation of the compiler for the .rift DSL
 *
 * This file implements the functionality to compile patterns from .rift DSL files
 * into bytecode for the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/dsl/rift_dsl_compiler.h
#include "core/bytecode/bytecode.h"
#include "core/bytecode/bytecode_system.h"
#include "core/errors/error.h"
#include "core/errors/regex_error.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 
 #include "core/bytecode/bytecode.h"
 #include "core/bytecode/bytecode_system.h"
 #include "core/errors/regex_error.h"
 
 /* Structure to hold compilation results */
 typedef struct {
     rift_bytecode_program_t **programs;
     size_t count;
     size_t capacity;
     char error_message[256];
     bool has_error;
 } rift_dsl_compilation_t;
 
 /* Forward declarations from rift_dsl_parser.c */
 extern void *rift_dsl_parse(const char *source);
 extern void rift_dsl_free(void *handle);
 extern const char *rift_dsl_get_error_message(void *handle);
 extern size_t rift_dsl_get_pattern_count(void *handle);
 extern bool rift_dsl_get_pattern(void *handle, size_t index, const char **name, const char **pattern);
 extern bool rift_dsl_get_pattern_flags(void *handle, size_t index, const char ***flags, size_t *count);
 
 /**
  * @brief Set an error in the compilation structure
  * 
  * @param compilation The compilation structure
  * @param message The error message
  */
 static void
 rift_dsl_compilation_error(rift_dsl_compilation_t *compilation, const char *message)
 {
     if (!compilation) {
         return;
     }
     
     compilation->has_error = true;
     strncpy(compilation->error_message, message, sizeof(compilation->error_message) - 1);
     compilation->error_message[sizeof(compilation->error_message) - 1] = '\0';
 }
 
 /**
  * @brief Create a new compilation structure
  * 
  * @param initial_capacity Initial capacity for compiled programs
  * @return New compilation structure or NULL on failure
  */
 static rift_dsl_compilation_t *
 rift_dsl_compilation_create(size_t initial_capacity)
 {
     rift_dsl_compilation_t *compilation = 
         (rift_dsl_compilation_t *)malloc(sizeof(rift_dsl_compilation_t));
     
     if (!compilation) {
         return NULL;
     }
     
     compilation->programs = 
         (rift_bytecode_program_t **)malloc(initial_capacity * sizeof(rift_bytecode_program_t *));
     
     if (!compilation->programs) {
         free(compilation);
         return NULL;
     }
     
     compilation->count = 0;
     compilation->capacity = initial_capacity;
     compilation->has_error = false;
     compilation->error_message[0] = '\0';
     
     return compilation;
 }
 
 /**
  * @brief Serialize a compilation to binary data
  * 
  * @param compilation The compilation structure
  * @param data Pointer to receive the serialized data
  * @param size Pointer to receive the size of the serialized data
  * @return true if successful, false otherwise
  */
 static bool
 rift_dsl_compilation_serialize(rift_dsl_compilation_t *compilation, 
                                uint8_t **data, size_t *size)
 {
     if (!compilation || !data || !size) {
         return false;
     }
     
     // Calculate total size needed
     size_t total_size = sizeof(uint32_t); // For program count
     size_t *program_sizes = (size_t *)malloc(compilation->count * sizeof(size_t));
     
     if (!program_sizes) {
         return false;
     }
     
     // Calculate size for each program
     for (size_t i = 0; i < compilation->count; i++) {
         size_t program_size = 0;
         if (!rift_bytecode_serialize(compilation->programs[i], NULL, &program_size)) {
             free(program_sizes);
             return false;
         }
         
         program_sizes[i] = program_size;
         total_size += sizeof(size_t) + program_size; // Size + program data
     }
     
     // Allocate memory for serialized data
     uint8_t *buffer = (uint8_t *)malloc(total_size);
     if (!buffer) {
         free(program_sizes);
         return false;
     }
     
     // Write program count
     *((uint32_t *)buffer) = (uint32_t)compilation->count;
     size_t offset = sizeof(uint32_t);
     
     // Write each program
     for (size_t i = 0; i < compilation->count; i++) {
         // Write program size
         *((size_t *)(buffer + offset)) = program_sizes[i];
         offset += sizeof(size_t);
         
         // Write program data
         size_t program_size = program_sizes[i];
         if (!rift_bytecode_serialize(compilation->programs[i], buffer + offset, &program_size)) {
             free(program_sizes);
             free(buffer);
             return false;
         }
         
         offset += program_size;
     }
     
     free(program_sizes);
     
     *data = buffer;
     *size = total_size;
     
     return true;
 }
 
 /**
  * @brief Deserialize a compilation from binary data
  * 
  * @param data The serialized data
  * @param size Size of the serialized data
  * @return Deserialized compilation structure or NULL on error
  */
 static rift_dsl_compilation_t *
 rift_dsl_compilation_deserialize(const uint8_t *data, size_t size)
 {
     if (!data || size < sizeof(uint32_t)) {
         return NULL;
     }
     
     // Read program count
     uint32_t program_count = *((uint32_t *)data);
     size_t offset = sizeof(uint32_t);
     
     // Create compilation structure
     rift_dsl_compilation_t *compilation = rift_dsl_compilation_create(program_count);
     if (!compilation) {
         return NULL;
     }
     
     // Initialize the regex error structure
     rift_regex_error_t regex_error;
     memset(&regex_error, 0, sizeof(regex_error));
     
     // Read each program
     for (uint32_t i = 0; i < program_count; i++) {
         // Check if we have enough data for the size
         if (offset + sizeof(size_t) > size) {
             rift_dsl_compilation_error(compilation, "Serialized data truncated");
             return compilation;
         }
         
         // Read program size
         size_t program_size = *((size_t *)(data + offset));
         offset += sizeof(size_t);
         
         // Check if we have enough data for the program
         if (offset + program_size > size) {
             rift_dsl_compilation_error(compilation, "Serialized data truncated");
             return compilation;
         }
         
         // Deserialize the program
         rift_bytecode_program_t *program = 
             rift_bytecode_deserialize(data + offset, program_size, &regex_error);
             
         if (!program) {
             char message[256];
             snprintf(message, sizeof(message), 
                     "Failed to deserialize program: %s", 
                     regex_error.message[0] ? regex_error.message : "Unknown error");
             rift_dsl_compilation_error(compilation, message);
             return compilation;
         }
         
         offset += program_size;
         
         // Add the program to the compilation
         if (!rift_dsl_compilation_add_program(compilation, program)) {
             rift_dsl_compilation_error(compilation, "Failed to store deserialized program");
             rift_bytecode_program_free(program);
             return compilation;
         }
     }
     
     return compilation;
 }
 
 /* Public API functions */
 
 /**
  * @brief Compile a .rift DSL source to bytecode
  * 
  * @param source The .rift DSL source code
  * @return Opaque handle to compiled bytecode or NULL on error
  */
 void *
 rift_dsl_compile(const char *source)
 {
     if (!source) {
         return NULL;
     }
     
     // Parse the DSL source
     void *dsl_handle = rift_dsl_parse(source);
     if (!dsl_handle) {
         return NULL;
     }
     
     // Compile the patterns
     rift_dsl_compilation_t *compilation = rift_dsl_compile_patterns(dsl_handle);
     
     // Free the DSL handle
     rift_dsl_free(dsl_handle);
     
     return compilation;
 }
 
 /**
  * @brief Free resources associated with compiled bytecode
  * 
  * @param handle Opaque handle returned by rift_dsl_compile
  */
 void
 rift_dsl_free_compilation(void *handle)
 {
     rift_dsl_compilation_free((rift_dsl_compilation_t *)handle);
 }
 
 /**
  * @brief Get error message from compiled bytecode
  * 
  * @param handle Opaque handle returned by rift_dsl_compile
  * @return Error message or NULL if no error
  */
 const char *
 rift_dsl_get_compilation_error(void *handle)
 {
     rift_dsl_compilation_t *compilation = (rift_dsl_compilation_t *)handle;
     if (!compilation || !compilation->has_error) {
         return NULL;
     }
     
     return compilation->error_message;
 }
 
 /**
  * @brief Get number of compiled patterns
  * 
  * @param handle Opaque handle returned by rift_dsl_compile
  * @return Number of compiled patterns
  */
 size_t
 rift_dsl_get_compiled_count(void *handle)
 {
     rift_dsl_compilation_t *compilation = (rift_dsl_compilation_t *)handle;
     if (!compilation) {
         return 0;
     }
     
     return compilation->count;
 }
 
 /**
  * @brief Get compiled bytecode program by index
  * 
  * @param handle Opaque handle returned by rift_dsl_compile
  * @param index Index of the program
  * @return Bytecode program or NULL if index is out of bounds
  */
 const rift_bytecode_program_t *
 rift_dsl_get_compiled_program(void *handle, size_t index)
 {
     rift_dsl_compilation_t *compilation = (rift_dsl_compilation_t *)handle;
     if (!compilation || index >= compilation->count) {
         return NULL;
     }
     
     return compilation->programs[index];
 }
 
 /**
  * @brief Serialize compiled bytecode to binary data
  * 
  * @param handle Opaque handle returned by rift_dsl_compile
  * @param data Pointer to receive the serialized data
  * @param size Pointer to receive the size of the serialized data
  * @return true if successful, false otherwise
  */
 bool
 rift_dsl_serialize_compilation(void *handle, uint8_t **data, size_t *size)
 {
     return rift_dsl_compilation_serialize((rift_dsl_compilation_t *)handle, data, size);
 }
 
 /**
  * @brief Deserialize compiled bytecode from binary data
  * 
  * @param data The serialized data
  * @param size Size of the serialized data
  * @return Opaque handle to compiled bytecode or NULL on error
  */
 void *
 rift_dsl_deserialize_compilation(const uint8_t *data, size_t size)
 {
     return rift_dsl_compilation_deserialize(data, size);
 }
 
 /**
  * @brief Execute a compiled pattern on an input string
  * 
  * @param handle Opaque handle returned by rift_dsl_compile
  * @param index Index of the pattern to execute
  * @param input Input string to match against
  * @param input_length Length of the input string or (size_t)-1 to use strlen
  * @param match Pointer to store match information (can be NULL)
  * @return true if pattern matched, false otherwise
  */
 bool
 rift_dsl_execute(void *handle, size_t index, const char *input, 
                  size_t input_length, rift_regex_match_t *match)
 {
     rift_dsl_compilation_t *compilation = (rift_dsl_compilation_t *)handle;
     if (!compilation || index >= compilation->count || !input) {
         return false;
     }
     
     // Get the program
     rift_bytecode_program_t *program = compilation->programs[index];
     
     // Create a VM
     rift_bytecode_vm_t *vm = rift_bytecode_vm_create(program, input, input_length);
     if (!vm) {
         return false;
     }
     
     // Execute the program
     bool result = rift_bytecode_execute(program, vm, match);
     
     // Free the VM
     rift_bytecode_vm_free(vm);
     
     return result;
 }
 
 /**
  * @brief Free resources associated with a compilation structure
  * 
  * @param compilation The compilation structure to free
  */
 static void
 rift_dsl_compilation_free(rift_dsl_compilation_t *compilation)
 {
     if (!compilation) {
         return;
     }
     
     // Free all compiled programs
     for (size_t i = 0; i < compilation->count; i++) {
         rift_bytecode_program_free(compilation->programs[i]);
     }
     
     free(compilation->programs);
     free(compilation);
 }
 
 /**
  * @brief Add a program to the compilation
  * 
  * @param compilation The compilation structure
  * @param program The bytecode program to add
  * @return true if successful, false otherwise
  */
 static bool
 rift_dsl_compilation_add_program(rift_dsl_compilation_t *compilation, 
                                  rift_bytecode_program_t *program)
 {
     if (!compilation || !program) {
         return false;
     }
     
     // Check if we need to grow the array
     if (compilation->count >= compilation->capacity) {
         size_t new_capacity = compilation->capacity * 2;
         rift_bytecode_program_t **new_programs = (rift_bytecode_program_t **)realloc(
             compilation->programs, new_capacity * sizeof(rift_bytecode_program_t *));
         
         if (!new_programs) {
             return false;
         }
         
         compilation->programs = new_programs;
         compilation->capacity = new_capacity;
     }
     
     // Add the program
     compilation->programs[compilation->count++] = program;
     
     return true;
 }
 
 /**
  * @brief Convert flag string to numeric flags
  * 
  * @param flag_name The flag name
  * @return The numeric flag value or 0 if unknown
  */
 static rift_regex_flags_t
 rift_dsl_flag_to_numeric(const char *flag_name)
 {
     if (!flag_name) {
         return 0;
     }
     
     // Define mappings from flag names to numeric values
     struct {
         const char *name;
         rift_regex_flags_t value;
     } flag_mappings[] = {
         { "CASE_INSENSITIVE", 0x00000001 },
         { "MULTILINE", 0x00000002 },
         { "DOTALL", 0x00000004 },
         { "EXTENDED", 0x00000008 },
         { "OPTIMIZE", 0x00000010 },
         { "UTF8", 0x00000020 },
         { "JIT", 0x00000040 },
         { NULL, 0 }
     };
     
     // Look up the flag
     for (int i = 0; flag_mappings[i].name != NULL; i++) {
         if (strcmp(flag_name, flag_mappings[i].name) == 0) {
             return flag_mappings[i].value;
         }
     }
     
     return 0;
 }
 
 /**
  * @brief Compile patterns from a DSL file
  * 
  * @param dsl_handle The DSL file handle from rift_dsl_parse
  * @return Compilation result structure or NULL on error
  */
 static rift_dsl_compilation_t *
 rift_dsl_compile_patterns(void *dsl_handle)
 {
     if (!dsl_handle) {
         return NULL;
     }
     
     // Check if there was a parsing error
     const char *error_message = rift_dsl_get_error_message(dsl_handle);
     if (error_message) {
         // Create a compilation with the error
         rift_dsl_compilation_t *compilation = rift_dsl_compilation_create(1);
         if (!compilation) {
             return NULL;
         }
         
         rift_dsl_compilation_error(compilation, error_message);
         return compilation;
     }
     
     // Get the number of patterns
     size_t pattern_count = rift_dsl_get_pattern_count(dsl_handle);
     if (pattern_count == 0) {
         // Create an empty compilation
         return rift_dsl_compilation_create(1);
     }
     
     // Create a compilation with enough capacity for all patterns
     rift_dsl_compilation_t *compilation = rift_dsl_compilation_create(pattern_count);
     if (!compilation) {
         return NULL;
     }
     
     // Initialize the regex error structure
     rift_regex_error_t regex_error;
     memset(&regex_error, 0, sizeof(regex_error));
     
     // Compile each pattern
     for (size_t i = 0; i < pattern_count; i++) {
         const char *name;
         const char *pattern;
         
         // Get pattern and name
         if (!rift_dsl_get_pattern(dsl_handle, i, &name, &pattern)) {
             char message[128];
             snprintf(message, sizeof(message), "Failed to get pattern at index %zu", i);
             rift_dsl_compilation_error(compilation, message);
             return compilation;
         }
         
         // Get flags
         const char **flags;
         size_t flag_count;
         rift_regex_flags_t numeric_flags = 0;
         
         if (rift_dsl_get_pattern_flags(dsl_handle, i, &flags, &flag_count)) {
             // Convert string flags to numeric values
             for (size_t j = 0; j < flag_count; j++) {
                 numeric_flags |= rift_dsl_flag_to_numeric(flags[j]);
             }
         }
         
         // Compile the pattern to bytecode
         rift_bytecode_program_t *program = 
             rift_bytecode_compile(pattern, numeric_flags, &regex_error);
         
         if (!program) {
             char message[256];
             snprintf(message, sizeof(message), 
                     "Failed to compile pattern '%s': %s", 
                     name, regex_error.message[0] ? regex_error.message : "Unknown error");
             rift_dsl_compilation_error(compilation, message);
             return compilation;
         }
         
         // Add the program to the compilation
         if (!rift_dsl_compilation_add_program(compilation, program)) {
             char message[128];
             snprintf(message, sizeof(message), 
                     "Failed to store compiled pattern '%s'", name);
             rift_dsl_compilation_error(compilation, message);
             rift_bytecode_program_free(program);
             return compilation;
         }
     }
     
     return compilation;
 }
