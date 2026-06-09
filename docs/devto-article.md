---
title: "RIFT Is a Flexible Translator: How Regex-Driven Pattern Matching Powers Multi-Target Code Generation"
published: false
description: "A deep dive into how RIFT's regex automaton turns raw source text into a governed, multi-target IR — with working C proof-of-concept code you can run today."
tags: compiler, c, opensource, programming
cover_image: ""
canonical_url: "https://github.com/obinexusmk2/rift"
---

# RIFT Is a Flexible Translator: How Regex-Driven Pattern Matching Powers Multi-Target Code Generation

> **Repo:** [github.com/obinexusmk2/rift](https://github.com/obinexusmk2/rift)  
> **Version:** v5.0.0 · Language: C11 · License: OBINexus

---

Most compiler tutorials start with a recursive-descent parser and a hardwired grammar. RIFT takes a different path. Its tokeniser has no hardwired rules. Every token class is a regular expression registered at startup, and the recognition engine is a data-driven finite-state automaton that can be extended at runtime — from macro files, plugin grammars, or programmatic registration calls.

This post walks through three working C programs that demonstrate the core idea from the ground up, then shows how those ideas connect to a production governance pipeline that spans seven distinct compilation stages.

---

## Why Build Another Translator?

The short answer: because none of the existing tools are *flexible* in the right dimension.

LLVM is powerful but monolithic. Transpilers like Babel are language-specific. Tree-sitter is excellent for syntax highlighting but not designed for governed code emission. When you want to take a single source file and emit correct, auditable output in C, JavaScript, Python, Go, Lua, and WebAssembly Text Format — from the same parse tree, with the same semantic guarantees — you need a different architecture.

RIFT's answer is a three-layer pipeline:

```
Source text
    ↓  [Regex Automaton — pattern.c / lexer.c]
Token IR
    ↓  [Parser → Semantic → Validator]
Governed AST
    ↓  [Bytecode → Verifier → Emitter]
Target output (C / JS / Python / Go / Lua / WAT)
```

The first layer — pattern matching — is what this article is about. Get it right, and everything downstream becomes composable.

---

## The Regex Automaton: Core Data Model

The automaton is three structs. Here is the complete definition from `examples/02_poc_automaton/rift_poc.c`:

```c
typedef struct State {
    char   *pattern;    /* POSIX extended regex */
    bool    is_final;   /* accepting state? */
    size_t  id;         /* monotone unique identity */
} State;

typedef struct Transition {
    State *from_state;
    char  *input_pattern;   /* regex on the arc label */
    State *to_state;
} Transition;

typedef struct RegexAutomaton {
    State      **states;
    size_t       state_count;
    size_t       state_capacity;   /* dynamic — doubles on overflow */

    Transition **transitions;
    size_t       transition_count;
    size_t       transition_capacity;

    State *initial_state;
    State *current_state;
} RegexAutomaton;
```

No grammar file format to learn. No special DSL. Token classes are strings — POSIX extended regular expressions — registered by calling `automaton_add_state`.

### Why POSIX ERE, not something fancier?

Three reasons:

1. **Portability.** `regcomp` / `regexec` are available on every POSIX-compliant platform — Linux, macOS, and Windows via MSYS2/MinGW — without adding a dependency.
2. **Auditability.** A plain regex string is human-readable. When a governance audit needs to verify what token classes a build was using, the answer is a list of strings in the schema, not a compiled DFA blob.
3. **Composability.** Because each state's pattern is independent, registering a new token class (say, from a macro file) cannot break existing classes. There is no re-compilation of a monolithic grammar table.

---

## Example 1 — Minimal Tokenizer

The simplest form strips out the automaton machinery entirely and shows classification in three lines of logic (`examples/01_tokenization/tokenziation.c`):

```c
State *identifier = state_create("^[a-zA-Z_][a-zA-Z0-9_]*$", false);
State *number     = state_create("^[0-9]+$",                  false);
State *op         = state_create("^[-+*/]$",                  false);

for (int i = 0; i < n; i++) {
    const char *tok = tokens[i];
    if      (state_matches(identifier, tok)) puts("Identifier");
    else if (state_matches(number,     tok)) puts("Number");
    else if (state_matches(op,         tok)) puts("Operator");
}
```

The `state_matches` function is the entire engine:

```c
bool state_matches(const State *s, const char *text) {
    regex_t rx;
    if (regcomp(&rx, s->pattern, REG_EXTENDED) != 0) return false;
    int rc = regexec(&rx, text, 0, NULL, 0);
    regfree(&rx);
    return (rc == 0);
}
```

Feed it `{"x", "+", "123", "*", "y", "42"}` and you get:

```
Token     Type
──────    ──────────
x         Identifier
+         Operator
123       Number
*         Operator
y         Identifier
42        Number
```

Build and run it yourself:

```bash
git clone https://github.com/obinexusmk2/rift
cd rift/examples/01_tokenization
gcc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -o tokenize tokenziation.c
./tokenize
```

---

## Example 2 — Full Automaton + IR Generator

A tokeniser without an IR is a classifier, not a compiler. The full proof-of-concept adds two more structures (`examples/02_poc_automaton/rift_poc.c`):

```c
typedef struct TokenNode {
    char *type;    /* matched state's pattern — canonical class name */
    char *value;   /* raw lexeme from source */
} TokenNode;

typedef struct IRGenerator {
    RegexAutomaton *automaton;
    TokenNode     **nodes;       /* growing list */
    size_t          node_count;
    size_t          node_capacity;
} IRGenerator;
```

`ir_generator_process_token` is the translation step:

```c
TokenNode *
ir_generator_process_token(IRGenerator *g, const char *token) {
    State *s = automaton_get_next_state(g->automaton, token);
    if (!s) return NULL;

    TokenNode *node = malloc(sizeof *node);
    node->type  = strdup(s->pattern);   /* structural type */
    node->value = strdup(token);         /* raw spelling */
    return node;
}
```

Each `TokenNode` carries both the **structural type** (which pattern class matched) and the **raw lexeme** (what the source actually said). This matters for code emission: a JavaScript emitter might want to rewrite `*` as `Math.multiply` in certain contexts, but it still needs the original spelling to make that decision.

### Capacity doubling — the open extension point

```c
if (a->state_count >= a->state_capacity) {
    size_t  nc = a->state_capacity * 2;
    State **ns = realloc(a->states, sizeof(State *) * nc);
    ...
}
```

Because the state registry is a heap-allocated dynamic array, calling `automaton_add_state` at any point — including after parsing has begun — is safe. This is what allows RIFT macros (`.rf` files) to register new token classes without restarting the lexer.

---

## Example 3 — The `choose` Function: Resolving Pattern Alternatives

When a grammar rule has multiple valid productions, the automaton needs to pick one. RIFT uses a primitive called `choose` — ordered selection without replacement (`examples/03_choose_pattern/choose.c`):

```c
void choose(const int *choose_sq, int choose_len,
            const char **target_sq, int target_len) {
    bool *used = calloc(target_len, sizeof(bool));

    for (int i = 0; i < choose_len; i++) {
        int index = choose_sq[i];
        int skip  = 0, actual = -1;

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

With `choose_sq = {0, 1, 1}` and `target = {"a","b","c","d"}`:

```
Round 0, index 0 → pool={a,b,c,d} → picks "a"  → pool={b,c,d}
Round 1, index 1 → pool={b,c,d}   → picks "c"  → pool={b,d}
Round 2, index 1 → pool={b,d}     → picks "d"  → pool={b}

Output:  a c d
```

### Why this matters for a flexible translator

A real grammar ambiguity looks like this:

```
expr := NUMBER
      | IDENTIFIER
      | '(' expr ')'
      | expr OP expr
```

When the parser sees `(x + 42)`, the automaton's state trace produces a choose sequence that says: "production 2 first, then production 0". The `choose` function resolves this to `'(' expr ')'` → `NUMBER`, which is the correct parse.

The same sequence, applied to the same grammar, always produces the same result. That is RIFT's *deterministic translation guarantee* — the same source file produces bit-identical IR regardless of which target you are emitting to.

---

## The Seven-Stage Governance Pipeline

Token classification is stage 0. After it comes six more stages, all described by `schema.json` in the repo root:

```
tokenizer → parser → semantic → validator → bytecode → verifier → emitter
```

Each stage has machine-verifiable governance contracts defined in the JSON schema:

```json
"governance_substages": {
  "tokenizer": {
    "lexeme_validation": true,
    "token_memory_constraints": 4096,
    "encoding_normalization": true
  },
  "parser": {
    "ast_depth_limit": 500,
    "syntax_strictness": "strict",
    "error_recovery": true
  },
  "validator": {
    "structural_acyclicity": true,
    "cost_bounds_enforced": true,
    "governance_hash_required": true
  }
}
```

Stage 5 — the optimizer — goes further. It requires a SHA-256 hash of all execution paths *before* optimization, a second hash *after*, and a `semantic_equivalence_proof` flag confirming that the transformation preserved program meaning:

```json
"stage_5_optimizer": {
  "optimizer_model": "AST-aware-minimizer-v2",
  "path_hash": "<sha256-of-paths-before>",
  "post_optimization_hash": "<sha256-of-paths-after>",
  "semantic_equivalence_proof": true,
  "security_level": "exploit_elimination",
  "audit_enabled": true
}
```

This is RIFT's **Constitutional Computing** principle: no transformation in the pipeline is anonymous. Every optimisation, macro expansion, and code emission is traceable back to a signed, auditable record.

---

## The Toolchain Chain

The proof-of-concept files sit at the bottom of a larger toolchain:

```
riftlang.exe  →  libriftlang.so / libriftlang.a  →  rift.exe
```

- `riftlang.exe` is the standalone inner compiler — it handles the regex→IR translation described in this article.
- `libriftlang.so / .a` exposes that translation as a library API for embedding.
- `rift.exe` is the full CLI: it orchestrates all seven pipeline stages, calls into `libriftlang`, and drives `nlink` + `polybuild` for multi-target output.

```bash
# Compile to C (default)
./build/rift compile program.rift

# Compile to JavaScript
./build/rift compile -o program.js program.rift

# Compile to Python
./build/rift compile -o program.py program.rift

# Show the token stream
./build/rift tokenize program.rift

# Show the AST
./build/rift parse program.rift
```

---

## Building the Project

```bash
git clone https://github.com/obinexusmk2/rift
cd rift

# Linux / macOS (WSL works too)
make clean && make

# Windows (PowerShell + MinGW)
make

# The built binary
./build/rift --version
# RIFT Is a Flexible Translator v5.0.0
```

The build produces three outputs: `build/rift` (CLI), `build/lib/librift.a` (static), and `build/lib/librift.so` (shared).

---

## Running the Examples

Each example is self-contained with a single `gcc` invocation:

```bash
# Example 1 — minimal tokenizer
cd examples/01_tokenization
gcc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -o tokenize tokenziation.c && ./tokenize

# Example 2 — full automaton + IR
cd ../02_poc_automaton
gcc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -o rift_poc rift_poc.c && ./rift_poc

# Example 3 — choose / pattern alternatives
cd ../03_choose_pattern
gcc -std=c11 -Wall -o choose choose.c && ./choose
```

---

## What's Next

The three examples here cover lexical analysis. The next layer is parsing — taking the `TokenNode` list from the IR generator and building an AST. That work lives in `src/lang/parser.c` and follows the same data-driven philosophy: grammar rules are registered as data, not compiled into a monolithic recursive-descent function.

The stages after that — semantic analysis, validation, bytecode, verification, emission — are each governed by the schema contracts shown above. Every stage can be individually audited, replaced, or extended without touching the others.

If you want to follow the development:

- **Repo:** [github.com/obinexusmk2/rift](https://github.com/obinexusmk2/rift)
- **Examples:** `rift/examples/`
- **Docs:** `rift/docs/pattern-matching.md`
- **Schema:** `rift/schema.json`
- **Issues / PRs:** welcome — especially additional emit targets and language bindings

---

*RIFT Is a Flexible Translator. Respect the scope. Respect the architecture.*  
*— OBINexus, Nnamdi Michael Okpala*
