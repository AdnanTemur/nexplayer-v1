pragma Singleton
import QtQuick
import Qt.labs.settings 1.0

Settings {
    category: "NexPlayer"
    
    // Theme settings
    property bool isDarkMode: true
    property string accentColor: "#10b981"
    
    // Audio settings
    property real volume: 0.7
    
    // Window settings
    property int windowWidth: 1280
    property int windowHeight: 720
    
    // Playback settings
    property string lastOpenedFile: ""
    property bool rememberLastFile: false
}