import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Button {
    id: root
    hoverEnabled: true

    Theme { id: theme }

    background: Rectangle {
        anchors.fill: parent
        radius: 16

        color: {
            if (root.down)
                return theme.buttonPressed
            if (root.hovered)
                return theme.buttonHover
            return theme.buttonColor
        }

        border.color: theme.borderColor
        border.width: 3

        Behavior on color {
            ColorAnimation { duration: 80 }
        }
    }

    contentItem: Item {
        anchors.fill: parent

        Text {
            text: root.text
            anchors.centerIn: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: theme.textColor
            font.pixelSize: 40
            font.bold: true
        }
    }
}
