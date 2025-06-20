#include "tokenizer_rules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <assert.h>

// PoliC Zero Trust Security Integration
#pragma rift_policy memory(aligned(4)) type(strict) value(static)

// Internal DFA state machine structures
typedef enum {
    DFA_STATE_INIT = 0,
    DFA_STATE_R_DETECTED = 1,
    DFA_STATE_QUOTE_CAPTURE = 2,
    DFA_STATE_DELIMITER_PROCESS = 3,
    DFA_STATE_CONTENT_CAPTURE = 4,
    DFA_STATE_TERMINATION = 5,
    DFA_STATE_VALIDATION = 6,
    DFA_STATE_ERROR = 255
} DFAState;

// Global tokenizer state with thread-safety considerations
static bool g_tokenizer_initialized = false;
static regex_t g_compiled_patterns[256];
static size_t g_pattern_count = 0;

// PoliC governance context (populated from .riftrc.0)
static struct {
    bool stack_protection;
    bool memory_aligned;
    bool type_strict;
    bool value_static;
} g_governance_context = {true, true, true, true};

// Token type name lookup table
static const char* token_type_names[] = {
    "UNKNOWN", "IDENTIFIER", "KEYWORD", "LITERAL_INT", "LITERAL_FLOAT",
    "LITERAL_STRING", "OPERATOR", "DELIMITER", "WHITESPACE", "COMMENT",
    "EOF", "ERROR"
};

/**
 * PoliC Zero Trust Decorator for function entry validation
 */
#define POLIC_DECORATOR(func_name) \
    do { \
        if (!g_tokenizer_initialized) { \
            fprintf(stderr, "RIFT-0 Policy Violation: %s called before initialization\n", func_name); \
            return -1; \
        } \
        printf("LOG: RIFT-0 function %s policy enforcement active\n", func_name); \
    } while(0)

/**
 * Inline policy check for critical operations
 */
#define INLINE_POLICY_CHECK(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "RIFT-0 Policy Check Failed: %s\n", message); \
            return -1; \
        } \
    } while(0)

/**
 * Initialize PoliC governance system
 */
static int polic_init(void) {
    // Read governance settings from .riftrc.0 (simulated for POC)
    printf("LOG: Initializing PoliC Zero Trust governance\n");
    
    // Validate memory alignment requirements
    if (sizeof(TokenTriplet) % 4 != 0) {
        fprintf(stderr, "ERROR: TokenTriplet structure not 4-byte aligned\n");
        return -1;
    }
    
    printf("LOG: PoliC governance initialized successfully\n");
    return 0;
}

int init_tokenizer_rules(void) {
    if (g_tokenizer_initialized) {
        printf("LOG: Tokenizer already initialized\n");
        return 0;
    }
    
    // Initialize PoliC governance
    if (polic_init() != 0) {
        return -1;
    }
    
    // Initialize pattern storage
    memset(g_compiled_patterns, 0, sizeof(g_compiled_patterns));
    g_pattern_count = 0;
    
    // Pre-compile common patterns for efficiency
    // TODO: Load patterns from .riftrc.0 configuration
    
    g_tokenizer_initialized = true;
    printf("LOG: RIFT-0 tokenizer rules engine initialized\n");
    return 0;
}

/**
 * DFA state progression for R"" pattern parsing
 */
