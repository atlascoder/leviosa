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
    anchors.fill: parent
    enableMenuAction: false

    property string locationUuid
    property alias discovery : controllerDiscovery
    property alias activeController : controllersModel.selectedControllerMac
    property string timezone

    property bool isOnWlan : netMonitor.onWlan ? true : false
    property bool allowedWlan : controllersModel.isOnWlan && (controllersModel.isCurrentLocation || controllersModel.isNewLocation)

    property bool selectedShadeGroupsNotFilled : false

    signal setupController(string uuid)
    signal editController(string mac)
    signal editGroup(string mac, int channel, string ip)
    signal addGroup(string mac)

    enableAddAction: selectedShadeGroupsNotFilled && (controllersModel.selectedControllerState == 0 || controllersModel.selectedControllerState == 5)
    onAddClicked: addGroup(controllersModel.macByIndex(controllersPager.currentIndex))


    title: controllersModel.locationName

    showStatusText: true
    statusText: isOnWlan ?
                    (controllersModel.isNewLocation ?
                         "new location" :
                        (controllersModel.isCurrentLocation ?
                            "current location" :
                            "via Internet")
                     )
                    : "via Internet"

    showSubTitle: true
    subTitle: controllersModel.selecetedControllerStatus

    function init(){
        console.log("INIT LOCATION PAGE");
        if(controllersModel.locationUuid != locationPage.locationUuid){
            console.log("init from emty UUID to: " + locationUuid)
            controllersModel.locationUuid = locationPage.locationUuid;
        }
        else{
            console.log("updating loaded location UUID to: " + locationUuid)
            controllersModel.updateModel();
            if(isOnWlan && controllersModel.isNewLocation)
                controllerDiscovery.isRunning = true;
        }
    }

    ControllersModel {
        id: controllersModel
        onIsOnWlanChanged: {
            if(allowedWlan && dataLoaded){
                console.log("On Wlan");
                controllerDiscovery.isRunning = true;
            }
            else{
                console.log("On Wan");
                controllerDiscovery.isRunning = false;
            }
        }
        isDiscovering: controllerDiscovery.isRunning
        isOnWlan: netMonitor.onWlan
        bssid: netMonitor.bssid
        onDataLoadedChanged: {
            console.log("Data loaded");
            if(dataLoaded && allowedWlan)
                controllerDiscovery.isRunning = true;
        }
    }

    SwipeView {
        id: controllersPager
        anchors.fill: parent
        visible: count > 0
        Repeater {
            model: controllersModel
            Loader {
                active: SwipeView.isCurrentItem// || SwipeView.isNextItem || SwipeView.isPreviousItem
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
        onFoundList: function(list){
            console.log("FOUND LIST: " + list);
            controllersModel.addControllersFromList(controllersModel.locationUuid, list);
        }
        onFound: function(mac, ip) {
            console.log("FOUND WINGARDIUM: " + mac + " / " + ip);
            controllersModel.checkIP(mac, ip);
        }
    }

    BlockScreen {
        id: emptyOnWanAlert
        anchors.fill: parent
        message: "You need to setup your first Leviosa WiShadeController being connected to WiFi network."
    }

    states : [
        State{
            name: "init"
            when: allowedWlan && !controllerDiscovery.discoFinished && !controllerDiscovery.isRunning
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: false }
            PropertyChanges { target: emptyOnWanAlert; visible: false }
        },
        State {
            name: "discovery first"
            when: allowedWlan && controllerDiscovery.isRunning && (controllersModel.count == 0)
            PropertyChanges { target: busyIndi; visible: true }
            PropertyChanges { target: emptyOnWanAlert; visible: false }
        },
        State {
            name: "discovery next"
            when: allowedWlan && controllerDiscovery.isRunning && (controllersModel.count > 0)
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: false }
            PropertyChanges { target: emptyOnWanAlert; visible: false }
        },
        State {
            name: "empty Wlan"
            when: allowedWlan && controllerDiscovery.discoFinished && (controllersModel.count == 0)
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: true }
            PropertyChanges { target: emptyOnWanAlert; visible: false }
        },
        State {
            name: "empty Wan"
            when:  (controllersModel.count == 0) && !allowedWlan
            PropertyChanges { target: emptyOnWanAlert; visible: true }
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: false }
        },
        State {
            name: "ready"
            when: (!allowedWlan || controllerDiscovery.discoFinished) && (controllersModel.count > 0)
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: false }
            PropertyChanges { target: emptyOnWanAlert; visible: false }
        }
    ]

}
