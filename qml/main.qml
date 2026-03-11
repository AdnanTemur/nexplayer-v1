import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects

ApplicationWindow {
    id: root
    visible: true
    width: Settings.windowWidth
    height: Settings.windowHeight
    minimumWidth: 800
    minimumHeight: 600
    title: "NexPlayer"
    color: Theme.backgroundColor

    Component.onCompleted: {
        console.log("NexPlayer started");
        console.log("Theme:", Theme.isDarkMode ? "Dark" : "Light");
        console.log("Accent Color:", Theme.accentColor);

        if (player) {
            player.volume = Settings.volume
            console.log("Applied saved volume:", Settings.volume)
        }
    }

    onClosing: {
        Settings.windowWidth = width
        Settings.windowHeight = height
    }

    // Settings popup (accessible from AdvancedControls)
    SettingsPopup {
        id: settingsPopup
    }

    // Main container
    Rectangle {
        anchors.fill: parent
        color: Theme.backgroundColor

        Behavior on color {
            ColorAnimation { duration: 300 }
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            VideoDisplay {
                id: videoDisplay
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 300
            }

            AdvancedControls {
                id: advancedControls
                Layout.fillWidth: true
                Layout.preferredHeight: 48
            }

            ControlBar {
                id: controlBar
                Layout.fillWidth: true
                Layout.preferredHeight: 120
            }
        }

        EffectsPanel {
            id: effectsPanel
            x: parent.width - width
            y: 0
            height: parent.height - controlBar.height
            width: 360
            visible: false

            Behavior on x {
                NumberAnimation {
                    duration: 300
                    easing.type: Easing.OutCubic
                }
            }
        }
        
        Toast {
            id: toast
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 200
        }

        Connections {
            target: player
            function onShowNotification(icon, message) {
                toast.show(icon, message, 3000);
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Open Media File"
        nameFilters: ["Media files (*.mp4 *.mkv *.avi *.mov *.mp3 *.flac *.wav)", "All files (*)"]
        onAccepted: {
            player.openFile(fileDialog.selectedFile);
        }
    }

    Shortcut {
        sequence: "Space"
        onActivated: player.togglePlayPause()
    }

    Shortcut {
        sequence: "F"
        onActivated: root.visibility = (root.visibility === Window.FullScreen) ? Window.Windowed : Window.FullScreen
    }

    Shortcut {
        sequence: StandardKey.Open
        onActivated: fileDialog.open()
    }
}