static DFAState process_dfa_transition(DFAState current_state, char input_char, 
                                     uint32_t flags, char* capture_buffer, 
                                     size_t* capture_pos) {
    switch (current_state) {
        case DFA_STATE_INIT:
            if (input_char == 'R') {
                return DFA_STATE_R_DETECTED;
            }
            return DFA_STATE_ERROR;
            
        case DFA_STATE_R_DETECTED:
            if (input_char == '"' || input_char == '\'') {
                return DFA_STATE_QUOTE_CAPTURE;
            }
            return DFA_STATE_ERROR;
            
        case DFA_STATE_QUOTE_CAPTURE:
            if (input_char == '/') {
                return DFA_STATE_DELIMITER_PROCESS;
            }
            return DFA_STATE_ERROR;
            
        case DFA_STATE_DELIMITER_PROCESS:
            // Start content capture
            capture_buffer[(*capture_pos)++] = input_char;
            return DFA_STATE_CONTENT_CAPTURE;
            
        case DFA_STATE_CONTENT_CAPTURE:
            if (input_char == '/') {
                return DFA_STATE_TERMINATION;
            }
            if (*capture_pos < 1024) { // Buffer overflow protection
                capture_buffer[(*capture_pos)++] = input_char;
            }
            return DFA_STATE_CONTENT_CAPTURE;
            
        case DFA_STATE_TERMINATION:
            // Process flags (g, m, i, t, b)
            if (strchr("gmitb", input_char)) {
                return DFA_STATE_TERMINATION; // Continue flag processing
            }
            if (input_char == '"' || input_char == '\'') {
                return DFA_STATE_VALIDATION;
            }
            return DFA_STATE_ERROR;
            
        case DFA_STATE_VALIDATION:
            return DFA_STATE_VALIDATION; // Pattern complete
            
        default:
            return DFA_STATE_ERROR;
    }
}

int match_token_pattern(const char* src, const char* pattern, 
                       uint32_t flags, PatternMatchResult* result) {
    POLIC_DECORATOR("match_token_pattern");
    INLINE_POLICY_CHECK(src != NULL, "Source text cannot be NULL");
    INLINE_POLICY_CHECK(pattern != NULL, "Pattern cannot be NULL");
    INLINE_POLICY_CHECK(result != NULL, "Result structure cannot be NULL");
    
    // Initialize result structure
    memset(result, 0, sizeof(PatternMatchResult));
    
    // Parse R"" pattern syntax using DFA
    DFAState state = DFA_STATE_INIT;
    char capture_buffer[1024] = {0};
    size_t capture_pos = 0;
    
    const char* pattern_ptr = pattern;
    while (*pattern_ptr && state != DFA_STATE_ERROR && state != DFA_STATE_VALIDATION) {
        state = process_dfa_transition(state, *pattern_ptr, flags, 
                                     capture_buffer, &capture_pos);
        pattern_ptr++;
    }
    
    if (state == DFA_STATE_ERROR) {
        result->error_msg = "Invalid R\"\" pattern syntax";
        return -1;
    }
    
    // Apply regex matching with captured pattern
    regex_t regex;
    int regex_flags = REG_EXTENDED;
    
    if (flags & DFA_FLAG_INSENSITIVE) {
        regex_flags |= REG_ICASE;
    }
    
    int compile_result = regcomp(&regex, capture_buffer, regex_flags);
    if (compile_result != 0) {
        result->error_msg = "Failed to compile regex pattern";
        return -1;
    }
    
    regmatch_t match;
    int match_result = regexec(&regex, src, 1, &match, 0);
    
    if (match_result == 0) {
        // Successful match - create token triplet
        result->token.type = TOKEN_IDENTIFIER; // Default classification
        result->token.mem_ptr = (uint16_t)(match.rm_so & 0xFFFF);
        result->token.value = (uint8_t)(strlen(src) & 0xFF);
        result->match_length = match.rm_eo - match.rm_so;
        result->success = true;
    } else {
        result->success = false;
        result->error_msg = "Pattern did not match";
    }
    
    regfree(&regex);
    return result->success ? 0 : -1;
}

