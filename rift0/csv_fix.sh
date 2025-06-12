#!/bin/bash
echo "🔧 Quick CSV Fix..."
rm -f examples/csv/r_syntax_demo.c examples/csv/r_syntax_demo
cd examples/csv && make clean && make test
