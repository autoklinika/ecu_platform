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

    CANScannerTopBar {
        id: topBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        titleText: "PARAMETERS"
        leftButtonText: "BACK"
        rightText: ""
        onMenuClicked: Navigation.pop()
    }

    Rectangle {
        id: panel
        anchors.top: topBar.bottom
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
            spacing: 12

            Text {
                text: "BITRATE SCAN PARAMETERS"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 28
                font.bold: true
                color: theme.textColorDark
            }

            Text {
                text: "Ustawienia przygotowane pod osobny ekran parametrów"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 18
                color: theme.textColorMuted
            }

            Rectangle {
                width: parent.width
                height: 58
                radius: 10
                color: "#F6F6F6"
                border.color: "#DDDDDD"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    width: parent.width * 0.45
                    text: "INTERFACE"
                    font.pixelSize: 22
                    color: theme.textColorDark
                }

                TextField {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    width: 260
                    height: 42
                    text: "can0"
                    font.pixelSize: 22
                }
            }

            Rectangle {
                width: parent.width
                height: 58
                radius: 10
                color: "#FFFFFF"
                border.color: "#DDDDDD"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    width: parent.width * 0.45
                    text: "DETECTION MODE"
                    font.pixelSize: 22
                    color: theme.textColorDark
                }

                ComboBox {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    width: 260
                    height: 42
                    model: ["auto", "listen_only", "probe_only"]
                    font.pixelSize: 22
                }
            }

            Rectangle {
                width: parent.width
                height: 58
                radius: 10
                color: "#F6F6F6"
                border.color: "#DDDDDD"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    width: parent.width * 0.45
                    text: "TESTER SA (DEC)"
                    font.pixelSize: 22
                    color: theme.textColorDark
                }

                TextField {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    width: 260
                    height: 42
                    text: "241"
                    font.pixelSize: 22
                }
            }

            Rectangle {
                width: parent.width
                height: 58
                radius: 10
                color: "#FFFFFF"
                border.color: "#DDDDDD"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    width: parent.width * 0.45
                    text: "LISTEN TIME [ms]"
                    font.pixelSize: 22
                    color: theme.textColorDark
                }

                TextField {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    width: 260
                    height: 42
                    text: "800"
                    font.pixelSize: 22
                }
            }

            Rectangle {
                width: parent.width
                height: 58
                radius: 10
                color: "#F6F6F6"
                border.color: "#DDDDDD"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    width: parent.width * 0.45
                    text: "ONLY EXTENDED ID"
                    font.pixelSize: 22
                    color: theme.textColorDark
                }

                CheckBox {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    checked: true
                    text: checked ? "ON" : "OFF"
                    font.pixelSize: 22

                    indicator.width: 28
                    indicator.height: 28
                }
            }

            Rectangle {
                width: parent.width
                height: 70
                radius: 10
                color: "#F8F8F8"
                border.color: "#DDDDDD"

                Text {
                    anchors.centerIn: parent
                    text: "Na tym etapie ekran parametrów jest tylko wizualny."
                    font.pixelSize: 20
                    font.bold: true
                    color: theme.textColorDark
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
            width: 180
            height: 68
            text: "BACK"
            onClicked: Navigation.pop()
        }
    }
}