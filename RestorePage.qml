import QtQuick 2.7
import QtQuick.Controls 2.2

import "DefaultTheme.js" as DefTheme

WelcomeBrandPage {
    id: rootItem

	signal register()
    signal signIn()
    signal resetPassword()

    hintText: currentUser.lastMessage
    hintColor: currentUser.failed ? DefTheme.mainNegativeAccent : (currentUser.ready ? DefTheme.mainPositiveAccent : DefTheme.mainNeutralAccent)

    Connections {
        target: currentUser
        onRestoreRequestSent: resetPassword()
    }

    Column {
        id: controlsColumn
        width: parent.width * 0.75
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.verticalCenter

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
                font.capitalization: Font.AllLowercase
                Text {
                    anchors.fill: parent
                    visible: !(parent.focus || parent.text.length > 0)
                    color: "#b1b1b1"
                    text: "enter your email"
                    verticalAlignment: Text.AlignVCenter
                    font.italic: true
                    horizontalAlignment: Text.AlignHCenter
                }
                inputMethodHints: Qt.ImhNoPredictiveText || Qt.ImhEmailCharactersOnly
                onDisplayTextChanged: {
                    currentUser.email = text
                    currentUser.checkEmail()
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
            text: "RESTORE"
            enabled: currentUser.ready
            onClicked: currentUser.restorePassword()
        }

        Item {
            width: parent.width
            height: DefTheme.buttonHeight / 3
        }

        SmallButton {
            id: restoreButton
            anchors.right: parent.right
            text: "REGISTER ACCOUNT"
            onClicked: register()
        }

        Item {
            width: parent.width
            height: DefTheme.buttonHeight / 3
        }

        SmallButton {
            id: registerButton
            anchors.right: parent.right
            text: "SIGN IN"
            onClicked: signIn()
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
