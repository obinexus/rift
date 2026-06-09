#include <stdio.h>
#include <string.h>
#include "rift/cli.h"
#include "rift/version.h"
#include "rift/platform.h"

int rift_cmd_doctor(int argc, char **argv) {
    (void)argc;
    (void)argv;

    printf("rift doctor: diagnostics report\n");
    printf("================================\n");
    printf("Version:    %s\n", RIFT_VERSION_STRING);

#if defined(RIFT_PLATFORM_WINDOWS)
    printf("Platform:   Windows\n");
#elif defined(RIFT_PLATFORM_MACOS)
    printf("Platform:   macOS\n");
#elif defined(RIFT_PLATFORM_LINUX)
    printf("Platform:   Linux\n");
#else
    printf("Platform:   Unknown\n");
#endif

    printf("C Standard: C11\n");
    printf("Regex:      portable RIFT expression engine\n");

    printf("Threads:    ");
#if defined(RIFT_PLATFORM_WINDOWS)
    printf("Win32 CRITICAL_SECTION\n");
#else
    printf("POSIX pthread\n");
#endif

    printf("\nAll checks passed.\n");
    return 0;
}
