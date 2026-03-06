import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root

    // Values can be provided from Navigation.push(..., { vin, sw, hw })
    property string vin: CockpitController.vin.length > 0 ? CockpitController.vin : "---"
    property string sw: CockpitController.sw.length > 0 ? CockpitController.sw : "---"
    property string hw: CockpitController.hw.length > 0 ? CockpitController.hw : "---"

    Column {
        anchors.centerIn: parent
        spacing: 30

        Text {
            text: "VIN"
            font.pixelSize: 24
            color: "#666666"
            horizontalAlignment: Text.AlignHCenter
            width: 700
        }

        Text {
            text: root.vin
            font.pixelSize: 40
            font.bold: true
            color: "#2A2A2A"
            horizontalAlignment: Text.AlignHCenter
            width: 900
            wrapMode: Text.WrapAnywhere
        }

        Rectangle {
            width: 700
            height: 2
            color: "#CCCCCC"
        }

        Text {
            text: "SW: " + root.sw
            font.pixelSize: 26
            color: "#444444"
            horizontalAlignment: Text.AlignHCenter
            width: 700
            wrapMode: Text.WrapAnywhere
        }

        Text {
            text: "HW: " + root.hw
            font.pixelSize: 26
            color: "#444444"
            horizontalAlignment: Text.AlignHCenter
            width: 700
            wrapMode: Text.WrapAnywhere
        }

        StyledButton {
            width: 240
            height: 90
            text: LanguageManager.t("kafelek_ok")
            onClicked: Navigation.push("SACMenuPage.qml")
        }
    }
}
