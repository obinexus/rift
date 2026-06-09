#include "rift/parser.h"
#include <stdlib.h>
#include <string.h>

struct rift_parser {
    const rift_token_t *tokens;
    size_t              count;
    size_t              pos;
};

rift_parser_t *rift_parser_create(const rift_token_t *tokens, size_t count) {
    if (!tokens) return NULL;

    rift_parser_t *parser = (rift_parser_t *)calloc(1, sizeof(rift_parser_t));
    if (!parser) return NULL;

    parser->tokens = tokens;
    parser->count = count;
    parser->pos = 0;

    return parser;
}

void rift_parser_destroy(rift_parser_t *parser) {
    free(parser);
}

static rift_ast_node_t *ast_node_create(rift_ast_type_t type) {
    rift_ast_node_t *node = (rift_ast_node_t *)calloc(1, sizeof(rift_ast_node_t));
    if (node) {
        node->type = type;
    }
    return node;
}

rift_ast_node_t *rift_parser_parse(rift_parser_t *parser) {
    if (!parser) return NULL;

    rift_ast_node_t *program = ast_node_create(RIFT_AST_PROGRAM);
    if (!program) return NULL;

    rift_ast_node_t *tail = NULL;
    while (parser->pos < parser->count) {
        const rift_token_t *token = &parser->tokens[parser->pos++];
        rift_ast_type_t type;
        rift_ast_node_t *node;
        if (token->type == RIFT_TOKEN_EOF) break;
        if (token->type == RIFT_TOKEN_IDENTIFIER) type = RIFT_AST_IDENTIFIER;
        else if (token->type == RIFT_TOKEN_LITERAL_INT ||
                 token->type == RIFT_TOKEN_LITERAL_FLOAT ||
                 token->type == RIFT_TOKEN_LITERAL_STRING ||
                 token->type == RIFT_TOKEN_LITERAL_CHAR) type = RIFT_AST_LITERAL;
        else if (token->type == RIFT_TOKEN_OPERATOR) type = RIFT_AST_BINARY_OP;
        else if (token->type == RIFT_TOKEN_KEYWORD &&
                 token->lexeme && strcmp(token->lexeme, "return") == 0) {
            type = RIFT_AST_RETURN;
        } else {
            type = RIFT_AST_STATEMENT;
        }
        node = ast_node_create(type);
        if (!node) {
            rift_ast_free(program);
            return NULL;
        }
        node->value = token->value;
        node->complexity_score = (float)token->confidence;
        if (!program->children) program->children = node;
        else tail->next = node;
        tail = node;
    }

    return program;
}

void rift_ast_free(rift_ast_node_t *node) {
    if (!node) return;
    rift_ast_free(node->children);
    rift_ast_free(node->next);
    free(node);
}
