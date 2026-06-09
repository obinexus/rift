#include "rift/rift.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_librift_initialized;

rift_context_t *rift_init(void) {
    rift_context_t *ctx = (rift_context_t *)calloc(1, sizeof(rift_context_t));
    if (!ctx) return NULL;

    ctx->last_error = RIFT_SUCCESS;
    return ctx;
}

void rift_cleanup(rift_context_t *ctx) {
    if (!ctx) return;

    if (ctx->lexer)    rift_lexer_destroy(ctx->lexer);
    if (ctx->parser)   rift_parser_destroy(ctx->parser);
    if (ctx->semantic) rift_semantic_destroy(ctx->semantic);
    if (ctx->codegen)  rift_codegen_destroy(ctx->codegen);
    if (ctx->emitter)  rift_emitter_destroy(ctx->emitter);
    if (ctx->bridge)   rift_bridge_destroy(ctx->bridge);
    if (ctx->pattern)  rift_pattern_destroy(ctx->pattern);

    free(ctx);
}

rift_error_t rift_compile(rift_context_t *ctx,
                           const char *input_path,
                           const char *output_path) {
    FILE *input;
    long file_size;
    char *source = NULL;
    rift_token_t *tokens = NULL;
    size_t token_count = 0;
    size_t token_capacity = 0;
    RIFTSemanticMatrix *matrix = NULL;
    RIFTBridgeResult bridge_result;
    rift_ast_node_t *ast = NULL;
    rift_cir_program_t program;
    char *default_output = NULL;
    const char *target_output = output_path;
    rift_error_t error = RIFT_SUCCESS;

    memset(&bridge_result, 0, sizeof(bridge_result));
    if (!ctx || !input_path) return RIFT_ERROR_INVALID_ARGUMENT;
    input = fopen(input_path, "rb");
    if (!input) return RIFT_ERROR_FILE_ACCESS;
    if (fseek(input, 0, SEEK_END) != 0 ||
        (file_size = ftell(input)) < 0 ||
        fseek(input, 0, SEEK_SET) != 0) {
        fclose(input);
        return RIFT_ERROR_FILE_ACCESS;
    }
    source = (char *)malloc((size_t)file_size + 1);
    if (!source) {
        fclose(input);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    if (fread(source, 1, (size_t)file_size, input) != (size_t)file_size) {
        fclose(input);
        free(source);
        return RIFT_ERROR_FILE_ACCESS;
    }
    fclose(input);
    source[file_size] = '\0';

    if (ctx->verbose) printf("rift: tokenize %s\n", input_path);
    ctx->lexer = rift_lexer_create(source, (size_t)file_size);
    if (!ctx->lexer) {
        error = RIFT_ERROR_MEMORY_ALLOCATION;
        goto cleanup;
    }
    for (;;) {
        rift_token_t token = rift_lexer_next(ctx->lexer);
        if (token_count == token_capacity) {
            size_t next_capacity = token_capacity ? token_capacity * 2 : 32;
            rift_token_t *next = (rift_token_t *)realloc(
                tokens, next_capacity * sizeof(*tokens));
            if (!next) {
                rift_token_destroy(&token);
                error = RIFT_ERROR_MEMORY_ALLOCATION;
                goto cleanup;
            }
            tokens = next;
            token_capacity = next_capacity;
        }
        tokens[token_count++] = token;
        if (token.type == RIFT_TOKEN_EOF) break;
    }

    if (ctx->verbose) printf("rift: confidence bridge (%u tokens)\n",
                             (unsigned)token_count);
    matrix = rift_semantic_matrix_create(tokens, token_count, 0.70);
    if (!matrix || rift_bridge_traverse(matrix, &bridge_result) != 0) {
        error = RIFT_ERROR_PARSE_FAILURE;
        goto cleanup;
    }

    ctx->parser = rift_parser_create(tokens, token_count);
    ast = rift_parser_parse(ctx->parser);
    if (!ast) {
        error = RIFT_ERROR_PARSE_FAILURE;
        goto cleanup;
    }
    ctx->semantic = rift_semantic_create();
    if (!ctx->semantic || rift_semantic_analyze(ctx->semantic, ast) != 0) {
        error = RIFT_ERROR_SEMANTIC_FAILURE;
        goto cleanup;
    }
    if (rift_link(source, (size_t)file_size, &program) != 0) {
        error = RIFT_ERROR_LINK_FAILURE;
        goto cleanup;
    }

    if (!target_output) {
        size_t length = strlen(input_path);
        default_output = (char *)malloc(length + 3);
        if (!default_output) {
            error = RIFT_ERROR_MEMORY_ALLOCATION;
            goto cleanup;
        }
        memcpy(default_output, input_path, length);
        memcpy(default_output + length, ".c", 3);
        target_output = default_output;
    }
    if (ctx->verbose) printf("rift: emit C -> %s\n", target_output);
    if (rift_codec_emit(&program, "c", target_output) != 0) {
        error = RIFT_ERROR_EMIT_FAILURE;
        goto cleanup;
    }

cleanup:
    ctx->last_error = error;
    free(default_output);
    rift_ast_free(ast);
    rift_bridge_result_destroy(&bridge_result);
    rift_semantic_matrix_destroy(matrix);
    if (ctx->parser) {
        rift_parser_destroy(ctx->parser);
        ctx->parser = NULL;
    }
    if (ctx->lexer) {
        rift_lexer_destroy(ctx->lexer);
        ctx->lexer = NULL;
    }
    if (ctx->semantic) {
        rift_semantic_destroy(ctx->semantic);
        ctx->semantic = NULL;
    }
    if (tokens) {
        size_t i;
        for (i = 0; i < token_count; i++) rift_token_destroy(&tokens[i]);
    }
    free(tokens);
    free(source);
    return error;
}

static const char *error_strings[] = {
    "success",
    "invalid argument",
    "memory allocation failed",
    "file access error",
    "parse failure",
    "semantic analysis failure",
    "code generation failure",
    "emission failure",
    "link failure",
    "unsupported operation",
    "unknown error"
};

const char *rift_error_string(rift_error_t err) {
    if (err < 0 || err > RIFT_ERROR_UNKNOWN) return "invalid error code";
    return error_strings[err];
}

int librift_initialize(void) {
    g_librift_initialized = 1;
    return 1;
}

void librift_cleanup(void) {
    g_librift_initialized = 0;
}

int librift_check_version(void) {
    return g_librift_initialized;
}

const char *librift_get_version_string(void) {
    return RIFT_VERSION_STRING;
}
