/*
 * =================================================================
 * rift_tokenizer.h - RIFT-0 Core Tokenizer Interface (Corrected)
 * RIFT: RIFT Is a Flexible Translator
 * Component: DFA-based tokenization with regex composition
 * OBINexus Computing Framework - Stage 0 Implementation
 * 
 * Toolchain: riftlang.exe → .so.a → rift.exe → gosilang
 * Build Orchestration: nlink → polybuild (AEGIS Framework)
 * =================================================================
 */

#ifndef RIFT_0_CORE_TOKENIZER_H
#define RIFT_0_CORE_TOKENIZER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>

/* =================================================================
 * RIFT-0 VERSION & COMPLIANCE DEFINITIONS
 * =================================================================
 */

#define RIFT_TOKENIZER_VERSION 0x040000
#define RIFT_STAGE_TOKENIZATION 1
#define DEFAULT_THREAD_COUNT 32

/* =================================================================
 * AEGIS METHODOLOGY COMPLIANCE STRUCTURES
 * =================================================================
 */

/* Tokenizer Context Structure - AEGIS Compliant */
typedef struct rift_tokenizer_context {
    uint32_t version;
    bool initialized;
    uint32_t thread_count;
    bool dual_mode_enabled;
    bool aegis_compliant;
    void *stage_data;
    void *next_stage_input;
} rift_tokenizer_context_t;

/* Tokenizer Configuration Structure */
typedef struct rift_tokenizer_config {
    uint32_t processing_flags;
    uint32_t validation_level;
    bool trust_tagging_enabled;
    bool preserve_matched_state;
    char *output_format;
} rift_tokenizer_config_t;

/* Stage Execution Result Enumeration */
typedef enum {
    RIFT_TOKENIZER_SUCCESS = 0,
    RIFT_TOKENIZER_ERROR_INVALID_INPUT = -1,
    RIFT_TOKENIZER_ERROR_PROCESSING = -2,
    RIFT_TOKENIZER_ERROR_VALIDATION = -3,
    RIFT_TOKENIZER_ERROR_MEMORY = -4
} rift_tokenizer_result_t;

/* =================================================================
 * TOKEN DEFINITIONS FOR DFA PROCESSING
 * =================================================================
 */

/* Bitfield Token Format (32-bit packed structure) */
typedef struct {
    uint32_t type    : 8;   /* Token type identifier (0-255) */
    uint32_t mem_ptr : 16;  /* Memory pointer/offset (0-65535) */
    uint32_t value   : 8;   /* Token value/flags (0-255) */
} TokenTriplet;

/* Token Type Enumeration */
typedef enum {
    TOKEN_UNKNOWN = 0,
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_LITERAL_NUMBER,
    TOKEN_LITERAL_STRING,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATION,
    TOKEN_WHITESPACE,
    TOKEN_COMMENT,
    TOKEN_EOF,
    TOKEN_ERROR,
    /* DFA-specific tokens */
    TOKEN_REGEX_START,
    TOKEN_REGEX_END,
    TOKEN_COMPOSE_AND,
    TOKEN_COMPOSE_OR,
    TOKEN_COMPOSE_XOR,
    TOKEN_COMPOSE_NAND,
    TOKEN_DFA_STATE,
    TOKEN_MAX = 255
} TokenType;

/* Token Flags (stored in value field) */
typedef enum {
    TOKEN_FLAG_NONE       = 0x00,
    TOKEN_FLAG_GLOBAL     = 0x01,  /* g flag */
    TOKEN_FLAG_MULTILINE  = 0x02,  /* m flag */
    TOKEN_FLAG_IGNORECASE = 0x04,  /* i flag */
    TOKEN_FLAG_TOPDOWN    = 0x08,  /* t flag */
    TOKEN_FLAG_BOTTOMUP   = 0x10,  /* b flag */
    TOKEN_FLAG_COMPOSED   = 0x20,  /* Composed regex */
    TOKEN_FLAG_VALIDATED = 0x40,  /* DFA validated */
    TOKEN_FLAG_ERROR      = 0x80   /* Error state */
} TokenFlags;

