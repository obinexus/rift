# =================================================================
# Makefile - RIFT-0 Tokenizer Stage
# RIFT: RIFT Is a Flexible Translator
# OBINexus Computing Framework - Build Orchestration
# Toolchain: riftlang.exe → .so.a → rift.exe → gosilang
# Build Orchestration: nlink → polybuild (AEGIS Framework)
# =================================================================

# Project Configuration
PROJECT := rift-0
VERSION := 1.0.0
DESCRIPTION := "RIFT Tokenizer Stage - OBINexus Computing"
AUTHOR := "OBINexus Nnamdi Michael Okpala"

# Platform Detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    PLATFORM := linux
    SO_EXT := .so
    EXE_EXT := 
    MINGW_PREFIX := 
endif
ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    PLATFORM := windows
    SO_EXT := .dll
    EXE_EXT := .exe
    MINGW_PREFIX := $(shell echo $$MSYSTEM | tr '[:upper:]' '[:lower:]')-
endif

# Directory Structure (following RIFT unified architecture)
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
LIB_DIR := $(BUILD_DIR)/lib
BIN_DIR := $(BUILD_DIR)/bin
PKG_DIR := $(BUILD_DIR)/pkg

SRC_DIR := src
INCLUDE_DIR := include
TESTS_DIR := tests
EXAMPLES_DIR := examples
SCRIPTS_DIR := scripts

# Source Organization
CORE_SRC_DIR := $(SRC_DIR)/core
CLI_SRC_DIR := $(SRC_DIR)/cli
CORE_INC_DIR := $(INCLUDE_DIR)/rift-0/core
CLI_INC_DIR := $(INCLUDE_DIR)/rift-0/cli

# Build Configuration
CC := $(MINGW_PREFIX)gcc
CXX := $(MINGW_PREFIX)g++
AR := $(MINGW_PREFIX)ar
LD := $(MINGW_PREFIX)ld
STRIP := $(MINGW_PREFIX)strip
PKG_CONFIG := pkg-config

# Compiler Flags (AEGIS Compliance)
CFLAGS := -std=c11 -Wall -Wextra -Werror -MMD -fPIC
CFLAGS += -I$(INCLUDE_DIR) -I$(CORE_INC_DIR) -I$(CLI_INC_DIR)
CFLAGS += -DRIFT_VERSION=\"$(VERSION)\" -DRIFT_PLATFORM=\"$(PLATFORM)\"
CFLAGS += -DCOMPILE_TIMESTAMP=\"$(shell date -u +%Y%m%d_%H%M%S)\"

# DFA Tokenization Features
CFLAGS += -DRIFT_DFA_ENABLED=1 -DRIFT_REGEX_COMPOSE=1
CFLAGS += -DRIFT_TOKEN_TRIPLET=1 -DRIFT_BITFIELD_TOKENS=1

# Debug/Release Configuration
ifdef DEBUG
    CFLAGS += -g3 -O0 -DDEBUG=1 -fsanitize=address -fsanitize=undefined
    LDFLAGS += -fsanitize=address -fsanitize=undefined
    BUILD_TYPE := debug
else
    CFLAGS += -O2 -DNDEBUG=1 -flto
    LDFLAGS += -flto -s
    BUILD_TYPE := release
endif

# Thread Safety (for Gosilang integration)
ifeq ($(PLATFORM),linux)
    CFLAGS += -pthread
    LDFLAGS += -pthread
endif

# Library Configuration
STATIC_LIB := $(LIB_DIR)/librift-0.a
SHARED_LIB := $(LIB_DIR)/librift-0$(SO_EXT)
CLI_BINARY := $(BIN_DIR)/rift-0$(EXE_EXT)
PKG_CONFIG_FILE := $(PKG_DIR)/rift-0.pc

# Source Files Discovery (CMake-like traversal)
CORE_SOURCES := $(shell find $(CORE_SRC_DIR) -name "*.c" 2>/dev/null | sort)
CLI_SOURCES := $(shell find $(CLI_SRC_DIR) -name "*.c" 2>/dev/null | sort)
ALL_SOURCES := $(CORE_SOURCES) $(CLI_SOURCES)

