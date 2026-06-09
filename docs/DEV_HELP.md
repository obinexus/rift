# Systematic Development Workflow for LibRift

## Immediate Build Fix

First, let's address the immediate header inclusion problem:

```bash
# Run from project root
cd /home/nnamdi/Projects/obinexuscomputing/pkg/dev/librift

# 1. Fix the immediate include path issues
tools/linter/fix_includes.sh

# 2. Run the header synchronization to ensure proper includes
tools/formatter/sync_headers.sh
```

The primary issue appears to be that source files are attempting to include headers with incorrect paths. Files in `src/core/automaton/` are trying to include `automaton/automaton.h` when they should be including `core/automaton/automaton.h`.

## Systematic Development Workflow

Here's a methodical approach for continuing development:

### 1. Project Setup Phase

```bash
# Configure development environment
tools/setup/devsetup.sh

# Generate configuration template
tools/setup/generate_config_template.sh

# Set up testing infrastructure
tools/setup/setup_testing.sh
```

### 2. Module Development Cycle

For each module you're working on:

```bash
# 1. Generate module structure if new
tools/setup/generate_module.sh <module_name> <module_path>
# Example: tools/setup/generate_module.sh state core/automaton

# 2. Edit the source files

# 3. Synchronize headers after editing
tools/formatter/sync_headers.sh

# 4. Run quality checks
tools/linter/check_quality.sh

# 5. Fix any include issues if needed
tools/linter/fix_includes.sh

# 6. Build to test changes
tools/build.sh Debug
```

### 3. Testing Phase

```bash
# Generate test modules for components
tools/setup/generate_test_modules.sh <module_name>

# Run integration tests
tools/benchmark_suite/intergration_test_runner.sh

# Run benchmarks if performance is a concern
tools/benchmark_suite/benchmark_runner.sh
```

## Project-Wide Maintenance

Periodically run these commands to maintain code quality:

```bash
# Code formatting
tools/formatter/format.sh

# Comprehensive quality check
tools/linter/check_quality.sh
```

## Specific Instructions for Automaton Module

Based on your error, you're working with the automaton module. Here's a specific sequence:

```bash
# 1. Fix immediate include paths
mkdir -p include/core/automaton
cp -n src/core/automaton/*.h include/core/automaton/ 2>/dev/null || true

# 2. Manually fix the include in automaton.c
sed -i 's|#include "automaton/automaton.h"|#include "core/automaton/automaton.h"|g' src/core/automaton/automaton.c
sed -i 's|#include "automaton/|#include "core/automaton/|g' src/core/automaton/*.c

# 3. Synchronize headers
tools/formatter/sync_headers.sh

# 4. Rebuild
tools/build.sh Debug
```
