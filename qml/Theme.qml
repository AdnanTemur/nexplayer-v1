pragma Singleton
import QtQuick

QtObject {
    id: theme
    
    // Use Settings for persistence
    property bool isDarkMode: Settings.isDarkMode
    property color accentColor: Settings.accentColor
    
    // Watch for changes and save to Settings
    onIsDarkModeChanged: {
        Settings.isDarkMode = isDarkMode
    }
    
    onAccentColorChanged: {
        Settings.accentColor = accentColor.toString()
    }
    
    // Accent color presets
    readonly property var accentColors: [
        "#10b981", // Emerald (default)
        "#3b82f6", // Blue
        "#8b5cf6", // Purple
        "#ec4899", // Pink
        "#f59e0b", // Amber
        "#ef4444", // Red
        "#06b6d4", // Cyan
        "#84cc16"  // Lime
    ]
    
    // Dark theme colors
    readonly property color darkBackground: "#0f0f0f"
    readonly property color darkSurface: "#1a1a1a"
    readonly property color darkCard: "#252525"
    readonly property color darkText: "#ffffff"
    readonly property color darkTextSecondary: "#a3a3a3"
    readonly property color darkTextTertiary: "#737373"
    readonly property color darkBorder: "#404040"
    readonly property color darkDivider: "#2a2a2a"
    
    // Light theme colors
    readonly property color lightBackground: "#ffffff"
    readonly property color lightSurface: "#f5f5f5"
    readonly property color lightCard: "#ffffff"
    readonly property color lightText: "#171717"
    readonly property color lightTextSecondary: "#525252"
    readonly property color lightTextTertiary: "#737373"
    readonly property color lightBorder: "#e5e5e5"
    readonly property color lightDivider: "#f0f0f0"
    
    // Active colors based on theme
    readonly property color backgroundColor: isDarkMode ? darkBackground : lightBackground
    readonly property color surfaceColor: isDarkMode ? darkSurface : lightSurface
    readonly property color cardColor: isDarkMode ? darkCard : lightCard
    readonly property color textColor: isDarkMode ? darkText : lightText
    readonly property color textSecondary: isDarkMode ? darkTextSecondary : lightTextSecondary
    readonly property color textTertiary: isDarkMode ? darkTextTertiary : lightTextTertiary
    readonly property color borderColor: isDarkMode ? darkBorder : lightBorder
    readonly property color dividerColor: isDarkMode ? darkDivider : lightDivider
    
    // Accent-based colors
    readonly property color accentHover: Qt.lighter(accentColor, 1.1)
    readonly property color accentActive: Qt.darker(accentColor, 1.1)
    readonly property color accentTransparent: Qt.rgba(accentColor.r, accentColor.g, accentColor.b, 0.15)
    
    // Interactive element colors
    readonly property color buttonHoverColor: isDarkMode ? "#2a2a2a" : "#f0f0f0"
    readonly property color buttonActiveColor: isDarkMode ? "#353535" : "#e0e0e0"
    
    // Glass effect colors
    readonly property color glassBackground: isDarkMode ? "#20ffffff" : "#20000000"
    readonly property color glassBorder: isDarkMode ? "#30ffffff" : "#30000000"
    
    // Shadow
    readonly property color shadowColor: isDarkMode ? "#00000080" : "#00000030"
    
    // Functions
    function toggleTheme() {
        isDarkMode = !isDarkMode
    }
    
    function setAccentColor(color) {
        accentColor = color
    }
    
    Component.onCompleted: {
        console.log("Theme initialized - Dark mode:", isDarkMode, "Accent:", accentColor)
    }
}