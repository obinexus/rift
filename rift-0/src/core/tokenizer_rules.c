/*
 * =================================================================
 * tokenizer_rules.c - RIFT-0 DFA Tokenization Engine
 * RIFT: RIFT Is a Flexible Translator
 * Component: DFA-based tokenization with R-syntax support
 * OBINexus Computing Framework - Stage 0 Core Implementation
 * 
 * Implements deterministic finite automaton patterns for R"" and R''
 * syntax compilation with TokenTriplet bitfield emission.
 * 
 * Author: OBINexus Nnamdi Michael Okpala
 * Collaboration: AEGIS Framework Integration Team
 * =================================================================
 */

#include "rift-0/core/tokenizer.h"
#include "rift-0/core/tokenizer_rules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/* =================================================================
 * TOKENTRIPLET BITFIELD IMPLEMENTATION
 * =================================================================
 */

/* Validate TokenTriplet structure alignment for deterministic builds */
_Static_assert(sizeof(TokenTriplet) == 4, "TokenTriplet must be exactly 32 bits");
_Static_assert(offsetof(TokenTriplet, type) == 0, "type field must be at offset 0");

/* Create TokenTriplet with validation */
TokenTriplet rift_token_create(uint8_t type, uint16_t mem_ptr, uint8_t value) {
    TokenTriplet token = {
        .type = type,
        .mem_ptr = mem_ptr,
        .value = value
    };
    
    /* Validate bitfield integrity */
    assert(token.type == type);
    assert(token.mem_ptr == mem_ptr);
    assert(token.value == value);
    
    return token;
}

/* Validate TokenTriplet integrity */
bool rift_token_is_valid(const TokenTriplet* token) {
    if (!token) return false;
    
    /* Check for valid token type range */
    if (token->type >= TOKEN_MAX) return false;
    
    /* Validate memory pointer bounds */
    if (token->mem_ptr > RIFT_MAX_TOKEN_LENGTH) return false;
    
    return true;
}

/* Extract flags from TokenTriplet value field */
TokenFlags rift_token_get_flags(const TokenTriplet* token) {
    if (!token) return TOKEN_FLAG_NONE;
    return (TokenFlags)(token->value & 0xFF);
}

/* Set flags in TokenTriplet value field */
void rift_token_set_flags(TokenTriplet* token, TokenFlags flags) {
    if (!token) return;
    token->value = (uint8_t)(flags & 0xFF);
}

/* =================================================================
 * DFA STATE MACHINE IMPLEMENTATION
 * =================================================================
 */

/* Create DFA state with governance validation */
DFAState* rift_dfa_create_state(uint32_t state_id, bool is_final) {
    DFAState* state = calloc(1, sizeof(DFAState));
    if (!state) return NULL;
    
    state->state_id = state_id;
    state->is_final = is_final;
    state->is_start = false;
    state->transition_char = '\0';
    state->next_state = NULL;
    state->fail_state = NULL;
    state->token_type = TOKEN_UNKNOWN;
    state->match_count = 0;
    
    return state;
}

/* Add transition between DFA states */
bool rift_dfa_add_transition(DFAState* from, DFAState* to, char transition_char) {
    if (!from || !to) return false;
    
    /* For simplicity, this implements a single transition per state */
    /* Production implementation would use transition tables */
    from->transition_char = transition_char;
    from->next_state = to;
    
    return true;
}

/* Process input through DFA state machine */
DFAState* rift_dfa_process_input(DFAState* start, const char* input, size_t length) {
    if (!start || !input) return NULL;
    
    DFAState* current = start;
    
    for (size_t i = 0; i < length && current; i++) {
        char ch = input[i];
        
        /* Find transition for current character */
        if (current->transition_char == ch && current->next_state) {
            current = current->next_state;
            current->match_count++;
        } else if (current->fail_state) {
            current = current->fail_state;
        } else {
            return NULL; /* No valid transition */
        }
    }
    
    return current;
}

/* Check if DFA state is accepting */
bool rift_dfa_is_accepting_state(DFAState* state) {
    return state && state->is_final;
}

/* Get token type from DFA state */
TokenType rift_dfa_get_token_type(DFAState* state) {
    return state ? state->token_type : TOKEN_UNKNOWN;
}

