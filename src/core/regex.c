#include "rift/regex.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

typedef enum regex_node_type {
    RX_EMPTY,
    RX_LITERAL,
    RX_DOT,
    RX_CLASS,
    RX_CONCAT,
    RX_ALT,
    RX_REPEAT,
    RX_ANCHOR_START,
    RX_ANCHOR_END,
    RX_BOUNDARY
} regex_node_type_t;

typedef struct regex_node {
    regex_node_type_t type;
    struct regex_node *left;
    struct regex_node *right;
    unsigned char literal;
    unsigned char bitmap[32];
    int negate;
    int min_repeat;
    int max_repeat;
} regex_node_t;

typedef struct regex_parser {
    const char *pattern;
    size_t length;
    size_t position;
    char error[128];
    int error_offset;
} regex_parser_t;

static regex_node_t *node_create(regex_node_type_t type) {
    regex_node_t *node = (regex_node_t *)calloc(1, sizeof(*node));
    if (node) {
        node->type = type;
        node->max_repeat = 1;
    }
    return node;
}

static void node_destroy(regex_node_t *node) {
    if (!node) return;
    node_destroy(node->left);
    node_destroy(node->right);
    free(node);
}

static void parser_error(regex_parser_t *parser, const char *message) {
    if (parser->error[0]) return;
    parser->error_offset = (int)parser->position;
    strncpy(parser->error, message, sizeof(parser->error) - 1);
}

static int parser_peek(const regex_parser_t *parser) {
    if (parser->position >= parser->length) return -1;
    return (unsigned char)parser->pattern[parser->position];
}

static int parser_take(regex_parser_t *parser) {
    int value = parser_peek(parser);
    if (value >= 0) parser->position++;
    return value;
}

static void bitmap_set(unsigned char *bitmap, unsigned char value) {
    bitmap[value >> 3] |= (unsigned char)(1u << (value & 7u));
}

static int bitmap_get(const unsigned char *bitmap, unsigned char value) {
    return (bitmap[value >> 3] & (unsigned char)(1u << (value & 7u))) != 0;
}

static int escaped_character(regex_parser_t *parser, regex_node_t *node) {
    int c = parser_take(parser);
    if (c < 0) {
        parser_error(parser, "trailing escape");
        return 0;
    }

    if (c == 'b') {
        node->type = RX_BOUNDARY;
        return 1;
    }

    if (c == 'd' || c == 'w' || c == 's') {
        int i;
        node->type = RX_CLASS;
        for (i = 0; i < 256; i++) {
            int matches = 0;
            if (c == 'd') matches = isdigit(i);
            if (c == 'w') matches = isalnum(i) || i == '_';
            if (c == 's') matches = isspace(i);
            if (matches) bitmap_set(node->bitmap, (unsigned char)i);
        }
        return 1;
    }

    switch (c) {
        case 'n': c = '\n'; break;
        case 'r': c = '\r'; break;
        case 't': c = '\t'; break;
        default: break;
    }
    node->type = RX_LITERAL;
    node->literal = (unsigned char)c;
    return 1;
}

static int class_character(regex_parser_t *parser, int *is_class,
                           unsigned char *class_bitmap) {
    int c = parser_take(parser);
    int i;
    *is_class = 0;
    if (c < 0) return -1;
    if (c != '\\') return c;

    c = parser_take(parser);
    if (c < 0) return -1;
    if (c == 'd' || c == 'w' || c == 's') {
        *is_class = 1;
        for (i = 0; i < 256; i++) {
            int matches = c == 'd' ? isdigit(i)
                : c == 'w' ? (isalnum(i) || i == '_')
                : isspace(i);
            if (matches) bitmap_set(class_bitmap, (unsigned char)i);
        }
        return 0;
    }
    if (c == 'n') return '\n';
    if (c == 'r') return '\r';
    if (c == 't') return '\t';
    return c;
}

static regex_node_t *parse_expression(regex_parser_t *parser);

