import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// Reusable parameter control with label and slider
ColumnLayout {
    id: control
    
    property string label: "Parameter"
    property real from: 0
    property real to: 1
    property real value: 0.5
    property string suffix: ""
    property int decimals: 2
    
    signal parameterChanged(real value)
    
    Layout.fillWidth: true
    spacing: 8
    
    // Label and value
    RowLayout {
        Layout.fillWidth: true
        
        Text {
            text: control.label
            font.pixelSize: 12
            color: "#cccccc"
            Layout.fillWidth: true
        }
        
        Text {
            text: control.value.toFixed(control.decimals) + control.suffix
            font.pixelSize: 12
            font.bold: true
            color: "#ffffff"
        }
    }
    
    // Slider
    ModernSlider {
        id: slider
        Layout.fillWidth: true
        from: control.from
        to: control.to
        value: control.value
        
        onMoved: {
            control.value = value
            control.parameterChanged(value)
        }
    }
}
