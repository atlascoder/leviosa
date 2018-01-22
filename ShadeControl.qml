import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.ShadesGroupsModel 1.0
import com.atlascoder.Shade 1.0

import "DefaultTheme.js" as DefTheme

Control {
    id: rootItem
    width: 200
    height: width

    background: Rectangle {
        radius: rootItem.height / 50
        color: DefTheme.mainColorLight

        border.color: "#00000000"
        layer.enabled: true
        layer.effect: DropShadow {
            radius: 3
            transparentBorder: true
        }
    }

    signal edit()

    signal cmdShade(int gid, int cmd)

    property int channel

    property alias title : titleText.text
    property int days
    property int shadeState
    property string closeAtText
    property string openAtText

    property int margin : width / 30

    onStateChanged: {
        console.log("State => " + rootItem.state)
    }


    Rectangle {
        id: upButton
        height: parent.height / 3
        color: "#00000000"
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top

        Text {
            anchors.top: parent.top
            anchors.topMargin: rootItem.margin
            anchors.left: parent.left
            anchors.leftMargin: rootItem.margin
            width: parent.height / 4
            text: " " + rootItem.openAtText
            horizontalAlignment: Text.AlignLeft
            visible: rootItem.days != 0
            font.pixelSize: parent.height / 5
        }

        SequentialAnimation {
            id: openingAnimation
            PropertyAnimation {
                property: "opacity"
                from: 1.0
                to: 0
                duration: 250
                target: upIcon
            }
            PropertyAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 250
                target: upIcon
            }
            loops: Animation.Infinite
        }


        Image {
            id: upIcon
            anchors.fill: parent
            anchors.margins: rootItem.margin
            fillMode: Image.PreserveAspectFit
            source: "img/ic_up.png"
        }

        Text {
            id: upCaption
            anchors.centerIn: parent
            text: "OPEN"
            font.pointSize: parent.height/3
            visible: false
        }

        MouseArea {
            id: upTouchListener
            anchors.fill: parent
        }


    }

    Rectangle {
        id: downButton
        height: parent.height / 3
        color: "#00000000"
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        Text {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: rootItem.margin
            anchors.right: parent.right
            anchors.rightMargin: rootItem.margin
            width: parent.height / 4
            text: rootItem.closeAtText + " "
            horizontalAlignment: Text.AlignRight
            visible: rootItem.days != 0
            font.pixelSize: parent.height / 5
        }

        SequentialAnimation {
            id: closingAnimation
            PropertyAnimation {
                property: "opacity"
                from: 1.0
                to: 0
                duration: 250
                target: downIcon
            }
            PropertyAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 250
                target: downIcon
            }
            loops: Animation.Infinite
        }

        Text {
            id: downCaption
            anchors.centerIn: parent
            text: "CLOSED"
            font.pointSize: parent.height/3
            visible: false
        }

        Image {
            id: downIcon
            anchors.topMargin: 7
            anchors.bottomMargin: 7
            anchors.leftMargin: 7
            anchors.rightMargin: 7
            anchors.fill: parent
            anchors.margins: rootItem.margin
            fillMode: Image.PreserveAspectFit
            source: "img/ic_down.png"
        }

        MouseArea {
            id: downTouchListener
            anchors.fill: parent
        }

    }

    Rectangle {
        id: titleBar
        color: "#00000000"
        anchors.bottom: downButton.top
        anchors.top: upButton.bottom
        anchors.right: parent.right
        anchors.left: parent.left

        Rectangle {
            height: 1
            width: parent.width
            color: "#000000"
            opacity: 0.1
            anchors.top: parent.top
        }

        Rectangle {
            height: 1
            width: parent.width
            color: "#000000"
            opacity: 0.1
            anchors.bottom: parent.bottom
        }

        MouseArea {
            anchors.fill: parent
            onPressAndHold: edit()
        }

        Row {
            id: daysRow
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 2
            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.right: parent.right
            anchors.rightMargin: 2
            spacing: 2
            property int itemWidth : (width - spacing*6) / 7
            Repeater {
                model: ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"]
                delegate : Rectangle {
                    width: daysRow.itemWidth
                    color: (rootItem.days & (1 << index)) == 0 ? "#00000000" : DefTheme.mainColor
                    Text {
                        width: daysRow.itemWidth
                        font.pixelSize: width * 0.4
                        text: modelData
                        horizontalAlignment: Text.AlignHCenter
                        color: (rootItem.days & (1 << index)) == 0 ? "#000000" : "#ffffff"
                        opacity: (rootItem.days & (1 << index)) == 0 ? 0.2 : 0.8
                        font.bold: (rootItem.days & (1 << index)) != 0
                    }
                    height: childrenRect.height
                    radius: height / 4
                }
            }
        }

        Text {
            id: titleText
            color: "#ffffff"
            anchors.fill: parent
            anchors.leftMargin: rootItem.margin
            anchors.rightMargin: rootItem.margin
            wrapMode: Text.NoWrap
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: parent.height * 0.33
            elide: Text.ElideRight
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
        }

    }

    Timer {
        id: actionTimer
        interval: 5000
    }

    state: "interim"

    states: [
        State {
            name: "opened"
            when: rootItem.shadeState === Shade.Opened

            PropertyChanges {
                target: upTouchListener
                onClicked: cmdShade(rootItem.channel, Shade.Up)
                onPressAndHold: cmdShade(rootItem.channel, Shade.Open)
            }

            PropertyChanges {
                target: downTouchListener
                onClicked: cmdShade(rootItem.channel, Shade.Down)
                onPressAndHold: cmdShade(rootItem.channel, Shade.Close)
            }

            PropertyChanges {
                target: upIcon
                source: "img/ic_open.png"
                visible: false
            }

            PropertyChanges {
                target: upCaption
                visible: true
            }

            PropertyChanges {
                target: downIcon
                source: "img/ic_down.png"
            }
        },
        State {
            name: "closed"
            when: rootItem.shadeState === Shade.Closed

            PropertyChanges {
                target: downTouchListener
                onClicked: cmdShade(rootItem.channel, Shade.Down)
                onPressAndHold: cmdShade(rootItem.channel, Shade.Close)
            }

            PropertyChanges {
                target: upTouchListener
                onClicked: cmdShade(rootItem.channel, Shade.Up)
                onPressAndHold: cmdShade(rootItem.channel, Shade.Open)
            }

            PropertyChanges {
                target: downCaption
                visible: true
                text: "CLOSED"
            }

            PropertyChanges {
                target: downIcon
                source: "img/ic_close.png"
                visible: false
            }
        },
        State {
            name: "fullOpened"
            when: rootItem.shadeState === Shade.FullOpened

            PropertyChanges {
                target: upTouchListener
                onClicked: {}
                onPressAndHold: {}
            }

            PropertyChanges {
                target: downTouchListener
                onClicked: cmdShade(rootItem.channel, Shade.Down)
                onPressAndHold: cmdShade(rootItem.channel, Shade.Close)
            }

            PropertyChanges {
                target: upIcon
                visible: false
            }

            PropertyChanges {
                target: upCaption
                text: "Full OPEN"
                visible: true
            }
        },
        State {
            name: "fullClosed"
            when: rootItem.shadeState === Shade.FullClosed

            PropertyChanges {
                target: downTouchListener
                onClicked: {}
                onPressAndHold: {}
            }

            PropertyChanges {
                target: upTouchListener
                onClicked: cmdShade(rootItem.channel, Shade.Up)
                onPressAndHold: cmdShade(rootItem.channel, Shade.Open)
            }

            PropertyChanges {
                target: downCaption
                text: "Full CLOSED"
                visible: true
            }

            PropertyChanges {
                target: downIcon
                visible: false
            }
        },
        State {
            name: "opening"
            when: rootItem.shadeState === Shade.Open

            PropertyChanges {
                target: downIcon
                source: "img/ic_pause.png"
                visible: true
            }

            PropertyChanges {
                target: upTouchListener
                onClicked: {}
                onPressAndHold: {}
            }

            PropertyChanges {
                target: downTouchListener
                onClicked: cmdShade(rootItem.channel, Shade.Interim)
                onPressAndHold: cmdShade(rootItem.channel, Shade.Interim)
            }
            PropertyChanges {
                target: openingAnimation
                running: true
            }
            PropertyChanges {
                target: downCaption
                visible: false
            }
            PropertyChanges {
                target: actionTimer
                running: true
                onTriggered: cmdShade(rootItem.channel, Shade.FullOpened)
            }
            PropertyChanges {
                target: upIcon
                source: "img/ic_open.png"
            }
        },
        State {
            name: "closing"
            when: rootItem.shadeState === Shade.Close

            PropertyChanges {
                target: upIcon
                source: "img/ic_pause.png"
                visible: true
            }
            PropertyChanges {
                target: upTouchListener
                onClicked: cmdShade(rootItem.channel, Shade.Interim)
                onPressAndHold: cmdShade(rootItem.channel, Shade.Interim)
            }
            PropertyChanges {
                target: downTouchListener
                onClicked: {}
                onPressAndHold: {}
            }
            PropertyChanges {
                target: closingAnimation
                running: true
            }
            PropertyChanges {
                target: upCaption
                visible: false
            }
            PropertyChanges {
                target: actionTimer
                running: true
                onTriggered: cmdShade(rootItem.channel, Shade.FullClosed)
            }
            PropertyChanges {
                target: downIcon
                source: "img/ic_close.png"
            }
        },
        State {
            name: "up"
            when: rootItem.shadeState === Shade.Up

            PropertyChanges {
                target: downIcon
                source: "img/ic_pause.png"
                visible: true
            }

            PropertyChanges {
                target: upIcon
                source: "img/ic_up.png"
                visible: true
            }

            PropertyChanges {
                target: upTouchListener
                onClicked: {}
                onPressAndHold: {}
            }

            PropertyChanges {
                target: downTouchListener
                onClicked: cmdShade(rootItem.channel, Shade.Interim)
                onPressAndHold: cmdShade(rootItem.channel, Shade.Interim)
            }
            PropertyChanges {
                target: openingAnimation
                running: true
            }
            PropertyChanges {
                target: downCaption
                visible: false
            }
            PropertyChanges {
                target: upCaption
                visible: false
            }
            PropertyChanges {
                target: actionTimer
                running: true
                onTriggered: cmdShade(rootItem.channel, Shade.Opened)
            }
        },
        State {
            name: "down"
            when: rootItem.shadeState === Shade.Down

            PropertyChanges {
                target: upIcon
                source: "img/ic_pause.png"
                visible: true
            }

            PropertyChanges {
                target: downIcon
                source: "img/ic_down.png"
                visible: true
            }

            PropertyChanges {
                target: upTouchListener
                onClicked: cmdShade(rootItem.channel, Shade.Interim)
                onPressAndHold: cmdShade(rootItem.channel, Shade.Interim)
            }
            PropertyChanges {
                target: downTouchListener
                onClicked: {}
                onPressAndHold: {}
            }
            PropertyChanges {
                target: closingAnimation
                running: true
            }
            PropertyChanges {
                target: upCaption
                visible: false
            }
            PropertyChanges {
                target: downCaption
                visible: false
            }
            PropertyChanges {
                target: actionTimer
                running: true
                onTriggered: cmdShade(rootItem.Channel, Shade.Closed)
            }
        },
        State {
            name: "interim"
            when: rootItem.shadeState === Shade.Interim

            PropertyChanges {
                target: upIcon
                source: "img/ic_up.png"
                visible: true
            }
            PropertyChanges {
                target: downIcon
                source: "img/ic_down.png"
                visible: true
            }
            PropertyChanges {
                target: downTouchListener
                onClicked: cmdShade(rootItem.channel, Shade.Down)
                onPressAndHold: cmdShade(rootItem.channel, Shade.Close)
            }
            PropertyChanges {
                target: upTouchListener
                onClicked: cmdShade(rootItem.channel, Shade.Up)
                onPressAndHold: cmdShade(rootItem.channel, Shade.Open)
            }
            StateChangeScript {
                id: stopAnimation
                script: { openingAnimation.complete(); closingAnimation.complete(); }
            }
            PropertyChanges {
                target: actionTimer
                running: false
            }
        }
    ]
}
