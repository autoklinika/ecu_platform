import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    Theme { id: theme }

    function scanNow() {
        CANScanner.scanECU(
                    CANScanner.ecuInterface,
                    CANScanner.ecuBitrate,
                    CANScanner.ecuTesterMode,
                    CANScanner.ecuTesterSa,
                    CANScanner.ecuTesterFrom,
                    CANScanner.ecuTesterTo,
                    CANScanner.ecuAddrFrom,
                    CANScanner.ecuAddrTo,
                    CANScanner.ecuTimeoutMs,
                    CANScanner.ecuServiceName,
                    CANScanner.ecuStopOnFirst,
                    CANScanner.ecuDebugRx
                    )
    }

    function hex2(v) {
        var n = Number(v)
        if (isNaN(n))
            n = 0
        var s = n.toString(16).toUpperCase()
        if (s.length < 2)
            s = "0" + s
        return "0x" + s
    }

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
            anchors.margins: 16
            spacing: 12

            Text {
                text: "ECU SCANNER"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 28
                font.bold: true
                color: theme.textColorDark
            }

            Text {
                text: "Skanowanie adresów ECU i odpowiedzi diagnostycznych"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 18
                color: theme.textColorMuted
            }

            Rectangle {
                width: parent.width
                height: 110
                radius: 14
                color: "#FFFFFF"
                border.color: theme.separatorColor
                border.width: 1

                Column {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 8

                    Text {
                        text: "PROGRESS"
                        font.pixelSize: 18
                        font.bold: true
                        color: theme.textColorDark
                    }

                    Rectangle {
                        width: parent.width
                        height: 24
                        radius: 12
                        color: "#E9E9E9"
                        border.color: "#D5D5D5"

                        Rectangle {
                            height: parent.height
                            radius: parent.radius
                            width: parent.width * (CANScanner.progressPercent / 100.0)
                            color: "#1D4ED8"
                        }
                    }

                    Text {
                        text: CANScanner.progressCurrent + " / " + CANScanner.progressTotal + "    " + CANScanner.progressPercent + "%"
                        font.pixelSize: 16
                        color: theme.textColorDark
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 64
                radius: 12
                color: CANScanner.busy ? "#EEF4FF" : "#F8F8F8"
                border.color: CANScanner.busy ? "#C9D8FF" : "#DDDDDD"
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: CANScanner.status === "" ? "Ready to scan" : CANScanner.status
                    font.pixelSize: 20
                    font.bold: true
                    color: theme.textColorDark
                }
            }

            Rectangle {
                width: parent.width
                height: 124
                radius: 14
                color: "#FFFFFF"
                border.color: theme.separatorColor
                border.width: 1

                Column {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 8

                    Text {
                        text: "ACTIVE SERVICE"
                        font.pixelSize: 18
                        font.bold: true
                        color: theme.textColorDark
                    }

                    Text {
                        text: CANScanner.ecuServiceName
                        font.pixelSize: 18
                        color: theme.textColorDark
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        text: "STOP FIRST: " + (CANScanner.ecuStopOnFirst ? "ON" : "OFF")
                              + "    DEBUG RX: " + (CANScanner.ecuDebugRx ? "ON" : "OFF")
                        font.pixelSize: 16
                        color: theme.textColorMuted
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 128
                radius: 14
                color: "#FFFFFF"
                border.color: theme.separatorColor
                border.width: 1

                Column {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 8

                    Text {
                        text: "ACTIVE PARAMETERS"
                        font.pixelSize: 18
                        font.bold: true
                        color: theme.textColorDark
                    }

                    Text {
                        text: "IF: " + CANScanner.ecuInterface
                              + "    BR: " + CANScanner.ecuBitrate
                              + "    MODE: " + CANScanner.ecuTesterMode
                              + "    SA: " + root.hex2(CANScanner.ecuTesterSa)
                        font.pixelSize: 16
                        color: theme.textColorMuted
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        text: "T: " + root.hex2(CANScanner.ecuTesterFrom) + "-" + root.hex2(CANScanner.ecuTesterTo)
                              + "    ECU: " + root.hex2(CANScanner.ecuAddrFrom) + "-" + root.hex2(CANScanner.ecuAddrTo)
                              + "    TO: " + CANScanner.ecuTimeoutMs + " ms"
                        font.pixelSize: 16
                        color: theme.textColorMuted
                        wrapMode: Text.WordWrap
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
            text: CANScanner.busy ? "SCANNING..." : "START"
            onClicked: {
                if (!CANScanner.busy)
                    root.scanNow()
            }
        }

        StyledButton {
            width: 150
            height: 68
            text: "STOP"
            onClicked: CANScanner.stopScan()
        }

        StyledButton {
            width: 170
            height: 68
            text: "PARAMETERS"
            onClicked: Navigation.push("CANScannerECUParametersPage.qml")
        }

        StyledButton {
            width: 170
            height: 68
            text: "CLEAR RESULTS"
            onClicked: CANScanner.clearResults()
        }

        StyledButton {
            width: 190
            height: 68
            text: "LOG / RESULTS"
            onClicked: Navigation.push("CANScannerLogPage.qml")
        }
    }
}