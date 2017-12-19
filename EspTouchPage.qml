import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.Esptouch 1.0

import "DefaultTheme.js" as DefTheme

Page {
	id: espTouchPage
	visible: true
	width: 480
	height: 800

	signal espTouchStarted
	signal espTouchFinshed

	Esptouch {
		id: esptouch;
	}

	background: Rectangle {
		anchors.fill: parent
		color: DefTheme.mainColorBg
	}

	Column {
		id: column
        width: parent.width * 0.75
        anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
        spacing: 5

		Image {
			width: parent.width * 0.8
			anchors.horizontalCenter: parent.horizontalCenter
			fillMode: Image.PreserveAspectFit
			source: "img/003-wifi.svg"
		}

		Text {
			width: parent.width
			text: "<b>WiFi connection setup</b>"
			font.pixelSize: parent.width/16
			textFormat: Text.RichText
			verticalAlignment: Text.AlignVCenter
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

        Rectangle {
			height: espTouchPage.height * 0.02
            width: parent.width
			color: "#00000000"
			border.color: "#00000000"
        }

		Rectangle {
			id: rectangle
            width: parent.width

			color: "#00000000"
			border.color: "#00000000"

			Text {
				id: ssidIntro
				width: parent.width
				text: "<b>Check that the current WiFi network is the network you want connect WiShadeController to!</b><br><br>Try to use the same network for all Controllers in singe location."
				font.pixelSize: parent.width / 20
				horizontalAlignment: Text.AlignHCenter
				wrapMode: Text.WordWrap
			}

			Text {
				id: ssid
				anchors.top: ssidIntro.bottom
				anchors.left: parent.left
				anchors.leftMargin: 10
                anchors.right: parent.right
                anchors.rightMargin: 10
				font.pixelSize: parent.height / 4
				text: "SSID"
				font.bold: true
				anchors.topMargin: 12
				horizontalAlignment: Text.AlignHCenter
			}

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
				anchors.top: ssid.bottom
                anchors.topMargin: 2
                width: parent.width - 20
                height: 2
                color: "#000000"
            }

			height: childrenRect.height

        }



		Rectangle {
			height: espTouchPage.height * 0.08
            width: parent.width
			color: "#00000000"
			border.color: "#00000000"

            Image {
                id: passwordIcon
				source: "img/001-write.svg"
                height: parent.height / 2
                antialiasing: true
                fillMode: Image.PreserveAspectFit
                width: height
                x: 10
                anchors.verticalCenter: parent.verticalCenter
            }

            TextInput {
                id: passwordInput
                anchors.left: passwordIcon.right
                anchors.right: parent.right
                anchors.rightMargin: 10
				height: passwordIcon.height
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: parent.height / 3
				Text {
					anchors.fill: parent
					visible: !(parent.focus || parent.text.length > 0)
					color: "#b1b1b1"
					text: "input password"
					verticalAlignment: Text.AlignVCenter
					font.italic: true
					horizontalAlignment: Text.AlignHCenter
				}
				enabled: !esptouch.running
			}

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: passwordInput.bottom
                anchors.topMargin: 2
                width: parent.width - 20
                height: 2
                color: "#000000"
            }

        }

        Rectangle {
			color: DefTheme.mainColorLight
			border.color: DefTheme.mainColorLight
            width: parent.width
			height: espTouchPage.height * 0.08
            radius: height * 0.4
            Text {
				color: DefTheme.mainTextColor
                anchors.centerIn: parent
                font.pixelSize: parent.height / 2
				text: {
					return esptouch.running ? "STOP" : "START"
				}
            }
			MouseArea {
				anchors.fill: parent
				onClicked: {
					esptouch.running ? esptouch.interrupt() : esptouch.execute(ssid.text, ssid.text, passwordInput.text);
				}
			}
			Timer {
				id: espTouchTimeout
				repeat: false
				onTriggered: {
					espTouchFinshed();
					running = false;
				}
			}
        }
    }


}
