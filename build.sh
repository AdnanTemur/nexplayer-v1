#!/bin/bash

# NexPlayer Build Script
# Automatically builds the project with optimal settings

set -e  # Exit on error

echo "🎬 NexPlayer Build Script"
echo "=========================="
echo ""

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Detect OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macOS"
    CORES=$(sysctl -n hw.ncpu)
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="Linux"
    CORES=$(nproc)
else
    OS="Unknown"
    CORES=4
fi

echo -e "${BLUE}Platform:${NC} $OS"
echo -e "${BLUE}CPU Cores:${NC} $CORES"
echo ""

# Check dependencies
echo "🔍 Checking dependencies..."

check_command() {
    if command -v $1 &> /dev/null; then
        echo -e "  ${GREEN}✓${NC} $1"
        return 0
    else
        echo -e "  ${RED}✗${NC} $1 (not found)"
        return 1
    fi
}

MISSING=0

check_command cmake || MISSING=1
check_command qmake || MISSING=1

if [[ $MISSING -eq 1 ]]; then
    echo ""
    echo -e "${RED}Error: Missing dependencies!${NC}"
    echo ""
    
    if [[ "$OS" == "macOS" ]]; then
        echo "Install with: brew install cmake qt@6 ffmpeg"
    elif [[ "$OS" == "Linux" ]]; then
        echo "Install with: sudo apt install cmake qt6-base-dev libavcodec-dev"
    fi
    
    exit 1
fi

echo -e "${GREEN}All dependencies found!${NC}"
echo ""

# Build type
BUILD_TYPE=${1:-Release}
HOT_RELOAD=${2:-OFF}

if [[ "$BUILD_TYPE" != "Release" && "$BUILD_TYPE" != "Debug" ]]; then
    echo -e "${YELLOW}Warning: Invalid build type '$BUILD_TYPE', using 'Release'${NC}"
    BUILD_TYPE="Release"
fi

echo -e "${BLUE}Build Type:${NC} $BUILD_TYPE"

if [[ "$HOT_RELOAD" == "ON" || "$HOT_RELOAD" == "on" || "$HOT_RELOAD" == "yes" ]]; then
    echo -e "${BLUE}Hot Reload:${NC} ${GREEN}ENABLED${NC} 🔥"
    echo -e "  ${YELLOW}→ Edit QML files and see changes instantly!${NC}"
    HOT_RELOAD="ON"
else
    echo -e "${BLUE}Hot Reload:${NC} Disabled"
    HOT_RELOAD="OFF"
fi
echo ""

# Create build directory
BUILD_DIR="build"

if [[ -d "$BUILD_DIR" ]]; then
    echo "📁 Build directory exists"
    read -p "   Remove and rebuild? (y/N) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "   Removing old build..."
        rm -rf "$BUILD_DIR"
    fi
else
    echo "📁 Creating build directory..."
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo ""
echo "⚙️  Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DQML_HOT_RELOAD=$HOT_RELOAD .. || {
    echo -e "${RED}CMake configuration failed!${NC}"
    exit 1
}

# Build
echo ""
echo "🔨 Building NexPlayer..."
echo "   Using $CORES cores"
echo ""

cmake --build . --config $BUILD_TYPE -j$CORES || {
    echo -e "${RED}Build failed!${NC}"
    exit 1
}

# Success
echo ""
echo -e "${GREEN}✨ Build successful!${NC}"
echo ""

# Show binary location
if [[ "$BUILD_TYPE" == "Release" ]]; then
    if [[ -f "NexPlayer" ]]; then
        BINARY="./NexPlayer"
    elif [[ -f "Release/NexPlayer.exe" ]]; then
        BINARY="Release/NexPlayer.exe"
    else
        BINARY="NexPlayer"
    fi
else
    if [[ -f "NexPlayer" ]]; then
        BINARY="./NexPlayer"
    elif [[ -f "Debug/NexPlayer.exe" ]]; then
        BINARY="Debug/NexPlayer.exe"
    else
        BINARY="NexPlayer"
    fi
fi

echo "📦 Binary location: $BINARY"
echo ""

# Offer to run
read -p "🚀 Run NexPlayer now? (Y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Nn]$ ]]; then
    echo ""
    echo "Starting NexPlayer..."
    echo ""
    $BINARY
fi
