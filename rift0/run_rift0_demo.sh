#!/bin/bash
# RIFT Demo Wrapper with library path
export LD_LIBRARY_PATH="$(dirname "$0")/lib:$LD_LIBRARY_PATH"
exec "$(dirname "$0")/bin/rift0_demo" "$@"
