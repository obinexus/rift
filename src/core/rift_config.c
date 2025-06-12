#include "../../include/rift.h"

RiftConfig* rift_config_create(void) {
    RiftConfig* config = calloc(1, sizeof(RiftConfig));
    if (!config) return NULL;
    
    config->output_dir = strdup("./output");
    config->stage_name = strdup("rift.0");
    config->debug_mode = false;
    config->verbose = false;
    config->log_level = strdup("info");
    
    return config;
}

void rift_config_destroy(RiftConfig* config) {
    if (!config) return;
    
    free(config->output_dir);
    free(config->stage_name);
    free(config->log_level);
    free(config);
}

RiftResult rift_config_load(RiftConfig* config, const char* config_file) {
    if (!config || !config_file) return RIFT_ERROR_NULL_POINTER;
    
    FILE* file = fopen(config_file, "r");
    if (!file) return RIFT_ERROR_IO;
    
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        char* equals = strchr(line, '=');
        if (!equals) continue;
        
        *equals = '\0';
        char* key = line;
        char* value = equals + 1;
        
        char* newline = strchr(value, '\n');
        if (newline) *newline = '\0';
        
        if (strcmp(key, "output_dir") == 0) {
            free(config->output_dir);
            config->output_dir = strdup(value);
        } else if (strcmp(key, "stage_name") == 0) {
            free(config->stage_name);
            config->stage_name = strdup(value);
        } else if (strcmp(key, "debug") == 0) {
            config->debug_mode = (strcmp(value, "true") == 0);
        } else if (strcmp(key, "verbose") == 0) {
            config->verbose = (strcmp(value, "true") == 0);
        } else if (strcmp(key, "log_level") == 0) {
            free(config->log_level);
            config->log_level = strdup(value);
        }
    }
    
    fclose(file);
    return RIFT_SUCCESS;
}
