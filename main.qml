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

    // Welcome

    Loader {
        id: welcomePageLoader
        onLoaded: {
            item.onAuthenticate.connect(openAuthPage)
            item.onRegister.connect(openRegPage)
        }
    }

    function openWelcomePage(){
        if(welcomePageLoader.status != Loader.Ready)
            welcomePageLoader.source = "WelcomePage.qml"
        pager.replace(pager.initialItem, welcomePageLoader.item)
    }

    // FirstLogin

    Loader {
        id: syncPageLoader
        onLoaded: {
            item.onDownloaded.connect(function(){
                doLogin();
            })
            item.StackView.onRemoved.connect(function(){
                syncPageLoader.source = ""
            })
        }
    }

    function openSyncPage(){
        syncPageLoader.source = "SyncPage.qml"
        pager.replace(pager.initialItem, syncPageLoader.item)
    }

    // EmptyAccount

    Loader {
        id: emptyAccountPageLoader
        onLoaded: {
            item.onControllerAdded.connect(doLogin)
            item.StackView.onRemoved.connect(function(){ emptyAccountPageLoader.source = ""; })
        }
    }

    function openEmptyAccountPage(){
        emptyAccountPageLoader.source = "EmptyAccountPage.qml"
        pager.replace(pager.initialItem, emptyAccountPageLoader.item)
    }

    // SignIn

    Loader {
        id: authPageLoader
        onLoaded: {
            item.onRegister.connect(openRegPage)
            item.onSignedIn.connect(openSyncPage)
            item.onRestore.connect(openRestorePage)
        }
    }

    function openAuthPage(){
        if(authPageLoader.status == Loader.Ready)
            authPageLoader.source = ""
        authPageLoader.source = "AuthPage.qml"
        pager.replace(pager.currentItem, authPageLoader.item)
    }

    // SignUp

    Loader {
        id: regPageLoader
        onLoaded: {
            item.onAuthenticate.connect(openAuthPage)
            item.onRestore.connect(openRestorePage)
            item.onVerificate.connect(openVerifyPage)
            item.onFirstLogin.connect(doLogin)
        }
    }

    function openRegPage(){
        if(regPageLoader.status == Loader.Ready)
            regPageLoader.source = ""
        regPageLoader.source = "RegisterPage.qml"
        pager.replace(pager.currentItem, regPageLoader.item)
    }

    // Verify

    Loader {
        id: verifyPageLoader
        onLoaded: {
            item.onRegister.connect(openRegPage)
            item.onSignIn.connect(openAuthPage)
            item.onSignedIn.connect(openSyncPage)
            item.StackView.onRemoved.connect(function(){
                verifyPageLoader.source = ""
            })
        }
    }

    function openVerifyPage(){
        verifyPageLoader.source = "VerifyPage.qml"
        pager.replace(pager.currentItem, verifyPageLoader.item)
    }

    // Restore password

    Loader {
        id: restorePageLoader
        onLoaded: {
            item.onSignIn.connect(openAuthPage)
            item.onRegister.connect(openRegPage)
            item.onResetPassword.connect(openResetPasswordPage)
        }
    }

    function openRestorePage(){
        if(restorePageLoader.status != Loader.Ready)
            restorePageLoader.source = "RestorePage.qml"
        pager.replace(pager.currentItem, restorePageLoader.item)
    }

    // Reset password

    Loader {
        id: resetPasswordPageLoader
        onLoaded: {
            item.onSignIn.connect(openAuthPage)
            item.onSignedIn.connect(doLogin)
            item.onSignUp.connect(openRegPage)
        }
    }

    function openResetPasswordPage(){
        if(resetPasswordPageLoader.status != Loader.Ready)
            resetPasswordPageLoader.source = "ResetPasswordPage.qml"
        pager.replace(pager.currentItem, resetPasswordPageLoader.item)
    }

    // Change password

    Loader {
        id: changePasswordPageLoader
        onLoaded: {
            item.onCancel.connect(pager.pop)
            item.onRestore.connect(openRestorePage)
            item.onMenuClicked.connect(pager.pop)
        }
    }

    function openChangePasswordPage(){
        if(changePasswordPageLoader.status != Loader.Ready)
            changePasswordPageLoader.source = "ChangePwdPage.qml"
        pager.push(changePasswordPageLoader.item)
    }

    // doLogin function

    function doLogin() {
        if(currentUser.isAuthenticated){
            if(userData.locationsCount > 1)
                openLocationsPage()
            else if(userData.locationsCount === 1)
                openLocationPage(userData.firstLocationUuid, true)
            else
                openEmptyAccountPage()
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
            console.log("locations page loaded")
            item.onOpenLocation.connect(function(uuid, tzone, bssid){
                openLocationPage(uuid, false)
//                pager.push(locationPage, {"locationUuid": uuid, "tzone": tzone})
            })
            item.onEditLocation.connect(openEditLocationPage)
            item.onAddClicked.connect(openNewLocationPage)
            item.StackView.onActivating.connect(item.init)
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
        }
    }

    function openEditLocationPage(uuid){
        if(editLocationPageLoader.status != Loader.Ready)
            editLocationPageLoader.source = "EditLocationPage.qml"
        editLocationPageLoader.item.locationUuid = uuid
        pager.push(editLocationPageLoader.item)
    }

    // open Controls

    Loader {
        id: controlsPageLoader
        onLoaded: {
            item.StackView.onRemoved.connect(function(){
                verifyPageLoader.source = ""
            })
        }
    }

    function openControlsPage(){
        if(controlsPageLoader.status != Loader.Ready)
            controlsPageLoader.source = "ControlsPage.qml"
        pager.replace(pager.currentItem, controlsPageLoader.item)
    }

    // Location

    Loader {
        id: locationPageLoader
        onLoaded: {
            item.onMenuClicked.connect(pager.pop)
            item.onEditController.connect(openEditControllerPage)
            item.onEditGroup.connect(openEditShadeGroupPage)
            item.onAddGroup.connect(openNewShadeGroupPage)
            item.onSetupController.connect(openEspTouchPage)
            item.StackView.onActivated.connect(item.init)
            item.StackView.onDeactivating.connect(function(){
                item.discovery.isRunning = false
            })
            item.StackView.onRemoved.connect(function(){
                locationPageLoader.source = ""
            })
        }
    }

    function openLocationPage(uuid, single){
        locationPageLoader.source = "LocationPage.qml"
        locationPageLoader.item.locationUuid = uuid
        locationPageLoader.item.single = single
        pager.replace(pager.initialItem, locationPageLoader.item)
    }

    // New Shade group

    Loader {
        id: newShadeGroupPageLoader
        onLoaded: {
            item.onMenuClicked.connect(pager.pop)
        }
    }

    function openNewShadeGroupPage(mac){
        if(newShadeGroupPageLoader.status != Loader.Ready)
            newShadeGroupPageLoader.source = "NewShadeGroupPage.qml"
        newShadeGroupPageLoader.item.controllerMac = mac
        pager.push(newShadeGroupPageLoader.item)
    }

    // Edit Shade group

    Loader {
        id: editShadeGroupPageLoader
        onLoaded: {
            item.onMenuClicked.connect(pager.pop)
            item.StackView.onActivated.connect(item.init)
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

    // Edit Controller

    Loader {
        id: editControllerPageLoader
        onLoaded: {
            item.onMenuClicked.connect(pager.pop)
        }
    }

    function openEditControllerPage(mac, uuid){
        if(editControllerPageLoader.status != Loader.Ready)
            editControllerPageLoader.source = "EditControllerPage.qml"
        editControllerPageLoader.item.locationUuid = uuid
        editControllerPageLoader.item.controllerMac = mac
        pager.push(editControllerPageLoader.item)
    }

    // Edit Controller

    Loader {
        id: espTouchPageLoader
        onLoaded: {
            item.onMenuClicked.connect(pager.pop)
        }
    }

    function openEspTouchPage(uuid){
        if(espTouchPageLoader.status != Loader.Ready)
            espTouchPageLoader.source = "EspTouchPage.qml"
        pager.push(espTouchPageLoader.item)
    }

    StackView {
		id: pager
		anchors.fill: parent
	}

    Component.onCompleted: doLogin()

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

}
