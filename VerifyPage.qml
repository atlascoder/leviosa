import QtQuick 2.7
import QtQuick.Controls 2.2

import "DefaultTheme.js" as DefTheme

WelcomeBrandPage {
    id: rootItem

    signal register()

    signal signIn()
    signal signedIn()

    hintText: currentUser.lastMessage
    hintColor: currentUser.failed ? DefTheme.mainNegativeAccent : (currentUser.ready ? DefTheme.mainPositiveAccent : DefTheme.mainNeutralAccent)

    Component.onCompleted: currentUser.checkVerification()

    Connections {
        target: currentUser
        onVerified: {
            if(currentUser.password === "" || currentUser.email === "")
                rootItem.signIn()
            else
                currentUser.signIn()
        }
        onSignedIn: rootItem.signedIn()
    }

    Column {
        id: controlsPane
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
                    source: "img/key.png"
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
                    text: "enter verification code"
                    verticalAlignment: Text.AlignVCenter
                    font.italic: true
                    horizontalAlignment: Text.AlignHCenter
                }
                inputMethodHints: Qt.ImhNoPredictiveText || Qt.ImhEmailCharactersOnly
                onDisplayTextChanged: {
                    currentUser.password3 = text
                    currentUser.checkVerification()
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
            text: "VERIFY"
            onClicked: currentUser.verify()
        }

        Item {
            width: parent.width
            height: DefTheme.buttonHeight / 3
        }

        SmallButton {
            id: restoreButton
            anchors.right: parent.right
            text: "SEND CODE AGAIN"
            onClicked: currentUser.retryConfirmation()
        }

        Item {
            width: parent.width
            height: DefTheme.buttonHeight / 3
        }

        SmallButton {
            id: registerButton
            anchors.right: parent.right
            text: "SIGN UP"
            onClicked: register()
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
