```mermaid
classDiagram
    class BytecodeProgram {
        +instructions: BytecodeInstruction[]
        +instruction_count: uint32_t
        +capacity: uint32_t
        +group_count: uint32_t
        +flags: regex_flags_t
        +original_pattern: char*
        +char_class_map: uint32_t*
        +serialize() bool
        +deserialize() bool
        +execute() bool
    }
    
    class BytecodeInstruction {
        +opcode: bytecode_opcode_t
        +operand: union
        +execute(VM) bool
    }
    
    class BytecodeVM {
        +input: const char*
        +input_length: uint32_t
        +current_pos: uint32_t
        +captures: uint32_t*
        +capture_count: uint32_t
        +backtrack_stack: uint32_t*
        +stack_size: uint32_t
        +stack_capacity: uint32_t
        +timed_out: bool
        +max_instructions: uint64_t
        +instruction_counter: uint64_t
        +execute(program) bool
        +reset() void
    }
    
    class BytecodeCompiler {
        +compile(pattern) BytecodeProgram*
        +compile_ast(ast) BytecodeProgram*
        +compile_automaton(automaton) BytecodeProgram*
        +optimize(program) bool
    }
    
    class BridgeAPI {
        +initialize() bool
        +cleanup() void
        +compile(pattern) handle
        +create_matcher(pattern) handle
        +execute(bytecode) bool
        +serialize(pattern) bytecode
        +deserialize(bytecode) pattern
    }
    
    class WebBridge {
        -pattern_handles: map
        -matcher_handles: map
        +rift_webbridge_init() int
        +rift_webbridge_cleanup() void
        +rift_webbridge_compile() handle
        +rift_webbridge_serialize_pattern() uint32_t
        +rift_webbridge_deserialize_pattern() handle
        +rift_webbridge_run_bytecode() int
    }
    
    class NativeBridge {
        +initialize() bool
        +cleanup() void
        +compile(pattern) handle
        +execute(bytecode) bool
    }
    
    BridgeAPI <|-- WebBridge
    BridgeAPI <|-- NativeBridge
    BytecodeCompiler --> BytecodeProgram
    BytecodeProgram *-- BytecodeInstruction
    BytecodeProgram --> BytecodeVM
    BytecodeVM --> BytecodeInstruction
    WebBridge --> BytecodeProgram
    NativeBridge --> BytecodeProgram
    ```