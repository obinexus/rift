#include "rift/platform.h"
#if !defined(RIFT_PLATFORM_WINDOWS)
#  include <time.h>
#endif

/* Mutex implementation */

int rift_mutex_init(rift_mutex_t *mutex) {
    if (!mutex) return -1;
#if defined(RIFT_PLATFORM_WINDOWS)
    InitializeCriticalSection(mutex);
    return 0;
#else
    return pthread_mutex_init(mutex, NULL);
#endif
}

int rift_mutex_lock(rift_mutex_t *mutex) {
    if (!mutex) return -1;
#if defined(RIFT_PLATFORM_WINDOWS)
    EnterCriticalSection(mutex);
    return 0;
#else
    return pthread_mutex_lock(mutex);
#endif
}

int rift_mutex_unlock(rift_mutex_t *mutex) {
    if (!mutex) return -1;
#if defined(RIFT_PLATFORM_WINDOWS)
    LeaveCriticalSection(mutex);
    return 0;
#else
    return pthread_mutex_unlock(mutex);
#endif
}

int rift_mutex_destroy(rift_mutex_t *mutex) {
    if (!mutex) return -1;
#if defined(RIFT_PLATFORM_WINDOWS)
    DeleteCriticalSection(mutex);
    return 0;
#else
    return pthread_mutex_destroy(mutex);
#endif
}

/* Timing */

double rift_time_ms(void) {
#if defined(RIFT_PLATFORM_WINDOWS)
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)freq.QuadPart * 1000.0;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
#endif
}
