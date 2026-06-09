#include "rift/riftlang.h"
#include "rift/regex.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct language_entry {
    char *name;
    char *value;
    int pattern;
} language_entry_t;

struct riftlang_context {
    language_entry_t *bindings;
    size_t binding_count;
};

struct riftlang_program {
    language_entry_t *entries;
    size_t entry_count;
    char **pattern_names;
    size_t pattern_count;
};

typedef struct polar_pair {
    RIFTRegex regex;
    char *output;
    uint32_t priority;
} polar_pair_t;

struct RiftPatternEngine {
    RiftExecutionMode mode;
    polar_pair_t *pairs;
    size_t pair_count;
    size_t capacity;
};

static char *lang_strndup(const char *value, size_t length) {
    char *copy = (char *)malloc(length + 1);
    if (!copy) return NULL;
    memcpy(copy, value, length);
    copy[length] = '\0';
    return copy;
}

static char *lang_strdup(const char *value) {
    return value ? lang_strndup(value, strlen(value)) : NULL;
}

static const char *skip_space(const char *cursor) {
    for (;;) {
        while (*cursor && isspace((unsigned char)*cursor)) cursor++;
        if (cursor[0] == '/' && cursor[1] == '/') {
            while (*cursor && *cursor != '\n') cursor++;
            continue;
        }
        if (*cursor == '#') {
            while (*cursor && *cursor != '\n') cursor++;
            continue;
        }
        return cursor;
    }
}

static language_entry_t *find_entry(RLangProgram *program, const char *name) {
    size_t i;
    for (i = program->entry_count; i > 0; i--) {
        if (strcmp(program->entries[i - 1].name, name) == 0) {
            return &program->entries[i - 1];
        }
    }
    return NULL;
}

static language_entry_t *find_binding(RLangContext *ctx, const char *name) {
    size_t i;
    for (i = ctx->binding_count; i > 0; i--) {
        if (strcmp(ctx->bindings[i - 1].name, name) == 0) {
            return &ctx->bindings[i - 1];
        }
    }
    return NULL;
}

static int append_text(char **buffer, size_t *length, size_t *capacity,
                       const char *text, size_t text_length) {
    char *next;
    if (*length + text_length + 1 > *capacity) {
        size_t next_capacity = *capacity ? *capacity : 64;
        while (next_capacity < *length + text_length + 1) next_capacity *= 2;
        next = (char *)realloc(*buffer, next_capacity);
        if (!next) return 0;
        *buffer = next;
        *capacity = next_capacity;
    }
    memcpy(*buffer + *length, text, text_length);
    *length += text_length;
    (*buffer)[*length] = '\0';
    return 1;
}

static int append_literal(char **buffer, size_t *length, size_t *capacity,
                          const char *text, size_t text_length) {
    size_t i;
    const char *special = ".^$|()[]{}*+?\\";
    for (i = 0; i < text_length; i++) {
        if (strchr(special, text[i]) &&
            !append_text(buffer, length, capacity, "\\", 1)) return 0;
        if (!append_text(buffer, length, capacity, text + i, 1)) return 0;
    }
    return 1;
}

