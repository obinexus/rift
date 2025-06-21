/*
 * =================================================================
 * SINPHASÉ TOKENIZER ARCHITECTURE REORGANIZATION
 * RIFT: RIFT Is a Flexible Translator - Stage 0 Tokenization
 * OBINexus Computing Framework - AEGIS Methodology Compliance
 * 
 * Dependency Hierarchy (Single-Pass Compilation):
 * tokenizer_types.h -> tokenizer_rules.h -> tokenizer.h -> rift_tokenizer.h
 * 
 * Cost-Based Isolation: Core types isolated to prevent redefinition
 * Phase Constraint: Each header maintains single responsibility
 * =================================================================
 */

/* =================================================================
 * 1. CORE TYPES HEADER (tokenizer_types.h)
 * Foundation component - single source of truth for all types
 * =================================================================
 */

#ifndef RIFT_0_CORE_TOKENIZER_TYPES_H
#define RIFT_0_CORE_TOKENIZER_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* =================================================================
 * TOKENTRIPLET BITFIELD STRUCTURE - AEGIS COMPLIANT
 * =================================================================
 */

typedef struct __attribute__((packed)) {
    uint32_t type    : 8;   /* Token type identifier (0-255) */
    uint32_t mem_ptr : 16;  /* Memory pointer/offset (0-65535) */
    uint32_t value   : 8;   /* Token value/flags (0-255) */
} TokenTriplet;

/* Static assertions for deterministic builds */
#if defined(__cplusplus)
static_assert(sizeof(TokenTriplet) == 4, "TokenTriplet must be exactly 32 bits for single-pass compilation");
#else
_Static_assert(sizeof(TokenTriplet) == 4, "TokenTriplet must be exactly 32 bits for single-pass compilation");
#endif

/* =================================================================
 * TOKEN TYPE ENUMERATION - CORE CLASSIFICATION
 * =================================================================
 */

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
    
    /* R-syntax specific tokens */
    TOKEN_REGEX_START,       /* R" or R' marker */
    TOKEN_REGEX_END,         /* Closing quote marker */
    TOKEN_COMPOSE_AND,       /* R.AND composition */
    TOKEN_COMPOSE_OR,        /* R.OR composition */
    TOKEN_COMPOSE_XOR,       /* R.XOR composition */
    TOKEN_COMPOSE_NAND,      /* R.NAND composition */
    
    /* DFA state tokens */
    TOKEN_DFA_STATE,         /* DFA state transition */
    
    TOKEN_MAX = 255          /* Maximum token type value */
} TokenType;

/* =================================================================
 * TOKEN FLAGS ENUMERATION - PROCESSING CONTROL
 * =================================================================
 */

typedef enum {
    TOKEN_FLAG_NONE       = 0x00,
    TOKEN_FLAG_GLOBAL     = 0x01,  /* g flag - global matching */
    TOKEN_FLAG_MULTILINE  = 0x02,  /* m flag - multiline mode */
    TOKEN_FLAG_IGNORECASE = 0x04,  /* i flag - case insensitive */
    TOKEN_FLAG_TOPDOWN    = 0x08,  /* t flag - top-down evaluation */
    TOKEN_FLAG_BOTTOMUP   = 0x10,  /* b flag - bottom-up evaluation */
    TOKEN_FLAG_COMPOSED   = 0x20,  /* Token from composed regex */
    TOKEN_FLAG_VALIDATED  = 0x40,  /* DFA validated token */
    TOKEN_FLAG_ERROR      = 0x80   /* Error state marker */
} TokenFlags;

/* =================================================================
 * FORWARD DECLARATIONS - INTERFACE CONTRACTS
 * =================================================================
 */

typedef struct DFAState DFAState;
typedef struct RegexComposition RegexComposition;
typedef struct TokenizerContext TokenizerContext;
typedef struct TokenizerStats TokenizerStats;

#ifdef __cplusplus
}
#endif

#endif /* RIFT_0_CORE_TOKENIZER_TYPES_H */