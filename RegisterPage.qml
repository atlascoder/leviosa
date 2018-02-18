import QtQuick 2.7
import QtQuick.Controls 2.2

import "DefaultTheme.js" as DefTheme

WelcomeBrandPage {
    id: rootItem

    signal verificate()
    signal firstLogin()

    signal restore()
	signal authenticate()

    hintText: currentUser.lastMessage
    hintColor: currentUser.failed ? DefTheme.mainNegativeAccent : (currentUser.ready ? DefTheme.mainPositiveAccent : DefTheme.mainNeutralAccent)

    Component.onCompleted: {
        currentUser.clearInputs()
        currentUser.checkEmailPassword()
    }

    Connections{
        target: currentUser
        onSignedUpConfirmed: firstLogin()
        onSignedUpNotConfirmed: verificate()
    }

    Column {
        id: controlsColumn
        width: parent.width * 0.75
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.verticalCenter

        Rectangle {
            height: rootItem.height * 0.08
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
                onDisplayTextChanged: {
                    currentUser.email = text
                    currentUser.checkEmailPassword()
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
                onDisplayTextChanged: {
                    currentUser.password = text
                    currentUser.checkEmailPassword()
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
                onDisplayTextChanged: {
                    currentUser.password2 = text
                    currentUser.checkEmailPassword()
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
    }

	Column {
		id: column
        width: parent.width * 0.75
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: DefTheme.buttonHeight / 3

        ActionButton {
            id: signUpButton
            width: parent.width
            text: "SIGN UP"
            enabled: currentUser.ready
            checkable: false
            onClicked: currentUser.signUp()
        }

        Item {
            width: parent.width
            height: DefTheme.buttonHeight / 3
        }

        SmallButton {
            id: restoreButton
            anchors.right: parent.right
            text: "RESTORE PASSWORD"
            onClicked: restore()
        }

        Item {
            width: parent.width
            height: DefTheme.buttonHeight / 3
        }

        SmallButton {
            id: registerButton
            anchors.right: parent.right
            text: "SIGN IN"
            onClicked: authenticate()
        }

    }

    Item {
        id: busyPane
        visible: currentUser.busy
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
}
