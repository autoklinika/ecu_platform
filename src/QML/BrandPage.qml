import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    //property string brand: ""

    Theme { id: theme }

    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -20
        spacing: 36

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
        width: 220
        height: 90
        text: LanguageManager.t("kafelek_back")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        onClicked: Navigation.pop()
    }
}
