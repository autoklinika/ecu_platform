import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {

    property string vin: "YS2R4X20005399401"
    property string sw:  "SW_2.14.07"
    property string hw:  "HW_1.02"

    Column {
        anchors.centerIn: parent
        spacing: 30

        Text {
            text: "VIN"
            font.pixelSize: 24
            color: "#666666"
        }

        Text {
            text: vin
            font.pixelSize: 40
            font.bold: true
            color: "#2A2A2A"
        }

        Rectangle {
            width: 600
            height: 2
            color: "#CCCCCC"
        }

        Text {
            text: "SW: " + sw
            font.pixelSize: 26
            color: "#444444"
        }

        Text {
            text: "HW: " + hw
            font.pixelSize: 26
            color: "#444444"
        }

        StyledButton {
            width: 240
            height: 90
            text: LanguageManager.t("kafelek_ok")
            onClicked: Navigation.push("SACMenuPage.qml")
        }
    }
}
