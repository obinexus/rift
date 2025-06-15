# ===== Makefile - RIFT Stage 1 AEGIS Parser =====
# OBINexus Computing - TokenMemory → Parser Pipeline
# Build orchestration: nlink → polybuild integration
# Toolchain: riftlang.exe → .so.a → rift.exe → gosilang

# ===== Compiler Configuration =====
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude -g -O2 -MMD -MP -pthread -fPIC
LDFLAGS = -pthread -lm
AR = ar
ARFLAGS = rcs

# ===== Directory Structure =====
SRC_DIR = src
CORE_DIR = $(SRC_DIR)/core
CLI_DIR = $(SRC_DIR)/cli
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin
LIB_DIR = lib
TEST_DIR = tests
SCRIPTS_DIR = scripts
DOCS_DIR = docs

# ===== Core Library Sources =====
CORE_SOURCES = \
	$(CORE_DIR)/rift_parser.c \
	$(CORE_DIR)/rift_ast.c \
	$(CORE_DIR)/rift_automaton.c \
	$(CORE_DIR)/rift_token_memory.c \
	$(CORE_DIR)/rift_ir.c \
	$(CORE_DIR)/rift_core_utils.c

# ===== CLI Sources =====
CLI_SOURCES = $(CLI_DIR)/main.c $(CLI_DIR)/cli_args.c $(CLI_DIR)/cli_interface.c

# ===== Object Files =====
CORE_OBJECTS = $(CORE_SOURCES:$(CORE_DIR)/%.c=$(BUILD_DIR)/core/%.o)
CLI_OBJECTS = $(CLI_SOURCES:$(CLI_DIR)/%.c=$(BUILD_DIR)/cli/%.o)
ALL_OBJECTS = $(CORE_OBJECTS) $(CLI_OBJECTS)

# ===== Dependency Files =====
CORE_DEPS = $(CORE_SOURCES:$(CORE_DIR)/%.c=$(BUILD_DIR)/deps/core/%.d)
CLI_DEPS = $(CLI_SOURCES:$(CLI_DIR)/%.c=$(BUILD_DIR)/deps/cli/%.d)
ALL_DEPS = $(CORE_DEPS) $(CLI_DEPS)

# ===== Build Targets =====
TARGET = $(BIN_DIR)/rift1.exe
STATIC_LIB = $(LIB_DIR)/librift1.a
SHARED_LIB = $(LIB_DIR)/librift1.so

# ===== Phony Targets =====
.PHONY: all clean bootstrap install test verify-architecture
.PHONY: core-lib static-lib shared-lib cli debug release
.PHONY: nlink-integration polybuild-setup validate-obinexus

# ===== Default Target =====
all: directories $(TARGET) $(STATIC_LIB)

# ===== OBINexus Bootstrap =====
bootstrap: clean directories core-lib static-lib $(TARGET)
	@echo "🏗️  RIFT Stage 1 AEGIS Parser - Bootstrap Complete"
	@echo "✅ Core library: $(STATIC_LIB)"
	@echo "✅ CLI executable: $(TARGET)"
	@echo "✅ TokenMemory → Parser Pipeline ready"
	@echo "🔗 Toolchain: riftlang.exe → .so.a → rift.exe → gosilang"
	@echo "📋 AEGIS: Regex token matching + dual parsing strategies"

# ===== Directory Creation =====
directories:
	@mkdir -p $(BUILD_DIR)/core $(BUILD_DIR)/cli $(BUILD_DIR)/deps/core $(BUILD_DIR)/deps/cli
	@mkdir -p $(BIN_DIR) $(LIB_DIR) $(TEST_DIR) $(DOCS_DIR)
	@mkdir -p $(SCRIPTS_DIR)/build $(SCRIPTS_DIR)/validation

# ===== Core Library Build =====
core-lib: $(CORE_OBJECTS)
	@echo "📚 RIFT Stage 1 core library objects ready"

static-lib: $(STATIC_LIB)

$(STATIC_LIB): $(CORE_OBJECTS) | directories
	@echo "🔗 Creating RIFT Stage 1 static library..."
	$(AR) $(ARFLAGS) $@ $^
	@echo "✅ Static library: $@"

shared-lib: $(SHARED_LIB)

$(SHARED_LIB): $(CORE_OBJECTS) | directories
	@echo "🔗 Creating RIFT Stage 1 shared library..."
	$(CC) -shared $(CORE_OBJECTS) -o $@ $(LDFLAGS)
	@echo "✅ Shared library: $@"

# ===== CLI Executable =====
cli: $(TARGET)

$(TARGET): $(CLI_OBJECTS) $(STATIC_LIB) | directories
	@echo "🔗 Linking RIFT Stage 1 CLI executable..."
	$(CC) $(CLI_OBJECTS) -L$(LIB_DIR) -lrift1 -o $@ $(LDFLAGS)
	@echo "✅ CLI executable: $@"

# ===== Object File Rules =====
$(BUILD_DIR)/core/%.o: $(CORE_DIR)/%.c | directories
	@echo "🔨 Compiling core: $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/cli/%.o: $(CLI_DIR)/%.c | directories
	@echo "🔨 Compiling CLI: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# ===== Include Dependencies =====
-include $(ALL_DEPS)

