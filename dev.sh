#!/bin/bash

# NexPlayer Development Mode
# Builds with hot reload and runs the app

set -e

echo "🔥 NexPlayer Development Mode"
echo "=============================="
echo ""
echo "This will:"
echo "  1. Build with hot reload enabled"
echo "  2. Run NexPlayer"
echo "  3. Any QML changes will reload instantly!"
echo ""

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Create build directory
if [[ ! -d "build" ]]; then
    echo "📁 Creating build directory..."
    mkdir build
fi

cd build

# Configure with hot reload
echo ""
echo "⚙️  Configuring for development..."
cmake -DCMAKE_BUILD_TYPE=Debug -DQML_HOT_RELOAD=ON .. || {
    echo "Configuration failed!"
    exit 1
}

# Build
echo ""
echo "🔨 Building..."
if [[ "$OSTYPE" == "darwin"* ]]; then
    CORES=$(sysctl -n hw.ncpu)
else
    CORES=$(nproc 2>/dev/null || echo 4)
fi

cmake --build . -j$CORES || {
    echo "Build failed!"
    exit 1
}

# Success message
echo ""
echo -e "${GREEN}✨ Build successful!${NC}"
echo ""
echo -e "${YELLOW}🔥 HOT RELOAD ENABLED!${NC}"
echo ""
echo "Now you can:"
echo "  1. Edit any QML file in qml/ directory"
echo "  2. Save the file (Ctrl+S)"
echo "  3. See changes INSTANTLY in the running app!"
echo ""
echo "Try changing colors in qml/main.qml:"
echo "  accentColor: \"#6366f1\"  →  \"#10b981\""
echo ""
echo -e "${BLUE}Press Enter to start NexPlayer...${NC}"
read

# Run
echo ""
echo "🚀 Starting NexPlayer..."
echo ""

if [[ -f "NexPlayer" ]]; then
    ./NexPlayer
elif [[ -f "Debug/NexPlayer.exe" ]]; then
    ./Debug/NexPlayer.exe
else
    echo "Executable not found!"
    exit 1
fi
