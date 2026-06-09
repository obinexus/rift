/**
 * @file log_analyzer.c
 * @brief A log file analyzer that uses LibRift's r'' syntax for advanced pattern matching
 *
 * This program demonstrates LibRift's regex automaton capabilities for extracting
 * structured data from log files containing heterogeneous entry formats.
 *
 * @copyright Copyright (c) 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "librift/regex/automaton/automaton.h"
#include "librift/regex/automaton/flags.h"
#include "librift/regex/engine/matcher.h"
#include "librift/regex/engine/pattern.h"
#include "librift/regex/runtime/context.h"
#include "librift/regex/runtime/groups.h"

/* Define log entry types */
typedef enum {
    LOG_TYPE_UNKNOWN = 0,
    LOG_TYPE_ERROR,
    LOG_TYPE_WARNING,
    LOG_TYPE_INFO,
    LOG_TYPE_DEBUG,
    LOG_TYPE_TRANSACTION
} log_entry_type_t;

/* Structure to hold parsed log entry data */
typedef struct {
    log_entry_type_t type;
    char timestamp[32];
    char module[64];
    char message[512];
    struct {
        char user_id[64];
        char transaction_id[64];
        double amount;
        char status[32];
    } transaction_data;
} log_entry_t;

/* Function prototypes */
bool parse_log_entry(const char *line, log_entry_t *entry);
bool extract_timestamp(const char *line, char *timestamp, size_t max_len);
log_entry_type_t determine_log_type(const char *line);
bool extract_transaction_data(const char *line, log_entry_t *entry);
void print_log_entry(const log_entry_t *entry);
void analyze_logs(FILE *log_file);

/**
 * @brief Main entry point
 */
int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <log_file>\n", argv[0]);
        return 1;
    }

    const char *log_file_path = argv[1];
    FILE *log_file = fopen(log_file_path, "r");

    if (!log_file) {
        fprintf(stderr, "Error: Could not open log file '%s'\n", log_file_path);
        return 1;
    }

    printf("Analyzing log file: %s\n\n", log_file_path);
    analyze_logs(log_file);

    fclose(log_file);
    return 0;
}

/**
 * @brief Analyze all entries in a log file
 */
void
analyze_logs(FILE *log_file)
{
    char line[1024];
    size_t line_count = 0;
    size_t error_count = 0;
    size_t warning_count = 0;
    size_t transaction_count = 0;
    double total_transaction_amount = 0.0;
    size_t successful_transactions = 0;
    size_t failed_transactions = 0;

    // Process each line of the log file
    while (fgets(line, sizeof(line), log_file)) {
        line_count++;

        // Remove trailing newline if present
        size_t len = strlen(line);
        if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
        }

        // Parse the log entry
        log_entry_t entry;
        if (parse_log_entry(line, &entry)) {
            // Update statistics based on entry type
            switch (entry.type) {
            case LOG_TYPE_ERROR:
                error_count++;
                break;
            case LOG_TYPE_WARNING:
                warning_count++;
                break;
            case LOG_TYPE_TRANSACTION:
                transaction_count++;
                total_transaction_amount += entry.transaction_data.amount;

                if (strcmp(entry.transaction_data.status, "SUCCESS") == 0) {
                    successful_transactions++;
                } else if (strcmp(entry.transaction_data.status, "FAILED") == 0) {
                    failed_transactions++;
                }
                break;
            default:
                break;
            }

            // Print detailed information for errors and transactions
            if (entry.type == LOG_TYPE_ERROR || entry.type == LOG_TYPE_TRANSACTION) {
                print_log_entry(&entry);
            }
        }
    }

    // Print summary statistics
    printf("\n--- Log Analysis Summary ---\n");
    printf("Total log entries: %zu\n", line_count);
    printf("Error entries: %zu\n", error_count);
    printf("Warning entries: %zu\n", warning_count);
    printf("Transaction entries: %zu\n", transaction_count);
    printf("Total transaction amount: $%.2f\n", total_transaction_amount);
    printf("Successful transactions: %zu\n", successful_transactions);
    printf("Failed transactions: %zu\n", failed_transactions);
}

/**
 * @brief Parse a log entry line into a structured format
 */
