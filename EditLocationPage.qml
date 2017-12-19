import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

import "DefaultTheme.js" as DefTheme

Page {
	id: editLocation
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
				text: "Location edit"
				verticalAlignment: Qt.AlignVCenter
				horizontalAlignment: Qt.AlignHCenter
				Layout.fillWidth: true
				color: DefTheme.mainTextColor
				font.pixelSize: parent.height / 2
			}
			ToolButton{
				id: addButton
				enabled: false
				Image {
					fillMode: Image.PreserveAspectFit
					anchors.fill: parent
					anchors.margins: width / 4
				}
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
			text: "Name:"
		}
		TextField {
			width: parent.width
			height: 40
			horizontalAlignment: Qt.AlignHCenter
			verticalAlignment: Qt.AlignVCenter
			font.pixelSize: height*0.6
			background: Rectangle {
				anchors.fill: parent
				color: DefTheme.mainInputBg
			}
		}

		Text {
			text: "Time zone:"
		}

		ComboBox {
			id: timezoneCB
			width: parent.width
			height: 40
			model: ["Eastern", "Central", "Mountain", "Pacific", "Alaska", "Hawaii"]
			contentItem: Text {
				leftPadding: timezoneCB.indicator.width + timezoneCB.spacing
				rightPadding: timezoneCB.indicator.width + timezoneCB.spacing
				text: timezoneCB.displayText
				font.pixelSize: height * 0.6
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				elide: Text.ElideRight
			}
			background: Rectangle {
				anchors.fill: parent
				color: DefTheme.mainInputBg
			}
		}


		Text {
			text: "WiFi access point MAC:"
		}

		Text {
			width: parent.width
			height: 40
			horizontalAlignment: Qt.AlignHCenter
			verticalAlignment: Qt.AlignVCenter
			font.pixelSize: height*0.6
			color: DefTheme.mainDisabledTextColor
			text: "12:12:12:12:12:12"
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
