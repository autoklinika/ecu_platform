import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    visible: true
    title: "ECU Tester"

    Theme { id: theme }

    background: Rectangle {
        color: theme.bgColor
    }

    property date currentDateTime: new Date()

    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: currentDateTime = new Date()
    }

    // 📅 DATA
    Text {
        visible: stack.depth === 1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 20
        anchors.topMargin: 15
        text: Qt.formatDate(currentDateTime, "dd.MM.yyyy")
        font.pixelSize: 22
        font.bold: true
        color: "#444444"
    }

    // 🏷 NAZWA
    Text {
        visible: stack.depth === 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 15
        text: "ECU Bench Platform"
        font.pixelSize: 24
        font.bold: true
        color: "#2A2A2A"
    }

    // 🕒 GODZINA
    Text {
        visible: stack.depth === 1
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 20
        anchors.topMargin: 15
        text: Qt.formatTime(currentDateTime, "HH:mm:ss")
        font.pixelSize: 22
        font.bold: true
        color: "#444444"
    }

    StackView {
        id: stack
        anchors.fill: parent
        anchors.topMargin: 60
        initialItem: "MainMenu.qml"
    }

    Component.onCompleted: {
        Navigation.stack = stack
    }
    Rectangle {
    width: 80
    height: 80
    anchors.left: parent.left
    anchors.bottom: parent.bottom
    color: "transparent"
    z: 9999

    MouseArea {
        anchors.fill: parent
        onPressAndHold: {
            root.visibility = Window.Windowed
        }
    }
}
}
