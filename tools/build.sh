#!/bin/bash
# LibRift Build Script
# This script handles the build process for LibRift

set -e  # Exit on any error

# Define colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default configuration
BUILD_TYPE="Debug"
BUILD_TESTS=true
BUILD_DIR="build"
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)
CLEAN=false
INSTALL=false
INSTALL_PREFIX="/usr/local"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -t|--tests)
            BUILD_TESTS=true
            shift
            ;;
        --no-tests)
            BUILD_TESTS=false
            shift
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -i|--install)
            INSTALL=true
            shift
            ;;
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        -b|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -h|--help)
            echo "LibRift Build Script"
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  -r, --release         Build in Release mode"
            echo "  -d, --debug           Build in Debug mode (default)"
            echo "  -t, --tests           Build tests (default)"
            echo "  --no-tests            Don't build tests"
            echo "  -j, --jobs N          Set parallel build jobs (default: auto)"
            echo "  -c, --clean           Clean build directory before building"
            echo "  -i, --install         Install after building"
            echo "  --prefix DIR          Installation prefix (default: /usr/local)"
            echo "  -b, --build-dir DIR   Set build directory (default: build)"
            echo "  -h, --help            Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $key${NC}"
            exit 1
            ;;
    esac
done

# Set up build directory
if [ "$CLEAN" = true ] && [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Cleaning build directory: $BUILD_DIR${NC}"
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"

# Make sure the cmake modules directory exists
mkdir -p cmake/modules

# Check for check library
if [ "$BUILD_TESTS" = true ]; then
    if ! pkg-config --exists check 2>/dev/null; then
        echo -e "${YELLOW}Warning: Check unit testing framework not found.${NC}"
        echo -e "${YELLOW}Tests might not compile. Install Check with:${NC}"
        echo -e "${YELLOW}  Debian/Ubuntu: sudo apt-get install check${NC}"
        echo -e "${YELLOW}  Fedora: sudo dnf install check-devel${NC}"
        echo -e "${YELLOW}  macOS: brew install check${NC}"
    fi
fi

# Configure build with CMake
cd "$BUILD_DIR"
echo -e "${GREEN}Configuring build (type: $BUILD_TYPE)...${NC}"
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DLIBRIFT_BUILD_TESTS="$BUILD_TESTS" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"

# Build
echo -e "${GREEN}Building LibRift with $JOBS jobs...${NC}"
cmake --build . -- -j "$JOBS"

# Run tests if requested
if [ "$BUILD_TESTS" = true ]; then
    echo -e "${GREEN}Running tests...${NC}"
    ctest --output-on-failure
fi

# Install if requested
if [ "$INSTALL" = true ]; then
    echo -e "${GREEN}Installing to $INSTALL_PREFIX...${NC}"
    cmake --build . --target install
    echo -e "${GREEN}Installation complete!${NC}"
fi

echo -e "${GREEN}Build completed successfully!${NC}"