import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
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
    signal runEspTouch()
    signal deleteController(string mac)

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
            anchors.bottom: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Column {
            anchors.top: sadIcon.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: width / 12
            anchors.rightMargin: width / 12
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
                text: "You may reset your contoroller by holding 'Reset' button for 8 seconds, and then run Setup: "
                font.bold: true
                font.pixelSize: width / 20
                wrapMode: Text.Wrap
            }
            Button {
                id: setupButton
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width / 2
                onClicked: runEspTouch()
                contentItem: Text {
                    horizontalAlignment: Qt.AlignHCenter
                    text: "SETUP"
                    font.bold: true
                    font.pixelSize: width / 8
                    color: DefTheme.mainTextColor
                }
                background: Rectangle {
                    color: DefTheme.mainColorDark
                    radius: height / 4
                    layer.enabled: true
                    layer.effect: DropShadow {
                        radius: setupButton.down ? 1 : 3
                        transparentBorder: true
                    }
                }
            }
            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Qt.AlignHCenter
                text: "Or, you can delete controller with all its settings:"
                font.italic: true
                font.pixelSize: width / 20
                wrapMode: Text.Wrap
            }
            Button {
                id: deleteButton
                text: "DELETE\npress and hold"
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width / 2
                font.bold: true
                font.pixelSize: width / 12
                background: Rectangle {
                    color: DefTheme.mainNegativeAccent
                    layer.enabled: true
                    layer.effect: DropShadow {
                        radius: deleteButton.down ? 1 : 3
                        transparentBorder: true
                    }
                }
                onPressAndHold: deleteController(controllerMac)
            }
        }

    }

}
