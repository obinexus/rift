/*
 * =================================================================
 * tokenizer.c - RIFT-0 Core Tokenizer Implementation (AEGIS Compliant)
 * RIFT: RIFT Is a Flexible Translator
 * Component: High-level tokenizer lifecycle management
 * OBINexus Computing Framework - Stage 0 Implementation
 * 
 * R.IMPLEMENT(TokenizerContext, LifecycleManagement, InputProcessing)
 * R.FLAGS(thread_safe, memory_aligned, error_recoverable)
 * R.COMPOSE(Create, Process, Destroy) -> Complete lifecycle
 * 
 * Toolchain: riftlang.exe → .so.a → rift.exe → gosilang
 * Build Orchestration: nlink → polybuild (AEGIS Framework)
 * Author: Nnamdi Michael Okpala & AEGIS Integration Team
 * =================================================================
 */

#include "rift-0/core/tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <time.h>

#ifdef RIFT_THREAD_SUPPORT
#include <pthread.h>
#endif

/* =================================================================
 * INTERNAL UTILITY MACROS
 * =================================================================
 */

#define RIFT_SAFE_FREE(ptr) do { if (ptr) { free(ptr); (ptr) = NULL; } } while(0)
#define RIFT_MIN(a, b) ((a) < (b) ? (a) : (b))
#define RIFT_MAX(a, b) ((a) > (b) ? (a) : (b))

/* =================================================================
 * TOKENIZER CONTEXT LIFECYCLE MANAGEMENT
 * =================================================================
 */

/**
 * Create new tokenizer context with specified capacity
 * R.CREATE(TokenizerContext) -> AEGIS compliant initialization
 */
TokenizerContext* rift_tokenizer_create(size_t initial_capacity) {
    if (initial_capacity == 0) {
        initial_capacity = RIFT_TOKENIZER_DEFAULT_CAPACITY;
    }
    
    TokenizerContext* ctx = calloc(1, sizeof(TokenizerContext));
    if (!ctx) {
        return NULL;
    }
    
    /* Initialize token storage */
    ctx->tokens = calloc(initial_capacity, sizeof(TokenTriplet));
    if (!ctx->tokens) {
        free(ctx);
        return NULL;
    }
    
    ctx->token_capacity = initial_capacity;
    ctx->token_count = 0;
    
    /* Initialize pattern storage */
    ctx->composition_capacity = RIFT_TOKENIZER_MAX_PATTERNS;
    ctx->compositions = calloc(ctx->composition_capacity, sizeof(RegexComposition*));
    if (!ctx->compositions) {
        free(ctx->tokens);
        free(ctx);
        return NULL;
    }
    
    /* Initialize state */
    ctx->input_buffer = NULL;
    ctx->input_length = 0;
    ctx->input_position = 0;
    ctx->dfa_root = NULL;
    ctx->error_message = NULL;
    ctx->has_error = false;
    ctx->thread_safe = false;
    ctx->mutex = NULL;
    ctx->aegis_compliant = true;
    ctx->governance_score = 0;
    
    /* Initialize statistics */
    memset(&ctx->stats, 0, sizeof(TokenizerStats));
    ctx->stats.memory_allocated = sizeof(TokenizerContext) + 
                                  (initial_capacity * sizeof(TokenTriplet)) +
                                  (ctx->composition_capacity * sizeof(RegexComposition*));
    ctx->stats.memory_peak = ctx->stats.memory_allocated;
    
    return ctx;
}

/**
 * Destroy tokenizer context and free all resources
 * R.DESTROY(TokenizerContext) -> Complete cleanup
 */
void rift_tokenizer_destroy(TokenizerContext* ctx) {
    if (!ctx) return;
    
    /* Free token storage */
    RIFT_SAFE_FREE(ctx->tokens);
    
    /* Free input buffer */
    RIFT_SAFE_FREE(ctx->input_buffer);
    
    /* Free error message */
    RIFT_SAFE_FREE(ctx->error_message);
    
    /* Free pattern compositions */
    if (ctx->compositions) {
        for (size_t i = 0; i < ctx->composition_count; i++) {
            if (ctx->compositions[i]) {
                rift_regex_destroy(ctx->compositions[i]);
            }
        }
        free(ctx->compositions);
    }
    
    /* Free DFA state machine */
    if (ctx->dfa_root) {
        rift_dfa_destroy_states(ctx->dfa_root);
    }
    
    /* Free thread synchronization */
    #ifdef RIFT_THREAD_SUPPORT
    if (ctx->mutex) {
        pthread_mutex_destroy((pthread_mutex_t*)ctx->mutex);
        free(ctx->mutex);
    }
    #endif
    
    /* Zero and free context */
    memset(ctx, 0, sizeof(TokenizerContext));
    free(ctx);
}

/**
 * Reset tokenizer to initial state
 * R.RESET(TokenizerContext) -> Clean state preservation
 */