/* Cleanup DFA state machine */
void rift_dfa_destroy_states(DFAState* root) {
    if (!root) return;
    
    /* Simple cleanup - production would use proper graph traversal */
    if (root->next_state && root->next_state != root) {
        rift_dfa_destroy_states(root->next_state);
    }
    
    free(root);
}

/* =================================================================
 * R-SYNTAX REGEX COMPOSITION IMPLEMENTATION
 * =================================================================
 */

/* Regex flag mapping for R-syntax */
static const struct {
    char flag_char;
    TokenFlags flag_value;
    const char* description;
} R_FLAGS_MAP[] = {
    {'g', TOKEN_FLAG_GLOBAL,     "Global matching"},
    {'m', TOKEN_FLAG_MULTILINE,  "Multiline mode"},
    {'i', TOKEN_FLAG_IGNORECASE, "Case insensitive"},
    {'t', TOKEN_FLAG_TOPDOWN,    "Top-down evaluation"},
    {'b', TOKEN_FLAG_BOTTOMUP,   "Bottom-up evaluation"},
    {'\0', TOKEN_FLAG_NONE, NULL}
};

/* Parse R-syntax flags from string */
TokenFlags rift_parse_r_flags(const char* flag_string) {
    if (!flag_string) return TOKEN_FLAG_NONE;
    
    TokenFlags flags = TOKEN_FLAG_NONE;
    
    for (const char* p = flag_string; *p; p++) {
        for (int i = 0; R_FLAGS_MAP[i].flag_char != '\0'; i++) {
            if (*p == R_FLAGS_MAP[i].flag_char) {
                flags |= R_FLAGS_MAP[i].flag_value;
                break;
            }
        }
    }
    
    return flags;
}

/* Convert flags to string representation */
const char* rift_flags_to_string(TokenFlags flags) {
    static char buffer[32];
    buffer[0] = '\0';
    
    for (int i = 0; R_FLAGS_MAP[i].flag_char != '\0'; i++) {
        if (flags & R_FLAGS_MAP[i].flag_value) {
            size_t len = strlen(buffer);
            buffer[len] = R_FLAGS_MAP[i].flag_char;
            buffer[len + 1] = '\0';
        }
    }
    
    return buffer;
}

/* Compile R-syntax regex pattern */
RegexComposition* rift_regex_compile(const char* pattern, TokenFlags flags) {
    if (!pattern) return NULL;
    
    RegexComposition* regex = calloc(1, sizeof(RegexComposition));
    if (!regex) return NULL;
    
    regex->pattern_length = strlen(pattern);
    regex->pattern = malloc(regex->pattern_length + 1);
    if (!regex->pattern) {
        free(regex);
        return NULL;
    }
    
    strcpy(regex->pattern, pattern);
    regex->flags = flags;
    regex->is_composed = false;
    
    /* Create simple DFA for pattern */
    regex->start_state = rift_dfa_create_state(0, false);
    if (!regex->start_state) {
        free(regex->pattern);
        free(regex);
        return NULL;
    }
    
    regex->start_state->is_start = true;
    regex->current_state = regex->start_state;
    
    /* Build DFA from pattern - simplified implementation */
    DFAState* current = regex->start_state;
    uint32_t state_id = 1;
    
    for (size_t i = 0; i < regex->pattern_length; i++) {
        char ch = pattern[i];
        
        /* Skip R-syntax markers */
        if ((ch == 'R' && i + 1 < regex->pattern_length && 
             (pattern[i+1] == '"' || pattern[i+1] == '\'')) ||
            ch == '"' || ch == '\'') {
            continue;
        }
        
        DFAState* next_state = rift_dfa_create_state(state_id++, 
                                                    i == regex->pattern_length - 1);
        if (!next_state) break;
        
        rift_dfa_add_transition(current, next_state, ch);
        current = next_state;
    }
    
    /* Set final state token type */
    if (current && current->is_final) {
        current->token_type = TOKEN_REGEX_END;
    }
    
    return regex;
}

