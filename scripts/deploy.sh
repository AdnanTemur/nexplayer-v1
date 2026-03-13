PLATFORM=$(uname -s)

case $PLATFORM in
    Darwin)
        echo "Detected macOS"
        find scripts -name "*.sh" -exec chmod +x {} \;
        ./scripts/deploy_macos.sh
        ./scripts/create_dmg.sh
        echo "macOS package ready: build/*.dmg"
        ;;
    Linux)
        echo "Detected Linux"
        ./scripts/create_appimage.sh
        echo "Linux package ready: build/*.AppImage"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        echo "Detected Windows"
        ./scripts/deploy_windows.bat
        ./scripts/create_installer.bat
        echo "Windows installer ready: build/*.exe"
        ;;
    *)
        echo "Unknown platform: $PLATFORM"
        exit 1
        ;;
esac