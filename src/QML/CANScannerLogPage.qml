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
                text: "LOG / RESULTS"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 28
                font.bold: true
                color: theme.textColorDark
            }

            Text {
                text: CANScanner.status === "" ? "Przegląd logów i wyników skanowania" : CANScanner.status
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 18
                color: theme.textColorMuted
            }

            Rectangle {
                width: parent.width
                height: 64
                radius: 12
                color: "#FFFFFF"
                border.color: theme.separatorColor
                border.width: 1

                Row {
                    anchors.centerIn: parent
                    spacing: 18

                    StyledButton {
                        width: 180
                        height: 52
                        text: "LOG"
                        onClicked: root.showLog = true
                    }

                    StyledButton {
                        width: 180
                        height: 52
                        text: "RESULTS"
                        onClicked: root.showLog = false
                    }
                }
            }

            Rectangle {
                width: parent.width
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 0
                anchors.bottomMargin: 0
                height: parent.height - 64 - 64 - 24
                radius: 14
                color: "#FFFFFF"
                border.color: theme.separatorColor
                border.width: 1

                Loader {
                    anchors.fill: parent
                    anchors.margins: 12
                    sourceComponent: root.showLog ? logView : resultsView
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
            text: "LOG"
            onClicked: root.showLog = true
        }

        StyledButton {
            width: 150
            height: 68
            text: "RESULTS"
            onClicked: root.showLog = false
        }

        StyledButton {
            width: 170
            height: 68
            text: "CLEAR LOG"
            onClicked: CANScanner.clearLog()
        }

        StyledButton {
            width: 190
            height: 68
            text: "CLEAR RESULTS"
            onClicked: CANScanner.clearResults()
        }
    }

    Component {
    id: logView

    Flickable {
        id: logFlick
        clip: true
        contentWidth: width
        contentHeight: logTextItem.paintedHeight
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick

        function scrollToBottom() {
            contentY = Math.max(0, contentHeight - height)
        }

        Text {
            id: logTextItem
            width: logFlick.width
            text: CANScanner.logText
            wrapMode: Text.WrapAnywhere
            font.pixelSize: 20
            color: "#202020"
        }

        ScrollBar.vertical: ScrollBar { }

        onContentHeightChanged: scrollToBottom()

        Connections {
            target: CANScanner
            function onLogTextChanged() {
                logFlick.scrollToBottom()
            }
        }

        Component.onCompleted: scrollToBottom()
    }
}

    Component {
        id: resultsView

        ListView {
            id: resultsList
            model: CANScanner.results
            clip: true
            spacing: 10

            delegate: Rectangle {
                width: resultsList.width
                height: 104
                radius: 12
                color: index % 2 ? "#F3F3F3" : "#FAFAFA"
                border.color: "#DDDDDD"
                border.width: 1

                Column {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 6

                    Text {
                        text: modelData.ecu + " | tester " + modelData.tester + " | " + modelData.status
                        font.pixelSize: 20
                        font.bold: true
                        color: "#202020"
                        wrapMode: Text.WordWrap
                        width: parent.width
                    }

                    Text {
                        text: "REQ: " + modelData.reqId + "    RESP: " + modelData.respId
                        font.pixelSize: 18
                        color: "#404040"
                        wrapMode: Text.WordWrap
                        width: parent.width
                    }

                    Text {
                        text: "DATA: " + modelData.data
                        font.pixelSize: 18
                        color: "#404040"
                        wrapMode: Text.WordWrap
                        width: parent.width
                    }
                }
            }

            ScrollBar.vertical: ScrollBar { }
        }
    }
}