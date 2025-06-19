/*
 * =================================================================
 * RIFT Unified CLI Implementation - Main Entry Point
 * OBINexus Computing Framework - AEGIS Methodology Compliance
 * Technical Architecture: Systematic Waterfall Development
 * =================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <sys/stat.h>

// RIFT Core Framework Headers
#include <rift/core/common.h>
#include <rift/core/governance.h>
#include <rift/cli/commands.h>
#include <rift/cli/config.h>

// Stage-specific headers for unified access
#include <rift/core/tokenizer/tokenizer.h>
#include <rift/core/parser/parser.h>
#include <rift/core/semantic/analyzer.h>
#include <rift/core/validator/validator.h>
#include <rift/core/codegen/generator.h>
#include <rift/core/verifier/verifier.h>
#include <rift/core/emitter/emitter.h>

// AEGIS Governance Implementation
#include <rift/governance/policy.h>
#include <rift/governance/validation.h>
#include <rift/governance/config.h>

// =================================================================
// GLOBAL CONFIGURATION AND STATE
// =================================================================

// CLI configuration structure
typedef struct {
    char *config_file;
    char *input_file;
    char *output_file;
    bool verbose_mode;
    bool debug_mode;
    bool validate_only;
    bool aegis_compliance_check;
    int memory_alignment;
    rift_command_t command;
} rift_cli_config_t;

// Global CLI state
static rift_cli_config_t g_cli_config = {
    .config_file = ".riftrc",
    .input_file = NULL,
    .output_file = NULL,
    .verbose_mode = false,
    .debug_mode = false,
    .validate_only = false,
    .aegis_compliance_check = true,
    .memory_alignment = 4096,
    .command = RIFT_CMD_HELP
};

// AEGIS governance state
static rift_governance_t g_governance_state;

// =================================================================
// AEGIS GOVERNANCE INITIALIZATION
// =================================================================

/**
 * Initialize AEGIS governance framework
 * Enforces zero trust policy and memory alignment requirements
 */
static rift_result_t initialize_aegis_governance(void) {
    rift_result_t result = RIFT_SUCCESS;
    
    if (g_cli_config.verbose_mode) {
        printf("🔐 AEGIS Governance Framework Initialization\n");
    }
    
    // Initialize governance configuration
    result = rift_governance_init(&g_governance_state, g_cli_config.config_file);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ AEGIS governance initialization failed: %s\n", 
                rift_error_string(result));
        return result;
    }
    
    // Validate zero trust policy
    if (g_cli_config.aegis_compliance_check) {
        result = rift_governance_validate_zero_trust(&g_governance_state);
        if (result != RIFT_SUCCESS) {
            fprintf(stderr, "❌ Zero trust policy validation failed\n");
            return result;
        }
        
        if (g_cli_config.verbose_mode) {
            printf("✅ Zero trust policy validated\n");
        }
    }
    
    // Validate memory alignment requirements  
    result = rift_governance_validate_memory_alignment(&g_governance_state, 
                                                       g_cli_config.memory_alignment);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ Memory alignment validation failed (required: %d-bit)\n",
                g_cli_config.memory_alignment);
        return result;
    }
    
    if (g_cli_config.verbose_mode) {
        printf("✅ Memory alignment validated (%d-bit classical)\n", 
               g_cli_config.memory_alignment);
    }
    
    // Validate token triplet schema
    result = rift_governance_validate_token_schema(&g_governance_state);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ Token triplet schema validation failed\n");
        return result;
    }
    
    if (g_cli_config.verbose_mode) {
        printf("✅ Token triplet schema validated (type, value, memory)\n");
    }
    
    return RIFT_SUCCESS;
}

// =================================================================
// COMMAND IMPLEMENTATION FUNCTIONS
// =================================================================

/**
 * Execute complete compilation pipeline
 * Orchestrates all stages from tokenization through emission
 */
