import QtQuick 2.7
import QtQuick.Controls 2.2

import "DefaultTheme.js" as DefTheme

WelcomeBrandPage {
    id: rootItem

    signal downloaded()

    hintText: "Syncing with cloud.."

    Component.onCompleted: {
        userData.sync();
    }

    Connections {
        target: userData
        onSynced: {
            hintText = "Data downloaded."
            downloaded()
        }
    }

    BusyIndicator {
        visible: userData.isSyncing
        anchors.top: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }

}
