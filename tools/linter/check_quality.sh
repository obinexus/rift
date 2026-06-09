#!/bin/bash

# Comprehensive quality check

echo "Running code quality checks..."

# Check for files without copyright notices
echo "Checking for missing copyright notices..."
find src include -name "*.c" -o -name "*.h" | xargs grep -L "Copyright.*LibRift"

# Check for TODO comments
echo -e "\nChecking for TODO comments..."
find src include -name "*.c" -o -name "*.h" | xargs grep -n "TODO"

# Check for consistent header guards
echo -e "\nChecking header guards..."
find include -name "*.h" | while read -r header; do
    base_name=$(basename "$header" .h)
    path_part=$(dirname "$header" | sed 's|^include/||')
    expected_guard=$(echo "${path_part}_${base_name}_h" | tr '/' '_' | tr '[:lower:]' '[:upper:]')
    
    if ! grep -q "#ifndef ${expected_guard}" "$header"; then
        echo "Inconsistent guard in $header"
    fi
done

# Check for proper include ordering
echo -e "\nChecking include ordering..."
find src -name "*.c" | while read -r src_file; do
    corresponding_header=$(basename "$src_file" .c)
    if grep -n "#include" "$src_file" | grep -A 10 "\"$corresponding_header.h\"" | \
       grep -q -n "#include <"; then
        echo "Incorrect include ordering in $src_file (system headers should come before local headers)"
    fi
done

echo -e "\nQuality check complete."