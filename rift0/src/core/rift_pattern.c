#include "../../include/rift0/rift.h"
#include <ctype.h>
#include <regex.h>

// Fix for test failure: properly extract regex from RIFT syntax
RiftPattern* rift_parse_pattern(const char* pattern_string) {
    if (!pattern_string) return NULL;
    
    RiftPattern* pattern = calloc(1, sizeof(RiftPattern));
    if (!pattern) return NULL;
    
    pattern->raw_pattern = strdup(pattern_string);
    pattern->is_compiled = false;
    
    // Parse RIFT syntax: "[A-Z]+/gmi[t]" or R"[A-Z]+/gmi[t]"
    const char* input = pattern_string;
    
    // Skip R" prefix if present
    if (strncmp(input, "R\"", 2) == 0) {
        input += 2;
    } else if (*input == '"') {
        input += 1;
    }
    
    // Extract regex pattern (everything before first '/')
    const char* pattern_end = strchr(input, '/');
    if (pattern_end) {
        // Pattern with flags: "[A-Z]+/gmi[t]"
        size_t pattern_len = pattern_end - input;
        pattern->regex_pattern = strndup(input, pattern_len);
        
        // Extract flags (between / and [)
        const char* flags_start = pattern_end + 1;
        const char* flags_end = strchr(flags_start, '[');
        if (!flags_end) {
            flags_end = strchr(flags_start, '"');
            if (!flags_end) {
                flags_end = flags_start + strlen(flags_start);
            }
        }
        
        if (flags_end > flags_start) {
            pattern->flags = strndup(flags_start, flags_end - flags_start);
        } else {
            pattern->flags = strdup("");
        }
        
        // Extract mode from [mode]
        const char* mode_start = strchr(flags_start, '[');
        if (mode_start && mode_start[1] && mode_start[2] == ']') {
            pattern->mode = (RiftMatchMode)mode_start[1];
        } else {
            pattern->mode = RIFT_MODE_TOP_DOWN;
        }
    } else {
        // Pattern without flags: just "[A-Z]+"
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

// Enhanced pattern compilation with POSIX regex
RiftResult rift_pattern_compile(RiftPattern* pattern) {
    if (!pattern || !pattern->regex_pattern) {
        return RIFT_ERROR_NULL_POINTER;
    }
    
    if (pattern->is_compiled) {
        regfree(&pattern->compiled_regex);
    }
    
    // Compile regex with appropriate flags
    int regex_flags = REG_EXTENDED;
    
    // Process flags string
    if (pattern->flags) {
        for (const char* flag = pattern->flags; *flag; flag++) {
            switch (*flag) {
                case 'i': regex_flags |= REG_ICASE; break;
                case 'm': /* Multiline - handled in matching */ break;
                case 'g': /* Global - handled in matching */ break;
                default: break;
            }
        }
    }
    
    int result = regcomp(&pattern->compiled_regex, pattern->regex_pattern, regex_flags);
    if (result != 0) {
        return RIFT_ERROR_COMPILATION_FAILED;
    }
    
    pattern->is_compiled = true;
    return RIFT_SUCCESS;
}

// Thread-safe token creation with matched state preservation
typedef struct RiftTokenMemory {
    RiftToken** tokens;
    size_t capacity;
    size_t count;
    pthread_mutex_t mutex;
    _Atomic bool processing;
} RiftTokenMemory;

static RiftTokenMemory* global_token_memory = NULL;
static pthread_once_t token_memory_once = PTHREAD_ONCE_INIT;

static void init_token_memory(void) {
    global_token_memory = calloc(1, sizeof(RiftTokenMemory));
    if (global_token_memory) {
        global_token_memory->capacity = 1024;
        global_token_memory->tokens = calloc(global_token_memory->capacity, sizeof(RiftToken*));
        pthread_mutex_init(&global_token_memory->mutex, NULL);
        atomic_init(&global_token_memory->processing, false);
    }
}

RiftToken* rift_token_create_thread_safe(const char* type, const char* value, 
                                        size_t line, size_t column, 
                                        RiftState* matched_state) {
    pthread_once(&token_memory_once, init_token_memory);
    
    if (!global_token_memory) return NULL;
    
    pthread_mutex_lock(&global_token_memory->mutex);
    
    RiftToken* token = rift_token_create(type, value, line, column);
    if (token && matched_state) {
        token->matched_state = matched_state;
    }
    
    // Store in thread-safe memory
    if (global_token_memory->count < global_token_memory->capacity) {
        global_token_memory->tokens[global_token_memory->count++] = token;
    }
    
    pthread_mutex_unlock(&global_token_memory->mutex);
    
    return token;
}

RiftResult rift_pattern_compile(RiftPattern* pattern) {
    if (!pattern || !pattern->regex_pattern) {
        return RIFT_ERROR_NULL_POINTER;
    }
    
    int flags = REG_EXTENDED;
    
    // Process RIFT flags
    if (pattern->flags) {
        if (strchr(pattern->flags, 'i')) flags |= REG_ICASE;
        // Note: 'g' (global) and 'm' (multiline) are handled at match time
    }
    
    int result = regcomp(&pattern->compiled_regex, pattern->regex_pattern, flags);
    pattern->is_compiled = (result == 0);
    
    if (result != 0) {
        char error_buffer[256];
        regerror(result, &pattern->compiled_regex, error_buffer, sizeof(error_buffer));
        fprintf(stderr, "RIFT Pattern compilation failed: %s\n", error_buffer);
        return RIFT_ERROR_COMPILATION_FAILED;
    }
    
    return RIFT_SUCCESS;
}

void rift_pattern_destroy(RiftPattern* pattern) {
    if (!pattern) return;
    
    free(pattern->raw_pattern);
    free(pattern->regex_pattern);
    free(pattern->flags);
    
    if (pattern->is_compiled) {
        regfree(&pattern->compiled_regex);
    }
    
    free(pattern);
}
