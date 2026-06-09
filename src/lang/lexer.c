#include "rift/lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct rift_lexer {
    const char *source;
    size_t      length;
    size_t      pos;
    uint32_t    line;
    uint32_t    column;
};

rift_lexer_t *rift_lexer_create(const char *source, size_t length) {
    if (!source) return NULL;

    rift_lexer_t *lexer = (rift_lexer_t *)calloc(1, sizeof(rift_lexer_t));
    if (!lexer) return NULL;

    lexer->source = source;
    lexer->length = length;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->column = 1;

    return lexer;
}

void rift_lexer_destroy(rift_lexer_t *lexer) {
    free(lexer);
}

static char lexer_current(const rift_lexer_t *lexer) {
    if (lexer->pos >= lexer->length) return '\0';
    return lexer->source[lexer->pos];
}

static char lexer_advance(rift_lexer_t *lexer) {
    char c = lexer_current(lexer);
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    lexer->pos++;
    return c;
}

rift_token_t rift_lexer_next(rift_lexer_t *lexer) {
    if (!lexer || lexer->pos >= lexer->length) {
        return rift_token_create(RIFT_TOKEN_EOF, NULL);
    }

    /* Skip whitespace */
    while (lexer->pos < lexer->length && isspace(lexer_current(lexer))) {
        lexer_advance(lexer);
    }

    if (lexer->pos >= lexer->length) {
        return rift_token_create(RIFT_TOKEN_EOF, NULL);
    }

    uint32_t start_line = lexer->line;
    uint32_t start_col = lexer->column;
    char c = lexer_current(lexer);

    /* R"" static pattern or R'' dynamic pattern */
    if (c == 'R' && lexer->pos + 1 < lexer->length) {
        char next = lexer->source[lexer->pos + 1];
        if (next == '"' || next == '\'') {
            char delim = next;
            rift_token_type_t type = (delim == '"')
                ? RIFT_TOKEN_PATTERN_STATIC
                : RIFT_TOKEN_PATTERN_DYNAMIC;

            lexer_advance(lexer); /* skip R */
            lexer_advance(lexer); /* skip opening quote */
            if (lexer->pos < lexer->length && lexer_current(lexer) == delim) {
                lexer_advance(lexer); /* skip second quote for R"" or R'' */
            }

            /* Read until closing delimiter pair */
            size_t start = lexer->pos;
            while (lexer->pos < lexer->length) {
                if (lexer_current(lexer) == delim) {
                    if (lexer->pos + 1 < lexer->length &&
                        lexer->source[lexer->pos + 1] == delim) {
                        break;
                    }
                    break;
                }
                lexer_advance(lexer);
            }

            size_t len = lexer->pos - start;
            char *buf = (char *)malloc(len + 1);
            if (buf) {
                memcpy(buf, lexer->source + start, len);
                buf[len] = '\0';
            }

            /* Skip closing delimiters */
            if (lexer->pos < lexer->length) lexer_advance(lexer);
            if (lexer->pos < lexer->length && lexer_current(lexer) == delim) {
                lexer_advance(lexer);
            }

            rift_token_t token = rift_token_create(type, buf);
            token.row = start_line;
            token.column = start_col;
            free(buf);
            return token;
        }
    }

    /* Identifier or keyword */
    if (isalpha(c) || c == '_') {
        size_t start = lexer->pos;
        while (lexer->pos < lexer->length &&
               (isalnum(lexer_current(lexer)) || lexer_current(lexer) == '_')) {
            lexer_advance(lexer);
        }
        size_t len = lexer->pos - start;
        char *buf = (char *)malloc(len + 1);
        if (buf) {
            memcpy(buf, lexer->source + start, len);
            buf[len] = '\0';
        }

        /* Check keywords */
        rift_token_type_t type = RIFT_TOKEN_IDENTIFIER;
        const char *keywords[] = {
            "let", "fn", "if", "else", "while", "for", "return",
            "break", "continue", "def", "print", "true", "false", NULL
        };
        for (int i = 0; keywords[i]; i++) {
            if (strcmp(buf, keywords[i]) == 0) {
                type = RIFT_TOKEN_KEYWORD;
                break;
            }
        }

        rift_token_t token = rift_token_create(type, buf);
        token.row = start_line;
        token.column = start_col;
        free(buf);
        return token;
    }

    /* Number */
    if (isdigit(c)) {
        size_t start = lexer->pos;
        int is_float = 0;
        while (lexer->pos < lexer->length &&
               (isdigit(lexer_current(lexer)) || lexer_current(lexer) == '.')) {
            if (lexer_current(lexer) == '.') is_float = 1;
            lexer_advance(lexer);
        }
        size_t len = lexer->pos - start;
        char *buf = (char *)malloc(len + 1);
        if (buf) {
            memcpy(buf, lexer->source + start, len);
            buf[len] = '\0';
        }
        rift_token_type_t type = is_float ? RIFT_TOKEN_LITERAL_FLOAT : RIFT_TOKEN_LITERAL_INT;
        rift_token_t token = rift_token_create(type, buf);
        token.row = start_line;
        token.column = start_col;
        free(buf);
        return token;
    }

    /* String literal */
    if (c == '"') {
        lexer_advance(lexer);
        size_t start = lexer->pos;
        while (lexer->pos < lexer->length && lexer_current(lexer) != '"') {
            if (lexer_current(lexer) == '\\') lexer_advance(lexer);
            lexer_advance(lexer);
        }
        size_t len = lexer->pos - start;
        char *buf = (char *)malloc(len + 1);
        if (buf) {
            memcpy(buf, lexer->source + start, len);
            buf[len] = '\0';
        }
        if (lexer->pos < lexer->length) lexer_advance(lexer);
        rift_token_t token = rift_token_create(RIFT_TOKEN_LITERAL_STRING, buf);
        token.row = start_line;
        token.column = start_col;
        free(buf);
        return token;
    }

    /* Operators and delimiters */
    lexer_advance(lexer);
    char buf[2] = { c, '\0' };

    rift_token_type_t type = RIFT_TOKEN_UNKNOWN;
    if (strchr("+-*/%=<>!&|^~", c)) {
        type = RIFT_TOKEN_OPERATOR;
    } else if (strchr("(){}[];,.", c)) {
        type = RIFT_TOKEN_DELIMITER;
    }

    rift_token_t token = rift_token_create(type, buf);
    token.row = start_line;
    token.column = start_col;
    return token;
}

rift_token_t rift_lexer_peek(const rift_lexer_t *lexer) {
    rift_lexer_t copy = *lexer;
    return rift_lexer_next(&copy);
}

int rift_lexer_eof(const rift_lexer_t *lexer) {
    return !lexer || lexer->pos >= lexer->length;
}
