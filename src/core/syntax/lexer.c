/**
 * @file lexer.c
 * @brief Implementation of the regex syntax lexer component of LibRift
 *
 * This file implements the tokenization for regular expression patterns
 * with support for the extended `r''` syntax in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */
#define _POSIX_C_SOURCE 200809L

#include "core/syntax/lexer.h
#include "librift/syntax/lexer.h"
/**
tmp/librift_analysis/librift-4933472-W/backup/v1/src/syntax/lexer.h
tmp/librift_analysis/librift-4933472-W/backup/v2/syntax/lexer.h
tmp/librift_analysis/librift-4933472-W/src/core/syntax/lexer.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/syntax/lexer.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v2/syntax/lexer.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/syntax/lexer.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/syntax/lexer.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v2/syntax/lexer.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/syntax/lexer.h"
 * @brief Scan a token in standard regex syntax mode
 *
 * @param lexer The syntax lexer
 * @return The scanned token
 */
static rift_regex_token_t
scan_standard_syntax_token(rift_regex_syntax_lexer_t *lexer)
{
    char c = peek(lexer);
    size_t start_pos = lexer->position;

    /* Check for end of input */
    if (c == '\0') {
        return create_token(RIFT_REGEX_TOKEN_END, NULL, start_pos);
    }

    /* Handle special characters */
    lexer_advance(lexer);

    switch (c) {
    case '.':
        return create_char_token(RIFT_REGEX_TOKEN_DOT, c, start_pos);
    case '^':
        return create_char_token(RIFT_REGEX_TOKEN_CARET, c, start_pos);
    case '$':
        return create_char_token(RIFT_REGEX_TOKEN_DOLLAR, c, start_pos);
    case '*':
        return create_char_token(RIFT_REGEX_TOKEN_STAR, c, start_pos);
    case '+':
        return create_char_token(RIFT_REGEX_TOKEN_PLUS, c, start_pos);
    case '?':
        return create_char_token(RIFT_REGEX_TOKEN_QUESTION, c, start_pos);
    case '|':
        return create_char_token(RIFT_REGEX_TOKEN_PIPE, c, start_pos);
    case '(':
        return scan_group(lexer, start_pos);
    case ')':
        return create_char_token(RIFT_REGEX_TOKEN_RPAREN, c, start_pos);
    case '[':
        return scan_character_class(lexer, start_pos);
    case ']':
        return create_char_token(RIFT_REGEX_TOKEN_RBRACKET, c, start_pos);
    case '{':
        return scan_quantifier(lexer, start_pos);
    case '}':
        return create_char_token(RIFT_REGEX_TOKEN_RBRACE, c, start_pos);
    case ',':
        return create_char_token(RIFT_REGEX_TOKEN_COMMA, c, start_pos);
    case '\\':
        return scan_escape_sequence(lexer, start_pos);
    default:
        return create_char_token(RIFT_REGEX_TOKEN_LITERAL, c, start_pos);
    }
}

/**
 * @brief Scan a token in LibRift syntax mode
 *
 * @param lexer The syntax lexer
 * @return The scanned token
 */
