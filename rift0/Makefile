# Fixed RIFT Stage 0 Makefile with Proper Library Selection
# Supports: make LIB=SO (shared library) or make LIB=A (static library)
# Default: shared library only to avoid conflicts

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude -g -O2
LDFLAGS = -lm

# Library generation control - DEFAULT TO SHARED ONLY
LIB ?= SO
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

# Core source files (EXCLUDE main.c from library)
CORE_SOURCES = $(filter-out $(COREDIR)/main.c, $(wildcard $(COREDIR)/*.c))
CORE_OBJECTS = $(patsubst $(COREDIR)/%.c,$(BUILDDIR)/core_%.o,$(CORE_SOURCES))

# CLI source files
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

# Demo main object (separate from library)
DEMO_OBJECT = $(BUILDDIR)/core_main.o

# Library selection - CHOOSE ONE, NOT BOTH
ifeq ($(LIB),SO)
    LIBRARY_TARGET = $(LIBDIR)/librift0.so
    LIBCFLAGS = -fPIC
    LIBLINKFLAGS = -shared
    LINK_FLAGS = -L$(LIBDIR) -lrift0
else ifeq ($(LIB),A)
    LIBRARY_TARGET = $(LIBDIR)/librift0.a
    LIBCFLAGS = 
    LIBLINKFLAGS = 
    LINK_FLAGS = -L$(LIBDIR) -lrift0 -static
else ifeq ($(LIB),BOTH)
    LIBRARY_TARGET = $(LIBDIR)/librift0.so $(LIBDIR)/librift0.a
    LIBCFLAGS = -fPIC
    LIBLINKFLAGS = -shared
    LINK_FLAGS = -L$(LIBDIR) -lrift0
else
    LIBRARY_TARGET = $(LIBDIR)/librift0.so
    LIBCFLAGS = -fPIC
    LIBLINKFLAGS = -shared
    LINK_FLAGS = -L$(LIBDIR) -lrift0
endif

# Main targets
.PHONY: all clean test config lib install bootstrap help

all: directories $(LIBRARY_TARGET) $(BINDIR)/rift0.exe $(BINDIR)/rift0_demo

# Create directories
directories:
	@echo "🏗️  Initialized build directories"
	@mkdir -p $(BUILDDIR) $(BINDIR) $(LIBDIR) $(OUTPUTDIR)
	@mkdir -p $(SRCDIR)/core $(SRCDIR)/cli $(SRCDIR)/cli/command
	@mkdir -p $(TESTDIR)/qa_mocks $(EXAMPLEDIR)
	@mkdir -p include

# Core object files (library components only)
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

# Shared library generation
$(LIBDIR)/librift0.so: $(CORE_OBJECTS)
	@echo "🔗 Building shared library librift0.so..."
	@echo "   📚 Objects: $(words $(CORE_OBJECTS)) core files"
	$(CC) $(LIBLINKFLAGS) -o $@ $(CORE_OBJECTS) $(LDFLAGS)

# Static library generation  
$(LIBDIR)/librift0.a: $(CORE_OBJECTS)
	@echo "🔗 Building static library librift0.a..."
	@echo "   📚 Objects: $(words $(CORE_OBJECTS)) core files"
	ar rcs $@ $(CORE_OBJECTS)

# CLI executable - FIXED LINKING
$(BINDIR)/rift0.exe: $(CLI_OBJECTS) $(LIBRARY_TARGET)
	@echo "🔗 Linking RIFT Stage 0 CLI..."
	@echo "   📚 Library mode: $(LIB)"
	@echo "   🔗 Link flags: $(LINK_FLAGS)"
	@echo "   ✅ CLI objects: $(words $(CLI_OBJECTS)) files"
ifeq ($(LIB),SO)
	$(CC) $(CLI_OBJECTS) $(LINK_FLAGS) $(LDFLAGS) -o $@
else ifeq ($(LIB),A)
	$(CC) $(CLI_OBJECTS) $(LINK_FLAGS) $(LDFLAGS) -o $@
else
	$(CC) $(CLI_OBJECTS) $(LINK_FLAGS) $(LDFLAGS) -o $@
endif

# Demo executable - FIXED LINKING  
$(BINDIR)/rift0_demo: $(DEMO_OBJECT) $(LIBRARY_TARGET)
	@echo "🔗 Linking RIFT Stage 0 Demo..."
	@echo "   📚 Library mode: $(LIB)"
ifeq ($(LIB),SO)
	$(CC) $(DEMO_OBJECT) $(LINK_FLAGS) $(LDFLAGS) -o $@
else ifeq ($(LIB),A)
	$(CC) $(DEMO_OBJECT) $(LINK_FLAGS) $(LDFLAGS) -o $@
else
	$(CC) $(DEMO_OBJECT) $(LINK_FLAGS) $(LDFLAGS) -o $@
endif

# Test executable - FIXED LINKING
$(BUILDDIR)/test_rift0: $(TEST_OBJECTS) $(LIBRARY_TARGET)
	@echo "🔗 Linking test suite..."
	@echo "   📚 Library mode: $(LIB)"
ifeq ($(LIB),SO)
	$(CC) $(TEST_OBJECTS) $(LINK_FLAGS) $(LDFLAGS) -o $@
else ifeq ($(LIB),A)
	$(CC) $(TEST_OBJECTS) $(LINK_FLAGS) $(LDFLAGS) -o $@
else
	$(CC) $(TEST_OBJECTS) $(LINK_FLAGS) $(LDFLAGS) -o $@
endif

# Library target
lib: $(LIBRARY_TARGET)
	@echo "📚 Library generation complete"
	@echo "   📁 Generated: $(LIBRARY_TARGET)"
	@echo "   🔗 Mode: $(LIB)"

# Configuration
config:
	@echo "⚙️  Setting up enhanced configuration..."
	@if [ ! -f .riftrc ]; then \
		echo "# Enhanced RIFT Stage 0 Configuration" > .riftrc; \
		echo "output_dir=./output" >> .riftrc; \
		echo "stage_name=rift.0" >> .riftrc; \
		echo "debug=false" >> .riftrc; \
		echo "verbose=false" >> .riftrc; \
		echo "lib_mode=$(LIB)" >> .riftrc; \
		echo "✅ Generated .riftrc with lib_mode=$(LIB)"; \
	else \
		echo "✅ Using existing .riftrc"; \
	fi

# Testing with proper library path
test: $(BUILDDIR)/test_rift0
	@echo "🧪 Running QA tests with $(LIB) library..."
ifeq ($(LIB),SO)
	@export LD_LIBRARY_PATH=$(PWD)/$(LIBDIR):$$LD_LIBRARY_PATH && ./$(BUILDDIR)/test_rift0
else
	@./$(BUILDDIR)/test_rift0
endif

# Bootstrap with library selection
bootstrap:
	@echo "🚀 Enhanced RIFT Stage 0 Bootstrap"
	@echo "===================================="
	@echo "📚 Library mode: $(LIB)"
	@$(MAKE) clean
	@$(MAKE) all LIB=$(LIB)
	@$(MAKE) config
	@$(MAKE) test
	@echo ""
	@echo "✅ Enhanced RIFT Stage 0 bootstrap complete!"
	@echo "📊 Build Summary:"
	@echo "  📁 CLI executable: ./$(BINDIR)/rift0.exe"
	@echo "  📁 Demo executable: ./$(BINDIR)/rift0_demo"  
	@echo "  📁 Library: $(LIBRARY_TARGET)"
	@echo "  📁 Configuration: .riftrc"
	@echo ""
	@echo "🎯 Usage Examples:"
	@echo "  ./$(BINDIR)/rift0.exe $(EXAMPLEDIR)/sample.rift"
	@echo ""
	@echo "📚 Library Modes:"
	@echo "  make LIB=SO bootstrap    # Shared library (default)"
	@echo "  make LIB=A bootstrap     # Static library"
	@echo "  make LIB=BOTH lib        # Generate both libraries"
	@echo ""
	@echo "🔗 External Usage:"
	@echo "  gcc -I./include -L./$(LIBDIR) -lrift0 program.c -o program"

# Installation
install: all
	@echo "📦 Installing RIFT Stage 0..."
	@mkdir -p /usr/local/lib /usr/local/bin /usr/local/include
	@cp $(LIBRARY_TARGET) /usr/local/lib/
	@cp $(BINDIR)/rift0.exe /usr/local/bin/rift0
	@cp include/rift.h /usr/local/include/
	@echo "✅ Installation complete ($(LIB) library)"

# Clean
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf $(BUILDDIR) $(BINDIR) $(LIBDIR)
	@echo "✅ Clean complete"

# Help
help:
	@echo "Enhanced RIFT Stage 0 Makefile"
	@echo "=============================="
	@echo ""
	@echo "📚 Library Selection (IMPORTANT):"
	@echo "  make LIB=SO     # Shared library (.so) - Default"
	@echo "  make LIB=A      # Static library (.a)"
	@echo "  make LIB=BOTH   # Generate both (use SO for linking)"
	@echo ""
	@echo "🎯 Main Targets:"
	@echo "  make all        # Build everything with selected library"
	@echo "  make lib        # Build library only"
	@echo "  make test       # Run QA tests"
	@echo "  make bootstrap  # Complete setup"
	@echo "  make config     # Generate .riftrc"
	@echo "  make install    # Install system-wide"
	@echo "  make clean      # Clean build artifacts"
	@echo ""
	@echo "⚠️  CRITICAL: Choose ONE library type to avoid linker conflicts!"
	@echo "✅ Recommended: Use LIB=SO (shared library) for development"
	@echo "✅ Use LIB=A (static library) for distribution"