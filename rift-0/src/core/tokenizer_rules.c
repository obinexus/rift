/**
 * =================================================================
 * tokenizer_rules.c - RIFT-0 Complete DFA Tokenization Engine
 * RIFT: RIFT Is a Flexible Translator
 * Component: DFA-based tokenization with R"" pattern support and R-macros
 * OBINexus Computing Framework - Stage 0 Implementation
 * =================================================================
 */

#include "rift-0/core/tokenizer_rules.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <pthread.h>
#include <stdatomic.h>

/* Thread-local storage for tokenizer state */
static __thread bool encoder_initialized = false;
static __thread char error_buffer[512];
static __thread TokenTriplet* local_token_buffer = NULL;
static __thread size_t local_buffer_capacity = 0;

/* Global governance and thread safety */
static pthread_mutex_t tokenizer_mutex = PTHREAD_MUTEX_INITIALIZER;
static atomic_bool g_tokenizer_initialized = false;
static atomic_int g_active_threads = 0;

/* Pattern cache for compiled R"" expressions */
#define MAX_COMPILED_PATTERNS 256
static CompiledPattern g_compiled_patterns[MAX_COMPILED_PATTERNS];
static atomic_size_t g_pattern_count = 0;

/* Governance policy enforcement */
#define POLIC_CHECK(condition, message) \
    do { \
        if (!(condition)) { \
            snprintf(error_buffer, sizeof(error_buffer), "PoliC: %s", message); \
            fprintf(stderr, "RIFT-0 Policy Violation: %s\n", message); \
            return -1; \
        } \
    } while(0)

/**
 * DFA State Machine Structure for R"" pattern processing
 */
typedef struct {
    DFAState current_state;
    uint32_t flags;
    char delimiter[16];
    size_t delimiter_len;
    char* content_buffer;
    size_t content_size;
    size_t content_capacity;
    bool in_raw_mode;
    pthread_mutex_t state_mutex;
} DFAStateMachine;

/**
 * Thread-safe semaphore context for top-down/bottom-up switching
 */
typedef struct {
    atomic_int top_down_active;    // 101 signal
    atomic_int bottom_up_active;   // 010 signal
    pthread_cond_t context_switch;
    pthread_mutex_t switch_mutex;
} ThreadContext;

static ThreadContext g_thread_context = {
    .top_down_active = 0,
    .bottom_up_active = 0,
    .context_switch = PTHREAD_COND_INITIALIZER,
    .switch_mutex = PTHREAD_MUTEX_INITIALIZER
};

/* Character classification inline functions */
static inline bool is_identifier_start(char c) {
    return isalpha(c) || c == '_';
}

static inline bool is_identifier_continue(char c) {
    return isalnum(c) || c == '_';
}

static inline bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static inline bool is_operator_char(char c) {
    return strchr("+-*/%=<>!&|^~", c) != NULL;
}

static inline bool is_delimiter_char(char c) {
    return strchr("(){}[];,.", c) != NULL;
}

/**
 * R-Macro Implementation: Pattern Composition Functions
 */

/* R.compose - Merge two patterns into composite matcher */
static CompiledPattern* r_compose(const CompiledPattern* p1, const CompiledPattern* p2) {
    if (!p1 || !p2) return NULL;
    
    CompiledPattern* composite = malloc(sizeof(CompiledPattern));
    if (!composite) return NULL;
    
    // Allocate combined pattern buffer
    size_t total_size = p1->pattern_size + p2->pattern_size + 16;
    composite->pattern_data = malloc(total_size);
    if (!composite->pattern_data) {
        free(composite);
        return NULL;
    }
    
    // Combine flags (bitwise OR for composition)
    composite->flags = p1->flags | p2->flags;
    composite->pattern_size = total_size;
    composite->token_type = TOKEN_R_PATTERN;
    composite->ref_count = 1;
    
    // Create composite DFA (simplified union construction)
    memcpy(composite->pattern_data, p1->pattern_data, p1->pattern_size);
    memcpy((char*)composite->pattern_data + p1->pattern_size, p2->pattern_data, p2->pattern_size);
    
    return composite;
}

/* R.aggregate - Collect multiple pattern results */
static int r_aggregate(CompiledPattern** patterns, size_t count, TokenTriplet* results, size_t* result_count) {
    if (!patterns || !results || !result_count) return -1;
    
    *result_count = 0;
    for (size_t i = 0; i < count && *result_count < MAX_TOKENS_PER_AGGREGATE; i++) {
        if (patterns[i] && patterns[i]->last_match_valid) {
            results[*result_count] = patterns[i]->last_token;
            (*result_count)++;
        }
    }
    
    return 0;
}

