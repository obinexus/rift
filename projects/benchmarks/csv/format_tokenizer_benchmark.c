/**
 * @file format_tokenizer_benchmark.c
 * @brief Benchmark application for LibRift demonstrating thread-safe parsing of JSON and CSV
 *
 * This application demonstrates the capabilities of LibRift"s thread-safe components
 * and R"" syntax for parsing and tokenizing JSON and CSV data formats.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "librift/regex/automaton/automaton.h"
#include "librift/regex/engine/matcher.h"
#include "librift/regex/engine/pattern.h"
#include "librift/regex/errors/regex_error.h"
#include "librift/regex/runtime/safe_backtracker.h"
#include "librift/regex/runtime/thread_safe_context.h"
#include "librift/regex/syntax/integration.h"

/* Maximum number of tokens to extract from each file */
#define MAX_TOKENS 10000

/* Number of parser threads */
#define NUM_PARSER_THREADS 4

/* Maximum backtracking depth */
#define MAX_BACKTRACK_DEPTH 800

/* Token types for our parsers */
typedef enum {
    TOKEN_UNKNOWN = 0,
    /* JSON token types */
    TOKEN_JSON_OPEN_OBJECT,
    TOKEN_JSON_CLOSE_OBJECT,
    TOKEN_JSON_OPEN_ARRAY,
    TOKEN_JSON_CLOSE_ARRAY,
    TOKEN_JSON_STRING,
    TOKEN_JSON_NUMBER,
    TOKEN_JSON_BOOLEAN,
    TOKEN_JSON_NULL,
    TOKEN_JSON_COLON,
    TOKEN_JSON_COMMA,
    /* CSV token types */
    TOKEN_CSV_FIELD,
    TOKEN_CSV_QUOTED_FIELD,
    TOKEN_CSV_DELIMITER,
    TOKEN_CSV_NEWLINE
} token_type_t;

/* Structure to represent a token */
typedef struct {
    token_type_t type;
    char *value;
    size_t position;
    size_t length;
} token_t;

/* Structure to hold token extraction results */
typedef struct {
    token_t *tokens;
    size_t token_count;
    size_t capacity;
} token_list_t;

/* Parser thread arguments */
typedef struct {
    int thread_id;
    const char *data;
    size_t data_length;
    size_t start_offset;
    size_t end_offset;
    rift_regex_thread_safe_context_t *context;
    rift_regex_pattern_t *pattern;
    token_list_t *token_list;
    pthread_mutex_t *token_list_mutex;
    bool is_json;
    bool success;
    double elapsed_time;
} parser_thread_arg_t;

/* Function prototypes */
token_list_t *create_token_list(size_t initial_capacity);
void free_token_list(token_list_t *list);
bool add_token(token_list_t *list, token_type_t type, const char *value, size_t position,
               size_t length, pthread_mutex_t *mutex);
rift_regex_pattern_t *create_json_pattern(rift_regex_error_t *error);
rift_regex_pattern_t *create_csv_pattern(rift_regex_error_t *error);
void *parser_thread_function(void *arg);
bool token_extractor_callback(rift_regex_matcher_context_t *context, void *user_data,
                              rift_regex_error_t *error);
token_type_t determine_json_token_type(const char *token);
token_type_t determine_csv_token_type(const char *token);
void print_token_stats(token_list_t *list, bool is_json, double elapsed_time);
char *read_file(const char *filename, size_t *file_size);

/**
 * @brief Main function
 */
