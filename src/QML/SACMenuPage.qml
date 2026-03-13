import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    Theme { id: theme }

    property bool modalOpen: false

    function paramLabel(index) {
        switch (index) {
        case 0: return "Pressure 1"
        case 1: return "Pressure 2"
        case 2: return "Battery Permanent"
        case 3: return "Battery Ignition"
        default: return "---"
        }
    }

    function paramUnit(index) {
        switch (index) {
        case 0:
        case 1:
            return "bar"
        case 2:
        case 3:
            return "V"
        default:
            return ""
        }
    }

    function paramValid(index) {
        switch (index) {
        case 0: return CockpitController.pressure1Valid
        case 1: return CockpitController.pressure2Valid
        case 2: return CockpitController.voltagePermanentValid
        case 3: return CockpitController.voltageIgnitionValid
        default: return false
        }
    }

    function paramValue(index) {
        if (!paramValid(index))
            return "---"

        switch (index) {
        case 0: return Number(CockpitController.pressure1Bar).toFixed(2)
        case 1: return Number(CockpitController.pressure2Bar).toFixed(2)
        case 2: return Number(CockpitController.voltagePermanent).toFixed(1)
        case 3: return Number(CockpitController.voltageIgnition).toFixed(1)
        default: return "---"
        }
    }

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
        onMenuClicked: root.modalOpen = !root.modalOpen
    }

    Item {
        id: contentArea
        anchors.top: topBar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 24
        anchors.rightMargin: 24
        anchors.topMargin: 12
        anchors.bottomMargin: 20

        Rectangle {
            id: paramsPanel
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
                    text: "PARAMETERS"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 34
                    font.bold: true
                    color: theme.textColorDark
                }
            }

            Rectangle {
                id: listContainer
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
                    id: paramsListView
                    anchors.fill: parent
                    anchors.margins: 10
                    clip: true
                    spacing: 2
                    model: 4

                    delegate: Rectangle {
                        width: paramsListView.width
                        height: 56
                        radius: 8
                        color: index % 2 === 0 ? "#F6F6F6" : "#FFFFFF"
                        border.color: "#DDDDDD"

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 16
                            width: parent.width * 0.55
                            text: root.paramLabel(index)
                            font.pixelSize: 24
                            color: theme.textColorDark
                            elide: Text.ElideRight
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            anchors.rightMargin: 16
                            width: parent.width * 0.30
                            text: {
                                const v = root.paramValue(index)
                                if (v === "---")
                                    return "---"
                                return v + " " + root.paramUnit(index)
                            }
                            font.pixelSize: 24
                            color: root.paramValid(index) ? theme.textColorMuted : "#999999"
                            horizontalAlignment: Text.AlignRight
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

    Rectangle {
        visible: root.modalOpen
        width: 360
        height: 320
        radius: 26
        color: "#FFFFFF"
        border.color: theme.borderColor
        x: 32
        y: topBar.height + 16

        Column {
            anchors.centerIn: parent
            spacing: 24

            StyledButton {
                text: LanguageManager.t("kafelek_dtc")
                width: 280
                height: 68
                onClicked: {
                    root.modalOpen = false
                    Navigation.push("SACDTCPage.qml")
                }
            }

            StyledButton {
                text: LanguageManager.t("kafelek_activ")
                width: 280
                height: 68
            }

            StyledButton {
                text: LanguageManager.t("kafelek_back")
                width: 280
                height: 68
                onClicked: {
                    root.modalOpen = false
                    CockpitController.disconnect()
                    SystemController.resetCAN("can0")
                    Navigation.goHome()
                }
            }
        }
    }
}
