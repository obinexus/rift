# LibRift Command-Line Interface for Multi-Pass Processing

Here's a demonstration of how the LibRift command-line interface would be used for multi-pass file processing, leveraging the architecture from the UML diagram.

## Command Structure

The LibRift CLI provides command sequences for defining and executing multi-pass compiler pipelines:

```bash
rift compile [options] <input_file> --output <output_file>
```

## Multi-Pass Pipeline Configuration

For a multi-pass pipeline, we would use a configuration file (e.g., `pipeline.json`) that defines the phases and their dependencies:

```json
{
  "name": "compiler",
  "passes": [
    {
      "id": "tokenize",
      "type": "tokenizer",
      "config": {
        "rules": [
          { "pattern": "\\bif\\b", "type": "KEYWORD_IF" },
          { "pattern": "\\belse\\b", "type": "KEYWORD_ELSE" },
          { "pattern": "\\bfunction\\b", "type": "KEYWORD_FUNCTION" },
          { "pattern": "[a-zA-Z_][a-zA-Z0-9_]*", "type": "IDENTIFIER" },
          { "pattern": "\\d+", "type": "NUMBER" },
          { "pattern": "\"[^\"]*\"", "type": "STRING" },
          { "pattern": "\\{|\\}|\\(|\\)|;|,|\\.|=|\\+|-|\\*|/", "type": "SYMBOL" }
        ],
        "ignore": "\\s+|//.*$|/\\*[\\s\\S]*?\\*/"
      }
    },
    {
      "id": "parse",
      "type": "parser",
      "depends": ["tokenize"],
      "config": {
        "grammar": {
          "program": "statement*",
          "statement": "expression_stmt | if_stmt | function_decl",
          "expression_stmt": "expression ';'",
          "if_stmt": "KEYWORD_IF '(' expression ')' statement (KEYWORD_ELSE statement)?",
          "function_decl": "KEYWORD_FUNCTION IDENTIFIER '(' parameter_list? ')' block",
          "expression": "assignment",
          "assignment": "identifier '=' assignment | equality",
          "equality": "comparison (('==' | '!=') comparison)*"
        },
        "start": "program"
      }
    },
    {
      "id": "ast_build",
      "type": "ast_builder",
      "depends": ["parse"],
      "config": {
        "visitors": {
          "if_stmt": "IfStatementVisitor",
          "function_decl": "FunctionDeclVisitor",
          "expression": "ExpressionVisitor"
        }
      }
    },
    {
      "id": "semantic_analysis",
      "type": "transformer",
      "depends": ["ast_build"],
      "config": {
        "transformations": [
          "SymbolTableBuilder",
          "TypeChecker",
          "ConstantFolder"
        ]
      }
    },
    {
      "id": "optimizer",
      "type": "transformer",
      "depends": ["semantic_analysis"],
      "config": {
        "transformations": [
          "DeadCodeElimination",
          "ConstantPropagation",
          "InlineExpansion"
        ]
      }
    },
    {
      "id": "code_gen",
      "type": "code_generator",
      "depends": ["optimizer"],
      "config": {
        "target": "js",
        "module_type": "esm"
      }
    }
  ],
  "feedback_loops": [
    {
      "from": "semantic_analysis",
      "to": "tokenize",
      "condition": "macro_expansion_needed"
    },
    {
      "from": "optimizer",
      "to": "ast_build",
      "condition": "restructuring_needed"
    }
  ]
}
```

## CLI Usage Examples

### Basic Compilation

```bash
rift compile source.rft --config pipeline.json --output out.js
```

### Executing Specific Passes

```bash
# Run only tokenization and parsing
rift compile source.rft --config pipeline.json --passes tokenize,parse --output parsed.json

# Run from parsing through optimization (skips tokenization)
rift compile source.rft --config pipeline.json --from parse --to optimizer --input-tokens tokens.json --output optimized.json
```

### Debugging a Multi-Pass Pipeline

```bash
# Enable verbose output for debugging
rift compile source.rft --config pipeline.json --verbose --output out.js

# Output intermediate results for each pass
rift compile source.rft --config pipeline.json --dump-intermediate --output-dir ./build
```

### Visualizing the Pipeline

