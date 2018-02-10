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

    function disableDrawer(){
        drawer.interactive = false;
    }

    // Welcome

    Loader {
        id: welcomePageLoader
        onLoaded: {
            item.onAuthenticate.connect(openAuthPage)
            item.onRegister.connect(openRegPage)
            item.StackView.onActivated.connect(disableDrawer)
        }
    }

    function openWelcomePage(){
        if(welcomePageLoader.status != Loader.Ready)
            welcomePageLoader.source = "WelcomePage.qml"
        pager.replace(pager.initialItem, welcomePageLoader.item)
    }

    // SignIn

    Loader {
        id: authPageLoader
        onLoaded: {
            item.onRegister.connect(openRegPage)
            item.onSignedIn.connect(openLocationsPage)
            item.onRestore.connect(openRestorePage)
            item.StackView.onActivated(disableDrawer)
        }
    }

    function openAuthPage(){
        if(authPageLoader.status != Loader.Ready)
            authPageLoader.source = "AuthPage.qml"
        pager.replace(pager.currentItem, authPageLoader.item)
    }

    // SignUp

    Loader {
        id: regPageLoader
        onLoaded: {
            item.onAuthenticate.connect(openAuthPage)
            item.onRestore.connect(openRestorePage)
            item.onSignedIn.connect(openLocationsPage)
            item.StackView.onActivated(disableDrawer)
        }
    }

    function openRegPage(){
        if(regPageLoader.status != Loader.Ready)
            regPageLoader.source = "RegPage.qml"
        pager.replace(pager.currentItem, regPageLoader.item)
    }

    // Restore password

    Loader {
        id: restorePageLoader
        onLoaded: {
            item.onSignIn.connect(openAuthPage)
            item.onRegister.connect(openRegPage)
            item.StackView.onActivated(disableDrawer)
        }
    }

    function openRestorePage(){
        if(restorePageLoader.status != Loader.Ready)
            restorePageLoader.source = "RestorePage.qml"
        pager.replace(pager.currentItem, restorePageLoader.item)
    }

    // Change password

    Loader {
        id: changePasswordPageLoader
        onLoaded: {
            item.onCancel.connect(pager.pop)
            item.onRestore.connect(openRestorePage)
            item.onMenuClicked.connect(pager.pop)
            item.StackView.onActivated(disableDrawer)
        }
    }

    function openChangePasswordPage(){
        if(changePasswordPageLoader.status != Loader.Ready)
            changePasswordPageLoader.source = "ChangePwdPage.qml"
        pager.push(changePasswordPageLoader.item)
    }

    // Locations

    Loader {
        id: locationsPageLoader
        onLoaded: {
            console.log("locations page loaded")
            item.onMenuClicked.connect(drawer.open)
            item.onOpenLocation.connect(function(uuid, tzone, bssid){
                openLocationPage(uuid, tzone)
//                pager.push(locationPage, {"locationUuid": uuid, "tzone": tzone})
            })
            item.onEditLocation.connect(openEditLocationPage)
            item.onAddClicked.connect(openNewLocationPage)
            item.StackView.onActivating.connect(function(){
                drawer.interactive = true;
                item.init();
            })
            item.StackView.onDeactivating.connect(item.pause)
        }
    }

    function openLocationsPage(){
        if(locationsPageLoader.status != Loader.Ready)
            locationsPageLoader.source = "LocationsPage.qml"
        pager.replace(pager.currentItem, locationsPageLoader.item)
    }

    // New location

    Loader {
        id: newLocationPageLoader
        onLoaded: {
            item.onMenuClicked.connect(pager.pop)
            item.StackView.onActivated.connect(disableDrawer)
        }
    }

    function openNewLocationPage(){
        if(newLocationPageLoader.status != Loader.Ready)
            newLocationPageLoader.source = "NewLocationPage.qml"
        pager.push(newLocationPageLoader.item)
    }

    // Edit Location

    Loader {
        id: editLocationPageLoader
        onLoaded: {
            item.onMenuClicked.connect(pager.pop)
            item.StackView.onActivated.connect(disableDrawer)
        }
    }

    function openEditLocationPage(uuid){
        if(editLocationPageLoader.status != Loader.Ready)
            editLocationPageLoader.source = "EditLocationPage.qml"
        editLocationPageLoader.item.locationUuid = uuid
        pager.push(editLocationPageLoader.item)
    }

