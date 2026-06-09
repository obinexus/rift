#include <stdio.h>
#include <string.h>

#include "core/regex/runtime/pattern/pattern_complexity_strategy.h"
#include "ctest.h"

// Test basic complexity calculation for simple patterns
CTEST(pattern_complexity, simple_patterns)
{
    const char *simple_pattern = "abc";
    int complexity = calculate_pattern_complexity(simple_pattern);
    ASSERT_EQUAL(3, complexity); // Assuming complexity is based on length
}

// Test complexity for patterns with special characters
CTEST(pattern_complexity, special_chars)
{
    const char *pattern_with_special = "a[bc]d+";
    int complexity = calculate_pattern_complexity(pattern_with_special);
    ASSERT_TRUE(complexity > 5); // Should be more complex than length
}

// Test complexity for patterns with repetitions
CTEST(pattern_complexity, repetitions)
{
    const char *pattern_with_reps = "a{3,5}b*c+";
    int complexity = calculate_pattern_complexity(pattern_with_reps);
    ASSERT_TRUE(complexity > 9); // Should have high complexity
}

// Test complexity for patterns with alternation
CTEST(pattern_complexity, alternation)
{
    const char *pattern_with_alt = "foo|bar|baz";
    int complexity = calculate_pattern_complexity(pattern_with_alt);
    ASSERT_TRUE(complexity > 11); // Should consider alternations
}

// Test null and empty patterns
CTEST(pattern_complexity, edge_cases)
{
    ASSERT_EQUAL(0, calculate_pattern_complexity(""));
    ASSERT_EQUAL(0, calculate_pattern_complexity(NULL));
}
