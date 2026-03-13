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

    SACTopBar {
        id: topBar
        width: parent.width
        height: 96
        onMenuClicked: root.modalOpen = !root.modalOpen
    }

    Column {
        anchors.top: topBar.bottom
        anchors.topMargin: 24
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 18

        Text {
            text: "PARAMETERS"
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 34
            font.bold: true
            color: theme.textColorDark
        }

        Rectangle {
            width: 980
            height: 420
            radius: 20
            color: theme.cardColor
            border.color: theme.separatorColor

            Column {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 18

                Repeater {
                    model: 6

                    Row {
                        width: 920
                        spacing: 260

                        Text {
                            width: 420
                            text: "Parameter " + (index + 1)
                            font.pixelSize: 26
                            color: theme.textColorDark
                        }

                        Text {
                            width: 220
                            text: (Math.random() * 10).toFixed(2) + " bar"
                            font.pixelSize: 26
                            color: theme.textColorMuted
                            horizontalAlignment: Text.AlignRight
                        }
                    }
                }
            }
        }
    }

    Row {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 28
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 40

        StyledButton {
            text: LanguageManager.t("kafelek_prev")
            width: 220
            height: 80
        }

        StyledButton {
            text: LanguageManager.t("kafelek_next")
            width: 220
            height: 80
        }
    }

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

            StyledButton {
                text: LanguageManager.t("kafelek_dtc")
                width: 280
                height: 68
                onClicked: {
                    root.modalOpen = false
                    Navigation.push("SACDTCPage.qml")
                }
            }

            StyledButton {
                text: LanguageManager.t("kafelek_activ")
                width: 280
                height: 68
            }

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
