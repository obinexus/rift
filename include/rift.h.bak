/**
 * @file rift.h
 * @brief Main include file for the LibRift regex engine
 *
 * This file provides a single include point for all LibRift functionality,
 * properly including each module in the correct dependency order.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

 #ifndef LIBRIFT_H
 #define LIBRIFT_H
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /* Include version information */
 #include "core/config/config.h"
 #include "version.h"
 
 /* Core components */
 #include "core/memory/memory.h"
 #include "core/errors/regex_error.h"
 #include "core/errors/error.h"
 
 /* Automaton module */
 #include "core/automaton/flags.h"
 #include "core/automaton/state.h"
 #include "core/automaton/transition.h"
 #include "core/automaton/automaton.h"
 #include "core/automaton/minimizer.h"
 #include "core/automaton/regex.h"
 
 /* Parser module */
 #include "core/parser/ast_node.h"
 #include "core/parser/ast.h"
 #include "core/parser/parser.h"
 #include "core/parser/validator.h"
 
 /* Tokenizer module */
 #include "core/tokenizer/token_type.h"
 #include "core/tokenizer/token.h"
 #include "core/tokenizer/tokenizer.h"
 
 /* Syntax module */
 #include "core/syntax/lexer.h"
 #include "core/syntax/parser.h"
 #include "core/syntax/syntax.h"
 #include "core/syntax/integration.h"
 
 /* Compiler module */
 #include "core/compiler/compiler.h"
 #include "core/compiler/optimizer.h"
 
 /* Pattern definitions */
 #include "core/patterns/pattern_types.h"
 #include "core/patterns/pattern_extensions.h"
 #include "core/patterns/baseline_patterns.h"
 
 /* Regex module */
 #include "core/regex/flags.h"
 #include "core/regex/regex.h"
 
 /* Engine module */
 #include "core/engine/pattern.h"
 #include "core/engine/matcher.h"
 #include "core/engine/engine.h"
 
 /* Config module */
 #include "core/config/backtracker_limit_config.h"
 #include "core/config/backtracker_limit_registry.h"
 #include "core/config/bailout_strategy_manager.h"
 
 /* Bytecode module */
 #include "core/bytecode/bytecode.h"
 #include "core/bytecode/bytecode_program.h"
 #include "core/bytecode/bytecode_vm.h"
 #include "core/bytecode/bytecode_system.h"
 #include "core/bytecode/bytecode_compiler.h"
 #include "core/bytecode/bridge_api.h"
 
 /* Web bridge (for WebAssembly compatibility) */
 #include "core/webbridge/webbridge.h"
 
 /* 
  * LibRift library version macros
  * These can be used to check API compatibility at compile time
  */
 #ifndef LIBRIFT_VERSION_MAJOR
 #define LIBRIFT_VERSION_MAJOR 1
 #define LIBRIFT_VERSION_MINOR 0
 #define LIBRIFT_VERSION_PATCH 0
 #define LIBRIFT_VERSION_STRING "1.0.0"
 #endif
 
 /**
  * @brief Runtime version check function
  *
  * Use this to verify at runtime that the library version matches the expected version
  *
  * @return true if the used library version is compatible with the header version
  */
 bool librift_check_version(void);
 
 /**
  * @brief Initialize the LibRift library
  *
  * This must be called before using any LibRift functionality.
  *
  * @return true if initialization is successful, false otherwise
  */
 bool librift_initialize(void);
 
 /**
  * @brief Clean up and release resources used by LibRift
  *
  * Call this when finished using the library to release all resources.
  */
 void librift_cleanup(void);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* LIBRIFT_H */