import QtQuick 2.15
import Qt5Compat.GraphicalEffects

Item {
    id: root
    
    property string source: ""
    property color color: Theme.textColor
    property int size: 24
    
    width: size
    height: size
    
    Image {
        id: iconImage
        anchors.fill: parent
        source: root.source
        sourceSize: Qt.size(root.size, root.size)
        fillMode: Image.PreserveAspectFit
        visible: false
    }
    
    ColorOverlay {
        anchors.fill: iconImage
        source: iconImage
        color: root.color
        
        Behavior on color {
            ColorAnimation { duration: 150 }
        }
    }
}