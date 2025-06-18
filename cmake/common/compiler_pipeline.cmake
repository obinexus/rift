# compiler_pipeline.cmake
# RIFT Common Build Configuration and Utilities
# OBINexus Computing Framework - Technical Implementation

# RIFT version information
set(RIFT_VERSION_MAJOR 4)
set(RIFT_VERSION_MINOR 0)
set(RIFT_VERSION_PATCH 0)
set(RIFT_VERSION_STRING "4.0.0")

# AEGIS methodology compliance flags
set(RIFT_AEGIS_COMPLIANCE ON)
set(RIFT_ZERO_TRUST_GOVERNANCE ON)
set(RIFT_DUAL_MODE_PARSING ON)
set(RIFT_TRUST_TAGGING ON)

# Compiler compliance function
function(enforce_rift_compiler_standards TARGET_NAME)
    target_compile_options(${TARGET_NAME} PRIVATE
        -Wall -Wextra -Wpedantic -Werror
        -fstack-protector-strong
        -D_FORTIFY_SOURCE=2
    )
    
    target_compile_definitions(${TARGET_NAME} PRIVATE
        RIFT_VERSION_MAJOR=${RIFT_VERSION_MAJOR}
        RIFT_VERSION_MINOR=${RIFT_VERSION_MINOR}
        RIFT_VERSION_PATCH=${RIFT_VERSION_PATCH}
        RIFT_AEGIS_COMPLIANCE=1
        RIFT_ZERO_TRUST=1
    )
endfunction()

# RIFT stage creation macro with comprehensive configuration
macro(create_rift_stage STAGE_NAME STAGE_TYPE COMPONENT_NAME)
    message(STATUS "Configuring RIFT stage: ${STAGE_NAME} (${STAGE_TYPE})")
    
    # Collect source files
    file(GLOB_RECURSE STAGE_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.c"
    )
    
    file(GLOB_RECURSE STAGE_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/${STAGE_NAME}/core/*.h"
    )
    
    if(STAGE_SOURCES)
        # Create static library
        add_library(${STAGE_NAME}_static STATIC ${STAGE_SOURCES})
        target_include_directories(${STAGE_NAME}_static PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include
        )
        
        # Apply RIFT compiler standards
        enforce_rift_compiler_standards(${STAGE_NAME}_static)
        
        # Link dependencies
        target_link_libraries(${STAGE_NAME}_static 
            OpenSSL::SSL OpenSSL::Crypto Threads::Threads
        )
        
        # Create shared library
        add_library(${STAGE_NAME}_shared SHARED ${STAGE_SOURCES})
        target_include_directories(${STAGE_NAME}_shared PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include
        )
        enforce_rift_compiler_standards(${STAGE_NAME}_shared)
        target_link_libraries(${STAGE_NAME}_shared 
            OpenSSL::SSL OpenSSL::Crypto Threads::Threads
        )
        
        # Create executable
        add_executable(${STAGE_NAME}.exe ${STAGE_SOURCES})
        target_include_directories(${STAGE_NAME}.exe PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include
        )
        enforce_rift_compiler_standards(${STAGE_NAME}.exe)
        target_link_libraries(${STAGE_NAME}.exe 
            OpenSSL::SSL OpenSSL::Crypto Threads::Threads
        )
        
        # Set output directories
        set_target_properties(${STAGE_NAME}_static PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
        )
        set_target_properties(${STAGE_NAME}_shared PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
        )
        set_target_properties(${STAGE_NAME}.exe PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        )
        
        message(STATUS "  Static library: ${STAGE_NAME}_static")
        message(STATUS "  Shared library: ${STAGE_NAME}_shared") 
        message(STATUS "  Executable: ${STAGE_NAME}.exe")
    else()
        message(WARNING "No source files found for ${STAGE_NAME}")
    endif()
endmacro()

# Testing framework integration
function(add_rift_stage_tests STAGE_NAME)
    file(GLOB_RECURSE TEST_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/*.c"
    )
    
    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${STAGE_NAME}_${TEST_NAME} ${TEST_SOURCE})
        
        if(TARGET ${STAGE_NAME}_static)
            target_link_libraries(${STAGE_NAME}_${TEST_NAME} ${STAGE_NAME}_static)
            add_test(NAME ${STAGE_NAME}_${TEST_NAME} COMMAND ${STAGE_NAME}_${TEST_NAME})
            message(STATUS "  Test: ${STAGE_NAME}_${TEST_NAME}")
        endif()
    endforeach()
endfunction()

# Validation utilities
function(validate_rift_stage_structure STAGE_NAME)
    set(REQUIRED_DIRS
        "src/core"
        "include/${STAGE_NAME}/core"
        "tests/unit"
    )
    
    foreach(DIR ${REQUIRED_DIRS})
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}")
            message(WARNING "Missing required directory for ${STAGE_NAME}: ${DIR}")
        endif()
    endforeach()
endfunction()

message(STATUS "RIFT common pipeline configuration loaded")
