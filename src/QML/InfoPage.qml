import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {

    Column {
        anchors.centerIn: parent
        spacing: 30

        Text {
            text: "ECU Bench Platform"
            font.pixelSize: 42
            font.bold: true
            color: "#2A2A2A"
        }

        Text {
            text: "Version: 0.1"
            font.pixelSize: 28
            color: "#444444"
        }

        StyledButton {
            width: 240
            height: 90
            text: LanguageManager.t("kafelek_back")
            onClicked: Navigation.pop()
        }
    }
}