/* R.AND - Boolean intersection of pattern matches */
static bool r_and(const CompiledPattern** patterns, size_t count, const char* text, size_t pos) {
    for (size_t i = 0; i < count; i++) {
        if (!patterns[i] || !match_pattern_at_position(patterns[i], text, pos)) {
            return false;
        }
    }
    return count > 0;
}

/* R.OR - Boolean union of pattern matches */
static bool r_or(const CompiledPattern** patterns, size_t count, const char* text, size_t pos) {
    for (size_t i = 0; i < count; i++) {
        if (patterns[i] && match_pattern_at_position(patterns[i], text, pos)) {
            return true;
        }
    }
    return false;
}

/* R.XOR - Boolean exclusive or of pattern matches */
static bool r_xor(const CompiledPattern** patterns, size_t count, const char* text, size_t pos) {
    bool found_match = false;
    for (size_t i = 0; i < count; i++) {
        if (patterns[i] && match_pattern_at_position(patterns[i], text, pos)) {
            if (found_match) return false; // More than one match = false for XOR
            found_match = true;
        }
    }
    return found_match;
}

/* R.NAND - Boolean not-and of pattern matches */
static bool r_nand(const CompiledPattern** patterns, size_t count, const char* text, size_t pos) {
    return !r_and(patterns, count, text, pos);
}

/**
 * DFA State Progression with Thread-Safe Context Switching
 */
static DFAState process_dfa_transition(DFAStateMachine* dfa, char input_char, 
                                     const char* text, size_t position) {
    if (!dfa) return DFA_STATE_ERROR;
    
    pthread_mutex_lock(&dfa->state_mutex);
    
    switch (dfa->current_state) {
        case DFA_STATE_INIT:
            if (input_char == 'R') {
                dfa->current_state = DFA_STATE_R_DETECTED;
            } else if (is_identifier_start(input_char)) {
                dfa->current_state = DFA_STATE_IDENTIFIER;
            } else if (isdigit(input_char)) {
                dfa->current_state = DFA_STATE_NUMBER;
            } else if (is_whitespace(input_char)) {
                dfa->current_state = DFA_STATE_WHITESPACE;
            } else if (is_operator_char(input_char)) {
                dfa->current_state = DFA_STATE_OPERATOR;
            } else if (is_delimiter_char(input_char)) {
                dfa->current_state = DFA_STATE_DELIMITER;
            } else {
                dfa->current_state = DFA_STATE_ERROR;
            }
            break;
            
        case DFA_STATE_R_DETECTED:
            if (input_char == '"') {
                dfa->current_state = DFA_STATE_R_QUOTE_START;
                dfa->delimiter_len = 0;
            } else if (input_char == '\'') {
                dfa->current_state = DFA_STATE_R_SQUOTE_START;
                dfa->delimiter_len = 0;
            } else {
                // Not an R-pattern, backtrack to identifier
                dfa->current_state = DFA_STATE_IDENTIFIER;
            }
            break;
            
        case DFA_STATE_R_QUOTE_START:
            // Capture delimiter until closing quote
            if (input_char != '"' && dfa->delimiter_len < sizeof(dfa->delimiter) - 1) {
                dfa->delimiter[dfa->delimiter_len++] = input_char;
                dfa->delimiter[dfa->delimiter_len] = '\0';
            } else if (input_char == '"') {
                dfa->current_state = DFA_STATE_R_CONTENT;
                dfa->in_raw_mode = true;
            }
            break;
            
        case DFA_STATE_R_CONTENT:
            // Check for termination sequence
            if (input_char == '"' && position > 0) {
                // Look for delimiter match before quote
                size_t delim_start = position - dfa->delimiter_len;
                if (delim_start >= 0 && strncmp(text + delim_start, dfa->delimiter, dfa->delimiter_len) == 0) {
                    dfa->current_state = DFA_STATE_R_PATTERN_END;
                    dfa->in_raw_mode = false;
                }
            }
            // Capture content
            if (dfa->in_raw_mode && dfa->content_size < dfa->content_capacity - 1) {
                dfa->content_buffer[dfa->content_size++] = input_char;
            }
            break;
            
        case DFA_STATE_IDENTIFIER:
            if (!is_identifier_continue(input_char)) {
                dfa->current_state = DFA_STATE_ACCEPTING;
            }
            break;
            
        case DFA_STATE_NUMBER:
            if (!isdigit(input_char) && input_char != '.') {
                dfa->current_state = DFA_STATE_ACCEPTING;
            }
            break;
            
        case DFA_STATE_WHITESPACE:
            if (!is_whitespace(input_char)) {
                dfa->current_state = DFA_STATE_ACCEPTING;
            }
            break;
            
        default:
            dfa->current_state = DFA_STATE_ACCEPTING;
            break;
    }
    
    DFAState result = dfa->current_state;
    pthread_mutex_unlock(&dfa->state_mutex);
    return result;
}

