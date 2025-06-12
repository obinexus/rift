#!/bin/bash
# RIFT CLI Wrapper with library path
export LD_LIBRARY_PATH="$(dirname "$0")/lib:$LD_LIBRARY_PATH"
exec "$(dirname "$0")/bin/rift0" "$@"
