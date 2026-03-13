import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    Theme { id: theme }

    function startRead() {
        CockpitController.startDTCRead()
    }

    Component.onCompleted: startRead()

    Rectangle {
        anchors.fill: parent
        color: theme.bgColor
    }

    SACTopBar {
        id: topBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 96
        onMenuClicked: Navigation.pop()
    }

    Item {
        id: contentArea
        anchors.top: topBar.bottom
        anchors.bottom: bottomButtons.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 24
        anchors.rightMargin: 24
        anchors.topMargin: 12
        anchors.bottomMargin: 16

        Rectangle {
            id: dtcPanel
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
                    text: "DTC READOUT"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 34
                    font.bold: true
                    color: theme.textColorDark
                }

                Text {
                    visible: CockpitController.dtcBusy
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Reading DTC in extended session..."
                    font.pixelSize: 22
                    color: theme.textColorMuted
                }

                BusyIndicator {
                    visible: CockpitController.dtcBusy
                    running: CockpitController.dtcBusy
                    width: 56
                    height: 56
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    visible: CockpitController.dtcError.length > 0
                    width: parent.width
                    text: CockpitController.dtcError
                    font.pixelSize: 22
                    color: "red"
                    wrapMode: Text.WrapAnywhere
                    horizontalAlignment: Text.AlignHCenter
                }

                Text {
                    visible: CockpitController.dtcReady
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Stored DTC count: " + CockpitController.dtcList.length
                    font.pixelSize: 24
                    font.bold: true
                    color: theme.textColorDark
                }

                Text {
                    visible: CockpitController.dtcReady && CockpitController.dtcList.length === 0
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "No stored DTC"
                    font.pixelSize: 22
                    color: theme.textColorMuted
                }
            }

            Rectangle {
                id: listContainer
                visible: CockpitController.dtcReady && CockpitController.dtcList.length > 0
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
                    id: dtcListView
                    anchors.fill: parent
                    anchors.margins: 10
                    clip: true
                    spacing: 2
                    model: CockpitController.dtcList

                    delegate: Rectangle {
                        width: dtcListView.width
                        height: 54
                        radius: 8
                        color: index % 2 === 0 ? "#F6F6F6" : "#FFFFFF"
                        border.color: "#DDDDDD"

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 16
                            anchors.right: parent.right
                            anchors.rightMargin: 16
                            text: modelData
                            font.pixelSize: 22
                            color: theme.textColorDark
                            elide: Text.ElideRight
                        }
                    }

                    ScrollBar.vertical: ScrollBar {
                        active: true
                    }
                }
            }
        }
    }

    Row {
        id: bottomButtons
        spacing: 24
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 28
        anchors.horizontalCenter: parent.horizontalCenter

        StyledButton {
            width: 220
            height: 80
            text: "REFRESH"
            onClicked: root.startRead()
        }

        StyledButton {
            width: 220
            height: 80
            text: LanguageManager.t("kafelek_back")
            onClicked: Navigation.pop()
        }
    }
}
