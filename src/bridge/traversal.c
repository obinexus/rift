#include "rift/riftbridge.h"

#include <stdlib.h>
#include <string.h>

static size_t matrix_index(const RIFTSemanticMatrix *matrix,
                           size_t row, size_t column) {
    return row * matrix->columns + column;
}

RIFTSemanticMatrix *rift_semantic_matrix_create(
    RIFTToken *tokens, size_t token_count, double threshold) {
    RIFTSemanticMatrix *matrix;
    size_t rows = 1;
    size_t columns = 1;
    size_t i;

    if (!tokens && token_count) return NULL;
    for (i = 0; i < token_count; i++) {
        size_t row = tokens[i].row ? tokens[i].row : 1;
        size_t column = tokens[i].column ? tokens[i].column : i + 1;
        if (row > rows) rows = row;
        if (column > columns) columns = column;
    }

    matrix = (RIFTSemanticMatrix *)calloc(1, sizeof(*matrix));
    if (!matrix) return NULL;
    matrix->cells = (RIFTToken **)calloc(rows * columns, sizeof(*matrix->cells));
    if (!matrix->cells) {
        free(matrix);
        return NULL;
    }

    matrix->rows = rows;
    matrix->columns = columns;
    matrix->alpha = 1.0 / 3.0;
    matrix->beta = 1.0 / 3.0;
    matrix->gamma = 1.0 / 3.0;
    matrix->threshold = threshold >= 0.0 && threshold <= 1.0 ? threshold : 0.70;

    for (i = 0; i < token_count; i++) {
        size_t row = (tokens[i].row ? tokens[i].row : 1) - 1;
        size_t column = (tokens[i].column ? tokens[i].column : i + 1) - 1;
        matrix->cells[matrix_index(matrix, row, column)] = &tokens[i];
    }
    return matrix;
}

void rift_semantic_matrix_destroy(RIFTSemanticMatrix *matrix) {
    if (!matrix) return;
    free(matrix->cells);
    free(matrix);
}

double rift_bridge_token_confidence(const RIFTSemanticMatrix *matrix,
                                    const RIFTToken *token) {
    double lexical;
    double positional;
    double type_consistency;
    if (!matrix || !token) return 0.0;
    lexical = token->confidence;
    positional = token->context_confidence;
    type_consistency = token->type == RIFT_TOKEN_UNKNOWN ? 0.0 : 1.0;
    if (lexical < 0.0) lexical = 0.0;
    if (lexical > 1.0) lexical = 1.0;
    if (positional < 0.0) positional = 0.0;
    if (positional > 1.0) positional = 1.0;
    return matrix->alpha * lexical +
           matrix->beta * positional +
           matrix->gamma * type_consistency;
}

static int token_equals(const RIFTToken *token, const char *value) {
    return token && token->lexeme && strcmp(token->lexeme, value) == 0;
}

rift_semantic_intent_t rift_bridge_resolve_intent(
    const RIFTToken *token, const RIFTToken *previous, const RIFTToken *next) {
    (void)previous;
    (void)next;
    if (!token) return RIFT_INTENT_UNKNOWN;
    if (token->intent != RIFT_INTENT_UNKNOWN) return token->intent;
    if (token_equals(token, "let") || token_equals(token, "def") ||
        token_equals(token, "fn") || token_equals(token, "type") ||
        token_equals(token, "pattern")) {
        return RIFT_INTENT_DECLARE;
    }
    if (token_equals(token, "if") || token_equals(token, "while") ||
        token_equals(token, "for") || token_equals(token, "match")) {
        return RIFT_INTENT_CONTROL;
    }
    if (token_equals(token, "return") || token_equals(token, "break")) {
        return RIFT_INTENT_TERMINATE;
    }
    if (token->type == RIFT_TOKEN_OPERATOR && token_equals(token, "=")) {
        return RIFT_INTENT_ASSIGN;
    }
    if (token->type == RIFT_TOKEN_OPERATOR && token_equals(token, "?")) {
        return RIFT_INTENT_QUERY;
    }
    if (token->type == RIFT_TOKEN_DELIMITER &&
        (token_equals(token, ";") || token_equals(token, "."))) {
        return RIFT_INTENT_TERMINATE;
    }
    if (token->type == RIFT_TOKEN_DELIMITER &&
        (token_equals(token, ",") || token_equals(token, ":"))) {
        return RIFT_INTENT_SEPARATOR;
    }
    if (token->type == RIFT_TOKEN_IDENTIFIER && next &&
        token_equals(next, "(")) {
        return RIFT_INTENT_INVOKE;
    }
    return RIFT_INTENT_UNKNOWN;
}

