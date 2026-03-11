import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    property string brand: ""

    // ===== TYTUŁ =====
    Text {
        text: brand
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.22

        font.pixelSize: 48
        font.bold: true
        color: "#2A2A2A"
    }

    // ===== SAC (główny kafelek) =====
    StyledButton {
        width: 340
        height: 140

        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.38

        text: LanguageManager.t("kafelek_sac")
        onClicked: Navigation.push("SACSpeedPage.qml")
    }

    // ===== BACK (pozycja spójna z resztą aplikacji) =====
    StyledButton {
        width: 200
        height: 90

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 60

        text: LanguageManager.t("kafelek_back")
        onClicked: Navigation.pop()
    }
}
