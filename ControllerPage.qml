import QtQuick 2.7
import QtQuick.Controls 2.2
import com.atlascoder.ShadesGroupsModel 1.0
import com.atlascoder.Shade 1.0

Item {
    id: rootItem
    anchors.fill:parent

    property alias controllerMac : shadesGroupsModel.controllerMac

    signal cmd(int channel, int cmd)

    signal openGroupEdit(int cid, int gid)

    onVisibleChanged: {
        if(rootItem.visible)
        {
            shadesGroupsModel.controllerMac = rootItem.controllerMac;
            allShadesControl.visible = shadesGroupsModel.rowCount() > 1;
        }
    }

    ShadesGroupsModel {
        id: shadesGroupsModel
        onModelReset: {
            allShadesControl.visible = shadesGroupsModel.rowCount() > 1;
        }
    }

    AllShadesControl {
        id: allShadesControl
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        visible: shadesGroupsModel.rowCount() > 1
        z:1
        onCmdShade: function(shade_state){
            cmd(0, shade_state);
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
        signal editGroupSchedule(string n)

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

                channel: model.channel

                onEdit: openGroupEdit(controllerId, model.id)

                onCmdShade: function(gid, shade_state){
                    model.shadeState = shade_state;
                    cmd(gid, shade_state);
                }

            }
        }
    }

}