/**
 * Thread-Safe Context Switching for Top-Down/Bottom-Up Processing
 */
static void signal_context_switch(bool is_top_down) {
    pthread_mutex_lock(&g_thread_context.switch_mutex);
    
    if (is_top_down) {
        atomic_store(&g_thread_context.top_down_active, 101); // Semaphore signal
        atomic_store(&g_thread_context.bottom_up_active, 0);
    } else {
        atomic_store(&g_thread_context.bottom_up_active, 010); // Semaphore signal  
        atomic_store(&g_thread_context.top_down_active, 0);
    }
    
    pthread_cond_broadcast(&g_thread_context.context_switch);
    pthread_mutex_unlock(&g_thread_context.switch_mutex);
}

static bool wait_for_context(bool need_top_down) {
    pthread_mutex_lock(&g_thread_context.switch_mutex);
    
    while (true) {
        int top_down = atomic_load(&g_thread_context.top_down_active);
        int bottom_up = atomic_load(&g_thread_context.bottom_up_active);
        
        if (need_top_down && top_down == 101) {
            pthread_mutex_unlock(&g_thread_context.switch_mutex);
            return true;
        } else if (!need_top_down && bottom_up == 010) {
            pthread_mutex_unlock(&g_thread_context.switch_mutex);
            return true;
        }
        
        pthread_cond_wait(&g_thread_context.context_switch, &g_thread_context.switch_mutex);
    }
}

/**
 * Core Tokenization Function with R-Pattern Support
 */
int match_token_pattern(const char* src, const char* pattern, 
                       uint32_t flags, PatternMatchResult* result) {
    POLIC_CHECK(src != NULL, "Source text cannot be NULL");
    POLIC_CHECK(pattern != NULL, "Pattern cannot be NULL");
    POLIC_CHECK(result != NULL, "Result structure cannot be NULL");
    
    if (!atomic_load(&g_tokenizer_initialized)) {
        return -1;
    }
    
    // Initialize result
    memset(result, 0, sizeof(PatternMatchResult));
    
    // Check for top-down/bottom-up flag requirements
    bool needs_top_down = (flags & DFA_FLAG_TOP_DOWN) != 0;
    bool needs_bottom_up = (flags & DFA_FLAG_BOTTOM_UP) != 0;
    
    if (needs_top_down && needs_bottom_up) {
        // Concurrent processing required
        signal_context_switch(true); // Start with top-down
    }
    
    // Create DFA state machine
    DFAStateMachine dfa;
    memset(&dfa, 0, sizeof(dfa));
    dfa.current_state = DFA_STATE_INIT;
    dfa.flags = flags;
    pthread_mutex_init(&dfa.state_mutex, NULL);
    
    // Allocate content buffer for R-patterns
    dfa.content_capacity = 1024;
    dfa.content_buffer = malloc(dfa.content_capacity);
    if (!dfa.content_buffer) {
        pthread_mutex_destroy(&dfa.state_mutex);
        return -1;
    }
    
    size_t src_len = strlen(src);
    size_t start_pos = 0;
    
    // Process characters through DFA
    for (size_t i = 0; i < src_len; i++) {
        DFAState new_state = process_dfa_transition(&dfa, src[i], src, i);
        
        if (new_state == DFA_STATE_ACCEPTING || new_state == DFA_STATE_R_PATTERN_END) {
            // Token complete - determine type and create triplet
            TokenType token_type = TOKEN_UNKNOWN;
            
            if (new_state == DFA_STATE_R_PATTERN_END) {
                token_type = TOKEN_R_PATTERN;
            } else {
                // Classify based on first character and content
                char first_char = src[start_pos];
                if (is_identifier_start(first_char)) {
                    // Check for keywords
                    size_t token_len = i - start_pos;
                    if (strncmp(src + start_pos, "NULL", token_len) == 0 && token_len == 4) {
                        token_type = TOKEN_NULL_KEYWORD;
                    } else if (strncmp(src + start_pos, "nil", token_len) == 0 && token_len == 3) {
                        token_type = TOKEN_NIL_KEYWORD;
                    } else {
                        token_type = TOKEN_IDENTIFIER;
                    }
                } else if (isdigit(first_char)) {
                    token_type = TOKEN_LITERAL_NUMBER;
                } else if (is_operator_char(first_char)) {
                    token_type = TOKEN_OPERATOR;
                } else if (is_delimiter_char(first_char)) {
                    token_type = TOKEN_DELIMITER;
                } else if (is_whitespace(first_char)) {
                    token_type = TOKEN_WHITESPACE;
                }
            }
            
            // Create TokenTriplet
            result->token.type = (uint32_t)token_type;
            result->token.mem_ptr = (uint32_t)(start_pos & 0xFFFF); // 16-bit memory pointer
            result->token.value = (uint32_t)((i - start_pos) & 0xFF); // Token length as value
            result->match_length = i - start_pos;
            result->success = true;
            result->error_msg = NULL;
            
            break;
        } else if (new_state == DFA_STATE_ERROR) {
            result->success = false;
            result->error_msg = "DFA transition error";
            break;
        }
    }
    
    // Cleanup
    free(dfa.content_buffer);
    pthread_mutex_destroy(&dfa.state_mutex);
    
    return result->success ? 0 : -1;
}

