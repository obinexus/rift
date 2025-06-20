/*
 * =================================================================
 * tokenizer_rules.h - RIFT-0 Enhanced Tokenizer Rules Interface
 * RIFT: RIFT Is a Flexible Translator
 * Component: DFA-based tokenization with R-syntax composition
 * OBINexus Computing Framework - Stage 0 Core Definitions
 * 
 * Defines comprehensive tokenization rule system with deterministic
 * finite automaton patterns, R-syntax regex composition, and 
 * TokenTriplet bitfield structures for AEGIS governance compliance.
 * 
 * Author: OBINexus Nnamdi Michael Okpala
 * Framework: AEGIS with nlink → polybuild orchestration
 * =================================================================
 */

#ifndef RIFT_0_CORE_TOKENIZER_RULES_H
#define RIFT_0_CORE_TOKENIZER_RULES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

/* =================================================================
 * VERSION AND BUILD CONFIGURATION
 * =================================================================
 */

#define RIFT_TOKENIZER_VERSION_MAJOR    1
#define RIFT_TOKENIZER_VERSION_MINOR    0
#define RIFT_TOKENIZER_VERSION_PATCH    0

#define RIFT_VERSION_STRING "1.0.0"

/* Build feature flags */
#ifdef RIFT_DFA_ENABLED
#define RIFT_HAS_DFA_SUPPORT 1
#else
#define RIFT_HAS_DFA_SUPPORT 0
#endif

#ifdef RIFT_REGEX_COMPOSE
#define RIFT_HAS_REGEX_COMPOSE 1
#else
#define RIFT_HAS_REGEX_COMPOSE 0
#endif

#ifdef RIFT_TOKEN_TRIPLET
#define RIFT_HAS_TOKEN_TRIPLET 1
#else
#define RIFT_HAS_TOKEN_TRIPLET 0
#endif

/* =================================================================
 * TOKENTRIPLET BITFIELD STRUCTURE
 * =================================================================
 */

/* 32-bit packed TokenTriplet for deterministic memory layout */
typedef struct __attribute__((packed)) {
    uint32_t type    : 8;   /* Token type identifier (0-255) */
    uint32_t mem_ptr : 16;  /* Memory pointer/offset (0-65535) */
    uint32_t value   : 8;   /* Token value/flags (0-255) */
} TokenTriplet;

/* Static assertions for bitfield integrity */
_Static_assert(sizeof(TokenTriplet) == 4, 
               "TokenTriplet must be exactly 32 bits for deterministic builds");

/* Token Type Classification System */
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
    
    /* R-syntax specific token types */
    TOKEN_REGEX_START,       /* R" or R' marker */
    TOKEN_REGEX_END,         /* Closing quote marker */
    TOKEN_COMPOSE_AND,       /* R.AND composition */
    TOKEN_COMPOSE_OR,        /* R.OR composition */
    TOKEN_COMPOSE_XOR,       /* R.XOR composition */
    TOKEN_COMPOSE_NAND,      /* R.NAND composition */
    TOKEN_COMPOSE_NOT,       /* R.NOT composition */
    
    /* DFA state machine tokens */
    TOKEN_DFA_STATE,         /* DFA state transition */
    TOKEN_DFA_ACCEPT,        /* DFA acceptance state */
    TOKEN_DFA_REJECT,        /* DFA rejection state */
    
    TOKEN_MAX = 255          /* Maximum token type value */
} TokenType;

/* Token Flag System (stored in value field) */
typedef enum {
    TOKEN_FLAG_NONE       = 0x00,
    
    /* R-syntax pattern flags */
    TOKEN_FLAG_GLOBAL     = 0x01,  /* g flag - global matching */
    TOKEN_FLAG_MULTILINE  = 0x02,  /* m flag - multiline mode */
    TOKEN_FLAG_IGNORECASE = 0x04,  /* i flag - case insensitive */
    TOKEN_FLAG_TOPDOWN    = 0x08,  /* t flag - top-down evaluation */
    TOKEN_FLAG_BOTTOMUP   = 0x10,  /* b flag - bottom-up evaluation */
    
    /* Composition and validation flags */
    TOKEN_FLAG_COMPOSED   = 0x20,  /* Token from composed regex */
    TOKEN_FLAG_VALIDATED  = 0x40,  /* DFA validated token */
    TOKEN_FLAG_ERROR      = 0x80   /* Error state marker */
} TokenFlags;

/* =================================================================
 * DFA STATE MACHINE STRUCTURES
 * =================================================================
 */

/* Forward declaration */
struct DFAState;

