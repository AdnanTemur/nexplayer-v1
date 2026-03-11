import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects

Rectangle {
    id: panel

     // Store effect states
    property var eqBands: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    property real reverbRoomSize: 0.5
    property real reverbDamping: 0.0
    property real reverbWetLevel: 0.15
    property real reverbDryLevel: 0.7
    property real delayTime: 900
    property real delayFeedback: 0.35
    property real delayMix: 0.35
    
    property bool eqEnabled: true
    property bool reverbEnabled: false
    property bool delayEnabled: false
    
    color: Theme.cardColor

    // Reapply when file changes
    Connections {
        target: player
        function onCurrentFileChanged() {
            console.log("File changed, reapplying effects...")
            reapplyTimer.restart()
        }
    }
    
    Timer {
        id: reapplyTimer
        interval: 100
        onTriggered: {
            console.log("Reapplying all effect settings...")
            
            // Reapply effect enable/disable states
            player.enableEffect("Equalizer", eqEnabled)
            player.enableEffect("Reverb", reverbEnabled)
            player.enableEffect("Delay", delayEnabled)
            
            // Reapply EQ bands
            for (var i = 0; i < 10; i++) {
                player.setEQBand(i, eqBands[i])
            }
            
            // Reapply Reverb parameters
            player.setReverbRoomSize(reverbRoomSize)
            player.setReverbDamping(reverbDamping)
            player.setReverbWetLevel(reverbWetLevel)
            player.setReverbDryLevel(reverbDryLevel)
            
            // Reapply Delay parameters
            player.setDelayTime(delayTime)
            player.setDelayFeedback(delayFeedback)
            player.setDelayMix(delayMix)
            
            console.log("Effects reapplied!")
        }
    }
    
    Behavior on color {
        ColorAnimation { duration: 200 }
    }
    
    // Left border
    Rectangle {
        anchors.left: parent.left
        width: 1
        height: parent.height
        color: Theme.borderColor
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            
            Icon {
                source: "qrc:/icons/zap.svg"
                width: 24
                height: 24
                color: Theme.accentColor
            }
            
            Text {
                text: "Audio Effects"
                font.pixelSize: 20
                font.bold: true
                color: Theme.textColor
                Layout.fillWidth: true
            }
            
            Rectangle {
                width: 32
                height: 32
                radius: 6
                color: closeMouseArea.containsMouse ? Theme.buttonHoverColor : "transparent"
                
                Behavior on color {
                    ColorAnimation { duration: 150 }
                }
                
                Text {
                    anchors.centerIn: parent
                    text: "×"
                    font.pixelSize: 24
                    color: Theme.textSecondary
                }
                
                MouseArea {
                    id: closeMouseArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onClicked: panel.visible = false
                }
            }
        }
        
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Theme.dividerColor
        }
        
        // Effects list
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AsNeeded
            
            ColumnLayout {
                width: panel.width - 40
                spacing: 16
                
                // Equalizer
               EffectCard {
                    title: "Equalizer"
                    iconSource: "qrc:/icons/sliders.svg"
                    enabled: panel.eqEnabled  // Bind to stored state
                    
                    // Update stored state when toggled
                    Connections {
                        target: this  // The card itself
                        function onEnabledChanged() {
                            panel.eqEnabled = enabled
                        }
                    }
                    
                    content: ColumnLayout {
                        spacing: 12
                        
                        Repeater {
                            model: [
                                {freq: "32Hz", value: 0},
                                {freq: "64Hz", value: 0},
                                {freq: "125Hz", value: 0},
                                {freq: "250Hz", value: 0},
                                {freq: "500Hz", value: 0},
                                {freq: "1kHz", value: 0},
                                {freq: "2kHz", value: 0},
                                {freq: "4kHz", value: 0},
                                {freq: "8kHz", value: 0},
                                {freq: "16kHz", value: 0}
                            ]
                            
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 12
                                
                                Text {
                                    text: modelData.freq
                                    font.pixelSize: 11
                                    color: Theme.textSecondary
                                    Layout.preferredWidth: 50
                                }
                                
                                ModernSlider {
                                    Layout.fillWidth: true
                                    from: -12
                                    to: 12
                                    value: panel.eqBands[index]  // Read from stored state
                                    onMoved: {
                                        panel.eqBands[index] = value  // Store it
                                        player.setEQBand(index, value)
                                    }
                                }
                                
                                Text {
                                    text: value.toFixed(1) + "dB"
                                    font.pixelSize: 11
                                    color: Theme.textColor
                                    Layout.preferredWidth: 45
                                    
                                    property real value: 0
                                    Component.onCompleted: {
                                        value = Qt.binding(() => parent.children[1].value)
                                    }
                                }
                            }
                        }
                    }
                }
                
                // Reverb
                EffectCard {
                    title: "Reverb"
                    iconSource: "qrc:/icons/waves.svg"  // We'll create this
                    enabled: false
                    
                    content: ColumnLayout {
                        spacing: 16
                        
                        ParameterControl {
                            label: "Room Size"
                            from: 0
                            to: 1
                            value: 0.5
                            onValueChanged: player.setReverbRoomSize(value)
                        }
                        
                        ParameterControl {
                            label: "Damping"
                            from: 0
                            to: 1
                            value: 0.0
                            onValueChanged: player.setReverbDamping(value)
                        }
                        
                        ParameterControl {
                            label: "Wet Level"
                            from: 0
                            to: 1
                            value: 0.15
                            onValueChanged: player.setReverbWetLevel(value)
                        }
                        
                        ParameterControl {
                            label: "Dry Level"
                            from: 0
                            to: 1
                            value: 0.7
                            onValueChanged: player.setReverbDryLevel(value)
                        }
                    }
                }
                
                // Delay
                EffectCard {
                    title: "Delay"
                    iconSource: "qrc:/icons/repeat.svg"
                    enabled: false
                    
                    content: ColumnLayout {
                        spacing: 16
                        
                        ParameterControl {
                            label: "Time"
                            from: 1
                            to: 2000
                            value: 900
                            suffix: " ms"
                            onValueChanged: player.setDelayTime(value)
                        }
                        
                        ParameterControl {
                            label: "Feedback"
                            from: 0
                            to: 0.95
                            value: 0.35
                            onValueChanged: player.setDelayFeedback(value)
                        }
                        
                        ParameterControl {
                            label: "Mix"
                            from: 0
                            to: 1
                            value: 0.35
                            onValueChanged: player.setDelayMix(value)
                        }
                    }
                }
                
                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }
}