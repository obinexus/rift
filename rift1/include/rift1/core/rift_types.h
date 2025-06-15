#ifdef __cplusplus
}
#endif

// ===== include/rift1/core/rift_types.h =====
#pragma once

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ===== Result Codes =====
typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_NULL_POINTER = -1,
    RIFT_ERROR_MEMORY_ALLOCATION = -2,
    RIFT_ERROR_FILE_NOT_FOUND = -3,
    RIFT_ERROR_INVALID_INPUT = -4,
    RIFT_ERROR_PARSE_FAILED = -5,
    RIFT_ERROR_PARSE_INCOMPLETE = -6,
    RIFT_ERROR_PATTERN_INVALID = -7,
    RIFT_ERROR_INVALID_PARSE_MODE = -8,
    RIFT_ERROR_PARSING_INCOMPLETE = -9
} RiftResult;

// ===== Token Types =====
typedef enum {
    RIFT_TOKEN_UNKNOWN = 0,
    RIFT_TOKEN_IDENTIFIER = 1,
    RIFT_TOKEN_INTEGER = 2,
    RIFT_TOKEN_FLOAT = 3,
    RIFT_TOKEN_STRING = 4,
    RIFT_TOKEN_CHAR = 5,
    RIFT_TOKEN_OPERATOR = 6,
    RIFT_TOKEN_DELIMITER = 7,
    RIFT_TOKEN_SEPARATOR = 8,
    RIFT_TOKEN_KEYWORD = 9,
    RIFT_TOKEN_WHITESPACE = 10,
    RIFT_TOKEN_EOF = 11
} RiftTokenType;

// ===== Forward Declarations =====
typedef struct RiftToken RiftToken;
typedef struct RiftState RiftState;
typedef struct RiftASTNode RiftASTNode;
typedef struct RiftStackFrame RiftStackFrame;
typedef struct RiftConfig RiftConfig;

// ===== Token Structure =====
struct RiftToken {
    char* type;                 // Token type string (preserved for OBINexus)
    char* value;                // Token value string
    char* lexeme;               // Original lexeme from source
    RiftTokenType type_enum;    // Classified token type enum
    RiftState* matched_state;   // AEGIS automaton matched state
    size_t line;                // Source line number
    size_t column;              // Source column number
};

// ===== Configuration Structure =====
struct RiftConfig {
    RiftParseMode parse_mode;
    bool verbose;
    bool debug;
    char* output_directory;
    char* stage_name;
};

#ifdef __cplusplus
}
#endif