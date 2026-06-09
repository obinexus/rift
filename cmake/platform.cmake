# Platform detection and compiler flags for RIFT

if(WIN32)
    add_compile_definitions(RIFT_PLATFORM_WINDOWS)
    if(MSVC)
        add_compile_options(/W4)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic)
    endif()
elseif(APPLE)
    add_compile_definitions(RIFT_PLATFORM_MACOS _POSIX_C_SOURCE=200809L _DEFAULT_SOURCE)
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(UNIX)
    add_compile_definitions(RIFT_PLATFORM_LINUX _POSIX_C_SOURCE=200809L _DEFAULT_SOURCE)
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Debug/Release flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(RIFT_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O2)
endif()
