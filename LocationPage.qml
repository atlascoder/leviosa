import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.LocationsModel 1.0
import com.atlascoder.ControllersModel 1.0
import com.atlascoder.ShadesGroupsModel 1.0
import com.atlascoder.ControllerDiscovery 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
	id: locationPage

    property alias locationUuid : controllersModel.locationUuid
    property alias activeController : controllersModel.selectedControllerMac

    property alias discovery : controllerDiscovery

    property bool single : true
    property bool refresh : true
    property string timezone

    enableMenuAction: single

    property bool selectedShadeGroupsNotFilled : false

    property bool isActive : false

    signal setupController(string uuid)
    signal editController(string mac)
    signal editGroup(string mac, int channel, string ip)
    signal addGroup(string mac)
    signal editLocation(string uuid)

//    signal controllersLoaded()

    signal goBack()

    onAddClicked: addGroup(controllersModel.macByIndex(controllersPager.currentIndex))

    title: controllersModel.locationName

    showStatusText: true
    statusText: controllersModel.locationStatusText

    showSubTitle: true
    subTitle: controllersModel.selecetedControllerStatus

    onTitleLongPressed: editLocation(locationUuid)

    function fireRefresh(){
        if(controllersModel.dataLoaded && controllersModel.isCurrentLocation && !controllerDiscovery.isRunning){
            controllerDiscovery.discoFinished = false
            controllerDiscovery.isRunning = true
        }
    }

    function init(){
        console.log("INIT LOCATION PAGE (n = " + controllersModel.rowCount() + ")");
        if(controllersModel.locationUuid != locationPage.locationUuid){
            console.log("Init controllers from empty/wrong UUID with " + locationUuid)
            controllersModel.locationUuid = locationPage.locationUuid
        }
        else{
            console.log("Updating controllers for UUID " + locationUuid)
            controllersModel.updateModel()
            fireRefresh()
        }
    }

    Connections {
        target: qGuiApp
        onApplicationStateChanged: {
            console.log("Application state: " + state);
            if(state !== 4 /*Qt::ApplicationActive*/){
                fireRefresh()
            }
        }
    }

    Connections {
        target: netMonitor
        onOnWlanChanged: {
            if(netMonitor.onWlan) fireRefresh()
        }
    }

    function pause() {
        console.log("Pause")
        controllerDiscovery.isRunning = false
    }

    ControllersModel {
        id: controllersModel
        isOnWlan: netMonitor.onWlan
        currentBssid: netMonitor.bssid
        discovering: controllerDiscovery.isRunning

        onDataLoadedChanged: fireRefresh()
        onSelectedControllerStatusChanged: {
            enableAddAction = selectedShadeGroupsNotFilled && (controllersModel.selectedControllerState === 0 || controllersModel.selectedControllerState === 5)
        }
    }

    SwipeView {
        id: controllersPager
        anchors.fill: parent
        visible: count > 0
        Repeater {
            model: controllersModel
            Loader {
                active: SwipeView.isCurrentItem
                sourceComponent: ControllerPage {
                    anchors.fill: parent
                    controllerMac: mac
                    onOpenGroupEdit: function(mac, c){
                        var ip = controllersModel.ipByMac(mac);
                        console.log("Open group with ip: " + ip);
                        editGroup(mac, c, ip);
                    }
                    Component.onCompleted: {
                        init();
                    }
                    onItemsLoaded: function(n) { selectedShadeGroupsNotFilled = n < 6; }
                    onCommandShade: function(channel, command){
                        controllersModel.commandShade(mac, channel, command);
                    }
                    failed: (controllerState === 2) || (controllerState === 4)
                    onDoSetup: setupController(locationUuid)
                    onDoRefresh: {
                        controllersModel.searchController(controllerMac)
                        fireRefresh()
                    }
                    onDeleteController: {
                        controllersModel.remove(controllerMac)
                        init()
                        controllersModel.searchController(controllerMac)
                        fireRefresh()
                    }
                }
            }
        }
    }

    footer: TabBar {
		id: bar
        visible: controllersPager.count > 1
        property int itemWidth: width / (controllersPager.count >= 3 ? 3 : 2)
        currentIndex: controllersPager.currentIndex
        Repeater {
            id: tabsRepeater
            model: controllersModel
            TabButton {
                width: bar.itemWidth
                text: name
                onPressAndHold: editController(mac)
            }
        }
        Component.onCompleted: controllersPager.currentIndex = currentIndex
        onCurrentIndexChanged: {
            console.log("Current index: " + controllersPager.currentIndex)
            controllersPager.currentIndex = currentIndex;
            controllersModel.setSelectedControllerIndex(currentIndex);
        }
    }

    BusyIndicator {
        id: busyIndi
        anchors.centerIn: parent
        running: visible
    }

    Rectangle {
        id: setupInquiry
        anchors.centerIn: parent
        width: parent.width * 0.8
        visible: false
        radius: 10
        color: DefTheme.mainColorLight

        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            radius: 0
        }

        Text {
            id: setupIntro
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 10
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            wrapMode: Text.WordWrap
            font.pixelSize: parent.width / 16
            text: "No controllers were found in the location."
            font.italic: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            color: DefTheme.mainTextColor
        }

        Rectangle {
            id: setupButton
            anchors.top: setupIntro.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            height: DefTheme.toolbarHeight
            width: DefTheme.toolbarHeight * 4
            radius: 6
            color: DefTheme.mainColorDark
            border.color: "#00000000"
            layer.enabled: true
            layer.effect: DropShadow {
                transparentBorder: true
                radius: setupMouse.pressed ? 1 : 4
            }
            Text {
                height: parent.height
                anchors.horizontalCenter: parent.horizontalCenter
                verticalAlignment: Text.AlignVCenter
                text: "SETUP"
                font.bold: true
                font.pixelSize: height / 2
                color: DefTheme.mainTextColor
            }
            MouseArea {
                id: setupMouse
                anchors.fill: parent
                onClicked: setupController(locationUuid)
            }
        }

        height: childrenRect.height + 30
    }

    ControllerDiscovery {
        id: controllerDiscovery
        property bool discoFinished : false
        onStarted: {
            console.log("disco started");
            discoFinished = false;
        }
        onFinished: {
            discoFinished = true;
        }
//        onFoundList: function(list){
//            console.log("FOUND LIST: " + list);
//            controllersModel.addControllersFromList(controllersModel.locationUuid, list);
//        }
        onFound: function(mac, ip) {
            console.log("FOUND CONTROLLER: " + mac + " / " + ip);
            controllersModel.checkIP(mac, ip);
        }
    }

    BlockScreen {
        id: emptyOnWanAlert
        visible: false
        anchors.fill: parent
        message: "Please, connect to your WiFi network in order to begin Setup!"
    }

    onMenuClicked: {
        if(single)
            drawer.open()
        else
            locationPage.goBack()
    }

    Drawer {
        id: drawer
        width:  0.66 * applicationWindow.width
        height: applicationWindow.height
        edge: Qt.LeftEdge
        interactive: single

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

            Button {
                width: parent.width
                text: "Add current location"
                visible: netMonitor.onWlan && controllersModel.locationBssid != netMonitor.bssid
                onClicked: {
                    drawer.close()
                    openNewLocationPage()
                }
            }

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
                    openWelcomePage()
                }
            }
        }
        Text {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            text: "ver 1"
        }
    }

    states : [
        State{
            name: "init"
            when: controllersModel.allowedToSearch && !controllerDiscovery.discoFinished && !controllerDiscovery.isRunning
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: false }
            PropertyChanges { target: emptyOnWanAlert; visible: false }
        },
        State {
            name: "discovery first"
            when: controllersModel.allowedToSearch && controllerDiscovery.isRunning && (controllersModel.count == 0)
            PropertyChanges { target: busyIndi; visible: true }
            PropertyChanges { target: emptyOnWanAlert; visible: false }
        },
        State {
            name: "discovery next"
            when: controllersModel.allowedToSearch && controllerDiscovery.isRunning && (controllersModel.count > 0)
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: false }
            PropertyChanges { target: emptyOnWanAlert; visible: false }
        },
        State {
            name: "empty Wlan"
            when: controllersModel.isCurrentLocation && controllerDiscovery.discoFinished && (controllersModel.count == 0)
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: true }
            PropertyChanges { target: emptyOnWanAlert; visible: false }
        },
        State {
            name: "empty Wan"
            when:  (controllersModel.count == 0) && !controllersModel.isCurrentLocation
            PropertyChanges { target: emptyOnWanAlert; visible: true }
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: false }
        },
        State {
            name: "ready"
            when: (!controllersModel.allowedToSearch || controllerDiscovery.discoFinished) && (controllersModel.count > 0)
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: false }
            PropertyChanges { target: emptyOnWanAlert; visible: false }
        }
    ]

}
