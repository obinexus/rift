/**
 * @file baseline_patterns.h
 * @brief Baseline regex patterns for core language functionality
 *
 * This file defines the fundamental regex patterns used by the LibRift engine
 * to represent standard automaton states and transitions. These patterns
 * can be used internally and extended externally.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#ifndef LIBRIFT_CORE_REGEX_PATTERNS_BASELINE_PATTERNS_H
#define LIBRIFT_CORE_REGEX_PATTERNS_BASELINE_PATTERNS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Empty state pattern - matches any non-alphanumeric, non-whitespace character
 * This represents an initial/empty automaton state
 */
#define RIFT_PATTERN_EMPTY_STATE R"([^A-Za-z0-9\s])"

/**
 * @brief Whitespace pattern - matches any whitespace character
 * Used for transitions between states where whitespace is ignored
 */
#define RIFT_PATTERN_WHITESPACE R"(\s+)"

/**
 * @brief Word boundary pattern - matches a word boundary
 * Used for transition points in the automaton
 */
#define RIFT_PATTERN_WORD_BOUNDARY R"(\b)"

/**
 * @brief Identifier pattern - matches a valid identifier
 * Used for recognizing variable names, function names, etc.
 */
#define RIFT_PATTERN_IDENTIFIER R"([a-zA-Z_][a-zA-Z0-9_]*)"

/**
 * @brief Numeric literal pattern - matches integers and floating-point numbers
 * Used for recognizing numeric literals in the language
 */
#define RIFT_PATTERN_NUMERIC R"(-?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?)"

/**
 * @brief String literal pattern - matches quoted strings with proper escape handling
 * Used for recognizing string literals in the language
 */
#define RIFT_PATTERN_STRING R"("(?:[^"\\]|\\.)*")"

/**
 * @brief Comment pattern - matches single-line and multi-line comments
 * Used for recognizing and potentially ignoring comments
 */
#define RIFT_PATTERN_COMMENT R"((?://.*$|/\*(?:[^*]|\*[^/])*\*/))"

/**
 * @brief Operator pattern - matches common operators
 * Used for recognizing operators in expressions
 */
#define RIFT_PATTERN_OPERATOR R"([+\-*/=<>!&|^%~]+)"

/**
 * @brief Function pattern - matches function declarations
 * Used for recognizing function definitions
 */
#define RIFT_PATTERN_FUNCTION R"(function\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\([^)]*\))"

/**
 * @brief State transition pattern - for internal automaton representation
 * Used to match state transitions in the engine
 */
#define RIFT_PATTERN_STATE_TRANSITION R"((?:->|→))"

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_CORE_REGEX_PATTERNS_BASELINE_PATTERNS_H */
