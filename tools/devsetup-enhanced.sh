#!/bin/bash
#
# RIFT Development Environment Setup Script (Enhanced)
# ===================================================
#
# This script sets up a professional development environment for RIFT.
# Features:
#   - Parallel build support
#   - Multi-core compilation
#   - Progress tracking
#   - Colored output
#   - Pre-commit hooks
#   - Code formatting
#   - Static analysis
#   - CI/CD preparation
#
# Copyright © 2025 OBINexus Computing - Computing from the Heart
# Version: 2.0.0
# Date: May 28, 2026

set -euo pipefail  # Exit on error, undefined vars, pipe failure

# ============================================================================
# ANSI Color Definitions
# ============================================================================

readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[0;33m'
readonly BLUE='\033[0;34m'
readonly MAGENTA='\033[0;35m'
readonly CYAN='\033[0;36m'
readonly WHITE='\033[1;37m'
readonly NC='\033[0m'  # No Color

# ============================================================================
# Configuration Variables
# ============================================================================

SCRIPT_VERSION="2.0.0"
SCRIPT_NAME="$(basename "$0")"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Detect number of CPU cores
if [[ "$OSTYPE" == "darwin"* ]]; then
    NUM_CORES=$(sysctl -n hw.ncpu)
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    NUM_CORES=$(nproc)
else
    NUM_CORES=2
fi

# Build configuration
BUILD_TYPES=("debug" "release" "coverage")
CMAKE_VERSION_MIN="3.14"
GCC_VERSION_MIN="7"

# ============================================================================
# Logging Functions
# ============================================================================

log_banner() {
    echo -e "${CYAN}"
    echo "  ██████╗ ██╗███████╗████████╗"
    echo "  ██╔══██╗██║██╔════╝╚══██╔══╝"
    echo "  ██████╔╝██║█████╗     ██║   "
    echo "  ██╔══██╗██║██╔══╝     ██║   "
    echo "  ██║  ██║██║██║        ██║   "
    echo "  ╚═╝  ╚═╝╚═╝╚═╝        ╚═╝   "
    echo -e "${NC}"
    echo -e "${BLUE}RIFT Is a Flexible Translator${NC}"
    echo -e "${MAGENTA}Development Environment Setup (v${SCRIPT_VERSION})${NC}"
    echo -e "${MAGENTA}Enhanced Multi-Core Build System${NC}"
    echo -e "${MAGENTA}Copyright © 2025 OBINexus Computing${NC}"
    echo ""
}

section() {
    echo ""
    echo -e "${BLUE}╔════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║${NC} $1" | head -c 57
    echo -e "${BLUE}║${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════════════════════╝${NC}"
}

success() {
    echo -e "${GREEN}✓${NC} $1"
}

warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

error() {
    echo -e "${RED}✗${NC} $1"
}

info() {
    echo -e "${CYAN}ℹ${NC} $1"
}

progress() {
    echo -e "${MAGENTA}→${NC} $1"
}

command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# ============================================================================
# System Detection
# ============================================================================

detect_os() {
    section "Detecting Operating System"

    if [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
        info "macOS detected"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
        if [ -f /etc/debian_version ]; then
            DISTRO="debian"
            info "Debian/Ubuntu/Mint detected"
        elif [ -f /etc/redhat-release ]; then
            DISTRO="redhat"
            info "RedHat/CentOS/Fedora detected"
        elif [ -f /etc/arch-release ]; then
            DISTRO="arch"
            info "Arch Linux detected"
        else
            DISTRO="unknown"
            warning "Unknown Linux distribution"
        fi
    else
        OS="unknown"
        error "Unsupported operating system"
        exit 1
    fi

    info "Number of CPU cores detected: $NUM_CORES"
}

# ============================================================================
# Dependency Installation
# ============================================================================

install_dependencies() {
    section "Installing Dependencies"

    case "$OS" in
        linux)
            case "${DISTRO:-unknown}" in
                debian)
                    progress "Installing Debian/Ubuntu packages..."
                    sudo apt-get update
                    sudo apt-get install -y --no-install-recommends \
                        build-essential \
                        cmake \
                        git \
                        gcc \
                        g++ \
                        make \
                        pkg-config \
                        libpcre3-dev \
                        valgrind \
                        doxygen \
                        graphviz \
                        cppcheck \
                        clang-format \
                        clang-tidy \
                        lcov \
                        parallel
                    success "Debian packages installed"
                    ;;
                redhat)
                    progress "Installing RedHat/CentOS packages..."
                    if command_exists dnf; then
                        sudo dnf install -y \
                            gcc gcc-c++ cmake make git pkgconfig \
                            pcre-devel valgrind doxygen graphviz \
                            cppcheck clang-tools-extra lcov parallel
                    else
                        sudo yum install -y \
                            gcc gcc-c++ cmake make git pkgconfig \
                            pcre-devel valgrind doxygen graphviz \
                            cppcheck clang-tools-extra lcov parallel
                    fi
                    success "RedHat packages installed"
                    ;;
                arch)
                    progress "Installing Arch Linux packages..."
                    sudo pacman -Sy --noconfirm \
                        base-devel cmake git pcre valgrind doxygen \
                        graphviz cppcheck clang lcov parallel
                    success "Arch packages installed"
                    ;;
            esac
            ;;
        macos)
            progress "Installing macOS packages via Homebrew..."
            if ! command_exists brew; then
                info "Installing Homebrew..."
                /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
            fi

            brew update
            brew install cmake pcre doxygen graphviz cppcheck lcov gnu-parallel
            success "macOS packages installed"
            ;;
    esac
}