static rift_regex_token_t
scan_rift_syntax_token(rift_regex_syntax_lexer_t *lexer)
{
    char c = peek(lexer);
    size_t start_pos = lexer->position;

    /* Check for end of input */
    if (c == '\0') {
        return create_token(RIFT_REGEX_TOKEN_END, NULL, start_pos);
    }

    /* Check for the closing quote character */
    if (c == lexer->quote_char) {
        lexer_advance(lexer);
        return create_token(RIFT_REGEX_TOKEN_RIFT_QUOTE_END,
                            (lexer->quote_char == '\'') ? "'" : "\"", start_pos);
    }

    /* Handle escape sequences */
    if (c == '\\') {
        lexer_advance(lexer);

        /* Handle LibRift special escapes differently */
        char next = peek(lexer);

        /* Check for escaped quote character */
        if (next == lexer->quote_char) {
            lexer_advance(lexer);
            char buffer[3] = {'\\', lexer->quote_char, '\0'};
            return create_token(RIFT_REGEX_TOKEN_RIFT_ESCAPE, buffer, start_pos);
        }

        /* Other escape sequences are handled normally */
        lexer->position--; /* Backtrack to the backslash */
        return scan_escape_sequence(lexer, start_pos);
    }

    /* Handle other tokens same as regular syntax */
    lexer_advance(lexer);

    switch (c) {
    case '.':
        return create_char_token(RIFT_REGEX_TOKEN_DOT, c, start_pos);
    case '^':
        return create_char_token(RIFT_REGEX_TOKEN_CARET, c, start_pos);
    case '$':
        return create_char_token(RIFT_REGEX_TOKEN_DOLLAR, c, start_pos);
    case '*':
        return create_char_token(RIFT_REGEX_TOKEN_STAR, c, start_pos);
    case '+':
        return create_char_token(RIFT_REGEX_TOKEN_PLUS, c, start_pos);
    case '?':
        return create_char_token(RIFT_REGEX_TOKEN_QUESTION, c, start_pos);
    case '|':
        return create_char_token(RIFT_REGEX_TOKEN_PIPE, c, start_pos);
    case '(':
        return scan_group(lexer, start_pos);
    case ')':
        return create_char_token(RIFT_REGEX_TOKEN_RPAREN, c, start_pos);
    case '[':
        return scan_character_class(lexer, start_pos);
    case ']':
        return create_char_token(RIFT_REGEX_TOKEN_RBRACKET, c, start_pos);
    case '{':
        return scan_quantifier(lexer, start_pos);
    case '}':
        return create_char_token(RIFT_REGEX_TOKEN_RBRACE, c, start_pos);
    case ',':
        return create_char_token(RIFT_REGEX_TOKEN_COMMA, c, start_pos);
    default:
        return create_char_token(RIFT_REGEX_TOKEN_LITERAL, c, start_pos);
    }
}

/**
 * @brief Create a token with the given type and value
 *
 * @param type The token type
 * @param value The token value (will be copied)
 * @param position The position of the token in the input
 * @return The created token
 */
static rift_regex_token_t
create_token(rift_regex_token_type_t type, const char *value, size_t position)
{
    rift_regex_token_t token;
    token.type = type;
    token.position = position;

    if (value) {
        token.value = strdup(value);
    } else {
        token.value = NULL;
    }

    return token;
}

/**
 * @brief Create a token with a single character value
 *
 * @param type The token type
 * @param c The character value
 * @param position The position of the token in the input
 * @return The created token
 */
static rift_regex_token_t
create_char_token(rift_regex_token_type_t type, char c, size_t position)
{
    char buffer[2] = {c, '\0'};
    return create_token(type, buffer, position);
}

bool
check_rift_syntax(const char *input)
{
    if (!input || !*input) {
        return false;
    }

    // Check for 'R' prefix followed by a quote
    return input[0] == 'R' && (input[1] == '\'' || input[1] == '"');
}

/**
 * @brief Get the next token from the input
 *
 * @param lexer The syntax lexer
 * @return The next token
 */
rift_regex_token_t
rift_regex_syntax_lexer_next_token(rift_regex_syntax_lexer_t *lexer)
{

    if (!lexer) {
        return create_token(RIFT_REGEX_TOKEN_ERROR, "Null lexer pointer", 0);
    }

    /* Check for LibRift syntax prefix at the beginning */
    if (lexer->position == 0 && check_rift_syntax(lexer->input)) {
        /* Mark that we're using LibRift syntax */
        lexer->is_rift_syntax = true;

        /* Process the 'r' prefix */
        lexer_advance(lexer);
        rift_regex_token_t prefix_token = create_token(RIFT_REGEX_TOKEN_RIFT_PREFIX, "R", 0);

        /* Process the quote character */
        lexer->quote_char = lexer_advance(lexer);
        lexer->current = create_char_token(RIFT_REGEX_TOKEN_RIFT_QUOTE_START, lexer->quote_char, 1);
        lexer->current = prefix_token;
        return prefix_token;
    }

    /* If in LibRift mode and we've seen the opening quote but not the closing one */
    if (lexer->is_rift_syntax && lexer->position > 2) {
        rift_regex_token_t token = scan_rift_syntax_token(lexer);
        lexer->current = token;
        return token;
    }

    /* Standard regex syntax */
    rift_regex_token_t token = scan_standard_syntax_token(lexer);
    lexer->current = token;
    return token;
}

