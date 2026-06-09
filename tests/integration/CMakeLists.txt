# Integration Tests Configuration for LibRift
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
    ${CMAKE_CURRENT_SOURCE_DIR}/helpers
)

# Create a directory for test artifacts if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_artifacts)

# Create a helper library for integration tests if helper files exist
set(INTEGRATION_HELPER_SOURCES "")
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/helpers)
    file(GLOB INTEGRATION_HELPER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/helpers/*.c")
    if(INTEGRATION_HELPER_SOURCES)
        add_library(integration_test_helpers STATIC ${INTEGRATION_HELPER_SOURCES})
        target_link_libraries(integration_test_helpers
            PRIVATE
            librift_core
            librift_regex
        )
    endif()
endif()

# Define test categories
set(TEST_CATEGORIES
    parser_engine     # Tests integration between parser and engine
    automaton_matcher # Tests integration between automaton and matcher
    syntax_compiler   # Tests integration between syntax and compiler
    bytecode_engine   # Tests integration between bytecode and engine
    pattern_runtime   # Tests integration between pattern and runtime components
    full_pipeline     # Tests the full regex compilation and execution pipeline
    accessibility     # Tests accessibility features integration
)

# Helper macro for adding integration tests for a specific category
macro(add_integration_test_category CATEGORY)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CATEGORY})
        message(STATUS "Setting up integration tests for category: ${CATEGORY}")
        
        # Get all test files for this category
        file(GLOB TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${CATEGORY}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/${CATEGORY}/*.cpp")
        
        # Skip if no test files found
        if(TEST_FILES)
            # For each test file, create a test executable
            foreach(TEST_FILE ${TEST_FILES})
                # Extract the test name from the file name
                get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
                
                # Create the full test name
                set(FULL_TEST_NAME "integration_${CATEGORY}_${TEST_NAME}")
                
                # Add the test executable
                add_executable(${FULL_TEST_NAME} ${TEST_FILE})
                
                # Link with required libraries
                target_link_libraries(${FULL_TEST_NAME}
                    PRIVATE
                    rift # Link with combined library for integration tests
                    GTest::gtest
                    GTest::gtest_main
                )
                
                # If helper library exists, link with it
                if(TARGET integration_test_helpers)
                    target_link_libraries(${FULL_TEST_NAME} PRIVATE integration_test_helpers)
                endif()
                
                # Add the test to CTest
                add_test(
                    NAME ${FULL_TEST_NAME}
                    COMMAND ${FULL_TEST_NAME}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_artifacts
                )
                
                # Set test properties for timeouts and environment variables
                set_tests_properties(${FULL_TEST_NAME} PROPERTIES
                    TIMEOUT 60  # Longer timeout for integration tests
                    ENVIRONMENT "LIBRIFT_TEST_MODE=1;LIBRIFT_TEST_DATA_DIR=${CMAKE_CURRENT_SOURCE_DIR}/test_data"
                )
            endforeach()
        else()
            message(STATUS "No test files found for category: ${CATEGORY}")
        endif()
    endif()
endmacro()

# Add tests for each category
foreach(CATEGORY ${TEST_CATEGORIES})
    add_integration_test_category(${CATEGORY})
endforeach()

# Add data-driven integration tests if test data exists
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test_data)
    file(GLOB TEST_DATA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test_data/*.json" "${CMAKE_CURRENT_SOURCE_DIR}/test_data/*.txt")
    
    if(TEST_DATA_FILES AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/data_driven/data_driven_test.c)
        # Add data-driven test executable
        add_executable(integration_data_driven_test 
            ${CMAKE_CURRENT_SOURCE_DIR}/data_driven/data_driven_test.c)
        
        target_link_libraries(integration_data_driven_test
            PRIVATE
            rift
            GTest::gtest
            GTest::gtest_main
        )
        
        # For each test data file, add a test that uses the same executable with different data
        foreach(DATA_FILE ${TEST_DATA_FILES})
            get_filename_component(DATA_NAME ${DATA_FILE} NAME_WE)
            add_test(
                NAME integration_data_driven_${DATA_NAME}
                COMMAND integration_data_driven_test --data_file=${DATA_FILE}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_artifacts
            )
        endforeach()
    endif()
endif()

# Performance regression tests
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/performance)
    file(GLOB PERF_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/performance/*.c")
    
    if(PERF_TEST_FILES)
        foreach(PERF_TEST_FILE ${PERF_TEST_FILES})
            get_filename_component(PERF_TEST_NAME ${PERF_TEST_FILE} NAME_WE)
            add_executable(integration_perf_${PERF_TEST_NAME} ${PERF_TEST_FILE})
            
            target_link_libraries(integration_perf_${PERF_TEST_NAME}
                PRIVATE
                rift
                GTest::gtest
                GTest::gtest_main
            )
            
            # Add as a test but with "performance" label
            add_test(
                NAME integration_perf_${PERF_TEST_NAME}
                COMMAND integration_perf_${PERF_TEST_NAME}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_artifacts
            )
            
            # Set properties for performance tests
            set_tests_properties(integration_perf_${PERF_TEST_NAME} PROPERTIES
                LABELS "performance"
                TIMEOUT 120  # Longer timeout for performance tests
            )
        endforeach()
        
        # Add a custom target to run only performance tests
        add_custom_target(run_performance_tests
            COMMAND ${CMAKE_CTEST_COMMAND} -L performance --output-on-failure
            COMMENT "Running performance regression tests..."
        )
    endif()
endif()

# Custom test output formats
add_custom_target(integration_test_report
    COMMAND ${CMAKE_COMMAND} -E echo "Generating integration test report..."
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --timeout 120
    COMMENT "Running integration tests with detailed output..."
)