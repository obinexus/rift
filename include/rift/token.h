#ifndef RIFT_TOKEN_H
#define RIFT_TOKEN_H

#include <stddef.h>
#include <stdint.h>
#include "rift/platform.h"
#include "rift/memory.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Token types */
typedef enum rift_token_type {
    RIFT_TOKEN_EOF = 0,
    RIFT_TOKEN_KEYWORD,
    RIFT_TOKEN_IDENTIFIER,
    RIFT_TOKEN_LITERAL_INT,
    RIFT_TOKEN_LITERAL_FLOAT,
    RIFT_TOKEN_LITERAL_STRING,
    RIFT_TOKEN_LITERAL_CHAR,
    RIFT_TOKEN_OPERATOR,
    RIFT_TOKEN_DELIMITER,
    RIFT_TOKEN_COMMENT,
    RIFT_TOKEN_WHITESPACE,
    RIFT_TOKEN_PATTERN_STATIC,   /* R"" - compile-time static string */
    RIFT_TOKEN_PATTERN_DYNAMIC,  /* R'' - runtime dynamic character */
    RIFT_TOKEN_HEXDUMP,          /* Hexdump canonical form */
    RIFT_TOKEN_UNKNOWN
} rift_token_type_t;

typedef rift_token_type_t RIFTTokenType;

typedef enum rift_token_memory_type {
    RIFT_MEMORY_LITERAL = 0,
    RIFT_MEMORY_REFERENCE,
    RIFT_MEMORY_HEAP,
    RIFT_MEMORY_BORROWED,
    RIFT_MEMORY_SHARED,
    RIFT_MEMORY_EXCLUSIVE,
    RIFT_MEMORY_STATIC,
    RIFT_MEMORY_THREAD_LOCAL
} rift_token_memory_type_t;

typedef enum rift_semantic_intent {
    RIFT_INTENT_UNKNOWN = 0,
    RIFT_INTENT_DECLARE,
    RIFT_INTENT_ASSIGN,
    RIFT_INTENT_CONTROL,
    RIFT_INTENT_INVOKE,
    RIFT_INTENT_QUERY,
    RIFT_INTENT_TERMINATE,
    RIFT_INTENT_SEPARATOR
} rift_semantic_intent_t;

/* Token value union */
typedef union rift_token_value {
    int64_t  i64;
    uint64_t u64;
    double   f64;
    char    *str;
} rift_token_value_t;

/*
 * Token Triplet: the fundamental RIFT unit
 *   - type:   semantic classification
 *   - value:  the token's data
 *   - memory: memory governance for this token
 */
typedef struct rift_token {
    rift_token_type_t    type;
    rift_token_value_t   value;
    rift_memory_span_t   memory;
    uint8_t              validation_bits;
    char                *lexeme;
    size_t               lexeme_length;
    double               confidence;
    double               context_confidence;
    uint32_t             row;
    uint32_t             column;
    uint32_t             offset;
    uint32_t             nesting_depth;
    rift_token_memory_type_t memory_type;
    uint32_t             memory_scope_id;
    int                  memory_borrowed;
    rift_semantic_intent_t intent;
    void                *semantic_data;
} rift_token_t;

typedef rift_token_t RIFTToken;

/* Source compatibility with the earlier public spelling. */
typedef rift_token_t RiftToken;
typedef rift_token_type_t RiftTokenType;

RIFT_API rift_token_t  rift_token_create(rift_token_type_t type, const char *value);
RIFT_API void          rift_token_destroy(rift_token_t *token);
RIFT_API int           rift_token_validate(const rift_token_t *token);
RIFT_API const char   *rift_token_type_name(rift_token_type_t type);
RIFT_API rift_token_t  rift_token_borrow(rift_token_type_t type,
                                          const char *value,
                                          size_t length);
RIFT_API void          rift_token_set_position(rift_token_t *token,
                                                uint32_t row,
                                                uint32_t column,
                                                uint32_t offset);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_TOKEN_H */