static RIFTBridgeASTNode *bridge_node_create(RIFTBridgeNodeType type,
                                             RIFTToken *token,
                                             double confidence) {
    RIFTBridgeASTNode *node =
        (RIFTBridgeASTNode *)calloc(1, sizeof(*node));
    if (!node) return NULL;
    node->type = type;
    node->source_token = token;
    node->aggregate_confidence = confidence;
    return node;
}

static void bridge_node_destroy(RIFTBridgeASTNode *node) {
    size_t i;
    if (!node) return;
    for (i = 0; i < node->child_count; i++) {
        bridge_node_destroy(node->children[i]);
    }
    free(node->children);
    free(node);
}

int rift_bridge_traverse(RIFTSemanticMatrix *matrix, RIFTBridgeResult *result) {
    size_t row;
    if (!matrix || !result) return -1;
    memset(result, 0, sizeof(*result));
    result->roots = (RIFTBridgeASTNode **)calloc(
        matrix->rows, sizeof(*result->roots));
    if (!result->roots) return -1;

    for (row = 0; row < matrix->rows; row++) {
        RIFTBridgeASTNode *root =
            bridge_node_create(RIFT_BRIDGE_NONTERMINAL, NULL, 0.0);
        size_t column;
        double confidence_sum = 0.0;
        if (!root) {
            rift_bridge_result_destroy(result);
            return -1;
        }

        for (column = 0; column < matrix->columns; column++) {
            RIFTToken *token = matrix->cells[matrix_index(matrix, row, column)];
            RIFTBridgeASTNode *child;
            double confidence;
            RIFTToken *previous = column > 0
                ? matrix->cells[matrix_index(matrix, row, column - 1)] : NULL;
            RIFTToken *next = column + 1 < matrix->columns
                ? matrix->cells[matrix_index(matrix, row, column + 1)] : NULL;
            if (!token || token->type == RIFT_TOKEN_EOF) continue;
            confidence = rift_bridge_token_confidence(matrix, token);
            if (confidence < matrix->threshold) {
                result->rejected_tokens++;
                continue;
            }
            token->confidence = confidence;
            token->intent = rift_bridge_resolve_intent(token, previous, next);
            child = bridge_node_create(RIFT_BRIDGE_TERMINAL, token, confidence);
            if (!child) {
                bridge_node_destroy(root);
                rift_bridge_result_destroy(result);
                return -1;
            }
            child->intent = token->intent;
            {
                RIFTBridgeASTNode **children =
                    (RIFTBridgeASTNode **)realloc(
                        root->children,
                        (root->child_count + 1) * sizeof(*root->children));
                if (!children) {
                    bridge_node_destroy(child);
                    bridge_node_destroy(root);
                    rift_bridge_result_destroy(result);
                    return -1;
                }
                root->children = children;
                root->children[root->child_count++] = child;
            }
            confidence_sum += confidence;
            result->accepted_tokens++;
        }

        if (root->child_count) {
            root->aggregate_confidence = confidence_sum / root->child_count;
            result->roots[result->root_count++] = root;
        } else {
            bridge_node_destroy(root);
        }
    }
    return 0;
}

void rift_bridge_result_destroy(RIFTBridgeResult *result) {
    size_t i;
    if (!result) return;
    for (i = 0; i < result->root_count; i++) {
        bridge_node_destroy(result->roots[i]);
    }
    free(result->roots);
    memset(result, 0, sizeof(*result));
}