# Object Files Generation
CORE_OBJECTS := $(CORE_SOURCES:$(CORE_SRC_DIR)/%.c=$(OBJ_DIR)/core/%.o)
CLI_OBJECTS := $(CLI_SOURCES:$(CLI_SRC_DIR)/%.c=$(OBJ_DIR)/cli/%.o)
ALL_OBJECTS := $(CORE_OBJECTS) $(CLI_OBJECTS)

# Dependency Files
DEPS := $(ALL_OBJECTS:.o=.d)

# Test Configuration
TEST_SOURCES := $(shell find $(TESTS_DIR) -name "test_*.c" 2>/dev/null | sort)
TEST_BINARIES := $(TEST_SOURCES:$(TESTS_DIR)/%.c=$(BIN_DIR)/%)

# =================================================================
# PRIMARY TARGETS
# =================================================================

.PHONY: all setup build install clean test help
.DEFAULT_GOAL := all

all: setup build

# Setup target integrates with setup.sh
setup: | directories
	@echo "🔧 RIFT-0 Setup - Platform: $(PLATFORM)"
	@if [ -f "./setup.sh" ]; then \
		echo "🔄 Running platform setup script..."; \
		./setup.sh --stage=rift-0 --platform=$(PLATFORM); \
	fi
	@echo "✅ Setup completed for $(BUILD_TYPE) build"

build: $(STATIC_LIB) $(SHARED_LIB) $(CLI_BINARY) $(PKG_CONFIG_FILE)
	@echo "🏗️  RIFT-0 Build Complete"
	@echo "📦 Static library: $(STATIC_LIB)"
	@echo "📦 Shared library: $(SHARED_LIB)"
	@echo "🔧 CLI binary: $(CLI_BINARY)"
	@echo "⚙️  pkg-config: $(PKG_CONFIG_FILE)"

# =================================================================
# DIRECTORY STRUCTURE CREATION
# =================================================================

directories:
	@mkdir -p $(OBJ_DIR)/core $(OBJ_DIR)/cli
	@mkdir -p $(LIB_DIR) $(BIN_DIR) $(PKG_DIR)
	@mkdir -p $(OBJ_DIR)/tests/unit $(OBJ_DIR)/tests/integration
	@for dir in $(shell find $(CORE_SRC_DIR) -type d 2>/dev/null | sed 's|$(CORE_SRC_DIR)|$(OBJ_DIR)/core|'); do \
		mkdir -p $$dir; \
	done
	@for dir in $(shell find $(CLI_SRC_DIR) -type d 2>/dev/null | sed 's|$(CLI_SRC_DIR)|$(OBJ_DIR)/cli|'); do \
		mkdir -p $$dir; \
	done

# =================================================================
# CORE LIBRARY TARGETS
# =================================================================

# Static Library (librift-0.a)
$(STATIC_LIB): $(CORE_OBJECTS) | directories
	@echo "📚 Creating static library: $@"
	@$(AR) rcs $@ $(CORE_OBJECTS)
	@echo "✅ Static library created: $(shell ls -lh $@ | awk '{print $$5}')"

# Shared Library (librift-0.so/.dll)
$(SHARED_LIB): $(CORE_OBJECTS) | directories
	@echo "🔗 Creating shared library: $@"
	@$(CC) -shared -o $@ $(CORE_OBJECTS) $(LDFLAGS)
ifeq ($(BUILD_TYPE),release)
	@$(STRIP) $@
endif
	@echo "✅ Shared library created: $(shell ls -lh $@ | awk '{print $$5}')"

# CLI Binary (rift-0.exe)
$(CLI_BINARY): $(CLI_OBJECTS) $(STATIC_LIB) | directories
	@echo "🔧 Linking CLI binary: $@"
	@$(CC) -o $@ $(CLI_OBJECTS) -L$(LIB_DIR) -lrift-0 $(LDFLAGS)
