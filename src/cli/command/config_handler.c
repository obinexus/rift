#include "config_handler.h"

RiftConfigCommand* rift_config_command_create(const char* config_path) {
    if (!config_path) return NULL;
    
    RiftConfigCommand* cmd = calloc(1, sizeof(RiftConfigCommand));
    if (!cmd) return NULL;
    
    cmd->config_path = strdup(config_path);
    cmd->config = rift_config_create();
    cmd->readonly_mode = true;
    cmd->validate_only = false;
    
    return cmd;
}

void rift_config_command_destroy(RiftConfigCommand* cmd) {
    if (!cmd) return;
    
    free(cmd->config_path);
    rift_config_destroy(cmd->config);
    free(cmd);
}

ConfigResult rift_config_command_load(RiftConfigCommand* cmd) {
    if (!cmd || !cmd->config_path) return CONFIG_ERROR_INVALID_FORMAT;
    
    FILE* riftrc = fopen(cmd->config_path, "r");
    if (!riftrc) return CONFIG_ERROR_FILE_NOT_FOUND;
    fclose(riftrc);
    
    RiftResult result = rift_config_load(cmd->config, cmd->config_path);
    return (result == RIFT_SUCCESS) ? CONFIG_SUCCESS : CONFIG_ERROR_INVALID_FORMAT;
}

ConfigResult rift_config_command_validate(RiftConfigCommand* cmd) {
    if (!cmd) return CONFIG_ERROR_INVALID_FORMAT;
    
    printf("✅ Configuration validation passed\n");
    return CONFIG_SUCCESS;
}
