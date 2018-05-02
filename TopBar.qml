import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import "DefaultTheme.js" as DefTheme

ToolBar {

    signal menuAction()
    signal addAction()
    signal headerLongPressed()

    property string title : "Leviosa Smart Shades"

    property bool enableAddAction
    property bool enableMenuAction

    property bool showSubHeader : false
    property string subHeaderText

    property bool showStatusText : false
    property string statusText

    height: DefTheme.toolbarHeight
    background: Rectangle {
        anchors.fill: parent
        color: DefTheme.mainColorDark
        layer.enabled: true
        layer.effect: DropShadow {
            anchors.fill: parent
            transparentBorder: true
            radius: 3
        }
    }

    RowLayout {
        anchors.fill: parent
        Item {
            width: 3
            height: parent.height
        }
        ToolButton {
            id: menuButton
            Image {
                id: menuIcon
                fillMode: Image.PreserveAspectFit
                source: enableMenuAction ? "img/004-bars.svg" : "img/002-left-arrow.svg"
                anchors.fill: parent
                anchors.margins: parent.height / 4
            }
            onClicked: menuAction()
        }
        Item {
            id: pageTitle
            height: parent.height
            Layout.fillWidth: true

            Text {
                id: headline
                elide: Text.ElideRight
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Qt.AlignHCenter
                width: parent.width
                color: DefTheme.mainTextColor
                text: title
                font.bold: true
                font.pixelSize: parent.height / 2.5
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                verticalAlignment: Qt.AlignVCenter
                anchors.top: parent.top
                anchors.bottom: headline.top
                font.pixelSize: height * 0.8
                text: statusText
                font.italic: true
                color: DefTheme.mainTextColor
                visible: showStatusText
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                verticalAlignment: Qt.AlignVCenter
                anchors.top: headline.bottom
                anchors.bottom: parent.bottom
                font.pixelSize: height * 0.8
                text: subHeaderText
                font.italic: true
                color: DefTheme.mainTextColor
                visible: showSubHeader
            }

            MouseArea {
                anchors.fill: parent
                onPressAndHold: headerLongPressed()
            }
        }
        ToolButton{
            id: addButton
            Image {
                fillMode: Image.PreserveAspectFit
                source: "img/plus-white.svg"
                anchors.fill: parent
                anchors.margins: parent.height / 4
            }
            enabled: enableAddAction
            opacity: enableAddAction ? 1 : 0
            onClicked: addAction()
        }
        Item {
            width: 3
            height: parent.height
        }
    }
}

