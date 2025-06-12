# Corrected RIFT Stage 0 Makefile with Fixed Structure
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude -g -O2
LDFLAGS = -lm

# Library generation control
LIB ?= BOTH
LIBDIR = lib
BINDIR = bin
BUILDDIR = build
OUTPUTDIR = output

# Source directories
SRCDIR = src
COREDIR = $(SRCDIR)/core
CLIDIR = $(SRCDIR)/cli
CLICMDDIR = $(CLIDIR)/command
TESTDIR = tests
EXAMPLEDIR = examples

# Core source files
CORE_SOURCES = $(wildcard $(COREDIR)/*.c)
CORE_OBJECTS = $(patsubst $(COREDIR)/%.c,$(BUILDDIR)/core_%.o,$(CORE_SOURCES))

# CLI main source files
CLI_MAIN_SOURCES = $(wildcard $(CLIDIR)/*.c)
CLI_MAIN_OBJECTS = $(patsubst $(CLIDIR)/%.c,$(BUILDDIR)/cli_%.o,$(CLI_MAIN_SOURCES))

# CLI command source files
CLI_CMD_SOURCES = $(wildcard $(CLICMDDIR)/*.c)
CLI_CMD_OBJECTS = $(patsubst $(CLICMDDIR)/%.c,$(BUILDDIR)/cli_command_%.o,$(CLI_CMD_SOURCES))

# All CLI objects
CLI_OBJECTS = $(CLI_MAIN_OBJECTS) $(CLI_CMD_OBJECTS)

# Test source files
TEST_SOURCES = $(wildcard $(TESTDIR)/qa_mocks/*.c)
TEST_OBJECTS = $(patsubst $(TESTDIR)/qa_mocks/%.c,$(BUILDDIR)/test_%.o,$(TEST_SOURCES))

# Library targets
ifeq ($(LIB),SO)
    LIBRARY_TARGETS = $(LIBDIR)/librift0.so
    LIBCFLAGS = -fPIC
    LIBLINKFLAGS = -shared
else ifeq ($(LIB),A)
    LIBRARY_TARGETS = $(LIBDIR)/librift0.a
    LIBCFLAGS = 
    LIBLINKFLAGS = 
else
    LIBRARY_TARGETS = $(LIBDIR)/librift0.so $(LIBDIR)/librift0.a
    LIBCFLAGS = -fPIC
    LIBLINKFLAGS = -shared
endif

.PHONY: all clean test config lib bootstrap

all: directories $(LIBRARY_TARGETS) $(BINDIR)/rift0.exe $(BINDIR)/rift0_demo

directories:
	@echo "🏗️  Initialized build directories"
	@mkdir -p $(BUILDDIR) $(BINDIR) $(LIBDIR) $(OUTPUTDIR)

# Core object files
$(BUILDDIR)/core_%.o: $(COREDIR)/%.c
	@echo "🔨 Compiling core $<..."
	$(CC) $(CFLAGS) $(LIBCFLAGS) -c $< -o $@

# CLI main object files
$(BUILDDIR)/cli_%.o: $(CLIDIR)/%.c
	@echo "🔨 Compiling CLI $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# CLI command object files
$(BUILDDIR)/cli_command_%.o: $(CLICMDDIR)/%.c
	@echo "🔨 Compiling CLI command $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Test object files
$(BUILDDIR)/test_%.o: $(TESTDIR)/qa_mocks/%.c
	@echo "🔨 Compiling test $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Shared library
$(LIBDIR)/librift0.so: $(CORE_OBJECTS)
	@echo "🔗 Building shared library librift0.so..."
	$(CC) $(LIBLINKFLAGS) -o $@ $(CORE_OBJECTS) $(LDFLAGS)

# Static library
$(LIBDIR)/librift0.a: $(CORE_OBJECTS)
	@echo "🔗 Building static library librift0.a..."
	ar rcs $@ $(CORE_OBJECTS)

# CLI executable
$(BINDIR)/rift0.exe: $(CLI_OBJECTS) $(LIBRARY_TARGETS)
	@echo "🔗 Linking RIFT Stage 0 CLI..."
ifeq ($(LIB),A)
	$(CC) $(CLI_OBJECTS) -L$(LIBDIR) -lrift0 -static $(LDFLAGS) -o $@
else
	$(CC) $(CLI_OBJECTS) -L$(LIBDIR) -lrift0 $(LDFLAGS) -o $@
endif

# Demo executable
$(BINDIR)/rift0_demo: $(BUILDDIR)/core_main.o $(LIBRARY_TARGETS)
	@echo "🔗 Linking RIFT Stage 0 Demo..."
ifeq ($(LIB),A)
	$(CC) $(BUILDDIR)/core_main.o -L$(LIBDIR) -lrift0 -static $(LDFLAGS) -o $@
else
	$(CC) $(BUILDDIR)/core_main.o -L$(LIBDIR) -lrift0 $(LDFLAGS) -o $@
endif

# Test executable
$(BUILDDIR)/test_rift0: $(TEST_OBJECTS) $(LIBRARY_TARGETS)
	@echo "🔗 Linking test suite..."
ifeq ($(LIB),A)
	$(CC) $(TEST_OBJECTS) -L$(LIBDIR) -lrift0 -static $(LDFLAGS) -o $@
else
	$(CC) $(TEST_OBJECTS) -L$(LIBDIR) -lrift0 $(LDFLAGS) -o $@
endif

# Library target
lib: $(LIBRARY_TARGETS)
	@echo "📚 Library generation complete: $(LIBRARY_TARGETS)"

# Configuration
config:
	@echo "⚙️  Setting up .riftrc configuration..."
	@if [ ! -f .riftrc ]; then \
		echo "# Enhanced RIFT Stage 0 Configuration" > .riftrc; \
		echo "output_dir=./output" >> .riftrc; \
		echo "stage_name=rift.0" >> .riftrc; \
		echo "debug=false" >> .riftrc; \
		echo "verbose=false" >> .riftrc; \
		echo "✅ Generated .riftrc"; \
	else \
		echo "✅ Using existing .riftrc"; \
	fi

# Testing
test: $(BUILDDIR)/test_rift0
	@echo "🧪 Running QA tests..."
	@export LD_LIBRARY_PATH=$(PWD)/$(LIBDIR):$$LD_LIBRARY_PATH && ./$(BUILDDIR)/test_rift0

# Bootstrap
bootstrap:
	@echo "🚀 Building corrected RIFT Stage 0..."
	@$(MAKE) clean
	@$(MAKE) all
	@$(MAKE) config
	@$(MAKE) test
	@echo ""
	@echo "✅ Corrected RIFT Stage 0 bootstrap complete!"
	@echo "📊 Build Summary:"
	@echo "  📁 CLI executable: ./$(BINDIR)/rift0.exe"
	@echo "  📁 Demo executable: ./$(BINDIR)/rift0_demo"
	@echo "  📁 Libraries: $(LIBRARY_TARGETS)"
	@echo ""
	@echo "🎯 Usage:"
	@echo "  ./$(BINDIR)/rift0.exe $(EXAMPLEDIR)/sample.rift"
	@echo "  ./$(BINDIR)/rift0_demo"

# Clean
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf $(BUILDDIR) $(BINDIR) $(LIBDIR)