bool rift_tokenizer_reset(TokenizerContext* ctx) {
    if (!ctx) return false;
    
    /* Reset processing state */
    ctx->token_count = 0;
    ctx->input_position = 0;
    ctx->has_error = false;
    
    /* Clear error message */
    RIFT_SAFE_FREE(ctx->error_message);
    
    /* Reset statistics (preserve allocations) */
    size_t memory_allocated = ctx->stats.memory_allocated;
    size_t memory_peak = ctx->stats.memory_peak;
    memset(&ctx->stats, 0, sizeof(TokenizerStats));
    ctx->stats.memory_allocated = memory_allocated;
    ctx->stats.memory_peak = memory_peak;
    
    return true;
}

/* =================================================================
 * INPUT MANAGEMENT FUNCTIONS
 * =================================================================
 */

/**
 * Set input text for processing
 * R.INPUT(buffer, length) -> Safe buffer management
 */
bool rift_tokenizer_set_input(TokenizerContext* ctx, const char* input, size_t length) {
    if (!ctx || !input) {
        return false;
    }
    
    /* Free existing buffer */
    RIFT_SAFE_FREE(ctx->input_buffer);
    
    /* Allocate and copy new buffer */
    ctx->input_buffer = malloc(length + 1);
    if (!ctx->input_buffer) {
        ctx->has_error = true;
        ctx->error_message = strdup("Memory allocation failed for input buffer");
        return false;
    }
    
    memcpy(ctx->input_buffer, input, length);
    ctx->input_buffer[length] = '\0';
    ctx->input_length = length;
    ctx->input_position = 0;
    
    /* Update memory statistics */
    ctx->stats.memory_allocated += length + 1;
    ctx->stats.memory_peak = RIFT_MAX(ctx->stats.memory_peak, ctx->stats.memory_allocated);
    
    return true;
}

/**
 * Set input from file
 * R.INPUT(filename) -> File-based input loading
 */
bool rift_tokenizer_set_input_file(TokenizerContext* ctx, const char* filename) {
    if (!ctx || !filename) {
        return false;
    }
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        ctx->has_error = true;
        ctx->error_message = strdup("Failed to open input file");
        return false;
    }
    
    /* Get file size */
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size < 0) {
        fclose(file);
        ctx->has_error = true;
        ctx->error_message = strdup("Failed to determine file size");
        return false;
    }
    
    /* Allocate buffer */
    char* buffer = malloc(file_size + 1);
    if (!buffer) {
        fclose(file);
        ctx->has_error = true;
        ctx->error_message = strdup("Memory allocation failed for file buffer");
        return false;
    }
    
    /* Read file */
    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);
    
    if (bytes_read != (size_t)file_size) {
        free(buffer);
        ctx->has_error = true;
        ctx->error_message = strdup("Failed to read complete file");
        return false;
    }
    
    buffer[file_size] = '\0';
    
    /* Set input using buffer */
    bool result = rift_tokenizer_set_input(ctx, buffer, file_size);
    free(buffer);
    
    return result;
}

/* =================================================================
 * TOKEN PROCESSING FUNCTIONS
 * =================================================================
 */

/**
 * Process input and generate tokens
 * R.PROCESS(TokenizerContext) -> DFA-based tokenization
 */
bool rift_tokenizer_process(TokenizerContext* ctx) {
    if (!ctx || !ctx->input_buffer) {
        return false;
    }
    
    clock_t start_time = clock();
    ctx->token_count = 0;
    
    /* Basic whitespace-aware tokenization for demo */
    size_t pos = 0;
    while (pos < ctx->input_length) {
        char ch = ctx->input_buffer[pos];
        
        /* Ensure token capacity */
        if (ctx->token_count >= ctx->token_capacity) {
            size_t new_capacity = ctx->token_capacity * 2;
            TokenTriplet* new_tokens = realloc(ctx->tokens, 
                                               new_capacity * sizeof(TokenTriplet));
            if (!new_tokens) {
                ctx->has_error = true;
                ctx->error_message = strdup("Token capacity expansion failed");
                return false;
            }
            ctx->tokens = new_tokens;
            ctx->token_capacity = new_capacity;
        }
        
        /* Simple character classification */
        TokenType type = TOKEN_UNKNOWN;
        if (ch >= 'a' && ch <= 'z') type = TOKEN_IDENTIFIER;
        else if (ch >= 'A' && ch <= 'Z') type = TOKEN_IDENTIFIER;
        else if (ch >= '0' && ch <= '9') type = TOKEN_LITERAL_NUMBER;
        else if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') type = TOKEN_WHITESPACE;
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') type = TOKEN_OPERATOR;
        else type = TOKEN_PUNCTUATION;
        
        /* Create token */
        ctx->tokens[ctx->token_count] = rift_token_create(type, pos, 0);
        ctx->token_count++;
        
        pos++;
    }
    
    /* Add EOF token */
    if (ctx->token_count < ctx->token_capacity) {
        ctx->tokens[ctx->token_count] = rift_token_create(TOKEN_EOF, ctx->input_length, 0);
        ctx->token_count++;
    }
    
    /* Update statistics */
    clock_t end_time = clock();
    ctx->stats.processing_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    ctx->stats.tokens_processed = ctx->input_length;
    ctx->stats.tokens_generated = ctx->token_count;
    
    return true;
}

