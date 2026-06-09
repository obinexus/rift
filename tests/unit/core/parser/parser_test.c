/**
 * @file parser_test.c
 * @brief Test suite for the regex parser component of LibRift
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "librift/parser/parser.h"
#include "librift/parser/tokenizer.h"
#include "librift/parser/validator.h"

/* Test fixture setup and teardown */
static int
setup(void **state)
{
    rift_regex_parser_t *parser = rift_regex_parser_create(RIFT_REGEX_FLAG_NONE, false);
    if (!parser) {
        return -1;
    }
    *state = parser;
    return 0;
}

static int
teardown(void **state)
{
    rift_regex_parser_t *parser = (rift_regex_parser_t *)*state;
    rift_regex_parser_free(parser);
    return 0;
}

/* Test cases */
static void
test_parser_create_destroy(void **state)
{
    (void)state; /* Unused */

    rift_regex_parser_t *parser = rift_regex_parser_create(RIFT_REGEX_FLAG_NONE, false);
    assert_non_null(parser);
    rift_regex_parser_free(parser);
}

static void
test_parser_set_tokenizer(void **state)
{
    rift_regex_parser_t *parser = (rift_regex_parser_t *)*state;
    rift_regex_tokenizer_t *tokenizer = rift_regex_tokenizer_create();

    assert_non_null(tokenizer);
    assert_true(rift_regex_parser_set_tokenizer(parser, tokenizer));

    /* Tokenizer is now owned by the parser */
}

static void
test_parser_simple_pattern(void **state)
{
    rift_regex_parser_t *parser = (rift_regex_parser_t *)*state;
    const char *pattern = "abc";

    rift_regex_ast_t *ast = rift_regex_parser_parse(parser, pattern);
    assert_non_null(ast);

    /* Free ast manually since we're not calling parser_free yet */
    /* (In actual implementation, would need proper AST free function) */
}

static void
test_parser_with_options(void **state)
{
    rift_regex_parser_t *parser = (rift_regex_parser_t *)*state;
    const char *pattern = "abc";

    rift_regex_ast_t *ast =
        rift_regex_parser_parse_with_options(parser, pattern, RIFT_REGEX_FLAG_CASE_INSENSITIVE);
    assert_non_null(ast);
}

static void
test_parser_complex_pattern(void **state)
{
    rift_regex_parser_t *parser = (rift_regex_parser_t *)*state;
    const char *pattern = "(a|b)*c+d?";

    rift_regex_ast_t *ast = rift_regex_parser_parse(parser, pattern);
    assert_non_null(ast);
}

static void
test_parser_invalid_pattern(void **state)
{
    rift_regex_parser_t *parser = (rift_regex_parser_t *)*state;
    const char *pattern = "(unclosed";

    rift_regex_ast_t *ast = rift_regex_parser_parse(parser, pattern);
    assert_null(ast);

    /* Verify error was set */
    rift_regex_error_t error = rift_regex_pattern_parser_get_error(parser);
    assert_int_not_equal(error, RIFT_REGEX_ERROR_NONE);

    size_t pos = rift_regex_parser_get_error_position(parser);
    assert_int_not_equal(pos, (size_t)-1);
}

int
main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parser_create_destroy),
        cmocka_unit_test_setup_teardown(test_parser_set_tokenizer, setup, teardown),
        cmocka_unit_test_setup_teardown(test_parser_simple_pattern, setup, teardown),
        cmocka_unit_test_setup_teardown(test_parser_with_options, setup, teardown),
        cmocka_unit_test_setup_teardown(test_parser_complex_pattern, setup, teardown),
        cmocka_unit_test_setup_teardown(test_parser_invalid_pattern, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