# ============================================================================
# Tool Verification
# ============================================================================

check_tools() {
    section "Checking Required Tools"

    local required_tools=("cmake" "gcc" "make" "git")
    local missing_tools=()

    for tool in "${required_tools[@]}"; do
        if command_exists "$tool"; then
            success "$tool found"
        else
            error "$tool not found"
            missing_tools+=("$tool")
        fi
    done

    if [ ${#missing_tools[@]} -ne 0 ]; then
        error "Missing tools: ${missing_tools[*]}"
        exit 1
    fi

    # Check CMake version
    CMAKE_VERSION=$(cmake --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1)
    CMAKE_MAJOR=$(echo "$CMAKE_VERSION" | cut -d. -f1)
    CMAKE_MINOR=$(echo "$CMAKE_VERSION" | cut -d. -f2)

    if [ "$CMAKE_MAJOR" -lt 3 ] || ([ "$CMAKE_MAJOR" -eq 3 ] && [ "$CMAKE_MINOR" -lt 14 ]); then
        warning "CMake version $CMAKE_VERSION detected (requires 3.14+)"
        exit 1
    else
        success "CMake version $CMAKE_VERSION meets requirements"
    fi

    # Check GCC version
    GCC_VERSION=$(gcc --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1)
    GCC_MAJOR=$(echo "$GCC_VERSION" | cut -d. -f1)

    if [ "$GCC_MAJOR" -lt 7 ]; then
        warning "GCC version $GCC_VERSION detected (recommends 7.0+)"
    else
        success "GCC version $GCC_VERSION meets requirements"
    fi
}

# ============================================================================
# Build Directory Setup
# ============================================================================

setup_build_dirs() {
    section "Setting Up Build Directories"

    cd "$PROJECT_ROOT"

    for build_type in "${BUILD_TYPES[@]}"; do
        progress "Creating build/$build_type..."
        mkdir -p "build/$build_type"
    done

    success "Build directories created"
}

# ============================================================================
# CMake Configuration with Parallel Support
# ============================================================================

configure_cmake_parallel() {
    section "Configuring CMake (Parallel)"

    cd "$PROJECT_ROOT"

    # Debug build
    progress "Configuring debug build ($(($NUM_CORES * 2)) jobs)..."
    (
        cd build/debug
        cmake ../.. \
            -DCMAKE_BUILD_TYPE=Debug \
            -DBUILD_TESTING=ON \
            -DCMAKE_BUILD_PARALLEL_LEVEL=$NUM_CORES \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    ) &
    DEBUG_PID=$!

    # Release build
    progress "Configuring release build ($(($NUM_CORES * 2)) jobs)..."
    (
        cd build/release
        cmake ../.. \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_BUILD_PARALLEL_LEVEL=$NUM_CORES \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    ) &
    RELEASE_PID=$!

    # Coverage build
    progress "Configuring coverage build..."
    (
        cd build/coverage
        cmake ../.. \
            -DCMAKE_BUILD_TYPE=Debug \
            -DBUILD_TESTING=ON \
            -DENABLE_COVERAGE=ON \
            -DCMAKE_BUILD_PARALLEL_LEVEL=$NUM_CORES
    ) &
    COVERAGE_PID=$!

    # Wait for all configurations
    wait $DEBUG_PID && success "Debug build configured"
    wait $RELEASE_PID && success "Release build configured"
    wait $COVERAGE_PID && success "Coverage build configured"
}

# ============================================================================
# Parallel Build Execution
# ============================================================================

build_parallel() {
    section "Building (Parallel, $NUM_CORES cores)"

    cd "$PROJECT_ROOT"

    progress "Building debug version..."
    (cd build/debug && make -j$NUM_CORES) &
    DEBUG_BUILD_PID=$!

    progress "Building release version..."
    (cd build/release && make -j$NUM_CORES) &
    RELEASE_BUILD_PID=$!

    # Wait for builds
    if wait $DEBUG_BUILD_PID; then
        success "Debug build completed"
    else
        error "Debug build failed"
        exit 1
    fi

    if wait $RELEASE_BUILD_PID; then
        success "Release build completed"
    else
        error "Release build failed"
        exit 1
    fi
}

# ============================================================================
# Git Hooks Setup
# ============================================================================

setup_git_hooks() {
    section "Setting Up Git Hooks"

    cd "$PROJECT_ROOT"

    if [ ! -d ".git" ]; then
        warning "Not a Git repository, skipping hooks"
        return
    fi

    progress "Creating pre-commit hook..."
    cat > ".git/hooks/pre-commit" << 'HOOK_EOF'
#!/bin/bash
files=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|h)$')
[ -z "$files" ] && exit 0

if command -v clang-format >/dev/null 2>&1; then
    for file in $files; do
        clang-format -i "$file"
        git add "$file"
    done
fi

if command -v cppcheck >/dev/null 2>&1; then
    cppcheck --quiet --error-exitcode=1 $files || exit 1
fi

exit 0
HOOK_EOF

    chmod +x ".git/hooks/pre-commit"
    success "Pre-commit hook installed"
}

# ============================================================================
# Code Formatting Setup
# ============================================================================

setup_formatting() {
    section "Setting Up Code Formatting"

    cd "$PROJECT_ROOT"

    progress "Creating .clang-format configuration..."
    cat > ".clang-format" << 'CLANG_FORMAT_EOF'
---
Language: Cpp
Standard: c11
BasedOnStyle: LLVM
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100
AllowShortFunctionsOnASingleLine: None
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
BreakBeforeBraces: Linux
PointerAlignment: Right
SortIncludes: true
IncludeBlocks: Regroup
IncludeCategories:
  - Regex:           '^<rift/.*\.h>'
    Priority:        2
  - Regex:           '^<.*\.h>'
    Priority:        1
  - Regex:           '.*'
    Priority:        3
...
CLANG_FORMAT_EOF

    success ".clang-format created"

    progress "Creating format.sh script..."
    mkdir -p tools
    cat > "tools/format.sh" << 'FORMAT_SCRIPT_EOF'
#!/bin/bash
# Format all C/C++ code in the project

echo "Formatting code..."
find include src -name "*.c" -o -name "*.h" | \
    parallel --pipe --max-lines 10 \
    'xargs -I {} clang-format -i {}'

echo "Formatting complete!"
FORMAT_SCRIPT_EOF

    chmod +x "tools/format.sh"
    success "format.sh script created"
}

# ============================================================================
# Environment Setup
# ============================================================================

setup_environment() {
    section "Setting Up Environment Variables"

    cd "$PROJECT_ROOT"

    case "$OS" in
        linux|macos)
            ENV_FILE="$HOME/.rift_env"

            progress "Creating environment file..."
            cat > "$ENV_FILE" << EOF
# RIFT environment variables
export RIFT_ROOT="$(pwd)"
export RIFT_BUILD_JOBS=$NUM_CORES
export PATH="\$RIFT_ROOT/build/debug/bin:\$RIFT_ROOT/build/release/bin:\$PATH"
export LD_LIBRARY_PATH="\$RIFT_ROOT/build/debug/lib:\$RIFT_ROOT/build/release/lib:\$LD_LIBRARY_PATH"
export CMAKE_BUILD_PARALLEL_LEVEL=$NUM_CORES
EOF

            success "Environment file created at $ENV_FILE"

            progress "Adding to shell profile..."
            if [[ "$OSTYPE" == "darwin"* ]] && [ -f "$HOME/.zshrc" ]; then
                SHELL_PROFILE="$HOME/.zshrc"
            elif [ -f "$HOME/.bashrc" ]; then
                SHELL_PROFILE="$HOME/.bashrc"
            elif [ -f "$HOME/.profile" ]; then
                SHELL_PROFILE="$HOME/.profile"
            fi

            if [ -n "$SHELL_PROFILE" ] && ! grep -q "source $ENV_FILE" "$SHELL_PROFILE"; then
                echo "source $ENV_FILE" >> "$SHELL_PROFILE"
                success "Environment setup added to $SHELL_PROFILE"
            fi
            ;;
    esac
}

# ============================================================================
# Testing and Verification
# ============================================================================

run_tests() {
    section "Running Tests"

    cd "$PROJECT_ROOT/build/debug"

    if [ -f "CTest" ]; then
        progress "Running CTest suite..."
        ctest --output-on-failure -j$NUM_CORES
        success "Tests completed"
    else
        info "No tests found"
    fi
}

# ============================================================================
# Main Execution
# ============================================================================

main() {
    log_banner

    detect_os
    install_dependencies
    check_tools
    setup_build_dirs
    configure_cmake_parallel
    build_parallel
    setup_git_hooks
    setup_formatting
    setup_environment
    run_tests

    section "Setup Complete!"
    success "RIFT development environment is ready!"
    info "CPU cores available: $NUM_CORES"
    info "Build directory: build/"
    info "Binaries location: build/debug/bin/ (debug), build/release/bin/ (release)"

    echo ""
    echo -e "${GREEN}Next steps:${NC}"
    echo "  1. Source environment: source ~/.rift_env"
    echo "  2. Start development: cd build/debug && make"
    echo "  3. Run tests: cd build/debug && ctest"
    echo "  4. Build release: cd build/release && make -j$NUM_CORES"
}

main "$@"
