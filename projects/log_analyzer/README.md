# LibRift Log Analyzer

This project demonstrates the power of LibRift's regex automaton-based pattern matching capabilities, specifically highlighting the `r''` syntax extension for regular expressions. The Log Analyzer parses complex log files with heterogeneous entry formats and extracts structured data for analysis.

## Key Features

- Utilizes LibRift's `r''` syntax for advanced pattern matching
- Extracts timestamps, log levels, module names, and messages from log entries
- Provides detailed extraction of transaction data with structured capture groups
- Generates statistical analysis of log entries by type
- Demonstrates advanced pattern-matching techniques for complex log formats

## Technical Implementation

The Log Analyzer leverages several key components of LibRift:

### 1. `r''` Syntax Extension

The program demonstrates the powerful pattern syntax extension provided by LibRift:

```c
// Pattern to extract timestamp with r'' syntax
rift_regex_pattern_t *timestamp_pattern = rift_regex_compile(
    r'^\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\]',
    flags,
    &error
);
```

This syntax provides significant advantages:
- More readable regex patterns (no need for double-escaping)
- Clearer visualization of capture groups
- Better integration with automated code generation tools

### 2. State Machine-Based Pattern Matching

The program uses LibRift's automaton-based pattern matching engine, which converts regular expressions into efficient state machines for matching:

```c
// Create matcher for the pattern
rift_regex_matcher_t *matcher = rift_matcher_create(pattern, RIFT_MATCHER_OPTION_NONE);
```

### 3. Capture Group Processing

The program demonstrates how to extract and process capture groups:

```c
if (result && match.group_count >= 4) {
    // Copy transaction data from capture groups
    strncpy(entry->transaction_data.user_id, match.groups[1].value, 
            sizeof(entry->transaction_data.user_id) - 1);
    
    // Additional capture group processing...
}
```

## Building and Running

To build the project:

```bash
make
```

To run the log analyzer:

```bash
./log_analyzer sample_log.txt
```

## System Requirements

- C compiler with C11 support
- LibRift library (installed with -lrift linkage)

## Implementation Notes

1. The program demonstrates systematic error handling for all regex operations
2. Memory management is properly handled with all resources being freed
3. Complex patterns are broken down into multiple functions for maintainability
4. The log entry structure provides a well-defined representation of parsed data

## Future Enhancements

Possible enhancements for this project:

1. Add support for custom log formats via configuration file
2. Implement multi-threaded log processing for large files
3. Add visualization of log statistics
4. Extend transaction analysis with advanced metrics
5. Provide filtering capabilities for targeted log analysis

## License

MIT License (2025)