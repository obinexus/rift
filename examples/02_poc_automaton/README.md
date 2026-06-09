# Example 02 — Full Regex Automaton + IR Generator

This is the core proof-of-concept for RIFT's pattern-matching engine. It shows the complete object graph that the production compiler is built on.

## Data structures

```
RegexAutomaton
├── State[]         — each state holds a POSIX ERE pattern
└── Transition[]    — directed arcs between states, labelled with patterns

IRGenerator
├── RegexAutomaton* — the driving automaton
└── TokenNode[]     — the growing IR list
    ├── .type       — canonical class name (the matched state's pattern)
    └── .value      — raw lexeme from source input
```

## Build

```bash
gcc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -o rift_poc rift_poc.c
./rift_poc
```

## Expected output

```
RIFT IR Generator — token stream for: x + 123 * y
─────────────────────────────────────────────────

Token     Type        State ID
──────    ────────    ────────
x         Identifier  pattern=^[a-zA-Z_][a-zA-Z0-9_]*$
+         Operator    pattern=^[+\-*/]$
123       Number      pattern=^[0-9]+$
*         Operator    pattern=^[+\-*/]$
y         Identifier  pattern=^[a-zA-Z_][a-zA-Z0-9_]*$

IR generation complete. 5 tokens processed.
```

## Key design decision: dynamic capacity

Both the state array and the transition array grow by doubling on overflow:

```c
size_t nc  = a->state_capacity * 2;
State **ns = realloc(a->states, sizeof(State *) * nc);
```

This keeps the registration API open for runtime extension (macro expansion, plugin grammar fragments) without requiring callers to pre-declare a fixed grammar size.

## Connection to the production compiler

In `src/lang/lexer.c`, the `LexerContext` struct holds a `RegexAutomaton` populated from the grammar definition at startup. The IR node list it produces becomes the `TokenStream` fed into `src/lang/parser.c`. From there it flows through semantic analysis, validation, and finally reaches the emitter which writes the target language output.

## Next example

→ [`../03_choose_pattern/`](../03_choose_pattern/) — ordered selection from pattern alternatives