int
main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Usage: %s <json_file> <csv_file>\n", argv[0]);
        return 1;
    }

    /* Read input files */
    const char *json_filename = argv[1];
    const char *csv_filename = argv[2];

    size_t json_size = 0;
    char *json_data = read_file(json_filename, &json_size);
    if (!json_data) {
        fprintf(stderr, "Error: Failed to read JSON file " % s "\n", json_filename);
        return 1;
    }

    size_t csv_size = 0;
    char *csv_data = read_file(csv_filename, &csv_size);
    if (!csv_data) {
        fprintf(stderr, "Error: Failed to read CSV file " % s "\n", csv_filename);
        free(json_data);
        return 1;
    }

    printf("Loaded:\n");
    printf("- JSON file: %s (%zu bytes)\n", json_filename, json_size);
    printf("- CSV file: %s (%zu bytes)\n", csv_filename, csv_size);

    /* Create token lists */
    token_list_t *json_tokens = create_token_list(MAX_TOKENS);
    token_list_t *csv_tokens = create_token_list(MAX_TOKENS);

    if (!json_tokens || !csv_tokens) {
        fprintf(stderr, "Error: Failed to create token lists\n");
        free(json_data);
        free(csv_data);
        free_token_list(json_tokens);
        free_token_list(csv_tokens);
        return 1;
    }

    /* Create mutexes for token lists */
    pthread_mutex_t json_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t csv_mutex = PTHREAD_MUTEX_INITIALIZER;

    /* Create patterns */
    rift_regex_error_t error = {0};
    rift_regex_pattern_t *json_pattern = create_json_pattern(&error);
    if (!json_pattern) {
        fprintf(stderr, "Error: Failed to create JSON pattern: %s\n", error.message);
        free(json_data);
        free(csv_data);
        free_token_list(json_tokens);
        free_token_list(csv_tokens);
        return 1;
    }

    error.code = RIFT_REGEX_ERROR_NONE;
    error.message[0] = "\0";
    rift_regex_pattern_t *csv_pattern = create_csv_pattern(&error);
    if (!csv_pattern) {
        fprintf(stderr, "Error: Failed to create CSV pattern: %s\n", error.message);
        free(json_data);
        free(csv_data);
        free_token_list(json_tokens);
        free_token_list(csv_tokens);
        rift_regex_pattern_free(json_pattern);
        return 1;
    }

    /* Get the automatons from the patterns */
    rift_regex_automaton_t *json_automaton = rift_regex_pattern_get_automaton(json_pattern);
    rift_regex_automaton_t *csv_automaton = rift_regex_pattern_get_automaton(csv_pattern);

    /* Create thread-safe contexts */
    rift_regex_thread_safe_context_t *json_context =
        rift_thread_safe_context_create(json_automaton, 10, MAX_BACKTRACK_DEPTH);
    rift_regex_thread_safe_context_t *csv_context =
        rift_thread_safe_context_create(csv_automaton, 10, MAX_BACKTRACK_DEPTH);

    if (!json_context || !csv_context) {
        fprintf(stderr, "Error: Failed to create thread-safe contexts\n");
        free(json_data);
        free(csv_data);
        free_token_list(json_tokens);
        free_token_list(csv_tokens);
        rift_regex_pattern_free(json_pattern);
        rift_regex_pattern_free(csv_pattern);
        if (json_context)
            rift_thread_safe_context_free(json_context);
        if (csv_context)
            rift_thread_safe_context_free(csv_context);
        return 1;
    }

    /* Create parser threads */
    pthread_t json_threads[NUM_PARSER_THREADS];
    pthread_t csv_threads[NUM_PARSER_THREADS];
    parser_thread_arg_t json_thread_args[NUM_PARSER_THREADS];
    parser_thread_arg_t csv_thread_args[NUM_PARSER_THREADS];

    printf("\nStarting %d parser threads for each file format...\n", NUM_PARSER_THREADS);

    /* Calculate chunk sizes for each thread */
    size_t json_chunk_size = json_size / NUM_PARSER_THREADS;
    size_t csv_chunk_size = csv_size / NUM_PARSER_THREADS;

    /* Create and start JSON parser threads */
    for (int i = 0; i < NUM_PARSER_THREADS; i++) {
        json_thread_args[i].thread_id = i;
        json_thread_args[i].data = json_data;
        json_thread_args[i].data_length = json_size;
        json_thread_args[i].start_offset = i * json_chunk_size;
        json_thread_args[i].end_offset =
            (i == NUM_PARSER_THREADS - 1) ? json_size : (i + 1) * json_chunk_size;
        json_thread_args[i].context = json_context;
        json_thread_args[i].pattern = json_pattern;
        json_thread_args[i].token_list = json_tokens;
        json_thread_args[i].token_list_mutex = &json_mutex;
        json_thread_args[i].is_json = true;
        json_thread_args[i].success = false;
        json_thread_args[i].elapsed_time = 0.0;

        /* Set input for this thread */
        if (!rift_thread_safe_context_set_input(
                json_context, json_data + json_thread_args[i].start_offset,
                json_thread_args[i].end_offset - json_thread_args[i].start_offset)) {
            fprintf(stderr, "Error: Failed to set input for JSON thread %d\n", i);
            continue;
        }

        /* Create thread */
        if (pthread_create(&json_threads[i], NULL, parser_thread_function, &json_thread_args[i]) !=
            0) {
            fprintf(stderr, "Error: Failed to create JSON thread %d\n", i);
            continue;
        }
    }

    /* Create and start CSV parser threads */
    for (int i = 0; i < NUM_PARSER_THREADS; i++) {
        csv_thread_args[i].thread_id = i;
        csv_thread_args[i].data = csv_data;
        csv_thread_args[i].data_length = csv_size;
        csv_thread_args[i].start_offset = i * csv_chunk_size;
        csv_thread_args[i].end_offset =
            (i == NUM_PARSER_THREADS - 1) ? csv_size : (i + 1) * csv_chunk_size;
        csv_thread_args[i].context = csv_context;
        csv_thread_args[i].pattern = csv_pattern;
        csv_thread_args[i].token_list = csv_tokens;
        csv_thread_args[i].token_list_mutex = &csv_mutex;
        csv_thread_args[i].is_json = false;
        csv_thread_args[i].success = false;
        csv_thread_args[i].elapsed_time = 0.0;

        /* Set input for this thread */
        if (!rift_thread_safe_context_set_input(
                csv_context, csv_data + csv_thread_args[i].start_offset,
                csv_thread_args[i].end_offset - csv_thread_args[i].start_offset)) {
            fprintf(stderr, "Error: Failed to set input for CSV thread %d\n", i);
            continue;
        }

        /* Create thread */
        if (pthread_create(&csv_threads[i], NULL, parser_thread_function, &csv_thread_args[i]) !=
            0) {
            fprintf(stderr, "Error: Failed to create CSV thread %d\n", i);
            continue;
        }
    }

    /* Wait for all threads to complete */
    for (int i = 0; i < NUM_PARSER_THREADS; i++) {
        pthread_join(json_threads[i], NULL);
        pthread_join(csv_threads[i], NULL);
    }

    /* Calculate total elapsed time */
    double json_elapsed_time = 0.0;
    double csv_elapsed_time = 0.0;

    for (int i = 0; i < NUM_PARSER_THREADS; i++) {
        json_elapsed_time += json_thread_args[i].elapsed_time;
        csv_elapsed_time += csv_thread_args[i].elapsed_time;
    }

    json_elapsed_time /= NUM_PARSER_THREADS;
    csv_elapsed_time /= NUM_PARSER_THREADS;

    /* Print results */
    printf("\nParsing Results:\n");
    printf("=================\n\n");

    print_token_stats(json_tokens, true, json_elapsed_time);
    print_token_stats(csv_tokens, false, csv_elapsed_time);

    /* Clean up */
    free(json_data);
    free(csv_data);
    free_token_list(json_tokens);
    free_token_list(csv_tokens);
    rift_regex_pattern_free(json_pattern);
    rift_regex_pattern_free(csv_pattern);
    rift_thread_safe_context_free(json_context);
    rift_thread_safe_context_free(csv_context);
    pthread_mutex_destroy(&json_mutex);
    pthread_mutex_destroy(&csv_mutex);

    return 0;
}