/**
 * @brief Peek at the next token without consuming it
 *
 * @param lexer The syntax lexer
 * @return The next token
 */
rift_regex_token_t
rift_regex_syntax_lexer_peek_token(rift_regex_syntax_lexer_t *lexer)
{
    /* Save current state */
    size_t saved_position = lexer->position;
    rift_regex_token_t saved_current = lexer->current;

    /* Get the next token */
    rift_regex_token_t token = rift_regex_syntax_lexer_next_token(lexer);

    /* Restore state */
    lexer->position = saved_position;
    lexer->current = saved_current;

    return token;
}

/**
 * @brief Create a new syntax lexer
 *
 * @param input The input string to tokenize
 * @return A new syntax lexer or NULL on failure
 */
rift_regex_syntax_lexer_t *
rift_regex_syntax_lexer_create(const char *input)
{
    if (!input) {
        return NULL;
    }

    rift_regex_syntax_lexer_t *lexer = malloc(sizeof(rift_regex_syntax_lexer_t));
    if (!lexer) {
        return NULL;
    }

    lexer->input = input;
    lexer->position = 0;
    lexer->length = strlen(input);
    lexer->is_rift_syntax = false;
    lexer->quote_char = '\0';
    lexer->current.type = RIFT_REGEX_TOKEN_END;
    lexer->current.value = NULL;
    lexer->current.position = 0;
    lexer->error[0] = '\0';

    return lexer;
}

/**
 * @brief Free resources associated with a syntax lexer
 *
 * @param lexer The syntax lexer to free
 */
void
rift_regex_syntax_lexer_free(rift_regex_syntax_lexer_t *lexer)
{
    if (!lexer) {
        return;
    }

    /* Free the current token's value if any */
    if (lexer->current.value) {
        free(lexer->current.value);
    }

    free(lexer);
}

/**
 * @brief Get the current position in the input string
 *
 * @param lexer The syntax lexer
 * @return The current position
 */
size_t
rift_regex_syntax_lexer_get_position(const rift_regex_syntax_lexer_t *lexer)
{
    return lexer ? lexer->position : 0;
}

/**
 * @brief Get the current character at the input position
 *
 * @param lexer The syntax lexer
 * @return The current character or '\0' if at the end of input
 */
char
rift_regex_syntax_lexer_get_current_char(const rift_regex_syntax_lexer_t *lexer)
{
    if (!lexer || lexer->position >= lexer->length) {
        return '\0';
    }

    return lexer->input[lexer->position];
}

/**
 * @brief Check if the lexer is at the end of input
 *
 * @param lexer The syntax lexer
 * @return true if at the end of input, false otherwise
 */
bool
rift_regex_syntax_lexer_is_at_end(const rift_regex_syntax_lexer_t *lexer)
{
    return !lexer || lexer->position >= lexer->length;
}

/**
 * @brief Check if the current input is using the LibRift r'' syntax
 *
 * @param lexer The syntax lexer
 * @return true if the input is using LibRift syntax, false otherwise
 */
bool
rift_regex_syntax_lexer_is_rift_syntax(const rift_regex_syntax_lexer_t *lexer)
{
    return lexer && lexer->is_rift_syntax;
}

/**
 * @brief Get the last error message
 *
 * @param lexer The syntax lexer
 * @return The last error message or NULL if no error occurred
 */
const char *
rift_regex_syntax_lexer_get_error(const rift_regex_syntax_lexer_t *lexer)
{
    if (!lexer || lexer->error[0] == '\0') {
        return NULL;
    }

    return lexer->error;
}

/**
 * @brief Reset the lexer to the beginning of the input
 *
 * @param lexer The syntax lexer
 */