/* Boolean composition: AND operation */
RegexComposition* rift_regex_compose_and(RegexComposition* a, RegexComposition* b) {
    if (!a || !b) return NULL;
    
    RegexComposition* result = calloc(1, sizeof(RegexComposition));
    if (!result) return NULL;
    
    /* Create combined pattern */
    size_t combined_length = a->pattern_length + b->pattern_length + 16;
    result->pattern = malloc(combined_length);
    if (!result->pattern) {
        free(result);
        return NULL;
    }
    
    snprintf(result->pattern, combined_length, "(%s)&(%s)", 
             a->pattern, b->pattern);
    result->pattern_length = strlen(result->pattern);
    result->flags = a->flags | b->flags | TOKEN_FLAG_COMPOSED;
    result->is_composed = true;
    
    /* Create composite DFA - simplified */
    result->start_state = rift_dfa_create_state(0, false);
    result->current_state = result->start_state;
    
    return result;
}

/* Boolean composition: OR operation */
RegexComposition* rift_regex_compose_or(RegexComposition* a, RegexComposition* b) {
    if (!a || !b) return NULL;
    
    RegexComposition* result = calloc(1, sizeof(RegexComposition));
    if (!result) return NULL;
    
    size_t combined_length = a->pattern_length + b->pattern_length + 16;
    result->pattern = malloc(combined_length);
    if (!result->pattern) {
        free(result);
        return NULL;
    }
    
    snprintf(result->pattern, combined_length, "(%s)|(%s)", 
             a->pattern, b->pattern);
    result->pattern_length = strlen(result->pattern);
    result->flags = a->flags | b->flags | TOKEN_FLAG_COMPOSED;
    result->is_composed = true;
    
    result->start_state = rift_dfa_create_state(0, false);
    result->current_state = result->start_state;
    
    return result;
}

/* Boolean composition: XOR operation */
RegexComposition* rift_regex_compose_xor(RegexComposition* a, RegexComposition* b) {
    if (!a || !b) return NULL;
    
    RegexComposition* result = calloc(1, sizeof(RegexComposition));
    if (!result) return NULL;
    
    size_t combined_length = a->pattern_length + b->pattern_length + 16;
    result->pattern = malloc(combined_length);
    if (!result->pattern) {
        free(result);
        return NULL;
    }
    
    snprintf(result->pattern, combined_length, "(%s)^(%s)", 
             a->pattern, b->pattern);
    result->pattern_length = strlen(result->pattern);
    result->flags = a->flags | b->flags | TOKEN_FLAG_COMPOSED;
    result->is_composed = true;
    
    result->start_state = rift_dfa_create_state(0, false);
    result->current_state = result->start_state;
    
    return result;
}

/* Boolean composition: NAND operation */
RegexComposition* rift_regex_compose_nand(RegexComposition* a, RegexComposition* b) {
    if (!a || !b) return NULL;
    
    RegexComposition* result = calloc(1, sizeof(RegexComposition));
    if (!result) return NULL;
    
    size_t combined_length = a->pattern_length + b->pattern_length + 16;
    result->pattern = malloc(combined_length);
    if (!result->pattern) {
        free(result);
        return NULL;
    }
    
    snprintf(result->pattern, combined_length, "~((%s)&(%s))", 
             a->pattern, b->pattern);
    result->pattern_length = strlen(result->pattern);
    result->flags = a->flags | b->flags | TOKEN_FLAG_COMPOSED;
    result->is_composed = true;
    
    result->start_state = rift_dfa_create_state(0, false);
    result->current_state = result->start_state;
    
    return result;
}

/* Pattern matching with DFA */
bool rift_regex_match(RegexComposition* regex, const char* input, size_t length) {
    if (!regex || !input) return false;
    
    DFAState* final_state = rift_dfa_process_input(regex->start_state, input, length);
    return rift_dfa_is_accepting_state(final_state);
}

/* Pattern finding with DFA */
bool rift_regex_find(RegexComposition* regex, const char* input, size_t length,
                     size_t* match_start, size_t* match_length) {
    if (!regex || !input || !match_start || !match_length) return false;
    
    /* Simple linear search - production would use more efficient algorithms */
    for (size_t i = 0; i < length; i++) {
        for (size_t j = i; j <= length; j++) {
            if (rift_regex_match(regex, input + i, j - i)) {
                *match_start = i;
                *match_length = j - i;
                return true;
            }
        }
    }
    
    return false;
}

