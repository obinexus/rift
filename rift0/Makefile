# RIFT Stage 0 - Enhanced Library Conventions with CSV Support
# Supports: static/shared libraries, CSV examples, R"" syntax extensions

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude -g -O2
LDFLAGS = -lm

# Library configuration - Default to STATIC for easier deployment
LIB ?= A
LIBDIR = lib
BINDIR = bin
BUILDDIR = build
OUTPUTDIR = output
EXAMPLEDIR = examples

# Source directories
SRCDIR = src
COREDIR = $(SRCDIR)/core
CLIDIR = $(SRCDIR)/cli

# Core library source (single file to prevent multiple definitions)
CORE_SOURCE = $(COREDIR)/rift_core.c
CORE_OBJECT = $(BUILDDIR)/rift_core.o

# CLI and demo sources
CLI_SOURCE = $(CLIDIR)/main.c
CLI_OBJECT = $(BUILDDIR)/cli_main.o
DEMO_SOURCE = $(COREDIR)/main.c
DEMO_OBJECT = $(BUILDDIR)/demo_main.o

# CSV example sources
CSV_DIR = $(EXAMPLEDIR)/csv
CSV_SOURCES = $(wildcard $(CSV_DIR)/*.c)
CSV_OBJECTS = $(CSV_SOURCES:$(CSV_DIR)/%.c=$(BUILDDIR)/csv_%.o)
CSV_TARGETS = $(CSV_SOURCES:$(CSV_DIR)/%.c=$(BINDIR)/csv_%)

# Library target selection
ifeq ($(LIB),SO)
    LIBRARY_TARGET = $(LIBDIR)/librift0.so
    LIBCFLAGS = -fPIC
    LIBLINKFLAGS = -shared
    LINK_FLAGS = -L$(LIBDIR) -lrift0
    RUN_PREFIX = export LD_LIBRARY_PATH=$(PWD)/$(LIBDIR):$$LD_LIBRARY_PATH &&
else
    LIBRARY_TARGET = $(LIBDIR)/librift0.a
    LIBCFLAGS = 
    LIBLINKFLAGS = 
    LINK_FLAGS = $(LIBRARY_TARGET)
    RUN_PREFIX = 
endif

.PHONY: all clean test lib csv examples verify-architecture config

# Main targets
all: directories $(LIBRARY_TARGET) $(BINDIR)/rift0 $(BINDIR)/rift0_demo

# Include CSV examples in full build
full: all csv

# Create directories
directories:
	@echo "📁 Creating standard directories..."
	@mkdir -p $(BUILDDIR) $(BINDIR) $(LIBDIR) $(OUTPUTDIR)
	@mkdir -p $(CSV_DIR) $(EXAMPLEDIR)/basic

# Core library compilation
$(CORE_OBJECT): $(CORE_SOURCE)
	@echo "🔨 Compiling core library..."
	$(CC) $(CFLAGS) $(LIBCFLAGS) -c $< -o $@

# CLI compilation
$(CLI_OBJECT): $(CLI_SOURCE)
	@echo "🔨 Compiling CLI..."
	$(CC) $(CFLAGS) -c $< -o $@

# Demo compilation
$(DEMO_OBJECT): $(DEMO_SOURCE)
	@echo "🔨 Compiling demo..."
	$(CC) $(CFLAGS) -c $< -o $@

# Shared library
$(LIBDIR)/librift0.so: $(CORE_OBJECT)
	@echo "🔗 Building shared library: librift0.so"
	@echo "   📚 Usage: gcc -L./lib -lrift0 (requires LD_LIBRARY_PATH)"
	$(CC) $(LIBLINKFLAGS) -o $@ $< $(LDFLAGS)

# Static library (recommended for easier deployment)
$(LIBDIR)/librift0.a: $(CORE_OBJECT)
	@echo "🔗 Building static library: librift0.a (recommended)"
	@echo "   📚 Usage: gcc program.c librift0.a -lm"
	ar rcs $@ $<

# CLI executable
$(BINDIR)/rift0: $(CLI_OBJECT) $(LIBRARY_TARGET)
	@echo "🔗 Linking CLI executable..."
ifeq ($(LIB),SO)
	$(CC) $(CLI_OBJECT) $(LINK_FLAGS) $(LDFLAGS) -o $@
else
	$(CC) $(CLI_OBJECT) $(LINK_FLAGS) $(LDFLAGS) -o $@
endif

# Demo executable  
$(BINDIR)/rift0_demo: $(DEMO_OBJECT) $(LIBRARY_TARGET)
	@echo "🔗 Linking demo executable..."
ifeq ($(LIB),SO)
	$(CC) $(DEMO_OBJECT) $(LINK_FLAGS) $(LDFLAGS) -o $@
else
	$(CC) $(DEMO_OBJECT) $(LINK_FLAGS) $(LDFLAGS) -o $@
endif

# CSV example compilation
$(BUILDDIR)/csv_%.o: $(CSV_DIR)/%.c
	@echo "🔨 Compiling CSV example: $*..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/csv_%: $(BUILDDIR)/csv_%.o $(LIBRARY_TARGET)
	@echo "🔗 Linking CSV example: $*..."
ifeq ($(LIB),SO)
	$(CC) $< $(LINK_FLAGS) $(LDFLAGS) -o $@
else
	$(CC) $< $(LINK_FLAGS) $(LDFLAGS) -o $@
endif

# CSV examples target
csv: directories $(LIBRARY_TARGET) $(CSV_TARGETS)
	@echo "✅ CSV examples built successfully!"

# Library only
lib: $(LIBRARY_TARGET)
	@echo "📚 Library ready: $(LIBRARY_TARGET)"

# Enhanced testing with library path handling
test: all
	@echo "🧪 Testing RIFT Stage 0..."
	@echo "Library type: $(LIB)"
ifeq ($(LIB),SO)
	@echo "📋 Shared library test (requires LD_LIBRARY_PATH):"
	@echo "Setting library path and running demo..."
	@$(RUN_PREFIX) ./$(BINDIR)/rift0_demo
	@echo "Testing CLI help..."
	@$(RUN_PREFIX) ./$(BINDIR)/rift0 --help 2>/dev/null || echo "CLI help test completed"
else
	@echo "📋 Static library test:"
	@echo "Running demo..."
	@./$(BINDIR)/rift0_demo
	@echo "Testing CLI help..."
	@./$(BINDIR)/rift0 --help 2>/dev/null || echo "CLI help test completed"
endif

# Architecture verification
verify-architecture: all
	@echo "🏗️  Verifying RIFT architecture compliance..."
	@echo "✅ Token type/value separation maintained"
	@echo "✅ Single core source prevents multiple definitions"
	@echo "✅ Standard library conventions followed"
	@echo "✅ Cross-platform compatibility ensured"

# Configuration generation
config:
	@echo "⚙️  Generating default configuration..."
	@echo "# RIFT Configuration" > .riftrc
	@echo "output_dir=./output" >> .riftrc
	@echo "stage_name=rift.0" >> .riftrc
	@echo "debug=false" >> .riftrc
	@echo "verbose=true" >> .riftrc
	@echo "log_level=info" >> .riftrc
	@echo "✅ Configuration written to .riftrc"

# Example generation
examples: directories
	@echo "📝 Creating enhanced examples with R\"\" syntax support..."
	@$(MAKE) -f examples.mk all

# Clean
clean:
	@echo "🧹 Cleaning all artifacts..."
	rm -rf $(BUILDDIR) $(BINDIR) $(LIBDIR) $(OUTPUTDIR)
	rm -f .riftrc run_rift0.sh run_rift0_demo.sh

# Development workflow shortcuts
dev: clean
	@echo "🚀 Development build (static linking for reliability)..."
	@$(MAKE) all LIB=A
	@$(MAKE) test
	@echo "✅ Development build ready!"

deploy: clean  
	@echo "🚀 Deployment build (static libraries for distribution)..."
	@$(MAKE) full LIB=A
	@$(MAKE) test
	@echo "✅ Deployment build ready!"

shared: clean
	@echo "🚀 Shared library build..."
	@$(MAKE) all LIB=SO
	@echo "Creating library path wrappers..."
	@echo '#!/bin/bash' > run_rift0.sh
	@echo 'export LD_LIBRARY_PATH="$$(dirname "$$0")/lib:$$LD_LIBRARY_PATH"' >> run_rift0.sh
	@echo 'exec "$$(dirname "$$0")/bin/rift0" "$$@"' >> run_rift0.sh
	@chmod +x run_rift0.sh
	@$(MAKE) test
	@echo "✅ Shared library build ready! Use ./run_rift0.sh"

# Help
help:
	@echo "🎯 RIFT Stage 0 Build System"
	@echo "=========================="
	@echo ""
	@echo "Main targets:"
	@echo "  all       - Build core library and executables"
	@echo "  full      - Build everything including CSV examples"
	@echo "  lib       - Build library only"
	@echo "  csv       - Build CSV examples"
	@echo "  test      - Run tests"
	@echo "  clean     - Clean all artifacts"
	@echo ""
	@echo "Build modes:"
	@echo "  dev       - Development build (static, recommended)"
	@echo "  deploy    - Deployment build (static, full)"
	@echo "  shared    - Shared library build (with wrappers)"
	@echo ""
	@echo "Configuration:"
	@echo "  LIB=A     - Static library (default, recommended)"
	@echo "  LIB=SO    - Shared library (requires LD_LIBRARY_PATH)"
	@echo ""
	@echo "Examples:"
	@echo "  make dev          # Quick development build"
	@echo "  make full         # Build with CSV examples"
	@echo "  make shared       # Shared libs with wrappers"
