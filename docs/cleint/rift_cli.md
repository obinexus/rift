# LibRift CLI System Design Plan

Based on the project structure and the architectural diagram shown, I'll develop a comprehensive CLI system plan for LibRift that supports both single-pass and multi-pass compiler workflows.

## Core CLI Architecture

The CLI architecture should follow a modular command pattern that maps closely to the components in the client architecture diagram while providing a consistent interface for users. The system will support both immediate operations and pipeline construction.

```
rift
├── commands/
│   ├── tokenize.c       # Tokenization commands
│   ├── parse.c          # Parsing commands
│   ├── ast.c            # AST manipulation commands
│   ├── optimize.c       # Optimization commands
│   ├── bytecode.c       # Bytecode generation commands
│   └── pipeline.c       # Pipeline configuration commands
├── core/
│   ├── command.c        # Command infrastructure
│   ├── config.c         # Configuration management
│   └── pipeline.c       # Pipeline execution engine
└── main.c               # Entry point
```

## Command Module Structure

Each command module will follow a consistent pattern:

1. Command registration function
2. Command execution function
3. Command help/usage information
4. Option parsing logic
5. Result formatting logic

## Phase-Specific Commands

### 1. Tokenizer Phase

```
rift tokenize [options] <input-file>
  --output, -o <file>       Output tokens to file
  --format <json|text>      Output format (default: text)
  --rules <file>            Custom tokenization rules
  --ignore <pattern>        Pattern to ignore during tokenization
  --case-sensitive          Enable case sensitivity
  --debug                   Show token positions and types
```

### 2. Parser Phase

```
rift parse [options] <input-file|token-file>
  --output, -o <file>       Output parse tree to file
  --format <json|dot|text>  Output format (default: text)
  --grammar <file>          Custom grammar definition
  --start <symbol>          Set start symbol
  --tokens <file>           Use tokenized input from file
  --debug                   Show parser states and decisions
  --recovery                Enable error recovery
```

### 3. AST Builder Phase

```
rift ast [options] <input-file|parse-tree-file>
  --output, -o <file>       Output AST to file
  --format <json|dot|text>  Output format (default: text)
  --transform <file>        Apply transformations from file
  --optimize                Apply AST optimizations
  --parse-tree <file>       Use parse tree from file
  --visualize               Generate graphical AST view
```

### 4. Bytecode Generation Phase

```
rift bytecode [options] <input-file|ast-file>
  --output, -o <file>       Output bytecode to file
  --format <binary|text>    Output format (default: binary)
  --target <platform>       Target platform
  --opt-level <0-3>         Optimization level
  --ast <file>              Use AST from file
  --debug-info              Include debug information
```

### 5. Pipeline Configuration

```
rift pipeline [options] <input-file>
  --config, -c <file>       Pipeline configuration file
  --output, -o <file>       Final output file
  --temp-dir <dir>          Directory for intermediate files
  --steps <steps>           Comma-separated pipeline steps
  --skip <steps>            Steps to skip
  --debug <steps>           Steps to run in debug mode
  --visualize <steps>       Steps to visualize
```

### 6. Multi-Pass Integration

```
rift project [options] <project-dir>
  --init                    Initialize project structure
  --build                   Build the entire project
  --analyze                 Analyze project dependencies
  --compile <file>          Compile a specific file
  --watch                   Watch for changes and recompile
  --passes <n>              Number of compilation passes
```

## Integration UML Diagram

```
                  +----------------+
                  |     RiftCLI    |
                  +----------------+
                         |
                         v
        +--------------------------------+
        |        CommandDispatcher       |
        +--------------------------------+
          |        |        |        |
          v        v        v        v
   +----------+ +------+ +-----+ +----------+
   | Tokenizer| |Parser| | AST | | Bytecode |
   +----------+ +------+ +-----+ +----------+
          |        |        |        |
          v        v        v        v
   +----------+ +------+ +-----+ +----------+
   |  Token   | | Tree | | Node | |   Code   |
   +----------+ +------+ +-----+ +----------+
                         |
                         v
                  +-------------+
                  | PipelineExec|
                  +-------------+
                         |
                         v
                  +-------------+
                  |MultiPassIntg|
                  +-------------+
```

## Implementation Strategy

1. **Command Registration System**: Implement a flexible command registration system that allows new commands to be easily added without modifying the core CLI code.

2. **Output Format Adapters**: Create adapters for different output formats (text, JSON, DOT diagrams) that can be plugged into any command.

3. **Pipeline Configuration**: Develop a configuration format that allows users to define custom pipelines with specific options for each stage.

4. **Intermediate Storage**: Implement a system for managing intermediate compilation artifacts between pipeline stages.

5. **Error Reporting**: Create a consistent error reporting mechanism that works across all compilation phases.

6. **Progress Tracking**: Add support for progress tracking during long-running multi-pass compilations.

## Example Usage Scenarios

### Single-Pass Compilation
```bash
# Direct compilation (auto-pipeline)
rift compile source.rf -o output.exe

# Manual pipeline with visualization
rift tokenize source.rf -o tokens.json
rift parse tokens.json -o parse.json --visualize
rift ast parse.json -o ast.json --optimize
rift bytecode ast.json -o output.bin
```

### Multi-Pass Development
```bash
# Initialize project
rift project --init myproject

# Define custom pipeline
rift pipeline --config pipeline.json source.rf -o output.bin

# Multi-pass compilation with intermediate validation
rift project --build --passes 3 --validate-after 1,2
```

