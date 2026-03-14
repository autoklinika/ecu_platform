import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    width: parent ? parent.width : 1280
    height: 96

    property bool online: CockpitController.connected
    property string vin: CockpitController.vin
    property string leftButtonText: LanguageManager.t("kafelek_menu")

    signal menuClicked()

    Theme { id: theme }

    Rectangle {
        anchors.fill: parent
        color: theme.topBarColor
    }

    StyledButton {
        anchors.left: parent.left
        anchors.leftMargin: 16
        anchors.verticalCenter: parent.verticalCenter
        width: 192
        height: 64
        text: root.leftButtonText
        onClicked: root.menuClicked()
    }

 

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing: 12

        Rectangle {
            width: 16
            height: 16
            radius: 8
            color: root.online ? "#0A9F34" : "#D22D2D"
        }

        Text {
            text: root.online ? "ONLINE" : "OFFLINE"
            font.pixelSize: 26
            font.bold: true
            color: root.online ? "#0A9F34" : "#D22D2D"
        }
    }

    Text {
        anchors.right: parent.right
        anchors.rightMargin: 32
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 26
        font.bold: true
        color: theme.textColorDark
        text: {
            const currentVin = root.vin || "---"
            if (currentVin.length >= 7)
                return currentVin.slice(currentVin.length - 7)
            return currentVin
        }
    }
}
