#!/bin/bash
# Run Tests Script for LibRift
# -----------------------------------------------------------------------------

# Define directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Go to build directory
cd "$BUILD_DIR" || { echo "Failed to change to build directory"; exit 1; }

# Configure if CMakeCache.txt doesn't exist
if [ ! -f "CMakeCache.txt" ]; then
    echo "Configuring project..."
    cmake "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=Debug
fi

# Build the project
echo "Building project..."
cmake --build .

# Function to run tests with different options
run_tests() {
    local test_type=$1
    local test_pattern=$2
    
    echo "Running $test_type tests..."
    ctest --output-on-failure $test_pattern
    return $?
}
set(CTEST_OUTPUT_ON_FAILURE ON)
set(CTEST_USE_LAUNCHERS ON)

# Generate test summary
add_custom_target(test_summary
# Parse command line arguments
if [ $# -eq 0 ]; then
    # Default: run all tests
    echo "----------------------------------------"
    echo "LibRift Test Summary"
    echo "----------------------------------------"
    run_tests "all" ""
    exit_code=$?
else
    case "$1" in
        "unit")
            run_tests "unit" "-R ^unit_"
            exit_code=$?
            ;;
        "integration")
            run_tests "integration" "-R ^integration_"
            exit_code=$?
            ;;
        "verbose")
            echo "Running tests with verbose output..."
            ctest -V
            exit_code=$?
            ;;
        "summary")
            echo "----------------------------------------"
            echo "LibRift Test Summary"
# Generate coverage report if requested
if [ "$1" == "coverage" ]; then
    # Check if lcov and gcov are installed
    if command -v lcov >/dev/null 2>&1 && command -v gcov >/dev/null 2>&1 && command -v genhtml >/dev/null 2>&1; then
        echo "Generating code coverage report..."
        
        # Clean coverage data
        lcov --directory "$BUILD_DIR" --zerocounters
        
        # Run all tests
        ctest --no-tests=error
        
        # Capture coverage data
        lcov --directory "$BUILD_DIR" --capture --output-file "$BUILD_DIR/coverage.info"
        
        # Filter out system files and test files
        lcov --remove "$BUILD_DIR/coverage.info" '/usr/*' '*/tests/*' '*/external/*' '*/googletest/*' --output-file "$BUILD_DIR/coverage.info.filtered"
        
        # Generate HTML report
        genhtml "$BUILD_DIR/coverage.info.filtered" --output-directory "$BUILD_DIR/coverage_report"
        
        # Display results
        echo "Coverage report generated: $BUILD_DIR/coverage_report/index.html"
    else
        echo "Error: lcov, gcov, or genhtml not found. Cannot generate coverage report."
        exit_code=1
    fi
fi

# Return to original directory
cd "$SCRIPT_DIR" || { echo "Failed to return to script directory"; exit 1; }

# Exit with the status from the tests
exit $exit_code     # Display results
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated: ${CMAKE_BINARY_DIR}/coverage_report/index.html"
            
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report..."
        )
    else()
        message(WARNING "Code coverage tools not found. Coverage reporting disabled.")
        set(CODE_COVERAGE OFF)
    endif()
endif()