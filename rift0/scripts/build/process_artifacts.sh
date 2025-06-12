#!/bin/bash
# Process all .rift files to .rift.0 artifacts

echo "🎯 Processing RIFT artifacts..."

# Ensure rift0 is built
if [ ! -f "bin/rift0" ]; then
    echo "🔨 Building rift0 first..."
    make all
fi

# Process all .rift files
find examples -name "*.rift" | while read rift_file; do
    echo "Processing: $rift_file"
    
    # Generate .rift.0 artifact
    ./bin/rift0 "$rift_file"
    
    # Move to artifacts directory if generated
    if [ -f "$rift_file.0" ]; then
        mv "$rift_file.0" "out/artifacts/rift0/"
        echo "  ✅ Generated: $(basename "$rift_file").0"
    fi
done

echo "📊 Artifact processing complete"
echo "📁 Artifacts stored in: out/artifacts/rift0/"
ls -la out/artifacts/rift0/
