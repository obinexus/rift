#!/bin/bash

# =============================================================================
# R Extension Tree Structure Validation Script
# OBINexus RIFT Governance-First Architecture
# 
# Ensures R.extend uses pointer/reference for new async features
# Validates tree structure for UML relationship extensions
# Prevents codebase cluttering and enforces governance
# =============================================================================

set -euo pipefail

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="${SCRIPT_DIR}/.."
LOG_FILE="${PROJECT_ROOT}/logs/r_validation.log"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging function
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "${LOG_FILE}"
}

error() {
    echo -e "${RED}ERROR: $1${NC}" | tee -a "${LOG_FILE}"
}

success() {
    echo -e "${GREEN}SUCCESS: $1${NC}" | tee -a "${LOG_FILE}"
}

warning() {
    echo -e "${YELLOW}WARNING: $1${NC}" | tee -a "${LOG_FILE}"
}

info() {
    echo -e "${BLUE}INFO: $1${NC}" | tee -a "${LOG_FILE}"
}

# =============================================================================
# Tree Structure Validation Functions
# =============================================================================

validate_r_extension_structure() {
    log "Validating R extension tree structure..."
    
    local errors=0
    
    # Check for R.extend pointer/reference usage
    info "Checking R.extend pointer usage patterns..."
    
    # Find all R.extend calls and validate they use pointers
    while IFS= read -r -d '' file; do
        if grep -l "R_extend\|R\.extend" "$file" >/dev/null 2>&1; then
            info "Analyzing R.extend usage in: $file"
            
            # Check for proper pointer usage in R.extend
            if grep -n "R_extend.*\*" "$file" >/dev/null 2>&1; then
                success "✓ $file uses proper pointer references in R.extend"
            else
                warning "⚠ $file may not use proper pointer references in R.extend"
                grep -n "R_extend\|R\.extend" "$file" | while read -r line; do
                    warning "  Line: $line"
                done
                ((errors++))
            fi
            
            # Check for async feature patterns
            if grep -n "async\|await\|yield\|co_" "$file" >/dev/null 2>&1; then
                info "  Found async patterns - validating governance compliance..."
                if grep -n "governance_validated.*true\|\.governance_validated" "$file" >/dev/null 2>&1; then
                    success "  ✓ Async features have governance validation"
                else
                    error "  ✗ Async features missing governance validation"
                    ((errors++))
                fi
            fi
        fi
    done < <(find "${PROJECT_ROOT}/src" -name "*.c" -type f -print0)
    
    return $errors
}

validate_uml_relationship_patterns() {
    log "Validating UML relationship pattern implementations..."
    
    local errors=0
    
    # Required UML relationship types
    local uml_types=("composition" "association" "aggregation" "inheritance")
    
    for uml_type in "${uml_types[@]}"; do
        info "Checking for $uml_type pattern implementation..."
        
        # Look for pattern definitions
        if grep -r "R_${uml_type^^}_PATTERN\|R_${uml_type}" "${PROJECT_ROOT}/src" >/dev/null 2>&1; then
            success "✓ Found $uml_type pattern implementation"
            
            # Validate pattern syntax uses R"" or R''
            if grep -r "R\".*${uml_type}\|R'.*${uml_type}" "${PROJECT_ROOT}/src" >/dev/null 2>&1; then
                success "  ✓ Uses proper R\"\" or R'' syntax"
            else
                warning "  ⚠ May not use R\"\" or R'' syntax properly"
                ((errors++))
            fi
        else
            error "✗ Missing $uml_type pattern implementation"
            ((errors++))
        fi
    done
    
    return $errors
}

