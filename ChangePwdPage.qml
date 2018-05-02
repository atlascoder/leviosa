import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.UserLogin 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
    id: rootItem

    title: qsTr("Change password")

    showLogo: false
    enableAddAction: false
    enableMenuAction: false

    signal cancel()
    signal restore()

    UserLogin {
        id: currentUser
        email: "change@password.my"
        password: passwordInput.text
        password2: passwordConfirmInput.text
        password3: currentPwdInput.text
    }

	Column {
		id: column
        width: parent.width * 0.75
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Flickable {
            height: rootItem.height * 0.08
            clip: true
            width: parent.width
            Text {
                id: hintText
                width: parent.width
                text: currentUser.lastMessage
                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pixelSize: parent.width / 18
            }
        }

        Rectangle {
            height: rootItem.height * 0.08
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
				font.capitalization: Font.MixedCase
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: parent.height / 3
				Text {
					anchors.fill: parent
					visible: !(parent.focus || parent.text.length > 0)
					color: "#b1b1b1"
                    text: "enter new password"
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
            height: rootItem.height * 0.08
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
				z: 1
				Text {
					anchors.fill: parent
					visible: !(parent.focus || parent.text.length > 0)
					color: "#b1b1b1"
                    text: "confirm new password"
					verticalAlignment: Text.AlignVCenter
					font.italic: true
					horizontalAlignment: Text.AlignHCenter
				}
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

        Rectangle {
            height: rootItem.height * 0.08
            width: parent.width
            color: "#00000000"

            Image {
                id: currentPwdIcon
                source: "img/001-write.svg"
                height: parent.height / 2
                antialiasing: true
                fillMode: Image.PreserveAspectFit
                width: height
                x: 10
                anchors.verticalCenter: parent.verticalCenter
            }

            TextInput {
                id: currentPwdInput
                anchors.left: currentPwdIcon.right
                anchors.right: parent.right
                anchors.rightMargin: 10
                height: currentPwdIcon.height
                horizontalAlignment: Text.AlignHCenter
                font.capitalization: Font.MixedCase
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: parent.height / 3
                Text {
                    anchors.fill: parent
                    visible: !(parent.focus || parent.text.length > 0)
                    color: "#b1b1b1"
                    text: "enter current password"
                    verticalAlignment: Text.AlignVCenter
                    font.italic: true
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: currentPwdIcon.bottom
                anchors.topMargin: 2
                width: parent.width - 20
                height: 2
                color: "#000000"
            }

        }

        Item {
            width: parent.width
            height: DefTheme.buttonHeight / 3
        }

        ActionButton {
            width: parent.width
            onClicked: currentUser.changePassword()
            text: "CHANGE"
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
            name: "Authenticated"
            when: currentUser.authState === UserLogin.Authenticated
            StateChangeScript {
                script: {
                    rootItem.signedIn();
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
