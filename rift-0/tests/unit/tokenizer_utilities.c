/**
 * =================================================================
 * tokenizer_utilities.c - RIFT-0 Tokenizer Utility Functions
 * RIFT: RIFT Is a Flexible Translator
 * Component: Utility functions for pattern matching and validation
 * OBINexus Computing Framework - Stage 0 Implementation
 * =================================================================
 */

#include "rift-0/core/tokenizer_rules.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <regex.h>
#include <pthread.h>
#include <errno.h>

/* Pattern matching cache for performance optimization */
static pthread_mutex_t pattern_cache_mutex = PTHREAD_MUTEX_INITIALIZER;
static CompiledPattern* pattern_cache[MAX_COMPILED_PATTERNS];
static size_t pattern_cache_count = 0;

/**
 * Match pattern at specific position
 * Internal utility for R-macro operations
 */
bool match_pattern_at_position(const CompiledPattern* pattern, const char* text, size_t pos) {
    if (!pattern || !text || !pattern->pattern_data) {
        return false;
    }
    
    // Bounds checking
    size_t text_len = strlen(text);
    if (pos >= text_len) {
        return false;
    }
    
    // Simple pattern matching implementation
    // In a production system, this would use the compiled DFA
    const char* pattern_str = (const char*)pattern->pattern_data;
    const char* test_position = text + pos;
    
    // Handle different token types based on pattern type
    switch (pattern->token_type) {
        case TOKEN_IDENTIFIER:
            return is_identifier_start(*test_position) ||
                   (pos > 0 && is_identifier_continue(*test_position));
            
        case TOKEN_LITERAL_NUMBER:
            return isdigit(*test_position) || *test_position == '.';
            
        case TOKEN_OPERATOR:
            return strchr("+-*/%=<>!&|^~", *test_position) != NULL;
            
        case TOKEN_DELIMITER:
            return strchr("(){}[];,.", *test_position) != NULL;
            
        case TOKEN_WHITESPACE:
            return isspace(*test_position);
            
        case TOKEN_R_PATTERN:
            // R-pattern matching requires more sophisticated logic
            return strncmp(test_position, "R\"", 2) == 0 || strncmp(test_position, "R'", 2) == 0;
            
        default:
            return false;
    }
}

/**
 * Parse R-pattern flags from string
 */
static uint32_t parse_r_pattern_flags(const char* flag_str) {
    uint32_t flags = 0;
    
    if (!flag_str) return flags;
    
    for (const char* p = flag_str; *p; p++) {
        switch (*p) {
            case 'g': flags |= DFA_FLAG_GLOBAL; break;
            case 'm': flags |= DFA_FLAG_MULTILINE; break;
            case 'i': flags |= DFA_FLAG_INSENSITIVE; break;
            case 't': flags |= DFA_FLAG_TOP_DOWN; break;
            case 'b': flags |= DFA_FLAG_BOTTOM_UP; break;
            // Ignore other characters
        }
    }
    
    return flags;
}

/**
 * Extract delimiter from R-pattern string
 */
static int extract_r_pattern_delimiter(const char* pattern_str, char* delimiter, size_t max_len) {
    if (!pattern_str || !delimiter || max_len == 0) {
        return -1;
    }
    
    // Look for R" or R' prefix
    if (strncmp(pattern_str, "R\"", 2) != 0 && strncmp(pattern_str, "R'", 2) != 0) {
        return -1;
    }
    
    const char* start = pattern_str + 2;
    const char* content_start = strchr(start, '(');
    
    if (!content_start) {
        return -1;
    }
    
    size_t delim_len = content_start - start;
    if (delim_len >= max_len) {
        return -1;
    }
    
    strncpy(delimiter, start, delim_len);
    delimiter[delim_len] = '\0';
    
    return (int)delim_len;
}

/**
 * Compile R"" pattern string into DFA structure
 */
