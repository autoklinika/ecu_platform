import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {

    property int speed: 250
    property string canInterface: "can0"
    property int connectTimeoutMs: 5000
    property bool connectionError: false
    property bool started: false
    property int elapsedMs: 0

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

        StyledButton {
            visible: connectionError
            width: 240
            height: 90
            text: LanguageManager.t("kafelek_back")
            onClicked: {
                CockpitController.disconnect()
                SystemController.resetCAN(canInterface)
                Navigation.pop()
            }
        }
    }

    Timer {
        interval: 150
        running: true
        repeat: true

        onTriggered: {
            if (!started) {
                SystemController.configureCAN(canInterface, speed * 1000)
                if (CockpitController.start(canInterface, speed * 1000))
                    started = true
            }

            elapsedMs += interval

            if (CockpitController.ecuReady) {
                Navigation.push("SACMainPage.qml", {
                    vin: CockpitController.vin,
                    sw: CockpitController.sw,
                    hw: CockpitController.hw
                })
                running = false
                return
            }

            if (elapsedMs >= connectTimeoutMs) {
                connectionError = true
                CockpitController.disconnect()
                SystemController.resetCAN(canInterface)
                running = false
                return
            }

            if (CockpitController.error.length > 0) {
                connectionError = true
                CockpitController.disconnect()
                SystemController.resetCAN(canInterface)
                running = false
            }
        }
    }
}
