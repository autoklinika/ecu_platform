import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    Theme { id: theme }

    function toIntAuto(textValue) {
        var s = String(textValue).trim()
        if (s.length === 0)
            return 0

        if (s.indexOf("0x") === 0 || s.indexOf("0X") === 0)
            return parseInt(s, 16)

        if (/[A-Fa-f]/.test(s))
            return parseInt(s, 16)

        return parseInt(s, 10)
    }

    function formatHexByte(value) {
        var n = Number(value)
        if (isNaN(n))
            n = 0
        var s = n.toString(16).toUpperCase()
        if (s.length < 2)
            s = "0" + s
        return "0x" + s
    }

    function saveParameters() {
        CANScanner.bitrateInterface = ifaceField.text
        CANScanner.bitrateDetectMode = detectMode.currentText
        CANScanner.bitrateTesterSa = toIntAuto(testerSa.text)
        CANScanner.bitrateListenMs = parseInt(listenMs.text, 10)
        CANScanner.bitrateExtendedOnly = extOnly.checked
        infoText.text = "Parameters saved"
    }

    Rectangle {
        anchors.fill: parent
        color: theme.bgColor
    }

    Rectangle {
        id: panel
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
            anchors.margins: 16
            spacing: 12

            Text {
                text: "BITRATE PARAMETERS"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 28
                font.bold: true
                color: theme.textColorDark
            }

            Text {
                text: "Ustawienia wspólne dla skanowania bitrate"
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
                    width: parent.width * 0.42
                    text: "INTERFACE"
                    font.pixelSize: 22
                    color: theme.textColorDark
                }

                TextField {
                    id: ifaceField
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    width: 240
                    height: 42
                    text: CANScanner.bitrateInterface
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
                    width: parent.width * 0.42
                    text: "DETECTION MODE"
                    font.pixelSize: 22
                    color: theme.textColorDark
                }

                ComboBox {
                    id: detectMode
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    width: 240
                    height: 42
                    model: ["auto", "listen_only", "probe_only"]
                    font.pixelSize: 22

                    Component.onCompleted: {
                        var idx = model.indexOf(CANScanner.bitrateDetectMode)
                        currentIndex = idx >= 0 ? idx : 0
                    }
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
                    width: parent.width * 0.42
                    text: "TESTER SA [HEX]"
                    font.pixelSize: 22
                    color: theme.textColorDark
                }

                TextField {
                    id: testerSa
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    width: 240
                    height: 42
                    text: formatHexByte(CANScanner.bitrateTesterSa)
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
                    width: parent.width * 0.42
                    text: "LISTEN TIME [ms]"
                    font.pixelSize: 22
                    color: theme.textColorDark
                }

                TextField {
                    id: listenMs
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    width: 240
                    height: 42
                    text: String(CANScanner.bitrateListenMs)
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
                    width: parent.width * 0.42
                    text: "ONLY EXTENDED ID"
                    font.pixelSize: 22
                    color: theme.textColorDark
                }

                CheckBox {
                    id: extOnly
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    checked: CANScanner.bitrateExtendedOnly
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
                    id: infoText
                    anchors.centerIn: parent
                    text: "Zapisz zmiany przyciskiem SAVE"
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
            width: 150
            height: 68
            text: "BACK"
            onClicked: Navigation.pop()
        }

        StyledButton {
            width: 150
            height: 68
            text: "SAVE"
            onClicked: root.saveParameters()
        }
    }
}