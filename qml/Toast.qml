import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects

// Toast notification component
Rectangle {
    id: toast
    width: Math.min(parent.width * 0.8, 500)
    height: toastText.height + 32
    radius: 12
    color: "#1a1a1a"
    border.color: "#6366f1"
    border.width: 2
    opacity: 0
    z: 1000
    
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 100
    
    // Shadow effect
    layer.enabled: true
    layer.effect: DropShadow {
        horizontalOffset: 0
        verticalOffset: 4
        radius: 16
        samples: 33
        color: "#80000000"
    }
    
    // Content
    Row {
        anchors.centerIn: parent
        spacing: 12
        
        // Icon
        Text {
            id: toastIcon
            font.pixelSize: 24
            color: "#6366f1"
            anchors.verticalCenter: parent.verticalCenter
        }
        
        // Message
        Text {
            id: toastText
            font.pixelSize: 14
            color: "#ffffff"
            wrapMode: Text.Wrap
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    
    // Show animation
    NumberAnimation {
        id: showAnimation
        target: toast
        property: "opacity"
        to: 1
        duration: 300
        easing.type: Easing.OutCubic
    }
    
    // Hide animation
    NumberAnimation {
        id: hideAnimation
        target: toast
        property: "opacity"
        to: 0
        duration: 300
        easing.type: Easing.InCubic
    }
    
    // Auto-hide timer
    Timer {
        id: hideTimer
        interval: 3000
        onTriggered: hideAnimation.start()
    }
    
    // Public function to show toast
    function show(icon, message, duration) {
        toastIcon.text = icon
        toastText.text = message
        hideTimer.interval = duration || 3000
        
        showAnimation.start()
        hideTimer.restart()
    }
}