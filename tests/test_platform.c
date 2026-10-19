#include <stdio.h>
#include <stdlib.h>
#include "rift/platform.h"

#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #cond); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

int main(void) {
    rift_mutex_t mutex;
    double first;
    double second;

    /* rift_time_ms needs clock_gettime(CLOCK_MONOTONIC) on POSIX. */
    first = rift_time_ms();
    second = rift_time_ms();
    CHECK(first > 0.0);
    CHECK(second >= first);

    /* The mutex wrappers need pthreads on POSIX. */
    CHECK(rift_mutex_init(&mutex) == 0);
    CHECK(rift_mutex_lock(&mutex) == 0);
    CHECK(rift_mutex_unlock(&mutex) == 0);
    CHECK(rift_mutex_destroy(&mutex) == 0);
    CHECK(rift_mutex_init(NULL) == -1);

    puts("test_platform: PASS");
    return 0;
}
