import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    property string vin: "---"
    property string sw: "---"
    property string hw: "---"

    Theme { id: theme }

    Rectangle {
        anchors.fill: parent
        color: theme.bgColor
    }

    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -20
        spacing: 24

        Text {
            text: "VIN"
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 24
            color: theme.textColorMuted
        }

        Text {
            text: root.vin
            width: 980
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WrapAnywhere
            font.pixelSize: 40
            font.bold: true
            color: theme.textColorDark
        }

        Rectangle {
            width: 760
            height: 2
            anchors.horizontalCenter: parent.horizontalCenter
            color: theme.separatorColor
        }

        Column {
            width: 900
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 16

            Text {
                text: "SW: " + root.sw
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WrapAnywhere
                font.pixelSize: 28
                color: theme.textColorDark
            }

            Text {
                text: "HW: " + root.hw
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WrapAnywhere
                font.pixelSize: 28
                color: theme.textColorDark
            }
        }
    }

    StyledButton {
        width: 240
        height: 90
        text: LanguageManager.t("kafelek_ok")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        onClicked: Navigation.push("SACMenuPage.qml")
    }
}
