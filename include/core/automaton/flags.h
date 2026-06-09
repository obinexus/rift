/**
 * @file flags.h
 * @brief Header file defining regex flags for the LibRift regex engine
 *
 * This file defines the various flags that control the behavior of the
 * regex engine, affecting pattern compilation and execution.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <regex.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "core/errors/regex_error.h"
#ifndef LIBRIFT_REGEX_AUTOMATON_FLAGS_H
#define LIBRIFT_REGEX_AUTOMATON_FLAGS_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Flags for controlling regex pattern compilation and matching
 */
typedef enum rift_regex_flags {
    RIFT_REGEX_FLAG_NONE = 0x00000000,              /**< No flags */
    RIFT_REGEX_FLAG_CASE_INSENSITIVE = 0x00000001,  /**< Case insensitive matching */
    RIFT_REGEX_FLAG_MULTILINE = 0x00000002,         /**< ^ and $ match start/end of line */
    RIFT_REGEX_FLAG_DOTALL = 0x00000004,            /**< . matches newline too */
    RIFT_REGEX_FLAG_EXTENDED = 0x00000008,          /**< Ignore whitespace and comments */
    RIFT_REGEX_FLAG_ANCHORED = 0x00000010,          /**< Pattern must match at start of string */
    RIFT_REGEX_FLAG_DOLLAR_ENDONLY = 0x00000020,    /**< $ only matches at end of string */
    RIFT_REGEX_FLAG_UNGREEDY = 0x00000040,          /**< Quantifiers are not greedy by default */
    RIFT_REGEX_FLAG_UTF8 = 0x00000080,              /**< Treat pattern and subject as UTF-8 */
    RIFT_REGEX_FLAG_NO_AUTO_CAPTURE = 0x00000100,   /**< Disable numbered capturing parentheses */
    RIFT_REGEX_FLAG_NO_UTF8_CHECK = 0x00000200,     /**< Skip checks for valid UTF-8 */
    RIFT_REGEX_FLAG_AUTO_CALLOUT = 0x00000400,      /**< Automatically insert callouts */
    RIFT_REGEX_FLAG_FIRSTLINE = 0x00000800,         /**< Match must be on first line of subject */
    RIFT_REGEX_FLAG_DUPNAMES = 0x00001000,          /**< Allow duplicate named subpatterns */
    RIFT_REGEX_FLAG_NEWLINE_CR = 0x00002000,        /**< \n matches CR only */
    RIFT_REGEX_FLAG_NEWLINE_LF = 0x00004000,        /**< \n matches LF only */
    RIFT_REGEX_FLAG_NEWLINE_CRLF = 0x00008000,      /**< \n matches CRLF only */
    RIFT_REGEX_FLAG_NEWLINE_ANY = 0x00010000,       /**< \n matches any Unicode newline */
    RIFT_REGEX_FLAG_NEWLINE_ANYCRLF = 0x00020000,   /**< \n matches CR, LF, or CRLF */
    RIFT_REGEX_FLAG_BSR_ANYCRLF = 0x00040000,       /**< \R matches CR, LF, or CRLF only */
    RIFT_REGEX_FLAG_BSR_UNICODE = 0x00080000,       /**< \R matches any Unicode newline */
    RIFT_REGEX_FLAG_JAVASCRIPT_COMPAT = 0x00100000, /**< JavaScript compatibility mode */
    RIFT_REGEX_FLAG_UCP = 0x00200000,               /**< Use Unicode properties for \d, \w, etc. */
    RIFT_REGEX_FLAG_NO_START_OPTIMIZE = 0x00400000, /**< Disable start-of-pattern optimizations */
    RIFT_REGEX_FLAG_NO_UTF8_CHECK_SUBJECT =
        0x00800000,                               /**< Skip UTF-8 validity check for subject */
    RIFT_REGEX_FLAG_NO_AUTO_POSSESS = 0x01000000, /**< Disable auto-possessification optimization */
    RIFT_REGEX_FLAG_OPTIMIZE_SIZE = 0x02000000,   /**< Optimize for size rather than speed */
    RIFT_REGEX_FLAG_OPTIMIZE_SPEED = 0x04000000,  /**< Optimize for speed rather than size */
    RIFT_REGEX_FLAG_DEBUG = 0x04000000,           /**< Enable debug mode */
    RIFT_REGEX_FLAG_STRICT = 0x08000000,          /**< Strict syntax checking */
    RIFT_REGEX_FLAG_BACKTRACK_LIMIT = 0x10000000, /**< Enable backtracking limit */
    RIFT_REGEX_FLAG_OPTIMIZE = 0x20000000,        /**< Enable optimization mode */
    RIFT_REGEX_FLAG_RIFT_SYNTAX = 0x40000000,     /**< Use LibRift's extended r'' syntax */
    RIFT_REGEX_FLAG_USE_DFA = 0x80000000,         /**< Use DFA matching engine */

    RIFT_REGEX_FLAG_ALL = 0x7FFFFFFF /**< All flags combined (except mutually exclusive ones) */
} rift_regex_flags_t;

/**
 * @brief Check if a flag is set in a flags value
 *
 * @param flags The flags value
 * @param flag The flag to check
 * @return Non-zero if the flag is set, 0 otherwise
 */
#define RIFT_REGEX_HAS_FLAG(flags, flag) ((flags) & (flag))

/**
 * @brief Add a flag to a flags value
 *
 * @param flags Pointer to the flags value
 * @param flag The flag to add
 */
#define RIFT_REGEX_ADD_FLAG(flags, flag) (*(flags) |= (flag))

/**
 * @brief Remove a flag from a flags value
 *
 * @param flags Pointer to the flags value
 * @param flag The flag to remove
 */
#define RIFT_REGEX_REMOVE_FLAG(flags, flag) (*(flags) &= ~(flag))

/**
 * @brief Convert a regex flavor's modifiers to LibRift flags
 *
 * @param modifiers String containing regex modifiers (e.g., "imsx")
 * @return The corresponding LibRift regex flags
 */
rift_regex_flags_t rift_regex_flags_from_modifiers(const char *modifiers);

/**
 * @brief Convert LibRift flags to a regex flavor's modifiers
 *
 * @param flags The LibRift regex flags
 * @param buffer Buffer to store the modifiers string
 * @param buffer_size Size of the buffer
 * @return Number of characters written to the buffer (excluding null terminator)
 */
size_t rift_regex_flags_to_modifiers(rift_regex_flags_t flags, char *buffer, size_t buffer_size);

/**
 * @brief Get a string description of a flag
 *
 * @param flag The flag to describe
 * @return A string description of the flag
 */
const char *rift_regex_flag_description(rift_regex_flags_t flag);

/**
 * @brief Check if two flags are compatible
 *
 * Some flags are mutually exclusive, such as different newline modes.
 * This function checks if two flags can be used together.
 *
 * @param flag1 First flag
 * @param flag2 Second flag
 * @return true if the flags are compatible, false otherwise
 */
bool rift_regex_flags_are_compatible(rift_regex_flags_t flag1, rift_regex_flags_t flag2);

/**
 * @brief Resolve conflicts between flags
 *
 * If the provided flags contain conflicts (e.g., multiple newline modes),
 * this function resolves them according to priority rules.
 *
 * @param flags The flags to resolve
 * @return The resolved flags
 */
rift_regex_flags_t rift_regex_resolve_flag_conflicts(rift_regex_flags_t flags);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_AUTOMATON_FLAGS_H */