static regex_node_t *parse_class(regex_parser_t *parser) {
    regex_node_t *node = node_create(RX_CLASS);
    int first = 1;
    if (!node) return NULL;

    if (parser_peek(parser) == '^') {
        parser_take(parser);
        node->negate = 1;
    }

    while (parser_peek(parser) >= 0 && parser_peek(parser) != ']') {
        unsigned char extra[32] = {0};
        int is_class = 0;
        int start = class_character(parser, &is_class, extra);
        int i;
        if (start < 0) {
            parser_error(parser, "unterminated character class");
            node_destroy(node);
            return NULL;
        }
        if (is_class) {
            for (i = 0; i < 32; i++) node->bitmap[i] |= extra[i];
            first = 0;
            continue;
        }

        if (!first && start == ']' ) break;
        first = 0;

        if (parser_peek(parser) == '-' &&
            parser->position + 1 < parser->length &&
            parser->pattern[parser->position + 1] != ']') {
            int end_is_class = 0;
            unsigned char ignored[32] = {0};
            int end;
            parser_take(parser);
            end = class_character(parser, &end_is_class, ignored);
            if (end < start || end_is_class) {
                parser_error(parser, "invalid character range");
                node_destroy(node);
                return NULL;
            }
            for (i = start; i <= end; i++) bitmap_set(node->bitmap, (unsigned char)i);
        } else {
            bitmap_set(node->bitmap, (unsigned char)start);
        }
    }

    if (parser_take(parser) != ']') {
        parser_error(parser, "unterminated character class");
        node_destroy(node);
        return NULL;
    }
    return node;
}

static regex_node_t *parse_atom(regex_parser_t *parser) {
    int c = parser_take(parser);
    regex_node_t *node;

    if (c < 0 || c == '|' || c == ')') {
        if (c >= 0) parser->position--;
        return node_create(RX_EMPTY);
    }
    if (c == '(') {
        node = parse_expression(parser);
        if (!node) return NULL;
        if (parser_take(parser) != ')') {
            parser_error(parser, "unclosed group");
            node_destroy(node);
            return NULL;
        }
        return node;
    }
    if (c == '[') return parse_class(parser);
    if (c == '.') return node_create(RX_DOT);
    if (c == '^') return node_create(RX_ANCHOR_START);
    if (c == '$') return node_create(RX_ANCHOR_END);

    node = node_create(RX_LITERAL);
    if (!node) return NULL;
    if (c == '\\') {
        if (!escaped_character(parser, node)) {
            node_destroy(node);
            return NULL;
        }
    } else {
        node->literal = (unsigned char)c;
    }
    return node;
}

static int parse_number(regex_parser_t *parser, int *value) {
    int found = 0;
    int result = 0;
    while (isdigit(parser_peek(parser))) {
        int digit = parser_take(parser) - '0';
        if (result > (INT_MAX - digit) / 10) {
            parser_error(parser, "repeat count is too large");
            return 0;
        }
        result = result * 10 + digit;
        found = 1;
    }
    *value = result;
    return found;
}

static regex_node_t *parse_repeat(regex_parser_t *parser) {
    regex_node_t *atom = parse_atom(parser);
    regex_node_t *repeat;
    int c;
    int min = 0;
    int max = -1;
    if (!atom) return NULL;

    c = parser_peek(parser);
    if (c == '*') {
        parser_take(parser);
    } else if (c == '+') {
        parser_take(parser);
        min = 1;
    } else if (c == '?') {
        parser_take(parser);
        max = 1;
    } else if (c == '{') {
        parser_take(parser);
        if (!parse_number(parser, &min)) {
            parser_error(parser, "repeat count expected");
            node_destroy(atom);
            return NULL;
        }
        max = min;
        if (parser_peek(parser) == ',') {
            parser_take(parser);
            if (!parse_number(parser, &max)) max = -1;
        }
        if (parser_take(parser) != '}' || (max >= 0 && max < min)) {
            parser_error(parser, "invalid repeat range");
            node_destroy(atom);
            return NULL;
        }
    } else {
        return atom;
    }

    repeat = node_create(RX_REPEAT);
    if (!repeat) {
        node_destroy(atom);
        return NULL;
    }
    repeat->left = atom;
    repeat->min_repeat = min;
    repeat->max_repeat = max;
    return repeat;
}