void
rift_regex_syntax_lexer_reset(rift_regex_syntax_lexer_t *lexer)
{
    if (!lexer) {
        return;
    }

    lexer->position = 0;

    /* Free the current token's value if any */
    if (lexer->current.value) {
        free(lexer->current.value);
        lexer->current.value = NULL;
    }

    lexer->current.type = RIFT_REGEX_TOKEN_END;
    lexer->current.position = 0;
    lexer->is_rift_syntax = false;
    lexer->quote_char = '\0';
    lexer->error[0] = '\0';
}

/**
 * @brief Free resources associated with a token
 *
 * @param token The token to free
 */
void
rift_regex_syntax_token_free(rift_regex_token_t *token)
{
    if (!token) {
        return;
    }

    if (token->value) {
        free(token->value);
        token->value = NULL;
    }
}

/**
 * @brief Get the name of a token type
 *
 * @param type The token type
 * @return The name of the token type
 */
const char *
rift_regex_syntax_token_type_name(rift_regex_token_type_t type)
{
    static const char *token_type_names[] = {"LITERAL",
                                             "DOT",
                                             "CARET",
                                             "DOLLAR",
                                             "STAR",
                                             "PLUS",
                                             "QUESTION",
                                             "LPAREN",
                                             "RPAREN",
                                             "LBRACKET",
                                             "RBRACKET",
                                             "PIPE",
                                             "LBRACE",
                                             "RBRACE",
                                             "COMMA",
                                             "QUANTIFIER",
                                             "BACKSLASH",
                                             "CHAR_CLASS",
                                             "GROUP_START",
                                             "GROUP_END",
                                             "NAMED_GROUP",
                                             "NON_CAPTURING",
                                             "LOOKAHEAD",
                                             "NEGATIVE_LOOKAHEAD",
                                             "LOOKBEHIND",
                                             "NEGATIVE_LOOKBEHIND",
                                             "ATOMIC_GROUP",
                                             "COMMENT",
                                             "OPTION",
                                             "BACKREFERENCE",
                                             "NAMED_BACKREFERENCE",
                                             "ESCAPE_SEQUENCE",
                                             "WORD_BOUNDARY",
                                             "NOT_WORD_BOUNDARY",
                                             "START_OF_INPUT",
                                             "END_OF_INPUT",
                                             "BACKREF_RESET",
                                             "RIFT_PREFIX",
                                             "RIFT_QUOTE_START",
                                             "RIFT_QUOTE_END",
                                             "RIFT_ESCAPE",
                                             "ERROR",
                                             "END"};

    if (type >= 0 && type <= RIFT_REGEX_TOKEN_END) {
        return token_type_names[type];
    }

    return "UNKNOWN";
}

/**
 * @brief Check if the input string begins with r'' syntax
 *
 * This is a utility function to quickly check if a pattern string
 * uses the LibRift extended syntax format without creating a lexer.
 *
 * @param input The input string to check
 * @return true if the input uses LibRift syntax, false otherwise
 */
bool
rift_regex_is_rift_syntax(const char *input)
{
    return check_rift_syntax(input);
} /**
   * @file lexer.c
   * @brief Implementation of the regex syntax lexer component of LibRift
   *
   * This file implements the tokenization for regular expression patterns
   * with support for the extended `r''` syntax in the LibRift regex engine.
   *
   * @copyright Copyright (c) 2025 LibRift Project
   * @license MIT License
   */

/**
 * @brief lexer_advance the lexer position and return the character at the new position
 *
 * @param lexer The syntax lexer
 * @return The character at the new position, or '\0' if past end of input
 */
static char
lexer_advance(rift_regex_syntax_lexer_t *lexer)
{
    if (lexer->position >= lexer->length) {
        return '\0';
    }

    return lexer->input[lexer->position++];
}

/**
 * @brief Peek at the current character without advancing
 *
 * @param lexer The syntax lexer
 * @return The current character, or '\0' if at end of input
 */
static char
peek(const rift_regex_syntax_lexer_t *lexer)
{
    if (lexer->position >= lexer->length) {
        return '\0';
    }

    return lexer->input[lexer->position];
}

/**
 * @brief Peek at the next character (one ahead of current position)
 *
 * @param lexer The syntax lexer
 * @return The next character, or '\0' if next would be past end of input
 */
