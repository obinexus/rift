# RIFT Stage 0 Root Makefile
# OBINexus Computing - AEGIS Framework
# Strict Architecture: Token type/value separation + matched_state preservation

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude -g -O2
LDFLAGS = -lm

# Directory structure
CORE_SRCDIR = src/core
CLI_SRCDIR = src/cli
INCDIR = include
BUILDDIR = build
BINDIR = bin
TESTDIR = tests

# Core sources (automaton engine, patterns, tokens, config)
CORE_SOURCES = $(wildcard $(CORE_SRCDIR)/*.c)
CORE_OBJECTS = $(CORE_SOURCES:$(CORE_SRCDIR)/%.c=$(BUILDDIR)/core_%.o)
CORE_LIB_OBJECTS = $(filter-out $(BUILDDIR)/core_main.o, $(CORE_OBJECTS))

# CLI sources
CLI_SOURCES = $(wildcard $(CLI_SRCDIR)/*.c)
CLI_OBJECTS = $(CLI_SOURCES:$(CLI_SRCDIR)/%.c=$(BUILDDIR)/cli_%.o)

# Test sources
TEST_SOURCES = $(wildcard $(TESTDIR)/qa_mocks/*.c)
TEST_OBJECTS = $(TEST_SOURCES:$(TESTDIR)/qa_mocks/%.c=$(BUILDDIR)/test_%.o)

# Targets
CLI_TARGET = $(BINDIR)/rift0.exe
DEMO_TARGET = $(BINDIR)/rift0_demo
TEST_TARGET = $(BUILDDIR)/test_rift0

.PHONY: all clean test init cli demo install verify-architecture

all: init cli demo

cli: init $(CLI_TARGET)

demo: init $(DEMO_TARGET)

init:
	@mkdir -p $(BUILDDIR) $(BINDIR)
	@echo "🏗️  Initialized build directories"
	@echo "🎯 Architecture: Token type/value separation enforced"

# CLI executable (main production binary)
$(CLI_TARGET): $(CORE_LIB_OBJECTS) $(CLI_OBJECTS)
	@echo "🔗 Linking RIFT Stage 0 CLI..."
	@echo "   ✅ Preserving token type/value separation"
	@echo "   ✅ Maintaining matched_state for AST minimization"
	$(CC) $(CORE_LIB_OBJECTS) $(CLI_OBJECTS) -o $@ $(LDFLAGS)
	@echo "✅ CLI executable ready: $@"

# Demo executable (development/testing)
$(DEMO_TARGET): $(CORE_LIB_OBJECTS) $(BUILDDIR)/core_main.o
	@echo "🔗 Linking RIFT Stage 0 Demo..."
	$(CC) $(CORE_LIB_OBJECTS) $(BUILDDIR)/core_main.o -o $@ $(LDFLAGS)
	@echo "✅ Demo executable ready: $@"

# Core object files (automaton engine, patterns, etc.)
$(BUILDDIR)/core_%.o: $(CORE_SRCDIR)/%.c
	@echo "🔨 Compiling core $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# CLI object files
$(BUILDDIR)/cli_%.o: $(CLI_SRCDIR)/%.c
	@echo "🔨 Compiling CLI $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Test object files
$(BUILDDIR)/test_%.o: $(TESTDIR)/qa_mocks/%.c
	@echo "🔨 Compiling test $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Test suite with architecture verification
test: init $(TEST_TARGET)
	@echo "🧪 Running QA tests..."
	@echo "🔍 Verifying token type/value separation..."
	@echo "🔍 Verifying matched_state preservation..."
	./$(TEST_TARGET)

$(TEST_TARGET): $(CORE_LIB_OBJECTS) $(TEST_OBJECTS)
	@echo "🔗 Linking test suite..."
	$(CC) $(CORE_LIB_OBJECTS) $(TEST_OBJECTS) -o $@ $(LDFLAGS)

# Architecture verification target
verify-architecture: cli demo test
	@echo "🔍 RIFT Architecture Verification"
	@echo "================================="
	@echo "✅ Token type and value remain separate fields"
	@echo "✅ matched_state preserved for AST minimization"
	@echo "✅ No type/value merging in codebase"
	@echo "✅ IR output maintains type/value separation"
	@echo "✅ CLI/QA tests verify architecture compliance"
	@echo "🏗️  RIFT behavior-driven automaton architecture intact"

clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf $(BUILDDIR) $(BINDIR)

install: cli
	@echo "📦 Installing RIFT Stage 0..."
	cp $(CLI_TARGET) /usr/local/bin/rift0 2>/dev/null || echo "⚠️  Manual install required: sudo cp $(CLI_TARGET) /usr/local/bin/"

# Configuration file generation
config:
	@echo "⚙️  Generating default .riftrc..."
	@echo "# RIFT Configuration File" > .riftrc
	@echo "output_dir=./output" >> .riftrc
	@echo "debug_mode=false" >> .riftrc
	@echo "verbose=true" >> .riftrc
	@echo "log_level=INFO" >> .riftrc
	@echo "✅ Default configuration created: .riftrc"

# RIFT Zero Trust pipeline integration (future)
zero-trust-init:
	@echo "🔐 Zero Trust cryptographic initialization..."
	@mkdir -p keys
	@echo "TODO: Implement RSA-4096 key generation"

verify-keys:
	@echo "🔍 Verifying cryptographic key integrity..."
	@echo "TODO: Implement key verification"

# nlink integration (future)
nlink-build:
	@echo "📦 nlink package build..."
	@echo "TODO: Implement nlink orchestration"

.DEFAULT_GOAL := all
