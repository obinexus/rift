# LibRift - CMake Build Configuration
# Comprehensive Build System for Cross-Platform Regex Engine

cmake_minimum_required(VERSION 3.16)

# Project Metadata Definition
project(LibRift 
    VERSION 1.0.0
    DESCRIPTION "High-Performance Cross-Platform Regex Engine"
    LANGUAGES C CXX
)

# ============================================================================
# BUILD CONFIGURATION OPTIONS
# ============================================================================
option(LIBRIFT_BUILD_TESTS "Enable test suite compilation" ON)
option(LIBRIFT_USE_CTEST "Use CTest for running tests" ON)
option(LIBRIFT_BUILD_EXAMPLES "Build example applications" OFF)
option(LIBRIFT_BUILD_DOCS "Generate documentation" OFF)
option(LIBRIFT_ENABLE_OPTIMIZATIONS "Enable performance optimizations" ON)
option(LIBRIFT_ENABLE_VERBOSE_LOGGING "Enable detailed logging" OFF)
option(LIBRIFT_USE_MEMORY_POOL "Use custom memory management" ON)
option(LIBRIFT_ENABLE_THREAD_SAFETY "Enable thread-safe components" ON)

# ============================================================================
# COMPILER CONFIGURATION
# ============================================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific optimization flags
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(LIBRIFT_COMMON_FLAGS 
        "-Wall -Wextra -Wpedantic -Werror"
    )
    set(CMAKE_C_FLAGS_DEBUG "${LIBRIFT_COMMON_FLAGS} -g -O0")
    set(CMAKE_C_FLAGS_RELEASE "${LIBRIFT_COMMON_FLAGS} -O3")
elseif(MSVC)
    set(LIBRIFT_COMMON_FLAGS 
        "/W4 /WX"
    )
    set(CMAKE_C_FLAGS_DEBUG "${LIBRIFT_COMMON_FLAGS} /Zi")
    set(CMAKE_C_FLAGS_RELEASE "${LIBRIFT_COMMON_FLAGS} /O2")
endif()

# ============================================================================
# DEPENDENCY MANAGEMENT
# ============================================================================
# Fetch external dependencies
include(FetchContent)

# GoogleTest for testing
if(LIBRIFT_BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.12.1
    )
    FetchContent_MakeAvailable(googletest)
endif()

# ============================================================================
# SOURCE FILE DISCOVERY
# ============================================================================
# Macro for discovering source files
macro(discover_sources BASE_DIR RESULT_VAR)
    file(GLOB_RECURSE ${RESULT_VAR}
        "${BASE_DIR}/*.c"
        "${BASE_DIR}/*.cpp"
    )
endmacro()

# Discover source files for different components
discover_sources("${CMAKE_CURRENT_SOURCE_DIR}/core" LIBRIFT_CORE_SOURCES)
discover_sources("${CMAKE_CURRENT_SOURCE_DIR}/core/regex" LIBRIFT_REGEX_SOURCES)
discover_sources("${CMAKE_CURRENT_SOURCE_DIR}/cli" LIBRIFT_CLI_SOURCES)

# Validate source discovery
if(NOT LIBRIFT_CORE_SOURCES)
    message(FATAL_ERROR "No core source files discovered. Verify project structure.")
endif()

# ============================================================================
# LIBRARY DEFINITION
# ============================================================================
# Create core library
add_library(librift_core STATIC 
    ${LIBRIFT_CORE_SOURCES}
)

target_include_directories(librift_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Compile definitions based on configuration
target_compile_definitions(librift_core PRIVATE
    $<$<BOOL:${LIBRIFT_USE_MEMORY_POOL}>:LIBRIFT_USE_MEMORY_POOL>
    $<$<BOOL:${LIBRIFT_ENABLE_THREAD_SAFETY}>:LIBRIFT_THREAD_SAFE>
)
# ============================================================================
# TESTING CONFIGURATION
# ============================================================================
if(LIBRIFT_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    if(LIBRIFT_USE_CTEST)
        include(CTest)
    else()
        enable_testing()
    endif()
    add_subdirectory(tests)
endif()

# ============================================================================
# INSTALLATION CONFIGURATION
# ============================================================================
include(GNUInstallDirs)

install(TARGETS librift_core
install(TARGETS librift_core
    EXPORT LibRiftTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
message(STATUS "  Version:              ${PROJECT_VERSION}")
message(STATUS "  Build Tests:          ${LIBRIFT_BUILD_TESTS}")
message(STATUS "  Use CTest:            ${LIBRIFT_USE_CTEST}")
message(STATUS "  Build Examples:       ${LIBRIFT_BUILD_EXAMPLES}")
message(STATUS "  Memory Pool:          ${LIBRIFT_USE_MEMORY_POOL}")
message(STATUS "  Thread Safety:        ${LIBRIFT_ENABLE_THREAD_SAFETY}")
message(STATUS "  Build Tests:          ${LIBRIFT_BUILD_TESTS}")
message(STATUS "  Build Examples:       ${LIBRIFT_BUILD_EXAMPLES}")
message(STATUS "  Memory Pool:          ${LIBRIFT_USE_MEMORY_POOL}")
message(STATUS "  Thread Safety:        ${LIBRIFT_ENABLE_THREAD_SAFETY}")