static rift_result_t execute_compile_command(void) {
    rift_result_t result = RIFT_SUCCESS;
    
    printf("🚀 RIFT Complete Compilation Pipeline\n");
    printf("Input: %s\n", g_cli_config.input_file ? g_cli_config.input_file : "<stdin>");
    printf("Output: %s\n", g_cli_config.output_file ? g_cli_config.output_file : "<stdout>");
    printf("\n");
    
    // Stage 0: Tokenization
    printf("📝 [Stage 0] Tokenization...\n");
    result = rift_command_tokenize(g_cli_config.input_file, "tokens.json", 
                                   g_cli_config.verbose_mode);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ Tokenization failed\n");
        return result;
    }
    printf("✅ Tokenization completed\n");
    
    // Stage 1: Parsing
    printf("🌳 [Stage 1] Parsing...\n");
    result = rift_command_parse("tokens.json", "ast.json", 
                                g_cli_config.verbose_mode);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ Parsing failed\n");
        return result;
    }
    printf("✅ Parsing completed\n");
    
    // Stage 2: Semantic Analysis
    printf("🧠 [Stage 2] Semantic Analysis...\n");
    result = rift_command_analyze("ast.json", "semantic_ast.json",
                                  g_cli_config.verbose_mode);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ Semantic analysis failed\n");
        return result;
    }
    printf("✅ Semantic analysis completed\n");
    
    // Stage 3: Validation
    printf("✅ [Stage 3] Validation...\n");
    result = rift_command_validate("semantic_ast.json", "validated_ast.json",
                                   g_cli_config.verbose_mode);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ Validation failed\n");
        return result;
    }
    printf("✅ Validation completed\n");
    
    // Stage 4: Bytecode Generation
    printf("⚙️ [Stage 4] Bytecode Generation...\n");
    result = rift_command_generate("validated_ast.json", "bytecode.rbc",
                                   g_cli_config.verbose_mode);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ Bytecode generation failed\n");
        return result;
    }
    printf("✅ Bytecode generation completed\n");
    
    // Stage 5: Verification
    printf("🔍 [Stage 5] Verification...\n");
    result = rift_command_verify("bytecode.rbc", "verified_bytecode.rbc",
                                 g_cli_config.verbose_mode);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ Verification failed\n");
        return result;
    }
    printf("✅ Verification completed\n");
    
    // Stage 6: Emission
    printf("📤 [Stage 6] Emission...\n");
    result = rift_command_emit("verified_bytecode.rbc", 
                               g_cli_config.output_file ? g_cli_config.output_file : "result.rbc",
                               g_cli_config.verbose_mode);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "❌ Emission failed\n");
        return result;
    }
    printf("✅ Emission completed\n");
    
    printf("\n🎉 Complete compilation pipeline executed successfully\n");
    return RIFT_SUCCESS;
}

/**
 * Execute governance validation commands
 */
static rift_result_t execute_governance_command(const char *subcommand) {
    rift_result_t result = RIFT_SUCCESS;
    
    if (strcmp(subcommand, "validate-memory") == 0) {
        printf("🧠 Memory Alignment Validation\n");
        result = rift_governance_validate_memory_alignment(&g_governance_state,
                                                           g_cli_config.memory_alignment);
        if (result == RIFT_SUCCESS) {
            printf("✅ Memory alignment validated (%d-bit)\n", g_cli_config.memory_alignment);
        }
    } else if (strcmp(subcommand, "validate-tokens") == 0) {
        printf("🎫 Token Schema Validation\n");
        result = rift_governance_validate_token_schema(&g_governance_state);
        if (result == RIFT_SUCCESS) {
            printf("✅ Token triplet schema validated\n");
        }
    } else if (strcmp(subcommand, "validate-governance") == 0) {
        printf("🔐 AEGIS Governance Validation\n");
        result = rift_governance_validate_complete(&g_governance_state);
        if (result == RIFT_SUCCESS) {
            printf("✅ Complete AEGIS governance validation passed\n");
        }
    } else {
        fprintf(stderr, "❌ Unknown governance command: %s\n", subcommand);
        result = RIFT_ERROR_INVALID_ARGUMENT;
    }
    
    return result;
}

// =================================================================
// COMMAND LINE ARGUMENT PROCESSING
// =================================================================

/**
 * Display CLI usage information
 */
static void print_usage(const char *program_name) {
    printf("RIFT Compiler Unified CLI - AEGIS Framework v%s\n", RIFT_VERSION);
    printf("OBINexus Computing - Systematic Waterfall Development\n\n");
    
    printf("Usage: %s [OPTIONS] COMMAND [ARGS...]\n\n", program_name);
    
    printf("Commands:\n");
    printf("  compile                    Execute complete compilation pipeline\n");
    printf("  tokenize                   Execute tokenization stage only\n");
    printf("  parse                      Execute parsing stage only\n");
    printf("  analyze                    Execute semantic analysis stage only\n");
    printf("  validate                   Execute validation stage only\n");
    printf("  generate                   Execute bytecode generation stage only\n");
    printf("  verify                     Execute verification stage only\n");
    printf("  emit                       Execute emission stage only\n");
    printf("  validate-memory            Validate memory alignment compliance\n");
    printf("  validate-tokens            Validate token schema compliance\n");
    printf("  validate-governance        Validate complete AEGIS governance\n");
    printf("  config                     Configuration management\n");
    printf("  version                    Display version information\n");
    printf("  help                       Display this help message\n\n");
    
    printf("Options:\n");
    printf("  -i, --input FILE           Input file (default: stdin)\n");
    printf("  -o, --output FILE          Output file (default: stdout)\n");
    printf("  -c, --config FILE          Configuration file (default: .riftrc)\n");
    printf("  -v, --verbose              Enable verbose output\n");
    printf("  -d, --debug                Enable debug mode\n");
    printf("      --validate-only        Run validation checks only\n");
    printf("      --memory-alignment N   Set memory alignment (default: 4096)\n");
    printf("      --no-aegis             Disable AEGIS compliance checks\n");
    printf("  -h, --help                 Display this help message\n\n");
    
    printf("Examples:\n");
    printf("  %s compile -i program.rift -o program.rbc\n", program_name);
    printf("  %s tokenize --input 'let x = 42;' --verbose\n", program_name);
    printf("  %s validate-governance --config .riftrc\n", program_name);
    printf("  %s config --show\n", program_name);
    printf("\nMore information: https://docs.obinexus.com/rift\n");
}

