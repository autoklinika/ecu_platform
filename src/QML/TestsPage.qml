import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {

    property int btnW: 280
    property int btnH: 120
    property int gapX: 60
    property int gapY: 60

    property int cols: 3
    property int rows: 2

    // Wysokość siatki
    property int gridHeight: rows * btnH + (rows - 1) * gapY
    property int gridWidth: cols * btnW + (cols - 1) * gapX

    // Obszar roboczy (bez BACK)
    property int bottomReserved: 150   // 90 przycisk + 40 margin + zapas

    // Środek między topem strony a dolnym przyciskiem
    property int startX: (width - gridWidth) / 2
    property int startY: (height - bottomReserved - gridHeight) / 2

    property var brands: ["DAF","MAN","SCANIA","IVECO","MB","VOLVO"]

    Repeater {
        model: brands.length

        StyledButton {
            width: btnW
            height: btnH

            property int col: index % cols
            property int row: Math.floor(index / cols)

            x: startX + col * (btnW + gapX)
            y: startY + row * (btnH + gapY)

            text: brands[index]

            onClicked: {
                if (brands[index] === "DAF")
                    Navigation.push("BrandPage.qml", { brand: "DAF" })
            }
        }
    }

    // BACK – stała pozycja przy dole
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
