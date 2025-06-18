# CMakeLists.txt - rift-4
# RIFT: RIFT Is a Flexible Translator
# Stage: bytecode (bytecode)
# OBINexus Computing Framework - Build Orchestration

cmake_minimum_required(VERSION 3.16)

project(rift-4
    VERSION 4.0.0
    DESCRIPTION "RIFT bytecode Stage - bytecode"
    LANGUAGES C
)

# Build configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Include common pipeline configuration
if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake")
    include("${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake")
endif()

# Compiler flags for AEGIS compliance
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic -Werror -DRIFT_STAGE_BYTECODE=1 -DRIFT_DEBUG=1")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG -Wall -Wextra -Wpedantic -Werror -DRIFT_STAGE_BYTECODE=1")

# Security flags
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

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/rift/core
)

# Source files
file(GLOB_RECURSE STAGE_SOURCES 
    "src/core/*.c"
)

file(GLOB_RECURSE STAGE_HEADERS
    "include/rift-4/core/*.h"
)

# External dependencies
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Create static library
if(STAGE_SOURCES)
    add_library(rift-4_static STATIC ${STAGE_SOURCES})
    target_include_directories(rift-4_static PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
    )
    target_link_libraries(rift-4_static 
        OpenSSL::SSL OpenSSL::Crypto Threads::Threads
    )
    
    # Create shared library
    add_library(rift-4_shared SHARED ${STAGE_SOURCES})
    target_include_directories(rift-4_shared PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
    )
    target_link_libraries(rift-4_shared 
        OpenSSL::SSL OpenSSL::Crypto Threads::Threads
    )
    
    # Create standalone executable
    add_executable(rift-4.exe ${STAGE_SOURCES})
    target_include_directories(rift-4.exe PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
    )
    target_link_libraries(rift-4.exe 
        OpenSSL::SSL OpenSSL::Crypto Threads::Threads
    )
    
    # Set output directories
    set_target_properties(rift-4_static PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    )
    set_target_properties(rift-4_shared PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    )
    set_target_properties(rift-4.exe PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    )
    
    # Stage-specific configuration
    # Bytecode generation with trust tagging
    target_compile_definitions(${stage_name}_static PRIVATE 
        RIFT_BYTECODE_GENERATION=1
        RIFT_TRUST_TAGGING=1
        RIFT_AEGIS_COMPLIANCE=1
    )
else()
    message(WARNING "No source files found for ${stage_name}")
    # Create placeholder executable to satisfy build requirements
    add_custom_target(${stage_name}_placeholder
        COMMAND ${CMAKE_COMMAND} -E echo "Placeholder target for ${stage_name}"
    )
endif()

# Testing framework
enable_testing()
file(GLOB_RECURSE TEST_SOURCES "tests/unit/*.c")
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    if(STAGE_SOURCES)
        add_executable(${stage_name}_${TEST_NAME} ${TEST_SOURCE})
        target_link_libraries(${stage_name}_${TEST_NAME} ${stage_name}_static)
        add_test(NAME ${stage_name}_${TEST_NAME} COMMAND ${stage_name}_${TEST_NAME})
    endif()
endforeach()

# Installation
if(STAGE_SOURCES)
    install(TARGETS ${stage_name}_static ${stage_name}_shared ${stage_name}.exe
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
    install(FILES ${STAGE_HEADERS} DESTINATION include/${stage_name}/core)
endif()

message(STATUS "${stage_name} configuration complete")