/**
 * Full Source Tokenization
 */
int tokenize_source(const char* src, TokenTriplet* tokens, 
                   size_t max_tokens, size_t* token_count) {
    POLIC_CHECK(src != NULL, "Source text cannot be NULL");
    POLIC_CHECK(tokens != NULL, "Token array cannot be NULL");
    POLIC_CHECK(token_count != NULL, "Token count pointer cannot be NULL");
    POLIC_CHECK(max_tokens > 0, "Maximum token count must be positive");
    
    *token_count = 0;
    size_t src_len = strlen(src);
    size_t pos = 0;
    
    while (pos < src_len && *token_count < max_tokens) {
        // Skip leading whitespace for efficiency
        while (pos < src_len && is_whitespace(src[pos])) {
            pos++;
        }
        
        if (pos >= src_len) break;
        
        PatternMatchResult result;
        int match_result = match_token_pattern(src + pos, "", 0, &result);
        
        if (match_result == 0 && result.success) {
            tokens[*token_count] = result.token;
            (*token_count)++;
            pos += result.match_length;
        } else {
            // Skip unrecognized character
            pos++;
        }
    }
    
    return 0;
}

/**
 * Initialize Tokenizer Rules Engine
 */
int init_tokenizer_rules(void) {
    if (atomic_load(&g_tokenizer_initialized)) {
        return 0; // Already initialized
    }
    
    pthread_mutex_lock(&tokenizer_mutex);
    
    // Initialize governance system
    printf("LOG: Initializing RIFT-0 PoliC governance system\n");
    
    // Validate TokenTriplet structure alignment
    POLIC_CHECK(sizeof(TokenTriplet) == 4, "TokenTriplet must be exactly 4 bytes");
    POLIC_CHECK(sizeof(TokenTriplet) % 4 == 0, "TokenTriplet must be 4-byte aligned");
    
    // Initialize pattern cache
    memset(g_compiled_patterns, 0, sizeof(g_compiled_patterns));
    atomic_store(&g_pattern_count, 0);
    
    // Initialize thread context
    atomic_store(&g_thread_context.top_down_active, 0);
    atomic_store(&g_thread_context.bottom_up_active, 0);
    
    atomic_store(&g_tokenizer_initialized, true);
    
    pthread_mutex_unlock(&tokenizer_mutex);
    
    printf("LOG: RIFT-0 tokenizer rules engine initialized successfully\n");
    return 0;
}

/**
 * Cleanup Tokenizer Rules Engine
 */
void cleanup_tokenizer_rules(void) {
    if (!atomic_load(&g_tokenizer_initialized)) {
        return;
    }
    
    pthread_mutex_lock(&tokenizer_mutex);
    
    // Cleanup thread-local buffers
    if (local_token_buffer) {
        free(local_token_buffer);
        local_token_buffer = NULL;
        local_buffer_capacity = 0;
    }
    
    // Cleanup compiled patterns
    for (size_t i = 0; i < atomic_load(&g_pattern_count); i++) {
        if (g_compiled_patterns[i].pattern_data) {
            free(g_compiled_patterns[i].pattern_data);
            g_compiled_patterns[i].pattern_data = NULL;
        }
    }
    
    atomic_store(&g_pattern_count, 0);
    atomic_store(&g_tokenizer_initialized, false);
    
    pthread_mutex_unlock(&tokenizer_mutex);
    
    printf("LOG: RIFT-0 tokenizer rules engine cleanup complete\n");
}