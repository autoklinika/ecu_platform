import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {

    Column {
        anchors.centerIn: parent
        spacing: 40

        Text {
            text: LanguageManager.t("settings_language")
            font.pixelSize: 36
            font.bold: true
            color: "#2A2A2A"
        }

        Row {
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

        StyledButton {
            width: 240
            height: 90
            text: LanguageManager.t("kafelek_back")
            onClicked: Navigation.pop()
        }
    }
}
