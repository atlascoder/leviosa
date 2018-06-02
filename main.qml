import QtQuick 2.7
import QtQuick.Controls 2.2

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

    Connections {
        target: qGuiApp
        onApplicationStateChanged: {
            console.log("Application state: " + state);
            if(state === 4 /*Qt::ApplicationActive*/){
                controllerAPIs.check()
            }
        }
    }

    // Welcome

    Loader {
        id: welcomePageLoader
        onLoaded: {
            item.onAuthenticate.connect(openAuthPage)
            item.onRegister.connect(openRegPage)
            item.onRemoved.connect(function(){ source = "" })
            pager.replace(pager.initialItem, item)
        }
    }

    function openWelcomePage(){
        welcomePageLoader.source = "WelcomePage.qml"
    }

    // FirstLogin

    Loader {
        id: syncPageLoader
        onLoaded: {
            item.onDownloaded.connect(doLogin)
            item.onRemoved.connect(function(){ source = "" })
            pager.replace(pager.initialItem, item)
        }
    }

    function openSyncPage(){
        syncPageLoader.source = "SyncPage.qml"
    }

    // EmptyAccount

    Loader {
        id: emptyAccountPageLoader
        onLoaded: {
            item.onControllerAdded.connect(doLogin)
            item.onShowMenu.connect(openDrawer)
            item.onRemoved.connect(function(){ source = "" })
            pager.push(item)
        }
    }

    function openEmptyAccountPage(){
        emptyAccountPageLoader.source = "SetupControllerPage.qml"
    }

    // SignIn

    Loader {
        id: authPageLoader
        onLoaded: {
            item.onRegister.connect(openRegPage)
            item.onSignedIn.connect(openSyncPage)
            item.onRestore.connect(openRestorePage)
            item.onRemoved.connect(function(){ source = "" })
            pager.replace(pager.initialItem, item)
        }
    }

    function openAuthPage(){
        authPageLoader.source = "AuthPage.qml"
    }

    // SignUp

    Loader {
        id: regPageLoader
        onLoaded: {
            item.onAuthenticate.connect(openAuthPage)
            item.onRestore.connect(openRestorePage)
            item.onVerificate.connect(openVerifyPage)
            item.onFirstLogin.connect(doLogin)
            item.onRemoved.connect(function(){ source = "" })
            pager.replace(pager.initialItem, item)
        }
    }

    function openRegPage(){
        regPageLoader.source = "RegisterPage.qml"
    }

    // Verify

    Loader {
        id: verifyPageLoader
        onLoaded: {
            item.onRegister.connect(openRegPage)
            item.onSignIn.connect(openAuthPage)
            item.onSignedIn.connect(openSyncPage)
            item.onRemoved.connect(function(){ source = "" })
            pager.clear()
            pager.push(item)
        }
    }

    function openVerifyPage(){
        verifyPageLoader.source = "VerifyPage.qml"
    }

    // Restore password

    Loader {
        id: restorePageLoader
        onLoaded: {
            item.onSignIn.connect(openAuthPage)
            item.onRegister.connect(openRegPage)
            item.onResetPassword.connect(openResetPasswordPage)
            item.onRemoved.connect(function(){ source = "" })
            pager.replace(pager.currentItem, item)
        }
    }

    function openRestorePage(){
        restorePageLoader.source = "RestorePage.qml"
    }

    // Reset password

    Loader {
        id: resetPasswordPageLoader
        onLoaded: {
            item.onSignIn.connect(openAuthPage)
            item.onSignedIn.connect(doLogin)
            item.onSignUp.connect(openRegPage)
            item.onRemoved.connect(function(){ source = "" })
            pager.replace(pager.initialItem, item)
        }
    }

    function openResetPasswordPage(){
        resetPasswordPageLoader.source = "ResetPasswordPage.qml"
    }

    // Change password

    Loader {
        id: changePasswordPageLoader
        onLoaded: {
            item.onCancel.connect(pager.pop)
            item.onRestore.connect(openRestorePage)
            item.onMenuClicked.connect(pager.pop)
            item.onRemoved.connect(function(){ source = "" })
            pager.push(item)
        }
    }

    function openChangePasswordPage(){
        changePasswordPageLoader.source = "ChangePwdPage.qml"
    }

    // doLogin function

    function doLogin() {
        if(currentUser.isAuthenticated){
            var n = userData.locationsCount
            if (n > 0) {
                if (n === 1 && (pager.currentItem == null || pager.currentItem != locationPageLoader.item)) {
                    console.log("Opening single location")
                    pager.clear()
                    openLocationsPage()
                    openLocationPage(userData.firstLocationUuid)
                }
                else if (n > 1 && (pager.currentItem == null || pager.currentItem != locationsPageLoader.item)) {
                    console.log("Opening locations")
                    pager.clear()
                    openLocationsPage()
                    console.log("StackView depth " + pager.depth + ", w/h: " + pager.width + "/" + pager.height)
                }
                else {
                    console.log("Nothing to change - already opened! Current item = " + pager.currentItem)
                }
            }
            else {
                pager.clear()
                openEmptyAccountPage()
            }
        }
        else if(currentUser.requireConfirmation)
            openVerifyPage()
        else
            openWelcomePage()
    }

    // Locations

    Loader {
        id: locationsPageLoader
        onLoaded: {
            item.onOpenLocation.connect(openLocationPage)
            item.onEditLocation.connect(openEditLocationPage)
            item.onAddClicked.connect(openNewLocationPage)
            item.onShowMenu.connect(openDrawer)
            item.onRemoved.connect(function(){ source = ""; })
            pager.push(item)
        }
    }

    function openLocationsPage(){
        locationsPageLoader.source = "LocationsPage.qml"
    }

    // New location

    Loader {
        id: newLocationPageLoader
        onLoaded: {
            item.onGoBack(pager.pop)
            item.onLocationChanged.connect(pager.pop)
            item.onLocationDeleted.connect(pager.pop)
            item.onLocationCreated.connect(openLocationPage)
            item.onRemoved.connect(function(){ source = "" })
        }
    }

    function openNewLocationPage(bssid){
        newLocationPageLoader.source = "EditLocationPage.qml"
        newLocationPageLoader.item.bssid = bssid
        pager.push(newLocationPageLoader.item)
    }

    // Edit Location

    Loader {
        id: editLocationPageLoader
        onLoaded: {
            item.onGoBack.connect(pager.pop)
            item.onLocationChanged.connect(pager.pop)
            item.onLocationDeleted.connect(doLogin)
            item.onLocationCreated.connect(openLocationPage)
            item.onRemoved.connect(function(){ source = "" })
        }
    }

    function openEditLocationPage(uuid){
        editLocationPageLoader.source = "EditLocationPage.qml"
        editLocationPageLoader.item.uuid = uuid
        pager.push(editLocationPageLoader.item)
    }

    // Location

    Loader {
        id: locationPageLoader
        onLoaded: {
            item.onGoBack.connect(pager.pop)
            item.onShowMenu.connect(openDrawer)
            item.onOpenZone.connect(openZonePage)
            item.onEditZone.connect(openEditZonePage)
            item.onEditLocation.connect(openEditLocationPage)
            item.onRemoved.connect(function(){ source = "" })
        }
    }

    function openLocationPage(uuid){
        var n = userData.controllersCountForLocation(uuid);
        if (n === 1) {
            zonePageLoader.source = "ZonePage.qml"
            zonePageLoader.item.uuid = userData.firstControllerInLocation(uuid)
            if (zonePageLoader.item.single) {
                pager.replace(pager.initialItem, zonePageLoader.item)
            }
            else {
                pager.push(zonePageLoader.item)
            }
        }
        else {
            locationPageLoader.source = "LocationPage.qml"
            locationPageLoader.item.uuid = uuid
            pager.push(locationPageLoader.item)
        }
    }

    // Zone (controller) page

    Loader {
        id: zonePageLoader
        onLoaded: {
            item.onGoBack.connect(pager.pop)
            item.onShowMenu.connect(openDrawer)
            item.onAddLocation.connect(openNewLocationPage)
            item.onEditLocation.connect(openEditLocationPage)
            item.onEditGroup.connect(openEditShadeGroupPage)
            item.onAddGroup.connect(openNewShadeGroupPage)
            item.onRemoved.connect(function(){ source = "" })
        }
    }

    function openZonePage(uuid) {
        zonePageLoader.source = "ZonePage.qml"
        zonePageLoader.item.uuid = uuid
        if (zonePageLoader.item.single) {
            pager.replace(pager.initialItem, zonePageLoader.item)
        }
        else {
            pager.push(zonePageLoader.item)
        }
    }

    // New Shade group

    Loader {
        id: newShadeGroupPageLoader
        onLoaded: {
            item.onGoBack.connect(pager.pop)
            item.onRemoved.connect(function(){ source = "" })
        }
    }

    function openNewShadeGroupPage(mac){
        newShadeGroupPageLoader.source = "ShadeGroupPage.qml"
        newShadeGroupPageLoader.item.controllerMac = mac
        pager.push(newShadeGroupPageLoader.item)
    }

    // Edit Shade group

    Loader {
        id: editShadeGroupPageLoader
        onLoaded: {
            item.onGoBack.connect(pager.pop)
            item.onRemoved.connect(function(){ source = "" })
        }
    }

    function openEditShadeGroupPage(uuid){
        editShadeGroupPageLoader.source = "ShadeGroupPage.qml"
        editShadeGroupPageLoader.item.uuid = uuid
        pager.push(editShadeGroupPageLoader.item)
    }

    // Edit Controller

    Loader {
        id: editZonePageLoader
        onLoaded: {
            item.onGoBack.connect(pager.pop)
            item.onZoneDeleted.connect(doLogin)
            item.onRemoved.connect(function(){ source = "" })
        }
    }

    function openEditZonePage(uuid){
        editZonePageLoader.source = "EditZonePage.qml"
        editZonePageLoader.item.uuid = uuid
        pager.push(editZonePageLoader.item)
    }

    // Esptouch

    Loader {
        id: setupControllerPageLoader
        onLoaded: {
            item.onGoBack.connect(pager.pop)
            item.onControllerAdded.connect(doLogin)
            item.enableMenuAction = false
            item.onRemoved.connect(function(){ source = "" })
            pager.push(item)
        }
    }

    function openSetupControllerPage(){
        setupControllerPageLoader.source = "SetupControllerPage.qml"
    }

    StackView {
		id: pager
		anchors.fill: parent
	}

    Component.onCompleted: doLogin()

	onClosing: {
        if (Qt.platform.os == "android" && pager.depth > 1 && !pager.currentItem.enableMenuAction) {
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
            source: "img/sad.png"
            anchors.centerIn: parent
        }

        Column {
            anchors.top: sadIcon.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: width / 12
            anchors.rightMargin: width / 12
            spacing: width / 12
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Qt.AlignHCenter
                text: "Network unavailable"
                font.bold: true
                font.pixelSize: width / 12
            }
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Qt.AlignHCenter
                text: "The application is working only with network connection."
                font.bold: true
                font.pixelSize: width / 20
                wrapMode: Text.Wrap
            }
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Qt.AlignHCenter
                text: "This screen will disappear once some connection established."
                font.italic: true
                font.pixelSize: width / 20
                wrapMode: Text.Wrap
            }
        }

    }

    function openDrawer() {
        if (!drawer.interactive) drawer.interactive = true
        drawer.open()
    }

    function closeDrawer() {
        drawer.close()
    }

    function enableDrawer() {
        drawer.interactive = true
    }

    function disableDrawer() {
        drawer.close()
        drawer.interactive = false
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
                visible: netMonitor.onWlan
            }

            Button {
                width: parent.width
                text: "Edit Location"
                onClicked: {
                    closeDrawer()
                    if (pager.currentItem != null) {
                        if (pager.currentItem == zonePageLoader.item)
                            openEditLocationPage(pager.currentItem.locationUuid)
                        else
                            openEditLocationPage(pager.currentItem.uuid)
                    }
                }
                visible: pager.currentItem != null
                         && (pager.currentItem == zonePageLoader.item
                             || pager.currentItem == locationPageLoader.item)
            }

            Button {
                width: parent.width
                text: "Setup New Zone"
                onClicked: {
                    closeDrawer()
                    openSetupControllerPage()
                }
                visible: netMonitor.onWlan
            }

            Rectangle {
                width: parent.width
                height: 2
                color: DefTheme.secColorDark
            }

            Button {
                width: parent.width
                text: "Open WebSite"
                onClicked: {
                    closeDrawer()
                    Qt.openUrlExternally("https://leviosashades.com") ;
                }
            }

            Button {
                width: parent.width
                text: "Call to Support"
                onClicked: {
                    closeDrawer()
                    Qt.openUrlExternally("tel:%1".arg("+19802061260")) ;
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
                    disableDrawer()
                    openWelcomePage()
                }
            }
        }

        Text{
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.rightMargin: 6
            text: "version: " + userData.version
            font.pixelSize: 8
            font.italic: true
            color: "#a0000000"
        }
    }
}