static regex_node_t *parse_concat(regex_parser_t *parser) {
    regex_node_t *result = NULL;
    while (parser_peek(parser) >= 0 &&
           parser_peek(parser) != '|' &&
           parser_peek(parser) != ')') {
        regex_node_t *next = parse_repeat(parser);
        regex_node_t *concat;
        if (!next) {
            node_destroy(result);
            return NULL;
        }
        if (!result) {
            result = next;
            continue;
        }
        concat = node_create(RX_CONCAT);
        if (!concat) {
            node_destroy(result);
            node_destroy(next);
            return NULL;
        }
        concat->left = result;
        concat->right = next;
        result = concat;
    }
    return result ? result : node_create(RX_EMPTY);
}

static regex_node_t *parse_expression(regex_parser_t *parser) {
    regex_node_t *left = parse_concat(parser);
    if (!left) return NULL;
    while (parser_peek(parser) == '|') {
        regex_node_t *right;
        regex_node_t *alt;
        parser_take(parser);
        right = parse_concat(parser);
        if (!right) {
            node_destroy(left);
            return NULL;
        }
        alt = node_create(RX_ALT);
        if (!alt) {
            node_destroy(left);
            node_destroy(right);
            return NULL;
        }
        alt->left = left;
        alt->right = right;
        left = alt;
    }
    return left;
}

static int char_equal(unsigned char left, unsigned char right, uint32_t flags) {
    if (flags & RIFT_REGEX_CASE_INSENSITIVE) {
        return tolower(left) == tolower(right);
    }
    return left == right;
}

static int is_word(unsigned char value) {
    return isalnum(value) || value == '_';
}

static int set_changed(const unsigned char *left, const unsigned char *right,
                       size_t count) {
    return memcmp(left, right, count) != 0;
}

static int match_node(const regex_node_t *node, const char *text, size_t length,
                      size_t start, unsigned char *out, uint32_t flags);

static int match_repeat(const regex_node_t *node, const char *text, size_t length,
                        size_t start, unsigned char *out, uint32_t flags) {
    unsigned char *current = (unsigned char *)calloc(length + 1, 1);
    unsigned char *next = (unsigned char *)calloc(length + 1, 1);
    unsigned char *single = (unsigned char *)calloc(length + 1, 1);
    int count = 0;
    int limit = node->max_repeat >= 0
        ? node->max_repeat
        : (int)length + node->min_repeat + 1;
    size_t i;

    if (!current || !next || !single) {
        free(current);
        free(next);
        free(single);
        return 0;
    }
    current[start] = 1;

    for (;;) {
        if (count >= node->min_repeat) {
            for (i = 0; i <= length; i++) {
                if (current[i]) out[i] = 1;
            }
        }
        if (count >= limit) break;

        memset(next, 0, length + 1);
        for (i = 0; i <= length; i++) {
            size_t j;
            if (!current[i]) continue;
            memset(single, 0, length + 1);
            if (!match_node(node->left, text, length, i, single, flags)) continue;
            for (j = 0; j <= length; j++) {
                if (single[j]) next[j] = 1;
            }
        }
        if (!set_changed(current, next, length + 1)) break;
        memcpy(current, next, length + 1);
        count++;
    }

    free(current);
    free(next);
    free(single);
    return 1;
}

