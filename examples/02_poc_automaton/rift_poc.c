/**
 * examples/02_poc_automaton/rift_poc.c
 *
 * RIFT Is a Flexible Translator — Proof of Concept #2
 * Full regex-driven finite-state automaton with IR generation.
 *
 * This file demonstrates the complete object graph that underpins RIFT's
 * lexical and pattern-matching layers:
 *
 *   RegexAutomaton  — directed graph of States connected by Transitions
 *   IRGenerator     — wraps the automaton; converts tokens → IR nodes
 *   TokenNode       — the unit of the Intermediate Representation
 *
 * The IR node list produced here is the data structure consumed by every
 * downstream pipeline stage (parser → semantic → validator → emitter).
 *
 * Build:
 *   gcc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -o rift_poc rift_poc.c
 *
 * Run:
 *   ./rift_poc
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>

/* ── Forward declarations ────────────────────────────────────────────────── */

typedef struct State        State;
typedef struct Transition   Transition;
typedef struct RegexAutomaton RegexAutomaton;
typedef struct TokenNode    TokenNode;
typedef struct IRGenerator  IRGenerator;

/* ── State ───────────────────────────────────────────────────────────────── */

struct State {
    char   *pattern;    /* POSIX extended regex that names this class */
    bool    is_final;   /* true = accepting state */
    size_t  id;         /* monotone unique identity */
};

static size_t g_next_id = 1;
static size_t generate_id(void) { return g_next_id++; }

State *
state_create(const char *pattern, bool is_final)
{
    State *s = malloc(sizeof *s);
    if (!s) return NULL;

    s->pattern  = strdup(pattern);
    if (!s->pattern) { free(s); return NULL; }

    s->is_final = is_final;
    s->id       = generate_id();
    return s;
}

void
state_destroy(State *s)
{
    if (!s) return;
    free(s->pattern);
    free(s);
}

bool
state_matches(const State *s, const char *text)
{
    if (!s || !text) return false;

    regex_t rx;
    if (regcomp(&rx, s->pattern, REG_EXTENDED) != 0) return false;

    int rc = regexec(&rx, text, 0, NULL, 0);
    regfree(&rx);
    return (rc == 0);
}

/* ── Transition ──────────────────────────────────────────────────────────── */

struct Transition {
    State *from_state;
    char  *input_pattern;   /* regex on the arc label */
    State *to_state;
};

/* ── RegexAutomaton ──────────────────────────────────────────────────────── */

struct RegexAutomaton {
    State      **states;
    size_t       state_count;
    size_t       state_capacity;

    Transition **transitions;
    size_t       transition_count;
    size_t       transition_capacity;

    State *initial_state;
    State *current_state;
};

RegexAutomaton *
automaton_create(void)
{
    RegexAutomaton *a = malloc(sizeof *a);
    if (!a) return NULL;

    a->state_capacity = 10;
    a->states = malloc(sizeof(State *) * a->state_capacity);
    if (!a->states) { free(a); return NULL; }

    a->transition_capacity = 20;
    a->transitions = malloc(sizeof(Transition *) * a->transition_capacity);
    if (!a->transitions) { free(a->states); free(a); return NULL; }

    a->state_count      = 0;
    a->transition_count = 0;
    a->initial_state    = NULL;
    a->current_state    = NULL;
    return a;
}

void
automaton_destroy(RegexAutomaton *a)
{
    if (!a) return;
    for (size_t i = 0; i < a->state_count; i++)
        state_destroy(a->states[i]);
    free(a->states);

    for (size_t i = 0; i < a->transition_count; i++) {
        free(a->transitions[i]->input_pattern);
        free(a->transitions[i]);
    }
    free(a->transitions);
    free(a);
}

/**
 * automaton_add_state — register a new state in the automaton.
 *
 * Capacity is doubled on overflow — O(1) amortised — so callers can
 * register states dynamically (e.g. from macro expansion).
 * The first registered state automatically becomes the initial state.
 */
State *
automaton_add_state(RegexAutomaton *a, const char *pattern, bool is_final)
{
    if (!a || !pattern) return NULL;

    if (a->state_count >= a->state_capacity) {
        size_t  nc  = a->state_capacity * 2;
        State **ns  = realloc(a->states, sizeof(State *) * nc);
        if (!ns) return NULL;
        a->states         = ns;
        a->state_capacity = nc;
    }

    State *s = state_create(pattern, is_final);
    if (!s) return NULL;

    a->states[a->state_count++] = s;

    if (!a->initial_state) {
        a->initial_state = s;
        a->current_state = s;
    }
    return s;
}

/**
 * automaton_add_transition — add a labelled directed edge.
 *
 * The `pattern` on the transition edge is the regex that must match for the
 * automaton to follow this arc. In a classical DFA every arc is mutually
 * exclusive; in RIFT's pattern layer arcs can overlap — the first match wins,
 * matching the behaviour of PEG parsers.
 */
bool
automaton_add_transition(RegexAutomaton *a, State *from,
                         const char *pattern, State *to)
{
    if (!a || !from || !pattern || !to) return false;

    if (a->transition_count >= a->transition_capacity) {
        size_t       nc = a->transition_capacity * 2;
        Transition **nt = realloc(a->transitions, sizeof(Transition *) * nc);
        if (!nt) return false;
        a->transitions         = nt;
        a->transition_capacity = nc;
    }

    Transition *t = malloc(sizeof *t);
    if (!t) return false;

    t->from_state    = from;
    t->input_pattern = strdup(pattern);
    if (!t->input_pattern) { free(t); return false; }
    t->to_state = to;

    a->transitions[a->transition_count++] = t;
    return true;
}

