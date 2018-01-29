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

    property alias locationUuid : controllersModel.locationUuid
    property alias discovery : controllerDiscovery
    property alias activeController : controllersModel.selectedControllerMac

    signal setupController(string uuid)
    signal editController(string mac)
    signal editGroup(string mac, int channel)
    signal editSchedule(string mac, int channel)
    signal addGroup(string mac)

    onAddClicked: addGroup(controllersModel.macByIndex(controllersPager.currentIndex))


    title: controllersModel.locationName

    function init(){
        controllersModel.updateModel();
        discovery.isRunning = true;
    }

    ControllersModel {
        id: controllersModel
        isOnWlan: netMonitor.onWlan && netMonitor.bssid == selectedControllerMac
        onModelReset: {
            console.log("Model reset, index: " + controllersPager.currentIndex);
        }
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
                    onOpenGroupEdit: function(mac, c){ editGroup(mac, c); }
                    onCmd: function(channel, cmd){ controllersModel.shadeCmd(model.id, channel, cmd); }
                    onVisibleChanged: {
                        if(visible) init();
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
            model: controllersPager.count
            TabButton {
                width: bar.itemWidth
                text: controllersModel.nameByIndex(model.index)
                onPressAndHold: editController(controllersModel.macByIndex(model.index))
            }
        }
        Component.onCompleted: controllersPager.currentIndex = currentIndex
        onCurrentIndexChanged: {
            console.log("Current index: " + controllersPager.currentIndex)
            controllersPager.currentIndex = currentIndex;
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
        property bool discoFinished : false
        onStarted: {
            console.log("disco started");
            discoFinished = false;
        }
        onFinished: {
            discoFinished = true;
        }
        onFoundList: function(list){
            console.log("FOUND: " + controllerDiscovery.foundList);
            controllersModel.addControllersFromList(controllersModel.locationUuid, list);
        }
    }

    states : [
        State{
            name: "init"
            when: !controllerDiscovery.discoFinished && !controllerDiscovery.isRunning
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: false }
        },
        State {
            name: "discovery first"
            when: controllerDiscovery.isRunning && controllersModel.rowCount() === 0
            PropertyChanges { target: busyIndi; visible: true }
        },
        State {
            name: "discovery next"
            when: controllerDiscovery.isRunning && controllersModel.rowCount() > 0
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: false }
            PropertyChanges { target: locationPage; enableAddAction: true; showLogo: true }
        },
        State {
            name: "empty"
            when: !controllerDiscovery.isRunning && controllersModel.rowCount() === 0
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: true }
        },
        State {
            name: "ready"
            when: !controllerDiscovery.isRunning && controllersModel.rowCount() > 0
            PropertyChanges { target: busyIndi; visible: false }
            PropertyChanges { target: setupInquiry; visible: false }
            PropertyChanges { target: locationPage; enableAddAction: true }
        }
    ]

}