/* =================================================================
 * DFA AUTOMATON STRUCTURES
 * =================================================================
 */

/* DFA State Structure (5-tuple automaton) */
typedef struct DFAState {
    uint32_t state_id;              /* Unique state identifier */
    bool is_final;                  /* Final state flag */
    bool is_start;                  /* Start state flag */
    char transition_char;           /* Transition character */
    struct DFAState* next_state;    /* Next state pointer */
    struct DFAState* fail_state;    /* Failure state pointer */
    TokenType token_type;           /* Associated token type */
    uint32_t match_count;           /* Number of matches */
} DFAState;

/* Regex Composition Structure */
typedef struct {
    char* pattern;                  /* Raw regex pattern */
    TokenFlags flags;               /* Compilation flags */
    DFAState* start_state;          /* DFA start state */
    DFAState* current_state;        /* Current processing state */
    bool is_composed;               /* Composition status */
    size_t pattern_length;          /* Pattern byte length */
} RegexComposition;

/* =================================================================
 * ENHANCED TOKENIZER CONTEXT STRUCTURE
 * =================================================================
 */

/* Extended Tokenizer Context for DFA Processing */
typedef struct {
    const char* input_buffer;       /* Source input buffer */
    size_t buffer_length;           /* Buffer size in bytes */
    size_t current_position;        /* Current parsing position */
    size_t line_number;             /* Current line (1-based) */
    size_t column_number;           /* Current column (1-based) */
    
    /* Token output management */
    TokenTriplet* token_buffer;     /* Output token array */
    size_t token_capacity;          /* Maximum tokens */
    size_t token_count;             /* Current token count */
    
    /* DFA processing state */
    DFAState* dfa_root;             /* Root DFA state machine */
    RegexComposition* compositions; /* Active regex compositions */
    size_t composition_count;       /* Number of compositions */
    
    /* Error handling */
    char error_message[256];        /* Last error description */
    size_t error_position;          /* Error location in input */
    bool has_error;                 /* Error flag */
    
    /* Thread safety */
    void* mutex_handle;             /* Platform-specific mutex */
    bool thread_safe_mode;          /* Thread safety enabled */
} TokenizerContext;

/* =================================================================
 * CORE TOKENIZER API - AEGIS FRAMEWORK COMPATIBLE
 * =================================================================
 */

/* Primary Lifecycle Management */
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

/* Stage-Specific Functions */
rift_tokenizer_result_t rift_tokenizer_set_pattern(rift_tokenizer_context_t *ctx, const char *pattern);
rift_tokenizer_result_t rift_tokenizer_tokenize_input(rift_tokenizer_context_t *ctx, const char *input);

/* Enhanced DFA Processing API */
TokenizerContext* rift_tokenizer_create(size_t initial_capacity);
void rift_tokenizer_destroy(TokenizerContext* ctx);
bool rift_tokenizer_reset(TokenizerContext* ctx);
bool rift_tokenizer_set_input(TokenizerContext* ctx, const char* input, size_t length);
bool rift_tokenizer_set_input_file(TokenizerContext* ctx, const char* filename);

/* Core Tokenization Processing */
bool rift_tokenizer_process_enhanced(TokenizerContext* ctx);
TokenTriplet* rift_tokenizer_get_tokens(TokenizerContext* ctx, size_t* count);
TokenTriplet rift_tokenizer_next_token(TokenizerContext* ctx);

/* =================================================================
 * REGEX COMPOSITION API (R"" and R'' syntax)
 * =================================================================
 */

/* Regex Pattern Compilation */
RegexComposition* rift_regex_compile(const char* pattern, TokenFlags flags);
void rift_regex_destroy(RegexComposition* regex);

/* Boolean Composition Operations */
RegexComposition* rift_regex_compose_and(RegexComposition* a, RegexComposition* b);
RegexComposition* rift_regex_compose_or(RegexComposition* a, RegexComposition* b);
RegexComposition* rift_regex_compose_xor(RegexComposition* a, RegexComposition* b);
RegexComposition* rift_regex_compose_nand(RegexComposition* a, RegexComposition* b);

