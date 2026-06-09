#ifndef RIFT_PARSER_H
#define RIFT_PARSER_H

#include "rift/platform.h"
#include "rift/token.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum rift_ast_type {
    RIFT_AST_PROGRAM,
    RIFT_AST_DECLARATION,
    RIFT_AST_FUNCTION,
    RIFT_AST_BLOCK,
    RIFT_AST_STATEMENT,
    RIFT_AST_RETURN,
    RIFT_AST_IF,
    RIFT_AST_WHILE,
    RIFT_AST_FOR,
    RIFT_AST_BREAK,
    RIFT_AST_BINARY_OP,
    RIFT_AST_UNARY_OP,
    RIFT_AST_CALL,
    RIFT_AST_LITERAL,
    RIFT_AST_IDENTIFIER
} rift_ast_type_t;

typedef struct rift_ast_node {
    rift_ast_type_t       type;
    rift_token_value_t    value;
    uint32_t              matched_state;
    float                 complexity_score;
    struct rift_ast_node *children;
    struct rift_ast_node *next;
} rift_ast_node_t;

typedef struct rift_parser rift_parser_t;

RIFT_API rift_parser_t   *rift_parser_create(const rift_token_t *tokens, size_t count);
RIFT_API void             rift_parser_destroy(rift_parser_t *parser);
RIFT_API rift_ast_node_t *rift_parser_parse(rift_parser_t *parser);
RIFT_API void             rift_ast_free(rift_ast_node_t *node);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_PARSER_H */
