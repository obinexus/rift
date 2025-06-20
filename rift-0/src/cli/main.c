/**
 * =================================================================
 * main.c - RIFT-0 Command Line Interface
 * RIFT: RIFT Is a Flexible Translator
 * Component: CLI for tokenization testing and validation
 * OBINexus Computing Framework - Stage 0 Implementation
 * =================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include "rift-0/core/tokenizer_rules.h"
#include "rift-0/cli/cli_interface.h"

/* CLI Configuration Structure */
typedef struct {
    char* input_file;
    char* output_file;
    char* pattern;
    uint32_t flags;
    bool verbose;
    bool csv_output;
    bool qa_mode;
    bool benchmark_mode;
    int thread_count;
} CLIConfig;

/* Default configuration */
static CLIConfig g_config = {
    .input_file = NULL,
    .output_file = NULL,
    .pattern = NULL,
    .flags = 0,
    .verbose = false,
    .csv_output = false,
    .qa_mode = false,
    .benchmark_mode = false,
    .thread_count = 1
};

/**
 * Print usage information
 */
static void print_usage(const char* program_name) {
    printf("RIFT-0 Tokenizer - OBINexus Computing Framework\n");
    printf("Usage: %s [OPTIONS] [INPUT_FILE]\n\n", program_name);
    printf("Options:\n");
    printf("  -h, --help              Show this help message\n");
    printf("  -v, --verbose           Enable verbose output\n");
    printf("  -o, --output FILE       Output file (default: stdout)\n");
    printf("  -p, --pattern PATTERN   R\"\" pattern to test\n");
    printf("  -f, --flags FLAGS       DFA flags (g,m,i,t,b)\n");
    printf("  -c, --csv               Output in CSV format\n");
    printf("  -q, --qa                Run QA validation tests\n");
    printf("  -b, --benchmark         Run performance benchmarks\n");
    printf("  -t, --threads N         Number of threads for processing\n");
    printf("\nDFA Flags:\n");
    printf("  g  Global matching\n");
    printf("  m  Multiline mode\n");
    printf("  i  Case insensitive\n");
    printf("  t  Top-down processing\n");
    printf("  b  Bottom-up processing\n");
    printf("\nExamples:\n");
    printf("  %s -p 'R\"/[A-Z]+/gi\"' input.rift\n", program_name);
    printf("  %s -c -o tokens.csv source.rift\n", program_name);
    printf("  %s -q --verbose\n", program_name);
    printf("\nRIFT: Like if yacc and regex had a morally questionable but very fast child.\n");
}

/**
 * Parse DFA flags from string
 */
static uint32_t parse_flags(const char* flag_str) {
    uint32_t flags = 0;
    
    if (!flag_str) return flags;
    
    for (const char* p = flag_str; *p; p++) {
        switch (*p) {
            case 'g': flags |= DFA_FLAG_GLOBAL; break;
            case 'm': flags |= DFA_FLAG_MULTILINE; break;
            case 'i': flags |= DFA_FLAG_INSENSITIVE; break;
            case 't': flags |= DFA_FLAG_TOP_DOWN; break;
            case 'b': flags |= DFA_FLAG_BOTTOM_UP; break;
            default:
                fprintf(stderr, "Warning: Unknown flag '%c'\n", *p);
                break;
        }
    }
    
    return flags;
}

/**
 * Read file content into buffer
 */
static char* read_file_content(const char* filename, size_t* size) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s': %s\n", filename, strerror(errno));
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size < 0) {
        fprintf(stderr, "Error: Cannot determine file size for '%s'\n", filename);
        fclose(file);
        return NULL;
    }
    
    // Allocate buffer
    char* buffer = malloc(file_size + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Memory allocation failed for file '%s'\n", filename);
        fclose(file);
        return NULL;
    }
    
    // Read content
    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';
    
    fclose(file);
    
    if (size) *size = bytes_read;
    return buffer;
}

/**
 * Output token in CSV format
 */
