# RIFT Pattern Matching & Regex Automaton — Deep Dive

> **Repository:** https://github.com/obinexusmk2/rift  
> **Component:** `src/lang/pattern.c` · `src/lang/lexer.c` · `include/rift/pattern.h`  
> **Proof-of-concept sources:** `examples/01_tokenization/` · `examples/02_poc_automaton/` · `examples/03_choose_pattern/`

---

## Overview

RIFT's core identity as a **Flexible Translator** is built on one foundational insight: *every language, at its lowest level, is a set of pattern-matching rules applied to a stream of text*. Before a RIFT source file can be compiled, bridged, or emitted to a target (C, JS, Python, Go, Lua, WASM), it must first be reduced to a well-defined token stream. That reduction is governed entirely by a **regex-driven finite-state automaton**.

This document explains how that automaton works, traces it from the proof-of-concept C implementations through to the full RIFT pipeline, and shows why this design enables RIFT's multi-target translation promise.

---

## 1. The Problem: Why Pattern Matching, Not Hand-Written Rules

Traditional translators hard-code token recognition logic: `if (ch == '+') return TOK_PLUS`. This works for a single fixed grammar but breaks the moment you need to:

- Add a new target language with different operator semantics
- Support quantum-mode execution where state transitions carry probability weights
- Embed a governance validation layer that can be audited at each pipeline stage

RIFT takes a different approach. Every token class is described by a **regular expression**. The recogniser is a data-driven automaton, not a hand-written switch statement. To add a new token class — even across a language target — you register a new `(pattern, state)` pair. No code changes required.

---

## 2. The Regex Automaton Data Model

The automaton is built from three core structures. Here they are as implemented in `examples/02_poc_automaton/rift_poc.c`:

```c
typedef struct State {
    char*  pattern;    /* POSIX extended regex */
    bool   is_final;   /* accepting state? */
    size_t id;         /* monotonically-assigned identity */
} State;

typedef struct Transition {
    State* from_state;
    char*  input_pattern;   /* regex triggering this edge */
    State* to_state;
} Transition;

typedef struct RegexAutomaton {
    State**     states;
    size_t      state_count;
    size_t      state_capacity;   /* dynamic growth */

    Transition** transitions;
    size_t       transition_count;
    size_t       transition_capacity;

    State* initial_state;
    State* current_state;
} RegexAutomaton;
```

### Why dynamic arrays with capacity doubling?

```c
if (automaton->state_count >= automaton->state_capacity) {
    size_t new_capacity = automaton->state_capacity * 2;
    State** new_states = realloc(automaton->states,
                                 sizeof(State*) * new_capacity);
    ...
}
```

RIFT's grammar is **open**: users can register macros (`.rf` files), and each macro may introduce new token classes. Hard-coding a fixed array size would cap expressiveness. The doubling strategy keeps amortised insert at O(1) while avoiding frequent reallocations.

---

## 3. State Matching: POSIX `regcomp` / `regexec`

Each `State` carries a full POSIX extended regex. Matching is performed by compiling the pattern on demand:

```c
bool state_matches(State* state, const char* text) {
    regex_t regex;
    int result = regcomp(&regex, state->pattern, REG_EXTENDED);
    if (result) return false;

    result = regexec(&regex, text, 0, NULL, 0);
    regfree(&regex);               /* always free to avoid leaks */

    return (result == 0);
}
```

### Production note

In the full RIFT pipeline (`src/lang/lexer.c`), patterns are compiled **once** at lexer initialisation and cached for the lifetime of a compilation unit. The per-call compile/free cycle in the POC is intentionally simplified for clarity.

---

## 4. Token Classification: The Simpler Form

`examples/01_tokenization/tokenziation.c` strips away the full automaton machinery and shows the classification logic in its purest form:

```c
// Define token patterns
State* identifier = state_create("^[a-zA-Z_]\\w*$", false);
State* number     = state_create("^\\d+$",           false);
State* op         = state_create("^[+\\-*/]$",        false);

// Classify each token
for (int i = 0; i < token_count; i++) {
    const char* tok = tokens[i];
    if      (state_matches(identifier, tok)) printf("Identifier\n");
    else if (state_matches(number,     tok)) printf("Number\n");
    else if (state_matches(op,         tok)) printf("Operator\n");
    else                                     printf("Unknown\n");
}
```

Running this against `{"x", "+", "123", "*", "y", "42"}` produces:

```
Token: x     | Type: Identifier
Token: +     | Type: Operator
Token: 123   | Type: Number
Token: *     | Type: Operator
Token: y     | Type: Identifier
Token: 42    | Type: Number
```

This is exactly the output that feeds RIFT's IR generator.

---

## 5. From Tokens to IR: The `IRGenerator`

Once a token is classified, it becomes a `TokenNode` in the **intermediate representation**:

```c
typedef struct TokenNode {
    char* type;   /* state pattern as canonical type name */
    char* value;  /* raw lexeme */
} TokenNode;

typedef struct IRGenerator {
    RegexAutomaton* automaton;
    TokenNode**     nodes;
    size_t          node_count;
    size_t          node_capacity;
} IRGenerator;
```

