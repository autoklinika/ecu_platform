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
        CANScanner.ecuInterface = ifaceField.text
        CANScanner.ecuBitrate = parseInt(bitrateField.text, 10)
        CANScanner.ecuTesterMode = testerMode.currentText
        CANScanner.ecuTesterSa = toIntAuto(testerSa.text)
        CANScanner.ecuTesterFrom = toIntAuto(testerFrom.text)
        CANScanner.ecuTesterTo = toIntAuto(testerTo.text)
        CANScanner.ecuAddrFrom = toIntAuto(ecuFrom.text)
        CANScanner.ecuAddrTo = toIntAuto(ecuTo.text)
        CANScanner.ecuTimeoutMs = parseInt(timeoutMs.text, 10)
        CANScanner.ecuServiceName = serviceName.currentText
        CANScanner.ecuStopOnFirst = stopFirst.checked
        CANScanner.ecuDebugRx = debugRx.checked
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

        Flickable {
            anchors.fill: parent
            anchors.margins: 16
            contentWidth: width
            contentHeight: contentColumn.height
            clip: true

            Column {
                id: contentColumn
                width: parent.width
                spacing: 12

                Text {
                    text: "ECU PARAMETERS"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 28
                    font.bold: true
                    color: theme.textColorDark
                }

                Text {
                    text: "Ustawienia wspólne dla skanowania ECU"
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
                        text: CANScanner.ecuInterface
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
                        text: "BITRATE [bit/s]"
                        font.pixelSize: 22
                        color: theme.textColorDark
                    }

                    TextField {
                        id: bitrateField
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 16
                        width: 240
                        height: 42
                        text: String(CANScanner.ecuBitrate)
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
                        text: "TESTER MODE"
                        font.pixelSize: 22
                        color: theme.textColorDark
                    }

                    ComboBox {
                        id: testerMode
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 16
                        width: 240
                        height: 42
                        model: ["manual", "auto"]
                        font.pixelSize: 22

                        Component.onCompleted: {
                            var idx = model.indexOf(CANScanner.ecuTesterMode)
                            currentIndex = idx >= 0 ? idx : 0
                        }
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
                        text: formatHexByte(CANScanner.ecuTesterSa)
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
                        text: "TESTER RANGE [HEX]"
                        font.pixelSize: 22
                        color: theme.textColorDark
                    }

                    Row {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 16
                        spacing: 10

                        TextField {
                            id: testerFrom
                            width: 100
                            height: 42
                            text: formatHexByte(CANScanner.ecuTesterFrom)
                            font.pixelSize: 22
                        }

                        Text {
                            text: "-"
                            anchors.verticalCenter: parent.verticalCenter
                            font.pixelSize: 22
                            color: theme.textColorDark
                        }

                        TextField {
                            id: testerTo
                            width: 100
                            height: 42
                            text: formatHexByte(CANScanner.ecuTesterTo)
                            font.pixelSize: 22
                        }
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
                        text: "ECU RANGE [HEX]"
                        font.pixelSize: 22
                        color: theme.textColorDark
                    }

                    Row {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 16
                        spacing: 10

                        TextField {
                            id: ecuFrom
                            width: 100
                            height: 42
                            text: formatHexByte(CANScanner.ecuAddrFrom)
                            font.pixelSize: 22
                        }

                        Text {
                            text: "-"
                            anchors.verticalCenter: parent.verticalCenter
                            font.pixelSize: 22
                            color: theme.textColorDark
                        }

                        TextField {
                            id: ecuTo
                            width: 100
                            height: 42
                            text: formatHexByte(CANScanner.ecuAddrTo)
                            font.pixelSize: 22
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
                        text: "TIMEOUT [ms]"
                        font.pixelSize: 22
                        color: theme.textColorDark
                    }

                    TextField {
                        id: timeoutMs
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 16
                        width: 240
                        height: 42
                        text: String(CANScanner.ecuTimeoutMs)
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
                        text: "SERVICE"
                        font.pixelSize: 22
                        color: theme.textColorDark
                    }

                    ComboBox {
                        id: serviceName
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 16
                        width: 420
                        height: 42
                        model: [
                            "TesterPresent (3E 00)",
                            "DiagnosticSessionControl (10 01)",
                            "ReadDataByIdentifier VIN (22 F1 90)"
                        ]
                        font.pixelSize: 22

                        Component.onCompleted: {
                            var idx = model.indexOf(CANScanner.ecuServiceName)
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

                    Row {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 16
                        anchors.rightMargin: 16
                        spacing: 40

                        Row {
                            spacing: 12

                            Text {
                                text: "STOP ON FIRST RESPONSE"
                                anchors.verticalCenter: parent.verticalCenter
                                font.pixelSize: 22
                                color: theme.textColorDark
                            }

                            CheckBox {
                                id: stopFirst
                                anchors.verticalCenter: parent.verticalCenter
                                checked: CANScanner.ecuStopOnFirst
                                text: checked ? "ON" : "OFF"
                                font.pixelSize: 22
                                indicator.width: 28
                                indicator.height: 28
                            }
                        }

                        Row {
                            spacing: 12

                            Text {
                                text: "DEBUG RX"
                                anchors.verticalCenter: parent.verticalCenter
                                font.pixelSize: 22
                                color: theme.textColorDark
                            }

                            CheckBox {
                                id: debugRx
                                anchors.verticalCenter: parent.verticalCenter
                                checked: CANScanner.ecuDebugRx
                                text: checked ? "ON" : "OFF"
                                font.pixelSize: 22
                                indicator.width: 28
                                indicator.height: 28
                            }
                        }
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