import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import "DefaultTheme.js" as DefTheme

Page {
	id: welcomePage
    visible: true

	signal authenticate()
	signal register()

	background: Rectangle {
		anchors.fill: parent
		color: DefTheme.mainColorBg
	}

	Column {
		id: column
		width: parent.width * 0.75
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		spacing: 24


		Rectangle {
			width: parent.width * 0.8
			color: "transparent"
			border.color: "transparent"
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
			width: parent.width
			height: parent.spacing * 3
			color: "transparent"
			border.color: "transparent"
		}

		Rectangle {
			id: signInButton
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width
			height: welcomePage.height * 0.08
			color: DefTheme.mainColorLight
			border.color: DefTheme.mainColorLight
			radius: height * 0.4
			Text {
				color: DefTheme.mainColorBg
				anchors.centerIn: parent
				font.pixelSize: parent.height / 2
				text: qsTr("SIGN IN")
			}
			MouseArea {
				anchors.fill: parent
				onClicked: welcomePage.authenticate()
			}
		}

		Rectangle {
			id: signUpButton
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width
			height: welcomePage.height * 0.08
			color: "transparent"
			border.color: DefTheme.mainColorLight
			radius: height * 0.4
			Text {
				color: DefTheme.mainColorLight
				anchors.centerIn: parent
				font.pixelSize: parent.height / 2
				text: qsTr("SIGN UP")
			}
			MouseArea {
				anchors.fill: parent
				onClicked: welcomePage.register()
			}
		}

	}

}