static void output_token_csv(FILE* output, const TokenTriplet* token, 
                           const char* source_text, size_t sequence_id) {
    // Extract token text
    size_t start_pos = token->mem_ptr;
    size_t length = token->value;
    
    // Calculate line and column
    size_t line = 1, col = 1;
    for (size_t i = 0; i < start_pos && source_text[i]; i++) {
        if (source_text[i] == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }
    }
    
    // Get token type name
    const char* type_name = "UNKNOWN";
    switch (token->type) {
        case TOKEN_IDENTIFIER: type_name = "IDENTIFIER"; break;
        case TOKEN_KEYWORD: type_name = "KEYWORD"; break;
        case TOKEN_LITERAL_STRING: type_name = "LITERAL_STRING"; break;
        case TOKEN_LITERAL_NUMBER: type_name = "LITERAL_NUMBER"; break;
        case TOKEN_OPERATOR: type_name = "OPERATOR"; break;
        case TOKEN_DELIMITER: type_name = "DELIMITER"; break;
        case TOKEN_R_PATTERN: type_name = "R_PATTERN"; break;
        case TOKEN_NULL_KEYWORD: type_name = "NULL_KEYWORD"; break;
        case TOKEN_NIL_KEYWORD: type_name = "NIL_KEYWORD"; break;
        case TOKEN_WHITESPACE: type_name = "WHITESPACE"; break;
        case TOKEN_COMMENT: type_name = "COMMENT"; break;
        case TOKEN_EOF: type_name = "EOF"; break;
        case TOKEN_ERROR: type_name = "ERROR"; break;
    }
    
    // Extract token value
    char token_value[256] = {0};
    if (length > 0 && length < sizeof(token_value) - 1) {
        strncpy(token_value, source_text + start_pos, length);
        token_value[length] = '\0';
        
        // Escape CSV special characters
        for (char* p = token_value; *p; p++) {
            if (*p == '"') *p = '\''; // Simple escape
            if (*p == '\n') *p = ' ';
            if (*p == '\r') *p = ' ';
        }
    }
    
    fprintf(output, "%zu,%s,%s,%s,%zu,%zu,%zu,%u,%u\n",
            sequence_id, type_name, token_value, token_value,
            line, start_pos, start_pos + length,
            token->mem_ptr, token->value);
}

/**
 * Run tokenization on input
 */
static int run_tokenization(void) {
    const char* input_text = NULL;
    char* file_content = NULL;
    size_t content_size = 0;
    
    // Get input text
    if (g_config.input_file) {
        file_content = read_file_content(g_config.input_file, &content_size);
        if (!file_content) return -1;
        input_text = file_content;
    } else {
        // Read from stdin
        fprintf(stderr, "Reading from stdin... (Ctrl+D to end)\n");
        size_t buffer_size = 1024;
        file_content = malloc(buffer_size);
        if (!file_content) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            return -1;
        }
        
        size_t total_read = 0;
        int c;
        while ((c = getchar()) != EOF) {
            if (total_read >= buffer_size - 1) {
                buffer_size *= 2;
                char* new_buffer = realloc(file_content, buffer_size);
                if (!new_buffer) {
                    fprintf(stderr, "Error: Memory reallocation failed\n");
                    free(file_content);
                    return -1;
                }
                file_content = new_buffer;
            }
            file_content[total_read++] = c;
        }
        file_content[total_read] = '\0';
        input_text = file_content;
        content_size = total_read;
    }
    
    if (g_config.verbose) {
        printf("Input size: %zu bytes\n", content_size);
        printf("Processing with flags: 0x%02X\n", g_config.flags);
    }
    
    // Open output file
    FILE* output = stdout;
    if (g_config.output_file) {
        output = fopen(g_config.output_file, "w");
        if (!output) {
            fprintf(stderr, "Error: Cannot open output file '%s': %s\n", 
                   g_config.output_file, strerror(errno));
            free(file_content);
            return -1;
        }
    }
    
    // Tokenize
    const size_t MAX_TOKENS = 10000;
    TokenTriplet* tokens = malloc(MAX_TOKENS * sizeof(TokenTriplet));
    if (!tokens) {
        fprintf(stderr, "Error: Memory allocation failed for tokens\n");
        if (output != stdout) fclose(output);
        free(file_content);
        return -1;
    }
    
    size_t token_count = 0;
    int result = tokenize_source(input_text, tokens, MAX_TOKENS, &token_count);
    
    if (result != 0) {
        fprintf(stderr, "Error: Tokenization failed with code %d\n", result);
        free(tokens);
        if (output != stdout) fclose(output);
        free(file_content);
        return -1;
    }
    
    if (g_config.verbose) {
        printf("Generated %zu tokens\n", token_count);
    }
    
    // Output results
    if (g_config.csv_output) {
        // CSV header
        fprintf(output, "sequence_id,token_type,raw_value,processed_value,line,col_start,col_end,mem_ptr,value\n");
        
        for (size_t i = 0; i < token_count; i++) {
            output_token_csv(output, &tokens[i], input_text, i + 1);
        }
    } else {
        // Human-readable format
        fprintf(output, "RIFT-0 Tokenization Results\n");
        fprintf(output, "===========================\n\n");
        
        for (size_t i = 0; i < token_count; i++) {
            const TokenTriplet* token = &tokens[i];
            size_t start_pos = token->mem_ptr;
            size_t length = token->value;
            
            fprintf(output, "Token %zu:\n", i + 1);
            fprintf(output, "  Type: %u\n", token->type);
            fprintf(output, "  Position: %u\n", token->mem_ptr);
            fprintf(output, "  Length: %u\n", token->value);
            
            if (length > 0 && start_pos + length <= content_size) {
                fprintf(output, "  Text: \"");
                for (size_t j = 0; j < length && j < 50; j++) {
                    char c = input_text[start_pos + j];
                    if (c == '\n') fprintf(output, "\\n");
                    else if (c == '\t') fprintf(output, "\\t");
                    else if (c == '\r') fprintf(output, "\\r");
                    else fprintf(output, "%c", c);
                }
                if (length > 50) fprintf(output, "...");
                fprintf(output, "\"\n");
            }
            fprintf(output, "\n");
        }
    }
    
    // Cleanup
    free(tokens);
    if (output != stdout) fclose(output);
    free(file_content);
    
    return 0;
}

