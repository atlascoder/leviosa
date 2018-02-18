import QtQuick 2.0
import QtQuick.Controls 2.2
import "DefaultTheme.js" as DefTheme

Rectangle {
    id: rootItem
    color: DefTheme.mainModalBg
    anchors.fill: parent

    property string message

    property int mod : rootItem.width / 8

    Image {
        id: icon
        anchors.bottom: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        source: "img/information.png"
    }

    Text {
        id: messageText
        text: message
        anchors.top: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: mod
        anchors.leftMargin: mod
        anchors.rightMargin: mod
        font.pixelSize: mod / 2.5
        horizontalAlignment: Qt.AlignHCenter
        wrapMode: Text.WordWrap
    }
}
