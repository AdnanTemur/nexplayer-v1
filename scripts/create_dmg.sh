#!/bin/bash
set -e

APP_NAME="NexPlayer"
VERSION="1.0.0"
DMG_NAME="${APP_NAME}-${VERSION}-macOS"
BUILD_DIR="build"

echo "📀 Creating DMG installer..."

# Check if app exists
if [ ! -d "${BUILD_DIR}/${APP_NAME}.app" ]; then
    echo "❌ Error: ${APP_NAME}.app not found in ${BUILD_DIR}/"
    exit 1
fi

# Create temporary DMG directory
DMG_DIR="${BUILD_DIR}/dmg"
rm -rf ${DMG_DIR}
mkdir -p ${DMG_DIR}

# Copy app bundle
echo "📦 Copying app bundle..."
cp -R ${BUILD_DIR}/${APP_NAME}.app ${DMG_DIR}/

# Create symbolic link to Applications folder
echo "🔗 Creating Applications symlink..."
ln -s /Applications ${DMG_DIR}/Applications

# Create DMG
echo "💿 Building DMG..."
hdiutil create -volname "${APP_NAME}" \
    -srcfolder ${DMG_DIR} \
    -ov -format UDZO \
    ${BUILD_DIR}/${DMG_NAME}.dmg

# Cleanup
rm -rf ${DMG_DIR}

echo "✅ DMG created: ${BUILD_DIR}/${DMG_NAME}.dmg"