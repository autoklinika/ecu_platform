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
        anchors.verticalCenterOffset: -10
        spacing: 34

       

        Grid {
            anchors.horizontalCenter: parent.horizontalCenter
            columns: 3
            rowSpacing: 28
            columnSpacing: 28

            StyledButton {
                width: 220
                height: 110
                text: "DAF"
                onClicked: Navigation.push("BrandPage.qml", { brand: "DAF" })
            }

            StyledButton {
                width: 220
                height: 110
                text: "MAN"
            }

            StyledButton {
                width: 220
                height: 110
                text: "SCANIA"
            }

            StyledButton {
                width: 220
                height: 110
                text: "IVECO"
            }

            StyledButton {
                width: 220
                height: 110
                text: "MB"
            }

            StyledButton {
                width: 220
                height: 110
                text: "VOLVO"
            }
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
