# Example 03 — Ordered Pattern Selection (`choose`)

This example demonstrates the combinatorial primitive that underlies RIFT's macro alternative resolution and pattern disambiguation.

## The core idea

When a grammar rule has multiple valid productions, RIFT's automaton generates a *selection sequence* — a list of relative indices into the remaining candidates. The `choose` function consumes this sequence to pick productions deterministically, without replacement.

```
choose_sq = {0, 1, 1}
target    = {a, b, c, d}

Round 0, index 0 → pool={a,b,c,d} → picks "a" → pool={b,c,d}
Round 1, index 1 → pool={b,c,d}   → picks "c" → pool={b,d}
Round 2, index 1 → pool={b,d}     → picks "d" → pool={b}

Output: a c d
```

## Why "without replacement"?

In a real grammar, each alternative is a distinct syntactic form. Selecting the same production twice in one expansion would be malformed. The `used[]` bitmap enforces this invariant.

## Build

```bash
gcc -std=c11 -Wall -o choose choose.c
./choose
```

## Connection to RIFT

In the production compiler (`src/lang/pattern.c`), the choose sequence is derived from the automaton's state trace during parsing. When the parser encounters a non-deterministic choice point, the automaton encodes its disambiguation decision as a relative index. The `choose` algorithm then resolves the selected production, which is passed to the codegen layer.

Because the selection sequence is fully determined by the automaton state, the same source file always produces the same IR — this is RIFT's *deterministic translation guarantee*.