static char
peek_next(const rift_regex_syntax_lexer_t *lexer)
{
    if (lexer->position + 1 >= lexer->length) {
        return '\0';
    }

    return lexer->input[lexer->position + 1];
}

/**
 * @brief Set an error message
 *
 * @param lexer The syntax lexer
 * @param format Format string for the error message
 * @param ... Additional arguments for the format string
 */
static void
set_error(rift_regex_syntax_lexer_t *lexer, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(lexer->error, RIFT_REGEX_MAX_ERROR_LENGTH, format, args);
    va_end(args);
}

/**
 * @brief Scan an escape sequence in standard regex syntax
 *
 * @param lexer The syntax lexer
 * @param start_pos Position where the escape sequence starts
 * @return The token for the escape sequence
 */
static rift_regex_token_t
scan_escape_sequence(rift_regex_syntax_lexer_t *lexer, size_t start_pos)
{
    char c = lexer_advance(lexer);

    if (c == '\0') {
        set_error(lexer, "Unterminated escape sequence at position %zu", start_pos);
        return create_token(RIFT_REGEX_TOKEN_ERROR, "Unterminated escape sequence", start_pos);
    }

    /* Handle specific escape sequences */
    switch (c) {
    case 'd':
        return create_token(RIFT_REGEX_TOKEN_ESCAPE_SEQUENCE, "\\d", start_pos);
    case 'D':
        return create_token(RIFT_REGEX_TOKEN_ESCAPE_SEQUENCE, "\\D", start_pos);
    case 'w':
        return create_token(RIFT_REGEX_TOKEN_ESCAPE_SEQUENCE, "\\w", start_pos);
    case 'W':
        return create_token(RIFT_REGEX_TOKEN_ESCAPE_SEQUENCE, "\\W", start_pos);
    case 's':
        return create_token(RIFT_REGEX_TOKEN_ESCAPE_SEQUENCE, "\\s", start_pos);
    case 'S':
        return create_token(RIFT_REGEX_TOKEN_ESCAPE_SEQUENCE, "\\S", start_pos);
    case 'b':
        return create_token(RIFT_REGEX_TOKEN_WORD_BOUNDARY, "\\b", start_pos);
    case 'B':
        return create_token(RIFT_REGEX_TOKEN_NOT_WORD_BOUNDARY, "\\B", start_pos);
    case 'A':
        return create_token(RIFT_REGEX_TOKEN_START_OF_INPUT, "\\A", start_pos);
    case 'Z':
        return create_token(RIFT_REGEX_TOKEN_END_OF_INPUT, "\\Z", start_pos);
    case 'z':
        return create_token(RIFT_REGEX_TOKEN_END_OF_INPUT, "\\z", start_pos);
    case 'K':
        return create_token(RIFT_REGEX_TOKEN_BACKREF_RESET, "\\K", start_pos);

    /* Digit means a backreference */
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
        char buffer[16] = {'\\', c, '\0'};

        /* Read any additional digits for the backreference */
        int i = 2;
        while (isdigit(peek(lexer)) && i < 15) {
            buffer[i++] = lexer_advance(lexer);
        }
        buffer[i] = '\0';

        return create_token(RIFT_REGEX_TOKEN_BACKREFERENCE, buffer, start_pos);
    }

    /* Named backreference: \k<name> or \k'name' or \k{name} */
    case 'k': {
        if (peek(lexer) == '<' || peek(lexer) == '\'' || peek(lexer) == '{') {
            char open_char = lexer_advance(lexer);
            char close_char;

            /* Determine the closing character */
            if (open_char == '<')
                close_char = '>';
            else if (open_char == '\'')
                close_char = '\'';
            else
                close_char = '}';

            char buffer[TOKEN_VALUE_BUFFER_SIZE] = {'\\', 'k', open_char, '\0'};
            int i = 3;

            /* Read until the closing character or end of input */
            while (peek(lexer) != close_char && peek(lexer) != '\0' &&
                   i < TOKEN_VALUE_BUFFER_SIZE - 2) {
                buffer[i++] = lexer_advance(lexer);
            }

            /* Add the closing character if present */
            if (peek(lexer) == close_char) {
                buffer[i++] = lexer_advance(lexer);
                buffer[i] = '\0';
                return create_token(RIFT_REGEX_TOKEN_NAMED_BACKREFERENCE, buffer, start_pos);
            } else {
                set_error(lexer, "Unterminated named backreference at position %zu", start_pos);
                return create_token(RIFT_REGEX_TOKEN_ERROR, "Unterminated named backreference",
                                    start_pos);
            }
        }
        /* If not a proper named backreference format, treat as a literal escape */
        return create_token(RIFT_REGEX_TOKEN_ESCAPE_SEQUENCE, "\\k", start_pos);
    }

    /* Escaped characters that retain their special meaning */
    case '.':
    case '*':
    case '+':
    case '?':
    case '|':
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case '^':
    case '$':
    case '\\': {
        char buffer[3] = {'\\', c, '\0'};
        return create_token(RIFT_REGEX_TOKEN_ESCAPE_SEQUENCE, buffer, start_pos);
    }

    /* Any other escaped character is treated as the literal character */
    default: {
        char buffer[2] = {c, '\0'};
        return create_token(RIFT_REGEX_TOKEN_LITERAL, buffer, start_pos);
    }
    }
}

