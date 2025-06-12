/**
 * @file r_extensions.h
 * @brief OBINexus RIFT R Syntax Extensions - Macro Definitions
 * 
 * Comprehensive macro system for:
 * - UML relationship modeling (Composition, Association, Aggregation, Inheritance)
 * - Boolean logic operations (AND, OR, XOR, NOT, NAND, NOR)
 * - Functional composition with point-free programming
 * - Governance-first validation patterns
 * 
 * Part of OBINexus Computing RIFT Governance-First Architecture
 * Usage: #include "r_extensions.h" in any RIFT module
 * 
 * @copyright OBINexus Computing Department 2025
 */

#ifndef R_EXTENSIONS_H
#define R_EXTENSIONS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* =============================================================================
 * R Boolean Logic Macros - Core Operations
 * Used throughout codebase for governance validation and control flow
 * ============================================================================= */

// Primary Boolean Operations
#define R_AND(a, b)         ((a) & (b))
#define R_OR(a, b)          ((a) | (b))
#define R_XOR(a, b)         ((a) ^ (b))
#define R_NOT(a)            (~(a))

// Extended Boolean Operations
#define R_NAND(a, b)        R_NOT(R_AND(a, b))
#define R_NOR(a, b)         R_NOT(R_OR(a, b))

// Conditional Boolean Operations
#define R_IMPLIES(a, b)     R_OR(R_NOT(a), (b))
#define R_EQUIV(a, b)       R_AND(R_IMPLIES(a, b), R_IMPLIES(b, a))

// Multi-operand Boolean Operations
#define R_AND3(a, b, c)     R_AND(R_AND(a, b), (c))
#define R_OR3(a, b, c)      R_OR(R_OR(a, b), (c))
#define R_MAJORITY(a, b, c) R_OR(R_AND(a, b), R_OR(R_AND(a, c), R_AND(b, c)))

/* =============================================================================
 * R UML Relationship Pattern Macros
 * Defines regex patterns for UML relationship recognition
 * ============================================================================= */

// Composition: Strong "has-a" relationship (lifecycle dependency)
#define R_COMPOSITION_PATTERN \
    R"class\s+(\w+)\s*\{\s*([^}]*\b(?:composed_of|owns)\s+(\w+)[^}]*)\s*\}"

// Association: Basic relationship between classes
#define R_ASSOCIATION_PATTERN \
    R"class\s+(\w+)\s*\{\s*([^}]*\b(?:associated_with|uses)\s+(\w+)[^}]*)\s*\}"

// Aggregation: Weak "has-a" relationship (no lifecycle dependency)
#define R_AGGREGATION_PATTERN \
    R"class\s+(\w+)\s*\{\s*([^}]*\b(?:aggregates|contains)\s+(\w+)[^}]*)\s*\}"

// Inheritance: "is-a" relationship
#define R_INHERITANCE_PATTERN \
    R"class\s+(\w+)\s+(?:extends|inherits_from)\s+(\w+)\s*\{"

// Dependency: Temporary relationship
#define R_DEPENDENCY_PATTERN \
    R"class\s+(\w+)\s*\{\s*([^}]*\b(?:depends_on|requires)\s+(\w+)[^}]*)\s*\}"

/* =============================================================================
 * R Functional Composition Macros
 * Point-free programming patterns for data-agnostic operations
 * ============================================================================= */

// Function composition - f(g(x))
#define R_COMPOSE(f, g) \
    ({ __typeof__(g) _g = (g); __typeof__(f) _f = (f); \
       void* _compose_fn(void* x) { return _f(_g(x)); } \
       _compose_fn; })

// Pipe operation - x |> f
#define R_PIPE(x, f)        f(x)

// Chain multiple operations
#define R_CHAIN(x, ...)     R_CHAIN_IMPL(x, __VA_ARGS__)
#define R_CHAIN_IMPL(x, f, ...) \
    (__VA_OPT__(R_CHAIN_IMPL(f(x), __VA_ARGS__)) __VA_NO_OPT__(f(x)))

// Apply function to each element
#define R_MAP(fn, arr, size) \
    ({ for(size_t _i = 0; _i < (size); _i++) { (arr)[_i] = fn((arr)[_i]); } (arr); })

// Filter elements based on predicate
#define R_FILTER(pred, arr, size, result_size) \
    ({ size_t _j = 0; \
       for(size_t _i = 0; _i < (size); _i++) { \
           if(pred((arr)[_i])) { (arr)[_j++] = (arr)[_i]; } \
       } \
       *(result_size) = _j; (arr); })

// Reduce array to single value
#define R_REDUCE(fn, arr, size, init) \
    ({ __typeof__(init) _acc = (init); \
       for(size_t _i = 0; _i < (size); _i++) { _acc = fn(_acc, (arr)[_i]); } \
       _acc; })

