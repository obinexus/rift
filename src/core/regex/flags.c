/**
core/regex/flags.h
tmp/librift_analysis/librift-4933472-W/backup/v1/src/automaton/flags.h
tmp/librift_analysis/librift-4933472-W/src/core/automaton/flags.h
tmp/librift_analysis/librift-4933472-W/src/core/regex/flags.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/automaton/flags.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/automaton/flags.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/regex/flags.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/automaton/flags.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/automaton/flags.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/regex/flags.h"
core/automaton/flags.h"
 * @file flags.c
 * @brief Implementation of regex flags for the LibRift regex engine
 *
 * This file implements the various flag-related functions that control
 * the behavior of the regex engine, affecting pattern compilation and execution.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/automaton/flags.h
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


/* Flag descriptions lookup table */
static const struct {
    rift_regex_flags_t flag;
    const char *description;
} flag_descriptions[] =
    {
        {RIFT_REGEX_FLAG_NONE, "No flags"},
        {RIFT_REGEX_FLAG_CASE_INSENSITIVE, "Case insensitive matching"},
        {RIFT_REGEX_FLAG_MULTILINE, "^ and $ match start/end of line"},
        {RIFT_REGEX_FLAG_DOTALL, ". matches newline too"},
        {RIFT_REGEX_FLAG_EXTENDED, "Ignore whitespace and comments"},
        {RIFT_REGEX_FLAG_ANCHORED, "Pattern must match at start of string"},
        {RIFT_REGEX_FLAG_DOLLAR_ENDONLY, "$ only matches at end of string"},
        {RIFT_REGEX_FLAG_UNGREEDY, "Quantifiers are not greedy by default"},
        {RIFT_REGEX_FLAG_UTF8, "Treat pattern and subject as UTF-8"},
        {RIFT_REGEX_FLAG_NO_AUTO_CAPTURE, "Disable numbered capturing parentheses"},
        {RIFT_REGEX_FLAG_NO_UTF8_CHECK, "Skip checks for valid UTF-8"},
        {RIFT_REGEX_FLAG_AUTO_CALLOUT, "Automatically insert callouts"},
        {RIFT_REGEX_FLAG_FIRSTLINE, "Match must be on first line of subject"},
        {RIFT_REGEX_FLAG_DUPNAMES, "Allow duplicate named subpatterns"},
        {RIFT_REGEX_FLAG_NEWLINE_CR, "\\n matches CR only"},
        {RIFT_REGEX_FLAG_NEWLINE_LF, "\\n matches LF only"},
        {RIFT_REGEX_FLAG_NEWLINE_CRLF, "\\n matches CRLF only"},
        {RIFT_REGEX_FLAG_NEWLINE_ANY, "\\n matches any Unicode newline"},
        {RIFT_REGEX_FLAG_NEWLINE_ANYCRLF, "\\n matches CR, LF, or CRLF"},
        {RIFT_REGEX_FLAG_BSR_ANYCRLF, "\\R matches CR, LF, or CRLF only"},
        {RIFT_REGEX_FLAG_BSR_UNICODE, "\\R matches any Unicode newline"},
        {RIFT_REGEX_FLAG_JAVASCRIPT_COMPAT, "JavaScript compatibility mode"},
        {RIFT_REGEX_FLAG_UCP, "Use Unicode properties for \\d, \\w, etc."},
        {RIFT_REGEX_FLAG_NO_START_OPTIMIZE, "Disable start-of-pattern optimizations"},
        {RIFT_REGEX_FLAG_NO_UTF8_CHECK_SUBJECT, "Skip UTF-8 validity check for subject"},
        {RIFT_REGEX_FLAG_NO_AUTO_POSSESS, "Disable auto-possessification optimization"},
        {RIFT_REGEX_FLAG_OPTIMIZE_SIZE, "Optimize for size rather than speed"},
        {RIFT_REGEX_FLAG_DEBUG, "Enable debug mode"},
        {RIFT_REGEX_FLAG_STRICT, "Strict syntax checking"},
        {RIFT_REGEX_FLAG_BACKTRACK_LIMIT, "Enable backtracking limit"},
        {RIFT_REGEX_FLAG_RIFT_SYNTAX, "Use LibRift's extended r'' syntax"},

}

;

/* Number of flag descriptions */
#define NUM_FLAG_DESCRIPTIONS (sizeof(flag_descriptions) / sizeof(flag_descriptions[0]))

