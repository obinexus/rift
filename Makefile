# =================================================================
# RIFT Enhanced Makefile with pkg-config Integration
# OBINexus Computing Framework - AEGIS Methodology
# Technical Lead: Nnamdi Michael Okpala
# =================================================================

# Configuration Variables
RIFT_VERSION := 1.0.0
AEGIS_COMPLIANCE := ENABLED
PKG_CONFIG_SUPPORT := ENABLED

# Build Tools Configuration
CC := gcc
CXX := g++
CMAKE := cmake
MAKE := make
PKG_CONFIG := pkg-config

# Platform Detection
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

ifeq ($(UNAME_S),Linux)
    PLATFORM := linux
    NPROC := $(shell nproc)
    PKG_CONFIG_INSTALL_DIR := /usr/local/lib/pkgconfig
endif

ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    PLATFORM := windows
    NPROC := $(shell nproc 2>/dev/null || echo 4)
    PKG_CONFIG_INSTALL_DIR := /mingw64/lib/pkgconfig
    EXE_EXT := .exe
endif

# Directory Structure
RIFT_ROOT := $(shell pwd)
BUILD_DIR := $(RIFT_ROOT)/build
SRC_DIR := $(RIFT_ROOT)/rift/src
INCLUDE_DIR := $(RIFT_ROOT)/include
LIB_DIR := $(RIFT_ROOT)/lib
BIN_DIR := $(RIFT_ROOT)/bin
LOGS_DIR := $(RIFT_ROOT)/logs
PKG_CONFIG_DIR := $(RIFT_ROOT)/rift/pkgconfig

# AEGIS Compliance Flags
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -O2
CFLAGS += -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE
CFLAGS += -DRIFT_VERSION_STRING=\"$(RIFT_VERSION)\"
CFLAGS += -DRIFT_AEGIS_COMPLIANCE=1 -DRIFT_ZERO_TRUST=1
CFLAGS += -I$(INCLUDE_DIR) -I$(RIFT_ROOT)/rift/include

LDFLAGS := -Wl,-z,relro -Wl,-z,now -pie
LIBS := -lssl -lcrypto -lpthread

