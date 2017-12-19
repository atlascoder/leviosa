import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Extras 1.4

import "DefaultTheme.js" as DefTheme

Page {
	id: addGroupSchedule
	visible: true
	width: 480
	height: 800

	signal menuClicked()

	header: ToolBar {
		id: toolbar
		background: Rectangle {
			anchors.fill: parent
			color: DefTheme.mainColorDark
			layer.enabled: true
			layer.effect: DropShadow {
				anchors.fill: parent
				transparentBorder: true
				radius: 3
			}
		}
		RowLayout {
			anchors.fill: parent
			ToolButton {
				id: menuButton
				Image {
					id: menuIcon
					fillMode: Image.PreserveAspectFit
					source: "img/002-left-arrow.svg"
					anchors.fill: parent
					anchors.margins: width / 4
				}
				onClicked: menuClicked()
			}
			Label {
				id: pageTitle
				text: "Set schedule"
				verticalAlignment: Qt.AlignVCenter
				horizontalAlignment: Qt.AlignHCenter
				Layout.fillWidth: true
				color: DefTheme.mainTextColor
				font.pixelSize: parent.height / 2
			}
			ToolButton{
				id: addButton
				enabled: false
			}
		}
	}

	background: Rectangle {
		anchors.fill: parent
		color: DefTheme.mainColorBg
	}

	Column {
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: 12
		spacing: 12
		Text {
			text: "Week days:"
		}
		Row {
			width: parent.width
			WeekDayCheckBox {
				text: "Sun"
				width: parent.width / 7
			}
			WeekDayCheckBox {
				text: "Mon"
				width: parent.width / 7
			}
			WeekDayCheckBox {
				text: "Tue"
				width: parent.width / 7
			}
			WeekDayCheckBox {
				text: "Wed"
				width: parent.width / 7
			}
			WeekDayCheckBox {
				text: "Thu"
				width: parent.width / 7
			}
			WeekDayCheckBox {
				text: "Fri"
				width: parent.width / 7
			}
			WeekDayCheckBox {
				text: "Sat"
				width: parent.width / 7
			}
		}

		Text {
			width: parent.width
			font.underline: true
			font.bold: true
			text: "Open at"
			horizontalAlignment: Text.AlignHCenter
		}

		Row {
			anchors.horizontalCenter: parent.horizontalCenter
			SpinBox {
				id: openHour
				from: 0
				to: 12
				value: 7
				stepSize: 1
				font.pixelSize: height * 0.5
				font.bold: true
			}
			Text {
				height: parent.height
				text: ":"
				font.pixelSize: height * 0.5
				font.bold: true
				leftPadding: 6
				rightPadding: 6
				verticalAlignment: Qt.AlignVCenter
			}
			SpinBox {
				id: openMinute
				from: 0
				to: 55
				value: 30
				stepSize: 5
				font.pixelSize: height * 0.5
				font.bold: true
			}
			Text{
				text: "AM"
				height: parent.height
				font.pixelSize: height * 0.5
				leftPadding: 12
				rightPadding: 3
				verticalAlignment: Qt.AlignVCenter
				font.bold: true
				opacity: openTimePM.checked ? 0.3 : 1
			}
			Switch {
				id: openTimePM
				checked: false
			}
			Text{
				text: "PM"
				height: parent.height
				font.pixelSize: height * 0.5
				leftPadding: 3
				rightPadding: 12
				verticalAlignment: Qt.AlignVCenter
				font.bold: true
				opacity: openTimePM.checked ? 1 : 0.3
			}
		}

		Text {
			width: parent.width
			font.underline: true
			font.bold: true
			text: "Close at"
			horizontalAlignment: Text.AlignHCenter
		}

		Row {
			anchors.horizontalCenter: parent.horizontalCenter
			SpinBox {
				id: closeHour
				from: 0
				to: 12
				value: 7
				stepSize: 1
				font.pixelSize: height * 0.5
				font.bold: true
			}
			Text {
				height: parent.height
				text: ":"
				font.pixelSize: height * 0.5
				font.bold: true
				leftPadding: 6
				rightPadding: 6
				verticalAlignment: Qt.AlignVCenter
			}
			SpinBox {
				id: closeMinute
				from: 0
				to: 55
				stepSize: 5
				font.pixelSize: height * 0.5
				font.bold: true
			}
			Text{
				text: "AM"
				height: parent.height
				font.pixelSize: height * 0.5
				leftPadding: 12
				rightPadding: 3
				verticalAlignment: Qt.AlignVCenter
				font.bold: true
				opacity: closeTimePM.checked ? 0.3 : 1
			}
			Switch {
				id: closeTimePM
				checked: true
			}
			Text{
				text: "PM"
				height: parent.height
				font.pixelSize: height * 0.5
				leftPadding: 3
				rightPadding: 12
				verticalAlignment: Qt.AlignVCenter
				font.bold: true
				opacity: closeTimePM.checked ? 1 : 0.3
			}
		}

		Item {
			width: parent.width
			height: 20
		}

		Item {
			width: parent.width
			height: childrenRect.height

			ActionButton {
				id: cancelBtn
				text: "Cancel"
				anchors.top: parent.top
				anchors.right: parent.horizontalCenter
				anchors.left: parent.left
				anchors.leftMargin: 6
				height: 40
			}

			ActionButton {
				id: saveBtn
				text: "Save"
				anchors.top: parent.top
				anchors.left: parent.horizontalCenter
				anchors.right: parent.right
				anchors.leftMargin: 6
				height: 40
			}

		}
	}

}
