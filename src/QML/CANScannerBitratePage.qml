import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    Theme { id: theme }

    function startScan() {
        CANScanner.scanBitrate(
                    CANScanner.bitrateInterface,
                    CANScanner.bitrateDetectMode,
                    CANScanner.bitrateTesterSa,
                    CANScanner.bitrateListenMs,
                    CANScanner.bitrateExtendedOnly
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

    property string resultText: {
        if (CANScanner.busy)
            return "SCANNING..."
        if (CANScanner.detectedBitrate !== "")
            return CANScanner.detectedBitrate
        if (CANScanner.status === "Nie wykryto bitrate")
            return "NO BITRATE DETECTED"
        return "---"
    }

    property color resultColor: {
        if (CANScanner.busy)
            return "#1D4ED8"
        if (CANScanner.detectedBitrate !== "")
            return "#0A9F34"
        if (CANScanner.status === "Nie wykryto bitrate")
            return "#D22D2D"
        return theme.textColorDark
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
        anchors.bottom: buttonsRow.top
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
                text: "BITRATE SCANNER"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 28
                font.bold: true
                color: theme.textColorDark
            }

            Text {
                text: "Detekcja bitrate magistrali CAN"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 18
                color: theme.textColorMuted
            }

            Rectangle {
                width: parent.width
                height: 128
                radius: 14
                color: "#FFFFFF"
                border.color: theme.separatorColor
                border.width: 1

                Column {
                    anchors.centerIn: parent
                    spacing: 4

                    Text {
                        text: "SCAN RESULT"
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 18
                        color: theme.textColorMuted
                    }

                    Text {
                        text: root.resultText
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 36
                        font.bold: true
                        color: root.resultColor
                    }

                    Text {
                        text: {
                            if (CANScanner.busy)
                                return "Detection in progress"
                            if (CANScanner.detectedBitrate !== "")
                                return "Detected successfully"
                            if (CANScanner.status === "Nie wykryto bitrate")
                                return "Scan completed without match"
                            return "Ready to scan"
                        }
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 16
                        color: theme.textColorMuted
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 100
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
                        height: 22
                        radius: 11
                        color: "#E9E9E9"
                        border.color: "#D5D5D5"

                        Rectangle {
                            height: parent.height
                            radius: parent.radius
                            width: parent.width * (CANScanner.progressPercent / 100.0)
                            color: CANScanner.detectedBitrate !== "" ? "#0A9F34" : "#1D4ED8"
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
                height: 60
                radius: 12
                color: CANScanner.detectedBitrate !== "" ? "#ECFDF3"
                                                         : (CANScanner.status === "Nie wykryto bitrate" ? "#FFF1F2" : "#F8F8F8")
                border.color: CANScanner.detectedBitrate !== "" ? "#B7E4C7"
                                                                : (CANScanner.status === "Nie wykryto bitrate" ? "#F5C2C7" : "#DDDDDD")
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: {
                        if (CANScanner.busy)
                            return "Scanning in progress..."
                        if (CANScanner.detectedBitrate !== "")
                            return "Detected bitrate: " + CANScanner.detectedBitrate
                        if (CANScanner.status === "Nie wykryto bitrate")
                            return "Scan finished: no bitrate detected"
                        return "Ready to scan"
                    }
                    font.pixelSize: 20
                    font.bold: true
                    color: root.resultColor
                }
            }

            Rectangle {
                width: parent.width
                height: 96
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
                        text: "IF: " + CANScanner.bitrateInterface
                              + "    MODE: " + CANScanner.bitrateDetectMode
                              + "    SA: " + root.hex2(CANScanner.bitrateTesterSa)
                        font.pixelSize: 16
                        color: theme.textColorMuted
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        text: "LISTEN: " + CANScanner.bitrateListenMs + " ms"
                              + "    EXT ONLY: " + (CANScanner.bitrateExtendedOnly ? "ON" : "OFF")
                        font.pixelSize: 16
                        color: theme.textColorMuted
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }

    Row {
        id: buttonsRow
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
                    root.startScan()
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
            onClicked: Navigation.push("CANScannerParametersPage.qml")
        }

        StyledButton {
            width: 190
            height: 68
            text: "LOG / RESULTS"
            onClicked: Navigation.push("CANScannerLogPage.qml")
        }
    }
}