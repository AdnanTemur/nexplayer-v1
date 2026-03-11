import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects

// Beautiful card for each effect
Rectangle {
    id: card
    
    property string title: "Effect"
    property string icon: ""  // Emoji fallback
    property string iconSource: ""  // NEW: SVG icon
    property bool enabled: false
    property alias content: contentLoader.sourceComponent
    
    Layout.fillWidth: true
    Layout.preferredHeight: column.height + 32
    
    radius: 12
    color: enabled ? Theme.accentTransparent : Theme.surfaceColor
    border.color: enabled ? Theme.accentColor : Theme.borderColor
    border.width: 1
    
    Behavior on color {
        ColorAnimation { duration: 200 }
    }
    
    Behavior on border.color {
        ColorAnimation { duration: 200 }
    }
    
    // Glow when enabled (using DropShadow instead of MultiEffect)
    layer.enabled: enabled
    layer.effect: DropShadow {
        horizontalOffset: 0
        verticalOffset: 4
        radius: 8
        samples: 17
        color: Qt.rgba(Theme.accentColor.r, Theme.accentColor.g, Theme.accentColor.b, 0.3)
    }
    
    Column {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 16
        spacing: 16
        
        // Header
        RowLayout {
            width: parent.width
            spacing: 12
            
            // Icon and title
            RowLayout {
                spacing: 10
                Layout.fillWidth: true
                
                // SVG Icon (if provided)
                Icon {
                    source: card.iconSource
                    width: 20
                    height: 20
                    color: card.enabled ? Theme.accentColor : Theme.textSecondary
                    visible: card.iconSource !== ""
                    
                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }
                }
                
                // Emoji fallback
                Text {
                    text: card.icon
                    font.pixelSize: 20
                    opacity: card.enabled ? 1.0 : 0.5
                    visible: card.iconSource === ""
                    
                    Behavior on opacity {
                        NumberAnimation { duration: 200 }
                    }
                }
                
                Text {
                    text: card.title
                    font.pixelSize: 15
                    font.weight: Font.Medium
                    color: card.enabled ? Theme.textColor : Theme.textSecondary
                    
                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }
                }
            }
            
            // Enable/disable switch
            Switch {
                id: enableSwitch
                checked: card.enabled
                
                onCheckedChanged: {
                    card.enabled = checked
                    player.enableEffect(card.title, checked)
                }
                
                indicator: Rectangle {
                    implicitWidth: 44
                    implicitHeight: 24
                    x: enableSwitch.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 12
                    color: enableSwitch.checked ? Theme.accentColor : Theme.surfaceColor
                    border.color: enableSwitch.checked ? Theme.accentHover : Theme.borderColor
                    border.width: 1
                    
                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }
                    
                    Behavior on border.color {
                        ColorAnimation { duration: 200 }
                    }
                    
                    Rectangle {
                        id: switchHandle
                        x: enableSwitch.checked ? parent.width - width - 3 : 3
                        y: 3
                        width: 18
                        height: 18
                        radius: 9
                        color: "white"
                        
                        Behavior on x {
                            NumberAnimation {
                                duration: 200
                                easing.type: Easing.OutCubic
                            }
                        }
                    }
                    
                    // Handle shadow
                    DropShadow {
                        anchors.fill: switchHandle
                        source: switchHandle
                        horizontalOffset: 0
                        verticalOffset: 1
                        radius: 4
                        samples: 9
                        color: "#00000030"
                    }
                }
            }
        }
        
        // Content (parameters)
        Loader {
            id: contentLoader
            width: parent.width
            visible: card.enabled
            opacity: card.enabled ? 1.0 : 0.0
            
            Behavior on opacity {
                NumberAnimation { duration: 200 }
            }
        }
    }
}