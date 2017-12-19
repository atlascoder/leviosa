import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import QtQuick.Extras 1.4
import "DefaultTheme.js" as DefTheme

Window {
    id: window
    visible: true
    width: 480
    height: 800
	color: DefTheme.mainColorBg
    property alias logoImage: logoImage
    title: qsTr("Corb")

    Image {
        id: logoImage
        anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.75
		anchors.verticalCenterOffset: -height/2
		fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: parent.verticalCenter
        smooth: true
		source: "img/logo-big.svg"
    }



    Rectangle {
        id: signInButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.height * 0.2
        width: parent.width * 0.75
        height: parent.height * 0.08
		color: DefTheme.mainColorLight
		border.color: DefTheme.mainColorLight
        radius: height * 0.4
        Text {
			color: DefTheme.mainColorBg
            anchors.centerIn: parent
            font.pixelSize: parent.height / 2
            text: qsTr("SIGN IN")
        }
    }

    Rectangle {
        id: signUpButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.height * 0.1
        width: parent.width * 0.75
        height: parent.height * 0.08
		color: "transparent"
		border.color: DefTheme.mainColorLight
        radius: height * 0.4
        Text {
			color: DefTheme.mainColorLight
            anchors.centerIn: parent
            font.pixelSize: parent.height / 2
            text: qsTr("SIGN UP")
        }
    }

}
