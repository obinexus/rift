#include "rift_check.h"
#include <stdio.h>
#include "rift/riftbridge.h"

int main(void) {
    RIFTToken tokens[4];
    RIFTSemanticMatrix *matrix;
    RIFTBridgeResult result;
    int traversal_result;
    tokens[0] = rift_token_create(RIFT_TOKEN_KEYWORD, "let");
    tokens[1] = rift_token_create(RIFT_TOKEN_IDENTIFIER, "answer");
    tokens[2] = rift_token_create(RIFT_TOKEN_OPERATOR, "=");
    tokens[3] = rift_token_create(RIFT_TOKEN_LITERAL_INT, "42");
    rift_token_set_position(&tokens[0], 1, 1, 0);
    rift_token_set_position(&tokens[1], 1, 5, 4);
    rift_token_set_position(&tokens[2], 1, 12, 11);
    rift_token_set_position(&tokens[3], 1, 14, 13);

    matrix = rift_semantic_matrix_create(tokens, 4, 0.70);
    RIFT_CHECK(matrix != NULL);
    traversal_result = rift_bridge_traverse(matrix, &result);
    RIFT_CHECK(traversal_result == 0);
    RIFT_CHECK(result.root_count == 1);
    RIFT_CHECK(result.accepted_tokens == 4);
    RIFT_CHECK(tokens[0].intent == RIFT_INTENT_DECLARE);
    RIFT_CHECK(tokens[2].intent == RIFT_INTENT_ASSIGN);

    rift_bridge_result_destroy(&result);
    rift_semantic_matrix_destroy(matrix);
    for (int i = 0; i < 4; i++) rift_token_destroy(&tokens[i]);
    puts("test_bridge: PASS");
    return 0;
}
