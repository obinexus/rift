#!/bin/bash
# integration_test_runner.sh

# Configuration
BUILD_DIR="./build"
TEST_DIR="./integration"
RIFT_CLI="$BUILD_DIR/rift-cli"
TEST_LOG="$TEST_DIR/test_results.log"

# Ensure build directory exists
mkdir -p $BUILD_DIR

# Compile the project
echo "Compiling LibRift..."
cmake -B $BUILD_DIR .
cmake --build $BUILD_DIR

# Run tests
echo "Running integration tests..."
echo "Test run: $(date)" > $TEST_LOG

# Test 1: Basic pattern matching
echo "Test 1: Basic pattern matching" >> $TEST_LOG
$RIFT_CLI -pattern "a(b+)c" -input "abbbc" -lrift >> $TEST_LOG
if [ $? -eq 0 ]; then
    echo "Test 1: PASSED" >> $TEST_LOG
else
    echo "Test 1: FAILED" >> $TEST_LOG
fi

# More tests...

echo "Integration tests completed."