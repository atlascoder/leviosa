import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.UserLogin 1.0

import "DefaultTheme.js" as DefTheme
import "libs/aws-sdk.js" as AWS

Page {
	id: registerPage
    visible: true

	title: qsTr("Authentication")

	background: Rectangle {
		anchors.fill: parent
		color: DefTheme.mainColorBg
	}

    signal signedIn()
    signal restore()
	signal authenticate()

    UserLogin {
        id: currentUser
        email: emailInput.text
        password: passwordInput.text
        password2: passwordConfirmInput.text
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
            height: registerPage.height * 0.08
            width: parent.width
            text: currentUser.lastMessage
            wrapMode: Text.WordWrap
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: parent.width / 18
        }

        Rectangle {
			height: registerPage.height * 0.08
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
				font.capitalization: Font.MixedCase
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: parent.height / 3
				Text {
					anchors.fill: parent
					visible: !(parent.focus || parent.text.length > 0)
					color: "#b1b1b1"
					text: "enter valid email"
					verticalAlignment: Text.AlignVCenter
					font.italic: true
					horizontalAlignment: Text.AlignHCenter
				}
                inputMethodHints: Qt.ImhEmailCharactersOnly || Qt.ImhNoPredictiveText
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
			height: registerPage.height * 0.08
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
				horizontalAlignment: Text.AlignHCenter
				font.capitalization: Font.MixedCase
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: parent.height / 3
                echoMode: TextInput.Password
                passwordMaskDelay: 200
                Text {
					anchors.fill: parent
					visible: !(parent.focus || parent.text.length > 0)
					color: "#b1b1b1"
                    text: "enter password"
					verticalAlignment: Text.AlignVCenter
					font.italic: true
					horizontalAlignment: Text.AlignHCenter
				}
                inputMethodHints: Qt.ImhNoPredictiveText
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
			height: registerPage.height * 0.08
			width: parent.width
			color: "#00000000"
			border.color: "#00000000"

			Image {
				id: passwordConfirmIcon
				source: "img/001-write.svg"
				height: parent.height / 2
				antialiasing: true
				fillMode: Image.PreserveAspectFit
				width: height
				x: 10
				anchors.verticalCenter: parent.verticalCenter
			}

			TextInput {
				id: passwordConfirmInput
				anchors.left: passwordConfirmIcon.right
				anchors.right: parent.right
				anchors.rightMargin: 10
				height: passwordConfirmIcon.height
				horizontalAlignment: Text.AlignHCenter
				font.capitalization: Font.MixedCase
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: parent.height / 3
                echoMode: TextInput.Password
                passwordMaskDelay: 200
				z: 1
				Text {
					anchors.fill: parent
					visible: !(parent.focus || parent.text.length > 0)
					color: "#b1b1b1"
					text: "confirm password"
					verticalAlignment: Text.AlignVCenter
					font.italic: true
					horizontalAlignment: Text.AlignHCenter
				}
                inputMethodHints: Qt.ImhNoPredictiveText
			}

			Rectangle {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.top: passwordConfirmInput.bottom
				anchors.topMargin: 2
				width: parent.width - 20
				height: 2
				color: "#000000"
			}

		}

        Button {
            id: signUpButton
            width: parent.width
            height: registerPage.height * 0.08
            background: Rectangle{
                anchors.fill:parent
                color: DefTheme.mainColorLight
                border.color: DefTheme.mainColorLight
                radius: height * 0.4
                layer.enabled: true
                layer.effect: DropShadow {
                    transparentBorder: true
                    radius: signUpButton.down ? 1 : 3
                }
            }
            Text {
                color:DefTheme.mainTextColor
                opacity: signUpButton.enabled ? 1 : 0.3
                anchors.centerIn: parent
                font.pixelSize: parent.height / 2
                text: qsTr("SIGN UP")
            }
            onClicked: currentUser.signUp()
        }

        Rectangle {
            color: "#00000000"
            width: parent.width
			height: registerPage.height * 0.03
        }

        Rectangle {
            color: "#00000000"
            width: parent.width
			height: registerPage.height * 0.05

            Rectangle {
                id: restoreButton
                anchors.right: parent.right
				color: DefTheme.secColor
                anchors.verticalCenter: parent.verticalCenter
                Text{
					text: "Restore"
					font.pointSize: 14
					font.family: "Verdana"
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
					onClicked: restore()
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
			height: registerPage.height * 0.05

            Rectangle {
                id: registerButton
                anchors.right: parent.right
				color: DefTheme.secColor
                anchors.verticalCenter: parent.verticalCenter
                Text{
					text: "Sign in"
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
					onClicked: authenticate()
				}
            }

            Text {
                anchors.right: registerButton.left
				anchors.rightMargin: 8
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Have account already?")
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
                target: signUpButton
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
                target: signUpButton
                enabled: true
            }
            PropertyChanges {
                target: hintText
                color: DefTheme.mainPositiveAccent
            }
        },
        State {
            name: "Registering"
            when: currentUser.authState === UserLogin.Registering
            PropertyChanges {
                target: signUpButton
                enabled: false
            }
            PropertyChanges {
                target: busyPane
                visible: true
            }
        },
        State {
            name: "Authenticating"
            when: currentUser.authState === UserLogin.Authenticating
            PropertyChanges {
                target: signUpButton
                enabled: false
            }
            PropertyChanges {
                target: busyPane
                visible: true
            }
        },
        State {
            name: "Downloading"
            when: currentUser.authState === UserLogin.Downloading
            PropertyChanges {
                target: signUpButton
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
            StateChangeScript {
                script: {
                    registerPage.signedIn();
                }
            }
        },
        State {
            name: "Failed"
            when: currentUser.authState == UserLogin.Failed
            PropertyChanges {
                target: signUpButton
                enabled: false
            }

            PropertyChanges {
                target: busyPane
                visible: false
            }
            PropertyChanges {
                target: hintText
                color: DefTheme.mainNegativeAccent
            }
        }
    ]
}
