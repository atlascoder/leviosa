import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Extras 1.4

import "DefaultTheme.js" as DefTheme

Page {
	id: editGroupSchedule
	visible: true
	width: 480
	height: 800

	signal menuClicked()

	header: ToolBar {
		id: toolbar
		height: DefTheme.toolbarHeight
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

		Item {
			id: openTimeItem
			width: parent.width
			Text{
				id: openTimeDelim
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: parent.width / 10
				text: ":"
			}
			Text{
				anchors.right: openTimeDelim.left
				font.pixelSize: parent.width / 10
				text: Math.floor(openSlider.value)
			}
			Text{
				anchors.left: openTimeDelim.right
				font.pixelSize: parent.width / 10
				text: Math.round(60*(openSlider.value % 1))
			}
			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				Text{
					text: "AM"
					height: parent.height
					font.pixelSize: parent.width / 10
					leftPadding: 12
					rightPadding: 3
					verticalAlignment: Qt.AlignVCenter
					font.bold: true
					opacity: openTimePM.checked ? 0.3 : 1
				}
				Switch {
					id: openTimePM
					width: openTimeItem.width / 6
					height: width / 3
					anchors.verticalCenter: parent.verticalCenter
					checked: false
				}
				Text{
					text: "PM"
					height: parent.height
					font.pixelSize: parent.width / 10
					leftPadding: 3
					rightPadding: 12
					verticalAlignment: Qt.AlignVCenter
					font.bold: true
					opacity: openTimePM.checked ? 1 : 0.3
				}
			}
			height: childrenRect.height
		}

		Slider {
			id: openSlider
			from: 1
			to: 12.75
			stepSize: 0.25
			value: 7
			width: parent.width
		}


		Text {
			width: parent.width
			font.underline: true
			font.bold: true
			text: "Close at"
			horizontalAlignment: Text.AlignHCenter
		}

		Item {
			id: closeTimeItem
			width: parent.width
			Text{
				id: closeTimeDelim
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: parent.width / 10
				text: ":"
			}
			Text{
				anchors.right: closeTimeDelim.left
				font.pixelSize: parent.width / 10
				text: Math.floor(closeSlider.value)
			}
			Text{
				anchors.left: closeTimeDelim.right
				font.pixelSize: parent.width / 10
				text: Math.round(60*(closeSlider.value % 1))
			}
			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				Text{
					text: "AM"
					height: parent.height
					font.pixelSize: parent.width / 10
					leftPadding: 12
					rightPadding: 3
					verticalAlignment: Qt.AlignVCenter
					font.bold: true
					opacity: closeTimePM.checked ? 0.3 : 1
				}
				Switch {
					id: closeTimePM
					width: closeTimeItem.width / 6
					height: width / 3
					anchors.verticalCenter: parent.verticalCenter
					checked: true
				}
				Text{
					text: "PM"
					height: parent.height
					font.pixelSize: parent.width / 10
					leftPadding: 3
					rightPadding: 12
					verticalAlignment: Qt.AlignVCenter
					font.bold: true
					opacity: closeTimePM.checked ? 1 : 0.3
				}
			}
			height: childrenRect.height
		}

		Slider {
			id: closeSlider
			from: 1
			to: 12.75
			stepSize: 0.25
			value: 7
			width: parent.width
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
