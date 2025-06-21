/*
 * =================================================================
 * tokenizer_rules.c - RIFT-0 DFA Rules & Pattern Engine Implementation
 * RIFT: RIFT Is a Flexible Translator
 * Component: DFA transition rules and pattern compilation engine
 * OBINexus Computing Framework - Stage 0 Implementation
 * 
 * R.IMPLEMENT(DFATransitions, PatternCompilation, RuleValidation)
 * R.FLAGS(deterministic, rule_cached, pattern_optimized)
 * R.COMPOSE(StateManagement, TransitionLogic, PatternMatching)
 * 
 * Toolchain: riftlang.exe → .so.a → rift.exe → gosilang
 * Build Orchestration: nlink → polybuild (AEGIS Framework)
 * Author: Nnamdi Michael Okpala & AEGIS Integration Team
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
 * TOKENTRIPLET BITFIELD VALIDATION
 * =================================================================
 */

/* Compile-time assertions for AEGIS compliance */
_Static_assert(sizeof(TokenTriplet) == 4, "TokenTriplet must be exactly 32 bits");

/* =================================================================
 * TOKEN UTILITY FUNCTIONS
 * =================================================================
 */

/**
 * Create TokenTriplet with validation
 * R.CREATE(TokenTriplet) -> Validated bitfield construction
 */
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

/**
 * Validate TokenTriplet integrity
 * R.VALIDATE(TokenTriplet) -> Structural validation
 */
bool rift_token_is_valid(const TokenTriplet* token) {
    if (!token) return false;
    
    /* Basic range validation */
    if (token->type > TOKEN_ERROR) return false;
    if (token->mem_ptr > RIFT_TOKENIZER_MAX_TOKENS) return false;
    
    return true;
}

/**
 * Convert token type to string representation
 * R.STRING(TokenType) -> Human readable representation
 */
const char* rift_token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_UNKNOWN:         return "UNKNOWN";
        case TOKEN_IDENTIFIER:      return "IDENTIFIER";
        case TOKEN_KEYWORD:         return "KEYWORD";
        case TOKEN_LITERAL_STRING:  return "LITERAL_STRING";
        case TOKEN_LITERAL_NUMBER:  return "LITERAL_NUMBER";
        case TOKEN_OPERATOR:        return "OPERATOR";
        case TOKEN_PUNCTUATION:     return "PUNCTUATION";
        case TOKEN_DELIMITER:       return "DELIMITER";
        case TOKEN_WHITESPACE:      return "WHITESPACE";
        case TOKEN_COMMENT:         return "COMMENT";
        case TOKEN_EOF:             return "EOF";
        case TOKEN_ERROR:           return "ERROR";
        default:                    return "INVALID";
    }
}

/* =================================================================
 * DFA STATE MANAGEMENT FUNCTIONS
 * =================================================================
 */

/**
 * Create new DFA state
 * R.CREATE(DFAState) -> State machine node construction
 */
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

/**
 * Add transition between DFA states
 * R.TRANSITION(from, to, char) -> State connection establishment
 */
void rift_dfa_add_transition(DFAState* from, DFAState* to, char transition_char) {
    if (!from || !to) {
        return;
    }
    
    from->transition_char = transition_char;
    from->next_state = to;
}

/**
 * Process input through DFA
 * R.PROCESS(DFAState, input) -> Input consumption via state machine
 */
DFAState* rift_dfa_process_input(DFAState* start, const char* input, size_t length) {
    if (!start || !input) return NULL;
    
    DFAState* current = start;
    
    for (size_t i = 0; i < length; i++) {
        char ch = input[i];
        
        if (current->transition_char == ch && current->next_state) {
            current = current->next_state;
            current->match_count++;
        } else if (current->fail_state) {
            current = current->fail_state;
        } else {
            break;
        }
    }
    
    return current;
}

/**
 * Check if DFA state is accepting
 * R.ACCEPTING(DFAState) -> Final state validation
 */
bool rift_dfa_is_accepting_state(DFAState* state) {
    return state && state->is_final;
}

/**
 * Get token type for DFA state
 * R.TOKEN_TYPE(DFAState) -> Associated token type retrieval
 */
TokenType rift_dfa_get_token_type(DFAState* state) {
    return state ? state->token_type : TOKEN_UNKNOWN;
}

/**
 * Destroy DFA state tree
 * R.DESTROY(DFAState) -> Recursive state machine cleanup
 */
void rift_dfa_destroy_states(DFAState* root) {
    if (!root) return;
    
    /* Recursive cleanup with cycle detection */
    if (root->next_state && root->next_state != root) {
        rift_dfa_destroy_states(root->next_state);
    }
    
    /* Note: fail_state typically points to existing states, 
     * so we don't recursively delete it to avoid double-free */
    
    free(root);
}

/* =================================================================
 * REGEX COMPOSITION FUNCTIONS
 * =================================================================
 */

/**
 * Compile regex pattern into DFA
 * R.COMPILE(pattern, flags) -> Pattern to DFA transformation
 */
