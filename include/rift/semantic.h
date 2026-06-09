#ifndef RIFT_SEMANTIC_H
#define RIFT_SEMANTIC_H

#include "rift/platform.h"
#include "rift/parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Z->Y->X internal reduction:
 *   Z (Lexical Pattern Space):  regex automata -> token normalization
 *   Y (Semantic Transition):    token -> intermediate representation
 *   X (C Binding Layer):        IR -> C-compatible code
 *
 * This is internal logic, not an exposed pipeline.
 */

typedef struct rift_semantic rift_semantic_t;

RIFT_API rift_semantic_t *rift_semantic_create(void);
RIFT_API void             rift_semantic_destroy(rift_semantic_t *sem);
RIFT_API int              rift_semantic_analyze(rift_semantic_t *sem, rift_ast_node_t *ast);
RIFT_API int              rift_semantic_resolve_types(rift_semantic_t *sem);
RIFT_API int              rift_semantic_tag_regions(rift_semantic_t *sem);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_SEMANTIC_H */
