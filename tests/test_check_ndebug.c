/*
 * Built with NDEBUG defined in every configuration. The failing check below
 * must still fire; CTest expects exit status 1 and the failure message.
 */
#ifndef NDEBUG
#define NDEBUG
#endif
#include "rift_check.h"

int main(void) {
    volatile int value = 1;
    RIFT_CHECK(value == 2);
    puts("test_check_ndebug: check was skipped");
    return 0;
}
