import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {

    property int btnW: 360
    property int btnH: 160
    property int gapX: 120

    property int totalW: btnW * 2 + gapX
    property int startX: (width - totalW) / 2
    property int startY: (height - btnH) / 2

    // 250 kbps
    StyledButton {
        width: btnW
        height: btnH
        x: startX
        y: startY
        text: "250 kbps"

        onClicked: {
            // docelowo: start CAN 250
            Navigation.push("SACInitPage.qml", { speed: 250 })
        }
    }

    // 500 kbps
    StyledButton {
        width: btnW
        height: btnH
        x: startX + btnW + gapX
        y: startY
        text: "500 kbps"

        onClicked: {
            // docelowo: start CAN 500
            Navigation.push("SACInitPage.qml", { speed: 500 })
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