/* Pattern Matching with DFA */
bool rift_regex_match(RegexComposition* regex, const char* input, size_t length);
bool rift_regex_find(RegexComposition* regex, const char* input, size_t length, 
                     size_t* match_start, size_t* match_length);

/* =================================================================
 * DFA STATE MACHINE API
 * =================================================================
 */

/* DFA Construction */
DFAState* rift_dfa_create_state(uint32_t state_id, bool is_final);
void rift_dfa_destroy_states(DFAState* root);
bool rift_dfa_add_transition(DFAState* from, DFAState* to, char transition_char);

/* DFA Processing */
DFAState* rift_dfa_process_input(DFAState* start, const char* input, size_t length);
bool rift_dfa_is_accepting_state(DFAState* state);
TokenType rift_dfa_get_token_type(DFAState* state);

/* =================================================================
 * UTILITY & VALIDATION API
 * =================================================================
 */

/* Token Utilities */
const char* rift_token_type_name(TokenType type);
const char* rift_token_flags_string(TokenFlags flags);
bool rift_token_is_valid(const TokenTriplet* token);

/* Debug & Introspection */
void rift_tokenizer_print_stats(const rift_tokenizer_context_t* ctx);
void rift_tokenizer_print_tokens(const TokenizerContext* ctx);
bool rift_tokenizer_validate_dfa(const TokenizerContext* ctx);

/* Error Handling */
const char* rift_tokenizer_get_error(const TokenizerContext* ctx);
bool rift_tokenizer_has_error(const TokenizerContext* ctx);
void rift_tokenizer_clear_error(TokenizerContext* ctx);

/* =================================================================
 * THREAD SAFETY API (for Gosilang integration)
 * =================================================================
 */

/* Thread Safety Control */
bool rift_tokenizer_enable_thread_safety(TokenizerContext* ctx);
bool rift_tokenizer_disable_thread_safety(TokenizerContext* ctx);
bool rift_tokenizer_is_thread_safe(const TokenizerContext* ctx);

/* Thread-Safe Processing */
TokenizerContext* rift_tokenizer_clone(const TokenizerContext* source);
bool rift_tokenizer_merge_results(TokenizerContext* target, const TokenizerContext* source);

/* =================================================================
 * CONSTANTS & LIMITS
 * =================================================================
 */

#define RIFT_TOKENIZER_VERSION_MAJOR    1
#define RIFT_TOKENIZER_VERSION_MINOR    0
#define RIFT_TOKENIZER_VERSION_PATCH    0

#define RIFT_MAX_TOKEN_LENGTH           4096
#define RIFT_MAX_PATTERN_LENGTH         1024
#define RIFT_MAX_DFA_STATES             65536
#define RIFT_DEFAULT_TOKEN_CAPACITY     1024
#define RIFT_MAX_COMPOSITIONS           64
#define RIFT_MAX_ERROR_MESSAGE          256

/* Regex Composition Syntax Markers */
#define RIFT_REGEX_RAW_QUOTE            "R\""
#define RIFT_REGEX_RAW_SINGLE           "R'"
#define RIFT_COMPOSE_AND                "R.AND"
#define RIFT_COMPOSE_OR                 "R.OR"
#define RIFT_COMPOSE_XOR                "R.XOR"
#define RIFT_COMPOSE_NAND               "R.NAND"
#define RIFT_COMPOSE_NOT                "R.NOT"

/* =================================================================
 * VERSION & BUILD INFO
 * =================================================================
 */

/* Version Information */
const char* rift_tokenizer_version(void);
const char* rift_tokenizer_build_info(void);
uint32_t rift_tokenizer_version_number(void);

/* Feature Detection */
bool rift_tokenizer_has_dfa_support(void);
bool rift_tokenizer_has_regex_compose(void);
bool rift_tokenizer_has_thread_safety(void);
bool rift_tokenizer_has_caching(void);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_0_CORE_TOKENIZER_H */