# RIFT Studio

RIFT Studio is a lightweight desktop GUI for the RIFT toolchain. It provides a source editor, file operations, and one-click access to the existing `rift tokenize`, `rift parse`, and `rift compile` commands.

It intentionally does **not** duplicate compiler logic: the GUI writes the current editor buffer to a temporary `.rf` file when necessary and invokes the repository's CLI. The RIFT repository is C/C++ and already exposes a portable CMake/Make build plus a CLI with `compile`, `tokenize`, and `parse` commands.

## Requirements

- Python 3.10+
- Tkinter (usually packaged as `python3-tk` on Debian/Ubuntu)
- A built RIFT CLI

The GUI is dependency-free beyond Python's standard library and Tkinter.

## Build RIFT

From the repository root:

```sh
make
```

The repository's Makefile configures an out-of-source CMake build and builds the libraries, CLIs, and tests.

## Run the GUI

```sh
python3 examples/rift-studio/app.py
```

If the GUI cannot locate the executable automatically, point it at the built CLI:

```sh
RIFT_BIN=/absolute/path/to/rift python3 examples/rift-studio/app.py
```

## Workflow

1. Open an existing `.rf`/`.rift` file or start with the scratch buffer.
2. Edit the RIFT source.
3. Use **Tokenize** to inspect the lexer stream.
4. Use **Parse** to exercise the parser entry point.
5. Use **Compile** to send the source through the full RIFT compilation pipeline.
6. Read stdout/stderr in the output pane.

Unsaved editor content is passed through a temporary `.rf` file, so you can inspect changes without saving them first.
