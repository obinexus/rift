#include "rift/riftlang.h"
#include "rift/regex.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
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
    bool allow_unknown;
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

#define NO_DECLARATION ((size_t)-1)

/* A top-level `pattern` or `let`, found before anything is compiled. */
typedef struct declaration {
    char *name;
    char *expression;
    const char *name_at;        /* where the name starts in the source */
    const char *expression_at;  /* where the expression starts in the source */
    int pattern;
} declaration_t;

typedef struct compile_state {
    RLangContext *ctx;
    RLangProgram *program;
    const char *source;
    const char *path;           /* diagnostic prefix, or NULL */
    declaration_t *declarations;
    size_t declaration_count;
    size_t current;             /* declaration being compiled */
    char *error;                /* first error, formatted */
    char *warnings;
    size_t warnings_length;
    size_t warnings_capacity;
    int failed;
} compile_state_t;

static char *vformat_text(const char *format, va_list args) {
    va_list copy;
    int length;
    char *text;
    va_copy(copy, args);
    length = vsnprintf(NULL, 0, format, copy);
    va_end(copy);
    if (length < 0) return NULL;
    text = (char *)malloc((size_t)length + 1);
    if (text) vsnprintf(text, (size_t)length + 1, format, args);
    return text;
}

static char *format_text(const char *format, ...) {
    va_list args;
    char *text;
    va_start(args, format);
    text = vformat_text(format, args);
    va_end(args);
    return text;
}

static void source_position(const char *source, const char *where,
                            unsigned *line, unsigned *column) {
    const char *cursor;
    *line = 1;
    *column = 1;
    for (cursor = source; cursor < where && *cursor; cursor++) {
        if (*cursor == '\n') {
            (*line)++;
            *column = 1;
        } else {
            (*column)++;
        }
    }
}

/* "[PATH:]LINE:COLUMN: SEVERITY: MESSAGE", or "[PATH: ]SEVERITY: MESSAGE"
 * when the diagnostic has no position. */
static char *vdiagnostic(const compile_state_t *state, const char *where,
                         const char *severity, const char *format,
                         va_list args) {
    const char *path = state->path ? state->path : "";
    char *message = vformat_text(format, args);
    char *diagnostic;
    if (!message) return NULL;
    if (where) {
        unsigned line;
        unsigned column;
        source_position(state->source, where, &line, &column);
        diagnostic = format_text("%s%s%u:%u: %s: %s", path,
                                 state->path ? ":" : "", line, column,
                                 severity, message);
    } else {
        diagnostic = format_text("%s%s%s: %s", path,
                                 state->path ? ": " : "", severity, message);
    }
    free(message);
    return diagnostic;
}

static void out_of_memory(compile_state_t *state) {
    state->failed = 1;
    if (!state->error) state->error = lang_strdup("out of memory");
}

/* Records the first error; later ones are ignored. */
static void error_at(compile_state_t *state, const char *where,
                     const char *format, ...) {
    va_list args;
    if (state->failed) return;
    state->failed = 1;
    va_start(args, format);
    state->error = vdiagnostic(state, where, "error", format, args);
    va_end(args);
    if (!state->error) out_of_memory(state);
}

static void warning_at(compile_state_t *state, const char *where,
                       const char *format, ...) {
    va_list args;
    char *warning;
    va_start(args, format);
    warning = vdiagnostic(state, where, "warning", format, args);
    va_end(args);
    if (!warning ||
        !append_text(&state->warnings, &state->warnings_length,
                     &state->warnings_capacity, warning, strlen(warning)) ||
        !append_text(&state->warnings, &state->warnings_length,
                     &state->warnings_capacity, "\n", 1)) {
        out_of_memory(state);
    }
    free(warning);
}

static const char *source_at(const char *origin, const char *expression,
                             const char *cursor) {
    return origin + (cursor - expression);
}

/* Whether an expression mentions `name`, using compile_expression's
 * lexical rules (comments and literals are skipped). */
