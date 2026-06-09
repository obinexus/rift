# Unit Tests Configuration for LibRift
# -----------------------------------------------------------------------------

# Require CMake 3.14 for improved CTest functionality
cmake_minimum_required(VERSION 3.14)

# Set module directory path for custom modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Find GoogleTest package
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Define include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Helper macro for adding unit tests for a specific module
macro(add_unit_test_module MODULE_NAME)
    # Check if the directory exists
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME})
        message(STATUS "Setting up unit tests for module: ${MODULE_NAME}")
        
        # Get all test files for this module
        file(GLOB TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}/*.cpp")
        
        # Skip if no test files found
        if(TEST_FILES)
            # For each test file, create a test executable
            foreach(TEST_FILE ${TEST_FILES})
                # Extract the test name from the file name
                get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
                
                # Create the full test name
                set(FULL_TEST_NAME "unit_${MODULE_NAME}_${TEST_NAME}")
                
                # Add the test executable
                add_executable(${FULL_TEST_NAME} ${TEST_FILE})
                
                # Link with required libraries
                target_link_libraries(${FULL_TEST_NAME}
                    PRIVATE
                    librift_core
                    librift_regex
                    GTest::gtest
                    GTest::gtest_main
                )
                
                # If there are module-specific dependencies, link them
                if(TARGET librift_${MODULE_NAME})
                    target_link_libraries(${FULL_TEST_NAME} PRIVATE librift_${MODULE_NAME})
                endif()
                
                # Add the test to CTest
                add_test(
                    NAME ${FULL_TEST_NAME}
                    COMMAND ${FULL_TEST_NAME}
                )
                
                # Set test properties for timeouts and environment variables
                set_tests_properties(${FULL_TEST_NAME} PROPERTIES
                    TIMEOUT 30  # 30 second timeout
                    ENVIRONMENT "LIBRIFT_TEST_MODE=1"
                )
            endforeach()
        else()
            message(STATUS "No test files found for module: ${MODULE_NAME}")
        endif()
    endif()
endmacro()

# Define all modules to be tested
set(MODULE_LIST
    automaton
    parser
    tokenizer
    engine
    memory
    errors
    runtime
    core
    syntax
    bytecode
    compiler
    accessibility
    cli
    visualizer
)

# Add tests for each module
foreach(MODULE ${MODULE_LIST})
    add_unit_test_module(${MODULE})
endforeach()

# Add any standalone test executables that require special handling
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/automaton/automaton_tests.c)
    add_executable(unit_automaton_tests ${CMAKE_CURRENT_SOURCE_DIR}/automaton/automaton_tests.c)
    target_link_libraries(unit_automaton_tests
        PRIVATE
        librift_core
        GTest::gtest
        GTest::gtest_main
    )
    add_test(
        NAME unit_automaton_tests
        COMMAND unit_automaton_tests
    )
endif()

# Memory leak detection for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        # For each test, add memory leak detection
        get_property(TEST_NAMES DIRECTORY PROPERTY TESTS)
        foreach(TEST_NAME ${TEST_NAMES})
            set_tests_properties(${TEST_NAME} PROPERTIES
                ENVIRONMENT "ASAN_OPTIONS=detect_leaks=1:leak_check=full:detect_stack_use_after_return=1"
            )
        endforeach()
    endif()
endif()

# Custom test output formats
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_target(unit_test_report
        COMMAND ${CMAKE_COMMAND} -E echo "Generating unit test report..."
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --schedule-random
        COMMENT "Running unit tests with detailed output..."
    )
endif()