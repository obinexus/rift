// ===== src/cli/main.c =====
#include "rift1/core/rift_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

static void print_usage(const char* program_name) {
    printf("🔤 RIFT Stage 1 - AEGIS Parser\n");
    printf("===============================\n");
    printf("Usage: %s [options] <input_file>\n\n", program_name);
    printf("Options:\n");
    printf("  -h, --help              Show this help message\n");
    printf("  -v, --version           Show version information\n");
    printf("  -V, --verbose           Enable verbose output\n");
    printf("  -o, --output <file>     Specify output file (default: <input>.1)\n");
    printf("  -m, --mode <mode>       Set parsing mode (bottom-up|top-down)\n");
    printf("  -c, --config <file>     Use custom configuration file\n");
    printf("  -d, --output-dir <dir>  Set output directory\n");
    printf("  --debug                 Enable debug mode\n");
    printf("  --ast-print             Print AST to stdout\n");
    printf("\n");
    printf("Parsing Modes:\n");
    printf("  bottom-up              Shift-reduce parsing (default)\n");
    printf("  top-down               Recursive descent parsing\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s input.rift.0                    # Basic parsing\n", program_name);
    printf("  %s --verbose input.rift.0          # Verbose output\n", program_name);
    printf("  %s -o custom.rift.1 input.rift.0   # Custom output file\n", program_name);
    printf("  %s --mode top-down input.rift.0    # Top-down parsing\n", program_name);
    printf("\n");
    printf("🏗️  RIFT Architecture: AEGIS Pattern-Based Parser\n");
    printf("📚 OBINexus Computing - TokenMemory → Parser Pipeline\n");
    printf("🔗 Toolchain: riftlang.exe → .so.a → rift.exe → gosilang\n");
}

static void print_version(void) {
    printf("🔤 RIFT Stage 1 AEGIS Parser v1.0.0\n");
    printf("Automaton Engine for Generative Interpretation & Syntax\n");
    printf("OBINexus Computing - Governance-First Compilation Architecture\n");
    printf("Built: %s %s\n", __DATE__, __TIME__);
    printf("\n");
    printf("🏗️  Features: Regex token matching + dual parsing strategies\n");
    printf("🔗 Toolchain: riftlang.exe → .so.a → rift.exe → gosilang\n");
    printf("📋 Stage: 1 (AEGIS Pattern-Based Parsing)\n");
}

static RiftParseMode parse_mode_from_string(const char* mode_str) {
    if (!mode_str) return RIFT_PARSE_DEFAULT;
    
    if (strcmp(mode_str, "bottom-up") == 0) {
        return RIFT_PARSE_BOTTOM_UP;
    } else if (strcmp(mode_str, "top-down") == 0) {
        return RIFT_PARSE_TOP_DOWN;
    }
    
    return RIFT_PARSE_DEFAULT;
}

static char* generate_output_filename(const char* input_file) {
    if (!input_file) return NULL;
    
    const char* dot = strrchr(input_file, '.');
    size_t base_len = dot ? (size_t)(dot - input_file) : strlen(input_file);
    
    char* output_file = malloc(base_len + 8); // ".rift.1\0"
    if (!output_file) return NULL;
    
    strncpy(output_file, input_file, base_len);
    strcpy(output_file + base_len, ".rift.1");
    
    return output_file;
}