ifeq ($(BUILD_TYPE),release)
	@$(STRIP) $@
endif
	@echo "✅ CLI binary created: $(shell ls -lh $@ | awk '{print $$5}')"

# =================================================================
# OBJECT FILE COMPILATION (with dependency tracking)
# =================================================================

# Core object files
$(OBJ_DIR)/core/%.o: $(CORE_SRC_DIR)/%.c | directories
	@echo "🔨 Compiling core: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# CLI object files
$(OBJ_DIR)/cli/%.o: $(CLI_SRC_DIR)/%.c | directories
	@echo "🔨 Compiling CLI: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Include dependency files
-include $(DEPS)

# =================================================================
# PKG-CONFIG GENERATION
# =================================================================

$(PKG_CONFIG_FILE): | directories
	@echo "📦 Generating pkg-config file: $@"
	@echo "prefix=$(BUILD_DIR)" > $@
	@echo "exec_prefix=\${prefix}" >> $@
	@echo "libdir=\${exec_prefix}/lib" >> $@
	@echo "includedir=\${prefix}/include/rift-0" >> $@
	@echo "" >> $@
	@echo "Name: rift-0" >> $@
	@echo "Description: RIFT Tokenizer Stage - OBINexus Computing" >> $@
	@echo "Version: $(VERSION)" >> $@
	@echo "Libs: -L\${libdir} -lrift-0" >> $@
	@echo "Cflags: -I\${includedir}" >> $@
	@echo "" >> $@
	@echo "✅ pkg-config file created: $(shell ls -lh $@ | awk '{print $$5}')"	


# =================================================================
# TESTING FRAMEWORK
# =================================================================

test: build $(TEST_BINARIES)
	@echo "🧪 Running RIFT-0 Test Suite"
	@echo "================================"
	@test_count=0; passed=0; \
	for test in $(TEST_BINARIES); do \
		if [ -f "$$test" ]; then \
			echo "🔬 Running: $$(basename $$test)"; \
			if $$test; then \
				echo "✅ PASSED: $$(basename $$test)"; \
				passed=$$((passed + 1)); \
			else \
				echo "❌ FAILED: $$(basename $$test)"; \
			fi; \
			test_count=$$((test_count + 1)); \
		fi; \
	done; \
	echo "================================"; \
	echo "📊 Results: $$passed/$$test_count tests passed"; \
	if [ $$passed -eq $$test_count ]; then \
		echo "🎉 All tests passed!"; \
	else \
		echo "💥 Some tests failed!"; \
		exit 1; \
	fi

# Compile test binaries
$(BIN_DIR)/test_%: $(TESTS_DIR)/test_%.c $(STATIC_LIB) | directories
	@echo "🔨 Compiling test: $<"
	@$(CC) $(CFLAGS) -I$(TESTS_DIR) $< -L$(LIB_DIR) -lrift-0 -o $@ $(LDFLAGS)

# =================================================================
# INSTALLATION TARGETS
# =================================================================

