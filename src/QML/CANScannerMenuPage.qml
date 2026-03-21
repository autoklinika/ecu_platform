import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    Theme { id: theme }

    Rectangle {
        anchors.fill: parent
        color: theme.bgColor
    }

    Rectangle {
        id: mainPanel
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: bottomBar.top
        anchors.margins: 16
        anchors.bottomMargin: 14
        radius: 18
        color: theme.cardColor
        border.color: theme.separatorColor
        border.width: 1

        Column {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 18

            Text {
                text: "CAN SCANNER"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 30
                font.bold: true
                color: theme.textColorDark
            }

            Text {
                text: "Wybierz funkcję diagnostyczną"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 18
                color: theme.textColorMuted
            }

            Item {
                width: 1
                height: 8
            }

            Grid {
                id: tileGrid
                anchors.horizontalCenter: parent.horizontalCenter
                rows: 2
                columns: 2
                rowSpacing: 22
                columnSpacing: 22

                StyledButton {
                    width: 300
                    height: 170
                    text: "BITRATE SCAN"
                    onClicked: Navigation.push("CANScannerBitratePage.qml")
                }

                StyledButton {
                    width: 300
                    height: 170
                    text: "ECU SCAN"
                    onClicked: Navigation.push("CANScannerECUPage.qml")
                }

                StyledButton {
                    width: 300
                    height: 170
                    text: "LOG / RESULTS"
                    onClicked: Navigation.push("CANScannerLogPage.qml")
                }

                StyledButton {
                    width: 300
                    height: 170
                    text: "PARAMETERS"
                    onClicked: Navigation.push("CANScannerParametersPage.qml")
                }
            }
        }
    }

    Row {
        id: bottomBar
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 16
        spacing: 16

        StyledButton {
            width: 150
            height: 68
            text: "BACK"
            onClicked: Navigation.pop()
        }
    }
}