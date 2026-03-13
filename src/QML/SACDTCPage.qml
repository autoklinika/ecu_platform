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
        anchors.bottomMargin: 16

        Rectangle {
            width: 980
            height: 470
            anchors.centerIn: parent
            radius: 20
            color: theme.cardColor
            border.color: theme.separatorColor

            Column {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 18

                Text {
                    text: "DTC READOUT"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 34
                    font.bold: true
                    color: theme.textColorDark
                }

                Item {
                    width: 1
                    height: 8
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
                    text: "Stored DTC count: " + CockpitController.dtcList.length
                    font.pixelSize: 24
                    font.bold: true
                    color: theme.textColorDark
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    visible: CockpitController.dtcReady && CockpitController.dtcList.length === 0
                    text: "No stored DTC"
                    font.pixelSize: 22
                    color: theme.textColorMuted
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Rectangle {
                    visible: CockpitController.dtcReady && CockpitController.dtcList.length > 0
                    width: parent.width
                    height: 280
                    radius: 14
                    color: "#FFFFFF"
                    border.color: theme.separatorColor

                    ListView {
                        anchors.fill: parent
                        anchors.margins: 10
                        clip: true
                        model: CockpitController.dtcList

                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 52
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
