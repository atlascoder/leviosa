import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3

Rectangle {
    id: rootItem
    anchors.fill: parent
    color: "#ebffffff"

    property string message

    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.rightMargin: parent.width / 16
        anchors.leftMargin: parent.width / 16
        color: "#f1f1f1"

        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            radius: 5
            samples: 11
        }

        ColumnLayout{
            width: parent.width
            spacing: width / 16

            Text {
                id: messageText
                Layout.fillWidth: true
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                wrapMode: Text.Wrap
                font.pixelSize: parent.width/ 16
                text: message
                font.bold: true
                padding: 12
            }

            Button {
                id: closeButton
                Layout.bottomMargin: height/2
                text: "CLOSE"
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: rootItem.visible = false
            }
        }
        height: childrenRect.height
    }
}