static char *compile_expression(RLangContext *ctx, RLangProgram *program,
                                const char *expression) {
    const char *cursor = expression;
    char *result = NULL;
    size_t length = 0;
    size_t capacity = 0;
    while (*(cursor = skip_space(cursor))) {
        if (*cursor == '@' || *cursor == ';') {
            cursor++;
            continue;
        }
        if ((*cursor == 'r' || *cursor == 'R') && cursor[1] == '"') {
            const char *start = cursor + 2;
            const char *end = start;
            while (*end && *end != '"') {
                if (*end == '\\' && end[1]) end++;
                end++;
            }
            if (!*end || !append_text(&result, &length, &capacity,
                                      start, (size_t)(end - start))) goto fail;
            cursor = end + 1;
            continue;
        }
        if (*cursor == '/') {
            const char *start = ++cursor;
            while (*cursor && *cursor != '/') {
                if (*cursor == '\\' && cursor[1]) cursor++;
                cursor++;
            }
            if (!*cursor || !append_text(&result, &length, &capacity,
                                         start, (size_t)(cursor - start))) goto fail;
            cursor++;
            continue;
        }
        if (*cursor == '"') {
            const char *start = ++cursor;
            while (*cursor && *cursor != '"') {
                if (*cursor == '\\' && cursor[1]) cursor++;
                cursor++;
            }
            if (!*cursor || !append_literal(&result, &length, &capacity,
                                            start, (size_t)(cursor - start))) goto fail;
            cursor++;
            continue;
        }
        if (isalpha((unsigned char)*cursor) || *cursor == '_') {
            const char *start = cursor++;
            char *name;
            language_entry_t *entry;
            while (isalnum((unsigned char)*cursor) || *cursor == '_') cursor++;
            name = lang_strndup(start, (size_t)(cursor - start));
            if (!name) goto fail;
            entry = find_entry(program, name);
            if (!entry) entry = find_binding(ctx, name);
            if (entry) {
                if (!append_text(&result, &length, &capacity, "(", 1) ||
                    !append_text(&result, &length, &capacity,
                                 entry->value, strlen(entry->value)) ||
                    !append_text(&result, &length, &capacity, ")", 1)) {
                    free(name);
                    goto fail;
                }
            } else if (!append_literal(&result, &length, &capacity,
                                       name, strlen(name))) {
                free(name);
                goto fail;
            }
            free(name);
            continue;
        }
        if (*cursor == '{') {
            const char *end = strchr(cursor, '}');
            int digits_only = 1;
            const char *scan;
            if (end) {
                for (scan = cursor + 1; scan < end; scan++) {
                    if (*scan != ',' && !isdigit((unsigned char)*scan)) {
                        digits_only = 0;
                        break;
                    }
                }
                if (digits_only) {
                    if (!append_text(&result, &length, &capacity, cursor,
                                     (size_t)(end - cursor + 1))) goto fail;
                    cursor = end + 1;
                    continue;
                }
            }
            cursor++;
            continue;
        }
        if (*cursor == '}') {
            cursor++;
            continue;
        }
        if (!append_text(&result, &length, &capacity, cursor, 1)) goto fail;
        cursor++;
    }
    if (!result) result = lang_strdup("");
    return result;

fail:
    free(result);
    return NULL;
}

static int program_add(RLangProgram *program, const char *name,
                       char *value, int pattern) {
    language_entry_t *entries = (language_entry_t *)realloc(
        program->entries, (program->entry_count + 1) * sizeof(*entries));
    if (!entries) return 0;
    program->entries = entries;
    entries = &program->entries[program->entry_count++];
    entries->name = lang_strdup(name);
    entries->value = value;
    entries->pattern = pattern;
    if (!entries->name) return 0;
    if (pattern) {
        char **names = (char **)realloc(
            program->pattern_names,
            (program->pattern_count + 1) * sizeof(*names));
        if (!names) return 0;
        program->pattern_names = names;
        program->pattern_names[program->pattern_count++] = entries->name;
    }
    return 1;
}

static char *compile_block(RLangContext *ctx, RLangProgram *program,
                           const char *expression) {
    const char *open = skip_space(expression);
    const char *close;
    char *body;
    char *line;
    char *last = NULL;
    if (*open != '{') return compile_expression(ctx, program, expression);
    close = strrchr(open, '}');
    if (!close) return NULL;
    body = lang_strndup(open + 1, (size_t)(close - open - 1));
    if (!body) return NULL;

    line = strtok(body, "\n;");
    while (line) {
        const char *cursor = skip_space(line);
        if (strncmp(cursor, "let ", 4) == 0) {
            const char *name_start = skip_space(cursor + 4);
            const char *name_end = name_start;
            const char *equals;
            char *name;
            char *value;
            while (isalnum((unsigned char)*name_end) || *name_end == '_') name_end++;
            equals = strchr(name_end, '=');
            if (equals) {
                name = lang_strndup(name_start, (size_t)(name_end - name_start));
                value = compile_expression(ctx, program, equals + 1);
                if (name && value) program_add(program, name, value, 0);
                else free(value);
                free(name);
            }
        } else if (*cursor) {
            free(last);
            last = lang_strdup(cursor);
        }
        line = strtok(NULL, "\n;");
    }
    free(body);
    if (!last) return NULL;
    {
        char *compiled = compile_expression(ctx, program, last);
        free(last);
        return compiled;
    }
}

