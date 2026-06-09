# RIFT

RIFT Is a Flexible Translator: a portable C11 regex engine, translator,
RiftLang DSL, confidence-gated RIFT-0 to RIFT-1 bridge, and test framework.

## Public APIs

```c
#include <rift/rift.h>
#include <rift/riftlang.h>
#include <rift/rifttest.h>
#include <rift/riftbridge.h>
```

The canonical model exposes `RIFTRegex`, `RIFTToken`, `RIFTTokenType`, and
`RIFTMemory`. Legacy `RiftToken` and `rift_token_t` spellings remain available.

## Build

GNU Make provides a portable wrapper around the CMake build:

```sh
make
make test
make install PREFIX=build/install
```

On Windows, plain `make` uses MinGW and configures `build/mingw`. On Linux it
uses the default CMake generator and configures `build/make`. Override
`BUILD_DIR`, `GENERATOR`, `CONFIG`, `PREFIX`, or `CMAKE` when needed:

```powershell
make CONFIG=Debug BUILD_DIR=build/mingw-debug
```

The equivalent direct CMake workflow is:

```sh
cmake -S . -B build/local -DRIFT_BUILD_TESTS=ON
cmake --build build/local --parallel
ctest --test-dir build/local --output-on-failure
```

RIFT requires an out-of-source build. Configuring with `cmake -S . -B .` is
rejected so generated CMake files cannot replace the maintained root Makefile.

Artifacts are written to `build/bin/<platform-toolchain>/`. Each of `rift`,
`riftlang`, and `rifttest` is produced as a static and shared library. The
`rift` and `riftlang` command-line translators are emitted beside them.

Installed CMake targets:

```cmake
find_package(RIFT CONFIG REQUIRED)
target_link_libraries(app PRIVATE
    RIFT::rift
    RIFT::riftlang
    RIFT::rifttest
)
```

## CLI

```sh
rift compile input.rift output.c
rift tokenize input.rift
riftlang input.rift -o patterns.c
riftlang --validate input.rift
```

Linux GCC/Clang and Windows MSVC/MinGW builds are covered by CI.
