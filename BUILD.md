# Building NexPlayer

Detailed build instructions for all platforms.

## Table of Contents
1. [Windows Build](#windows-build)
2. [macOS Build](#macos-build)
3. [Linux Build](#linux-build)
4. [Common Issues](#common-issues)

---

## Windows Build

### Step 1: Install Prerequisites

#### Option A: Using Chocolatey (Recommended)
```powershell
# Install Chocolatey if not already installed
# See: https://chocolatey.org/install

# Install dependencies
choco install cmake -y
choco install git -y
choco install visualstudio2022community -y
```

#### Option B: Manual Installation
1. **Visual Studio 2022** (Community/Professional/Enterprise)
   - Download: https://visualstudio.microsoft.com/
   - Install "Desktop development with C++" workload

2. **CMake** (3.16+)
   - Download: https://cmake.org/download/
   - Add to PATH during installation

3. **Qt 6** (6.5+)
   - Download: https://www.qt.io/download-open-source
   - Install Qt 6.5+ with MSVC compiler
   - Note the installation path (e.g., `C:\Qt\6.5.3\msvc2019_64`)

4. **FFmpeg**
   - Download precompiled: https://www.gyan.dev/ffmpeg/builds/
   - Get "ffmpeg-release-full-shared" package
   - Extract to `C:\ffmpeg`
   - Add `C:\ffmpeg\bin` to PATH

### Step 2: Set Environment Variables

```powershell
# Qt path
$env:Qt6_DIR = "C:\Qt\6.5.3\msvc2019_64"

# FFmpeg paths
$env:FFMPEG_ROOT = "C:\ffmpeg"
$env:PATH += ";C:\ffmpeg\bin"
```

### Step 3: Build

```powershell
# Clone repository
git clone https://github.com/adnantemur/nexplayer-v1.git
cd nexplayer

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake -G "Visual Studio 17 2022" -A x64 `
      -DQt6_DIR="$env:Qt6_DIR\lib\cmake\Qt6" `
      ..

# Build
cmake --build . --config Release

# Run
.\Release\NexPlayer.exe
```

### Alternative: Qt Creator
1. Open `CMakeLists.txt` in Qt Creator
2. Configure project with MSVC kit
3. Build → Run

---

## macOS Build

### Step 1: Install Prerequisites

#### Using Homebrew (Recommended)
```bash
# Install Homebrew if not already installed
# See: https://brew.sh/

# Install dependencies
brew install cmake qt@6 ffmpeg pkg-config
```

### Step 2: Set Environment Variables

```bash
# Add to ~/.zshrc or ~/.bash_profile
export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"
export Qt6_DIR="/opt/homebrew/opt/qt@6"
export PKG_CONFIG_PATH="/opt/homebrew/opt/ffmpeg/lib/pkgconfig:$PKG_CONFIG_PATH"
```

### Step 3: Build

```bash
# Clone repository
git clone https://github.com/adnantemur/nexplayer-v1.git
cd nexplayer

# Create build directory
mkdir build && cd build

# Configure
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build (use all CPU cores)
make -j$(sysctl -n hw.ncpu)

# Create app bundle
make install

# Run
open NexPlayer.app
# or
./NexPlayer
```

### Alternative: Xcode
```bash
# Generate Xcode project
cmake -G Xcode ..

# Open in Xcode
open NexPlayer.xcodeproj
```

---

## Linux Build

### Ubuntu/Debian

#### Step 1: Install Prerequisites

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    git \
    qt6-base-dev \
    qt6-declarative-dev \
    qt6-multimedia-dev \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    libswscale-dev \
    libswresample-dev \
    pkg-config
```

#### Step 2: Build

```bash
# Clone repository
git clone https://github.com/adnantemur/nexplayer-v1.git
cd nexplayer

# Create build directory
mkdir build && cd build

# Configure
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
make -j$(nproc)

# Install (optional)
sudo make install

# Run
./NexPlayer
```

### Fedora/RHEL

```bash
# Install dependencies
sudo dnf install -y \
    cmake \
    gcc-c++ \
    qt6-qtbase-devel \
    qt6-qtdeclarative-devel \
    qt6-qtmultimedia-devel \
    ffmpeg-devel \
    pkgconfig

# Build (same as Ubuntu)
```

### Arch Linux

```bash
# Install dependencies
sudo pacman -S cmake qt6-base qt6-declarative qt6-multimedia ffmpeg pkgconf

# Build (same as Ubuntu)
```

---

## Common Issues

### Issue: Qt6 not found

**Solution (Windows):**
```powershell
cmake -DQt6_DIR="C:\Qt\6.5.3\msvc2019_64\lib\cmake\Qt6" ..
```

**Solution (macOS):**
```bash
export Qt6_DIR="/opt/homebrew/opt/qt@6"
cmake ..
```

**Solution (Linux):**
```bash
sudo apt install qt6-base-dev
```

### Issue: FFmpeg not found

**Windows:**
- Ensure FFmpeg is in PATH
- Set `FFMPEG_ROOT` environment variable

**macOS/Linux:**
```bash
# Check if pkg-config can find FFmpeg
pkg-config --modversion libavcodec

# If not, install FFmpeg development packages
```

### Issue: Build fails with "missing QML modules"

**Solution:**
```bash
# Make sure Qt Quick modules are installed
# Ubuntu
sudo apt install qt6-declarative-dev

# macOS
brew reinstall qt@6

# Windows
# Reinstall Qt with "Qt Quick" component selected
```

### Issue: Application crashes on startup

**Check:**
1. All Qt plugins are available
2. FFmpeg libraries are in PATH
3. Run with debugging: `./NexPlayer --debug`

### Issue: Video playback not working

**This is expected in v1.0** - Video decoding is stubbed. Full implementation coming in Phase 2.

### Issue: No audio output

**This is expected in v1.0** - Audio engine needs JUCE/PortAudio integration (Phase 2).

---

## Development Build

For faster iteration during development:

```bash
# Build in Debug mode
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run with debugging
./NexPlayer
```

## Optimization Build

For maximum performance:

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-O3 -march=native" \
      ..
make
```

---

## IDE Setup

### Visual Studio Code
1. Install C/C++ extension
2. Install CMake Tools extension
3. Open folder
4. Select CMake kit
5. Build and debug

### CLion
1. Open `CMakeLists.txt`
2. CLion will auto-configure
3. Build → Run

### Qt Creator
1. File → Open File or Project
2. Select `CMakeLists.txt`
3. Configure project
4. Build → Run

---

## Next Steps

After successful build:
1. Try opening a media file (Ctrl+O)
2. Experiment with audio effects panel
3. Check logs for any warnings
4. Read CONTRIBUTING.md to add features

For help: Open an issue on GitHub or visit nexylius.com
