import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    property int speed: 250
    property string canInterface: "can0"
    property int connectTimeoutMs: 5000
    property bool connectionError: false
    property bool started: false
    property bool canPrepared: false
    property int elapsedMs: 0

    Theme { id: theme }

    Rectangle {
        anchors.fill: parent
        color: theme.bgColor
    }

    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -20
        spacing: 28

        Text {
            text: "CONNECTING TO ECU..."
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 40
            font.bold: true
            color: theme.textColorDark
        }

        Text {
            text: speed + " kbps"
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 28
            color: theme.textColorMuted
        }

        BusyIndicator {
            anchors.horizontalCenter: parent.horizontalCenter
            running: !connectionError
            visible: !connectionError
            width: 60
            height: 60
        }

        Text {
            visible: connectionError
            text: "Connection failed"
            anchors.horizontalCenter: parent.horizontalCenter
            color: "red"
            font.pixelSize: 24
        }
    }

    StyledButton {
        visible: connectionError
        width: 240
        height: 90
        text: LanguageManager.t("kafelek_back")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        onClicked: {
            CockpitController.disconnect()
            SystemController.resetCAN(canInterface)
            Navigation.pop()
        }
    }

    Timer {
        interval: 150
        running: true
        repeat: true

        onTriggered: {
            if (!started) {
                if (!canPrepared) {
                    SystemController.configureCAN(canInterface, speed * 1000)
                    canPrepared = true
                }

                if (CockpitController.start(canInterface, speed * 1000))
                    started = true

                return
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

            if (elapsedMs >= 1000 && CockpitController.error.length > 0) {
                connectionError = true
                CockpitController.disconnect()
                SystemController.resetCAN(canInterface)
                running = false
            }
        }
    }
}