# ===== Installation =====
install: all
	@echo "📦 Installing RIFT Stage 1..."
	install -d /usr/local/bin /usr/local/lib /usr/local/include/rift1
	install $(TARGET) /usr/local/bin/
	install $(STATIC_LIB) /usr/local/lib/
	cp -r $(INCLUDE_DIR)/rift1/* /usr/local/include/rift1/
	@echo "✅ RIFT Stage 1 installed to /usr/local"

# ===== Testing =====
test: all
	@echo "🧪 Running RIFT Stage 1 test suite..."
	@$(SCRIPTS_DIR)/validation/run_tests.sh
	@echo "✅ All tests passed"

# ===== Architecture Validation =====
verify-architecture: all
	@echo "🔍 Validating AEGIS architecture compliance..."
	@$(SCRIPTS_DIR)/validation/validate_architecture.sh
	@echo "✅ AEGIS architecture validated"

validate-obinexus: verify-architecture
	@echo "🔍 Validating OBINexus compliance..."
	@$(SCRIPTS_DIR)/validation/validate_obinexus.sh
	@echo "✅ OBINexus compliance validated"

# ===== Build Orchestration Integration =====
nlink-integration:
	@echo "🔗 Setting up nlink integration..."
	@$(SCRIPTS_DIR)/build/setup_nlink.sh
	@echo "✅ nlink integration ready"

polybuild-setup: nlink-integration
	@echo "🏗️  Setting up polybuild orchestration..."
	@$(SCRIPTS_DIR)/build/setup_polybuild.sh
	@echo "✅ polybuild orchestration ready"

# ===== Debug/Release Builds =====
debug: CFLAGS += -DDEBUG -g3 -O0
debug: all

release: CFLAGS += -DNDEBUG -O3 -flto
release: LDFLAGS += -flto
release: all

# ===== Cleanup =====
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(LIB_DIR)
	@echo "✅ Clean complete"

distclean: clean
	@echo "🧹 Deep cleaning..."
	rm -rf $(DOCS_DIR)/generated $(TEST_DIR)/reports
	@echo "✅ Distribution clean complete"

# ===== Help =====
help:
	@echo "🔤 RIFT Stage 1 AEGIS Parser - Build System"
	@echo "============================================"
	@echo ""
	@echo "Targets:"
	@echo "  all                 - Build everything (default)"
	@echo "  bootstrap          - Full OBINexus bootstrap"
	@echo "  core-lib           - Build core library objects"
	@echo "  static-lib         - Build static library"
	@echo "  shared-lib         - Build shared library"
	@echo "  cli                - Build CLI executable"
	@echo "  test               - Run test suite"
	@echo "  verify-architecture - Validate AEGIS compliance"
	@echo "  validate-obinexus  - Validate OBINexus compliance"
	@echo "  install            - Install to system"
	@echo "  clean              - Clean build artifacts"
	@echo "  help               - Show this help"
	@echo ""
	@echo "Build Orchestration:"
	@echo "  nlink-integration  - Setup nlink integration"
	@echo "  polybuild-setup    - Setup polybuild orchestration"
	@echo ""
	@echo "🏗️  Architecture: AEGIS Parser with dual parsing strategies"
	@echo "🔗 Toolchain: riftlang.exe → .so.a → rift.exe → gosilang"

# ===== Project Structure Generation =====
create-structure:
	@echo "📁 Creating RIFT Stage 1 project structure..."
	@mkdir -p src/core src/cli include/rift1/core
	@mkdir -p tests/unit tests/integration tests/qa_mocks
	@mkdir -p scripts/build scripts/validation scripts/deployment
	@mkdir -p examples docs bin lib build
	@touch src/core/.gitkeep src/cli/.gitkeep
	@touch tests/unit/.gitkeep tests/integration/.gitkeep
	@touch examples/.gitkeep docs/.gitkeep
	@echo "✅ Project structure created"

# ===== Development Utilities =====
format:
	@echo "🎨 Formatting source code..."
	@find src include -name "*.c" -o -name "*.h" | xargs clang-format -i
	@echo "✅ Code formatted"

analyze:
	@echo "🔍 Running static analysis..."
	@cppcheck --enable=all --inconclusive src/
	@echo "✅ Static analysis complete"

docs:
	@echo "📖 Generating documentation..."
	@doxygen docs/Doxyfile
	@echo "✅ Documentation generated"

# ===== File Generation Targets =====
generate-headers: directories
	@echo "📝 Generating core headers..."
	@$(SCRIPTS_DIR)/build/generate_headers.sh

generate-stubs: directories  
	@echo "📝 Generating implementation stubs..."
	@$(SCRIPTS_DIR)/build/generate_stubs.sh

# ===== Dependency Information =====
show-deps:
	@echo "📋 RIFT Stage 1 Dependencies:"
	@echo "  - libc (C11 standard library)"
	@echo "  - POSIX regex support"
	@echo "  - pthread for threading"
	@echo "  - libm for math functions"
	@echo ""
	@echo "🏗️  Build Tools:"
	@echo "  - gcc (C11 support required)"
	@echo "  - make (GNU Make compatible)"
	@echo "  - ar (for static libraries)"
	@echo ""
	@echo "🔗 OBINexus Integration:"
	@echo "  - nlink (build orchestration)"
	@echo "  - polybuild (pipeline management)"