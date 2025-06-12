#include "../../include/rift.h"
#include "command/config_handler.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        rift_print_usage(argv[0]);
        return 1;
    }
    
    // Enhanced CLI with .riftrc processing
    RiftConfigCommand* config_cmd = rift_config_command_create(".riftrc");
    if (config_cmd) {
        rift_config_command_load(config_cmd);
        printf("✅ Configuration loaded from .riftrc\n");
    }
    
    // Process input file
    const char* input_file = argv[1];
    const char* output_file = "./output/sample.rift.0";
    
    RiftResult result = rift_process_file(input_file, output_file, 
                                         config_cmd ? config_cmd->config : NULL);
    
    if (result == RIFT_SUCCESS) {
        printf("✅ Stage 0 processing complete\n");
    } else {
        printf("❌ Processing failed: %s\n", rift_result_string(result));
    }
    
    rift_config_command_destroy(config_cmd);
    return (result == RIFT_SUCCESS) ? 0 : 1;
}
