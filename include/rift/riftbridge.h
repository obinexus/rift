#ifndef RIFT_RIFTBRIDGE_H
#define RIFT_RIFTBRIDGE_H

#include <stddef.h>
#include <stdint.h>
#include "rift/platform.h"
#include "rift/token.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RIFTSemanticMatrix {
    RIFTToken **cells;
    size_t rows;
    size_t columns;
    double alpha;
    double beta;
    double gamma;
    double threshold;
} RIFTSemanticMatrix;

typedef enum RIFTBridgeNodeType {
    RIFT_BRIDGE_TERMINAL = 0,
    RIFT_BRIDGE_NONTERMINAL
} RIFTBridgeNodeType;

typedef struct RIFTBridgeASTNode {
    RIFTBridgeNodeType type;
    double aggregate_confidence;
    struct RIFTBridgeASTNode **children;
    size_t child_count;
    RIFTToken *source_token;
    rift_semantic_intent_t intent;
} RIFTBridgeASTNode;

typedef struct RIFTBridgeResult {
    RIFTBridgeASTNode **roots;
    size_t root_count;
    size_t accepted_tokens;
    size_t rejected_tokens;
} RIFTBridgeResult;

RIFT_API RIFTSemanticMatrix *rift_semantic_matrix_create(
    RIFTToken *tokens, size_t token_count, double threshold);
RIFT_API void rift_semantic_matrix_destroy(RIFTSemanticMatrix *matrix);
RIFT_API double rift_bridge_token_confidence(
    const RIFTSemanticMatrix *matrix, const RIFTToken *token);
RIFT_API rift_semantic_intent_t rift_bridge_resolve_intent(
    const RIFTToken *token, const RIFTToken *previous, const RIFTToken *next);
RIFT_API int rift_bridge_traverse(
    RIFTSemanticMatrix *matrix, RIFTBridgeResult *result);
RIFT_API void rift_bridge_result_destroy(RIFTBridgeResult *result);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_RIFTBRIDGE_H */
