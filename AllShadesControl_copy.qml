import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import "DefaultTheme.js" as DefTheme

Rectangle {
    id: rootItem
    width: parent.width
    height: width / 4 > 120 ? 120 : width / 4
    color: DefTheme.mainColorLight

    border.width: 0
    border.color: "#00000000"

	layer.enabled: true
	layer.effect: DropShadow {
        radius: 3
		transparentBorder: true
		verticalOffset: -1
	}


	Rectangle {
		id: title
        height: parent.height / 3
        color: DefTheme.mainColor
		anchors.top: parent.top
		anchors.right: parent.right
		anchors.left: parent.left

		Text {
			id: titleText
			color: DefTheme.mainTextColor
            text: "ALL"
            style: Text.Normal
            font.pixelSize: parent.height*0.8
            font.bold: true
            anchors.centerIn: parent
		}
	}

    Item {
        id: upButton
        width: parent.width / 6
        anchors.top: title.bottom
        anchors.bottom: parent.bottom
		anchors.left: parent.left

        Text {
            id: upText
            text: "OPENED"
            visible: false
            anchors.centerIn: parent
            font.pixelSize: parent.width / 4
		}

		Image {
			id: openIndicator
            anchors.fill: parent
            anchors.margins: width / 8
            source: "img/010-arrows.svg"
            fillMode: Image.PreserveAspectFit
		}

		SequentialAnimation {
			id: openingAnimation
			PropertyAnimation {
				property: "opacity"
				from: 1.0
				to: 0
				duration: 250
				target: openIndicator
			}
			PropertyAnimation {
				property: "opacity"
				from: 0
				to: 1
				duration: 250
				target: openIndicator
			}
			loops: Animation.Infinite
		}

		MouseArea {
            id: upTouchListener
            anchors.topMargin: 0
		  anchors.fill: parent
		}

        Rectangle {
            height: parent.height * 0.66
            anchors.verticalCenter: parent.verticalCenter
            width: 1
            anchors.right: parent.right
            color: "#000000"
            opacity: 0.1
        }
	}

    Item {
        id: downButton
        width: parent.width / 6
		anchors.right: parent.right
        anchors.top: title.bottom
        anchors.bottom: parent.bottom

		Text {
			id: downText
            text: "CLOSED"
            visible: false
            anchors.centerIn: parent
            font.pixelSize: parent.width / 4
		}

		Image {
			id: closeIndicator
            rotation: 180
            anchors.fill: parent
            anchors.margins: width / 8
			fillMode: Image.PreserveAspectFit
            source: "img/010-arrows.svg"
        }

		SequentialAnimation {
			id: closingAnimation
			PropertyAnimation {
				property: "opacity"
				from: 1.0
				to: 0
				duration: 250
				target: closeIndicator
			}
			PropertyAnimation {
				property: "opacity"
				from: 0
				to: 1
				duration: 250
				target: closeIndicator
			}
			loops: Animation.Infinite
		}

		MouseArea {
			id: downTouchListener
			anchors.fill: parent
		}
        Rectangle {
            height: parent.height * 0.66
            anchors.verticalCenter: parent.verticalCenter
            width: 1
            anchors.left: parent.left
            color: "#000000"
            opacity: 0.1
        }
	}

    Row {
        id: scheduleBar
        anchors.top: title.bottom
        anchors.bottom: parent.bottom
        anchors.right: downButton.left
        anchors.left: upButton.right
        property int itemWidth : (rootItem.width - upButton.width - downButton.width) / 9
        Text {
            id: openAt
            text: "9:00 AM"
            wrapMode: Text.WordWrap
            height: parent.height
            width: scheduleBar.itemWidth
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            font.pixelSize: scheduleBar.itemWidth / 3
            font.bold: true

            Rectangle {
                height: parent.height * 0.33
                anchors.verticalCenter: parent.verticalCenter
                width: 1
                anchors.right: parent.right
                color: "#000000"
                opacity: 0.1
            }
        }

        Repeater {
            model: ["S\nu\nn", "M\no\nn", "T\nu\ne", "W\ne\nd", "T\nh\nu", "F\nr\ni", "S\na\nt"]
            delegate: Text {
                text: modelData
                font.capitalization: Font.AllUppercase
                wrapMode: Text.WordWrap
                height: parent.height
                width: scheduleBar.itemWidth
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                font.pixelSize: scheduleBar.itemWidth * 0.4
                lineHeight: 0.80
                Rectangle {
                    height: parent.height * 0.33
                    anchors.verticalCenter: parent.verticalCenter
                    width: 1
                    anchors.right: parent.right
                    color: "#000000"
                    opacity: 0.1
                }
            }
        }

        Text {
            id: closeAt
            text: "9:00 PM"
            wrapMode: Text.WordWrap
            height: parent.height
            width: scheduleBar.itemWidth
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            font.pixelSize: scheduleBar.itemWidth / 3
            font.bold: true
        }
    }

    state: "interim"
    states: [
        State {
            name: "opened"

            PropertyChanges {
                target: upTouchListener
                onClicked: {}
            }

            PropertyChanges {
                target: downTouchListener
                onClicked: {
                    roomItem.state = "closing";
                }
            }

            PropertyChanges {
                target: openIndicator
                visible: false
            }

            PropertyChanges {
                target: upText
                visible: true
            }
        },
        State {
            name: "closed"

            PropertyChanges {
                target: downTouchListener
                onClicked: {}
            }

            PropertyChanges {
                target: upTouchListener
                onClicked: {
                    rootItem.state = "opening";
                }

            }

            PropertyChanges {
                target: downText
                visible: true
            }

            PropertyChanges {
                target: closeIndicator
                visible: false
            }
        },
        State {
            name: "opening"
            PropertyChanges {
                target: downTouchListener
                onClicked: {
                    rootItem.state = "interim"
                }
            }
            PropertyChanges {
                target: openingAnimation
                running: true
            }
            PropertyChanges {
                target: downText
                visible: false
            }
            PropertyChanges {
                target: closeIndicator
                source: "img/001-circle.svg"
            }
        },
        State {
            name: "closing"
            PropertyChanges {
                target: upTouchListener
                onClicked: {
                    rootItem.state = "interim"
                }
            }
            PropertyChanges {
                target: closingAnimation
                running: true
            }
            PropertyChanges {
                target: upText
                visible: false
            }

            PropertyChanges {
                target: openIndicator
                source: "img/001-circle.svg"
            }
        },
        State {
            name: "interim"
            PropertyChanges {
                target: openIndicator
                source: "img/010-arrows.svg"
                visible: true
            }
            PropertyChanges {
                target: closeIndicator
                source: "img/010-arrows.svg"
                visible: true
            }
            PropertyChanges {
                target: downTouchListener
                onClicked: {
                    rootItem.state = "closing"
                }
            }
            PropertyChanges {
                target: upTouchListener
                onClicked: {
                    rootItem.state = "opening"
                }
            }
            StateChangeScript {
                id: stopAnimation
                script: {
                    openingAnimation.complete()
                    closingAnimation.complete();
                }
            }
        }
    ]

}