/* DFA State Structure (5-tuple automaton: Q, Σ, δ, q0, F) */
typedef struct DFAState {
    uint32_t state_id;              /* Unique state identifier (Q) */
    bool is_final;                  /* Final state flag (F) */
    bool is_start;                  /* Start state flag (q0) */
    char transition_char;           /* Transition character (Σ) */
    struct DFAState* next_state;    /* Next state pointer (δ) */
    struct DFAState* fail_state;    /* Failure state pointer */
    TokenType token_type;           /* Associated token type */
    uint32_t match_count;           /* Match counter for validation */
} DFAState;

/* DFA Transition Table Entry */
typedef struct {
    uint32_t from_state;
    uint32_t to_state;
    char input_char;
    TokenType emit_token;
} DFATransition;

/* =================================================================
 * R-SYNTAX REGEX COMPOSITION STRUCTURES
 * =================================================================
 */

/* Regex Composition Structure for R"" and R'' patterns */
typedef struct {
    char* pattern;                  /* Raw regex pattern string */
    size_t pattern_length;          /* Pattern byte length */
    TokenFlags flags;               /* Compilation flags */
    DFAState* start_state;          /* DFA start state */
    DFAState* current_state;        /* Current processing state */
    bool is_composed;               /* Composition status flag */
    uint32_t composition_id;        /* Unique composition identifier */
} RegexComposition;

/* R-syntax composition operators */
typedef enum {
    R_OP_NONE = 0,
    R_OP_AND,                       /* R.AND boolean intersection */
    R_OP_OR,                        /* R.OR boolean union */
    R_OP_XOR,                       /* R.XOR exclusive or */
    R_OP_NAND,                      /* R.NAND negated and */
    R_OP_NOT                        /* R.NOT negation */
} RCompositionOperator;

/* =================================================================
 * TOKENIZER CONTEXT AND STATE MANAGEMENT
 * =================================================================
 */

/* Tokenizer Context for stateful processing */
typedef struct {
    /* Input management */
    const char* input_buffer;       /* Source input buffer */
    size_t buffer_length;           /* Buffer size in bytes */
    size_t current_position;        /* Current parsing position */
    size_t line_number;             /* Current line (1-based) */
    size_t column_number;           /* Current column (1-based) */
    
    /* Token output management */
    TokenTriplet* token_buffer;     /* Output token array */
    size_t token_capacity;          /* Maximum token capacity */
    size_t token_count;             /* Current token count */
    
    /* DFA processing state */
    DFAState* dfa_root;             /* Root DFA state machine */
    RegexComposition** compositions; /* Active regex compositions */
    size_t composition_count;       /* Number of active compositions */
    size_t composition_capacity;    /* Maximum compositions */
    
    /* R-syntax pattern cache */
    struct {
        char* name;
        RegexComposition* composition;
    } pattern_cache[RIFT_MAX_COMPOSITIONS];
    size_t cache_count;
    
    /* Error handling and diagnostics */
    char error_message[RIFT_MAX_ERROR_MESSAGE];
    size_t error_position;          /* Error location in input */
    bool has_error;                 /* Error flag */
    
    /* Thread safety and concurrency */
    void* mutex_handle;             /* Platform-specific mutex */
    bool thread_safe_mode;          /* Thread safety enabled */
    
    /* Performance monitoring */
    uint64_t total_tokens;
    uint64_t total_characters;
    uint64_t processing_time_ns;
    uint64_t dfa_transitions;
    uint64_t cache_hits;
    uint64_t cache_misses;
} TokenizerContext;

/* Performance statistics structure */
typedef struct {
    uint64_t total_tokens;
    uint64_t total_characters;
    uint64_t processing_time_ns;
    uint64_t dfa_transitions;
    uint64_t cache_hits;
    uint64_t cache_misses;
} TokenizerStats;

/* =================================================================
 * CONSTANTS AND LIMITS
 * =================================================================
 */

#define RIFT_MAX_TOKEN_LENGTH           4096
#define RIFT_MAX_PATTERN_LENGTH         1024
#define RIFT_MAX_DFA_STATES             65536
#define RIFT_DEFAULT_TOKEN_CAPACITY     1024
#define RIFT_MAX_COMPOSITIONS           64
#define RIFT_MAX_ERROR_MESSAGE          256
#define RIFT_MAX_CACHE_ENTRIES          32

/* R-syntax pattern markers */
#define RIFT_REGEX_RAW_QUOTE            "R\""
#define RIFT_REGEX_RAW_SINGLE           "R'"
#define RIFT_COMPOSE_AND                "R.AND"
#define RIFT_COMPOSE_OR                 "R.OR"
#define RIFT_COMPOSE_XOR                "R.XOR"
#define RIFT_COMPOSE_NAND               "R.NAND"
#define RIFT_COMPOSE_NOT                "R.NOT"

