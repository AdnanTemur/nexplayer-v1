import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects
import QtQuick.Effects

Popup {
    id: popup
    
    width: 450
    height: 520
    anchors.centerIn: Overlay.overlay
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    
    background: Rectangle {
        color: Theme.cardColor
        radius: 12
        border.color: Theme.borderColor
        border.width: 1
        
        layer.enabled: true
        layer.effect: DropShadow {
            color: Theme.shadowColor
            radius: 16
            samples: 33
            verticalOffset: 4
        }
    }
    
    contentItem: Item {
        ColumnLayout {
            anchors.fill: parent
            spacing: 0
            
            // Header
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 56
                color: "transparent"
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10
                    
                    Rectangle {
                        width: 32
                        height: 32
                        radius: 16
                        color: Theme.accentTransparent
                        
                        Icon {
                            anchors.centerIn: parent
                            source: "qrc:/icons/settings.svg"
                            width: 16
                            height: 16
                            color: Theme.accentColor
                        }
                    }
                    
                    Text {
                        text: "Settings"
                        font.pixelSize: 18
                        font.weight: Font.Bold
                        color: Theme.textColor
                        Layout.fillWidth: true
                    }
                    
                    Button {
                        Layout.preferredWidth: 28
                        Layout.preferredHeight: 28
                        
                        contentItem: Text {
                            text: "×"
                            color: Theme.textSecondary
                            font.pixelSize: 20
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        
                        background: Rectangle {
                            color: parent.hovered ? Theme.buttonHoverColor : "transparent"
                            radius: 4
                            
                            Behavior on color {
                                ColorAnimation { duration: 150 }
                            }
                        }
                        
                        onClicked: popup.close()
                    }
                }
            }
            
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: Theme.dividerColor
            }
            
            // Content - Scrollable
            Flickable {
                Layout.fillWidth: true
                Layout.fillHeight: true
                contentHeight: contentColumn.height
                clip: true
                
                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }
                
                ColumnLayout {
                    id: contentColumn
                    width: parent.width
                    spacing: 0
                    
                    Item { Layout.preferredHeight: 12 }
                    
                    // ===== APPEARANCE SECTION =====
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.leftMargin: 16
                        Layout.rightMargin: 16
                        spacing: 10
                        
                        Text {
                            text: "APPEARANCE"
                            font.pixelSize: 11
                            font.weight: Font.DemiBold
                            color: Theme.textSecondary
                            Layout.fillWidth: true
                        }
                        
                        // Theme Mode
                        Rectangle {
                            Layout.fillWidth: true
                            height: 70
                            color: Theme.surfaceColor
                            radius: 12
                            border.color: Theme.borderColor
                            border.width: 1
                            
                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 12
                                
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 4
                                    
                                    Text {
                                        text: "Theme Mode"
                                        font.pixelSize: 15
                                        font.weight: Font.Medium
                                        color: Theme.textColor
                                    }
                                    
                                    Text {
                                        text: Theme.isDarkMode ? "Dark mode" : "Light mode"
                                        font.pixelSize: 13
                                        color: Theme.textSecondary
                                    }
                                }
                                
                                RowLayout {
                                    spacing: 6
                                    
                                    // Light mode button
                                    Rectangle {
                                        width: 44
                                        height: 44
                                        radius: 8
                                        color: !Theme.isDarkMode ? Theme.accentColor : "transparent"
                                        border.color: !Theme.isDarkMode ? "transparent" : Theme.borderColor
                                        border.width: 1
                                        
                                        Behavior on color {
                                            ColorAnimation { duration: 200 }
                                        }
                                        
                                        Icon {
                                            source: "qrc:/icons/sun.svg"
                                            width: 22
                                            height: 22
                                            anchors.centerIn: parent
                                            color: !Theme.isDarkMode ? "white" : Theme.textSecondary
                                        }
                                        
                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: Theme.isDarkMode = false
                                        }
                                    }
                                    
                                    // Dark mode button
                                    Rectangle {
                                        width: 44
                                        height: 44
                                        radius: 8
                                        color: Theme.isDarkMode ? Theme.accentColor : "transparent"
                                        border.color: Theme.isDarkMode ? "transparent" : Theme.borderColor
                                        border.width: 1
                                        
                                        Behavior on color {
                                            ColorAnimation { duration: 200 }
                                        }
                                        
                                        Icon {
                                            source: "qrc:/icons/moon.svg"
                                            width: 20
                                            height: 20
                                            anchors.centerIn: parent
                                            color: Theme.isDarkMode ? "white" : Theme.textSecondary
                                        }
                                        
                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: Theme.isDarkMode = true
                                        }
                                    }
                                }
                            }
                        }
                        
                        // Accent Color Card
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: accentContent.height + 24
                            color: Theme.surfaceColor
                            radius: 10
                            border.color: Theme.borderColor
                            border.width: 1
                            
                            ColumnLayout {
                                id: accentContent
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.top: parent.top
                                anchors.margins: 12
                                spacing: 10
                                
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 2
                                    
                                    Text {
                                        text: "Accent Color"
                                        font.pixelSize: 13
                                        font.weight: Font.Medium
                                        color: Theme.textColor
                                    }
                                    
                                    Text {
                                        text: "Choose your theme color"
                                        font.pixelSize: 11
                                        color: Theme.textSecondary
                                    }
                                }
                                
                                Flow {
                                    Layout.fillWidth: true
                                    spacing: 8
                                    
                                    Repeater {
                                        model: Theme.accentColors
                                        
                                        Rectangle {
                                            width: 36
                                            height: 36
                                            radius: 18
                                            color: modelData
                                            border.color: Theme.accentColor.toString() === modelData ? "white" : "transparent"
                                            border.width: 2.5
                                            
                                            Rectangle {
                                                anchors.centerIn: parent
                                                width: 16
                                                height: 16
                                                radius: 8
                                                color: "white"
                                                visible: Theme.accentColor.toString() === modelData
                                            }
                                            
                                            MouseArea {
                                                anchors.fill: parent
                                                cursorShape: Qt.PointingHandCursor
                                                onClicked: Theme.setAccentColor(modelData)
                                            }
                                            
                                            Behavior on border.width {
                                                NumberAnimation { duration: 150 }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    Item { Layout.preferredHeight: 16 }
                    
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.leftMargin: 16
                        Layout.rightMargin: 16
                        Layout.preferredHeight: 1
                        color: Theme.dividerColor
                        opacity: 0.5
                    }
                    
                    Item { Layout.preferredHeight: 16 }
                    
                    // ===== AUDIO SECTION =====
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.leftMargin: 24
                        Layout.rightMargin: 24
                        spacing: 16
                        
                        Text {
                            text: "Audio"
                            font.pixelSize: 13
                            font.weight: Font.Medium
                            color: Theme.textColor
                            font.capitalization: Font.AllUppercase
                            opacity: 0.6
                        }
                        
                        Rectangle {
                            Layout.fillWidth: true
                            height: volumeContent.height + 72
                            color: Theme.surfaceColor
                            radius: 12
                            border.color: Theme.borderColor
                            border.width: 1
                            
                            ColumnLayout {
                                id: volumeContent
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 16
                                
                                RowLayout {
                                    Layout.fillWidth: true
                                    
                                    Text {
                                        text: "Volume"
                                        font.pixelSize: 15
                                        font.weight: Font.Medium
                                        color: Theme.textColor
                                        Layout.fillWidth: true
                                    }
                                    
                                    Text {
                                        text: Math.round(volumeSlider.value * 100) + "%"
                                        font.pixelSize: 15
                                        font.weight: Font.Bold
                                        color: Theme.accentColor
                                    }
                                }
                                
                                Slider {
                                    id: volumeSlider
                                    Layout.fillWidth: true
                                    from: 0
                                    to: 1
                                    value: Settings.volume
                                    stepSize: 0.01
                                    
                                    onValueChanged: {
                                        Settings.volume = value
                                        // Apply to player immediately if it exists
                                        if (player) {
                                            player.volume = value
                                        }
                                    }
                                    
                                    // Sync with player volume changes
                                    Connections {
                                        target: player
                                        function onVolumeChanged() {
                                            if (player && Math.abs(volumeSlider.value - player.volume) > 0.01) {
                                                volumeSlider.value = player.volume
                                                Settings.volume = player.volume
                                            }
                                        }
                                    }
                                    
                                    background: Rectangle {
                                        x: volumeSlider.leftPadding
                                        y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                                        width: volumeSlider.availableWidth
                                        height: 6
                                        radius: 3
                                        color: Theme.borderColor
                                        
                                        Rectangle {
                                            width: volumeSlider.visualPosition * parent.width
                                            height: parent.height
                                            color: Theme.accentColor
                                            radius: 3
                                            
                                            Behavior on width {
                                                NumberAnimation { duration: 100 }
                                            }
                                        }
                                    }
                                    
                                    handle: Rectangle {
                                        x: volumeSlider.leftPadding + volumeSlider.visualPosition * (volumeSlider.availableWidth - width)
                                        y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                                        width: 18
                                        height: 18
                                        radius: 9
                                        color: volumeSlider.pressed ? Theme.accentActive : Theme.accentColor
                                        border.color: "white"
                                        border.width: 2
                                    }
                                }
                                
                                // Volume icon indicator
                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 8
                                    
                                    Icon {
                                        source: {
                                            var vol = volumeSlider.value
                                            if (vol === 0) return "qrc:/icons/volume-x.svg"
                                            else if (vol < 0.5) return "qrc:/icons/volume-1.svg"
                                            else return "qrc:/icons/volume-2.svg"
                                        }
                                        width: 20
                                        height: 20
                                        color: Theme.textSecondary
                                    }
                                    
                                    Text {
                                        text: volumeSlider.value === 0 ? "Muted" : 
                                            volumeSlider.value < 0.3 ? "Quiet" :
                                            volumeSlider.value < 0.7 ? "Medium" : "Loud"
                                        font.pixelSize: 12
                                        color: Theme.textTertiary
                                    }
                                    
                                    Item { Layout.fillWidth: true }
                                    
                                    // Quick preset buttons
                                    RowLayout {
                                        spacing: 4
                                        
                                        Repeater {
                                            model: [
                                                { "label": "25%", "value": 0.25 },
                                                { "label": "50%", "value": 0.5 },
                                                { "label": "75%", "value": 0.75 },
                                                { "label": "100%", "value": 1.0 }
                                            ]
                                            
                                            Rectangle {
                                                width: 42
                                                height: 24
                                                radius: 4
                                                color: Math.abs(volumeSlider.value - modelData.value) < 0.05 ? 
                                                    Theme.accentTransparent : "transparent"
                                                border.color: Theme.borderColor
                                                border.width: 1
                                                
                                                Text {
                                                    anchors.centerIn: parent
                                                    text: modelData.label
                                                    font.pixelSize: 10
                                                    color: Theme.textSecondary
                                                }
                                                
                                                MouseArea {
                                                    anchors.fill: parent
                                                    cursorShape: Qt.PointingHandCursor
                                                    onClicked: {
                                                        volumeSlider.value = modelData.value
                                                        if (player) player.volume = modelData.value
                                                    }
                                                }
                                                
                                                Behavior on color {
                                                    ColorAnimation { duration: 150 }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    Item { Layout.preferredHeight: 16 }
                    
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.leftMargin: 16
                        Layout.rightMargin: 16
                        Layout.preferredHeight: 1
                        color: Theme.dividerColor
                        opacity: 0.5
                    }
                    
                    Item { Layout.preferredHeight: 16 }
                    
                    // ===== ABOUT SECTION =====
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.leftMargin: 16
                        Layout.rightMargin: 16
                        spacing: 10
                        
                        Text {
                            text: "ABOUT"
                            font.pixelSize: 11
                            font.weight: Font.DemiBold
                            color: Theme.textSecondary
                        }
                        
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 90
                            color: Theme.surfaceColor
                            radius: 10
                            border.color: Theme.borderColor
                            border.width: 1
                            
                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing: 6
                                
                                Text {
                                    text: "NexPlayer"
                                    font.pixelSize: 16
                                    font.weight: Font.Bold
                                    color: Theme.textColor
                                    Layout.alignment: Qt.AlignHCenter
                                }
                                
                                Text {
                                    text: "Version 1.0.0"
                                    font.pixelSize: 12
                                    color: Theme.textSecondary
                                    Layout.alignment: Qt.AlignHCenter
                                }
                                
                                Text {
                                    text: "Built with Qt • FFmpeg"
                                    font.pixelSize: 10
                                    color: Theme.textTertiary
                                    Layout.alignment: Qt.AlignHCenter
                                }
                            }
                        }
                    }
                    
                    Item { Layout.preferredHeight: 16 }
                }
            }
        }
    }
    
    // Smooth animations
    enter: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0.0
            to: 1.0
            duration: 200
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            property: "scale"
            from: 0.95
            to: 1.0
            duration: 200
            easing.type: Easing.OutBack
            easing.overshoot: 1.2
        }
    }
    
    exit: Transition {
        NumberAnimation {
            property: "opacity"
            from: 1.0
            to: 0.0
            duration: 150
            easing.type: Easing.InCubic
        }
        NumberAnimation {
            property: "scale"
            from: 1.0
            to: 0.95
            duration: 150
            easing.type: Easing.InCubic
        }
    }
}