/**
 * Run QA validation tests
 */
static int run_qa_tests(void) {
    printf("RIFT-0 QA Validation Tests\n");
    printf("==========================\n\n");
    
    // Policy2 QA Matrix test cases
    struct {
        const char* input;
        TokenType expected_type;
        const char* category;
    } test_cases[] = {
        // True Positives - Valid input yields correct token
        {"identifier123", TOKEN_IDENTIFIER, "truePositive"},
        {"NULL", TOKEN_NULL_KEYWORD, "truePositive"},
        {"nil", TOKEN_NIL_KEYWORD, "truePositive"},
        {"42", TOKEN_LITERAL_NUMBER, "truePositive"},
        {"+", TOKEN_OPERATOR, "truePositive"},
        {"(", TOKEN_DELIMITER, "truePositive"},
        
        // True Negatives - Invalid input correctly rejected
        {"123abc", TOKEN_LITERAL_NUMBER, "trueNegative"},
        {"", TOKEN_IDENTIFIER, "trueNegative"},
        
        // Test end
        {NULL, TOKEN_UNKNOWN, NULL}
    };
    
    int passed = 0, failed = 0;
    
    for (int i = 0; test_cases[i].input != NULL; i++) {
        if (g_config.verbose) {
            printf("Test %d: %s (%s)\n", i + 1, test_cases[i].input, test_cases[i].category);
        }
        
        int result = policy2_qa_validate(test_cases[i].input, 
                                       test_cases[i].expected_type,
                                       test_cases[i].category);
        
        if (result == 0) {
            passed++;
            if (g_config.verbose) printf("  PASS\n");
        } else {
            failed++;
            printf("  FAIL: %s\n", test_cases[i].input);
        }
    }
    
    printf("\nQA Test Results:\n");
    printf("  Passed: %d\n", passed);
    printf("  Failed: %d\n", failed);
    printf("  Total:  %d\n", passed + failed);
    
    return (failed == 0) ? 0 : -1;
}

/**
 * Parse command line arguments
 */
static int parse_arguments(int argc, char* argv[]) {
    static struct option long_options[] = {
        {"help",      no_argument,       0, 'h'},
        {"verbose",   no_argument,       0, 'v'},
        {"output",    required_argument, 0, 'o'},
        {"pattern",   required_argument, 0, 'p'},
        {"flags",     required_argument, 0, 'f'},
        {"csv",       no_argument,       0, 'c'},
        {"qa",        no_argument,       0, 'q'},
        {"benchmark", no_argument,       0, 'b'},
        {"threads",   required_argument, 0, 't'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "hvo:p:f:cqbt:", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                print_usage(argv[0]);
                exit(0);
                break;
                
            case 'v':
                g_config.verbose = true;
                break;
                
            case 'o':
                g_config.output_file = strdup(optarg);
                break;
                
            case 'p':
                g_config.pattern = strdup(optarg);
                break;
                
            case 'f':
                g_config.flags = parse_flags(optarg);
                break;
                
            case 'c':
                g_config.csv_output = true;
                break;
                
            case 'q':
                g_config.qa_mode = true;
                break;
                
            case 'b':
                g_config.benchmark_mode = true;
                break;
                
            case 't':
                g_config.thread_count = atoi(optarg);
                if (g_config.thread_count < 1) {
                    fprintf(stderr, "Error: Thread count must be positive\n");
                    return -1;
                }
                break;
                
            case '?':
                fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
                return -1;
                
            default:
                return -1;
        }
    }
    
    // Get input file if provided
    if (optind < argc) {
        g_config.input_file = strdup(argv[optind]);
    }
    
    return 0;
}

/**
 * Main entry point
 */
int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (parse_arguments(argc, argv) != 0) {
        return EXIT_FAILURE;
    }
    
    if (g_config.verbose) {
        printf("RIFT-0 Tokenizer - OBINexus Computing Framework\n");
        printf("Initializing tokenizer rules engine...\n");
    }
    
    // Initialize tokenizer
    int init_result = init_tokenizer_rules();
    if (init_result != 0) {
        fprintf(stderr, "Error: Failed to initialize tokenizer rules engine\n");
        return EXIT_FAILURE;
    }
    
    int result = 0;
    
    // Run appropriate mode
    if (g_config.qa_mode) {
        result = run_qa_tests();
    } else if (g_config.benchmark_mode) {
        printf("Benchmark mode not yet implemented\n");
        result = -1;
    } else {
        result = run_tokenization();
    }
    
    // Cleanup
    cleanup_tokenizer_rules();
    
    if (g_config.input_file) free(g_config.input_file);
    if (g_config.output_file) free(g_config.output_file);
    if (g_config.pattern) free(g_config.pattern);
    
    return (result == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}