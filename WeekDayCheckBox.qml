import QtQuick 2.7
import QtQuick.Controls 2.2

CheckBox {
	id: control
	text: "Monday"

	property int indicatorSize : 26
	property int checkSize : 14

	contentItem: Text {
		id: controlText
		x: control.leftPadding
		y: control.topPadding
		text: control.text
		font: control.font
		opacity: enabled ? 1.0 : 0.3
		color: control.down ? "#a1a1a1" : "#000000"
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignBottom
	}

	indicator: Rectangle {
		id: indicatorRect
		implicitWidth: indicatorSize
		implicitHeight: indicatorSize
		x: (parent.width - width) / 2
		y: control.topPadding
		radius: indicatorSize * 0.1
		border.color: control.down ? "#a1a1a1" : "#000000"

		Rectangle {
			width: checkSize
			height: checkSize
			x: (indicatorSize - checkSize) / 2
			y: (indicatorSize - checkSize) / 2
			radius: checkSize * 0.1
			color: control.down ? "#a1a1a1" : "#000000"
			visible: control.checked
		}
	}

	height: control.topPadding + controlText.contentHeight + font.pixelSize*0.5 + indicatorSize + control.bottomPadding

}