/* Destroy regex composition */
void rift_regex_destroy(RegexComposition* regex) {
    if (!regex) return;
    
    if (regex->pattern) {
        free(regex->pattern);
    }
    
    if (regex->start_state) {
        rift_dfa_destroy_states(regex->start_state);
    }
    
    free(regex);
}

/* =================================================================
 * TOKENIZER CONTEXT IMPLEMENTATION
 * =================================================================
 */

/* Create tokenizer context with specified capacity */
TokenizerContext* rift_tokenizer_create(size_t initial_capacity) {
    TokenizerContext* ctx = calloc(1, sizeof(TokenizerContext));
    if (!ctx) return NULL;
    
    ctx->token_capacity = initial_capacity > 0 ? initial_capacity : RIFT_DEFAULT_TOKEN_CAPACITY;
    ctx->token_buffer = malloc(ctx->token_capacity * sizeof(TokenTriplet));
    if (!ctx->token_buffer) {
        free(ctx);
        return NULL;
    }
    
    ctx->compositions = malloc(RIFT_MAX_COMPOSITIONS * sizeof(RegexComposition*));
    if (!ctx->compositions) {
        free(ctx->token_buffer);
        free(ctx);
        return NULL;
    }
    
    /* Initialize state */
    ctx->input_buffer = NULL;
    ctx->buffer_length = 0;
    ctx->current_position = 0;
    ctx->line_number = 1;
    ctx->column_number = 1;
    ctx->token_count = 0;
    ctx->composition_count = 0;
    ctx->dfa_root = NULL;
    ctx->has_error = false;
    ctx->error_position = 0;
    ctx->error_message[0] = '\0';
    ctx->thread_safe_mode = false;
    ctx->mutex_handle = NULL;
    
    return ctx;
}

/* Set input buffer for tokenization */
bool rift_tokenizer_set_input(TokenizerContext* ctx, const char* input, size_t length) {
    if (!ctx || !input) return false;
    
    ctx->input_buffer = input;
    ctx->buffer_length = length;
    ctx->current_position = 0;
    ctx->line_number = 1;
    ctx->column_number = 1;
    ctx->token_count = 0;
    ctx->has_error = false;
    
    return true;
}

/* Process input and generate tokens */
bool rift_tokenizer_process(TokenizerContext* ctx) {
    if (!ctx || !ctx->input_buffer) return false;
    
    ctx->token_count = 0;
    ctx->has_error = false;
    
    while (ctx->current_position < ctx->buffer_length) {
        if (ctx->token_count >= ctx->token_capacity) {
            snprintf(ctx->error_message, sizeof(ctx->error_message),
                    "Token buffer overflow at position %zu", ctx->current_position);
            ctx->has_error = true;
            return false;
        }
        
        /* Simple tokenization - skip whitespace */
        char ch = ctx->input_buffer[ctx->current_position];
        
        if (isspace(ch)) {
            if (ch == '\n') {
                ctx->line_number++;
                ctx->column_number = 1;
            } else {
                ctx->column_number++;
            }
            ctx->current_position++;
            continue;
        }
        
        /* Create token for current character */
        TokenType type = TOKEN_UNKNOWN;
        if (isalpha(ch)) {
            type = TOKEN_IDENTIFIER;
        } else if (isdigit(ch)) {
            type = TOKEN_LITERAL_NUMBER;
        } else {
            type = TOKEN_PUNCTUATION;
        }
        
        TokenTriplet token = rift_token_create(
            type,
            (uint16_t)(ctx->current_position & 0xFFFF),
            0
        );
        
        ctx->token_buffer[ctx->token_count++] = token;
        ctx->current_position++;
        ctx->column_number++;
    }
    
    /* Add EOF token */
    if (ctx->token_count < ctx->token_capacity) {
        TokenTriplet eof_token = rift_token_create(TOKEN_EOF, 0, 0);
        ctx->token_buffer[ctx->token_count++] = eof_token;
    }
    
    return true;
}

/* Get generated tokens */
TokenTriplet* rift_tokenizer_get_tokens(TokenizerContext* ctx, size_t* count) {
    if (!ctx || !count) return NULL;
    
    *count = ctx->token_count;
    return ctx->token_buffer;
}

