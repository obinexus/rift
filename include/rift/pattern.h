#ifndef RIFT_PATTERN_H
#define RIFT_PATTERN_H

#include "rift/platform.h"
#include "rift/regex.h"
#include "rift/token.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * RIFT Pattern Matching:
 *   R""(str) - static string template (C++ raw string / compile-time)
 *   RIFT''   - dynamic character template (C / runtime)
 *
 * Patterns use RIFT Regular Expressions for namespace matching.
 */

typedef struct rift_pattern {
    char           *expression;
    int             is_static;     /* 1 = R"" static, 0 = R'' dynamic */
    rift_regex_t    compiled;
} rift_pattern_t;

typedef struct rift_pattern_engine rift_pattern_engine_t;

RIFT_API rift_pattern_engine_t *rift_pattern_create(void);
RIFT_API void                   rift_pattern_destroy(rift_pattern_engine_t *engine);
RIFT_API int                    rift_pattern_compile(rift_pattern_engine_t *engine,
                                                      const char *expr, int is_static);
RIFT_API int                    rift_pattern_match(const rift_pattern_engine_t *engine,
                                                    const char *input,
                                                    rift_token_t *out_token);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_PATTERN_H */
