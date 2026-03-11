cmake_minimum_required(VERSION 3.16)
project(NexPlayer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Hot Reload option for development
option(QML_HOT_RELOAD "Enable QML hot reload (development mode)" OFF)

if(QML_HOT_RELOAD)
    message(STATUS "🔥 QML Hot Reload ENABLED - Edit QML files and see changes instantly!")
    add_definitions(-DQML_HOT_RELOAD)
endif()

# Qt Configuration
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick QuickControls2 Multimedia)

# FFmpeg Configuration
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec
    libavformat
    libavutil
    libswscale
    libswresample
)

# JUCE Configuration (you'll add JUCE as a submodule or install it)
# For now, we'll structure the code to easily integrate JUCE later
# add_subdirectory(external/JUCE)

# Source files
set(SOURCES
    src/main.cpp
    src/core/PlayerController.cpp
    src/video/VideoDecoder.cpp
    src/video/VideoRenderer.cpp
    src/video/VideoPlayer.cpp
    src/audio/AudioEngine.cpp
    src/audio/AudioDecoder.cpp
    src/audio/AudioOutput.cpp
    src/audio/EffectChain.cpp
    src/audio/effects/EQEffect.cpp
    src/audio/effects/ReverbEffect.cpp
    src/audio/effects/DelayEffect.cpp
)

set(HEADERS
    src/core/PlayerController.h
    src/video/VideoDecoder.h
    src/video/VideoRenderer.h
    src/video/VideoPlayer.h
    src/audio/AudioEngine.h
    src/audio/AudioDecoder.h
    src/audio/AudioOutput.h
    src/audio/EffectChain.h
    src/audio/effects/AudioEffect.h
    src/audio/effects/EQEffect.h
    src/audio/effects/ReverbEffect.h
    src/audio/effects/DelayEffect.h
    src/audio/effects/FlangerEffect.cpp
    src/audio/effects/PhaserEffect.cpp
    src/audio/effects/CompressorEffect.cpp
)

# QML Resources
set(QML_RESOURCES
    qml/qml.qrc
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${QML_RESOURCES}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Multimedia
    PkgConfig::FFMPEG
    # juce::juce_audio_basics
    # juce::juce_audio_devices
    # juce::juce_audio_formats
    # juce::juce_audio_processors
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Platform-specific settings
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.nexylius.nexplayer
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
