import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import "DefaultTheme.js" as DefTheme

Page {
	id: locationsPage
	visible: true
	width: 480
	height: 800
	title: "Locations"

	signal openLocation(string bss)
	signal editLocation(string bss)
	signal menuClicked()
	signal addClicked()

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
					source: "img/004-bars.svg"
					anchors.fill: parent
					anchors.margins: width / 4
				}
				onClicked: menuClicked()
			}
			Label {
				id: pageTitle
				text: title
				verticalAlignment: Qt.AlignVCenter
				horizontalAlignment: Qt.AlignHCenter
				Layout.fillWidth: true
				color: DefTheme.mainTextColor
				font.pixelSize: parent.height / 2
			}
			ToolButton{
				id: addButton
				Image {
					fillMode: Image.PreserveAspectFit
					source: "img/plus-white.svg"
					anchors.fill: parent
					anchors.margins: width / 4
				}
				onClicked: addClicked()
			}
		}
	}


	background: Rectangle {
		anchors.fill: parent
		color: DefTheme.mainColorBg
	}

	GridView {
		id: gridView
		anchors.fill: parent
		cellWidth: {
			if (parent.width <= 240)
				return parent.width
			else{
				var n = 1;
				while(parent.width / n > 240){ n++; }
				return Math.round(parent.width/n);
			}
		}
		cellHeight: cellWidth
		model: locationsList
		delegate: Item {
			id: locItem
			width: gridView.cellWidth
			height: gridView.cellHeight
			Button {
				id: itemRect
				anchors.fill: parent
				anchors.margins: 10
				background: Rectangle {
					radius: 4
					layer.enabled: true
					layer.effect: DropShadow {
						radius: itemRect.down ? 1 : 3
						transparentBorder: true
					}
					color: DefTheme.mainColorLight
				}

				Text {
					id: timeline
					width: parent.width
					anchors.top: parent.top
					anchors.margins: height / 3
					horizontalAlignment: Qt.AlignHCenter
					color: DefTheme.mainTextColor
					text: new Date(new Date().getTime() + utcOffset*3600000).toLocaleTimeString("en-US")
					font.pixelSize: width / 10
				}

				Text {
					id: nameLine
					width: parent.width
					anchors.bottom: parent.bottom
					anchors.margins: 4
					horizontalAlignment: Qt.AlignHCenter
					text: name
					font.pixelSize: parent.height / 8
					color: DefTheme.mainTextColor
					elide: Text.ElideRight
				}
				Item {
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.top: timeline.bottom
					anchors.bottom: nameLine.top
					anchors.margins: parent.width / 6
					Image {
						id: icon
						anchors.fill: parent
						verticalAlignment: Qt.AlignVCenter
						horizontalAlignment: Qt.AlignHCenter
						fillMode: Image.PreserveAspectFit
						source: lan ? "img/001-computer-1.svg" : "img/001-internet.svg"
					}
					Text {
						anchors.top: icon.bottom
						anchors.margins: 4
						anchors.horizontalCenter: icon.horizontalCenter
						text: lan ? "You are here" : "Remote"
						color: online ? DefTheme.mainPositiveAccent : DefTheme.mainNegativeAccent
					}
					Text {
						anchors.bottom: icon.top
						anchors.margins: 4
						anchors.horizontalCenter: icon.horizontalCenter
						text: online ? "online" : "offline"
						color: online ? DefTheme.mainPositiveAccent : DefTheme.mainNegativeAccent
					}
					ColorOverlay {
						anchors.fill: parent
						source: icon
						color: online ? DefTheme.mainPositiveAccent : DefTheme.mainNegativeAccent
					}
				}
				onClicked: {
					openLocation(name);
				}
				onPressAndHold: {
					editLocation(name);
				}
			}
		}
	}

	ListModel {
		id: locationsList
		ListElement {
			bss: "mac1"
			name: "Family Rancho"
			tzone: "Eastern"
			utcOffset: -5
			lan: true
			online: true
		}
		ListElement {
			bss: "mac2"
			name: "Town House"
			tzone: "Central"
			utcOffset: -6
			lan: false
			online: true
		}
		ListElement {
			bss: "mac3"
			name: "Flat"
			tzone: "Pacific"
			utcOffset: -8
			lan: false
			offline: false
		}
	}

}
