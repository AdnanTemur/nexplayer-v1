set -e

echo "Creating AppImage for Linux..."

# Download linuxdeploy if not exists
if [ ! -f "linuxdeploy-x86_64.AppImage" ]; then
    wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    chmod +x linuxdeploy-x86_64.AppImage
fi

if [ ! -f "linuxdeploy-plugin-qt-x86_64.AppImage" ]; then
    wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
    chmod +x linuxdeploy-plugin-qt-x86_64.AppImage
fi

# Create AppDir
rm -rf AppDir
mkdir -p AppDir

# Create AppImage
export QMAKE=/usr/lib/qt6/bin/qmake
./linuxdeploy-x86_64.AppImage \
    --executable=build/NexPlayer \
    --desktop-file=assets/nexplayer.desktop \
    --icon-file=assets/icon.png \
    --appdir=AppDir \
    --plugin=qt \
    --output=appimage

mv NexPlayer-*.AppImage build/

echo "AppImage created: build/NexPlayer-*.AppImage"