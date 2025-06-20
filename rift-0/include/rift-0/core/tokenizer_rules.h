/**
 * =================================================================
 * tokenizer_rules.h - RIFT-0 Tokenizer Rules Module
 * RIFT: RIFT Is a Flexible Translator
 * Component: DFA-based tokenization with R"" pattern support
 * OBINexus Computing Framework - Stage 0 Implementation
 * =================================================================
 */

#ifndef RIFT_0_TOKENIZER_RULES_H
#define RIFT_0_TOKENIZER_RULES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TokenTriplet: Memory-efficient token representation using bitfields
 * Compliant with C99/C11 memory mapping and thread-safe requirements
 */
typedef struct {
    uint32_t type : 8;      /* Token type identifier (0-255) */
    uint32_t mem_ptr : 16;  /* Memory pointer offset (0-65535) */
    uint32_t value : 8;     /* Token value/flags (0-255) */
} TokenTriplet;

/**
 * DFA State Flags for R"" pattern processing
 * Supports: g(lobal), m(ultiline), i(nsensitive), t(op-down), b(ottom-up)
 */
typedef enum {
    DFA_FLAG_GLOBAL     = 0x01,
    DFA_FLAG_MULTILINE  = 0x02,
    DFA_FLAG_INSENSITIVE = 0x04,
    DFA_FLAG_TOP_DOWN   = 0x08,
    DFA_FLAG_BOTTOM_UP  = 0x10
} DFAFlags;

/**
 * Token Types for RIFT language semantics
 */
typedef enum {
    TOKEN_UNKNOWN = 0,
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_LITERAL_STRING,
    TOKEN_LITERAL_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_DELIMITER,
    TOKEN_R_PATTERN,    /* R"" pattern token */
    TOKEN_NULL_KEYWORD, /* NULL semantic */
    TOKEN_NIL_KEYWORD,  /* nil semantic */
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

/**
 * DFA State Machine Structure
 */
typedef struct {
    int current_state;
    DFAFlags flags;
    char delimiter[16];     /* For R"delimiter" patterns */
    size_t delimiter_len;
    bool in_raw_mode;
    char *content_buffer;
    size_t content_size;
    size_t content_capacity;
} DFAStateMachine;

/**
 * Tokenization Result Structure
 */
typedef struct {
    TokenTriplet *tokens;
    size_t count;
    size_t capacity;
    bool success;
    char *error_message;
} TokenizationResult;

/**
 * Core tokenization functions
 */

/**
 * Initialize the tokenizer rules system
 * Must be called before any tokenization operations
 */
void init_tokenizer_rules(void);

/**
 * Cleanup tokenizer rules system
 */
void cleanup_tokenizer_rules(void);

/**
 * Main pattern matching function for R"" syntax
 * @param src: Source string to tokenize
 * @param out: Output TokenTriplet (single token)
 * @return: Number of characters consumed, or -1 on error
 */
int match_token_pattern(const char *src, TokenTriplet *out);

/**
 * Full tokenization of source string
 * @param src: Source string to tokenize
 * @param src_len: Length of source string
 * @return: TokenizationResult structure (caller must free)
 */
TokenizationResult tokenize_source(const char *src, size_t src_len);

/**
 * DFA State Machine operations
 */

/**
 * Initialize DFA state machine
 */
DFAStateMachine* dfa_create(void);

/**
 * Process single character through DFA
 * @param dfa: DFA state machine
 * @param ch: Character to process
 * @return: true if valid transition, false on error
 */
bool dfa_process_char(DFAStateMachine *dfa, char ch);

/**
 * Check if DFA is in accepting state
 */
bool dfa_is_accepting(const DFAStateMachine *dfa);

/**
 * Reset DFA to initial state
 */
void dfa_reset(DFAStateMachine *dfa);

/**
 * Destroy DFA state machine
 */
void dfa_destroy(DFAStateMachine *dfa);

/**
 * Utility functions for token validation
 */

/**
 * Validate token type/value separation (SSA compliance)
 * @param token: Token to validate
 * @return: true if token is valid
 */
bool validate_token_separation(const TokenTriplet *token);

/**
 * Check if string represents NULL or nil semantic
 * @param str: String to check
 * @param len: String length
 * @return: TOKEN_NULL_KEYWORD, TOKEN_NIL_KEYWORD, or TOKEN_UNKNOWN
 */
TokenType classify_null_nil_semantic(const char *str, size_t len);

/**
 * Thread-safe token encoding
 * Uses thread-local storage and atomic operations
 */
TokenTriplet R_encode_safe(TokenType type, uint16_t mem_ptr, uint8_t value);

/**
 * Free tokenization result
 */
void free_tokenization_result(TokenizationResult *result);

/**
 * Debug utilities
 */
const char* token_type_to_string(TokenType type);
void print_token_triplet(const TokenTriplet *token);
void print_tokenization_result(const TokenizationResult *result);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_0_TOKENIZER_RULES_H */
