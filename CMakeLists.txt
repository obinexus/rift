# CMakeLists.txt - RIFT Compiler Pipeline Root Configuration
# RIFT: RIFT Is a Flexible Translator
# OBINexus Computing Framework - Root Build Orchestration
# Technical Implementation: Systematic Pipeline Integration

cmake_minimum_required(VERSION 3.16)

project(RIFT_Compiler
    VERSION 4.0.0
    DESCRIPTION "RIFT Is a Flexible Translator - Modular Compiler Pipeline"
    LANGUAGES C
)

# Build configuration standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Root directory definitions
set(RIFT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(RIFT_CMAKE_DIR ${RIFT_ROOT_DIR}/cmake)
set(RIFT_INCLUDE_DIR ${RIFT_ROOT_DIR}/include)
set(RIFT_SRC_DIR ${RIFT_ROOT_DIR}/src)
set(RIFT_LIB_DIR ${RIFT_ROOT_DIR}/lib)
set(RIFT_BIN_DIR ${RIFT_ROOT_DIR}/bin)

# Ensure required directories exist
file(MAKE_DIRECTORY ${RIFT_LIB_DIR})
file(MAKE_DIRECTORY ${RIFT_BIN_DIR})
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/logs)

# Include common pipeline configuration (create if missing)
if(EXISTS "${RIFT_CMAKE_DIR}/common/compiler_pipeline.cmake")
    include(${RIFT_CMAKE_DIR}/common/compiler_pipeline.cmake)
else()
    message(WARNING "Common pipeline configuration missing - using fallback")
endif()

# Compiler flags for AEGIS methodology compliance
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic -Werror -DRIFT_DEBUG=1")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG -Wall -Wextra -Wpedantic -Werror")

# Security and governance flags
add_compile_options(
    -fstack-protector-strong
    -D_FORTIFY_SOURCE=2
    -fPIE
    -DRIFT_AEGIS_COMPLIANCE=1
    -DRIFT_ZERO_TRUST=1
)

add_link_options(
    -Wl,-z,relro
    -Wl,-z,now
    -pie
)

# Global include directories
include_directories(
    ${RIFT_INCLUDE_DIR}
    ${RIFT_INCLUDE_DIR}/rift/core
)

# External dependencies with error handling
find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL required for AEGIS trust tagging")
endif()

find_package(Threads REQUIRED)
if(NOT Threads_FOUND)
    message(FATAL_ERROR "Threads required for dual-mode parsing")
endif()

# Function to safely add subdirectory if it exists
function(safe_add_subdirectory DIR_NAME)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DIR_NAME}/CMakeLists.txt")
        add_subdirectory(${DIR_NAME})
        message(STATUS "Added subdirectory: ${DIR_NAME}")
    else()
        message(WARNING "Subdirectory missing: ${DIR_NAME} - creating placeholder")
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${DIR_NAME})
        file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/${DIR_NAME}/CMakeLists.txt
            "# Placeholder CMakeLists.txt for ${DIR_NAME}\n"
            "message(STATUS \"Placeholder configuration for ${DIR_NAME}\")\n"
        )
        add_subdirectory(${DIR_NAME})
    endif()
endfunction()

# Pipeline stages with safe addition
message(STATUS "Configuring RIFT pipeline stages...")
safe_add_subdirectory(rift-0)  # Tokenization
safe_add_subdirectory(rift-1)  # Parsing
safe_add_subdirectory(rift-2)  # Semantic Analysis
safe_add_subdirectory(rift-3)  # Validation
safe_add_subdirectory(rift-4)  # Bytecode Generation
safe_add_subdirectory(rift-5)  # Verification
safe_add_subdirectory(rift-6)  # Emission

# Core library components with safe addition
message(STATUS "Configuring core components...")
safe_add_subdirectory(src/core)
safe_add_subdirectory(src/cli)
safe_add_subdirectory(src/config)

# Testing framework
enable_testing()
safe_add_subdirectory(tests)

# Documentation generation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY
    )
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating RIFT API documentation with Doxygen"
    )
else()
    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E echo "Doxygen not found - documentation generation skipped"
    )
endif()

# Installation configuration
install(DIRECTORY ${RIFT_INCLUDE_DIR}/rift DESTINATION include)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.riftrc")
    install(FILES .riftrc DESTINATION share/rift)
endif()

# Custom targets for development workflow
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${RIFT_LIB_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${RIFT_BIN_DIR}
    COMMENT "Deep clean of all build artifacts"
)

add_custom_target(pipeline-demo
    COMMAND ${CMAKE_COMMAND} -E echo "Executing RIFT pipeline demonstration..."
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/demo_pipeline.sh
    DEPENDS rift-0.exe rift-1.exe rift-2.exe rift-3.exe rift-4.exe rift-5.exe rift-6.exe
    COMMENT "Running complete RIFT pipeline demonstration"
)

# Validation target
add_custom_target(validate-architecture
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/validation/integrated_aegis_validation.sh
    COMMENT "Validating AEGIS methodology compliance"
)

# Build summary and status reporting
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

# Final configuration validation
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug mode: Comprehensive logging and validation enabled")
else()
    message(STATUS "Release mode: Optimized for production deployment")
endif()

message(STATUS "RIFT root configuration complete - ready for compilation")