RLangContext *rlang_context_create(void) {
    return (RLangContext *)calloc(1, sizeof(RLangContext));
}

void rlang_context_free(RLangContext *ctx) {
    size_t i;
    if (!ctx) return;
    for (i = 0; i < ctx->binding_count; i++) {
        free(ctx->bindings[i].name);
        free(ctx->bindings[i].value);
    }
    free(ctx->bindings);
    free(ctx);
}

bool rlang_set_binding(RLangContext *ctx, const char *name, const char *value) {
    language_entry_t *entry;
    language_entry_t *bindings;
    if (!ctx || !name || !value) return false;
    entry = find_binding(ctx, name);
    if (entry) {
        char *replacement = lang_strdup(value);
        if (!replacement) return false;
        free(entry->value);
        entry->value = replacement;
        return true;
    }
    bindings = (language_entry_t *)realloc(
        ctx->bindings, (ctx->binding_count + 1) * sizeof(*bindings));
    if (!bindings) return false;
    ctx->bindings = bindings;
    entry = &ctx->bindings[ctx->binding_count++];
    memset(entry, 0, sizeof(*entry));
    entry->name = lang_strdup(name);
    entry->value = lang_strdup(value);
    return entry->name && entry->value;
}

RLangCompileResult rlang_compile(RLangContext *ctx, const char *source) {
    RLangCompileResult result;
    RLangProgram *program;
    const char *cursor;
    memset(&result, 0, sizeof(result));
    if (!ctx || !source) {
        result.error_message = lang_strdup("context and source are required");
        return result;
    }
    program = (RLangProgram *)calloc(1, sizeof(*program));
    if (!program) {
        result.error_message = lang_strdup("out of memory");
        return result;
    }

    cursor = source;
    while (*(cursor = skip_space(cursor))) {
        int pattern = 0;
        const char *name_start;
        const char *name_end;
        const char *expression_start;
        const char *expression_end;
        char *name;
        char *expression;
        char *compiled;
        int depth = 0;

        if (strncmp(cursor, "pattern", 7) == 0 &&
            isspace((unsigned char)cursor[7])) {
            pattern = 1;
            cursor += 7;
        } else if (strncmp(cursor, "let", 3) == 0 &&
                   isspace((unsigned char)cursor[3])) {
            cursor += 3;
        } else {
            while (*cursor && *cursor != '\n') cursor++;
            continue;
        }
        cursor = skip_space(cursor);
        name_start = cursor;
        while (isalnum((unsigned char)*cursor) || *cursor == '_') cursor++;
        name_end = cursor;
        cursor = skip_space(cursor);
        if (*cursor != '=') {
            result.error_message = lang_strdup("expected '=' after declaration");
            goto fail;
        }
        cursor = skip_space(cursor + 1);
        expression_start = cursor;
        if (*cursor == '{') {
            do {
                if (*cursor == '{') depth++;
                if (*cursor == '}') depth--;
                cursor++;
            } while (*cursor && depth > 0);
            expression_end = cursor;
        } else {
            while (*cursor && *cursor != '\n' && *cursor != ';') cursor++;
            expression_end = cursor;
        }
        name = lang_strndup(name_start, (size_t)(name_end - name_start));
        expression = lang_strndup(
            expression_start, (size_t)(expression_end - expression_start));
        if (!name || !expression) {
            free(name);
            free(expression);
            result.error_message = lang_strdup("out of memory");
            goto fail;
        }
        compiled = compile_block(ctx, program, expression);
        free(expression);
        if (!compiled || !program_add(program, name, compiled, pattern)) {
            free(name);
            free(compiled);
            result.error_message = lang_strdup("invalid pattern expression");
            goto fail;
        }
        free(name);
        if (*cursor == ';') cursor++;
    }

    if (!program->pattern_count) {
        result.error_message = lang_strdup("no pattern declarations found");
        goto fail;
    }
    result.success = true;
    result.program = program;
    return result;

fail:
    rlang_program_free(program);
    return result;
}