/**
 * @brief Convert a regex flavor's modifiers to LibRift flags
 *
 * @param modifiers String containing regex modifiers (e.g., "imsx")
 * @return The corresponding LibRift regex flags
 */
rift_regex_flags_t
rift_regex_flags_from_modifiers(const char *modifiers)
{
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_NONE;

    if (!modifiers) {
        return flags;
    }

    for (const char *c = modifiers; *c; c++) {
        switch (tolower((unsigned char)*c)) {
        case 'i':
            flags |= RIFT_REGEX_FLAG_CASE_INSENSITIVE;
            break;
        case 'm':
            flags |= RIFT_REGEX_FLAG_MULTILINE;
            break;
        case 's':
            flags |= RIFT_REGEX_FLAG_DOTALL;
            break;
        case 'x':
            flags |= RIFT_REGEX_FLAG_EXTENDED;
            break;
        case 'a':
            flags |= RIFT_REGEX_FLAG_ANCHORED;
            break;
        case 'u':
            flags |= RIFT_REGEX_FLAG_UTF8;
            break;
        case 'g':
            flags |= RIFT_REGEX_FLAG_UNGREEDY;
            break;
        case 'j':
            flags |= RIFT_REGEX_FLAG_JAVASCRIPT_COMPAT;
            break;
        case 'p':
            flags |= RIFT_REGEX_FLAG_UCP;
            break;
        case 'd':
            flags |= RIFT_REGEX_FLAG_DEBUG;
            break;
        case 'r':
            flags |= RIFT_REGEX_FLAG_RIFT_SYNTAX;
            break;
        default:
            /* Ignore unknown modifiers */
            break;
        }
    }

    return flags;
}

/**
 * @brief Convert LibRift flags to a regex flavor's modifiers
 *
 * @param flags The LibRift regex flags
 * @param buffer Buffer to store the modifiers string
 * @param buffer_size Size of the buffer
 * @return Number of characters written to the buffer (excluding null terminator)
 */
size_t
rift_regex_flags_to_modifiers(rift_regex_flags_t flags, char *buffer, size_t buffer_size)
{
    if (!buffer || buffer_size == 0) {
        return 0;
    }

    size_t pos = 0;

    /* Add modifier characters based on flags */
    if (flags & RIFT_REGEX_FLAG_CASE_INSENSITIVE && pos < buffer_size - 1) {
        buffer[pos++] = 'i';
    }
    if (flags & RIFT_REGEX_FLAG_MULTILINE && pos < buffer_size - 1) {
        buffer[pos++] = 'm';
    }
    if (flags & RIFT_REGEX_FLAG_DOTALL && pos < buffer_size - 1) {
        buffer[pos++] = 's';
    }
    if (flags & RIFT_REGEX_FLAG_EXTENDED && pos < buffer_size - 1) {
        buffer[pos++] = 'x';
    }
    if (flags & RIFT_REGEX_FLAG_ANCHORED && pos < buffer_size - 1) {
        buffer[pos++] = 'a';
    }
    if (flags & RIFT_REGEX_FLAG_UTF8 && pos < buffer_size - 1) {
        buffer[pos++] = 'u';
    }
    if (flags & RIFT_REGEX_FLAG_UNGREEDY && pos < buffer_size - 1) {
        buffer[pos++] = 'g';
    }
    if (flags & RIFT_REGEX_FLAG_JAVASCRIPT_COMPAT && pos < buffer_size - 1) {
        buffer[pos++] = 'j';
    }
    if (flags & RIFT_REGEX_FLAG_UCP && pos < buffer_size - 1) {
        buffer[pos++] = 'p';
    }
    if (flags & RIFT_REGEX_FLAG_DEBUG && pos < buffer_size - 1) {
        buffer[pos++] = 'd';
    }
    if (flags & RIFT_REGEX_FLAG_RIFT_SYNTAX && pos < buffer_size - 1) {
        buffer[pos++] = 'r';
    }

    /* Ensure null termination */
    buffer[pos] = '\0';

    return pos;
}

/**
 * @brief Get a string description of a flag
 *
 * @param flag The flag to describe
 * @return A string description of the flag
 */
