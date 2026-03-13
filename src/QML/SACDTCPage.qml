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
        width: parent.width
        height: 96
        onMenuClicked: Navigation.pop()
    }

    Column {
        anchors.top: topBar.bottom
        anchors.topMargin: 24
        anchors.left: parent.left
        anchors.leftMargin: 40
        anchors.right: parent.right
        anchors.rightMargin: 40
        spacing: 20

        Text {
            text: "DTC READOUT"
            font.pixelSize: 30
            font.bold: true
            color: theme.textColor
        }

        Text {
            visible: CockpitController.dtcBusy
            text: "Reading DTC in extended session..."
            font.pixelSize: 22
            color: theme.textColorPar
        }

        BusyIndicator {
            visible: CockpitController.dtcBusy
            running: CockpitController.dtcBusy
            width: 56
            height: 56
        }

        Text {
            visible: CockpitController.dtcError.length > 0
            text: CockpitController.dtcError
            font.pixelSize: 22
            color: "red"
            wrapMode: Text.WrapAnywhere
            width: parent.width
        }

        Rectangle {
            visible: CockpitController.dtcReady
            width: parent.width
            height: 500
            radius: 16
            color: "#FFFFFF"
            border.color: theme.borderColor

            Column {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 16

                Text {
                    text: "Stored DTC count: " + CockpitController.dtcList.length
                    font.pixelSize: 24
                    font.bold: true
                    color: theme.textColor
                }

                Text {
                    visible: CockpitController.dtcReady && CockpitController.dtcList.length === 0
                    text: "No stored DTC"
                    font.pixelSize: 22
                    color: theme.textColorPar
                }

                ListView {
                    visible: CockpitController.dtcList.length > 0
                    width: parent.width
                    height: 400
                    clip: true
                    model: CockpitController.dtcList

                    delegate: Rectangle {
                        width: ListView.view.width
                        height: 56
                        color: index % 2 === 0 ? "#F6F6F6" : "#FFFFFF"
                        border.color: "#DDDDDD"

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 16
                            text: modelData
                            font.pixelSize: 22
                            color: theme.textColor
                        }
                    }
                }
            }
        }
    }

    Row {
        spacing: 20
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