validate_boolean_logic_macros() {
    log "Validating Boolean logic macro implementations..."
    
    local errors=0
    local required_macros=("R_AND" "R_OR" "R_XOR" "R_NOT" "R_NAND" "R_NOR")
    
    for macro in "${required_macros[@]}"; do
        info "Checking for $macro macro..."
        
        if grep -r "#define $macro" "${PROJECT_ROOT}/src" "${PROJECT_ROOT}/include" >/dev/null 2>&1; then
            success "✓ Found $macro macro definition"
            
            # Validate macro implementation
            local definition=$(grep -r "#define $macro" "${PROJECT_ROOT}/src" "${PROJECT_ROOT}/include" | head -1)
            if [[ "$definition" =~ \(\(.*\).*\(.*\)\) ]]; then
                success "  ✓ Proper macro parameter structure"
            else
                warning "  ⚠ Check macro parameter structure: $definition"
            fi
        else
            error "✗ Missing $macro macro definition"
            ((errors++))
        fi
    done
    
    return $errors
}

validate_functional_composition() {
    log "Validating functional composition implementations..."
    
    local errors=0
    
    # Check for R.compose implementation
    info "Checking R.compose implementation..."
    if grep -r "R_compose\|R\.compose" "${PROJECT_ROOT}/src" >/dev/null 2>&1; then
        success "✓ Found R.compose implementation"
        
        # Check for lambda-like function patterns
        if grep -r "lambda\|->.*(" "${PROJECT_ROOT}/src" >/dev/null 2>&1; then
            success "  ✓ Found lambda-like function patterns"
        else
            warning "  ⚠ Consider adding lambda-like function patterns for better composition"
        fi
        
        # Check for data-agnostic patterns (void* usage)
        if grep -r "void\s*\*.*data\|void\s*\*.*input" "${PROJECT_ROOT}/src" >/dev/null 2>&1; then
            success "  ✓ Found data-agnostic patterns"
        else
            warning "  ⚠ Consider adding data-agnostic function signatures"
        fi
    else
        error "✗ Missing R.compose implementation"
        ((errors++))
    fi
    
    return $errors
}

validate_governance_compliance() {
    log "Validating governance compliance patterns..."
    
    local errors=0
    
    # Check for governance validation patterns
    info "Checking governance validation patterns..."
    if grep -r "governance_validated\|\.gov\|GOVERNANCE" "${PROJECT_ROOT}/src" >/dev/null 2>&1; then
        success "✓ Found governance validation patterns"
        
        # Count governance checks
        local gov_checks=$(grep -r "governance_validated" "${PROJECT_ROOT}/src" | wc -l)
        info "  Found $gov_checks governance validation checks"
        
        if [ "$gov_checks" -lt 3 ]; then
            warning "  ⚠ Consider adding more governance validation checks"
        fi
    else
        error "✗ Missing governance validation patterns"
        ((errors++))
    fi
    
    return $errors
}

# =============================================================================
# Codebase Structure Analysis
# =============================================================================

analyze_codebase_structure() {
    log "Analyzing codebase structure for cluttering issues..."
    
    info "Core vs CLI module separation analysis:"
    
    # Check src/core directory
    if [ -d "${PROJECT_ROOT}/src/core" ]; then
        local core_files=$(find "${PROJECT_ROOT}/src/core" -name "*.c" | wc -l)
        info "  Core module files: $core_files"
        
        # Check for CLI dependencies in core
        if grep -r "cli\|command\|main.*argc" "${PROJECT_ROOT}/src/core" >/dev/null 2>&1; then
            warning "  ⚠ Core module may have CLI dependencies"
            grep -r "cli\|command" "${PROJECT_ROOT}/src/core" | head -5
        else
            success "  ✓ Core module appears clean of CLI dependencies"
        fi
    else
        error "  ✗ Missing src/core directory structure"
    fi
    
    # Check src/cli directory
    if [ -d "${PROJECT_ROOT}/src/cli" ]; then
        local cli_files=$(find "${PROJECT_ROOT}/src/cli" -name "*.c" | wc -l)
        info "  CLI module files: $cli_files"
        
        # Check for core logic in CLI
        if grep -r "rift_regex\|automaton" "${PROJECT_ROOT}/src/cli" >/dev/null 2>&1; then
            warning "  ⚠ CLI module may contain core logic"
            grep -r "rift_regex\|automaton" "${PROJECT_ROOT}/src/cli" | head -5
        else
            success "  ✓ CLI module appears properly separated"
        fi
    else
        error "  ✗ Missing src/cli directory structure"
    fi
}