```bash
# Generate a visualization of the pipeline and its dependencies
rift visualize-pipeline --config pipeline.json --output pipeline-graph.svg

# Trace the execution of the pipeline with a specific input
rift trace source.rft --config pipeline.json --output trace.log
```

## Implementation of a Multi-Pass Command

Here's how a command would be implemented to handle the multi-pass processing:

```typescript
class RiftCompileCommand implements Command {
  constructor(private client: RiftClient) {}

  async execute(args: string[], options: CommandOptions): Promise<number> {
    const inputFile = args[0];
    const configPath = options.config || 'pipeline.json';
    const outputFile = options.output;
    
    // Load configuration
    const config = await this.loadConfiguration(configPath);
    
    // Create pipeline
    const pipeline = this.client.createPipeline(config.name);
    
    // Configure pipeline components based on config
    this.configurePipeline(pipeline, config);
    
    // Set up feedback loops for multi-pass processing
    this.setupFeedbackLoops(pipeline, config.feedback_loops);
    
    // Read input file
    const input = await fs.readFile(inputFile, 'utf8');
    
    // Execute pipeline
    const result = await pipeline.process(input);
    
    // Handle errors if any
    if (result.hasErrors()) {
      this.reportErrors(result.getErrors());
      return 1;
    }
    
    // Write output
    if (outputFile) {
      const output = this.generateOutput(result, config.passes);
      await fs.writeFile(outputFile, output);
    }
    
    return 0;
  }
  
  private configurePipeline(pipeline: Pipeline, config: PipelineConfig): void {
    // Create and configure components based on the pipeline configuration
    const components = new Map<string, Component>();
    
    // Create components first
    for (const pass of config.passes) {
      const component = this.createComponent(pass.type, pass.config);
      components.set(pass.id, component);
    }
    
    // Build dependency graph and configure pipeline
    for (const pass of config.passes) {
      const component = components.get(pass.id);
      
      // Link dependencies
      if (pass.depends) {
        for (const depId of pass.depends) {
          const dependency = components.get(depId);
          component.linkDependency(dependency);
        }
      }
      
      // Add to pipeline
      pipeline.addComponent(pass.id, component);
    }
  }
  
  private setupFeedbackLoops(pipeline: Pipeline, feedbackLoops: FeedbackLoopConfig[]): void {
    for (const loop of feedbackLoops) {
      pipeline.addFeedbackLoop(loop.from, loop.to, loop.condition);
    }
  }
  
  // Other helper methods...
}
```

## Example Command Session

Here's an example session showing how the CLI would be used for an iterative development process:

```bash
# Initial tokenization to verify lexical structure
$ rift compile src/main.rft --config pipelines/lang.json --passes tokenize --dump-tokens
[INFO] Tokenization completed. Found 1420 tokens.
[INFO] Tokens dumped to 'tokens.json'

# Run parsing to verify grammar
$ rift compile src/main.rft --config pipelines/lang.json --passes tokenize,parse --dump-parse-tree
[INFO] Tokenization completed. Found 1420 tokens.
[INFO] Parsing completed. Parse tree has 842 nodes.
[INFO] Parse tree dumped to 'parse_tree.json'
[WARNING] 3 parser warnings found. See 'parse_warnings.log'

# Fix the grammar and run again
$ rift compile src/main.rft --config pipelines/lang_updated.json --passes tokenize,parse
[INFO] Tokenization completed. Found 1420 tokens.
[INFO] Parsing completed. Parse tree has 845 nodes.
[INFO] No parser warnings.

# Full compilation with intermediate results
$ rift compile src/main.rft --config pipelines/lang_updated.json --dump-intermediate --output-dir ./build
[INFO] Tokenization completed.
[INFO] Parsing completed.
[INFO] AST building completed.
[INFO] Semantic analysis completed.
[INFO] Optimization completed.
[INFO] Code generation completed.
[INFO] Output written to './build/main.js'
[INFO] Intermediate results written to './build/intermediate/'
```

This CLI design leverages the flexible client architecture shown in the UML diagram, providing a comprehensive command interface for language development that's both accessible and powerful. The multi-pass approach supports complex language processing tasks while maintaining a clear and consistent interface.