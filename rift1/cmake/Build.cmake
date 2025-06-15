# Core tokenizer components
target_link_libraries(rift1_tokenizer 
    rift1_lexer
    rift1_token_types
    rift1_token_values
    rift1_token_memory
)

# Parser integration
target_link_libraries(rift1_parser
    rift1_tokenizer
    rift1_ast
)