import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    Theme { id: theme }

    property bool modalOpen: false

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
        anchors.bottom: bottomButtons.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 24
        anchors.rightMargin: 24
        anchors.topMargin: 12
        anchors.bottomMargin: 16

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
                    model: 6

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
                            text: "Parameter " + (index + 1)
                            font.pixelSize: 24
                            color: theme.textColorDark
                            elide: Text.ElideRight
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            anchors.rightMargin: 16
                            width: parent.width * 0.28
                            text: (Math.random() * 10).toFixed(2) + " bar"
                            font.pixelSize: 24
                            color: theme.textColorMuted
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

    Row {
        id: bottomButtons
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 28
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 24

        StyledButton {
            width: 220
            height: 80
            text: LanguageManager.t("kafelek_back")
            onClicked: Navigation.pop()
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