//    Component {
//        id: editLocationPage
//        Loader {
//            property string locationUuid
//            sourceComponent: EditLocationPage {
//                onMenuClicked: {
//                    pager.pop()
//                    if(pager.currentItem.id === locationsPage)
//                        openLocationsPage()//pager.replace(pager.currentItem, locationsPage);
//                }
//            }
//            onLoaded: {
//                item.locationUuid = locationUuid
//            }
//            StackView.onActivated: {
//                drawer.interactive = false;
//            }
//        }
//    }

    // Location

    Loader {
        id: locationPageLoader
        onLoaded: {
            item.onMenuClicked.connect(pager.pop)
            item.onEditController.connect(openEditControllerPage)
            item.onEditGroup.connect(openEditShadeGroupPage)
            item.onAddGroup.connect(openNewShadeGroupPage)
            item.onSetupController.connect(openEspTouchPage)
            item.onTitleLongPressed.connect(openLocationPage)
            item.StackView.onActivated.connect(function(){
                drawer.interactive = true
                item.init()
            })
            item.StackView.onDeactivating.connect(function(){
                drawer.interactive = false
                item.discovery.isRunning = false
            })
        }
    }

    function openLocationPage(uuid, tzone){
        if(locationPageLoader.status != Loader.Ready)
            locationPageLoader.source = "LocationPage.qml"
        locationPageLoader.item.locationUuid = uuid
        locationPageLoader.item.timezone = tzone
        pager.push(locationPageLoader.item)
        locationsPageLoader.item.init()
    }

//    Component {
//        id: locationPage
//        Loader {
//            property string locationUuid
//            property string tzone
//            sourceComponent: LocationPage {
//                locationUuid: pager.selectedLocationUuid
//                onMenuClicked: pager.pop()
//                onEditController: function(mac){
//                    pager.push(editControllerPage, {"controllerMac": mac , "locationUuid": locationUuid})
//                }
//                onEditGroup:function(mac, channel, controllerIp){
//                    pager.push(editShadesGroupPage, {"controllerMac": mac, "selectedChannel": channel, "controllerIp": controllerIp});
//                }
//                onAddGroup: function(mac){
//                    pager.push(newShadesGroupPage, {"controllerMac": mac});
//                }
//                onSetupController: function(uuid){
//                    pager.push(espTouchPage, {"locationUuid": uuid});
//                }
//                onTitleLongPressed: openLocationPage(locationUuid)
//            }
//            onLoaded: {
//                item.locationUuid = locationUuid
//                item.timezone = tzone
//            }
//            StackView.onActivated: {
//                drawer.interactive = true;
//                item.init();
//            }
//            StackView.onDeactivating: {
//                drawer.interactive = false;
//                item.discovery.isRunning = false;
//            }
//        }
//    }


    // New Shade group

    Loader {
        id: newShadeGroupPageLoader
        onLoaded: {
            item.onMenuClicked.connect(pager.pop)
            item.StackView.onActivated.connect(disableDrawer)
        }
    }

    function openNewShadeGroupPage(mac){
        if(newShadeGroupPageLoader.status != Loader.Ready)
            newShadeGroupPageLoader.source = "NewShadeGroupPage.qml"
        newShadeGroupPageLoader.item.controllerMac = mac
        pager.push(newShadeGroupPageLoader.item)
    }


//    Component {
//        id: newShadesGroupPage
//        Loader {
//            property string controllerMac
//            sourceComponent: NewShadesGroupPage {
//                onMenuClicked: pager.pop()
//            }
//            StackView.onActivated: {
//                drawer.interactive = false;
//            }
//            onLoaded: {
//                item.controllerMac = controllerMac;
//            }
//        }
//    }

    // Edit Shade group

    Loader {
        id: editShadeGroupPageLoader
        onLoaded: {
            item.onMenuClicked.connect(pager.pop)
            item.StackView.onActivated.connect(function(){
                disableDrawer()
                item.init()
            })
        }
    }

    function openEditShadeGroupPage(mac, channel, ip){
        if(editShadeGroupPageLoader.status != Loader.Ready)
            editShadeGroupPageLoader.source = "EditShadeGroupPage.qml"
        editShadeGroupPageLoader.item.controllerMac = mac
        editShadeGroupPageLoader.item.selectedChannel = channel
        editShadeGroupPageLoader.item.controllerIp = ip
        pager.push(editShadeGroupPageLoader.item)
    }


