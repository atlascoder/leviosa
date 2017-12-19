import QtQuick 2.0
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

import "DefaultTheme.js" as DefTheme

Rectangle {
	width: parent.wodth
	height: width/4
	radius: 6

	signal edit()
	signal openSchedule()

	gradient: Gradient {
	 GradientStop {
		 position: 1
		 color: DefTheme.mainColorLowGrad
	 }

	 GradientStop {
		 position: 0
		 color: DefTheme.mainColor
	 }
	}
	border.width: 0
	property alias title: titleText.text

	Rectangle {
		id: title
		height: width/12
		color: "#00000000"
		border.width: 0
		anchors.top: parent.top
		anchors.topMargin: 0
		anchors.right: parent.right
		anchors.rightMargin: 0
		anchors.left: parent.left
		anchors.leftMargin: 0

		Text {
			id: titleText
			x: 159
			y: 8
			color: "#ffffff"
			text: title
			font.pixelSize: parent.height*0.75
			elide: Text.ElideRight
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter

			MouseArea {
				anchors.fill: parent
				onPressAndHold: edit()
			}
		}

		Image {
			id: scheduleIcon
			x: 437
			y: 44
			width: height
			height: parent.height*0.8
			anchors.verticalCenter: parent.verticalCenter
			anchors.rightMargin: width/4
			anchors.right: parent.right
			fillMode: Image.PreserveAspectFit
			source: "img/schedule-w.svg"
			MouseArea {
				anchors.fill: parent
				onClicked: openSchedule()
			}
		}
	}

	Slider {
		id: slider
		x: 100
		width: parent.width*0.6
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 0
		anchors.top: title.bottom
		anchors.topMargin: 0
		anchors.horizontalCenterOffset: 0
		anchors.horizontalCenter: parent.horizontalCenter
		orientation: Qt.Horizontal
		stepSize: 25
		to: 100
		value: 50
		handle.height: height * 0.4
		handle.width: height * 0.4
		onValueChanged: {
			positionPercentage.text = value + ' %'
		}
		Text {
			id: positionPercentage
			anchors.horizontalCenter: slider.handle.horizontalCenter
			text: parent.value + '%'
			font.pixelSize: parent.height / 5
			anchors.bottom: parent.bottom
			anchors.bottomMargin: 4
			verticalAlignment: Text.AlignVCenter
			horizontalAlignment: Text.AlignHCenter
		}
	}

	Image {
		id: upIcon
		y: 48
		width: height
		height: parent.height / 2
		fillMode: Image.PreserveAspectFit
		anchors.leftMargin: parent.height / 8
		anchors.left: parent.left
		anchors.verticalCenterOffset: parent.height/6
		anchors.verticalCenter: parent.verticalCenter
		source: "img/window-frame.svg"

		Text {
			id: upText
			width: parent.width
			text: qsTr("OPEN")
			anchors.top: parent.top
			anchors.topMargin: 4
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignTop
			wrapMode: Text.WordWrap
			font.pixelSize: parent.height / 5
			anchors.horizontalCenter: parent.horizontalCenter
		}

		Image {
			id: openIndicator
			width: parent.width
			source: "img/move-up.svg"
			fillMode: Image.PreserveAspectFit
			anchors.bottom: parent.bottom
			anchors.bottomMargin: 8
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
		  opacity: 1
		  anchors.fill: parent
		  onClicked: {
			  if (openingAnimation.running){
				openingAnimation.complete();
				openingAnimation.stop();
				upText.text = "OPEN";
			  }
			  else{
				openingAnimation.start();
				  upText.text = "TAP TO STOP";
			  }
			  console.log("Up command");
		  }
		}
	}

	Image {
		id: downIcon
		x: 408
		y: 48
		width: height
		height: parent.height / 2
		fillMode: Image.PreserveAspectFit
		anchors.rightMargin: parent.height / 8
		anchors.right: parent.right
		anchors.verticalCenterOffset: parent.height/6
		anchors.verticalCenter: parent.verticalCenter
		source: "img/window-frame.svg"

		Text {
			id: downText
			x: -376
			y: 25
			width: parent.width
			text: qsTr("CLOSE")
			anchors.bottom: parent.bottom
			anchors.bottomMargin: 8
			fontSizeMode: Text.FixedSize
			verticalAlignment: Text.AlignBottom
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
			font.pixelSize: parent.height / 5
			opacity: 1
			anchors.horizontalCenter: parent.horizontalCenter
		}

		Image {
			id: closeIndicator
			width: parent.width
			anchors.top: parent.top
			anchors.topMargin: 4
			fillMode: Image.PreserveAspectFit
			source: "img/move-down.svg"
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
			onClicked: {
				if (closingAnimation.running){
				  closingAnimation.complete();
				  closingAnimation.stop();
				  downText.text = "CLOSE";
				}
				else{
				  closingAnimation.start();
				  downText.text = "TAP TO STOP";
				}
				console.log("Down command")
			}
		}
	}
}