RegexComposition* rift_regex_compile(const char* pattern, TokenFlags flags) {
    if (!pattern) return NULL;
    
    RegexComposition* comp = malloc(sizeof(RegexComposition));
    if (!comp) return NULL;
    
    /* Initialize composition structure */
    comp->pattern = strdup(pattern);
    if (!comp->pattern) {
        free(comp);
        return NULL;
    }
    
    comp->flags = flags;
    comp->pattern_length = strlen(pattern);
    comp->is_composed = false;
    
    /* Create basic DFA for simple patterns */
    uint32_t state_id = 0;
    comp->start_state = rift_dfa_create_state(state_id++, false);
    if (!comp->start_state) {
        free(comp->pattern);
        free(comp);
        return NULL;
    }
    
    comp->start_state->is_start = true;
    
    /* Build simple character-by-character DFA */
    DFAState* current = comp->start_state;
    
    for (size_t i = 0; i < comp->pattern_length; i++) {
        char ch = pattern[i];
        
        DFAState* next_state = rift_dfa_create_state(state_id++, 
                                                     (i == comp->pattern_length - 1));
        if (!next_state) {
            rift_dfa_destroy_states(comp->start_state);
            free(comp->pattern);
            free(comp);
            return NULL;
        }
        
        rift_dfa_add_transition(current, next_state, ch);
        current = next_state;
    }
    
    /* Set final state properties */
    if (current && current->is_final) {
        current->token_type = TOKEN_IDENTIFIER; /* Default classification */
    }
    
    comp->current_state = comp->start_state;
    comp->is_composed = true;
    
    return comp;
}

/**
 * Test regex match against input
 * R.MATCH(RegexComposition, input) -> Pattern matching validation
 */
bool rift_regex_match(const RegexComposition* regex, const char* input, size_t length) {
    if (!regex || !input || !regex->start_state) {
        return false;
    }
    
    DFAState* final_state = rift_dfa_process_input(regex->start_state, input, length);
    return rift_dfa_is_accepting_state(final_state);
}

/**
 * Destroy regex composition
 * R.DESTROY(RegexComposition) -> Pattern cleanup
 */
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
 * PATTERN CACHE MANAGEMENT
 * =================================================================
 */

/* Simple pattern cache for demonstration */
static struct {
    char* pattern_names[RIFT_TOKENIZER_MAX_PATTERNS];
    RegexComposition* patterns[RIFT_TOKENIZER_MAX_PATTERNS];
    size_t count;
} pattern_cache = {0};

/**
 * Cache compiled pattern by name
 * R.CACHE(pattern_name, RegexComposition) -> Pattern storage
 */
bool rift_tokenizer_cache_pattern(TokenizerContext* ctx, const char* name,
                                   const char* pattern, TokenFlags flags) {
    if (!ctx || !name || !pattern || pattern_cache.count >= RIFT_TOKENIZER_MAX_PATTERNS) {
        return false;
    }
    
    /* Compile pattern */
    RegexComposition* comp = rift_regex_compile(pattern, flags);
    if (!comp) {
        return false;
    }
    
    /* Store in cache */
    size_t index = pattern_cache.count;
    pattern_cache.pattern_names[index] = strdup(name);
    pattern_cache.patterns[index] = comp;
    pattern_cache.count++;
    
    /* Update context statistics */
    ctx->stats.regex_patterns = pattern_cache.count;
    
    return true;
}

/**
 * Retrieve cached pattern by name
 * R.LOOKUP(pattern_name) -> Cached pattern retrieval
 */
RegexComposition* rift_tokenizer_get_cached_pattern(TokenizerContext* ctx, const char* name) {
    if (!ctx || !name) return NULL;
    
    for (size_t i = 0; i < pattern_cache.count; i++) {
        if (pattern_cache.pattern_names[i] && 
            strcmp(pattern_cache.pattern_names[i], name) == 0) {
            return pattern_cache.patterns[i];
        }
    }
    
    return NULL;
}

/* =================================================================
 * THREAD SAFETY FUNCTIONS
 * =================================================================
 */

/**
 * Enable thread safety for tokenizer context
 * R.THREAD_SAFE(enable) -> Mutex initialization
 */
bool rift_tokenizer_enable_thread_safety(TokenizerContext* ctx) {
    if (!ctx) return false;
    
    #ifdef RIFT_THREAD_SUPPORT
    if (!ctx->mutex) {
        ctx->mutex = malloc(sizeof(pthread_mutex_t));
        if (!ctx->mutex) return false;
        
        if (pthread_mutex_init((pthread_mutex_t*)ctx->mutex, NULL) != 0) {
            free(ctx->mutex);
            ctx->mutex = NULL;
            return false;
        }
    }
    ctx->thread_safe = true;
    return true;
    #else
    return false;
    #endif
}

/**
 * Disable thread safety
 * R.THREAD_SAFE(disable) -> Mutex cleanup
 */
bool rift_tokenizer_disable_thread_safety(TokenizerContext* ctx) {
    if (!ctx) return false;
    
    ctx->thread_safe = false;
    
    #ifdef RIFT_THREAD_SUPPORT
    if (ctx->mutex) {
        pthread_mutex_destroy((pthread_mutex_t*)ctx->mutex);
        free(ctx->mutex);
        ctx->mutex = NULL;
    }
    #endif
    
    return true;
}

/**
 * Check if thread safety is enabled
 * R.THREAD_SAFE(check) -> Thread safety status
 */
bool rift_tokenizer_is_thread_safe(const TokenizerContext* ctx) {
    return ctx ? ctx->thread_safe : false;
}