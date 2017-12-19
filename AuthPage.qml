import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import "DefaultTheme.js" as DefTheme

Page {
	id: authPage
    visible: true
    width: 480
    height: 800

	signal register()
	signal restore()
	signal signedIn()

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


		Rectangle {
			width: parent.width * 0.8
			color: "#00000000"
			Image {
				id: logoPic
				width: parent.width
				fillMode: Image.PreserveAspectFit
				source: "img/logo-big.svg"
			}
			ColorOverlay {
				width: logoPic.width
				height: logoPic.height
				source: logoPic
				color: DefTheme.mainColorDark
			}
			height: childrenRect.height
			anchors.horizontalCenter: parent.horizontalCenter
		}


        Rectangle {
			height: authPage.height * 0.02
            width: parent.width
            color: "#00000000"
        }

        Rectangle {
			height: authPage.height * 0.08
            width: parent.width

            color: "#00000000"

            Image {
                id: emailIcon
				source: "img/003-black-back-closed-envelope-shape.svg"
                height: parent.height / 2
                antialiasing: true
                fillMode: Image.PreserveAspectFit
                width: height
                x: 10
                anchors.verticalCenter: parent.verticalCenter
            }

            TextInput {
                id: emailInput
                anchors.left: emailIcon.right
                anchors.right: parent.right
                anchors.rightMargin: 10
                height: emailIcon.height
				horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: parent.height / 3
				Text {
					anchors.fill: parent
					visible: !(parent.focus || parent.text.length > 0)
					color: "#b1b1b1"
					text: "input your email"
					verticalAlignment: Text.AlignVCenter
					font.italic: true
					horizontalAlignment: Text.AlignHCenter
				}
			}

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: emailInput.bottom
                anchors.topMargin: 2
                width: parent.width - 20
                height: 2
                color: "#000000"
            }

        }

        Rectangle {
			height: authPage.height * 0.08
            width: parent.width
            color: "#00000000"

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
				echoMode: TextInput.Password
                font.capitalization: Font.AllUppercase
				horizontalAlignment: Text.AlignHCenter
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
			height: authPage.height * 0.08
            radius: height * 0.4
            Text {
				color: DefTheme.mainTextColor
                anchors.centerIn: parent
                font.pixelSize: parent.height / 2
                text: qsTr("SIGN IN")
            }
			MouseArea {
				anchors.fill: parent
				onClicked: {
					signedIn();
				}
			}
        }

        Rectangle {
            color: "#00000000"
            width: parent.width
			height: authPage.height * 0.03
        }

        Rectangle {
            color: "#00000000"
            width: parent.width
			height: authPage.height * 0.05

            Rectangle {
                id: restoreButton
                anchors.right: parent.right
				color: DefTheme.secColorLight
                anchors.verticalCenter: parent.verticalCenter
                Text{
					text: "Restore"
					font.pointSize: 14
					style: Text.Normal
					topPadding: 3
					bottomPadding: 3
					leftPadding: 6
					rightPadding: 6
					color: DefTheme.mainTextColor
					font.capitalization: Font.AllUppercase
				}
				width: childrenRect.width
				height: childrenRect.height
                radius: height * 0.5
				MouseArea {
					anchors.fill: parent
					onClicked: {
						restore()
					}
				}
            }

            Text {
                anchors.right: restoreButton.left
				anchors.rightMargin: 8
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Forgot password?")
            }
        }

        Rectangle {
            id: rectangle
            color: "#00000000"
            width: parent.width
			height: authPage.height * 0.05

            Rectangle {
                id: registerButton
                anchors.right: parent.right
				color: DefTheme.secColorLight
                anchors.verticalCenter: parent.verticalCenter
                Text{
					text: "Sign up"
					font.pointSize: 14
					style: Text.Normal
					topPadding: 3
					bottomPadding: 3
					leftPadding: 6
					rightPadding: 6
					color: DefTheme.mainTextColor
                    font.capitalization: Font.AllUppercase
                }
				width: childrenRect.width
				height: childrenRect.height
                radius: height * 0.5
				MouseArea {
					anchors.fill: parent
					onClicked: {
						register()
					}
				}
            }

            Text {
                anchors.right: registerButton.left
				anchors.rightMargin: 8
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Don't have account yet?")
            }
        }
    }


}
