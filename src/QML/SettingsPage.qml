import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    anchors.fill: parent

    Theme { id: theme }

    Column {
        width: parent.width
        anchors.top: parent.top
        anchors.topMargin: 28
        spacing: 34

        Text {
            text: LanguageManager.t("menu_settings")
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 40
            font.bold: true
            color: theme.textColorDark
        }

        Item {
            width: 1
            height: 8
        }

        StyledButton {
            width: 420
            height: 120
            anchors.horizontalCenter: parent.horizontalCenter
            text: LanguageManager.t("settings_language")
            onClicked: Navigation.push(Qt.resolvedUrl("LanguagePage.qml"))
        }
    }

    StyledButton {
        width: 240
        height: 90
        text: LanguageManager.t("kafelek_back")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        onClicked: Navigation.pop()
    }
}
