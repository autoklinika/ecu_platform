import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    visible: true
    visibility: Window.FullScreen
    flags: Qt.FramelessWindowHint
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

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: "MainMenu.qml"
        z: 1
    }

    Component.onCompleted: {
        Navigation.stack = stack
    }

    Text {
        visible: stack.depth === 1
        z: 10
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 20
        anchors.topMargin: 15
        text: Qt.formatDate(currentDateTime, "dd.MM.yyyy")
        font.pixelSize: 22
        font.bold: true
        color: "#444444"
    }

    Text {
        visible: stack.depth === 1
        z: 10
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 15
        text: "ECU Bench Platform"
        font.pixelSize: 24
        font.bold: true
        color: "#2A2A2A"
    }

    Item {
        id: wifiIcon
        visible: stack.depth === 1
        z: 10

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 140
        anchors.topMargin: 10

        width: 40
        height: 30

        property string wifiState: SystemController.wifiStatus()

        Timer {
            interval: 3000
            running: true
            repeat: true
            onTriggered: wifiIcon.wifiState = SystemController.wifiStatus()
        }

        property color iconColor: {
            if (wifiState === "connected")
                return "#23c936"
            if (wifiState === "connecting")
                return "#ffb300"
            return "#d41717"
        }

        SequentialAnimation on opacity {
            running: wifiIcon.wifiState === "connecting"
            loops: Animation.Infinite

            NumberAnimation {
                from: 1.0
                to: 0.4
                duration: 600
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                from: 0.4
                to: 1.0
                duration: 600
                easing.type: Easing.InOutQuad
            }
        }

        Canvas {
            id: canvas
            anchors.fill: parent

            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)

                ctx.strokeStyle = wifiIcon.iconColor
                ctx.fillStyle = wifiIcon.iconColor
                ctx.lineWidth = 3

                var centerX = width / 2
                var bottomY = height - 2

                ctx.beginPath()
                ctx.arc(centerX, bottomY, 14, Math.PI, 2 * Math.PI)
                ctx.stroke()

                ctx.beginPath()
                ctx.arc(centerX, bottomY, 9, Math.PI, 2 * Math.PI)
                ctx.stroke()

                ctx.beginPath()
                ctx.arc(centerX, bottomY, 4, Math.PI, 2 * Math.PI)
                ctx.stroke()

                ctx.beginPath()
                ctx.arc(centerX, bottomY, 2, 0, 2 * Math.PI)
                ctx.fill()
            }
        }

        onIconColorChanged: canvas.requestPaint()
    }

    Text {
        visible: stack.depth === 1
        z: 10
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 20
        anchors.topMargin: 15
        text: Qt.formatTime(currentDateTime, "HH:mm:ss")
        font.pixelSize: 22
        font.bold: true
        color: "#444444"
    }
}
