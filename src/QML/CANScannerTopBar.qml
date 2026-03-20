import QtQuick
import QtQuick.Controls
import ecu_gui 1.0

Item {
    id: root
    width: parent ? parent.width : 1280
    height: shortcutModel.length > 0 ? 152 : 96

    property string titleText: "CAN SCANNER"
    property string rightText: ""
    property bool online: true
    property string leftButtonText: "MENU"
    property var shortcutModel: []

    signal menuClicked()

    Theme { id: theme }

    Rectangle {
        anchors.fill: parent
        color: theme.topBarColor
    }

    StyledButton {
        id: leftButton
        anchors.left: parent.left
        anchors.leftMargin: 16
        anchors.top: parent.top
        anchors.topMargin: 16
        width: 192
        height: 64
        text: root.leftButtonText
        onClicked: root.menuClicked()
    }

    Text {
        id: titleLabel
        text: root.titleText
        anchors.left: leftButton.right
        anchors.leftMargin: 24
        anchors.verticalCenter: statusRow.verticalCenter
        font.pixelSize: 28
        font.bold: true
        color: theme.textColor
        elide: Text.ElideRight
        width: Math.max(180, parent.width * 0.28)
    }

    Row {
        id: statusRow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 18
        spacing: 12

        Rectangle {
            width: 16
            height: 16
            radius: 8
            anchors.verticalCenter: parent.verticalCenter
            color: root.online ? "#0A9F34" : "#D22D2D"
        }

        Text {
            text: root.online ? "ONLINE" : "OFFLINE"
            font.pixelSize: 26
            font.bold: true
            color: root.online ? "#0A9F34" : "#D22D2D"
        }
    }

    Text {
        id: rightLabel
        text: root.rightText
        anchors.right: parent.right
        anchors.rightMargin: 32
        anchors.verticalCenter: statusRow.verticalCenter
        font.pixelSize: 24
        font.bold: true
        color: theme.textColorDark
        horizontalAlignment: Text.AlignRight
        elide: Text.ElideRight
        width: Math.max(220, parent.width * 0.22)
    }

    ListView {
        id: shortcutsView
        visible: root.shortcutModel && root.shortcutModel.length > 0
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 24
        anchors.rightMargin: 24
        anchors.bottomMargin: 14
        height: 42
        orientation: ListView.Horizontal
        spacing: 10
        clip: true
        interactive: contentWidth > width
        boundsBehavior: Flickable.StopAtBounds
        model: root.shortcutModel

        delegate: Rectangle {
            required property var modelData
            width: shortcutRow.width + 26
            height: 42
            radius: 10
            color: "#FFFFFF"
            border.color: theme.separatorColor

            Row {
                id: shortcutRow
                anchors.centerIn: parent
                spacing: 8

                Text {
                    text: modelData.label ? modelData.label + ":" : ""
                    font.pixelSize: 18
                    font.bold: true
                    color: theme.textColorMuted
                }

                Text {
                    text: modelData.value ? String(modelData.value) : "---"
                    font.pixelSize: 18
                    font.bold: true
                    color: theme.textColorDark
                }
            }
        }

        ScrollBar.horizontal: ScrollBar { }
    }
}