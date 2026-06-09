#!/bin/bash
# benchmark_runner.sh

# Configuration
LIBRIFT_CLI="./build/rift-cli"
YACC_EXEC="yacc"
TEST_CASES_DIR="./benchmark/test_cases"
RESULTS_DIR="./benchmark/results"
ITERATIONS=5

# Ensure directories exist
mkdir -p $RESULTS_DIR

# Generate test cases if they don't exist
if [ ! -d "$TEST_CASES_DIR" ]; then
    echo "Generating test cases..."
    mkdir -p $TEST_CASES_DIR
    ./benchmark/generate_test_cases.sh
fi

# Run benchmarks
echo "Running benchmarks..."

for test_case in $TEST_CASES_DIR/*.in; do
    base_name=$(basename $test_case .in)
    
    # LibRift benchmark
    echo "Benchmarking LibRift on $base_name"
    librift_time=0
    for i in $(seq 1 $ITERATIONS); do
        start_time=$(date +%s.%N)
        $LIBRIFT_CLI -pattern "$(cat $TEST_CASES_DIR/$base_name.pattern)" -input "$(cat $test_case)" -lrift > /dev/null
        end_time=$(date +%s.%N)
        execution_time=$(echo "$end_time - $start_time" | bc)
        librift_time=$(echo "$librift_time + $execution_time" | bc)
    done
    librift_avg=$(echo "$librift_time / $ITERATIONS" | bc -l)
    
    # YACC benchmark
    # Similar implementation for YACC
    
    # Record results
    echo "$base_name,$librift_avg,$yacc_avg" >> $RESULTS_DIR/timing_results.csv
done

echo "Benchmarks completed. Results in $RESULTS_DIR/timing_results.csv"