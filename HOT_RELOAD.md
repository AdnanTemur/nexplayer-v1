# 🔥 Hot Reload Development Guide

## Never Rebuild for UI Changes Again!

Qt/QML supports **instant hot reload** - edit QML files and see changes immediately without recompiling!

---

## 🚀 Quick Start with Hot Reload

### Step 1: Build with Hot Reload Enabled

```bash
mkdir build && cd build
cmake -DQML_HOT_RELOAD=ON ..
make
```

### Step 2: Run the App

```bash
./NexPlayer
```

### Step 3: Edit QML Files

Open any QML file in your editor:
```bash
# Edit UI files
nano ../qml/ControlBar.qml
# or use VS Code, Qt Creator, etc.
```

### Step 4: Save and See Changes!

**Just save the file** - changes appear **instantly** in the running app! 🎉

No rebuild needed. No restart needed. Just edit and save!

---

## 📝 What You Can Change Instantly

### ✅ Changes That Hot Reload (No rebuild needed)
- Colors, sizes, positions
- Text content
- Animations and transitions
- Layout changes
- New UI elements
- Button styles
- Visibility/opacity
- Properties and bindings
- QML logic (JavaScript)

### ❌ Changes That Need Rebuild
- C++ code changes
- Adding new C++ classes
- Changing C++ signals/slots
- CMakeLists.txt changes
- Adding new QML files (need to update qml.qrc)

---

## 🎨 Typical Development Workflow

### For UI Development (Hot Reload)
```bash
# Build once
mkdir build && cd build
cmake -DQML_HOT_RELOAD=ON -DCMAKE_BUILD_TYPE=Debug ..
make

# Run app
./NexPlayer &

# Edit QML files in your favorite editor
code ../qml/  # VS Code
# or
open -a "Qt Creator" ../qml/main.qml  # Qt Creator

# Save files → See changes instantly!
```

### For C++ Development
```bash
# Make C++ changes
vim ../src/audio/effects/EQEffect.cpp

# Rebuild (fast incremental build)
make

# Restart app
killall NexPlayer
./NexPlayer
```

---

## 🛠️ IDE Setup for Best Experience

### Option 1: Qt Creator (Recommended)

**Best QML editor with live preview!**

```bash
# Open project
qtcreator ../CMakeLists.txt

# In Qt Creator:
# 1. Tools → Options → Qt Quick → QML/JS Editing
# 2. Enable "Enable Qt Quick Designer"
# 3. Edit QML files → See live preview!
```

**Features:**
- ✅ QML syntax highlighting
- ✅ Auto-completion
- ✅ Live preview panel
- ✅ Integrated debugger
- ✅ Visual designer

### Option 2: VS Code

```bash
# Install extensions
code --install-extension ms-vscode.cpptools
code --install-extension bbenoist.QML

# Open project
code ..
```

**Extensions:**
- `QML` by bbenoist - Syntax highlighting
- `C/C++` by Microsoft - C++ support
- `CMake Tools` - Build integration

### Option 3: Any Text Editor

```bash
# Sublime Text, Vim, Emacs, Nano, etc.
# Just save QML files → Changes reload automatically!
```

---

## 🎯 Hot Reload Best Practices

### 1. Keep App Running
Leave NexPlayer running while you develop UI. Make changes, save, watch them appear!

### 2. Use Debug Build for Development
```bash
cmake -DQML_HOT_RELOAD=ON -DCMAKE_BUILD_TYPE=Debug ..
```
Debug builds are faster to compile and easier to debug.

### 3. Disable Hot Reload for Production
```bash
cmake -DQML_HOT_RELOAD=OFF -DCMAKE_BUILD_TYPE=Release ..
```
Production builds embed QML in the binary (faster startup).

### 4. Organize Your Workflow
- **Left monitor**: Running app
- **Right monitor**: Code editor
- Edit → Save → See changes instantly!

---

## 🐛 Troubleshooting Hot Reload

### Changes Not Appearing?

**Check 1: Is hot reload enabled?**
```bash
# App should print on startup:
🔥 Hot Reload Mode: Edit QML files and save to see changes!
```

