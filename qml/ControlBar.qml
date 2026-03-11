import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: controlBar
    
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
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12
        
        // Timeline/seek bar
        TimelineBar {
            id: timeline
            Layout.fillWidth: true
            Layout.preferredHeight: 40
        }
        
        // Main controls row
        RowLayout {
            Layout.fillWidth: true
            spacing: 16
            
            // Left: Playback controls
            RowLayout {
                spacing: 8
                
                // Play/Pause button
                ModernButton {
                    iconSource: player.playing ? "qrc:/icons/pause.svg" : "qrc:/icons/play.svg"
                    size: 40
                    isPrimary: true
                    onClicked: player.togglePlayPause()
                }
                
                // Stop button
                ModernButton {
                    iconSource: "qrc:/icons/square.svg"
                    size: 40
                    onClicked: player.stop()
                }
                
                // Time display
                Text {
                    text: formatTime(player.position) + " / " + formatTime(player.duration)
                    font.pixelSize: 13
                    color: Theme.textSecondary
                    Layout.leftMargin: 8
                }
            }
            
            // Center: Spacer
            Item {
                Layout.fillWidth: true
            }
            
            // Right: Volume and effects
            RowLayout {
                spacing: 12
                
                // Volume control
                RowLayout {
                    spacing: 8
                    property real previousVolume: 0.7
                    
                    ModernButton {
                        iconSource: {
                            if (player.volume === 0) return "qrc:/icons/volume-x.svg"
                            else if (player.volume < 0.5) return "qrc:/icons/volume-1.svg"
                            else return "qrc:/icons/volume-2.svg"
                        }
                        size: 36
                        onClicked: {
                            if (player) {
                                if (player.volume > 0) {
                                    parent.previousVolume = player.volume
                                    player.volume = 0
                                } else {
                                    player.volume = parent.previousVolume
                                }
                            }
                        }
                    }
                    
                    ModernSlider {
                        id: volumeSlider
                        Layout.preferredWidth: 100
                        from: 0
                        to: 1
                        value: player.volume
                        onMoved: {
                            player.volume = value
                            if (value > 0) {
                                parent.previousVolume = value
                            }
                        }
                    }
                }
                
                // Effects button
                ModernButton {
                    iconSource: "qrc:/icons/zap.svg"
                    size: 40
                    highlighted: effectsPanel.visible
                    onClicked: effectsPanel.visible = !effectsPanel.visible
                    
                    ToolTip.text: "Audio Effects"
                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                }
                
                // Open file button
                ModernButton {
                    iconSource: "qrc:/icons/folder-open.svg"
                    size: 40
                    onClicked: fileDialog.open()
                    
                    ToolTip.text: "Open File (Ctrl+O)"
                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                }
            }
        }
    }
    
    function formatTime(seconds) {
        if (isNaN(seconds) || seconds < 0) return "0:00"
        
        var mins = Math.floor(seconds / 60)
        var secs = Math.floor(seconds % 60)
        return mins + ":" + (secs < 10 ? "0" : "") + secs
    }
}