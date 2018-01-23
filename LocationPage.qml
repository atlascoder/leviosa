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

    property alias locationUuid : locationModel.selectedLocationUuid
    property int activeController : -1

    signal setupController(int locationId)
    signal editController(int id)
    signal editGroup(int cid, int gid)
    signal editSchedule(int cid, int gid)
    signal addGroup(int controllerId)

    onAddClicked: addGroup(controllersModel.data(controllersModel.indexOfRow(controllersPager.currentIndex), controllersModel.roleByName("id")))


    LocationsModel {
        id: locationModel
        selectedLocationUuid: locationUuid
    }

    title: locationModel.selectedLocationName

//    StackView.onVisibleChanged: {
//        if(StackView.visible){
//            locationPage.title = controllersModel.locationName;
//            controllersModel.setLocation(locationPage.locationId);
//            controllersPager.currentIndex = locationPage.activeController;
//            controllerDiscovery.isRunning = false;
//            controllerDiscovery.isRunning = true;
//        }
//    }

    ControllersModel {
        id: controllersModel
        isOnWlan: netMonitor.onWlan && netMonitor.bssid == selectedControllerMac
    }

    SwipeView {
        id: controllersPager
        anchors.fill: parent
        visible: count > 0
        Repeater {
            model: controllersModel
            Loader {
                active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
                sourceComponent: ControllerPage {
                    anchors.fill: parent
                    controllerMac: mac
                    onOpenGroupEdit: function(cid, gid){ editGroup(cid, gid); }
                    onCmd: function(channel, cmd){ controllersModel.shadeCmd(model.id, channel, cmd); }
                }
            }
        }
        onCurrentIndexChanged: {
            locationPage.activeController = controllersPager.currentIndex;
            bar.currentIndex = controllersPager.currentIndex;
        }
    }

    footer: TabBar {
		id: bar
		visible: controllersPager.count > 1
        property int itemWidth: width / (controllersPager.count >= 3 ? 3 : 2)
        Repeater {
            id: tabsRepeater
            model: controllersModel
            TabButton {
                width: bar.itemWidth
                text: name
                onPressAndHold: editController(id)
            }
        }
        onVisibleChanged: {
            if(bar.visible && bar.currentIndex > 0){
                var i = bar.currentIndex;
                bar.setCurrentIndex(-1);
                bar.setCurrentIndex(i);
            }
        }
        onCurrentIndexChanged: {
            if(controllersPager.currentIndex != bar.currentIndex)
                controllersPager.currentIndex = bar.currentIndex;
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
                onClicked: setupController(locationId)
            }
        }

        height: childrenRect.height + 30
    }

    ControllerDiscovery {
        id: controllerDiscovery
        onStarted: {
            console.log("disco started");
        }
        onFound: function(mac, ip){
            controllersModel.addControllerWithMacAndIP(mac, ip);
            locationPage.state = controllersModel.rowCount() > 0 ? "discovery next" : "discovery first";
        }
        onFinished: {
            locationPage.state = controllersModel.rowCount() > 0 ? "ready" : "empty";
        }
    }

    Timer {
        id: discoFirstTimeout
        interval: 5000
        onTriggered: locationPage.state = "empty"
    }

    states : [
        State {
            name: "init"
            PropertyChanges { target: controllerDiscovery; isRunning: true }
        },
        State {
            name: "discovery first"
            PropertyChanges { target: busyIndi; visible: true }
            PropertyChanges { target: controllerDiscovery; isRunning: true}
        },
        State {
            name: "discovery next"
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: false }
            PropertyChanges { target: locationPage; enableAddAction: true; showLogo: true }
            StateChangeScript {
                script: {
                    if(!controllerDiscovery.isRunning)
                        controllerDiscovery.isRunning = true;
                }
            }
        },
        State {
            name: "empty"
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: true }
        },
        State {
            name: "ready"
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: false }
            PropertyChanges { target: locationPage; enableAddAction: true }
        }

    ]

    state : controllersModel.rowCount() > 0 ? "discovery next" : "discovery first"

}