/* DFA construction limits */
#define RIFT_DFA_MAX_STATES             1024
#define RIFT_DFA_MAX_TRANSITIONS        4096
#define RIFT_DFA_STACK_SIZE             256

/* =================================================================
 * CORE API FUNCTION DECLARATIONS
 * =================================================================
 */

/* TokenTriplet management functions */
TokenTriplet rift_token_create(uint8_t type, uint16_t mem_ptr, uint8_t value);
bool rift_token_is_valid(const TokenTriplet* token);
TokenFlags rift_token_get_flags(const TokenTriplet* token);
void rift_token_set_flags(TokenTriplet* token, TokenFlags flags);

/* DFA state machine functions */
DFAState* rift_dfa_create_state(uint32_t state_id, bool is_final);
void rift_dfa_destroy_states(DFAState* root);
bool rift_dfa_add_transition(DFAState* from, DFAState* to, char transition_char);
DFAState* rift_dfa_process_input(DFAState* start, const char* input, size_t length);
bool rift_dfa_is_accepting_state(DFAState* state);
TokenType rift_dfa_get_token_type(DFAState* state);

/* R-syntax regex composition functions */
RegexComposition* rift_regex_compile(const char* pattern, TokenFlags flags);
void rift_regex_destroy(RegexComposition* regex);
RegexComposition* rift_regex_compose_and(RegexComposition* a, RegexComposition* b);
RegexComposition* rift_regex_compose_or(RegexComposition* a, RegexComposition* b);
RegexComposition* rift_regex_compose_xor(RegexComposition* a, RegexComposition* b);
RegexComposition* rift_regex_compose_nand(RegexComposition* a, RegexComposition* b);
bool rift_regex_match(RegexComposition* regex, const char* input, size_t length);
bool rift_regex_find(RegexComposition* regex, const char* input, size_t length,
                     size_t* match_start, size_t* match_length);

/* Tokenizer context management */
TokenizerContext* rift_tokenizer_create(size_t initial_capacity);
void rift_tokenizer_destroy(TokenizerContext* ctx);
bool rift_tokenizer_reset(TokenizerContext* ctx);
bool rift_tokenizer_set_input(TokenizerContext* ctx, const char* input, size_t length);
bool rift_tokenizer_set_input_file(TokenizerContext* ctx, const char* filename);

/* Core tokenization processing */
bool rift_tokenizer_process(TokenizerContext* ctx);
TokenTriplet* rift_tokenizer_get_tokens(TokenizerContext* ctx, size_t* count);
TokenTriplet rift_tokenizer_next_token(TokenizerContext* ctx);

/* Pattern caching and management */
bool rift_tokenizer_cache_pattern(TokenizerContext* ctx, const char* name,
                                  const char* pattern, TokenFlags flags);
RegexComposition* rift_tokenizer_get_cached_pattern(TokenizerContext* ctx, const char* name);

/* Error handling */
const char* rift_tokenizer_get_error(const TokenizerContext* ctx);
bool rift_tokenizer_has_error(const TokenizerContext* ctx);
void rift_tokenizer_clear_error(TokenizerContext* ctx);

/* Thread safety */
bool rift_tokenizer_enable_thread_safety(TokenizerContext* ctx);
bool rift_tokenizer_disable_thread_safety(TokenizerContext* ctx);
bool rift_tokenizer_is_thread_safe(const TokenizerContext* ctx);

/* Performance monitoring */
TokenizerStats rift_tokenizer_get_stats(const TokenizerContext* ctx);
void rift_tokenizer_reset_stats(TokenizerContext* ctx);

/* =================================================================
 * UTILITY FUNCTIONS
 * =================================================================
 */

/* Flag parsing and conversion */
TokenFlags rift_parse_r_flags(const char* flag_string);
const char* rift_flags_to_string(TokenFlags flags);

/* Token type utilities */
const char* rift_token_type_name(TokenType type);
const char* rift_token_flags_string(TokenFlags flags);

/* Debug and validation functions */
void rift_tokenizer_print_stats(const TokenizerContext* ctx);
void rift_tokenizer_print_tokens(const TokenizerContext* ctx);
bool rift_tokenizer_validate_dfa(const TokenizerContext* ctx);

/* Version and feature detection */
const char* rift_tokenizer_version(void);
const char* rift_tokenizer_build_info(void);
uint32_t rift_tokenizer_version_number(void);
bool rift_tokenizer_has_dfa_support(void);
bool rift_tokenizer_has_regex_compose(void);
bool rift_tokenizer_has_thread_safety(void);
bool rift_tokenizer_has_caching(void);

/* =================================================================
 * ADVANCED R-SYNTAX PROCESSING
 * =================================================================
 */

