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

    StackView.onActivated: reload()


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

    Rectangle {
        anchors.fill: parent
        id: disabledModal
        color: DefTheme.mainModalBg
        visible: failed
        z: 2

        MouseArea {
            anchors.fill: parent
            enabled: parent.visible
        }

        Image {
            id: sadIcon
            fillMode: Image.PreserveAspectFit
            width: parent.width / 4
            source: "img/sad.png"
            anchors.top: parent.top
            anchors.topMargin: parent.width / 5
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Column {
            anchors.top: sadIcon.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: parent.width / 12
            anchors.rightMargin: parent.width / 12
            spacing: width / 24
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Qt.AlignHCenter
                text: "Controller is not responding"
                font.bold: true
                font.pixelSize: width / 14
            }
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Qt.AlignHCenter
                text: "1. You can try to find it again"
                font.bold: true
                font.pixelSize: width / 20
                wrapMode: Text.Wrap
            }
            ActionButton {
                id: refreshButton
                anchors.horizontalCenter: parent.horizontalCenter
                text: "REFRESH"
                onClicked: {
                    rootItem.doRefresh()
                }
            }
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Qt.AlignHCenter
                text: "2. You may reinstall your contoroller by holding 'Reset' button for 8 seconds, and then run Setup"
                font.bold: true
                font.pixelSize: width / 20
                wrapMode: Text.Wrap
            }
            ActionButton {
                id: setupButton
                anchors.horizontalCenter: parent.horizontalCenter
                text: "SETUP"
                outline: true
                onClicked: doSetup()
            }
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Qt.AlignHCenter
                text: "3. Or, you can delete controller with all its settings"
                font.italic: true
                font.pixelSize: width / 20
                wrapMode: Text.Wrap
            }
            DeleteButton {
                id: deleteButton
                anchors.horizontalCenter: parent.horizontalCenter
                onPressAndHold: deleteController(controllerMac)
            }
        }
    }
}
