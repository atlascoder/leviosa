import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.ShadeGroupModel 1.0
import com.atlascoder.Shade 1.0

import "DefaultTheme.js" as DefTheme

Control {
    id: rootItem
    width: 200
    height: width / 2

    property string allShadesState
    property bool disabled : false
    property alias title: titleText.text

    property int controllerState : 0

    signal edit()
    signal open()
    signal doCommandShade(int command)

    MouseArea {
        id: clickListener
        anchors.fill: parent
        onPressAndHold: edit()
    }

    background: Rectangle {
        radius: 6
        color: DefTheme.mainColorLight

        border.color: "#00000000"
        layer.enabled: true
        layer.effect: DropShadow {
            radius: 3
            transparentBorder: true
        }
    }

    Rectangle {
        id: controllerIndicator
        width: 12
        height: 12
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 6
        radius: 6
        border.color: "#00000000"
    }

    Text {
        id: titleText
        color: "#ffffff"
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        wrapMode: Text.WordWrap
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: parent.height * 0.2
        elide: Text.ElideRight
        font.bold: true
    }

    // states in correspondance to
    // controller states: {Wan, Searching, NotFound, Configuring, ConfigFailed, Wlan}
    states: [
        State {
            name: "Wan"
            when: controllerState == 0
            PropertyChanges { target: controllerIndicator; color: "transparent" }
            PropertyChanges { target: clickListener; onClicked: { open() } }
        },
        State {
            name: "Searching"
            when: controllerState == 1
            PropertyChanges { target: controllerIndicator; color: "#eff946" }
            PropertyChanges { target: clickListener; onClicked: { open() } }
//            PropertyChanges {
//                target: clickListener
//                onClicked: {
//                    alertBox.showMessage(
//                                "Zone Controller Not Ready",
//                                "Close this screen and try to command",
//                                ""
//                            )
//                }
//            }
        },
        State {
            name: "NotFound"
            when: controllerState == 2
            PropertyChanges { target: controllerIndicator; color: "#e00000" }
            PropertyChanges { target: clickListener; onClicked: { open() } }
//            PropertyChanges {
//                target: clickListener
//                onClicked: {
//                    alertBox.showMessage(
//                                "Zone Controller Error",
//                                "1. Check the network connection of this device.\n2. Check for power to the Zone (solid green and yellow lights)",
//                                ""
//                            )
//                }
//            }
        },
        State {
            name: "Wlan"
            when: controllerState == 3
            PropertyChanges { target: controllerIndicator; color: "#87fb5b" }
            PropertyChanges { target: clickListener; onClicked: { open() } }
        }
    ]
}
