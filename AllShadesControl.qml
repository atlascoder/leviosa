import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import com.atlascoder.Shade 1.0
import "DefaultTheme.js" as DefTheme

Rectangle {
    id: rootItem
    width: parent.width
    height: width / 5 > 120 ? 120 : width / 5
    color: DefTheme.mainColorLight
    radius: 6

    property int shadeState : Shade.Interim
    property string connectionStateText : ""
    property string controllerTimeText : ""

    signal cmdShade(int cmd)

    border.width: 0
    border.color: "#00000000"

    function reset(){
        shadeState = Shade.Interim
    }

	layer.enabled: true
	layer.effect: DropShadow {
        radius: 3
		transparentBorder: true
	}

    Item {
        id: upButton
        width: parent.width / 6
        anchors.leftMargin: 6
        anchors.top: parent.top
        anchors.bottom: parent.bottom
		anchors.left: parent.left

        Text {
            id: upText
            text: "OPEN"
            visible: false
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
            font.bold: true
            font.pixelSize: parent.width / 4
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
            anchors.margins: width / 8
            source: "img/010-arrows.svg"
            fillMode: Image.PreserveAspectFit
        }

		MouseArea {
            id: upTouchListener
            anchors.topMargin: 0
            anchors.fill: parent
		}
	}

    Rectangle {
        height: parent.height * 0.9
        anchors.verticalCenter: parent.verticalCenter
        width: 1
        anchors.left: upButton.right
        anchors.leftMargin: 6
        color: "#000000"
        opacity: 0.1
    }

    Item {
        id: downButton
        width: parent.width / 6
        anchors.rightMargin: 6
		anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

		Text {
            id: downText
            text: "CLOSED"
            visible: false
            anchors.fill: parent
            wrapMode: Text.WordWrap
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
            font.pixelSize: parent.width / 4
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

        Image {
            id: downIcon
            anchors.fill: parent
            anchors.margins: width / 8
            fillMode: Image.PreserveAspectFit
            source: "img/ic_down.png"
        }

		MouseArea {
			id: downTouchListener
            anchors.fill: parent
        }

	}

    Rectangle {
        height: parent.height * 0.9
        anchors.verticalCenter: parent.verticalCenter
        width: 1
        anchors.right: downButton.left
        anchors.rightMargin: 6
        color: "#000000"
        opacity: 0.1
    }

    Text {
        id: scheduleBar
        text: "ALL SHADES"
        font.bold: true
        color: DefTheme.mainTextColor
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        bottomPadding: 4
        topPadding: 4
        anchors.top: parent.top
        anchors.right: downButton.left
        anchors.left: upButton.right
        anchors.bottom: parent.bottom
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: parent.height / 4
    }

    Timer {
        id: actionTimer
        interval: 5000
    }

    function updateShadeState(shadeState){
        cmdShade(shadeState);
        rootItem.shadeState = shadeState;
    }

    states: [
        State {
            name: "opened"
            when: rootItem.shadeState === Shade.Opened

            PropertyChanges {
                target: upTouchListener
                onClicked: updateShadeState(Shade.Up)
                onPressAndHold: updateShadeState(Shade.Open)
            }

            PropertyChanges {
                target: downTouchListener
                onClicked: updateShadeState(Shade.Down)
                onPressAndHold: updateShadeState(Shade.Close)
            }

            PropertyChanges {
                target: upIcon
                visible: false
            }

            PropertyChanges {
                target: downIcon
                source: "img/ic_down.png"
                visible: true
            }

            PropertyChanges {
                target: upText
                text: "OPEN"
                visible: true
            }
            PropertyChanges {
                target: downText
                visible: false
            }
        },
        State {
            name: "closed"
            when: rootItem.shadeState === Shade.Closed

            PropertyChanges {
                target: downTouchListener
                onClicked: updateShadeState(Shade.Down)
                onPressAndHold: updateShadeState(Shade.Close)
            }

            PropertyChanges {
                target: upTouchListener
                onClicked: updateShadeState(Shade.Up)
                onPressAndHold: updateShadeState(Shade.Open)
            }

            PropertyChanges {
                target: upIcon
                source: "img/ic_up.png"
                visible: true
            }

            PropertyChanges {
                target: downIcon
                visible: false
            }

            PropertyChanges {
                target: upText
                visible: false
            }

            PropertyChanges {
                target: downText
                text: "CLOSED"
                visible: true
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
                onClicked: updateShadeState(Shade.Down)
                onPressAndHold: updateShadeState(Shade.Close)
            }

            PropertyChanges {
                target: upIcon
                visible: false
            }

            PropertyChanges {
                target: downIcon
                source: "img/ic_down.png"
                visible: true
            }

            PropertyChanges {
                target: upText
                text: "FULL OPEN"
                visible: true
            }

            PropertyChanges {
                target: downText
                visible: false
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
                onClicked: updateShadeState(Shade.Up)
                onPressAndHold: updateShadeState(Shade.Open)
            }

            PropertyChanges {
                target: downIcon
                visible: false
            }

            PropertyChanges {
                target: upIcon
                source: "img/ic_up.png"
                visible: true
            }

            PropertyChanges {
                target: upText
                visible: false
            }

            PropertyChanges {
                target: downText
                text: "FULL CLOSED"
                visible: true
            }
        },
        State {
            name: "opening"
            when: rootItem.shadeState === Shade.Open

            PropertyChanges {
                target: upTouchListener
                onClicked: {}
                onPressAndHold: {}
            }

            PropertyChanges {
                target: downTouchListener
                onClicked: updateShadeState(Shade.Interim)
                onPressAndHold: updateShadeState(Shade.Interim)
            }

            PropertyChanges {
                target: upIcon
                source: "img/ic_open.png"
                visible: true
            }

            PropertyChanges {
                target: downIcon
                source: "img/ic_pause.png"
                visible: true
            }

            PropertyChanges {
                target: upText
                visible: false
            }
            PropertyChanges {
                target: downText
                visible: false
            }

            PropertyChanges {
                target: openingAnimation
                running: true
            }

            PropertyChanges {
                target: actionTimer
                running: true
                onTriggered: {
                    rootItem.shadeState = Shade.FullOpened;
                }
            }
        },
        State {
            name: "closing"
            when: rootItem.shadeState === Shade.Close

            PropertyChanges {
                target: upTouchListener
                onClicked: updateShadeState(Shade.Interim)
                onPressAndHold: updateShadeState(Shade.Interim)
            }

            PropertyChanges {
                target: downTouchListener
                onClicked: {}
                onPressAndHold: {}
            }

            PropertyChanges {
                target: upIcon
                source: "img/ic_pause.png"
                visible: true
            }

            PropertyChanges {
                target: downIcon
                source: "img/ic_close.png"
                visible: true
            }

            PropertyChanges {
                target: upText
                visible: false
            }

            PropertyChanges {
                target: downText
                visible: false
            }

            PropertyChanges {
                target: closingAnimation
                running: true
            }

            PropertyChanges {
                target: actionTimer
                running: true
                onTriggered: {
                    rootItem.shadeState = Shade.FullClosed;
                }
            }
        },
        State {
            name: "up"
            when: rootItem.shadeState === Shade.Up

            PropertyChanges {
                target: upTouchListener
                onClicked: {}
                onPressAndHold: {}
            }

            PropertyChanges {
                target: downTouchListener
                onClicked: updateShadeState(Shade.Interim)
                onPressAndHold: updateShadeState(Shade.Interim)
            }

            PropertyChanges {
                target: upIcon
                source: "img/ic_up.png"
                visible: true
            }

            PropertyChanges {
                target: downIcon
                source: "img/ic_pause.png"
                visible: true
            }

            PropertyChanges {
                target: upText
                visible: false
            }

            PropertyChanges {
                target: downText
                visible: false
            }

            PropertyChanges {
                target: openingAnimation
                running: true
            }

            PropertyChanges {
                target: actionTimer
                running: true
                onTriggered: {
                    rootItem.shadeState = Shade.Opened;
                }
            }
        },
        State {
            name: "down"
            when: rootItem.shadeState === Shade.Down

            PropertyChanges {
                target: upTouchListener
                onClicked: updateShadeState(Shade.Interim)
                onPressAndHold: updateShadeState(Shade.Interim)
            }
            PropertyChanges {
                target: downTouchListener
                onClicked: {}
                onPressAndHold: {}
            }
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
                target: upText
                visible: false
            }
            PropertyChanges {
                target: downText
                visible: false
            }
            PropertyChanges {
                target: closingAnimation
                running: true
            }
            PropertyChanges {
                target: actionTimer
                running: true
                onTriggered: {
                    rootItem.shadeState = Shade.Closed;
                }
            }
        },
        State {
            name: "interim"
            when: rootItem.shadeState === Shade.Interim

            PropertyChanges {
                target: downTouchListener
                onClicked: updateShadeState(Shade.Down)
                onPressAndHold: updateShadeState(Shade.Close)
            }
            PropertyChanges {
                target: upTouchListener
                onClicked: updateShadeState(Shade.Up)
                onPressAndHold: updateShadeState(Shade.Open)
            }
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
