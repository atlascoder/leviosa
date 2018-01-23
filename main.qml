import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.UserLogin 1.0

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
    signal changePassword

	signal signedIn
	signal signedUp
	signal espTouchStart
	signal espTouchFinish

    UserLogin {
        id: userLogin
    }

    Component {
        id: welcomePage
        Loader {
            sourceComponent: WelcomePage {
                anchors.fill: parent
                onAuthenticate: pager.replace(pager.currentItem, authPage)
                onRegister: pager.replace(pager.currentItem, regPage)
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
        }
    }

    Component {
        id: authPage
        Loader {
            sourceComponent: AuthPage {
                onRegister: pager.replace(pager.currentItem, regPage)
                onSignedIn: pager.replace(pager.currentItem, locationsPage)
                onRestore: pager.replace(pager.currentItem, restorePage);
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
        }
    }

    Component {
        id: regPage
        Loader {
            sourceComponent: RegPage {
                onAuthenticate: pager.replace(pager.currentItem, authPage)
                onRestore: pager.replace(pager.currentItem, restorePage)
                onSignedIn: pager.replace(pager.currentItem, locationsPage)
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
        }
    }

    Component {
        id: restorePage
        Loader {
            sourceComponent: RestorePage {
                onSignIn: pager.replace(pager.currentItem, authPage)
                onRegister: pager.replace(pager.currentItem, regPage)
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
        }
    }

    Component {
        id: changePasswordPage
        Loader {
            sourceComponent: ChangePwdPage {
                onCancel: pager.pop()
                onRestore: pager.replace(pager.currentItem, restorePage)
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
        }
    }


    Component {
        id: locationsPage
        Loader {
            sourceComponent: LocationsPage {
                onMenuClicked: drawer.open()
                onOpenLocation: function(uuid){
                    pager.push(locationPage, {"locationUuid": uuid});
                }
                onEditLocation: function(uuid){
                    pager.push(editLocationPage, {"locationUuid": uuid})
                }
                onAddClicked: pager.push(newLocationPage)
            }

            StackView.onActivating: {
                drawer.interactive = true;
                item.init();
            }

            StackView.onDeactivating: {
                item.pause();
            }

        }
    }

    Component {
        id: newLocationPage
        Loader {
            sourceComponent: NewLocationPage {
                onMenuClicked: pager.pop()
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
        }
    }

    Component {
        id: editLocationPage
        Loader {
            property string locationUuid
            sourceComponent: EditLocationPage {
                onMenuClicked: {
                    pager.pop()
                    if(pager.currentItem.id === locationsPage)
                        pager.replace(pager.currentItem, locationsPage);
                }
            }
            onLoaded: {
                item.locationUuid = locationUuid
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
        }
    }

    Component {
        id: locationPage
        Loader {
            property string locationUuid
            sourceComponent: LocationPage {
                locationUuid: pager.selectedLocationUuid
                onMenuClicked: pager.pop()
                onEditController: function(mac){
                    pager.push(editControllerPage, {"controllerMac": mac })
                }
                onEditGroup:function(mac, channel){
                    pager.push(editShadesGroupPage, {"controllerMac": mac, "selectedChannel": channel});
                }
                onEditSchedule: function(mac, channel){
                    pager.push(editGroupSchedule, {"controllerMac": mac, "selectedChannel": channel});
                }
                onAddGroup: function(mac){
                    pager.push(newShadesGroupPage, {"controllerMac": mac});
                }
                onSetupController: function(uuid){
                    pager.push(espTouchPage, {"locationUuid": uuid});
                }
                onTitleLongPressed: {
                    pager.push(editLocationPage)
                }
            }
            onLoaded: {
                item.locationUuid = locationUuid
            }
            StackView.onActivated: {
                drawer.interactive = true;
            }
        }
    }

    Component {
        id: newShadesGroupPage
        Loader {
            property string controllerMac
            sourceComponent: NewShadesGroupPage {
                onMenuClicked: pager.pop()
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
            onLoaded: {
                item.controllerMac = controllerMac;
            }
        }
    }

    Component {
        id: editShadesGroupPage
        Loader {
            property string controllerMac
            property int selectedChannel
            sourceComponent: EditShadesGroupPage {
                onMenuClicked: pager.pop();
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
            onLoaded: {
                item.controllerMac = controllerMac;
                item.selectedChannel = selectedChannel;
            }
        }
    }

    Component {
        id: editGroupSchedule
        Loader {
            property string controllerMac
            property int selectedChannel
            sourceComponent: EditGroupSchedule {
                onMenuClicked: pager.pop()
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
            onLoaded: {
                item.controllerMac = controllerMac;
                item.selectedChannel = selectedChannel;
            }
        }
    }

    Component {
        id: editControllerPage
        Loader {
            property string locationUuid
            property string controllerMac
            sourceComponent: EditControllerPage {
                onMenuClicked: pager.pop()
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
            onLoaded: {
                item.locatioUuid = locationUuid;
                item.controllerMac = controllerMac;
            }
        }
    }

    Component {
        id: newControllerPage
        Loader {
            sourceComponent: NewControllerPage {
                onMenuClicked: pager.pop()
            }
            StackView.onActivated: {
                drawer.interactive = false;
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
                onMenuClicked: {
                    pager.pop()
                }
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
        }
    }

    StackView {
		id: pager
		anchors.fill: parent

        Component.onCompleted: {
            if(userLogin.authState===UserLogin.Authenticated)
                pager.push(pager.initialItem, locationsPage);
            else
                pager.push(pager.initialItem, welcomePage);
        }

        property string selectedBss

        property int selectedLocationId : -1
        property int selectedControllerId : -1
        property int selectedShadesGroupId : -1

	}

	Drawer {
		id: drawer
		width:  0.66 * applicationWindow.width
		height: applicationWindow.height
        edge: Qt.LeftEdge
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
                text: userLogin.email
				font.pixelSize: 16
				color: DefTheme.secTextColor
				font.bold: true
			}

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Sign Out"
                onClicked: {
                    userLogin.signOut();
                    drawer.close();
                    drawer.interactive = false;
                    pager.clear();
                    pager.push(welcomePage);
                }
            }

            Rectangle {
				width: parent.width
				height: 2
				color: DefTheme.secColorDark
			}

			Button {
				width: parent.width
                text: "Setup Controller"
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

            Rectangle {
                width: parent.width
                height: 2
                color: DefTheme.secColorDark
            }

            Button {
                width: parent.width
                text: "Change password"
                onClicked: {
                    drawer.close();
                    drawer.interactive = false;
                    pager.push(pager.currentItem, changePasswordPage)
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

    Rectangle {
        anchors.fill: parent
        id: disconnected
        color: "#a0a0a0a0"
        visible: !netMonitor.connected
        MouseArea {
            anchors.fill: parent
            enabled: parent.visible
        }

        Text {
            anchors.centerIn: parent
            color: "#ff0000"
            text: "Connection required!"
        }
    }

}
