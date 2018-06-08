import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

import "DefaultTheme.js" as DefTheme

ApplicationWindow {
	id: applicationWindow
    visible: false
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

    Behavior on opacity {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutQuad
        }
    }

    opacity: 0

    Component.onCompleted: {
        doLogin()
        applicationWindow.visible = true
        applicationWindow.opacity = 1
    }

    Connections {
        target: qGuiApp
        onApplicationStateChanged: {
            console.log("Application state: " + state);
            if(state === 4 /*Qt::ApplicationActive*/){
                controllerAPIs.check()
            }
        }
    }

    // EULA

    Loader {
        id: eulaPageLoader
        onLoaded: {
            item.onAgree.connect(doLogin)
            item.onDecline.connect(function(){ Qt.quit() })
            item.onGoBack.connect(pager.pop)
            item.StackView.onRemoved.connect(function(){ source = "" })
            item.hideMenuButton = (pager.depth == 0)
            pager.push(item)
        }
    }

    function openEulaPage(){
        eulaPageLoader.source = "EULA.qml"
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
        if (currentUser.licenseAgreed) {
            if(currentUser.isAuthenticated){
                var n = userData.locationsCount
                if (n > 0) {
                    if (n === 1 && (pager.currentItem == null || pager.currentItem != locationPageLoader.item)) {
                        pager.clear()
                        openLocationsPage()
                        openLocationPage(userData.firstLocationUuid)
                    }
                    else if (n > 1 && (pager.currentItem == null || pager.currentItem != locationsPageLoader.item)) {
                        pager.clear()
                        openLocationsPage()
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
        else {
            openEulaPage()
        }
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

	onClosing: {
        if (Qt.platform.os == "android" && pager.depth > 1 && !pager.currentItem.enableMenuAction && netMonitor.connected) {
			close.accepted = false;
			pager.pop();
		}
	}

    Item {
        id: alertScreen
        anchors.fill: parent
        visible: true
        z: -1

        ParallelAnimation {
            id: showAlertAnim
            NumberAnimation { target: msgBoxBg; property: "opacity"; to: 0.2; duration: 200 }
            NumberAnimation { target: msgBoxScaler; property: "xScale"; to: 1; duration: 200 }
            onStarted: {
                msgBoxBg.opacity = 0
                msgBoxScaler.xScale = 0
                alertScreen.z = 1
            }
        }

        ParallelAnimation {
            id: hideAlertAnim
            NumberAnimation { target: msgBoxBg; property: "opacity"; from: 0.2; to: 0; duration: 100 }
            NumberAnimation { target: msgBoxScaler; property: "xScale"; from: 1; to: 0; duration: 100 }
            onStopped: {
                alertScreen.z = -1
            }
        }

        Connections {
            target: alertBox
            onActiveChanged: {
                showAlertAnim.loops = 1
                hideAlertAnim.loops = 1
                if (alertBox.active) {
                    showAlertAnim.start()
                }
                else {
                    alertScreen.z = -1
                }
            }
        }

        Rectangle {
            id: msgBoxBg
            anchors.fill: parent
            color: "black"
            opacity: 0.25
        }

        Rectangle {
            id: msgBox
            anchors.centerIn: parent
            width: parent.width * 0.8
            height: parent.height / 5
            layer.enabled: true
            radius : 6
            color: "white"
            layer.effect: DropShadow {
                transparentBorder: true
                radius: parent.down ? 1 : 4
            }

            transform: Scale {
                id: msgBoxScaler
                origin.x: msgBox.width / 2
                xScale: 0
            }

            Text {
                id: msgHeading
                width: parent.width
                anchors.top: parent.top
                anchors.margins: 6
                text: alertBox.title
                elide: Text.ElideRight
                font.pixelSize: 18
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }
            Rectangle {
                id: msgHeadingLine
                anchors.top: msgHeading.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width * 0.9
                height: 1
                color: "black"
            }

            Item {
                anchors.top: msgHeadingLine.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 6

                Column {
                    width: parent.width
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 3
                    Text {
                        width: parent.width
                        text: alertBox.message
                        wrapMode: Text.WordWrap
                        font.pixelSize: 14
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                    }
                    Text {
                        width: parent.width
                        text: alertBox.description
                        wrapMode: Text.WordWrap
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: alertBox.hide()
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
                text: "License"
                onClicked: {
                    closeDrawer()
                    openEulaPage()
                }
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
    }
}
