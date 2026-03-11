import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// Advanced video controls bar
Rectangle {
    id: advancedControls
    height: 56
    color: Theme.surfaceColor
    
    Behavior on color {
        ColorAnimation { duration: 200 }
    }
    
    // Top border
    Rectangle {
        anchors.top: parent.top
        width: parent.width
        height: 1
        color: Theme.borderColor
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 12

        // Skip Backward 10s
        ModernButton {
            iconSource: "qrc:/icons/skip-back.svg"
            size: 36
            onClicked: if (player) player.skipBackward(10)
            ToolTip.text: "Skip Back 10s"
            ToolTip.visible: hovered
            ToolTip.delay: 500
        }

        // Skip Forward 10s
        ModernButton {
            iconSource: "qrc:/icons/skip-forward.svg"
            size: 36
            onClicked: if (player) player.skipForward(10)
            ToolTip.text: "Skip Forward 10s"
            ToolTip.visible: hovered
            ToolTip.delay: 500
        }

        // Separator
        Rectangle {
            width: 1
            height: 24
            color: Theme.dividerColor
            opacity: 0.5
        }

        // Frame Step Backward
        ModernButton {
            iconSource: "qrc:/icons/step-back.svg"
            size: 32
            onClicked: if (player) player.stepBackward()
            ToolTip.text: "Previous Frame"
            ToolTip.visible: hovered
            ToolTip.delay: 500
        }

        // Frame Step Forward
        ModernButton {
            iconSource: "qrc:/icons/step-forward.svg"
            size: 32
            onClicked: if (player) player.stepForward()
            ToolTip.text: "Next Frame"
            ToolTip.visible: hovered
            ToolTip.delay: 500
        }

        // Separator
        Rectangle {
            width: 1
            height: 24
            color: Theme.dividerColor
            opacity: 0.5
        }

        // Screenshot
        ModernButton {
            iconSource: "qrc:/icons/camera.svg"
            size: 36
            onClicked: if (player) player.saveScreenshot()
            ToolTip.text: "Screenshot"
            ToolTip.visible: hovered
            ToolTip.delay: 500
        }

        // Loop Toggle
        ModernButton {
            iconSource: "qrc:/icons/repeat.svg"
            size: 36
            highlighted: player ? player.loop : false
            onClicked: {
                if (player) {
                    player.setLoop(!player.loop);
                }
            }
            ToolTip.text: (player && player.loop) ? "Loop: ON" : "Loop: OFF"
            ToolTip.visible: hovered
            ToolTip.delay: 500
        }

        // Separator
        Rectangle {
            width: 1
            height: 24
            color: Theme.dividerColor
            opacity: 0.5
        }

        // Playback Speed Dropdown
        RowLayout {
            spacing: 8

            Icon {
                source: "qrc:/icons/gauge.svg"
                width: 16
                height: 16
                color: Theme.textSecondary
            }

            ComboBox {
                id: speedCombo
                Layout.preferredWidth: 85
                model: ["0.25x", "0.5x", "0.75x", "1x", "1.25x", "1.5x", "2x"]

                Component.onCompleted: {
                    currentIndex = 3; // Start at 1x
                }

                Connections {
                    target: player
                    function onPlaybackSpeedChanged(speed) {
                        var speeds = [0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 2.0];
                        var index = speeds.indexOf(speed);
                        if (index >= 0) {
                            speedCombo.currentIndex = index;
                        }
                    }
                }

                background: Rectangle {
                    radius: 6
                    color: speedCombo.pressed ? Theme.buttonActiveColor : 
                           speedCombo.hovered ? Theme.buttonHoverColor : Theme.surfaceColor
                    border.color: Theme.borderColor
                    border.width: 1
                    
                    Behavior on color {
                        ColorAnimation { duration: 150 }
                    }
                }

                contentItem: Text {
                    text: speedCombo.displayText
                    font.pixelSize: 12
                    font.weight: Font.Medium
                    color: Theme.textColor
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 10
                }
                
                delegate: ItemDelegate {
                    width: speedCombo.width
                    
                    contentItem: Text {
                        text: modelData
                        font.pixelSize: 12
                        color: Theme.textColor
                        verticalAlignment: Text.AlignVCenter
                        leftPadding: 10
                    }
                    
                    background: Rectangle {
                        color: parent.highlighted ? Theme.accentTransparent : 
                               parent.hovered ? Theme.buttonHoverColor : "transparent"
                        
                        Behavior on color {
                            ColorAnimation { duration: 100 }
                        }
                    }
                }
                
                popup: Popup {
                    y: speedCombo.height + 4
                    width: speedCombo.width
                    padding: 4
                    
                    background: Rectangle {
                        color: Theme.cardColor
                        border.color: Theme.borderColor
                        border.width: 1
                        radius: 8
                    }
                    
                    contentItem: ListView {
                        clip: true
                        implicitHeight: contentHeight
                        model: speedCombo.popup.visible ? speedCombo.delegateModel : null
                        currentIndex: speedCombo.highlightedIndex
                    }
                }

                onActivated: {
                    var speeds = [0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 2.0];
                    if (player) player.setPlaybackSpeed(speeds[index]);
                }
            }
        }

        // Spacer
        Item {
            Layout.fillWidth: true
        }

        // Speed Indicator
        Rectangle {
            width: 60
            height: 28
            radius: 6
            color: (player && player.playbackSpeed !== 1.0) ? Theme.accentTransparent : Theme.surfaceColor
            border.color: (player && player.playbackSpeed !== 1.0) ? Theme.accentColor : Theme.borderColor
            border.width: 1
            
            Behavior on color {
                ColorAnimation { duration: 200 }
            }
            
            Behavior on border.color {
                ColorAnimation { duration: 200 }
            }
            
            Text {
                anchors.centerIn: parent
                text: player ? (player.playbackSpeed.toFixed(2) + "x") : "1.00x"
                font.pixelSize: 12
                font.weight: Font.Medium
                color: (player && player.playbackSpeed !== 1.0) ? Theme.accentColor : Theme.textSecondary
                
                Behavior on color {
                    ColorAnimation { duration: 200 }
                }
            }
        }
        
        // Separator
        Rectangle {
            width: 1
            height: 24
            color: Theme.dividerColor
            opacity: 0.5
        }
        
        // Settings Button
        ModernButton {
            iconSource: "qrc:/icons/settings.svg"
            size: 36
            onClicked: settingsPopup.open()
            ToolTip.text: "Settings"
            ToolTip.visible: hovered
            ToolTip.delay: 500
        }
    }
}