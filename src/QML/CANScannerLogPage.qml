import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    Theme { id: theme }

    property bool showLog: true

    Rectangle {
        anchors.fill: parent
        color: theme.bgColor
    }

    CANScannerTopBar {
        id: topBar
        width: parent.width
        titleText: "LOG / RESULTS"
        rightText: CANScanner.status
        onMenuClicked: Navigation.pop()
    }

    Row {
        anchors.top: topBar.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 24

        StyledButton {
            text: "LOG"
            width: 220
            height: 84
            onClicked: root.showLog = true
        }

        StyledButton {
            text: "RESULTS"
            width: 220
            height: 84
            onClicked: root.showLog = false
        }
    }

    Rectangle {
        anchors.top: topBar.bottom
        anchors.topMargin: 130
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 24
        color: "#ffffff"
        radius: 18
        border.color: theme.borderColor
        border.width: 2

        Loader {
            anchors.fill: parent
            anchors.margins: 14
            sourceComponent: root.showLog ? logView : resultsView
        }
    }

    Component {
        id: logView

        ScrollView {
            TextArea {
                readOnly: true
                text: CANScanner.logText
                wrapMode: TextArea.WrapAnywhere
                font.pixelSize: 20
                color: "#202020"
                selectByMouse: true
            }
        }
    }

    Component {
        id: resultsView

        ListView {
            model: CANScanner.results
            clip: true
            spacing: 8

            delegate: Rectangle {
                width: ListView.view.width
                height: 90
                radius: 12
                color: index % 2 ? "#ececec" : "#f7f7f7"

                Column {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 4

                    Text {
                        text: modelData.ecu + " | tester " + modelData.tester + " | " + modelData.status
                        font.pixelSize: 20
                        color: "#202020"
                    }

                    Text {
                        text: "REQ: " + modelData.reqId + "    RESP: " + modelData.respId
                        font.pixelSize: 18
                        color: "#404040"
                    }

                    Text {
                        text: "DATA: " + modelData.data
                        font.pixelSize: 18
                        color: "#404040"
                        elide: Text.ElideRight
                        width: parent.width - 20
                    }
                }
            }

            ScrollBar.vertical: ScrollBar { }
        }
    }
}