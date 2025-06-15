#pragma once
// RIFT Stage 1 Parser API - AEGIS Architecture
// OBINexus Computing - TokenMemory → Parser Pipeline
// Part of: riftlang.exe → .so.a → rift.exe → gosilang toolchain

#ifdef __cplusplus
extern "C" {
#endif

#include "rift1/core/rift_types.h"     // Basic types: RiftResult, RiftConfig, RiftToken
#include "rift1/core/rift_ast.h"       // AST node definitions
#include "rift1/core/rift_token_memory.h" // Token memory management

// Forward declarations - AEGIS Engine Components
typedef struct Rift1ParseEngine Rift1ParseEngine;
typedef struct RiftAutomaton RiftAutomaton;
typedef struct RiftPattern RiftPattern;

// ===== RIFT Stage 1 Parser Engine Management =====

/**
 * Create new RIFT Stage 1 parsing engine with AEGIS automaton
 * @return Initialized parse engine or NULL on failure
 */
Rift1ParseEngine* rift1_engine_create(void);

/**
 * Destroy RIFT Stage 1 parsing engine and cleanup AEGIS components
 * @param engine Engine to destroy (safe to pass NULL)
 */
void rift1_engine_destroy(Rift1ParseEngine* engine);

// ===== TokenMemory Integration =====

/**
 * Load token memory into parser engine for Stage 0 → Stage 1 processing
 * @param engine Parse engine
 * @param memory Token memory from Stage 0 (.rift.0)
 * @return RIFT_SUCCESS or error code
 */
RiftResult rift1_engine_load_tokenmemory(Rift1ParseEngine* engine, 
                                        RiftTokenMemory* memory);

// ===== AEGIS Pattern-Based Parsing =====

/**
 * Parse loaded tokens using AEGIS automaton pattern matching
 * @param engine Parse engine with loaded tokens
 * @return RIFT_SUCCESS or parsing error
 */
RiftResult rift1_parse_tokens(Rift1ParseEngine* engine);

/**
 * Generate AST from parsed tokens through AEGIS transformation
 * @param engine Parse engine with completed parsing
 * @return RIFT_SUCCESS or AST generation error
 */
RiftResult rift1_generate_ast(Rift1ParseEngine* engine);

// ===== Stage Pipeline Integration =====

/**
 * Complete Stage 1 processing: input → tokenload → parse → AST → output
 * @param engine Parse engine
 * @param input_file .rift.0 file from Stage 0
 * @param output_file .rift.1 file for Stage 3
 * @return RIFT_SUCCESS or processing error
 */
RiftResult rift1_process(Rift1ParseEngine* engine, 
                        const char* input_file, 
                        const char* output_file);

/**
 * Convenience function: Stage 0 → Stage 1 direct processing
 * @param rift0_file Input .rift.0 file
 * @param rift1_file Output .rift.1 file
 * @param config RIFT configuration (can be NULL for defaults)
 * @return RIFT_SUCCESS or processing error
 */
RiftResult rift1_process_stage0_to_stage1(const char* rift0_file, 
                                         const char* rift1_file, 
                                         RiftConfig* config);

// ===== Zero Trust Governance Integration =====

/**
 * Verify cryptographic signature of Stage 0 output before processing
 * @param rift0_file Stage 0 file to verify
 * @return RIFT_SUCCESS if signature valid, error otherwise
 */
RiftResult rift1_verify_stage0_signature(const char* rift0_file);

/**
 * Sign Stage 1 output with cryptographic signature for Zero Trust pipeline
 * @param rift1_file Stage 1 file to sign
 * @return RIFT_SUCCESS if signing successful, error otherwise
 */
RiftResult rift1_sign_stage1_output(const char* rift1_file);

#ifdef __cplusplus
}
#endif

// ===== OBINexus Computing Metadata =====
// Architecture: AEGIS pattern-based transformation with Zero Trust governance
// Pipeline Stage: 1 (Parsing/Grammar Analysis)
// Security Level: Standard
// Dependencies: librift1-types, librift1-tokenmemory, librift1-ast
// Build: nlink → polybuild integration required