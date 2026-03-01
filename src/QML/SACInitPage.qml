import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {

    property int speed: 250
    property bool connectionError: false

    Column {
        anchors.centerIn: parent
        spacing: 40

        Text {
            text: "Connecting to ECU..."
            font.pixelSize: 36
            font.bold: true
            color: "#2A2A2A"
        }

        Text {
            text: speed + " kbps"
            font.pixelSize: 28
            color: "#666666"
        }

        BusyIndicator {
            running: true
            width: 60
            height: 60
        }

        Text {
            visible: connectionError
            text: "Connection failed"
            color: "red"
            font.pixelSize: 22
        }
    }

    Timer {
        interval: 2000
        running: true
        repeat: false
        onTriggered: {
            if (!connectionError)
                Navigation.push("SACMainPage.qml")
        }
    }
}
