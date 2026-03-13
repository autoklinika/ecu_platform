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
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 96
        onMenuClicked: root.modalOpen = !root.modalOpen
    }

    Item {
        id: contentArea
        anchors.top: topBar.bottom
        anchors.bottom: bottomButtons.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 24
        anchors.rightMargin: 24
        anchors.topMargin: 0
        anchors.bottomMargin: 16

        Rectangle {
            width: 980
            height: 420
            anchors.centerIn: parent
            radius: 20
            color: theme.cardColor
            border.color: theme.separatorColor

            Column {
                anchors.centerIn: parent
                width: parent.width - 60
                spacing: 18

                Repeater {
                    model: 6

                    Row {
                        width: parent.width
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
        id: bottomButtons
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
        y: topBar.height + 16

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
                    Navigation.goHome()
                }
            }
        }
    }
}