int main(int argc, char* argv[]) {
    // Configuration variables
    const char* input_file = NULL;
    const char* output_file = NULL;
    const char* config_file = NULL;
    const char* output_dir = NULL;
    const char* mode_str = "bottom-up";
    bool verbose = false;
    bool debug = false;
    bool ast_print = false;
    bool show_help = false;
    bool show_version = false;

    // Long option definitions
    static struct option long_options[] = {
        {"help",       no_argument,       0, 'h'},
        {"version",    no_argument,       0, 'v'},
        {"verbose",    no_argument,       0, 'V'},
        {"output",     required_argument, 0, 'o'},
        {"mode",       required_argument, 0, 'm'},
        {"config",     required_argument, 0, 'c'},
        {"output-dir", required_argument, 0, 'd'},
        {"debug",      no_argument,       0, 1000},
        {"ast-print",  no_argument,       0, 1001},
        {0, 0, 0, 0}
    };

    // Parse command line arguments
    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "hvVo:m:c:d:", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                show_help = true;
                break;
            case 'v':
                show_version = true;
                break;
            case 'V':
                verbose = true;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'm':
                mode_str = optarg;
                break;
            case 'c':
                config_file = optarg;
                break;
            case 'd':
                output_dir = optarg;
                break;
            case 1000:
                debug = true;
                break;
            case 1001:
                ast_print = true;
                break;
            case '?':
                fprintf(stderr, "Use --help for usage information.\n");
                return EXIT_FAILURE;
            default:
                break;
        }
    }

    // Handle help and version
    if (show_help) {
        print_usage(argv[0]);
        return EXIT_SUCCESS;
    }

    if (show_version) {
        print_version();
        return EXIT_SUCCESS;
    }

    // Get input file
    if (optind < argc) {
        input_file = argv[optind];
    } else {
        fprintf(stderr, "Error: Input file required\n");
        fprintf(stderr, "Use %s --help for usage information.\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Generate output filename if not provided
    char* generated_output = NULL;
    if (!output_file) {
        generated_output = generate_output_filename(input_file);
        output_file = generated_output;
        if (!output_file) {
            fprintf(stderr, "Error: Failed to generate output filename\n");
            return EXIT_FAILURE;
        }
    }

    // Parse mode
    RiftParseMode parse_mode = parse_mode_from_string(mode_str);
    if (parse_mode == RIFT_PARSE_DEFAULT) {
        fprintf(stderr, "Warning: Unknown parse mode '%s', defaulting to bottom-up\n", mode_str);
        parse_mode = RIFT_PARSE_BOTTOM_UP;
    }

    if (verbose) {
        printf("🔤 RIFT Stage 1 - Starting AEGIS Parsing\n");
        printf("Input file: %s\n", input_file);
        printf("Output file: %s\n", output_file);
        printf("Parse mode: %s\n", mode_str);
        if (config_file) printf("Config file: %s\n", config_file);
        if (output_dir) printf("Output directory: %s\n", output_dir);
        printf("\n");
    }

    // Create parse engine
    Rift1ParseEngine* engine = rift1_engine_create();
    if (!engine) {
        fprintf(stderr, "Error: Failed to create RIFT Stage 1 parse engine\n");
        free(generated_output);
        return EXIT_FAILURE;
    }

    // Set parse mode
    RiftResult mode_result = rift1_engine_set_parse_mode(engine, parse_mode);
    if (mode_result != RIFT_SUCCESS) {
        fprintf(stderr, "Error: Failed to set parse mode: %s\n", 
                rift_result_to_string(mode_result));
        rift1_engine_destroy(engine);
        free(generated_output);
        return EXIT_FAILURE;
    }

    // Process the file
    if (verbose) {
        printf("🔍 Processing Stage 0 → Stage 1...\n");
    }

    RiftResult result = rift1_process(engine, input_file, output_file);

    if (result == RIFT_SUCCESS) {
        if (verbose) {
            printf("✅ RIFT Stage 1 processing complete\n");
            printf("📄 Output written to: %s\n", output_file);
        }

        // Print AST if requested
        if (ast_print && engine->parse_tree) {
            printf("\n🌳 Abstract Syntax Tree:\n");
            printf("========================\n");
            rift_ast_print(engine->parse_tree, 0);
        }

        if (verbose) {
            printf("\n🏗️  AEGIS Architecture: Pattern-based parsing complete\n");
            printf("🔗 Ready for Stage 3 (AST → Bytecode)\n");
        }
    } else {
        fprintf(stderr, "Error: RIFT Stage 1 processing failed: %s\n", 
                rift_result_to_string(result));
        rift1_engine_destroy(engine);
        free(generated_output);
        return EXIT_FAILURE;
    }

    // Cleanup
    rift1_engine_destroy(engine);
    free(generated_output);

    return EXIT_SUCCESS;
}

// ===== src/cli/cli_args.c =====
#include "rift1/core/rift_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RiftConfig* rift_config_create(void) {
    RiftConfig* config = calloc(1, sizeof(RiftConfig));
    if (!config) return NULL;

    config->parse_mode = RIFT_PARSE_BOTTOM_UP;
    config->verbose = false;
    config->debug = false;

    return config;
}

void rift_config_destroy(RiftConfig* config) {
    if (!config) return;

    free(config->output_directory);
    free(config->stage_name);
    free(config);
}