static int expression_references(const char *expression, const char *name) {
    const char *cursor = expression;
    size_t name_length = strlen(name);
    while (*(cursor = skip_space(cursor))) {
        char quote = 0;
        if ((*cursor == 'r' || *cursor == 'R') && cursor[1] == '"') {
            quote = '"';
            cursor += 2;
        } else if (*cursor == '"' || *cursor == '/') {
            quote = *cursor++;
        }
        if (quote) {
            while (*cursor && *cursor != quote) {
                if (*cursor == '\\' && cursor[1]) cursor++;
                cursor++;
            }
            if (*cursor) cursor++;
            continue;
        }
        if (isalpha((unsigned char)*cursor) || *cursor == '_') {
            const char *start = cursor;
            while (isalnum((unsigned char)*cursor) || *cursor == '_') cursor++;
            if ((size_t)(cursor - start) == name_length &&
                strncmp(start, name, name_length) == 0) return 1;
            continue;
        }
        cursor++;
    }
    return 0;
}

static size_t find_declaration(const compile_state_t *state, const char *name) {
    size_t i;
    for (i = 0; i < state->declaration_count; i++) {
        if (strcmp(state->declarations[i].name, name) == 0) return i;
    }
    return NO_DECLARATION;
}

/* Breadth-first search through references between declarations. Stores the
 * chain from `from` to `target` (both included) and returns its length, or
 * returns 0 when `target` is unreachable. */
static size_t reference_chain(const compile_state_t *state, size_t from,
                              size_t target, size_t *chain) {
    size_t count = state->declaration_count;
    size_t *parent = (size_t *)malloc(count * sizeof(*parent));
    size_t *queue = (size_t *)malloc(count * sizeof(*queue));
    size_t head = 0;
    size_t tail = 0;
    size_t length = 0;
    size_t node;
    size_t next;
    if (parent && queue) {
        for (node = 0; node < count; node++) parent[node] = NO_DECLARATION;
        parent[from] = from;
        queue[tail++] = from;
        while (head < tail && parent[target] == NO_DECLARATION) {
            node = queue[head++];
            for (next = 0; next < count; next++) {
                if (parent[next] == NO_DECLARATION &&
                    expression_references(state->declarations[node].expression,
                                          state->declarations[next].name)) {
                    parent[next] = node;
                    queue[tail++] = next;
                }
            }
        }
        if (parent[target] != NO_DECLARATION) {
            for (node = target; ; node = parent[node]) {
                chain[length++] = node;
                if (node == from) break;
            }
            for (node = 0; node < length / 2; node++) {
                next = chain[node];
                chain[node] = chain[length - 1 - node];
                chain[length - 1 - node] = next;
            }
        }
    }
    free(parent);
    free(queue);
    return length;
}

/* Explains why `name` (at `where`) does not resolve. */
static void report_unresolved(compile_state_t *state, const char *name,
                              const char *where) {
    const declaration_t *current = &state->declarations[state->current];
    size_t later = find_declaration(state, name);
    size_t *chain;
    size_t length;

    if (strcmp(name, current->name) == 0) {
        error_at(state, where, "recursive reference to '%s'", name);
        return;
    }
    if (later == NO_DECLARATION || later < state->current) {
        error_at(state, where, "undefined name '%s'", name);
        return;
    }

    chain = (size_t *)malloc(state->declaration_count * sizeof(*chain));
    if (!chain) {
        out_of_memory(state);
        return;
    }
    length = reference_chain(state, later, state->current, chain);
    if (length) {
        char *cycle = NULL;
        size_t cycle_length = 0;
        size_t cycle_capacity = 0;
        size_t i;
        int ok = append_text(&cycle, &cycle_length, &cycle_capacity,
                             current->name, strlen(current->name));
        for (i = 0; ok && i < length; i++) {
            const char *step = state->declarations[chain[i]].name;
            ok = append_text(&cycle, &cycle_length, &cycle_capacity, " -> ", 4) &&
                 append_text(&cycle, &cycle_length, &cycle_capacity,
                             step, strlen(step));
        }
        if (ok) {
            error_at(state, where, "recursive reference to '%s' (%s)",
                     name, cycle);
        } else {
            out_of_memory(state);
        }
        free(cycle);
    } else {
        unsigned line;
        unsigned column;
        source_position(state->source, state->declarations[later].name_at,
                        &line, &column);
        error_at(state, where, "use of '%s' before its definition at %u:%u",
                 name, line, column);
    }
    free(chain);
}

/* Compiles one expression to regex text. `origin` is where `expression`
 * starts in the source, for diagnostics. */
