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
        titleText: "CAN SCANNER"
        leftButtonText: "BACK"
        rightText: ""
        onMenuClicked: Navigation.pop()
    }

    Grid {
        id: tileGrid
        anchors.top: topBar.bottom
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 30
        rows: 2
        columns: 2
        rowSpacing: 28
        columnSpacing: 28

        StyledButton {
            width: 320
            height: 180
            text: "BITRATE SCAN"
            onClicked: Navigation.push("CANScannerBitratePage.qml")
        }

        StyledButton {
            width: 320
            height: 180
            text: "ECU SCAN"
            onClicked: Navigation.push("CANScannerECUPage.qml")
        }

        StyledButton {
            width: 320
            height: 180
            text: "LOG / RESULTS"
            onClicked: Navigation.push("CANScannerLogPage.qml")
        }

        StyledButton {
            width: 320
            height: 180
            text: "PARAMETERS"
            onClicked: Navigation.push("CANScannerParametersPage.qml")
        }
    }
}