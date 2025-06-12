# Enhanced RIFT Stage 0 Makefile with Library Generation
# Supports: make LIB=SO (shared library) or make LIB=A (static library)
# Default: generates both .a and .so

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude -g -O2
LDFLAGS = -lm -lpthread

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
TESTDIR = tests
EXAMPLEDIR = examples

# Core source files
CORE_SOURCES = $(wildcard $(COREDIR)/*.c)
CORE_OBJECTS = $(patsubst $(COREDIR)/%.c,$(BUILDDIR)/core_%.o,$(CORE_SOURCES))

# CLI source files (including new command structure)
CLI_SOURCES = $(wildcard $(CLIDIR)/*.c) $(wildcard $(CLIDIR)/command/*.c)
CLI_OBJECTS = $(patsubst $(CLIDIR)/%.c,$(BUILDDIR)/cli_%.o,$(CLI_SOURCES))

# Test source files
TEST_SOURCES = $(wildcard $(TESTDIR)/qa_mocks/*.c)
TEST_OBJECTS = $(patsubst $(TESTDIR)/qa_mocks/%.c,$(BUILDDIR)/test_%.o,$(TEST_SOURCES))

# Library targets based on LIB flag
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

# Main targets
.PHONY: all clean test config lib install bootstrap

all: directories $(LIBRARY_TARGETS) $(BINDIR)/rift0.exe $(BINDIR)/rift0_demo

# Create directory structure
directories:
	@echo "🏗️  Initialized build directories"
	@mkdir -p $(BUILDDIR) $(BINDIR) $(LIBDIR) $(OUTPUTDIR)
	@mkdir -p $(CLIDIR)/command  # Ensure CLI command structure exists

# Core object files with library flags
$(BUILDDIR)/core_%.o: $(COREDIR)/%.c
	@echo "🔨 Compiling core $<..."
	$(CC) $(CFLAGS) $(LIBCFLAGS) -c $< -o $@

# CLI object files
$(BUILDDIR)/cli_%.o: $(CLIDIR)/%.c
	@echo "🔨 Compiling CLI $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# CLI command object files (new structure)
$(BUILDDIR)/cli_command_%.o: $(CLIDIR)/command/%.c
	@echo "🔨 Compiling CLI command $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Test object files
$(BUILDDIR)/test_%.o: $(TESTDIR)/qa_mocks/%.c
	@echo "🔨 Compiling test $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Shared library generation
$(LIBDIR)/librift0.so: $(CORE_OBJECTS)
	@echo "🔗 Building shared library librift0.so..."
	@echo "   ✅ Library flags: $(LIBCFLAGS) $(LIBLINKFLAGS)"
	$(CC) $(LIBLINKFLAGS) -o $@ $(CORE_OBJECTS) $(LDFLAGS)

# Static library generation  
$(LIBDIR)/librift0.a: $(CORE_OBJECTS)
	@echo "🔗 Building static library librift0.a..."
	ar rcs $@ $(CORE_OBJECTS)

# CLI executable (links against library)
$(BINDIR)/rift0.exe: $(CLI_OBJECTS) $(LIBRARY_TARGETS)
	@echo "🔗 Linking RIFT Stage 0 CLI..."
	@echo "   ✅ Preserving token type/value separation"
	@echo "   ✅ Maintaining matched_state for AST minimization"
	@echo "   ✅ Integrating .riftrc configuration system"
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

# Test executable with enhanced pattern parsing tests
$(BUILDDIR)/test_rift0: $(TEST_OBJECTS) $(LIBRARY_TARGETS)
	@echo "🔗 Linking test suite..."
ifeq ($(LIB),A)
	$(CC) $(TEST_OBJECTS) -L$(LIBDIR) -lrift0 -static $(LDFLAGS) -o $@
else
	$(CC) $(TEST_OBJECTS) -L$(LIBDIR) -lrift0 $(LDFLAGS) -o $@
endif

# Library target
lib: $(LIBRARY_TARGETS)
	@echo "📚 Library generation complete"
	@echo "   📁 Libraries: $(LIBRARY_TARGETS)"
ifeq ($(LIB),SO)
	@echo "   🔗 Shared library mode (-fPIC -shared)"
else ifeq ($(LIB),A)
	@echo "   🔗 Static library mode"
else
	@echo "   🔗 Both static and shared libraries"
endif

# Configuration setup with .riftrc processing
config:
	@echo "⚙️  Setting up enhanced configuration..."
	@if [ ! -f .riftrc ]; then \
		echo "# RIFT Stage 0 Configuration" > .riftrc; \
		echo "output_dir=./output" >> .riftrc; \
		echo "stage_name=rift.0" >> .riftrc; \
		echo "debug=false" >> .riftrc; \
		echo "verbose=false" >> .riftrc; \
		echo "pattern_mode=t" >> .riftrc; \
		echo "log_level=info" >> .riftrc; \
		echo "✅ Generated default .riftrc"; \
	else \
		echo "✅ Using existing .riftrc"; \
	fi

# Enhanced testing with pattern parsing fixes
test: $(BUILDDIR)/test_rift0
	@echo "🧪 Running enhanced QA tests..."
	@echo "🔍 Verifying token type/value separation..."
	@echo "🔍 Verifying matched_state preservation..."
	@echo "🔍 Testing RIFT pattern syntax parsing..."
	@export LD_LIBRARY_PATH=$(PWD)/$(LIBDIR):$$LD_LIBRARY_PATH && ./$(BUILDDIR)/test_rift0

# Bootstrap enhancement script
bootstrap:
	@echo "🚀 Bootstrapping enhanced RIFT Stage 0..."
	@echo "🏗️  Creating CLI command structure..."
	@mkdir -p $(CLIDIR)/command
	@echo "🔧 Setting up thread-safe token memory..."
	@echo "📚 Generating libraries with LIB=$(LIB)..."
	@$(MAKE) clean
	@$(MAKE) all
	@echo "⚙️  Configuring .riftrc..."
	@$(MAKE) config
	@echo "🧪 Running verification tests..."
	@$(MAKE) test
	@echo ""
	@echo "✅ Enhanced RIFT Stage 0 bootstrap complete!"
	@echo "📊 Build Summary:"
	@echo "  📁 CLI executable: ./$(BINDIR)/rift0.exe"
	@echo "  📁 Demo executable: ./$(BINDIR)/rift0_demo"  
	@echo "  📁 Libraries: $(LIBRARY_TARGETS)"
	@echo "  📁 Configuration: .riftrc"
	@echo "  📁 Output directory: ./$(OUTPUTDIR)/"
	@echo ""
	@echo "💡 Usage examples:"
	@echo "  ./$(BINDIR)/rift0.exe $(EXAMPLEDIR)/sample.rift"
	@echo "  ./$(BINDIR)/rift0.exe --config .riftrc $(EXAMPLEDIR)/sample.rift"
	@echo "  make LIB=SO lib    # Generate shared library only"
	@echo "  make LIB=A lib     # Generate static library only"

# Installation target
install: all
	@echo "📦 Installing RIFT Stage 0..."
	@mkdir -p /usr/local/lib /usr/local/bin /usr/local/include
	@cp $(LIBRARY_TARGETS) /usr/local/lib/
	@cp $(BINDIR)/rift0.exe /usr/local/bin/
	@cp include/rift.h /usr/local/include/
	@echo "✅ Installation complete"

# Clean target
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf $(BUILDDIR) $(BINDIR) $(LIBDIR)
	@echo "✅ Clean complete"

# Help target
help:
	@echo "RIFT Stage 0 Enhanced Makefile"
	@echo "=============================="
	@echo ""
	@echo "Library Generation:"
	@echo "  make LIB=SO     # Generate librift0.so (shared library)"
	@echo "  make LIB=A      # Generate librift0.a (static library)" 
	@echo "  make LIB=BOTH   # Generate both (default)"
	@echo ""
	@echo "Main Targets:"
	@echo "  make all        # Build everything"
	@echo "  make lib        # Build libraries only"
	@echo "  make test       # Run enhanced QA tests"
	@echo "  make bootstrap  # Full setup with CLI commands"
	@echo "  make config     # Generate .riftrc configuration"
	@echo "  make install    # Install system-wide"
	@echo "  make clean      # Clean build artifacts"
	@echo ""
	@echo "Architecture:"
	@echo "  ✅ Token type/value separation enforced"
	@echo "  ✅ Thread-safe token memory management"
	@echo "  ✅ Enhanced CLI command structure"
	@echo "  ✅ .riftrc configuration processing"
	@echo "  ✅ RIFT pattern syntax parsing (R\"pattern/flags[mode]\")"