`ir_generator_process_token` drives the automaton forward and stamps each recognised lexeme into an IR node:

```c
TokenNode* ir_generator_process_token(IRGenerator* gen, const char* token) {
    State* next_state = automaton_get_next_state(gen->automaton, token);
    if (!next_state) return NULL;

    TokenNode* node  = malloc(sizeof(TokenNode));
    node->type  = strdup(next_state->pattern);
    node->value = strdup(token);
    return node;
}
```

The IR node list is what every downstream stage (parser → semantic → validator → emitter) consumes. Because IR nodes carry both the **matched pattern** (type) and the **raw text** (value), a codegen target can reconstruct or transform the original expression in any target language.

---

## 6. The `choose` Function: Ordered Pattern Selection

`examples/03_choose_pattern/choose.c` demonstrates a concept that looks simple but is central to RIFT's macro expansion and pattern alternative resolution:

```c
void choose(int* choose_sq, int choose_len,
            const char** target_sq, int target_len) {
    bool* used = calloc(target_len, sizeof(bool));

    for (int i = 0; i < choose_len; i++) {
        int index     = choose_sq[i];
        int skip      = 0;
        int actual    = -1;

        for (int j = 0; j < target_len; j++) {
            if (!used[j]) {
                if (skip == index) { actual = j; break; }
                skip++;
            }
        }
        if (actual != -1) {
            printf("%s ", target_sq[actual]);
            used[actual] = true;
        }
    }
    free(used);
}
```

Given `choose_sq = {0, 1, 1}` and `target_sq = {"a","b","c","d"}`, the output is `a b c` because:

- Round 0: skip 0 unused → pick `a` (index 0). Mark used.
- Round 1: skip 1 unused → pick `b` (index 1 of remaining `{b,c,d}`). Mark used.
- Round 2: skip 1 unused → pick `c` (index 1 of remaining `{c,d}`). Mark used.

This is **ordered selection without replacement**. In RIFT's pattern layer, the same algorithm drives the resolution of macro alternatives: when a `.rf` macro expands into multiple candidate productions, the `choose` sequence determines which production is selected at each expansion site, preserving determinism.

---

## 7. Pipeline Governance: Connecting the Schema

The automaton and IR generator are only the first two stages of RIFT's full pipeline, which is formally described in `schema.json`:

```
tokenizer → parser → semantic → validator → bytecode → verifier → emitter
```

Each stage has machine-verifiable governance contracts:

| Stage      | Key Invariant |
|------------|---------------|
| tokenizer  | lexeme validation + memory constraints |
| parser     | AST depth limit + syntax strictness |
| semantic   | type checking + scope validation |
| validator  | structural acyclicity + cost bounds |
| bytecode   | opcode validation + operand alignment |
| verifier   | bytecode integrity + stack safety |
| emitter    | target architecture + optimisation level |

The Stage 5 optimizer additionally requires a SHA-256 `path_hash` before and after optimisation, with `semantic_equivalence_proof` to formally verify that transformations preserve program meaning. This is RIFT's **Constitutional Computing** principle: every transformation must be auditable.

```json
"stage_5_optimizer": {
  "optimizer_model": "AST-aware-minimizer-v2",
  "minimization_verified": true,
  "path_hash": "<sha256-before>",
  "post_optimization_hash": "<sha256-after>",
  "audit_enabled": true,
  "security_level": "exploit_elimination",
  "semantic_equivalence_proof": true
}
```

---

## 8. Why This Architecture Enables Multi-Target Translation

A regex automaton with an open state registry means:

1. **Adding a new target language** requires only registering new emission rules in the codegen layer — the tokenizer, parser, and semantic stages are completely unchanged.
2. **Adding a new token class** (e.g., for a DSL macro) requires only one `automaton_add_state` call.
3. **Quantum mode** can weight transitions probabilistically without touching the deterministic classical path.
4. **Governance hooks** can intercept at any stage boundary because the IR is a stable, typed data structure.

The `riftlang.exe → .so.a → rift.exe` toolchain chain is the embodiment of this: the standalone `riftlang` compiler handles the inner regex→IR translation, the shared library exposes it as an API surface, and the top-level `rift` CLI orchestrates the full multi-stage pipeline through `nlink` and `polybuild`.

---

## Further Reading

- [`examples/01_tokenization/`](../examples/01_tokenization/) — minimal regex tokenizer demo  
- [`examples/02_poc_automaton/`](../examples/02_poc_automaton/) — full automaton + IR generator  
- [`examples/03_choose_pattern/`](../examples/03_choose_pattern/) — ordered pattern selection  
- [`schema.json`](../schema.json) — full pipeline governance schema  
- [`src/lang/pattern.c`](../src/lang/pattern.c) — production pattern implementation  
- [dev.to article](./devto-article.md) — blog-style introduction for a public audience