/**
 * @brief Create a token list
 */
token_list_t *
create_token_list(size_t initial_capacity)
{
    token_list_t *list = (token_list_t *)malloc(sizeof(token_list_t));
    if (!list) {
        return NULL;
    }

    list->tokens = (token_t *)malloc(initial_capacity * sizeof(token_t));
    if (!list->tokens) {
        free(list);
        return NULL;
    }

    list->token_count = 0;
    list->capacity = initial_capacity;

    return list;
}

/**
 * @brief Free a token list
 */
void
free_token_list(token_list_t *list)
{
    if (!list) {
        return;
    }

    if (list->tokens) {
        for (size_t i = 0; i < list->token_count; i++) {
            free(list->tokens[i].value);
        }
        free(list->tokens);
    }

    free(list);
}

/**
 * @brief Add a token to the list
 */
bool
add_token(token_list_t *list, token_type_t type, const char *value, size_t position, size_t length,
          pthread_mutex_t *mutex)
{
    if (!list || !value) {
        return false;
    }

    /* Lock the mutex if provided */
    if (mutex) {
        pthread_mutex_lock(mutex);
    }

    /* Check if we need to expand the list */
    if (list->token_count >= list->capacity) {
        token_t *new_tokens =
            (token_t *)realloc(list->tokens, list->capacity * 2 * sizeof(token_t));
        if (!new_tokens) {
            if (mutex) {
                pthread_mutex_unlock(mutex);
            }
            return false;
        }

        list->tokens = new_tokens;
        list->capacity *= 2;
    }

    /* Add the token */
    list->tokens[list->token_count].type = type;
    list->tokens[list->token_count].value = strdup(value);
    list->tokens[list->token_count].position = position;
    list->tokens[list->token_count].length = length;
    list->token_count++;

    /* Unlock the mutex if provided */
    if (mutex) {
        pthread_mutex_unlock(mutex);
    }

    return true;
}

