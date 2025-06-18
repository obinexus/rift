/*
 * tokenizer.h - RIFT tokenization Interface
 * RIFT: RIFT Is a Flexible Translator
 * Stage: rift-0
 * OBINexus Computing Framework - Technical Implementation
 */

#ifndef RIFT_TOKENIZER_H
#define RIFT_TOKENIZER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* tokenization stage configuration */
#define RIFT_STAGE_TOKENIZATION 1
#define RIFT_TOKENIZER_VERSION 0x040000

/* AEGIS methodology compliance structures */
typedef struct rift_tokenizer_context {
    uint32_t version;
    bool initialized;
    uint32_t thread_count;
    bool dual_mode_enabled;
    bool aegis_compliant;
    void *stage_data;
    void *next_stage_input;
} rift_tokenizer_context_t;

typedef struct rift_tokenizer_config {
    uint32_t processing_flags;
    uint32_t validation_level;
    bool trust_tagging_enabled;
    bool preserve_matched_state;
    char *output_format;
} rift_tokenizer_config_t;

/* Stage execution result */
typedef enum {
    RIFT_TOKENIZER_SUCCESS = 0,
    RIFT_TOKENIZER_ERROR_INVALID_INPUT = -1,
    RIFT_TOKENIZER_ERROR_PROCESSING = -2,
    RIFT_TOKENIZER_ERROR_VALIDATION = -3,
    RIFT_TOKENIZER_ERROR_MEMORY = -4
} rift_tokenizer_result_t;

/* Core API functions */
rift_tokenizer_context_t* rift_tokenizer_init(rift_tokenizer_config_t *config);
rift_tokenizer_result_t rift_tokenizer_process(
    rift_tokenizer_context_t *ctx,
    const void *input,
    size_t input_size,
    void **output,
    size_t *output_size
);
rift_tokenizer_result_t rift_tokenizer_validate(rift_tokenizer_context_t *ctx);
void rift_tokenizer_cleanup(rift_tokenizer_context_t *ctx);

/* Stage-specific functions */
/* Tokenization-specific functions */
rift_tokenizer_result_t rift_tokenizer_set_pattern(rift_tokenizer_context_t *ctx, const char *pattern);
rift_tokenizer_result_t rift_tokenizer_tokenize_input(rift_tokenizer_context_t *ctx, const char *input);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_${component^^}_H */
