#include <stdio.h>
#include "rift_check.h"
#include "rift/platform.h"

int main(void) {
    rift_mutex_t mutex;
    double first;
    double second;

    /* rift_time_ms needs clock_gettime(CLOCK_MONOTONIC) on POSIX. */
    first = rift_time_ms();
    second = rift_time_ms();
    RIFT_CHECK(first > 0.0);
    RIFT_CHECK(second >= first);

    /* The mutex wrappers need pthreads on POSIX. */
    RIFT_CHECK(rift_mutex_init(&mutex) == 0);
    RIFT_CHECK(rift_mutex_lock(&mutex) == 0);
    RIFT_CHECK(rift_mutex_unlock(&mutex) == 0);
    RIFT_CHECK(rift_mutex_destroy(&mutex) == 0);
    RIFT_CHECK(rift_mutex_init(NULL) == -1);

    puts("test_platform: PASS");
    return 0;
}