/* =============================================================================
 * R Extension System Macros
 * Macros for extending functionality with governance validation
 * ============================================================================= */

// Extension with governance validation
#define R_EXTEND(base, extension, validation_fn) \
    ({ bool _valid = validation_fn ? validation_fn(extension) : true; \
       _valid ? r_extend_impl(base, extension) : NULL; })

// Safe extension with error handling
#define R_EXTEND_SAFE(base, extension, validation_fn, error_handler) \
    ({ bool _valid = validation_fn ? validation_fn(extension) : true; \
       if (!_valid && error_handler) error_handler("Extension validation failed"); \
       _valid ? r_extend_impl(base, extension) : NULL; })

// Async extension (pointer-based for async features)
#define R_EXTEND_ASYNC(base_ptr, extension_ptr, callback) \
    ({ if (base_ptr && extension_ptr) { \
           r_extend_async_impl(base_ptr, extension_ptr, callback); \
       } })

/* =============================================================================
 * R Governance Validation Macros
 * Macros for ensuring governance compliance throughout codebase
 * ============================================================================= */

// Governance validation check
#define R_GOVERNANCE_CHECK(condition, message) \
    ({ if (!(condition)) { \
           r_governance_log_violation(__FILE__, __LINE__, message); \
           return false; \
       } true; })

// Governance triangle validation (attack, rollback, stability)
#define R_GOVERNANCE_TRIANGLE(attack_risk, rollback_cost, stability_impact) \
    ({ double _total = (attack_risk) + (rollback_cost) + (stability_impact); \
       bool _valid = _total <= 1.0 && \
                    (attack_risk) <= 0.4 && \
                    (rollback_cost) <= 0.4 && \
                    (stability_impact) <= 0.4; \
       if (!_valid) r_governance_log_triangle_violation(_total); \
       _valid; })

// Resource allocation with governance
#define R_ALLOC_GOVERNED(size, purpose) \
    ({ void* _ptr = r_governed_alloc(size, purpose, __FILE__, __LINE__); \
       r_governance_track_allocation(_ptr, size); \
       _ptr; })

/* =============================================================================
 * R Relationship Type Definitions
 * Enums and structs for UML relationship modeling
 * ============================================================================= */

typedef enum {
    R_REL_COMPOSITION = 0,
    R_REL_ASSOCIATION,
    R_REL_AGGREGATION,
    R_REL_INHERITANCE,
    R_REL_DEPENDENCY,
    R_REL_REALIZATION,
    R_REL_COUNT
} r_relationship_type_t;

typedef struct r_relationship {
    char *source_class;
    char *target_class;
    r_relationship_type_t type;
    bool governance_validated;
    uint64_t creation_timestamp;
    void *metadata;
} r_relationship_t;

typedef struct r_model {
    r_relationship_t **relationships;
    size_t count;
    size_t capacity;
    bool governance_locked;
} r_model_t;

/* =============================================================================
 * R Utility Macros
 * General-purpose macros for common operations
 * ============================================================================= */

// Safe string operations
#define R_SAFE_STR(str)     ((str) ? (str) : "")
#define R_STR_EQ(a, b)      (strcmp(R_SAFE_STR(a), R_SAFE_STR(b)) == 0)
#define R_STR_EMPTY(str)    (!str || strlen(str) == 0)

// Memory safety macros
#define R_SAFE_FREE(ptr)    ({ if (ptr) { free(ptr); ptr = NULL; } })
#define R_NULL_CHECK(ptr, action) ({ if (!(ptr)) { action; } })

// Array operations
#define R_ARRAY_SIZE(arr)   (sizeof(arr) / sizeof((arr)[0]))
#define R_ARRAY_LAST(arr)   ((arr)[R_ARRAY_SIZE(arr) - 1])

