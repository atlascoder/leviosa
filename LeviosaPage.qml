import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import "DefaultTheme.js" as DefTheme

Page {
    id: leviosaPage
    visible: true
    width: 480
    height: 800

    signal menuClicked()
    signal addClicked()
    signal titleLongPressed()
    signal menuSelected()

    property bool enableMenuAction : true
    property bool enableAddAction : false
    property bool showLogo : true

    property alias showStatusText: toolbar.showStatusText
    property alias statusText: toolbar.statusText

    header: TopBar {
        id: toolbar
        onMenuAction: {
            menuSelected();
            menuClicked();
        }
        onAddAction: addClicked()
        title: leviosaPage.title
        enableMenuAction: leviosaPage.enableMenuAction
        enableAddAction: leviosaPage.enableAddAction
        onHeaderLongPressed: titleLongPressed()
    }

    background: Rectangle {
        anchors.fill: parent
        color: DefTheme.mainColorBg
    }

    Image {
        id: image
        anchors.centerIn: parent
        width: parent.width*0.75
        fillMode: Image.PreserveAspectFit
        source: "img/logo-blue.svg"
        opacity: 0.2
        visible: showLogo
    }

}