static char *compile_expression(compile_state_t *state, const char *expression,
                                const char *origin) {
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
            if (!*end) {
                error_at(state, source_at(origin, expression, cursor),
                         "unterminated raw string");
                goto fail;
            }
            if (!append_text(&result, &length, &capacity,
                             start, (size_t)(end - start))) goto oom;
            cursor = end + 1;
            continue;
        }
        if (*cursor == '/') {
            const char *literal = cursor;
            const char *start = ++cursor;
            while (*cursor && *cursor != '/') {
                if (*cursor == '\\' && cursor[1]) cursor++;
                cursor++;
            }
            if (!*cursor) {
                error_at(state, source_at(origin, expression, literal),
                         "unterminated regular expression");
                goto fail;
            }
            if (!append_text(&result, &length, &capacity,
                             start, (size_t)(cursor - start))) goto oom;
            cursor++;
            continue;
        }
        if (*cursor == '"') {
            const char *literal = cursor;
            const char *start = ++cursor;
            while (*cursor && *cursor != '"') {
                if (*cursor == '\\' && cursor[1]) cursor++;
                cursor++;
            }
            if (!*cursor) {
                error_at(state, source_at(origin, expression, literal),
                         "unterminated string");
                goto fail;
            }
            if (!append_literal(&result, &length, &capacity,
                                start, (size_t)(cursor - start))) goto oom;
            cursor++;
            continue;
        }
        if (isalpha((unsigned char)*cursor) || *cursor == '_') {
            const char *start = cursor++;
            char *name;
            language_entry_t *entry;
            while (isalnum((unsigned char)*cursor) || *cursor == '_') cursor++;
            name = lang_strndup(start, (size_t)(cursor - start));
            if (!name) goto oom;
            entry = find_entry(state->program, name);
            if (!entry) entry = find_binding(state->ctx, name);
            if (!entry) {
                report_unresolved(state, name,
                                  source_at(origin, expression, start));
                free(name);
                goto fail;
            }
            free(name);
            if (!append_text(&result, &length, &capacity, "(", 1) ||
                !append_text(&result, &length, &capacity,
                             entry->value, strlen(entry->value)) ||
                !append_text(&result, &length, &capacity, ")", 1)) goto oom;
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
                                     (size_t)(end - cursor + 1))) goto oom;
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
        if (!append_text(&result, &length, &capacity, cursor, 1)) goto oom;
        cursor++;
    }
    if (!result) result = lang_strdup("");
    if (!result) out_of_memory(state);
    return result;

oom:
    out_of_memory(state);
fail:
    free(result);
    return NULL;
}

/* Takes ownership of `value`, also on failure. */
static int program_add(RLangProgram *program, const char *name,
                       char *value, int pattern) {
    language_entry_t *entries = (language_entry_t *)realloc(
        program->entries, (program->entry_count + 1) * sizeof(*entries));
    if (!entries) {
        free(value);
        return 0;
    }
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

static char *compile_block(compile_state_t *state, const char *expression,
                           const char *origin) {
    const char *open = skip_space(expression);
    const char *close;
    const char *body_origin;
    const char *last = NULL;
    char *body;
    char *line;
    char *compiled;
    if (*open != '{') return compile_expression(state, expression, origin);
    close = strrchr(open, '}');
    if (!close) {
        error_at(state, source_at(origin, expression, open),
                 "unterminated block");
        return NULL;
    }
    body = lang_strndup(open + 1, (size_t)(close - open - 1));
    if (!body) {
        out_of_memory(state);
        return NULL;
    }
    body_origin = source_at(origin, expression, open + 1);

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
                if (!name) {
                    out_of_memory(state);
                    free(body);
                    return NULL;
                }
                value = compile_expression(state, equals + 1,
                                           source_at(body_origin, body, equals + 1));
                if (!value || !program_add(state->program, name, value, 0)) {
                    if (value) out_of_memory(state);
                    free(name);
                    free(body);
                    return NULL;
                }
                free(name);
            }
        } else if (*cursor) {
            last = cursor;
        }
        line = strtok(NULL, "\n;");
    }
    if (!last) {
        error_at(state, source_at(origin, expression, open),
                 "block has no result expression");
        free(body);
        return NULL;
    }
    compiled = compile_expression(state, last, source_at(body_origin, body, last));
    free(body);
    return compiled;
}

