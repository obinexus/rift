// cli/command/config_handler.h
#ifndef RIFT_CONFIG_HANDLER_H
#define RIFT_CONFIG_HANDLER_H
#include <pthread.h>
#include <atomic.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>
#include "../../include/rift.h"

// Configuration command structure for .riftrc handling
typedef struct RiftConfigCommand {
    char* config_path;
    RiftConfig* config;
    bool readonly_mode;
    bool validate_only;
} RiftConfigCommand;

// Configuration processing results
typedef enum {
    CONFIG_SUCCESS = 0,
    CONFIG_ERROR_FILE_NOT_FOUND = -1,
    CONFIG_ERROR_INVALID_FORMAT = -2,
    CONFIG_ERROR_READONLY_VIOLATION = -3,
    CONFIG_ERROR_PATTERN_INVALID = -4
} ConfigResult;

// Core configuration command functions
RiftConfigCommand* rift_config_command_create(const char* config_path);
void rift_config_command_destroy(RiftConfigCommand* cmd);
ConfigResult rift_config_command_load(RiftConfigCommand* cmd);
ConfigResult rift_config_command_validate(RiftConfigCommand* cmd);
ConfigResult rift_config_command_apply(RiftConfigCommand* cmd, RiftEngine* engine);

// .riftrc specific processing
ConfigResult rift_process_riftrc(const char* riftrc_path, RiftConfig* config);
ConfigResult rift_validate_riftrc_readonly(const char* riftrc_path);

// Pattern mode configuration
typedef enum {
    PATTERN_MODE_TOP_DOWN = 't',
    PATTERN_MODE_BOTTOM_UP = 'b',
    PATTERN_MODE_AUTO = 'a'
} PatternMode;

ConfigResult rift_config_set_pattern_mode(RiftConfig* config, PatternMode mode);
PatternMode rift_config_get_pattern_mode(const RiftConfig* config);

#endif // RIFT_CONFIG_HANDLER_H
