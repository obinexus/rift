#include <stdio.h>
#include <string.h>
#include "rift/cli.h"
#include "rift/lexer.h"
#include <stdlib.h>

int rift_cmd_tokenize(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: rift tokenize [options] <input-file>\n\n");
        printf("Tokenize a RIFT source file and output the token stream.\n\n");
        printf("Options:\n");
        printf("  --format <f>  Output format (text, json, csv)\n");
        printf("  --help        Show this help message\n");
        return 0;
    }

    FILE *stream = fopen(argv[1], "rb");
    long size;
    char *source;
    rift_lexer_t *lexer;
    if (!stream) {
        fprintf(stderr, "rift tokenize: cannot open '%s'\n", argv[1]);
        return 1;
    }
    fseek(stream, 0, SEEK_END);
    size = ftell(stream);
    fseek(stream, 0, SEEK_SET);
    if (size < 0 || !(source = (char *)malloc((size_t)size + 1))) {
        fclose(stream);
        return 1;
    }
    if (fread(source, 1, (size_t)size, stream) != (size_t)size) {
        fclose(stream);
        free(source);
        return 1;
    }
    fclose(stream);
    source[size] = '\0';
    lexer = rift_lexer_create(source, (size_t)size);
    if (!lexer) {
        free(source);
        return 1;
    }
    for (;;) {
        rift_token_t token = rift_lexer_next(lexer);
        printf("%u:%u %-16s %s\n", token.row, token.column,
               rift_token_type_name(token.type),
               token.lexeme ? token.lexeme : "");
        if (token.type == RIFT_TOKEN_EOF) {
            rift_token_destroy(&token);
            break;
        }
        rift_token_destroy(&token);
    }
    rift_lexer_destroy(lexer);
    free(source);
    return 0;
}
