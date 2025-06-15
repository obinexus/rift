/**
 * RIFT Advanced Configuration Structures
 * OBINexus AEGIS Framework - Complete type definitions
 * 
 * Structure IS the syntax - maintains token type/value separation
 * Preserves matched_state for AST minimization
 */

#ifndef RIFT_ADVANCED_CONFIG_STRUCTURES_H
#define RIFT_ADVANCED_CONFIG_STRUCTURES_H

#include <stdint.h>
#include <stdbool.h>

// AEGIS Pipeline Mode Enumeration
typedef enum {
    RIFT_PIPELINE_SEQUENTIAL = 0,
    RIFT_PIPELINE_PARALLEL = 1,
    RIFT_PIPELINE_HYBRID = 2
} RiftPipelineMode;

// OBINexus Threading Model Enumeration
typedef enum {
    RIFT_THREADING_MODEL_1_PARALLEL = 0,    // True parallelism with dedicated cores
    RIFT_THREADING_MODEL_2_CONCURRENT = 1,  // Shared-core time-sliced
    RIFT_THREADING_HYBRID = 2               // Both models enabled
} RiftThreadingModel;

// Pipeline Configuration Structure
typedef struct {
    bool enabled;
    RiftPipelineMode mode;
    uint32_t max_parallel_stages;
    bool sequence_validation;
    bool artifact_tracking;
    bool stage0_parallel;
    bool stage1_parallel;
    bool stage2_sequential;
    bool final_sequential;
} RiftPipelineConfig;

// Threading Configuration Structure (OBINexus Models 1 & 2)
typedef struct {
    RiftThreadingModel model;
    
    // Model 1: True Parallelism
    bool model1_enabled;
    bool model1_dedicated_cores;
    bool model1_governance_isolation;
    bool model1_cryptographic_identity;
    
    // Model 2: Shared-Core Concurrent
    bool model2_enabled;
    bool model2_time_sliced;
    bool model2_governance_reconciliation;
    bool model2_parent_child_hierarchy;
} RiftThreadingConfig;

// Worker Pool Configuration Structure
typedef struct {
    uint32_t default_pool_size;
    uint32_t max_pool_size;
    uint32_t min_pool_size;
    uint32_t worker_timeout;
    bool governance_validation_per_worker;
    
    // Specialized pools for pipeline stages
    uint32_t tokenizer_pool_size;
    uint32_t parser_pool_size;
    uint32_t ast_pool_size;
    uint32_t validator_pool_size;
} RiftWorkerPoolConfig;

// Governance Configuration Structure (OBINexus Governance Triangle)
typedef struct {
    bool enabled;
    bool strict_mode;
    bool cryptographic_validation;
    bool audit_trail;
    bool thread_governance_isolation;
    bool worker_cryptographic_identity;
    bool cross_thread_validation;
    bool governance_context_inheritance;
    
    // Governance triangle thresholds
    double attack_risk_threshold;
    double rollback_cost_threshold;
    double stability_impact_threshold;
    double max_governance_vector;
} RiftGovernanceConfig;

// Complete Advanced Configuration Structure
typedef struct RiftAdvancedConfig {
    // Basic configuration
    char* output_dir;
    char* stage_name;
    bool debug_mode;
    bool verbose;
    char* log_level;
    
    // AEGIS Framework nested configurations
    RiftPipelineConfig pipeline;
    RiftThreadingConfig threading;
    RiftWorkerPoolConfig worker_pools;
    RiftGovernanceConfig governance;
    
    // TokenMemory pipeline state preservation
    size_t matched_state;           // AEGIS state minimization support
    uint64_t token_validation_hash; // Cryptographic token integrity
    bool zero_trust_enabled;        // Zero Trust pipeline compliance
} RiftAdvancedConfig;

// Function declarations that match the implementation
RiftAdvancedConfig* rift_advanced_config_create(void);
void rift_advanced_config_destroy(RiftAdvancedConfig* config);
int rift_advanced_config_load(RiftAdvancedConfig* config, const char* config_file);
int rift_advanced_config_validate(const RiftAdvancedConfig* config);
void rift_demo_pipeline_config(const RiftAdvancedConfig* config);

#endif // 
#RIFT_ADVANCED_CONFIG_STRUCTURES_H