# =============================================================================
# Suggested Refactoring Actions
# =============================================================================

suggest_refactoring_actions() {
    log "Generating refactoring suggestions..."
    
    cat << EOF > "${PROJECT_ROOT}/REFACTORING_SUGGESTIONS.md"
# OBINexus RIFT Codebase Refactoring Suggestions

Generated on: $(date)

## Core/CLI Separation Issues

### Recommended Actions:

1. **Extract Core Library**
   - Move all \`rift_regex_*\` functions to \`src/core/\`
   - Move all automaton logic to \`src/core/automaton/\`
   - Create clean API boundaries in \`include/rift/core/\`

2. **Isolate CLI Components**
   - Move command parsing to \`src/cli/commands/\`
   - Move main() functions to \`src/cli/main/\`
   - Remove core dependencies from CLI modules

3. **R Extension Organization**
   - Create \`src/core/r_extensions/\` directory
   - Separate UML patterns: \`src/core/r_extensions/uml/\`
   - Separate Boolean logic: \`src/core/r_extensions/logic/\`
   - Separate composition: \`src/core/r_extensions/compose/\`

## File Reorganization Plan

\`\`\`
src/
├── core/
│   ├── automaton/          # Core automaton engine
│   ├── regex/              # Regex pattern engine  
│   ├── r_extensions/       # R syntax extensions
│   │   ├── uml/           # UML relationship patterns
│   │   ├── logic/         # Boolean logic macros
│   │   └── compose/       # Functional composition
│   └── governance/         # Governance validation
├── cli/
│   ├── commands/          # CLI command handlers
│   ├── main/              # CLI entry points
│   └── interface/         # User interface logic
└── shared/
    ├── config/            # Shared configuration
    └── utils/             # Shared utilities
\`\`\`

## Implementation Priority

1. **High Priority**: Separate core regex engine from CLI
2. **Medium Priority**: Organize R extensions into modules
3. **Low Priority**: Optimize governance validation patterns

## Validation Commands

After refactoring, run:
\`\`\`bash
./scripts/validate_r_extensions.sh
make test-governance
make test-r-extensions
\`\`\`

EOF

    success "Refactoring suggestions generated: ${PROJECT_ROOT}/REFACTORING_SUGGESTIONS.md"
}

# =============================================================================
# Main Execution
# =============================================================================

main() {
    log "Starting R Extension Tree Structure Validation"
    
    # Create logs directory if it doesn't exist
    mkdir -p "${PROJECT_ROOT}/logs"
    
    local total_errors=0
    
    echo -e "${BLUE}===============================================${NC}"
    echo -e "${BLUE}  OBINexus R Extension Validation${NC}"
    echo -e "${BLUE}===============================================${NC}"
    
    # Run all validations
    validate_r_extension_structure
    total_errors=$((total_errors + $?))
    
    validate_uml_relationship_patterns  
    total_errors=$((total_errors + $?))
    
    validate_boolean_logic_macros
    total_errors=$((total_errors + $?))
    
    validate_functional_composition
    total_errors=$((total_errors + $?))
    
    validate_governance_compliance
    total_errors=$((total_errors + $?))
    
    analyze_codebase_structure
    
    suggest_refactoring_actions
    
    echo -e "${BLUE}===============================================${NC}"
    
    if [ $total_errors -eq 0 ]; then
        success "✓ All R extension validations passed!"
        echo -e "${GREEN}Tree structure is governance-compliant${NC}"
        exit 0
    else
        error "✗ Found $total_errors validation issues"
        echo -e "${RED}Review logs and refactoring suggestions${NC}"
        exit 1
    fi
}

# Run with error handling
trap 'error "Script failed at line $LINENO"' ERR

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
