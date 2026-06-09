# LibRift Development Guide

## Build Commands
- `make` - Debug build (default)
- `make release` - Release build with optimizations
- `make test` - Run all tests
- `make unittest` - Run unit tests
- `make automaton_test` - Run specific automaton tests
- `make integration_test` - Run integration tests
- `make benchmark` - Run performance benchmarks
- `make format` - Run code formatter (clang-format)
- `make clean` - Remove build artifacts
- `make install` - Install the library

## Code Style Guidelines
- **Naming**: Functions use `snake_case` with `rift_` prefix
- **Types**: Use `snake_case_t` suffix for typedefs
- **Constants**: Use `UPPERCASE` for constants and macros
- **Error Handling**: Return `bool` for success/failure, pass errors via pointer parameters
- **Memory**: Creator is responsible for freeing, clean up resources in reverse acquisition order
- **Formatting**: Use project's clang-format rules
- **File Structure**: 
  - Headers (.h) in `include/librift/`
  - Implementation (.c) in `src/`
  - Organized by components (core, regex, visualizer)
- **Documentation**: Document functions with descriptive comments, parameters, and return values
