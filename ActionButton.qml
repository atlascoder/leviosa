import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import "DefaultTheme.js" as DefTheme

Button {
	background: Rectangle {
		anchors.fill: parent
		color: DefTheme.mainInputBg
		layer.enabled: true
		layer.effect: DropShadow {
			transparentBorder: true
			radius: parent.down ? 1 : 2
		}
	}
}
