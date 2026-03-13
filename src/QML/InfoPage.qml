import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    Theme { id: theme }

    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -20
        spacing: 26

        Text {
            text: "ECU Bench Platform"
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 42
            font.bold: true
            color: theme.textColorDark
        }

        Rectangle {
            width: 760
            height: 220
            radius: 20
            color: theme.cardColor
            border.color: theme.separatorColor

            Column {
                anchors.centerIn: parent
                spacing: 18

                Text {
                    text: "Version: 0.1"
                    font.pixelSize: 28
                    color: theme.textColorMuted
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    text: "Diagnostic / Bench GUI"
                    font.pixelSize: 24
                    color: theme.textColorMuted
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    StyledButton {
        width: 240
        height: 90
        text: LanguageManager.t("kafelek_back")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        onClicked: Navigation.pop()
    }
}
