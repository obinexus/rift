#!/bin/bash

# LibRift Test Module Generation Script
# Version: 1.0
# Purpose: Systematically create test modules for different components of LibRift

# Configuration Variables
PROJECT_ROOT=$(git rev-parse --show-toplevel 2>/dev/null || echo ".")
TESTS_DIR="${PROJECT_ROOT}/tests"
INCLUDE_DIR="${PROJECT_ROOT}/include"

# Color Codes for Enhanced Readability
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Logging Function
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to generate test header
generate_test_header() {
    local module_name="$1"
    local header_path="$2"
    
    cat > "$header_path" << EOL
#ifndef LIBRIFT_TEST_${module_name^^}_H
#define LIBRIFT_TEST_${module_name^^}_H

#include <check.h>

/* Test Suite for ${module_name} Module */
Suite * ${module_name}_test_suite(void);

#endif /* LIBRIFT_TEST_${module_name^^}_H */
EOL
}

# Function to generate test implementation
generate_test_implementation() {
    local module_name="$1"
    local impl_path="$2"
    
    cat > "$impl_path" << EOL
#include <check.h>
#include "test_${module_name}.h"
#include "librift/core/regex/${module_name}.h"  // Adjust include path as needed

/* Basic Test Case Template */
START_TEST(test_${module_name}_initialization)
{
    /* TODO: Implement initialization test */
    ck_assert(1);  // Placeholder assertion
}
END_TEST

/* Test Suite Creation */
Suite * ${module_name}_test_suite(void) {
    Suite *s = suite_create("${module_name^} Module");
    
    TCase *tc_core = tcase_create("Core Functionality");
    tcase_add_test(tc_core, test_${module_name}_initialization);
    suite_add_tcase(s, tc_core);
    
    return s;
}

/* Main Test Runner (Optional, can be separate) */
int main(void) {
    Suite *suite = ${module_name}_test_suite();
    SRunner *runner = srunner_create(suite);
    
    srunner_run_all(runner, CK_NORMAL);
    int number_failed = srunner_ntests_failed(runner);
    
    srunner_free(runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
EOL
}

# Main Function to Create Test Modules
create_test_module() {
    local module_name="$1"
    local test_type="${2:-unit}"
    
    # Determine directories
    local test_dir="${TESTS_DIR}/${test_type}"
    local module_test_dir="${test_dir}/${module_name}"
    
    # Create directories if they don't exist
    mkdir -p "$module_test_dir"
    
    # Generate test header
    local header_path="${module_test_dir}/test_${module_name}.h"
    generate_test_header "$module_name" "$header_path"
    log_info "Generated test header: $header_path"
    
    # Generate test implementation
    local impl_path="${module_test_dir}/test_${module_name}.c"
    generate_test_implementation "$module_name" "$impl_path"
    log_info "Generated test implementation: $impl_path"
}

# Help/Usage Function
usage() {
    echo "Usage: $0 [OPTIONS] MODULE_NAME"
    echo
    echo "Options:"
    echo "  -t, --type     Test type (default: unit, alternatives: integration, performance)"
    echo "  -h, --help     Display this help message"
    echo
    echo "Example: $0 automaton"
    echo "         $0 -t integration matcher"
}

# Argument Parsing
if [[ $# -eq 0 ]]; then
    usage
    exit 1
fi

# Parse command-line arguments
TEMP=$(getopt -o t:h --long type:,help -n "$0" -- "$@")
if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi
eval set -- "$TEMP"

test_type="unit"
while true; do
    case "$1" in
        -t | --type )
            test_type="$2"
            shift 2
            ;;
        -h | --help )
            usage
            exit 0
            ;;
        -- ) 
            shift
            break
            ;;
        * ) 
            break
            ;;
    esac
done

# Validate remaining argument
if [[ $# -ne 1 ]]; then
    log_error "Exactly one module name is required."
    usage
    exit 1
fi

# Execute module creation
create_test_module "$1" "$test_type"

log_info "Test module for '$1' created successfully in ${test_type} tests."
