#include "../../include/rift.h"

// CRITICAL FIX: Extract regex pattern from RIFT syntax
RiftPattern* rift_parse_pattern(const char* pattern_string) {
    if (!pattern_string) return NULL;
    
    RiftPattern* pattern = calloc(1, sizeof(RiftPattern));
    if (!pattern) return NULL;
    
    pattern->raw_pattern = strdup(pattern_string);
    pattern->is_compiled = false;
    
    const char* input = pattern_string;
    
    // Skip R" prefix if present
    if (strncmp(input, "R\"", 2) == 0) {
        input += 2;
    } else if (*input == '"') {
        input += 1;
    }
    
    // Extract regex pattern (before first '/')
    const char* pattern_end = strchr(input, '/');
    if (pattern_end) {
        // Extract just the pattern part: "[A-Z]+" from "[A-Z]+/gmi[t]"
        pattern->regex_pattern = strndup(input, pattern_end - input);
        
        // Extract flags
        const char* flags_start = pattern_end + 1;
        const char* flags_end = strchr(flags_start, '[');
        if (!flags_end) flags_end = strchr(flags_start, '"');
        if (!flags_end) flags_end = flags_start + strlen(flags_start);
        
        pattern->flags = strndup(flags_start, flags_end - flags_start);
        
        // Extract mode
        const char* mode_start = strchr(flags_start, '[');
        if (mode_start && mode_start[1] && mode_start[2] == ']') {
            pattern->mode = (RiftMatchMode)mode_start[1];
        } else {
            pattern->mode = RIFT_MODE_TOP_DOWN;
        }
    } else {
        // No flags, just pattern
        const char* end_quote = strchr(input, '"');
        if (end_quote) {
            pattern->regex_pattern = strndup(input, end_quote - input);
        } else {
            pattern->regex_pattern = strdup(input);
        }
        pattern->flags = strdup("");
        pattern->mode = RIFT_MODE_TOP_DOWN;
    }
    
    return pattern;
}

void rift_pattern_destroy(RiftPattern* pattern) {
    if (!pattern) return;
    
    if (pattern->is_compiled) {
        regfree(&pattern->compiled_regex);
    }
    
    free(pattern->raw_pattern);
    free(pattern->regex_pattern);
    free(pattern->flags);
    free(pattern);
}

RiftResult rift_pattern_compile(RiftPattern* pattern) {
    if (!pattern || !pattern->regex_pattern) {
        return RIFT_ERROR_NULL_POINTER;
    }
    
    if (pattern->is_compiled) {
        regfree(&pattern->compiled_regex);
    }
    
    int regex_flags = REG_EXTENDED;
    if (pattern->flags && strchr(pattern->flags, 'i')) {
        regex_flags |= REG_ICASE;
    }
    
    int result = regcomp(&pattern->compiled_regex, pattern->regex_pattern, regex_flags);
    if (result != 0) {
        return RIFT_ERROR_COMPILATION_FAILED;
    }
    
    pattern->is_compiled = true;
    return RIFT_SUCCESS;
}
