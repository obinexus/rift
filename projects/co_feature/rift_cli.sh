#!/bin/bash
# LibRift Command-Line Client Usage Demo
# This script demonstrates how to use the 'rift' command-line tool
# for various regular expression operations.

echo "===================================================="
echo "  LibRift Command-Line Client Demonstration"
echo "===================================================="
echo

# Check if the rift command is available
if ! command -v rift &> /dev/null; then
    echo "Error: The 'rift' command is not found."
    echo "Please ensure LibRift is properly installed."
    exit 1
fi

echo "1. Basic pattern matching"
echo "----------------------------------------------------"
echo "$ rift match 'function\\s+([a-zA-Z][a-zA-Z0-9_]*)' 'function test() {}'"
rift match 'function\s+([a-zA-Z][a-zA-Z0-9_]*)' 'function test() {}'

echo
echo "2. Using the R'' syntax for cleaner patterns"
echo "----------------------------------------------------"
echo "$ rift match -r 'function\s+([a-zA-Z][a-zA-Z0-9_]*)' 'function test() {}'"
rift match -r 'function\s+([a-zA-Z][a-zA-Z0-9_]*)' 'function test() {}'

echo
echo "3. Compiling a pattern for reuse"
echo "----------------------------------------------------"
echo "$ rift compile -r 'function\s+([a-zA-Z][a-zA-Z0-9_]*)' -o function_pattern.rift"
rift compile -r 'function\s+([a-zA-Z][a-zA-Z0-9_]*)' -o function_pattern.rift
echo "Pattern compiled and saved to function_pattern.rift"

echo
echo "4. Using a compiled pattern"
echo "----------------------------------------------------"
echo "$ rift match -p function_pattern.rift 'function test() {}'"
rift match -p function_pattern.rift 'function test() {}'

echo
echo "5. Extracting capture groups"
echo "----------------------------------------------------"
echo "$ rift extract -r 'function\s+([a-zA-Z][a-zA-Z0-9_]*)' 'function test() {}'"
rift extract -r 'function\s+([a-zA-Z][a-zA-Z0-9_]*)' 'function test() {}'

echo
echo "6. Validating a pattern"
echo "----------------------------------------------------"
echo "$ rift validate -r 'function\s+([a-zA-Z][a-zA-Z0-9_]*)'"
rift validate -r 'function\s+([a-zA-Z][a-zA-Z0-9_]*)'

echo
echo "7. Pattern complexity analysis"
echo "----------------------------------------------------"
echo "$ rift analyze -r '(?:co\s+)?function\s+([a-zA-Z][a-zA-Z0-9_]*)\s*\(\s*\)\s*\{\s*\}'"
rift analyze -r '(?:co\s+)?function\s+([a-zA-Z][a-zA-Z0-9_]*)\s*\(\s*\)\s*\{\s*\}'

echo
echo "8. Using multiple patterns in a script"
echo "----------------------------------------------------"
cat << 'EOF' > multi_pattern.rift
# LibRift Script
# Define patterns
PATTERN func_pattern = r'function\s+([a-zA-Z][a-zA-Z0-9_]*)'
PATTERN co_pattern = r'co\s+function\s+([a-zA-Z][a-zA-Z0-9_]*)'

# Apply patterns
MATCH func_pattern "function regularFunc() {}"
MATCH co_pattern "co function coFunc() {}"

# Extract information
EXTRACT func_pattern "function extractThis() {}" GROUP 1
EOF

echo "$ rift run multi_pattern.rift"
rift run multi_pattern.rift

echo
echo "9. Introducing a new language feature"
echo "----------------------------------------------------"
cat << 'EOF' > language_extension.rift
# LibRift Language Extension Script
# Original function pattern
PATTERN orig_func = r'function\s+([a-zA-Z][a-zA-Z0-9_]*)\s*\(\s*\)\s*\{\s*\}'

# Extended pattern with 'co' modifier
PATTERN extended_func = r'(?:co\s+)?function\s+([a-zA-Z][a-zA-Z0-9_]*)\s*\(\s*\)\s*\{\s*\}'

# Check if input is a co-function
PATTERN is_co = r'co\s+function'

# Test with regular function
SET input1 = "function test1() {}"
MATCH extended_func input1
IF MATCH is_co input1
  PRINT "Co-Function: " + GROUP 1
ELSE
  PRINT "Function: " + GROUP 1
ENDIF

# Test with co-function
SET input2 = "co function test2() {}"
MATCH extended_func input2
IF MATCH is_co input2
  PRINT "Co-Function: " + GROUP 1
ELSE
  PRINT "Function: " + GROUP 1
ENDIF
EOF

echo "$ rift run language_extension.rift"
rift run language_extension.rift

echo
echo "10. Visualizing the automaton (if visualization is enabled)"
echo "----------------------------------------------------"
echo "$ rift visualize -r '(?:co\s+)?function\s+([a-zA-Z][a-zA-Z0-9_]*)' -o function_automaton.svg"
rift visualize -r '(?:co\s+)?function\s+([a-zA-Z][a-zA-Z0-9_]*)' -o function_automaton.svg
echo "Automaton visualization saved to function_automaton.svg"

# Clean up temporary files
rm -f function_pattern.rift multi_pattern.rift language_extension.rift function_automaton.svg

echo
echo "===================================================="
echo "  LibRift Command-Line Demo Complete"
echo "===================================================="