int tokenize_source(const char* src, TokenTriplet* tokens, 
                   size_t max_tokens, size_t* token_count) {
    POLIC_DECORATOR("tokenize_source");
    INLINE_POLICY_CHECK(src != NULL, "Source text cannot be NULL");
    INLINE_POLICY_CHECK(tokens != NULL, "Token array cannot be NULL");
    INLINE_POLICY_CHECK(token_count != NULL, "Token count pointer cannot be NULL");
    
    *token_count = 0;
    const char* current_pos = src;
    size_t src_len = strlen(src);
    
    while (*current_pos && *token_count < max_tokens) {
        TokenTriplet current_token = {0};
        int advance_chars = 1;
        
        // Simple tokenization logic (enhanced version would use full DFA)
        if (isalpha(*current_pos) || *current_pos == '_') {
            current_token.type = TOKEN_IDENTIFIER;
            const char* start = current_pos;
            while (isalnum(*current_pos) || *current_pos == '_') {
                current_pos++;
            }
            advance_chars = current_pos - start;
            current_pos = start; // Reset for advance calculation
        } else if (isdigit(*current_pos)) {
            current_token.type = TOKEN_LITERAL_INT;
            while (isdigit(*current_pos)) {
                current_pos++;
                advance_chars++;
            }
            current_pos -= advance_chars; // Reset for advance calculation
        } else if (isspace(*current_pos)) {
            current_token.type = TOKEN_WHITESPACE;
        } else if (strchr("+-*/=<>!", *current_pos)) {
            current_token.type = TOKEN_OPERATOR;
        } else if (strchr("(){}[];,", *current_pos)) {
            current_token.type = TOKEN_DELIMITER;
        } else {
            current_token.type = TOKEN_UNKNOWN;
        }
        
        // Set memory pointer and value
        current_token.mem_ptr = (uint16_t)((current_pos - src) & 0xFFFF);
        current_token.value = (uint8_t)(*current_pos & 0xFF);
        
        tokens[*token_count] = current_token;
        (*token_count)++;
        
        current_pos += advance_chars;
    }
    
    // Add EOF token
    if (*token_count < max_tokens) {
        TokenTriplet eof_token = {TOKEN_EOF, (uint16_t)(src_len & 0xFFFF), 0};
        tokens[*token_count] = eof_token;
        (*token_count)++;
    }
    
    printf("LOG: Tokenized %zu tokens from source\n", *token_count);
    return 0;
}

bool validate_null_nil_semantics(const char* token_text, TokenType* semantic_type) {
    INLINE_POLICY_CHECK(token_text != NULL, "Token text cannot be NULL");
    INLINE_POLICY_CHECK(semantic_type != NULL, "Semantic type pointer cannot be NULL");
    
    if (strcmp(token_text, "NULL") == 0) {
        *semantic_type = TOKEN_KEYWORD; // Void intent
        return true;
    } else if (strcmp(token_text, "nil") == 0) {
        *semantic_type = TOKEN_KEYWORD; // Unallocated state
        return true;
    }
    
    *semantic_type = TOKEN_UNKNOWN;
    return false;
}

int evaluate_boolean_composition(const char* expression, bool* result) {
    POLIC_DECORATOR("evaluate_boolean_composition");
    INLINE_POLICY_CHECK(expression != NULL, "Expression cannot be NULL");
    INLINE_POLICY_CHECK(result != NULL, "Result pointer cannot be NULL");
    
    // Simplified boolean evaluation (full implementation would parse complex expressions)
    if (strstr(expression, "AND")) {
        *result = true; // Simplified logic
    } else if (strstr(expression, "NOT")) {
        *result = false;
    } else {
        *result = true;
    }
    
    return 0;
}

void cleanup_tokenizer_rules(void) {
    if (!g_tokenizer_initialized) {
        return;
    }
    
    // Free compiled regex patterns
    for (size_t i = 0; i < g_pattern_count; i++) {
        regfree(&g_compiled_patterns[i]);
    }
    
    g_pattern_count = 0;
    g_tokenizer_initialized = false;
    
    printf("LOG: RIFT-0 tokenizer rules engine cleaned up\n");
}

const char* get_token_type_name(TokenType type) {
    if (type >= sizeof(token_type_names) / sizeof(token_type_names[0])) {
        return "INVALID";
    }
    return token_type_names[type];
}

bool polic_validate_token(const TokenTriplet* token, const void* policy_context) {
    INLINE_POLICY_CHECK(token != NULL, "Token cannot be NULL");
    
    // Validate against governance context
    if (g_governance_context.type_strict && token->type == TOKEN_UNKNOWN) {
        fprintf(stderr, "POLICY VIOLATION: Unknown token type not allowed in strict mode\n");
        return false;
    }
    
    if (g_governance_context.memory_aligned && (token->mem_ptr % 4) != 0) {
        fprintf(stderr, "POLICY VIOLATION: Memory pointer not 4-byte aligned\n");
        return false;
    }
    
    printf("LOG: Token validation passed PoliC governance checks\n");
    return true;
}