bool
parse_log_entry(const char *line, log_entry_t *entry)
{
    if (!line || !entry) {
        return false;
    }

    // Initialize entry
    memset(entry, 0, sizeof(log_entry_t));

    // Extract timestamp first
    if (!extract_timestamp(line, entry->timestamp, sizeof(entry->timestamp))) {
        return false;
    }

    // Determine log entry type
    entry->type = determine_log_type(line);

    // Use LibRift's r'' syntax for complex pattern matching
    rift_regex_error_t error;
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_RIFT_SYNTAX;

    // Pattern to extract module and message
    rift_regex_pattern_t *module_pattern = rift_regex_compile(
        "^\\[\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\] \\[([A-Za-z0-9_-]+)\\] \\[.*?\\]: (.+)$",
        flags, &error);

    if (!module_pattern) {
        return false;
    }

    // Create matcher for the pattern
    rift_regex_matcher_t *matcher = rift_matcher_create(module_pattern, RIFT_MATCHER_OPTION_NONE);
    if (!matcher) {
        rift_regex_pattern_free(module_pattern);
        return false;
    }

    // Set the input string
    if (!rift_matcher_set_input(matcher, line, strlen(line))) {
        rift_matcher_free(matcher);
        rift_regex_pattern_free(module_pattern);
        return false;
    }

    // Match the pattern
    rift_regex_match_t match;
    bool result = rift_matcher_matches(matcher, &match);

    if (result && match.group_count >= 2) {
        // Copy module name (group 1) and message (group 2)
        strncpy(entry->module, match.groups[1].value, sizeof(entry->module) - 1);
        strncpy(entry->message, match.groups[2].value, sizeof(entry->message) - 1);
    }

    // Clean up resources
    rift_match_free(&match);
    rift_matcher_free(matcher);
    rift_regex_pattern_free(module_pattern);

    // For transaction logs, extract additional data
    if (entry->type == LOG_TYPE_TRANSACTION) {
        extract_transaction_data(line, entry);
    }

    return true;
}

/**
 * @brief Extract timestamp from log line
 */
bool
extract_timestamp(const char *line, char *timestamp, size_t max_len)
{
    rift_regex_error_t error;
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_RIFT_SYNTAX;

    // Pattern to extract timestamp
    rift_regex_pattern_t *timestamp_pattern =
        rift_regex_compile("^\\[(\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2})\\]", flags, &error);

    if (!timestamp_pattern) {
        return false;
    }

    // Create matcher for the pattern
    rift_regex_matcher_t *matcher =
        rift_matcher_create(timestamp_pattern, RIFT_MATCHER_OPTION_NONE);
    if (!matcher) {
        rift_regex_pattern_free(timestamp_pattern);
        return false;
    }

    // Set the input string
    if (!rift_matcher_set_input(matcher, line, strlen(line))) {
        rift_matcher_free(matcher);
        rift_regex_pattern_free(timestamp_pattern);
        return false;
    }

    // Match the pattern
    rift_regex_match_t match;
    bool result = rift_matcher_find_next(matcher, &match);

    if (result && match.group_count >= 1) {
        // Copy timestamp (group 1)
        strncpy(timestamp, match.groups[1].value, max_len - 1);
        timestamp[max_len - 1] = '\0';
    } else {
        // Default timestamp if not found
        strncpy(timestamp, "Unknown", max_len - 1);
        timestamp[max_len - 1] = '\0';
        result = false;
    }

    // Clean up resources
    rift_match_free(&match);
    rift_matcher_free(matcher);
    rift_regex_pattern_free(timestamp_pattern);

    return result;
}

/**
 * @brief Determine log entry type based on content
 */
log_entry_type_t
determine_log_type(const char *line)
{
    rift_regex_error_t error;
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_RIFT_SYNTAX;

    // Patterns for different log types
    const char *patterns[] = {
        "^\\[.*?\\] \\[.*?\\] \\[ERROR\\]:", "^\\[.*?\\] \\[.*?\\] \\[WARN\\]:",
        "^\\[.*?\\] \\[.*?\\] \\[INFO\\]:", "^\\[.*?\\] \\[.*?\\] \\[DEBUG\\]:",
        "^\\[.*?\\] \\[TRANSACTION\\] \\[.*?\\]:"};

    log_entry_type_t types[] = {LOG_TYPE_ERROR, LOG_TYPE_WARNING, LOG_TYPE_INFO, LOG_TYPE_DEBUG,
                                LOG_TYPE_TRANSACTION};

    // Try each pattern in order
    for (size_t i = 0; i < sizeof(patterns) / sizeof(patterns[0]); i++) {
        rift_regex_pattern_t *pattern = rift_regex_compile(patterns[i], flags, &error);
        if (!pattern) {
            continue;
        }

        rift_regex_matcher_t *matcher = rift_matcher_create(pattern, RIFT_MATCHER_OPTION_NONE);
        if (!matcher) {
            rift_regex_pattern_free(pattern);
            continue;
        }

        if (!rift_matcher_set_input(matcher, line, strlen(line))) {
            rift_matcher_free(matcher);
            rift_regex_pattern_free(pattern);
            continue;
        }

        bool result = rift_matcher_find_next(matcher, NULL);

        rift_matcher_free(matcher);
        rift_regex_pattern_free(pattern);

        if (result) {
            return types[i];
        }
    }

    return LOG_TYPE_UNKNOWN;
}