// Debugging and logging
#define R_DEBUG(msg, ...)   ({ if (r_debug_enabled()) \
                               fprintf(stderr, "[R_DEBUG] " msg "\n", ##__VA_ARGS__); })

#define R_LOG(level, msg, ...) ({ r_log(level, __FILE__, __LINE__, msg, ##__VA_ARGS__); })

/* =============================================================================
 * R Pattern Matching Macros
 * Advanced pattern matching for regex-based parsing
 * ============================================================================= */

// Pattern compilation with error handling
#define R_COMPILE_PATTERN(pattern_str, flags) \
    ({ rift_regex_error_t _error; \
       rift_regex_pattern_t *_pattern = rift_regex_compile(pattern_str, flags, &_error); \
       if (!_pattern) R_LOG(R_LOG_ERROR, "Pattern compilation failed: %s", _error.message); \
       _pattern; })

// Pattern matching with governance validation
#define R_MATCH_GOVERNED(pattern, input, governance_fn) \
    ({ rift_regex_match_t _match; \
       rift_regex_matcher_t *_matcher = rift_matcher_create(pattern, RIFT_MATCHER_OPTION_NONE); \
       bool _success = false; \
       if (_matcher && rift_matcher_set_input(_matcher, input, strlen(input))) { \
           if (rift_matcher_matches(_matcher, &_match)) { \
               _success = governance_fn ? governance_fn(&_match) : true; \
           } \
       } \
       if (_matcher) rift_matcher_free(_matcher); \
       _success ? &_match : NULL; })

/* =============================================================================
 * R Performance Optimization Macros
 * Macros for optimizing common operations
 * ============================================================================= */

// Likely/unlikely branch prediction hints
#define R_LIKELY(x)         __builtin_expect(!!(x), 1)
#define R_UNLIKELY(x)       __builtin_expect(!!(x), 0)

// Fast bit operations
#define R_IS_POWER_OF_2(x)  (((x) & ((x) - 1)) == 0)
#define R_ALIGN_UP(x, a)    (((x) + (a) - 1) & ~((a) - 1))
#define R_ALIGN_DOWN(x, a)  ((x) & ~((a) - 1))

// Cache optimization hints
#define R_PREFETCH(ptr)     __builtin_prefetch(ptr, 0, 3)
#define R_HOT               __attribute__((hot))
#define R_COLD              __attribute__((cold))

/* =============================================================================
 * R Error Handling Macros
 * Comprehensive error handling with governance integration
 * ============================================================================= */

typedef enum {
    R_SUCCESS = 0,
    R_ERROR_NULL_POINTER,
    R_ERROR_INVALID_PARAMETER,
    R_ERROR_MEMORY_ALLOCATION,
    R_ERROR_GOVERNANCE_VIOLATION,
    R_ERROR_PATTERN_COMPILATION,
    R_ERROR_IO_FAILURE,
    R_ERROR_COUNT
} r_error_code_t;

#define R_TRY(expr) \
    ({ r_error_code_t _result = (expr); \
       if (_result != R_SUCCESS) { \
           R_LOG(R_LOG_ERROR, "Operation failed with code %d", _result); \
           return _result; \
       } \
       _result; })

#define R_ASSERT(condition, error_code) \
    ({ if (R_UNLIKELY(!(condition))) { \
           R_LOG(R_LOG_ERROR, "Assertion failed: %s", #condition); \
           return error_code; \
       } })

/* =============================================================================
 * Function Declarations
 * Prototypes for functions that support the macro system
 * ============================================================================= */

#ifdef __cplusplus
extern "C" {
#endif

// Core extension functions
void* r_extend_impl(void *base, void *extension);
void r_extend_async_impl(void **base_ptr, void **extension_ptr, void (*callback)(void*));

// Governance functions
void r_governance_log_violation(const char *file, int line, const char *message);
void r_governance_log_triangle_violation(double total_risk);
bool r_governance_validate_extension(void *extension);

// Memory management
void* r_governed_alloc(size_t size, const char *purpose, const char *file, int line);
void r_governance_track_allocation(void *ptr, size_t size);

// Logging and debugging
bool r_debug_enabled(void);
void r_log(int level, const char *file, int line, const char *format, ...);

// Pattern matching support
bool r_pattern_governance_validator(rift_regex_match_t *match);

#ifdef __cplusplus
}
#endif

/* =============================================================================
 * Usage Examples (commented out for production)
 * ============================================================================= */

#if 0
// Example 1: Boolean logic usage
bool validate_user_permissions(int user_flags, int required_flags) {
    return R_AND(user_flags, required_flags) == required_flags;
}

// Example 2: UML relationship pattern matching
void parse_class_relationships(const char *source_code) {
    rift_regex_pattern_t *comp_pattern = R_COMPILE_PATTERN(R_COMPOSITION_PATTERN, RIFT_REGEX_FLAG_RIFT_SYNTAX);
    // ... pattern matching logic
}

// Example 3: Functional composition
int double_value(int x) { return x * 2; }
int add_ten(int x) { return x + 10; }

void demonstrate_composition() {
    int data[] = {1, 2, 3, 4, 5};
    R_MAP(double_value, data, 5);  // [2, 4, 6, 8, 10]
    R_MAP(add_ten, data, 5);       // [12, 14, 16, 18, 20]
}

// Example 4: Governance validation
r_relationship_t* create_validated_relationship(const char *source, const char *target) {
    r_relationship_t *rel = R_ALLOC_GOVERNED(sizeof(r_relationship_t), "UML relationship");
    if (!rel) return NULL;
    
    rel->source_class = strdup(source);
    rel->target_class = strdup(target);
    rel->governance_validated = R_GOVERNANCE_CHECK(
        R_AND(R_STR_EMPTY(source) == false, R_STR_EMPTY(target) == false),
        "Source and target classes must be non-empty"
    );
    
    return rel;
}
#endif

#endif // R_EXTENSIONS_H