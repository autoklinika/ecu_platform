import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    width: parent.width
    height: parent.height * (60 / 480)

    property bool online: true
    property string vin: "YS2R4X20005399401"

    signal menuClicked()

    readonly property real scaleX: width / 800
    readonly property real scaleY: parent.height / 480

    Rectangle {
        anchors.fill: parent
        color: "#CFCFCF"
    }

    /*
        SAC MODULE text (20,20)
    */


    /*
        MENU button (10,10,120,40)
        overlay – jak Arduino
    */
    StyledButton {
        x: 10 * scaleX
        y: 10 * scaleY
        width: 120 * scaleX
        height: 40 * scaleY
        text: LanguageManager.t("kafelek_menu")
        onClicked: root.menuClicked()
    }

    /*
        STATUS (centerX = 400)
    */
    Item {
        x: (400 * scaleX)
        y: 18 * scaleY

        Row {
            spacing: 6 * scaleX

            Rectangle {
                width: 10 * scaleX
                height: 10 * scaleX
                radius: 5 * scaleX
                color: online ? "#00A000" : "#D00000"
            }

            Text {
                text: online ? "ONLINE" : "OFFLINE"
                font.pixelSize: 16 * scaleY
                font.bold: true
                color: online ? "#00A000" : "#D00000"
            }
        }
    }

    /*
        VIN right aligned like 780 - width
    */
    Text {
        id: vinText
        font.pixelSize: 16 * scaleY
        font.bold: true
        color: "#2A2A2A"

        text: {
            if (vin.length >= 7)
                return vin.slice(vin.length - 7)
            return vin
        }

        Component.onCompleted: {
            x = (780 * scaleX) - width
            y = 20 * scaleY
        }

        onWidthChanged: {
            x = (780 * scaleX) - width
        }
    }
}
