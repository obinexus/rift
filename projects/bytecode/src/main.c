// Example usage of the DSL framework
#include <stdio.h>
#include <stdbool.h>
#include "rift_dsl.h"

int main(int argc, char* argv[]) {
    // Parse the .rift file
    void* dsl_handle = rift_dsl_load_file("test_bytecode_serialization.rift");
    if (!dsl_handle) {
        fprintf(stderr, "Failed to parse .rift file\n");
        return 1;
    }

    // Compile patterns to bytecode
    void* compilation = rift_dsl_compile(dsl_handle);
    if (!compilation) {
        fprintf(stderr, "Compilation failed: %s\n", 
                rift_dsl_get_error_message(dsl_handle));
        rift_dsl_free(dsl_handle);
        return 1;
    }

    // Run all test cases
    void* test_report = rift_dsl_test_run(dsl_handle);
    
    // Obtain the report
    char report_buffer[4096];
    rift_dsl_test_get_report_text(test_report, report_buffer, sizeof(report_buffer));
    
    // Print results
    printf("%s\n", report_buffer);
    
    // Check if all tests passed
    bool all_passed = rift_dsl_test_all_passed(test_report);
    
    // Clean up
    rift_dsl_test_report_free(test_report);
    rift_dsl_free_compilation(compilation);
    rift_dsl_free(dsl_handle);
    
    return all_passed ? 0 : 1;
}