/* Skips a top-level construct that is not `pattern` or `let`: up to the end
 * of its line or `;`, including any `{ ... }` body it opens. Returns NULL
 * after reporting an error. */
static const char *skip_unsupported(compile_state_t *state, const char *start) {
    const char *cursor = start;
    const char *word_end = start;
    int depth = 0;
    int word_length;
    while (isalnum((unsigned char)*word_end) || *word_end == '_') word_end++;
    if (word_end == start) word_end = start + 1;
    word_length = (int)(word_end - start);
    if (!state->ctx->allow_unknown) {
        error_at(state, start, "unsupported construct '%.*s'", word_length, start);
        return NULL;
    }
    warning_at(state, start, "unsupported construct '%.*s' ignored",
               word_length, start);
    while (*cursor) {
        if ((cursor[0] == '/' && cursor[1] == '/') || cursor[0] == '#') {
            while (*cursor && *cursor != '\n') cursor++;
            continue;
        }
        if (*cursor == '"' || *cursor == '\'') {
            char quote = *cursor++;
            while (*cursor && *cursor != quote && *cursor != '\n') {
                if (*cursor == '\\' && cursor[1] && cursor[1] != '\n') cursor++;
                cursor++;
            }
            if (*cursor == quote) cursor++;
            continue;
        }
        if (*cursor == '{') {
            depth++;
        } else if (*cursor == '}') {
            if (depth > 0) depth--;
        } else if (depth == 0 && (*cursor == '\n' || *cursor == ';')) {
            return cursor + 1;
        }
        cursor++;
    }
    if (depth > 0) {
        error_at(state, start, "unterminated '{' in unsupported construct '%.*s'",
                 word_length, start);
        return NULL;
    }
    return cursor;
}

static void add_declaration(compile_state_t *state, const char *name_at,
                            const char *name_end, const char *expression_at,
                            const char *expression_end, int pattern) {
    declaration_t *declarations = (declaration_t *)realloc(
        state->declarations,
        (state->declaration_count + 1) * sizeof(*declarations));
    declaration_t *declaration;
    if (!declarations) {
        out_of_memory(state);
        return;
    }
    state->declarations = declarations;
    declaration = &declarations[state->declaration_count];
    declaration->name = lang_strndup(name_at, (size_t)(name_end - name_at));
    declaration->expression = lang_strndup(
        expression_at, (size_t)(expression_end - expression_at));
    declaration->name_at = name_at;
    declaration->expression_at = expression_at;
    declaration->pattern = pattern;
    if (!declaration->name || !declaration->expression) {
        free(declaration->name);
        free(declaration->expression);
        out_of_memory(state);
        return;
    }
    state->declaration_count++;
}

/* First pass: find every top-level declaration, so that the second pass can
 * tell an undefined name from a forward or recursive reference. */
static void scan_declarations(compile_state_t *state) {
    const char *cursor = state->source;
    while (!state->failed && *(cursor = skip_space(cursor))) {
        int pattern = 0;
        const char *name_at;
        const char *name_end;
        const char *expression_at;
        size_t name_length;
        size_t i;
        int depth = 0;

        if (strncmp(cursor, "pattern", 7) == 0 &&
            isspace((unsigned char)cursor[7])) {
            pattern = 1;
            cursor += 7;
        } else if (strncmp(cursor, "let", 3) == 0 &&
                   isspace((unsigned char)cursor[3])) {
            cursor += 3;
        } else {
            cursor = skip_unsupported(state, cursor);
            continue;
        }
        cursor = skip_space(cursor);
        name_at = cursor;
        while (isalnum((unsigned char)*cursor) || *cursor == '_') cursor++;
        name_end = cursor;
        name_length = (size_t)(name_end - name_at);
        cursor = skip_space(cursor);
        if (*cursor != '=') {
            error_at(state, cursor, "expected '=' after declaration");
            return;
        }
        for (i = 0; i < state->declaration_count; i++) {
            const declaration_t *first = &state->declarations[i];
            if (strlen(first->name) == name_length &&
                strncmp(first->name, name_at, name_length) == 0) {
                unsigned line;
                unsigned column;
                source_position(state->source, first->name_at, &line, &column);
                error_at(state, name_at,
                         "duplicate definition of '%s' (first defined at %u:%u)",
                         first->name, line, column);
                return;
            }
        }
        cursor = skip_space(cursor + 1);
        expression_at = cursor;
        if (*cursor == '{') {
            do {
                if (*cursor == '{') depth++;
                if (*cursor == '}') depth--;
                cursor++;
            } while (*cursor && depth > 0);
        } else {
            while (*cursor && *cursor != '\n' && *cursor != ';') cursor++;
        }
        add_declaration(state, name_at, name_end, expression_at, cursor, pattern);
        if (*cursor == ';') cursor++;
    }
}

