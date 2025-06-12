#include "../../include/rift.h"
#include "command/config_handler.h"
#include "command/stage_processor.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        rift_print_usage(argv[0]);
        return 1;
    }
    
    // Simple argument parsing for now
    const char* input_file = argv[1];
    
    // Load configuration
    RiftConfigCommand* config_cmd = rift_config_command_create(".riftrc");
    if (config_cmd) {
        ConfigResult result = rift_config_command_load(config_cmd);
        if (result == CONFIG_SUCCESS) {
            printf("✅ Configuration loaded from .riftrc\n");
        }
    }
    
    // Create stage processor
    RiftStageProcessor* processor = rift_stage_processor_create(input_file, config_cmd ? config_cmd->config : NULL);
    if (!processor) {
        fprintf(stderr, "❌ Failed to create stage processor\n");
        rift_config_command_destroy(config_cmd);
        return 1;
    }
    
    // Process the file
    printf("🔤 RIFT Stage 0 - Tokenization/Lexical Analysis\n");
    printf("Input:  %s\n", processor->input_file);
    printf("Output: %s\n", processor->output_file);
    
    RiftResult result = rift_stage_processor_run(processor);
    
    if (result == RIFT_SUCCESS) {
        // Count tokens
        FILE* ir_file = fopen(processor->output_file, "r");
        int token_count = 0;
        if (ir_file) {
            char line[1024];
            while (fgets(line, sizeof(line), ir_file)) {
                if (strncmp(line, "TOKEN", 5) == 0) {
                    token_count++;
                }
            }
            fclose(ir_file);
        }
        
        printf("Processed '%s' -> '%s' (%d tokens)\n", 
               processor->input_file, processor->output_file, token_count);
        printf("✅ Stage 0 processing complete\n");
        printf("💡 Next: Stage 1 (Parsing/Grammar Analysis)\n");
    } else {
        printf("❌ Processing failed: %s\n", rift_result_string(result));
    }
    
    rift_stage_processor_destroy(processor);
    rift_config_command_destroy(config_cmd);
    
    return (result == RIFT_SUCCESS) ? 0 : 1;
}
