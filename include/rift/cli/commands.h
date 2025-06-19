/*
 * =================================================================
 * RIFT CLI Core Command Interface Headers
 * OBINexus Computing Framework - AEGIS Methodology
 * Technical Implementation: Systematic Waterfall Development
 * =================================================================
 */

#ifndef RIFT_CLI_COMMANDS_H
#define RIFT_CLI_COMMANDS_H

#include <stdbool.h>
#include <rift/core/common.h>

#ifdef __cplusplus
extern "C" {
#endif

// =================================================================
// COMMAND ENUMERATION AND TYPES
// =================================================================

/**
 * Enumeration of all supported CLI commands
 * Maps to individual stage executables and governance functions
 */
typedef enum {
    RIFT_CMD_NONE = 0,
    RIFT_CMD_HELP,
    RIFT_CMD_VERSION,
    RIFT_CMD_CONFIG,
    
    // Pipeline stage commands
    RIFT_CMD_COMPILE,           // Complete pipeline execution
    RIFT_CMD_TOKENIZE,          // Stage 0: Tokenization
    RIFT_CMD_PARSE,             // Stage 1: Parsing
    RIFT_CMD_ANALYZE,           // Stage 2: Semantic Analysis
    RIFT_CMD_VALIDATE,          // Stage 3: Validation
    RIFT_CMD_GENERATE,          // Stage 4: Bytecode Generation
    RIFT_CMD_VERIFY,            // Stage 5: Verification
    RIFT_CMD_EMIT,              // Stage 6: Emission
    
    // AEGIS governance commands
    RIFT_CMD_GOVERNANCE,        // Governance validation umbrella
    RIFT_CMD_VALIDATE_MEMORY,   // Memory alignment validation
    RIFT_CMD_VALIDATE_TOKENS,   // Token schema validation
    RIFT_CMD_VALIDATE_AEGIS     // Complete AEGIS compliance
} rift_command_t;

/**
 * Command execution context structure
 * Contains all necessary parameters for command execution
 */
typedef struct {
    const char *input_file;
    const char *output_file;
    const char *config_file;
    bool verbose_mode;
    bool debug_mode;
    bool validate_only;
    int memory_alignment;
    void *stage_specific_data;
} rift_command_context_t;

// =================================================================
// PIPELINE STAGE COMMAND INTERFACE
// =================================================================

/**
 * Execute tokenization stage
 * Converts RIFT source code into token stream using AEGIS automaton engine
 * 
 * @param input_source Input source code or file path
 * @param output_path Output JSON token file path
 * @param verbose Enable verbose output
 * @return RIFT_SUCCESS on success, error code on failure
 */
rift_result_t rift_command_tokenize(const char *input_source, 
                                    const char *output_path,
                                    bool verbose);

/**
 * Execute parsing stage  
 * Converts token stream into Abstract Syntax Tree using dual-mode parser
 * 
 * @param input_tokens Path to JSON token file
 * @param output_ast Path to output AST JSON file
 * @param verbose Enable verbose output
 * @return RIFT_SUCCESS on success, error code on failure
 */
rift_result_t rift_command_parse(const char *input_tokens,
                                 const char *output_ast,
                                 bool verbose);

/**
 * Execute semantic analysis stage
 * Enriches AST with type information and semantic validation
 * 
 * @param input_ast Path to input AST JSON file
 * @param output_semantic_ast Path to output semantic AST file
 * @param verbose Enable verbose output
 * @return RIFT_SUCCESS on success, error code on failure
 */
rift_result_t rift_command_analyze(const char *input_ast,
                                   const char *output_semantic_ast,
                                   bool verbose);

/**
 * Execute validation stage
 * Performs comprehensive validation of semantic AST
 * 
 * @param input_semantic_ast Path to input semantic AST file
 * @param output_validated_ast Path to output validated AST file
 * @param verbose Enable verbose output
 * @return RIFT_SUCCESS on success, error code on failure
 */
rift_result_t rift_command_validate(const char *input_semantic_ast,
                                    const char *output_validated_ast,
                                    bool verbose);

/**
 * Execute bytecode generation stage
 * Converts validated AST into RIFT bytecode format
 * 
 * @param input_validated_ast Path to input validated AST file
 * @param output_bytecode Path to output bytecode (.rbc) file
 * @param verbose Enable verbose output
 * @return RIFT_SUCCESS on success, error code on failure
 */
rift_result_t rift_command_generate(const char *input_validated_ast,
                                    const char *output_bytecode,
                                    bool verbose);

/**
 * Execute verification stage
 * Verifies bytecode integrity and security properties
 * 
 * @param input_bytecode Path to input bytecode file
 * @param output_verified_bytecode Path to output verified bytecode file
 * @param verbose Enable verbose output
 * @return RIFT_SUCCESS on success, error code on failure
 */
rift_result_t rift_command_verify(const char *input_bytecode,
                                  const char *output_verified_bytecode,
                                  bool verbose);

/**
 * Execute emission stage
 * Emits final bytecode container with metadata and signatures
 * 
 * @param input_verified_bytecode Path to input verified bytecode file
 * @param output_final Path to final output container file
 * @param verbose Enable verbose output
 * @return RIFT_SUCCESS on success, error code on failure
 */
rift_result_t rift_command_emit(const char *input_verified_bytecode,
                                const char *output_final,
                                bool verbose);

// =================================================================
// CONFIGURATION MANAGEMENT INTERFACE
// =================================================================

/**
 * Execute configuration management commands
 * Handles .riftrc and pkg.riftrc governance configuration
 * 
 * @param config_file Path to configuration file
 * @param argc Number of additional arguments
 * @param argv Additional command arguments
 * @return RIFT_SUCCESS on success, error code on failure
 */
rift_result_t rift_command_config(const char *config_file,
                                  int argc,
                                  char **argv);

// =================================================================
// AEGIS GOVERNANCE COMMAND INTERFACE
// =================================================================

/**
 * Validate memory alignment compliance
 * Ensures 4096-bit classical memory alignment requirements
 * 
 * @param alignment_bits Required alignment in bits
 * @return RIFT_SUCCESS on compliance, error code on failure
 */
rift_result_t rift_command_validate_memory_alignment(int alignment_bits);

/**
 * Validate token schema compliance
 * Ensures token_type, token_value, token_memory triplet enforcement
 * 
 * @return RIFT_SUCCESS on compliance, error code on failure
 */
rift_result_t rift_command_validate_token_schema(void);

/**
 * Validate complete AEGIS governance compliance
 * Comprehensive validation of all AEGIS methodology requirements
 * 
 * @param config_file Path to governance configuration file
 * @return RIFT_SUCCESS on full compliance, error code on failure
 */
rift_result_t rift_command_validate_aegis_compliance(const char *config_file);

// =================================================================
// UTILITY AND HELPER FUNCTIONS
// =================================================================

/**
 * Convert command string to command enum
 * 
 * @param command_str String representation of command
 * @return Corresponding rift_command_t enum value
 */
rift_command_t rift_parse_command_string(const char *command_str);

/**
 * Get human-readable command description
 * 
 * @param command Command enum value
 * @return String description of command
 */
const char *rift_command_description(rift_command_t command);

/**
 * Validate command context for execution
 * 
 * @param context Command execution context
 * @param command Command to validate context for
 * @return RIFT_SUCCESS if context is valid, error code otherwise
 */
rift_result_t rift_validate_command_context(const rift_command_context_t *context,
                                            rift_command_t command);

/**
 * Initialize default command context
 * 
 * @param context Context structure to initialize
 */
void rift_init_command_context(rift_command_context_t *context);

/**
 * Cleanup command context resources
 * 
 * @param context Context structure to cleanup
 */
void rift_cleanup_command_context(rift_command_context_t *context);

// =================================================================
// ERROR HANDLING AND LOGGING
// =================================================================

/**
 * Log command execution start
 * 
 * @param command Command being executed
 * @param context Execution context
 */
void rift_log_command_start(rift_command_t command,
                            const rift_command_context_t *context);

/**
 * Log command execution completion
 * 
 * @param command Command that completed
 * @param result Execution result
 * @param duration_ms Execution duration in milliseconds
 */
void rift_log_command_complete(rift_command_t command,
                               rift_result_t result,
                               uint64_t duration_ms);

/**
 * Get error string for command execution failure
 * 
 * @param command Command that failed
 * @param result Error result code
 * @return Human-readable error description
 */
const char *rift_command_error_string(rift_command_t command,
                                      rift_result_t result);

// =================================================================
// STAGE INTEGRATION VERIFICATION
// =================================================================

/**
 * Verify all stage libraries are properly linked
 * Ensures rift-{0..6}_static libraries are accessible
 * 
 * @return RIFT_SUCCESS if all stages linked, error code otherwise
 */
rift_result_t rift_verify_stage_linkage(void);

/**
 * Get version information for all linked stages
 * 
 * @param version_buffer Buffer to store version information
 * @param buffer_size Size of version buffer
 * @return RIFT_SUCCESS on success, error code on failure
 */
rift_result_t rift_get_stage_versions(char *version_buffer, size_t buffer_size);

/**
 * Validate pipeline integrity
 * Ensures all stages can communicate properly through unified interface
 * 
 * @return RIFT_SUCCESS if pipeline integrity verified, error code otherwise
 */
rift_result_t rift_validate_pipeline_integrity(void);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CLI_COMMANDS_H */

/*
 * =================================================================
 * IMPLEMENTATION NOTES
 * =================================================================
 * 
 * This header defines the complete command interface for the unified
 * RIFT CLI implementation. Key architectural decisions:
 * 
 * 1. **Command Enumeration**: All commands are enumerated for type safety
 *    and systematic dispatch handling.
 * 
 * 2. **Context Structure**: Command context encapsulates all execution
 *    parameters, enabling consistent parameter passing across stages.
 * 
 * 3. **Stage Integration**: Functions verify that all rift-{0..6}_static
 *    libraries are properly linked and accessible.
 * 
 * 4. **AEGIS Compliance**: Dedicated functions for governance validation
 *    ensure systematic enforcement of AEGIS methodology requirements.
 * 
 * 5. **Error Handling**: Comprehensive error reporting and logging
 *    support systematic debugging and operational monitoring.
 * 
 * 6. **Pipeline Integrity**: Validation functions ensure end-to-end
 *    pipeline operation and stage communication integrity.
 * 
 * Implementation files required:
 * - src/cli/commands/tokenize.c
 * - src/cli/commands/parse.c
 * - src/cli/commands/analyze.c
 * - src/cli/commands/validate.c
 * - src/cli/commands/generate.c
 * - src/cli/commands/verify.c
 * - src/cli/commands/emit.c
 * - src/cli/commands/config.c
 * - src/cli/commands/governance.c
 * - src/cli/commands/utils.c
 * 
 * =================================================================
 */