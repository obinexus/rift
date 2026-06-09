#!/bin/bash

# Root directories
INCLUDE_DIR="include"
SRC_DIR="src"

# Function to create header file if it doesn't exist
create_header_if_missing() {
    local c_file="$1"
    local module_name=$(basename "$c_file" .c)
    local module_path=$(dirname "$c_file" | sed "s|^$SRC_DIR/||")
    local header_path="$INCLUDE_DIR/$module_path/$module_name.h"
    local header_dir=$(dirname "$header_path")
    
    # Create header directory if it doesn't exist
    if [ ! -d "$header_dir" ]; then
        mkdir -p "$header_dir"
        echo "Created directory: $header_dir"
    fi
    
    # Create header file if it doesn't exist
    if [ ! -f "$header_path" ]; then
        local guard_name=$(echo "${module_path}_${module_name}_h" | tr '/' '_' | tr '[:lower:]' '[:upper:]')
        
        echo "Creating missing header: $header_path"
        cat > "$header_path" << EOL
/**
 * @file ${module_name}.h
 * @brief Header file for the ${module_name} module
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#ifndef ${guard_name}
#define ${guard_name}

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

#endif /* ${guard_name} */
EOL
    fi
    
    # Return the relative path to the header from include dir
    echo "$module_path/$module_name.h"
}

# Process all C files
find "$SRC_DIR" -type f -name "*.c" | while read -r c_file; do
    echo "Processing $c_file"
    
    # Extract module name and path
    module_name=$(basename "$c_file" .c)
    module_path=$(dirname "$c_file" | sed "s|^$SRC_DIR/||")
    
    # Check if header exists, create if missing
    header_relative_path=$(create_header_if_missing "$c_file")
    
    # Extract existing includes
    includes=$(grep -o '#include\s\+"[^"]\+"' "$c_file" | sort | uniq)
    
    # Check if the corresponding header is already included
    header_include="#include \"$header_relative_path\""
    if ! echo "$includes" | grep -q "$header_relative_path"; then
        # Add the include at the top of the file
        # Create a temporary file with the new include
        temp_file=$(mktemp)
        echo "$header_include" > "$temp_file"
        cat "$c_file" >> "$temp_file"
        mv "$temp_file" "$c_file"
        echo "Added include: $header_include to $c_file"
    fi
    
    # Check for other dependencies by looking at functions used
    # This is a basic approach and may need refinement
    dependencies=$(grep -o '[a-zA-Z_][a-zA-Z0-9_]*(' "$c_file" | tr -d '(' | sort | uniq)
    
    for func in $dependencies; do
        # Skip functions defined in this file
        if grep -q "^[a-zA-Z_][a-zA-Z0-9_]* $func" "$c_file" || \
           grep -q "^[a-zA-Z_][a-zA-Z0-9_]*\s\+$func" "$c_file"; then
            continue
        fi
        
        # Look for the function in other header files
        potential_header=$(find "$INCLUDE_DIR" -type f -name "*.h" -exec grep -l "$func" {} \; | head -1)
        
        if [ -n "$potential_header" ]; then
            header_include_path=$(echo "$potential_header" | sed "s|^$INCLUDE_DIR/||")
            if ! grep -q "#include \"$header_include_path\"" "$c_file"; then
                # Add the include after the main header
                sed -i "/#include \"$header_relative_path\"/a #include \"$header_include_path\"" "$c_file"
                echo "Added dependency: #include \"$header_include_path\" to $c_file"
            fi
        fi
    done
done

echo "Header synchronization complete."