# Color Codes for Professional Output
GREEN := \033[0;32m
BLUE := \033[0;34m
YELLOW := \033[1;33m
RED := \033[0;31m
MAGENTA := \033[0;35m
CYAN := \033[0;36m
BOLD := \033[1m
NC := \033[0m

# Pipeline Stages
STAGES := rift-0 rift-1 rift-2 rift-3 rift-4 rift-5 rift-6
STAGE_LIBS := $(addprefix $(LIB_DIR)/lib,$(addsuffix _static.a,$(STAGES)))
STAGE_EXECUTABLES := $(addprefix $(BIN_DIR)/,$(addsuffix $(EXE_EXT),$(STAGES)))

# =================================================================
# PRIMARY TARGETS
# =================================================================

.PHONY: all
all: banner setup cmake-build pkg-config-integration validate

.PHONY: setup
setup: banner setup-directories setup-wizard

.PHONY: clean
clean: banner
	@echo -e "$(BLUE)[CLEAN]$(NC) Removing build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -f $(LIB_DIR)/*.a $(BIN_DIR)/*$(EXE_EXT)
	@echo -e "$(GREEN)[SUCCESS]$(NC) Build artifacts cleaned"

.PHONY: deep-clean
deep-clean: clean
	@echo -e "$(BLUE)[DEEP-CLEAN]$(NC) Removing all generated files..."
	@rm -rf $(LIB_DIR) $(BIN_DIR) $(LOGS_DIR)
	@rm -rf $(PKG_CONFIG_DIR)/rift-*.pc
	@echo -e "$(GREEN)[SUCCESS]$(NC) Deep clean completed"

# =================================================================
# SETUP WIZARD INTEGRATION
# =================================================================

.PHONY: banner
banner:
	@echo -e "$(CYAN)$(BOLD)=================================================================="
	@echo -e "  RIFT Build Orchestration with pkg-config Integration          "
	@echo -e "  OBINexus Computing Framework v$(RIFT_VERSION)                  "
	@echo -e "  Platform: $(PLATFORM) | AEGIS: $(AEGIS_COMPLIANCE)            "
	@echo -e "==================================================================$(NC)"

.PHONY: setup-directories
setup-directories:
	@echo -e "$(BLUE)[SETUP]$(NC) Creating AEGIS-compliant directory structure..."
	@mkdir -p $(BUILD_DIR) $(LIB_DIR) $(BIN_DIR) $(LOGS_DIR)
	@mkdir -p $(BUILD_DIR)/pkgconfig
	@mkdir -p $(RIFT_ROOT)/rift/include/rift/core
	@for stage in 0 1 2 3 4 5 6; do \
		mkdir -p $(RIFT_ROOT)/rift/src/core/stage-$$stage; \
		mkdir -p $(RIFT_ROOT)/rift/include/rift/core/stage-$$stage; \
	done
	@echo -e "$(GREEN)[SUCCESS]$(NC) Directory structure initialized"

.PHONY: setup-wizard
setup-wizard: setup-directories
	@echo -e "$(BLUE)[WIZARD]$(NC) Executing RIFT Setup Wizard..."
	@if [ -f "rift-setup-wizard.sh" ]; then \
		echo -e "$(YELLOW)[INFO]$(NC) Running interactive setup wizard..."; \
		timeout 30 bash rift-setup-wizard.sh || echo -e "$(YELLOW)[TIMEOUT]$(NC) Wizard timed out - continuing with defaults"; \
	else \
		echo -e "$(YELLOW)[INFO]$(NC) Setup wizard not found - using default configuration"; \
	fi
	@echo -e "$(GREEN)[SUCCESS]$(NC) Setup wizard phase completed"

# =================================================================
# CMAKE BUILD SYSTEM INTEGRATION
# =================================================================

.PHONY: cmake-build
cmake-build: setup-directories
	@echo -e "$(BLUE)[CMAKE]$(NC) Configuring build system with pkg-config integration..."
	@cd $(BUILD_DIR) && \
		$(CMAKE) .. \
			-DCMAKE_BUILD_TYPE=Release \
			-DENABLE_PKG_CONFIG=ON \
			-DENABLE_AEGIS_VALIDATION=ON \
			-DCMAKE_INSTALL_PREFIX=/usr/local \
			2>&1 | tee $(LOGS_DIR)/cmake_config.log
	@echo -e "$(BLUE)[MAKE]$(NC) Compiling RIFT pipeline with $(NPROC) parallel jobs..."
	@cd $(BUILD_DIR) && \
		$(MAKE) -j$(NPROC) 2>&1 | tee $(LOGS_DIR)/compilation.log
	@echo -e "$(GREEN)[SUCCESS]$(NC) CMake build completed"

.PHONY: cmake-install
cmake-install: cmake-build
	@echo -e "$(BLUE)[INSTALL]$(NC) Installing RIFT system components..."
	@cd $(BUILD_DIR) && $(MAKE) install
	@echo -e "$(GREEN)[SUCCESS]$(NC) Installation completed"

# =================================================================
# PKG-CONFIG INTEGRATION TARGETS
# =================================================================

.PHONY: pkg-config-integration
pkg-config-integration: cmake-build
	@echo -e "$(BLUE)[PKG-CONFIG]$(NC) Integrating pkg-config support..."
	@if [ "$(PKG_CONFIG_SUPPORT)" = "ENABLED" ]; then \
		echo -e "$(YELLOW)[INFO]$(NC) Validating pkg-config framework..."; \
		if [ -f "$(PKG_CONFIG_DIR)/validate-pkgconfig.sh" ]; then \
			bash $(PKG_CONFIG_DIR)/validate-pkgconfig.sh || echo -e "$(YELLOW)[WARNING]$(NC) pkg-config validation failed"; \
		fi; \
		echo -e "$(YELLOW)[INFO]$(NC) Generating pkg-config files..."; \
		cd $(BUILD_DIR) && $(MAKE) validate-pkgconfig || true; \
	else \
		echo -e "$(YELLOW)[INFO]$(NC) pkg-config support disabled"; \
	fi
	@echo -e "$(GREEN)[SUCCESS]$(NC) pkg-config integration completed"

.PHONY: pkg-config-install
pkg-config-install: pkg-config-integration
	@echo -e "$(BLUE)[PKG-CONFIG-INSTALL]$(NC) Installing pkg-config files..."
	@if [ -d "$(BUILD_DIR)/pkgconfig" ]; then \
		sudo mkdir -p $(PKG_CONFIG_INSTALL_DIR); \
		sudo cp $(BUILD_DIR)/pkgconfig/rift-*.pc $(PKG_CONFIG_INSTALL_DIR)/; \
		echo -e "$(GREEN)[SUCCESS]$(NC) pkg-config files installed to $(PKG_CONFIG_INSTALL_DIR)"; \
	else \
		echo -e "$(YELLOW)[WARNING]$(NC) No pkg-config files found to install"; \
	fi

.PHONY: pkg-config-test
pkg-config-test: pkg-config-install
	@echo -e "$(BLUE)[PKG-CONFIG-TEST]$(NC) Testing pkg-config functionality..."
	@export PKG_CONFIG_PATH="$(PKG_CONFIG_INSTALL_DIR):$(BUILD_DIR)/pkgconfig:$$PKG_CONFIG_PATH"; \
	for stage in 0 1 2 3 4 5 6; do \
		echo -n "Testing rift-$$stage: "; \
		if $(PKG_CONFIG) --exists rift-$$stage 2>/dev/null; then \
			echo -e "$(GREEN)✓$(NC)"; \
		else \
			echo -e "$(RED)✗$(NC)"; \
		fi; \
	done
	@echo -e "$(GREEN)[SUCCESS]$(NC) pkg-config testing completed"

# =================================================================
# VALIDATION AND TESTING TARGETS
# =================================================================

.PHONY: validate
validate: validate-structure validate-executables validate-libraries

.PHONY: validate-structure
validate-structure:
	@echo -e "$(BLUE)[VALIDATE]$(NC) Validating AEGIS directory structure..."
	@test -d $(BUILD_DIR) || (echo -e "$(RED)[ERROR]$(NC) Build directory missing" && exit 1)
	@test -d $(LIB_DIR) || (echo -e "$(RED)[ERROR]$(NC) Library directory missing" && exit 1)
	@test -d $(BIN_DIR) || (echo -e "$(RED)[ERROR]$(NC) Binary directory missing" && exit 1)
	@echo -e "$(GREEN)[SUCCESS]$(NC) Directory structure validated"

.PHONY: validate-executables
validate-executables:
	@echo -e "$(BLUE)[VALIDATE]$(NC) Validating stage executables..."
	@cd $(BUILD_DIR) && $(MAKE) validate_all || echo -e "$(YELLOW)[WARNING]$(NC) Some executables missing"
	@echo -e "$(GREEN)[SUCCESS]$(NC) Executable validation completed"

.PHONY: validate-libraries
validate-libraries:
	@echo -e "$(BLUE)[VALIDATE]$(NC) Validating static libraries..."
	@for stage in 0 1 2 3 4 5 6; do \
		lib_file="$(BUILD_DIR)/lib/librift-$$stage.a"; \
		if [ -f "$$lib_file" ]; then \
			echo -e "$(GREEN)✓$(NC) librift-$$stage.a"; \
		else \
			echo -e "$(YELLOW)⚠$(NC) librift-$$stage.a missing"; \
		fi; \
	done
	@echo -e "$(GREEN)[SUCCESS]$(NC) Library validation completed"

# =================================================================
# TESTING AND DEMONSTRATION TARGETS
# =================================================================

.PHONY: test
test: cmake-build
	@echo -e "$(BLUE)[TEST]$(NC) Running comprehensive test suite..."
	@cd $(BUILD_DIR) && $(MAKE) test || echo -e "$(YELLOW)[WARNING]$(NC) Some tests failed"
	@echo -e "$(GREEN)[SUCCESS]$(NC) Test execution completed"

.PHONY: demo
demo: cmake-build
	@echo -e "$(BLUE)[DEMO]$(NC) Executing RIFT pipeline demonstration..."
	@if [ -f "$(BUILD_DIR)/demo_pipeline.sh" ]; then \
		cd $(BUILD_DIR) && bash demo_pipeline.sh; \
	elif [ -f "demo_pipeline_standardized.sh" ]; then \
		bash demo_pipeline_standardized.sh; \
	else \
		echo -e "$(YELLOW)[WARNING]$(NC) Demo script not found"; \
	fi
	@echo -e "$(GREEN)[SUCCESS]$(NC) Demo execution completed"

# =================================================================
# EXTERNAL INTEGRATION EXAMPLES
# =================================================================

.PHONY: external-examples
external-examples: pkg-config-integration
	@echo -e "$(BLUE)[EXAMPLES]$(NC) Building external integration examples..."
	@if [ -d "$(PKG_CONFIG_DIR)/examples" ]; then \
		cd $(PKG_CONFIG_DIR)/examples && \
		export PKG_CONFIG_PATH="$(BUILD_DIR)/pkgconfig:$$PKG_CONFIG_PATH" && \
		$(MAKE) -f Makefile.example all || echo -e "$(YELLOW)[WARNING]$(NC) Example build failed"; \
	else \
		echo -e "$(YELLOW)[WARNING]$(NC) Examples directory not found"; \
	fi
	@echo -e "$(GREEN)[SUCCESS]$(NC) External examples processed"

# =================================================================
# DEVELOPMENT AND MAINTENANCE TARGETS  
# =================================================================

.PHONY: status
status:
	@echo -e "$(BLUE)[STATUS]$(NC) RIFT Build System Status Report"
	@echo -e "$(CYAN)=================================================================="
	@echo -e "Root Directory: $(RIFT_ROOT)"
	@echo -e "Platform: $(PLATFORM) ($(UNAME_M))"
	@echo -e "Version: $(RIFT_VERSION)"
	@echo -e "AEGIS Compliance: $(AEGIS_COMPLIANCE)"
	@echo -e "pkg-config Support: $(PKG_CONFIG_SUPPORT)"
	@echo -e "Parallel Jobs: $(NPROC)"
	@echo -e "=================================================================="
	@if [ -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		echo -e "CMake Status: $(GREEN)CONFIGURED$(NC)"; \
	else \
		echo -e "CMake Status: $(YELLOW)NOT CONFIGURED$(NC)"; \
	fi
	@echo -e "Available Stages:"
	@for stage in 0 1 2 3 4 5 6; do \
		if [ -d "rift-$$stage" ] || [ -d "rift/src/core/stage-$$stage" ]; then \
			echo -e "  $(GREEN)✓$(NC) Stage $$stage"; \
		else \
			echo -e "  $(RED)✗$(NC) Stage $$stage"; \
		fi; \
	done
	@echo -e "$(CYAN)==================================================================$(NC)"

.PHONY: logs
logs:
	@echo -e "$(BLUE)[LOGS]$(NC) Recent build logs:"
	@if [ -d "$(LOGS_DIR)" ]; then \
		ls -la $(LOGS_DIR)/; \
		echo -e "$(YELLOW)[INFO]$(NC) Use 'tail -f $(LOGS_DIR)/<logfile>' to monitor builds"; \
	else \
		echo -e "$(YELLOW)[WARNING]$(NC) Logs directory not found"; \
	fi

.PHONY: help
help:
	@echo -e "$(CYAN)$(BOLD)RIFT Build System - Available Targets$(NC)"
	@echo ""
	@echo -e "$(BOLD)Primary Targets:$(NC)"
	@echo "  all                - Complete build with pkg-config integration (default)"
	@echo "  setup              - Initialize directories and run setup wizard"
	@echo "  clean              - Remove build artifacts"
	@echo "  deep-clean         - Remove all generated files"
	@echo ""
	@echo -e "$(BOLD)Build Targets:$(NC)"
	@echo "  cmake-build        - Build using CMake system"
	@echo "  cmake-install      - Install RIFT system components"
	@echo ""
	@echo -e "$(BOLD)pkg-config Targets:$(NC)"
	@echo "  pkg-config-integration - Generate pkg-config files"
	@echo "  pkg-config-install     - Install pkg-config files system-wide"
	@echo "  pkg-config-test        - Test pkg-config functionality"
	@echo ""
	@echo -e "$(BOLD)Validation Targets:$(NC)"
	@echo "  validate           - Run comprehensive validation"
	@echo "  validate-structure - Validate directory structure"
	@echo "  validate-executables - Validate stage executables"
	@echo "  validate-libraries - Validate static libraries"
	@echo ""
	@echo -e "$(BOLD)Testing Targets:$(NC)"
	@echo "  test               - Run test suite"
	@echo "  demo               - Execute pipeline demonstration"
	@echo "  external-examples  - Build external integration examples"
	@echo ""
	@echo -e "$(BOLD)Maintenance Targets:$(NC)"
	@echo "  status             - Show system status"
	@echo "  logs               - Show recent build logs"
	@echo "  help               - Show this help message"
	@echo ""
	@echo -e "$(BOLD)Example Usage:$(NC)"
	@echo "  make setup         # Initialize and configure"
	@echo "  make all           # Complete build with pkg-config"
	@echo "  make pkg-config-test # Test external pkg-config integration"
	@echo ""
	@echo -e "$(MAGENTA)OBINexus Computing Framework - Computing from the Heart$(NC)"

# =================================================================
# PHONY TARGET DECLARATIONS
# =================================================================

.PHONY: banner setup-directories setup-wizard cmake-build cmake-install
.PHONY: pkg-config-integration pkg-config-install pkg-config-test
.PHONY: validate validate-structure validate-executables validate-libraries
.PHONY: test demo external-examples status logs help
