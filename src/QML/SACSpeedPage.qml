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
        spacing: 38

       

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 80

            StyledButton {
                width: 360
                height: 160
                text: LanguageManager.t("kafelek_can_250")
                onClicked: Navigation.push("SACInitPage.qml", { speed: 250 })
            }

            StyledButton {
                width: 360
                height: 160
                text: LanguageManager.t("kafelek_can_500")
                onClicked: Navigation.push("SACInitPage.qml", { speed: 500 })
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
        onClicked: {
            CockpitController.disconnect()
            SystemController.resetCAN("can0")
            Navigation.pop()
        }
    }
}
