import QtQuick 2.7
import QtQuick.Controls 2.2

import "DefaultTheme.js" as DefTheme

WelcomeBrandPage {
	id: welcomePage

	signal authenticate()
	signal register()

    Column {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: DefTheme.toolbarHeight
        height: childrenRect.height
        width: parent.width * 0.75
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: DefTheme.toolbarHeight / 2

        ActionButton {
            id: goSignInButton
            width: parent.width
            text: "SIGN IN"
            onClicked: authenticate()
        }

        ActionButton {
            id: goSignUpButton
            width: parent.width
            outline: true
            text: "SIGN UP"
            onClicked: register()
        }
    }

}
