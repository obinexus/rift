#!/bin/bash

# Systematic fix for include issues in LibRift project
# This improved script analyzes dependencies before modification

echo "Starting systematic include structure repair..."

# Create backup of source files
backup_dir="backup_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$backup_dir"
echo "Creating backups in $backup_dir"
find src include -name "*.c" -o -name "*.h" | xargs -I{} cp --parents {} "$backup_dir/"

# Phase 1: Fix duplicate core prefixes
echo "Phase 1: Fixing duplicate core prefixes..."
find src include -name "*.c" -o -name "*.h" | xargs sed -i 's|#include "core/core/|#include "core/|g'

# Phase 2: Fix incorrect include paths based on module structure
echo "Phase 2: Fixing incorrect module paths..."
find src include -name "*.c" -o -name "*.h" | while read -r file; do
    # Add core/ prefix to module includes if they don't have it
    for module in automaton engine errors regex parser; do
        sed -i "s|#include \"$module/|#include \"core/$module/|g" "$file"
    done
done

# Phase 3: Analyze and fix header include ordering
echo "Phase 3: Fixing include ordering..."
find src -name "*.c" | while read -r src_file; do
    base_name=$(basename "$src_file" .c)
    header_path=$(find include -name "${base_name}.h")
    
    if [ -n "$header_path" ]; then
        # 1. Make corresponding header the first include
        first_include=$(grep -n "#include" "$src_file" | head -1 | cut -d: -f1)
        
        if [ -n "$first_include" ]; then
            # Extract current includes
            includes=$(grep "#include" "$src_file")
            # Remove all includes
            sed -i '/^#include/d' "$src_file"
            
            # Build proper include list with corresponding header first
            header_include="#include \"$(echo "$header_path" | sed 's|include/||')\""
            echo "$header_include" > temp_includes.txt
            
            # System includes second
            echo "$includes" | grep "#include <" | sort >> temp_includes.txt
            
            # Other local includes third
            echo "$includes" | grep "#include \"" | grep -v "$header_include" | sort >> temp_includes.txt
            
            # Insert organized includes back at the original position
            sed -i "${first_include}r temp_includes.txt" "$src_file"
            rm temp_includes.txt
        fi
    fi
done

# Phase 4: Remove duplicate includes
echo "Phase 4: Removing duplicate includes..."
find src include -name "*.c" -o -name "*.h" | while read -r file; do
    if [ ! -f "$file" ]; then
        echo "Warning: File $file not found, skipping..."
        continue
    fi
    
    # Create temporary file with unique includes, maintaining order of first occurrence
    awk '!seen[$0]++' <(grep "^#include" "$file") > temp_includes.txt
    
    # Create a backup of the file
    cp "$file" "${file}.bak"
    
    # Remove all includes from file
    sed -i '/^#include/d' "$file"
    
    # Get line number where includes should start (after any comments and defines)
    insert_line=$(grep -n "^[[:space:]]*$" "$file" | head -1 | cut -d: -f1)
    if [ -z "$insert_line" ]; then
        insert_line=1
    fi
    
    # Insert unique includes at the right position
    sed -i "${insert_line}r temp_includes.txt" "$file"
    rm -f temp_includes.txt
done

# Phase 5: Analyze potential circular dependencies
echo "Phase 5: Analyzing potential circular dependencies..."
for module in automaton engine errors regex parser; do
    module_files=$(find include -path "*/core/$module/*.h")
    echo "Module: $module"
    
    for file in $module_files; do
        file_includes=$(grep "#include \"core/$module/" "$file" | sort)
        if [ -n "$file_includes" ]; then
            echo "  Potential circular dependency in $file:"
            echo "$file_includes" | sed 's/^/    /'
        fi
    done
done


echo "Fix complete. Please review changes and test compilation."