/* R-syntax pattern validation */
typedef enum {
    R_PARSE_SUCCESS = 0,
    R_PARSE_INVALID_SYNTAX,
    R_PARSE_INVALID_FLAGS,
    R_PARSE_PATTERN_TOO_LONG,
    R_PARSE_COMPOSITION_ERROR,
    R_PARSE_DFA_CONSTRUCTION_FAILED
} RParseResult;

/* R-syntax parsing functions */
RParseResult rift_r_syntax_parse(const char* input, size_t length,
                                 RegexComposition** output);
bool rift_r_syntax_validate(const char* pattern);
RCompositionOperator rift_r_syntax_detect_operator(const char* input, size_t length);

/* Advanced composition functions */
RegexComposition* rift_r_syntax_parse_composition(const char* expression);
bool rift_r_syntax_optimize_dfa(RegexComposition* regex);

/* =================================================================
 * AEGIS GOVERNANCE INTEGRATION
 * =================================================================
 */

/* Governance validation functions */
typedef enum {
    GOVERNANCE_PASS = 0,
    GOVERNANCE_FAIL_SECURITY,
    GOVERNANCE_FAIL_VALIDATION,
    GOVERNANCE_FAIL_COMPLIANCE
} GovernanceResult;

GovernanceResult rift_governance_validate_pattern(const char* pattern);
GovernanceResult rift_governance_validate_token(const TokenTriplet* token);
GovernanceResult rift_governance_validate_context(const TokenizerContext* ctx);

/* Audit trail functions */
typedef struct {
    uint64_t timestamp;
    const char* operation;
    const char* pattern;
    GovernanceResult result;
    char details[256];
} AuditEntry;

bool rift_governance_log_audit(const AuditEntry* entry);
size_t rift_governance_get_audit_trail(AuditEntry* buffer, size_t capacity);

/* =================================================================
 * MACRO DEFINITIONS FOR ENHANCED FUNCTIONALITY
 * =================================================================
 */

/* R-syntax pattern matching macros */
#define R_PATTERN(pattern, flags) \
    rift_regex_compile("R\"" pattern "\"", rift_parse_r_flags(flags))

#define R_MATCH(regex, input) \
    rift_regex_match((regex), (input), strlen(input))

#define R_AND(a, b) rift_regex_compose_and((a), (b))
#define R_OR(a, b)  rift_regex_compose_or((a), (b))
#define R_XOR(a, b) rift_regex_compose_xor((a), (b))
#define R_NAND(a, b) rift_regex_compose_nand((a), (b))

/* TokenTriplet manipulation macros */
#define TOKEN_CREATE(type, ptr, val) rift_token_create((type), (ptr), (val))
#define TOKEN_GET_TYPE(token) ((token).type)
#define TOKEN_GET_PTR(token) ((token).mem_ptr)
#define TOKEN_GET_VALUE(token) ((token).value)
#define TOKEN_SET_FLAG(token, flag) rift_token_set_flags(&(token), (flag))

/* DFA state management macros */
#define DFA_CREATE_STATE(id, final) rift_dfa_create_state((id), (final))
#define DFA_IS_FINAL(state) rift_dfa_is_accepting_state(state)
#define DFA_GET_TOKEN_TYPE(state) rift_dfa_get_token_type(state)

/* Error handling macros */
#define RIFT_CHECK_NULL(ptr) do { \
    if (!(ptr)) { \
        fprintf(stderr, "RIFT Error: NULL pointer at %s:%d\n", __FILE__, __LINE__); \
        return false; \
    } \
} while(0)

#define RIFT_CHECK_BOUNDS(val, max) do { \
    if ((val) >= (max)) { \
        fprintf(stderr, "RIFT Error: Bounds check failed at %s:%d\n", __FILE__, __LINE__); \
        return false; \
    } \
} while(0)

/* =================================================================
 * COMPILER-SPECIFIC OPTIMIZATIONS
 * =================================================================
 */

/* GCC-specific optimizations */
#ifdef __GNUC__
#define RIFT_LIKELY(x)      __builtin_expect(!!(x), 1)
#define RIFT_UNLIKELY(x)    __builtin_expect(!!(x), 0)
#define RIFT_PURE           __attribute__((pure))
#define RIFT_CONST          __attribute__((const))
#define RIFT_HOT            __attribute__((hot))
#define RIFT_COLD           __attribute__((cold))
#else
#define RIFT_LIKELY(x)      (x)
#define RIFT_UNLIKELY(x)    (x)
#define RIFT_PURE
#define RIFT_CONST
#define RIFT_HOT
#define RIFT_COLD
#endif

/* Function annotations for performance optimization */
#define RIFT_INLINE static inline __attribute__((always_inline))
#define RIFT_NOINLINE __attribute__((noinline))

#ifdef __cplusplus
}
#endif

#endif /* RIFT_0_CORE_TOKENIZER_RULES_H */