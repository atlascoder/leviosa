import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.UserLogin 1.0

import "DefaultTheme.js" as DefTheme

Page {
    id: rootItem
    visible: true
    width: 480
    height: 800

	signal register()
    signal signIn()

	background: Rectangle {
		anchors.fill: parent
		color: DefTheme.mainColorBg
	}

    UserLogin{
        id: currentUser
        email: emailInput.text
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


        Text {
            id: hintText
            height: rootItem.height * 0.08
            width: parent.width
            text: currentUser.lastMessage
            wrapMode: Text.WordWrap
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 16
            color: rootItem.state != "NotReady" && rootItem.state != "Failed" ? DefTheme.mainPositiveAccent : DefTheme.mainNegativeAccent
        }

        Rectangle {
            id: emailRow
            height: rootItem.height * 0.08
            width: parent.width

            color: "#00000000"

            Item {
                id: emailIcon
                height: parent.height / 2
                width: height
                anchors.verticalCenter: parent.verticalCenter
                x: 10
                Image {
                    id: emailPic
                    height: parent.height
                    width: parent.width
                    source: "img/003-black-back-closed-envelope-shape.svg"
                    anchors.centerIn: parent
                }
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
                    text: "enter your email"
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

        Button {
            id: signInButton
            width: parent.width
            height: rootItem.height * 0.08
            background: Rectangle{
                anchors.fill:parent
                color: DefTheme.mainColorLight
                border.color: DefTheme.mainColorLight
                radius: height * 0.4
                layer.enabled: true
                layer.effect: DropShadow {
                    transparentBorder: true
                    radius: signInButton.down ? 1 : 3
                }
            }
            Text {
                color:DefTheme.mainTextColor
                opacity: signInButton.enabled ? 1 : 0.3
                anchors.centerIn: parent
                font.pixelSize: parent.height / 2
                text: qsTr("RESTORE")
            }
            onClicked: currentUser.signIn()
        }

        Rectangle {
            color: "#00000000"
            width: parent.width
            height: rootItem.height * 0.03
        }

        Rectangle {
            color: "#00000000"
            width: parent.width
            height: rootItem.height * 0.05

            Rectangle {
                id: restoreButton
                anchors.right: parent.right
				color: DefTheme.secColorLight
                anchors.verticalCenter: parent.verticalCenter
                Text{
                    text: "Sign In"
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
                        rootItem.signIn()
					}
				}
            }

            Text {
                anchors.right: restoreButton.left
				anchors.rightMargin: 8
				anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Remembered?")
            }
        }

        Rectangle {
            id: rectangle
            color: "#00000000"
            width: parent.width
            height: rootItem.height * 0.05

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

    Item {
        id: busyPane
        visible: false
        anchors.fill: parent
        Rectangle {
            anchors.fill: parent
            opacity: 0.7
        }

        BusyIndicator {
            id: busyIndicator
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {}
        }
    }

    states:[
        State {
            name: "NotReady"
            when: currentUser.authState === UserLogin.BadInputs
            PropertyChanges {
                target: signInButton
                enabled: false
            }
            PropertyChanges {
                target: hintText
                color: DefTheme.mainWarningAccent
            }
        },
        State {
            name: "Ready"
            when: currentUser.authState === UserLogin.Ready
            PropertyChanges {
                target: signInButton
                enabled: true
            }
            PropertyChanges {
                target: hintText
                color: DefTheme.mainPositiveAccent
            }
        },
        State {
            name: "Authenticating"
            when: currentUser.authState === UserLogin.Authenticating
            PropertyChanges {
                target: signInButton
                enabled: false
            }
            PropertyChanges {
                target: busyPane
                visible: true
            }
        },
        State {
            name: "Authenticated"
            when: currentUser.authState === UserLogin.Authenticated
            PropertyChanges {
                target: signInButton
                enabled: false
            }
            StateChangeScript {
                name: "openApp"
                script: {
                    rootItem.signIn();
                }
            }
        },
        State {
            name: "Failed"
            when: currentUser.authState === UserLogin.Failed
            PropertyChanges {
                target: signInButton
                enabled: false
            }
            PropertyChanges {
                target: hintText
                color: DefTheme.mainNegativeAccent
            }
            PropertyChanges {
                target: busyPane
                visible: false
            }
        }
    ]

}
