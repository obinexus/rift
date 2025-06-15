#include "../../include/rift.h"
#include <getopt.h>

static void print_help() {
    printf("🔤 RIFT Stage 0 - Tokenizer\n");
    printf("Usage: rift0 [options] <input_file>\n");
    printf("Options:\n");
    printf("  -h, --help     Show help\n");
    printf("  -v, --version  Show version\n");
    printf("  --verbose      Enable verbose output\n");
    printf("Examples:\n");
    printf("  rift0 input.rift\n");
    printf("  rift0 --verbose input.rift\n");
}

int main(int argc, char* argv[]) {
    bool verbose = false;
    bool show_help = false;
    bool show_version = false;
    
    // Simple argument parsing
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            show_help = true;
        } else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            show_version = true;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        }
    }
    
    if (show_help) {
        print_help();
        return 0;
    }
    
    if (show_version) {
        rift_print_version();
        return 0;
    }
    
    // Find input file (first non-option argument)
    const char* input_file = NULL;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            input_file = argv[i];
            break;
        }
    }
    
    if (!input_file) {
        printf("❌ Error: No input file specified\n");
        printf("Try 'rift0 --help' for usage information.\n");
        return 1;
    }
    
    // Create config
    RiftConfig* config = rift_config_create();
    if (!config) {
        fprintf(stderr, "❌ Failed to create configuration\n");
        return 1;
    }
    
    config->verbose = verbose;
    
    // Generate output filename
    char output_file[1024];
    snprintf(output_file, sizeof(output_file), "%s.0", input_file);
    
    if (verbose) {
        printf("🔤 RIFT Stage 0 - Tokenization\n");
        printf("Input:  %s\n", input_file);
        printf("Output: %s\n", output_file);
    }
    
    // Process the file
    RiftResult result = rift_process_file(input_file, output_file, config);
    
    if (result == RIFT_SUCCESS) {
        printf("✅ Stage 0 processing complete\n");
    } else {
        printf("❌ Processing failed: %s\n", rift_result_string(result));
    }
    
    rift_config_destroy(config);
    return (result == RIFT_SUCCESS) ? 0 : 1;
}
