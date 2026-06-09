# Example 01 — Minimal Regex Tokenizer

This is the simplest possible demonstration of RIFT's tokenisation engine.

## What it shows

A `State` holds a POSIX extended regular expression. `state_matches()` evaluates any text against that pattern using the POSIX `regcomp` / `regexec` API. A linear scan of registered states classifies each incoming token.

## Build

```bash
gcc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -o tokenize tokenziation.c
./tokenize
```

## Expected output

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

## How this maps to the RIFT pipeline

In the full compiler (`src/lang/lexer.c`), `State` objects are registered during lexer initialisation from the grammar definition. The `regcomp` call happens once per pattern, and the cached `regex_t` is reused for every source file. The output of the lexer is a `TokenStream` — an ordered list of `(kind, lexeme, source_location)` triples — which is consumed by the parser.

## Next example

→ [`../02_poc_automaton/`](../02_poc_automaton/) — full finite-state automaton with transitions and IR generation