/**
 * @brief Create a pattern for JSON parsing
 *
 * This function demonstrates the use of R"" syntax for creating
 * regex patterns that are much more readable than traditional regex.
 */
rift_regex_pattern_t *
create_json_pattern(rift_regex_error_t *error)
{
    /* Using R"" syntax for cleaner pattern definition */
    const char *pattern =
        R"(\s*(?:                                # Whitespace followed by one of:
            ([\{\}])                           |  # Curly braces (object start/end)
            ([\[\]])                           |  # Square brackets (array start/end)
            ("(?:[^"\\]|\\.)*")               |  # String with escapes
            (-?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?) |  # Number (int or float)
            (true|false|null)                  |  # Boolean or null literal
            ([,])                              |  # Comma
            (:)                                   # Colon
          ))";

    /* Note: In traditional regex, this would be much harder to read:
     * "\\s*(?:([\\{\\}])|([\\[\\]])|(\\"(?:[^\\"\\\\]|\\\\.)*\\")|(-?\\d+(?:\\.\\d+)?(?:[eE][+-]?\\d+)?)|([,])|(true|false|null)|([:]))"
     */

    /* Set flags to enable R"" syntax */
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_EXTENDED | RIFT_REGEX_FLAG_RIFT_SYNTAX;

    /* Compile the pattern */
    return rift_regex_compile(pattern, flags, error);
}

/**
 * @brief Create a pattern for CSV parsing
 */
rift_regex_pattern_t *
create_csv_pattern(rift_regex_error_t *error)
{
    /* Using R"" syntax for cleaner pattern definition */
    const char *pattern =
        R"((?:                                   # One of:
            ("(?:[^"]|"")*")                   |  # Quoted field with optional doubled quotes
            ([^",\r\n]+)                       |  # Unquoted field (no commas or newlines)
            (,)                                |  # Delimiter (comma)
            (\r\n|\n|\r)                          # Newline (various formats)
          ))";

    /* Set flags to enable R"" syntax and case-insensitive matching */
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_EXTENDED | RIFT_REGEX_FLAG_RIFT_SYNTAX;

    /* Compile the pattern */
    return rift_regex_compile(pattern, flags, error);
}

