# =================================================================
# RIFT Compiler Pipeline - Root CMake Configuration
# OBINexus Computing Framework v4.0.0
# Technical Implementation: Systematic Pipeline Compilation
# AEGIS Methodology Compliance: ENABLED
# Zero Trust Governance: ENABLED
# =================================================================

cmake_minimum_required(VERSION 3.16)
project(RIFT_Compiler VERSION 4.0.0 LANGUAGES C)

# Build configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# AEGIS Framework Configuration
option(RIFT_ENABLE_MEMORY_SAFETY "Enable memory safety features" ON)
option(RIFT_ENABLE_STRICT_MODE "Enable strict compilation mode" ON)
option(RIFT_BUILD_TESTS "Build test suite" ON)

# Security and compliance flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic -Werror -DRIFT_DEBUG=1")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG -Wall -Wextra -Wpedantic -Werror")

add_compile_options(
    -fstack-protector-strong
    -D_FORTIFY_SOURCE=2
    -fPIE
)

add_link_options(
    -Wl,-z,relro
    -Wl,-z,now
    -pie
)

# External dependencies
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Include common pipeline infrastructure
include(cmake/common/compiler_pipeline.cmake)

# Global include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# RIFT Pipeline Stages
add_subdirectory(rift-0)  # Tokenization
add_subdirectory(rift-1)  # Parsing  
add_subdirectory(rift-2)  # Semantic Analysis
add_subdirectory(rift-3)  # Validation
add_subdirectory(rift-4)  # Bytecode Generation
add_subdirectory(rift-5)  # Verification
add_subdirectory(rift-6)  # Code Emission

# Core components
add_subdirectory(src/core)
add_subdirectory(src/cli)
add_subdirectory(src/config)

# Testing framework
if(RIFT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation generation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_SOURCE_DIR}/docs/Doxyfile.in 
                   ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating RIFT API documentation"
    )
endif()

# Configuration summary
message(STATUS "=================================================================")
message(STATUS "RIFT Compiler Root Configuration Summary:")
message(STATUS "  Project: ${PROJECT_NAME}")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "  Source Directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "  Binary Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  OpenSSL Version: ${OPENSSL_VERSION}")
message(STATUS "  AEGIS Compliance: ENABLED")
message(STATUS "  Zero Trust Governance: ENABLED")
message(STATUS "=================================================================")
