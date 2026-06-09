#ifndef RIFT_PLATFORM_H
#define RIFT_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Platform detection */
#if defined(_WIN32) || defined(_WIN64)
    #define RIFT_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
    #define RIFT_PLATFORM_MACOS 1
#elif defined(__linux__)
    #define RIFT_PLATFORM_LINUX 1
#else
    #define RIFT_PLATFORM_UNKNOWN 1
#endif

/* Component export/import macros. */
#if defined(RIFT_PLATFORM_WINDOWS)
    #if defined(RIFT_CORE_BUILD_DLL)
        #define RIFT_API __declspec(dllexport)
    #elif defined(RIFT_CORE_USE_DLL)
        #define RIFT_API __declspec(dllimport)
    #else
        #define RIFT_API
    #endif
    #if defined(RIFTLANG_BUILD_DLL)
        #define RIFTLANG_API __declspec(dllexport)
    #elif defined(RIFTLANG_USE_DLL)
        #define RIFTLANG_API __declspec(dllimport)
    #else
        #define RIFTLANG_API
    #endif
    #if defined(RIFTTEST_BUILD_DLL)
        #define RIFTTEST_API __declspec(dllexport)
    #elif defined(RIFTTEST_USE_DLL)
        #define RIFTTEST_API __declspec(dllimport)
    #else
        #define RIFTTEST_API
    #endif
    #define RIFT_CALL __cdecl
#else
    #if defined(__GNUC__) || defined(__clang__)
        #define RIFT_API __attribute__((visibility("default")))
        #define RIFTLANG_API __attribute__((visibility("default")))
        #define RIFTTEST_API __attribute__((visibility("default")))
    #else
        #define RIFT_API
        #define RIFTLANG_API
        #define RIFTTEST_API
    #endif
    #define RIFT_CALL
#endif

/* Thread abstraction */
#if defined(RIFT_PLATFORM_WINDOWS)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    typedef CRITICAL_SECTION rift_mutex_t;
    typedef HANDLE rift_thread_t;
#else
    #include <pthread.h>
    typedef pthread_mutex_t rift_mutex_t;
    typedef pthread_t rift_thread_t;
#endif

/* Platform functions */
RIFT_API int  rift_mutex_init(rift_mutex_t *mutex);
RIFT_API int  rift_mutex_lock(rift_mutex_t *mutex);
RIFT_API int  rift_mutex_unlock(rift_mutex_t *mutex);
RIFT_API int  rift_mutex_destroy(rift_mutex_t *mutex);

RIFT_API double rift_time_ms(void);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_PLATFORM_H */