/**
 * @brief Parser thread function
 */
void *
parser_thread_function(void *arg)
{
    parser_thread_arg_t *thread_arg = (parser_thread_arg_t *)arg;
    rift_regex_error_t error = {0};

    /* Record start time */
    clock_t start_time = clock();

    /* Execute the token extractor with thread-local context */
    thread_arg->success = rift_thread_safe_context_execute(
        thread_arg->context, token_extractor_callback, thread_arg, &error);

    /* Record end time */
    clock_t end_time = clock();
    thread_arg->elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    if (!thread_arg->success) {
        printf("Thread %d: Error: %s\n", thread_arg->thread_id, error.message);
    }

    return NULL;
}

/**
 * @brief Token extraction callback
 *
 * This function is executed with a thread-local matcher context. It
 * handles the actual token extraction within the thread.
 */
bool
token_extractor_callback(rift_regex_matcher_context_t *context, void *user_data,
                         rift_regex_error_t *error)
{
    parser_thread_arg_t *arg = (parser_thread_arg_t *)user_data;

    /* Set the current position to the start of the chunk */
    rift_matcher_context_set_position(context, 0);

    /* Extract tokens until end of input */
    while (!rift_matcher_context_is_at_end(context)) {
        /* Get the current position */
        size_t start_pos = rift_matcher_context_get_position(context);

        /* Try to match the pattern */
        rift_regex_match_result_t *result = rift_regex_matcher_match(
            arg->pattern, rift_matcher_context_get_input(context) + start_pos,
            rift_matcher_context_get_input_length(context) - start_pos, error);

        if (!result) {
            /* No match, advance to the next character */
            if (!rift_matcher_context_advance(context)) {
                break;
            }
            continue;
        }

        /* Extract the token value */
        if (result->group_count >= 1) {
            for (size_t i = 1; i < result->group_count; i++) {
                if (result->groups[i].value) {
                    /* Determine token type */
                    token_type_t type = arg->is_json
                                            ? determine_json_token_type(result->groups[i].value)
                                            : determine_csv_token_type(result->groups[i].value);

                    /* Add the token to the list */
                    if (type != TOKEN_UNKNOWN) {
                        add_token(arg->token_list, type, result->groups[i].value,
                                  arg->start_offset + start_pos + result->groups[i].start,
                                  strlen(result->groups[i].value), arg->token_list_mutex);
                    }
                    break;
                }
            }
        }

        /* Advance to the end of the match */
        rift_matcher_context_set_position(context, start_pos + result->end_pos);

        /* Free the match result */
        rift_match_result_free(result);
    }

    return true;
}

/**
 * @brief Determine the type of a JSON token
 */
token_type_t
determine_json_token_type(const char *token)
{
    if (!token) {
        return TOKEN_UNKNOWN;
    }

    if (token[0] == "{")
        return TOKEN_JSON_OPEN_OBJECT;
    if (token[0] == "}")
        return TOKEN_JSON_CLOSE_OBJECT;
    if (token[0] == "[")
        return TOKEN_JSON_OPEN_ARRAY;
    if (token[0] == "]")
        return TOKEN_JSON_CLOSE_ARRAY;
    if (token[0] == "'")
        return TOKEN_JSON_STRING;
    if (token[0] == ":")
        return TOKEN_JSON_COLON;
    if (token[0] == ",")
        return TOKEN_JSON_COMMA;

    if (isdigit(token[0]) || token[0] == "-")
        return TOKEN_JSON_NUMBER;

    if (strcmp(token, "true") == 0 || strcmp(token, "false") == 0)
        return TOKEN_JSON_BOOLEAN;
    if (strcmp(token, "null") == 0)
        return TOKEN_JSON_NULL;

    return TOKEN_UNKNOWN;
}