static int match_node(const regex_node_t *node, const char *text, size_t length,
                      size_t start, unsigned char *out, uint32_t flags) {
    size_t i;
    if (!node || start > length) return 0;

    switch (node->type) {
        case RX_EMPTY:
            out[start] = 1;
            return 1;
        case RX_LITERAL:
            if (start < length &&
                char_equal((unsigned char)text[start], node->literal, flags)) {
                out[start + 1] = 1;
            }
            return 1;
        case RX_DOT:
            if (start < length && text[start] != '\n') out[start + 1] = 1;
            return 1;
        case RX_CLASS:
            if (start < length) {
                int matches = bitmap_get(node->bitmap, (unsigned char)text[start]);
                if (node->negate) matches = !matches;
                if (matches) out[start + 1] = 1;
            }
            return 1;
        case RX_ANCHOR_START:
            if (start == 0 ||
                ((flags & RIFT_REGEX_MULTILINE) && text[start - 1] == '\n')) {
                out[start] = 1;
            }
            return 1;
        case RX_ANCHOR_END:
            if (start == length ||
                ((flags & RIFT_REGEX_MULTILINE) && text[start] == '\n')) {
                out[start] = 1;
            }
            return 1;
        case RX_BOUNDARY: {
            int before = start > 0 && is_word((unsigned char)text[start - 1]);
            int after = start < length && is_word((unsigned char)text[start]);
            if (before != after) out[start] = 1;
            return 1;
        }
        case RX_ALT:
            match_node(node->left, text, length, start, out, flags);
            match_node(node->right, text, length, start, out, flags);
            return 1;
        case RX_CONCAT: {
            unsigned char *middle = (unsigned char *)calloc(length + 1, 1);
            unsigned char *end = (unsigned char *)calloc(length + 1, 1);
            if (!middle || !end) {
                free(middle);
                free(end);
                return 0;
            }
            match_node(node->left, text, length, start, middle, flags);
            for (i = 0; i <= length; i++) {
                size_t j;
                if (!middle[i]) continue;
                memset(end, 0, length + 1);
                match_node(node->right, text, length, i, end, flags);
                for (j = 0; j <= length; j++) {
                    if (end[j]) out[j] = 1;
                }
            }
            free(middle);
            free(end);
            return 1;
        }
        case RX_REPEAT:
            return match_repeat(node, text, length, start, out, flags);
    }
    return 0;
}

int rift_regex_compile_ex(RIFTRegex *regex, const char *pattern, uint32_t flags) {
    regex_parser_t parser;
    regex_node_t *root;
    size_t length;
    if (!regex || !pattern) return -1;

    memset(regex, 0, sizeof(*regex));
    memset(&parser, 0, sizeof(parser));
    length = strlen(pattern);
    parser.pattern = pattern;
    parser.length = length;
    parser.error_offset = -1;

    root = parse_expression(&parser);
    if (!root || parser.error[0] || parser.position != length) {
        if (!parser.error[0]) parser_error(&parser, "unexpected token");
        regex->error_offset = parser.error_offset;
        strncpy(regex->error_message, parser.error, sizeof(regex->error_message) - 1);
        node_destroy(root);
        return -1;
    }

    regex->pattern = (char *)malloc(length + 1);
    if (!regex->pattern) {
        node_destroy(root);
        strncpy(regex->error_message, "out of memory", sizeof(regex->error_message) - 1);
        return -1;
    }
    memcpy(regex->pattern, pattern, length + 1);
    regex->implementation = root;
    regex->flags = flags;
    regex->error_offset = -1;
    return 0;
}

int rift_regex_compile(RIFTRegex *regex, const char *pattern) {
    return rift_regex_compile_ex(regex, pattern, RIFT_REGEX_NONE);
}

int rift_regex_match_n(const RIFTRegex *regex, const char *text, size_t length) {
    size_t start;
    unsigned char *ends;
    if (!regex || !regex->implementation || !text) return -1;

    ends = (unsigned char *)calloc(length + 1, 1);
    if (!ends) return -1;
    for (start = 0; start <= length; start++) {
        size_t i;
        memset(ends, 0, length + 1);
        match_node((const regex_node_t *)regex->implementation, text, length,
                   start, ends, regex->flags);
        for (i = 0; i <= length; i++) {
            if (ends[i]) {
                free(ends);
                return 0;
            }
        }
    }
    free(ends);
    return 1;
}

int rift_regex_match(const RIFTRegex *regex, const char *text) {
    if (!text) return -1;
    return rift_regex_match_n(regex, text, strlen(text));
}

int rift_regex_is_match(const char *pattern, const char *text) {
    RIFTRegex regex;
    int result;
    if (rift_regex_compile(&regex, pattern) != 0) return 0;
    result = rift_regex_match(&regex, text) == 0;
    rift_regex_free(&regex);
    return result;
}

const char *rift_regex_error(const RIFTRegex *regex) {
    if (!regex || !regex->error_message[0]) return "";
    return regex->error_message;
}

void rift_regex_free(RIFTRegex *regex) {
    if (!regex) return;
    node_destroy((regex_node_t *)regex->implementation);
    free(regex->pattern);
    memset(regex, 0, sizeof(*regex));
}
