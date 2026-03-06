import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    Theme { id: theme }

    property bool modalOpen: false

    Rectangle {
        anchors.fill: parent
        color: theme.bgColor
    }

    // =========================
    // TOP BAR (60 → 96)
    // =========================

    Rectangle {
        id: topBar
        width: parent.width
        height: 96
        color: theme.topBarColor

        Text {
            text: "SAC MODULE"
            anchors.left: parent.left
            anchors.leftMargin: 32
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 28
            font.bold: true
            color: theme.textColor
        }

        // MENU button
        StyledButton {
            text: LanguageManager.t("kafelek_menu")
            width: 192
            height: 64
            x: 16
            y: 16
            onClicked: root.modalOpen = !root.modalOpen
        }

        // VIN (mock)
        Text {
            text: "1234567"
            anchors.right: parent.right
            anchors.rightMargin: 32
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 26
            color: theme.textColor
        }

        // ONLINE status center
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            spacing: 12

            Rectangle {
                width: 16
                height: 16
                radius: 8
                color: "green"
            }

            Text {
                text: "ONLINE"
                font.pixelSize: 26
                color: "green"
            }
        }
    }

    // =========================
    // PARAMETER LIST
    // =========================

    Column {
        anchors.top: topBar.bottom
        anchors.topMargin: 32
        anchors.left: parent.left
        anchors.leftMargin: 64
        spacing: 20

        Repeater {
            model: 6

            Row {
                spacing: 400

                Text {
                    text: "Parameter " + (index + 1)
                    font.pixelSize: 26
                    color: theme.textColorPar
                }

                Text {
                    text: (Math.random()*10).toFixed(2) + " bar"
                    font.pixelSize: 26
                    color: theme.textColorPar
                }
            }
        }
    }

    // =========================
    // PAGINATION
    // =========================

    StyledButton {
        text: LanguageManager.t("kafelek_prev")
        width: 192
        height: 80
        x: 320
        y: 672
    }

    StyledButton {
        text: LanguageManager.t("kafelek_next")
        width: 192
        height: 80
        x: 768
        y: 672
    }

    // =========================
    // MODAL
    // =========================

    Rectangle {
        visible: root.modalOpen
        width: 360
        height: 320
        radius: 26
        color: "white"
        border.color: theme.borderColor

        x: 32
        y: 112

        Column {
            anchors.centerIn: parent
            spacing: 24

            StyledButton { text: LanguageManager.t("kafelek_dtc"); width: 280; height: 68 }
            StyledButton { text: LanguageManager.t("kafelek_activ"); width: 280; height: 68 }
            StyledButton {
                text: LanguageManager.t("kafelek_back")
                width: 280
                height: 68

                onClicked: {
                    root.modalOpen = false
                    CockpitController.disconnect()
                    SystemController.resetCAN("can0")
                    Navigation.push("MainMenu.qml")
                }
            }
        }
    }
}
