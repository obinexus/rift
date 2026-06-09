#ifndef RIFT_RIFTLANG_H
#define RIFT_RIFTLANG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "rift/platform.h"
#include "rift/token.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct riftlang_program RLangProgram;
typedef struct riftlang_context RLangContext;

typedef struct RLangBinding {
    const char *name;
    const char *value;
} RLangBinding;

typedef struct RLangCompileResult {
    bool success;
    char *error_message;
    RLangProgram *program;
} RLangCompileResult;

RIFTLANG_API RLangContext *rlang_context_create(void);
RIFTLANG_API void rlang_context_free(RLangContext *ctx);
RIFTLANG_API RLangCompileResult rlang_compile(RLangContext *ctx, const char *source);
RIFTLANG_API RLangCompileResult rlang_load_file(RLangContext *ctx, const char *filepath);
RIFTLANG_API char *rlang_execute_pattern(RLangProgram *program, const char *pattern_name);
RIFTLANG_API bool rlang_get_patterns(RLangProgram *program,
                                     const char ***out_patterns,
                                     size_t *out_count);
RIFTLANG_API bool rlang_set_binding(RLangContext *ctx,
                                    const char *name,
                                    const char *value);
RIFTLANG_API void rlang_program_free(RLangProgram *program);
RIFTLANG_API void rlang_compile_result_free(RLangCompileResult *result);
RIFTLANG_API void *rlang_parse(const char *source);
RIFTLANG_API void rlang_ast_free(void *node);
RIFTLANG_API bool rlang_validate(const char *source, char **out_error);
RIFTLANG_API void rlang_print_program(RLangProgram *program, FILE *stream);
RIFTLANG_API bool rlang_export_c(RLangProgram *program, const char *filepath);

/* Compatibility surface for the earlier polar-pattern runtime. */
typedef enum RiftExecutionMode {
    RIFT_MODE_CLASSICAL = 0,
    RIFT_MODE_QUANTUM,
    RIFT_MODE_HYBRID
} RiftExecutionMode;

typedef enum RiftPatternPolarity {
    RIFT_PATTERN_LEFT = 0,
    RIFT_PATTERN_RIGHT
} RiftPatternPolarity;

typedef struct RiftPatternEngine RiftPatternEngine;

typedef enum RiftPolicyResult {
    RIFT_POLICY_DENY = 0,
    RIFT_POLICY_ALLOW,
    RIFT_POLICY_DEFER
} RiftPolicyResult;

typedef struct RiftResultMatrix2x2 {
    RiftPolicyResult matrix[2][2];
    double validation_threshold;
    uint64_t validations_passed;
    uint64_t validations_failed;
    uint64_t validations_deferred;
} RiftResultMatrix2x2;

typedef RIFTMemory RiftMemorySpan;

RIFTLANG_API RiftPatternEngine *rift_pattern_engine_create(RiftExecutionMode mode);
RIFTLANG_API void rift_pattern_engine_destroy(RiftPatternEngine *engine);
RIFTLANG_API bool rift_pattern_engine_add_pair(RiftPatternEngine *engine,
                                               const char *left_pattern,
                                               const char *right_pattern,
                                               uint32_t priority);
RIFTLANG_API bool rift_pattern_engine_compile(RiftPatternEngine *engine);
RIFTLANG_API char *rift_pattern_engine_match(RiftPatternEngine *engine,
                                             const char *input,
                                             size_t *output_len,
                                             uint32_t *matched_priority);
RIFTLANG_API RiftResultMatrix2x2 *rift_result_matrix_create(double threshold);
RIFTLANG_API void rift_result_matrix_destroy(RiftResultMatrix2x2 *matrix);
RIFTLANG_API RiftPolicyResult rift_policy_validate(RiftResultMatrix2x2 *matrix,
                                                   bool input_valid,
                                                   bool output_valid);
RIFTLANG_API double rift_policy_get_validation_ratio(
    const RiftResultMatrix2x2 *matrix);
RIFTLANG_API bool rift_policy_meets_threshold(const RiftResultMatrix2x2 *matrix);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_RIFTLANG_H */
