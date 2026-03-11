import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects

Rectangle {
    id: videoArea
    color: Theme.backgroundColor
    
    Behavior on color {
        ColorAnimation { duration: 200 }
    }
    
    // Video renderer
    Item {
        id: videoContainer
        anchors.fill: parent
        
        Component.onCompleted: {
            if (player.videoRenderer) {
                player.videoRenderer.parent = videoContainer
                player.videoRenderer.anchors.fill = videoContainer
                console.log("Video renderer connected!")
            }
        }
    }
    
    // Placeholder when no video
    Rectangle {
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.4, 400)
        height: width
        color: "transparent"
        visible: !player.currentFile
        
        Column {
            anchors.centerIn: parent
            spacing: 20
            
            Rectangle {
                width: 120
                height: 120
                radius: 60
                anchors.horizontalCenter: parent.horizontalCenter
                color: Theme.accentColor
                
                Text {
                    anchors.centerIn: parent
                    text: "▶"
                    font.pixelSize: 48
                    font.bold: true
                    color: "white"
                }
                
                layer.enabled: true
                layer.effect: DropShadow {
                    horizontalOffset: 0
                    verticalOffset: 4
                    radius: 16
                    samples: 33
                    color: Qt.rgba(Theme.accentColor.r, Theme.accentColor.g, Theme.accentColor.b, 0.4)
                }
            }
            
            Text {
                text: "NexPlayer"
                font.pixelSize: 32
                font.bold: true
                color: Theme.textColor
                anchors.horizontalCenter: parent.horizontalCenter
            }
            
            Text {
                text: "Drop a file or press Ctrl+O to begin"
                font.pixelSize: 14
                color: Theme.textSecondary
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        onDoubleClicked: {
            if (ApplicationWindow.window.visibility === Window.FullScreen) {
                ApplicationWindow.window.visibility = Window.Windowed
            } else {
                ApplicationWindow.window.visibility = Window.FullScreen
            }
        }
        
        onClicked: {
            player.togglePlayPause()
        }
    }
}