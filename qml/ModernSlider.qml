import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects

// Modern slider with gradient and smooth animations
Slider {
    id: slider
    
    from: 0
    to: 1
    value: 0.5
    
    background: Rectangle {
        x: slider.leftPadding
        y: slider.topPadding + slider.availableHeight / 2 - height / 2
        implicitWidth: 100
        implicitHeight: 6
        width: slider.availableWidth
        height: implicitHeight
        radius: 3
        color: Theme.borderColor  // CHANGED
        
        Behavior on color {
            ColorAnimation { duration: 200 }
        }
        
        Rectangle {
            width: slider.visualPosition * parent.width
            height: parent.height
            radius: parent.radius
            color: Theme.accentColor  // CHANGED - Solid color now
            
            Behavior on color {
                ColorAnimation { duration: 200 }
            }
        }
    }
    
    handle: Rectangle {
        x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
        y: slider.topPadding + slider.availableHeight / 2 - height / 2
        implicitWidth: 16
        implicitHeight: 16
        radius: 8
        color: "#ffffff"
        visible: slider.hovered || slider.pressed
        
        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 0
            verticalOffset: 2
            radius: 8
            samples: 17
            color: "#40000000"
        }
        
        scale: slider.pressed ? 1.2 : 1
        Behavior on scale {
            NumberAnimation { duration: 100 }
        }
    }
    
    // Smooth value changes
    Behavior on value {
        NumberAnimation {
            duration: 50
            easing.type: Easing.OutQuad
        }
    }
}