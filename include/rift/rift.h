#ifndef RIFT_H
#define RIFT_H

/*
 * RIFT Is a Flexible Translator
 * Master public header
 *
 * No explicit stages. No YACC. Just RIFT.
 */

#include "rift/version.h"
#include "rift/platform.h"
#include "rift/regex.h"
#include "rift/memory.h"
#include "rift/token.h"
#include "rift/lexer.h"
#include "rift/parser.h"
#include "rift/bytecode.h"
#include "rift/semantic.h"
#include "rift/codegen.h"
#include "rift/emitter.h"
#include "rift/codec.h"
#include "rift/bridge.h"
#include "rift/riftbridge.h"
#include "rift/pattern.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Error codes */
typedef enum rift_error {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_INVALID_ARGUMENT,
    RIFT_ERROR_MEMORY_ALLOCATION,
    RIFT_ERROR_FILE_ACCESS,
    RIFT_ERROR_PARSE_FAILURE,
    RIFT_ERROR_SEMANTIC_FAILURE,
    RIFT_ERROR_CODEGEN_FAILURE,
    RIFT_ERROR_EMIT_FAILURE,
    RIFT_ERROR_LINK_FAILURE,
    RIFT_ERROR_UNSUPPORTED,
    RIFT_ERROR_UNKNOWN
} rift_error_t;

/* RIFT context: the single runtime state */
typedef struct rift_context {
    rift_lexer_t           *lexer;
    rift_parser_t          *parser;
    rift_semantic_t        *semantic;
    rift_codegen_t         *codegen;
    rift_emitter_t         *emitter;
    rift_bridge_t          *bridge;
    rift_pattern_engine_t  *pattern;
    rift_error_t            last_error;
    int                     verbose;
    int                     debug;
} rift_context_t;

RIFT_API rift_context_t *rift_init(void);
RIFT_API void            rift_cleanup(rift_context_t *ctx);
RIFT_API rift_error_t    rift_compile(rift_context_t *ctx,
                                       const char *input_path,
                                       const char *output_path);
RIFT_API const char     *rift_error_string(rift_error_t err);

/* Legacy initialization API retained for source compatibility. */
RIFT_API int             librift_initialize(void);
RIFT_API void            librift_cleanup(void);
RIFT_API int             librift_check_version(void);
RIFT_API const char     *librift_get_version_string(void);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_H */
