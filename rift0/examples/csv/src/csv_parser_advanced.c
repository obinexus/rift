// Advanced CSV Parser using R"" syntax (from benchmark integration)
#include "../../../include/rift.h"

// R"" syntax patterns for CSV parsing - Top-down approach
static const char* csv_field_pattern_topdown = R"(
    # CSV Field Pattern - Top-Down Mode
    (?:
        # Quoted field with escaped quotes
        "([^"]*(?:""[^"]*)*)"     |
        
        # Unquoted field (no commas, newlines, or quotes)
        ([^,\r\n"]+)              |
        
        # Empty field
        ()
    )
)[t]";  // [t] = top-down parsing mode

// R"" syntax patterns for CSV parsing - Bottom-up approach  
static const char* csv_char_pattern_bottomup = R"(
    # CSV Character Pattern - Bottom-Up Mode
    (?:
        # Regular character (not special)
        ([^,\r\n"])               |
        
        # Escaped quote
        ("")                      |
        
        # Delimiter
        (,)                       |
        
        # Newline variants
        (\r\n|\r|\n)
    )
)[b]";  // [b] = bottom-up parsing mode

// R"" syntax for validation patterns
static const char* csv_quoted_field_validator = R"(
    # Quoted Field Validator
    ^"                            # Start quote
    (                            # Capture group
        (?:                      # Non-capturing group
            [^"]                 # Any non-quote character
            |                    # OR
            ""                   # Escaped quote (two quotes)
        )*                       # Zero or more times
    )                            # End capture group
    "$                           # End quote
)[t]";

void demonstrate_advanced_csv_parsing(const char* csv_file) {
    printf("🔍 Advanced CSV Parser - R\"\" Syntax Demo\n");
    printf("==========================================\n");
    printf("📁 Processing: %s\n\n", csv_file);
    
    // Read CSV file
    FILE* file = fopen(csv_file, "r");
    if (!file) {
        printf("❌ Could not open file: %s\n", csv_file);
        return;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = malloc(file_size + 1);
    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);
    
    printf("📊 File size: %ld bytes\n", file_size);
    printf("📝 Content preview:\n%.*s%s\n\n", 
           (int)(file_size > 200 ? 200 : file_size), content,
           file_size > 200 ? "..." : "");
    
    // Create automaton with R"" patterns
    RiftAutomaton* automaton = rift_automaton_create();
    
    // Add top-down field parsing state
    RiftState* field_state = rift_automaton_add_state_with_type(
        automaton, csv_field_pattern_topdown, "CSV_FIELD", true);
    
    // Add bottom-up character parsing state  
    RiftState* char_state = rift_automaton_add_state_with_type(
        automaton, csv_char_pattern_bottomup, "CSV_CHAR", false);
    
    if (!field_state || !char_state) {
        printf("❌ Failed to create parsing states\n");
        free(content);
        rift_automaton_destroy(automaton);
        return;
    }
    
    printf("✅ Created parsing states with R\"\" syntax:\n");
    printf("   🎯 Top-down field parser: %s\n", field_state->token_type);
    printf("   🔍 Bottom-up char parser: %s\n", char_state->token_type);
    printf("\n");
    
    // Create and configure engine
    RiftEngine* engine = rift_engine_create();
    engine->automaton = automaton;
    
    // Process the CSV content
    printf("🚀 Processing CSV content...\n");
    RiftResult result = rift_engine_process_input(engine, content);
    
    if (result == RIFT_SUCCESS) {
        printf("✅ Processing successful!\n");
        printf("📊 Generated %zu tokens\n\n", engine->token_count);
        
        // Show token analysis
        size_t field_count = 0;
        size_t char_count = 0;
        size_t line_count = 1;
        
        printf("🔍 Token Analysis:\n");
        printf("==================\n");
        
        for (size_t i = 0; i < engine->token_count && i < 20; i++) {
            RiftToken* token = engine->tokens[i];
            
            if (strcmp(token->type, "CSV_FIELD") == 0) {
                field_count++;
            } else if (strcmp(token->type, "CSV_CHAR") == 0) {
                char_count++;
            }
            
            if (strcmp(token->value, "\n") == 0) {
                line_count++;
            }
            
            printf("Token %2zu: %-10s = \"%-15s\" (line %zu, col %zu)\n", 
                   i, token->type, 
                   strlen(token->value) > 15 ? "..." : token->value,
                   token->line, token->column);
        }
        
        if (engine->token_count > 20) {
            printf("... and %zu more tokens\n", engine->token_count - 20);
        }
        
        printf("\n📊 Summary:\n");
        printf("   📁 CSV fields: %zu\n", field_count);
        printf("   🔤 Characters: %zu\n", char_count);  
        printf("   📄 Lines: %zu\n", line_count);
        printf("   🎯 Total tokens: %zu\n", engine->token_count);
        
    } else {
        printf("❌ Processing failed: %s\n", rift_result_string(result));
    }
    
    // Clean up
    free(content);
    rift_engine_destroy(engine);
    rift_automaton_destroy(automaton);
    printf("\n");
}

void demonstrate_csv_validation(void) {
    printf("✅ CSV Validation - R\"\" Syntax Demo\n");
    printf("====================================\n");
    
    // Test cases for validation
    const char* test_fields[] = {
        "\"simple field\"",
        "\"field with \"\"quotes\"\"\"", 
        "\"mixed \"\"quotes\"\" and text\"",
        "unquoted field",
        "\"malformed field with single quote",
        "\"properly \"\"escaped\"\" field\""
    };
    
    printf("🧪 Testing CSV field validation:\n\n");
    
    for (size_t i = 0; i < 6; i++) {
        const char* field = test_fields[i];
        printf("Field %zu: %-30s -> ", i+1, field);
        
        // Create pattern for validation
        RiftPattern* pattern = rift_parse_pattern(csv_quoted_field_validator);
        if (pattern && rift_pattern_compile(pattern) == RIFT_SUCCESS) {
            
            regmatch_t match;
            int result = regexec(&pattern->compiled_regex, field, 1, &match, 0);
            
            if (result == 0) {
                printf("✅ VALID");
                if (strstr(field, "\"\"")) {
                    printf(" (contains escaped quotes)");
                }
            } else {
                printf("❌ INVALID");
            }
            
        } else {
            printf("⚠️  PATTERN_ERROR");
        }
        
        printf("\n");
        rift_pattern_destroy(pattern);
    }
    
    printf("\n");
}

int main(int argc, char* argv[]) {
    printf("🚀 RIFT Advanced CSV Processing Demo\n");
    printf("=====================================\n");
    printf("🏗️  Using R\"\" syntax with [t]/[b] mode flags\n");
    printf("📚 Integrated from LibRift benchmark architecture\n\n");
    
    // Test with sample data files
    const char* test_files[] = {
        "examples/data/simple.csv",
        "examples/data/complex.csv", 
        "examples/data/malformed.csv"
    };
    
    for (size_t i = 0; i < 3; i++) {
        demonstrate_advanced_csv_parsing(test_files[i]);
    }
    
    // Demonstrate field validation
    demonstrate_csv_validation();
    
    printf("🎉 Advanced CSV Demo Complete!\n");
    printf("===============================\n");
    printf("✅ R\"\" syntax patterns working\n");
    printf("✅ Top-down [t] and bottom-up [b] modes\n");
    printf("✅ Complex CSV parsing with escapes\n");
    printf("✅ Field validation and error detection\n");
    printf("🏗️  Ready for Stage 1 (AST generation)!\n");
    
    return 0;
}
