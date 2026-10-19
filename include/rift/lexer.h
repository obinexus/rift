#ifndef RIFT_LEXER_H
#define RIFT_LEXER_H

#include "rift/platform.h"
#include "rift/token.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rift_lexer rift_lexer_t;

RIFT_API rift_lexer_t *rift_lexer_create(const char *source, size_t length);
RIFT_API void          rift_lexer_destroy(rift_lexer_t *lexer);
RIFT_API rift_token_t  rift_lexer_next(rift_lexer_t *lexer);
RIFT_API rift_token_t  rift_lexer_peek(const rift_lexer_t *lexer);
RIFT_API int           rift_lexer_eof(const rift_lexer_t *lexer);
/* Non-zero once rift_lexer_next could not allocate a token's value. That
 * call returns an invalid RIFT_TOKEN_UNKNOWN token and lexing stops: later
 * calls return EOF. */
RIFT_API int           rift_lexer_has_error(const rift_lexer_t *lexer);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_LEXER_H */