RLangCompileResult rlang_load_file(RLangContext *ctx, const char *filepath) {
    RLangCompileResult result;
    FILE *stream;
    long size;
    char *source;
    memset(&result, 0, sizeof(result));
    if (!filepath || !(stream = fopen(filepath, "rb"))) {
        result.error_message = lang_strdup("unable to open RiftLang source");
        return result;
    }
    if (fseek(stream, 0, SEEK_END) != 0 ||
        (size = ftell(stream)) < 0 ||
        fseek(stream, 0, SEEK_SET) != 0) {
        fclose(stream);
        result.error_message = lang_strdup("unable to read RiftLang source");
        return result;
    }
    source = (char *)malloc((size_t)size + 1);
    if (!source || fread(source, 1, (size_t)size, stream) != (size_t)size) {
        fclose(stream);
        free(source);
        result.error_message = lang_strdup("unable to read RiftLang source");
        return result;
    }
    fclose(stream);
    source[size] = '\0';
    result = rlang_compile(ctx, source);
    free(source);
    return result;
}

char *rlang_execute_pattern(RLangProgram *program, const char *pattern_name) {
    language_entry_t *entry;
    if (!program || !pattern_name) return NULL;
    entry = find_entry(program, pattern_name);
    return entry && entry->pattern ? lang_strdup(entry->value) : NULL;
}

bool rlang_get_patterns(RLangProgram *program, const char ***out_patterns,
                        size_t *out_count) {
    if (!program || !out_patterns || !out_count) return false;
    *out_patterns = (const char **)program->pattern_names;
    *out_count = program->pattern_count;
    return true;
}

void rlang_program_free(RLangProgram *program) {
    size_t i;
    if (!program) return;
    for (i = 0; i < program->entry_count; i++) {
        free(program->entries[i].name);
        free(program->entries[i].value);
    }
    free(program->entries);
    free(program->pattern_names);
    free(program);
}

void rlang_compile_result_free(RLangCompileResult *result) {
    if (!result) return;
    free(result->error_message);
    result->error_message = NULL;
    if (result->program) rlang_program_free(result->program);
    result->program = NULL;
    result->success = false;
}

void *rlang_parse(const char *source) {
    RLangContext *ctx = rlang_context_create();
    RLangCompileResult result = rlang_compile(ctx, source);
    rlang_context_free(ctx);
    free(result.error_message);
    return result.success ? result.program : NULL;
}

void rlang_ast_free(void *node) {
    rlang_program_free((RLangProgram *)node);
}

bool rlang_validate(const char *source, char **out_error) {
    RLangContext *ctx = rlang_context_create();
    RLangCompileResult result = rlang_compile(ctx, source);
    bool valid = result.success;
    rlang_context_free(ctx);
    if (out_error) {
        *out_error = result.error_message;
        result.error_message = NULL;
    }
    rlang_compile_result_free(&result);
    return valid;
}

void rlang_print_program(RLangProgram *program, FILE *stream) {
    size_t i;
    if (!program || !stream) return;
    for (i = 0; i < program->entry_count; i++) {
        if (program->entries[i].pattern) {
            fprintf(stream, "pattern %s = r\"%s\"\n",
                    program->entries[i].name, program->entries[i].value);
        }
    }
}

static void export_c_string(FILE *stream, const char *value) {
    while (*value) {
        if (*value == '\\' || *value == '"') fputc('\\', stream);
        if (*value == '\n') fputs("\\n", stream);
        else fputc(*value, stream);
        value++;
    }
}

bool rlang_export_c(RLangProgram *program, const char *filepath) {
    FILE *stream;
    size_t i;
    if (!program || !filepath || !(stream = fopen(filepath, "wb"))) return false;
    fputs("/* Generated by RiftLang. */\n#include <stddef.h>\n\n", stream);
    for (i = 0; i < program->entry_count; i++) {
        if (!program->entries[i].pattern) continue;
        fprintf(stream, "const char *rift_pattern_%s = \"",
                program->entries[i].name);
        export_c_string(stream, program->entries[i].value);
        fputs("\";\n", stream);
    }
    fclose(stream);
    return true;
}

RiftPatternEngine *rift_pattern_engine_create(RiftExecutionMode mode) {
    RiftPatternEngine *engine =
        (RiftPatternEngine *)calloc(1, sizeof(*engine));
    if (engine) engine->mode = mode;
    return engine;
}

