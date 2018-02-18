import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

import "DefaultTheme.js" as DefTheme

Page {

    property alias hintText : hintMessageText.text
    property alias hintColor: hintMessageText.color

    background: Rectangle {
        anchors.fill: parent
        color: DefTheme.mainColorBg
    }

    Image {
        id: logoPic
        anchors.top: parent.top
        anchors.topMargin: parent.width * 0.12
        anchors.bottom: parent.verticalCenter
        anchors.bottomMargin: parent.height * 0.1
        anchors.horizontalCenter: parent.horizontalCenter
        fillMode: Image.PreserveAspectFit
        source: "img/logo-big.svg"
    }
    ColorOverlay {
        anchors.fill: logoPic
        source: logoPic
        color: DefTheme.mainColorDark
    }

    Flickable {
        id: hintPane
        height: parent.height * 0.08
        width: parent.width * 0.75
        anchors.bottom: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        contentWidth: width
        contentHeight: hintMessageText.height
        flickableDirection: Flickable.VerticalFlick
        clip: true

        Text {
            id: hintMessageText
            width: parent.width
            anchors.verticalCenter: parent.verticalCenter
            wrapMode: Text.WordWrap
            horizontalAlignment: Qt.AlignHCenter
            font.pixelSize: parent.width / 18
        }
    }
}