/**
 * automaton_get_next_state — advance the automaton on `input`.
 *
 * Scans all registered states for the first pattern match.
 * In a production implementation this would use a precomputed transition
 * table keyed by (current_state, input_class) for O(1) lookup.
 */
State *
automaton_get_next_state(RegexAutomaton *a, const char *input)
{
    if (!a || !a->current_state || !input) return NULL;

    for (size_t i = 0; i < a->state_count; i++) {
        if (state_matches(a->states[i], input)) {
            a->current_state = a->states[i];
            return a->states[i];
        }
    }
    return NULL;
}

/* ── TokenNode (IR leaf) ─────────────────────────────────────────────────── */

struct TokenNode {
    char *type;    /* canonical type — the matched state's pattern */
    char *value;   /* raw lexeme from source */
};

/* ── IRGenerator ─────────────────────────────────────────────────────────── */

struct IRGenerator {
    RegexAutomaton *automaton;
    TokenNode     **nodes;
    size_t          node_count;
    size_t          node_capacity;
};

IRGenerator *
ir_generator_create(RegexAutomaton *automaton)
{
    if (!automaton) return NULL;

    IRGenerator *g = malloc(sizeof *g);
    if (!g) return NULL;

    g->automaton     = automaton;
    g->node_capacity = 10;
    g->nodes         = malloc(sizeof(TokenNode *) * g->node_capacity);
    if (!g->nodes) { free(g); return NULL; }

    g->node_count = 0;
    return g;
}

void
ir_generator_destroy(IRGenerator *g)
{
    if (!g) return;
    for (size_t i = 0; i < g->node_count; i++) {
        free(g->nodes[i]->type);
        free(g->nodes[i]->value);
        free(g->nodes[i]);
    }
    free(g->nodes);
    free(g);
}

/**
 * ir_generator_process_token — core translation step.
 *
 * Drives the automaton forward on `token`, then stamps a new TokenNode
 * with:
 *   .type  = the matched state's pattern (canonical class name)
 *   .value = the raw input lexeme
 *
 * This node is the atomic unit fed into the parser. Because it carries both
 * the structured type AND the raw text, every downstream stage can either
 * reason about the type or inspect the original source spelling.
 */
TokenNode *
ir_generator_process_token(IRGenerator *g, const char *token)
{
    if (!g || !token) return NULL;

    State *s = automaton_get_next_state(g->automaton, token);
    if (!s) return NULL;

    TokenNode *node = malloc(sizeof *node);
    if (!node) return NULL;

    node->type  = strdup(s->pattern);
    node->value = strdup(token);

    if (!node->type || !node->value) {
        free(node->type);
        free(node->value);
        free(node);
        return NULL;
    }
    return node;
}

/* ── Demo ────────────────────────────────────────────────────────────────── */

void
create_simple_lexer(void)
{
    RegexAutomaton *a = automaton_create();
    if (!a) { fputs("OOM\n", stderr); return; }

    /* Register token classes */
    State *identifier = automaton_add_state(a, "^[a-zA-Z_][a-zA-Z0-9_]*$", false);
    State *number     = automaton_add_state(a, "^[0-9]+$",                  false);
    State *op         = automaton_add_state(a, "^[-+*/]$",                  false);
    State *ws         = automaton_add_state(a, "^[ \\t\\n]+$",              false);

    if (!identifier || !number || !op || !ws) {
        fputs("Failed to create states\n", stderr);
        automaton_destroy(a);
        return;
    }

    /*
     * Transitions are optional in this model: the get_next_state function
     * does a pattern scan rather than a table lookup, so explicit arc
     * registration is only required when you want to enforce sequencing
     * (e.g. "an operator must follow an identifier or number, never another
     * operator"). For this demo we omit sequencing constraints.
     */
    (void)automaton_add_transition(a, identifier, "^[-+*/]$", op);
    (void)automaton_add_transition(a, number,     "^[-+*/]$", op);

    IRGenerator *g = ir_generator_create(a);
    if (!g) {
        fputs("Failed to create IR generator\n", stderr);
        automaton_destroy(a);
        return;
    }

    /* Feed a simple expression token stream */
    const char *tokens[]  = {"x", "+", "123", "*", "y"};
    size_t      n_tokens  = sizeof tokens / sizeof *tokens;

    printf("RIFT IR Generator — token stream for: x + 123 * y\n");
    printf("─────────────────────────────────────────────────\n\n");
    printf("%-8s  %-10s  %s\n", "Token", "Type", "State ID");
    printf("%-8s  %-10s  %s\n", "──────", "────────", "────────");

    for (size_t i = 0; i < n_tokens; i++) {
        TokenNode *node = ir_generator_process_token(g, tokens[i]);
        if (node) {
            /* Derive a human-readable type from the pattern */
            const char *type = "unknown";
            if (strstr(node->type, "[a-zA-Z_]")) type = "Identifier";
            else if (strstr(node->type, "[0-9]"))    type = "Number";
            else if (strstr(node->type, "[-+"))       type = "Operator";

            printf("%-8s  %-10s  pattern=%s\n", node->value, type, node->type);
            free(node->type);
            free(node->value);
            free(node);
        } else {
            printf("%-8s  %-10s\n", tokens[i], "UNMATCHED");
        }
    }

    printf("\nIR generation complete. %zu tokens processed.\n", n_tokens);

    ir_generator_destroy(g);
    automaton_destroy(a);
}

int
main(void)
{
    create_simple_lexer();
    return 0;
}
