#ifndef RIFT_CODEGEN_H
#define RIFT_CODEGEN_H

#include "rift/platform.h"
#include "rift/parser.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rift_codegen rift_codegen_t;

RIFT_API rift_codegen_t *rift_codegen_create(void);
RIFT_API void            rift_codegen_destroy(rift_codegen_t *cg);
RIFT_API int             rift_codegen_generate(rift_codegen_t *cg, const rift_ast_node_t *ast);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CODEGEN_H */
