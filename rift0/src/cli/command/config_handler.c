#include "config_handler.h"

RiftConfigCommand* rift_config_command_create(const char* config_path) {
    RiftConfigCommand* cmd = calloc(1, sizeof(RiftConfigCommand));
    if (!cmd) return NULL;
    cmd->config_path = strdup(config_path);
    cmd->config = rift_config_create();
    cmd->readonly_mode = true;
    return cmd;
}

void rift_config_command_destroy(RiftConfigCommand* cmd) {
    if (!cmd) return;
    free(cmd->config_path);
    rift_config_destroy(cmd->config);
    free(cmd);
}

RiftResult rift_config_command_load(RiftConfigCommand* cmd) {
    if (!cmd) return RIFT_ERROR_NULL_POINTER;
    return rift_config_load(cmd->config, cmd->config_path);
}