const char *
rift_regex_flag_description(rift_regex_flags_t flag)
{
    /* Handle the special case of no flags */
    if (flag == RIFT_REGEX_FLAG_NONE) {
        return "No flags";
    }

    /* If the flag is a single bit (a power of 2), find its description */
    if ((flag & (flag - 1)) == 0) {
        for (size_t i = 0; i < NUM_FLAG_DESCRIPTIONS; i++) {
            if (flag_descriptions[i].flag == flag) {
                return flag_descriptions[i].description;
            }
        }
    }

    /* For combined flags or unknown flags */
    return "Combined flags";
}

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
bool
rift_regex_flags_are_compatible(rift_regex_flags_t flag1, rift_regex_flags_t flag2)
{
    /* Special cases: No flags or same flags are always compatible */
    if (flag1 == RIFT_REGEX_FLAG_NONE || flag2 == RIFT_REGEX_FLAG_NONE || flag1 == flag2) {
        return true;
    }

    /* Check for mutually exclusive newline mode flags */
    rift_regex_flags_t newline_flags = RIFT_REGEX_FLAG_NEWLINE_CR | RIFT_REGEX_FLAG_NEWLINE_LF |
                                       RIFT_REGEX_FLAG_NEWLINE_CRLF | RIFT_REGEX_FLAG_NEWLINE_ANY |
                                       RIFT_REGEX_FLAG_NEWLINE_ANYCRLF;

    if ((flag1 & newline_flags) && (flag2 & newline_flags) &&
        ((flag1 & newline_flags) != (flag2 & newline_flags))) {
        return false;
    }

    /* Check for mutually exclusive BSR mode flags */
    rift_regex_flags_t bsr_flags = RIFT_REGEX_FLAG_BSR_ANYCRLF | RIFT_REGEX_FLAG_BSR_UNICODE;

    if ((flag1 & bsr_flags) && (flag2 & bsr_flags) &&
        ((flag1 & bsr_flags) != (flag2 & bsr_flags))) {
        return false;
    }

    /* All other flags are compatible */
    return true;
}

/**
 * @brief Resolve conflicts between flags
 *
 * If the provided flags contain conflicts (e.g., multiple newline modes),
 * this function resolves them according to priority rules.
 *
 * @param flags The flags to resolve
 * @return The resolved flags
 */
rift_regex_flags_t
rift_regex_resolve_flag_conflicts(rift_regex_flags_t flags)
{
    rift_regex_flags_t resolved_flags = flags;

    /* Resolve conflicts for newline mode flags */
    rift_regex_flags_t newline_flags = RIFT_REGEX_FLAG_NEWLINE_CR | RIFT_REGEX_FLAG_NEWLINE_LF |
                                       RIFT_REGEX_FLAG_NEWLINE_CRLF | RIFT_REGEX_FLAG_NEWLINE_ANY |
                                       RIFT_REGEX_FLAG_NEWLINE_ANYCRLF;

    if ((resolved_flags & newline_flags) && (resolved_flags & newline_flags) != resolved_flags) {
        /* Clear all newline flags */
        resolved_flags &= ~newline_flags;

        /* Set the highest priority newline flag */
        if (flags & RIFT_REGEX_FLAG_NEWLINE_ANY) {
            resolved_flags |= RIFT_REGEX_FLAG_NEWLINE_ANY;
        } else if (flags & RIFT_REGEX_FLAG_NEWLINE_ANYCRLF) {
            resolved_flags |= RIFT_REGEX_FLAG_NEWLINE_ANYCRLF;
        } else if (flags & RIFT_REGEX_FLAG_NEWLINE_CRLF) {
            resolved_flags |= RIFT_REGEX_FLAG_NEWLINE_CRLF;
        } else if (flags & RIFT_REGEX_FLAG_NEWLINE_LF) {
            resolved_flags |= RIFT_REGEX_FLAG_NEWLINE_LF;
        } else if (flags & RIFT_REGEX_FLAG_NEWLINE_CR) {
            resolved_flags |= RIFT_REGEX_FLAG_NEWLINE_CR;
        }
    }

    /* Resolve conflicts for BSR mode flags */
    rift_regex_flags_t bsr_flags = RIFT_REGEX_FLAG_BSR_ANYCRLF | RIFT_REGEX_FLAG_BSR_UNICODE;

    if ((resolved_flags & bsr_flags) && (resolved_flags & bsr_flags) != resolved_flags) {
        /* Clear all BSR flags */
        resolved_flags &= ~bsr_flags;

        /* Set the highest priority BSR flag */
        if (flags & RIFT_REGEX_FLAG_BSR_UNICODE) {
            resolved_flags |= RIFT_REGEX_FLAG_BSR_UNICODE;
        } else if (flags & RIFT_REGEX_FLAG_BSR_ANYCRLF) {
            resolved_flags |= RIFT_REGEX_FLAG_BSR_ANYCRLF;
        }
    }

    return resolved_flags;
}
