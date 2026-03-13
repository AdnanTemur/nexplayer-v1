#!/bin/bash
set -e

echo "🍎 Deploying NexPlayer for macOS..."

APP_NAME="NexPlayer"
BUILD_DIR="build"

# Use Homebrew Qt explicitly
if [ -f "/opt/homebrew/bin/macdeployqt" ]; then
    QT_DEPLOY="/opt/homebrew/bin/macdeployqt"
    QT_LIB="/opt/homebrew/opt/qt/lib"
elif [ -f "/usr/local/bin/macdeployqt" ]; then
    # Fallback to old Homebrew location
    QT_DEPLOY="/usr/local/bin/macdeployqt"
    QT_LIB="/usr/local/opt/qt/lib"
else
    echo "❌ Error: Homebrew Qt not found!"
    echo "Install: brew install qt"
    exit 1
fi

echo "Using Qt: $QT_DEPLOY"
echo "Qt libs: $QT_LIB"

# Bundle Qt frameworks - with explicit lib path
echo "📦 Bundling Qt frameworks..."
$QT_DEPLOY ${BUILD_DIR}/${APP_NAME}.app \
    -qmldir=qml \
    -libpath=$QT_LIB \
    -always-overwrite

# Check if deployment succeeded
if [ ! -d "${BUILD_DIR}/${APP_NAME}.app/Contents/Frameworks" ]; then
    echo "⚠️  Warning: Qt frameworks not bundled properly"
fi

# Bundle FFmpeg
echo "📦 Bundling FFmpeg..."
mkdir -p ${BUILD_DIR}/${APP_NAME}.app/Contents/Frameworks

# Auto-detect FFmpeg location
if [ -d "/opt/homebrew/opt/ffmpeg/lib" ]; then
    FFMPEG_LIB="/opt/homebrew/opt/ffmpeg/lib"
elif [ -d "/usr/local/opt/ffmpeg/lib" ]; then
    FFMPEG_LIB="/usr/local/opt/ffmpeg/lib"
else
    echo "⚠️  FFmpeg not found"
    FFMPEG_LIB=""
fi

if [ -n "$FFMPEG_LIB" ]; then
    echo "FFmpeg libs: $FFMPEG_LIB"
    for lib in libavcodec libavformat libavutil libswresample libswscale; do
        # Copy all versions of the library
        find $FFMPEG_LIB -name "${lib}.*.dylib" -exec cp {} ${BUILD_DIR}/${APP_NAME}.app/Contents/Frameworks/ \;
    done
fi

# Fix library paths
echo "🔧 Fixing library install names..."
cd ${BUILD_DIR}/${APP_NAME}.app/Contents/MacOS

# Fix executable
for lib in libavcodec libavformat libavutil libswresample libswscale; do
    OLD_PATH=$(otool -L ${APP_NAME} | grep ${lib} | awk '{print $1}')
    if [ -n "$OLD_PATH" ]; then
        LIB_NAME=$(basename $OLD_PATH)
        install_name_tool -change "$OLD_PATH" "@executable_path/../Frameworks/$LIB_NAME" ${APP_NAME}
        echo "  Fixed: $lib"
    fi
done

cd - > /dev/null

echo "✅ macOS deployment complete!"
echo ""
echo "Test the app:"
echo "  open ${BUILD_DIR}/${APP_NAME}.app"