/**
 * Get processed tokens
 * R.GET(tokens, count) -> Token array access
 */
TokenTriplet* rift_tokenizer_get_tokens(TokenizerContext* ctx, size_t* count) {
    if (!ctx || !count) {
        return NULL;
    }
    
    *count = ctx->token_count;
    return ctx->tokens;
}

/**
 * Get next token (streaming interface)
 * R.NEXT(TokenTriplet) -> Sequential token access
 */
TokenTriplet rift_tokenizer_next_token(TokenizerContext* ctx) {
    if (!ctx || ctx->input_position >= ctx->token_count) {
        return rift_token_create(TOKEN_EOF, 0, TOKEN_FLAG_ERROR);
    }
    
    return ctx->tokens[ctx->input_position++];
}

/* =================================================================
 * ERROR HANDLING FUNCTIONS
 * =================================================================
 */

/**
 * Get last error message
 */
const char* rift_tokenizer_get_error(const TokenizerContext* ctx) {
    return (ctx && ctx->error_message) ? ctx->error_message : "No error";
}

/**
 * Check if context has error
 */
bool rift_tokenizer_has_error(const TokenizerContext* ctx) {
    return ctx ? ctx->has_error : true;
}

/**
 * Clear error state
 */
void rift_tokenizer_clear_error(TokenizerContext* ctx) {
    if (!ctx) return;
    
    ctx->has_error = false;
    RIFT_SAFE_FREE(ctx->error_message);
}

/* =================================================================
 * STATISTICS AND DIAGNOSTICS
 * =================================================================
 */

/**
 * Get tokenizer statistics
 */
TokenizerStats rift_tokenizer_get_stats(const TokenizerContext* ctx) {
    TokenizerStats empty_stats = {0};
    return ctx ? ctx->stats : empty_stats;
}

/**
 * Reset statistics
 */
void rift_tokenizer_reset_stats(TokenizerContext* ctx) {
    if (!ctx) return;
    
    memset(&ctx->stats, 0, sizeof(TokenizerStats));
}

/**
 * Print tokenizer statistics
 */
void rift_tokenizer_print_stats(const TokenizerContext* ctx) {
    if (!ctx) return;
    
    printf("=== RIFT-0 Tokenizer Statistics ===\n");
    printf("Tokens Processed: %zu\n", ctx->stats.tokens_processed);
    printf("Tokens Generated: %zu\n", ctx->stats.tokens_generated);
    printf("Memory Allocated: %zu bytes\n", ctx->stats.memory_allocated);
    printf("Memory Peak: %zu bytes\n", ctx->stats.memory_peak);
    printf("DFA States: %zu\n", ctx->stats.dfa_states_created);
    printf("Regex Patterns: %zu\n", ctx->stats.regex_patterns);
    printf("Processing Time: %.6f seconds\n", ctx->stats.processing_time);
    printf("Error Count: %u\n", ctx->stats.error_count);
    printf("AEGIS Compliant: %s\n", ctx->aegis_compliant ? "Yes" : "No");
    printf("==================================\n");
}

/**
 * Print generated tokens
 */
void rift_tokenizer_print_tokens(const TokenizerContext* ctx) {
    if (!ctx) return;
    
    printf("=== RIFT-0 Generated Tokens ===\n");
    for (size_t i = 0; i < ctx->token_count; i++) {
        TokenTriplet token = ctx->tokens[i];
        printf("Token[%zu]: type=%u, ptr=%u, val=%u (%s)\n",
               i, token.type, token.mem_ptr, token.value,
               rift_token_type_to_string(token.type));
    }
    printf("================================\n");
}

/**
 * Validate DFA structure
 */
bool rift_tokenizer_validate_dfa(const TokenizerContext* ctx) {
    if (!ctx || !ctx->dfa_root) {
        return false;
    }
    
    return rift_dfa_is_accepting_state(ctx->dfa_root) || ctx->dfa_root->next_state != NULL;
}

/* =================================================================
 * VERSION INFORMATION
 * =================================================================
 */

/**
 * Get tokenizer version string
 */
const char* rift_tokenizer_version(void) {
    return RIFT_VERSION_STRING;
}

/**
 * Get build information
 */
const char* rift_tokenizer_build_info(void) {
    return "RIFT-0 Tokenizer - AEGIS Framework - OBINexus Computing";
}