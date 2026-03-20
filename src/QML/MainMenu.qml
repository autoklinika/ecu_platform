import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {

    property int btnW: 380
    property int btnH: 160
    property int gapX: 120
    property int gapY: 80

    property int totalW: btnW * 2 + gapX
    property int totalH: btnH * 2 + gapY

    property int startX: (width - totalW) / 2
    property int startY: (height - totalH) / 2

    StyledButton {
        text: LanguageManager.t("menu_test")
        width: btnW
        height: btnH
        x: startX
        y: startY
        onClicked: Navigation.push("TestsPage.qml")
    }

    StyledButton {
        text: LanguageManager.t("menu_settings")
        width: btnW
        height: btnH
        x: startX + btnW + gapX
        y: startY
        onClicked: Navigation.push("SettingsPage.qml")
    }

    StyledButton {
        text: LanguageManager.t("menu_can")
        width: btnW
        height: btnH
        x: startX
        y: startY + btnH + gapY
         onClicked: Navigation.push("CANScannerMenuPage.qml")
    }

    StyledButton {
        text: LanguageManager.t("menu_info")
        width: btnW
        height: btnH
        x: startX + btnW + gapX
        y: startY + btnH + gapY
        onClicked: Navigation.push("InfoPage.qml")
    }
     StyledButton {
        text: "Exit"
        width: 150
        height: 80

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20

        onClicked: Qt.quit()
    }
}
