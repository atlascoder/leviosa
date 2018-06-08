import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import com.atlascoder.ZoneModel 1.0
import com.atlascoder.Shade 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
    id: rootItem

    property alias uuid : zoneModel.uuid
    property alias single: zoneModel.single
    property alias locationUuid: zoneModel.locationUuid

    enableMenuAction: zoneModel.single
    enableAddAction: zoneModel.count < 6

    property bool failed : false

    signal addLocation(string uuid)
    signal editLocation(string uuid)
    signal setupController(string uuid)
    signal editGroup(string uuid)
    signal addGroup(string mac)

    onAddClicked: addGroup(zoneModel.controllerMac)

    title: zoneModel.title
    statusText: zoneModel.controllerStatus
    showStatusText: true

    showSubTitle: true
    subTitle: zoneModel.timezone

    function reload() {
        zoneModel.reloadShadeGroups()
        controllerAPIs.check()
        userData.sync()
    }

    Connections {
        target: alertBox
        onActiveChanged: {
            if (alertBox.active) {
                zoneModel.resetControls()
                allShadesControl.reset()
            }
        }
    }

    StackView.onActivated: {
        reload()
    }

    StackView.onDeactivating: {
        zoneModel.resetControls()
    }

    ZoneModel {
        id: zoneModel
    }

    Connections {
        target: userData
        onDataUpdated: {
            reload()
        }
    }

    AllShadesControl {
        id: allShadesControl
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 12
        visible: zoneModel.count > 1
        z:1
        onCmdShade: function(shade_state){
            zoneModel.commandController("", shade_state);
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

        signal editShadesGroup(string n)

        model: zoneModel

        delegate: Item {
            width: gridView.cellWidth
            height: gridView.cellHeight
            ShadeControl {
                anchors.fill: parent
                anchors.margins: 6
                title: model.name
                shadeState: model.shadeState
                days: model.days
                openAtText: model.openAtText
                closeAtText: model.closeAtText

                allShadesState: allShadesControl.state

                channel: model.channel

                onEdit: editGroup(model.uuid)

                onDoCommandShade: function(shade_state){
                    zoneModel.commandController(model.uuid, shade_state);
                }

                onDifferentState: {
                    console.log("Different state")
                    allShadesControl.state="interim"
                }

            }
        }
    }

}
