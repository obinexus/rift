# RIFT Stage 0 - Standard Library Conventions
# Supports standard library naming: librift0.a, librift0.so
# Link with: gcc -L./lib -lrift0

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude -g -O2
LDFLAGS = -lm

# Library type selection
LIB ?= SO
LIBDIR = lib
BINDIR = bin
BUILDDIR = build
OUTPUTDIR = output

# Source files
SRCDIR = src
COREDIR = $(SRCDIR)/core
CLIDIR = $(SRCDIR)/cli

# SINGLE CORE SOURCE to prevent multiple definitions
CORE_SOURCE = $(COREDIR)/rift_core.c
CORE_OBJECT = $(BUILDDIR)/rift_core.o

# CLI sources
CLI_SOURCE = $(CLIDIR)/main.c
CLI_OBJECT = $(BUILDDIR)/cli_main.o

# Demo source (separate from library)
DEMO_SOURCE = $(COREDIR)/main.c
DEMO_OBJECT = $(BUILDDIR)/demo_main.o

# Standard library naming
ifeq ($(LIB),SO)
    LIBRARY_TARGET = $(LIBDIR)/librift0.so
    LIBCFLAGS = -fPIC
    LIBLINKFLAGS = -shared
    LINK_FLAGS = -L$(LIBDIR) -lrift0
else ifeq ($(LIB),A)
    LIBRARY_TARGET = $(LIBDIR)/librift0.a
    LIBCFLAGS = 
    LIBLINKFLAGS = 
    LINK_FLAGS = -L$(LIBDIR) -lrift0
else
    LIBRARY_TARGET = $(LIBDIR)/librift0.so
    LIBCFLAGS = -fPIC
    LIBLINKFLAGS = -shared
    LINK_FLAGS = -L$(LIBDIR) -lrift0
endif

.PHONY: all clean test lib

all: directories $(LIBRARY_TARGET) $(BINDIR)/rift0 $(BINDIR)/rift0_demo

# Create directories
directories:
	@echo "📁 Creating standard directories..."
	@mkdir -p $(BUILDDIR) $(BINDIR) $(LIBDIR) $(OUTPUTDIR)

# Compile core library (single source)
$(CORE_OBJECT): $(CORE_SOURCE)
	@echo "🔨 Compiling core library..."
	$(CC) $(CFLAGS) $(LIBCFLAGS) -c $< -o $@

# Compile CLI
$(CLI_OBJECT): $(CLI_SOURCE)
	@echo "🔨 Compiling CLI..."
	$(CC) $(CFLAGS) -c $< -o $@

# Compile demo
$(DEMO_OBJECT): $(DEMO_SOURCE)
	@echo "🔨 Compiling demo..."
	$(CC) $(CFLAGS) -c $< -o $@

# Build shared library (standard naming: librift0.so)
$(LIBDIR)/librift0.so: $(CORE_OBJECT)
	@echo "🔗 Building standard shared library: librift0.so"
	@echo "   📚 Link with: gcc -L./lib -lrift0"
	$(CC) $(LIBLINKFLAGS) -o $@ $< $(LDFLAGS)

# Build static library (standard naming: librift0.a)
$(LIBDIR)/librift0.a: $(CORE_OBJECT)
	@echo "🔗 Building standard static library: librift0.a"
	@echo "   📚 Link with: gcc -L./lib -lrift0"
	ar rcs $@ $<

# Build CLI executable (links to library)
$(BINDIR)/rift0: $(CLI_OBJECT) $(LIBRARY_TARGET)
	@echo "🔗 Linking CLI executable..."
ifeq ($(LIB),SO)
	$(CC) $(CLI_OBJECT) $(LINK_FLAGS) $(LDFLAGS) -o $@
else
	$(CC) $(CLI_OBJECT) $(LINK_FLAGS) $(LDFLAGS) -o $@
endif

# Build demo executable (links to library)
$(BINDIR)/rift0_demo: $(DEMO_OBJECT) $(LIBRARY_TARGET)
	@echo "🔗 Linking demo executable..."
ifeq ($(LIB),SO)
	$(CC) $(DEMO_OBJECT) $(LINK_FLAGS) $(LDFLAGS) -o $@
else
	$(CC) $(DEMO_OBJECT) $(LINK_FLAGS) $(LDFLAGS) -o $@
endif

# Library only
lib: $(LIBRARY_TARGET)
	@echo "📚 Standard library ready: $(LIBRARY_TARGET)"
	@echo "🔗 Usage: gcc -I./include -L./lib -lrift0 program.c"

# Test library linking
test: all
	@echo "🧪 Testing library linking..."
ifeq ($(LIB),SO)
	@echo "📋 Shared library test:"
	@export LD_LIBRARY_PATH=$(PWD)/$(LIBDIR):$$LD_LIBRARY_PATH && ldd $(BINDIR)/rift0 | grep rift0 || echo "✅ Library linked correctly"
	@echo "🎯 Running demo..."
	@export LD_LIBRARY_PATH=$(PWD)/$(LIBDIR):$$LD_LIBRARY_PATH && ./$(BINDIR)/rift0_demo
else
	@echo "📋 Static library test:"
	@echo "🎯 Running demo..."
	@./$(BINDIR)/rift0_demo
endif

# Clean
clean:
	@echo "🧹 Cleaning artifacts..."
	rm -rf $(BUILDDIR) $(BINDIR) $(LIBDIR)

# Bootstrap
bootstrap: clean
	@echo "🚀 RIFT Stage 0 - Standard Library Bootstrap"
	@echo "============================================="
	@echo "📚 Library type: $(LIB)"
	@echo "📁 Standard naming: librift0.{a,so}"
	@echo "🔗 Link usage: gcc -L./lib -lrift0"
	@echo ""
	@$(MAKE) all LIB=$(LIB)
	@$(MAKE) test
	@echo ""
	@echo "✅ Bootstrap complete with standard conventions!"
	@echo "📊 Generated:"
	@echo "   📚 Library: $(LIBRARY_TARGET)"
	@echo "   🎯 CLI: $(BINDIR)/rift0"
	@echo "   🎮 Demo: $(BINDIR)/rift0_demo"
	@echo ""
	@echo "🔗 Standard usage examples:"
	@echo "   gcc -I./include -L./lib -lrift0 program.c"
	@echo "   export LD_LIBRARY_PATH=./lib:$$LD_LIBRARY_PATH"
	@echo "   ./bin/rift0 input.rift"
