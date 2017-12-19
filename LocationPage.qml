import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import "DefaultTheme.js" as DefTheme

Page {
	id: locationPage
	visible: true
	width: 480
	height: 800

	property string bss
	property string activeController

	signal menuClicked()
	signal editController(string mac)
	signal editGroup(string n)
	signal editSchedule(string n)
	signal addGroup(string controller)

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
				text: bss
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
				onClicked: addGroup(activeController)
			}
		}
	}

	background: Rectangle {
		anchors.fill: parent
		color: DefTheme.mainColorBg
	}

	Image {
		id: image
		anchors.centerIn: parent
		width: parent.width*0.75
		fillMode: Image.PreserveAspectFit
		source: "img/logo-blue.svg"
		opacity: 0.2
	}

	footer: TabBar {
		id: bar
		currentIndex: controllersPager.currentIndex
		visible: controllersPager.count > 1
		onCurrentIndexChanged: {
			activeController = currentIndex;
		}
		Repeater {
			model: ["My House", "Guest house", "Pool"]

			TabButton {
				text: modelData
				onPressAndHold: editController(model[bar.currentIndex])
			}
		}
	}

	SwipeView {
		id: controllersPager
		anchors.fill: parent
		currentIndex: bar.currentIndex
		Item {
			ControllerPage {
				anchors.fill: parent
				onEditShadesGroup: function(n){
					editGroup(n);
				}
				onEditGroupSchedule: function(n){
					editSchedule(n);
				}
				shadeGroups: ListModel {

					ListElement {
						name: "Kitchen"
					}

					ListElement {
						name: "Rose Bedroom"
					}

					ListElement {
						name: "Blue Bedroom"
					}

					ListElement {
						name: "Child Room"
					}

					ListElement {
						name: "Penthouse"
					}

					ListElement {
						name: "Study"
					}

					ListElement {
						name: "Hall"
					}
				}
			}
		}
		Item {
			ControllerPage {
				anchors.fill: parent
				onEditShadesGroup: function(n){
					editGroup(n);
				}
				onEditGroupSchedule: function(n){
					editSchedule(n);
				}
				shadeGroups: ListModel {

					ListElement {
						name: "Kitchen"
					}

					ListElement {
						name: "Bedroom"
					}

					ListElement {
						name: "Cinema"
					}

					ListElement {
						name: "Hall"
					}
				}
			}
		}
		Item {
			ControllerPage {
				anchors.fill: parent
				onEditShadesGroup: function(n){
					editGroup(n);
				}
				onEditGroupSchedule: function(n){
					editSchedule(n);
				}
				shadeGroups: ListModel {

					ListElement {
						name: "Roof"
					}

				}
			}
		}
	}



}
