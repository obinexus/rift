// CSV Benchmark - Thread-safe parsing inspired by format_tokenizer_benchmark.c
#include "../../../include/rift.h"
#include <time.h>

// Token type definitions from benchmark
typedef enum {
    TOKEN_CSV_FIELD = 100,
    TOKEN_CSV_QUOTED_FIELD,
    TOKEN_CSV_DELIMITER,
    TOKEN_CSV_NEWLINE,
    TOKEN_CSV_EOF
} csv_token_type_t;

// Enhanced CSV patterns using R"" syntax
static const char* csv_comprehensive_pattern = R"(
    # Comprehensive CSV Pattern with all cases
    (?:
        # Case 1: Quoted field with potential escaped quotes
        "([^"]*(?:""[^"]*)*)"     |
        
        # Case 2: Unquoted field (no special characters)
        ([^,\r\n"]+)              |
        
        # Case 3: Empty field (just delimiter)
        (?=,|$)()                 |
        
        # Case 4: Delimiter
        (,)                       |
        
        # Case 5: Newline (various formats)
        (\r\n|\r|\n)
    )
)[t]";  // Top-down comprehensive parsing

void benchmark_csv_parsing(const char* csv_file, int iterations) {
    printf("⚡ CSV Parsing Benchmark\n");
    printf("=======================\n");
    printf("📁 File: %s\n", csv_file);
    printf("🔄 Iterations: %d\n\n", iterations);
    
    // Read file once
    FILE* file = fopen(csv_file, "r");
    if (!file) {
        printf("❌ Could not open file: %s\n", csv_file);
        return;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = malloc(file_size + 1);
    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);
    
    // Setup parsing infrastructure
    RiftAutomaton* automaton = rift_automaton_create();
    RiftState* csv_state = rift_automaton_add_state_with_type(
        automaton, csv_comprehensive_pattern, "CSV_TOKEN", true);
    
    if (!csv_state) {
        printf("❌ Failed to create CSV parsing state\n");
        free(content);
        rift_automaton_destroy(automaton);
        return;
    }
    
    // Benchmark the parsing
    clock_t start_time = clock();
    size_t total_tokens = 0;
    
    for (int i = 0; i < iterations; i++) {
        RiftEngine* engine = rift_engine_create();
        engine->automaton = automaton;
        
        rift_engine_process_input(engine, content);
        total_tokens += engine->token_count;
        
        rift_engine_destroy(engine);
    }
    
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    // Calculate statistics
    double avg_tokens_per_iteration = (double)total_tokens / iterations;
    double tokens_per_second = total_tokens / elapsed_time;
    double throughput_mb_per_sec = (file_size * iterations) / (1024.0 * 1024.0) / elapsed_time;
    
    printf("📊 Benchmark Results:\n");
    printf("=====================\n");
    printf("⏱️  Total time: %.4f seconds\n", elapsed_time);
    printf("📊 Total tokens: %zu\n", total_tokens);
    printf("📈 Avg tokens/iteration: %.1f\n", avg_tokens_per_iteration);
    printf("⚡ Tokens/second: %.1f\n", tokens_per_second);
    printf("🚀 Throughput: %.2f MB/sec\n", throughput_mb_per_sec);
    
    free(content);
    rift_automaton_destroy(automaton);
}

void analyze_csv_structure(const char* csv_file) {
    printf("🔍 CSV Structure Analysis\n");
    printf("=========================\n");
    
    FILE* file = fopen(csv_file, "r");
    if (!file) return;
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = malloc(file_size + 1);
    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);
    
    // Analyze structure
    size_t lines = 0;
    size_t quoted_fields = 0;
    size_t unquoted_fields = 0;
    size_t empty_fields = 0;
    size_t escaped_quotes = 0;
    
    for (char* p = content; *p; p++) {
        if (*p == '\n') lines++;
        if (*p == '"' && p[1] == '"') escaped_quotes++;
    }
    
    printf("📄 Lines: %zu\n", lines + 1); // +1 for last line without newline
    printf("📊 File size: %ld bytes\n", file_size);
    printf("🔤 Escaped quotes found: %zu\n", escaped_quotes);
    
    // Estimate field counts (rough)
    size_t commas = 0;
    for (char* p = content; *p; p++) {
        if (*p == ',') commas++;
    }
    printf("📈 Estimated fields: %zu\n", commas + lines + 1);
    
    free(content);
    printf("\n");
}

int main(int argc, char* argv[]) {
    printf("⚡ RIFT CSV Benchmark Suite\n");
    printf("===========================\n");
    printf("🏗️  R\"\" syntax performance testing\n");
    printf("📚 Based on LibRift format_tokenizer_benchmark\n\n");
    
    const char* test_files[] = {
        "examples/data/simple.csv",
        "examples/data/complex.csv"
    };
    
    for (size_t i = 0; i < 2; i++) {
        analyze_csv_structure(test_files[i]);
        benchmark_csv_parsing(test_files[i], 100);
        printf("\n");
    }
    
    return 0;
}
