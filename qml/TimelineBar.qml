import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects

// Modern timeline/seek bar
Item {
    id: timeline
    height: 40
    
    property real position: player.position
    property real duration: player.duration > 0 ? player.duration : 1
    property real progress: position / duration
    
    // Background track
    Rectangle {
        id: track
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: 6
        radius: 3
        color: Theme.surfaceColor  // CHANGED
        border.color: Theme.borderColor  // CHANGED
        border.width: 1
        
        Behavior on color {
            ColorAnimation { duration: 200 }
        }
        
        // Progress fill - NOW USES THEME
        Rectangle {
            id: progressBar
            height: parent.height
            width: parent.width * timeline.progress
            radius: parent.radius
            color: Theme.accentColor  // CHANGED - Solid color
            
            Behavior on width {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.OutQuad
                }
            }
            
            Behavior on color {
                ColorAnimation { duration: 200 }
            }
        }
        
        // Buffer indicator (placeholder)
        Rectangle {
            height: parent.height - 2
            width: parent.width * 0.7
            anchors.verticalCenter: parent.verticalCenter
            x: 1
            radius: parent.radius
            color: Theme.borderColor  // CHANGED
            opacity: 0.3
            visible: false  // Enable when buffering is implemented
        }
    }
    
    // Hover preview - NOW USES THEME
    Rectangle {
        id: hoverPreview
        visible: mouseArea.containsMouse
        x: Math.max(0, Math.min(parent.width - width, mouseArea.mouseX - width / 2))
        y: -height - 8
        width: 80
        height: 32
        radius: 8
        color: Theme.cardColor  // CHANGED
        border.color: Theme.borderColor  // CHANGED
        border.width: 1
        
        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 0
            verticalOffset: 2
            radius: 8
            samples: 17
            color: Theme.shadowColor
        }
        
        Text {
            anchors.centerIn: parent
            text: formatHoverTime(mouseArea.mouseX / timeline.width * timeline.duration)
            font.pixelSize: 12
            color: Theme.textColor  // CHANGED
            
            Behavior on color {
                ColorAnimation { duration: 200 }
            }
        }
        
        // Arrow pointing down - NOW USES THEME
        Canvas {
            anchors.top: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width: 12
            height: 6
            
            onPaint: {
                var ctx = getContext("2d")
                ctx.fillStyle = Theme.cardColor  // CHANGED
                ctx.beginPath()
                ctx.moveTo(0, 0)
                ctx.lineTo(width / 2, height)
                ctx.lineTo(width, 0)
                ctx.closePath()
                ctx.fill()
            }
            
            // Repaint when theme changes
            Connections {
                target: Theme
                function onIsDarkModeChanged() {
                    parent.requestPaint()
                }
            }
        }
    }
    
    // Seek handle
    Rectangle {
        id: handle
        x: (parent.width - width) * timeline.progress
        anchors.verticalCenter: track.verticalCenter
        width: 16
        height: 16
        radius: 8
        color: "#ffffff"
        visible: mouseArea.containsMouse || mouseArea.pressed
        
        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 0
            verticalOffset: 2
            radius: 8
            samples: 17
            color: "#40000000"
        }
        
        Behavior on x {
            enabled: !mouseArea.pressed
            NumberAnimation {
                duration: 100
                easing.type: Easing.OutQuad
            }
        }
        
        scale: mouseArea.pressed ? 1.2 : 1
        Behavior on scale {
            NumberAnimation { duration: 100 }
        }
    }
    
    // Mouse interaction
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        
        onClicked: {
            var newPosition = (mouse.x / width) * timeline.duration
            player.seek(newPosition)
        }
        
        onPositionChanged: {
            if (pressed) {
                var newPosition = (mouse.x / width) * timeline.duration
                player.seek(newPosition)
            }
        }
    }
    
    function formatHoverTime(seconds) {
        if (isNaN(seconds) || seconds < 0) return "0:00"
        var mins = Math.floor(seconds / 60)
        var secs = Math.floor(seconds % 60)
        return mins + ":" + (secs < 10 ? "0" : "") + secs
    }
}