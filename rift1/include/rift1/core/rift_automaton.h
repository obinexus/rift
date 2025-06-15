
// ===== include/rift1/core/rift_automaton.h =====
#pragma once

#include "rift_types.h"
#include <regex.h>

#ifdef __cplusplus
extern "C" {
#endif

// ===== AEGIS Automaton Structures =====
typedef struct RiftPattern RiftPattern;
typedef struct RiftAutomaton RiftAutomaton;

struct RiftState {
    char* name;
    char* pattern;              // Regex pattern for this state
    bool is_final;
    bool is_initial;
    size_t state_id;
    
    // Transition information
    struct RiftState** transitions;
    size_t transition_count;
};

struct RiftPattern {
    char* pattern_string;
    regex_t compiled_regex;
    RiftTokenType target_type;
    int priority;
};

struct RiftAutomaton {
    RiftState** states;
    size_t state_count;
    size_t state_capacity;
    RiftState* initial_state;
    RiftState* current_state;
};

// ===== Automaton Functions =====
RiftAutomaton* rift_automaton_create(void);
void rift_automaton_destroy(RiftAutomaton* automaton);
RiftResult rift_automaton_add_state(RiftAutomaton* automaton, RiftState* state);
RiftState* rift_state_create(const char* name, const char* pattern);
void rift_state_destroy(RiftState* state);

// ===== Pattern Functions =====
RiftPattern* rift_pattern_create(const char* pattern, RiftTokenType type);
void rift_pattern_destroy(RiftPattern* pattern);

#ifdef __cplusplus
}
#endif