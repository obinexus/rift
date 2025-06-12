#ifndef RIFT_STAGE_PROCESSOR_H
#define RIFT_STAGE_PROCESSOR_H

#include "../../../include/rift.h"
#include "config_handler.h"

typedef struct RiftStageProcessor {
    RiftEngine* engine;
    RiftConfig* config;
    char* input_file;
    char* output_file;
    int stage_number;
} RiftStageProcessor;

RiftStageProcessor* rift_stage_processor_create(const char* input_file, RiftConfig* config);
void rift_stage_processor_destroy(RiftStageProcessor* processor);
RiftResult rift_stage_processor_run(RiftStageProcessor* processor);
RiftResult rift_process_stage0_tokenization(RiftStageProcessor* processor);
RiftResult rift_generate_executable_from_ir(const char* ir_file, const char* output_exe);

#endif
