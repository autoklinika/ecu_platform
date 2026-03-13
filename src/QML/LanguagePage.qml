import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    anchors.fill: parent

    Theme { id: theme }

    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -20
        spacing: 32

        Text {
            text: LanguageManager.t("settings_language")
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 40
            font.bold: true
            color: theme.textColorDark
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 40

            StyledButton {
                width: 300
                height: 120
                text: LanguageManager.t("language_polish")
                onClicked: LanguageManager.setLanguage("pl")
            }

            StyledButton {
                width: 300
                height: 120
                text: LanguageManager.t("language_english")
                onClicked: LanguageManager.setLanguage("en")
            }
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