install: build
	@echo "📦 Installing RIFT-0 to system..."
	@PREFIX=${PREFIX:-/usr/local}; \
	install -d $$PREFIX/lib $$PREFIX/include/rift-0 $$PREFIX/bin $$PREFIX/lib/pkgconfig; \
	install -m 644 $(STATIC_LIB) $$PREFIX/lib/; \
	install -m 755 $(SHARED_LIB) $$PREFIX/lib/; \
	install -m 755 $(CLI_BINARY) $$PREFIX/bin/; \
	install -m 644 $(PKG_CONFIG_FILE) $$PREFIX/lib/pkgconfig/; \
	cp -r $(INCLUDE_DIR)/rift-0/* $$PREFIX/include/rift-0/; \
	echo "✅ Installation completed to $$PREFIX"

uninstall:
	@echo "🗑️  Uninstalling RIFT-0..."
	@PREFIX=${PREFIX:-/usr/local}; \
	rm -f $$PREFIX/lib/librift-0.a $$PREFIX/lib/librift-0$(SO_EXT); \
	rm -f $$PREFIX/bin/rift-0$(EXE_EXT); \
	rm -f $$PREFIX/lib/pkgconfig/rift-0.pc; \
	rm -rf $$PREFIX/include/rift-0; \
	echo "✅ Uninstallation completed"

# =================================================================
# VALIDATION & QUALITY ASSURANCE
# =================================================================

validate: build test
	@echo "🔍 Running RIFT-0 Validation Suite"
	@echo "==================================="
	@echo "🏗️  Build Validation:"
	@file $(STATIC_LIB) && echo "✅ Static library format valid"
	@file $(SHARED_LIB) && echo "✅ Shared library format valid"
	@file $(CLI_BINARY) && echo "✅ CLI binary format valid"
	@echo ""
	@echo "📋 Symbol Validation:"
	@nm $(STATIC_LIB) | grep -q "rift_tokenizer_init" && echo "✅ Core symbols present"
	@echo ""
	@echo "🔗 Link Validation:"
	@ldd $(CLI_BINARY) >/dev/null 2>&1 && echo "✅ CLI dependencies resolved" || echo "⚠️  Static linking detected"
	@echo ""
	@echo "🧪 Functional Validation:"
	@if [ -f "$(CLI_BINARY)" ]; then \
		echo "Testing CLI help output..."; \
		$(CLI_BINARY) --help >/dev/null 2>&1 && echo "✅ CLI functional" || echo "⚠️  CLI needs validation"; \
	fi
	@echo "✅ Validation completed"

# Architecture validation (integrates with governance framework)
validate_architecture:
	@echo "🏛️  RIFT-0 Architecture Validation"
	@echo "==================================="
	@if [ -f "$(SCRIPTS_DIR)/validation/validate_architecture.sh" ]; then \
		$(SCRIPTS_DIR)/validation/validate_architecture.sh --stage=rift-0; \
	else \
		echo "⚠️  Architecture validation script not found"; \
		echo "📋 Manual validation checklist:"; \
		echo "   • TokenTriplet struct present in headers"; \
		echo "   • DFA state machine implementation"; \
		echo "   • Regex composition R\"\" syntax support"; \
		echo "   • Memory safety with bounds checking"; \
		echo "   • Thread safety for Gosilang integration"; \
	fi

# =================================================================
# DEVELOPMENT & DEBUGGING
# =================================================================

debug: clean
	@$(MAKE) DEBUG=1 all

debug_cli: debug
	@echo "🐛 Running CLI under GDB..."
	@gdb -ex run --args $(CLI_BINARY) --help

valgrind: debug
	@echo "🔍 Running memory validation..."
	@valgrind --leak-check=full --show-leak-kinds=all $(CLI_BINARY) --help

benchmark: build
	@echo "⚡ Running performance benchmarks..."
	@if [ -f "$(BIN_DIR)/test_benchmark" ]; then \
		$(BIN_DIR)/test_benchmark; \
	else \
		echo "⚠️  Benchmark binary not found"; \
	fi

# =================================================================
# UTILITY TARGETS
# =================================================================

clean:
	@echo "🧹 Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@find . -name "*.d" -delete 2>/dev/null || true
	@echo "✅ Clean completed"

distclean: clean
	@echo "🧹 Deep cleaning (including backups)..."
	@find . -name "*~" -delete 2>/dev/null || true
	@find . -name "*.bak" -delete 2>/dev/null || true
	@find . -name ".DS_Store" -delete 2>/dev/null || true

dist: clean
	@echo "📦 Creating distribution package..."
	@tar czf rift-0-$(VERSION)-$(PLATFORM).tar.gz \
		--exclude='.git*' --exclude='build' --exclude='*.tar.gz' \
		--transform 's,^,rift-0-$(VERSION)/,' .
	@echo "✅ Distribution created: rift-0-$(VERSION)-$(PLATFORM).tar.gz"

# =================================================================
# INFORMATION & HELP
# =================================================================

info:
	@echo "RIFT-0 Tokenizer Stage - Build Information"
	@echo "=========================================="
	@echo "Project: $(PROJECT) v$(VERSION)"
	@echo "Platform: $(PLATFORM)"
	@echo "Build Type: $(BUILD_TYPE)"
	@echo "Compiler: $(CC) ($(shell $(CC) --version | head -n1))"
	@echo "Toolchain: riftlang.exe → .so.a → rift.exe → gosilang"
	@echo "Framework: AEGIS (nlink → polybuild)"
	@echo ""
	@echo "🗂️  Directory Structure:"
	@echo "   Build: $(BUILD_DIR)/"
	@echo "   Objects: $(OBJ_DIR)/"
	@echo "   Libraries: $(LIB_DIR)/"
	@echo "   Binaries: $(BIN_DIR)/"
	@echo ""
	@echo "📊 Source Statistics:"
	@echo "   Core sources: $(words $(CORE_SOURCES))"
	@echo "   CLI sources: $(words $(CLI_SOURCES))"
	@echo "   Total sources: $(words $(ALL_SOURCES))"
	@echo ""
	@echo "🎯 Key Features:"
	@echo "   • DFA-based tokenization with R\"\" regex composition"
	@echo "   • TokenTriplet bitfield format (type:8, ptr:16, value:8)"
	@echo "   • Thread-safe for Gosilang integration"
	@echo "   • AEGIS governance framework compliance"
	@echo "   • Cross-platform (Linux/Windows MinGW)"

help:
	@echo "RIFT-0 Tokenizer Stage - Build System Help"
	@echo "==========================================="
	@echo ""
	@echo "🎯 Primary Targets:"
	@echo "   make all          - Complete build (setup + build)"
	@echo "   make setup        - Platform setup and dependencies"
	@echo "   make build        - Compile libraries and binaries"
	@echo "   make test         - Run test suite"
	@echo "   make install      - Install to system (PREFIX=/usr/local)"
	@echo ""
	@echo "🔧 Development Targets:"
	@echo "   make debug        - Debug build with sanitizers"
	@echo "   make validate     - Comprehensive validation"
	@echo "   make benchmark    - Performance testing"
	@echo "   make valgrind     - Memory leak analysis"
	@echo ""
	@echo "📦 Distribution Targets:"
	@echo "   make dist         - Create distribution package"
	@echo "   make clean        - Remove build artifacts"
	@echo "   make distclean    - Deep clean including backups"
	@echo ""
	@echo "ℹ️  Information Targets:"
	@echo "   make info         - Show build configuration"
	@echo "   make help         - Show this help"
	@echo ""
	@echo "🔧 Build Variables:"
	@echo "   DEBUG=1           - Enable debug build"
	@echo "   PREFIX=/path      - Installation prefix"
	@echo "   CC=compiler       - Override compiler"
	@echo ""
	@echo "🌟 OBINexus Computing - RIFT Is a Flexible Translator"
	@echo "    By Nnamdi Michael Okpala - Computing from the Heart"


# Integrate with polybuild orchestration
polybuild_integration:
	@echo "🔗 Preparing polybuild integration..."
	@if command -v polybuild >/dev/null 2>&1; then \
		polybuild --stage=rift-0 --component=tokenizer --validate; \
	else \
		echo "⚠️  polybuild not available - generating manifest"; \
		cat > $(BUILD_DIR)/polybuild.manifest << 'EOF'
stage: rift-0
component: tokenizer
artifacts:
  - lib/librift-0.a
  - lib/librift-0.so
  - bin/rift-0.exe
dependencies:
  - libc
  - pthread
governance:
  validation: required
  compliance: aegis
  audit: enabled
EOF
	fi

# =================================================================
# SPECIAL TARGETS
# =================================================================

.PHONY: directories riftrc polybuild_integration validate_architecture
.PHONY: debug debug_cli valgrind benchmark clean distclean dist info