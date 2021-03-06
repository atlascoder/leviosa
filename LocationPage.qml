import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.LocationModel 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
	id: locationPage

    property alias uuid : locationModel.uuid
    property string timezone

    enableMenuAction: locationModel.single

    signal setupController(string uuid)
    signal openZone(string uuid)
    signal editZone(string uuid)
    signal editLocation(string uuid)

    onAddClicked: addZone(locationModel.uuid)

    title: locationModel.name

    showStatusText: true
    statusText: netMonitor.onWlan && netMonitor.bssid === locationModel.bssid ? "Current location" : "via Internet"
    showSubTitle: true
    subTitle: locationModel.timezone

    function fireRefresh() {
        console.log("Refresh location")
        allShadesControl.reset()
        controllerAPIs.check()
    }

    StackView.onActivated: fireRefresh()

    Connections {
        target: alertBox
        onActiveChanged: {
            if (alertBox.active) {
                allShadesControl.reset()
            }
        }
    }

    Connections {
        target: qGuiApp
        onApplicationStateChanged: {
            console.log("Application state: " + state);
            if(state === 4 /*Qt::ApplicationActive*/){
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

    LocationModel {
        id: locationModel
        onDataChanged: {
            reloadData()
        }
    }

    AllShadesControl {
        id: allShadesControl
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 12
        visible: locationModel.count > 1
        z:1
        onCmdShade: function(shade_state){
            locationModel.commandShade(shade_state);
        }
    }

    GridView {
        id: gridView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: allShadesControl.top
        anchors.margins: 6

        cellWidth: height > width ? (width / 2) : (width / 3)
        cellHeight: height > width ? (height / 3) : (height / 2)

        model: locationModel

        delegate: Item {
            width: gridView.cellWidth
            height: gridView.cellHeight
            ZoneControl {
                anchors.fill: parent
                anchors.margins: 6
                title: model.name
                controllerState: model.controllerState
                onEdit: editZone(uuid)
                onOpen: openZone(uuid)
            }
        }
    }

}