/**
 * @brief Extract transaction-specific data from a log line
 */
bool
extract_transaction_data(const char *line __attribute__((unused)), log_entry_t *entry)
{
    rift_regex_error_t error;
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_RIFT_SYNTAX;

    // Pattern to extract transaction data
    // This demonstrates the power of the r'' syntax with a complex pattern
    rift_regex_pattern_t *transaction_pattern =
        rift_regex_compile("TRANSACTION: user=([A-Za-z0-9_-]+), txn_id=([A-Za-z0-9-]+), "
                           "amount=\\$([\\d\\.]+), status=([A-Z]+)",
                           flags, &error);

    if (!transaction_pattern) {
        return false;
    }

    // Create matcher for the pattern
    rift_regex_matcher_t *matcher =
        rift_matcher_create(transaction_pattern, RIFT_MATCHER_OPTION_NONE);
    if (!matcher) {
        rift_regex_pattern_free(transaction_pattern);
        return false;
    }

    // Set the input string
    if (!rift_matcher_set_input(matcher, entry->message, strlen(entry->message))) {
        rift_matcher_free(matcher);
        rift_regex_pattern_free(transaction_pattern);
        return false;
    }

    // Match the pattern
    rift_regex_match_t match;
    bool result = rift_matcher_find_next(matcher, &match);

    if (result && match.group_count >= 4) {
        // Copy transaction data from capture groups
        strncpy(entry->transaction_data.user_id, match.groups[1].value,
                sizeof(entry->transaction_data.user_id) - 1);

        strncpy(entry->transaction_data.transaction_id, match.groups[2].value,
                sizeof(entry->transaction_data.transaction_id) - 1);

        entry->transaction_data.amount = atof(match.groups[3].value);

        strncpy(entry->transaction_data.status, match.groups[4].value,
                sizeof(entry->transaction_data.status) - 1);
    } else {
        result = false;
    }

    // Clean up resources
    rift_match_free(&match);
    rift_matcher_free(matcher);
    rift_regex_pattern_free(transaction_pattern);

    return result;
}

/**
 * @brief Print detailed information about a log entry
 */
void
print_log_entry(const log_entry_t *entry)
{
    if (!entry) {
        return;
    }

    // Print common information
    printf("-------------------------------------------------------\n");
    printf("Timestamp: %s\n", entry->timestamp);
    printf("Module: %s\n", entry->module);

    // Print type-specific information
    switch (entry->type) {
    case LOG_TYPE_ERROR:
        printf("Type: ERROR\n");
        printf("Message: %s\n", entry->message);
        break;

    case LOG_TYPE_WARNING:
        printf("Type: WARNING\n");
        printf("Message: %s\n", entry->message);
        break;

    case LOG_TYPE_INFO:
        printf("Type: INFO\n");
        printf("Message: %s\n", entry->message);
        break;

    case LOG_TYPE_DEBUG:
        printf("Type: DEBUG\n");
        printf("Message: %s\n", entry->message);
        break;

    case LOG_TYPE_TRANSACTION:
        printf("Type: TRANSACTION\n");
        printf("User ID: %s\n", entry->transaction_data.user_id);
        printf("Transaction ID: %s\n", entry->transaction_data.transaction_id);
        printf("Amount: $%.2f\n", entry->transaction_data.amount);
        printf("Status: %s\n", entry->transaction_data.status);
        break;

    default:
        printf("Type: UNKNOWN\n");
        printf("Message: %s\n", entry->message);
        break;
    }
}