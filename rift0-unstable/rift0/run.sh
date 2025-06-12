#!/bin/bash

echo "🚀 RIFT Stage 0 - Enhanced CLI Demo"
echo "========================================="

# Build the project
echo "🔨 Building RIFT Stage 0..."
make clean && make all

if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

# Generate default configuration
echo ""
echo "⚙️  Setting up configuration..."
make config

echo ""
echo "🎯 Testing CLI with sample file..."
echo "-----------------------------------"

# Test CLI with sample file
mkdir -p output
./bin/rift0.exe --verbose examples/sample.rift

if [ $? -eq 0 ]; then
    echo ""
    echo "📋 Generated IR file:"
    echo "====================="
    if [ -f "output/sample.rift.0" ]; then
        head -20 output/sample.rift.0
        echo ""
        echo "... (see full file at output/sample.rift.0)"
    else
        echo "⚠️  Output file not found"
    fi
fi

echo ""
echo "🧪 Running QA tests..."
echo "----------------------"

# Run tests
make test

echo ""
echo "📊 Build Summary:"
echo "  📁 CLI executable: ./bin/rift0.exe"
echo "  📁 Demo executable: ./build/rift0_demo"
echo "  📁 Test executable: ./build/test_rift0"
echo "  📁 Configuration: .riftrc"
echo "  📁 Output directory: ./output/"
echo ""
echo "💡 Usage examples:"
echo "  ./bin/rift0.exe examples/sample.rift"
echo "  ./bin/rift0.exe --verbose --output ./build examples/sample.rift"
echo "  ./bin/rift0.exe --config custom.riftrc examples/sample.rift"
echo ""
echo "📋 RIFT Stage 0 Summary:"
echo "  ✅ Strict compiler compliance (-Werror -Wall -Wextra -Wpedantic)"
echo "  ✅ CLI system with .rift.0 IR output"
echo "  ✅ Configuration system (.riftrc)"
echo "  ✅ Enhanced error handling and memory management"
echo "  ✅ QA framework with comprehensive edge case testing"
echo ""
echo "🎉 Ready for Stage 1 (Parsing/Grammar Analysis)!"
