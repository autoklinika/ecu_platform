import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    Theme { id: theme }

    function startRead() {
        CockpitController.setRuntimePollingEnabled(false)
        CockpitController.startDTCRead()
    }

    Component.onCompleted: startRead()

    Component.onDestruction: {
        CockpitController.setRuntimePollingEnabled(true)
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

                Column {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 6

                    Rectangle {
                        width: parent.width
                        height: 42
                        radius: 8
                        color: "#EFEFEF"
                        border.color: "#DDDDDD"

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 16
                            text: "DTC"
                            font.pixelSize: 20
                            font.bold: true
                            color: theme.textColorDark
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            anchors.rightMargin: 24
                            text: "STATUS"
                            font.pixelSize: 20
                            font.bold: true
                            color: theme.textColorDark
                        }
                    }

                    ListView {
                        id: dtcListView
                        width: parent.width
                        height: parent.height - 48
                        clip: true
                        spacing: 2
                        model: CockpitController.dtcList

                        delegate: Rectangle {
                            width: dtcListView.width
                            height: 54
                            radius: 8
                            color: index % 2 === 0 ? "#F6F6F6" : "#FFFFFF"
                            border.color: "#DDDDDD"

                            property string rawText: String(modelData)

                            property string dtcCode: {
                                var parts = rawText.split(/\s+/)
                                return parts.length > 0 ? parts[0] : rawText
                            }

                            property string dtcStatus: {
                                if (rawText.indexOf("ACTIVE") >= 0)
                                    return "ACTIVE"
                                if (rawText.indexOf("STORED") >= 0)
                                    return "STORED"
                                if (rawText.indexOf("PENDING") >= 0)
                                    return "PENDING"
                                if (rawText.indexOf("INACTIVE") >= 0)
                                    return "INACTIVE"
                                return "---"
                            }

                            property color statusColor: {
                                if (dtcStatus === "ACTIVE")
                                    return "#D62828"
                                if (dtcStatus === "STORED")
                                    return "#F08C00"
                                if (dtcStatus === "PENDING")
                                    return "#C77D00"
                                return theme.textColorDark
                            }

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.leftMargin: 16
                                width: parent.width * 0.55
                                text: parent.dtcCode
                                font.pixelSize: 22
                                color: theme.textColorDark
                                elide: Text.ElideRight
                            }

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.right: parent.right
                                anchors.rightMargin: 16
                                width: parent.width * 0.25
                                text: parent.dtcStatus
                                font.pixelSize: 22
                                font.bold: parent.dtcStatus === "ACTIVE"
                                color: parent.statusColor
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
