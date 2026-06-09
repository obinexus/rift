# LibRift Thread Safety Integration Guide

## Overview

This guide explains how to integrate the thread-safe components into your LibRift-based applications to avoid race conditions and ensure reliable performance in multi-threaded environments.

## Background

The standard LibRift implementation has several thread safety issues:

1. **Shared State Management**: Matcher and backtracker state is shared across threads
2. **Unbounded Backtracking**: No limits on backtracking depth, which can lead to stack overflows
3. **Race Conditions**: Multiple threads accessing the same regex objects concurrently
4. **Missing Synchronization**: No mutexes or thread-local storage for concurrent access

## New Components

The thread-safe implementation introduces these new components:

1. **Thread-Safe Context**: A wrapper around the matcher context that provides thread-local storage
2. **Safe Backtracker**: A thread-safe backtracker with depth limits to prevent excessive recursion
3. **Backtracker Extensions**: Additional functionality for the backtracker component

## Integration Steps

### 1. Include the Thread-Safe Headers

```c
#include "librift/regex/runtime/thread_safe_context.h"
#include "librift/regex/runtime/safe_backtracker.h"
```

### 2. Create a Thread-Safe Context

Replace direct usage of matcher contexts with thread-safe contexts:

```c
// Before:
rift_regex_matcher_context_t *context = rift_matcher_context_create(input, input_length, max_groups);

// After:
rift_regex_thread_safe_context_t *ts_context = 
    rift_thread_safe_context_create(automaton, max_groups, MAX_BACKTRACK_DEPTH);
```

### 3. Execute Operations with Thread-Local Context

Use the execute function to run operations with a thread-local context:

```c
bool match_result = rift_thread_safe_context_execute(
    ts_context,
    matching_callback_function,
    user_data,
    &error
);
```

Define a callback function that will be called with the thread-local context:

```c
bool matching_callback_function(
    rift_regex_matcher_context_t *local_context,
    void *user_data,
    rift_regex_error_t *error
) {
    // Perform matcher operations using local_context
    // ...
    return true; // Return success/failure
}
```

### 4. Enforce Backtracking Limits

Replace direct usage of the backtracker with the safe backtracker:

```c
// Before:
rift_regex_backtracker_t *backtracker = rift_backtracker_create(num_groups);
rift_backtracker_push(backtracker, state, position, group_starts, group_ends, num_groups);

// After:
rift_regex_safe_backtracker_t *safe_bt = rift_safe_backtracker_create(MAX_DEPTH, num_groups);
rift_safe_backtracker_push(safe_bt, state, position, group_starts, group_ends, num_groups, &error);
```

### 5. Handle Thread-Safe Error Checking

Always check for BACKTRACKING_LIMIT errors:

```c
if (error.code == RIFT_REGEX_ERROR_BACKTRACKING_LIMIT) {
    // Handle catastrophic backtracking scenario
    fprintf(stderr, "Warning: Maximum backtracking depth exceeded. Pattern may be inefficient.\n");
    // Implement appropriate fallback behavior
}
```

### 6. Set and Reset Input for Each Thread

When working with multiple threads, ensure you set the input string for each context:

```c
rift_thread_safe_context_set_input(ts_context, input_string, input_length);
```

### 7. Lock When Necessary

For operations that must be synchronized across all threads:

```c
if (rift_thread_safe_context_lock(ts_context)) {
    // Perform thread-synchronized operations
    // ...
    rift_thread_safe_context_unlock(ts_context);
}
```

### 8. Clean Up Resources

Always free thread-safe resources:

```c
rift_thread_safe_context_free(ts_context);
rift_safe_backtracker_free(safe_bt);
```

## Thread Safety Best Practices

1. **Pattern Compilation**: Compile patterns once and share the compiled pattern across threads
2. **Reference Counting**: Use `rift_thread_safe_context_ref()` and `rift_thread_safe_context_unref()` for shared contexts
3. **Timeout Support**: Set timeouts to prevent long-running matches from blocking threads
4. **Backtracking Limits**: Always set reasonable backtracking limits based on your application's needs
5. **Error Handling**: Implement robust error handling for thread-specific failures

## Performance Considerations

1. **Thread-Local Allocation**: Each thread creates its own matcher context, which adds memory overhead
2. **Mutex Contention**: Minimize operations that require locking the shared mutex
3. **Context Switching**: Consider using thread pools to reduce context switching overhead
4. **Memory Usage**: Monitor memory usage when working with many threads
5. **Backtracking Depth**: Lower maximum backtracking depth improves safety but may limit complex patterns

## Implementation Details

