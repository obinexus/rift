/*
 * test_tokenizer.c - Unit Tests for RIFT tokenization
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework - Test Implementation
 */

#include "rift-0/core/tokenizer.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Test initialization and cleanup */
void test_tokenizer_init_cleanup() {
    printf("Testing tokenizer initialization and cleanup...\n");
    
    rift_tokenizer_config_t config = {0};
    config.processing_flags = 0x01;
    config.validation_level = 3;
    
    rift_tokenizer_context_t *ctx = rift_tokenizer_init(&config);
    assert(ctx != NULL);
    assert(ctx->initialized == true);
    assert(ctx->aegis_compliant == true);
    
    rift_tokenizer_cleanup(ctx);
    printf("✅ Initialization and cleanup test passed\n");
}

/* Test stage processing */
void test_tokenizer_processing() {
    printf("Testing tokenizer processing...\n");
    
    rift_tokenizer_config_t config = {0};
    rift_tokenizer_context_t *ctx = rift_tokenizer_init(&config);
    assert(ctx != NULL);
    
    const char *input = "test input data";
    void *output = NULL;
    size_t output_size = 0;
    
    rift_tokenizer_result_t result = rift_tokenizer_process(
        ctx, input, strlen(input), &output, &output_size);
    
    assert(result == RIFT_TOKENIZER_SUCCESS);
    assert(output != NULL);
    assert(output_size > 0);
    
    free(output);
    rift_tokenizer_cleanup(ctx);
    printf("✅ Processing test passed\n");
}

/* Test validation */
void test_tokenizer_validation() {
    printf("Testing tokenizer validation...\n");
    
    rift_tokenizer_config_t config = {0};
    config.validation_level = 3;
    
    rift_tokenizer_context_t *ctx = rift_tokenizer_init(&config);
    assert(ctx != NULL);
    
    rift_tokenizer_result_t result = rift_tokenizer_validate(ctx);
    assert(result == RIFT_TOKENIZER_SUCCESS);
    
    rift_tokenizer_cleanup(ctx);
    printf("✅ Validation test passed\n");
}

int main() {
    printf("Running unit tests for RIFT tokenization (rift-0)\n");
    printf("========================================\n");
    
    test_tokenizer_init_cleanup();
    test_tokenizer_processing();
    test_tokenizer_validation();
    
    printf("\n🎉 All tokenizer unit tests passed!\n");
    return 0;
}