/**
 * @brief Determine the type of a CSV token
 */
token_type_t
determine_csv_token_type(const char *token)
{
    if (!token) {
        return TOKEN_UNKNOWN;
    }

    if (token[0] == "'")
        return TOKEN_CSV_QUOTED_FIELD;
    if (token[0] == ",")
        return TOKEN_CSV_DELIMITER;
    if (token[0] == "\r" || token[0] == "\n")
        return TOKEN_CSV_NEWLINE;

    return TOKEN_CSV_FIELD;
}

/**
 * @brief Print statistics about the extracted tokens
 */
void
print_token_stats(token_list_t *list, bool is_json, double elapsed_time)
{
    if (!list) {
        return;
    }

    printf("%s Parsing Results:\n", is_json ? "JSON" : "CSV");
    printf("------------------------\n");
    printf("Total tokens: %zu\n", list->token_count);
    printf("Elapsed time: %.4f seconds\n", elapsed_time);
    printf("Tokens per second: %.2f\n", list->token_count / elapsed_time);

    /* Count token types */
    size_t type_counts[20] = {0};

    for (size_t i = 0; i < list->token_count; i++) {
        if (list->tokens[i].type < 20) {
            type_counts[list->tokens[i].type]++;
        }
    }

    printf("\nToken type distribution:\n");

    if (is_json) {
        printf("Object open {: %zu\n", type_counts[TOKEN_JSON_OPEN_OBJECT]);
        printf("Object close }: %zu\n", type_counts[TOKEN_JSON_CLOSE_OBJECT]);
        printf("Array open [: %zu\n", type_counts[TOKEN_JSON_OPEN_ARRAY]);
        printf("Array close ]: %zu\n", type_counts[TOKEN_JSON_CLOSE_ARRAY]);
        printf("String: %zu\n", type_counts[TOKEN_JSON_STRING]);
        printf("Number: %zu\n", type_counts[TOKEN_JSON_NUMBER]);
        printf("Boolean: %zu\n", type_counts[TOKEN_JSON_BOOLEAN]);
        printf("Null: %zu\n", type_counts[TOKEN_JSON_NULL]);
        printf("Colon: %zu\n", type_counts[TOKEN_JSON_COLON]);
        printf("Comma: %zu\n", type_counts[TOKEN_JSON_COMMA]);
    } else {
        printf("Field: %zu\n", type_counts[TOKEN_CSV_FIELD]);
        printf("Quoted field: %zu\n", type_counts[TOKEN_CSV_QUOTED_FIELD]);
        printf("Delimiter: %zu\n", type_counts[TOKEN_CSV_DELIMITER]);
        printf("Newline: %zu\n", type_counts[TOKEN_CSV_NEWLINE]);
    }

    /* Print some sample tokens */
    printf("\nSample tokens:\n");

    size_t sample_count = list->token_count > 5 ? 5 : list->token_count;
    for (size_t i = 0; i < sample_count; i++) {
        printf("- Type: %d, Value: " % .30s % s "\n", list->tokens[i].type, list->tokens[i].value,
               strlen(list->tokens[i].value) > 30 ? "..." : "");
    }

    printf("\n");
}

/**
 * @brief Read a file into memory
 */
char *
read_file(const char *filename, size_t *file_size)
{
    FILE *file = fopen(filename, "rb");
    if (!file) {
        return NULL;
    }

    /* Get file size */
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    /* Allocate memory for file contents */
    char *buffer = (char *)malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    /* Read file into buffer */
    size_t read_size = fread(buffer, 1, size, file);
    buffer[read_size] = "\0";

    /* Set file size output parameter */
    if (file_size) {
        *file_size = read_size;
    }

    fclose(file);
    return buffer;
}