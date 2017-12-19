import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import com.atlascoder.netmon 1.0

import "DefaultTheme.js" as DefTheme

ApplicationWindow {
	id: applicationWindow
	visible: true
	width: 480
	height: 800
	color: DefTheme.mainColorBg
	title: "Leviosa Smart Shades"

	signal authenticate
	signal register
	signal restore
	signal signedIn
	signal signedUp
	signal signedOut
	signal espTouchStart
	signal espTouchFinish

	NetMon {
		id: netmon
	}

	StackView {
		id: pager
		anchors.fill: parent
		initialItem: welcomePage

		Component {
			id: welcomePage
			Loader {
				sourceComponent: WelcomePage {
					anchors.fill: parent
					onAuthenticate: pager.replace(pager.currentItem, authPage)
					onRegister: pager.replace(pager.currentItem, regPage)
				}
			}
		}

		Component {
			id: authPage
			Loader {
				sourceComponent: AuthPage {
					onRegister: pager.replace(pager.currentItem, regPage)
					onSignedIn: pager.replace(pager.currentItem, locationsPage)
					onRestore: pager.replace(pager.currentItem, welcomePage);
				}
			}
		}

		Component {
			id: regPage
			Loader {
				sourceComponent: RegPage {
					onAuthenticate: pager.replace(pager.currentItem, authPage)
					onRestore: pager.replace(pager.currentItem, welcomePage)
					onSignedUp: pager.replace(pager.currentItem, espTouchPage)
				}
			}
		}
		property string selectedBss
		Component {
			id: locationsPage
			Loader {
				sourceComponent: LocationsPage {
					onMenuClicked: drawer.open()
					onOpenLocation: function(bss){
						pager.selectedBss = bss;
						pager.push(locationPage);
					}
					onEditLocation: function(name){
						pager.selectedBss = name;
						pager.push(editLocationPage)
					}
					onAddClicked: pager.push(newLocationPage)
				}
				onLoaded: {
					drawer.interactive = true;
				}
			}
		}

		Component {
			id: newLocationPage
			Loader {
				sourceComponent: NewLocationPage {
					onMenuClicked: pager.pop()
				}
			}
		}

		Component {
			id: editLocationPage
			Loader {
				sourceComponent: EditLocationPage {
					onMenuClicked: pager.pop()
				}
			}
		}

		Component {
			id: locationPage
			Loader {
				sourceComponent: LocationPage {
					bss: pager.selectedBss
					onMenuClicked: pager.pop()
					onEditController: pager.push(editControllerPage)
					onEditGroup: pager.push(editShadesGroupPage)
					onEditSchedule: pager.push(editGroupSchedule)
					onAddGroup: function(cntlr){
						pager.push(newShadesGroupPage)
					}
				}
			}
		}

		Component {
			id: newShadesGroupPage
			Loader {
				sourceComponent: NewShadesGroupPage {
					onMenuClicked: pager.pop()
				}
			}
		}

		Component {
			id: editShadesGroupPage
			Loader {
				sourceComponent: EditShadesGroupPage {
					onMenuClicked: pager.pop()
				}
			}
		}

		Component {
			id: editGroupSchedule
			Loader {
				sourceComponent: EditGroupSchedule {
					onMenuClicked: pager.pop()
				}
			}
		}

		Component {
			id: editControllerPage
			Loader {
				sourceComponent: EditControllerPage {
					onMenuClicked: pager.pop()
				}
			}
		}

		Component {
			id: newControllerPage
			Loader {
				sourceComponent: NewControllerPage {
					onMenuClicked: pager.pop()
				}
			}
		}

		Component {
			id: espTouchPage
			Loader {
				sourceComponent: EspTouchPage {
					onEspTouchFinshed: pager.replace(pager.currentItem, locationsPage)
					StackView.onActivated: {
						pageTitle = "Controller setup"
					}
				}
			}
		}
	}

	Drawer {
		id: drawer
		width:  0.66 * applicationWindow.width
		height: applicationWindow.height
		edge: Qt.LeftEdge
		interactive: false
		background: Rectangle {
			color: DefTheme.secColorLight
		}

		Column {
			anchors.fill: parent
			anchors.margins: parent.width / 20
			spacing: 6

			Text{
				height: parent.width / 8
				anchors.horizontalCenter: parent.horizontalCenter
				verticalAlignment: Qt.AlignVCenter
				text: "customer@gmail.com"
				font.pixelSize: 16
				color: DefTheme.secTextColor
				font.bold: true
			}

			Button {
				anchors.horizontalCenter: parent.horizontalCenter
				text: "Sign Out"
				onClicked: {
					signedOut();
					drawer.close();
					drawer.interactive = false;
					pager.replace(pager.currentItem, welcomePage);
				}
			}

			Rectangle {
				width: parent.width
				height: 2
				color: DefTheme.secColorDark
			}

			Button {
				width: parent.width
				text: "Connect Controller to WiFi"
				onClicked: {
					drawer.close()
					pager.push(espTouchPage);
				}
			}

			Button {
				width: parent.width
				text: "Call to Support"
				onClicked: {
					Qt.openUrlExternally("tel:%1".arg("+19802061260")) ;
				}
			}

			Button {
				width: parent.width
				text: "Open WebSite"
				onClicked: {
					Qt.openUrlExternally("https://leviosashades.com") ;
				}
			}


		}
	}

	onClosing: {
		if (Qt.platform.os == "android" && pager.depth > 1) {
			close.accepted = false;
			pager.pop();
		}
	}

	Text {
		id: networkState
		anchors.bottom: parent.bottom
		anchors.margins: 10
		anchors.horizontalCenter: parent.horizontalCenter
		font.pixelSize: parent.hight / 12
		text: netmon.onlineState
	}
}