/* Second pass: declarations compile in source order, and a name refers only
 * to a declaration before it (or to a context binding). */
static void compile_declaration(compile_state_t *state, size_t index) {
    const declaration_t *declaration = &state->declarations[index];
    char *compiled;
    state->current = index;
    compiled = compile_block(state, declaration->expression,
                             declaration->expression_at);
    if (!compiled) {
        error_at(state, declaration->expression_at, "invalid pattern expression");
        return;
    }
    if (!program_add(state->program, declaration->name, compiled,
                     declaration->pattern)) {
        out_of_memory(state);
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

void rlang_context_set_allow_unknown(RLangContext *ctx, bool allow) {
    if (ctx) ctx->allow_unknown = allow;
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

static RLangCompileResult compile_source(RLangContext *ctx, const char *source,
                                         const char *path) {
    RLangCompileResult result;
    compile_state_t state;
    char *scan_error;
    int scan_failed;
    size_t i;
    memset(&result, 0, sizeof(result));
    if (!ctx || !source) {
        result.error_message = lang_strdup("context and source are required");
        return result;
    }
    memset(&state, 0, sizeof(state));
    state.ctx = ctx;
    state.source = source;
    state.path = path;
    state.program = (RLangProgram *)calloc(1, sizeof(*state.program));
    if (!state.program) {
        result.error_message = lang_strdup("out of memory");
        return result;
    }

    scan_declarations(&state);
    /* Compile the declarations scanned before any scan error: an error in
     * one of them comes earlier in the source, so it is reported instead. */
    scan_failed = state.failed;
    scan_error = state.error;
    state.failed = 0;
    state.error = NULL;
    for (i = 0; i < state.declaration_count && !state.failed; i++) {
        compile_declaration(&state, i);
    }
    if (state.failed) {
        free(scan_error);
    } else if (scan_failed) {
        state.failed = 1;
        state.error = scan_error ? scan_error : lang_strdup("out of memory");
    } else if (!state.program->pattern_count) {
        error_at(&state, NULL, "no pattern declarations found");
    }

    for (i = 0; i < state.declaration_count; i++) {
        free(state.declarations[i].name);
        free(state.declarations[i].expression);
    }
    free(state.declarations);
    result.warnings = state.warnings;
    if (state.failed) {
        result.error_message = state.error ? state.error : lang_strdup("out of memory");
        rlang_program_free(state.program);
        return result;
    }
    result.success = true;
    result.program = state.program;
    return result;
}

RLangCompileResult rlang_compile(RLangContext *ctx, const char *source) {
    return compile_source(ctx, source, NULL);
}

RLangCompileResult rlang_load_file(RLangContext *ctx, const char *filepath) {
    RLangCompileResult result;
    FILE *stream;
    long size;
    char *source;
    memset(&result, 0, sizeof(result));
    if (!filepath) {
        result.error_message = lang_strdup("unable to open RiftLang source");
        return result;
    }
    if (!(stream = fopen(filepath, "rb"))) {
        result.error_message = format_text(
            "%s: error: unable to open RiftLang source", filepath);
        return result;
    }
    if (fseek(stream, 0, SEEK_END) != 0 ||
        (size = ftell(stream)) < 0 ||
        fseek(stream, 0, SEEK_SET) != 0) {
        fclose(stream);
        result.error_message = format_text(
            "%s: error: unable to read RiftLang source", filepath);
        return result;
    }
    source = (char *)malloc((size_t)size + 1);
    if (!source || fread(source, 1, (size_t)size, stream) != (size_t)size) {
        fclose(stream);
        free(source);
        result.error_message = format_text(
            "%s: error: unable to read RiftLang source", filepath);
        return result;
    }
    fclose(stream);
    source[size] = '\0';
    result = compile_source(ctx, source, filepath);
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
    free(result->warnings);
    result->warnings = NULL;
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
