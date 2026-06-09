# Portable GNU Make entry point for RIFT.
#
# CMake owns the actual build graph. This wrapper always configures an
# out-of-source build so generated files never overwrite repository files.

CMAKE ?= cmake
CMAKE_CTEST_COMMAND ?= ctest
CONFIG ?= Release
PREFIX ?= build/install

ifeq ($(OS),Windows_NT)
BUILD_DIR ?= build/mingw
GENERATOR ?= MinGW Makefiles
else
BUILD_DIR ?= build/make
GENERATOR ?=
endif

ifneq ($(strip $(GENERATOR)),)
GENERATOR_ARG := -G "$(GENERATOR)"
endif

CONFIGURE_ARGS := -S . -B "$(BUILD_DIR)" $(GENERATOR_ARG) \
	-DCMAKE_BUILD_TYPE="$(CONFIG)" \
	-DRIFT_BUILD_TESTS=ON
BUILD_ARGS := --build "$(BUILD_DIR)" --config "$(CONFIG)" --parallel

.DEFAULT_GOAL := all

.PHONY: help configure all \
	rift rift-shared riftlang riftlang-shared rifttest rifttest-shared \
	rift-cli riftlang-cli test install clean

help:
	@echo "RIFT portable build targets:"
	@echo "  make                 Configure and build all libraries, CLIs, and tests"
	@echo "  make configure       Configure the out-of-source CMake build"
	@echo "  make rift            Build the static RIFT core library"
	@echo "  make rift-shared     Build the shared RIFT core library"
	@echo "  make riftlang        Build the static RiftLang library"
	@echo "  make riftlang-shared Build the shared RiftLang library"
	@echo "  make rifttest        Build the static RiftTest library"
	@echo "  make rifttest-shared Build the shared RiftTest library"
	@echo "  make rift-cli        Build the rift command-line tool"
	@echo "  make riftlang-cli    Build the riftlang command-line tool"
	@echo "  make test            Build and run the test suite"
	@echo "  make install         Install beneath PREFIX (default: build/install)"
	@echo "  make clean           Clean only the selected BUILD_DIR"
	@echo ""
	@echo "Overrides: CMAKE, CMAKE_CTEST_COMMAND, BUILD_DIR, GENERATOR, CONFIG, PREFIX"

configure:
	"$(CMAKE)" $(CONFIGURE_ARGS)

all: configure
	"$(CMAKE)" $(BUILD_ARGS)

rift: configure
	"$(CMAKE)" $(BUILD_ARGS) --target rift

rift-shared: configure
	"$(CMAKE)" $(BUILD_ARGS) --target rift_shared

riftlang: configure
	"$(CMAKE)" $(BUILD_ARGS) --target riftlang

riftlang-shared: configure
	"$(CMAKE)" $(BUILD_ARGS) --target riftlang_shared

rifttest: configure
	"$(CMAKE)" $(BUILD_ARGS) --target rifttest

rifttest-shared: configure
	"$(CMAKE)" $(BUILD_ARGS) --target rifttest_shared

rift-cli: configure
	"$(CMAKE)" $(BUILD_ARGS) --target rift_cli

riftlang-cli: configure
	"$(CMAKE)" $(BUILD_ARGS) --target riftlang_cli

test: all
	"$(CMAKE_CTEST_COMMAND)" --test-dir "$(BUILD_DIR)" --build-config "$(CONFIG)" --output-on-failure

install: all
	"$(CMAKE)" --install "$(BUILD_DIR)" --config "$(CONFIG)" --prefix "$(PREFIX)"

clean:
	"$(CMAKE)" --build "$(BUILD_DIR)" --config "$(CONFIG)" --target clean
