#ifndef RIFT_TESTS_RIFT_CHECK_H
#define RIFT_TESTS_RIFT_CHECK_H

#include <stdio.h>
#include <stdlib.h>

/*
 * RIFT_CHECK evaluates its condition in every build configuration. Unlike
 * assert(), NDEBUG does not remove it, so Release test runs still check
 * results. A failed check prints the location and expression, then exits
 * with a non-zero status.
 */
#define RIFT_CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "%s:%d: check failed: %s\n", \
                __FILE__, __LINE__, #cond); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

#endif /* RIFT_TESTS_RIFT_CHECK_H */