RiftResult rift_config_load_file(RiftConfig* config, const char* filename) {
    if (!config || !filename) return RIFT_ERROR_NULL_POINTER;

    FILE* file = fopen(filename, "r");
    if (!file) return RIFT_ERROR_FILE_NOT_FOUND;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;

        // Parse key=value pairs
        char* equals = strchr(line, '=');
        if (!equals) continue;

        *equals = '\0';
        char* key = line;
        char* value = equals + 1;

        // Trim newline
        char* newline = strchr(value, '\n');
        if (newline) *newline = '\0';

        // Process configuration options
        if (strcmp(key, "parse_mode") == 0) {
            if (strcmp(value, "top-down") == 0) {
                config->parse_mode = RIFT_PARSE_TOP_DOWN;
            } else if (strcmp(value, "bottom-up") == 0) {
                config->parse_mode = RIFT_PARSE_BOTTOM_UP;
            }
        } else if (strcmp(key, "verbose") == 0) {
            config->verbose = (strcmp(value, "true") == 0);
        } else if (strcmp(key, "debug") == 0) {
            config->debug = (strcmp(value, "true") == 0);
        }
    }

    fclose(file);
    return RIFT_SUCCESS;
}

// ===== src/cli/cli_interface.c =====
#include "rift1/core/rift_types.h"
#include <stdio.h>
#include <stdlib.h>

void rift_print_banner(void) {
    printf("╔══════════════════════════════════════╗\n");
    printf("║    RIFT Stage 1 - AEGIS Parser      ║\n");
    printf("║  OBINexus Computing Architecture     ║\n");
    printf("╚══════════════════════════════════════╝\n");
    printf("\n");
    printf("🏗️  Architecture: Pattern-Based Parsing\n");
    printf("🔗 Toolchain: riftlang.exe → .so.a → rift.exe → gosilang\n");
    printf("📋 Features: Regex token matching + dual parsing strategies\n");
    printf("\n");
}

void rift_print_stage_info(void) {
    printf("📋 RIFT Stage 1 Information:\n");
    printf("===========================\n");
    printf("Stage: 1 (AEGIS Pattern-Based Parsing)\n");
    printf("Input: .rift.0 files (tokenized)\n");
    printf("Output: .rift.1 files (parsed with AST)\n");
    printf("Engine: AEGIS (Automaton Engine for Generative Interpretation & Syntax)\n");
    printf("Strategies: Bottom-up (shift-reduce) and Top-down (recursive descent)\n");
    printf("Token Matching: POSIX regex patterns\n");
    printf("\n");
    printf("🔗 Pipeline Integration:\n");
    printf("  Stage 0 → Stage 1: TokenMemory → Parser Pipeline\n");
    printf("  Stage 1 → Stage 3: AST → Bytecode Generation\n");
    printf("\n");
}

void rift_print_progress(const char* message, int percentage) {
    printf("\r🔍 %s... [", message);
    
    int bar_length = 30;
    int filled = (percentage * bar_length) / 100;
    
    for (int i = 0; i < bar_length; i++) {
        if (i < filled) {
            printf("█");
        } else {
            printf("░");
        }
    }
    
    printf("] %d%%", percentage);
    fflush(stdout);
    
    if (percentage >= 100) {
        printf("\n");
    }
}

void rift_print_error(const char* error_msg, RiftResult error_code) {
    printf("❌ Error: %s\n", error_msg);
    printf("   Code: %s (%d)\n", rift_result_to_string(error_code), error_code);
    printf("\n");
    printf("💡 Troubleshooting:\n");
    
    switch (error_code) {
        case RIFT_ERROR_FILE_NOT_FOUND:
            printf("   - Check if the input file exists\n");
            printf("   - Verify file permissions\n");
            printf("   - Ensure .rift.0 stage was completed\n");
            break;
        case RIFT_ERROR_PARSE_FAILED:
            printf("   - Check input file syntax\n");
            printf("   - Try different parsing mode (--mode top-down|bottom-up)\n");
            printf("   - Enable debug mode (--debug) for more details\n");
            break;
        case RIFT_ERROR_MEMORY_ALLOCATION:
            printf("   - Check available system memory\n");
            printf("   - Consider processing smaller files\n");
            break;
        default:
            printf("   - Use --verbose for more information\n");
            printf("   - Check the RIFT documentation\n");
            break;
    }
}

void rift_print_success_summary(const char* input_file, const char* output_file, RiftParseMode mode) {
    printf("✅ RIFT Stage 1 Processing Complete\n");
    printf("===================================\n");
    printf("Input: %s\n", input_file);
    printf("Output: %s\n", output_file);
    printf("Mode: %s parsing\n", 
           mode == RIFT_PARSE_TOP_DOWN ? "Top-down" : "Bottom-up");
    printf("\n");
    printf("🏗️  AEGIS Architecture: Pattern-based parsing successful\n");
    printf("🔗 Ready for Stage 3 (AST → Bytecode Generation)\n");
    printf("\n");
}