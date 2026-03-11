import QtQuick 2.15
import QtQuick.Dialogs

FileDialog {
    id: fileDialog
    title: "Open Media File"
    nameFilters: [
        "All Media Files (*.mp4 *.mkv *.avi *.mov *.wmv *.flv *.webm *.mp3 *.flac *.wav *.aac *.ogg *.m4a)",
        "Video Files (*.mp4 *.mkv *.avi *.mov *.wmv *.flv *.webm)",
        "Audio Files (*.mp3 *.flac *.wav *.aac *.ogg *.m4a)",
        "All Files (*)"
    ]
}