//    Component {
//        id: editShadesGroupPage
//        Loader {
//            property string controllerMac
//            property int selectedChannel
//            property string controllerIp

//            sourceComponent: EditShadesGroupPage {
//                onMenuClicked: pager.pop();
//            }
//            StackView.onActivated: {
//                drawer.interactive = false;
//                item.init();
//            }
//            onLoaded: {
//                item.controllerMac = controllerMac;
//                item.selectedChannel = selectedChannel;
//                item.controllerIp = controllerIp;
//            }
//        }
//    }

    // Edit Controller

    Loader {
        id: editControllerPageLoader
        onLoaded: {
            item.onMenuClicked.connect(pager.pop)
            item.StackView.onActivated.connect(disableDrawer)
        }
    }

    function openEditControllerPage(mac, uuid){
        if(editControllerPageLoader.status != Loader.Ready)
            editControllerPageLoader.source = "EditControllerPage.qml"
        editControllerPageLoader.item.locationUuid = uuid
        editControllerPageLoader.item.controllerMac = mac
        pager.push(editControllerPageLoader.item)
    }

//    Component {
//        id: editControllerPage
//        Loader {
//            property string locationUuid
//            property string controllerMac
//            sourceComponent: EditControllerPage {
//                onMenuClicked: pager.pop()
//            }
//            StackView.onActivated: {
//                drawer.interactive = false;
//            }
//            onLoaded: {
//                item.locationUuid = locationUuid;
//                item.controllerMac = controllerMac;
//            }
//        }
//    }

    // Edit Controller

    Loader {
        id: espTouchPageLoader
        onLoaded: {
            item.onMenuClicked.connect(pager.pop)
            item.StackView.onActivated.connect(disableDrawer)
        }
    }

    function openEspTouchPage(uuid){
        if(espTouchPageLoader.status != Loader.Ready)
            espTouchPageLoader.source = "EspTouchPage.qml"
        pager.push(espTouchPageLoader.item)
    }

//    Component {
//        id: espTouchPage
//        Loader {
//            sourceComponent: EspTouchPage {
//                onEspTouchFinshed: pager.replace(pager.currentItem, locationsPage)
//                StackView.onActivated: {
//                    pageTitle = "Controller setup"
//                }
//                onMenuClicked: {
//                    pager.pop()
//                }
//            }
//            StackView.onActivated: {
//                drawer.interactive = false;
//            }
//        }
//    }

    StackView {
		id: pager
		anchors.fill: parent

        Component.onCompleted: {
            if(currentUser.isAuthenticated)
                openLocationsPage()
            else
                openWelcomePage()
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
                text: currentUser.email
				font.pixelSize: 16
				color: DefTheme.secTextColor
				font.bold: true
			}

            Rectangle {
				width: parent.width
				height: 2
				color: DefTheme.secColorDark
			}

//			Button {
//				width: parent.width
//                text: "Setup Controller"
//				onClicked: {
//					drawer.close()
//					pager.push(espTouchPage);
//				}
//			}

            Button {
                width: parent.width
                text: "Open WebSite"
                onClicked: {
                    Qt.openUrlExternally("https://leviosashades.com") ;
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
                text: "Change password"
                onClicked: {
                    drawer.close()
                    openChangePasswordPage()
                }
            }

            Rectangle {
                width: parent.width
                height: 2
                color: DefTheme.secColorDark
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Sign Out"
                font.bold: true
                onClicked: {
                    currentUser.signOut();
                    drawer.close();
                    drawer.interactive = false;
                    pager.clear();
                    openWelcomePage()
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
        color: DefTheme.mainModalBg
        visible: !netMonitor.connected

        MouseArea {
            anchors.fill: parent
            enabled: parent.visible
        }

        Image {
            id: sadIcon
            fillMode: Image.PreserveAspectFit
            width: parent.width / 4
            source: "img/robo.png"
            anchors.centerIn: parent
        }

        Text {
            text: "No network - no work!"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: sadIcon.bottom
            font.bold: true
            font.pixelSize: parent.width / 20
        }
    }

}