CompiledPattern* compile_r_pattern(const char* pattern_str, uint32_t* flags) {
    if (!pattern_str || !flags) {
        return NULL;
    }
    
    // Check pattern cache first
    pthread_mutex_lock(&pattern_cache_mutex);
    for (size_t i = 0; i < pattern_cache_count; i++) {
        if (pattern_cache[i] && pattern_cache[i]->pattern_data) {
            const char* cached_pattern = (const char*)pattern_cache[i]->pattern_data;
            if (strcmp(cached_pattern, pattern_str) == 0) {
                // Found in cache, increment reference count
                atomic_fetch_add(&pattern_cache[i]->ref_count, 1);
                *flags = pattern_cache[i]->flags;
                pthread_mutex_unlock(&pattern_cache_mutex);
                return pattern_cache[i];
            }
        }
    }
    pthread_mutex_unlock(&pattern_cache_mutex);
    
    // Allocate new compiled pattern
    CompiledPattern* compiled = malloc(sizeof(CompiledPattern));
    if (!compiled) {
        return NULL;
    }
    
    memset(compiled, 0, sizeof(CompiledPattern));
    
    // Parse pattern string
    char delimiter[MAX_DELIMITER_LENGTH];
    int delim_len = extract_r_pattern_delimiter(pattern_str, delimiter, sizeof(delimiter));
    
    if (delim_len < 0) {
        // Not an R-pattern, treat as regular pattern
        compiled->token_type = TOKEN_IDENTIFIER; // Default
        *flags = 0;
    } else {
        compiled->token_type = TOKEN_R_PATTERN;
        
        // Extract flags from pattern end
        const char* flag_start = strrchr(pattern_str, ')');
        if (flag_start) {
            flag_start = strchr(flag_start, '"');
            if (flag_start) {
                flag_start++;
                *flags = parse_r_pattern_flags(flag_start);
            }
        }
    }
    
    // Store pattern data (simplified - in production would compile to DFA)
    size_t pattern_len = strlen(pattern_str);
    compiled->pattern_data = malloc(pattern_len + 1);
    if (!compiled->pattern_data) {
        free(compiled);
        return NULL;
    }
    
    strcpy((char*)compiled->pattern_data, pattern_str);
    compiled->pattern_size = pattern_len + 1;
    compiled->flags = *flags;
    atomic_store(&compiled->ref_count, 1);
    compiled->last_match_valid = false;
    
    // Add to cache if space available
    pthread_mutex_lock(&pattern_cache_mutex);
    if (pattern_cache_count < MAX_COMPILED_PATTERNS) {
        pattern_cache[pattern_cache_count++] = compiled;
    }
    pthread_mutex_unlock(&pattern_cache_mutex);
    
    return compiled;
}

/**
 * Release compiled pattern memory
 */
void release_compiled_pattern(CompiledPattern* pattern) {
    if (!pattern) {
        return;
    }
    
    int ref_count = atomic_fetch_sub(&pattern->ref_count, 1);
    
    if (ref_count <= 1) {
        // Last reference, cleanup
        if (pattern->pattern_data) {
            free(pattern->pattern_data);
            pattern->pattern_data = NULL;
        }
        
        // Remove from cache
        pthread_mutex_lock(&pattern_cache_mutex);
        for (size_t i = 0; i < pattern_cache_count; i++) {
            if (pattern_cache[i] == pattern) {
                // Shift remaining patterns
                for (size_t j = i; j < pattern_cache_count - 1; j++) {
                    pattern_cache[j] = pattern_cache[j + 1];
                }
                pattern_cache_count--;
                break;
            }
        }
        pthread_mutex_unlock(&pattern_cache_mutex);
        
        free(pattern);
    }
}

/**
 * Validate TokenTriplet structure compliance
 */
int validate_token_triplet(const TokenTriplet* token) {
    if (!token) {
        return RIFT_ERROR_NULL_POINTER;
    }
    
    // Check bitfield constraints
    if (token->type > 255) {
        return RIFT_ERROR_POLICY_VIOLATION; // Type field is 8 bits
    }
    
    if (token->mem_ptr > 65535) {
        return RIFT_ERROR_POLICY_VIOLATION; // Memory pointer is 16 bits
    }
    
    if (token->value > 255) {
        return RIFT_ERROR_POLICY_VIOLATION; // Value field is 8 bits
    }
    
    // Check token type validity
    if (token->type >= TOKEN_ERROR && token->type != TOKEN_ERROR) {
        return RIFT_ERROR_POLICY_VIOLATION; // Invalid token type
    }
    
    // Memory alignment check (should be 4-byte aligned)
    if (sizeof(TokenTriplet) != 4) {
        return RIFT_ERROR_POLICY_VIOLATION; // Structure size violation
    }
    
    return 0; // Valid
}

