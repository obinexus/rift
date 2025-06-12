#include "../../include/rift.h"
#include <assert.h>

void test_pattern_parsing(void) {
    printf("🧪 Testing RIFT pattern parsing...\n");
    
    const char* test_pattern = "[A-Z]+/gmi[t]";
    printf("   Input pattern: '%s'\n", test_pattern);
    
    RiftPattern* pattern = rift_parse_pattern(test_pattern);
    assert(pattern != NULL);
    
    printf("   Extracted regex: '%s'\n", pattern->regex_pattern);
    assert(strcmp(pattern->regex_pattern, "[A-Z]+") == 0);
    assert(strcmp(pattern->flags, "gmi") == 0);
    assert(pattern->mode == RIFT_MODE_TOP_DOWN);
    
    printf("   ✅ Pattern extraction: PASSED\n");
    printf("   ✅ Flags extraction: PASSED\n");
    printf("   ✅ Mode extraction: PASSED\n");
    
    rift_pattern_destroy(pattern);
}

int main(void) {
    printf("🔬 RIFT Stage 0 QA Tests\n");
    printf("========================\n");
    
    test_pattern_parsing();
    
    printf("✅ All tests passed!\n");
    return 0;
}
