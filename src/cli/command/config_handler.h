#ifndef RIFT_CONFIG_HANDLER_H
#define RIFT_CONFIG_HANDLER_H

#include "../../../include/rift.h"

typedef struct RiftConfigCommand {
    char* config_path;
    RiftConfig* config;
    bool readonly_mode;
    bool validate_only;
} RiftConfigCommand;

typedef enum {
    CONFIG_SUCCESS = 0,
    CONFIG_ERROR_FILE_NOT_FOUND = -1,
    CONFIG_ERROR_INVALID_FORMAT = -2,
    CONFIG_ERROR_READONLY_VIOLATION = -3
} ConfigResult;

RiftConfigCommand* rift_config_command_create(const char* config_path);
void rift_config_command_destroy(RiftConfigCommand* cmd);
ConfigResult rift_config_command_load(RiftConfigCommand* cmd);
ConfigResult rift_config_command_validate(RiftConfigCommand* cmd);

#endif
