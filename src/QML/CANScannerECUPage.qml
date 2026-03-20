import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    Theme { id: theme }

    function shortcutItems() {
        return [
            { label: "IF", value: ifaceField ? ifaceField.text : "can0" },
            { label: "BR", value: bitrateField ? bitrateField.text : "250000" },
            { label: "MODE", value: testerMode ? testerMode.currentText : "manual" },
            { label: "SA", value: testerSa ? testerSa.text : "241" },
            { label: "T", value: (testerFrom ? testerFrom.text : "240") + "-" + (testerTo ? testerTo.text : "255") },
            { label: "ECU", value: (ecuFrom ? ecuFrom.text : "0") + "-" + (ecuTo ? ecuTo.text : "254") },
            { label: "TO", value: (timeoutMs ? timeoutMs.text : "120") + " ms" }
        ]
    }

    function scanNow() {
        CANScanner.scanECU(
            ifaceField.text,
            parseInt(bitrateField.text),
            testerMode.currentText,
            parseInt(testerSa.text),
            parseInt(testerFrom.text),
            parseInt(testerTo.text),
            parseInt(ecuFrom.text),
            parseInt(ecuTo.text),
            parseInt(timeoutMs.text),
            serviceName.currentText,
            stopFirst.checked,
            debugRx.checked
        )
    }

    Rectangle {
        anchors.fill: parent
        color: theme.bgColor
    }

    CANScannerTopBar {
        id: topBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        titleText: "ECU SCAN"
        rightText: CANScanner.status
        shortcutModel: root.shortcutItems()
        onMenuClicked: Navigation.pop()
    }

    Item {
        id: contentArea
        anchors.top: topBar.bottom
        anchors.bottom: bottomBar.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 24
        anchors.rightMargin: 24
        anchors.topMargin: 12
        anchors.bottomMargin: 16

        Rectangle {
            id: ecuPanel
            anchors.fill: parent
            radius: 20
            color: theme.cardColor
            border.color: theme.separatorColor

            Column {
                id: headerColumn
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 24
                anchors.rightMargin: 24
                anchors.topMargin: 24
                spacing: 14

                Text {
                    text: "ECU SCANNER"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 34
                    font.bold: true
                    color: theme.textColorDark
                }

                Text {
                    text: "Konfiguracja programu testującego i zakresu skanowania"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 22
                    color: theme.textColorMuted
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "PROGRESS: " + CANScanner.progressCurrent + " / " + CANScanner.progressTotal + " (" + CANScanner.progressPercent + "%)"
                    font.pixelSize: 24
                    font.bold: true
                    color: theme.textColorDark
                }
            }

            Rectangle {
                id: formContainer
                anchors.top: headerColumn.bottom
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 24
                anchors.rightMargin: 24
                anchors.topMargin: 20
                anchors.bottomMargin: 24
                radius: 14
                color: "#FFFFFF"
                border.color: theme.separatorColor

                ListView {
                    id: settingsList
                    anchors.fill: parent
                    anchors.margins: 10
                    clip: true
                    spacing: 2
                    model: settingsModel

                    delegate: Rectangle {
                        property var entry: modelData
                        property string label: entry.label
                        property string type: entry.type
                        property string value: entry.value
                        property var choices: entry.choices
                        property bool checkedValue: entry.checkedValue
                        property int fieldWidth: entry.fieldWidth
                        property string unit: entry.unit

                        width: settingsList.width
                        height: type === "check" ? 64 : 72
                        radius: 8
                        color: index % 2 === 0 ? "#F6F6F6" : "#FFFFFF"
                        border.color: "#DDDDDD"

                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 16
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width * 0.34
                            text: label
                            font.pixelSize: 24
                            color: theme.textColorDark
                            elide: Text.ElideRight
                        }

                        Loader {
                            anchors.right: parent.right
                            anchors.rightMargin: 16
                            anchors.verticalCenter: parent.verticalCenter
                            sourceComponent: {
                                if (type === "combo") return comboEditor
                                if (type === "range") return rangeEditor
                                if (type === "service") return serviceEditor
                                if (type === "check") return checkEditor
                                return textEditor
                            }
                        }
                    }

                    ScrollBar.vertical: ScrollBar { active: true }
                }
            }
        }
    }

    Row {
        id: bottomBar
        spacing: 24
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 28
        anchors.horizontalCenter: parent.horizontalCenter

        StyledButton {
            width: 240
            height: 80
            text: "START"
            onClicked: root.scanNow()
        }

        StyledButton {
            width: 240
            height: 80
            text: "CLEAR RESULTS"
            onClicked: CANScanner.clearResults()
        }

        StyledButton {
            width: 220
            height: 80
            text: "LOG / RESULTS"
            onClicked: Navigation.push("CANScannerLogPage.qml")
        }
    }

    property var settingsModel: [
        { label: "INTERFACE", type: "text", value: "can0", choices: [], checkedValue: false, fieldWidth: 220, unit: "" },
        { label: "BITRATE [bit/s]", type: "text", value: "250000", choices: [], checkedValue: false, fieldWidth: 220, unit: "" },
        { label: "TESTER MODE", type: "combo", value: "manual", choices: ["manual", "auto"], checkedValue: false, fieldWidth: 220, unit: "" },
        { label: "TESTER SA (DEC)", type: "text", value: "241", choices: [], checkedValue: false, fieldWidth: 220, unit: "" },
        { label: "TESTER RANGE", type: "range", value: "240-255", choices: [], checkedValue: false, fieldWidth: 220, unit: "" },
        { label: "ECU RANGE", type: "range", value: "0-254", choices: [], checkedValue: false, fieldWidth: 220, unit: "" },
        { label: "TIMEOUT [ms]", type: "text", value: "120", choices: [], checkedValue: false, fieldWidth: 220, unit: "ms" },
        { label: "SERVICE", type: "service", value: "TesterPresent (3E 00)", choices: ["TesterPresent (3E 00)", "DiagnosticSessionControl (10 01)", "ReadDataByIdentifier VIN (22 F1 90)"], checkedValue: false, fieldWidth: 420, unit: "" },
        { label: "STOP ON FIRST RESPONSE", type: "check", value: "", choices: [], checkedValue: true, fieldWidth: 220, unit: "" },
        { label: "DEBUG RX", type: "check", value: "", choices: [], checkedValue: true, fieldWidth: 220, unit: "" }
    ]

    Component {
        id: textEditor
        TextField {
            width: fieldWidth
            height: 46
            font.pixelSize: 22
            text: value

            Component.onCompleted: {
                if (label === "INTERFACE") ifaceField = this
                else if (label === "BITRATE [bit/s]") bitrateField = this
                else if (label === "TESTER SA (DEC)") testerSa = this
                else if (label === "TIMEOUT [ms]") timeoutMs = this
            }
        }
    }

    Component {
        id: comboEditor
        ComboBox {
            width: fieldWidth
            height: 46
            font.pixelSize: 22
            model: choices

            Component.onCompleted: {
                currentIndex = Math.max(0, find(value))
                if (label === "TESTER MODE") testerMode = this
            }
        }
    }

    Component {
        id: rangeEditor
        Row {
            spacing: 10

            TextField {
                id: fromField
                width: 100
                height: 46
                font.pixelSize: 22
                text: value.split("-")[0]

                Component.onCompleted: {
                    if (label === "TESTER RANGE") testerFrom = this
                    else if (label === "ECU RANGE") ecuFrom = this
                }
            }

            Text {
                text: "-"
                font.pixelSize: 24
                color: theme.textColorDark
                anchors.verticalCenter: parent.verticalCenter
            }

            TextField {
                id: toField
                width: 100
                height: 46
                font.pixelSize: 22
                text: value.split("-")[1]

                Component.onCompleted: {
                    if (label === "TESTER RANGE") testerTo = this
                    else if (label === "ECU RANGE") ecuTo = this
                }
            }
        }
    }

    Component {
        id: serviceEditor
        ComboBox {
            width: 420
            height: 46
            font.pixelSize: 22
            model: choices

            Component.onCompleted: {
                currentIndex = Math.max(0, find(value))
                serviceName = this
            }
        }
    }

    Component {
        id: checkEditor
        CheckBox {
            height: 46
            checked: checkedValue
            text: checked ? "ON" : "OFF"
            font.pixelSize: 22

            indicator.width: 28
            indicator.height: 28

            Component.onCompleted: {
                if (label === "STOP ON FIRST RESPONSE") stopFirst = this
                else if (label === "DEBUG RX") debugRx = this
            }
        }
    }

    property var ifaceField
    property var bitrateField
    property var testerMode
    property var testerSa
    property var testerFrom
    property var testerTo
    property var ecuFrom
    property var ecuTo
    property var timeoutMs
    property var serviceName
    property var stopFirst
    property var debugRx
}