void rift_pattern_engine_destroy(RiftPatternEngine *engine) {
    size_t i;
    if (!engine) return;
    for (i = 0; i < engine->pair_count; i++) {
        rift_regex_free(&engine->pairs[i].regex);
        free(engine->pairs[i].output);
    }
    free(engine->pairs);
    free(engine);
}

bool rift_pattern_engine_add_pair(RiftPatternEngine *engine,
                                  const char *left_pattern,
                                  const char *right_pattern,
                                  uint32_t priority) {
    polar_pair_t *pairs;
    polar_pair_t *pair;
    if (!engine || !left_pattern || !right_pattern) return false;
    if (engine->pair_count == engine->capacity) {
        size_t capacity = engine->capacity ? engine->capacity * 2 : 8;
        pairs = (polar_pair_t *)realloc(
            engine->pairs, capacity * sizeof(*pairs));
        if (!pairs) return false;
        engine->pairs = pairs;
        engine->capacity = capacity;
    }
    pair = &engine->pairs[engine->pair_count];
    memset(pair, 0, sizeof(*pair));
    if (rift_regex_compile(&pair->regex, left_pattern) != 0) return false;
    pair->output = lang_strdup(right_pattern);
    if (!pair->output) {
        rift_regex_free(&pair->regex);
        return false;
    }
    pair->priority = priority;
    engine->pair_count++;
    return true;
}

bool rift_pattern_engine_compile(RiftPatternEngine *engine) {
    return engine && engine->pair_count > 0;
}

char *rift_pattern_engine_match(RiftPatternEngine *engine, const char *input,
                                size_t *output_len,
                                uint32_t *matched_priority) {
    polar_pair_t *best = NULL;
    size_t i;
    char *result;
    if (output_len) *output_len = 0;
    if (!engine || !input || !output_len) return NULL;
    for (i = 0; i < engine->pair_count; i++) {
        polar_pair_t *pair = &engine->pairs[i];
        if (rift_regex_match(&pair->regex, input) == 0 &&
            (!best || pair->priority < best->priority)) {
            best = pair;
        }
    }
    if (!best) return NULL;
    result = lang_strdup(best->output);
    if (!result) return NULL;
    *output_len = strlen(result);
    if (matched_priority) *matched_priority = best->priority;
    return result;
}

RiftResultMatrix2x2 *rift_result_matrix_create(double threshold) {
    RiftResultMatrix2x2 *matrix =
        (RiftResultMatrix2x2 *)calloc(1, sizeof(*matrix));
    if (!matrix) return NULL;
    matrix->matrix[0][0] = RIFT_POLICY_DENY;
    matrix->matrix[0][1] = RIFT_POLICY_DENY;
    matrix->matrix[1][0] = RIFT_POLICY_DENY;
    matrix->matrix[1][1] = RIFT_POLICY_ALLOW;
    matrix->validation_threshold =
        threshold > 0.0 && threshold <= 1.0 ? threshold : 0.85;
    return matrix;
}

void rift_result_matrix_destroy(RiftResultMatrix2x2 *matrix) {
    free(matrix);
}

RiftPolicyResult rift_policy_validate(RiftResultMatrix2x2 *matrix,
                                      bool input_valid, bool output_valid) {
    RiftPolicyResult result;
    if (!matrix) return RIFT_POLICY_DENY;
    result = matrix->matrix[input_valid ? 1 : 0][output_valid ? 1 : 0];
    if (result == RIFT_POLICY_ALLOW) matrix->validations_passed++;
    else if (result == RIFT_POLICY_DEFER) matrix->validations_deferred++;
    else matrix->validations_failed++;
    return result;
}

double rift_policy_get_validation_ratio(const RiftResultMatrix2x2 *matrix) {
    uint64_t total;
    if (!matrix) return 0.0;
    total = matrix->validations_passed + matrix->validations_failed;
    return total ? (double)matrix->validations_passed / (double)total : 0.0;
}

bool rift_policy_meets_threshold(const RiftResultMatrix2x2 *matrix) {
    return matrix &&
        rift_policy_get_validation_ratio(matrix) >= matrix->validation_threshold;
}