### Thread-Local Storage

This implementation uses POSIX thread-local storage (TLS) via `pthread_key_create()` and `pthread_setspecific()`/`pthread_getspecific()`. Each thread gets its own matcher context and backtracker instance, which prevents race conditions during matching operations.

### Synchronization Mechanisms

Mutexes (`pthread_mutex_t`) are used to protect access to shared resources, such as the global context and configuration settings. The implementation follows these principles:

1. **Minimal Locking**: Lock only when necessary to reduce contention
2. **Fine-Grained Locks**: Use separate locks for different resources
3. **Thread-Local First**: Prefer thread-local storage over locks when possible

### Backtracking Limits

The safe backtracker enforces strict limits on recursion depth to prevent stack overflow and catastrophic backtracking. This is especially important for patterns with nested quantifiers like `(a+)+` that can lead to exponential backtracking.

## Compatibility Notes

This thread-safe implementation is designed to work with the existing LibRift API with minimal changes. Most applications can be updated by replacing direct context and backtracker usage with their thread-safe counterparts.

## Example: Multi-Threaded Log Processing

Here's a complete example of using the thread-safe components to process log files with multiple threads:

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "librift/regex/runtime/thread_safe_context.h"
#include "librift/regex/engine/pattern.h"

#define MAX_BACKTRACK_DEPTH 1000
#define NUM_THREADS 4

typedef struct {
    rift_regex_thread_safe_context_t *context;
    const char *log_chunk;
    size_t chunk_size;
    int thread_id;
    int match_count;
} thread_data_t;

bool log_processor(rift_regex_matcher_context_t *context, void *user_data, rift_regex_error_t *error) {
    thread_data_t *data = (thread_data_t *)user_data;
    
    // Set the input for this thread's context
    if (!rift_matcher_context_set_input(context, data->log_chunk, data->chunk_size)) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INTERNAL, 
                "Failed to set input for thread");
        }
        return false;
    }
    
    // Process log lines
    int matches = 0;
    size_t pos = 0;
    
    while (pos < data->chunk_size) {
        // Find the next line
        size_t line_start = pos;
        while (pos < data->chunk_size && data->log_chunk[pos] != '\n') {
            pos++;
        }
        
        // Process this line
        rift_matcher_context_set_position(context, line_start);
        
        // Check for match (details omitted for brevity)
        // ...
        
        matches++;
        pos++; // Skip the newline
    }
    
    data->match_count = matches;
    return true;
}

void *thread_function(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    rift_regex_error_t error = {0};
    
    // Process this thread's chunk
    bool success = rift_thread_safe_context_execute(
        data->context,
        log_processor,
        data,
        &error
    );
    
    if (!success) {
        fprintf(stderr, "Thread %d: Error: %s\n", data->thread_id, error.message);
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    // Implementation of multi-threaded log processing
    // ...
}
```

## Troubleshooting

### Common Issues

1. **Backtracking Limit Exceeded**
   - **Symptom**: Error code `RIFT_REGEX_ERROR_BACKTRACKING_LIMIT`
   - **Cause**: Pattern leads to excessive backtracking
   - **Solution**: Optimize regex pattern or increase backtracking limit

2. **Thread-Local Storage Failure**
   - **Symptom**: Error getting thread-local context
   - **Cause**: Failed to allocate memory or initialize TLS
   - **Solution**: Check system resources and thread limits

3. **Mutex Deadlocks**
   - **Symptom**: Application hangs
   - **Cause**: Improper locking/unlocking of mutexes
   - **Solution**: Ensure every lock has a corresponding unlock

4. **Memory Leaks**
   - **Symptom**: Increasing memory usage over time
   - **Cause**: Not freeing thread-local resources
   - **Solution**: Ensure proper cleanup of thread-local contexts

### Diagnostic Tools

1. **Backtracking Depth Monitoring**
   ```c
   size_t current_depth = rift_safe_backtracker_get_depth(safe_bt, &error);
   printf("Current backtracking depth: %zu\n", current_depth);
   ```

2. **Thread-Safe Context Validation**
   ```c
   rift_regex_matcher_context_t *local_ctx = rift_thread_safe_context_get_local(ts_context);
   if (!local_ctx) {
       fprintf(stderr, "Failed to get thread-local context\n");
   }
   ```

## Conclusion

By implementing these thread-safe components, LibRift can now safely be used in multi-threaded applications without the risk of race conditions or excessive backtracking. This implementation maintains compatibility with the existing API while adding crucial safety features for concurrent usage.

For additional assistance or to report issues with the thread-safe implementation, please refer to the LibRift documentation or contact the development team.