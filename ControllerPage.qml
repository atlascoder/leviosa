import QtQuick 2.7
import QtQuick.Controls 2.2
import com.atlascoder.ShadesGroupsModel 1.0
import com.atlascoder.Shade 1.0

import "DefaultTheme.js" as DefTheme

Item {
    id: rootItem
    anchors.fill:parent

    property string controllerMac
    property alias channel : shadesGroupsModel.selectedChannel
    property alias dataModel: shadesGroupsModel

    property bool failed : false

    signal itemsLoaded(int count);
    signal openGroupEdit(string mac, int channel)
    signal commandShade(int channel, int command)

    function init(){
        shadesGroupsModel.controllerMac = controllerMac
        allShadesControl.visible = shadesGroupsModel.count > 1
        itemsLoaded(shadesGroupsModel.count);
    }

    ShadesGroupsModel {
        id: shadesGroupsModel
    }

    onVisibleChanged: {
        if(visible) init();
    }

    AllShadesControl {
        id: allShadesControl
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        visible: shadesGroupsModel.count > 1
        z:1
        onCmdShade: function(shade_state){
            commandShade(0, shade_state);
        }
        onShadeStateChanged: {
            console.log("Change states to all");
            shadesGroupsModel.setStateToAll(allShadesControl.shadeState);
        }
    }

    GridView {
        id: gridView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: allShadesControl.top

        cellWidth: height > width ? (width / 2) : (width / 3)
        cellHeight: height > width ? (height / 3) : (height / 2)

        signal editShadesGroup(string n)

        model: shadesGroupsModel

        delegate: Item {
            width: gridView.cellWidth
            height: gridView.cellHeight
            ShadeControl {
                anchors.centerIn: parent
                width: parent.width * 0.95
                height: parent.height * 0.95
                title: model.name
                shadeState: model.shadeState
                days: model.days
                openAtText: model.openAtText
                closeAtText: model.closeAtText

                allShadesState: allShadesControl.state

                channel: model.channel

                onEdit: openGroupEdit(controllerMac, channel)

                onDoCommandShade: function(gid, shade_state){
                    model.shadeState = shade_state;
                    commandShade(gid, shade_state);
                }

                onDifferentState: {
                    console.log("Different state")
                    allShadesControl.state="interim"
                }

            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: DefTheme.mainModalBg
        visible: failed
        z: 2

        Text {
            anchors.centerIn: parent
            color: DefTheme.mainNegativeAccent
            text: "Controller is not responding."
            font.pixelSize: parent.width / 16
        }

        MouseArea {
            anchors.fill: parent
        }
    }

}
