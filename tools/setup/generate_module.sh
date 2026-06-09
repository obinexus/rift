#!/bin/bash

# Generate a new module with source and header files

if [ $# -lt 1 ]; then
    echo "Usage: $0 <module_name> [module_path]"
    echo "Example: $0 tokenizer core/parser"
    exit 1
fi

MODULE_NAME="$1"
MODULE_PATH="${2:-}"

SRC_DIR="src"
INCLUDE_DIR="include"

# Create directories
mkdir -p "$SRC_DIR/$MODULE_PATH"
mkdir -p "$INCLUDE_DIR/$MODULE_PATH"

# Create header file
HEADER_PATH="$INCLUDE_DIR/$MODULE_PATH/$MODULE_NAME.h"
GUARD_NAME=$(echo "${MODULE_PATH}_${MODULE_NAME}_h" | tr '/' '_' | tr '[:lower:]' '[:upper:]')

cat > "$HEADER_PATH" << EOL
/**
 * @file ${MODULE_NAME}.h
 * @brief Header file for the ${MODULE_NAME} module
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#ifndef ${GUARD_NAME}
#define ${GUARD_NAME}

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations and function prototypes */

#ifdef __cplusplus
}
#endif

#endif /* ${GUARD_NAME} */
EOL

# Create source file
SRC_PATH="$SRC_DIR/$MODULE_PATH/$MODULE_NAME.c"
INCLUDE_PATH=$([ -z "$MODULE_PATH" ] && echo "$MODULE_NAME.h" || echo "$MODULE_PATH/$MODULE_NAME.h")

cat > "$SRC_PATH" << EOL
/**
 * @file ${MODULE_NAME}.c
 * @brief Implementation of the ${MODULE_NAME} module
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "$INCLUDE_PATH"

/* Implementation */

EOL

echo "Created module: $MODULE_NAME"
echo "Source: $SRC_PATH"
echo "Header: $HEADER_PATH"