/**
 * @brief Scan a character class [...]
 *
 * @param lexer The syntax lexer
 * @param start_pos Position where the character class starts
 * @return The token for the character class
 */
static rift_regex_token_t
scan_character_class(rift_regex_syntax_lexer_t *lexer, size_t start_pos)
{
    char buffer[TOKEN_VALUE_BUFFER_SIZE] = {'[', '\0'};
    int i = 1;
    bool escaped = false;

    /* Read until the closing bracket or end of input */
    while (true) {
        char c = peek(lexer);

        if (c == '\0') {
            set_error(lexer, "Unterminated character class at position %zu", start_pos);
            return create_token(RIFT_REGEX_TOKEN_ERROR, "Unterminated character class", start_pos);
        }

        lexer_advance(lexer);

        /* Add the character to the buffer if there's room */
        if (i < TOKEN_VALUE_BUFFER_SIZE - 1) {
            buffer[i++] = c;
        }

        /* Check for closing bracket (if not escaped and not at the beginning) */
        if (c == ']' && !escaped && i > 2) {
            /* If the first character after [ is ^, need at least 3 chars */
            if (buffer[1] != '^' || i > 3) {
                buffer[i] = '\0';
                return create_token(RIFT_REGEX_TOKEN_CHAR_CLASS, buffer, start_pos);
            }
        }

        /* Update escaped status */
        if (c == '\\' && !escaped) {
            escaped = true;
        } else {
            escaped = false;
        }
    }
}

/**
 * @brief Scan a group (...)
 *
 * @param lexer The syntax lexer
 * @param start_pos Position where the group starts
 * @return The token for the group
 */