/**
 * Parse command line arguments
 */
static rift_result_t parse_arguments(int argc, char **argv) {
    int opt;
    int option_index = 0;
    
    static struct option long_options[] = {
        {"input",           required_argument, 0, 'i'},
        {"output",          required_argument, 0, 'o'},
        {"config",          required_argument, 0, 'c'},
        {"verbose",         no_argument,       0, 'v'},
        {"debug",           no_argument,       0, 'd'},
        {"help",            no_argument,       0, 'h'},
        {"validate-only",   no_argument,       0, 1001},
        {"memory-alignment", required_argument, 0, 1002},
        {"no-aegis",        no_argument,       0, 1003},
        {0, 0, 0, 0}
    };
    
    while ((opt = getopt_long(argc, argv, "i:o:c:vdh", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'i':
                g_cli_config.input_file = strdup(optarg);
                break;
            case 'o':
                g_cli_config.output_file = strdup(optarg);
                break;
            case 'c':
                g_cli_config.config_file = strdup(optarg);
                break;
            case 'v':
                g_cli_config.verbose_mode = true;
                break;
            case 'd':
                g_cli_config.debug_mode = true;
                g_cli_config.verbose_mode = true;
                break;
            case 'h':
                g_cli_config.command = RIFT_CMD_HELP;
                return RIFT_SUCCESS;
            case 1001:
                g_cli_config.validate_only = true;
                break;
            case 1002:
                g_cli_config.memory_alignment = atoi(optarg);
                if (g_cli_config.memory_alignment <= 0) {
                    fprintf(stderr, "❌ Invalid memory alignment: %s\n", optarg);
                    return RIFT_ERROR_INVALID_ARGUMENT;
                }
                break;
            case 1003:
                g_cli_config.aegis_compliance_check = false;
                break;
            default:
                fprintf(stderr, "❌ Invalid option. Use --help for usage information.\n");
                return RIFT_ERROR_INVALID_ARGUMENT;
        }
    }
    
    // Parse command
    if (optind < argc) {
        const char *command_str = argv[optind];
        
        if (strcmp(command_str, "compile") == 0) {
            g_cli_config.command = RIFT_CMD_COMPILE;
        } else if (strcmp(command_str, "tokenize") == 0) {
            g_cli_config.command = RIFT_CMD_TOKENIZE;
        } else if (strcmp(command_str, "parse") == 0) {
            g_cli_config.command = RIFT_CMD_PARSE;
        } else if (strcmp(command_str, "analyze") == 0) {
            g_cli_config.command = RIFT_CMD_ANALYZE;
        } else if (strcmp(command_str, "validate") == 0) {
            g_cli_config.command = RIFT_CMD_VALIDATE;
        } else if (strcmp(command_str, "generate") == 0) {
            g_cli_config.command = RIFT_CMD_GENERATE;
        } else if (strcmp(command_str, "verify") == 0) {
            g_cli_config.command = RIFT_CMD_VERIFY;
        } else if (strcmp(command_str, "emit") == 0) {
            g_cli_config.command = RIFT_CMD_EMIT;
        } else if (strncmp(command_str, "validate-", 9) == 0) {
            g_cli_config.command = RIFT_CMD_GOVERNANCE;
        } else if (strcmp(command_str, "config") == 0) {
            g_cli_config.command = RIFT_CMD_CONFIG;
        } else if (strcmp(command_str, "version") == 0) {
            g_cli_config.command = RIFT_CMD_VERSION;
        } else if (strcmp(command_str, "help") == 0) {
            g_cli_config.command = RIFT_CMD_HELP;
        } else {
            fprintf(stderr, "❌ Unknown command: %s\n", command_str);
            return RIFT_ERROR_INVALID_ARGUMENT;
        }
    }
    
    return RIFT_SUCCESS;
}

// =================================================================
// MAIN ENTRY POINT
// =================================================================

/**
 * Main CLI entry point
 * Implements systematic AEGIS methodology with complete error handling
 */
int main(int argc, char **argv) {
    rift_result_t result = RIFT_SUCCESS;
    int exit_code = EXIT_SUCCESS;
    
    // Parse command line arguments
    result = parse_arguments(argc, argv);
    if (result != RIFT_SUCCESS) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }
    
    // Handle help and version commands early
    if (g_cli_config.command == RIFT_CMD_HELP) {
        print_usage(argv[0]);
        goto cleanup;
    }
    
    if (g_cli_config.command == RIFT_CMD_VERSION) {
        printf("RIFT Compiler v%s\n", RIFT_VERSION);
        printf("OBINexus Computing Framework - AEGIS Methodology\n");
        printf("Build: %s %s\n", __DATE__, __TIME__);
        printf("Memory Alignment: %d-bit classical\n", g_cli_config.memory_alignment);
        printf("Token Schema: triplet validation (type, value, memory)\n");
        printf("Governance: AEGIS compliance %s\n", 
               g_cli_config.aegis_compliance_check ? "ENABLED" : "DISABLED");
        goto cleanup;
    }
    
    // Initialize AEGIS governance framework
    if (g_cli_config.aegis_compliance_check) {
        result = initialize_aegis_governance();
        if (result != RIFT_SUCCESS) {
            exit_code = EXIT_FAILURE;
            goto cleanup;
        }
    }
    
    // Execute command based on CLI configuration
    switch (g_cli_config.command) {
        case RIFT_CMD_COMPILE:
            result = execute_compile_command();
            break;
            
        case RIFT_CMD_TOKENIZE:
            result = rift_command_tokenize(g_cli_config.input_file, 
                                           g_cli_config.output_file,
                                           g_cli_config.verbose_mode);
            break;
            
        case RIFT_CMD_PARSE:
            result = rift_command_parse(g_cli_config.input_file,
                                        g_cli_config.output_file,
                                        g_cli_config.verbose_mode);
            break;
            
        case RIFT_CMD_ANALYZE:
            result = rift_command_analyze(g_cli_config.input_file,
                                          g_cli_config.output_file,
                                          g_cli_config.verbose_mode);
            break;
            
        case RIFT_CMD_VALIDATE:
            result = rift_command_validate(g_cli_config.input_file,
                                           g_cli_config.output_file,
                                           g_cli_config.verbose_mode);
            break;
            
        case RIFT_CMD_GENERATE:
            result = rift_command_generate(g_cli_config.input_file,
                                           g_cli_config.output_file,
                                           g_cli_config.verbose_mode);
            break;
            
        case RIFT_CMD_VERIFY:
            result = rift_command_verify(g_cli_config.input_file,
                                         g_cli_config.output_file,
                                         g_cli_config.verbose_mode);
            break;
            
        case RIFT_CMD_EMIT:
            result = rift_command_emit(g_cli_config.input_file,
                                       g_cli_config.output_file,
                                       g_cli_config.verbose_mode);
            break;
            
        case RIFT_CMD_GOVERNANCE:
            result = execute_governance_command(argv[optind]);
            break;
            
        case RIFT_CMD_CONFIG:
            result = rift_command_config(g_cli_config.config_file,
                                         argc - optind - 1,
                                         argv + optind + 1);
            break;
            
        default:
            fprintf(stderr, "❌ No command specified. Use --help for usage information.\n");
            result = RIFT_ERROR_INVALID_ARGUMENT;
            break;
    }
    
    // Set exit code based on result
    if (result != RIFT_SUCCESS) {
        exit_code = EXIT_FAILURE;
        if (g_cli_config.verbose_mode) {
            fprintf(stderr, "❌ Command failed with error: %s\n", rift_error_string(result));
        }
    }
    
cleanup:
    // Cleanup allocated resources
    if (g_cli_config.input_file) {
        free(g_cli_config.input_file);
    }
    if (g_cli_config.output_file) {
        free(g_cli_config.output_file);
    }
    if (g_cli_config.config_file && strcmp(g_cli_config.config_file, ".riftrc") != 0) {
        free(g_cli_config.config_file);
    }
    
    // Cleanup governance framework
    if (g_cli_config.aegis_compliance_check) {
        rift_governance_cleanup(&g_governance_state);
    }
    
    return exit_code;
}