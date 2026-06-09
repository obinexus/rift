# LibRift Test Configuration
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.14)

# Set up testing
enable_testing()

# Set module directory path for custom modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Add integration tests if directory exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/integration")
    file(GLOB_RECURSE INTEGRATION_TEST_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/integration/*.c"
    )
    list(APPEND TEST_SOURCES ${INTEGRATION_TEST_SOURCES})
endif()

# Create test utility library if utils directory exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/utils")
    file(GLOB TEST_UTILS_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.c"
    )
    
    add_library(test_utils STATIC ${TEST_UTILS_SOURCES})
    target_include_directories(test_utils PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
    )
    target_link_libraries(test_utils librift_core ${CHECK_LIBRARIES})
endif()

# Create test executable
add_executable(test_suite ${TEST_SOURCES})
target_link_libraries(test_suite 
    librift_core 
    ${CHECK_LIBRARIES}
    $<TARGET_NAME_IF_EXISTS:test_utils>
)

# Add the test
add_test(NAME LibRiftTests COMMAND test_suite)
)

# Define test sources
file(GLOB_RECURSE TEST_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/*.c"
)

# Include unit and integration test directories if they exist
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/unit)
    add_subdirectory(unit)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/integration)
    add_subdirectory(integration)
endif()

# Set up a target to run all tests
add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMENT "Running all LibRift tests..."
)

# Set up individual targets for running specific test categories
add_custom_target(run_unit_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -R "^unit_" --output-on-failure
    COMMENT "Running unit tests..."
)

add_custom_target(run_integration_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -R "^integration_" --output-on-failure
    COMMENT "Running integration tests..."
)

# Configure CTest output format
set(CTEST_OUTPUT_ON_FAILURE ON)
set(CTEST_USE_LAUNCHERS ON)

# Generate test summary
add_custom_target(test_summary
    COMMAND ${CMAKE_COMMAND} -E echo "----------------------------------------"
    COMMAND ${CMAKE_COMMAND} -E echo "LibRift Test Summary"
    COMMAND ${CMAKE_COMMAND} -E echo "----------------------------------------"
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --no-tests=error
    COMMENT "Generating test summary..."
)

# Add target for running tests with verbose output
add_custom_target(run_tests_verbose
    COMMAND ${CMAKE_CTEST_COMMAND} -V
    COMMENT "Running all tests with verbose output..."
)

# XML report generation for CI integration if needed
find_program(XSLTPROC_EXECUTABLE xsltproc)
if(XSLTPROC_EXECUTABLE)
    add_custom_target(generate_test_report
        COMMAND ${CMAKE_CTEST_COMMAND} -T Test --no-tests=error
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/test_reports
        COMMAND ${XSLTPROC_EXECUTABLE} ${CMAKE_SOURCE_DIR}/cmake/ctest2junit.xsl ${CMAKE_BINARY_DIR}/Testing/*/Test.xml > ${CMAKE_BINARY_DIR}/test_reports/junit-results.xml
        COMMENT "Generating JUnit-style XML test report..."
    )
endif()

# Setup code coverage for GCC/Clang if requested
option(CODE_COVERAGE "Enable code coverage reporting" OFF)
if(CODE_COVERAGE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Find required tools
    find_program(GCOV_PATH gcov)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    
    if(GCOV_PATH AND LCOV_PATH AND GENHTML_PATH)
        message(STATUS "Code coverage enabled")
        
        # Add coverage flags to compiler
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
        
        # Add custom target for generating coverage report
        add_custom_target(coverage
            # Clean coverage data
            COMMAND ${LCOV_PATH} --directory ${CMAKE_BINARY_DIR} --zerocounters
            
            # Run all tests
            COMMAND ${CMAKE_CTEST_COMMAND} --no-tests=error
            
            # Capture coverage data
            COMMAND ${LCOV_PATH} --directory ${CMAKE_BINARY_DIR} --capture --output-file ${CMAKE_BINARY_DIR}/coverage.info
            
            # Filter out system files and test files
            COMMAND ${LCOV_PATH} --remove ${CMAKE_BINARY_DIR}/coverage.info '/usr/*' '*/tests/*' '*/external/*' '*/googletest/*' --output-file ${CMAKE_BINARY_DIR}/coverage.info.filtered
            
            # Generate HTML report
            COMMAND ${GENHTML_PATH} ${CMAKE_BINARY_DIR}/coverage.info.filtered --output-directory ${CMAKE_BINARY_DIR}/coverage_report
            
            # Display results
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated: ${CMAKE_BINARY_DIR}/coverage_report/index.html"
            
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report..."
        )
    else()
        message(WARNING "Code coverage tools not found. Coverage reporting disabled.")
        set(CODE_COVERAGE OFF)
    endif()
endif()