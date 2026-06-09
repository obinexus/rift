#include "rift/pattern.h"
#include <stdlib.h>
#include <string.h>

/* strdup may not be available in strict C11 */
static char *rift_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char *copy = (char *)malloc(len);
    if (copy) memcpy(copy, s, len);
    return copy;
}

struct rift_pattern_engine {
    rift_pattern_t *patterns;
    int             count;
    int             capacity;
};

rift_pattern_engine_t *rift_pattern_create(void) {
    rift_pattern_engine_t *engine = (rift_pattern_engine_t *)calloc(1, sizeof(rift_pattern_engine_t));
    if (!engine) return NULL;
    engine->capacity = 16;
    engine->patterns = (rift_pattern_t *)calloc(engine->capacity, sizeof(rift_pattern_t));
    return engine;
}

void rift_pattern_destroy(rift_pattern_engine_t *engine) {
    if (!engine) return;
    for (int i = 0; i < engine->count; i++) {
        free(engine->patterns[i].expression);
        rift_regex_free(&engine->patterns[i].compiled);
    }
    free(engine->patterns);
    free(engine);
}

int rift_pattern_compile(rift_pattern_engine_t *engine, const char *expr, int is_static) {
    if (!engine || !expr) return -1;
    if (engine->count >= engine->capacity) return -1;

    rift_pattern_t *p = &engine->patterns[engine->count];
    p->expression = rift_strdup(expr);
    p->is_static = is_static;

    if (rift_regex_compile(&p->compiled, expr) != 0) {
        free(p->expression);
        return -1;
    }

    engine->count++;
    return 0;
}

int rift_pattern_match(const rift_pattern_engine_t *engine, const char *input,
                        rift_token_t *out_token) {
    if (!engine || !input || !out_token) return -1;

    for (int i = 0; i < engine->count; i++) {
        if (rift_regex_match(&engine->patterns[i].compiled, input) == 0) {
            rift_token_type_t type = engine->patterns[i].is_static
                ? RIFT_TOKEN_PATTERN_STATIC
                : RIFT_TOKEN_PATTERN_DYNAMIC;
            *out_token = rift_token_create(type, input);
            return 0;
        }
    }

    return -1;
}