**Check 2: Did you save the file?**
Most editors need explicit save (Ctrl+S).

**Check 3: Is the QML file imported correctly?**
```qml
// In main.qml, components are imported like:
import "." as Components
Components.ControlBar { }
```

**Check 4: QML syntax error?**
Check the console for errors:
```
QML ControlBar: TypeError: Cannot read property 'x' of null
```

### File Not Found Error?

```bash
# Make sure QML files are relative to build directory
# Structure should be:
NexPlayer/
  build/
    NexPlayer (executable)
  qml/
    main.qml
```

### Need to Rebuild?

If you changed C++ code or added new files:
```bash
cd build
make -j8
killall NexPlayer
./NexPlayer
```

---

## 💡 Advanced: QML Live (Official Qt Tool)

For even more advanced hot reload:

```bash
# Install QML Live
# Allows remote hot reload, multiple device sync, etc.

# Ubuntu/Debian
sudo apt install qml-live

# macOS
brew install qt-creator  # includes QML Live

# Run with QML Live
qmllive main.qml
```

---

## 🎨 Example: Changing Colors Live

### Before (Purple theme):
```qml
// qml/main.qml
readonly property color accentColor: "#6366f1"  // Indigo
```

### Edit and Save:
```qml
// qml/main.qml
readonly property color accentColor: "#10b981"  // Green
```

**Result:** Colors change **instantly** - no rebuild, no restart! 🎉

---

## 📊 Development Speed Comparison

| Task | Without Hot Reload | With Hot Reload |
|------|-------------------|-----------------|
| Change button color | 30-60 seconds | **< 1 second** |
| Adjust layout | 30-60 seconds | **< 1 second** |
| Add UI element | 30-60 seconds | **< 1 second** |
| Test animation | 30-60 seconds | **< 1 second** |
| Tweak spacing | 30-60 seconds | **< 1 second** |

**Hot reload saves hours of development time!**

---

## 🔄 When to Use Each Mode

### Development Mode (Hot Reload ON)
```bash
cmake -DQML_HOT_RELOAD=ON -DCMAKE_BUILD_TYPE=Debug ..
```
**Use for:**
- UI design and tweaking
- Testing animations
- Experimenting with layouts
- Rapid prototyping

### Production Mode (Hot Reload OFF)
```bash
cmake -DQML_HOT_RELOAD=OFF -DCMAKE_BUILD_TYPE=Release ..
```
**Use for:**
- Final builds
- Distribution
- Performance testing
- Release packages

---

## 🎓 Pro Tips

### 1. Split Screen Development
```
┌──────────────┬──────────────┐
│              │              │
│  NexPlayer   │  VS Code     │
│  (Running)   │  (Editing)   │
│              │              │
└──────────────┴──────────────┘
```

### 2. Use `console.log()` in QML
```qml
Button {
    onClicked: {
        console.log("Button clicked!")
        console.log("Volume:", player.volume)
    }
}
```
Output appears in terminal immediately!

### 3. Quick Iterations
1. Make change in QML
2. Save (Ctrl+S)
3. See result
4. Adjust
5. Save
6. Repeat!

Each iteration takes **< 1 second**!

### 4. Test Multiple Themes
```qml
// Quickly switch between color schemes
readonly property color accentColor: "#6366f1"  // Try different colors!
// Save → instant preview → try another → save → instant preview
```

---

## 📚 Learn More

- [Qt QML Hot Reload Docs](https://doc.qt.io/qt-6/qtquick-qml-runtime.html)
- [QML Live Tool](https://doc.qt.io/QtApplicationManager/qml-live.html)
- [Qt Creator QML Profiler](https://doc.qt.io/qtcreator/creator-qml-performance-monitor.html)

---

## 🎉 Summary

**Hot reload for QML = Instant UI changes!**

1. Build with `-DQML_HOT_RELOAD=ON`
2. Run NexPlayer
3. Edit QML files
4. Save
5. **See changes instantly!**

No more waiting for rebuilds. Design UI at the speed of thought! 🚀

---

**Happy coding!** Your UI development just got 10x faster! ⚡
