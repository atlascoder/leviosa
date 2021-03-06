import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import "DefaultTheme.js" as DefTheme

Button {
    id: control
    height: DefTheme.buttonHeight

    property bool outline: false

    contentItem: Text {
        id: captionText
        horizontalAlignment: Qt.AlignHCenter
        color: control.outline ? DefTheme.mainColorLight : DefTheme.mainColorBg
        font.pixelSize: parent.height * 0.6
        verticalAlignment: Qt.AlignVCenter
        elide: Text.ElideRight
        leftPadding: parent.height / 4
        rightPadding: parent.height / 4
        text: control.text
    }
	background: Rectangle {
		anchors.fill: parent
        color: control.outline ? DefTheme.mainColorBg : DefTheme.mainColorLight
        border.width: control.outline ? 1 : 0
        border.color: DefTheme.mainColorLight
		layer.enabled: true
        radius : height / 2.714
		layer.effect: DropShadow {
			transparentBorder: true
            radius: parent.down ? 1 : 4
		}
        opacity: control.enabled ? 1 : 0.4
	}
}
