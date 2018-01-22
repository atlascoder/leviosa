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
                onOpenLocation: function(id){
                    pager.push(locationPage, {"locationId": id});
                }
                onEditLocation: function(id){
                    pager.push(editLocationPage, {"locationId": id})
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
            property int locationId: -1
            sourceComponent: EditLocationPage {
                onMenuClicked: {
                    pager.pop()
                    if(pager.currentItem.id === locationsPage)
                        pager.replace(pager.currentItem, locationsPage);
                }
            }
            onLoaded: {
                item.locationId = locationId
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
        }
    }

    Component {
        id: locationPage
        Loader {
            property int locationId: -1
            sourceComponent: LocationPage {
                locationId: pager.selectedLocationId
                onMenuClicked: pager.pop()
                onEditController: function(id){
                    pager.selectedControllerId = id;
                    pager.push(editControllerPage)
                }
                onEditGroup:function(cid, gid){
                    pager.selectedShadesGroupId = gid;
                    pager.selectedControllerId = cid;
                    console.log("edit cid:gid " + cid + ":" + gid);
                    pager.push(editShadesGroupPage);
                }
                onEditSchedule: function(cid, gid){
                    pager.selectedControllerId = cid;
                    pager.selectedShadesGroupId = gid;
                    pager.push(editGroupSchedule);
                }
                onAddGroup: function(controllerId){
                    pager.selectedControllerId = controllerId;
                    pager.push(newShadesGroupPage);
                }
                onSetupController: function(locationId){
                    pager.selectedLocationId = locationId;
                    pager.push(espTouchPage);
                }
                onTitleLongPressed: {
                    pager.push(editLocationPage)
                }
            }
            onLoaded: {
                item.locationId = locationId
            }
            StackView.onActivated: {
                drawer.interactive = true;
            }
        }
    }

    Component {
        id: newShadesGroupPage
        Loader {
            sourceComponent: NewShadesGroupPage {
                controllerId: pager.selectedControllerId
                onMenuClicked: pager.pop()
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
        }
    }

    Component {
        id: editShadesGroupPage
        Loader {
            sourceComponent: EditShadesGroupPage {
                controllerId: pager.selectedControllerId;
                shadesGroupId: pager.selectedShadesGroupId;
                onMenuClicked: pager.pop();
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
        }
    }

    Component {
        id: editGroupSchedule
        Loader {
            sourceComponent: EditGroupSchedule {
                controllerId: pager.selectedControllerId
                shadesGroupId: pager.selectedShadesGroupId
                onMenuClicked: pager.pop()
            }
            StackView.onActivated: {
                drawer.interactive = false;
            }
        }
    }

    Component {
        id: editControllerPage
        Loader {
            sourceComponent: EditControllerPage {
                locationId: pager.selectedLocationId
                controllerId: pager.selectedControllerId
                onMenuClicked: pager.pop()
            }
            StackView.onActivated: {
                drawer.interactive = false;
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
