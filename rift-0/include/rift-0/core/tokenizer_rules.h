/*
 * =================================================================
 * tokenizer_rules.h - RIFT-0 DFA Rules & Pattern Engine Interface
 * RIFT: RIFT Is a Flexible Translator
 * Component: DFA transition rules and pattern compilation
 * OBINexus Computing Framework - Stage 0 Implementation
 * 
 * R.ISOLATE(DFATransitions, PatternCompilation, RuleValidation)
 * R.FLAGS(rule_based, deterministic, pattern_cache)
 * R.DEPENDENCY(tokenizer.h) -> Canonical types only
 * 
 * Toolchain: riftlang.exe → .so.a → rift.exe → gosilang
 * Build Orchestration: nlink → polybuild (AEGIS Framework)
 * Author: Nnamdi Michael Okpala & AEGIS Integration Team
 * =================================================================
 */

#ifndef RIFT_0_CORE_TOKENIZER_RULES_H
#define RIFT_0_CORE_TOKENIZER_RULES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Canonical type dependencies - NO redefinitions */
#include "rift-0/core/tokenizer.h"

/* =================================================================
 * RULE ENGINE CONSTANTS
 * =================================================================
 */

#define RIFT_MAX_RULE_PATTERNS 512
#define RIFT_MAX_RULE_DEPTH 64
#define RIFT_RULE_CACHE_SIZE 256

/* =================================================================
 * PATTERN COMPILATION RULES
 * =================================================================
 */

/* Pattern compilation context */
typedef struct {
    const char*     pattern_source;     /* Original pattern */
    TokenFlags      compilation_flags;  /* Compilation options */
    DFAState*       state_machine;      /* Generated DFA */
    uint32_t        rule_id;            /* Unique rule identifier */
    bool            validated;          /* Rule validation status */
    size_t          complexity_score;   /* Pattern complexity */
} PatternRule;

/* Rule compilation results */
typedef enum {
    RULE_COMPILE_SUCCESS = 0,
    RULE_COMPILE_SYNTAX_ERROR,
    RULE_COMPILE_COMPLEXITY_EXCEEDED,
    RULE_COMPILE_MEMORY_ERROR,
    RULE_COMPILE_VALIDATION_FAILED
} RuleCompileResult;

/* =================================================================
 * RULE ENGINE FUNCTIONS (NO TYPE REDEFINITIONS)
 * =================================================================
 */

/* Pattern rule compilation */
RuleCompileResult rift_rule_compile_pattern(const char* pattern, 
                                             TokenFlags flags,
                                             PatternRule* out_rule);

/* Rule validation and optimization */
bool rift_rule_validate_dfa(const DFAState* dfa_root);
bool rift_rule_optimize_transitions(DFAState* dfa_root);
uint32_t rift_rule_calculate_complexity(const char* pattern);

/* Rule caching system */
bool rift_rule_cache_store(const char* pattern_name, const PatternRule* rule);
const PatternRule* rift_rule_cache_lookup(const char* pattern_name);
void rift_rule_cache_clear(void);

/* Rule debugging and diagnostics */
void rift_rule_print_dfa(const DFAState* dfa_root);
void rift_rule_dump_transitions(const DFAState* state);
bool rift_rule_verify_determinism(const DFAState* dfa_root);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_0_CORE_TOKENIZER_RULES_H */