/**
 * Advanced pattern matching with DFA simulation
 */
static bool simulate_dfa_match(const char* pattern, const char* text, size_t pos, uint32_t flags) {
    if (!pattern || !text) {
        return false;
    }
    
    // Simple DFA simulation for basic patterns
    size_t pattern_len = strlen(pattern);
    size_t text_len = strlen(text);
    
    if (pos >= text_len) {
        return false;
    }
    
    // Handle case insensitive flag
    bool case_insensitive = (flags & DFA_FLAG_INSENSITIVE) != 0;
    
    // Simple character-by-character matching
    for (size_t i = 0; i < pattern_len && pos + i < text_len; i++) {
        char pattern_char = pattern[i];
        char text_char = text[pos + i];
        
        if (case_insensitive) {
            pattern_char = tolower(pattern_char);
            text_char = tolower(text_char);
        }
        
        if (pattern_char != text_char) {
            return false;
        }
    }
    
    return true;
}

/**
 * Character classification utility functions
 */
bool is_identifier_start(char c) {
    return isalpha(c) || c == '_';
}

bool is_identifier_continue(char c) {
    return isalnum(c) || c == '_';
}

bool is_operator_char(char c) {
    return strchr("+-*/%=<>!&|^~", c) != NULL;
}

bool is_delimiter_char(char c) {
    return strchr("(){}[];,.", c) != NULL;
}

/**
 * Thread context management utilities
 */
void signal_context_switch(bool is_top_down) {
    // Implementation moved to tokenizer_rules.c to avoid duplication
    // This is a stub for compatibility
}

bool wait_for_context(bool need_top_down) {
    // Implementation moved to tokenizer_rules.c to avoid duplication
    // This is a stub for compatibility
    return true;
}

/**
 * Pattern string parsing utilities
 */
static bool is_valid_r_pattern_syntax(const char* pattern_str) {
    if (!pattern_str) {
        return false;
    }
    
    // Check for R" or R' prefix
    if (strncmp(pattern_str, "R\"", 2) != 0 && strncmp(pattern_str, "R'", 2) != 0) {
        return false;
    }
    
    // Find opening parenthesis
    const char* open_paren = strchr(pattern_str + 2, '(');
    if (!open_paren) {
        return false;
    }
    
    // Find closing parenthesis and matching quote
    const char* close_paren = strrchr(pattern_str, ')');
    if (!close_paren || close_paren <= open_paren) {
        return false;
    }
    
    // Check for matching quote after closing parenthesis
    char quote_char = pattern_str[1]; // " or '
    const char* close_quote = strchr(close_paren, quote_char);
    if (!close_quote) {
        return false;
    }
    
    return true;
}

/**
 * Enhanced pattern compilation with error reporting
 */
CompiledPattern* compile_r_pattern_with_validation(const char* pattern_str, uint32_t* flags, char* error_buffer, size_t error_buffer_size) {
    if (!pattern_str || !flags) {
        if (error_buffer && error_buffer_size > 0) {
            snprintf(error_buffer, error_buffer_size, "NULL pattern string or flags pointer");
        }
        return NULL;
    }
    
    // Validate pattern syntax
    if (!is_valid_r_pattern_syntax(pattern_str)) {
        if (error_buffer && error_buffer_size > 0) {
            snprintf(error_buffer, error_buffer_size, "Invalid R-pattern syntax: %s", pattern_str);
        }
        return NULL;
    }
    
    // Compile pattern
    CompiledPattern* result = compile_r_pattern(pattern_str, flags);
    if (!result && error_buffer && error_buffer_size > 0) {
        snprintf(error_buffer, error_buffer_size, "Failed to compile pattern: memory allocation error");
    }
    
    return result;
}

