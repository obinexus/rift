#ifndef TOKENIZER_H
#define TOKENIZER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tokenizer_rules.h"
#include <stddef.h>
#include <stdbool.h>

/**
 * RIFT-0 Core Tokenizer Interface
 * 
 * Implementation leverages Nnamdi Okpala's state machine optimization research
 * for deterministic O(n) tokenization with minimal memory overhead.
 * 
 * Key architectural principles:
 * - Single-pass processing without backtracking
 * - PoliC zero-trust governance integration
 * - Thread-safe operation with thread-local storage
 * - Deterministic build artifacts for multi-stage compilation
 */

// RIFT-0 Governance Policy Enforcement
#pragma rift_policy memory(aligned(4)) type(strict) value(static)

/**
 * Initialize the RIFT-0 tokenizer subsystem
 * 
 * This function must be called before any tokenization operations.
 * Implements PoliC governance validation and state machine initialization
 * following the waterfall methodology progression gates.
 * 
 * Thread Safety: Safe for concurrent initialization across threads
 * Memory Requirements: Minimal allocation, uses thread-local storage
 * 
 * @return 0 on successful initialization, negative error code on failure
 */
int tokenizer_initialize(void);

/**
 * Process tokenization for a source file
 * 
 * Implements the core RIFT-0 tokenization pipeline with deterministic
 * output generation. Utilizes state machine minimization techniques
 * from Nnamdi's optimization research for maximum efficiency.
 * 
 * @param filename Path to source file for tokenization
 * @param tokens Output array for generated token triplets (caller allocated)
 * @param max_tokens Maximum number of tokens to generate (buffer size)
 * @param token_count Actual number of tokens generated (output parameter)
 * 
 * @return 0 on successful tokenization, negative error code on failure
 * 
 * Error Conditions:
 * - File not found or read permission denied
 * - Memory allocation failure for file buffer
 * - PoliC governance validation failure
 * - Token buffer overflow (exceeds max_tokens)
 */
int tokenizer_process_file(const char* filename, TokenTriplet* tokens, 
                          size_t max_tokens, size_t* token_count);

/**
 * Process tokenization for an input string
 * 
 * Direct string tokenization interface for in-memory processing.
 * Ideal for REPL environments, test suites, and embedded processing.
 * 
 * @param input Source string for tokenization (null-terminated)
 * @param tokens Output array for generated token triplets (caller allocated)
 * @param max_tokens Maximum number of tokens to generate (buffer size)
 * @param token_count Actual number of tokens generated (output parameter)
 * 
 * @return 0 on successful tokenization, negative error code on failure
 */
int tokenizer_process_string(const char* input, TokenTriplet* tokens, 
                           size_t max_tokens, size_t* token_count);

/**
 * Retrieve the last error message from tokenizer operations
 * 
 * Provides detailed diagnostic information for failed operations.
 * Error messages include context for systematic debugging within
 * our waterfall development methodology.
 * 
 * @return Pointer to null-terminated error message string
 *         Returns empty string if no error has occurred
 */
const char* tokenizer_get_error_message(void);

/**
 * Check if tokenizer is in error state
 * 
 * Enables systematic error checking as part of our methodical
 * testing approach. Should be checked after each tokenization operation.
 * 
 * @return true if tokenizer has encountered an error, false otherwise
 */
bool tokenizer_has_error(void);

/**
 * Reset tokenizer error state
 * 
 * Clears current error condition to allow continued operation.
 * Part of the error recovery strategy in our systematic approach.
 */
void tokenizer_reset_error(void);

/**
 * Cleanup tokenizer resources
 * 
 * Releases all allocated resources and resets tokenizer state.
 * Essential for proper resource management in long-running processes.
 * Thread-safe cleanup with proper synchronization.
 */
void tokenizer_cleanup(void);

/**
 * Print formatted token information for debugging
 * 
 * Diagnostic utility for systematic testing and validation.
 * Outputs human-readable token representation following our
 * documentation standards.
 * 
 * @param token Pointer to token triplet for display
 */
void tokenizer_print_token(const TokenTriplet* token);

/**
 * Print complete token stream for analysis
 * 
 * Comprehensive diagnostic output for token stream validation.
 * Essential tool for systematic verification of tokenization results
 * within our waterfall testing methodology.
 * 
 * @param tokens Array of token triplets to display
 * @param count Number of tokens in the array
 */
void tokenizer_print_tokens(const TokenTriplet* tokens, size_t count);

/**
 * Advanced tokenization configuration structure
 * 
 * Provides fine-grained control over tokenization behavior
 * for specialized processing requirements.
 */
typedef struct {
    bool enable_line_tracking;      // Track line/column information
    bool skip_whitespace;           // Filter whitespace tokens
    bool skip_comments;             // Filter comment tokens
    bool validate_governance;       // Enable PoliC validation
    size_t max_identifier_length;   // Identifier length limit
    size_t max_string_length;       // String literal length limit
} TokenizerConfig;

/**
 * Configure tokenizer behavior with advanced options
 * 
 * Enables systematic customization of tokenization behavior
 * for specific project requirements within the RIFT-0 framework.
 * 
 * @param config Pointer to configuration structure
 * @return 0 on successful configuration, negative error code on failure
 */
int tokenizer_configure(const TokenizerConfig* config);

/**
 * Get current tokenizer configuration
 * 
 * Retrieves active configuration for validation and debugging.
 * Supports systematic configuration management approaches.
 * 
 * @param config Output parameter for current configuration
 * @return 0 on success, negative error code on failure
 */
int tokenizer_get_configuration(TokenizerConfig* config);

/**
 * Validate tokenization results against governance policies
 * 
 * Implements systematic validation of token streams against
 * .riftrc.0 governance rules. Critical component of our
 * methodical quality assurance approach.
 * 
 * @param tokens Array of tokens to validate
 * @param count Number of tokens in array
 * @param policy_file Path to governance policy file (.riftrc.0)
 * @return true if validation passes, false on policy violations
 */
bool tokenizer_validate_governance(const TokenTriplet* tokens, size_t count, 
                                  const char* policy_file);

/**
 * Generate CSV export of token stream for analysis
 * 
 * Produces structured output for systematic analysis and
 * integration with external validation tools. Supports
 * collaborative debugging with Nnamdi's optimization tools.
 * 
 * @param tokens Array of tokens to export
 * @param count Number of tokens in array
 * @param output_file Path for CSV output file
 * @return 0 on successful export, negative error code on failure
 */
int tokenizer_export_csv(const TokenTriplet* tokens, size_t count, 
                        const char* output_file);

#ifdef __cplusplus
}
#endif

#endif // TOKENIZER_H