/* Get error message */
const char* rift_tokenizer_get_error(const TokenizerContext* ctx) {
    return ctx ? ctx->error_message : "Invalid context";
}

/* Check if tokenizer has error */
bool rift_tokenizer_has_error(const TokenizerContext* ctx) {
    return ctx && ctx->has_error;
}

/* Destroy tokenizer context */
void rift_tokenizer_destroy(TokenizerContext* ctx) {
    if (!ctx) return;
    
    if (ctx->token_buffer) {
        free(ctx->token_buffer);
    }
    
    if (ctx->compositions) {
        for (size_t i = 0; i < ctx->composition_count; i++) {
            if (ctx->compositions[i]) {
                rift_regex_destroy(ctx->compositions[i]);
            }
        }
        free(ctx->compositions);
    }
    
    if (ctx->dfa_root) {
        rift_dfa_destroy_states(ctx->dfa_root);
    }
    
    free(ctx);
}

/* =================================================================
 * UTILITY FUNCTIONS
 * =================================================================
 */

/* Get token type name */
const char* rift_token_type_name(TokenType type) {
    static const char* type_names[] = {
        "UNKNOWN", "IDENTIFIER", "KEYWORD", "LITERAL_NUMBER",
        "LITERAL_STRING", "OPERATOR", "PUNCTUATION", "WHITESPACE",
        "COMMENT", "EOF", "ERROR", "REGEX_START", "REGEX_END",
        "COMPOSE_AND", "COMPOSE_OR", "COMPOSE_XOR", "COMPOSE_NAND",
        "DFA_STATE"
    };
    
    if (type < sizeof(type_names) / sizeof(type_names[0])) {
        return type_names[type];
    }
    
    return "INVALID";
}

/* Get flags string representation */
const char* rift_token_flags_string(TokenFlags flags) {
    return rift_flags_to_string(flags);
}

/* Print tokenizer statistics */
void rift_tokenizer_print_stats(const TokenizerContext* ctx) {
    if (!ctx) return;
    
    printf("RIFT-0 Tokenizer Statistics:\n");
    printf("  Input length: %zu characters\n", ctx->buffer_length);
    printf("  Token count: %zu\n", ctx->token_count);
    printf("  Current position: %zu\n", ctx->current_position);
    printf("  Line: %zu, Column: %zu\n", ctx->line_number, ctx->column_number);
    printf("  Compositions: %zu\n", ctx->composition_count);
    printf("  Thread safe: %s\n", ctx->thread_safe_mode ? "Yes" : "No");
    printf("  Has error: %s\n", ctx->has_error ? "Yes" : "No");
    if (ctx->has_error) {
        printf("  Error: %s\n", ctx->error_message);
    }
}

/* Validate DFA structure */
bool rift_tokenizer_validate_dfa(const TokenizerContext* ctx) {
    if (!ctx || !ctx->dfa_root) return false;
    
    /* Basic DFA validation - check for start state */
    return ctx->dfa_root->is_start;
}

/* =================================================================
 * VERSION AND BUILD INFORMATION
 * =================================================================
 */

/* Get tokenizer version */
const char* rift_tokenizer_version(void) {
    return RIFT_VERSION_STRING;
}

/* Get build information */
const char* rift_tokenizer_build_info(void) {
    return "RIFT-0 Tokenizer with DFA and R-syntax support - "
           "OBINexus Computing Framework";
}

/* Get version number */
uint32_t rift_tokenizer_version_number(void) {
    return (RIFT_TOKENIZER_VERSION_MAJOR << 16) |
           (RIFT_TOKENIZER_VERSION_MINOR << 8) |
           RIFT_TOKENIZER_VERSION_PATCH;
}

/* Feature detection functions */
bool rift_tokenizer_has_dfa_support(void) {
    return true;
}

bool rift_tokenizer_has_regex_compose(void) {
    return true;
}

bool rift_tokenizer_has_thread_safety(void) {
#ifdef RIFT_ENABLE_THREAD_SAFETY
    return true;
#else
    return false;
#endif
}

bool rift_tokenizer_has_caching(void) {
    return true;
}