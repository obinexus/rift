]#!/bin/bash

# LibRift Testing Environment Setup Script
# Version: 1.0
# Purpose: Automate testing environment configuration and dependency installation

# Color Codes
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Logging Functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Detect Operating System
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$OSTYPE" == "msys"* ]] || [[ "$OSTYPE" == "win"* ]]; then
        echo "windows"
    else
        echo "unknown"
    fi
}

# Install Dependencies for Linux
install_linux_deps() {
    log_info "Installing dependencies for Linux"
    sudo apt-get update
    sudo apt-get install -y \
        build-essential \
        cmake \
        check \
        valgrind \
        lcov \
        libsubunit-dev

    # Optional: Install gcovr for coverage reporting
    sudo pip3 install gcovr
}

# Install Dependencies for macOS
install_macos_deps() {
    log_info "Installing dependencies for macOS"
    if ! command -v brew &> /dev/null; then
        log_error "Homebrew not found. Please install Homebrew first."
        exit 1
    fi

    brew update
    brew install \
        cmake \
        check \
        valgrind \
        lcov

    # Optional: Install gcovr
    pip3 install gcovr
}

# Install Dependencies for Windows (using MSYS2/MinGW)
install_windows_deps() {
    log_info "Installing dependencies for Windows"
    if ! command -v pacman &> /dev/null; then
        log_error "MSYS2/MinGW not found. Please install MSYS2 first."
        exit 1
    fi

    pacman -Syu --noconfirm
    pacman -S --noconfirm \
        mingw-w64-x86_64-toolchain \
        mingw-w64-x86_64-cmake \
        mingw-w64-x86_64-check
}

# Create Basic CMake Configuration for Testing
create_cmake_testing_config() {
    local project_root=$(git rev-parse --show-toplevel 2>/dev/null || echo ".")
    local cmake_file="${project_root}/CMakeLists.txt"

    log_info "Generating CMake configuration for testing"
    
    cat > "$cmake_file" << EOL
cmake_minimum_required(VERSION 3.10)
project(LibRift C)

# C Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find Check Testing Framework
find_package(Check REQUIRED)

# Include Directories
include_directories(
    \${PROJECT_SOURCE_DIR}/include
    \${CHECK_INCLUDE_DIRS}
)

# Enable Testing
enable_testing()

# Add test subdirectory
add_subdirectory(tests)

# Optional: Coverage Configuration
if(CMAKE_BUILD_TYPE MATCHES Debug)
    include(CodeCoverage)
    setup_target_for_coverage_lcov(
        NAME coverage
        EXECUTABLE ctest
        DEPENDENCIES test_suite
    )
endif()
EOL
}

# Main Setup Function
setup_testing_environment() {
    local os=$(detect_os)

    log_info "Detected Operating System: $os"

    case "$os" in
        linux)
            install_linux_deps
            ;;
        macos)
            install_macos_deps
            ;;
        windows)
            install_windows_deps
            ;;
        *)
            log_error "Unsupported operating system"
            exit 1
            ;;
    esac

    # Create basic CMake configuration
    create_cmake_testing_config

    log_info "LibRift testing environment setup complete!"
}

# Help Function
usage() {
    echo "Usage: $0 [OPTIONS]"
    echo
    echo "Options:"
    echo "  -h, --help     Display this help message"
    echo
    echo "This script sets up the testing environment for LibRift."
}

# Parse Arguments
case "$1" in
    -h|--help)
        usage
        exit 0
        ;;
esac

# Execute Setup
setup_testing_environment