static rift_regex_token_t
scan_group(rift_regex_syntax_lexer_t *lexer, size_t start_pos)
{
    /* Check if it's a special group starting with (? */
    if (peek(lexer) == '?') {
        lexer_advance(lexer); /* Consume the '?' */

        char next = peek(lexer);
        switch (next) {
        case ':': {
            lexer_advance(lexer); /* Consume the ':' */
            return create_token(RIFT_REGEX_TOKEN_NON_CAPTURING, "(?:", start_pos);
        }
        case '=': {
            lexer_advance(lexer); /* Consume the '=' */
            return create_token(RIFT_REGEX_TOKEN_LOOKAHEAD, "(?=", start_pos);
        }
        case '!': {
            lexer_advance(lexer); /* Consume the '!' */
            return create_token(RIFT_REGEX_TOKEN_NEGATIVE_LOOKAHEAD, "(?!", start_pos);
        }
        case '<': {
            lexer_advance(lexer); /* Consume the '<' */

            /* Check next character to determine the type */
            next = peek(lexer);
            if (next == '=') {
                lexer_advance(lexer); /* Consume the '=' */
                return create_token(RIFT_REGEX_TOKEN_LOOKBEHIND, "(?<=", start_pos);
            } else if (next == '!') {
                lexer_advance(lexer); /* Consume the '!' */
                return create_token(RIFT_REGEX_TOKEN_NEGATIVE_LOOKBEHIND, "(?<!", start_pos);
            } else {
                /* Named capturing group */
                return create_token(RIFT_REGEX_TOKEN_NAMED_GROUP, "(?<", start_pos);
            }
        }
        case '>': {
            lexer_advance(lexer); /* Consume the '>' */
            return create_token(RIFT_REGEX_TOKEN_ATOMIC_GROUP, "(?>", start_pos);
        }
        case '#': {
            lexer_advance(lexer); /* Consume the '#' */
            return create_token(RIFT_REGEX_TOKEN_COMMENT, "(?#", start_pos);
        }
        case 'i':
        case 'm':
        case 's':
        case 'x':
        case 'J':
        case 'U':
        case 'X': {
            /* Option flags */
            char buffer[TOKEN_VALUE_BUFFER_SIZE] = {'(', '?', next, '\0'};
            int i = 3;

            lexer_advance(lexer); /* Consume the flag */

            /* Read additional flags */
            while (strchr("imsxJUX", peek(lexer)) && i < TOKEN_VALUE_BUFFER_SIZE - 2) {
                buffer[i++] = lexer_advance(lexer);
            }

            /* Check for closing ) or : */
            if (peek(lexer) == ')' || peek(lexer) == ':') {
                buffer[i++] = lexer_advance(lexer);
                buffer[i] = '\0';
                return create_token(RIFT_REGEX_TOKEN_OPTION, buffer, start_pos);
            } else {
                set_error(lexer, "Invalid option group at position %zu", start_pos);
                return create_token(RIFT_REGEX_TOKEN_ERROR, "Invalid option group", start_pos);
            }
        }
        case 'P': {
            /* Python-style named group or named backreference */
            lexer_advance(lexer); /* Consume the 'P' */

            if (peek(lexer) == '<') {
                lexer_advance(lexer); /* Consume the '<' */
                return create_token(RIFT_REGEX_TOKEN_NAMED_GROUP, "(?P<", start_pos);
            } else if (peek(lexer) == '=') {
                lexer_advance(lexer); /* Consume the '=' */
                return create_token(RIFT_REGEX_TOKEN_NAMED_BACKREFERENCE, "(?P=", start_pos);
            } else {
                set_error(lexer, "Invalid Python-style group at position %zu", start_pos);
                return create_token(RIFT_REGEX_TOKEN_ERROR, "Invalid Python-style group",
                                    start_pos);
            }
        }
        default: {
            set_error(lexer, "Invalid special group at position %zu", start_pos);
            return create_token(RIFT_REGEX_TOKEN_ERROR, "Invalid special group", start_pos);
        }
        }
    }

    /* Regular capturing group */
    return create_token(RIFT_REGEX_TOKEN_LPAREN, "(", start_pos);
}

/**
 * @brief Scan a quantifier {...}
 *
 * @param lexer The syntax lexer
 * @param start_pos Position where the quantifier starts
 * @return The token for the quantifier
 */
static rift_regex_token_t
scan_quantifier(rift_regex_syntax_lexer_t *lexer, size_t start_pos)
{
    char buffer[TOKEN_VALUE_BUFFER_SIZE] = {'{', '\0'};
    int i = 1;

    /* Read until the closing brace or end of input */
    while (true) {
        char c = peek(lexer);

        if (c == '\0') {
            set_error(lexer, "Unterminated quantifier at position %zu", start_pos);
            return create_token(RIFT_REGEX_TOKEN_ERROR, "Unterminated quantifier", start_pos);
        }

        lexer_advance(lexer);

        /* Add the character to the buffer if there's room */
        if (i < TOKEN_VALUE_BUFFER_SIZE - 1) {
            buffer[i++] = c;
        }

        /* Check for closing brace */
        if (c == '}') {
            buffer[i] = '\0';
            return create_token(RIFT_REGEX_TOKEN_QUANTIFIER, buffer, start_pos);
        }
    }
}
