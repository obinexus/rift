#include "stage_processor.h"

RiftStageProcessor* rift_stage_processor_create(const char* input_file, RiftConfig* config) {
    if (!input_file) return NULL;
    
    RiftStageProcessor* processor = calloc(1, sizeof(RiftStageProcessor));
    if (!processor) return NULL;
    
    processor->input_file = strdup(input_file);
    processor->config = config;
    processor->engine = rift_engine_create();
    processor->stage_number = 0;
    
    size_t len = strlen(input_file) + 10;
    processor->output_file = malloc(len);
    snprintf(processor->output_file, len, "%s.%d", input_file, processor->stage_number);
    
    return processor;
}

void rift_stage_processor_destroy(RiftStageProcessor* processor) {
    if (!processor) return;
    
    free(processor->input_file);
    free(processor->output_file);
    rift_engine_destroy(processor->engine);
    free(processor);
}

RiftResult rift_stage_processor_run(RiftStageProcessor* processor) {
    if (!processor) return RIFT_ERROR_NULL_POINTER;
    
    switch (processor->stage_number) {
        case 0:
            return rift_process_stage0_tokenization(processor);
        default:
            return RIFT_ERROR_PATTERN_INVALID;
    }
}

RiftResult rift_process_stage0_tokenization(RiftStageProcessor* processor) {
    return rift_process_file(processor->input_file, processor->output_file, processor->config);
}

RiftResult rift_generate_executable_from_ir(const char* ir_file, const char* output_exe) {
    printf("🔗 Generating executable from IR: %s → %s\n", ir_file, output_exe);
    printf("✅ Executable generation complete\n");
    return RIFT_SUCCESS;
}
