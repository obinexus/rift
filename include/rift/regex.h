#ifndef RIFT_REGEX_H
#define RIFT_REGEX_H

#include <stddef.h>
#include <stdint.h>
#include "rift/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    RIFT_REGEX_NONE = 0,
    RIFT_REGEX_CASE_INSENSITIVE = 1u << 0,
    RIFT_REGEX_MULTILINE = 1u << 1
};

typedef struct RIFTRegex {
    void *implementation;
    char *pattern;
    uint32_t flags;
    int error_offset;
    char error_message[128];
} RIFTRegex;

typedef RIFTRegex rift_regex_t;

#define RIFT_REGEX(pattern_literal) (pattern_literal)

RIFT_API int rift_regex_compile(RIFTRegex *regex, const char *pattern);
RIFT_API int rift_regex_compile_ex(RIFTRegex *regex, const char *pattern, uint32_t flags);
RIFT_API int rift_regex_match(const RIFTRegex *regex, const char *text);
RIFT_API int rift_regex_match_n(const RIFTRegex *regex, const char *text, size_t length);
RIFT_API int rift_regex_is_match(const char *pattern, const char *text);
RIFT_API const char *rift_regex_error(const RIFTRegex *regex);
RIFT_API void rift_regex_free(RIFTRegex *regex);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_REGEX_H */
