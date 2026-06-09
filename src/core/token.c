#include "rift/token.h"
#include <stdlib.h>
#include <string.h>

rift_token_t rift_token_create(rift_token_type_t type, const char *value) {
    rift_token_t token;
    memset(&token, 0, sizeof(token));
    token.type = type;
    token.confidence = type == RIFT_TOKEN_UNKNOWN ? 0.0 : 1.0;
    token.context_confidence = 1.0;
    token.row = 1;
    token.column = 1;
    token.memory_type = RIFT_MEMORY_LITERAL;
    token.memory.alignment = RIFT_MEMORY_ALIGN_DEFAULT;

    if (value) {
        size_t len = strlen(value) + 1;
        token.memory = rift_memory_alloc(len, RIFT_MEMORY_ALIGN_DEFAULT);
        if (token.memory.ptr) {
            memcpy(token.memory.ptr, value, len);
            token.value.str = (char *)token.memory.ptr;
            token.lexeme = token.value.str;
            token.lexeme_length = len - 1;
            token.memory_type = RIFT_MEMORY_HEAP;
        }
    }

    token.validation_bits = token.memory.ptr || !value ? 0x07 : 0;
    return token;
}

void rift_token_destroy(rift_token_t *token) {
    if (!token) return;
    rift_memory_free(&token->memory);
    memset(token, 0, sizeof(*token));
}

int rift_token_validate(const rift_token_t *token) {
    if (!token) return 0;
    if (token->type == RIFT_TOKEN_UNKNOWN) return 0;
    if ((token->validation_bits & 0x07) != 0x07) return 0;
    return rift_memory_validate(&token->memory);
}

rift_token_t rift_token_borrow(rift_token_type_t type,
                               const char *value,
                               size_t length) {
    rift_token_t token;
    memset(&token, 0, sizeof(token));
    token.type = type;
    token.value.str = (char *)value;
    token.lexeme = (char *)value;
    token.lexeme_length = length;
    token.memory = rift_memory_borrow((void *)value, length);
    token.memory_type = RIFT_MEMORY_BORROWED;
    token.memory_borrowed = 1;
    token.confidence = type == RIFT_TOKEN_UNKNOWN ? 0.0 : 1.0;
    token.context_confidence = 1.0;
    token.row = 1;
    token.column = 1;
    token.validation_bits = 0x07;
    return token;
}

void rift_token_set_position(rift_token_t *token,
                             uint32_t row,
                             uint32_t column,
                             uint32_t offset) {
    if (!token) return;
    token->row = row;
    token->column = column;
    token->offset = offset;
}

static const char *token_type_names[] = {
    "EOF", "KEYWORD", "IDENTIFIER",
    "LITERAL_INT", "LITERAL_FLOAT", "LITERAL_STRING", "LITERAL_CHAR",
    "OPERATOR", "DELIMITER", "COMMENT", "WHITESPACE",
    "PATTERN_STATIC", "PATTERN_DYNAMIC", "HEXDUMP", "UNKNOWN"
};

const char *rift_token_type_name(rift_token_type_t type) {
    if (type < 0 || type > RIFT_TOKEN_UNKNOWN) return "INVALID";
    return token_type_names[type];
}