/**
 * Utility function to get token type name for debugging
 */
const char* get_token_type_name(TokenType type) {
    switch (type) {
        case TOKEN_UNKNOWN: return "UNKNOWN";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_KEYWORD: return "KEYWORD";
        case TOKEN_LITERAL_STRING: return "LITERAL_STRING";
        case TOKEN_LITERAL_NUMBER: return "LITERAL_NUMBER";
        case TOKEN_OPERATOR: return "OPERATOR";
        case TOKEN_DELIMITER: return "DELIMITER";
        case TOKEN_R_PATTERN: return "R_PATTERN";
        case TOKEN_NULL_KEYWORD: return "NULL_KEYWORD";
        case TOKEN_NIL_KEYWORD: return "NIL_KEYWORD";
        case TOKEN_WHITESPACE: return "WHITESPACE";
        case TOKEN_COMMENT: return "COMMENT";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        default: return "INVALID";
    }
}

/**
 * Memory management utilities for TokenTriplet arrays
 */
TokenTriplet* allocate_token_array(size_t count) {
    if (count == 0 || count > CLI_MAX_TOKENS) {
        return NULL;
    }
    
    size_t total_size = count * sizeof(TokenTriplet);
    TokenTriplet* tokens = aligned_alloc(4, total_size); // 4-byte alignment for bitfields
    
    if (tokens) {
        memset(tokens, 0, total_size);
    }
    
    return tokens;
}

void deallocate_token_array(TokenTriplet* tokens) {
    if (tokens) {
        free(tokens);
    }
}

/**
 * Performance monitoring utilities
 */
static struct {
    size_t total_patterns_compiled;
    size_t total_matches_attempted;
    size_t successful_matches;
    double total_match_time;
    pthread_mutex_t stats_mutex;
} performance_stats = {
    .total_patterns_compiled = 0,
    .total_matches_attempted = 0,
    .successful_matches = 0,
    .total_match_time = 0.0,
    .stats_mutex = PTHREAD_MUTEX_INITIALIZER
};

void record_pattern_compilation(void) {
    pthread_mutex_lock(&performance_stats.stats_mutex);
    performance_stats.total_patterns_compiled++;
    pthread_mutex_unlock(&performance_stats.stats_mutex);
}

void record_match_attempt(bool successful, double time_ms) {
    pthread_mutex_lock(&performance_stats.stats_mutex);
    performance_stats.total_matches_attempted++;
    if (successful) {
        performance_stats.successful_matches++;
    }
    performance_stats.total_match_time += time_ms;
    pthread_mutex_unlock(&performance_stats.stats_mutex);
}

void get_performance_stats(size_t* compiled, size_t* attempted, size_t* successful, double* avg_time) {
    pthread_mutex_lock(&performance_stats.stats_mutex);
    if (compiled) *compiled = performance_stats.total_patterns_compiled;
    if (attempted) *attempted = performance_stats.total_matches_attempted;
    if (successful) *successful = performance_stats.successful_matches;
    if (avg_time) {
        *avg_time = performance_stats.total_matches_attempted > 0 ? 
                    performance_stats.total_match_time / performance_stats.total_matches_attempted : 0.0;
    }
    pthread_mutex_unlock(&performance_stats.stats_mutex);
}

/**
 * Cleanup all utility resources
 */
void cleanup_tokenizer_utilities(void) {
    // Clear pattern cache
    pthread_mutex_lock(&pattern_cache_mutex);
    for (size_t i = 0; i < pattern_cache_count; i++) {
        if (pattern_cache[i]) {
            release_compiled_pattern(pattern_cache[i]);
            pattern_cache[i] = NULL;
        }
    }
    pattern_cache_count = 0;
    pthread_mutex_unlock(&pattern_cache_mutex);
    
    // Reset performance stats
    pthread_mutex_lock(&performance_stats.stats_mutex);
    memset(&performance_stats, 0, sizeof(performance_stats));
    pthread_mutex_unlock(&performance_stats.stats_mutex);
}