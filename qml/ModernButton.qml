import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    
    property string iconText: ""
    property string iconSource: ""
    property int size: 40
    property bool isPrimary: false
    property bool highlighted: false
    property alias hovered: button.hovered
    property alias pressed: button.pressed
    
    signal clicked()
    
    width: size
    height: size
    
    Button {
        id: button
        anchors.fill: parent
        
        onClicked: root.clicked()
        
        contentItem: Item {
            // Icon (SVG)
            Icon {
                anchors.centerIn: parent
                source: root.iconSource
                size: root.size * 0.5
                color: {
                    if (root.isPrimary) {
                        return "white"
                    } else if (root.highlighted) {
                        return Theme.accentColor
                    } else {
                        return Theme.textColor
                    }
                }
                visible: root.iconSource !== ""
            }
            
            // Fallback to emoji/text
            Text {
                anchors.centerIn: parent
                text: root.iconText
                font.pixelSize: root.size * 0.45
                color: {
                    if (root.isPrimary) {
                        return "white"
                    } else if (root.highlighted) {
                        return Theme.accentColor
                    } else {
                        return Theme.textColor
                    }
                }
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                visible: root.iconSource === ""
                
                Behavior on color {
                    ColorAnimation { duration: 150 }
                }
            }
        }
        
        background: Rectangle {
            color: {
                if (root.isPrimary) {
                    return button.pressed ? Theme.accentActive : 
                           button.hovered ? Theme.accentHover : Theme.accentColor
                } else if (root.highlighted) {
                    return button.pressed ? Theme.accentTransparent :
                           button.hovered ? Theme.accentTransparent : "transparent"
                } else {
                    return button.pressed ? Theme.buttonActiveColor :
                           button.hovered ? Theme.buttonHoverColor : "transparent"
                }
            }
            radius: root.size * 0.2
            border.color: root.highlighted ? Theme.accentColor : "transparent"
            border.width: root.highlighted ? 2 : 0
            
            Behavior on color {
                ColorAnimation { duration: 150 }
            }
        }
    }
}