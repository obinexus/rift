# Makefile for Thread-Safe LibRift Components
# This makefile compiles the thread-safe context and backtracker implementations
# and integrates them with the existing LibRift codebase.

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -I./include -pthread
LDFLAGS = -pthread

# Source directories
REGEX_DIR = src/regex
RUNTIME_DIR = $(REGEX_DIR)/runtime
ERRORS_DIR = $(REGEX_DIR)/errors
CORE_DIR = src/core

# Source files
THREAD_SAFE_SRCS = \
    $(RUNTIME_DIR)/thread_safe_context.c \
    $(RUNTIME_DIR)/safe_backtracker.c \
    $(RUNTIME_DIR)/backtracker_extension.c

# Demo application
DEMO_SRC = examples/thread_safe_demo.c

# Object files
THREAD_SAFE_OBJS = $(THREAD_SAFE_SRCS:.c=.o)
DEMO_OBJ = $(DEMO_SRC:.c=.o)

# Existing LibRift library
LIBRIFT = lib/librift.a

# Output binary
DEMO_BIN = bin/thread_safe_demo

# Phony targets
.PHONY: all clean directories

# Default target
all: directories $(DEMO_BIN)

# Create necessary directories
directories:
	@mkdir -p bin lib $(RUNTIME_DIR) $(ERRORS_DIR)

# Compile thread-safe components
$(RUNTIME_DIR)/%.o: $(RUNTIME_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile the demo application
$(DEMO_OBJ): $(DEMO_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Link thread-safe components into LibRift
$(LIBRIFT): $(THREAD_SAFE_OBJS)
	ar rcs $@ $(THREAD_SAFE_OBJS)

# Link the demo application
$(DEMO_BIN): $(DEMO_OBJ) $(LIBRIFT)
	$(CC) $^ -o $@ $(LDFLAGS)

# Clean up
clean:
	rm -f $(THREAD_SAFE_OBJS) $(DEMO_OBJ) $(DEMO_BIN)
	rm -f $(LIBRIFT)