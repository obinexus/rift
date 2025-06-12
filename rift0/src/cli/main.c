#include "../../include/rift.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        rift_print_usage(argv[0]);
        return 1;
    }
    
    const char* input_file = argv[1];
    
    RiftConfig* config = rift_config_create();
    if (!config) {
        fprintf(stderr, "❌ Failed to create configuration\n");
        return 1;
    }
    
    // Try to load .riftrc
    rift_config_load(config, ".riftrc");
    
    // Generate output filename
    char output_file[1024];
    snprintf(output_file, sizeof(output_file), "%s.0", input_file);
    
    printf("🔤 RIFT Stage 0 - Tokenization\n");
    printf("Input:  %s\n", input_file);
    printf("Output: %s\n", output_file);
    
    RiftResult result = rift_process_file(input_file, output_file, config);
    
    if (result == RIFT_SUCCESS) {
        printf("✅ Stage 0 processing complete\n");
    } else {
        printf("❌ Processing failed: %s\n", rift_result_string(result));
    }
    
    rift_config_destroy(config);
    return (result == RIFT_SUCCESS) ? 0 : 1;
}
