import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {

    Column {
        anchors.centerIn: parent
        spacing: 40

        Text {
            text: LanguageManager.t("menu_settings")
            font.pixelSize: 36
            font.bold: true
            color: "#2A2A2A"
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        StyledButton {
            width: 420
            height: 120
            text: LanguageManager.t("settings_language")
            onClicked: Navigation.push("LanguagePage.qml")
        }

        StyledButton {
            width: 240
            height: 90
            text: LanguageManager.t("kafelek_back")
            onClicked: Navigation.pop()
        }
    }
}
