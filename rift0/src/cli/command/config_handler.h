#ifndef RIFT_CONFIG_HANDLER_H
#define RIFT_CONFIG_HANDLER_H

#include "../../../include/rift.h"

typedef struct RiftConfigCommand {
    char* config_path;
    RiftConfig* config;
    bool readonly_mode;
} RiftConfigCommand;

RiftConfigCommand* rift_config_command_create(const char* config_path);
void rift_config_command_destroy(RiftConfigCommand* cmd);
RiftResult rift_config_command_load(RiftConfigCommand* cmd);

#endif
