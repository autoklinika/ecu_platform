import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    anchors.fill: parent
    property string brand: ""

    Theme { id: theme }

    Column {
        width: parent.width
        anchors.top: parent.top
        anchors.topMargin: 28
        spacing: 42

        Text {
            text: brand
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 48
            font.bold: true
            color: theme.textColorDark
        }

        StyledButton {
            width: 340
            height: 140
            anchors.horizontalCenter: parent.horizontalCenter
            text: LanguageManager.t("kafelek_sac")
            onClicked: Navigation.push("SACSpeedPage.qml")
        }
    }

    StyledButton {
        width: 200
        height: 90
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        text: LanguageManager.t("kafelek_back")
        onClicked: Navigation.pop()
    }
}
