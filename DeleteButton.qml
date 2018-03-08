import QtQuick 2.0
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

import "DefaultTheme.js" as DefTheme

Button {
    id: control

    contentItem: Column {
        anchors.centerIn: control
        Text {
            id: delCaption
            anchors.horizontalCenter: parent.horizontalCenter
            text: "DELETE"
            font.bold: true
            color: DefTheme.mainTextColor
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "press and hold"
            font.pixelSize: delCaption.font.pixelSize * 0.7
            horizontalAlignment: Qt.AlignHCenter
            color: DefTheme.mainTextColor
        }
    }

    background: Rectangle {
        color: DefTheme.mainNegativeAccent
        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            radius: down ? 1 : 4
        }
    }
}
